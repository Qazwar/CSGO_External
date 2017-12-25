#include "main.h"
#include "Hax_BunnyHop.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"
#include "Hax_CSGO_Manager.h"

// Hax
CBunnyHop::CBunnyHop(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_dwEnableKey(VK_F8), m_dwJumpKey(VK_SPACE), m_dwReleaseInterval(20), m_bUseAutoStrafe(false)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CBunnyHop::CBunnyHop");

	// Get entity data
	m_pLocalEntity = m_cCSGOManager->GetLocalEntity();
	if (!m_pLocalEntity || !m_pLocalEntity.get()) {
		DEBUG_LOG(LL_ERR, "Local entity initilization fail!");
		abort();
	}

	// Settings
	LoadConfigs();
}

CBunnyHop::~CBunnyHop()
{
	DEBUG_LOG(LL_SYS, "CBunnyHop::~CBunnyHop");

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

void CBunnyHop::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGOBunnyHopSettings();

	// Get Settings
	m_dwEnableKey		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetBunnyHopToggleKey();
	m_dwJumpKey			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetBunnyHopJumpKey();
	m_dwReleaseInterval = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetBunnyHopReleaseInterval();
	m_bUseAutoStrafe	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetBunnyHopUseAutoStrafe();
}

void CBunnyHop::OnUpdate()
{
	if (m_bEnabled == true && m_bSuspended == false)
	{
		if (m_pLocalEntity->IsAlive_State() && !m_pLocalEntity->IsDormant())
		{
			if (m_pLocalEntity->IsMoving())
			{
				if (GetAsyncKeyState(m_dwJumpKey) & 0x8000)
				{
					if (m_cCSGOManager->GetMemoryHelper()->Read<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer()) == 5) // if getjump
						m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer(), 4); // setjump false

					if (m_bUseAutoStrafe)
					{
						auto vCurrentViewAngles = m_cCSGOManager->GetEngineHelper()->GetViewAngle();
						if (vCurrentViewAngles.GetY() != m_vPrevViewAngle.GetY())
						{
							if (vCurrentViewAngles.GetY() > m_vPrevViewAngle.GetY())
								m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceLeft_Pointer(), 6);
							else
								m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceRight_Pointer(), 6);
						}
						m_vPrevViewAngle = vCurrentViewAngles;
					}

					if (m_cCSGOManager->GetMemoryHelper()->Read<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer()) != 5) // if getjump			
					{
						auto dwFlags = m_pLocalEntity->GetFlags();
						if (dwFlags & (FL_ONGROUND | FL_PARTIALGROUND))
						{
							m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer(), 5);
						}
						else
						{
							m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer(), 4);
							m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer(), 5);
							Sleep(m_dwReleaseInterval + m_cCSGOManager->GetEngineHelper()->Random<int>(Vector2(5, 12)));
							m_cCSGOManager->GetMemoryHelper()->Write<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetForceJump_Pointer(), 4);
						}
					}
				}
			}
		}
	}
}


#if HAX_MULTI_THREAD == true
DWORD WINAPI CBunnyHop::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CBunnyHop*)lpParam;
	return This->ThreadRoutine();
}

DWORD CBunnyHop::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Bunny hop routine started!");

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

bool CBunnyHop::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "Bunny Hop thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CBunnyHop::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "Bunny Hop thread terminated!");
		return true;
	}
	return false;
}

void CBunnyHop::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif