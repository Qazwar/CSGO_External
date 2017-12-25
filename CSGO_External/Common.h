#pragma once
#include "PatternTemplate.h"
#include "XOR.h"

// Static
static const DWORD g_dwEntityCount = 0x40; // 64
static const DWORD g_dwForceUpdate = 0x16C;

// Offset list
static enum EOffsets
{
	// NOTE: "Pattern offset", Extra value on hazedumper
	NULL_Pattern					= 0x0,
	NULL_Addres						= 0x0,

	LocalPlayer_Pattern				= 0x1,
	LocalPlayer_Address				= 0x10,

	EntityList_Pattern				= 0x1,
	EntityList_Address				= 0x0,

	GlowObjectManager_Pattern		= 0x19,
	GlowObjectManager_Address		= 0x0,

	ForceAttack_Pattern				= 0x2,
	ForceAttack_Address				= 0x0,

	ForceJumo_Pattern				= 0x2,
	ForceJump_Address				= 0x0,

	Dormant_Pattern					= 0xC,
	Dormant_Address					= 0x0,

	ClientState_Pattern				= 0x1,
	ClientState_Address				= 0x0,

	Ingame_Pattern					= 0x2,
	Ingame_Address					= 0x0,

	ViewAngles_Pattern				= 0x4,
	ViewAngles_Address				= 0x0,

	GlobalVars_Pattern				= 0x1,
	GlobalVars_Address				= 0x0,

	ForceLeft_Pattern				= 0x1D1,
	ForceLeft_Address				= 0x0,

	ForceRight_Pattern				= 0x200,
	ForceRight_Address				= 0x0,

	ClientStateState_Pattern		= 0x2,
	ClientStateState_Address		= 0x0,

	MouseEnable_Pattern				= 0x1,
	MouseEnable_Address				= 0x30,
	
	Input_Pattern					= 0x1,
	Input_Address					= 0x0,

	PlayerInfo_Pattern				= 0x2,
	PlayerInfo_Address				= 0x0,

	Sensitivity_Pattern				= 0x2,
	Sensitivity_Address				= 0x2C,

	SendPackets_Pattern				= 0x1,
	SendPackets_Address				= 0x0,

	ClientStateViewAngles_Pattern	= 0x4,
	ClientStateViewAngles_Address	= 0,
	
	PlayerResource_Pattern			= 0x2,
	PlayerResource_Address			= 0,

	ViewMatrix_Pattern				= 0x3,
	ViewMatrix_Address				= 0xB0
};

// TODO: XOR bytes & mask
// Pattern informations
static struct TLocalPlayer {
	unsigned long	patternOffset	= EOffsets::LocalPlayer_Pattern;
	unsigned long	AddrOffset		= EOffsets::LocalPlayer_Address;
	
	const char*		mask			= "x????xx????????x????xxx";
	unsigned char	pattern[23]		= { 0xA3, 0x0, 0x0, 0x0, 0x0, 0xc7, 0x05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xE8, 0x0, 0x0, 0x0, 0x0, 0x59, 0xC3, 0x6A };
} SLocalPlayer;

static struct TEntityList {
	unsigned long	patternOffset	= EOffsets::EntityList_Pattern;
	unsigned long	AddrOffset		= EOffsets::EntityList_Address;
	
	const char*		mask			= "x????xxxxx????xx";
	unsigned char	pattern[16]		= { 0xBB, 0x0, 0x0, 0x0, 0x0, 0x83, 0xFF, 0x01, 0x0F, 0x8C, 0x0, 0x0, 0x0, 0x0, 0x3B, 0XF8 };
} SEntityList;

static struct TGlowObjectManager {
	unsigned long	patternOffset	= EOffsets::GlowObjectManager_Pattern;
	unsigned long	AddrOffset		= EOffsets::GlowObjectManager_Address;

	const char*		mask			= "x????xxxx";
	unsigned char	pattern[9]		= { 0xA1, 0x0, 0x0, 0x0, 0x0, 0xA8, 0x01, 0x75, 0x4B };
} SGlowObjectManager;

static struct TForceAttack {
	unsigned long	patternOffset	= EOffsets::ForceAttack_Pattern;
	unsigned long	AddrOffset		= EOffsets::ForceAttack_Address;

	const char*		mask			= "xx????xx????xxxxxxx";
	unsigned char	pattern[19]		= { 0x89, 0x0D, 0x0, 0x0, 0x0, 0x0, 0x8B, 0x0D, 0x0, 0x0, 0x0, 0x0, 0x8B, 0xF2, 0x8B, 0xC1, 0x83, 0xCE, 0x04 };
} SForceAttack;

static struct TForceJump {
	unsigned long	patternOffset	= EOffsets::ForceJumo_Pattern;
	unsigned long	AddrOffset		= EOffsets::ForceJump_Address;

	const char*		mask			= "xx????xxxxxxx";
	unsigned char	pattern[13]		= { 0x8B, 0x0D, 0x0, 0x0, 0x0, 0x0, 0x8B, 0xD6,	0x8B, 0xC1, 0x83, 0xCA, 0x02 };
} SForceJump;

static struct TDormant {
	unsigned long	patternOffset	= EOffsets::Dormant_Pattern;
	unsigned long	AddrOffset		= EOffsets::Dormant_Address;

	const char*		mask			= "xxxxxxxxxxxx????x";
	unsigned char	pattern[17]		= { 0x55, 0x8B, 0xEC, 0x53, 0x8B, 0x5D, 0x08, 0x56, 0x8B, 0xF1, 0x88, 0x9E, 0x0, 0x0, 0x0, 0x0, 0xE8 };
} SDormant;

static struct TClientState {
	unsigned long	patternOffset	= EOffsets::ClientState_Pattern;
	unsigned long	AddrOffset		= EOffsets::ClientState_Address;

	const char*		mask			= "x????xxxxxxxxxx";
	unsigned char	pattern[15]		= { 0xA1, 0x0, 0x0, 0x0, 0x0, 0x33, 0xD2, 0x6A,	0x00, 0x6A, 0x00, 0x33, 0xC9, 0x89, 0xB0 };
} SClientState;

static struct TIngame {
	unsigned long	patternOffset	= EOffsets::Ingame_Pattern;
	unsigned long	AddrOffset		= EOffsets::Ingame_Address;

	const char*		mask			= "xx????xxxxx";
	unsigned char	pattern[11]		= { 0x83, 0xB9, 0x0, 0x0, 0x0, 0x0, 0x06, 0x0F,	0x94, 0xC0, 0xC3 };
} SIngame;

static struct TViewAngles {
	unsigned long	patternOffset	= EOffsets::ViewAngles_Pattern;
	unsigned long	AddrOffset		= EOffsets::ViewAngles_Address;

	const char*		mask			= "xxxx????xxxxx????";
	unsigned char	pattern[17]		= { 0xF3, 0x0F, 0x11, 0x80, 0x0, 0x0, 0x0, 0x0, 0xD9, 0x46, 0x04, 0xD9, 0x05, 0x0, 0x0, 0x0, 0x0 };
} SViewAngles;

static struct TGlobalVars {
	unsigned long	patternOffset	= EOffsets::GlobalVars_Pattern;
	unsigned long	AddrOffset		= EOffsets::GlobalVars_Address;

	const char*		mask			= "x????x????xxxxx";
	unsigned char	pattern[15]		= { 0x68, 0x0, 0x0, 0x0, 0x0, 0x68, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x50, 0x08, 0x85, 0xC0 };
} SGlobalVars;

static struct TForceLeft {
	unsigned long	patternOffset	= EOffsets::ForceLeft_Pattern;
	unsigned long	AddrOffset		= EOffsets::ForceLeft_Address;

	const char*		mask			= "xxxxxxxx";
	unsigned char	pattern[8]		= { 0x55, 0x8B, 0xEC, 0x51, 0x53, 0x8A, 0x5D, 0x08 };
} SForceLeft;

static struct TForceRight {
	unsigned long	patternOffset	= EOffsets::ForceRight_Pattern;
	unsigned long	AddrOffset		= EOffsets::ForceRight_Address;

	const char*		mask			= "xxxxxxxx";
	unsigned char	pattern[8]		= { 0x55, 0x8B, 0xEC, 0x51, 0x53, 0x8A, 0x5D, 0x08 };
} SForceRight;

static struct TClientStateState {
	unsigned long	patternOffset	= EOffsets::ClientStateState_Pattern;
	unsigned long	AddrOffset		= EOffsets::ClientStateState_Address;

	const char*		mask			= "xx?????xxxx";
	unsigned char	pattern[11]		= { 0x83, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0F, 0x94, 0xC0, 0xC3 };
} SClientStateState;

static struct TMouseEnable {
	unsigned long	patternOffset	= EOffsets::MouseEnable_Pattern;
	unsigned long	AddrOffset		= EOffsets::MouseEnable_Address;

	const char*		mask			= "x????xxxxxxx";
	unsigned char	pattern[12]		= { 0xB9, 0x0, 0x0, 0x0, 0x0, 0xFF, 0x50, 0x34, 0x85, 0xC0, 0x75, 0x10 };
} SMouseEnable;

static struct TInput {
	unsigned long	patternOffset	= EOffsets::Input_Pattern;
	unsigned long	AddrOffset		= EOffsets::Input_Address;

	const char*		mask			= "x????xxxxxxxx";
	unsigned char	pattern[13]		= { 0xB9, 0x0, 0x0, 0x0, 0x0, 0xF3, 0x0F, 0x11, 0x04, 0x24, 0xFF, 0x50, 0x10 };
} SInput;

static struct TPlayerInfo {
	unsigned long	patternOffset	= EOffsets::PlayerInfo_Pattern;
	unsigned long	AddrOffset		= EOffsets::PlayerInfo_Address;

	const char*		mask			= "xx????xxxx????xx";
	unsigned char	pattern[16]		= { 0x8B, 0x89, 0x0, 0x0, 0x0, 0x0, 0x85, 0xC9, 0x0F, 0x84, 0x0, 0x0, 0x0, 0x0, 0x8B, 0x1 };
} SPlayerInfo;

static struct TSensitivity {
	unsigned long	patternOffset	= EOffsets::Sensitivity_Pattern;
	unsigned long	AddrOffset		= EOffsets::Sensitivity_Address;

	const char*		mask			= "xx????xxxxxx????xxxxx?xxxxx????xxxxxx";
	unsigned char	pattern[37]		= { 0x81, 0xF9, 0x0, 0x0, 0x0, 0x0, 0x75, 0x1D, 0xF3, 0x0F, 0x10, 0x05, 0x0, 0x0, 0x0, 0x0,
										0xF3, 0x0F, 0x11, 0x44, 0x24, 0x0, 0x8B, 0x44, 0x24, 0x18, 0x35, 0x0, 0x0, 0x0, 0x0, 0x89, 0x44, 0x24, 0x0C, 0xEB, 0x0B
									  };
} SSensitivity;

static struct TSendPackets {
	unsigned long	patternOffset	= EOffsets::SendPackets_Pattern;
	unsigned long	AddrOffset		= EOffsets::SendPackets_Address;

	const char*		mask			= "xxxxxxxxxxxxxxx?????xx";
	unsigned char	pattern[22]		= { 0xB3, 0x01, 0x8B, 0x01, 0x8B, 0x40, 0x10, 0xFF, 0xD0, 0x84, 0xC0, 0x74, 0x0F, 0x80, 0xBF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0F, 0x84 };
} SSendPackets;

static struct TClientStateViewAngles {
	unsigned long	patternOffset	= EOffsets::ClientStateViewAngles_Pattern;
	unsigned long	AddrOffset		= EOffsets::ClientStateViewAngles_Address;

	const char*		mask			= "xxxx????xxxxx";
	unsigned char	pattern[13]		= { 0xF3, 0x0F, 0x11, 0x80, 0x0, 0x0, 0x0, 0x0, 0xD9, 0x46, 0x4, 0xD9, 0x5 };
} SClientStateViewAngles;

static struct TPlayerResource {
	unsigned long	patternOffset	= EOffsets::PlayerResource_Pattern;
	unsigned long	AddrOffset		= EOffsets::PlayerResource_Address;

	const char*		mask			= "xx????xxxx????xx";
	unsigned char	pattern[16]		= { 0x8B, 0x3D, 0x0, 0x0, 0x0, 0x0, 0x85, 0xFF, 0x0F, 0x84, 0x0, 0x0, 0x0, 0x0, 0x81, 0xC7 };
} SPlayerResource;

static struct TViewMatrix {
	unsigned long	patternOffset	= EOffsets::ViewMatrix_Pattern;
	unsigned long	AddrOffset		= EOffsets::ViewMatrix_Address;

	const char*		mask			= "xxx????xx????x";
	unsigned char	pattern[14]		= { 0x0F, 0x10, 0x05, 0x0, 0x0, 0x0, 0x0, 0x8D, 0x85, 0x0, 0x0, 0x0, 0x0, 0xB9 };
} SViewMatrix;


// Pattern Builders
static auto LocalPlayerPattern()
{
	return CMemoryPattern(SLocalPlayer.pattern, SLocalPlayer.mask, SLocalPlayer.patternOffset, SLocalPlayer.AddrOffset);
}

static auto EntityListPattern()
{
	return CMemoryPattern(SEntityList.pattern, SEntityList.mask, SEntityList.patternOffset, SEntityList.AddrOffset);
}

static auto GlowObjectManagerPattern()
{
	return CMemoryPattern(SGlowObjectManager.pattern, SGlowObjectManager.mask, SGlowObjectManager.patternOffset, SGlowObjectManager.AddrOffset);
}

static auto ForceAttackPattern()
{
	return CMemoryPattern(SForceAttack.pattern, SForceAttack.mask, SForceAttack.patternOffset, SForceAttack.AddrOffset);
}

static auto ForceJumpPattern()
{
	return CMemoryPattern(SForceJump.pattern, SForceJump.mask, SForceJump.patternOffset, SForceJump.AddrOffset);
}

static auto DormantPattern()
{
	return CMemoryPattern(SDormant.pattern, SDormant.mask, SDormant.patternOffset, SDormant.AddrOffset);
}

static auto ClientStatePattern()
{
	return CMemoryPattern(SClientState.pattern, SClientState.mask, SClientState.patternOffset, SClientState.AddrOffset);
}

static auto IngamePattern()
{
	return CMemoryPattern(SIngame.pattern, SIngame.mask, SIngame.patternOffset, SIngame.AddrOffset);
}

static auto ViewAnglesPattern()
{
	return CMemoryPattern(SViewAngles.pattern, SViewAngles.mask, SViewAngles.patternOffset, SViewAngles.AddrOffset);
}

static auto GlobalVarsPattern()
{
	return CMemoryPattern(SGlobalVars.pattern, SGlobalVars.mask, SGlobalVars.patternOffset, SGlobalVars.AddrOffset);
}

static auto ForceLeftPattern()
{
	return CMemoryPattern(SForceLeft.pattern, SForceLeft.mask, SForceLeft.patternOffset, SForceLeft.AddrOffset);
}

static auto ForceRightPattern()
{
	return CMemoryPattern(SForceRight.pattern, SForceRight.mask, SForceRight.patternOffset, SForceRight.AddrOffset);
}

static auto ClientStateStatePattern()
{
	return CMemoryPattern(SClientStateState.pattern, SClientStateState.mask, SClientStateState.patternOffset, SClientStateState.AddrOffset);
}

static auto MouseEnablePattern()
{
	return CMemoryPattern(SMouseEnable.pattern, SMouseEnable.mask, SMouseEnable.patternOffset, SMouseEnable.AddrOffset);
}

static auto InputPattern()
{
	return CMemoryPattern(SInput.pattern, SInput.mask, SInput.patternOffset, SInput.AddrOffset);
}

static auto PlayerInfoPattern()
{
	return CMemoryPattern(SPlayerInfo.pattern, SPlayerInfo.mask, SPlayerInfo.patternOffset, SPlayerInfo.AddrOffset);
}

static auto SensitivityPattern()
{
	return CMemoryPattern(SSensitivity.pattern, SSensitivity.mask, SSensitivity.patternOffset, SSensitivity.AddrOffset);
}

static auto SendPacketsPattern()
{
	return CMemoryPattern(SSendPackets.pattern, SSendPackets.mask, SSendPackets.patternOffset, SSendPackets.AddrOffset);
}

static auto ClientStateViewAnglesPattern()
{
	return CMemoryPattern(SClientStateViewAngles.pattern, SClientStateViewAngles.mask, SClientStateViewAngles.patternOffset, SClientStateViewAngles.AddrOffset);
}

static auto PlayerResourcePattern()
{
	return CMemoryPattern(SPlayerResource.pattern, SPlayerResource.mask, SPlayerResource.patternOffset, SPlayerResource.AddrOffset);
}

static auto ViewMatrixPattern()
{
	return CMemoryPattern(SViewMatrix.pattern, SViewMatrix.mask, SViewMatrix.patternOffset, SViewMatrix.AddrOffset);
}



#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)   
#define IN_SPEED					(1 << 17)	
#define IN_WALK						(1 << 18)	
#define IN_ZOOM						(1 << 19)	
#define IN_WEAPON1					(1 << 20)	
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)	
#define	IN_ATTACK3					(1 << 25)
#define	FL_ONGROUND					(1 << 0)	
#define FL_DUCKING					(1 << 1)	
#define	FL_WATERJUMP				(1 << 2)	
#define FL_ONTRAIN					(1 << 3)
#define FL_INRAIN					(1 << 4)	
#define FL_FROZEN					(1 << 5)
#define FL_ATCONTROLS				(1 << 6)
#define	FL_CLIENT					(1 << 7)	
#define FL_FAKECLIENT				(1 << 8)	
#define	FL_INWATER					(1 << 9)										
#define	FL_FLY						(1 << 10)	
#define	FL_SWIM						(1 << 11)
#define	FL_CONVEYOR					(1 << 12)
#define	FL_NPC						(1 << 13)
#define	FL_GODMODE					(1 << 14)
#define	FL_NOTARGET					(1 << 15)
#define	FL_AIMTARGET				(1 << 16)	
#define	FL_PARTIALGROUND			(1 << 17)	
#define FL_STATICPROP				(1 << 18)
#define FL_GRAPHED					(1 << 19) 
#define FL_GRENADE					(1 << 20)
#define FL_STEPMOVEMENT				(1 << 21)	
#define FL_DONTTOUCH				(1 << 22)
#define FL_BASEVELOCITY				(1 << 23)
#define FL_WORLDBRUSH				(1 << 24)	
#define FL_OBJECT					(1 << 25) 
#define FL_KILLME					(1 << 26)
#define FL_ONFIRE					(1 << 27)	
#define FL_DISSOLVING				(1 << 28)
#define FL_TRANSRAGDOLL				(1 << 29)
#define FL_UNBLOCKABLE_BY_PLAYER	(1 << 30) 
#define PLAYER_FLAG_BITS			10
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

// CSGO enums
static enum SignOnState : int {
	SIGNONSTATE_NONE = 0,
	SIGNONSTATE_CHALLENGE = 1,
	SIGNONSTATE_CONNECTED = 2,
	SIGNONSTATE_NEW = 3,
	SIGNONSTATE_PRESPAWN = 4,
	SIGNONSTATE_SPAWN = 5,
	SIGNONSTATE_FULL = 6,
	SIGNONSTATE_CHANGELEVEL = 7
};

static enum LifeState : unsigned char
{
	LIFE_ALIVE = 0,// alive
	LIFE_DYING = 1, // playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD = 2, // dead. lying still.
	MAX_LIFESTATE
};

static enum ClassId
{
	ClassId_CAI_BaseNPC = 0,
	ClassId_CAK47,
	ClassId_CBaseAnimating,
	ClassId_CBaseAnimatingOverlay,
	ClassId_CBaseAttributableItem,
	ClassId_CBaseButton,
	ClassId_CBaseCombatCharacter,
	ClassId_CBaseCombatWeapon,
	ClassId_CBaseCSGrenade,
	ClassId_CBaseCSGrenadeProjectile,
	ClassId_CBaseDoor,
	ClassId_CBaseEntity,
	ClassId_CBaseFlex,
	ClassId_CBaseGrenade,
	ClassId_CBaseParticleEntity,
	ClassId_CBasePlayer,
	ClassId_CBasePropDoor,
	ClassId_CBaseTeamObjectiveResource,
	ClassId_CBaseTempEntity,
	ClassId_CBaseToggle,
	ClassId_CBaseTrigger,
	ClassId_CBaseViewModel,
	ClassId_CBaseVPhysicsTrigger,
	ClassId_CBaseWeaponWorldModel,
	ClassId_CBeam,
	ClassId_CBeamSpotlight,
	ClassId_CBoneFollower,
	ClassId_CBreakableProp,
	ClassId_CBreakableSurface,
	ClassId_CC4,
	ClassId_CCascadeLight,
	ClassId_CChicken,
	ClassId_CColorCorrection,
	ClassId_CColorCorrectionVolume,
	ClassId_CCSGameRulesProxy,
	ClassId_CCSPlayer,
	ClassId_CCSPlayerResource,
	ClassId_CCSRagdoll,
	ClassId_CCSTeam,
	ClassId_CDEagle,
	ClassId_CDecoyGrenade,
	ClassId_CDecoyProjectile,
	ClassId_CDynamicLight,
	ClassId_CDynamicProp,
	ClassId_CEconEntity,
	ClassId_CEconWearable,
	ClassId_CEmbers,
	ClassId_CEntityDissolve,
	ClassId_CEntityFlame,
	ClassId_CEntityFreezing,
	ClassId_CEntityParticleTrail,
	ClassId_CEnvAmbientLight,
	ClassId_CEnvDetailController,
	ClassId_CEnvDOFController,
	ClassId_CEnvParticleScript,
	ClassId_CEnvProjectedTexture,
	ClassId_CEnvQuadraticBeam,
	ClassId_CEnvScreenEffect,
	ClassId_CEnvScreenOverlay,
	ClassId_CEnvTonemapController,
	ClassId_CEnvWind,
	ClassId_CFEPlayerDecal,
	ClassId_CFireCrackerBlast,
	ClassId_CFireSmoke,
	ClassId_CFireTrail,
	ClassId_CFish,
	ClassId_CFlashbang,
	ClassId_CFogController,
	ClassId_CFootstepControl,
	ClassId_CFunc_Dust,
	ClassId_CFunc_LOD,
	ClassId_CFuncAreaPortalWindow,
	ClassId_CFuncBrush,
	ClassId_CFuncConveyor,
	ClassId_CFuncLadder,
	ClassId_CFuncMonitor,
	ClassId_CFuncMoveLinear,
	ClassId_CFuncOccluder,
	ClassId_CFuncReflectiveGlass,
	ClassId_CFuncRotating,
	ClassId_CFuncSmokeVolume,
	ClassId_CFuncTrackTrain,
	ClassId_CGameRulesProxy,
	ClassId_CHandleTest,
	ClassId_CHEGrenade,
	ClassId_CHostage,
	ClassId_CHostageCarriableProp,
	ClassId_CIncendiaryGrenade,
	ClassId_CInferno,
	ClassId_CInfoLadderDismount,
	ClassId_CInfoOverlayAccessor,
	ClassId_CItem_Healthshot,
	ClassID_CItemDogTags,
	ClassId_CKnife,
	ClassId_CKnifeGG,
	ClassId_CLightGlow,
	ClassId_CMaterialModifyControl,
	ClassId_CMolotovGrenade,
	ClassId_CMolotovProjectile,
	ClassId_CMovieDisplay,
	ClassId_CParticleFire,
	ClassId_CParticlePerformanceMonitor,
	ClassId_CParticleSystem,
	ClassId_CPhysBox,
	ClassId_CPhysBoxMultiplayer,
	ClassId_CPhysicsProp,
	ClassId_CPhysicsPropMultiplayer,
	ClassId_CPhysMagnet,
	ClassId_CPlantedC4,
	ClassId_CPlasma,
	ClassId_CPlayerResource,
	ClassId_CPointCamera,
	ClassId_CPointCommentaryNode,
	ClassId_CPointWorldText,
	ClassId_CPoseController,
	ClassId_CPostProcessController,
	ClassId_CPrecipitation,
	ClassId_CPrecipitationBlocker,
	ClassId_CPredictedViewModel,
	ClassId_CProp_Hallucination,
	ClassId_CPropDoorRotating,
	ClassId_CPropJeep,
	ClassId_CPropVehicleDriveable,
	ClassId_CRagdollManager,
	ClassId_CRagdollProp,
	ClassId_CRagdollPropAttached,
	ClassId_CRopeKeyframe,
	ClassId_CSCAR17,
	ClassId_CSceneEntity,
	ClassId_CSensorGrenade,
	ClassId_CSensorGrenadeProjectile,
	ClassId_CShadowControl,
	ClassId_CSlideshowDisplay,
	ClassId_CSmokeGrenade,
	ClassId_CSmokeGrenadeProjectile,
	ClassId_CSmokeStack,
	ClassId_CSpatialEntity,
	ClassId_CSpotlightEnd,
	ClassId_CSprite,
	ClassId_CSpriteOriented,
	ClassId_CSpriteTrail,
	ClassId_CStatueProp,
	ClassId_CSteamJet,
	ClassId_CSun,
	ClassId_CSunlightShadowControl,
	ClassId_CTeam,
	ClassId_CTeamplayRoundBasedRulesProxy,
	ClassId_CTEArmorRicochet,
	ClassId_CTEBaseBeam,
	ClassId_CTEBeamEntPoint,
	ClassId_CTEBeamEnts,
	ClassId_CTEBeamFollow,
	ClassId_CTEBeamLaser,
	ClassId_CTEBeamPoints,
	ClassId_CTEBeamRing,
	ClassId_CTEBeamRingPoint,
	ClassId_CTEBeamSpline,
	ClassId_CTEBloodSprite,
	ClassId_CTEBloodStream,
	ClassId_CTEBreakModel,
	ClassId_CTEBSPDecal,
	ClassId_CTEBubbles,
	ClassId_CTEBubbleTrail,
	ClassId_CTEClientProjectile,
	ClassId_CTEDecal,
	ClassId_CTEDust,
	ClassId_CTEDynamicLight,
	ClassId_CTEEffectDispatch,
	ClassId_CTEEnergySplash,
	ClassId_CTEExplosion,
	ClassId_CTEFireBullets,
	ClassId_CTEFizz,
	ClassId_CTEFootprintDecal,
	ClassId_CTEFoundryHelpers,
	ClassId_CTEGaussExplosion,
	ClassId_CTEGlowSprite,
	ClassId_CTEImpact,
	ClassId_CTEKillPlayerAttachments,
	ClassId_CTELargeFunnel,
	ClassId_CTEMetalSparks,
	ClassId_CTEMuzzleFlash,
	ClassId_CTEParticleSystem,
	ClassId_CTEPhysicsProp,
	ClassId_CTEPlantBomb,
	ClassId_CTEPlayerAnimEvent,
	ClassId_CTEPlayerDecal,
	ClassId_CTEProjectedDecal,
	ClassId_CTERadioIcon,
	ClassId_CTEShatterSurface,
	ClassId_CTEShowLine,
	ClassId_CTesla,
	ClassId_CTESmoke,
	ClassId_CTESparks,
	ClassId_CTESprite,
	ClassId_CTESpriteSpray,
	ClassId_CTest_ProxyToggle_Networkable,
	ClassId_CTestTraceline,
	ClassId_CTEWorldDecal,
	ClassId_CTriggerPlayerMovement,
	ClassId_CTriggerSoundOperator,
	ClassId_CVGuiScreen,
	ClassId_CVoteController,
	ClassId_CWaterBullet,
	ClassId_CWaterLODControl,
	ClassId_CWeaponAug,
	ClassId_CWeaponAWP,
	ClassId_CWeaponBaseItem,
	ClassId_CWeaponBizon,
	ClassId_CWeaponCSBase,
	ClassId_CWeaponCSBaseGun,
	ClassId_CWeaponCycler,
	ClassId_CWeaponElite,
	ClassId_CWeaponFamas,
	ClassId_CWeaponFiveSeven,
	ClassId_CWeaponG3SG1,
	ClassId_CWeaponGalil,
	ClassId_CWeaponGalilAR,
	ClassId_CWeaponGlock,
	ClassId_CWeaponHKP2000,
	ClassId_CWeaponM249,
	ClassId_CWeaponM3,
	ClassId_CWeaponM4A1,
	ClassId_CWeaponMAC10,
	ClassId_CWeaponMag7,
	ClassId_CWeaponMP5Navy,
	ClassId_CWeaponMP7,
	ClassId_CWeaponMP9,
	ClassId_CWeaponNegev,
	ClassId_CWeaponNOVA,
	ClassId_CWeaponP228,
	ClassId_CWeaponP250,
	ClassId_CWeaponP90,
	ClassId_CWeaponSawedoff,
	ClassId_CWeaponSCAR20,
	ClassId_CWeaponScout,
	ClassId_CWeaponSG550,
	ClassId_CWeaponSG552,
	ClassId_CWeaponSG556,
	ClassId_CWeaponSSG08,
	ClassId_CWeaponTaser,
	ClassId_CWeaponTec9,
	ClassId_CWeaponTMP,
	ClassId_CWeaponUMP45,
	ClassId_CWeaponUSP,
	ClassId_CWeaponXM1014,
	ClassId_CWorld,
	ClassId_DustTrail,
	ClassId_MovieExplosion,
	ClassId_ParticleSmokeGrenade,
	ClassId_RocketTrail,
	ClassId_SmokeTrail,
	ClassId_SporeExplosion,
	ClassId_SporeTrail,
	MAX_CLASSID
};

