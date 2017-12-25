#include "main.h"
#include "Hax_CSGO_Manager.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"

// Hax
CCSGOManager::CCSGOManager(DWORD dwTargetPID, const std::shared_ptr<CHaxData> & cHaxData) :
	m_dwTargetPID(dwTargetPID), m_dwClientBase(0), m_dwEngineBase(0), m_dwEnginePointer(0), m_dwClientStateStatePointer(0),
	m_GlowESPHelper(nullptr), m_BunnyHopHelper(nullptr), m_TriggerBotHelper(nullptr), m_NoRecoilHelper(nullptr), m_CSGOMiscHelper(nullptr), m_AimBotHelper(nullptr),
	m_cHaxData(cHaxData), m_cFastMemoryHelper(nullptr), m_cProcHelper(nullptr), m_cMemHelper(nullptr), m_cEngineHelper(nullptr),
	m_hThread(INVALID_HANDLE_VALUE), m_bDisabled(false), m_bIsStarted(false), m_bIsSuspended(false)
{
	DEBUG_LOG(LL_SYS, "CCSGOManager::CCSGOManager");

	// Init classes
	m_cFastMemoryHelper = std::make_unique<CFastMemory>(m_dwTargetPID, FAST_MEMORY_WITHOUT_SUSPEND);
	if (!m_cFastMemoryHelper->Initialize()) {
		DEBUG_LOG(LL_ERR, "Fast Memory helper class initilization fail!");
		abort();
	}

	m_cProcHelper = m_cFastMemoryHelper->GetProcessHelperInstance();
	if (!m_cProcHelper) {
		DEBUG_LOG(LL_ERR, "Process helper class initilization fail!");
		abort();
	}

	m_cMemHelper = std::make_shared<CMemoryHelper>(m_cProcHelper->GetHandle());
	if (!m_cMemHelper || !m_cMemHelper.get()) {
		DEBUG_LOG(LL_ERR, "Memory helper class initilization fail!");
		abort();
	}
}

CCSGOManager::~CCSGOManager()
{
	DEBUG_LOG(LL_SYS, "CCSGOManager::~CCSGOManager");

	// Hax classes
	m_GlowESPHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_GlowESPHelper->SendStopSignal();
#endif
	m_GlowESPHelper.reset();
	m_GlowESPHelper = nullptr;

	m_BunnyHopHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_BunnyHopHelper->SendStopSignal();
#endif
	m_BunnyHopHelper.reset();
	m_BunnyHopHelper = nullptr;

	m_TriggerBotHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_TriggerBotHelper->SendStopSignal();
#endif
	m_TriggerBotHelper.reset();
	m_TriggerBotHelper = nullptr;

	m_NoRecoilHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_NoRecoilHelper->SendStopSignal();
#endif
	m_NoRecoilHelper.reset();
	m_NoRecoilHelper = nullptr;

	m_CSGOMiscHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_CSGOMiscHelper->SendStopSignal();
#endif
	m_CSGOMiscHelper.reset();
	m_CSGOMiscHelper = nullptr;

	m_AimBotHelper->Disable();
#if HAX_MULTI_THREAD == true
	m_AimBotHelper->SendStopSignal();
#endif
	m_AimBotHelper.reset();
	m_AimBotHelper = nullptr;

	// Memory
	m_cFastMemoryHelper->Finalize();

	// Thread
	if (IS_VALID_HANDLE(m_hThread))
	{
		DWORD dwThreadExitCode = 0;
		if (GetExitCodeThread(m_hThread, &dwThreadExitCode) && dwThreadExitCode == STILL_ACTIVE)
			::TerminateThread(m_hThread, EXIT_SUCCESS);

		CloseHandle(m_hThread);
	}
	m_hThread = INVALID_HANDLE_VALUE;

	// Veriables
	m_dwTargetPID = 0;
	m_bDisabled = false;
}


bool CCSGOManager::InitHax()
{
	// Build engine helper
	m_cEngineHelper = std::make_shared<CEngine>(shared_from_this());
	if (!m_cEngineHelper || !m_cEngineHelper.get()) {
		DEBUG_LOG(LL_ERR, "Engine helper class initilization fail!");
		abort();
	}

	// Module Info
	auto pClientModule = m_cProcHelper->GetModule(XOR("client.dll"));
	if (!pClientModule.IsValid()) {
		DEBUG_LOG(LL_CRI, "Client.dll module not found!");
		abort();
		return false;
	}
	m_dwClientBase = (DWORD)pClientModule.GetAddress();

	auto pEngineModule = m_cProcHelper->GetModule(XOR("engine.dll"));
	if (!pEngineModule.IsValid()) {
		DEBUG_LOG(LL_CRI, "Engine.dll module not found!");
		abort();
		return false;
	}
	m_dwEngineBase = (DWORD)pEngineModule.GetAddress();

	m_dwEnginePointer = m_cMemHelper->Read<DWORD>(m_dwEngineBase + m_cHaxData->GetCSGODataInstance()->GetClientState_Pointer());
	if (!m_dwEnginePointer) {
		DEBUG_LOG(LL_CRI, "m_dwEnginePointer address not found! Engine Base: %p Client state: %p", m_dwEngineBase, m_cHaxData->GetCSGODataInstance()->GetClientState_Pointer());
		abort();
		return false;
	}

	m_dwClientStateStatePointer = m_cHaxData->GetCSGODataInstance()->GetClientStateState_Pointer();
	if (!m_dwClientStateStatePointer) {
		DEBUG_LOG(LL_CRI, "m_dwClientStateStatePointer address not found!");
		abort();
		return false;
	}


	// Data
	for (auto & p : pEntitiesArray)
	{
		p = std::make_shared<CEntity>(shared_from_this());
		if (!p || !p.get()) {
			DEBUG_LOG(LL_CRI, "Entity creation fail!");
			abort();
		}
	}

	pLocalEntity = std::make_shared<CEntity>(shared_from_this());
	if (!pLocalEntity || !pLocalEntity.get()) {
		DEBUG_LOG(LL_CRI, "Local Entity creation fail!");
		abort();
	}


	// Hax classes

	/// Glow ESP
	m_GlowESPHelper = std::make_unique<CGlowESP>(shared_from_this());
	if (!m_GlowESPHelper || !m_GlowESPHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_GlowESPHelper allocation failed!");
		return false;
	}

	/// Bunny Hop
	m_BunnyHopHelper = std::make_unique<CBunnyHop>(shared_from_this());
	if (!m_BunnyHopHelper || !m_BunnyHopHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_BunnyHopHelper initilization failed!");
		return false;
	}

	/// Trigger bot
	m_TriggerBotHelper = std::make_unique<CTriggerBot>(shared_from_this());
	if (!m_TriggerBotHelper || !m_TriggerBotHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_TriggerBotHelper initilization failed!");
		return false;
	}

	/// No recoil
	m_NoRecoilHelper = std::make_unique<CNoRecoil>(shared_from_this());
	if (!m_NoRecoilHelper || !m_NoRecoilHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_NoRecoilHelper initilization failed!");
		return false;
	}

	/// Misc
	m_CSGOMiscHelper = std::make_unique<CCSGOMisc>(shared_from_this());
	if (!m_CSGOMiscHelper || !m_CSGOMiscHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_CSGOMiscHelper initilization failed!");
		return false;
	}

	/// Aimbot
	m_AimBotHelper = std::make_unique<CAimBot>(shared_from_this());
	if (!m_AimBotHelper || !m_AimBotHelper.get()) {
		DEBUG_LOG(LL_CRI, "m_AimBotHelper initilization failed!");
		return false;
	}


#if HAX_MULTI_THREAD == true
	auto bCreateGlowThreadRet = m_GlowESPHelper->CreateThread();
	if (!bCreateGlowThreadRet) {
		DEBUG_LOG(LL_CRI, "m_GlowESPHelper thread creation failed!");
		return false;
	}
	auto bCreateBunnyHopThreadRet = m_BunnyHopHelper->CreateThread();
	if (!bCreateBunnyHopThreadRet) {
		DEBUG_LOG(LL_CRI, "m_BunnyHopHelper thread creation failed!");
		return false;
	}
	auto bCreateTriggerBotThreadRet = m_TriggerBotHelper->CreateThread();
	if (!bCreateTriggerBotThreadRet) {
		DEBUG_LOG(LL_CRI, "m_TriggerBotHelper thread creation failed!");
		return false;
	}
	auto bCreateNoRecoilThreadRet = m_NoRecoilHelper->CreateThread();
	if (!bCreateNoRecoilThreadRet) {
		DEBUG_LOG(LL_CRI, "m_NoRecoilHelper thread creation failed!");
		return false;
	}
	auto bCreateMiscThreadRet = m_CSGOMiscHelper->CreateThread();
	if (!bCreateMiscThreadRet) {
		DEBUG_LOG(LL_CRI, "m_CSGOMiscHelper thread creation failed!");
		return false;
	}
	auto bCreateAimBotThreadRet = m_AimBotHelper->CreateThread();
	if (!bCreateAimBotThreadRet) {
		DEBUG_LOG(LL_CRI, "m_AimBotHelper thread creation failed!");
		return false;
	}
#endif

	return true;
}

void CCSGOManager::OnKeyChange()
{
	// Reload config
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		while (GetAsyncKeyState(VK_DELETE) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		
		Suspend();

		Sleep(1000); // wait for unfinished jobs

		m_cHaxData->ReloadSettings();

		if (m_GlowESPHelper)
			m_GlowESPHelper->LoadConfigs();

		if (m_CSGOMiscHelper)
			m_CSGOMiscHelper->LoadConfigs();

		if (m_BunnyHopHelper)
			m_BunnyHopHelper->LoadConfigs();

		if (m_TriggerBotHelper)
			m_TriggerBotHelper->LoadConfigs();

		if (m_NoRecoilHelper)
			m_NoRecoilHelper->LoadConfigs();

		if (m_AimBotHelper)
			m_AimBotHelper->LoadConfigs();

		// Sleep(1000); // wait for unfinished jobs

		Resume();

		Beep(750, 250);
		DEBUG_LOG(LL_SYS, "Configs reloaded.");
	}

	// Force Update
	if (GetAsyncKeyState(VK_F12) & 0x8000)
	{
		while (GetAsyncKeyState(VK_F12) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		m_cEngineHelper->ForceUpdate();
	}

	// Panic buttons
	if (GetAsyncKeyState(VK_INSERT) & 0x8000) // Unload
	{
		while (GetAsyncKeyState(VK_INSERT) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		DEBUG_LOG(LL_SYS, "Panic button press detected! Hax unload routine started.");

		g_haxApp->HaxAppWatcher(m_dwTargetPID, g_haxApp->GetTargetFileName(), false); // send fake terminate signal
	}

	if (GetAsyncKeyState(VK_END) & 0x8000) // Disable all haxs
	{
		while (GetAsyncKeyState(VK_END) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		DEBUG_LOG(LL_SYS, "Panic button press detected! Hax disable routine started.");
		
		if (m_bIsSuspended)
			Resume();
		else
			Suspend();
	}
	
	if (GetAsyncKeyState(VK_SNAPSHOT) & 0x8000) // Close game process
	{
		while (GetAsyncKeyState(VK_F10) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		DEBUG_LOG(LL_SYS, "Panic button press detected! Hax disable routine started.");

		GetProcessHelper()->Terminate();
	}

	// Common
	if (m_GlowESPHelper)
	{
		if (GetAsyncKeyState(m_GlowESPHelper->GetEnableKey()) & 0x8000)
		{
			m_GlowESPHelper->IsEnabled() ? m_GlowESPHelper->Disable() : m_GlowESPHelper->Enable();

			while (GetAsyncKeyState(m_GlowESPHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "Glow ESP %s", m_GlowESPHelper->IsEnabled() ? "enabled!" : "disabled!");
		}
	}

	if (m_BunnyHopHelper)
	{
		if (GetAsyncKeyState(m_BunnyHopHelper->GetEnableKey()) & 0x8000)
		{
			m_BunnyHopHelper->IsEnabled() ? m_BunnyHopHelper->Disable() : m_BunnyHopHelper->Enable();

			while (GetAsyncKeyState(m_BunnyHopHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "Bunny Hop %s", m_BunnyHopHelper->IsEnabled() ? "enabled!" : "disabled!");
		}
	}

	if (m_TriggerBotHelper)
	{
		if (GetAsyncKeyState(m_TriggerBotHelper->GetEnableKey()) & 0x8000)
		{
			m_TriggerBotHelper->IsEnabled() ? m_TriggerBotHelper->Disable() : m_TriggerBotHelper->Enable();

			while (GetAsyncKeyState(m_TriggerBotHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "Trigger bot %s", m_TriggerBotHelper->IsEnabled() ? "enabled!" : "disabled!");
		}

		if (m_TriggerBotHelper->IsHoldKeyEnabled())
		{
			if (GetAsyncKeyState(m_TriggerBotHelper->GetHoldKey()) & 0x8000)
			{
				while (GetAsyncKeyState(m_TriggerBotHelper->GetHoldKey()) & 0x8000)
					Sleep(1);

				m_TriggerBotHelper->PressHoldKey();

				Beep(750, 250);
				DEBUG_LOG(LL_SYS, "Trigger bot hold key %s", m_TriggerBotHelper->IsPressedToHoldkey() ? "enabled!" : "disabled!");
			}
		}
	}

	if (m_NoRecoilHelper)
	{
		if (GetAsyncKeyState(m_NoRecoilHelper->GetEnableKey()) & 0x8000)
		{
			m_NoRecoilHelper->IsEnabled() ? m_NoRecoilHelper->Disable() : m_NoRecoilHelper->Enable();

			while (GetAsyncKeyState(m_NoRecoilHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "No recoil %s", m_NoRecoilHelper->IsEnabled() ? "enabled!" : "disabled!");
		}
	}

	if (m_CSGOMiscHelper)
	{
		if (GetAsyncKeyState(m_CSGOMiscHelper->GetEnableKey()) & 0x8000)
		{
			m_CSGOMiscHelper->IsEnabled() ? m_CSGOMiscHelper->Disable() : m_CSGOMiscHelper->Enable();

			while (GetAsyncKeyState(m_CSGOMiscHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "Misc features %s", m_CSGOMiscHelper->IsEnabled() ? "enabled!" : "disabled!");
		}
	}

	if (m_AimBotHelper)
	{
		if (GetAsyncKeyState(m_AimBotHelper->GetEnableKey()) & 0x8000)
		{
			m_AimBotHelper->IsEnabled() ? m_AimBotHelper->Disable() : m_AimBotHelper->Enable();

			while (GetAsyncKeyState(m_AimBotHelper->GetEnableKey()) & 0x8000)
				Sleep(1);

			Beep(750, 250);
			DEBUG_LOG(LL_SYS, "Aimbot %s", m_AimBotHelper->IsEnabled() ? "enabled!" : "disabled!");
		}
	}
}

void CCSGOManager::OnUpdate()
{
#if HAX_MULTI_THREAD == false
	if (m_cEngineHelper->IsInGame())
	{
		if (pLocalEntity->Update(EEntityTypes::ENTITY_LOCAL))
		{
			if (m_GlowESPHelper)
				m_GlowESPHelper->OnUpdate();

			if (m_CSGOMiscHelper)
				m_CSGOMiscHelper->OnUpdate();

			if (m_BunnyHopHelper)
				m_BunnyHopHelper->OnUpdate();

			if (m_TriggerBotHelper)
				m_TriggerBotHelper->OnUpdate();

			if (m_NoRecoilHelper)
				m_NoRecoilHelper->OnUpdate();

			if (m_AimBotHelper)
				m_AimBotHelper->OnUpdate();
		}
	}
#endif
}

void CCSGOManager::Suspend()
{
	m_bIsSuspended = true;

	if (m_GlowESPHelper)
		m_GlowESPHelper->Suspend();

	if (m_BunnyHopHelper)
		m_BunnyHopHelper->Suspend();

	if (m_TriggerBotHelper)
		m_TriggerBotHelper->Suspend();

	if (m_NoRecoilHelper)
		m_NoRecoilHelper->Suspend();

	if (m_CSGOMiscHelper)
		m_CSGOMiscHelper->Suspend();

	if (m_AimBotHelper)
		m_AimBotHelper->Suspend();
}

void CCSGOManager::Resume()
{
	m_bIsSuspended = false;

	if (m_GlowESPHelper)
		m_GlowESPHelper->Resume();

	if (m_BunnyHopHelper)
		m_BunnyHopHelper->Resume();

	if (m_TriggerBotHelper)
		m_TriggerBotHelper->Resume();

	if (m_NoRecoilHelper)
		m_NoRecoilHelper->Resume();

	if (m_CSGOMiscHelper)
		m_CSGOMiscHelper->Resume();

	if (m_AimBotHelper)
		m_AimBotHelper->Resume();
}

// Thread
DWORD WINAPI CCSGOManager::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CCSGOManager*)lpParam;
	return This->ThreadRoutine();
}

DWORD CCSGOManager::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "CSGO Manager routine started!");

	InitHax();
	m_bIsStarted = true;

	while (1)
	{
		if (m_bDisabled)
		{
			DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
			Suspend();
			break;
		}

		OnUpdate();
		Sleep(1);
	}

	return 0;
}

bool CCSGOManager::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "CSGO Manager thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CCSGOManager::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "CSGO Manager thread terminated!");
		return true;
	}
	return false;
}
