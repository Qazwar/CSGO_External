#pragma once
#include "HaxConfigParser.h"

#ifdef GetProp
	#undef GetProp
#endif

enum EGameCodes
{
	GAMECODE_NULL,

	GAMECODE_CSGO,
	GAMECODE_METIN2,

	GAMECODE_MAX
};

class CCSGOData;
extern const std::size_t g_MaxAttempt;

class CHaxData : public std::enable_shared_from_this<CHaxData>
{
	public:
		CHaxData(DWORD dwTargetPID, DWORD dwGameCode);
		~CHaxData();

		// Classes
		auto			GetCSGODataInstance() { return m_cCSGOData; };
		auto			GetSettingsInstance() { return m_cSettings; };

		// Settings
		bool			ReloadSettings();

		int				GetInteger(const std::string & szName);
		DWORD			GetUnsignedLong(const std::string & szName);
		double			GetDouble(const std::string & szName);
		std::string		GetString(const std::string & szName);
		bool			GetBoolean(const std::string & szName);
		std::vector <std::string>				GetVector(const std::string & szName);
		std::map	<std::string, std::string>	GetMap(const std::string & szName);

		bool			ParsePreDefinedPointers();

		bool			ParseCSGOGlowESPSettings();
		bool			ParseCSGOBunnyHopSettings();
		bool			ParseCSGOTriggerBotSettings();
		bool			ParseCSGONoRecoilSettings();
		bool			ParseCSGOMiscSettings();
		bool			ParseCSGOAimBotSettings();

	private:
		// Common
		DWORD						m_dwTargetPID;
		DWORD						m_dwGameCode;

		// Game specific classes
		std::shared_ptr <CConfigParser>	m_cSettings;
		std::shared_ptr <CCSGOData>		m_cCSGOData;
};

class CCSGOData
{
	public:
		CCSGOData(DWORD dwTargetPID);
		~CCSGOData();

	// Netvar manager
	public:
		void		DumpNetvars();
		int			GetNetVar(const char *tableName, const char *propName);

	protected:
		int			GetProp(const char *tableName, const char *propName, RecvProp **prop = 0);
		int			GetProp(RecvTable *recvTable, const char *propName, RecvProp **prop = 0);

		RecvTable * GetTable(const char *tableName);
		void		DumpTable(RecvTable *table);

	private:
		std::vector <RecvTable*>    m_NetvarTables;

	// Builders
	public:
		bool		LoadRemoteModulesAndFunctions();
		bool		WaitForModuleInitilization(DWORD dwMSDelay);
		bool		FindPointers(bool bSaveToConfig);
		bool		FindNetvars();

		// Common
		std::string		CreateVersionHash();

		// Pointers
		auto		GetLocalPlayer_Pointer()			{ return m_dwLocalPlayer;				};
		auto		GetEntityList_Pointer()				{ return m_dwEntityList;				};
		auto		GetGlowObjectManager_Pointer()		{ return m_dwGlowObject;				};
		auto		GetForceAttack_Pointer()			{ return m_dwForceAttack;				};
		auto		GetForceJump_Pointer()				{ return m_dwForceJump;					};
		auto		GetDormant_Pointer()				{ return m_bDormant;					};
		auto		GetClientState_Pointer()			{ return m_dwClientState;				};
		auto		GetIngame_Pointer()					{ return m_dwInGame;					};
		auto		GetViewAngles_Pointer()				{ return m_dwViewAngles;				};
		auto		GetGlobalVars_Pointer()				{ return m_dwGlobalVars;				};
		auto		GetForceLeft_Pointer()				{ return m_dwForceLeft;					};
		auto		GetForceRight_Pointer()				{ return m_dwForceRight;				};
		auto		GetClientStateState_Pointer()		{ return m_dwClientState_State;			};
		auto		GetMouseEnable_Pointer()			{ return m_dwMouseEnable;				};
		auto		GetInput_Pointer()					{ return m_dwInput;						};
		auto		GetPlayerInfo_Pointer()				{ return m_dwPlayerInfo;				};
		auto		GetSensitivity_Pointer()			{ return m_dwSensitivity;				};
		auto		GetSendPackets_Pointer()			{ return m_dwSendPackets;				};
		auto		GetClientStateViewAngles_Pointer()	{ return m_dwClientState_ViewAngles;	};
		auto		GetPlayerResource_Pointer()			{ return m_dwPlayerResource;			};
		auto		GetViewMatrix_Pointer()				{ return m_dwViewMatrix;				};
		
		auto		SetLocalPlayer_Pointer(DWORD dwNewValue)			{ m_dwLocalPlayer = dwNewValue;				};
		auto		SetEntityList_Pointer(DWORD dwNewValue)				{ m_dwEntityList = dwNewValue;				};
		auto		SetGlowObjectManager_Pointer(DWORD dwNewValue)		{ m_dwGlowObject = dwNewValue;				};
		auto		SetForceAttack_Pointer(DWORD dwNewValue)			{ m_dwForceAttack = dwNewValue;				};
		auto		SetForceJump_Pointer(DWORD dwNewValue)				{ m_dwForceJump = dwNewValue;				};
		auto		SetDormant_Pointer(DWORD dwNewValue)				{ m_bDormant = dwNewValue;					};
		auto		SetClientState_Pointer(DWORD dwNewValue)			{ m_dwClientState = dwNewValue;				};
		auto		SetIngame_Pointer(DWORD dwNewValue)					{ m_dwInGame = dwNewValue;					};
		auto		SetViewAngles_Pointer(DWORD dwNewValue)				{ m_dwViewAngles = dwNewValue;				};
		auto		SetGlobalVars_Pointer(DWORD dwNewValue)				{ m_dwGlobalVars = dwNewValue;				};
		auto		SetForceLeft_Pointer(DWORD dwNewValue)				{ m_dwForceLeft = dwNewValue;				};
		auto		SetForceRight_Pointer(DWORD dwNewValue)				{ m_dwForceRight = dwNewValue;				};
		auto		SetClientStateState_Pointer(DWORD dwNewValue)		{ m_dwClientState_State = dwNewValue;		};
		auto		SetMouseEnable_Pointer(DWORD dwNewValue)			{ m_dwMouseEnable = dwNewValue;				};
		auto		SetInput_Pointer(DWORD dwNewValue)					{ m_dwInput = dwNewValue;					};
		auto		SetPlayerInfo_Pointer(DWORD dwNewValue)				{ m_dwPlayerInfo = dwNewValue;				};
		auto		SetSensitivity_Pointer(DWORD dwNewValue)			{ m_dwSensitivity = dwNewValue;				};
		auto		SetSendPackets_Pointer(DWORD dwNewValue)			{ m_dwSendPackets = dwNewValue;				};
		auto		SetClientStateViewAngles_Pointer(DWORD dwNewValue)	{ m_dwClientState_ViewAngles = dwNewValue;	};
		auto		SetPlayerResource_Pointer(DWORD dwNewValue)			{ m_dwPlayerResource = dwNewValue;			};
		auto		SetViewMatrix_Pointer(DWORD dwNewValue)				{ m_dwViewMatrix = dwNewValue;				};

		// Netvars
		auto		GetCrossHairID_Netvar()			{ return m_iCrossHairID;		};
		auto		GetTeamNum_Netvar()				{ return m_iTeamNum;			};
		auto		GetFlags_Netvar()				{ return m_fFlags;				};
		auto		GetGlowIndex_Netvar()			{ return m_iGlowIndex;			};
		auto		GetSpotted_Netvar()				{ return m_bSpotted;			};
		auto		GetFlashMaxAlpha_Netvar()		{ return m_flFlashMaxAlpha;		};
		auto		GetLifeState_Netvar()			{ return m_lifeState;			};
		auto		GetHealth_Netvar()				{ return m_iHealth;				};
		auto		GetAimPunch_Netvar()			{ return m_aimPunch;			};
		auto		GetShotsFired_Netvar()			{ return m_iShotsFired;			};
		auto		GetHasDefuser_Netvar()			{ return m_bHasDefuser;			};
		auto		GetC4Blow_Netvar()				{ return m_flC4Blow;			};
		auto		GetSpottedByMask_Netvar()		{ return m_bSpottedByMask;		};
		auto		GetModelIndex_Netvar()			{ return m_nModelIndex;			};
		auto		GetActiveWeapon_Netvar()		{ return m_hActiveWeapon;		};
		auto		GetItem_NetVar()				{ return m_Item;				};
		auto		GetItemDefinitionIndex_NetVar() { return m_iItemDefinitionIndex;};
		auto		GetAttributeManager_NetVar()	{ return m_AttributeManager;	};
		auto		GetIsScoped_NetVar()			{ return m_bIsScoped;			};
		auto		GetWorldModelIndex_NetVar()		{ return m_iWorldModelIndex;	};
		auto		GetZoomLevel_NetVar()			{ return m_zoomLevel;			};
		auto		GetLocal_NetVar()				{ return m_Local;				};
		auto		GetViewPunchAngle_NetVar()		{ return m_viewPunchAngle;		};
		auto		GetClip1_NetVar()				{ return m_iClip1;				};
		auto		GetNextPrimaryAttack_NetVar()	{ return m_flNextPrimaryAttack;	};
		auto		GetTickBase_Netvar()			{ return m_nTickBase;			};
		auto		GetVelocity_Netvar()			{ return m_vecVelocity;			};
		auto		GetGunGameImmunity_Netvar()		{ return m_bGunGameImmunity;	};
		auto		GetVecOrigin_Netvar()			{ return m_vecOrigin;			};
		auto		GetPlayerC4_Netvar()			{ return m_iPlayerC4;			};
		auto		GetViewOffset_Netvar()			{ return m_vecViewOffset;		};
		auto		GetBoneMatrix_Netvar()			{ return m_dwBoneMatrix;		};
		auto		GetClrRender_Netvar()			{ return m_clrRender;			};
		
		
		// Settings
		/// Glow ESP
		auto		SetGlowESPIsEnabled(bool bValue)			{ m_bEnableGlow = bValue;				};
		auto		GetGlowESPIsEnabled()						{ return m_bEnableGlow;					};

		auto		SetGlowESPEnableKey(DWORD dwValue)			{ m_dwGlowESPEnableKey = dwValue;		};
		auto		GetGlowESPEnableKey()						{ return m_dwGlowESPEnableKey;			};

		auto		SetGlowESPUpdateInterval(DWORD dwValue)		{ m_dwGlowESPUpdateInterval = dwValue;	};
		auto		GetGlowESPUpdateInterval()					{ return m_dwGlowESPUpdateInterval;		};

		auto		SetGlowESPEnemyOnly(bool bValue)			{ m_bIsGlowESPEnemyOnly = bValue;		};
		auto		GetGlowESPEnemyOnly()						{ return m_bIsGlowESPEnemyOnly;			};

		auto		SetGlowESPVisibleOnly(bool bValue)			{ m_bIsGlowESPVisibleOnly = bValue;		};
		auto		GetGlowESPVisibleOnly()						{ return m_bIsGlowESPVisibleOnly;		};

		auto		SetGlowESPEnemyColor(DWORD dwValue)			{ m_dwGlowESPEnemyColor = dwValue;		};
		auto		GetGlowESPEnemyColor()						{ return m_dwGlowESPEnemyColor;			};

		auto		SetGlowESPFriendColor(DWORD dwValue)		{ m_dwGlowESPFriendColor = dwValue;		};
		auto		GetGlowESPFriendColor()						{ return m_dwGlowESPFriendColor;		};

