// ***************************************************************************
// Author: B. Josh Becker
// Description: This page contains stocks, mostly functions that create messages
//		that are sent to the client
// ***************************************************************************

stock Create_TE_Smoke(originSight[3], position[3], iSprite, scale, framerate){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_Smoke",0)
	#endif

	message_begin( MSG_PVS, SVC_TEMPENTITY, originSight )
	write_byte( TE_SMOKE )
	write_coord( position[0] )			// Position
	write_coord( position[1] )
	write_coord( position[2] )
	write_short( iSprite )				// Sprite index
	write_byte( scale )					// scale * 10
	write_byte( framerate  )			// framerate
	message_end()
}

stock Create_DeathMsg_CS(killer_id,victim_id,headshot,weaponname[]){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_DeathMsg_CS",victim_id)
	#endif

	message_begin(MSG_ALL,gmsgDeathMsg)
	write_byte(killer_id)
	write_byte(victim_id)
	write_byte(headshot)
	write_string(weaponname)
	message_end()
}

stock Create_TE_IMPLOSION(position[3], radius, count, life){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_IMPLOSION",0)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )
	write_byte ( TE_IMPLOSION )
	write_coord( position[0] )			// position (X)
	write_coord( position[1] )			// position (Y)
	write_coord( position[2] )			// position (Z)
	write_byte ( radius )				// radius
	write_byte ( count )				// count
	write_byte ( life )					// life in 0.1's
	message_end()
}

stock Create_TE_BEAMCYLINDER(origin[3], center[3], axis[3], iSprite, startFrame, frameRate, life, width, amplitude, red, green, blue, brightness, speed){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_BEAMCYLINDER",0)
	#endif

	message_begin( MSG_PAS, SVC_TEMPENTITY, origin )
	write_byte( TE_BEAMCYLINDER )
	write_coord( center[0] )			// center position (X)
	write_coord( center[1] )			// center position (Y)
	write_coord( center[2] )			// center position (Z)
	write_coord( axis[0] )				// axis and radius (X)
	write_coord( axis[1] )				// axis and radius (Y)
	write_coord( axis[2] )				// axis and radius (Z)
	write_short( iSprite )				// sprite index
	write_byte( startFrame )			// starting frame
	write_byte( frameRate )				// frame rate in 0.1's
	write_byte( life )					// life in 0.1's
	write_byte( width )					// line width in 0.1's
	write_byte( amplitude )				// noise amplitude in 0.01's
	write_byte( red )					// color (red)
	write_byte( green )					// color (green)
	write_byte( blue )					// color (blue)
	write_byte( brightness )			// brightness
	write_byte( speed )					// scroll speed in 0.1's
	message_end()
}

stock Create_TE_EXPLOSION( origin[3], origin2[3], iSprite, scale, frameRate, flags ){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_EXPLOSION",0)
	#endif

	message_begin( MSG_PVS, SVC_TEMPENTITY, origin )
	write_byte( TE_EXPLOSION )
	write_coord( origin2[0] )			// position (X)
	write_coord( origin2[1] )			// position (Y)
	write_coord( origin2[2]	)			// position (Z)
	write_short( iSprite )				// sprite index
	write_byte( scale )					// scale in 0.1's
	write_byte( frameRate )				// framerate
	write_byte( flags )					// flags
	message_end()
}

stock Create_ScreenShake(id, amount, duration, frequency){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_ScreenShake",id)
	#endif

	message_begin(MSG_ONE,gmsgScreenShake,{0,0,0},id) 
	write_short( amount )				// ammount 
	write_short( duration )				// lasts this long 
	write_short( frequency )			// frequency
	message_end()
}

stock Create_ScreenFade(id, duration, holdtime, fadetype, red, green, blue, alpha){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_ScreenFade",id)
	#endif

	message_begin( MSG_ONE,gmsgScreenFade,{0,0,0},id )			
	write_short( duration )			// fade lasts this long duration
	write_short( holdtime )			// fade lasts this long hold time
	write_short( fadetype )			// fade type (in / out)
	write_byte( red )				// fade red
	write_byte( green )				// fade green
	write_byte( blue )				// fade blue
	write_byte( alpha )				// fade alpha
	message_end()
}

