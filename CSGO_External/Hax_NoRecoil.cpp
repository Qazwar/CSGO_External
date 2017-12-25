#include "main.h"
#include "Hax_NoRecoil.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Hax_CSGO_Manager.h"

// Hax
CNoRecoil::CNoRecoil(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_dwEnableKey(VK_F11)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CNoRecoil::CNoRecoil");

	// Get entity data
	m_pLocalEntity = m_cCSGOManager->GetLocalEntity();
	if (!m_pLocalEntity || !m_pLocalEntity.get()) {
		DEBUG_LOG(LL_ERR, "Local entity initilization fail!");
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

CNoRecoil::~CNoRecoil()
{
	DEBUG_LOG(LL_SYS, "CNoRecoil::~CNoRecoil");

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

void CNoRecoil::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGONoRecoilSettings();

	// Get Settings
	m_dwEnableKey	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetNoRecoilEnableKey();
	m_dEfficiency	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetNoRecoilEfficiency();
}

void CNoRecoil::OnUpdate()
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
					if (m_pWeaponEntity->GetAmmo() > 0)
					{
						if (m_pLocalEntity->WeaponCanApplyRCS())
						{
							auto vAdjustedPunch = Vector2(2.f * m_dEfficiency, 2.f * m_dEfficiency);
							Vector3 vNewPunch = m_pLocalEntity->GetAimPunch() * m_cCSGOManager->GetEngineHelper()->CalculatePunchReduction(vAdjustedPunch, vAdjustedPunch);

							if (m_pLocalEntity->GetShotsFired() >= 0 && m_pLocalEntity->CanAttack() && m_cCSGOManager->GetEngineHelper()->GetAttack())
							{
								auto vAimAngle = m_cCSGOManager->GetEngineHelper()->GetViewAngle();
								vAimAngle -= (vNewPunch - m_vOldPunch);
								auto vViewAngle = m_cCSGOManager->GetEngineHelper()->GetViewAngle();

								Vector3 vSmoothAimAngle = m_cCSGOManager->GetEngineHelper()->CalculateSmoothAngle(vViewAngle, vAimAngle, m_cCSGOManager->GetEngineHelper()->Random<float>(Vector2(50.f, 100.f) /* SmoothRecoilControl */));
								m_cCSGOManager->GetEngineHelper()->SetSendPackets(false);
								m_cCSGOManager->GetEngineHelper()->SetViewAngle(vSmoothAimAngle);
								m_cCSGOManager->GetEngineHelper()->SetSendPackets(true);
							}

							m_pLocalEntity->SetVisualPunch(Vector3(0.f));
							m_vOldPunch = vNewPunch;
						}
					}
				}
			}
		}
	}
}


#if HAX_MULTI_THREAD == true
DWORD WINAPI CNoRecoil::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CNoRecoil*)lpParam;
	return This->ThreadRoutine();
}

DWORD CNoRecoil::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "No recoil routine started!");

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

bool CNoRecoil::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "No recoil thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CNoRecoil::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "No recoil thread terminated!");
		return true;
	}
	return false;
}

void CNoRecoil::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif
