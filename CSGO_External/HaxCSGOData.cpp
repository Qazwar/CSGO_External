#include "main.h"
#include "Common.h"
#include "HaxData.h"
#include "FastMemoryHelper.h"
#include "PatternScan.h"
#include "Defines.h"

CCSGOData::CCSGOData(DWORD dwTargetPID) :
	m_dwTargetPID(dwTargetPID),
	m_dwLocalPlayer(0), m_dwEntityList(0), m_dwGlowObject(0), m_dwForceAttack(0), m_dwForceJump(0), m_bDormant(0), m_dwClientState(0), m_dwInGame(0), m_dwViewAngles(0), m_dwGlobalVars(0), m_dwForceLeft(0), m_dwForceRight(0), m_dwClientState_State(0), m_dwMouseEnable(0), m_dwInput(0), m_dwPlayerInfo(0), m_dwSensitivity(0), m_dwSendPackets(0), m_dwClientState_ViewAngles(0), m_dwPlayerResource(0), m_dwViewMatrix(0),
	m_iCrossHairID(0), m_iTeamNum(0), m_fFlags(0), m_iGlowIndex(0), m_bSpotted(0), m_flFlashMaxAlpha(0), m_lifeState(0), m_iHealth(0), m_aimPunch(0), m_iShotsFired(0), m_bHasDefuser(0), m_flC4Blow(0), m_bSpottedByMask(0), m_nModelIndex(0), m_hActiveWeapon(0), m_Item(0), m_iItemDefinitionIndex(0), m_AttributeManager(0), m_bIsScoped(0), m_iWorldModelIndex(0), m_zoomLevel(0), m_Local(0), m_viewPunchAngle(0), m_iClip1(0), m_flNextPrimaryAttack(0), m_nTickBase(0), m_vecVelocity(0), m_bGunGameImmunity(0), m_vecOrigin(0), m_iPlayerC4(0), m_vecViewOffset(0), m_dwBoneMatrix(0), m_clrRender(0),
	m_hSteamNetworkingSockets(nullptr), m_hSteamApi(nullptr), m_hTier0(nullptr), m_hVstdlib(nullptr), m_hPhonon3d(nullptr), m_hClient(nullptr), m_hEngine(nullptr),
	m_pClient(nullptr), m_pEntList(nullptr), m_pEngine(nullptr)
{
}

CCSGOData::~CCSGOData()
{
	m_dwTargetPID = 0;

	if (m_hSteamNetworkingSockets)
		FreeLibrary(m_hSteamNetworkingSockets);
	m_hSteamNetworkingSockets = nullptr;

	if (m_hSteamApi)
		FreeLibrary(m_hSteamApi);
	m_hSteamApi = nullptr;

	if (m_hTier0)
		FreeLibrary(m_hTier0);
	m_hTier0 = nullptr;

	if (m_hVstdlib)
		FreeLibrary(m_hVstdlib);
	m_hVstdlib = nullptr;

	if (m_hPhonon3d)
		FreeLibrary(m_hPhonon3d);
	m_hPhonon3d = nullptr;

	if (m_hClient)
		FreeLibrary(m_hClient);
	m_hClient = nullptr;

	if (m_hEngine)
		FreeLibrary(m_hEngine);
	m_hEngine = nullptr;

	m_pClient = nullptr;
	m_pEntList = nullptr;
	m_pEngine = nullptr;
}

HMODULE CCSGOData::LoadRemoteModule(const std::string & szModuleName)
{
	HMODULE hModule = nullptr;

	auto fastMemHelper = std::make_unique<CFastMemory>(m_dwTargetPID, FAST_MEMORY_WITHOUT_SUSPEND);
	if (fastMemHelper->Initialize())
	{
		if (fastMemHelper->GetProcessHelperInstance()->ModuleCount())
		{
			if (fastMemHelper->GetProcessHelperInstance()->CheckCSGOModulesIsLoaded())
			{
				auto szTargetModule = fastMemHelper->GetProcessHelperInstance()->GetModule(szModuleName.c_str());

				bool bModuleIsValid = szTargetModule.IsValid();
				if (bModuleIsValid)
				{
					hModule = LoadLibraryA(szTargetModule.GetPath());
					if (!hModule) {
						DEBUG_LOG(LL_ERR, "LoadLibrary fail! Error code: %u Module: %s", GetLastError(), szTargetModule.GetPath());
					}
				}
			}
		}
		fastMemHelper->Finalize();
	}
	return hModule;
}

bool CCSGOData::LoadRemoteModules()
{
	m_hSteamNetworkingSockets = LoadRemoteModule(XOR("steamnetworkingsockets.dll"));
	if (!m_hSteamNetworkingSockets) {
		DEBUG_LOG(LL_ERR, "steamnetworkingsockets.dll LoadRemoteModule fail!");
		return false;
	}
	m_hSteamApi = LoadRemoteModule(XOR("steam_api.dll"));
	if (!m_hSteamApi) {
		DEBUG_LOG(LL_ERR, "steam_api.dll LoadRemoteModule fail!");
		return false;
	}
	m_hTier0 = LoadRemoteModule(XOR("tier0.dll"));
	if (!m_hTier0) {
		DEBUG_LOG(LL_ERR, "tier0.dll LoadRemoteModule fail!");
		return false;
	}
	m_hVstdlib = LoadRemoteModule(XOR("vstdlib.dll"));
	if (!m_hVstdlib) {
		DEBUG_LOG(LL_ERR, "vstdlib.dll LoadRemoteModule fail!");
		return false;
	}
	m_hPhonon3d = LoadRemoteModule(XOR("phonon.dll"));
	if (!m_hPhonon3d) {
		DEBUG_LOG(LL_ERR, "phonon.dll LoadRemoteModule fail!");
		return false;
	}
	m_hClient = LoadRemoteModule(XOR("client.dll"));
	if (!m_hClient) {
		DEBUG_LOG(LL_ERR, "client.dll LoadRemoteModule fail!");
		return false;
	}
	m_hEngine = LoadRemoteModule(XOR("engine.dll"));
	if (!m_hEngine) {
		DEBUG_LOG(LL_ERR, "engine.dll LoadRemoteModule fail!");
		return false;
	}

	return true;
}

bool CCSGOData::LoadEngineFunctions()
{
	CreateInterfaceFn ClientFactory = (CreateInterfaceFn)GetProcAddress(m_hClient, XOR("CreateInterface"));
	if (!ClientFactory) {
		DEBUG_LOG(LL_ERR, "ClientFactory LoadEngineFunctions fail!");
		return false;
	}

	m_pClient = (CHLClient*)ClientFactory(XOR("VClient018"), nullptr);
	if (!m_pClient) {
		DEBUG_LOG(LL_ERR, "VClient018 LoadEngineFunctions fail!");
		return false;
	}

	m_pEntList = ClientFactory(XOR("VClientEntityList003"), nullptr);
	if (!m_pEntList) {
		DEBUG_LOG(LL_ERR, "VClientEntityList003 LoadEngineFunctions fail!");
		return false;
	}

	CreateInterfaceFn EngineFactory = (CreateInterfaceFn)GetProcAddress(m_hEngine, XOR("CreateInterface"));
	if (!EngineFactory) {
		DEBUG_LOG(LL_ERR, "EngineFactory LoadEngineFunctions fail!");
		return false;
	}

	m_pEngine = (CEngineClient*)EngineFactory(XOR("VEngineClient014"), nullptr);
	if (!m_pEngine) {
		DEBUG_LOG(LL_ERR, "m_pEngine LoadEngineFunctions fail!");
		return false;
	}

	return true;
}

bool CCSGOData::IsEngineLoaded()
{
	return (
		m_hSteamNetworkingSockets && m_hSteamApi && m_hTier0 && m_hVstdlib && m_hPhonon3d && m_hClient && m_hEngine && m_pClient && m_pEntList && m_pEngine
	);
}

bool CCSGOData::LoadRemoteModulesAndFunctions()
{
	bool bEngineRet = false;
	for (std::size_t i = 0; i < g_MaxAttempt; ++i)
	{
		bEngineRet = (LoadRemoteModules() && LoadEngineFunctions());
		if (bEngineRet) {
			DEBUG_LOG(LL_SYS, "Engine modules & functions found!");
			break;
		}

		Sleep(3000);
	}

	return bEngineRet;
}

bool CCSGOData::WaitForModuleInitilization(DWORD dwMSDelay)
{
	auto fastMemHelper = std::make_unique<CFastMemory>(m_dwTargetPID, FAST_MEMORY_WITHOUT_SUSPEND);
	if (fastMemHelper->Initialize())
	{
		fastMemHelper->GetProcessHelperInstance()->WaitLoadCSGOModules(dwMSDelay);
		return fastMemHelper->Finalize();
	}
	return false;
}

void CCSGOData::SavePointerToConfig(std::shared_ptr <CConfigParser> cSettings, bool bSaveToConfig, const std::string & szName, DWORD dwValue)
{
	if (bSaveToConfig && cSettings->IsExistKey(szName))
		cSettings->SetInteger(szName, dwValue);
}

bool CCSGOData::FindPointers(bool bSaveToConfig)
{
	// Setup helpers
	auto cSettings = std::make_shared<CConfigParser>(XOR("H4x.cfg"));
	if (!cSettings || !cSettings.get()) {
		DEBUG_LOG(LL_CRI, "H4x config file can not found!");
		abort();
		return false;
	}

	if (cSettings->Load() == false) {
		DEBUG_LOG(LL_CRI, "H4x config file can not load!");
		abort();
		return false;
	}

	auto fastMemHelper = std::make_unique<CFastMemory>(m_dwTargetPID, FAST_MEMORY_WITHOUT_SUSPEND);
	if (fastMemHelper->Initialize() == false) {
		DEBUG_LOG(LL_CRI, "Fast memory class initilization fail!");
		abort();
		return false;
	}

	// Get & parse module informations
	/// client.dll
	auto pClientModule = fastMemHelper->GetProcessHelperInstance()->GetModule(XOR("client.dll"));
	if (!pClientModule.IsValid()) {
		DEBUG_LOG(LL_CRI, "client.dll module can not found!");
		abort();
		return false;
	}

	auto dwClientLow = (DWORD)pClientModule.GetAddress();
	auto dwClientHigh = (DWORD)pClientModule.GetSize() + dwClientLow;
	DEBUG_LOG(LL_SYS, "Client module found! Low: %p High: %p", dwClientLow, dwClientHigh);

	/// engine.dll
	auto pEngineModule = fastMemHelper->GetProcessHelperInstance()->GetModule(XOR("engine.dll"));
	if (!pEngineModule.IsValid()) {
		DEBUG_LOG(LL_CRI, "engine.dll module can not found!");
		abort();
		return false;
	}

	auto dwEngineLow = (DWORD)pEngineModule.GetAddress();
	auto dwEngineHigh = (DWORD)pEngineModule.GetSize() + dwEngineLow;
	DEBUG_LOG(LL_SYS, "Engine module found! Low: %p High: %p", dwEngineLow, dwEngineHigh);


	// Hash
	auto szVersionHash = CreateVersionHash();
	if (cSettings->IsExistKey(XOR("process.versionhash")))
		cSettings->SetString(XOR("process.versionhash"), szVersionHash);


	// Init & scan patterns
	auto memScanner_c = std::make_unique<CMemoryScanner>(fastMemHelper->GetProcessHelperInstance()->GetHandle(), EScanFlags::MEM_SCAN_READ | EScanFlags::MEM_SCAN_SUBTRACT, dwClientLow, dwClientHigh);
	auto memScanner_e = std::make_unique<CMemoryScanner>(fastMemHelper->GetProcessHelperInstance()->GetHandle(), EScanFlags::MEM_SCAN_READ | EScanFlags::MEM_SCAN_SUBTRACT, dwEngineLow, dwEngineHigh);

	DEBUG_LOG(LL_SYS, "Loading patterns!");

	// Client.dll
	m_dwLocalPlayer = memScanner_c->Scan(LocalPlayerPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwLocalPlayer", m_dwLocalPlayer); // TODO: XOR
	DEBUG_LOG(LL_SYS, "m_dwLocalPlayer: %p", m_dwLocalPlayer);

	m_dwEntityList = memScanner_c->Scan(EntityListPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwEntityList", m_dwEntityList);
	DEBUG_LOG(LL_SYS, "m_dwEntityList: %p", m_dwEntityList);

	m_dwGlowObject = memScanner_c->Scan(GlowObjectManagerPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwGlowObject", m_dwGlowObject);
	DEBUG_LOG(LL_SYS, "m_dwGlowObject: %p", m_dwGlowObject);

	m_dwForceAttack = memScanner_c->Scan(ForceAttackPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwForceAttack", m_dwForceAttack);
	DEBUG_LOG(LL_SYS, "m_dwForceAttack: %p", m_dwForceAttack);

	m_dwForceJump = memScanner_c->Scan(ForceJumpPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwForceJump", m_dwForceJump);
	DEBUG_LOG(LL_SYS, "m_dwForceJump: %p", m_dwForceJump);

	m_dwForceLeft = memScanner_c->Scan(ForceLeftPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwForceLeft", m_dwForceLeft);
	DEBUG_LOG(LL_SYS, "m_dwForceLeft: %p", m_dwForceLeft);

	m_dwForceRight = memScanner_c->Scan(ForceRightPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwForceRight", m_dwForceRight);
	DEBUG_LOG(LL_SYS, "m_dwForceRight: %p", m_dwForceRight);

	m_dwMouseEnable = memScanner_c->Scan(MouseEnablePattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwMouseEnable", m_dwMouseEnable);
	DEBUG_LOG(LL_SYS, "m_dwMouseEnable: %p", m_dwMouseEnable);

	m_dwInput = memScanner_c->Scan(InputPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwInput", m_dwInput);
	DEBUG_LOG(LL_SYS, "m_dwInput: %p", m_dwInput);

	m_dwSensitivity = memScanner_c->Scan(SensitivityPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwSensitivity", m_dwSensitivity);
	DEBUG_LOG(LL_SYS, "m_dwSensitivity: %p", m_dwSensitivity);

	m_dwPlayerResource = memScanner_c->Scan(PlayerResourcePattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwPlayerResource", m_dwPlayerResource);
	DEBUG_LOG(LL_SYS, "m_dwPlayerResource: %p", m_dwPlayerResource);

	m_dwViewMatrix = memScanner_c->Scan(ViewMatrixPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwViewMatrix", m_dwViewMatrix);
	DEBUG_LOG(LL_SYS, "m_dwViewMatrix: %p", m_dwViewMatrix);


	/// Read only patterns
	memScanner_c->SetFlags(EScanFlags::MEM_SCAN_READ);

	m_bDormant = memScanner_c->Scan(DormantPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_bDormant", m_bDormant);
	DEBUG_LOG(LL_SYS, "m_bDormant: %p", m_bDormant);


	// Engine.dll
	m_dwClientState = memScanner_e->Scan(ClientStatePattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwClientState", m_dwClientState);
	DEBUG_LOG(LL_SYS, "m_dwClientState: %p", m_dwClientState);

	m_dwGlobalVars = memScanner_e->Scan(GlobalVarsPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwGlobalVars", m_dwGlobalVars);
	DEBUG_LOG(LL_SYS, "m_dwGlobalVars: %p", m_dwGlobalVars);


	/// Read only patterns
	memScanner_e->SetFlags(EScanFlags::MEM_SCAN_READ);

	m_dwInGame = memScanner_e->Scan(IngamePattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwInGame", m_dwInGame);
	DEBUG_LOG(LL_SYS, "m_dwInGame: %p", m_dwInGame);

	m_dwViewAngles = memScanner_e->Scan(ViewAnglesPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwViewAngles", m_dwViewAngles);
	DEBUG_LOG(LL_SYS, "m_dwViewAngles: %p", m_dwViewAngles);

	m_dwClientState_State = memScanner_e->Scan(ClientStateStatePattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwClientState_State", m_dwClientState_State);
	DEBUG_LOG(LL_SYS, "m_dwClientState_State: %p", m_dwClientState_State);

	m_dwPlayerInfo = memScanner_e->Scan(PlayerInfoPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwPlayerInfo", m_dwPlayerInfo);
	DEBUG_LOG(LL_SYS, "m_dwPlayerInfo: %p", m_dwPlayerInfo);

	m_dwClientState_ViewAngles = memScanner_e->Scan(ClientStateViewAnglesPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwClientState_ViewAngles", m_dwClientState_ViewAngles);
	DEBUG_LOG(LL_SYS, "m_dwClientState_ViewAngles: %p", m_dwClientState_ViewAngles);
	

	/// Subtract only
	memScanner_e->SetFlags(EScanFlags::MEM_SCAN_SUBTRACT);

	m_dwSendPackets = memScanner_e->Scan(SendPacketsPattern());
	SavePointerToConfig(cSettings, bSaveToConfig, "pointer.m_dwSendPackets", m_dwSendPackets);
	DEBUG_LOG(LL_SYS, "m_dwSendPackets: %p", m_dwSendPackets);

	

	// Complete works
	fastMemHelper->Finalize();

	cSettings->Save();
	cSettings.reset();
	cSettings = nullptr;


	// Result
	if (m_dwLocalPlayer && m_dwGlowObject && m_dwEntityList && m_dwForceAttack && m_dwForceJump && m_bDormant && m_dwClientState &&
		m_dwInGame && m_dwViewAngles && m_dwGlobalVars && m_dwForceLeft && m_dwForceRight && m_dwClientState_State && m_dwMouseEnable &&
		m_dwInput && m_dwPlayerInfo && m_dwClientState_ViewAngles && m_dwPlayerResource)
	{
		return true;
	}

	return false;
}

bool CCSGOData::FindNetvars()
{
	DEBUG_LOG(LL_SYS, "Loading netvar offsets!");

	m_iCrossHairID = GetNetVar("DT_CSPlayer", "m_bHasDefuser") + 0x5C; // TODO: XOR
	DEBUG_LOG(LL_SYS, "m_iCrossHairID: %p", m_iCrossHairID);

	m_iTeamNum = GetNetVar("DT_BaseEntity", "m_iTeamNum");
	DEBUG_LOG(LL_SYS, "m_iTeamNum: %p", m_iTeamNum);

	m_fFlags = GetNetVar("DT_BasePlayer", "m_fFlags");
	DEBUG_LOG(LL_SYS, "m_fFlags: %p", m_fFlags);

	m_iGlowIndex = GetNetVar("DT_CSPlayer", "m_flFlashDuration") + 0x18;
	DEBUG_LOG(LL_SYS, "m_iGlowIndex: %p", m_iGlowIndex);

	m_bSpotted = GetNetVar("DT_BaseEntity", "m_bSpotted");
	DEBUG_LOG(LL_SYS, "m_bSpotted: %p", m_bSpotted);

	m_flFlashMaxAlpha = GetNetVar("DT_CSPlayer", "m_flFlashMaxAlpha");
	DEBUG_LOG(LL_SYS, "m_flFlashMaxAlpha: %p", m_flFlashMaxAlpha);

	m_lifeState = GetNetVar("DT_BasePlayer", "m_lifeState");
	DEBUG_LOG(LL_SYS, "m_lifeState: %p", m_lifeState);

	m_iHealth = GetNetVar("DT_BasePlayer", "m_iHealth");
	DEBUG_LOG(LL_SYS, "m_iHealth: %p", m_iHealth);

	m_aimPunch = GetNetVar("DT_BasePlayer", "m_aimPunchAngle");
	DEBUG_LOG(LL_SYS, "m_aimPunch: %p", m_aimPunch);

	m_iShotsFired = GetNetVar("DT_CSPlayer", "m_iShotsFired");
	DEBUG_LOG(LL_SYS, "m_iShotsFired: %p", m_iShotsFired);

	m_bHasDefuser = GetNetVar("DT_CSPlayer", "m_bHasDefuser");
	DEBUG_LOG(LL_SYS, "m_bHasDefuser: %p", m_bHasDefuser);

	m_flC4Blow = GetNetVar("DT_PlantedC4", "m_flC4Blow");
	DEBUG_LOG(LL_SYS, "m_flC4Blow: %p", m_flC4Blow);

	m_bSpottedByMask = GetNetVar("DT_BaseEntity", "m_bSpottedByMask");
	DEBUG_LOG(LL_SYS, "m_bSpottedByMask: %p", m_bSpottedByMask);

	m_nModelIndex = GetNetVar("DT_BaseEntity", "m_nModelIndex");
	DEBUG_LOG(LL_SYS, "m_nModelIndex: %p", m_nModelIndex);

	m_hActiveWeapon = GetNetVar("DT_BasePlayer", "m_hActiveWeapon");
	DEBUG_LOG(LL_SYS, "m_hActiveWeapon: %p", m_hActiveWeapon);

	m_Item = GetNetVar("DT_AttributeContainer", "m_Item");
	DEBUG_LOG(LL_SYS, "m_Item: %p", m_Item);

	m_iItemDefinitionIndex = GetNetVar("DT_ScriptCreatedItem", "m_iItemDefinitionIndex");
	DEBUG_LOG(LL_SYS, "m_iItemDefinitionIndex: %p", m_iItemDefinitionIndex);

	m_AttributeManager = GetNetVar("DT_EconEntity", "m_AttributeManager");
	DEBUG_LOG(LL_SYS, "m_AttributeManager: %p", m_AttributeManager);

	m_bIsScoped = GetNetVar("DT_CSPlayer", "m_bIsScoped");
	DEBUG_LOG(LL_SYS, "m_bIsScoped: %p", m_bIsScoped);

	m_iWorldModelIndex = GetNetVar("DT_BaseCombatWeapon", "m_iWorldModelIndex");
	DEBUG_LOG(LL_SYS, "m_iWorldModelIndex: %p", m_iWorldModelIndex);

	m_zoomLevel = GetNetVar("DT_WeaponAWP", "m_zoomLevel");
	DEBUG_LOG(LL_SYS, "m_zoomLevel: %p", m_zoomLevel);

	m_Local = GetNetVar("DT_BasePlayer", "m_Local");
	DEBUG_LOG(LL_SYS, "m_Local: %p", m_Local);

	m_viewPunchAngle = GetNetVar("DT_BasePlayer", "m_viewPunchAngle");
	DEBUG_LOG(LL_SYS, "m_viewPunchAngle: %p", m_viewPunchAngle);

	m_iClip1 = GetNetVar("DT_BaseCombatWeapon", "m_iClip1");
	DEBUG_LOG(LL_SYS, "m_iClip1: %p", m_iClip1);

	m_flNextPrimaryAttack = GetNetVar("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	DEBUG_LOG(LL_SYS, "m_flNextPrimaryAttack: %p", m_flNextPrimaryAttack);

	m_nTickBase = GetNetVar("DT_CSPlayer", "m_nTickBase");
	DEBUG_LOG(LL_SYS, "m_nTickBase: %p", m_nTickBase);

	m_vecVelocity = GetNetVar("DT_CSPlayer", "m_vecVelocity[0]");
	DEBUG_LOG(LL_SYS, "m_vecVelocity: %p", m_vecVelocity);

	m_bGunGameImmunity = GetNetVar("DT_CSPlayer", "m_bGunGameImmunity");
	DEBUG_LOG(LL_SYS, "m_bGunGameImmunity: %p", m_bGunGameImmunity);
		
	m_vecOrigin = GetNetVar("DT_BasePlayer", "m_vecOrigin");
	DEBUG_LOG(LL_SYS, "m_vecOrigin: %p", m_vecOrigin);

	m_iPlayerC4 = GetNetVar("DT_CSPlayerResource", "m_iPlayerC4");
	DEBUG_LOG(LL_SYS, "m_iPlayerC4: %p", m_iPlayerC4);

	m_vecViewOffset = GetNetVar("DT_CSPlayer", "m_vecViewOffset[0]");
	DEBUG_LOG(LL_SYS, "m_vecViewOffset: %p", m_vecViewOffset);

	m_dwBoneMatrix = GetNetVar("DT_BaseAnimating", "m_nForceBone") + 28;
	DEBUG_LOG(LL_SYS, "m_dwBoneMatrix: %p", m_dwBoneMatrix);

	m_clrRender = GetNetVar("DT_BaseEntity", "m_clrRender");
	DEBUG_LOG(LL_SYS, "m_clrRender: %p", m_clrRender);

	
	// Result
	if (m_iCrossHairID && m_iTeamNum && m_fFlags && m_iGlowIndex && m_bSpotted && m_flFlashMaxAlpha && m_lifeState && m_iHealth &&
		m_aimPunch && m_iShotsFired && m_bHasDefuser && m_flC4Blow && m_bSpottedByMask && m_nModelIndex && m_hActiveWeapon && m_Item &&
		m_iItemDefinitionIndex && m_AttributeManager && m_bIsScoped && m_iWorldModelIndex && m_zoomLevel && m_Local && m_viewPunchAngle &&
		m_iClip1 && m_flNextPrimaryAttack && m_nTickBase && m_vecVelocity && m_bGunGameImmunity && m_vecOrigin && m_iPlayerC4 && m_vecViewOffset &&
		m_dwBoneMatrix && m_clrRender)
	{
		return true;
	}

	return false;
}

std::string CCSGOData::CreateVersionHash()
{
	auto szResult = std::string("");
	auto fastMemHelper = std::make_unique<CFastMemory>(m_dwTargetPID, FAST_MEMORY_WITHOUT_SUSPEND);
	if (fastMemHelper->Initialize())
	{
		auto pClientModule = fastMemHelper->GetProcessHelperInstance()->GetModule(XOR("client.dll"));
		if (pClientModule.IsValid())
		{
			auto pEngineModule = fastMemHelper->GetProcessHelperInstance()->GetModule(XOR("engine.dll"));
			if (pEngineModule.IsValid())
			{
				auto szEngineHash = g_haxApp->GetFunctionsInstance()->GetFileMd5(pEngineModule.GetPath());
				//DEBUG_LOG(LL_SYS, "Engine module info: %s[%s]", pEngineModule.GetPath(), szEngineHash.c_str());
				auto szClientHash = g_haxApp->GetFunctionsInstance()->GetFileMd5(pClientModule.GetPath());
				//DEBUG_LOG(LL_SYS, "Client module info: %s[%s]", pClientModule.GetPath(), szClientHash.c_str());
				auto szProcessHash = g_haxApp->GetFunctionsInstance()->GetFileMd5(fastMemHelper->GetProcessHelperInstance()->GetPath().c_str());
				//DEBUG_LOG(LL_SYS, "Process info: %s[%s]", fastMemHelper->GetProcessHelperInstance()->GetPath().c_str(), szProcessHash.c_str());

				auto szCombinedHashValue = szEngineHash + szClientHash + szProcessHash;
				auto szCombinedHash = g_haxApp->GetFunctionsInstance()->GetMd5((BYTE*)szCombinedHashValue.data(), szCombinedHashValue.size());
				DEBUG_LOG(LL_SYS, "Process combined hash: %s -> %s", szCombinedHashValue.c_str(), szCombinedHash.c_str());

				szResult = szCombinedHash;

				fastMemHelper->Finalize();
			}
		}
		fastMemHelper->Finalize();
	}
	return szResult;
}
