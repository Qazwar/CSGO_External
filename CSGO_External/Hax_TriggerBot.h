#pragma once
#include "HaxConfigParser.h"
#include "Defines.h"

enum ETriggerBotBones
{
	RANDOM_BONE = 99,
	NEAREST_BONE = 98
};

enum ETriggerBotWorkTypes
{
	WORKTYPE_NULL,
	WORKTYPE_INCROSS,
	WORKTYPE_HITBOX
};

extern class CEntity;
extern class CCSGOManager;

class CTriggerBot
{
	public:
		CTriggerBot(const std::shared_ptr<CCSGOManager> & csgoManager);
		~CTriggerBot();

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

		bool	IsHoldKeyEnabled() const	{ return m_bUseHoldKey; };
		DWORD	GetHoldKey()				{ return m_dwHoldKey;	};
		bool	IsPressedToHoldkey() const	{ return m_bPressedToHoldKey;	};
		void	PressHoldKey()				{ m_bPressedToHoldKey = !m_bPressedToHoldKey; };

	protected:
		DWORD	GetBestBoneId(std::map <DWORD, double> boneMap);

		bool	CalculateAngles();
		bool	SetAnglesToTarget(DWORD dwTargetBone);
		void	AttackToTarget(int iWeaponID);
		void	TriggerShot();
		void	TriggerBurst();

		void	HaxRoutine(int iEntity);
		void	HitboxRoutine();
		void	InCrossRoutine();

	private:
		// process data
		HWND								m_hWnd;

		// data
		std::shared_ptr <CEntity>			m_pLocalEntity;
		std::shared_ptr <CEntity>			m_pTargetEntity;
		std::shared_ptr <CEntity>			m_pWeaponEntity;

		// helper
		std::shared_ptr <CCSGOManager>		m_cCSGOManager;
		std::vector<int>					m_vHitboxes;

		// settings
		bool								m_bEnabled;
		bool								m_bSuspended;
		bool								m_bPressedToHoldKey;

		DWORD								m_dwWorkType;
		DWORD								m_dwEnableKey;
		DWORD								m_dwInterval;
		bool								m_bIsBurst;
		DWORD								m_dwBurstInterval;
		DWORD								m_dwBurstCount;
		bool								m_bAttackAsLongAsAlive;
		bool								m_bCheckSpawnProtect;
		bool								m_bAimLock;
		bool								m_bAutoCrouch;
		bool								m_bBlockOnJump;
		bool								m_bBlockOnMove;
		DWORD								m_dwTargetBone;
		float								m_dFov;
		bool								m_bSmoothEnabled;
		float								m_flSmoothValue;
		bool								m_bTraceRay;

		bool								m_bEnablePreWaitInterval;
		bool								m_bEnablePostWaitInterval;
		bool								m_bEnableBothWaitInterval;
		int									m_iRifleInterval;
		int									m_iPistolInterval;
		int									m_iSniperInterval;
		int									m_iSMGInterval;
		int									m_iShotgunInterval;
		int									m_iMGInterval;
		int									m_iRevolverInterval;

		bool								m_bEnableAutoSwitch;
		bool								m_bEnableScopeCheck;
		bool								m_bEnableAutoReZoom;
		DWORD								m_dwExtraSniperDelay;

		bool								m_bUseHoldKey;
		DWORD								m_dwHoldKey;

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
