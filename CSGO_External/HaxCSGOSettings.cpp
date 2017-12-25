#include "main.h"
#include "Defines.h"
#include "Common.h"
#include "HaxData.h"
#include "XOR.h"

bool CHaxData::ParsePreDefinedPointers()
{
	auto m_dwLocalPlayer = GetUnsignedLong(XOR("pointer.m_dwLocalPlayer"));
	if (!m_dwLocalPlayer)
		return false;
	GetCSGODataInstance()->SetLocalPlayer_Pointer(m_dwLocalPlayer);

	auto m_dwEntityList = GetUnsignedLong(XOR("pointer.m_dwEntityList"));
	if (!m_dwEntityList)
		return false;
	GetCSGODataInstance()->SetEntityList_Pointer(m_dwEntityList);

	auto m_dwGlowObject = GetUnsignedLong(XOR("pointer.m_dwGlowObject"));
	if (!m_dwGlowObject)
		return false;
	GetCSGODataInstance()->SetGlowObjectManager_Pointer(m_dwGlowObject);

	auto m_dwForceAttack = GetUnsignedLong(XOR("pointer.m_dwForceAttack"));
	if (!m_dwForceAttack)
		return false;
	GetCSGODataInstance()->SetForceAttack_Pointer(m_dwForceAttack);

	auto m_dwForceJump = GetUnsignedLong(XOR("pointer.m_dwForceJump"));
	if (!m_dwForceJump)
		return false;
	GetCSGODataInstance()->SetForceJump_Pointer(m_dwForceJump);

	auto m_dwForceLeft = GetUnsignedLong(XOR("pointer.m_dwForceLeft"));
	if (!m_dwForceLeft)
		return false;
	GetCSGODataInstance()->SetForceLeft_Pointer(m_dwForceLeft);

	auto m_dwForceRight = GetUnsignedLong(XOR("pointer.m_dwForceRight"));
	if (!m_dwForceRight)
		return false;
	GetCSGODataInstance()->SetForceRight_Pointer(m_dwForceRight);

	auto m_dwMouseEnable = GetUnsignedLong(XOR("pointer.m_dwMouseEnable"));
	if (!m_dwMouseEnable)
		return false;
	GetCSGODataInstance()->SetMouseEnable_Pointer(m_dwMouseEnable);

	auto m_dwInput = GetUnsignedLong(XOR("pointer.m_dwInput"));
	if (!m_dwInput)
		return false;
	GetCSGODataInstance()->SetInput_Pointer(m_dwInput);

	auto m_dwSensitivity = GetUnsignedLong(XOR("pointer.m_dwSensitivity"));
	if (!m_dwSensitivity)
		return false;
	GetCSGODataInstance()->SetSensitivity_Pointer(m_dwSensitivity);

	auto m_bDormant = GetUnsignedLong(XOR("pointer.m_bDormant"));
	if (!m_bDormant)
		return false;
	GetCSGODataInstance()->SetDormant_Pointer(m_bDormant);

	auto m_dwClientState = GetUnsignedLong(XOR("pointer.m_dwClientState"));
	if (!m_dwClientState)
		return false;
	GetCSGODataInstance()->SetClientState_Pointer(m_dwClientState);

	auto m_dwGlobalVars = GetUnsignedLong(XOR("pointer.m_dwGlobalVars"));
	if (!m_dwGlobalVars)
		return false;
	GetCSGODataInstance()->SetGlobalVars_Pointer(m_dwGlobalVars);

	auto m_dwInGame = GetUnsignedLong(XOR("pointer.m_dwInGame"));
	if (!m_dwInGame)
		return false;
	GetCSGODataInstance()->SetIngame_Pointer(m_dwInGame);

	auto m_dwViewAngles = GetUnsignedLong(XOR("pointer.m_dwViewAngles"));
	if (!m_dwViewAngles)
		return false;
	GetCSGODataInstance()->SetViewAngles_Pointer(m_dwViewAngles);

	auto m_dwClientState_State = GetUnsignedLong(XOR("pointer.m_dwClientState_State"));
	if (!m_dwClientState_State)
		return false;
	GetCSGODataInstance()->SetClientStateState_Pointer(m_dwClientState_State);

	auto m_dwPlayerInfo = GetUnsignedLong(XOR("pointer.m_dwPlayerInfo"));
	if (!m_dwPlayerInfo)
		return false;
	GetCSGODataInstance()->SetPlayerInfo_Pointer(m_dwPlayerInfo);

	auto m_dwSendPackets = GetUnsignedLong(XOR("pointer.m_dwSendPackets"));
	if (!m_dwSendPackets)
		return false;
	GetCSGODataInstance()->SetSendPackets_Pointer(m_dwSendPackets);

	auto m_dwClientState_ViewAngles = GetUnsignedLong(XOR("pointer.m_dwClientState_ViewAngles"));
	if (!m_dwClientState_ViewAngles)
		return false;
	GetCSGODataInstance()->SetClientStateViewAngles_Pointer(m_dwClientState_ViewAngles);

	auto m_dwPlayerResource = GetUnsignedLong(XOR("pointer.m_dwPlayerResource"));
	if (!m_dwPlayerResource)
		return false;
	GetCSGODataInstance()->SetPlayerResource_Pointer(m_dwPlayerResource);

	auto m_dwViewMatrix = GetUnsignedLong(XOR("pointer.m_dwViewMatrix"));
	if (!m_dwViewMatrix)
		return false;
	GetCSGODataInstance()->SetViewMatrix_Pointer(m_dwViewMatrix);

	return true;
}

