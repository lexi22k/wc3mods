/*��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.
*	SQLite
��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.*/

#define TOTAL_SQLITE_TABLES		3

new const szTablesSQLite[TOTAL_SQLITE_TABLES][] = 
{
	"CREATE TABLE `wc3_player` ( `player_id` INTEGER PRIMARY KEY AUTOINCREMENT, `player_steamid` varchar(25) NOT NULL default '', `player_ip` varchar(20) NOT NULL default '', `player_name` varchar(35) NOT NULL default '', `time` timestamp(14) NOT NULL );",
	"CREATE TABLE `wc3_player_race` ( `player_id` int(8) NOT NULL default '0', `race_id` tinyint(4) NOT NULL default '0', `race_xp` int(8) default NULL, PRIMARY KEY  (`player_id`,`race_id`) );",
	"CREATE TABLE `wc3_player_skill` ( `player_id` int(8) NOT NULL default '0', `skill_id` tinyint(4) NOT NULL default '0', `skill_level` tinyint(4) NOT NULL default '0', PRIMARY KEY  (`player_id`,`skill_id`) );"
};

new const szTableNames[TOTAL_SQLITE_TABLES][] = 
{
	"wc3_player",
	"wc3_player_race",
	"wc3_player_skill"
};

// Initiate the connection to the SQLite database
SQLITE_Init()
{
	new szError[256];

	// Attempt the Connection
	g_DB = dbi_connect( "", "", "", "war3ft", szError, 255 );

	// Verify our database connection has worked
	if ( !SQLITE_Check_Connection() )
	{
		log_amx( "Error: %s", szError );

		return;
	}

	server_print( "[WAR3FT] SQLite database connection successful" );

	// Create all tables if we need to
	for ( new i = 0; i < TOTAL_SQLITE_TABLES; i++ )
	{
		// Then create it!
		if ( !sqlite_table_exists( g_DB, szTableNames[i] ) )
		{
			new Result:ret = dbi_query( g_DB, szTablesSQLite[i] );

			if ( ret < RESULT_NONE )
			{
				SQLITE_Error( ret, szTablesSQLite[i], 1 );

				return;
			}
		}
	}

	/*
		These probably should be subject to a CVAR
		Lets fine tune the database:
			"synchronous = NORMAL"	- Put back the 2.x behaviour (faster than the defalt
						  for 3.x)
			"synchronous = OFF"	- Way faster, but it might get corrupted data if a
						  server os system crash occurs
			"integrity_check"	- well it's what it says, we do have to check the
						  value it returns since it's important
		PRAGMA commands don't return anything so no need to check the result of the query
	*/	

	new szIntegrityCheck[64], szQuery[128];
	format(szQuery, 127, "PRAGMA integrity_check");
	new Result:res = dbi_query( g_DB, szQuery );
	
	// Check for an error
	if ( res < RESULT_NONE )
	{
		SQLITE_Error( res, szQuery, 2 );
		return;
	}
	
	// Get the integrity check value
	while ( res && dbi_nextrow(res) > 0 )
	{
		dbi_result( res, "integrity_check", szIntegrityCheck, 63 );
	}

	// Free the result
	dbi_free_result( res );

	// Check to make sure the integrity check passed
	if ( !equali(szIntegrityCheck, "ok") )
	{
		// Should we disable saving here?
		log_amx( "[SQLITE] SQL Lite integrity check failed, disabling saving XP." );

		set_pcvar_num( CVAR_wc3_save_xp, 0 );

		return;
	}
		
	// Do some synchronous crap
	format( szQuery, 127, "PRAGMA synchronous = %d", SQLITE_SYNC_OFF );
	dbi_query( g_DB, szQuery );
}

// Close the SQLite connection
SQLITE_Close()
{
	if ( g_DB )
	{
		dbi_close( g_DB );
	}
}

SQLITE_Save( id )
{
	// Verify we have a database connection
	if ( !SQLITE_Check_Connection() )
	{
		return;
	}
	
	new szPlayerIP[20], szPlayerName[66], szPlayerID[32];
	get_user_name(		id, szPlayerName	, 65 );
	get_user_ip(		id, szPlayerIP		, 19 );
	get_user_authid(	id, szPlayerID		, 31 );

	// Prepare name for the query (playername is 66 in case all 33 characters are ')
	DB_FormatString( szPlayerName, 65 );

	new iSkillLevels[4];
	iSkillLevels[0] = SM_GetSkillLevel( id, SM_GetSkillByPos( id, SKILL_POS_1 ) );
	iSkillLevels[1] = SM_GetSkillLevel( id, SM_GetSkillByPos( id, SKILL_POS_2 ) );
	iSkillLevels[2] = SM_GetSkillLevel( id, SM_GetSkillByPos( id, SKILL_POS_3 ) );
	iSkillLevels[3] = SM_GetSkillLevel( id, SM_GetSkillByPos( id, SKILL_POS_4 ) );

	// Save the data
	new szQuery[512];
	format( szQuery, 511, "REPLACE INTO `%s` (`playerid`, `playerip`, `playername`, `xp`, `race`, `skill1`, `skill2`, `skill3`, `skill4`) VALUES ('%s', '%s', '%s', %d, %d, %d, %d, %d, %d)", g_DBTableName, szPlayerID, szPlayerIP, szPlayerName, p_data[id][P_XP], p_data[id][P_RACE], iSkillLevels[0], iSkillLevels[1], iSkillLevels[2], iSkillLevels[3] );

	new Result:res = dbi_query( g_DB, szQuery );
	
	// Verify we have a result
	if ( res < RESULT_NONE )
	{
		SQLITE_Error( res, szQuery, 4 );

		return;
	}

	return;
}

