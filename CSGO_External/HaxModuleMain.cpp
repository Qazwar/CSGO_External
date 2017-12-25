#include "main.h"
#include "Defines.h"
#include "Common.h"
#include "XOR.h"

CHaxModule::CHaxModule(DWORD dwProcessId) :
	m_dwProcessId(dwProcessId), m_dwGameCode(0),
	m_hMinimizeCheckThread(INVALID_HANDLE_VALUE), m_bHookTerminated(false), m_hHook(nullptr),
	m_cHaxData(nullptr),
	m_CSGOManager(nullptr)
{
}

CHaxModule::~CHaxModule()
{
	m_dwProcessId	= 0;
	m_dwGameCode	= 0;

	DisableHax();

	DestroyMinimizeCheck();

	if (IS_VALID_HANDLE(m_hMinimizeCheckThread))
	{
		DWORD dwThreadExitCode = 0;
		if (GetExitCodeThread(m_hMinimizeCheckThread, &dwThreadExitCode) && dwThreadExitCode == STILL_ACTIVE)
			::TerminateThread(m_hMinimizeCheckThread, EXIT_SUCCESS);

		CloseHandle(m_hMinimizeCheckThread);
	}
	m_hMinimizeCheckThread = INVALID_HANDLE_VALUE;

	m_CSGOManager.reset();
	m_CSGOManager = nullptr;
}


void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD dwEventCode, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	if (hwnd)
	{
		if (dwEventCode == EVENT_SYSTEM_MINIMIZESTART || dwEventCode == EVENT_SYSTEM_MINIMIZEEND)
		{
			DWORD dwProcessId = 0;
			auto dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);
			if (dwThreadId && dwProcessId)
			{
				auto vHandledProcesses = g_haxApp->GetHandledProcesses();
				if (vHandledProcesses.empty() == false)
				{
					for (auto & curProc : vHandledProcesses)
					{
						if (curProc->dwProcessId == dwProcessId)
						{
							if (curProc->haxController)
							{
								if (curProc->haxController->GetHaxModule() && curProc->haxController->GetHaxModule().get())
								{
									if (curProc->haxController->GetHaxModule()->IsStarted())
									{
										if (dwEventCode == EVENT_SYSTEM_MINIMIZESTART) {
											DEBUG_LOG(LL_SYS, "Game process window minimized! Hax temporary disabled.");
											curProc->haxController->GetHaxModule()->SuspendHax();
										}
										else {
											DEBUG_LOG(LL_SYS, "Game process window maximized! Hax re-enabled.");
											curProc->haxController->GetHaxModule()->ResumeHax();
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CHaxModule::DestroyMinimizeCheck()
{
	UnhookWinEvent(m_hHook);
	m_bHookTerminated = true;
}

DWORD WINAPI CHaxModule::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CHaxModule*)lpParam;
	return This->ThreadRoutine();
}

DWORD CHaxModule::ThreadRoutine(void)
{
	m_hHook = SetWinEventHook(EVENT_MIN, EVENT_MAX, NULL, HandleWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
	if (!m_hHook) {
		DEBUG_LOG(LL_ERR, "An error occurred on try setup hook! Error: %u", GetLastError());
		return 0;
	}

	MSG message;
	while (GetMessageA(&message, NULL, 0, 0) && !m_bHookTerminated) {
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}
	return 0;
}


bool CHaxModule::InitCSGO(bool bUsePreDefinedPointers)
{
	// Init data class
	auto csgoDataHelper = m_cHaxData->GetCSGODataInstance();
	if (!csgoDataHelper) {
		DEBUG_LOG(LL_CRI, "H4x data class can not load!");
		return false;
	}

	// Wait for module initilizations
	auto bWaitCSGORet = csgoDataHelper->WaitForModuleInitilization(1000);
	if (!bWaitCSGORet) {
		DEBUG_LOG(LL_CRI, "CSGO module wait failed!");
		return false;
	}

	// Check version
	auto szCurrentVersionHash = csgoDataHelper->CreateVersionHash();
	if (szCurrentVersionHash.empty()) {
		DEBUG_LOG(LL_CRI, "Version hash calculate failed!");
		return false;
	}
	auto szOldHash = m_cHaxData->GetString(XOR("process.versionhash"));
	if (szOldHash.empty()) {
		DEBUG_LOG(LL_CRI, "Pre-defined version hash not found!");
		return false;
	}
	DEBUG_LOG(LL_SYS, "Current version: %s Old version: %s Eq: %d", szCurrentVersionHash.c_str(), szOldHash.c_str(), szOldHash == szCurrentVersionHash);
	if (szOldHash != szCurrentVersionHash) {
		DEBUG_LOG(LL_ERR, "Version mismatch! Pointers will be checked again.");
		bUsePreDefinedPointers = false; // re-parse pointers
	}

	// Scan pointers
	bool bFindPointersRet = false;
	if (bUsePreDefinedPointers)
	{
		bFindPointersRet = m_cHaxData->ParsePreDefinedPointers();
		if (!bFindPointersRet) {
			DEBUG_LOG(LL_ERR, "Pre-defined Pointers can not loaded!");
		}
	}
	if (!bFindPointersRet) {
		bFindPointersRet = csgoDataHelper->FindPointers(bUsePreDefinedPointers);
		if (!bFindPointersRet) {
			DEBUG_LOG(LL_CRI, "Pointers can not found!");
			return false;
		}
	}

	// Init engine modules & functions
	bool bLoadModulesRet = csgoDataHelper->LoadRemoteModulesAndFunctions();
	if (!bLoadModulesRet) {
		DEBUG_LOG(LL_CRI, "Engine initilization failed!");
		return false;
	}

	// Dump netvars
	csgoDataHelper->DumpNetvars();

	// TODO: Unload modules

	auto bFindNetvarsRet = csgoDataHelper->FindNetvars();
	if (bFindNetvarsRet == false) {
		DEBUG_LOG(LL_CRI, "Netvar offsets can not found!");
		return false;
	}

	// Init csgo manager class
	m_CSGOManager = std::make_shared<CCSGOManager>(m_dwProcessId, m_cHaxData);
	if (!m_CSGOManager || !m_CSGOManager.get() || !m_CSGOManager->CreateThread()) {
		DEBUG_LOG(LL_CRI, "m_CSGOManager initilization failed!");
		return false;
	}

	return true;
}

void CHaxModule::InitHax()
{
	// Read & Parse Config File
	auto cSettings = std::make_unique<CConfigParser>(XOR("H4x.cfg"));
	if (!cSettings || !cSettings.get()) {
		DEBUG_LOG(LL_CRI, "H4x config file can not found!");
		abort();
		return;
	}

	if (cSettings->Load() == false) {
		DEBUG_LOG(LL_CRI, "H4x config file can not load!");
		abort();
		return;
	}

	if (cSettings->IsExistKey("process.gamecode") == false) {
		DEBUG_LOG(LL_CRI, "Game code config can not found!");
		abort();
		return;
	}

	if (cSettings->IsExistKey("process.usepredefinedpointers") == false) {
		DEBUG_LOG(LL_CRI, "Use Pre Defined config setting can not found!");
		abort();
		return;
	}

	m_dwGameCode = cSettings->Get_uint32("process.gamecode");
	DEBUG_LOG(LL_SYS, "Game code: %u", m_dwGameCode);

	auto bUsePreDefinedPointers = cSettings->GetBoolean("process.usepredefinedpointers");;
	DEBUG_LOG(LL_SYS, "Use Pre-Defined Pointers: %d", bUsePreDefinedPointers);

	cSettings.reset();
	cSettings = nullptr;


	// Init data class
	m_cHaxData = std::make_shared<CHaxData>(m_dwProcessId, (DWORD)m_dwGameCode);
	if (m_cHaxData == nullptr || m_cHaxData.get() == nullptr) {
		DEBUG_LOG(LL_ERR, "Hax data class initilization fail!");
		abort();
		return;
	}

	// Init minimize check
	DWORD dwThreadId = 0;
	m_hMinimizeCheckThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (!IS_VALID_HANDLE(m_hMinimizeCheckThread)) {
		DEBUG_LOG(LL_ERR, "Minimize check thread create fail!");
		abort();
		return;
	}

	// Init hax works
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
	{
		auto bInitCSGORet = InitCSGO(bUsePreDefinedPointers);
		if (bInitCSGORet == false && g_haxApp->GetFunctionsInstance()->ProcessHasThread(GetAttachedPID())) {
			DEBUG_LOG(LL_ERR, "InitCSGO function initilization fail!");
			abort();
			return;
		}
	}
	else {
		DEBUG_LOG(LL_ERR, "Unused game code: %u", m_dwGameCode);
		abort();
	}
}

void CHaxModule::ExecHax()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			m_CSGOManager->OnKeyChange();

	// Other game haxs
	// ...
}

void CHaxModule::EnableHax()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			m_CSGOManager->Enable();

	// Other game haxs
	// ...
}

void CHaxModule::DisableHax()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			m_CSGOManager->Disable();

	// Other game haxs
	// ...
}

void CHaxModule::SuspendHax()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			m_CSGOManager->Suspend();

	// Other game haxs
	// ...
}

void CHaxModule::ResumeHax()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			m_CSGOManager->Resume();

	// Other game haxs
	// ...
}

bool CHaxModule::IsStarted()
{
	// CSGO Haxs
	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		if (m_CSGOManager)
			return m_CSGOManager->IsStarted();

	// Other game haxs
	// ...

	return false;
}

