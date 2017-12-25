#pragma once
#include "HaxConfigParser.h"
#include "Vector3.h"
#include "Defines.h"

extern class CEntity;
extern class CCSGOManager;

class CBunnyHop
{
	public:
		CBunnyHop(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CBunnyHop();

		// common
		void	LoadConfigs();
		void	OnUpdate();
	
		// dynamics
		void	Enable()			{ m_bEnabled = true;	};
		void	Disable()			{ m_bEnabled = false;	};
		bool	IsEnabled() const	{ return m_bEnabled;	};

		void	Suspend()			{ m_bSuspended = true;	};
		void	Resume()			{ m_bSuspended = false;	};
		bool	IsSuspended() const	{ return m_bSuspended;	};

		DWORD	GetEnableKey()		{ return m_dwEnableKey; };

	private:
		// data
		std::shared_ptr <CEntity>			m_pLocalEntity;

		// helper
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;

		// settings
		bool								m_bEnabled;
		bool								m_bSuspended;
		Vector3								m_vPrevViewAngle;

		DWORD								m_dwEnableKey;
		DWORD								m_dwJumpKey;
		DWORD								m_dwReleaseInterval;
		bool								m_bUseAutoStrafe;

#if HAX_MULTI_THREAD == true
	public:
		bool					CreateThread();
		bool					TerminateThread();

		void					SendStopSignal();

	protected:
		DWORD					ThreadRoutine(void);
		static DWORD WINAPI		StartThreadRoutine(LPVOID lpParam);

	private:
		HANDLE					m_hThread;
		bool					m_bThreadIsStopped;
#endif
};
