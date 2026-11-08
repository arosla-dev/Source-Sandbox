//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The various ammo types for HL2	
//
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"
#include "ammodef.h"
#include "eventlist.h"
#include "npcevent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//---------------------------------------------------------
// Applies ammo quantity scale.
//---------------------------------------------------------
int ITEM_GiveAmmo( CBasePlayer *pPlayer, float flCount, const char *pszAmmoName, bool bSuppressSound = false )
{
	int iAmmoType = GetAmmoDef()->Index(pszAmmoName);
	if (iAmmoType == -1)
	{
		Msg("ERROR: Attempting to give unknown ammo type (%s)\n",pszAmmoName);
		return 0;
	}

	flCount *= g_pGameRules->GetAmmoQuantityScale(iAmmoType);

	// Don't give out less than 1 of anything.
	flCount = MAX( 1.0f, flCount );

	return pPlayer->GiveAmmo( flCount, iAmmoType, bSuppressSound );
}

// ========================================================================
//	>> FlareRound
// ========================================================================
class CItem_FlareRound : public CItem
{
public:
	DECLARE_CLASS(CItem_FlareRound, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/items/flare.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/items/flare.mdl");
	}
	bool MyTouch(CBasePlayer* pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, 1, "FlareRound"))
		{
			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO)
			{
				UTIL_Remove(this);
			}
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_flare_round, CItem_FlareRound);

// ========================================================================
//	>> BoxFlareRounds
// ========================================================================
#define SIZE_BOX_FLARE_ROUNDS 5

class CItem_BoxFlareRounds : public CItem
{
public:
	DECLARE_CLASS(CItem_BoxFlareRounds, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/items/boxflares.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/items/boxflares.mdl");
	}
	bool MyTouch(CBasePlayer* pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_BOX_FLARE_ROUNDS, "FlareRound"))
		{
			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO)
			{
				UTIL_Remove(this);
			}
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(item_box_flare_rounds, CItem_BoxFlareRounds);