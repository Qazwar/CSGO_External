#include "main.h"
#include "Hax_GlowESP.h"
#include "Defines.h"
#include "Common.h"

/* TODO
	- Color Types; hexcode, solid, health
	- Multiple visible check option; 0 show everyone, 1 spotted only, 2 visible only with bsp parser
*/

// Hax
CGlowESP::CGlowESP(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_bRadarIsEnabled(false), m_bEnableGlow(false), m_dwEnableKey(VK_F7), m_dwUpdateInterval(1), m_bIsEnemyOnly(false), m_bIsVisibleOnly(false), m_bShowC4(false), m_bShowC4Host(false), m_bShowChicken(false)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CGlowESP::CGlowESP");

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

	// Settings
	LoadConfigs();
}

CGlowESP::~CGlowESP()
{
	DEBUG_LOG(LL_SYS, "CGlowESP::~CGlowESP");

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

void CGlowESP::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGOGlowESPSettings();

	// Get Settings
	m_bRadarIsEnabled	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetRadarEnabled();
	m_bEnableGlow		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPIsEnabled();
	m_bChamsIsEnabled	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPChamsIsEnabled();
	m_dwEnableKey		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPEnableKey();
	m_dwUpdateInterval	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPUpdateInterval();
	m_bIsEnemyOnly		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPEnemyOnly();
	m_bIsVisibleOnly	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPVisibleOnly();

	// Create RGB Colors
	ConvertFromColor(m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPEnemyColor(), &m_EnemyColorArray);
	ConvertFromColor(m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPFriendColor(), &m_FriendColorArray);

	// Parse maps

	/// C4
	auto mapGlowESPC4Color = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPC4Color();
	if (mapGlowESPC4Color.begin() == mapGlowESPC4Color.end()) {
		DEBUG_LOG(LL_ERR, "mapGlowESPC4Color parse fail!");
		abort();
	}
	for (auto elem : mapGlowESPC4Color)
	{
		if (elem.first == "show") m_bShowC4 = elem.second == "true" ? true : false;
		if (elem.first == "color") ConvertFromColor(std::strtoul(elem.second.c_str(), NULL, 16), &m_C4ColorArray);
	}

	/// C4hOST
	auto mapGlowESPC4HostColor = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPC4HostColor();
	if (mapGlowESPC4HostColor.begin() == mapGlowESPC4HostColor.end()) {
		DEBUG_LOG(LL_ERR, "mapGlowESPC4HostColor parse fail!");
		abort();
	}
	for (auto elem : mapGlowESPC4HostColor)
	{
		if (elem.first == "show") m_bShowC4Host = elem.second == "true" ? true : false;
		if (elem.first == "color") ConvertFromColor(std::strtoul(elem.second.c_str(), NULL, 16), &m_C4HostColorArray);
	}

	/// Chicken
	auto mapGlowESPChickenColor = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowESPChickenColor();
	if (mapGlowESPChickenColor.begin() == mapGlowESPChickenColor.end()) {
		DEBUG_LOG(LL_ERR, "mapGlowESPChickenColor parse fail!");
		abort();
	}
	for (auto elem : mapGlowESPChickenColor)
	{
		if (elem.first == "show") m_bShowChicken = elem.second == "true" ? true : false;
		if (elem.first == "color") ConvertFromColor(std::strtoul(elem.second.c_str(), NULL, 16), &m_ChickenColorArray);
	}
}

void CGlowESP::OnUpdate()
{
	if (m_bEnabled == true && m_bSuspended == false)
	{
		// Glow
		auto dwGlowPointer = m_cCSGOManager->GetMemoryHelper()->Read<DWORD>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowObjectManager_Pointer());
		if (dwGlowPointer)
		{
			auto iGlowObjectCount = m_cCSGOManager->GetMemoryHelper()->Read<int>(m_cCSGOManager->GetClientBase() + m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetGlowObjectManager_Pointer() + 0x4);
			if (iGlowObjectCount)
			{
				for (int i = 0; i < iGlowObjectCount; i++)
				{
					auto glowObj = m_cCSGOManager->GetMemoryHelper()->Read<GlowObjectDefinition_t>(dwGlowPointer + (i * sizeof(GlowObjectDefinition_t)));
					if (glowObj.m_pEntity)
					{
						// Local entity
						if (m_pLocalEntity->Update(EEntityTypes::ENTITY_LOCAL))
						{
							if (m_pLocalEntity->IsAlive_State() && !m_pLocalEntity->IsDormant())
							{
								// Current entity
								m_pCurrentEntity->SetEntityBase(DWORD(glowObj.m_pEntity));

								// Hax
								bool bIsChanged = false;

								switch (m_pCurrentEntity->GetClassID())
								{
									case ClassId_CPlantedC4:
									case ClassId_CC4:
									{
										bIsChanged = true;

										glowObj.m_flRed		= m_C4ColorArray.at(0);
										glowObj.m_flGreen	= m_C4ColorArray.at(1);
										glowObj.m_flBlue	= m_C4ColorArray.at(2);
										glowObj.m_flAlpha	= m_C4ColorArray.at(3);
									} break;

									case ClassId_CChicken:
									{
										bIsChanged = true;

										glowObj.m_flRed		= m_ChickenColorArray.at(0);
										glowObj.m_flGreen	= m_ChickenColorArray.at(1);
										glowObj.m_flBlue	= m_ChickenColorArray.at(2);
										glowObj.m_flAlpha	= m_ChickenColorArray.at(3);
									} break;

									case ClassId_CCSPlayer:
									{
										if (m_pCurrentEntity->IsAlive_State() || !m_pCurrentEntity->IsDormant())
										{
											auto iCurrentTeam	= m_pCurrentEntity->GetTeamNumber();
											auto iLocalTeam		= m_pLocalEntity->GetTeamNumber();

											if (iCurrentTeam != iLocalTeam)
											{
												// Radar
												if (m_bRadarIsEnabled)
												{
													if (!m_pCurrentEntity->IsSpotted()) {
													//	DEBUG_LOG(LL_SYS, "entity: %d spotted!", i);
														m_pCurrentEntity->SetSpotted(true);
													}
												}

												// Glow style
												if (m_bEnableGlow && m_pLocalEntity->GetCrossHairID() == m_pCurrentEntity->GetIndex())
												{
													bIsChanged = true;

													auto flDistance = m_pLocalEntity->GetPosition().Distance(m_pCurrentEntity->GetPosition());
													if (flDistance < 20.f)
														glowObj.m_nGlowStyle = 1;
													else
														glowObj.m_nGlowStyle = 2;
												}

												// Chams
												if (m_bChamsIsEnabled)
												{
													m_pCurrentEntity->SetClrBase(Vector3(255, 170, 255)); // FIXME: Vector4
												}
											}
											else // Same team
											{
												// Chams
												if (m_bChamsIsEnabled)
												{
													m_pCurrentEntity->SetClrBase(Vector3(0, 255, 0));
												}
											}

											// Glow player
											if (m_bEnableGlow)
											{
												if (!m_bIsVisibleOnly || (m_bIsVisibleOnly && m_pCurrentEntity->IsSpottedByMask()))
												{
													bIsChanged = true;

													if (m_bIsEnemyOnly == false && iLocalTeam == iCurrentTeam) // Friend
													{
														glowObj.m_flRed		= m_FriendColorArray.at(0);
														glowObj.m_flGreen	= m_FriendColorArray.at(1);
														glowObj.m_flBlue	= m_FriendColorArray.at(2);
														glowObj.m_flAlpha	= m_FriendColorArray.at(3);
													}
													else if (iLocalTeam != iCurrentTeam) // Enemy
													{
														glowObj.m_flRed		= m_EnemyColorArray.at(0);
														glowObj.m_flGreen	= m_EnemyColorArray.at(1);
														glowObj.m_flBlue	= m_EnemyColorArray.at(2);
														glowObj.m_flAlpha	= m_EnemyColorArray.at(3);
													}
												}

												/// C4 Host
												if (m_cCSGOManager->GetEngineHelper()->GetC4Host() == m_pCurrentEntity->GetIndex())
												{
													bIsChanged = true;

													glowObj.m_flRed		= m_C4HostColorArray.at(0);
													glowObj.m_flGreen	= m_C4HostColorArray.at(1);
													glowObj.m_flBlue	= m_C4HostColorArray.at(2);
													glowObj.m_flAlpha	= m_C4HostColorArray.at(3);
												}
											}
										}
									} break;
								}

								if (m_bEnableGlow && bIsChanged)
								{
									glowObj.m_bRenderWhenOccluded = true;
									glowObj.m_bRenderWhenUnoccluded = false;

									// m_cCSGOManager->GetMemoryHelper()->Write<GlowObjectDefinition_t>(dwGlowPointer + (m_pCurrentEntity->GetGlowIndex() * sizeof(glowObj)), glowObj);
									m_cCSGOManager->GetMemoryHelper()->Write<GlowObjectDefinition_t>(dwGlowPointer + (i * sizeof(glowObj)), glowObj);
								}
							}
						}
					}

					Sleep(1); //
				}
			}
		}
	}
}


// Utils
void CGlowESP::ConvertFromColor(DWORD dwColor, std::array <float, 4> * pRgba)
{
	union {
		DWORD dwColor;
		BYTE byColors[4];
	} uColor = { dwColor };

	if (pRgba)
	{
		if (pRgba->at(0))
			pRgba->at(0) = static_cast<float>(uColor.byColors[0]) / 255.0f;

		if (pRgba->at(1))
			pRgba->at(1) = static_cast<float>(uColor.byColors[1]) / 255.0f;

		if (pRgba->at(2))
			pRgba->at(2) = static_cast<float>(uColor.byColors[2]) / 255.0f;

		if (pRgba->at(3))
			pRgba->at(3) = static_cast<float>(uColor.byColors[3]) / 255.0f;
	}
}

DWORD CGlowESP::ConvertToColor(std::array <float, 4> rgba)
{
	union {
		DWORD dwColor;
		BYTE byColors[4];
	} uColor;

	uColor.byColors[0] = static_cast<BYTE>(rgba.at(0) * 255.0f);
	uColor.byColors[1] = static_cast<BYTE>(rgba.at(1) * 255.0f);
	uColor.byColors[2] = static_cast<BYTE>(rgba.at(2) * 255.0f);
	uColor.byColors[3] = static_cast<BYTE>(rgba.at(3) * 255.0f);
	return uColor.dwColor;
}


#if HAX_MULTI_THREAD == true
DWORD WINAPI CGlowESP::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CGlowESP*)lpParam;
	return This->ThreadRoutine();
}

DWORD CGlowESP::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Glow ESP routine started!");

	while (1)
	{
		if (m_bThreadIsStopped == true)
			break;

		if (m_cCSGOManager->GetEngineHelper()->IsInGame())
			OnUpdate();

		Sleep(m_dwUpdateInterval);
	}

	return 0;
}

bool CGlowESP::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "Glow ESP thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CGlowESP::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "Glow ESP thread terminated!");
		return true;
	}
	return false;
}

void CGlowESP::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif
