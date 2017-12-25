#pragma once
#include "Timer.h"
#include "HaxModuleIndex.h"

class CHaxControl : public std::enable_shared_from_this<CHaxControl>
{
	public:
		CHaxControl(const std::string & szProcessName, DWORD dwProcessId, HANDLE hSonicHandle);
		~CHaxControl();

		bool	CreateHaxThread();
		bool	TerminateHaxThread();

		void	StartHax();
		void	StopHax();

		auto	GetHaxModule()		{ return m_cHaxModule; };
		auto	GetProcessHandle()	{ return m_hProcessHandle; };

	protected:
		bool	HaxIsUsable();

		DWORD	HaxRoutine(void);
		static DWORD WINAPI StartHaxRoutine(LPVOID lpParam);

	private:
		CTimer <std::chrono::milliseconds>	antiCheckTimer;

		std::shared_ptr <CHaxModule>		m_cHaxModule;

		HANDLE		m_hHaxThread;
		bool		m_bIsStartSignalReceived;
		bool		m_bIsStopSignalReceived;

		std::string m_szProcessName;
		DWORD		m_dwProcessId;
		HANDLE		m_hProcessHandle;


		/* TODO: 
		Store all created threads on a map
		*/
		std::map <DWORD /* dwThreadCode */, DWORD /* dwThreadId */> m_mapThreads;
	public:
		const auto & GetThreadMap() { return m_mapThreads; };
		auto ThreadMapIsEmpty() { return m_mapThreads.begin() == m_mapThreads.end(); };
		auto GetThreadMapSize() { return m_mapThreads.size(); };

		auto BindSelfThread(DWORD dwThreadCode, DWORD dwThreadId)
		{
			m_mapThreads[dwThreadCode] = dwThreadId;
		}
		auto IsSelfThreadFromThreadCode(DWORD dwThreadCode)
		{
			for (auto& curThread : m_mapThreads)
				if (curThread.first == dwThreadCode)
					return true;
			return false;
		}
		auto IsSelfThreadFromThreadId(DWORD dwThreadId)
		{
			for (auto& curThread : m_mapThreads)
				if (curThread.second == dwThreadId)
					return true;
			return false;
		}
		auto GetSelfThreadId(DWORD dwThreadCode)
		{
			auto it = m_mapThreads.find(dwThreadCode);
			return it->second;
		}
};