static enum ItemDefinitionIndex
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

static enum HitboxList
{
	HITBOX_NONE,
	HITBOX_PELVIS,
	HITBOX_L_THIGH,
	HITBOX_L_CALF,
	HITBOX_L_FOOT,
	HITBOX_R_THIGH,
	HITBOX_R_CALF,
	HITBOX_R_FOOT,
	HITBOX_SPINE,
	HITBOX_SPINE1,
	HITBOX_SPINE2,
	HITBOX_SPINE3,
	HITBOX_NECK,
	HITBOX_HEAD,
	HITBOX_L_UPPERARM,
	HITBOX_L_FOREARM,
	HITBOX_L_HAND,
	HITBOX_R_UPPERARM,
	HITBOX_R_FOREARM,
	HITBOX_R_HAND,
	HITBOX_L_CLAVICLE,
	HITBOX_R_CLAVICLE,
	HITBOX_HELMET,
	HITBOX_SPINE4,
	HITBOX_MAX,
};

static enum CSPlayerBones
{
	BONE_PELVIS,
	BONE_SPINE1,
	BONE_SPINE2,
	BONE_SPINE3,
	BONE_SPINE4,
	BONE_NECK,
	BONE_L_CLAVICLE,
	BONE_L_UPPER_ARM,
	BONE_L_FOREARM,
	BONE_L_HAND,
	BONE_HEAD,
	BONE_FORWARD,
	BONE_R_CLAVICLE,
	BONE_R_UPPER_ARM,
	BONE_R_FOREARM,
	BONE_R_HAND,
	BONE_WEAPON,
	BONE_WEAPON_SLIDE,
	BONE_WEAPON_R_HAND,
	BONE_WEAPON_L_HAND,
	BONE_WEAPON_CLIP1,
	BONE_WEAPON_CLIP2,
	BONE_SILENCER,
	BONE_R_THIGH,
	BONE_R_CALF,
	BONE_R_FOOT,
	BONE_L_THIGH,
	BONE_L_CALF,
	BONE_L_FOOT,
	BONE_L_WEAPON_HAND,
	BONE_R_WEAPON_HAND,
	BONE_L_FORETWIST,
	BONE_L_CALFTWIST,
	BONE_R_CALFTWIST,
	BONE_L_THIGHTWIST,
	BONE_R_THIGHTWIST,
	BONE_L_UPARMTWIST,
	BONE_R_UPARMTWIST,
	BONE_R_FORETWIST,
	BONE_R_TOE,
	BONE_L_TOE,
	BONE_R_FINGER01,
	BONE_R_FINGER02,
	BONE_R_FINGER03,
	BONE_R_FINGER04,
	BONE_R_FINGER05,
	BONE_R_FINGER06,
	BONE_R_FINGER07,
	BONE_R_FINGER08,
	BONE_R_FINGER09,
	BONE_R_FINGER10,
	BONE_R_FINGER11,
	BONE_R_FINGER12,
	BONE_R_FINGER13,
	BONE_R_FINGER14,
	BONE_L_FINGER01,
	BONE_L_FINGER02,
	BONE_L_FINGER03,
	BONE_L_FINGER04,
	BONE_L_FINGER05,
	BONE_L_FINGER06,
	BONE_L_FINGER07,
	BONE_L_FINGER08,
	BONE_L_FINGER09,
	BONE_L_FINGER10,
	BONE_L_FINGER11,
	BONE_L_FINGER12,
	BONE_L_FINGER13,
	BONE_L_FINGER14,
	BONE_L_FINGER15,
	BONE_R_FINGER15,
	BONE_MAX
};

