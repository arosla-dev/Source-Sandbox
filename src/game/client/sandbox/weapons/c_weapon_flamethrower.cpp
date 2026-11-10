//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ====
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_weapon__stubs.h"
#include "c_basecombatweapon.h"
#include "fx.h"
#include "particles_localspace.h"
#include "view.h"
#include "particles_attractor.h"
#include "in_buttons.h"
#include "c_physicsprop.h"
#include "dt_utlvector_recv.h"
#include "props_shared.h"



class C_WeaponFlamethrower: public C_BaseCombatWeapon
{
	DECLARE_CLASS( C_WeaponFlamethrower, C_BaseCombatWeapon );
public:
	C_WeaponFlamethrower( void );

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	virtual void OnDataChanged( DataUpdateType_t updateType );

	void StartAttackParticleEffect( bool bFlameThrower );
	void StopAttackParticleEffect( bool bFlameThrower  );

	void ManageAmmoParticleEffects( void );
	void TurnOffAllEffects( void );

	C_BaseViewModel *GetEffectViewModel( void );
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );

	bool Simulate( void );

private:
	bool m_bFlameEffects;
	bool m_bFiringSecondary;
	bool m_bFiringPrimary;
		
	CUtlReference<CNewParticleEffect> m_pHoseEffect;
	CUtlReference<CNewParticleEffect> m_pFireEffect;
	CUtlReference<CNewParticleEffect> m_pTubePrimaryAmmo;
	CUtlReference<CNewParticleEffect> m_pTubeSecondaryAmmo;
	CUtlReference<CNewParticleEffect> m_pTubeSecondaryModifier;
};

STUB_WEAPON_CLASS_IMPLEMENT( weapon_flamethrower, C_WeaponFlamethrower );

IMPLEMENT_CLIENTCLASS_DT( C_WeaponFlamethrower, DT_WeaponFlamethrower, CWeaponFlamethrower )
	RecvPropBool( RECVINFO( m_bFiringSecondary ) ),
	RecvPropBool( RECVINFO( m_bFiringPrimary ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C_WeaponFlamethrower::C_WeaponFlamethrower( void )
{
	m_bFlameEffects = false;
	m_pHoseEffect = NULL;
	m_pFireEffect = NULL;
	m_pTubePrimaryAmmo = NULL;
	m_pTubeSecondaryAmmo = NULL;
	m_pTubeSecondaryModifier = NULL;
}

C_BaseViewModel *C_WeaponFlamethrower::GetEffectViewModel( void )
{
	// Start the effect on the viewmodel if our owner is the local player
	C_BasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( C_BasePlayer::IsLocalPlayer( pOwner ) )
	{
		ACTIVE_SPLITSCREEN_PLAYER_GUARD_ENT( pOwner );
		return pOwner->GetViewModel();
	}

	return NULL;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_WeaponFlamethrower::Simulate( void )
{
	if ( IsEffectActive( EF_NODRAW ) )
	{
		StopAttackParticleEffect( true );
		StopAttackParticleEffect( false );
		TurnOffAllEffects();

		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_WeaponFlamethrower::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	SetNextClientThink(CLIENT_THINK_ALWAYS);

	if (m_bFiringPrimary == true && !IsEffectActive(EF_NODRAW))
	{
		StartAttackParticleEffect(true);
	}
	else
	{
		StopAttackParticleEffect(true);
	}

	if (m_bFiringSecondary == true && !IsEffectActive(EF_NODRAW))
	{
		StartAttackParticleEffect(false);
	}
	else
	{
		StopAttackParticleEffect(false);
	}

	if (!IsEffectActive(EF_NODRAW))
	{
		ManageAmmoParticleEffects();
	}
	else
	{
		TurnOffAllEffects();
	}
}

bool C_WeaponFlamethrower::Holster(CBaseCombatWeapon* pSwitchingTo)
{
	m_bFiringPrimary = false;
	m_bFiringSecondary = false;

	StopAttackParticleEffect(true);
	StopAttackParticleEffect(false);
	TurnOffAllEffects();

	return BaseClass::Holster(pSwitchingTo);
}

void C_WeaponFlamethrower::TurnOffAllEffects(void)
{
	C_BaseViewModel* pViewModel = GetEffectViewModel();

	if (m_pTubePrimaryAmmo)
	{
		if (pViewModel)
			pViewModel->ParticleProp()->StopEmission(m_pTubePrimaryAmmo);
		m_pTubePrimaryAmmo = NULL;
	}

	if (m_pTubeSecondaryAmmo)
	{
		if (pViewModel)
			pViewModel->ParticleProp()->StopEmission(m_pTubeSecondaryAmmo);
		m_pTubeSecondaryAmmo = NULL;
	}

	if (m_pTubeSecondaryModifier)
	{
		if (pViewModel)
			pViewModel->ParticleProp()->StopEmission(m_pTubeSecondaryModifier);
		m_pTubeSecondaryModifier = NULL;
	}
}



void C_WeaponFlamethrower::ManageAmmoParticleEffects( void )
{
	// FIXME: particle effects are broken, disabling for now
	return;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_WeaponFlamethrower::StartAttackParticleEffect( bool bFlameThrower )
{
	C_BasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( !pOwner )
		return;

	if ( bFlameThrower == true )
	{
		if ( !m_pFireEffect.IsValid() )
		{
			// Start the appropriate particle effect
			const char *pszParticleEffect = "fuel_hose";

			if ( GetEffectViewModel() )
			{
				m_pFireEffect = GetEffectViewModel()->ParticleProp()->Create( pszParticleEffect, PATTACH_POINT_FOLLOW, "ltube_muzzle" );
			}
		}
	}
	else
	{
		// Start the appropriate particle effect
		const char *pszParticleEffect = "fuel_hose";

		if ( GetEffectViewModel() )
		{
			if ( !m_pHoseEffect.IsValid() )
			{
				m_pHoseEffect = GetEffectViewModel()->ParticleProp()->Create( pszParticleEffect, PATTACH_POINT_FOLLOW, "rtube_muzzle" );
			}
		}
	}

	m_bFlameEffects = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_WeaponFlamethrower::StopAttackParticleEffect(bool bFlameThrower)
{
	C_BaseViewModel* pViewModel = GetEffectViewModel();

	if (bFlameThrower == true)
	{
		if (m_pFireEffect.IsValid())
		{
			if (pViewModel)
			{
				pViewModel->ParticleProp()->StopEmission(m_pFireEffect);
			}
			m_pFireEffect = NULL;
		}
	}
	else
	{
		if (m_pHoseEffect.IsValid())
		{
			if (pViewModel)
			{
				pViewModel->ParticleProp()->StopEmission(m_pHoseEffect);
			}
			m_pHoseEffect = NULL;
		}
	}

	m_bFlameEffects = false;
}