#include "main.h"
#include "Hax_TriggerBot.h"
#include "Defines.h"
#include "MemoryHelper.h"
#include "Common.h"
#include "Hax_CSGO_Manager.h"

#include "Matrix3x4.h"
#include "HitBox.h"

#define KEY_Q 0x51

/* TODO
	- Slow aim
	- Ignore jumping targets
*/

Hitbox_t Hitbox[21];

// Hax
CTriggerBot::CTriggerBot(const std::shared_ptr <CCSGOManager> & csgoManager) :
	m_cCSGOManager(csgoManager),
	m_bEnabled(false), m_bSuspended(false), m_bPressedToHoldKey(false), m_dwWorkType(ETriggerBotWorkTypes::WORKTYPE_NULL), m_dwEnableKey(VK_F9), m_dwInterval(0), m_bIsBurst(false), m_dwBurstInterval(0), m_dwBurstCount(3), m_bAutoCrouch(false), m_bBlockOnJump(false), m_bBlockOnMove(false), m_dwTargetBone(0), m_dFov(0.f), m_bSmoothEnabled(false), m_flSmoothValue(0.f), m_bTraceRay(false),
	m_bEnablePreWaitInterval(false), m_bEnablePostWaitInterval(false), m_bEnableBothWaitInterval(false), m_iRifleInterval(0), m_iPistolInterval(0), m_iSniperInterval(0), m_iSMGInterval(0), m_iShotgunInterval(0), m_iMGInterval(0), m_iRevolverInterval(0),
	m_bEnableAutoSwitch(false), m_bEnableScopeCheck(false), m_bEnableAutoReZoom(false), m_dwExtraSniperDelay(0),
	m_bUseHoldKey(false), m_dwHoldKey(VK_UP)
#if HAX_MULTI_THREAD == true
	, m_hThread(INVALID_HANDLE_VALUE), m_bThreadIsStopped(false)
#endif
{
	DEBUG_LOG(LL_SYS, "CTriggerBot::CTriggerBot");

	// Process data
	m_hWnd = m_cCSGOManager->GetProcessHelper()->GetWindow(XOR("Valve001"));
	if (!m_hWnd) {
		DEBUG_LOG(LL_ERR, "Target window not found!");
		abort();
	}

	// dats
	m_vHitboxes.clear();

	// Get entity data
	m_pLocalEntity = m_cCSGOManager->GetLocalEntity();
	if (!m_pLocalEntity || !m_pLocalEntity.get()) {
		DEBUG_LOG(LL_ERR, "Local entity initilization fail!");
		abort();
	}

	m_pTargetEntity = std::make_unique<CEntity>(m_cCSGOManager);
	if (!m_pTargetEntity || !m_pTargetEntity.get()) {
		DEBUG_LOG(LL_ERR, "InCross entity initilization fail!");
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

CTriggerBot::~CTriggerBot()
{
	DEBUG_LOG(LL_SYS, "CTriggerBot::~CTriggerBot");

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

void CTriggerBot::LoadConfigs()
{
	// Parse settings
	m_cCSGOManager->GetCSGODataHelper()->ParseCSGOTriggerBotSettings();

	// Get Settings
	m_dwWorkType			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotWorkType();
	m_dwEnableKey			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotEnableKey();
	m_dwInterval			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotInterval();
	m_bIsBurst				= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotIsBurst();
	m_dwBurstInterval		= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotBurstInterval();
	m_dwBurstCount			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotBurstCount();
	m_bAttackAsLongAsAlive	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotIsAttackAsLongAsAlive();
	m_bCheckSpawnProtect	= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotCheckSpawnProtect();
	m_bAimLock				= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotAimLock();
	m_bAutoCrouch			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotAutoCrouch();
	m_bBlockOnJump			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotBlockOnJump();
	m_bBlockOnMove			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotBlockOnMove();
	m_dwTargetBone			= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotTargetBone();
	m_dFov					= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotFOV();
	m_bTraceRay				= m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotTraceRay();

	// Parse multi settings
	// TODO: XOR
	auto vSmooth = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetTriggerBotSmooth();
	if (vSmooth.size() != 2 || (vSmooth[0].size() != 4 /* true */ && vSmooth[0].size() != 5 /* false */ ) || (vSmooth[1].empty() || !strstr(vSmooth[1].c_str(), ".")) ) {
		DEBUG_LOG(LL_ERR, "vSmooth parse fail!");
		abort();
	}
	m_bSmoothEnabled	= vSmooth[0] == "true" ? true : false;
	m_flSmoothValue		= std::atof(vSmooth[1].c_str());

	auto mapContinuousAttackIntervals = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetContinuousAttackIntervals();
	if (mapContinuousAttackIntervals.begin() == mapContinuousAttackIntervals.end()) {
		DEBUG_LOG(LL_ERR, "mapContinuousAttackIntervals parse fail!");
		abort();
	}
	for (auto elem : mapContinuousAttackIntervals)
	{
		if (elem.first == "pre_enable")		m_bEnablePreWaitInterval	= elem.second == "true" ? true : false;
		if (elem.first == "post_enable")	m_bEnablePostWaitInterval	= elem.second == "true" ? true : false;
		if (elem.first == "both_enable")	m_bEnableBothWaitInterval	= elem.second == "true" ? true : false;
		if (elem.first == "rifle")			m_iRifleInterval			= std::atoi(elem.second.c_str());
		if (elem.first == "pistol")			m_iPistolInterval			= std::atoi(elem.second.c_str());
		if (elem.first == "sniper")			m_iSniperInterval			= std::atoi(elem.second.c_str());
		if (elem.first == "smg")			m_iSMGInterval				= std::atoi(elem.second.c_str());
		if (elem.first == "shotgun")		m_iShotgunInterval			= std::atoi(elem.second.c_str());
		if (elem.first == "mg")				m_iMGInterval				= std::atoi(elem.second.c_str());
		if (elem.first == "revolver")		m_iRevolverInterval			= std::atoi(elem.second.c_str());
	}

	auto mapSniperMode = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetSniperModeValues();
	if (mapSniperMode.begin() == mapSniperMode.end()) {
		DEBUG_LOG(LL_ERR, "mapSniperMode parse fail!");
		abort();
	}
	for (auto elem : mapSniperMode)
	{
		if (elem.first == "auto_switch")	m_bEnableAutoSwitch		= elem.second == "true" ? true : false;
		if (elem.first == "scope_check")	m_bEnableScopeCheck		= elem.second == "true" ? true : false;
		if (elem.first == "auto_rezoom")	m_bEnableAutoReZoom		= elem.second == "true" ? true : false;
		if (elem.first == "extra_delay")	m_dwExtraSniperDelay	= std::atoi(elem.second.c_str());
	}

	auto mapHoldKeySettings = m_cCSGOManager->GetCSGODataHelper()->GetCSGODataInstance()->GetHoldKeyValues();
	if (mapHoldKeySettings.begin() == mapHoldKeySettings.end()) {
		DEBUG_LOG(LL_ERR, "mapHoldKeySettings parse fail!");
		abort();
	}
	for (auto elem : mapHoldKeySettings)
	{
		if (elem.first == "use")			m_bUseHoldKey	= elem.second == "true" ? true : false;
		if (elem.first == "key")			m_dwHoldKey		= std::atoi(elem.second.c_str());
	}

	// Configure settings
	auto targetBone = 0UL;
	if (m_dwTargetBone == (DWORD)-1) { // TODO: Whitelist bone config
		DEBUG_LOG(LL_SYS, "Random bone selection enabled!");
		targetBone = ETriggerBotBones::RANDOM_BONE;
	}
	else if (m_dwTargetBone == (DWORD)-2) {
		DEBUG_LOG(LL_SYS, "Nearest bone selection enabled!");
		targetBone = ETriggerBotBones::NEAREST_BONE;
	}
	else if (targetBone >= 0 && targetBone <= 82) {
		targetBone = m_dwTargetBone;

		/// hitbox configrations
//if (m_head)
		m_vHitboxes.push_back(HITBOX_HEAD);

/*
	if (m_chest)
	{
		m_hitboxes.push_back(HITBOX_NECK);
		m_hitboxes.push_back(HITBOX_L_CLAVICLE);
		m_hitboxes.push_back(HITBOX_R_CLAVICLE);
		m_hitboxes.push_back(HITBOX_SPINE3);
		m_hitboxes.push_back(HITBOX_SPINE4);
	}

	if (m_body)
	{
		m_hitboxes.push_back(HITBOX_SPINE1);
		m_hitboxes.push_back(HITBOX_SPINE2);
		m_hitboxes.push_back(HITBOX_PELVIS);
	}

	if (m_arms)
	{
		m_hitboxes.push_back(HITBOX_L_HAND);
		m_hitboxes.push_back(HITBOX_L_UPPERARM);
		m_hitboxes.push_back(HITBOX_L_FOREARM);
		m_hitboxes.push_back(HITBOX_R_HAND);
		m_hitboxes.push_back(HITBOX_R_UPPERARM);
		m_hitboxes.push_back(HITBOX_R_FOREARM);
	}

	if (m_legs)
	{
		m_hitboxes.push_back(HITBOX_L_FOOT);
		m_hitboxes.push_back(HITBOX_L_CALF);
		m_hitboxes.push_back(HITBOX_L_THIGH);
		m_hitboxes.push_back(HITBOX_R_FOOT);
		m_hitboxes.push_back(HITBOX_R_CALF);
		m_hitboxes.push_back(HITBOX_R_THIGH);
	}
	*/
	}
	else {
		DEBUG_LOG(LL_ERR, "Unknown bone id: %u", m_dwTargetBone);
		abort();
	}
	m_dwTargetBone = targetBone;

	if (m_dwExtraSniperDelay > 1000)
		m_dwExtraSniperDelay = 0;
}

void CTriggerBot::TriggerShot()
{
	if (!m_pLocalEntity->WeaponIsShootable())
		return;

	if (!m_pTargetEntity->IsAlive_Hp())
		return;

	if (!m_pLocalEntity->CanAttack())
		return;

	m_pLocalEntity->SendForceAttack(true, m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(m_dwInterval, 0UL, 10UL, true));
}

void CTriggerBot::TriggerBurst()
{
	for (auto i = 0UL; i < m_dwBurstCount; ++i)
	{
		TriggerShot();
		Sleep(m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(m_dwBurstInterval, 0UL, 10UL, true));
	}
}

void CTriggerBot::AttackToTarget(int iWeaponID)
{
	auto flFOV = 0.f;

	do {
		// Key stuffs is not necessary for sniper
		if (!m_pLocalEntity->WeaponIsSniper(iWeaponID) && !m_pLocalEntity->WeaponIsAutoSniper(iWeaponID) && iWeaponID != WEAPON_REVOLVER)
		{
			// Crouch before than shoot
			if (m_bAutoCrouch)
				m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, VK_LCONTROL, 0, EKeyTypes::KEY_DOWN);

			// Press left shift key while shooting
			m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, VK_LSHIFT, 0, EKeyTypes::KEY_DOWN);
		}

		// Pre wait
		if (m_bEnablePreWaitInterval || m_bEnableBothWaitInterval)
		{
			auto iDelay = 0;

			if (m_pLocalEntity->WeaponIsRifle(iWeaponID))
				iDelay = m_iRifleInterval;
			else if (m_pLocalEntity->WeaponIsPistol(iWeaponID))
				iDelay = m_iPistolInterval;
			else if (m_pLocalEntity->WeaponIsSniper(iWeaponID))
				iDelay = m_iSniperInterval;
			else if (m_pLocalEntity->WeaponIsSMG(iWeaponID))
				iDelay = m_iSMGInterval;
			else if (m_pLocalEntity->WeaponIsShotgun(iWeaponID))
				iDelay = m_iShotgunInterval;
			else if (m_pLocalEntity->WeaponIsMG(iWeaponID))
				iDelay = m_iMGInterval;

			if (iDelay)
				Sleep(DWORD(m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(iDelay, 0, 10, true)));
		}

		// Attack
		if (m_bIsBurst && m_pLocalEntity->WeaponCanBurst(iWeaponID))
			TriggerBurst();
		else
			TriggerShot();

		// post stuffs is not necessary for sniper and revolver, break loop
		if (m_pLocalEntity->WeaponIsSniper(iWeaponID) || m_pLocalEntity->WeaponIsAutoSniper(iWeaponID) || iWeaponID == WEAPON_REVOLVER)
			return;

		// Post stuffs
		if (m_pTargetEntity->GetHealth() > 0)
		{
			if (m_bAimLock)
			{
				CalculateAngles();
			}

			if (m_bEnablePostWaitInterval || m_bEnableBothWaitInterval)
			{
				auto iDelay = 0;

				if (m_pLocalEntity->WeaponIsRifle(iWeaponID))
					iDelay = m_iRifleInterval;
				else if (m_pLocalEntity->WeaponIsPistol(iWeaponID))
					iDelay = m_iPistolInterval;
				else if (m_pLocalEntity->WeaponIsSniper(iWeaponID))
					iDelay = m_iSniperInterval;
				else if (m_pLocalEntity->WeaponIsSMG(iWeaponID))
					iDelay = m_iSMGInterval;
				else if (m_pLocalEntity->WeaponIsShotgun(iWeaponID))
					iDelay = m_iShotgunInterval;
				else if (m_pLocalEntity->WeaponIsMG(iWeaponID))
					iDelay = m_iMGInterval;

				if (iDelay)
					Sleep(DWORD(m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(iDelay, 0, 10, true)));
			}
		}

		// Break left shift key after than shoot
		m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, VK_LSHIFT, 0, EKeyTypes::KEY_UP);

		// Break crouch after than shoot
		if (m_bAutoCrouch)
			m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, VK_LCONTROL, 0, EKeyTypes::KEY_UP);

	} while (
		(m_bAttackAsLongAsAlive && m_pTargetEntity->GetHealth() > 0) /* if as long as alive option is selected and entity still alive */ &&
		(
			m_pTargetEntity->GetIndex() == m_pLocalEntity->GetCrossHairID() /* and entity still on our cross */ ||
			m_cCSGOManager->GetEngineHelper()->IsInFOV(m_pLocalEntity->GetEyePosition(), m_pTargetEntity->GetBonePosition((m_dwTargetBone >= 0 && m_dwTargetBone <= 20) ? (CSPlayerBones)m_dwTargetBone : CSPlayerBones::BONE_HEAD), m_dFov, flFOV) /* or entity out of cross but fov distance is enough for pre-defined fov value */
		)
	);
	m_pTargetEntity->SetEntityBase(0); // remove last entity
}


bool CTriggerBot::SetAnglesToTarget(DWORD dwTargetBone)
{
	auto flDistance = 0.f;
	auto vEyePos	= m_pLocalEntity->GetEyePosition();
	auto vTargetPos = m_pTargetEntity->GetBonePosition((CSPlayerBones)dwTargetBone);
	auto bVisible	= m_pLocalEntity->RayTrace(vTargetPos - Vector3(m_dFov), vTargetPos + Vector3(m_dFov), flDistance);

	if (!m_bTraceRay || (m_bTraceRay && bVisible))
	{
		if (vEyePos.IsValid() && vTargetPos.IsValid())
		{
			auto flRandomizedX = m_cCSGOManager->GetEngineHelper()->Random<float>(Vector2(1.6f, 2.f)); // stable value 2.f
			auto flRandomizedY = m_cCSGOManager->GetEngineHelper()->Random<float>(Vector2(m_flSmoothValue));
			Vector2 vPunchReductionTriggerBot[2] = {
				Vector2(flRandomizedX, flRandomizedY) /* pitch */,
				Vector2(flRandomizedX, flRandomizedY) /* yaw */ 
			};

			auto vAimAngle = m_cCSGOManager->GetEngineHelper()->CalculateAngle(vEyePos, vTargetPos);
			vAimAngle -= (m_pLocalEntity->GetAimPunch() * m_cCSGOManager->GetEngineHelper()->CalculatePunchReduction(vPunchReductionTriggerBot[0], vPunchReductionTriggerBot[1]));
			if (m_bSmoothEnabled) {
				auto vViewAngle = m_cCSGOManager->GetEngineHelper()->GetViewAngle();
				vAimAngle = m_cCSGOManager->GetEngineHelper()->CalculateSmoothAngle(vViewAngle, vAimAngle, m_cCSGOManager->GetEngineHelper()->Random<float>(Vector2(m_flSmoothValue)));
			}

			if (!m_bSmoothEnabled)
				m_cCSGOManager->GetEngineHelper()->SetSendPackets(false);

			m_cCSGOManager->GetEngineHelper()->SetViewAngle(vAimAngle);	

			if (!m_bSmoothEnabled)
				m_cCSGOManager->GetEngineHelper()->SetSendPackets(true);

			return true;
		}
	}
	return false;
}
bool CTriggerBot::CalculateAngles()
{
	if (m_pTargetEntity->GetHealth() > 0)
	{	
		if (m_dwTargetBone == ETriggerBotBones::RANDOM_BONE)
		{
			auto dwTargetBone = m_cCSGOManager->GetEngineHelper()->Random<DWORD>(Vector2(6, 8)); // random value is chest, neck, head only
			SetAnglesToTarget(dwTargetBone);
			return true;
		}
		else if (m_dwTargetBone == ETriggerBotBones::NEAREST_BONE)
		{
			std::map <DWORD /* dwBone */, double /* dDistance */> mapDistances;
			for (auto i = 0; i < 20; i++) // check just first 20 bone
			{
				auto vBonePos	= m_pTargetEntity->GetBonePosition((CSPlayerBones)i);
				auto dDistance	= m_cCSGOManager->GetEngineHelper()->GetDistanceFromCrosshair(Vector2(vBonePos.GetX(), vBonePos.GetY()));
				mapDistances[i] = dDistance;
			}
			auto dwTargetBone = GetBestBoneId(mapDistances);
			// DEBUG_LOG(LL_SYS, "Best distance own bone: %u", dwTargetBone);
			SetAnglesToTarget(dwTargetBone);
			return true;
		}
		else {
			SetAnglesToTarget(m_dwTargetBone);
			return true;
		}

	}
	return false;
}


void CTriggerBot::HaxRoutine(int iEntity)
{
	if (m_pLocalEntity->IsAlive_State() && !m_pLocalEntity->IsDormant())
	{
		if (m_pTargetEntity->Update(EEntityTypes::ENTITY_BY_INDEX, iEntity))
		{
			if (m_pLocalEntity->CanAttack())
			{
				//DEBUG_LOG(LL_CRI, "indexs: me: %d target: %d", m_pLocalEntity->GetIndex(), m_pTargetEntity->GetIndex());
				if (m_pLocalEntity->GetIndex() != m_pTargetEntity->GetIndex())
				{
					//DEBUG_LOG(LL_CRI, "teams: me: %d target: %d", m_pLocalEntity->GetTeamNumber(), m_pTargetEntity->GetTeamNumber());
					if (m_pLocalEntity->GetTeamNumber() != m_pTargetEntity->GetTeamNumber())
					{
						//DEBUG_LOG(LL_CRI, "current flash alpha: %f", m_pLocalEntity->GetFlashMaxAlpha());
						//if (m_pLocalEntity->GetFlashMaxAlpha() == 255.f)
						{
							//DEBUG_LOG(LL_CRI, "alive: %d dormant: %d attack: %d", m_pTargetEntity->IsAlive_Hp(), m_pTargetEntity->IsDormant(), m_cCSGOManager->GetEngineHelper()->GetAttack());
							if (m_pTargetEntity->IsAlive_State() && !m_pTargetEntity->IsDormant() && !m_cCSGOManager->GetEngineHelper()->GetAttack())
							{
								auto dwFlags = m_pLocalEntity->GetFlags();
								//DEBUG_LOG(LL_CRI, "blockonjump: %d onground: %d", m_bBlockOnJump, dwFlags & (FL_ONGROUND | FL_PARTIALGROUND));
								if (!m_bBlockOnJump || (m_bBlockOnJump && (dwFlags & (FL_ONGROUND | FL_PARTIALGROUND))))
								{
									//DEBUG_LOG(LL_CRI, "blockonmove: %d ismove: %d", m_bBlockOnMove, m_pLocalEntity->IsMoving());
									if (!m_bBlockOnMove || (m_bBlockOnMove && !m_pLocalEntity->IsMoving()))
									{
										//DEBUG_LOG(LL_CRI, "checkspawn: %d isprotected: %d", m_bCheckSpawnProtect, m_pTargetEntity->IsSpawnProtected());
										if (!m_bCheckSpawnProtect || (m_bCheckSpawnProtect && !m_pTargetEntity->IsSpawnProtected()))
										{
											auto dwWeaponEntity = m_pLocalEntity->GetWeaponEntity();
											//DEBUG_LOG(LL_CRI, "weapon entity: %u", dwWeaponEntity);
											if (dwWeaponEntity)
											{
												if (m_pWeaponEntity->Update(EEntityTypes::ENTITY_BY_WEAPON, dwWeaponEntity))
												{
													//DEBUG_LOG(LL_CRI, "ammo: %d", m_pWeaponEntity->GetAmmo());
													if (m_pWeaponEntity->GetAmmo() > 0)
													{
														auto iWeaponID = m_pLocalEntity->GetWeaponID();
														//DEBUG_LOG(LL_CRI, "wepid: %d", iWeaponID);
														if (m_pLocalEntity->WeaponIsShootable(iWeaponID))
														{
															bool bIsSniper	= m_pLocalEntity->WeaponIsSniper(iWeaponID);
															auto bScoped	= m_pLocalEntity->IsScoped();
															auto iZoomLevel = m_pWeaponEntity->GetZoomLevel();

															//DEBUG_LOG(LL_CRI, "scopecheck: %d issniper: %d scoped: %d", m_bEnableScopeCheck, bIsSniper, bScoped);
															if (!m_bEnableScopeCheck || (m_bEnableScopeCheck && !bIsSniper) || (m_bEnableScopeCheck && bIsSniper && bScoped))
															{
																auto dwOriginalDelay = m_dwInterval;

																if (iWeaponID == WEAPON_REVOLVER && m_iRevolverInterval)
																	m_dwInterval = m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(m_iRevolverInterval, 0, 50, true);
																else if (bIsSniper && m_dwExtraSniperDelay)
																	m_dwInterval = m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(m_dwExtraSniperDelay, 0UL, 50UL, true);

																//DEBUG_LOG(LL_CRI, "delay: %u", m_dwInterval);
																AttackToTarget(iWeaponID);

																m_dwInterval = dwOriginalDelay;

																if (bIsSniper)
																{
																	if (m_bEnableAutoSwitch)
																	{
																		m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, KEY_Q, m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(150, 0, 50, true));
																		m_cCSGOManager->GetProcessHelper()->SendKey(m_hWnd, KEY_Q, m_cCSGOManager->GetEngineHelper()->GetRandomizedValue(250, 0, 50, true));

																		if (m_bEnableAutoReZoom)
																		{
																			//DEBUG_LOG(LL_CRI, "zoom lv.: %d", iZoomLevel);
																			for (int i = 0; i < iZoomLevel; i++)
																			{
																				mouse_event(MOUSEEVENTF_RIGHTDOWN, NULL, NULL, NULL, NULL);
																				Sleep(10);
																				mouse_event(MOUSEEVENTF_RIGHTUP, NULL, NULL, NULL, NULL);
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void CTriggerBot::InCrossRoutine()
{
	int iCrossHairID = m_pLocalEntity->GetCrossHairID();
	if (iCrossHairID > 0 && iCrossHairID <= g_dwEntityCount)
	{
		//DEBUG_LOG(LL_CRI, "target entity: %d", iCrossHairID);
		HaxRoutine(iCrossHairID);
	}
}

void CTriggerBot::HitboxRoutine()
{
	auto vEyePos = m_pLocalEntity->GetEyePosition();
//	DEBUG_LOG(LL_SYS, "Eye position: %f %f %f", vEyePos.GetX(), vEyePos.GetY(), vEyePos.GetZ());

	auto vViewAngles = m_cCSGOManager->GetEngineHelper()->GetViewAngle();
//	DEBUG_LOG(LL_SYS, "View Angles: %f %f %f", vViewAngles.GetX(), vViewAngles.GetY(), vViewAngles.GetZ());

	auto viewDirection = m_cCSGOManager->GetEngineHelper()->AngleToDirection(vViewAngles);
//	DEBUG_LOG(LL_SYS, "View Direction: %f %f %f", viewDirection.GetX(), viewDirection.GetY(), viewDirection.GetZ());

	for (auto i = 0UL; i < g_dwEntityCount; i++)
	{
		if (m_pTargetEntity->Update(EEntityTypes::ENTITY_BY_INDEX, i))
		{
			//DEBUG_LOG(LL_SYS, "Entity: %d", i);

			auto distance = 0.f;
			for (auto hitbox : m_vHitboxes)
			{
				//DEBUG_LOG(LL_SYS, "hitbox: %d", hitbox);
				//DEBUG_LOG(LL_SYS, "bone: %d", Hitbox[hitbox].iBone);

				if (m_dwWorkType == ETriggerBotWorkTypes::WORKTYPE_HITBOX)
				{
					float flFovMax = 1.3f; // TODO: config
					float flDistance = 0.f;

					auto vTargetPos = m_pTargetEntity->GetBonePosition((CSPlayerBones)Hitbox[hitbox].m_iBone);
					if (m_pLocalEntity->RayTrace(vTargetPos - Vector3(flFovMax), vTargetPos + Vector3(flFovMax), flDistance))
					{
						DEBUG_LOG(LL_SYS, "RayTrace done! distance: %f", distance);
						HaxRoutine(i);
					}
				}
			}
		}
	}
}

void CTriggerBot::OnUpdate()
{
	if (m_bEnabled == true && m_bSuspended == false)
	{
		if (!m_bUseHoldKey || (m_bUseHoldKey && m_bPressedToHoldKey))
		{
			switch (m_dwWorkType)
			{
				case ETriggerBotWorkTypes::WORKTYPE_INCROSS:
				{
					InCrossRoutine();
				} break;

				case ETriggerBotWorkTypes::WORKTYPE_HITBOX: // TODO
				{
					HitboxRoutine();
				} break;
			}
		}
	}
}

// Utils
struct CompareMinDistance
{
	typedef std::pair <DWORD, double> pairType;
	auto operator() (const pairType & lhs, const pairType & rhs) const
	{
		return lhs.second < rhs.second;
	}
};

DWORD CTriggerBot::GetBestBoneId(std::map <DWORD, double> boneMap)
{
	auto min = *min_element(boneMap.begin(), boneMap.end(), CompareMinDistance());
	return min.first;
}

// Thread
#if HAX_MULTI_THREAD == true
DWORD WINAPI CTriggerBot::StartThreadRoutine(LPVOID lpParam)
{
	auto This = (CTriggerBot*)lpParam;
	return This->ThreadRoutine();
}

DWORD CTriggerBot::ThreadRoutine(void)
{
	DEBUG_LOG(LL_SYS, "Trigger bot routine started!");

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

bool CTriggerBot::CreateThread()
{
	DWORD dwThreadId = 0;
	m_hThread = ::CreateThread(nullptr, 0, StartThreadRoutine, (void*)this, 0, &dwThreadId);
	if (IS_VALID_HANDLE(m_hThread)) {
		DEBUG_LOG(LL_SYS, "Trigger bot thread created! TID: %u", dwThreadId);
		return true;
	}
	return false;
}

bool CTriggerBot::TerminateThread()
{
	auto bTerminateRet = ::TerminateThread(m_hThread, EXIT_SUCCESS);
	if (bTerminateRet == TRUE) {
		DEBUG_LOG(LL_SYS, "Trigger bot thread terminated!");
		return true;
	}
	return false;
}

void CTriggerBot::SendStopSignal()
{
	if (m_bThreadIsStopped)
		return;

	DEBUG_LOG(LL_SYS, "Stop signal received for: %u", GetThreadId(m_hThread));
	m_bThreadIsStopped = true;
}
#endif

