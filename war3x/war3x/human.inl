// Begin HUMAN.INL

/* - Skill Configuration ---------------------------------------- */


new Float:s_Fortitude[2]        = {105.0,165.0};        // (racial) Fortitude (hitpoints gained)
new Float:s_Invisibility[3]     = {0.60,0.70,0.80};     // (skill1) Invisibility (percent invisibility)
new Float:s_Bash[3]             = {0.15,0.25,0.35};     // (skill2) Bash (chance to bash)
new s_BashDamage[3]             = {1,3,6};              // (skill2) Bash (bonus damage)
new s_ifArmor[3]                = {130,160,190};        // (skill3) Inner Fire (max armor)
new Float:s_ifDamage[3]         = {0.05,0.10,0.15};     // (skill3) Inner Fire (bonus damage)


/* - Skill Constants Configuration ------------------------------ */


#define BASH_DURATION                        1.0        // (  float) seconds player is bashed

#define INVISIBILITY_PRESHIFT                5.0        // (  float) length of time before invisibility shift
#define INVISIBILITY_DURATION               25.0        // (  float) length of time to fade from max invisibility to min
#define INVISIBILITY_SHIFTS                   15        // (integer) number of invisibility level shifts
#define INVISIBILITY_COOLDOWN                3.0        // (  float) seconds of cooldown after attacking / being hit with invis

new BASH_RGB[3] =                      {96,96,96};      // (integer) RGB of bash glow shell ( when bashed )


/* - Ultimate Configuration ------------------------------------- */


#define HOLYLIGHT_DAMAGE             50     // (integer) damage
#define HOLYLIGHT_HEAL               75     // (integer) heal
#define HOLYLIGHT_BLINDTIME           2     // (integer) seconds to blind undead upon nuke

#define FLAMESTRIKE_RADIUS            2     // (integer) meters from center that blaze is active
#define FLAMESTRIKE_DAMAGEIN         16     // (integer) damage / second while in flames
#define FLAMESTRIKE_DAMAGEOUT         6     // (integer) seconds to take damage when out of flames
#define FLAMESTRIKE_DURATION       10.0     // (integer) seconds flamestrike is active
#define FLAMESTRIKE_SINGEDURATION     3     // (integer) seconds flamestrike is active

#define AVATAR_DURATION            10.0     // (  float) seconds avatar is active
#define AVATAR_STEPSIZE               5     // (integer) ammount of health to give/remove each 0.1 second when growing/shrinking
#define AVATAR_SIZE                 160     // (integer) size of the glowshell on the player
#define AVATAR_HEALTH                60     // (integer) bonus health to apply
#define AVATAR_DAMAGE                 6     // (integer) bonus damage to apply
#define AVATAR_SPEED              180.0     // (  float) movement speed of player while in avatar

new AVATAR_RGB[3] =           {48,118,122};      // (integer) RGB of bash glow shell ( when bashed )


/* - Events ----------------------------------------------------- */


public Skills_Offensive_HU( attackerId, victimId, weaponId, iDamage, headshot ) {
#if ADVANCED_DEBUG
	log_function("public Skills_Offensive_HU( attackerId, victimId, weaponId, iDamage, headshot ) {");
#endif

    if ( g_PlayerInfo[attackerId][CURRENT_RACE] == RACE_HUMAN && get_user_team( attackerId ) != get_user_team( victimId ) && get_user_health( victimId ) > 0 )
    {
        // Bash

        if ( g_PlayerInfo[attackerId][CURRENT_SKILL2] )
            SBash( attackerId, victimId, weaponId, headshot );

        // Inner Fire

        if ( g_PlayerInfo[attackerId][CURRENT_SKILL3] && get_user_armor(attackerId) )
            SInnerFire( attackerId, victimId, weaponId, iDamage, headshot );
    }

    return PLUGIN_HANDLED;
}


public Skills_Defensive_HU( victimId ) {
#if ADVANCED_DEBUG
	log_function("public Skills_Defensive_HU( victimId ) {");
#endif

    if ( g_PlayerInfo[victimId][CURRENT_RACE] == RACE_HUMAN )
    {
        // Invisibility Cooldown ( taken care of in on_Damage() )
    }

    return PLUGIN_HANDLED;
}