bool CHaxData::ParseCSGOGlowESPSettings()
{
	// radar
	auto bRadarIsEnabled = GetBoolean(XOR("misc.radar"));
	GetCSGODataInstance()->SetRadarEnabled(bRadarIsEnabled);

	auto bGlowIsEnabled = GetBoolean(XOR("glow.enabled"));
	GetCSGODataInstance()->SetGlowESPIsEnabled(bGlowIsEnabled);

	// glow common
	auto dwGlowEnableKey = GetUnsignedLong(XOR("glow.togglekey"));
	if (!dwGlowEnableKey)
		return false;
	GetCSGODataInstance()->SetGlowESPEnableKey(dwGlowEnableKey);

	auto dwGlowUpdateInterval = GetUnsignedLong(XOR("glow.interval"));
	if (!dwGlowUpdateInterval)
		return false;
	GetCSGODataInstance()->SetGlowESPUpdateInterval(dwGlowUpdateInterval);

	auto bGlowIsEnemyOnly = GetBoolean(XOR("glow.enemyonly"));
	GetCSGODataInstance()->SetGlowESPEnemyOnly(bGlowIsEnemyOnly);

	auto bGlowIsVisibleOnly = GetBoolean(XOR("glow.visibleonly"));
	GetCSGODataInstance()->SetGlowESPVisibleOnly(bGlowIsVisibleOnly);


	// colors
	auto dwGlowFriendColor = GetUnsignedLong(XOR("glow.color.friend"));
	if (!dwGlowFriendColor)
		return false;
	GetCSGODataInstance()->SetGlowESPFriendColor(dwGlowFriendColor);

	auto dwGlowEnemyColor = GetUnsignedLong(XOR("glow.color.enemy"));
	if (!dwGlowEnemyColor)
		return false;
	GetCSGODataInstance()->SetGlowESPEnemyColor(dwGlowEnemyColor);

	auto mapGlowC4Color = GetMap(XOR("glow.c4"));
	if (mapGlowC4Color.empty())
		return false;
	GetCSGODataInstance()->SetGlowESPC4Color(mapGlowC4Color);

	auto mapGlowC4HostColor = GetMap(XOR("glow.c4host"));
	if (mapGlowC4HostColor.empty())
		return false;
	GetCSGODataInstance()->SetGlowESPC4HostColor(mapGlowC4HostColor);

	auto mapGlowChickenColor = GetMap(XOR("glow.chicken"));
	if (mapGlowChickenColor.empty())
		return false;
	GetCSGODataInstance()->SetGlowESPChickenColor(mapGlowChickenColor);

	auto bGlowIsChamsEnabled = GetBoolean(XOR("glow.enablechams"));
	GetCSGODataInstance()->SetGlowESPChamsIsEnabled(bGlowIsChamsEnabled);

	return true;
}


