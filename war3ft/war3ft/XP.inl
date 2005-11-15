/***********************************************************************
		XP FUNCTIONS (idea from war3x)
***********************************************************************/
stock XP_give(id, iXP){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_give",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	if(id==0)
		return 0

	// Check for min. players here
	new numberofplayers = get_playersnum() 

	if(numberofplayers < iCvar[FT_MIN_B4_XP])
		return 0

	p_data[id][P_XP] += iXP

	WAR3_Display_Level(id, DISPLAYLEVEL_SHOWGAINED)

	return iXP
}

stock XP_onDeath(victim_id, killer_id, weapon, headshot){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_onDeath",victim_id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	// Don't award XP to world
	if(killer_id==0)
		return PLUGIN_CONTINUE

	new victimName[32], iXP, XP
	get_user_name(victim_id,victimName,31)

	#if MOD == 1
	headshot--
	#endif
	if (killer_id && killer_id!=victim_id && victim_id){

		// He killed a teammate
		if (get_user_team(killer_id)==get_user_team(victim_id)){
			iXP = -1 * xpgiven[p_data[killer_id][P_LEVEL]]
			XP = XP_give(killer_id,iXP)
			if (iCvar[FT_KILL_OBJECTIVES]){
				client_print(killer_id,print_chat,"%s %L",g_MODclient,killer_id,"AWARDED_FOR_KILLING_TEAMMATE",XP)
			}
		}
		else{
			if (!iCvar[MP_WEAPONXPMODIFIER]){
				iXP = xpgiven[p_data[victim_id][P_LEVEL]]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_KILL_OBJECTIVES]){			
					client_print(killer_id,print_chat,"%s %L",g_MODclient,killer_id,"AWARDED_FOR_KILLING",XP,victimName)
				}
			}
			else
			{
				iXP = floatround(xpgiven[p_data[victim_id][P_LEVEL]] * weaponxpmultiplier[weapon])
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_KILL_OBJECTIVES]){
					client_print(killer_id,print_chat,"%s %L",g_MODclient,killer_id,"AWARDED_FOR_KILLING",XP,victimName)
				}
			}
		#if MOD == 0
			if( headshot && killer_id!=victim_id){
				iXP = iCvar[FT_HEADSHOT_BONUS]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_KILL_OBJECTIVES]){
					client_print(killer_id,print_chat,"%s %L",g_MODclient, killer_id,"AWARDED_FOR_HEADSHOT", XP)			
				}
			}
			if (victim_id==g_hostageSaver){
				iXP = iCvar[FT_KILLRESCUEMANXP]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_OBJECTIVES]){	
					client_print(killer_id,print_chat,"%s %L", g_MODclient, killer_id,"AWARDED_FOR_KILLING_HOSTAGE_R", XP)
				}
			}
			else if (victim_id==g_bombDefuser && killer_id!=g_bombDefuser){						
				iXP = iCvar[FT_DEFUSER_KILL_BONUS]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_OBJECTIVES]){				
					client_print(killer_id,print_chat, "%s %L", g_MODclient, killer_id,"AWARDED_FOR_KILLING_BOMB_D",XP)
				}	
			}
			else if (victim_id==g_bombCarrier && killer_id!=g_bombCarrier){						
				iXP = iCvar[FT_KILL_BOMB_CARRIER_BONUS]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_OBJECTIVES]){
					client_print(killer_id,print_chat, "%s %L", g_MODclient, killer_id,"AWARDED_FOR_KILLING_BOMB_C",XP)
				}
			}
			else if (victim_id==g_vipID && killer_id!=g_vipID){
				iXP = iCvar[FT_VIP_KILL_BONUS]
				XP = XP_give(killer_id,iXP)
				if (iCvar[FT_OBJECTIVES]){
					client_print(killer_id,print_chat, "%s %L", g_MODclient, killer_id,"AWARD_FOR_KILLING_VIP",XP)
				}
			}

		#endif
		}
	}

	return PLUGIN_CONTINUE
}

