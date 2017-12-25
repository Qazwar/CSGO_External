#include "main.h"
#include "Hax_AimBot.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"
#include "Hax_CSGO_Manager.h"

/*
	TODO

	* Hold key
	* Burst
	* Burst Interval
	* Autoshoot
	* Ammo check
	* Spot = head, neck, chest ... for aim
	* Multi-Spot = Select more than one aim spot
	* Auto duck
	* Auto scope
	* don't aim when localplayer is in Air
	* stop aim when aimtarget jumps
	* Best target bone
*/

// Hax
CAimBot::CAimBot(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_dwEnableKey(VK_F12),
	m_bHasEntity(false), m_iBestEntityID(0), m_flBestFov(9999.f), m_flBestDistance(1337.f), m_iLastLockedPositionOwner(0)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CAimBot::CAimBot");

	// Get entity data
	m_pLocalEntity = m_cCSGOManager->GetLocalEntity();
	if (!m_pLocalEntity || !m_pLocalEntity.get()) {
		DEBUG_LOG(LL_ERR, "Local entity initilization fail!");
		abort();
	}

	m_pCurrentEntity = std::make_shared<CEntity>(m_cCSGOManager);
	if (!m_pCurrentEntity || !m_pCurrentEntity.get()) {
		DEBUG_LOG(LL_ERR, "Current entity initilization fail!");
		abort();
	}

	m_pWeaponEntity = std::make_unique<CEntity>(m_cCSGOManager);
	if (!m_pWeaponEntity || !m_pWeaponEntity.get()) {
		DEBUG_LOG(LL_ERR, "m_pWeaponEntity entity initilization fail!");
		abort();
	}

	// Settings
	LoadConfigs();
}

CAimBot::~CAimBot()
{
	DEBUG_LOG(LL_SYS, "CAimBot::~CAimBot");

#if HAX_MULTI_THREAD == true
	if (IS_VALID_HANDLE(m_hThread))
	{
		DWORD dwThreadExitCode = 0;
		if (GetExitCodeThread(m_hThread, &dwThreadExitCode) && dwThreadExitCode == STILL_ACTIVE)
			::TerminateThread(m_hThread, EXIT_SUCCESS);

		CloseHandle(m_hThread);
	}
	m_hThread = INVALID_HANDLE_VALUE;

	m_bThreadIsStopped = false;
#endif
}

void CAimBot::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGOAimBotSettings();

	// Get Settings
	m_dwEnableKey = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetAimBotEnableKey();
}

// TODO: Move to config
auto flFOVAimBot = 2.7f;
auto flSmoothAimBot = 100.f;
auto vSmoothAimBot = Vector2(flSmoothAimBot, flSmoothAimBot);
Vector2 vPunchReductionAimBot[2] = {
	Vector2(2.f, 2.f),
	Vector2(2.f, 2.f)
};
auto eTargetBoneAimBot = 7;

bool CAimBot::IsAttackable()
{
	if (!m_pCurrentEntity->IsAlive_Hp())
		return false;

	auto iWeaponID = m_pLocalEntity->GetWeaponID();

	if (!m_pLocalEntity->WeaponIsShootable(iWeaponID))
		return false;

	if (m_pLocalEntity->WeaponIsSniper(iWeaponID) && !m_pLocalEntity->IsScoped())
		return false;

	if (iWeaponID == WEAPON_REVOLVER) /* TODO: Configure for revolver */
		return false;

	if (!m_pLocalEntity->CanAttack())
		return false;

	return true;
}

void CAimBot::ShootToTarget()
{
	do
	{
		m_pLocalEntity->SendForceAttack(true, m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(10UL, 0UL, 10UL, true), false);

	} while
	(
		m_pCurrentEntity->GetHealth() > 0 &&
		m_pCurrentEntity->GetIndex() == m_pLocalEntity->GetCrossHairID() &&
		m_pLocalEntity->WeaponCanBurst()
	);
}