public Ultimates_HU( Caster, Target ) {
#if ADVANCED_DEBUG
	log_function("public Ultimates_HU( Caster, Target ) {");
#endif

    // Holy Light

    if ( g_PlayerInfo[Caster][CURRENT_ULTIMATE] == ULTIMATE_HOLYLIGHT )
    {
        // Heal

        if ( get_user_team( Target ) == get_user_team( Caster ) && g_PlayerInfo[Target][CURRENT_RACE] != RACE_UNDEAD )
        {
            if ( get_user_health( Target ) == WAR3_get_maxhealth( Target ) && Caster != Target )
            {
                new szMessage[128];
                copy( szMessage, 127, FULLHEALTH_TARGET );

                WAR3_status_text( Caster, szMessage, 0.5 );

                Ultimate_Beep( Caster );
            }

            else
            {
                UHolyLight_Heal( Caster, Target );
                Ultimate_Cooldown( Caster, ULTIMATE_COOLDOWNDEFAULT );

                Invis_Cooldown( Caster );
            }
        }

        // Damage

        else if ( get_user_team( Target ) != get_user_team( Caster ) && g_PlayerInfo[Target][CURRENT_RACE] == RACE_UNDEAD )
        {
            UHolyLight_Dmg( Caster, Target );
            Ultimate_Cooldown( Caster, ULTIMATE_COOLDOWNDEFAULT );

            Invis_Cooldown( Caster );
        }

        else
        {
            new szMessage[128];
            copy( szMessage, 127, CANT_TARGET_HOLYLIGHT );

            WAR3_status_text( Caster, szMessage, 0.5 );

            Ultimate_Beep( Caster );
        }
    }

    // Flame Strike

    else if ( g_PlayerInfo[Caster][CURRENT_ULTIMATE] == ULTIMATE_FLAMESTRIKE && get_user_team( Target ) != get_user_team( Caster ) )
    {
        UFstrike_Init( Caster, Target );
        Ultimate_Cooldown( Caster, ULTIMATE_COOLDOWNDEFAULT );

        Invis_Cooldown( Caster );
    }

    // Avatar

    else if ( g_PlayerInfo[Caster][CURRENT_ULTIMATE] == ULTIMATE_AVATAR )
    {
        UAvatar_Cast( Caster );
        Ultimate_Cooldown( Caster, ULTIMATE_COOLDOWNDEFAULT );

        Invis_Cooldown( Caster );
    }

    else
    {
        Ultimate_Beep( Caster );
        return PLUGIN_HANDLED;
    }

    return PLUGIN_HANDLED;
}


/* - Racial Ability --------------------------------------------- */


// Fortitude

public SFortitude_Get( iLevel ) {
#if ADVANCED_DEBUG
	log_function("public SFortitude_Get( iLevel ) {");
#endif

    new Float:fLevel = float( iLevel );

    if ( fLevel > LEVEL_RACIALCAP )
        fLevel = LEVEL_RACIALCAP;

    new Float:fNewHealth = s_Fortitude[0] + ( ( ( s_Fortitude[1] - s_Fortitude[0] ) / LEVEL_RACIALCAP ) * fLevel );
    new iNewHealth = floatround( fNewHealth );

    return ( iNewHealth );
}


public SFortitude_Set( id ) {
#if ADVANCED_DEBUG
	log_function("public SFortitude_Set( id ) {");
#endif

    // Check if restricted

    if ( !WAR3_skill_enabled( id, RACE_HUMAN, SKILL_RACIAL ) )
        return PLUGIN_HANDLED;

    new iLevel = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] );
    new iNewHealth = SFortitude_Get( iLevel );

    set_user_health( id, iNewHealth );

    return PLUGIN_HANDLED;
}


/* - Skills ----------------------------------------------------- */


// Invisibility

public SInvis_Set( id ) {
#if ADVANCED_DEBUG
	log_function("public SInvis_Set( id ) {");
#endif

    g_bPlayerInvis[id] = true;

    new Float:fInvis = s_Invisibility[g_PlayerInfo[id][CURRENT_SKILL1] - 1];
    fInvis = ( 1.0 - fInvis ) * 255.0;

    new iInvis = floatround( fInvis );
    set_user_rendering( id, kRenderFxNone, 0, 0, 0, kRenderTransTexture, iInvis );

    Invis_Icon( id, ICON_SHOW );

    // Set shift task

    new parmInvis[1];
    parmInvis[0] = id;

    new iTaskId = TASK_INVIS + id;
    set_task( INVISIBILITY_PRESHIFT, "_SInvis_Shift", iTaskId, parmInvis, 1 );

    return PLUGIN_HANDLED;
}


