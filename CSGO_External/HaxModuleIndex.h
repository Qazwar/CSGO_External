#pragma once
#include "NetVarManager.h"
#include "HaxConfigParser.h"
#include "HaxData.h"
#include "Hax_CSGO_Manager.h"

class CHaxModule
{
	public:
		CHaxModule(DWORD dwProcessId);
		~CHaxModule();

		void	InitHax();
		void	ExecHax();
		void	EnableHax();
		void	DisableHax();
		void	SuspendHax();
		void	ResumeHax();
		bool	IsStarted();

		void	DestroyMinimizeCheck();

	public:
		auto	GetCSGOManager()				{ return m_CSGOManager; };

		DWORD	GetAttachedPID()				{ return m_dwProcessId; };
		DWORD	GetAttachedGameCode()			{ return m_dwGameCode; };

	protected:
		bool					InitCSGO(bool bUsePreDefinedPointers);

		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		DWORD								m_dwProcessId;
		DWORD								m_dwGameCode;

		HANDLE								m_hMinimizeCheckThread;
		bool								m_bHookTerminated;
		HWINEVENTHOOK						m_hHook;

		std::shared_ptr <CHaxData>			m_cHaxData;

		std::shared_ptr <CCSGOManager>		m_CSGOManager;
};