stock XP_Set(){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_set",0)
	#endif

	if (iCvar[MP_SAVEXP]){
		xpgiven = {6,8,10,12,14,16,18,20,24,28,32}
		xplevel = {0,100,200,400,800,1600,3200,6400,12800,25600,51200}
	}

	for(new i=0;i<11;i++)
		xplevel[i] = floatround(xplevel[i] * fCvar[MP_XPMULTIPLIER])

	XP_Set_Multiplier()
}

stock XP_Set_Multiplier(){
	#if ADVANCED_DEBUG
		writeDebugInfo("set_xpmultiplier", 0)
	#endif

	if (!iCvar[MP_WEAPONXPMODIFIER])
		return PLUGIN_CONTINUE

	weaponxpmultiplier[CSW_WORLDSPAWN]		= 0.0

#if MOD == 0
	weaponxpmultiplier[CSW_USP]				= 2.5
	weaponxpmultiplier[CSW_DEAGLE]			= 2.5
	weaponxpmultiplier[CSW_GLOCK18]			= 2.5
	weaponxpmultiplier[CSW_ELITE]			= 3.5
	weaponxpmultiplier[CSW_P228]			= 2.5
	weaponxpmultiplier[CSW_FIVESEVEN]		= 2.5

	weaponxpmultiplier[CSW_XM1014]			= 1.25
	weaponxpmultiplier[CSW_M3]				= 1.5

	weaponxpmultiplier[CSW_MP5NAVY]			= 1.0
	weaponxpmultiplier[CSW_UMP45]			= 1.25
	weaponxpmultiplier[CSW_P90]				= 1.25
	weaponxpmultiplier[CSW_TMP]				= 1.5
	weaponxpmultiplier[CSW_MAC10]			= 1.5
	weaponxpmultiplier[CSW_GALIL]			= 1.15
	weaponxpmultiplier[CSW_FAMAS]			= 1.15

	weaponxpmultiplier[CSW_AWP]				= 1.0
	weaponxpmultiplier[CSW_M4A1]			= 1.0
	weaponxpmultiplier[CSW_AK47]			= 1.0
	weaponxpmultiplier[CSW_AUG]				= 1.0
	weaponxpmultiplier[CSW_SG552]			= 1.0
	weaponxpmultiplier[CSW_G3SG1]			= 1.0
	weaponxpmultiplier[CSW_SG550]			= 1.0
	weaponxpmultiplier[CSW_M249]			= 1.25
	weaponxpmultiplier[CSW_SCOUT]			= 3.0

	weaponxpmultiplier[CSW_HEGRENADE]		= 1.75
	weaponxpmultiplier[CSW_KNIFE]			= 6.0

	weaponxpmultiplier[CSW_C4]				= 2.0
	weaponxpmultiplier[CSW_SMOKEGRENADE]	= 1.0
	weaponxpmultiplier[CSW_FLASHBANG]		= 1.0
#endif
#if MOD == 1
	weaponxpmultiplier[DODW_AMERKNIFE		] = 6.0
	weaponxpmultiplier[DODW_GERKNIFE		] = 6.0
	weaponxpmultiplier[DODW_BRITKNIFE		] = 6.0
	weaponxpmultiplier[DODW_GARAND_BUTT		] = 6.0

	weaponxpmultiplier[DODW_HANDGRENADE		] = 1.75
	weaponxpmultiplier[DODW_STICKGRENADE	] = 1.75
	weaponxpmultiplier[DODW_STICKGRENADE_EX	] = 1.75
	weaponxpmultiplier[DODW_HANDGRENADE_EX	] = 1.75

	weaponxpmultiplier[DODW_COLT			] = 1.0
	weaponxpmultiplier[DODW_LUGER			] = 1.0
	weaponxpmultiplier[DODW_GARAND			] = 1.0
	weaponxpmultiplier[DODW_SCOPED_KAR		] = 1.0
	weaponxpmultiplier[DODW_THOMPSON		] = 1.0
	weaponxpmultiplier[DODW_STG44			] = 1.0
	weaponxpmultiplier[DODW_SPRINGFIELD		] = 1.0
	weaponxpmultiplier[DODW_KAR				] = 1.0
	weaponxpmultiplier[DODW_BAR				] = 1.0
	weaponxpmultiplier[DODW_MP40			] = 1.0
	weaponxpmultiplier[DODW_MG42			] = 1.0
	weaponxpmultiplier[DODW_30_CAL			] = 1.0
	weaponxpmultiplier[DODW_SPADE			] = 1.0
	weaponxpmultiplier[DODW_M1_CARBINE		] = 1.0
	weaponxpmultiplier[DODW_MG34			] = 1.0
	weaponxpmultiplier[DODW_GREASEGUN		] = 1.0
	weaponxpmultiplier[DODW_FG42			] = 1.0
	weaponxpmultiplier[DODW_K43				] = 1.0
	weaponxpmultiplier[DODW_ENFIELD			] = 1.0
	weaponxpmultiplier[DODW_STEN			] = 1.0
	weaponxpmultiplier[DODW_BREN			] = 1.0
	weaponxpmultiplier[DODW_WEBLEY			] = 1.0
	weaponxpmultiplier[DODW_BAZOOKA			] = 1.0
	weaponxpmultiplier[DODW_PANZERSCHRECK	] = 1.0
	weaponxpmultiplier[DODW_PIAT			] = 1.0
	weaponxpmultiplier[DODW_SCOPED_FG42		] = 1.0
	weaponxpmultiplier[DODW_FOLDING_CARBINE	] = 1.0
	weaponxpmultiplier[DODW_KAR_BAYONET		] = 1.0
	weaponxpmultiplier[DODW_SCOPED_ENFIELD	] = 1.0
	weaponxpmultiplier[DODW_MILLS_BOMB		] = 1.0
	weaponxpmultiplier[DODW_ENFIELD_BAYONET	] = 1.0
	weaponxpmultiplier[DODW_MORTAR			] = 1.0
	weaponxpmultiplier[DODW_K43_BUTT		] = 1.0

#endif

	weaponxpmultiplier[CSW_LIGHTNING]		= 1.0
	weaponxpmultiplier[CSW_SUICIDE]			= 1.0
	weaponxpmultiplier[CSW_FLAME]			= 1.0
	weaponxpmultiplier[CSW_LOCUSTS]			= 1.1
	weaponxpmultiplier[CSW_SERPENTWARD]		= 1.0
	weaponxpmultiplier[CSW_SHADOW]			= 1.0	
	weaponxpmultiplier[CSW_THORNS]			= 1.0
	weaponxpmultiplier[CSW_CARAPACE]		= 1.0
	weaponxpmultiplier[CSW_CARRION]			= 1.0
	weaponxpmultiplier[CSW_ORB]				= 1.0
	weaponxpmultiplier[CSW_CONCOCTION]		= 1.0

	return PLUGIN_CONTINUE
}

public XP_Save(id){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Save",id)
	#endif
	
	if ( !warcraft3 )
	{
		return PLUGIN_CONTINUE;
	}

	if ( p_data[id][P_RACE]==0 || is_user_bot(id) || !iCvar[MP_SAVEXP] )
	{
		return PLUGIN_CONTINUE;
	}

	new playerid[33], playername[65], timet[32], ip[32]

	if ( iCvar[SV_MYSQL] ){
		if(mysql < SQL_OK){
			if(iSQLAttempts < SQL_ATTEMPTS)
				XP_Set_DBI()
			else if(iSQLAttempts != SQL_ATTEMPTS + 10){
				iSQLAttempts = SQL_ATTEMPTS + 10
				iCvar[MP_SAVEXP] = 0
				iCvar[SV_MYSQL] = 0
				log_amx("[%s] Unable to connect to the %s Database after %d tries, switching to short-term mode", g_MOD, SQLtype, iSQLAttempts)
				client_print(0,print_chat,"%s Unable to connect to the %s Database after %d tries, switching to short-term mode", g_MODclient, SQLtype, iSQLAttempts)
			}
			return PLUGIN_CONTINUE
		}
		new mquery[1024]
		get_user_name(id,playername,64)
		get_user_authid(id,playerid,31)
		get_user_ip(id,ip,31) 

		replace(playername, 64, "'", "\'")
		replace(playername, 64, "`", "")

		format(mquery, 1023, "REPLACE INTO `%s` (`playerid`, `playername`, `xp`, `race`, `skill1`, `skill2`, `skill3`, `skill4`) VALUES ('%s', '%s', %d, %d, %d, %d, %d, %d)", mysqltablename, (iCvar[FT_SAVEBY]==2) ? playername : ((iCvar[FT_SAVEBY]==1) ? ip : playerid), playername, p_data[id][P_XP], p_data[id][P_RACE], p_data[id][P_SKILL1], p_data[id][P_SKILL2], p_data[id][P_SKILL3], p_data[id][P_ULTIMATE])

#if SQL_DEBUG
		log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
 		new Result:ret = dbi_query(mysql, mquery)
#if SQL_DEBUG
		log_amx("[%s] ret=%d", g_MOD, ret)
#endif

		if (ret < RESULT_NONE) {
			new err[255]
			new errNum = dbi_error(mysql, err, 254)
			log_amx("[%s] DBI XP_Save error: %s (%d)", g_MOD, err, errNum)
			dbi_free_result(ret)
			return 1
		} 	

		//dbi_free_result(ret)		// Why is MySQL database complaining with this? SQLite works OK, so why?

	}
	else{			// Save it to a vault :)
		new temp[128], string[512]
		get_time("%m %d %H %M",timet,31)
		get_user_authid(id,playerid,31)
		get_user_name(id,playername,31)
		get_user_ip(id,ip,31)

		format( string,511,"%s %d %d %d %d %d %d %s %s %s", playerid, p_data[id][P_XP], p_data[id][P_RACE], p_data[id][P_SKILL1], p_data[id][P_SKILL2], p_data[id][P_SKILL3], p_data[id][P_ULTIMATE], ip, timet, playername );

		if(iCvar[FT_SAVEBY]==0)							// Save by steam ID
			format(temp,127,"%s_%d",playerid,p_data[id][P_RACE])
		else if(iCvar[FT_SAVEBY]==1)					// Save by IP address
			format(temp,127,"%s_%d",ip,p_data[id][P_RACE])
		else if(iCvar[FT_SAVEBY]==2)					// Save by Player name
			format(temp,127,"%s_%d",playername,p_data[id][P_RACE])

		set_vaultdata(temp,string)
	}
	return PLUGIN_CONTINUE
}

