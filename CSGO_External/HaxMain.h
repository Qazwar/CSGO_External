#pragma once

#include "DynamicWinAPI.h"
#include "Functions.h"
#include "spdlog_wrapper.h"
#include "WMI.h"
#include "ETW.h"
#include "ProcessHelper.h"
#include "HaxControl.h"

typedef struct _handled_process_data
{
	DWORD dwProcessId;
	std::shared_ptr <CHaxControl> haxController;
} SHandledProcessData, *PHandledProcessData;

class CHaxApp : public std::enable_shared_from_this<CHaxApp>
{
	public:
		CHaxApp();
		~CHaxApp();
	
		void	PreInitialize();
		void	Initialize();
		void	InitializeSelfSecurity();

		void	HaxAppWatcher(DWORD dwProcessId, const std::string & szProcessName, bool bIsCreated, HANDLE hSonicHandle = INVALID_HANDLE_VALUE);

	public:
		auto	LogHelperInstance()			{ return logHelper;				};
		auto	GetETWInstance()			{ return ETW;					};
		auto	GetWMIInstance()			{ return WMI;					};
		auto	GetFunctionsInstance()		{ return Functions;				};
		auto	GetWinApiHelperInstance()	{ return winAPIHelper;			};

		auto	AppIsInitiliazed()			{ return m_bAppIsInitiliazed;	};
		auto	GetTargetFileName()			{ return m_szTargetFilename;	};
		auto	IsETWWatcherEnabled()		{ return m_bWatchFromETW;		};

		auto	GetHandledProcesses()		{ return m_vHandledProcessList; };

	protected:
		auto			GetTargetProcessInformations();

		inline bool		ClassInitIsFailed();

	private:
		bool			m_bAppIsInitiliazed;

		std::string		m_szTargetFilename;
		std::string		m_szTargetParentname;
		bool			m_bWatchFromETW;

		std::shared_ptr <CLogger>			logHelper;
		std::shared_ptr <CETW>				ETW;
		std::shared_ptr <CWMI>				WMI;
		std::shared_ptr <CFunctions>		Functions;
		std::shared_ptr <CDynamicWinapi>	winAPIHelper;

		std::vector < std::shared_ptr <SHandledProcessData> >	m_vHandledProcessList;
};

