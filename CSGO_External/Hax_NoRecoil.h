#pragma once
#include "HaxConfigParser.h"
#include "Vector3.h"
#include "Defines.h"

extern class CEntity;
extern class CCSGOManager;

class CNoRecoil
{
	public:
		CNoRecoil(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CNoRecoil();

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
		std::shared_ptr <CEntity>			m_pWeaponEntity;

		// helper
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;

		// settings
		bool								m_bEnabled;
		bool								m_bSuspended;

		Vector3								m_vOldPunch;
		DWORD								m_dwViewAngle;
		DWORD								m_dwVecPunch;

		DWORD								m_dwEnableKey;
		double								m_dEfficiency;

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