public _SInvis_Shift( parmInvis[1] ) {
#if ADVANCED_DEBUG
	log_function("public _SInvis_Shift( parmInvis[1] ) {");
#endif

    new id = parmInvis[0];

    if ( !is_user_connected( id ) )
        return PLUGIN_HANDLED;

    // Find max shift value(s)

    new Float:fCurrentInvis = entity_get_float( id, EV_FL_renderamt );
    new Float:fInvisPercent = 1.0 - ( fCurrentInvis / 255.0 );

    if ( WAR3_is_invisible( id ) && fInvisPercent > s_Invisibility[LEVEL_1] )
    {
        new Float:fMaxLevel = ( 1.0 - s_Invisibility[g_PlayerInfo[id][CURRENT_SKILL1] - 1] ) * 255.0;
        new Float:fMinLevel = ( 1.0 - s_Invisibility[LEVEL_1] ) * 255.0;

        new Float:fInvisShift = ( fMinLevel - fMaxLevel ) / float( INVISIBILITY_SHIFTS );
        entity_set_float( id, EV_FL_renderamt, fCurrentInvis + fInvisShift );

        Invis_Icon( id, ICON_SHOW );

        // Repeat

        new Float:fNextCheck = INVISIBILITY_DURATION / float( INVISIBILITY_SHIFTS );

        new iTaskId = TASK_INVIS + id;
        set_task( fNextCheck, "_SInvis_Shift", iTaskId, parmInvis, 1 );
    }

    else
    {
        new iTaskId = TASK_INVIS + id;
        remove_task( iTaskId, 0 );
    }

    return PLUGIN_HANDLED;
}


// Bash

public SBash( iAttackerId, iVictimId, iWeaponId, iHeadshot ) {
#if ADVANCED_DEBUG
	log_function("public SBash( iAttackerId, iVictimId, iWeaponId, iHeadshot ) {");
#endif

    // Check if restricted

    if ( !WAR3_skill_enabled( iAttackerId, RACE_HUMAN, SKILL_2 ) )
        return PLUGIN_HANDLED;

    new Float:fBashChance = s_Bash[g_PlayerInfo[iAttackerId][CURRENT_SKILL2] - 1];
    new Float:fRandomNum  = random_float( 0.0,1.0 );

    if ( fBashChance < fRandomNum )
        return PLUGIN_HANDLED;


    new iBonusDamage = s_BashDamage[g_PlayerInfo[iAttackerId][CURRENT_SKILL2] - 1];

    // Add to player stats array

    if ( get_cvar_num( "mp_war3stats" ) )
    {
        playerSkill2Info[iAttackerId][0] += iBonusDamage;
        playerSkill2Info[iAttackerId][1] += 1;
    }

    // Glow

    Glow_Set( iVictimId, BASH_DURATION, BASH_RGB, 24 );

    // Apply Damage

    if ( iBonusDamage > 0 )
        WAR3_damage( iAttackerId, iVictimId, iWeaponId, iBonusDamage, iHeadshot, DAMAGE_CHECKARMOR );

    if ( is_user_alive( iVictimId ) )
    {
        // Immobilize

        SBash_Stun( iVictimId );

        // Screen Fade

        Create_ScreenFade( iVictimId, (1<<10), (1<<10), FADE_OUT, 255, 255, 255, 100 );
    }

    return PLUGIN_HANDLED;
}


public SBash_Stun( id ) {
#if ADVANCED_DEBUG
	log_function("public SBash_Stun( id ) {");
#endif

    g_bPlayerBashed[id] = true;
    g_bPlayerCantMove[id] = true;

    WAR3_set_speed( id );

    new parm_Stun[1];
    parm_Stun[0] = id;

    new TaskId = TASK_BASH + id;

    remove_task( TaskId, 0 );
    set_task( BASH_DURATION, "SBash_Remove", TaskId, parm_Stun, 1 );

    return PLUGIN_HANDLED;
}


public SBash_Remove( parm_Stun[1] ) {
#if ADVANCED_DEBUG
	log_function("public SBash_Remove( parm_Stun[1] ) {");
#endif

    new id = parm_Stun[0];

    new TaskId = TASK_BASH + id;
    remove_task( TaskId, 0 );

    g_bPlayerBashed[id] = false;

    Immobilize_Remove( id );
    WAR3_set_speed( id );

    return PLUGIN_HANDLED;
}


// Inner Fire

public SInnerFire( iAttackerId, iVictimId, iWeaponId, iDamage, iHeadshot ) {
#if ADVANCED_DEBUG
	log_function("public SInnerFire( iAttackerId, iVictimId, iWeaponId, iDamage, iHeadshot ) {");
#endif

    // Check if restricted

    if ( !WAR3_skill_enabled( iAttackerId, RACE_HUMAN, SKILL_3 ) )
        return PLUGIN_HANDLED;

    new Float:fBonusDamage = float( iDamage ) * s_ifDamage[g_PlayerInfo[iAttackerId][CURRENT_SKILL3] - 1];
    new iBonusDamage = floatround( fBonusDamage );

    // Add to player stats array

    if ( get_cvar_num( "mp_war3stats" ) )
    {
        playerSkill3Info[iAttackerId][0] += iBonusDamage;
    }

    // Apply Damage

    if ( iBonusDamage > 0 )
        WAR3_damage( iAttackerId, iVictimId, iWeaponId, iBonusDamage, iHeadshot, DAMAGE_CHECKARMOR );

    return PLUGIN_HANDLED;
}


