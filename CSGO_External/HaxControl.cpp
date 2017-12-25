#include "main.h"
#include "HaxControl.h"
#include "HaxModuleIndex.h"
#include "Defines.h"
#include "Antis.h"

CHaxControl::CHaxControl(const std::string & szProcessName, DWORD dwProcessId, HANDLE hSonicHandle) :
	m_szProcessName(szProcessName), m_dwProcessId(dwProcessId), m_hHaxThread(INVALID_HANDLE_VALUE), 
	m_bIsStartSignalReceived(false), m_bIsStopSignalReceived(false),
	m_cHaxModule(nullptr), m_hProcessHandle(hSonicHandle)
{
	DEBUG_LOG(LL_SYS, "CHaxControl::CHaxControl");
	antiCheckTimer.reset();
}

CHaxControl::~CHaxControl()
{
	antiCheckTimer.reset();

	if (m_cHaxModule && m_cHaxModule.get())
	{
		m_cHaxModule.reset();
		m_cHaxModule = nullptr;
	}

	if (IS_VALID_HANDLE(m_hHaxThread))
	{
		DWORD dwThreadExitCode = 0;
		if (GetExitCodeThread(m_hHaxThread, &dwThreadExitCode) && dwThreadExitCode == STILL_ACTIVE)
			TerminateThread(m_hHaxThread, EXIT_SUCCESS);

		CloseHandle(m_hHaxThread);
	}
	m_hHaxThread = INVALID_HANDLE_VALUE;

	m_bIsStartSignalReceived = false;
	m_bIsStopSignalReceived = false;

	m_szProcessName.clear();
	m_dwProcessId = 0;

	if (IS_VALID_HANDLE(m_hProcessHandle))
		CloseHandle(m_hProcessHandle);
	m_hProcessHandle = INVALID_HANDLE_VALUE;
}


DWORD WINAPI CHaxControl::StartHaxRoutine(LPVOID lpParam)
{
	auto This = (CHaxControl*)lpParam;
	return This->HaxRoutine();
}

bool CHaxControl::HaxIsUsable()
{
	if (m_bIsStopSignalReceived == true) {
		DEBUG_LOG(LL_ERR, "Hax stop signal received! Hax thread is will be stopped!");
		return false;
	}

	if (antiCheckTimer.diff() > 15000)
	{
		DEBUG_LOG(LL_SYS, "Anti check started!");

#ifndef _DEBUG
		if (CAntis::CheckCloseProtectedHandle()) {
			DEBUG_LOG(LL_ERR, "Debugger detected! Hax thread is will be stopped!");
			return false;
		}
#endif

		antiCheckTimer.reset();
	}

	return true;
}

DWORD CHaxControl::HaxRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Hax routine started!");

	if (HaxIsUsable() == false)
		return 0;

	m_cHaxModule = std::make_shared<CHaxModule>(m_dwProcessId);
	if (!m_cHaxModule || !m_cHaxModule.get())
		return 0;

	m_cHaxModule->InitHax();

	while (1)
	{
		if (m_bIsStartSignalReceived == false) {
			Sleep(10);
			continue;
		}

		if (HaxIsUsable() == false)
			break;

		m_cHaxModule->ExecHax();

		Sleep(1);
	}

	m_cHaxModule.reset();
	m_cHaxModule = nullptr;
	
	return 0;
}


void CHaxControl::StartHax()
{
	m_bIsStartSignalReceived = true;
	antiCheckTimer.reset();
}

void CHaxControl::StopHax()
{
	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hHaxThread));
	m_bIsStopSignalReceived = true;
	antiCheckTimer.reset();
}


bool CHaxControl::CreateHaxThread()
{
	DWORD dwThreadId = 0;
	m_hHaxThread = CreateThread(nullptr, 0, StartHaxRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hHaxThread)) {
		DEBUG_LOG(LL_SYS, "Hax thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CHaxControl::TerminateHaxThread()
{
	auto bTerminateRet = TerminateThread(m_hHaxThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "Hax thread terminated!");
		return true;
	}
	return false;
}