void CAimBot::LockAimBot()
{
	if (!IsAttackable())
		return;

	// DEBUG_LOG(LL_SYS, "Aim locked to: %d", m_pCurrentEntity->GetIndex());
	auto vEyePos = m_pLocalEntity->GetEyePosition();
	auto vTargetPos = m_pCurrentEntity->GetBonePosition((CSPlayerBones)eTargetBoneAimBot);

	// DEBUG_LOG(LL_SYS, "Locked pos distance: %f", m_vLastLockedPosition.Distance(vTargetPos));
	//if (!m_vLastLockedPosition.IsNull() && m_vLastLockedPosition.Distance(vTargetPos) < 5.f && m_iLastLockedPositionOwner == m_pCurrentEntity->GetIndex()) // fix lock spam, cause screen shake
	//{
	//	DEBUG_LOG(LL_SYS, "Too short distance with last locked position, lock func passed! Distance: %f", m_vLastLockedPosition.Distance(vTargetPos));
	//	return;
	//}

	auto vAimAngle = m_cCSGOManager->GetEngineHelper()->CalculateAngle(vEyePos, vTargetPos);
	
	// if (bRCSEnabled && m_pLocalEntity->WeaponCanApplyRCS())
		vAimAngle -= (m_pLocalEntity->GetAimPunch() * m_cCSGOManager->GetEngineHelper()->CalculatePunchReduction(vPunchReductionAimBot[0], vPunchReductionAimBot[1]));
	
	auto vViewAngle = m_cCSGOManager->GetEngineHelper()->GetViewAngle();
	
	// if (bSmoothEnabled)
		vAimAngle = m_cCSGOManager->GetEngineHelper()->CalculateSmoothAngle(vViewAngle, vAimAngle, flSmoothAimBot);

	// if (b_SilentAim && !bSmoothEnabled)
	//	m_cCSGOManager->GetEngineHelper()->SetSendPackets(false);

	m_cCSGOManager->GetEngineHelper()->SetViewAngle(vAimAngle);

	// if (b_SilentAim && !bSmoothEnabled)
	//	m_cCSGOManager->GetEngineHelper()->SetSendPackets(true);

	m_iLastLockedPositionOwner	= m_pCurrentEntity->GetIndex();
	m_vLastLockedPosition		= vTargetPos;
}