public XP_Retreive(id,returnrace){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Retreive",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	if (!iCvar[MP_SAVEXP])
		return PLUGIN_CONTINUE

	if (!id)
		return PLUGIN_CONTINUE

	new mquery[1024]

	new playerid[32], playername[32], ip[32]
	new xp[8], race[2], skill1[2], skill2[2], skill3[2], skill4[2]
	new racexp[9]={0,0,0,0,0,0,0,0,0}
	get_user_name(id,playername,31)
	get_user_authid(id,playerid,31)
	get_user_ip(id,ip,31) 

	if (iCvar[SV_MYSQL]){
		if(mysql < SQL_OK){
			if(iSQLAttempts < SQL_ATTEMPTS)
				XP_Set_DBI()
			else if(iSQLAttempts != SQL_ATTEMPTS + 10){
				iSQLAttempts = SQL_ATTEMPTS + 10
				iCvar[MP_SAVEXP] = 0
				iCvar[SV_MYSQL] = 0
				log_amx("[%s] Unable to connect to the %s Database after %d tries, switching to short-term mode", g_MOD, SQLtype, iSQLAttempts)
				client_print(0,print_chat,"%s Unable to connect to the %s Database after %d tries, switching to short-term mode", g_MODclient, SQLtype, iSQLAttempts)
			}
			return PLUGIN_CONTINUE
		}

		if (returnrace){
			format(mquery, 1023, "SELECT `xp`, `race` FROM `%s` WHERE (`playerid` = '%s')", mysqltablename, (iCvar[FT_SAVEBY]==2) ? playername : ((iCvar[FT_SAVEBY]==1) ? ip : playerid))

#if SQL_DEBUG
			log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
 			new Result:res = dbi_query(mysql, mquery)
#if SQL_DEBUG
			log_amx("[%s] res=%d", g_MOD, res)
#endif

			if (res < RESULT_NONE) {
				new err[255]
				new errNum = dbi_error(mysql, err, 254)
				log_amx("[%s] DBI XP_Retreive error: %s (%d)", g_MOD, err, errNum)
				dbi_free_result(res)
				return PLUGIN_CONTINUE
			}

			while (res && dbi_nextrow(res)>0) {
				dbi_result(res, "xp", xp, 7)
				dbi_result(res, "race", race, 1)
				if (str_to_num(race)>=1 && str_to_num(race)<=9)
					racexp[str_to_num(race)-1] = str_to_num(xp)
			}

			dbi_free_result(res) 

			if (p_data[id][P_RACE])
				racexp[p_data[id][P_RACE]-1]=p_data[id][P_XP]

			menu_Select_Race(id, racexp)
		}
		else{
			format(mquery, 1023, "SELECT `xp`, `skill1`, `skill2`, `skill3`, `skill4` FROM `%s` WHERE (`playerid` = '%s' AND `race` = %d)", mysqltablename, (iCvar[FT_SAVEBY]==2) ? playername : ((iCvar[FT_SAVEBY]==1) ? ip : playerid), p_data[id][P_RACE])

#if SQL_DEBUG
			log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
 			new Result:res = dbi_query(mysql, mquery)
#if SQL_DEBUG
			log_amx("[%s] res=%d", g_MOD, res)
#endif

			if (res < RESULT_NONE) {
				new err[255]
				new errNum = dbi_error(mysql, err, 254)
				log_amx("[%s] DBI XP_Retreive error: %s (%d)", g_MOD, err, errNum)
				dbi_free_result(res)

				return PLUGIN_CONTINUE
			}

			if (res && dbi_nextrow(res)>0){
				dbi_result(res, "xp", xp, 7)
				dbi_result(res, "skill1", skill1, 1)
				dbi_result(res, "skill2", skill2, 1)
				dbi_result(res, "skill3", skill3, 1)
				dbi_result(res, "skill4", skill4, 1)				

				p_data[id][P_XP]=str_to_num(xp)
				p_data[id][P_SKILL1]=str_to_num(skill1)
				p_data[id][P_SKILL2]=str_to_num(skill2)
				p_data[id][P_SKILL3]=str_to_num(skill3)
				p_data[id][P_ULTIMATE]=str_to_num(skill4)

				WAR3_Display_Level(id,DISPLAYLEVEL_SHOWRACE)

			}
			else{
				p_data[id][P_XP]=0
				p_data[id][P_SKILL1]=0
				p_data[id][P_SKILL2]=0
				p_data[id][P_SKILL3]=0
				p_data[id][P_ULTIMATE]=0
				WAR3_Display_Level(id,DISPLAYLEVEL_SHOWRACE)
			}

			dbi_free_result(res) 
		}
	}
	else{			// Get info from a vault
		new info[512], temp[128]
		new i, attempt
		if (returnrace)
		{
			for(i=1;i<9;i++){
				format(xp,7,"")
				format(race,1,"")
				format(temp,127,"")

				if(iCvar[FT_SAVEBY]==0)			// Get info by steam ID
					format(temp,127,"%s_%d",playerid,i)
				else if(iCvar[FT_SAVEBY]==1)		// Get info by IP address
					format(temp,127,"%s_%d",ip,i)
				else if(iCvar[FT_SAVEBY]==2)		// Get info by Player name
					format(temp,127,"%s_%d",playername,i)


				attempt = get_vaultdata(temp,info,511)
				parse(info, playerid,31, xp,7, race,1, skill1,1, skill2,1, skill3,1, skill4,1)
				if (str_to_num(race)>=1 && str_to_num(race)<=9)
					racexp[str_to_num(race)-1] = str_to_num(xp)
			}
			if (p_data[id][P_RACE])
				racexp[p_data[id][P_RACE]-1]=p_data[id][P_XP]

			menu_Select_Race(id, racexp)
		}
		else{

			if(iCvar[FT_SAVEBY]==0)			// Get info by steam ID
				format(temp,127,"%s_%d",playerid,p_data[id][P_RACE])
			else if(iCvar[FT_SAVEBY]==1)		// Get info by IP address
				format(temp,127,"%s_%d",ip,p_data[id][P_RACE])
			else if(iCvar[FT_SAVEBY]==2)		// Get info by Player name
				format(temp,127,"%s_%d",playername,p_data[id][P_RACE])

			attempt = get_vaultdata(temp,info,511)
			if (attempt){
				parse(info, playerid,31, xp,7, race,1, skill1,1, skill2,1, skill3,1, skill4,1)
				p_data[id][P_XP]=str_to_num(xp)
				p_data[id][P_SKILL1]=str_to_num(skill1)
				p_data[id][P_SKILL2]=str_to_num(skill2)
				p_data[id][P_SKILL3]=str_to_num(skill3)
				p_data[id][P_ULTIMATE]=str_to_num(skill4)
				WAR3_Display_Level(id,DISPLAYLEVEL_SHOWRACE)
			}
			else{
				p_data[id][P_XP]=0
				p_data[id][P_SKILL1]=0
				p_data[id][P_SKILL2]=0
				p_data[id][P_SKILL3]=0
				p_data[id][P_ULTIMATE]=0
				WAR3_Display_Level(id,DISPLAYLEVEL_SHOWRACE)
			}
		}
	}
	return PLUGIN_CONTINUE
}

