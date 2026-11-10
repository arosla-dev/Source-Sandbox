//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "soundenvelope.h"
#include "sdk_player.h"
#include "ai_default.h"
#include "ai_node.h"
#include "ai_navigator.h"
#include "ai_pathfinder.h"
#include "ai_moveprobe.h"
#include "ai_memory.h"
#include "ai_squad.h"
#include "ai_route.h"
#include "ai_senses.h"
#include "explode.h"
#include "ndebugoverlay.h"
#include "decals.h"
#include "gib.h"
#include "game.h"			
#include "ai_interactions.h"
#include "ieffects.h"
#include "vstdlib/random.h"
#include "engine/ienginesound.h"
#include "movevars_shared.h"
#include "npcevent.h"
#include "props.h"
#include "te_effect_dispatch.h"
#include "ai_squadslot.h"
#include "world.h"
#include "func_break.h"
#include "physics_impact_damage.h"
#include "soundent.h"
#include "ammodef.h"
#include "ai_basenpc_physicsflyer.h"
#include "sprite.h"
#include "spritetrail.h"
#include "player_pickup.h"
#include "particle_parse.h"
#include "particle_system.h"
#include "rope.h"
#include "smoke_trail.h"

#include "physics.h"
#include "physics_saverestore.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define HOVER_TURRET_PORTAL_EYE_ATTACHMENT		1

#define HOVER_MIN_HEIGHT						52

#define HOVER_TURRET_LASER_OFF					0
#define HOVER_TURRET_LASER_ON					1
#define HOVER_TURRET_LASER_FIRING				2

#define HOVER_TURRET_SHOT_KNOCKBACK				100.0f
#define HOVER_TURRET_SHOT_CYCLE					0.05f

#define HOVER_TURRET_LASER_DAMAGE				150.0f
#define HOVER_TURRET_LASER_FORCE_MULTIPLIER		0.05f

#define	HOVER_TURRET_TARGET_LOSS_DISTANCE		60
#define	HOVER_TURRET_RANGE						8192

#define HOVER_TURRET_LOCKON_TIME				1.5f

// Assets 

#define HOVER_TURRET_LASER_EFFECT	"effects/bluelaser1.vmt"
#define HOVER_TURRET_GLOW_SPRITE	"sprites/light_glow03.vmt"

#define HOVER_TURRET_DECAL_NAME "decals/scorchfade"
#define HOVER_TURRET_MODEL_NAME "models/props_bts/rocket_sentry.mdl"

#define HOVER_TURRET_LASER_BURN_SOUND "HL2Player.BurnPain"

#define	HOVER_TURRET_SOUND_LOCKING "NPC_RocketTurret.LockingBeep"
#define	HOVER_TURRET_SOUND_LOCKED "NPC_FloorTurret.LockedBeep"

ConVar	sk_hover_turret_health( "sk_hover_turret_health","150");

#define HOVER_TURRET_MODEL	"models/npcs/hover_turret.mdl"

//Turret states
enum hoverTurretState_e
{
	HOVER_TURRET_TARGETING,
	HOVER_TURRET_SEARCHING,
	HOVER_TURRET_TIPPED,
	HOVER_TURRET_DISABLED,
	HOVER_TURRET_COLLIDE,
	HOVER_TURRET_PICKUP,
	HOVER_TURRET_SHOTAT,
	HOVER_TURRET_DESTRUCTING,

	HOVER_TURRET_STATE_TOTAL, // end of talking states
};

//Turret states
enum hoverTurretAttackState_e
{
	HOVER_TURRET_FIND_TARGET,
	HOVER_TURRET_AIM_TARGET,
	HOVER_TURRET_SHOT_DISABLED,
	HOVER_TURRET_HIT_WITH_PHYSICS,
};

int ACT_HOVER_TURRET_SEARCH;
int ACT_HOVER_TURRET_ALERT;
int ACT_HOVER_TURRET_ANGRY;
int	ACT_HOVER_TURRET_DISABLED;

#define DEFAULT_SKIN					0
#define CURIOUS_SKIN					1
#define AGGRESSIVE_SKIN					2
#define	CRAZY_SKIN						3

const char *g_HoverTurretTalkNames[ HOVER_TURRET_STATE_TOTAL ] = 
{
	"NPC_HoverTurret.TalkActive",
	"NPC_HoverTurret.TalkSearch",
	"NPC_FloorTurret.TalkTipped",
	"NPC_FloorTurret.TalkDisabled",
	"NPC_FloorTurret.TalkCollide",
	"NPC_HoverTurret.TalkPickup",
	"NPC_FloorTurret.TalkShotAt",
	"NPC_HoverTurret.TalkDissolved"
};


const char* GetHoverTurretTalkName( int iState )
{
	return g_HoverTurretTalkNames[ iState ];
}

//-----------------------------------------------------------------------------
// Hover Turret Tether
//-----------------------------------------------------------------------------

class CHoverTurretTether : public CBaseAnimating
{
public:
	DECLARE_CLASS( CHoverTurretTether, CBaseAnimating );
	CHoverTurretTether( void );
	void UpdateOnRemove( void );

	void AttachEntities( EHANDLE hTurret, EHANDLE hAttachPoint );

	void ReleaseTether( void );

private:
	DECLARE_DATADESC();

			void	PullThink( void );
	virtual void	Spawn( void );

	float						m_flNaturalDistance;
	CHandle<CBaseEntity>		m_hAttachPoint;
	CHandle<CBaseEntity>		m_hTurret;

	IPhysicsSpring				*m_pSpring;

	CHandle<CRopeKeyframe>		m_hRope;
};

LINK_ENTITY_TO_CLASS( ent_hover_turret_tether, CHoverTurretTether );

BEGIN_DATADESC( CHoverTurretTether )

	DEFINE_PHYSPTR( m_pSpring ),
	DEFINE_FIELD( m_hRope, FIELD_EHANDLE ),
	
	DEFINE_FIELD( m_hAttachPoint, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hTurret, FIELD_EHANDLE ),

	DEFINE_FIELD( m_flNaturalDistance, FIELD_FLOAT ),
	
	DEFINE_THINKFUNC( PullThink )
END_DATADESC()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoverTurretTether::CHoverTurretTether( void )
{
	m_hTurret = NULL;
	m_hAttachPoint = NULL;
	m_pSpring = NULL;
	m_hRope = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoverTurretTether::ReleaseTether( void )
{
	if ( m_pSpring )
	{
		physenv->DestroySpring( m_pSpring );
		m_pSpring = NULL;
	}

	if( m_hRope != NULL )
	{
		m_hRope->DetachPoint(1);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoverTurretTether::UpdateOnRemove( void )
{
	if ( m_pSpring )
	{
		physenv->DestroySpring( m_pSpring );
		m_pSpring = NULL;
	}
	BaseClass::UpdateOnRemove();

	if( m_hRope != NULL )
	{
		m_hRope->DetachPoint(1);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoverTurretTether::AttachEntities( EHANDLE hTurret, EHANDLE hAttachPoint )
{
	m_hTurret = hTurret;
	m_hAttachPoint = hAttachPoint;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoverTurretTether::Spawn( void )
{
	BaseClass::Precache();
	SetModel( HOVER_TURRET_MODEL ); // need a model for vphysics
	AddEffects( EF_NODRAW );

	SetSolid( SOLID_VPHYSICS );
	AddSolidFlags( FSOLID_NOT_SOLID );

	VPhysicsInitShadow( false, false );
	SetMoveType( MOVETYPE_NONE );

	BaseClass::Spawn();

	// if we are attached then start thinking.
	if( m_hAttachPoint != NULL )
	{
		SetNextThink( gpGlobals->curtime ); // think as soon as we can
		SetThink( &CHoverTurretTether::PullThink );

		// call a think right off the bat to set up our spring.
		PullThink();
	}
}

static ConVar hover_turret_break_dist( "hover_turret_break_dist","10" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoverTurretTether::PullThink( void )
{
	CAI_BaseNPC *pTurret = (CAI_BaseNPC*) m_hTurret.Get();

	if ( pTurret == NULL )
	{
		return;
	}

	if ( m_pSpring == NULL )
	{
		Vector vecDistance = pTurret->GetAbsOrigin() - GetAbsOrigin();
		m_flNaturalDistance = vecDistance.Length() - 20;

		IPhysicsObject *pPhysObject = pTurret->VPhysicsGetObject();
		IPhysicsObject *pAttachPhysObject = VPhysicsGetObject();

		springparams_t spring;
		spring.constant = 10000;
		spring.damping = 16;
		spring.naturalLength = m_flNaturalDistance;
		spring.relativeDamping = 0;
		
		Vector up;
		GetVectors( NULL, NULL, &up );
		
		spring.startPosition = pTurret->GetAbsOrigin() + 20*up;
		spring.endPosition = GetAbsOrigin();

		spring.useLocalPositions = false;
		spring.onlyStretch = true;
		m_pSpring = physenv->CreateSpring( pPhysObject, pAttachPhysObject, &spring );

		if ( !m_hRope )
		{
			m_hRope = CRopeKeyframe::Create( this, pTurret, 0, 0, 1 );
			m_hRope->EnableWind( true );
		}
	}

	{
		Vector vecStart, vecEnd;
		m_pSpring->GetEndpoints( &vecStart, &vecEnd );

		vecStart -= vecEnd;
		if ( ( vecStart.Length() - m_flNaturalDistance ) > hover_turret_break_dist.GetFloat() )
		{
			pTurret->Event_Killed( CTakeDamageInfo( this, this, 200, DMG_BLAST ) );
			return;
		}

		// check to see if we should break.
		{
			Vector vecStart = GetAbsOrigin();

			Ray_t ray;
			ray.Init( vecStart, pTurret->GetAbsOrigin() );

			trace_t tr;
			UTIL_TraceRay( ray, MASK_SOLID_BRUSHONLY, NULL, COLLISION_GROUP_NONE, &tr );

			if ( tr.DidHitWorld() )
			{
				pTurret->Event_Killed( CTakeDamageInfo( this, this, 200, DMG_BLAST ) );
				return;
			}
		}
	}
	SetNextThink( gpGlobals->curtime );
}

//-----------------------------------------------------------------------------
// Hover Turret
//-----------------------------------------------------------------------------
class CNPC_HoverTurret : public CNPCBaseInteractive<CAI_BasePhysicsFlyingBot>, public CDefaultPlayerPickupVPhysics
{
	DECLARE_CLASS( CNPC_HoverTurret, CNPCBaseInteractive<CAI_BasePhysicsFlyingBot> );
	DECLARE_SERVERCLASS();

public:
	CNPC_HoverTurret();
	~CNPC_HoverTurret();

	Class_T			Classify( void ) { return CLASS_COMBINE; }

	// Attacking
	void			Event_Killed( const CTakeDamageInfo &info );
	void			TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr);
	void			NotifySystemEvent(CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params );

	void			ShootLaser( const Vector &vecSrc, const Vector &vecDirToEnemy );

	void			TalkThink( void );
	void			FindTargetThink( void );
	void			AimThink( void );

	void			SetFiringState( hoverTurretAttackState_e state );

	void			UpdateOnRemove( void );

	bool			OverrideMove(float flInterval);
	void			MoveToTarget(float flInterval, const Vector &MoveTarget);
	void			MoveExecute_Alive(float flInterval);
	void			MoveExecute_Dead(float flInterval);

	void			CreateSmokeTrail( void );
	void			DestroySmokeTrail( void );

	virtual Vector	BodyTarget( const Vector &posSrc, bool bNoisy = true ) { return WorldSpaceCenter(); }

	virtual float	GetHeadTurnRate( void ) { return 45.0f; } // Degrees per second
	virtual void	TurnHeadToTarget(float flInterval, const Vector &MoveTarget );
	bool			UpdateFacing( void );
	void			UpdateMuzzleMatrix( void );
	Vector			GetClosestVisibleEnemyPosition ( void );

	void			Precache(void);
	void			RunTask( const Task_t *pTask );
	void			Spawn(void);
	void			StartTask( const Task_t *pTask );

	void			GatherConditions();
	void			PrescheduleThink( void );
	void			Explode( void );
	void			StartDeathSequence( void );


	void			VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );
	virtual void	ClampMotorForces( Vector &linear, AngularImpulse &angular );

	// Player pickup
	virtual void	OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );
	virtual void	OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t Reason );
	virtual bool	HasPreferredCarryAnglesForPlayer( CBasePlayer *pPlayer );
	virtual QAngle	PreferredCarryAngles( void );

	// Use functions
	void	ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps() 
	{ 
		return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE;
	}

	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
	{
		CBasePlayer *pPlayer = ToBasePlayer( pActivator );
		if ( pPlayer )
		{
			pPlayer->PickupObject( this, false );
		}
	}

	CBasePlayer *HasPhysicsAttacker( float dt );

	float GetMaxEnginePower();
	bool IsMovementDisabled();

	DEFINE_CUSTOM_AI;

	DECLARE_DATADESC();
private:

	void MaintainGroundHeight( void );
	void TakeDamageFromPhysicsImpact( int index, gamevcollisionevent_t *pEvent );

	virtual bool	AllowedToIgnite( void ) { return true; }
	bool			OnBurning( void );

	// talking
	float			m_fNextTalk;
	int				m_iDesiredState;
	int 			m_iLastState;

	// Weapon variables
	bool			m_bAimingAtTarget;
	Vector			m_vecTargetPos;
	float			m_flAimStartTime;

	// networked vars
	CNetworkVar( int, m_iLaserState );
	CNetworkVar( int, m_sLaserHaloSprite );

	// Player pickup
	bool			m_bCarriedByPlayer;
	bool			m_bUseCarryAngles;

	// Movement variables
	Vector			m_vForceVelocity;		// Someone forced me to move
	Vector			m_vForceMoveTarget;

	Vector			m_vTargetBanking;

	EHANDLE			m_hAttachTarget;
	string_t		m_strAttachName;		// We are attached to the object here.

	EHANDLE			m_hTether;				// helper object that is going to handle our physics for us.
	float			m_flSentryTurnSpeed;	// Speed that the turret rotates around looking for targets

	bool			m_bCanPushPlayer;		// Do the shots from this turret prevent the player from advancing?

	float			m_flEngineStallTime;

	bool			m_bInitialPositionSet;

	COutputEvent	m_OnPhysGunPickup;
	COutputEvent	m_OnPhysGunDrop;

	float			m_fSparkTime;
	float			m_flDeathTime;
	CHandle<SmokeTrail>	m_hSmokeTrail;
	
	hoverTurretAttackState_e	m_iFiringState;
	CHandle<CSprite>			m_hEyeGlow;

	int				m_iMuzzleAttachment;
	matrix3x4_t		m_muzzleToWorld;
	int				m_muzzleToWorldTick;

	QAngle			m_vecGoalAngles;
	QAngle			m_vInitialLookAngles;
};

//-----------------------------------------------------------------------------
// HoverTurret schedules.
//-----------------------------------------------------------------------------
enum HoverTurretSchedules
{
	SCHED_HOVER_TURRET_IDLE = LAST_SHARED_SCHEDULE,
};


//-----------------------------------------------------------------------------
// HoverTurret tasks.
//-----------------------------------------------------------------------------
enum HoverTurretTasks
{
	TASK_HOVER_TURRET_HOVER = LAST_SHARED_TASK,
};

LINK_ENTITY_TO_CLASS( npc_hover_turret, CNPC_HoverTurret );

BEGIN_DATADESC( CNPC_HoverTurret )

DEFINE_FIELD( m_bAimingAtTarget,		FIELD_BOOLEAN ),
DEFINE_FIELD( m_vecTargetPos,			FIELD_VECTOR ),
DEFINE_FIELD( m_flAimStartTime,			FIELD_FLOAT),
DEFINE_FIELD( m_iFiringState,			FIELD_INTEGER),

DEFINE_FIELD( m_vForceVelocity,			FIELD_VECTOR),
DEFINE_FIELD( m_bInitialPositionSet,	FIELD_BOOLEAN),

DEFINE_FIELD( m_vecGoalAngles,			FIELD_VECTOR),
DEFINE_FIELD( m_vInitialLookAngles,		FIELD_VECTOR),
DEFINE_FIELD( m_vForceMoveTarget,		FIELD_VECTOR),
DEFINE_FIELD( m_vTargetBanking,			FIELD_VECTOR),

DEFINE_FIELD( m_flEngineStallTime,		FIELD_TIME),

DEFINE_FIELD( m_fNextTalk,				FIELD_FLOAT ),
DEFINE_FIELD( m_iDesiredState,			FIELD_INTEGER ),
DEFINE_FIELD( m_iLastState,				FIELD_INTEGER ),

// Player pickup
DEFINE_FIELD( m_bCarriedByPlayer,		FIELD_BOOLEAN ),
DEFINE_FIELD( m_bUseCarryAngles,		FIELD_BOOLEAN ),

DEFINE_FIELD( m_iMuzzleAttachment,		FIELD_INTEGER ),
DEFINE_FIELD( m_muzzleToWorldTick,		FIELD_INTEGER ),
DEFINE_FIELD( m_hEyeGlow,				FIELD_EHANDLE ),

DEFINE_FIELD( m_flDeathTime,			FIELD_FLOAT ),
DEFINE_FIELD( m_fSparkTime,				FIELD_FLOAT ),
DEFINE_FIELD( m_hSmokeTrail,			FIELD_EHANDLE ),
DEFINE_FIELD( m_hTether,				FIELD_EHANDLE ),

DEFINE_FIELD( m_hAttachTarget,			FIELD_EHANDLE ),
DEFINE_KEYFIELD( m_strAttachName,		FIELD_STRING,	"attachTarget" ),
DEFINE_KEYFIELD( m_flSentryTurnSpeed,	FIELD_FLOAT,	"sentryRotateSpeed" ),
DEFINE_KEYFIELD( m_bCanPushPlayer,		FIELD_BOOLEAN,	"canPushPlayer" ),

DEFINE_OUTPUT( m_OnPhysGunPickup, "OnPhysGunPickup" ),
DEFINE_OUTPUT( m_OnPhysGunDrop, "OnPhysGunDrop" ),

// Function Pointers
DEFINE_BASENPCINTERACTABLE_DATADESC(),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CNPC_HoverTurret, DT_NPC_HoverTurret)

	SendPropInt( SENDINFO( m_iLaserState ) ),
	SendPropInt( SENDINFO( m_sLaserHaloSprite ) ),

END_SEND_TABLE()


//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
CNPC_HoverTurret::CNPC_HoverTurret() :
	m_hEyeGlow( NULL )
{
#ifdef _DEBUG
	m_vForceMoveTarget.Init();
	m_vForceVelocity.Init();
#endif
	m_bUseCarryAngles = true;
	m_iLaserState = HOVER_TURRET_LASER_ON;
	m_sLaserHaloSprite = 0;

	m_hAttachTarget = NULL;
	m_hTether = NULL;
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
CNPC_HoverTurret::~CNPC_HoverTurret()
{
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::GatherConditions(  )
{
	BaseClass::GatherConditions();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::PrescheduleThink( void )
{
	// make a state and switch on that instead...
	if ( IsOnFire() )
	{
		OnBurning();
	}
	else if ( m_iHealth <= 0 )
	{
		if ( m_flDeathTime != 0 && 
			 m_flDeathTime < gpGlobals->curtime )
		{
			Explode();
		}
	}
	else 
	{
		switch( m_iFiringState )
		{
			case HOVER_TURRET_FIND_TARGET:
				FindTargetThink();
				break;
			case HOVER_TURRET_AIM_TARGET:
				AimThink();
				break;
			case HOVER_TURRET_SHOT_DISABLED:
				return;
		}
	}

	TalkThink();

	BaseClass::PrescheduleThink();
}

//-----------------------------------------------------------------------------
// Purpose: Sets the state of the glowing eye attached to the turret
// Input  : state - state the eye should be in
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::SetFiringState( hoverTurretAttackState_e state )
{
	if( !IsAlive() && state != HOVER_TURRET_SHOT_DISABLED )
	{
		return;
	}

	// Must have a valid eye to affect
	if ( !m_hEyeGlow )
	{
		// Create our eye sprite
		m_hEyeGlow = CSprite::SpriteCreate( HOVER_TURRET_GLOW_SPRITE, GetLocalOrigin(), false );
		if ( !m_hEyeGlow )
			return;

		m_hEyeGlow->SetTransparency( kRenderWorldGlow, 255, 0, 0, 128, kRenderFxNoDissipation );
		m_hEyeGlow->SetAttachment( this, HOVER_TURRET_PORTAL_EYE_ATTACHMENT );
	}

	bool bNewState = ( m_iFiringState != state );

	//Set the state
	switch( state )
	{
	default:
	case HOVER_TURRET_FIND_TARGET:
		m_bAimingAtTarget = false;
		m_flAimStartTime = 0.0f;

		m_iDesiredState = HOVER_TURRET_SEARCHING;
		m_iFiringState = HOVER_TURRET_FIND_TARGET;
		m_iLaserState.GetForModify() = HOVER_TURRET_LASER_ON;

		if ( bNewState )
		{
			m_nSkin = CRAZY_SKIN;
			SetActivity( (Activity) ACT_HOVER_TURRET_SEARCH );
		}

		m_hEyeGlow->SetColor( 0, 0, 255 );
		m_hEyeGlow->SetBrightness( 96, 0.1f );
		m_hEyeGlow->SetScale( 0.4f, 0.1f );
		break;
	case HOVER_TURRET_AIM_TARGET:
		m_iDesiredState = HOVER_TURRET_TARGETING;		
		m_bAimingAtTarget = true;
		m_vecTargetPos = GetClosestVisibleEnemyPosition();
		m_iFiringState = HOVER_TURRET_AIM_TARGET;
		m_flAimStartTime = gpGlobals->curtime;
		
		if ( bNewState )
		{
			SetActivity( (Activity) ACT_HOVER_TURRET_ALERT );
			m_nSkin = CURIOUS_SKIN;
//			m_iLaserState.GetForModify() = HOVER_TURRET_LASER_FIRING; // this is set elsewhere.
		}

		m_hEyeGlow->SetColor( 255, 255, 0 );
		m_hEyeGlow->SetBrightness( 196, 0.1f );
		m_hEyeGlow->SetScale( 0.4f, 0.1f );
		break;

	case HOVER_TURRET_SHOT_DISABLED:
		m_iFiringState = HOVER_TURRET_SHOT_DISABLED;
		m_iDesiredState = HOVER_TURRET_PICKUP;

		if ( bNewState )
		{
			SetActivity( (Activity) ACT_HOVER_TURRET_ANGRY );
			m_nSkin = AGGRESSIVE_SKIN;
			m_iLaserState.GetForModify() = HOVER_TURRET_LASER_OFF;
		}

		m_hEyeGlow->SetColor( 255, 0, 255 );
		m_hEyeGlow->SetScale( 0.1f, 3.0f );
		m_hEyeGlow->SetBrightness( 0, 3.0f );
		break;

	case HOVER_TURRET_HIT_WITH_PHYSICS:
		m_iFiringState = HOVER_TURRET_FIND_TARGET;
		m_iDesiredState = HOVER_TURRET_DESTRUCTING;
		SetActivity( (Activity) ACT_HOVER_TURRET_ANGRY );
		m_nSkin = AGGRESSIVE_SKIN;
		m_iLaserState.GetForModify() = HOVER_TURRET_LASER_OFF;

		m_hEyeGlow->SetColor( 255, 0, 0 );
		m_hEyeGlow->SetBrightness( 128, 0.1f );
		m_hEyeGlow->SetScale( 0.4f, 0.1f );
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::TalkThink( void )
{
//	// Working 2 enums into one integer
//	int iTalkState = state;

	// If the turret is dissolving go to a special state
	if ( IsDissolving() || IsOnFire() )
		m_iDesiredState = HOVER_TURRET_DESTRUCTING;

	// Need to play these sounds immediately
	if ( m_iLastState != m_iDesiredState && ( ( m_iDesiredState == HOVER_TURRET_TIPPED ) || 
												m_iDesiredState == HOVER_TURRET_DESTRUCTING || 
												m_iDesiredState == HOVER_TURRET_PICKUP ) )
	{
		m_fNextTalk = gpGlobals->curtime -1.0f;
	}

	// If we've changed states or are in a state with a repeating message
	if ( gpGlobals->curtime > m_fNextTalk && m_iLastState != m_iDesiredState )
	{
		m_iLastState = (hoverTurretState_e) m_iDesiredState;

		const char *pchScriptName = GetHoverTurretTalkName( m_iLastState );

		switch ( m_iDesiredState )
		{
			case HOVER_TURRET_TARGETING:
				EmitSound( pchScriptName );
				m_fNextTalk = gpGlobals->curtime + 1.75f;
				break;

			case HOVER_TURRET_SEARCHING:
				EmitSound( pchScriptName );
				m_fNextTalk = gpGlobals->curtime + 2.75f;
				break;

			case HOVER_TURRET_TIPPED:
				EmitSound( pchScriptName );
				m_fNextTalk = gpGlobals->curtime + 1.15f;
				break;

			case HOVER_TURRET_PICKUP:
				EmitSound( pchScriptName );
				m_fNextTalk = gpGlobals->curtime + 5.0f;
				break;

			case HOVER_TURRET_DESTRUCTING:
				EmitSound( pchScriptName );
				m_fNextTalk = gpGlobals->curtime + 10.0f;	// Never going to talk again
				break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::FindTargetThink( void )
{
	if( IsMovementDisabled() )
		return;

	GetEnemies()->RefreshMemories();
	GetSenses()->Look( HOVER_TURRET_RANGE );
	SetEnemy( BestEnemy() );

	if( m_iLaserState == HOVER_TURRET_LASER_OFF )
		m_iLaserState.GetForModify() = HOVER_TURRET_LASER_ON;

	if ( HasCondition( COND_SEE_ENEMY ) )
	{
		// see if this is something we could shoot at.
		CBaseEntity *pEnemy = GetEnemy();
		if ( pEnemy && pEnemy->IsAlive() )
		{
			//Look for our current enemy
			bool bEnemyInFOV = FInViewCone( pEnemy );
			bool bEnemyVisible = FVisible( pEnemy ) ;
			bool bEnemyVisibleThroughPortal = false;

			if( ( bEnemyInFOV && bEnemyVisible ) || bEnemyVisibleThroughPortal )
			{
				SetFiringState( HOVER_TURRET_AIM_TARGET );
			}
		}
	}
	else
	{
		m_vecGoalAngles = GetAbsAngles();
		m_flFieldOfView = 0.4f;
	}

	UpdateFacing();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::AimThink( void )
{
	bool bAbortAim = false;

	// if at any point we lost our target during the aiming we want to 
	if ( !HasCondition( COND_SEE_ENEMY ) || IsMovementDisabled() )
	{
		bAbortAim = true;
	}
	else if( GetEnemy() )
	{
		// check the difference between our last position and this one. If it's too big then abort.
		Vector vecEnemyPosition = GetClosestVisibleEnemyPosition();
		Vector vecEnemyMovement = vecEnemyPosition - m_vecTargetPos;
		
		m_vecTargetPos = vecEnemyPosition;

		if ( vecEnemyMovement.Length() > HOVER_TURRET_TARGET_LOSS_DISTANCE )
		{
			bAbortAim = true;
		}
	}

	if ( bAbortAim )
	{
		m_vecGoalAngles = GetAbsAngles();
		UpdateFacing();

		if( m_bAimingAtTarget )
		{
			SetFiringState( HOVER_TURRET_FIND_TARGET );
		}
		return;
	}

	UpdateFacing();

	// we shouldn't be able to get here without being in this activity, need to debug this...
	if ( GetActivity() != (Activity) ACT_HOVER_TURRET_ALERT )
		SetActivity( (Activity) ACT_HOVER_TURRET_ALERT );

	// we aimed for the required amount of time. See if we can still see our target, otherwise abort.
	if( (m_flAimStartTime + 1.5f) < gpGlobals->curtime )
	{
		//Look for our current enemy
		CBaseEntity *pEnemy = GetEnemy();
		if ( pEnemy && pEnemy->IsAlive() )
		{
			bool bEnemyInFOV = FInViewCone( pEnemy );
			bool bEnemyVisible = FVisible( pEnemy ) ;
			bool bEnemyVisibleThroughPortal = false;

			if( ( bEnemyInFOV && bEnemyVisible ) || bEnemyVisibleThroughPortal )
			{
				m_flAimStartTime -= HOVER_TURRET_SHOT_CYCLE;

				UpdateMuzzleMatrix();
					
				Vector	vecMuzzle, vecMuzzleDir;
				MatrixGetColumn( m_muzzleToWorld, 3, vecMuzzle );
				MatrixGetColumn( m_muzzleToWorld, 0, vecMuzzleDir );

				ShootLaser( vecMuzzle, vecMuzzleDir );

				if( m_iLaserState != HOVER_TURRET_LASER_FIRING )
					m_iLaserState.GetForModify() = HOVER_TURRET_LASER_FIRING;
			}
			else
			{
				SetFiringState( HOVER_TURRET_FIND_TARGET );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
Vector CNPC_HoverTurret::GetClosestVisibleEnemyPosition ( void )
{
	CBaseEntity* pEnemy = GetEnemy();
	assert( pEnemy );

	UpdateMuzzleMatrix();	
	Vector	vecMuzzlePos;
	MatrixGetColumn( m_muzzleToWorld, 3, vecMuzzlePos );
	Vector vecMidEnemy = ( 0.35f*pEnemy->CollisionProp()->WorldSpaceCenter() + 0.65f*pEnemy->BodyTarget( vecMuzzlePos, false ) );
	
	Vector vecMid = EyePosition();
	Vector vecBody = pEnemy->BodyTarget( vecMid );
	

	//Calculate dir and dist to enemy
	Vector	vecDirToEnemy = vecMidEnemy - vecMuzzlePos;	

	QAngle vecAnglesToEnemy;
	VectorNormalize( vecDirToEnemy );
	VectorAngles( vecDirToEnemy, vecAnglesToEnemy );
	
	//If we can see our enemy, face it
	m_vecGoalAngles = vecAnglesToEnemy;

	return vecMidEnemy;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr )
{
	BaseClass::TraceAttack( info, vecDir, ptr );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::Event_Killed( const CTakeDamageInfo &info )
{
	if ( m_hSmokeTrail == NULL )
	{
		CreateSmokeTrail();
		m_fSparkTime = gpGlobals->curtime + 0.1f;
	}

	if ( m_pMotionController != NULL )
	{
		physenv->DestroyMotionController( m_pMotionController );
		m_pMotionController = NULL;
	}

	if( m_hTether )
	{
		CHoverTurretTether* pTether = (CHoverTurretTether*) m_hTether.Get();
		pTether->ReleaseTether();
		EmitSound( "NPC_HoverTurret.Snap" );
		UTIL_Remove( m_hTether );
		m_hTether = NULL;
	}

	if( !m_bCarriedByPlayer )
	{
		StartDeathSequence();
	}

	SetFiringState( HOVER_TURRET_SHOT_DISABLED );
	m_iHealth = 0;
	
	m_OnDeath.FireOutput( this, this );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::StartDeathSequence( void )
{
	if ( m_flDeathTime == 0.f )
	{
		m_flDeathTime = gpGlobals->curtime + 2.5f;
		EmitSound ( HOVER_TURRET_SOUND_LOCKING );
		EmitSound ( HOVER_TURRET_SOUND_LOCKING, gpGlobals->curtime + 0.5f );
		EmitSound ( HOVER_TURRET_SOUND_LOCKING, gpGlobals->curtime + 1.0f );
		EmitSound ( HOVER_TURRET_SOUND_LOCKING, gpGlobals->curtime + 1.5f );
		EmitSound ( HOVER_TURRET_SOUND_LOCKED, gpGlobals->curtime + 2.0f );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::NotifySystemEvent(CBaseEntity *pNotify, notify_system_event_t eventType, const notify_system_event_params_t &params )
{
}

//-----------------------------------------------------------------------------
// Take damage from combine ball
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::TakeDamageFromPhysicsImpact( int index, gamevcollisionevent_t *pEvent )
{
	CBaseEntity *pHitEntity = pEvent->pEntities[!index];

//	// NOTE: Bypass the normal impact energy scale here.
	float flDamageScale = 20.0f;//PlayerHasMegaPhysCannon() ? 5.0f : 2.5f;

	// Scale by the mapmaker's energyscale
	flDamageScale *= m_impactEnergyScale;

	int damageType = 0;
	float damage = CalculateDefaultPhysicsDamage( index, pEvent, flDamageScale, true, damageType );
	
	if ( FClassnameIs( pHitEntity, "prop_energy_ball" ) )
	{
		damage = 100;
	}

	if ( damage == 0 )
		return;

	Vector damagePos;
	pEvent->pInternalData->GetContactPoint( damagePos );
	Vector damageForce = pEvent->postVelocity[index] * pEvent->pObjects[index]->GetMass();
	if ( damageForce == vec3_origin )
	{
		// This can happen if this entity is motion disabled, and can't move.
		// Use the velocity of the entity that hit us instead.
		damageForce = pEvent->postVelocity[!index] * pEvent->pObjects[!index]->GetMass();
	}

	if( damage > 50  )
	{
		Event_Killed( CTakeDamageInfo( pHitEntity, pHitEntity, damageForce, damagePos, damage, damageType ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	if ( m_iHealth <= 0 )
	{
		return;
	}

	BaseClass::VPhysicsCollision( index, pEvent );

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	m_flFieldOfView = -1.f;

	if ( pHitEntity )
	{
		if( !pHitEntity->IsPlayer() )
		{
			// Stall out for a short time, only if we hit an actual entity
			if ( !pHitEntity->IsWorld() )
			{
				m_flEngineStallTime = gpGlobals->curtime + 4.0f;

				if ( m_iFiringState != HOVER_TURRET_SHOT_DISABLED )
				{
					SetFiringState( HOVER_TURRET_HIT_WITH_PHYSICS );
				}
			}
			
			// we always take damage
			TakeDamageFromPhysicsImpact( index, pEvent );
		}
		else if( m_iFiringState != HOVER_TURRET_AIM_TARGET ) // we get a little bump if we were caught off guard
		{
			SetFiringState( HOVER_TURRET_FIND_TARGET );
			m_flEngineStallTime = gpGlobals->curtime + 0.5f;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::MaintainGroundHeight( void )
{
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::TurnHeadToTarget(float flInterval, const Vector &MoveTarget )
{
	float flDestYaw = VecToYaw( MoveTarget - GetLocalOrigin() );
	
	float newYaw = AI_ClampYaw( GetHeadTurnRate() * 10.0f, m_fHeadYaw, flDestYaw, gpGlobals->curtime - GetLastThink() );
		
	if ( newYaw != m_fHeadYaw )
	{
		m_fHeadYaw = newYaw;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Causes the turret to face its desired angles
//-----------------------------------------------------------------------------
bool CNPC_HoverTurret::UpdateFacing( void )
{
	bool  bMoved = false;
	UpdateMuzzleMatrix();

	Vector vecGoalDir;
	AngleVectors( m_vecGoalAngles, &vecGoalDir );

	Vector vecGoalLocalDir;
	VectorIRotate( vecGoalDir, m_muzzleToWorld, vecGoalLocalDir );

////	if ( g_debug_turret.GetBool() )
//	{
//		Vector	vecMuzzle, vecMuzzleDir;
//
//		MatrixGetColumn( m_muzzleToWorld, 3, vecMuzzle );
//		MatrixGetColumn( m_muzzleToWorld, 0, vecMuzzleDir );
//
//		NDebugOverlay::Cross3D( vecMuzzle, -Vector(2,2,2), Vector(2,2,2), 255, 255, 0, false, 0.05 );
//		NDebugOverlay::Cross3D( vecMuzzle+(vecMuzzleDir*256), -Vector(2,2,2), Vector(2,2,2), 255, 255, 0, false, 0.05 );
//		NDebugOverlay::Line( vecMuzzle, vecMuzzle+(vecMuzzleDir*256), 255, 255, 0, false, 0.05 );
//		
//		NDebugOverlay::Cross3D( vecMuzzle, -Vector(2,2,2), Vector(2,2,2), 255, 0, 0, false, 0.05 );
//		NDebugOverlay::Cross3D( vecMuzzle+(vecGoalDir*256), -Vector(2,2,2), Vector(2,2,2), 255, 0, 0, false, 0.05 );
//		NDebugOverlay::Line( vecMuzzle, vecMuzzle+(vecGoalDir*256), 255, 0, 0, false, 0.05 );
//	}

	QAngle vecGoalLocalAngles;
	VectorAngles( vecGoalLocalDir, vecGoalLocalAngles );

	float flDiff = AngleNormalize( UTIL_ApproachAngle(  vecGoalLocalAngles.x, 0.0, 0.25f * MaxYawSpeed() ) );
	SetPoseParameter( m_poseAim_Pitch, GetPoseParameter( m_poseAim_Pitch ) + ( flDiff / 2.5f ) );

	if ( fabs( flDiff ) > 0.1f )
	{
		bMoved = true;
	}

	// Update yaw
	flDiff = AngleNormalize( UTIL_ApproachAngle(  vecGoalLocalAngles.y, 0.0, 0.05f * MaxYawSpeed() ) );
	SetPoseParameter( m_poseAim_Yaw, GetPoseParameter( m_poseAim_Yaw ) + ( flDiff / 2.5f ) );

	if ( fabs( flDiff ) > 0.1f )
	{
		bMoved = true;
	}

	// You're going to make decisions based on this info.  So bump the bone cache after you calculate everything
	InvalidateBoneCache();

	return bMoved;
}

void CNPC_HoverTurret::UpdateMuzzleMatrix()
{
	if ( gpGlobals->tickcount != m_muzzleToWorldTick )
	{
		m_muzzleToWorldTick = gpGlobals->tickcount;
		GetAttachment( m_iMuzzleAttachment, m_muzzleToWorld );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles movement towards the last move target.
// Input  : flInterval - 
//-----------------------------------------------------------------------------
bool CNPC_HoverTurret::OverrideMove( float flInterval )
{	
	if ( m_iHealth <= 0 )
	{
		MoveExecute_Dead( flInterval );
		return true;
	}

	if ( !m_bInitialPositionSet )
	{
		m_vForceMoveTarget = GetAbsOrigin();
		m_vForceMoveTarget.z += HOVER_MIN_HEIGHT;
		m_bInitialPositionSet = true;
	}
	
	if ( VPhysicsGetObject() )
		VPhysicsGetObject()->Wake();

	// If we're stalled, do nothing
	if ( IsMovementDisabled() )
		return false;

	// verify we have a movement controller.
	if( !IsMovementDisabled() && m_pMotionController == NULL )
	{
		m_pMotionController = physenv->CreateMotionController( this );
		m_pMotionController->AttachObject( VPhysicsGetObject(), true );
	}

	MoveToTarget( flInterval, m_vForceMoveTarget );

	{
		MoveExecute_Alive( flInterval );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::MoveExecute_Alive( float flInterval )
{
	PhysicsCheckWaterTransition();

	Vector vCurrentVelocity = GetCurrentVelocity();

	// FIXME: move this
	if ( VPhysicsGetObject() && m_hTether != NULL )
		VPhysicsGetObject()->Wake();

	vCurrentVelocity.z -= 0.1f*flInterval * sv_gravity.GetFloat();

	// ----------------------------------------------------------------------------------------
	// Add in any forced velocity
	// ----------------------------------------------------------------------------------------
	SetCurrentVelocity( vCurrentVelocity + m_vForceVelocity );
	m_vForceVelocity = vec3_origin;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::MoveExecute_Dead( float flInterval )
{
	Vector newVelocity = GetCurrentVelocity();

	Vector forward, up;
	GetVectors( &forward, NULL, &up );

	// sparks.
	if (gpGlobals->curtime > m_fSparkTime)
	{
		g_pEffects->Sparks( GetAbsOrigin(), 1, 1 );
		m_fSparkTime = gpGlobals->curtime + random->RandomFloat(0.05, 0.1);
	}

	// ----------------------------------------------------------------------------------------
	// Add in any forced velocity
	// ----------------------------------------------------------------------------------------
	newVelocity += m_vForceVelocity;
	SetCurrentVelocity( newVelocity );
	m_vForceVelocity = vec3_origin;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPhysGunUser - 
//			reason - 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	if ( !IsAlive() )
	{
		return;
	}
	
	if ( m_pMotionController != NULL )
	{
		physenv->DestroyMotionController( m_pMotionController );
		m_pMotionController = NULL;
	}

	m_bCarriedByPlayer = true;

	SetFiringState( HOVER_TURRET_SHOT_DISABLED );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPhysGunUser - 
//			Reason - 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t Reason )
{
	m_bCarriedByPlayer = false;

	if ( m_iHealth <= 0 )
	{
		StartDeathSequence();
		return;
	}

	if ( m_hTether != NULL )
	{
		SetFiringState( HOVER_TURRET_FIND_TARGET );
		m_flEngineStallTime = gpGlobals->curtime + 2.5f;
		m_flFieldOfView = -1;
	}
	else
	{
		m_iDesiredState = HOVER_TURRET_DESTRUCTING;
		m_nSkin = DEFAULT_SKIN;

		SetActivity( (Activity) ACT_HOVER_TURRET_DISABLED );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Whether this should return carry angles
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_HoverTurret::HasPreferredCarryAnglesForPlayer( CBasePlayer *pPlayer )
{
	if ( !IsAlive() )
	{
		return false;
	}
	return m_bUseCarryAngles;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const QAngle
//-----------------------------------------------------------------------------
QAngle CNPC_HoverTurret::PreferredCarryAngles( void )
{
	static QAngle g_prefAngles;

	Vector vecUserForward;
	CBasePlayer *pPlayer = AI_GetSinglePlayer();
	pPlayer->EyeVectors( &vecUserForward );

	// If we're looking up, then face directly forward
	if ( vecUserForward.z >= 0.0f )
		return vec3_angle;

	// Otherwise, stay "upright"
	g_prefAngles.Init();
	g_prefAngles.x = -pPlayer->EyeAngles().x;
	
	return g_prefAngles;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::Precache( void )
{
	PrecacheModel( "models/props_junk/rock001a.mdl" );
	PrecacheModel("effects/redlaser1.vmt");

	PrecacheModel("models/props/futbol_Gib01.mdl");
	PrecacheModel("models/props/futbol_Gib02.mdl");
	PrecacheModel("models/props/futbol_Gib03.mdl");
	PrecacheModel("models/props/futbol_Gib04.mdl");

	PrecacheModel( HOVER_TURRET_MODEL );

	ADD_CUSTOM_ACTIVITY( CNPC_HoverTurret, ACT_HOVER_TURRET_SEARCH );
	ADD_CUSTOM_ACTIVITY( CNPC_HoverTurret, ACT_HOVER_TURRET_ALERT );
	ADD_CUSTOM_ACTIVITY( CNPC_HoverTurret, ACT_HOVER_TURRET_ANGRY );
	ADD_CUSTOM_ACTIVITY( CNPC_HoverTurret, ACT_HOVER_TURRET_DISABLED );	

	PrecacheScriptSound ( HOVER_TURRET_SOUND_LOCKING );
	PrecacheScriptSound ( HOVER_TURRET_SOUND_LOCKED );

	PrecacheScriptSound ( HOVER_TURRET_LASER_BURN_SOUND );
	PrecacheScriptSound ( "NPC_HoverTurret.Snap" );

	UTIL_PrecacheDecal( HOVER_TURRET_DECAL_NAME );
	PrecacheModel( HOVER_TURRET_MODEL_NAME );

	PrecacheModel(HOVER_TURRET_LASER_EFFECT);
	m_sLaserHaloSprite.GetForModify() = PrecacheModel(HOVER_TURRET_GLOW_SPRITE);

	for ( int iTalkScript = 0; iTalkScript < HOVER_TURRET_STATE_TOTAL; ++iTalkScript )
	{
		PrecacheScriptSound( g_HoverTurretTalkNames[ iTalkScript ] );
	}

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pTask - 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::RunTask( const Task_t *pTask )
{
	switch ( pTask->iTask )
	{
		// Override this task so we go for the enemy at eye level
	case TASK_HOVER_TURRET_HOVER:
		break;

	default:
		BaseClass::RunTask(pTask);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::Spawn(void)
{
	Precache();

	SetModel( HOVER_TURRET_MODEL );
	SetHullType( HULL_TINY_CENTERED );
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );

	SetMoveType( MOVETYPE_VPHYSICS );

	m_iHealth			= sk_hover_turret_health.GetFloat();
	m_NPCState			= NPC_STATE_NONE;

	SetPoseParameter( m_poseAim_Yaw, 0 );
	SetPoseParameter( m_poseAim_Pitch, 0 );
	SetActivity( (Activity) ACT_HOVER_TURRET_SEARCH );

	m_iLastState = m_iDesiredState = HOVER_TURRET_SEARCHING;
	SetFiringState( HOVER_TURRET_FIND_TARGET );

	SetNavType(NAV_FLY);

	AddEFlags( EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL );
	AddEffects( EF_NOSHADOW );

	SetBloodColor( DONT_BLEED );
	SetCurrentVelocity( vec3_origin );
	m_vForceVelocity.Init();
	m_vCurrentBanking.Init();
	m_vTargetBanking.Init();

	CapabilitiesAdd( bits_CAP_INNATE_MELEE_ATTACK1 | bits_CAP_MOVE_FLY | bits_CAP_SQUAD );

	// Noise modifier
	Vector	bobAmount;
	bobAmount.x = random->RandomFloat( -1.0f, 1.0f );
	bobAmount.y = random->RandomFloat( -1.0f, 1.0f );
	bobAmount.z = random->RandomFloat( -1.0f, 1.0f );

	SetNoiseMod( bobAmount );

	m_fHeadYaw = 0;

	NPCInit();

	SetViewOffset( Vector(0, 0, 10) );		// Position of the eyes relative to NPC's origin.
	m_flFieldOfView		= 0.4f;				

	// don't be an NPC, we want to collide with debris stuff
	SetCollisionGroup( COLLISION_GROUP_NONE );

	m_iMuzzleAttachment = LookupAttachment( "eyes" );
	m_vecGoalAngles = GetAbsAngles();
	m_vInitialLookAngles = GetAbsAngles();

	m_bCarriedByPlayer = false;
	m_flDeathTime = 0.f;

	if ( vec3_origin == GetAbsOrigin() )
	{
		m_bInitialPositionSet = false;
	}
	else
	{
		m_bInitialPositionSet = true;
		m_vForceMoveTarget = GetAbsOrigin();
	}

	m_hAttachTarget = gEntList.FindEntityByName( NULL, m_strAttachName );

	if ( m_hAttachTarget != NULL && m_hTether == NULL )
	{
		m_hTether = CreateEntityByName( "ent_hover_turret_tether" );
		m_hTether->SetAbsOrigin( m_hAttachTarget.Get()->GetAbsOrigin() );
		CHoverTurretTether *pHoverTurretTether = (CHoverTurretTether *) m_hTether.Get();
		pHoverTurretTether->AttachEntities( this, m_hAttachTarget );
		DispatchSpawn( m_hTether );

		CBaseEntity* pAttachEnt = m_hAttachTarget.Get();
		Vector vecToAttach = pAttachEnt->GetAbsOrigin() - GetAbsOrigin();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pTask - 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::StartTask( const Task_t *pTask )
{
	switch (pTask->iTask)
	{	
	case TASK_HOVER_TURRET_HOVER:
		break;

	default:
		BaseClass::StartTask(pTask);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::UpdateOnRemove( void )
{
	// just do a talk think, to say our death message.
	TalkThink();

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::ClampMotorForces( Vector &linear, AngularImpulse &angular )
{
	if ( m_iHealth <= 0 )
	{
		linear.x = 0.0f;
		linear.y = 0.0f;
		linear.z = 0.0f;

		angular.x = 0.0f;
		angular.y = 0.0f;
		angular.z = 0.0f;

		return;
	}
	
	linear.x = 0.0f;
	linear.y = 0.0f;
	linear.z = 0.0f;

	// cancel out our upright control if we aren't aiming at anything
	if ( !m_bAimingAtTarget )
	{
		angular.x *= 0.01f; // just a little, so we eventually settle down
		angular.y *= 0.01f;

		if ( m_flSentryTurnSpeed == 0.0f )
		{
			linear.x = random->RandomFloat( -50.0f, 50.0f );
			linear.y = random->RandomFloat( -50.0f, 50.0f );
			linear.z = random->RandomFloat( -150.0f, 150.0f );
		}
	}
}

CBasePlayer *CNPC_HoverTurret::HasPhysicsAttacker( float dt )
{
	return NULL;
}

//-----------------------------------------------------------------------------
// HoverTurrets that have been hacked by Alyx get more engine power (fly faster)
//-----------------------------------------------------------------------------
float CNPC_HoverTurret::GetMaxEnginePower()
{
	return 1.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pTask - 
//-----------------------------------------------------------------------------
bool CNPC_HoverTurret::IsMovementDisabled( void )
{
	if ( m_flEngineStallTime > gpGlobals->curtime ||
		 m_iFiringState == HOVER_TURRET_SHOT_DISABLED )
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::MoveToTarget(float flInterval, const Vector &vMoveTarget)
{
	if ( flInterval <= 0 || IsMovementDisabled() )
		return;

	if ( GetEnemy() != NULL && HasCondition( COND_SEE_ENEMY ) )
	{
		TurnHeadToTarget( flInterval, GetClosestVisibleEnemyPosition() );
	}
	else if ( m_flSentryTurnSpeed != 0.0f )
	{
		float newYaw = AI_ClampYaw( m_flSentryTurnSpeed, m_fHeadYaw, m_fHeadYaw + flInterval*m_flSentryTurnSpeed, gpGlobals->curtime - GetLastThink() );
			
		if ( newYaw != m_fHeadYaw )
		{
			m_fHeadYaw = newYaw;
		}
	}
	else
	{
		Vector vecMoveDir = vMoveTarget - GetAbsOrigin();
		vecMoveDir.NormalizeInPlace();
		
		Vector vecLookDir; 
		AngleVectors( m_vInitialLookAngles, &vecLookDir );

		vecLookDir = 5*vecLookDir + vecMoveDir;

		TurnHeadToTarget( flInterval, GetAbsOrigin() + vecLookDir );
	}

	return;
}

//-----------------------------------------------------------------------------
// Purpose: Fire!
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::ShootLaser( const Vector &vecSrc, const Vector &vecDirToEnemy )
{
	trace_t tr;
	Vector vecCenter = GetAbsOrigin();
	UTIL_TraceLine( vecSrc, vecSrc + vecDirToEnemy * HOVER_TURRET_RANGE, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
	
	if ( tr.fraction != 1.0 )
	{
		if ( tr.m_pEnt != NULL )
		{
			CBaseEntity *pHit = tr.m_pEnt;
			if ( pHit )
			{
				ClearMultiDamage();
				Vector dir = tr.endpos - GetAbsOrigin();
				VectorNormalize( dir );
				int nDamageType = DMG_ENERGYBEAM;

				CTakeDamageInfo info( this, this, HOVER_TURRET_LASER_DAMAGE * (gpGlobals->curtime - GetLastThink()), nDamageType );
				CalculateMeleeDamageForce( &info, dir, tr.endpos, m_bCanPushPlayer ? HOVER_TURRET_LASER_FORCE_MULTIPLIER : 0 );
				pHit->DispatchTraceAttack( info, dir, &tr );
				ApplyMultiDamage();
				UTIL_DecalTrace( &tr, "RedGlowFade" );

				// Burn it!
				if ( pHit->IsPlayer() )
				{
					pHit->EmitSound( HOVER_TURRET_LASER_BURN_SOUND );
				}
			}
		}
		g_pEffects->Sparks( tr.endpos );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles turrets being set on fire by lasers
//-----------------------------------------------------------------------------
bool CNPC_HoverTurret::OnBurning( void )
{
	// Tick down
	m_iHealth -= 1;

	if ( m_iHealth <= 0 )
	{
		Explode();
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Handles turrets being set on fire by lasers
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::Explode( void )
{
	ExplosionCreate(WorldSpaceCenter(), vec3_angle, this, 1000, 500.0f,
		SF_ENVEXPLOSION_NODAMAGE | SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS |
		SF_ENVEXPLOSION_NOSMOKE | SF_ENVEXPLOSION_NOFIREBALLSMOKE, 0);

	UTIL_ScreenShake(WorldSpaceCenter(), 25.0, 150.0, 1.0, 750.0f, SHAKE_START);

	CPVSFilter filter(WorldSpaceCenter());
	Vector gibVelocity = RandomVector(-150, 150);
	int iModelIndex = modelinfo->GetModelIndex(g_PropDataSystem.GetRandomChunkModel("MetalChunks"));
	for (int i = 0; i < 32; i++)
	{
		te->BreakModel(filter, 0.0, WorldSpaceCenter(), vec3_angle, Vector(16, 16, 72), gibVelocity, iModelIndex, 400, 1, 2.5, BREAK_METAL);
	}

	AddEffects(EF_NODRAW);
	SetThink(&CBaseEntity::SUB_Remove);
	SetNextThink(gpGlobals->curtime + 0.1f);
}

//-----------------------------------------------------------------------------
// Create smoke trail!
//-----------------------------------------------------------------------------
void CNPC_HoverTurret::CreateSmokeTrail()
{
	if ( m_hSmokeTrail != NULL )
		return;

	SmokeTrail *pSmokeTrail =  SmokeTrail::CreateSmokeTrail();
	if( !pSmokeTrail )
		return;

	pSmokeTrail->m_SpawnRate = 64;
	pSmokeTrail->m_ParticleLifetime = 0.5f;
	pSmokeTrail->m_StartSize	= 8;
	pSmokeTrail->m_EndSize		= 16;
	pSmokeTrail->m_SpawnRadius	= 16;
	pSmokeTrail->m_MinSpeed		= 60;
	pSmokeTrail->m_MaxSpeed		= 100;
	
	pSmokeTrail->m_StartColor.Init( 0.5f, 0.5f, 0.5f );
	pSmokeTrail->m_EndColor.Init( 0, 0, 0 );
	
	pSmokeTrail->SetLifetime(-1);
	pSmokeTrail->FollowEntity(this);

	m_hSmokeTrail = pSmokeTrail;
}

void CNPC_HoverTurret::DestroySmokeTrail()
{
	if ( m_hSmokeTrail.Get() )
	{
		UTIL_Remove( m_hSmokeTrail );
		m_hSmokeTrail = NULL;
	}
}

//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC( npc_hover_turret, CNPC_HoverTurret )

AI_END_CUSTOM_NPC()
