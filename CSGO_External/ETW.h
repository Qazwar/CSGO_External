#pragma once

class CETW : public std::enable_shared_from_this<CETW>
{
	public:
		CETW(const std::string & szWatchedAppName, const std::string & szTargetAppName);
		~CETW();

		void CreateWatcher();
		void TerminateWatcher();

	protected:
		void CheckProcessEvent(HANDLE hSonicHandle, DWORD dwProcessID, bool bIsCreated);
		void GetHandleForIOProcesses(HANDLE hIOPort);

	private:
		std::string m_szTargetAppName;
		std::string m_szWatchedAppName;

		bool		m_bWatcherIsTerminated;
};

