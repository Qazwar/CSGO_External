#pragma once
#include "HaxConfigParser.h"
#include "Defines.h"

extern class CEntity;
extern class CCSGOManager;

typedef struct _aimbot_entity_informations
{
	int		iEntity;
	float	flFOV;
	float	flDistance;
} SAimBotInfos, *PAimBotInfos;

class CAimBot
{
	public:
		CAimBot(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CAimBot();

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

	protected:
		bool			IsAttackable();
		void			ShootToTarget();
		void			LockAimBot();
		bool			IsEntityAttackable(int iEntityIndex);
		bool			CheckEntity(int iEntityIndex);
		void			CheckBestEntity();

	private:
		// data
		std::shared_ptr <CEntity>			m_pLocalEntity;
		std::shared_ptr <CEntity>			m_pCurrentEntity;
		std::shared_ptr <CEntity>			m_pWeaponEntity;

		// helper
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;

		// data
		int									m_iLastLockedPositionOwner;
		Vector3								m_vLastLockedPosition;
		bool								m_bHasEntity;
		int									m_iBestEntityID;
		float								m_flBestFov;
		float								m_flBestDistance;

		// settings
		bool								m_bEnabled;
		bool								m_bSuspended;

		DWORD								m_dwEnableKey;


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
