#pragma once
#include "HaxConfigParser.h"
#include "Defines.h"

extern class CEntity;
extern class CCSGOManager;

class CCSGOMisc
{
	public:
		CCSGOMisc(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CCSGOMisc();

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

		DWORD								m_dwEnableKey;
		bool								m_bNoFlashIsEnabled;
		bool								m_bNoHandsIsEnabled;
		bool								m_bAutoPistolIsEnabled;
		bool								m_bChangeFovIsEnabled;
		int									m_iFovValue;
		DWORD								m_dwFovKey;
		bool								m_bSlowAimIsEnabled;
		double								m_dSlowAimSensivity;
		bool								m_bFakeLagIsEnabled;
		bool								m_dwFakeLagKey;
		float								m_flOldSensitivty;

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
