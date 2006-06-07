/*��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.
*	Race: Blood Mage Functions
��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.��`�.��.*/

#define IMMOLATE_DAMAGE     35  // Initial damage done to target players
#define IMMOLATE_DOT_DAMAGE  5  // Damage done on each tick of the ultimate
#define IMMOLATE_DOT         4  // Number of times ultimate ticks


public BM_ULT_Immolate( iCaster, iTarget )
{
	emit_sound( iCaster, CHAN_STATIC, SOUND_IMMOLATE, 0.5, ATTN_NORM, 0, PITCH_NORM );

	// Emit sound and create the fire exploding effect on the player

	new vTargetOrigin[3];
	get_user_origin( iTarget, vTargetOrigin );

	Create_TE_EXPLOSION( vTargetOrigin, vTargetOrigin, g_sImmolate, 20, 24, 4 );
	Create_TE_EXPLOSION( vTargetOrigin, vTargetOrigin, g_sBurning, 30, 24, 4 );

	Create_ScreenShake( iTarget, (10<<12), (2<<12), (5<<12) );

	// Do initial immolate damage and make their screen shake a little

	WAR3_damage( iTarget, iCaster, IMMOLATE_DAMAGE, CSW_IMMOLATE, -1 )

	new parm_DoT[3];
	parm_DoT[0] = iCaster;
	parm_DoT[1] = iTarget;
	parm_DoT[2] = 1;

	// Start the ultimate DoT

	new TaskId = TASK_BURNING + iTarget;
	set_task( 1.0, "BM_ULT_Immolate_DoT", TaskId, parm_DoT, 3 );
	
	return PLUGIN_HANDLED;
}

public BM_ULT_Immolate_DoT( parm_DoT[3] )
{
	new iCaster = parm_DoT[0];
    new iTarget = parm_DoT[1];
	new iCounter = parm_DoT[2];
	
	// Stop DoT if the max number of ticks is reached

	if ( iCounter > IMMOLATE_DOT ) 
	{
		BM_ULT_Immolate_Remove( iTarget );
		return PLUGIN_HANDLED;
	}

	// Emit sound and show the burning effect on the player

    new vTargetOrigin[3];
    get_user_origin( iTarget, vTargetOrigin );

	emit_sound( iTarget, CHAN_STATIC, SOUND_IMMOLATE_BURNING, 0.5, ATTN_NORM, 0, PITCH_NORM );

	Create_TE_SPRITE( vTargetOrigin, g_sFire, 3, 200 );

	// Do the DoT damage

	WAR3_damage( iTarget, iCaster, IMMOLATE_DOT, CSW_IMMOLATE, -1 )

	// If the target is still alive after this, make their screen glow orange and start the task again

    if ( is_user_alive( iTarget ) )
    {
		p_data[iTarget][PB_ISBURNING] = true;

        Create_ScreenFade( iTarget, (1<<10), (1<<10), (1<<12), 255, 108, 0, 160 );

		parm_DoT[2]++;

		new TaskId = TASK_BURNING + iTarget;
		set_task( 1.0, "BM_ULT_Immolate_DoT", TaskId, parm_DoT, 3 );

	}

	return PLUGIN_HANDLED;
}


BM_ULT_Immolate_Remove( iTarget ) 
{
	// Clear global variables and stop the DoT task

	p_data[iTarget][PB_ISBURNING] = false;

	new TaskId = TASK_BURNING + iTarget;
    remove_task( TaskId, 0 );

    return PLUGIN_HANDLED;
}

// Check to see if a player will become a Phoenix
BM_PheonixCheck( id )
{
	if ( Verify_Skill(id, RACE_BLOOD, SKILL1) )
	{
		
		// Should the user be a Phoenix
		if ( random_float( 0.0, 1.0 ) <= p_pheonix[p_data[id][P_SKILL1]-1] ) 
		{
			p_data_b[id][PB_PHOENIX] = true;
		}
	}
}

// THis function is called when a user dies
BM_PhoenixSpawn( id )
{
	
	// If this user is about to respawn, then we don't even want to check
	if ( p_data[id][P_RESPAWNBY] )
	{
		return;
	}

	new iTeam = get_user_team( id );
	new iPhoenixID = BM_PhoenixExists( iTeam );
	
	// Then we have a Phoenix!!
	if ( iPhoenixID != -1 )
	{

		// Lets make sure they're not going to revive themself
		if ( iPhoenixID != id )
		{

			// The caster is no longer a Phoenix
			p_data_b[iPhoenixID][PB_PHOENIX] = false;
			
			// The user is about to respawn
			p_data[id][P_RESPAWNBY] = RESPAWN_PHOENIX;
			
			// Respawn the user in 0.7 seconds
			set_task( 0.7, "_SHARED_Spawn", TASK_SPAWN + id );

			new szCastername[32], szSpawnerName[32];

			get_user_name( iPhoenixID, szCastername,31 )
			get_user_name( id, szSpawnerName, 31 )

			client_print( id, print_chat, "%s %L", g_MODclient, id, "HAS_REVIVED_YOU", szCastername );
			client_print( iPhoenixID, print_chat, "%s %L", g_MODclient, iPhoenixID, "YOU_HAVE_REVIVED", szSpawnerName );
		}
	}

	return;
}

// Check to see if a Phoenix exists on a given team
BM_PhoenixExists( iTeam )
{
	new players[32], numberofplayers, id, i;
	get_players(players, numberofplayers, "a");

	// Loop through all the players to find someone with phoenix
	for ( i = 0; i < numberofplayers; i++ )
	{
		id = players[i];

		if ( get_user_team( id ) == iTeam )
		{
			if ( p_data_b[id][PB_PHOENIX] )
			{
				return id;
			}
		}
	}

	return -1;
}