/* - Holy Light ------------------------------------------------- */


// Healing Version

public UHolyLight_Heal( iCasterId, iTargetId ) {
#if ADVANCED_DEBUG
	log_function("public UHolyLight_Heal( iCasterId, iTargetId ) {");
#endif

    // Play Sound

    emit_sound( iTargetId, CHAN_STATIC, SOUND_HOLYLIGHT, 1.0, ATTN_NORM, 0, PITCH_NORM );

    // Status Text

    WAR3_status_text( iCasterId, HEAL_CAST, 1.0 );

    // Calculate New Health

    new iNewHealth = get_user_health( iTargetId ) + HOLYLIGHT_HEAL;
    new iMaxHealth = WAR3_get_maxhealth( iTargetId );

    new iHealthGiven = HOLYLIGHT_HEAL;

    if ( iNewHealth > iMaxHealth )
    {
        iHealthGiven -= ( iNewHealth - iMaxHealth );
        iNewHealth = iMaxHealth;
    }

    // Hud Message

    new szMessage[128], szPlayerName[32];
    get_user_name( iCasterId, szPlayerName, 31 );

    format( szMessage, 127, HEAL_TARGET, szPlayerName, iHealthGiven );

    WAR3_status_text( iTargetId, szMessage, 3.0 );

    // Apply Health

    set_user_health( iTargetId, iNewHealth );

    // Display Effects

    UHolyLight_Effects( iTargetId );

    // Give Support XP

    XP_Support_Heal( iCasterId, iHealthGiven );

    // Invisibility Cooldown

    Invis_Cooldown( iTargetId );

    return PLUGIN_HANDLED;
}


// Damage Version

public UHolyLight_Dmg( Caster, Target ) {
#if ADVANCED_DEBUG
	log_function("public UHolyLight_Dmg( Caster, Target ) {");
#endif

    // Play Sound

    emit_sound( Target, CHAN_STATIC, SOUND_HOLYLIGHT, 1.0, ATTN_NORM, 0, PITCH_NORM );

    // Display Effects

    UHolyLight_Effects( Target );

    // Check for Amulet

    if ( IAmulet_Ready( Caster, Target ) )
        return PLUGIN_HANDLED;

    // Hud Message

    new szMessage[128], szPlayerName[32];
    get_user_name( Caster, szPlayerName, 31 );

    format( szMessage, 127, CAST_HOLYLIGHT, szPlayerName, HOLYLIGHT_DAMAGE );

    new iBlindTime = HOLYLIGHT_BLINDTIME;
    new Float:fDuration = float( iBlindTime ) + 3.0;

    WAR3_status_text( Target, szMessage, fDuration );

    // Apply Damage

    WAR3_damage( Caster, Target, CSW_HOLYLIGHT, HOLYLIGHT_DAMAGE, CS_HEADSHOT_NO, DAMAGE_NOCHECKARMOR );

    if ( is_user_alive( Target ) )
    {
        // Set Velocity

        new Velocity[3];
        get_entity_velocity( Target, Velocity );

        Velocity[0] = 0;
        Velocity[1] = 0;

        set_entity_velocity( Target, Velocity );

        // Screen Fade

        Create_ScreenFade( Target, (1<<10), (HOLYLIGHT_BLINDTIME<<12), FADE_OUT, 255, 255, 255, 255 );
    }

    return PLUGIN_HANDLED;
}


// Effect

public UHolyLight_Effects( Target ) {
#if ADVANCED_DEBUG
	log_function("public UHolyLight_Effects( Target ) {");
#endif

    new Origin[3], Radius[3];

    // Main Cone

    get_user_origin( Target, Origin );
    Origin[2] += 16;
    Radius[2] = 50;

    Create_TE_BEAMCYLINDER( SHOWTO_ALL, Origin, Radius, SPR_SHOCKWAVE, 0, 0, 3, 50, 0, 255, 245, 220, 128, 0 );

    // Dark Ring

    get_user_origin( Target, Origin );
    Origin[2] += 4;
    Radius[2] = 160;

    Create_TE_BEAMCYLINDER( SHOWTO_ALL, Origin, Radius, SPR_SMOOTHWAVE, 0, 0, 3, 6, 0, 255, 180, 36, 128, 0 );

   // Light Ring

    get_user_origin( Target, Origin );
    Origin[2] += 16;
    Radius[2] = 200;

    Create_TE_BEAMCYLINDER( SHOWTO_ALL, Origin, Radius, SPR_SMOOTHWAVE, 0, 0, 3, 4, 0, 255, 220, 144, 128, 0 );

    // Light

    Create_TE_ELIGHT( SHOWTO_ALL, Target, 100, 255, 245, 220, 3, 0 );

    return PLUGIN_HANDLED;
}