		auto		SetGlowESPC4Color(std::map <std::string, std::string> szValues)		{ m_mapGlowESPC4Color = szValues;		};
		auto		GetGlowESPC4Color()													{ return m_mapGlowESPC4Color;			};

		auto		SetGlowESPC4HostColor(std::map <std::string, std::string> szValues)	{ m_mapGlowESPC4HostColor = szValues;	};
		auto		GetGlowESPC4HostColor()												{ return m_mapGlowESPC4HostColor;		};

		auto		SetGlowESPChickenColor(std::map <std::string, std::string> szValues){ m_mapGlowESPChickenColor = szValues;	};
		auto		GetGlowESPChickenColor()											{ return m_mapGlowESPChickenColor;		};

		auto		SetGlowESPChamsIsEnabled(bool bValue)		{ m_bIsGlowESPChamsIsEnabled = bValue;	};
		auto		GetGlowESPChamsIsEnabled()					{ return m_bIsGlowESPChamsIsEnabled;	};

		/// Bunny Hop
		auto		SetBunnyHopJumpKey(DWORD dwValue)			{ m_dwBunnyHopJumpKey = dwValue;		};
		auto		GetBunnyHopJumpKey()						{ return m_dwBunnyHopJumpKey;			};

		auto		SetBunnyHopToggleKey(DWORD dwValue)			{ m_dwBunnyHopToggleKey = dwValue;		};
		auto		GetBunnyHopToggleKey()						{ return m_dwBunnyHopToggleKey;			};

		auto		SetBunnyHopReleaseInterval(DWORD dwValue)	{ m_dwBunnyHopReleaseInterval = dwValue;};
		auto		GetBunnyHopReleaseInterval()				{ return m_dwBunnyHopReleaseInterval;	};

		auto		SetBunnyHopUseAutoStrafe(bool bValue)		{ m_bBunnyHopUseAutoStrafe = bValue;	};
		auto		GetBunnyHopUseAutoStrafe()					{ return m_bBunnyHopUseAutoStrafe;		};

		/// Trigger bot
		auto		SetTriggerBotWorkType(DWORD dwValue)			{ m_dwTriggerBotWorkType = dwValue;				};
		auto		GetTriggerBotWorkType()							{ return m_dwTriggerBotWorkType;				};

		auto		SetTriggerBotEnableKey(DWORD dwValue)			{ m_dwTriggerBotEnableKey = dwValue;			};
		auto		GetTriggerBotEnableKey()						{ return m_dwTriggerBotEnableKey;				};

		auto		SetTriggerBotInterval(DWORD dwValue)			{ m_dwTriggerBotInterval = dwValue;				};
		auto		GetTriggerBotInterval()							{ return m_dwTriggerBotInterval;				};

		auto		SetTriggerBotIsBurst(bool bValue)				{ m_bTriggerBotIsBurst = bValue;				};
		auto		GetTriggerBotIsBurst()							{ return m_bTriggerBotIsBurst;					};

		auto		SetTriggerBotBurstInterval(DWORD dwValue)		{ m_dwTriggerBotBurstInterval = dwValue;		};
		auto		GetTriggerBotBurstInterval()					{ return m_dwTriggerBotBurstInterval;			};

		auto		SetTriggerBotBurstCount(DWORD dwValue)			{ m_dwTriggerBotBurstCount = dwValue;			};
		auto		GetTriggerBotBurstCount()						{ return m_dwTriggerBotBurstCount;				};

		auto		SetTriggerBotIsAttackAsLongAsAlive(bool bValue)	{ m_bTriggerBotIsAttackAsLongAsAlive = bValue;	};
		auto		GetTriggerBotIsAttackAsLongAsAlive()			{ return m_bTriggerBotIsAttackAsLongAsAlive;	};

		auto		SetTriggerBotCheckSpawnProtect(bool bValue)		{ m_bTriggerBotCheckSpawnProtect = bValue;		};
		auto		GetTriggerBotCheckSpawnProtect()				{ return m_bTriggerBotCheckSpawnProtect;		};

		auto		SetTriggerBotAimLock(bool bValue)				{ m_bTriggerBotAimLock = bValue;				};
		auto		GetTriggerBotAimLock()							{ return m_bTriggerBotAimLock;					};
		
		auto		SetTriggerBotAutoCrouch(bool bValue)			{ m_bTriggerBotAutoCrouch = bValue;				};
		auto		GetTriggerBotAutoCrouch()						{ return m_bTriggerBotAutoCrouch;				};
		
		auto		SetTriggerBotBlockOnJump(bool bValue)			{ m_bTriggerBotBlockOnJump = bValue;			};
		auto		GetTriggerBotBlockOnJump()						{ return m_bTriggerBotBlockOnJump;				};
		
		auto		SetTriggerBotBlockOnMove(bool bValue)			{ m_bTriggerBotBlockOnMove = bValue;			};
		auto		GetTriggerBotBlockOnMove()						{ return m_bTriggerBotBlockOnMove;				};

		auto		SetTriggerBotTargetBone(DWORD dwValue)			{ m_dwTriggerBotTargetBone = dwValue;			};
		auto		GetTriggerBotTargetBone()						{ return m_dwTriggerBotTargetBone;				};

		auto		SetTriggerBotFOV(double dValue)					{ m_dTriggerBotFov = dValue;					};
		auto		GetTriggerBotFOV()								{ return m_dTriggerBotFov;						};

		auto		SetTriggerBotTraceRay(bool bValue)				{ m_bTriggerBotTraceRay = bValue;				};
		auto		GetTriggerBotTraceRay()							{ return m_bTriggerBotTraceRay;					};
		
		auto		SetTriggerBotSmooth(std::vector <std::string> szValues)		{ m_vTriggerBotSmooth = szValues;	};
		auto		GetTriggerBotSmooth()										{ return m_vTriggerBotSmooth;		};

		auto		SetContinuousAttackIntervals(std::map <std::string, std::string> szValues)		{ m_mapContinuousAttackIntervals = szValues;	};
		auto		GetContinuousAttackIntervals()													{ return m_mapContinuousAttackIntervals;		};

		auto		SetSniperModeValues(std::map <std::string, std::string> szValues)				{ m_mapSniperModeValues = szValues;				};
		auto		GetSniperModeValues()															{ return m_mapSniperModeValues;					};

		auto		SetHoldKeyValues(std::map <std::string, std::string> szValues)					{ m_mapHoldKeyValues = szValues;				};
		auto		GetHoldKeyValues()																{ return m_mapHoldKeyValues;					};


		/// No recoil
		auto		SetNoRecoilEnableKey(DWORD dwValue)			{ m_dwNoRecoilEnableKey = dwValue;		};
		auto		GetNoRecoilEnableKey()						{ return m_dwNoRecoilEnableKey;			};

		auto		SetNoRecoilEfficiency(double dValue)		{ m_dNoRecoilEfficiency = dValue;		};
		auto		GetNoRecoilEfficiency()						{ return m_dNoRecoilEfficiency;			};

		/// Misc
		auto		SetMiscEnableKey(DWORD dwValue)				{ m_dwMiscEnableKey = dwValue;			};
		auto		GetMiscEnableKey()							{ return m_dwMiscEnableKey;				};

		auto		SetNoFlashEnabled(bool bValue)				{ m_bNoFlashIsEnabled = bValue;			};
		auto		GetNoFlashEnabled()							{ return m_bNoFlashIsEnabled;			};

		auto		SetNoHandsEnabled(bool bValue)				{ m_bNoHandsIsEnabled = bValue;			};
		auto		GetNoHandsEnabled()							{ return m_bNoHandsIsEnabled;			};

		auto		SetAutoPistolEnabled(bool bValue)			{ m_bAutoPistolIsEnabled = bValue;		};
		auto		GetAutoPistolEnabled()						{ return m_bAutoPistolIsEnabled;		};

		auto		SetChangeFov(bool bValue)					{ m_bChangeFov = bValue;				};
		auto		GetChangeFov()								{ return m_bChangeFov;					};

		auto		SetFovValue(int iValue)						{ m_iFovValue = iValue;					};
		auto		GetFovValue()								{ return m_iFovValue;					};

		auto		SetFovKey(DWORD dwValue)					{ m_dwFovKey = dwValue;					};
		auto		GetFovKey()									{ return m_dwFovKey;					};

		auto		SetSlowAimEnabled(bool bValue)				{ m_bSlowAimIsEnabled = bValue;			};
		auto		GetSlowAimEnabled()							{ return m_bSlowAimIsEnabled;			};

		auto		SetSlowAimSensivity(double dValue)			{ m_dSlowAimSensivity = dValue;			};
		auto		GetSlowAimSensivity()						{ return m_dSlowAimSensivity;			};

		auto		SetRadarEnabled(bool bValue)				{ m_bRadarIsEnabled = bValue;			};
		auto		GetRadarEnabled()							{ return m_bRadarIsEnabled;				};

		auto		SetFakeLagEnabled(bool bValue)				{ m_bFakeLagIsEnabled = bValue;			};
		auto		GetFakeLagEnabled()							{ return m_bFakeLagIsEnabled;			};

		auto		SetFakeLagKey(DWORD dwValue)				{ m_dwFakeLagKey = dwValue;				};
		auto		GetFakeLagKey()								{ return m_dwFakeLagKey;				};
		
		auto		SetAimBotEnableKey(DWORD dwValue)			{ m_dwAimBotKey = dwValue;				};
		auto		GetAimBotEnableKey()						{ return m_dwAimBotKey;					};

	protected:
		void				SavePointerToConfig(std::shared_ptr <CConfigParser> cSettings, bool bSaveToConfig, const std::string & szName, DWORD dwValue);

		HMODULE				LoadRemoteModule(const std::string & szModuleName);
		bool				LoadRemoteModules();

		bool				IsEngineLoaded();
		bool				LoadEngineFunctions();

	private:
		// Common
		DWORD				m_dwTargetPID;

		// Pointers, Offsets
		DWORD				m_dwLocalPlayer;
		DWORD				m_dwEntityList;
		DWORD				m_dwGlowObject;
		DWORD				m_dwForceAttack;
		DWORD				m_dwForceJump;
		DWORD				m_bDormant;
		DWORD				m_dwClientState;
		DWORD				m_dwInGame;
		DWORD				m_dwViewAngles;
		DWORD				m_dwGlobalVars;
		DWORD				m_dwForceLeft;
		DWORD				m_dwForceRight;
		DWORD				m_dwClientState_State;
		DWORD				m_dwMouseEnable;
		DWORD				m_dwInput;
		DWORD				m_dwPlayerInfo;
		DWORD				m_dwSensitivity;
		DWORD				m_dwSendPackets;
		DWORD				m_dwClientState_ViewAngles;
		DWORD				m_dwPlayerResource;
		DWORD				m_dwViewMatrix;

		// Netvars
		DWORD				m_iCrossHairID;
		DWORD				m_iTeamNum;
		DWORD				m_fFlags;
		DWORD				m_iGlowIndex;
		DWORD				m_bSpotted;
		DWORD				m_flFlashMaxAlpha;
		DWORD				m_lifeState;
		DWORD				m_iHealth;
		DWORD				m_aimPunch;
		DWORD				m_iShotsFired;
		DWORD				m_bHasDefuser;
		DWORD				m_flC4Blow;
		DWORD				m_bSpottedByMask;
		DWORD				m_nModelIndex;
		DWORD				m_hActiveWeapon;
		DWORD				m_Item;
		DWORD				m_iItemDefinitionIndex;
		DWORD				m_AttributeManager;
		DWORD				m_bIsScoped;
		DWORD				m_iWorldModelIndex;
		DWORD				m_zoomLevel;
		DWORD				m_Local;
		DWORD				m_viewPunchAngle;
		DWORD				m_iClip1;
		DWORD				m_flNextPrimaryAttack;
		DWORD				m_nTickBase;
		DWORD				m_vecVelocity;
		DWORD				m_bGunGameImmunity;
		DWORD				m_vecOrigin;
		DWORD				m_iPlayerC4;
		DWORD				m_vecViewOffset;
		DWORD				m_dwBoneMatrix;
		DWORD				m_clrRender;

		// Modules
		HMODULE				m_hSteamNetworkingSockets;
		HMODULE				m_hSteamApi;
		HMODULE				m_hTier0;
		HMODULE				m_hVstdlib;
		HMODULE				m_hPhonon3d;
		HMODULE				m_hClient;
		HMODULE				m_hEngine;

		// Exported module functions
		CHLClient		*	m_pClient;
		void			*	m_pEntList;
		CEngineClient	*	m_pEngine;

		// Settings
		bool				m_bEnableGlow;
		DWORD				m_dwGlowESPEnableKey;
		DWORD				m_dwGlowESPUpdateInterval;
		bool				m_bIsGlowESPEnemyOnly;
		bool				m_bIsGlowESPVisibleOnly;
		DWORD				m_dwGlowESPEnemyColor;
		DWORD				m_dwGlowESPFriendColor;
		bool				m_bIsGlowESPChamsIsEnabled;
		std::map <std::string, std::string> m_mapGlowESPC4Color;
		std::map <std::string, std::string> m_mapGlowESPC4HostColor;
		std::map <std::string, std::string> m_mapGlowESPChickenColor;

		DWORD				m_dwBunnyHopJumpKey;
		DWORD				m_dwBunnyHopToggleKey;
		DWORD				m_dwBunnyHopReleaseInterval;
		bool				m_bBunnyHopUseAutoStrafe;

		DWORD				m_dwTriggerBotWorkType;
		DWORD				m_dwTriggerBotEnableKey;
		DWORD				m_dwTriggerBotInterval;
		bool				m_bTriggerBotIsBurst;
		DWORD				m_dwTriggerBotBurstInterval;
		DWORD				m_dwTriggerBotBurstCount;
		bool				m_bTriggerBotIsAttackAsLongAsAlive;
		bool				m_bTriggerBotCheckSpawnProtect;
		bool				m_bTriggerBotAimLock;
		bool				m_bTriggerBotAutoCrouch;
		bool				m_bTriggerBotBlockOnJump;
		bool				m_bTriggerBotBlockOnMove;
		DWORD				m_dwTriggerBotTargetBone;
		double				m_dTriggerBotFov;
		float				m_bTriggerBotTraceRay;
		std::vector <std::string>			m_vTriggerBotSmooth;
		std::map <std::string, std::string> m_mapContinuousAttackIntervals;
		std::map <std::string, std::string>	m_mapSniperModeValues;
		std::map <std::string, std::string>	m_mapHoldKeyValues;

		DWORD				m_dwNoRecoilEnableKey;
		double				m_dNoRecoilEfficiency;

		DWORD				m_dwMiscEnableKey;
		bool				m_bNoFlashIsEnabled;
		bool				m_bNoHandsIsEnabled;
		bool				m_bAutoPistolIsEnabled;
		bool				m_bChangeFov;
		int					m_iFovValue;
		DWORD				m_dwFovKey;
		bool				m_bSlowAimIsEnabled;
		double				m_dSlowAimSensivity;
		bool				m_bRadarIsEnabled;
		bool				m_bFakeLagIsEnabled;
		DWORD				m_dwFakeLagKey;
		DWORD				m_dwAimBotKey;
};