SQLITE_GetAllXP( id )
{
	// Verify we have a database connection
	if ( !SQLITE_Check_Connection() )
	{
		return;
	}

	new szKey[66];
	DB_GetKey( id, szKey, 65 );

	new szQuery[256];
	format(szQuery, 255, "SELECT `xp`, `race` FROM `%s` WHERE (`%s` = '%s')", g_DBTableName, g_szDBKey, szKey );
	
	new Result:res = dbi_query( g_DB, szQuery );
	
	// Make sure we have a result
	if ( res < RESULT_NONE )
	{
		SQLITE_Error( res, szQuery, 5 );
		
		return;
	}

	// Loop through all of the records to find the XP data
	new szXP[8], szRace[2], iXP, iRace, iRaceXP[MAX_RACES] = {0};
	while ( res && dbi_nextrow( res ) > 0 )
	{
		dbi_result( res, "xp", szXP, 7 );
		dbi_result( res, "race", szRace, 1 );

		iXP		= str_to_num( szXP		);
		iRace	= str_to_num( szRace	);
		
		// Save the user's XP in an array
		if ( iRace > 0 && iRace < MAX_RACES + 1 )
		{
			iRaceXP[iRace-1] = iXP;
		}
	}
	
	// Free the result set
	dbi_free_result( res );

	// Call the function that will display the "select a race" menu
	WC3_ChangeRaceShowMenu( id, iRaceXP );

	return;
}

SQLITE_SetData( id )
{
	// Verify we have a database connection
	if ( !SQLITE_Check_Connection() )
	{
		return;
	}

	new szKey[66];
	DB_GetKey( id, szKey, 65 );

	new szQuery[256];
	format( szQuery, 255, "SELECT `xp`, `skill1`, `skill2`, `skill3`, `skill4` FROM `%s` WHERE (`%s` = '%s' AND `race` = %d)", g_DBTableName, g_szDBKey, szKey );

	new Result:res = dbi_query( g_DB, szQuery );
	
	// Verify we have a result
	if ( res < RESULT_NONE )
	{
		SQLITE_Error( res, szQuery, 6 );

		return;
	}
	
	new iSkillIDs[4];
	iSkillIDs[0] = SM_GetSkillByPos( id, SKILL_POS_1 );
	iSkillIDs[1] = SM_GetSkillByPos( id, SKILL_POS_2 );
	iSkillIDs[2] = SM_GetSkillByPos( id, SKILL_POS_3 );
	iSkillIDs[3] = SM_GetSkillByPos( id, SKILL_POS_4 );

	// Then we have data in the database
	if ( res && dbi_nextrow( res ) > 0 )
	{
		new szXP[8], szSkill1[2], szSkill2[2], szSkill3[2], szSkill4[2];

		dbi_result( res, "xp"		, szXP		, 7 );
		dbi_result( res, "skill1"	, szSkill1	, 1 );
		dbi_result( res, "skill2"	, szSkill2	, 1 );
		dbi_result( res, "skill3"	, szSkill3	, 1 );
		dbi_result( res, "skill4"	, szSkill4	, 1 );			

		p_data[id][P_XP]		= str_to_num( szXP		);
		SM_SetSkillLevel( id, iSkillIDs[0], str_to_num( szSkill1 ) );
		SM_SetSkillLevel( id, iSkillIDs[1], str_to_num( szSkill2 ) );
		SM_SetSkillLevel( id, iSkillIDs[2], str_to_num( szSkill3 ) );
		SM_SetSkillLevel( id, iSkillIDs[3], str_to_num( szSkill4 ) );
	}

	// The user has no record, start them at 0
	else
	{
		p_data[id][P_XP]		= 0;
		SM_SetSkillLevel( id, iSkillIDs[0], 0 );
		SM_SetSkillLevel( id, iSkillIDs[1], 0 );
		SM_SetSkillLevel( id, iSkillIDs[2], 0 );
		SM_SetSkillLevel( id, iSkillIDs[3], 0 );

	}
	
	// Free the result
	dbi_free_result( res );

	// Set the race up
	WC3_SetRaceUp( id );

	return;
}

// Verifies that the database connection is ok
SQLITE_Check_Connection()
{
	
	if ( g_DB < SQL_OK )
	{
		return false;
	}

	return true;
}

// The id should be a unique number, so we know what function called it (useful for debugging)
SQLITE_Error( Result:res, query[], id )
{

	// Get the error message and log it
	new szError[256];
	new iError = dbi_error( g_DB, szError, 255 );
	log_amx( "[SQLITE] Error in querying database, location: %d", id );
	log_amx( "[SQLITE] Message: %s (%d)", szError, iError );
	log_amx( "[SQLITE] Query statement: %s ", query );

	// Free the result
	if ( res > RESULT_FAILED )
	{
		dbi_free_result( res );
	}
}

SQLITE_Prune()
{
	new szQuery[256];
	format( szQuery, 255, "DELETE FROM `%s` WHERE ((julianday(`time`) + %d) < julianday('now'))", g_DBTableName, get_pcvar_num( CVAR_wc3_days_before_delete ) );

	new Result:ret = dbi_query( g_DB, szQuery );

	if ( ret < RESULT_NONE )
	{
		SQLITE_Error( ret, szQuery, 7 );

		return;
	}
}

SQLITE_UpdateTimestamp( id )
{
	new szKey[66];
	DB_GetKey( id, szKey, 65 );

	new szQuery[256];
	format( szQuery, 255, "UPDATE `%s` SET time = NOW() WHERE (`%s` = '%s')", g_DBTableName, g_szDBKey, szKey );

	new Result:ret = dbi_query( g_DB, szQuery );

	if ( ret < RESULT_NONE )
	{
		SQLITE_Error( ret, szQuery, 8 );

		return;
	}
}