bool CHaxData::ParseCSGOBunnyHopSettings()
{
	auto dwBHOPJumpKey = GetUnsignedLong(XOR("bhop.jumpkey"));
	if (!dwBHOPJumpKey)
		return false;
	GetCSGODataInstance()->SetBunnyHopJumpKey(dwBHOPJumpKey);

	auto dwBHOPEnableKey = GetUnsignedLong(XOR("bhop.togglekey"));
	if (!dwBHOPEnableKey)
		return false;
	GetCSGODataInstance()->SetBunnyHopToggleKey(dwBHOPEnableKey);

	auto flBHOPReleaseInterval = GetDouble(XOR("bhop.releaseinterval"));
	if (!flBHOPReleaseInterval)
		return false;
	auto dReleaseInterval = static_cast<double>(flBHOPReleaseInterval * 0.001);
	auto dwReleaseInterval = static_cast<DWORD>(dReleaseInterval * 1000.0);
	GetCSGODataInstance()->SetBunnyHopReleaseInterval(dwReleaseInterval);

	auto bBHOPUseAutoStrafe = GetBoolean(XOR("bhop.useautostrafe"));
	GetCSGODataInstance()->SetBunnyHopUseAutoStrafe(bBHOPUseAutoStrafe);

	return true;
}

bool CHaxData::ParseCSGOTriggerBotSettings()
{
	auto dwTriggerWorkType = GetUnsignedLong(XOR("triggerbot.worktype"));
	if (!dwTriggerWorkType)
		return false;
	GetCSGODataInstance()->SetTriggerBotWorkType(dwTriggerWorkType);

	auto dwTriggerEnableKey = GetUnsignedLong(XOR("triggerbot.togglekey"));
	if (!dwTriggerEnableKey)
		return false;
	GetCSGODataInstance()->SetTriggerBotEnableKey(dwTriggerEnableKey);

	auto dwTriggerInterval = GetUnsignedLong(XOR("triggerbot.interval"));
	GetCSGODataInstance()->SetTriggerBotInterval(dwTriggerInterval);

	auto bTriggerIsBurst = GetBoolean(XOR("triggerbot.burst"));
	GetCSGODataInstance()->SetTriggerBotIsBurst(bTriggerIsBurst);

	auto dwTriggerBurstCount = GetUnsignedLong(XOR("triggerbot.burstcount"));
	GetCSGODataInstance()->SetTriggerBotBurstCount(dwTriggerBurstCount);

	auto dwTriggerBurstInterval = GetUnsignedLong(XOR("triggerbot.burstinterval"));
	GetCSGODataInstance()->SetTriggerBotBurstInterval(dwTriggerBurstInterval);

	auto bTriggerAttackAsLongAsAlive = GetBoolean(XOR("triggerbot.attackaslongasalive"));
	GetCSGODataInstance()->SetTriggerBotIsAttackAsLongAsAlive(bTriggerAttackAsLongAsAlive);

	auto bTriggerCheckSpawnProtect = GetBoolean(XOR("triggerbot.checkspawnprotect"));
	GetCSGODataInstance()->SetTriggerBotCheckSpawnProtect(bTriggerCheckSpawnProtect);

	auto mapTriggerContinuousAttackIntervals = GetMap(XOR("triggerbot.continuous_attack_intervals"));
	if (mapTriggerContinuousAttackIntervals.empty())
		return false;
	GetCSGODataInstance()->SetContinuousAttackIntervals(mapTriggerContinuousAttackIntervals);

	auto mapTriggerSniperMode = GetMap(XOR("triggerbot.sniper_mode"));
	if (mapTriggerSniperMode.empty())
		return false;
	GetCSGODataInstance()->SetSniperModeValues(mapTriggerSniperMode);

	auto mapTriggerHoldKey = GetMap(XOR("triggerbot.hold_key_settings"));
	if (mapTriggerHoldKey.empty())
		return false;
	GetCSGODataInstance()->SetHoldKeyValues(mapTriggerHoldKey);

	auto bTriggerUseAimlock = GetBoolean(XOR("triggerbot.useaimlock"));
	GetCSGODataInstance()->SetTriggerBotAimLock(bTriggerUseAimlock);

	auto bTriggerAutoCrouch = GetBoolean(XOR("triggerbot.autocrouch"));
	GetCSGODataInstance()->SetTriggerBotAutoCrouch(bTriggerAutoCrouch);
	
	auto bTriggerBlockOnJump = GetBoolean(XOR("triggerbot.blockonjump"));
	GetCSGODataInstance()->SetTriggerBotBlockOnJump(bTriggerBlockOnJump);

	auto bTriggerBlockOnMove = GetBoolean(XOR("triggerbot.blockonmove"));
	GetCSGODataInstance()->SetTriggerBotBlockOnMove(bTriggerBlockOnMove);

	auto dwTriggerTargetBone = GetUnsignedLong(XOR("triggerbot.targetbone"));
	if (!dwTriggerTargetBone)
		return false;
	GetCSGODataInstance()->SetTriggerBotTargetBone(dwTriggerTargetBone);

	auto flTriggerFOV = GetDouble(XOR("triggerbot.fov"));
	if (!flTriggerFOV)
		return false;
	GetCSGODataInstance()->SetTriggerBotFOV(flTriggerFOV);

	auto vTriggerSmoothValues = GetVector(XOR("triggerbot.smooth"));
	if (vTriggerSmoothValues.empty())
		return false;
	GetCSGODataInstance()->SetTriggerBotSmooth(vTriggerSmoothValues);

	auto bTriggerTraceRay = GetBoolean(XOR("triggerbot.traceray"));
	GetCSGODataInstance()->SetTriggerBotTraceRay(bTriggerTraceRay);
	
	return true;
}

