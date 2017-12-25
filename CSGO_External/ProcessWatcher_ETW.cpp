#include "main.h"
#include "ETW.h"
#include "ProcessName.h"
#include "Defines.h"

enum ENotificationTypes
{
	ProcessCreated = 6,
	ProcessTerminated = 7
};

CETW::CETW(const std::string & szWatchedAppName, const std::string & szTargetAppName) :
	m_szTargetAppName(szTargetAppName), m_szWatchedAppName(szWatchedAppName), 
	m_bWatcherIsTerminated(false)
{
}

CETW::~CETW()
{
	m_szTargetAppName.clear();
	m_szWatchedAppName.clear();

	m_bWatcherIsTerminated = false;
}

void CETW::CheckProcessEvent(HANDLE hSonicHandle, DWORD dwProcessID, bool bIsCreated)
{
	std::string szProcessName;
	if (bIsCreated == false)
	{
		auto safeProcHandle = std::make_unique<CSafeProcessHandle>(dwProcessID);
		HANDLE hHandle = safeProcHandle->CreateProcessHandle();

		if (IS_VALID_HANDLE(hHandle)) {
			szProcessName = CProcessName::GetProcessName(hHandle);
			CloseHandle(hHandle);
		}

		safeProcHandle->ClearVeriables();
	}
	if (szProcessName.empty())
		szProcessName = g_haxApp->GetFunctionsInstance()->GetProcessNameFromProcessId(dwProcessID);

	if (szProcessName.empty() == false && szProcessName.find(m_szTargetAppName) != std::string::npos)
		g_haxApp->HaxAppWatcher(dwProcessID, szProcessName, bIsCreated, hSonicHandle);
}

void CETW::GetHandleForIOProcesses(HANDLE hIOPort)
{
	DWORD numberOfBytesTransferred;
	ULONG_PTR completionKey;
	LPOVERLAPPED overlapped;

	while (GetQueuedCompletionStatus(hIOPort, &numberOfBytesTransferred, &completionKey, &overlapped, INFINITE))
	{
		if (m_bWatcherIsTerminated)
			break;

		auto dwProcessId = reinterpret_cast<DWORD>(overlapped);
		// DEBUG_LOG(LL_SYS, "Handled a process to ETW Watcher! PID %u Bytes: %d Key: %p", dwProcessId, numberOfBytesTransferred, completionKey);

		auto hSonicProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if (IS_VALID_HANDLE(hSonicProcess))
		{
			// Find process name of handled process
			auto szProcessName = CProcessName::GetProcessName(hSonicProcess);
			if (szProcessName.empty())
				szProcessName = g_haxApp->GetFunctionsInstance()->GetProcessNameFromProcessId(dwProcessId);
			
			// DEBUG_LOG(LL_SYS, "Handled a process to ETW Watcher! %u(%p) -> %s", dwProcessId, hSonicProcess, szProcessName.c_str());

			// Compare with target process name
			if (szProcessName.find(m_szTargetAppName) != std::string::npos)
				CheckProcessEvent(hSonicProcess, dwProcessId, numberOfBytesTransferred == ENotificationTypes::ProcessCreated);
			else
				CloseHandle(hSonicProcess);
		}
	}
}

void CETW::TerminateWatcher()
{
	m_bWatcherIsTerminated = true;
}

void CETW::CreateWatcher()
{
	// Predefined veriables
	std::vector <HANDLE> vWatchedProcessHandles;
	std::vector <DWORD>  vWatchedProcessList;
	bool bAssignedToAnyProcess = false;
	JOBOBJECT_ASSOCIATE_COMPLETION_PORT jobIOport;

	DEBUG_LOG(LL_SYS, "ETW Watcher started! Watched: %s Target: %s", m_szWatchedAppName.c_str(), m_szTargetAppName.c_str());

	// Common
	if (m_szWatchedAppName.empty())
		goto clean;

	// Process infos
	vWatchedProcessList = g_haxApp->GetFunctionsInstance()->GetProcessIdsFromProcessName(m_szWatchedAppName.c_str());
	if (vWatchedProcessList.empty())
		goto clean;

	/* Creating job to get instant notification of new child processes */
	auto hIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	if (!hIOPort)
		goto clean;

	auto hJobObject = CreateJobObjectA(NULL, NULL);
	if (!hJobObject)
		goto clean;
	
	jobIOport.CompletionKey = NULL;
	jobIOport.CompletionPort = hIOPort;
	auto bSetInfoJobObjStatus = SetInformationJobObject(hJobObject, JobObjectAssociateCompletionPortInformation, &jobIOport, sizeof(jobIOport));
	if (!bSetInfoJobObjStatus)
		goto clean;
	
	for (auto & dwCurrentWatchedProcessId : vWatchedProcessList)
	{
		auto hWatchedProcess = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, TRUE, dwCurrentWatchedProcessId);
		if (hWatchedProcess && hWatchedProcess != INVALID_HANDLE_VALUE)
		{
			if (AssignProcessToJobObject(hJobObject, hWatchedProcess))
				bAssignedToAnyProcess = true;

			vWatchedProcessHandles.emplace_back(hWatchedProcess);
		}
	}

	if (!bAssignedToAnyProcess)
		goto clean;

	// Create IO watcher
	GetHandleForIOProcesses(hIOPort);
 
	// keep alive
	WaitForSingleObject(hIOPort, INFINITE);

	/* Cleanup before returning handle */
clean:
	for (auto & hCurrentWatchedProcess : vWatchedProcessHandles)
	{
		if (hCurrentWatchedProcess && hCurrentWatchedProcess != INVALID_HANDLE_VALUE)
			CloseHandle(hCurrentWatchedProcess);
	}

	if (hIOPort && hIOPort != INVALID_HANDLE_VALUE)
		CloseHandle(hIOPort);

	if (hJobObject && hJobObject != INVALID_HANDLE_VALUE)
		CloseHandle(hJobObject);
}

