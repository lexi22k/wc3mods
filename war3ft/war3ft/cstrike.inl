public on_SetSpecMode(id) {
	#if ADVANCED_DEBUG
		writeDebugInfo("setSpecMode",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new arg[12] 
	read_data( 2 , arg , 11 ) 
	p_data[id][P_SPECMODE] = ( arg[10] == '2' ) ? 1 : 0 

	return PLUGIN_CONTINUE
}

public on_Spectate(id){
	#if ADVANCED_DEBUG
		writeDebugInfo("showRank",id)
	#endif


	if(!p_data[id][P_SPECMODE] || !warcraft3 || !iCvar[FT_SPEC_INFO])
		return PLUGIN_CONTINUE

	new a = read_data(2)

	if ( is_user_bot(id) )
		return PLUGIN_CONTINUE

	// Not a valid target
	if (!p_data_b[a][PB_ISCONNECTED])
		return PLUGIN_CONTINUE

	WAR3_Show_Spectator_Info(id, a)
	
	return PLUGIN_CONTINUE
}

public on_EndRound(){
	 #if ADVANCED_DEBUG
		writeDebugInfo("on_EndRound",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	endround=true

	new players[32], numberofplayers
	new y, id
	get_players(players, numberofplayers)

	for (y = 0; y < numberofplayers; ++y){
		id = players[y]
		p_data_b[id][PB_SPAWNEDFROMITEM]=false
		p_data_b[id][PB_PLAYERSPAWNED]=false

		p_data_b[id][PB_MOLE] = false
		p_data[id][P_DEFUSERINDEX] = 0
	}

	for(y=0;y<33;++y)
		spawnPointsused[y]=false

	g_freezecalled = 0
	g_buyCalled=false
	g_pheonixExistsT=0
	g_pheonixExistsCT=0
	g_randomizeCalled = false

	if( task_exists(TASK_BUYTIME) )
		remove_task(TASK_BUYTIME)

	if( task_exists(TASK_BOMBTIMER) )
		remove_task(TASK_BOMBTIMER)
	
	if (iCvar[MP_SAVEXP])
		XP_Save_All()

	race9_randomize()
	
	return PLUGIN_CONTINUE
}

public on_StatusIcon(id) {

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new szStatusItem[8]
	read_data(2, szStatusItem, 7)

	if ( equali(szStatusItem, "defuser") ){
		if ( read_data(1) )
			p_data_b[id][PB_DEFUSE] = true
		else
			p_data_b[id][PB_DEFUSE] = false
	}

	if ( equali(szStatusItem, "buyzone") ){
		if ( read_data(1) )
			p_data_b[id][PB_BUYZONE] = true
		else
			p_data_b[id][PB_BUYZONE] = false
	}

	return PLUGIN_CONTINUE
}

public on_PlayerAction(){ 
	#if ADVANCED_DEBUG
		writeDebugInfo("on_PlayerAction",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new sArg[MAX_VAR_LENGTH], sAction[MAX_VAR_LENGTH] 
	new sName[MAX_NAME_LENGTH]  
	new iUserId, id

	read_logargv(0,sArg,MAX_VAR_LENGTH) 
	read_logargv(2,sAction,MAX_VAR_LENGTH) 
	parse_loguser(sArg,sName,MAX_NAME_LENGTH,iUserId) 
	id = find_player("k",iUserId)

	if (!WAR3_CD_installed(id))
		return PLUGIN_CONTINUE

	if(id==0)						// Prevents the server from gaining XP
		return PLUGIN_CONTINUE

	// Bomb planted 
	if (equal(sAction,"Planted_The_Bomb")) {  

		g_fBombTime = get_gametime() + get_cvar_float("mp_c4timer")
		
		set_task((get_cvar_float("mp_c4timer") - 0.7), "on_B4TargetBombed",TASK_BOMBTIMER)

		new origin[3]
		new teamname[32]
		new player[32]
		new numplayers
		new targetorigin[3]
		new targetid
		
		get_user_origin(id, origin)
		get_user_team(id, teamname, 31)
		get_players(player, numplayers, "ae", teamname)
		g_bombCarrier = 0	

		new iXP = 3 * xpgiven[p_data[id][P_LEVEL]]												
		
		iXP = XP_give(id, iXP)
					
		if (iCvar[FT_OBJECTIVES]){				
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_PLANTING_BOMB",sName,iXP)
		}

		for (new i=0; i<numplayers; ++i){		// Includes self
			targetid=player[i]
			get_user_origin(targetid, targetorigin)
			if (get_distance(origin, targetorigin)<=iCvar[FT_XP_RADIUS]){
				iXP = iCvar[FT_BOMBPLANTERXP] + xpgiven[p_data[targetid][P_LEVEL]]	

				iXP = XP_give(targetid, iXP)

				if (iCvar[FT_OBJECTIVES]){
					get_user_name(targetid,sName,31)					
					client_print(targetid,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SUPPORTING_BOMB",sName,iXP)
				}				
			}
		}
	}
   // Bomb defused 
	else if (equal(sAction,"Defused_The_Bomb")) {
		new origin[3]
		new teamname[32]
		new player[32]
		new numplayers
		new targetorigin[3]
		new targetid
		
		get_user_origin(id, origin)
		get_user_team(id, teamname, 31)
		get_players(player, numplayers, "ae", teamname)
		
				
		new iXP = 3 *  xpgiven[p_data[id][P_LEVEL]]											
		
		iXP = XP_give(id, iXP)
				
		if (iCvar[FT_OBJECTIVES]){
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_DEFUSING_BOMB",sName,iXP)
		}

		for (new i=0; i<numplayers; ++i){		// Includes self
			targetid=player[i]
			get_user_origin(targetid, targetorigin)
			if (get_distance(origin, targetorigin)<=iCvar[FT_XP_RADIUS]){
				
				iXP = iCvar[FT_DEFUSEXP] +  xpgiven[p_data[targetid][P_LEVEL]]										
				iXP = XP_give(targetid, iXP)
							
				if (iCvar[FT_OBJECTIVES]){
					get_user_name(targetid,sName,31)						
					client_print(targetid,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SUPPORT_BOMB_D",sName,iXP)
				}					
			}
		}		
	} 
	// Bomb defusing with a kit 
	else if (equal(sAction,"Begin_Bomb_Defuse_With_Kit")) { 

		if((++p_data[id][P_DEFUSERINDEX] == 1) && (get_user_team(id) == 2) ){  // Team 1 = Terror, Team 2 = CT
			new iXP = iCvar[FT_DEFUSEXP] + xpgiven[p_data[id][P_LEVEL]]

			iXP = XP_give(id, iXP)
				
			if (iCvar[FT_OBJECTIVES]){	
				client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_ATTEMPT_BOMB_D",sName,iXP)
			}
		}               
   } 
   // Bomb defusing without a kit 
	else if (equal(sAction,"Begin_Bomb_Defuse_Without_Kit")) { 
     	
		if( (++p_data[id][P_DEFUSERINDEX] == 1) && (get_user_team(id) == 2) ){  // Team 1 = Terror, Team 2 = CT
			new iXP = xpgiven[p_data[id][P_LEVEL]]

			iXP = XP_give(id, iXP)

			if (iCvar[FT_OBJECTIVES]){
				client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_ATTEMPT_BOMB_D_BARE",id,iXP)
			}
		}
   } 
	// Spawned with the bomb 
	else if (equal(sAction,"Spawned_With_The_Bomb")) { 
		g_bombCarrier = id
	  
		new iXP = xpgiven[p_data[id][P_LEVEL]]											

		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES]){
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SPAWNING_WITH_BOMB",sName,iXP)
		}
	} 

	// Dropped bomb 
	else if (equal(sAction,"Dropped_The_Bomb")) { 
		g_bombCarrier = 0
	  
		new iXP = -1 * xpgiven[p_data[id][P_LEVEL]]											

		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_LOST_BOMB",sName,(-1*iXP))

   } 
	// Picked up bomb 
	else if (equal(sAction,"Got_The_Bomb")) { 
		g_bombCarrier = id
	  
		new iXP = xpgiven[p_data[id][P_LEVEL]]	

		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_PICKING_UP_BOMB",sName,iXP)

	} 
	// Hostage touched 
	else if (equal(sAction,"Touched_A_Hostage")) { 
		new iXP = xpgiven[p_data[id][P_LEVEL]]	

		iXP = XP_give(id, iXP)
				
		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_BEGINNING_HOSTAGE_RESCUE",sName,iXP)

		g_hostageSaver = id
	} 
	// Hostage rescued 
	else if (equal(sAction,"Rescued_A_Hostage")) { 
		new origin[3]
		new teamname[32]
		new player[32]
		new numplayers
		new targetorigin[3]
		new targetid		
		
		get_user_origin(id, origin)
		get_user_team(id, teamname, 31)
		get_players(player, numplayers, "ae", teamname)
		
		new iXP = xpgiven[p_data[id][P_LEVEL]]												
			
		iXP = XP_give(id, iXP)
				
		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_RESCUING_HOSTAGE",sName,iXP)

		// Supporters and self	
		for (new i=0; i<numplayers; ++i){
			targetid=player[i]
			get_user_origin(targetid, targetorigin)
			if (get_distance(origin, targetorigin)<=iCvar[FT_XP_RADIUS]){
				iXP = iCvar[FT_HOSTAGEXP] +  xpgiven[p_data[targetid][P_LEVEL]]	

				iXP = XP_give(targetid, iXP)

				if (iCvar[FT_OBJECTIVES]){
					get_user_name(targetid,sName,31)
					client_print(targetid,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SUPPORTING_HOSTAGE_RUN",sName,iXP)
				}					
			}				
		}				
   } 

	// Hostage killed 
	else if (equal(sAction,"Killed_A_Hostage")) { 
		new iXP = -1 * xpgiven[p_data[id][P_LEVEL]]

		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_KILLING_HOSTAGE",sName,(-1*iXP))		

	} 
	// VIP spawn 
	else if (equal(sAction,"Became_VIP")) { 
		g_vipID = id
	  
		new iXP = xpgiven[p_data[id][P_LEVEL]]										
		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES]){
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SPAWN_AS_VIP",sName,iXP)
		}
		
	}

	// VIP assassinated 
	else if (equal(sAction,"Assassinated_The_VIP")) { 
		new sNameVIP[MAX_NAME_LENGTH] 
		get_user_name( g_vipID,sNameVIP,MAX_NAME_LENGTH) 		

		new iXP = iCvar[FT_VIP_KILL_BONUS] + xpgiven[p_data[id][P_LEVEL]]	

		iXP = XP_give(id, iXP)

		if (iCvar[FT_OBJECTIVES])
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_KILLING_VIP",sName,iXP,sNameVIP) 	
	} 
	// VIP escaped 
	else if (equal(sAction,"Escaped_As_VIP")) { 							
		new origin[3]
		new teamname[32]
		new player[32]
		new numplayers
		new targetorigin[3]
		new targetid

		get_user_origin(id, origin)
		get_user_team(id, teamname, 31)
		get_players(player, numplayers, "ae", teamname)
		
		new iXP = xpgiven[p_data[id][P_LEVEL]]

		iXP = XP_give(id, iXP)

		new nName[31]			
		if (iCvar[FT_OBJECTIVES]){	
			client_print(id,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_ESCAPED_VIP",sName,iXP)
		}

		for (new i=0; i<numplayers; ++i){		// Includes self
			targetid=player[i]
			get_user_origin(targetid, targetorigin)
			if (get_distance(origin, targetorigin)<=iCvar[FT_XP_RADIUS] && targetid!=id){
				iXP = iCvar[FT_VIP_ESCAPE_BONUS] + xpgiven[p_data[targetid][P_LEVEL]]
					
				iXP = XP_give(targetid, iXP)

				if (iCvar[FT_OBJECTIVES]){
					get_user_name(targetid,nName,31)					
					client_print(targetid,print_chat, "%s %L", g_MODclient, id,"AWARD_FOR_SUPPORTING_VIP",nName,iXP,sName)
				}				
			}
		}
	} 

	return PLUGIN_HANDLED 
}

public on_TargetBombed() {
	#if ADVANCED_DEBUG
		writeDebugInfo("on_TargetBombed",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	set_task(0.1,"_on_TargetBombed",TASK_TARGETBOMBED)

	return PLUGIN_CONTINUE
}

public _on_TargetBombed(){
	#if ADVANCED_DEBUG
		writeDebugInfo("_on_TargetBombed",0)
	#endif


	if (!warcraft3)
		return PLUGIN_CONTINUE

	if (get_gametime() - g_fBombTime < 1.0){
		new players[32], numberofplayers, id, i
		get_players(players, numberofplayers)
		for(i=0;i<numberofplayers;i++){
			id = players[i]
			
			if( !p_data_b[id][PB_JUSTJOINED] && !is_user_alive(id) ){
				p_data_b[id][PB_DIEDLASTROUND] = true
			}
		}
	}

	return PLUGIN_CONTINUE
}

public on_B4TargetBombed(){
	#if ADVANCED_DEBUG
		writeDebugInfo("on_B4TargetBombed",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new players[32], numberofplayers, id, i
	get_players(players, numberofplayers)
	for(i=0;i<numberofplayers;i++){
		id = players[i]
		if(is_user_alive(id)){
			saveweapons(id)
		}
	}

	return PLUGIN_CONTINUE
}


public on_FreezeTimeComplete() {
	#if ADVANCED_DEBUG
		writeDebugInfo("on_FreezeTimeComplete",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	g_freezetime = false
	
	new players[32],numberofplayers, id, i, parm[1]
	get_players(players, numberofplayers)
	
	for (i = 0; i < numberofplayers; ++i){
		id=players[i]

		WAR3_Display_Level(id, DISPLAYLEVEL_NONE)
		parm[0]=id
		set_task(0.1,"unholyspeed",TASK_UNHOLYSPEED+id,parm,1)
	}

	return PLUGIN_CONTINUE
}

public on_TerroristWin(){
	#if ADVANCED_DEBUG
		writeDebugInfo("on_TerroristWin",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new players[32], numberofplayers,a, bool:give
	get_players(players,numberofplayers,"e","TERRORIST") 
	for (a = 0; a < numberofplayers; ++a){
		give = true
		if (iCvar[FT_CD]) {
			if (!!WAR3_CD_installed(players[a])){
				give = false
			}
		}

		if(p_data_b[players[a]][PB_JUSTJOINED] || p_data_b[players[a]][PB_CHANGINGTEAM])
			give = false

		if(give){
			new iXP = (iCvar[FT_ROUND_WIN_XP] + xpgiven[p_data[players[a]][P_LEVEL]])
			iXP = XP_give(players[a], iXP)
			if (iCvar[FT_OBJECTIVES]){				
				client_print(players[a],print_chat, "%s %L", g_MODclient, players[a],"AWARD_FOR_WINNING_ROUND",iXP)
			}				
		}
	}
	return PLUGIN_CONTINUE
}

public on_CTWin(){
	#if ADVANCED_DEBUG
		writeDebugInfo("on_CTWin",0)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new players[32], numberofplayers,a, bool:give
	get_players(players,numberofplayers,"e","CT") 
	for (a = 0; a < numberofplayers; ++a){
		give = true
		if (iCvar[FT_CD]) {
			if (!!WAR3_CD_installed(players[a])){
				give = false
			}
		}

		if(p_data_b[players[a]][PB_JUSTJOINED] || p_data_b[players[a]][PB_CHANGINGTEAM])
			give = false

		if(give){
			new iXP = (iCvar[FT_ROUND_WIN_XP] + xpgiven[p_data[players[a]][P_LEVEL]])
			iXP = XP_give(players[a], iXP)

			if (iCvar[FT_OBJECTIVES]){			
				client_print(players[a],print_chat, "%s %L", g_MODclient, players[a],"AWARD_FOR_WINNING_ROUND",iXP)
			}				
		}
	}
	return PLUGIN_CONTINUE
}

public on_ArmorType(id){
	#if ADVANCED_DEBUG
		writeDebugInfo("on_ArmorType",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	if (read_data(1))
		p_data_b[id][PB_HELMET]=true
	else
		p_data_b[id][PB_HELMET]=false

	return PLUGIN_CONTINUE
}

public on_WeapPickup(id){ 
	#if ADVANCED_DEBUG
		writeDebugInfo("on_WeapPickup",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	new weapon = read_data(1)

	if (weapon==25)
		++p_data[id][P_FLASHCOUNT]

	if ( isPrimary(weapon) )
		p_data_b[id][PB_SHIELD]=false

	return PLUGIN_CONTINUE 
}

public on_ShowStatus(id){
	#if ADVANCED_DEBUG
		writeDebugInfo("on_ShowStatus",id)
	#endif

	if (!warcraft3)
		return PLUGIN_CONTINUE

	if (g_freezetime){
		Create_StatusText(id, 0, "")
		return PLUGIN_HANDLED
	}

	new pid = read_data(2)

	if(get_user_team(id)==get_user_team(pid) && (p_data[pid][P_ITEM2]==ITEM_CHAMELEON || (p_data[pid][P_SKINCHANGED] == SKIN_HEX || p_data[pid][P_SKINCHANGED]==SKIN_SWITCH)) && is_user_alive(pid)){
		client_print(id,print_center,"%L",id,"HES_ON_YOUR_TEAM_DONT_SHOOT")
		client_cmd(id,"speak warcraft3/antend.wav")
	}

	// Used to show player icons
	if (g_spritesEnabled){
		if (warcraft3 && (get_user_team(id) == get_user_team(pid))){
			if (iCvar[FT_RACE_ICONS] && p_data[id][P_SHOWICONS]){
				Create_TE_PLAYERATTACHMENT(id, pid, 55, g_sRace[p_data[pid][P_RACE]], 15)
			}

			if (iCvar[FT_LEVEL_ICONS] && p_data[id][P_SHOWICONS])
				Create_TE_PLAYERATTACHMENT(id, pid, 35, g_sLevel[p_data[pid][P_LEVEL]], 16)
		}
	}

	return PLUGIN_CONTINUE
}