stock Create_ScoreInfo(id,frags,deaths,playerClass,team ){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_ScoreInfo",id)
	#endif

	message_begin(MSG_ALL,gmsgScoreInfo)
	write_byte(id)
	write_short(frags)
	write_short(deaths)
	write_short(playerClass)
	write_short(team)
	message_end()
}

stock Create_TE_SPRITETRAIL(start[3], end[3], iSprite, count, life, scale, velocity, random ){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_SPRITETRAIL",0)
	#endif

	message_begin( MSG_BROADCAST,SVC_TEMPENTITY)
	write_byte( TE_SPRITETRAIL )
	write_coord( start[0] )				// start position (X)
	write_coord( start[1] )				// start position (Y)
	write_coord( start[2] )				// start position (Z)
	write_coord( end[0] )				// end position (X)
	write_coord( end[1] )				// end position (Y)
	write_coord( end[2] )				// end position (Z)
	write_short( iSprite )				// sprite index
	write_byte( count )					// count
	write_byte( life)					// life in 0.1's
	write_byte( scale)					// scale in 0.1's
	write_byte( velocity )				// velocity along vector in 10's
	write_byte( random )				// randomness of velocity in 10's
	message_end()
}

stock Create_StatusText(id, linenumber, text[]){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_StatusText",id)
	#endif

	message_begin( MSG_ONE, gmsgStatusText, {0,0,0}, id )
	write_byte( linenumber )			// line number of status bar text
	write_string( text )				// status bar text
	message_end()
}

stock Create_BarTime(id, duration, flag){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_BarTime",id)
	#endif

	message_begin( MSG_ONE, gmsgBarTime, {0,0,0}, id )
	write_byte( duration ) // duration 
	write_byte( flag )
	message_end() 
}

stock Create_TE_BEAMFOLLOW(entity, iSprite, life, width, red, green, blue, alpha){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_BEAMFOLLOW",entity)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )
	write_byte( TE_BEAMFOLLOW )
	write_short( entity )			// entity
	write_short( iSprite )			// model
	write_byte( life )				// life
	write_byte( width )				// width
	write_byte( red )				// red
	write_byte( green )				// green
	write_byte( blue )				// blue
	write_byte( alpha )				// brightness
	message_end()
}

stock Create_TE_BEAMPOINTS(start[3], end[3], iSprite, startFrame, frameRate, life, width, noise, red, green, blue, alpha, speed){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_BEAMPOINTS",0)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )
	write_byte( TE_BEAMPOINTS )
	write_coord( start[0] )
	write_coord( start[1] )
	write_coord( start[2] )
	write_coord( end[0] )
	write_coord( end[1] )
	write_coord( end[2] )
	write_short( iSprite )			// model
	write_byte( startFrame )		// start frame
	write_byte( frameRate )			// framerate
	write_byte( life )				// life
	write_byte( width )				// width
	write_byte( noise )				// noise
	write_byte( red)				// red
	write_byte( green )				// green
	write_byte( blue )				// blue
	write_byte( alpha )				// brightness
	write_byte( speed )				// speed
	message_end()
}

stock Create_TE_BEAMENTS(startEntity, endEntity, iSprite, startFrame, frameRate, life, width, noise, red, green, blue, alpha, speed){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_BEAMENTS",startEntity)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )
	write_byte( TE_BEAMENTS )
	write_short( startEntity )		// start entity
	write_short( endEntity )		// end entity
	write_short( iSprite )			// model
	write_byte( startFrame )		// starting frame
	write_byte( frameRate )			// frame rate
	write_byte( life )				// life
	write_byte( width )				// line width
	write_byte( noise )				// noise amplitude
	write_byte( red )				// red
	write_byte( green )				// green
	write_byte( blue )				// blue
	write_byte( alpha )				// brightness
	write_byte( speed )				// scroll speed
	message_end()
}

stock Create_TE_ELIGHT(entity, start[3], radius, red, green, blue, life, decayRate){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_ELIGHT",entity)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )
	write_byte( TE_ELIGHT )
	write_short( entity )			// entity
	write_coord( start[0] )			// initial position
	write_coord( start[1] )			// initial position
	write_coord( start[2] )			// initial position
	write_coord( radius )			// radius
	write_byte( red )				// red
	write_byte( green )				// green
	write_byte( blue )				// blue
	write_byte( life )				// life
	write_coord( decayRate )		// decay rate
	message_end()
}

stock Create_TE_SPRAY(position[3], direction[3], iSprite, count, speed, noise, rendermode){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_SPRAY",0)
	#endif

	message_begin( MSG_BROADCAST,SVC_TEMPENTITY ) 
	write_byte( TE_SPRAY )
	write_coord( position[0] )		// Position
	write_coord( position[1] ) 
	write_coord( position[2] ) 
	write_coord( direction[0] )		// Direction 
	write_coord( direction[1] )  
	write_coord( direction[2] ) 
	write_short( iSprite )			// Sprite
	write_byte( count )				// count 
	write_byte( speed )				// speed 
	write_byte( noise )				// noise
	write_byte( rendermode )		// rendermode
	message_end() 
}

stock Create_TE_SPRITE(position[3], iSprite, scale, alpha){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_SPRITE",0)
	#endif

	message_begin( MSG_BROADCAST,SVC_TEMPENTITY ) 
	write_byte( TE_SPRITE ) 
	write_coord( position[0] )		// position)
	write_coord( position[1] ) 
	write_coord( position[2] ) 
	write_short( iSprite )			// sprite index
	write_byte( scale)				// scale in 0.1's
	write_byte( alpha )				// brightness
	message_end() 
}

stock Create_StatusIcon(id, status, sprite[], red, green, blue){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_StatusIcon",id)
	#endif

	message_begin( MSG_ONE, gmsgStatusIcon, {0,0,0}, id ) 
	write_byte( status )			// status 
	write_string( sprite )			// sprite name 
	write_byte( red )				// red 
	write_byte( green )				// green 
	write_byte( blue )				// blue 
	message_end()
}

stock Create_TE_LARGEFUNNEL(position[3], iSprite, flags){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_LARGEFUNNEL",0)
	#endif

	message_begin( MSG_BROADCAST, SVC_TEMPENTITY )	
	write_byte( TE_LARGEFUNNEL )
	write_coord( position[0] )		// origin, x
	write_coord( position[1] )		// origin, y
	write_coord( position[2] )		// origin, z
	write_short( iSprite )			// sprite (0 for none)
	write_short( flags )			// 0 for collapsing, 1 for sending outward
	message_end() 
}

stock Create_TE_PLAYERATTACHMENT(id, entity, vOffset, iSprite, life){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_PLAYERATTACHMENT",id)
	#endif

	message_begin( MSG_ONE, SVC_TEMPENTITY, { 0, 0, 0 }, id )
	write_byte( TE_PLAYERATTACHMENT )
	write_byte( entity )			// entity
	write_coord( vOffset )			// vertical offset ( attachment origin.z = player origin.z + vertical offset )
	write_short( iSprite )			// model index
	write_short( life )				// (life * 10 )
	message_end()
}

stock Create_TE_TELEPORT(position[3]){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_TELEPORT",0)
	#endif

	message_begin( MSG_BROADCAST,SVC_TEMPENTITY) 
	write_byte( TE_TELEPORT ) 
	write_coord( position[0] ) 
	write_coord( position[1] ) 
	write_coord( position[2] ) 
	message_end()
}

stock Create_TE_LAVASPLASH(position[3]){
	#if ADVANCED_DEBUG == 1
		writeDebugInfo("Create_TE_LAVASPLASH",0)
	#endif

	message_begin( MSG_BROADCAST,SVC_TEMPENTITY) 
	write_byte( TE_LAVASPLASH ) 
	write_coord( position[0] ) 
	write_coord( position[1] ) 
	write_coord( position[2] ) 
	message_end()
}