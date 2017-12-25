#include "main.h"
#include "Hax_CSGO_Misc.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"
#include "Hax_CSGO_Manager.h"

// Hax
CCSGOMisc::CCSGOMisc(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_dwEnableKey(VK_F5),
	m_bNoFlashIsEnabled(false), m_bNoHandsIsEnabled(false), m_bAutoPistolIsEnabled(false), m_bChangeFovIsEnabled(false), m_iFovValue(0), m_dwFovKey(VK_F12), m_bSlowAimIsEnabled(false), m_dSlowAimSensivity(0), m_bFakeLagIsEnabled(false), m_dwFakeLagKey(0), m_flOldSensitivty(0.f)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CCSGOMisc::CCSGOMisc");

	// Get entity data
	m_pLocalEntity = m_cCSGOManager->GetLocalEntity();
	if (!m_pLocalEntity || !m_pLocalEntity.get()) {
		DEBUG_LOG(LL_ERR, "Local entity initilization fail!");
		abort();
	}

	// Settings
	LoadConfigs();

	// hax settings
	m_flOldSensitivty = m_cCSGOManager->GetEngineHelper()->GetSensitivity();
}

CCSGOMisc::~CCSGOMisc()
{
	DEBUG_LOG(LL_SYS, "CCSGOMisc::~CCSGOMisc");

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

void CCSGOMisc::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGOMiscSettings();

	// Get Settings
	m_dwEnableKey			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetMiscEnableKey();
	m_bNoFlashIsEnabled		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetNoFlashEnabled();
	m_bNoHandsIsEnabled		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetNoHandsEnabled();
	m_bAutoPistolIsEnabled	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetAutoPistolEnabled();
	m_bChangeFovIsEnabled	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetChangeFov();
	m_iFovValue				= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFovValue();
	m_dwFovKey				= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFovKey();
	m_bSlowAimIsEnabled		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSlowAimEnabled();
	m_dSlowAimSensivity		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSlowAimSensivity();
	m_bFakeLagIsEnabled		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFakeLagEnabled();
	m_dwFakeLagKey			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetFakeLagKey();
}

void CCSGOMisc::OnUpdate()
{
	if (m_bEnabled == true && m_bSuspended == false)
	{
		if (m_bNoFlashIsEnabled)
		{
			if (m_pLocalEntity->GetFlashMaxAlpha() > 0.5f)
				m_pLocalEntity->SetFlashMaxAlpha(0.f);
		}

		if (m_bNoHandsIsEnabled)
		{
			m_pLocalEntity->SetModelIndex(1);
		}

		if (m_bAutoPistolIsEnabled)
		{
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (m_pLocalEntity->IsShotsFired())
				{
					if (m_pLocalEntity->IsAlive_State())
					{
						if (!m_cCSGOManager->GetEngineHelper()->IsMouseEnabled())
						{
							if (m_pLocalEntity->WeaponIsPistol())
								m_pLocalEntity->SendForceAttack();				
						}
					}
				}
			}
		}

		if (m_bChangeFovIsEnabled)
		{
			if (GetAsyncKeyState(m_dwFovKey) & 0x8000)
			{
				// reset fov
				if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
				{
					while (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
						Sleep(1);

					m_pLocalEntity->SetFov(65); // idk default value
					return;
				}

				auto pWeaponEntity = std::make_unique<CEntity>(m_cCSGOManager);
				if (pWeaponEntity && pWeaponEntity.get())
				{
					auto dwWeaponEntity = m_pLocalEntity->GetWeaponEntity();
					if (dwWeaponEntity)
					{
						if (pWeaponEntity->Update(EEntityTypes::ENTITY_BY_WEAPON, dwWeaponEntity))
						{
							auto dwOldFov	= m_pLocalEntity->GetFov();
							auto iWeaponID	= m_pLocalEntity->GetWeaponID();
							auto bScoped	= m_pLocalEntity->IsScoped();
							auto iZoomLevel = pWeaponEntity->GetZoomLevel();

							if (bScoped && iZoomLevel == 1)
								m_pLocalEntity->SetFov(40);
							else if (bScoped && iZoomLevel == 2)
								m_pLocalEntity->SetFov(10);
							else if (iWeaponID != 41 && iWeaponID != 59 && iWeaponID != 42)
								m_pLocalEntity->SetFov(50);
							else
								m_pLocalEntity->SetFov(m_iFovValue);
						}
					}
				}
			}
		}

		if (m_bSlowAimIsEnabled)
		{
			int iCrossHairID = m_pLocalEntity->GetCrossHairID();
			if (iCrossHairID > 0 && iCrossHairID <= g_dwEntityCount)
			{
				auto pCurrentEntity = std::make_unique<CEntity>(m_cCSGOManager);
				if (pCurrentEntity && pCurrentEntity.get())
				{
					if (pCurrentEntity->Update(EEntityTypes::ENTITY_BY_INDEX, iCrossHairID))
					{
						if (!m_pLocalEntity->IsShotsFired() && m_pLocalEntity->GetTeamNumber() != pCurrentEntity->GetTeamNumber() && pCurrentEntity->GetHealth() > 0)
						{
							m_cCSGOManager->GetEngineHelper()->SetSensitivity(m_dSlowAimSensivity);
						}
						else
						{
							m_cCSGOManager->GetEngineHelper()->SetSensitivity(m_flOldSensitivty);
						}
					}
				}
			}
		}

		if (m_bFakeLagIsEnabled)
		{
			if (GetAsyncKeyState(m_dwFakeLagKey) & 0x8000)
			{
				m_cCSGOManager->GetEngineHelper()->SetSendPackets(0);
				Sleep(120);
				m_cCSGOManager->GetEngineHelper()->SetSendPackets(1);
				Sleep(70);
			}
		}
	}
}


#if HAX_MULTI_THREAD == true
DWORD WINAPI CCSGOMisc::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CCSGOMisc*)lpParam;
	return This->ThreadRoutine();
}

DWORD CCSGOMisc::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "CSGO Misc routine started!");

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

bool CCSGOMisc::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "CSGO Misc thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CCSGOMisc::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "CSGO Misc thread terminated!");
		return true;
	}
	return false;
}

void CCSGOMisc::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif
