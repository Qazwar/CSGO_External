#include "main.h"
#include "HaxMain.h"
#include "spdlog_wrapper.h"
#include "XOR.h"
#include "Defines.h"
#include "HaxControl.h"
#include "WMI.h"
#include "Functions.h"
#include "DynamicWinAPI.h"
#include "ProcessHelper.h"
#include "MiniDump.h"
#include "VersionHelpers.h"

CHaxApp::CHaxApp() :
	m_bAppIsInitiliazed(false), m_szTargetFilename(""), m_szTargetParentname(""), m_bWatchFromETW(false)
{
	m_vHandledProcessList.clear();
}

CHaxApp::~CHaxApp()
{
	m_vHandledProcessList.clear();
}

auto CHaxApp::GetTargetProcessInformations()
{
	auto cSettings = std::make_unique<CConfigParser>(XOR("H4x.cfg"));
	if (!cSettings || !cSettings.get())
		return false;
	
	if (cSettings->Load() == false)
		return false;

	if (cSettings->IsExistKey(XOR("process.filename")) == false)
		return false;
	
	if (cSettings->IsExistKey(XOR("process.watchfrometw")) == false)
		return false;

	if (cSettings->IsExistKey(XOR("process.parentname")) == false)
		return false;

	m_szTargetFilename = cSettings->GetString(XOR("process.filename"));
	if (m_szTargetFilename.empty())
		return false;

	m_bWatchFromETW = cSettings->GetBoolean(XOR("process.watchfrometw"));
	m_szTargetParentname = cSettings->GetString(XOR("process.parentname"));

	if (m_bWatchFromETW && m_szTargetParentname.empty() == true)
		return false;
	
	return true;
}

void CHaxApp::PreInitialize()
{
	// Init Debug Console
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "a", stdout);
	freopen("CONIN$", "r", stdin);
#endif

	// Load target process informations
	if (GetTargetProcessInformations() == false) {
		Logf(CUSTOM_LOG_FILENAME, XOR("Target process informations initialization failed!\n"));
		abort();
		return;
	}

	// Init classes
	logHelper		= std::make_shared<CLogger>(XOR("H4x"), CUSTOM_LOG_FILENAME);
	ETW				= std::make_shared<CETW>(m_szTargetParentname.c_str(), m_szTargetFilename.c_str());
	WMI				= std::make_shared<CWMI>(m_szTargetFilename.c_str());
	Functions		= std::make_shared<CFunctions>();
	winAPIHelper	= std::make_shared<CDynamicWinapi>();

	if (ClassInitIsFailed()) {
		Logf(CUSTOM_LOG_FILENAME, XOR("Classes allocation failed!\n"));
		abort();
	}
	m_bAppIsInitiliazed = true;
}

bool CHaxApp::ClassInitIsFailed()
{
	if (logHelper && logHelper.get()) {
		DEBUG_LOG(LL_SYS, "logHelper:\t\t%p-%p",		logHelper, logHelper.get());
		DEBUG_LOG(LL_SYS, "ETW:\t\t%p-%p",				ETW, ETW.get());
		DEBUG_LOG(LL_SYS, "WMI:\t\t%p-%p",				WMI, WMI.get());
		DEBUG_LOG(LL_SYS, "Functions:\t\t%p-%p",		Functions, Functions.get());
		DEBUG_LOG(LL_SYS, "winAPIHelper:\t%p-%p",		winAPIHelper, winAPIHelper.get());
	}
#ifdef _DEBUG
	else {
		Logf(CUSTOM_LOG_FILENAME, "Log helper initilization failed!\n");
	}
#endif

	return (
		logHelper		== nullptr	|| logHelper.get()		== nullptr	|| 
		ETW				== nullptr	|| ETW.get()			== nullptr	||
		WMI				== nullptr	|| WMI.get()			== nullptr	||
		Functions		== nullptr	|| Functions.get()		== nullptr  ||
		winAPIHelper	== nullptr	|| winAPIHelper.get()	== nullptr
	);
}

void CHaxApp::HaxAppWatcher(DWORD dwProcessId, const std::string & szProcessName, bool bIsCreated, HANDLE hSonicHandle)
{
	DEBUG_LOG(LL_SYS, "PID: %u triggered! IsCreated: %d Process Name: %s", dwProcessId, bIsCreated, szProcessName.c_str());

	if (bIsCreated == false) // It's a terminated process
	{
		if (m_vHandledProcessList.empty() == false) // Handled process list isn't empty
		{
			for (auto & curProcData : m_vHandledProcessList) // Iterate handled processes
			{
				if (curProcData->dwProcessId == dwProcessId)
				{
					if (curProcData->haxController)
					{
						curProcData->haxController->StopHax(); // Send stop signal

						bool bTerminateRet = curProcData->haxController->TerminateHaxThread(); // Terminate hax thread
						if (bTerminateRet == false) {
							DEBUG_LOG(LL_SYS, "Hax thread terminate fail!");
							return;
						}

						for (auto iter = m_vHandledProcessList.begin(); iter != m_vHandledProcessList.end(); ++iter) // Remove hax process datas from handled process list
						{
							if (*iter == curProcData)
							{
								m_vHandledProcessList.erase(iter);
								break;
							}
						}

						curProcData->haxController.reset(); // Destroy hax controller class
						curProcData->haxController = nullptr;

						curProcData.reset(); // Destroy hax process data struct
						curProcData = nullptr;
					}
				}
			}
		}

		DEBUG_LOG(LL_SYS, "Process: %u removed from handled processes!", dwProcessId);
		return;
	}

	// useless if already handled pid
	if (m_vHandledProcessList.empty() == false)
		for (auto & curProcData : m_vHandledProcessList)
			if (curProcData->dwProcessId == dwProcessId)
				return;


	auto haxControl = std::make_shared<CHaxControl>(szProcessName, dwProcessId, hSonicHandle);

	bool bCreateRet = haxControl->CreateHaxThread();
	if (bCreateRet == false) {
		DEBUG_LOG(LL_ERR, "Hax thread create fail!");
		return;
	}
	haxControl->StartHax();

	auto curProcData				= std::make_shared<SHandledProcessData>();
		 curProcData->dwProcessId	= dwProcessId;
		 curProcData->haxController = haxControl;

	m_vHandledProcessList.push_back(curProcData);
}

void CHaxApp::Initialize()
{
	// Create mini dump handler
	CMiniDump::InitMiniDump();

	// Init WinAPI Table
	winAPIHelper->Initialize();

	// Init self security
	InitializeSelfSecurity();

	// Init process watcher
	if (m_bWatchFromETW)
		ETW->CreateWatcher(); // FIXME: Crash on handled process termination
	else
		WMI->CreateWatcher();
}
