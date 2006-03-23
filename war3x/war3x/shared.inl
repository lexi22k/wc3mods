// Begin SHARED.INL (sloppy file!)

/* - Glow Functions --------------------------------------------- */


public SHARED_GLOW_set( id, Float:fGlowTime, iRGB[3], iAmmount ) {

    if ( g_iPlayerAvatar[id] || g_bPlayerAvatarGrow[id] || g_bPlayerSleeping[id] )
        return PLUGIN_HANDLED;

    new task = TASK_GLOW + id;
    remove_task( task, 0 );

    g_bPlayerGlowing[id] = true;

    // Glow!

    set_user_rendering( id, kRenderFxGlowShell, iRGB[GLOW_R], iRGB[GLOW_G], iRGB[GLOW_B], kRenderNormal, iAmmount );

    new parm_Fade[3];
    parm_Fade[GLOW_ID] = id;
    parm_Fade[GLOW_AMMOUNT] = iAmmount;
    parm_Fade[GLOW_FADEOUT] = 10;

    set_task( fGlowTime, "SHARED_GLOW_fade", task, parm_Fade, 3 );

    return PLUGIN_HANDLED;
}



public SHARED_GLOW_fade( parm_Fade[3] ) {

    new id       = parm_Fade[GLOW_ID];
    new iAmmount = parm_Fade[GLOW_AMMOUNT];
    new iCounter = parm_Fade[GLOW_FADEOUT];

    new Float:fRGB[3];
    entity_get_vector( id, EV_VEC_rendercolor, fRGB );

    new Float:fR = ( fRGB[GLOW_R] / 10.0 ) * float( iCounter );
    new Float:fG = ( fRGB[GLOW_G] / 10.0 ) * float( iCounter );
    new Float:fB = ( fRGB[GLOW_B] / 10.0 ) * float( iCounter );

    // Glow!

    set_user_rendering( id, kRenderFxGlowShell, floatround( fR ), floatround( fG ), floatround( fB ), kRenderNormal, iAmmount );

    new task = TASK_GLOW + id;

    iCounter--;

    if ( iCounter )
    {
        parm_Fade[GLOW_FADEOUT] = iCounter;
        set_task( 0.1, "SHARED_GLOW_fade", task, parm_Fade, 3 );
    }

    else
    {
        SHARED_GLOW_remove( id );
    }

    return PLUGIN_HANDLED;
}


public SHARED_GLOW_remove( id ) {

    g_bPlayerGlowing[id] = false;

    new task = TASK_GLOW + id;
    remove_task( task, 0 );

    // Reset Rendering

    if ( is_user_connected( id ) )
        set_user_rendering( id );

    // Invis Cooldown

    SHARED_INVIS_cooldown( id );

    return PLUGIN_HANDLED;
}


/* - Invisibility Functions ------------------------------------- */


// Invisibility set ( shared )

public SHARED_INVIS_set( id ) {

    new parmInvis[1];
    parmInvis[0] = id;

    SHARED_INVIS_set_( parmInvis );

    return PLUGIN_HANDLED;
}


public SHARED_INVIS_set_( parm_Invis[1] ) {

    new id = parm_Invis[0];

    if ( !is_user_connected( id ) || !is_user_alive( id ) )
        return PLUGIN_HANDLED;

    if ( g_bPlayerGlowing[id] || g_bPlayerSleeping[id] || g_iPlayerAvatar[id] || g_bPlayerAvatarGrow[id] )
        return PLUGIN_HANDLED;

    if ( ( g_PlayerInfo[id][CURRENT_RACE] != RACE_HUMAN || ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && !g_PlayerInfo[id][CURRENT_SKILL1] ) ) && g_PlayerInfo[id][CURRENT_ITEM] != ITEM_CLOAK )
        return PLUGIN_HANDLED;

    g_bInvisCooldown[id] = false;

    new weapon, iClip, iAmmo;
    weapon = get_user_weapon( id, iClip, iAmmo );

    // Human Invisibility

    if ( weapon == CSW_KNIFE && g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && g_PlayerInfo[id][CURRENT_SKILL1] && WAR3_skill_enabled( id, RACE_HUMAN, SKILL_1 ) )
    {
        // Make sure cloak value is not greater

        if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_CLOAK && VALUE_CLOAK > s_Invisibility[g_PlayerInfo[id][CURRENT_SKILL1] - 1] )
            ITEM_CLOAK_set( id );

        else
        {
            HU_S_INVISIBILITY_set( id );
        }
    }

    // Cloak of Invisibility

    else if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_CLOAK )
    {
        ITEM_CLOAK_set( id );
    }

    // Remove

    else if ( g_bPlayerInvis[id] )
    {
        SHARED_INVIS_remove( id );
    }

    return PLUGIN_HANDLED;
}


public SHARED_INVIS_cooldown( id ) {

    // Do not proceed if no invisibility exists

    if ( ( g_PlayerInfo[id][CURRENT_RACE] != RACE_HUMAN || ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && !g_PlayerInfo[id][CURRENT_SKILL1] ) ) && g_PlayerInfo[id][CURRENT_ITEM] != ITEM_CLOAK )
        return PLUGIN_HANDLED;

    SHARED_INVIS_remove( id );

    g_bInvisCooldown[id] = true;

    new task = TASK_INVIS + id;

    new parm_Invis[1];
    parm_Invis[0] = id;

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && g_PlayerInfo[id][CURRENT_SKILL1] )
        set_task( INVISIBILITY_COOLDOWN, "SHARED_INVIS_set_", task, parm_Invis, 1 );

    else if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_CLOAK )
    {
        set_task( CLOAK_COOLDOWN, "SHARED_INVIS_set_", task, parm_Invis, 1 );
    }

    return PLUGIN_HANDLED;
}


public SHARED_INVIS_remove( id ) {

    if ( !g_bPlayerGlowing[id] && !g_bPlayerSleeping[id] && !g_iPlayerAvatar[id] )
        set_user_rendering( id );

    new task = TASK_INVIS + id;
    remove_task( task, 0 );

    g_bPlayerInvis[id] = false;
    g_bInvisCooldown[id] = false;

    SHARED_INVIS_icon( id, ICON_HIDE );

    return PLUGIN_HANDLED;
}


public SHARED_INVIS_icon( id, iStatus ) {

    // Make sure server enabled

    if ( !get_cvar_num( "war3x_statusicons" ) && iStatus == ICON_SHOW )
        return PLUGIN_HANDLED;

    // Make sure player is alive before showing

    if ( !is_user_alive( id ) && iStatus == ICON_SHOW )
        return PLUGIN_HANDLED;

    new Float:fInvisLevel = entity_get_float( id, EV_FL_renderamt );
    new iInvisLevel = floatround( fInvisLevel );

    Create_StatusIcon( id, iStatus, "suithelmet_empty", iInvisLevel, iInvisLevel, iInvisLevel );

    return PLUGIN_HANDLED;
}


public SHARED_IMMOBILIZE_remove( id ) {

    if ( !g_bPlayerBashed[id] && !g_PlayerRooted[id] && !g_bPlayerSleeping[id] && !g_bPlayerAvatarGrow[id] )
        g_bPlayerCantMove[id] = false;

    return PLUGIN_HANDLED;
}


public SHARED_SLOW_remove( id ) {

    if ( !g_bPlayerFrosted[id] && !g_PlayerStruck[id] && !g_bPlayerNova[id] )
        g_bPlayerSlowed[id] = false;

    return PLUGIN_HANDLED;
}


// Dispell Negative Effects (UNUSED ATM)

public Dispell_Negative( dispellerId, targetId )
{
    new szMessage[128], szPlayerName[32];
    get_user_name( dispellerId, szPlayerName, 31 );

    new bool:bHasEffects;
    g_bPlayerDispelled[targetId] = false;


    // Bash

    if ( g_bPlayerBashed[targetId] )
    {
        new parm_Stun[1];
        parm_Stun[0] = targetId;

        SBash_Remove( parm_Stun );
        bHasEffects = true;
    }

    // Frost Armor ( slow )

    if ( g_bPlayerFrosted[targetId] )
    {
        new parm_Slow[1];
        parm_Slow[0] = targetId;

        UD_S_FROSTARMOR_remove( parm_Slow );
        bHasEffects = true;
    }

    // Frost Nova ( slow )

    if ( g_bPlayerNova[targetId] )
    {
        new parm_Slow[1];
        parm_Slow[0] = targetId;

        UD_S_FROSTNOVA_remove( parm_Slow );
        bHasEffects = true;
    }

    // Shadow Strike

    if ( g_PlayerStruck[targetId] )
    {
        format( szMessage, 127, DISPELL_SHADOWSTRIKE, szPlayerName );

        WAR3_status_text( g_PlayerStruck[targetId], szMessage, 3.0 );

        NE_U_SHADOWSTRIKE_remove( targetId );
        bHasEffects = true;
    }

    // Entangling Roots

    if ( g_PlayerRooted[targetId] )
    {
        format( szMessage, 127, DISPELL_ROOTS, szPlayerName );

        WAR3_status_text( g_PlayerRooted[targetId], szMessage, 3.0 );

        NE_U_ROOT_remove( targetId );
        bHasEffects = true;
    }

    // Flame Strike

    if ( g_PlayerOnFire[targetId] || g_PlayerSingeing[targetId] )
    {
        HU_U_FLAMESTRIKE_remove( targetId );
        bHasEffects = true;
    }

    // Display Dispell Message

    if ( bHasEffects )
    {
        copy( szMessage, 127, DISPELL_GENERIC );
        WAR3_status_text( targetId, szMessage, 3.0 );
    }

    return PLUGIN_HANDLED;
}


// Dispell Positive Effects (UNUSED ATM)

public Dispell_Positive( dispellerId, targetId )
{
    if ( g_PlayerRejuv[targetId] )             // Rejuvenation
        NE_U_REJUVENATION_remove( targetId );

    if ( get_user_health( targetId ) > 100 && g_PlayerInfo[targetId][CURRENT_RACE] == RACE_HUMAN )
    {
        g_iDispellHealth[targetId] = get_user_health( targetId ) - 100;
        set_user_health( targetId, 100 );
    }

    return PLUGIN_HANDLED;
}


// Remove Dispell (UNUSED ATM)

public Dispell_Remove( id ) {

    if ( !g_bPlayerRespawned[id] )
    {
        g_bPlayerDispelled[id] = false;

        // Restore Lost Bonus HPs

        if ( g_iDispellHealth[id] )
        {
            new iNewHealth = g_iDispellHealth[id] + get_user_health( id );

            if ( iNewHealth > WAR3_get_maxhealth( id ) )
                iNewHealth = WAR3_get_maxhealth( id );

            set_user_health( id, iNewHealth );
        }
    }

    return PLUGIN_HANDLED;
}


/* - Miscellaneous Skill Functions ------------------------------ */


// Get max health

public WAR3_get_maxhealth( id ) {

    new iMaxHealth = 100;

    if ( g_bEvadeNextShot[id] )
        iMaxHealth = 1124;

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN )
    {
        new iLevel = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] );
        iMaxHealth = HU_S_FORTITUDE_get( iLevel );
    }

    if ( g_bPlayerAvatarGrow[id] || g_iPlayerAvatar[id] )
        iMaxHealth += AVATAR_HEALTH;

    return ( iMaxHealth );
}


// Get min health

public WAR3_get_minhealth( id ) {

    new iMinHealth = 0;

    if ( g_bEvadeNextShot[id] )
        iMinHealth = 1024;

    return ( iMinHealth );
}


// Get max armor

public WAR3_get_maxarmor( id ) {

    new iMaxArmor = 100;

    // VIP

    if ( id == g_Vip )
        iMaxArmor = 200;

    // Nature's Blessing

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_NIGHTELF && g_PlayerInfo[id][CURRENT_SKILL2] )
    {
        iMaxArmor += s_BlessingArmor[g_PlayerInfo[id][CURRENT_SKILL2] - 1] - 100;
    }

    // Frost Armor

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_UNDEAD && g_PlayerInfo[id][CURRENT_SKILL3] )
        iMaxArmor += UD_iFrostArmor_armor[g_PlayerInfo[id][CURRENT_SKILL3] - 1] - 100;

    // Inner Fire

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && g_PlayerInfo[id][CURRENT_SKILL3] )
        iMaxArmor += s_ifArmor[g_PlayerInfo[id][CURRENT_SKILL3] - 1] - 100;

    // Ring of Protection +5

    if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_RING )
        iMaxArmor += VALUE_RING;

    return ( iMaxArmor );
}



// Check max armor

public WAR3_check_armor( id ) {

    if ( get_user_armor( id ) > WAR3_get_maxarmor( id ) )
        set_user_armor( id, WAR3_get_maxarmor( id ) );

    return PLUGIN_HANDLED;
}


// Check Max Health

public WAR3_check_health( id ) {

    if ( get_user_health( id ) > WAR3_get_maxhealth( id ) )
        set_user_health( id, WAR3_get_maxhealth( id ) );

    return PLUGIN_HANDLED;
}


// Enable Armor Skill

public WAR3_armorskill_on( id ) {

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_UNDEAD && g_PlayerInfo[id][CURRENT_SKILL3] )
    {
        new iMultiplier = ARMOR_MAXFADE / TOTAL_SKILLLEVELS;
        new iFadeAmmount = iMultiplier * g_PlayerInfo[id][CURRENT_SKILL3];

        // Screen Fade

        if ( !g_bPlayerSleeping[id] )
            Create_ScreenFade( id, (2<<12), (1<<10), FADE_OUT, 91, 168, 248, iFadeAmmount );

        // Hud Message

        new szMessage[128];
        copy( szMessage, 127, ARMOR_MESSAGE_UD );

        WAR3_status_text( id, szMessage, 1.0 );

        // Play Client Sound

        client_cmd( id, "speak warcraft3/bonus/FrostArmorTarget1.wav" );
    }

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && g_PlayerInfo[id][CURRENT_SKILL3] )
    {
        new iMultiplier = ARMOR_MAXFADE / TOTAL_SKILLLEVELS;
        new iFadeAmmount = iMultiplier * g_PlayerInfo[id][CURRENT_SKILL3];

        // Screen Fade

        if ( !g_bPlayerSleeping[id] )
            Create_ScreenFade( id, (2<<12), (1<<10), FADE_OUT, 255, 160, 0, iFadeAmmount );

        // Hud Message

        new szMessage[128];
        copy( szMessage, 127, ARMOR_MESSAGE_HU );

        WAR3_status_text( id, szMessage, 1.0 );

        // Play Client Sound

        client_cmd( id, "speak warcraft3/bonus/InnerFireBirth.wav" );
    }

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_ORC )
    {
        new iMultiplier = ARMOR_MAXFADE / TOTAL_SKILLLEVELS;
        new iFadeAmmount = iMultiplier * g_PlayerInfo[id][CURRENT_SKILL3];

        // Screen Fade

        if ( !g_bPlayerSleeping[id] )
            Create_ScreenFade( id, (2<<12), (1<<10), FADE_OUT, 255, 0, 0, iFadeAmmount );

        // Hud Message

        new szMessage[128];
        copy( szMessage, 127, ARMOR_MESSAGE_OR );

        WAR3_status_text( id, szMessage, 1.0 );

        // Play Client Sound

        client_cmd( id, "speak warcraft3/bonus/OrcBurrowBattleStations.wav" );
    }

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_NIGHTELF && g_PlayerInfo[id][CURRENT_SKILL2] )
    {
        new iLevel = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] ) + 1;

        if ( iLevel > 10 )
            iLevel = 10;

        new Float:fRacialCap = LEVEL_RACIALCAP + 1.0;

        new iMultiplier = ARMOR_MAXFADE / floatround( fRacialCap );
        new iFadeAmmount = iMultiplier * iLevel;

        // Screen Fade

        if ( !g_bPlayerSleeping[id] )
            Create_ScreenFade( id, (2<<12), (1<<10), FADE_OUT, 0, 255, 224, iFadeAmmount );

        // Hud Message

        new szMessage[128];
        copy( szMessage, 127, ARMOR_MESSAGE_NE );

        WAR3_status_text( id, szMessage, 1.0 );

        // Play Client Sound

        client_cmd( id, "speak warcraft3/bonus/BarkSkinTarget1.wav" );
    }

    return PLUGIN_HANDLED;
}


// Disable Armor Skill

public WAR3_armorskill_off( id ) {

    new szMessage[128];

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_UNDEAD && g_PlayerInfo[id][CURRENT_SKILL3] )
        copy( szMessage, 127, ARMOR_MESSAGE_FADE_UD );

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN && g_PlayerInfo[id][CURRENT_SKILL3] )
        copy( szMessage, 127, ARMOR_MESSAGE_FADE_HU );

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_ORC )
        copy( szMessage, 127, ARMOR_MESSAGE_FADE_OR );

    else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_NIGHTELF && g_PlayerInfo[id][CURRENT_SKILL2] )
        copy( szMessage, 127, ARMOR_MESSAGE_FADE_NE );

    else
    {
        return PLUGIN_HANDLED;
    }

    WAR3_status_text( id, szMessage, 1.0 );
    client_cmd( id, "speak warcraft3/bonus/ShadowMeld1.wav" );

    return PLUGIN_HANDLED;
}


// Give bonus health

public WAR3_set_health( id ) {

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_HUMAN )
        HU_S_FORTITUDE_set( id );

    return PLUGIN_HANDLED;
}




// Set player speed

public WAR3_set_speed( id ) {

    new parmSpeed[1];
    parmSpeed[0] = id;

    _WAR3_set_speed( parmSpeed );

    return PLUGIN_HANDLED;
}


public _WAR3_set_speed( parm_Speed[1] ) {

    new id = parm_Speed[0];

    if ( g_bFreezeTime || !is_user_connected( id ) )
        return PLUGIN_HANDLED;

    if ( g_bPlayerCantMove[id] )                        // Immobilized
    {
        set_user_maxspeed( id, SPEED_IMMOBILIZE );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, SPEED_IMMOBILIZE );
    }

    else if ( g_iPlayerAvatar[id] )                     // Avatar ( precedes slowing effects )
    {
        set_user_maxspeed( id, AVATAR_SPEED );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, AVATAR_SPEED );
    }

    else if ( g_bPlayerWalk[id] )                       // Wind Walk ( precedes slowing effects )
    {
        set_user_maxspeed( id, WINDWALK_SPEED );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, WINDWALK_SPEED );
    }

    else if ( g_bPlayerNova[id] )                       // Frost Nova
    {
        set_user_maxspeed( id, FROSTARMOR_SLOWDURATION );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, FROSTARMOR_SLOWDURATION );
    }

    else if ( g_bPlayerFrosted[id] )                    // Frost Slow
    {
        set_user_maxspeed( id, FROSTARMOR_SLOWSPEED );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, FROSTARMOR_SLOWSPEED );
    }

    else if ( g_PlayerStruck[id] )                      // Shadow Strike
    {
        set_user_maxspeed( id, SHADOWSTRIKE_SPEED );

        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, SHADOWSTRIKE_SPEED );
    }

    else
    {
        if ( is_user_bot( id ) )
            set_entity_maxspeed( id, BOTSPEED_NORMAL );

        // VIP ( no speed bonuses )

        if ( id == g_Vip )
        {
            set_user_maxspeed( id, CS_SPEED_VIP );

            if ( is_user_bot( id ) )
                set_entity_maxspeed( id, CS_SPEED_VIP );

            return PLUGIN_CONTINUE;
        }

        new weapon, iClip, iAmmo;
        weapon = get_user_weapon( id, iClip, iAmmo );

        // Unholy Aura

        if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_UNDEAD )
            UD_S_UNHOLY_set_speed( id, weapon );

        // Bloodlust

        /*else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_ORC && g_PlayerInfo[id][CURRENT_SKILL1] && ( weapon == CSW_KNIFE || cs_get_weapon_type_( weapon ) == CS_WEAPON_TYPE_GRENADE ) )
        {
            new Float:fLustSpeed = s_BloodlustSpeed[g_PlayerInfo[id][CURRENT_SKILL1] - 1];

            if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_BOOTS )
            {
                new Float:fNewSpeed = VALUE_BOOTS - SPEED_KNIFE + fLustSpeed;
                set_user_maxspeed( id, fNewSpeed );
            }

            else
            {
                set_user_maxspeed( id, fLustSpeed );
            }
        }*/

		// Berserk

		else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_ORC && g_PlayerInfo[id][CURRENT_SKILL1] && ( weapon == CSW_KNIFE || cs_get_weapon_type_( weapon ) == CS_WEAPON_TYPE_GRENADE ) )
		{
			OR_S_BERSERK_speed( id );
		}

        // Boots of speed

        else if ( g_PlayerInfo[id][CURRENT_ITEM] == ITEM_BOOTS )
            set_user_maxspeed( id, VALUE_BOOTS );

        // Nature's blessing

        else if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_NIGHTELF && g_PlayerInfo[id][CURRENT_SKILL2] && get_user_armor( id ) )
            NE_S_BLESSING_set_speed( id, weapon );

        else
        {
            if ( g_Vip == id )
                set_user_maxspeed( id, CS_SPEED_VIP );

            else if ( g_bPlayerZoomed[id] )
                set_user_maxspeed( id, CS_WEAPON_SPEED_ZOOM[weapon] );

            else
            {
                set_user_maxspeed( id, CS_WEAPON_SPEED[weapon] );
            }
        }
    }

    return PLUGIN_HANDLED;
}


// Activate all skills (round start/spend skill)

public WAR3_enable_skills( id ) {

    new parm_Skills[1];
    parm_Skills[0] = id;

    // Activate speed altering abilities

    set_task( 0.1, "_WAR3_set_speed", 0, parm_Skills, 1 );

    // Check for Evade

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_NIGHTELF && g_PlayerInfo[id][CURRENT_SKILL1] )
    {
        NE_S_EVASION_prep( id );
        NE_S_EVASION_health( id );
    }

    // Regenerate Health / Armor

    if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_ORC && !g_bPlayerRegen[id] )
        OR_S_REGENERATION_set( id );

    // Set Invisibility

    SHARED_INVIS_set( id );

    // Enable Ultimate

    Ultimate_Enable( id );

    // Hudmessages

    WAR3_hud_xp( id, HUDMESSAGE_FX_FADEIN, 10.0, 0.0, 2.0, 3.0 );
    WAR3_hud_item( id, HUDMESSAGE_FX_FADEIN, 10.0, 0.0, 2.0, 3.0 );

    return PLUGIN_HANDLED;
}


/* - Skill Functions -------------------------------------------- */


// Actions to perform on victim

public WAR3_death_victim( id ) {

    // XP Actions

    XP_Death( id );

    // Drop Item

    if ( g_PlayerInfo[id][CURRENT_ITEM] && random_float( 0.0, 1.0 ) >= ITEM_DROPRATE )
        Item_DropDead( id, g_PlayerInfo[id][CURRENT_ITEM] );

	// Frost Nova

	if ( g_PlayerInfo[id][CURRENT_RACE] == RACE_UNDEAD && g_PlayerInfo[id][CURRENT_SKILL2] )
			UD_S_FROSTNOVA( id );

    // Check for Ankh / Reincarnation

    if ( !g_bPlayerRespawned[id] )
    {
        if ( !g_PlayerBackpack[id][CURRENT_PRIMARY] && !g_PlayerBackpack[id][CURRENT_SECONDARY] && !g_PlayerBackpack[id][CURRENT_HEGRENADE] && !g_PlayerBackpack[id][CURRENT_FLASHBANG] && !g_PlayerBackpack[id][CURRENT_SMOKEGRENADE] && !g_PlayerBackpack[id][CURRENT_ARMOR] && !g_PlayerBackpack[id][CURRENT_DEFUSE] )
            ITEM_ANKH_set( id );
    }

    else
    {
        g_bPlayerRespawned[id] = false;
    }

    if ( g_PlayerInfo[id][CURRENT_ITEM] != ITEM_ANKH )
        g_PlayerInfo[id][CURRENT_ITEM] = 0;


    // Check if New Skills Available

    WAR3_check_skills( id );

    // Purge tasks from Round

    Purge_Death( id );

    // Deplete Armor

    g_bArmorDepleted[id] = true;

    // Remove Player from Objective Arrays

    if ( g_BombCarrier == id )
        g_BombCarrier = 0;

    if ( g_BombDefuser == id )
        g_BombDefuser = 0;

    // Remove Invisibility

    if ( WAR3_is_invisible( id ) )
        set_user_rendering( id );

    // Clear XP Hudmesssage

    set_hudmessage( 0, 0, 0, -1.0, -1.0, 0, 0.0, 0.1, 0.0, 0.0, HUDMESSAGE_CHAN_XP );
    show_hudmessage( id, "" );

    return PLUGIN_HANDLED;
}


public Purge_Death( id ) {

    // Common Purges

    Purge_Common( id );

    // Ammo

    for ( new i = 0; i < 31; i++ )
    {
        g_iPlayerAmmo[id][i] = 0;
    }

    g_iPlayerArmor[id] = {0,0};

    // Clear amulet charges (if applicable)

    g_iAmuletCharges[id] = 0;

    // Clear StatusIcons

    Ultimate_Icon( id, ICON_HIDE );

    return PLUGIN_HANDLED;
}


public Purge_FreezeStart() {

    // Remove Duplicate Start Timers

    remove_task( TASK_NEWROUND, 0 );

    // Remove Ultimate Warmup Timer

    remove_task( TASK_ULTIMATEWARMUP, 0 );

    // Remove Hudmessage timer

    remove_task( TASK_SHOWITEM, 0 );

    // Remove Temporary Models

    Remove_TempEnts( "WAR3X_ITEM", 0 );
    Remove_TempEnts( "DEATH_COIL", 0 );
    Remove_TempEnts( "ROOT_PLANT", 0 );
    Remove_TempEnts( "IMPALE_CLAW", 0 );
    Remove_TempEnts( "SLEEP_Z", 0 );
    Remove_TempEnts( "REJUV_FLARE", 0 );
    Remove_TempEnts( "FLAME_STRIKE", 0 );
    Remove_TempEnts( "HEAL_EFFECT", 0 );

    // Reset Objective Globals

    g_BombPlanter = 0;
    g_BombDefuser = 0;
    g_bBombPlanted = false;
    g_bHostageRescued = false;

    return PLUGIN_HANDLED;
}


public Purge_NewRound( id ) {

    // Common Purges

    Purge_Common( id );

    // Remove Glow

    SHARED_GLOW_remove( id );

    // Globals

    g_bPlayerRespawned[id]  = false;
    g_bFlameStrikeCast[id]  = false;

    return PLUGIN_HANDLED;
}


public Purge_Common( id ) {

    // Ultimate / Skill Effects

    if ( g_bPlayerRegen[id] )
        OR_S_REGENERATION_remove( id );

    if ( g_PlayerStruck[id] )
        NE_U_SHADOWSTRIKE_remove( id );

    if ( g_PlayerRooted[id] )
        NE_U_ROOT_remove( id );

    if ( g_PlayerOnFire[id] || g_PlayerSingeing[id] )
        HU_U_FLAMESTRIKE_remove( id );

    if ( g_PlayerRejuv[id] )
        NE_U_REJUVENATION_remove( id );

    if ( g_PlayerImpaled[id] )
        UD_U_IMPALE_remove( id );

    if ( g_bPlayerBashed[id] )
    {
        new parm_Bash[1];
        parm_Bash[0] = id;

        SBash_Remove( parm_Bash );
    }

    if ( g_bPlayerSleeping[id] )
        UD_U_SLEEP_remove( id );

    if ( g_PlayerInfo[id][CURRENT_ULTIMATE] == ULTIMATE_WINDWALK )
        OR_U_WINDWALK_remove( id );

    if ( g_bPlayerAvatarGrow[id] || g_iPlayerAvatar[id] )
        HU_U_AVATAR_remove( id );

    if ( g_bPlayerFrosted[id] )
    {
        new parm_Slow[1];
        parm_Slow[0] = id;

        UD_S_FROSTARMOR_remove( parm_Slow );
    }

    if ( g_bPlayerNova[id] )
    {
        new parm_Slow[1];
        parm_Slow[0] = id;

        UD_S_FROSTNOVA_remove( parm_Slow );
    }

    if ( g_bPlayerInvis[id] )
    {
        SHARED_INVIS_remove( id );
    }

    // Halt Icons

    g_bPlayerSaveMe[id]     = false;
    g_bPlayerDispellMe[id]  = false;

    Remove_TE_PLAYERATTACHMENT( SEENBY_ALL, id );

    Ultimate_Disable( id );

    remove_task( TASK_INVIS + id, 0 );
    remove_task( TASK_EVASION + id, 0 );


    // Globals

    g_iChargeUltimate[id]   = 0;
    g_iDispellHealth[id]    = 0;

    g_iEvasionDamage[id]    = 0;
    g_fEvasionTime[id]      = 0.0;

    g_PlayerTarget[id]      = 0;
    g_PlayerTargetTeam[id]  = 0;
    g_TargetFormat[id][0]   = 0;
    g_bNewTargetType[id]    = false;

    g_bPlayerZoomed[id]     = false;
    g_bGotDefuser[id]       = false;

    g_bPlayerSlowed[id]     = false;
    g_bPlayerCantMove[id]   = false;
    g_bPlayerDispelled[id]  = false;
    g_bInvisCooldown[id]    = false;
    g_bEvadeNextShot[id]    = false;
    g_bTouchHostage[id]     = false;
    g_bChangeUltimate[id]   = false;
    g_bAnkhDropWeapons[id]  = false;

    g_fUltimateCooldown[id] = get_gametime();

    Remove_BarTime( id );

    return PLUGIN_HANDLED;
}


public Purge_PlayerInfo( id ) {

    if ( g_Vip == id )
        g_Vip = 0;

    if ( g_BombCarrier == id )
        g_BombCarrier = 0;

    if ( g_BombDefuser == id )
        g_BombDefuser = 0;

    g_iHostageKills[id]     = 0;
    g_SaveIds[id][0]        = 0;

    g_fCommandFlood[id]     = 0.0;

    playerSkill1Info[id]    = {0,0};
    playerSkill2Info[id]    = {0,0};
    playerSkill3Info[id]    = {0,0};
    playerRoundXp[id]       = {0,0};
    g_PlayerBackpack[id]    = {0,0,0,0,0,0,0,0};
    g_PlayerInfo[id]        = {0,0,0,0,0,0,0,0,0,0,0,0};

    g_bFlameStrikeCast[id]  = false;
    g_bInBuyZone[id]        = false;

    // Unset XP variables

    for ( new iRaceId; iRaceId < TOTAL_RACES; iRaceId++ )
    {
		g_iXPtotal[id][iRaceId] = 0;


		if ( get_cvar_num("war3x_save_xp_sql") == 1 )
		{
         	g_iXPfetched[id][iRaceId] = 0;
		}
    }

    return PLUGIN_HANDLED;
}



// Enable / Disable Warcraft 3

public WAR3_toggle( id ) {

    if ( !( get_user_flags( id ) & ADMIN_RCON ) )
    {
        client_print( id, print_console, "[%s] You have no access to that command.", AMX_NAME );
        return PLUGIN_HANDLED;
    }

    new szArgs[4];
    read_args( szArgs, 3 );

    if ( equali( szArgs, "on" ) || equali( szArgs, "1" ) )
        WAR3_enable( id );

    else if ( equali( szArgs, "off" ) || equali( szArgs, "0" ) )
        WAR3_disable( id );

    else
    {
        client_print( id, print_console, WAR3X_FORMAT );
    }

    return PLUGIN_HANDLED;
}


public WAR3_enable( id ) {

    if ( g_bWar3xEnabled )
    {
        client_print( id, print_console, WAR3_enableD_ALREADY );
        return PLUGIN_HANDLED;
    }


    client_print( id, print_console, WAR3_enableD );
    set_cvar_num( "war3x_enabled", 1 );


    // Show Hudmessage

    set_hudmessage( 60, 160, 220, HUDMESSAGE_POS_CENTER, 0.30, 2, 0.1, 4.0, 0.02, 0.02, HUDMESSAGE_CHAN_SERVER );
    show_hudmessage( 0, WAR3_enableD_HUD );

    server_cmd( "sv_restart 5" );

    return PLUGIN_HANDLED;
}


public WAR3_disable( id ) {

    if ( !g_bWar3xEnabled )
    {
        client_print( id, print_console, WAR3_disableD_ALREADY );
        return PLUGIN_HANDLED;
    }

    client_print( id, print_console, WAR3_disableD );
    set_cvar_num( "war3x_enabled", 0 );


    // Kill Hudmessage(s)

    set_hudmessage( 0, 0, 0, -1.0, -1.0, 0, 0.0, 0.1, 0.0, 0.0, 1 );
    show_hudmessage( id, "" );

    set_hudmessage( 0, 0, 0, -1.0, -1.0, 0, 0.0, 0.1, 0.0, 0.0, 2 );
    show_hudmessage( id, "" );

    set_hudmessage( 0, 0, 0, -1.0, -1.0, 0, 0.0, 0.1, 0.0, 0.0, 3 );
    show_hudmessage( id, "" );


    // Show Hudmessage

    set_hudmessage( 220, 0, 36, HUDMESSAGE_POS_CENTER, 0.30, 2, 0.1, 4.0, 0.02, 0.02, HUDMESSAGE_CHAN_SERVER );
    show_hudmessage( 0, WAR3_disableD_HUD );

    server_cmd( "sv_restart 5" );


    new Players[32], iTotalPlayers;
    get_players( Players, iTotalPlayers );

    for ( new iPlayerNum = 0; iPlayerNum < iTotalPlayers; iPlayerNum++ )
    {
        new playerId = Players[iPlayerNum];

        // Save XP

//        if ( g_SaveIds[playerId] && !is_user_bot(playerId) && g_bSaveXp && g_PlayerInfo[playerId][CURRENT_RACE] ) {

//            if ( g_bSaveXp == SAVE_MYSQL )
//                mysql_war3write(playerId, mysqlTablename[g_PlayerInfo[playerId][CURRENT_RACE] - 1])

//        }

        Purge_Death( playerId );
        Purge_NewRound( playerId );
        Purge_PlayerInfo( playerId );
    }

    return PLUGIN_HANDLED;
}


// War3 Vote

public WAR3_vote_start( id ) {

    if ( !get_cvar_num( "war3x_vote_allow" ) && !( get_user_flags( id ) & ADMIN_VOTE ) )
    {
        client_print( id, print_chat, WAR3X_VOTE_NOVOTE );
        return PLUGIN_HANDLED;
    }

    if ( get_user_flags( id ) & ADMIN_VOTE || get_cvar_num( "war3x_vote_allow" ) )
    {
        if ( g_fVoteRunning > get_gametime() )
        {
            client_print( id, print_chat, WAR3X_VOTE_ALREADY );
            return PLUGIN_HANDLED;
        }

        if ( g_fVoteRunning > 0.0 && ( g_fVoteRunning + get_cvar_float( "amx_vote_delay" ) > get_gametime() ) )
        {
            client_print( id, print_chat, WAR3X_VOTE_NOTALLOWED );
            return PLUGIN_HANDLED;
        }


        new szPlayerName[32];
        new szMenu[256];
        new iKeys = (1<<0)|(1<<1);

        if ( g_bWar3xEnabled )
            format( szMenu, 255, MENU_VOTE_DISABLE );

        else
        {
            format( szMenu, 255, MENU_VOTE_ENABLE );
        }

        new Float:fVoteTime = get_cvar_float( "amx_vote_time" ) + 2.0;

        g_fVoteRunning = get_gametime() + fVoteTime;
        g_fVoteRatio = get_cvar_float( "war3x_vote_ratio" );

        show_menu( 0, iKeys, szMenu, floatround( fVoteTime ) );
        set_task( fVoteTime, "WAR3_vote_tally" );

        get_user_name( id, szPlayerName, 31 );
        client_print( 0, print_chat, WAR3X_WAR3_vote_startED, szPlayerName );

        g_iVoteCounts = {0,0};
    }

    return PLUGIN_HANDLED;
}


public WAR3_vote_count( id, iKey ) {

    if ( get_cvar_float( "amx_vote_answers" ) )
    {
        new szPlayerName[32];
        get_user_name( id, szPlayerName, 31 );

        client_print( 0, print_chat, "%s voted %s", szPlayerName, iKey ? WAR3X_VOTE_AGAINST : WAR3X_VOTE_FOR );
    }

    g_iVoteCounts[iKey]++;

    return PLUGIN_HANDLED;
}


public WAR3_vote_tally() {

    new szWar3Status[32];
    new Players[32], iTotalPlayers;
    new iNumVotes = g_iVoteCounts[0] + g_iVoteCounts[1];

    get_players( Players, iTotalPlayers, "c" );

    new Float:fVoteResult = float( iTotalPlayers ) ? ( float( iNumVotes ) / float( iTotalPlayers ) ) : 0.0;

    if ( fVoteResult < g_fVoteRatio )
    {
        client_print( 0, print_chat, WAR3X_VOTE_FAILED, g_iVoteCounts[0], g_iVoteCounts[1], g_fVoteRatio );
        return PLUGIN_HANDLED;
    }

    if ( !g_bWar3xEnabled )
    {
        if ( g_iVoteCounts[0] > g_iVoteCounts[1] )
        {
            WAR3_enable( 0 );
            copy( szWar3Status, 32, WAR3X_VOTE_ENABLED );
        }

        else
        {
            copy( szWar3Status, 32, WAR3X_VOTE_REMAINDISABLED );
        }
    }

    else
    {
        if ( g_iVoteCounts[0] > g_iVoteCounts[1] )
        {
            WAR3_disable( 0 );
            copy( szWar3Status, 32, WAR3X_VOTE_DISABLED );
        }

        else
        {
            copy( szWar3Status, 32, WAR3X_VOTE_REMAINENABLED );
        }
    }

    if ( g_iVoteCounts[0] != g_iVoteCounts[1] )
    {
        client_print( 0, print_chat, WAR3X_VOTE_SUCCESS, g_iVoteCounts[0], g_iVoteCounts[1], g_fVoteRatio );
        client_print( 0, print_chat, "* Warcraft 3: Expansion %s", szWar3Status );
    }

    else if ( ( iNumVotes > 0 ) && ( g_iVoteCounts[0] == g_iVoteCounts[1] ) )
   {
        client_print( 0, print_chat, WAR3X_VOTE_TIE, g_iVoteCounts[0], g_iVoteCounts[1] );
        client_print( 0, print_chat, "* Warcraft 3: Expansion %s", szWar3Status );
   }

    return PLUGIN_HANDLED;
}


public WAR3_check_level( id, iOldXp, iNewXp ) {

    if ( WAR3_get_level( iOldXp ) < WAR3_get_level( iNewXp ) )
    {
        new iLevel = WAR3_get_level( iNewXp );

        new szMessage[64];
        format( szMessage, 63, INFO_GAINLEVEL, iLevel );

        set_hudmessage( 255, 255, 255, HUDMESSAGE_POS_CENTER, HUDMESSAGE_POS_INFO, 0, 6.0, 5.0, 0.1, 0.5, HUDMESSAGE_CHAN_INFO );
        show_hudmessage( id, szMessage );

        // Activate Racial Skills

        if ( !g_bFreezeTime )
            WAR3_enable_skills( id );

        // Play Sound

        emit_sound( id, CHAN_STATIC, SOUND_LEVELUP, 1.0, ATTN_NORM, 0, PITCH_NORM );

        // Show skill selection menu

        WAR3_check_skills( id );
    }

    return PLUGIN_HANDLED;
}


public WAR3_check_skills( id ) {

    new Ultimate = 0;

    if ( g_PlayerInfo[id][CURRENT_ULTIMATE] )
        Ultimate = 1;

    new iTotalpoints = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] );
    new iUsedpoints = g_PlayerInfo[id][CURRENT_SKILL1] + g_PlayerInfo[id][CURRENT_SKILL2] + g_PlayerInfo[id][CURRENT_SKILL3] + Ultimate;

    if ( iUsedpoints < iTotalpoints )
    {
        menu_SelectSkills( id );
        return PLUGIN_HANDLED;
    }

    return PLUGIN_HANDLED;
}


public War3x_StoreSession( id, iRaceId ) {

    g_PlayerInfo[id][CURRENT_RACE] = iRaceId + 1;

    if ( g_bSaveXp && !is_user_bot( id ) )
    {
        g_PlayerInfo[id][CURRENT_XP] = g_iXPtotal[id][iRaceId];
        Retrieve_Skills( id, iRaceId );
    }

    else
    {
        // Just reset skills so xp carries over

        g_PlayerInfo[id][CURRENT_SKILL1]    = 0;
        g_PlayerInfo[id][CURRENT_SKILL2]    = 0;
        g_PlayerInfo[id][CURRENT_SKILL3]    = 0;
        g_PlayerInfo[id][CURRENT_ULTIMATE]  = 0;
    }

    return PLUGIN_HANDLED;
}


// Check if skill is available to get

public War3x_GetSkillCheck( iLevel, iSkillLevel ) {

    if ( iSkillLevel > TOTAL_SKILLLEVELS )
        return ( 0 );


    if ( iLevel >= GETSKILLCHECK[iSkillLevel - 1] )
        return ( 1 );

    else
    {
        return ( 0 );
    }


    return PLUGIN_HANDLED;
}


// Map Start event

public WAR3_map_start() {

    if ( !get_cvar_num( "war3x_enabled" ) )
        return PLUGIN_HANDLED;

    g_bWar3xEnabled = true;

    // Set correct Version/Date cvars

    set_cvar_string( "Warcraft_3_Expansion", WAR3X_VERSION );
    set_cvar_string( "Warcraft_3_Expansion_Date", WAR3X_DATE );

    // Save XP?

    if ( get_cvar_num( "war3x_save_xp" ) == 1 && !g_bMapRestricted )
    {
        g_bSaveXp = true;

		if ( get_cvar_num("war3x_save_xp_sql") == 1 )
		{
            mysql_init();
		}
    }

    // Set Level XP

    Set_LevelXp();

    // Check race restrictions ( make sure all races not restricted )

    new szCvar[8];
    get_cvar_string( "war3x_restrict_races", szCvar, 7 );

    if ( equal( szCvar, "abcd" ) )
        set_cvar_string( "war3x_restrict_races", "" );

    // Check if help files exist

    new bool:bCreateFiles;

    for ( new iRaceNum = 0; iRaceNum < TOTAL_RACES; iRaceNum++ )
    {
        new szFilename[64];
        format( szFilename, 63, "%s/help/%s.html", WAR3X_DIR, RACEKEYNAME[iRaceNum] );

        if ( !file_exists( szFilename ) )
            bCreateFiles = true;
    }

    if ( bCreateFiles )
        HELP_create_files();

    // Load map-specific settings

    new szMapName[64];
    get_mapname( szMapName, 63 );

    new szFilename[128];
    format( szFilename, 127, "%s/map_specific/%s.cfg", WAR3X_DIR, szMapName );

    if ( file_exists( szFilename ) )
    {
        new szCmd[64];
        format( szCmd, 63, "exec %s", szFilename );

        server_cmd( szCmd );
    }

    return PLUGIN_HANDLED;
}


// Check if Map is Restricted to Short-Term XP

public Map_Restricted() {

    new szRestrictFile[64];
    format( szRestrictFile, 63, "%s/short_term_maps.cfg", WAR3X_DIR );

    if ( file_exists( szRestrictFile ) )
    {
        new iLineNum, szData[64], iTextLen;

        while ( read_file( szRestrictFile, iLineNum, szData, 63, iTextLen ) )
        {
            new szMapName[64], szRestrictName[64];

            get_mapname( szMapName, 63 );
            new iLen = copyc( szRestrictName, 63, szData, '*' );

            if ( equali( szMapName, szRestrictName, iLen ) )
                return ( 1 );

            iLineNum++;
        }
    }

    return ( 0 );
}


public Map_Disabled() {

    new szDisabledFile[64];
    format( szDisabledFile, 63, "%s/disabled_maps.cfg", WAR3X_DIR );

    if ( file_exists( szDisabledFile ) )
    {
        new iLineNum, szData[64], iTextLen;

        while ( read_file( szDisabledFile, iLineNum, szData, 63, iTextLen ) )
        {
            new szMapName[64], szDisabledName[64];

            get_mapname( szMapName, 63 );
            new iLen = copyc( szDisabledName, 63, szData, '*' );

            if ( equali( szMapName, szDisabledName, iLen ) )
                return ( 1 );

            iLineNum++;
        }
    }

    return ( 0 );
}


// Save Me!

public WAR3_icon_lowhealth( id ) {

    if ( !is_user_alive( id ) )
    {
        if ( Ultimate_Type( id ) & ULTIMATE_TYPE_HEAL )
            Icon_SaveMe_Undraw( id );

        return PLUGIN_HANDLED;
    }

    if ( WAR3_is_lowhealth( id ) )
    {
        if ( !g_bPlayerSaveMe[id] )
        {
            WAR3_icon_lowhealth_draw( id );
            g_bPlayerSaveMe[id] = true;
        }
    }

    else
    {
        if ( g_bPlayerSaveMe[id] )
        {
            Remove_TE_PLAYERATTACHMENT( SEENBY_ALL, id );
            g_bPlayerSaveMe[id] = false;
        }
    }

    return PLUGIN_HANDLED;
}


static WAR3_icon_lowhealth_draw( victim ) {

    new TeamPlayers[32], iTotalPlayers;
    new szTeamName[16];

    if ( get_user_team( victim ) == CS_TEAM_TERRORIST )
        copy( szTeamName, 15, "TERRORIST" );

    else
    {
        copy( szTeamName, 15, "CT" );
    }

    get_players( TeamPlayers, iTotalPlayers, "ace", szTeamName );

    // Display Health Icon Overhead For Teammates with Healing Ultimates

    for ( new iPlayerNum = 0; iPlayerNum < iTotalPlayers; iPlayerNum++ )
    {
        new iHealerId = TeamPlayers[iPlayerNum];

        if ( victim != iHealerId && Ultimate_Type( iHealerId ) & ULTIMATE_TYPE_HEAL )
        {
            new bool:bTargetable = true;

            if ( !g_PlayerOptions[iHealerId][OPTION_HEALICONS] )
                bTargetable = false;

            if ( !WAR3_can_heal( iHealerId, victim ) )
                bTargetable = false;

            if ( bTargetable )
                Create_TE_PLAYERATTACHMENT( iHealerId, victim, 40, ICON_SAVEME, 9999 );
        }
    }

    return PLUGIN_HANDLED;
}

public Icon_SaveMe_Undraw( id ) {

    new TeamPlayers[32], iTotalPlayers;
    new szTeamName[16];

    if ( get_user_team( id ) == CS_TEAM_TERRORIST )
        copy( szTeamName, 15, "TERRORIST" );

    else
    {
        copy( szTeamName, 15, "CT" );
    }

    get_players( TeamPlayers, iTotalPlayers, "a", szTeamName );

    // Remove health icon from low health teammates

    for ( new iPlayerNum = 0; iPlayerNum < iTotalPlayers; iPlayerNum++ )
    {
        new teammate = TeamPlayers[iPlayerNum];

        if ( g_bPlayerSaveMe[teammate] )
            Remove_TE_PLAYERATTACHMENT( id, teammate );
    }

    return PLUGIN_HANDLED;
}


// Dispell Me!

public Icon_DispellMe( id ) {

    if ( g_PlayerOptions[id][OPTION_HEALICONS] )
        return PLUGIN_HANDLED;

    if ( WAR3_is_dispellable( id ) )
    {
        if ( !g_bPlayerDispellMe[id] )
        {
            Icon_DispellMe_Draw( id );
            g_bPlayerDispellMe[id] = true;
        }
    }

    else
    {
        if ( g_bPlayerDispellMe[id] )
        {
            Remove_TE_PLAYERATTACHMENT( SEENBY_ALL, id );
            g_bPlayerDispellMe[id] = false;
        }
    }

    return PLUGIN_HANDLED;
}


public Icon_DispellMe_Draw( id ) {

    if ( g_PlayerOptions[id][OPTION_HEALICONS] )
        return PLUGIN_HANDLED;


    new TeamPlayers[32], iTotalPlayers;
    new szTeamName[16];

    if ( get_user_team( id ) == CS_TEAM_TERRORIST )
        copy( szTeamName, 15, "TERRORIST" );

    else
    {
        copy( szTeamName, 15, "CT" );
    }

    get_players( TeamPlayers, iTotalPlayers, "e", szTeamName );

    for ( new iPlayerNum = 0; iPlayerNum < iTotalPlayers; iPlayerNum++ )
    {
        new teamId = TeamPlayers[iPlayerNum];

        if ( id != teamId && is_user_alive( teamId ) && Ultimate_Type( teamId ) & ULTIMATE_TYPE_DISPELL && Ultimate_Target( teamId ) & ULTIMATE_TARGET_TEAM )
            Create_TE_PLAYERATTACHMENT( teamId, id, 40, ICON_DISPELLME, 9999 );
    }

    return PLUGIN_HANDLED;
}



public Get_SkillName( iRaceId, iSkillNum, id, szSkillName[32] ) {

	LANG_GetSkillNameHelper( iRaceId + 1, iSkillNum, id, szSkillName, 31 )

    return PLUGIN_HANDLED;
}


// Generic Healing Effect

public Shared_Heal_Effect( iTargetId ) {             // Shared Effect(s)

    // Flutters!

    for ( new i = 0; i < 4; i++ )
    {
        new Float:fTargetOrigin[3];
        entity_get_vector( iTargetId, EV_VEC_origin, fTargetOrigin );

        // Randomize

        fTargetOrigin[0] = random_float( fTargetOrigin[0] - 20.0, fTargetOrigin[0] + 20.0 );
        fTargetOrigin[1] = random_float( fTargetOrigin[1] - 20.0, fTargetOrigin[1] + 20.0 );
        fTargetOrigin[2] = random_float( fTargetOrigin[2] - 20.0, fTargetOrigin[2] + 20.0 );

        // Sprite

        new szSpriteName[64];
        copy( szSpriteName, 63, "sprites/muz6.spr" );

        new iHealEnt = Create_TempEnt( "HEAL_EFFECT", szSpriteName, fTargetOrigin, MOVETYPE_NOCLIP, SOLID_NOT, 0.4 );

        // Project Upwards

        new Float:fVelocity[3];
        fVelocity[2] = random_float( 115.0, 200.0 );

        entity_set_vector( iHealEnt, EV_VEC_velocity, fVelocity );

        // Create Trail

        Create_TE_BEAMFOLLOW( SHOWTO_ALL_BROADCAST, iHealEnt, SPR_BEAMFOLLOW, 5, random_num( 1,3 ), 255, random_num( 200,255 ), random_num( 0,100 ), 128 );

        // Create Lighting ( on player )

//        Create_TE_ELIGHT( SHOWTO_ALL_BROADCAST, targetId, random_num( 25,100 ), random_num( 127,255 ), 0, random_num( 127,255 ), 5, 5 );

        // Render

        entity_set_float( iHealEnt, EV_FL_renderamt, random_float( 96.0, 128.0 ) );
        entity_set_int( iHealEnt, EV_INT_rendermode, kRenderTransAdd );
        entity_set_int( iHealEnt, EV_INT_renderfx, kRenderFxNone );

        entity_set_float( iHealEnt, EV_FL_scale, random_float( 0.2, 0.6 ) );
    }

    return PLUGIN_HANDLED;
}


/* - 4. Show Level ( /level ) ----------------------------------- */


// Level / Rank hud message

public WAR3_hud_level( id ) {

    if ( !g_bWar3xEnabled )
        return PLUGIN_HANDLED;

    // Make sure race selected

    if ( !g_PlayerInfo[id][CURRENT_RACE] )
    {
        menu_SelectRace( id );
        return PLUGIN_HANDLED;
    }

    new iRaceNum = g_PlayerInfo[id][CURRENT_RACE] - 1;

    new szMessage[73], szData[64], iLen, szRaceName[32];
    new iLevel = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] );
	
	LANG_GetRaceName( iRaceNum, id, szRaceName, 31 );
    WAR3_race_info( id, iRaceNum, RACEINFO_CLASSNAME, szData );
    iLen += format( szMessage[iLen], 72 - iLen, "%s^nLevel %d ( %s )", szRaceName, iLevel, szData );

    if ( g_PlayerInfo[id][CURRENT_ULTIMATE] )
    {
        new iUltimateNum = g_PlayerInfo[id][CURRENT_ULTIMATE] + TOTAL_SKILLSTRAINED;

        WAR3_race_info( id, iRaceNum, iUltimateNum, szData );
        iLen += format( szMessage[iLen], 72 - iLen, "^nUltimate: %s", szData );
    }

    set_hudmessage( 255, 255, 255, HUDMESSAGE_POS_CENTER, HUDMESSAGE_POS_LEVEL, 0, 6.0, 5.0, 0.5, 1.0, HUDMESSAGE_CHAN_LEVEL );
    show_hudmessage( id, szMessage );


    return PLUGIN_HANDLED;
}


// Skill Info Messages

public war3_chatskills( id, raceId, ShowHelp ) {

    // VIP Skills and/or Ultimates Disabled

    if ( id == g_Vip )
    {
        new szVipMsg[128];
        format( szVipMsg, 127, VIP_NOSKILLS );

        client_print( id, print_chat, szVipMsg );

        return PLUGIN_HANDLED;
    }

    new szMessage[128], szSkills[64], szData[64], iLen;

    new iLevel = WAR3_get_level( g_PlayerInfo[id][CURRENT_XP] );

    if ( iLevel > floatround( LEVEL_RACIALCAP ) )
        iLevel = floatround( LEVEL_RACIALCAP );

    new Float:fRacialPercentage = ( ( float( iLevel ) + 1.0 ) / ( LEVEL_RACIALCAP + 1.0 ) ) * 100.0;

    // Build skills string

    if ( g_PlayerInfo[id][CURRENT_SKILL1] )
    {
        WAR3_race_info( id, raceId, RACEINFO_SKILL1, szData );
        iLen += format( szSkills[iLen], 128 - iLen, ", %s %d", szData, g_PlayerInfo[id][CURRENT_SKILL1] );
    }

    if ( g_PlayerInfo[id][CURRENT_SKILL2] )
    {
        WAR3_race_info( id, raceId, RACEINFO_SKILL2, szData );
        iLen += format( szSkills[iLen], 128 - iLen, ", %s %d", szData, g_PlayerInfo[id][CURRENT_SKILL2] );
    }

    if ( g_PlayerInfo[id][CURRENT_SKILL3] )
    {
        WAR3_race_info( id, raceId, RACEINFO_SKILL3, szData );
        iLen += format( szSkills[iLen], 128 - iLen, ", %s %d", szData, g_PlayerInfo[id][CURRENT_SKILL3] );
    }

    // Build message (with racial)

    WAR3_race_info( id, raceId, RACEINFO_RACIAL, szData );
    format( szMessage, 127, INFO_CURRENTSKILLS, szData, fRacialPercentage, "%%", szSkills );

    client_print( id, print_chat, szMessage );

    // Show /skills Notification?

    if ( ShowHelp )
        client_print( id, print_chat, INFO_SKILLDETAILS );

    return PLUGIN_HANDLED;
}

stock is_war3_entity( iEnt, szClassName[] = "" )
{
	// Entities that war3x creates
	new szWar3Entities[][64] = {"WAR3X_ITEM", "DEATH_COIL", "ROOT_PLANT", "IMPALE_CLAW", "SLEEP_Z", "REJUV_FLARE", "FLAME_STRIKE", "HEAL_EFFECT" };

	// Don't want to look up info for an invalid ent
	if ( !is_valid_ent(iEnt) )
	{
		return false;
	}

	if ( strlen(szClassName) == 0 )
	{
		entity_get_string( iEnt, EV_SZ_classname, szClassName, 63 );
	}

	// See if this class is in our list
	for ( new i = 0; i < sizeof(szWar3Entities); i++ )
	{
		if ( equal(szClassName, szWar3Entities[i]) )
		{
			return true;
		}
	}

	return false;
}

// End of SHARED.INL