bool CHaxData::ParseCSGONoRecoilSettings()
{
	auto dwRCSEnableKey = GetUnsignedLong(XOR("norecoil.togglekey"));
	if (!dwRCSEnableKey)
		return false;
	GetCSGODataInstance()->SetNoRecoilEnableKey(dwRCSEnableKey);

	auto flRCSEfficiency = GetDouble(XOR("norecoil.efficiency"));
	GetCSGODataInstance()->SetNoRecoilEfficiency(flRCSEfficiency);

	return true;
}

bool CHaxData::ParseCSGOMiscSettings()
{
	auto dwMISCEnableKey = GetUnsignedLong(XOR("misc.enablekey"));
	if (!dwMISCEnableKey)
		return false;
	GetCSGODataInstance()->SetMiscEnableKey(dwMISCEnableKey);

	auto bMISCNoFlash = GetBoolean(XOR("misc.noflash"));
	GetCSGODataInstance()->SetNoFlashEnabled(bMISCNoFlash);

	auto bMISCNoHands = GetBoolean(XOR("misc.nohands"));
	GetCSGODataInstance()->SetNoHandsEnabled(bMISCNoHands);

	auto bMISCAutoPistol = GetBoolean(XOR("misc.autopistol"));
	GetCSGODataInstance()->SetAutoPistolEnabled(bMISCAutoPistol);

	auto bMISCChangeFov = GetBoolean(XOR("misc.changefov"));
	GetCSGODataInstance()->SetChangeFov(bMISCChangeFov);

	auto iMISCFovValue = GetInteger(XOR("misc.fovvalue"));
	if (!iMISCFovValue)
		return false;
	GetCSGODataInstance()->SetFovValue(iMISCFovValue);

	auto dwMISCFovKey = GetUnsignedLong(XOR("misc.fovkey"));
	if (!dwMISCFovKey)
		return false;
	GetCSGODataInstance()->SetFovKey(dwMISCFovKey);

	auto bMISCSlowAim = GetBoolean(XOR("misc.slowaim"));
	GetCSGODataInstance()->SetSlowAimEnabled(bMISCSlowAim);

	auto flMISCSlowAimSensivity = GetDouble(XOR("misc.slowaimsensivity"));
	GetCSGODataInstance()->SetSlowAimSensivity(flMISCSlowAimSensivity);

	auto bMISCFakeLag = GetBoolean(XOR("misc.fakelag"));
	GetCSGODataInstance()->SetFakeLagEnabled(bMISCFakeLag);

	auto dwMISCFakeLagKey = GetUnsignedLong(XOR("misc.fakelagkey"));
	if (!dwMISCFakeLagKey)
		return false;
	GetCSGODataInstance()->SetFakeLagKey(dwMISCFakeLagKey);

	return true;
}

bool CHaxData::ParseCSGOAimBotSettings()
{
	auto dwAimBotEnableKey = GetUnsignedLong(XOR("aimbot.enablekey"));
	if (!dwAimBotEnableKey)
		return false;
	GetCSGODataInstance()->SetAimBotEnableKey(dwAimBotEnableKey);

	return true;
}