/* - Flame Strike ----------------------------------------------- */


public UFstrike_Init( Caster, Target ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Init( Caster, Target ) {");
#endif

    // Play Sound

    emit_sound( Target, CHAN_STATIC, SOUND_FLAMESTRIKE, 1.0, ATTN_NORM, 0, PITCH_NORM );

    new parmCast[2];
    parmCast[0] = Caster;
    parmCast[1] = Target;

    UFstrike_Cast( parmCast );

    return PLUGIN_HANDLED;
}


// Cast Flame Strike

public UFstrike_Cast( parmCast[2] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Cast( parmCast[2] ) {");
#endif

    new Caster = parmCast[0];
    new Target = parmCast[1];

    // Make Sure Player is on Ground

    if ( get_entity_flags( Target ) & FL_ONGROUND )
    {
        // Hud Message

        new szMessage[128], szPlayerName[32];
        get_user_name( Caster, szPlayerName, 31 );

        format( szMessage, 127, CAST_FLAMESTRIKE, szPlayerName );

        WAR3_status_text( Target, szMessage, 3.0 );

        // Cast Flamestrike

        new Origin[3];
        get_user_origin( Target, Origin );

        UFstrike_Effects( Origin );
        g_bFlameStrikeCast[Caster] = true;

        new Float:fDuration = FLAMESTRIKE_DURATION;
        new iDuration = floatround( fDuration );

        new iCounter = ( iDuration * 5 );

        new parmCheck[5];
        parmCheck[0] = Caster;
        parmCheck[1] = iCounter;
        parmCheck[2] = Origin[0];
        parmCheck[3] = Origin[1];
        parmCheck[4] = Origin[2];

        UFstrike_Check( parmCheck );
    }

    else
    {
        new TaskId = TASK_FLAMESTRIKE_CASTER + Caster;
        set_task( 0.1, "UFstrike_Cast", TaskId, parmCast, 2 );
    }

    return PLUGIN_HANDLED;
}


// Fire Check

public UFstrike_Check( parmCheck[5] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Check( parmCheck[5] ) {");
#endif

    new Caster   = parmCheck[0];
    new iCounter = parmCheck[1];

    if ( iCounter && g_bFlameStrikeCast[Caster] )
    {
        new Origin[3];
        Origin[0] = parmCheck[2];
        Origin[1] = parmCheck[3];
        Origin[2] = parmCheck[4];

        iCounter--;

        // Check for Players in Range

        new Victims[32], iTotalPlayers, VictimOrigin[3];
        get_players( Victims, iTotalPlayers, "a" );

        // Find player Distance(s)

        for ( new iPlayerNum = 0; iPlayerNum < iTotalPlayers; iPlayerNum++ )
        {
            new Victim = Victims[iPlayerNum];
            get_user_origin( Victim, VictimOrigin );

            new bool:bPlayerImmune;

            if ( g_PlayerOnFire[Victim] || g_bPlayerSleeping[Victim] || g_iPlayerAvatar[Victim] || Victim == g_Vip )
                bPlayerImmune = true;

            if ( !bPlayerImmune && get_distance( Origin, VictimOrigin ) / 40 <= FLAMESTRIKE_RADIUS )
            {
                new parmDot[5];
                parmDot[0] = Caster;
                parmDot[1] = Victim;
                parmDot[2] = VictimOrigin[0];
                parmDot[3] = VictimOrigin[1];
                parmDot[4] = VictimOrigin[2];

                UFstrike_FireDmg( parmDot );
            }
        }

        parmCheck[1] = iCounter;

        new TaskId = TASK_FLAMESTRIKE_CASTER + Caster;
        set_task( 0.2, "UFstrike_Check", TaskId, parmCheck, 6 );
    }

    else
    {
        g_bFlameStrikeCast[Caster] = false;
    }

    return PLUGIN_HANDLED;
}


// Fire Damage

public UFstrike_FireDmg( parmDot[5] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_FireDmg( parmDot[5] ) {");
#endif

    new Caster = parmDot[0];
    new Victim = parmDot[1];

    // Play sound!

    emit_sound( Victim, CHAN_STATIC, SOUND_FLAMESTRIKE_BURN, 0.5, ATTN_NORM, 0, PITCH_NORM );

    // Burn Effect(s)

    new Origin[3];
    Origin[0] = parmDot[2];
    Origin[1] = parmDot[3];
    Origin[2] = parmDot[4];

    Create_TE_SPRITE( SHOWTO_ALL_BROADCAST, Origin, SPR_FLAMEBURST, 5, 200 );

    // Apply Damage

    WAR3_damage( Caster, Victim, CSW_FLAMESTRIKE, FLAMESTRIKE_DAMAGEIN, CS_HEADSHOT_NO, DAMAGE_NOCHECKARMOR );

    if ( is_user_alive( Victim ) )
    {
        // Fire Damage

        Create_Damage( Victim, 0, 0, CS_DMG_BURN );

        // Screen Fade

        Create_ScreenFade( Victim, (1<<10), (1<<10), FADE_OUT, 255, 108, 0, 160 );

        // Set Globals

        g_PlayerSingeing[Victim] = 0;
        g_PlayerOnFire[Victim]   = Caster;

        new TaskId = TASK_FLAMESTRIKE_VICTIM + Victim;
        set_task( 1.0, "UFstrike_OnFire", TaskId, parmDot, 5 );

        Icon_DispellMe( Victim );
    }

    return PLUGIN_HANDLED;
}


// Fire Task

public UFstrike_OnFire( parmDot[5] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_OnFire( parmDot[5] ) {");
#endif

    new Caster = parmDot[0];
    new Victim = parmDot[1];

    new Origin[3];
    Origin[0] = parmDot[2];
    Origin[1] = parmDot[3];
    Origin[2] = parmDot[4];

    new VictimOrigin[3];
    get_user_origin( Victim, VictimOrigin );

    new bool:bPlayerImmune;

    if ( g_bPlayerSleeping[Victim] || g_iPlayerAvatar[Victim] )
        bPlayerImmune = true;

    if ( !bPlayerImmune && get_distance( Origin, VictimOrigin ) / 40 <= FLAMESTRIKE_RADIUS && g_bFlameStrikeCast[Caster] )
        UFstrike_FireDmg( parmDot );

    else
    {
        g_PlayerOnFire[Victim] = 0;
        g_PlayerSingeing[Victim] = Caster;

        new parmSinge[3];
        parmSinge[0] = Caster;
        parmSinge[1] = Victim;
        parmSinge[2] = FLAMESTRIKE_SINGEDURATION;

        UFstrike_Singe( parmSinge );
    }

    return PLUGIN_HANDLED;
}

// Singe Damage

public UFstrike_Singe( parmSinge[3] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Singe( parmSinge[3] ) {");
#endif

    new Caster   = parmSinge[0];
    new Victim   = parmSinge[1];
    new iCounter = parmSinge[2];

    // Apply Damage

    WAR3_damage( Caster, Victim, CSW_FLAMESTRIKE, FLAMESTRIKE_DAMAGEOUT, CS_HEADSHOT_NO, DAMAGE_NOCHECKARMOR );

    if ( !g_PlayerOnFire[Victim] && is_user_alive( Victim ) )
    {
        // Screen Fade

        if ( !g_bPlayerSleeping[Victim] )
        {
            Create_ScreenFade( Victim, (1<<10), (1<<10), FADE_OUT, 255, 108, 0, 20 );
        }

        // Check DOT Counter

        iCounter--;

        if ( iCounter )
        {
            parmSinge[2] = iCounter;

            new TaskId = TASK_FLAMESTRIKE_VICTIM + Victim;
            set_task( 1.0, "UFstrike_Singe", TaskId, parmSinge, 5 );
        }
    }

    else
    {
        UFstrike_Remove( Victim );
    }

    return PLUGIN_HANDLED;
}


// Remove

public UFstrike_Remove( Victim ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Remove( Victim ) {");
#endif

    g_PlayerOnFire[Victim]   = 0;
    g_PlayerSingeing[Victim] = 0;

    new TaskId = TASK_FLAMESTRIKE_VICTIM + Victim;
    remove_task( TaskId, 0 );

    Icon_DispellMe( Victim );

    return PLUGIN_HANDLED;
}


// Flame Pillar Effects

public UFstrike_Effects( Origin[3] ) {
#if ADVANCED_DEBUG
	log_function("public UFstrike_Effects( Origin[3] ) {");
#endif

    Origin[2] -= 5;

    new Float:fFlameBorder = 75.0;
    new Float:fTempOrigin[3];

    //new szSpriteName[32];

    // Border

    for ( new i = 1; i < 5; i++ )
    {
        fTempOrigin[0] = float( Origin[0] );
        fTempOrigin[1] = float( Origin[1] );
        fTempOrigin[2] = float( Origin[2] );

        switch ( i )
        {
            case 1:         fTempOrigin[0] += ( fFlameBorder );
            case 2:         fTempOrigin[0] += ( -1.0 * fFlameBorder );
            case 3:         fTempOrigin[1] += ( fFlameBorder );
            case 4:         fTempOrigin[1] += ( -1.0 * fFlameBorder );
        }

        Create_TempSprite( "FLAME_STRIKE", "sprites/cexplo.spr", fTempOrigin, MOVETYPE_NONE, SOLID_TRIGGER, random_float( 10.0, 20.0 ), random_float( 0.4, 0.8 ), FLAMESTRIKE_DURATION + 1.5 );
    }


    // Inner Diagonal

    fFlameBorder = 2.0 * ( fFlameBorder / 3.0 );

    for ( new i = 1; i < 5; i++ )
    {
        fTempOrigin[0] = float( Origin[0] );
        fTempOrigin[1] = float( Origin[1] );
        fTempOrigin[2] = float( Origin[2] );

        switch ( i )
        {
            case 1:
            {
                fTempOrigin[0] += ( fFlameBorder );
                fTempOrigin[1] += ( -1.0 * fFlameBorder );
            }
            case 2:
            {
                fTempOrigin[0] += ( fFlameBorder );
                fTempOrigin[1] += ( fFlameBorder );
            }
            case 3:
            {
                fTempOrigin[0] += ( -1.0 * fFlameBorder );
                fTempOrigin[1] += ( -1.0 * fFlameBorder );
            }
            case 4:
            {
                fTempOrigin[0] += ( -1.0 * fFlameBorder );
                fTempOrigin[1] += ( fFlameBorder );
            }
        }

        Create_TempSprite( "FLAME_STRIKE", "sprites/fexplo.spr", fTempOrigin, MOVETYPE_NONE, SOLID_TRIGGER, random_float( 10.0, 20.0 ), random_float( 0.4, 0.8 ), FLAMESTRIKE_DURATION + 1.5 );
    }


    // Inner Square

    fFlameBorder = 2.0 * ( fFlameBorder / 3.0 );

    for ( new i = 1; i < 5; i++ )
    {
        fTempOrigin[0] = float( Origin[0] );
        fTempOrigin[1] = float( Origin[1] );
        fTempOrigin[2] = float( Origin[2] );

        switch ( i )
        {
            case 1:         fTempOrigin[0] += ( fFlameBorder );
            case 2:         fTempOrigin[0] += ( -1.0 * fFlameBorder );
            case 3:         fTempOrigin[1] += ( fFlameBorder );
            case 4:         fTempOrigin[1] += ( -1.0 * fFlameBorder );
        }

        Create_TempSprite( "FLAME_STRIKE", "sprites/cexplo.spr", fTempOrigin, MOVETYPE_NONE, SOLID_TRIGGER, random_float( 10.0, 20.0 ), random_float( 0.4, 0.8 ), FLAMESTRIKE_DURATION + 1.5 );
    }

    return PLUGIN_HANDLED;
}


/* - Avatar ----------------------------------------------------- */


public UAvatar_Cast( cIndex ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Cast( cIndex ) {");
#endif

    g_bPlayerCantMove[cIndex] = true;
    g_bPlayerAvatarGrow[cIndex] = true;

    g_iPlayerAvatar[cIndex] = 0;

    // Play sound

    emit_sound( cIndex, CHAN_STATIC, SOUND_AVATAR, 1.0, ATTN_NORM, 0, PITCH_NORM );

    // Immobilize

    set_user_maxspeed( cIndex, SPEED_IMMOBILIZE );

    // Status Text

    new szMessage[128];
    format( szMessage, 127, CAST_AVATAR );

    WAR3_status_text( cIndex, szMessage, 3.0 );

    // Screenshake

    Create_ScreenShake( cIndex, (10<<12), (2<<12), (5<<12) );

    // Add health/effects

    new parmGrow[2];
    parmGrow[0] = cIndex;

    UAvatar_Grow( parmGrow );

    // Stop beeping

    return PLUGIN_HANDLED;
}


public UAvatar_Grow( parmGrow[2] ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Grow( parmGrow[2] ) {");
#endif

    new pIndex = parmGrow[0];
    new iCounter = parmGrow[1];

    new iMaxCounts = AVATAR_HEALTH / AVATAR_STEPSIZE;

    iCounter += 1;

    // Increase health

    new iNewHealth = get_user_health( pIndex ) + AVATAR_STEPSIZE;

    if ( iNewHealth > WAR3_get_maxhealth( pIndex ) )
        iNewHealth = WAR3_get_maxhealth( pIndex );

    set_user_health( pIndex, iNewHealth );

    // Increase shell size

    new iShellSize = ( AVATAR_SIZE / iMaxCounts ) * iCounter;
    set_entity_rendering( pIndex, kRenderFxGlowShell, AVATAR_RGB[GLOW_R], AVATAR_RGB[GLOW_G], AVATAR_RGB[GLOW_B], kRenderNormal, iShellSize );

    // Loop again

    if ( iCounter < iMaxCounts )
    {
        parmGrow[1] = iCounter;

        new iTaskId = TASK_AVATAR + pIndex;
        set_task( 0.1, "UAvatar_Grow", iTaskId, parmGrow, 2 );
    }

    else
    {
        UAvatar_Immunity( pIndex );
    }

    return PLUGIN_HANDLED;
}


public UAvatar_Immunity( pIndex ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Immunity( pIndex ) {");
#endif

    g_bPlayerAvatarGrow[pIndex] = false;
    g_iPlayerAvatar[pIndex] = get_user_health( pIndex );

    Immobilize_Remove( pIndex );
    WAR3_set_speed( pIndex );

	// Bartime

    new iDuration = floatround( AVATAR_DURATION );
    Create_BarTime( pIndex, iDuration );

    // Set duration timer

    new parmShrink[3];
    parmShrink[0] = pIndex;

    new iTaskId = TASK_AVATAR + pIndex;
    set_task( AVATAR_DURATION, "UAvatar_Shrink", iTaskId, parmShrink, 3 );

    return PLUGIN_HANDLED;
}


public UAvatar_Shrink( parmShrink[3] ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Shrink( parmShrink[3] ) {");
#endif

    new pIndex = parmShrink[0];
    new iCounter = parmShrink[1];
    //new iHealth = parmShrink[2];


    new iMaxCounts = AVATAR_HEALTH / AVATAR_STEPSIZE;

    iCounter += 1;

    // Decrease health

    new iNewHealth = get_user_health( pIndex ) - AVATAR_STEPSIZE;

     // Don't kill player

    if ( iNewHealth < 1 )
        iNewHealth = 1;

    set_user_health( pIndex, iNewHealth );

    // Decrease shell size

    new iShellSize = AVATAR_SIZE - ( ( AVATAR_SIZE / iMaxCounts ) * iCounter );
    set_entity_rendering( pIndex, kRenderFxGlowShell, AVATAR_RGB[GLOW_R], AVATAR_RGB[GLOW_G], AVATAR_RGB[GLOW_B], kRenderNormal, iShellSize );

    // Loop again

    if ( iCounter < iMaxCounts )
    {
        parmShrink[1] = iCounter;

        new iTaskId = TASK_AVATAR + pIndex;
        set_task( 0.1, "UAvatar_Shrink", iTaskId, parmShrink, 2 );
    }

    else
    {
        UAvatar_Finished( pIndex );
    }

    return PLUGIN_HANDLED;
}


public UAvatar_Finished( pIndex ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Finished( pIndex ) {");
#endif

    // Status Text

    new szMessage[128];
    format( szMessage, 127, FINISH_AVATAR );

    WAR3_status_text( pIndex, szMessage, 3.0 );

    UAvatar_Remove( pIndex );

    // Remove Shell

    set_user_rendering( pIndex );

    // Invisibility Cooldown

    Invis_Cooldown( pIndex );

    // Reset Speed

    WAR3_set_speed( pIndex );

    // Cooldown

    Ultimate_Cooldown( pIndex, ULTIMATE_COOLDOWNDEFAULT );

    return PLUGIN_HANDLED;
}


public UAvatar_Damage( aIndex, vIndex, wIndex, iHeadshot ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Damage( aIndex, vIndex, wIndex, iHeadshot ) {");
#endif

    // Add to bonus damage array

    WAR3_damage( aIndex, vIndex, wIndex, AVATAR_DAMAGE, iHeadshot, DAMAGE_CHECKARMOR );

    if ( is_user_alive( vIndex ) )
    {
        // Screen Fade

        new iFadeAlpha = AVATAR_DAMAGE * 3;
        Create_ScreenFade( vIndex, (1<<10), (1<<10), FADE_OUT, AVATAR_RGB[GLOW_R], AVATAR_RGB[GLOW_G], AVATAR_RGB[GLOW_B], iFadeAlpha );
    }

    return PLUGIN_HANDLED;
}


public UAvatar_Remove( pIndex ) {
#if ADVANCED_DEBUG
	log_function("public UAvatar_Remove( pIndex ) {");
#endif

    g_bPlayerAvatarGrow[pIndex] = false;
    g_iPlayerAvatar[pIndex] = 0;

    // Remove BarTime

    Remove_BarTime( pIndex );

    new iTaskId = TASK_AVATAR + pIndex;
    remove_task( iTaskId, 0 );

    return PLUGIN_HANDLED;
}

// End of HUMAN.INL