bool CAimBot::IsEntityAttackable(int iEntityIndex)
{
	if (m_pLocalEntity->GetIndex() != m_pCurrentEntity->GetIndex())
	{
		if (m_pCurrentEntity->IsAlive_Hp() && !m_pCurrentEntity->IsDormant())
		{
			if (m_pLocalEntity->GetTeamNumber() != m_pCurrentEntity->GetTeamNumber())
			{
				auto vEyePos	= m_pLocalEntity->GetEyePosition();
				auto vTargetPos = m_pCurrentEntity->GetBonePosition((CSPlayerBones)eTargetBoneAimBot);

				float flFOV = 0.f;
				auto bIsInFov = m_cCSGOManager->GetEngineHelper()->IsInFOV(vEyePos, vTargetPos, flFOVAimBot, flFOV);
				if (flFOV <= flFOVAimBot * 10)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CAimBot::CheckEntity(int iEntityIndex)
{
	//DEBUG_LOG(LL_SYS, "Check entity called for: %d", iEntityIndex);

	if (m_pLocalEntity->GetIndex() != m_pCurrentEntity->GetIndex())
	{
		if (m_pCurrentEntity->IsAlive_Hp() && !m_pCurrentEntity->IsDormant())
		{
			if (m_pLocalEntity->GetTeamNumber() != m_pCurrentEntity->GetTeamNumber())
			{
				//DEBUG_LOG(LL_SYS, "Available target entity: %d", iEntityIndex);

				auto vEyePos	= m_pLocalEntity->GetEyePosition();
				auto vTargetPos = m_pCurrentEntity->GetBonePosition((CSPlayerBones)eTargetBoneAimBot);

				float flFOV		= 0.f;
				auto bIsInFov	= m_cCSGOManager->GetEngineHelper()->IsInFOV(vEyePos, vTargetPos, flFOVAimBot, flFOV);
				// DEBUG_LOG(LL_SYS, "Entity: %d InFov: %d Fov: %f Distance: %f", iEntityIndex, bIsInFov, flFOV, m_pLocalEntity->GetPosition().Distance(m_pCurrentEntity->GetPosition()));
				if (flFOV <= flFOVAimBot * 10)
				{
					DEBUG_LOG(LL_SYS, "Entity: %d Fov: %f Distance: %f", iEntityIndex, flFOV, m_pLocalEntity->GetPosition().Distance(m_pCurrentEntity->GetPosition()));
					// if (m_bLegitMode)
					{
						if (flFOV < m_flBestFov)
						{
							//DEBUG_LOG(LL_SYS, "New best target entity: %d fov: %f", iEntityIndex, flFOV);

							m_bHasEntity	= true;
							m_iBestEntityID = iEntityIndex;
							m_flBestFov		= flFOV;
							return true;
						}
					}

#if 0
					if (m_bRageMode)
					{
						auto flDistance = m_pLocalEntity->GetPosition().Distance(m_pCurrentEntity->GetPosition());
						if (flDistance < m_flBestDistance)
						{
							// DEBUG_LOG(LL_SYS, "New best target entity: %d fov: %f", i, flFOV);

							m_bHasEntity		= true;
							m_iBestEntityID		= i;
							m_flBestDistance	= flDistance;
							return true;
						}
					}
#endif
				}
			}
		}
	}
	return false;
}

void CAimBot::CheckBestEntity()
{
	if (m_bHasEntity)
	{
		if (m_pCurrentEntity->Update(EEntityTypes::ENTITY_BY_INDEX, m_iBestEntityID))
		{
			auto bIsSpotted	= m_pCurrentEntity->IsSpottedByMask();
			if (bIsSpotted)
			{
				auto vTargetPos = m_pCurrentEntity->GetBonePosition((CSPlayerBones)eTargetBoneAimBot);

				// TODO: Visible check
				//DEBUG_LOG(LL_SYS, "Best entity found!");

				auto flOldSensitivty = m_cCSGOManager->GetEngineHelper()->GetSensitivity();
				m_cCSGOManager->GetEngineHelper()->SetSensitivity(0.5);

				LockAimBot();

				// if (m_bAutoShoot)
					ShootToTarget();

				m_cCSGOManager->GetEngineHelper()->SetSensitivity(flOldSensitivty);
			}
		}
	}
}


void CAimBot::OnUpdate()
{
	if (m_bEnabled == true && m_bSuspended == false)
	{
		if (m_pLocalEntity->IsAlive_State() && !m_pLocalEntity->IsDormant())
		{
			auto dwWeaponEntity = m_pLocalEntity->GetWeaponEntity();
			if (dwWeaponEntity)
			{
				if (m_pWeaponEntity->Update(EEntityTypes::ENTITY_BY_WEAPON, dwWeaponEntity))
				{
					// Clear last best entity
					m_bHasEntity = false;
					m_iBestEntityID = 0;
					m_flBestFov = 9999.f;
					m_flBestDistance = 9999.f;

					int iCrossHairID = m_pLocalEntity->GetCrossHairID();
					if (iCrossHairID > 0 && iCrossHairID <= g_dwEntityCount)
					{
						if (m_pCurrentEntity->Update(EEntityTypes::ENTITY_BY_INDEX, iCrossHairID))
						{
							//DEBUG_LOG(LL_SYS, "InCross entity found: %d", iCrossHairID);

							if (CheckEntity(iCrossHairID)) {
								//DEBUG_LOG(LL_SYS, "InCross entity is available!");
								CheckBestEntity();
							}
						}
					}

					for (auto i = 0UL; i < g_dwEntityCount; i++)
					{
						if (m_pCurrentEntity->Update(EEntityTypes::ENTITY_BY_INDEX, i))
							CheckEntity(i);

						Sleep(1);
					}

					CheckBestEntity();
				}
			}
		}
	}
}

#if HAX_MULTI_THREAD == true
DWORD WINAPI CAimBot::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CAimBot*)lpParam;
	return This->ThreadRoutine();
}

DWORD CAimBot::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Aim bot routine started!");

	while (1)
	{
		if (m_bThreadIsStopped == true)
			break;

		if (m_cCSGOManager->GetEngineHelper()->IsInGame())
		{
			if (m_pLocalEntity->Update(EEntityTypes::ENTITY_LOCAL))
				OnUpdate();
		}

		Sleep(1);
	}

	return 0;
}

bool CAimBot::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "Aim bot thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CAimBot::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "Aim bot thread terminated!");
		return true;
	}
	return false;
}

void CAimBot::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif