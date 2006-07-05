stock get_user_maxhealth(id){

	new maxHealth = 100

	// Human Devotion Skill

	if ( SM_VerifySkill( id, SKILL_DEVOTION ) )
	{
		maxHealth += p_data[id][P_SKILL2] * p_devotion;
	}


	// Player has a health bonus from the Periapt of Health

	if ( ITEM_Has( id, ITEM_HEALTH ) )
		maxHealth += get_pcvar_num( CVAR_wc3_health );

	return maxHealth
}

// Function checks to see if the weapon is a pistol
stock SHARED_IsSecondaryWeapon( iWeaponID )
{
	// Check for Counter-Strike or Condition Zero
	if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
	{
		if ( iWeaponID == CSW_ELITE || iWeaponID == CSW_FIVESEVEN || iWeaponID == CSW_USP || iWeaponID == CSW_GLOCK18 || iWeaponID == CSW_DEAGLE || iWeaponID == CSW_P90 )
		{
			return true;
		}
	}
	
	// Check for Day of Defeat
	else if ( g_MOD == GAME_DOD )
	{

	}


	return false;
}