public XP_Client_Save(id,show){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_client_save",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	if (iCvar[MP_SAVEXP]){
		if(show)
		{
			if ( iCvar[FT_DISABLE_SAVEXP] )
			{
				client_print(id, print_chat, "%s Typing /savexp does nothing on this server",g_MODclient)
			}
			else
			{
				XP_Save(id)
				client_print(id,print_chat, "%s XP saved.",g_MODclient)
			}
		}
	}	
	else if (show)
		client_print(id,print_chat,"%s %L",g_MODclient, id,"XP_IS_NOT_SAVED")
	
	return PLUGIN_CONTINUE
}

public XP_Set_DBI(){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Set_DBI",0)
	#endif

	if (iCvar[SV_MYSQL]){
		get_cvar_string("sv_mysqltablename",mysqltablename,63)
		new mhost[64], muser[32], mpass[32], mdb[128], int_check[64]
		new mquery[512]

		dbi_type(SQLtype, 15)
#if SQL_DEBUG
		log_amx("[%s] dbi_type: %s", g_MOD, SQLtype)
#endif

		if (equali(SQLtype, g_MySQL))
		{
			iSQLtype = SQL_MYSQL
			copy(SQLtype, strlen(g_MySQL)+1, g_MySQL)
		}
		else if (equali(SQLtype, g_SQLite))
		{
			iSQLtype = SQL_SQLITE
			copy(SQLtype, strlen(g_SQLite)+1, g_SQLite)
		}
		else
		{
			iSQLtype = SQL_NONE
			log_amx("[%s] Unsupported database type found (%s), the supported databases are %s or %s", g_MOD, SQLtype, g_MySQL, g_SQLite)
			return PLUGIN_CONTINUE
		}

#if SQL_DEBUG
		log_amx("[%s] iSQLtype = %d, SQLtype = %s", g_MOD, iSQLtype, SQLtype)
#endif

		if (iSQLtype == SQL_MYSQL)
			format(mquery, 511, "CREATE TABLE IF NOT EXISTS `%s` (`playerid` VARCHAR(35) NOT NULL DEFAULT '', `playername` VARCHAR(35) NOT NULL DEFAULT '', `xp` INT(11) NOT NULL DEFAULT 0, `race` TINYINT(4) NOT NULL DEFAULT 0, `skill1` TINYINT(4) NOT NULL DEFAULT 0, `skill2` TINYINT(4) NOT NULL DEFAULT 0, `skill3` TINYINT(4) NOT NULL DEFAULT 0, `skill4` TINYINT(4) NOT NULL DEFAULT 0, `time` TIMESTAMP(14) NOT NULL, PRIMARY KEY (`playerid`, `race`))", mysqltablename)
		else if (iSQLtype == SQL_SQLITE)
			format(mquery, 511, "CREATE TABLE `%s` (`playerid` VARCHAR(35) NOT NULL DEFAULT '', `playername` VARCHAR(35) NOT NULL DEFAULT '', `xp` INT(11) NOT NULL DEFAULT 0, `race` TINYINT(4) NOT NULL DEFAULT 0, `skill1` TINYINT(4) NOT NULL DEFAULT 0, `skill2` TINYINT(4) NOT NULL DEFAULT 0, `skill3` TINYINT(4) NOT NULL DEFAULT 0, `skill4` TINYINT(4) NOT NULL DEFAULT 0, `time` TIMESTAMP(14) NOT NULL DEFAULT CURRENT_TIMESTAMP, PRIMARY KEY (`playerid`, `race`))", mysqltablename)

		get_cvar_string("FT_mysql_host",mhost,63)
		get_cvar_string("FT_mysql_user",muser,31)
		get_cvar_string("FT_mysql_pass",mpass,31)
		get_cvar_string("FT_mysql_db",mdb,127)

		new err[255], errNum = 0
#if SQL_DEBUG
		log_amx("[%s] dbi_connect(%s, %s, %s, %s)", g_MOD, mhost, muser, mpass, mdb)
#endif
		mysql = dbi_connect(mhost,muser,mpass,mdb)
#if SQL_DEBUG
		log_amx("[%s] mysql=%d", g_MOD, mysql)
#endif

		if (mysql < SQL_OK) {
			errNum = dbi_error(mysql, err, 254)
			log_amx("[%s] DBI XP_Set_DBI error: %s (%d)", g_MOD, err, errNum)

			if(iSQLAttempts < SQL_ATTEMPTS){
				new Float:delay = 25.0
				log_amx("[%s] Will attempt to re-connect to the %s Database in %f seconds", g_MOD, SQLtype, delay)
				set_task(delay, "XP_Set_DBI", TASK_SETMYSQL)
			}
			iSQLAttempts++
			return 1
		}
		else
			log_amx("[%s] Connection to %s Database successful (%d)", g_MOD, SQLtype, mysql)

		new Result:ret
		if (iSQLtype != SQL_SQLITE || !sqlite_table_exists(mysql, mysqltablename)) {
#if SQL_DEBUG
			log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
			ret = dbi_query(mysql, mquery)
#if SQL_DEBUG
			log_amx("[%s] ret=%d", g_MOD, ret)
#endif

			if (ret < RESULT_NONE) {
				errNum = dbi_error(mysql, err, 254)
				log_amx("[%s] DBI XP_Set_DBI error: %s (%d)", g_MOD, err, errNum)
				dbi_free_result(ret)
				return 1
			}
		}

		//dbi_free_result(ret)		// Why is MySQL database complaining with this? SQLite works OK, so why?

		if (iSQLtype == SQL_SQLITE) { // These probably should be subject to a CVAR
			/*
				Lets fine tune the database:
			 		"synchronous = NORMAL"	- Put back the 2.x behaviour (faster than the defalt
								  for 3.x)
					"synchronous = OFF"	- Way faster, but it might get corrupted data if a
								  server os system crash occurs
					"integrity_check"	- well it's what it says, we do have to check the
								  value it returns since it's important
				PRAGMA commands don't return anything so no need to check the result of the query
			 */
			format(mquery, 511, "PRAGMA integrity_check")
#if SQL_DEBUG
			log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
			new Result:res = dbi_query(mysql, mquery)


			while (res && dbi_nextrow(res)>0)
			{
				dbi_result(res, "integrity_check", int_check, 63)
			}

			dbi_free_result(res)
#if SQL_DEBUG
			log_amx("[%s] int_check=%s", g_MOD, int_check)
#endif

			if (!equali(int_check, "ok")) {
				log_amx("[%s] DBI XP_Set_DBI error: %s", g_MOD, int_check)
				return 1
			}
			format(mquery, 511, "PRAGMA synchronous = %d", SQLITE_SYNC_OFF)
#if SQL_DEBUG
			log_amx("[%s] dbi_query(%d, %s)", g_MOD, mysql, mquery)
#endif
			dbi_query(mysql, mquery)
		}
	}

	return PLUGIN_CONTINUE
}

public XP_Reset(id,saychat){
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Reset",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	p_data[id][P_LEVEL]=0
	p_data[id][P_XP]=0
	p_data[id][P_SKILL1]=0
	p_data[id][P_SKILL2]=0
	p_data[id][P_SKILL3]=0
	p_data[id][P_ULTIMATE]=0

	WAR3_Display_Level(id,DISPLAYLEVEL_NONE)

	XP_Save(id)

	if(saychat==1)
		client_print(id,print_chat,"%s %L",g_MODclient, id,"YOUR_XP_HAS_BEEN_RESET")
	else
		console_print(id,"%L",id,"YOUR_XP_HAS_BEEN_RESET",g_MOD)

	return PLUGIN_CONTINUE
}	

public XP_Prune()
{
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Prune",id)
	#endif

	if ( iCvar[SV_MYSQL_AUTO_PRUNING] && iSQLtype == SQL_MYSQL )
	{
		new szTableName[64];
		get_cvar_string("sv_mysqltablename", szTableName, 63);
			
		new query[256];

		if ( iSQLtype == SQL_MYSQL )
		{
			// Timestamp format: 20030912122142
			// Y = 2003 M = 09 D = 12 H = 12 M = 21 S = 42	
			format( query, 255, "DELETE FROM `%s` WHERE DATE_SUB(CURDATE(),INTERVAL %d DAY) > time;", szTableName, iCvar[SV_DAYSBEFOREDELETE] );
		}
		else if ( iSQLtype == SQL_SQLITE )
		{
			// Timestamp format: 2003-09-12 12:21:42
			// Y = 2003 M = 09 D = 12 H = 12 M = 21 S = 42
			format( query, 255, "DELETE FROM `%s` WHERE ((julianday(`time`) + %d) < julianday('now'))", szTableName, iCvar[SV_DAYSBEFOREDELETE] );
		}

		dbi_query(mysql, query);

		// Vacuum the SQL LITE DB
		if (iSQLtype == SQL_SQLITE)
		{
			format( query, 255, "VACUUM `%s`", szTableName );

			dbi_query( mysql, query );
		}

		log_amx("[%s] Database pruning successful, data older than %d days was removed", g_MOD, iCvar[SV_DAYSBEFOREDELETE]);
	}
}

// Close the database connection
public XP_CloseDB()
{
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_CloseDB", 0)
	#endif

	if ( iCvar[SV_MYSQL] )
	{
		if( mysql )
		{
			dbi_close( mysql );
		}
	}
}

// This function will save the XP for all players, but it will save the data every 0.1 seconds (reduce lag?)
public XP_Save_All()
{
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Save_All", 0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE;
	
	if ( !iCvar[MP_SAVEXP] )
		return PLUGIN_CONTINUE;

	new players[32], numofplayers, parm[1];
	get_players( players, numofplayers );
	new Float:time = 0.0;

	for ( new i=0; i < numofplayers; i++ )
	{
		parm[0] = players[i];

		set_task( time, "XP_Save_Helper", TASK_SAVE_ALL + players[i], parm[0], 1 );

		time += 0.1;
	}

	return PLUGIN_CONTINUE;
}

// Helper function to call save for XP_Save_All
public XP_Save_Helper( parm[1] )
{
	#if ADVANCED_DEBUG
		writeDebugInfo("XP_Save_Helper", parm[0])
	#endif

	XP_Save( parm[0] );
}