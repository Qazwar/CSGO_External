#include "main.h"
#include "Common.h"
#include "HaxData.h"
#include "Defines.h"

const std::size_t g_MaxAttempt = 3;

CHaxData::CHaxData(DWORD dwTargetPID, DWORD dwGameCode) :
	m_dwTargetPID(dwTargetPID), m_dwGameCode(dwGameCode), m_cSettings(nullptr), m_cCSGOData(nullptr)
{
	m_cSettings = std::make_shared<CConfigParser>(XOR("H4x.cfg"));
	auto bLoadSettingsRet = m_cSettings->Load();
	assert(bLoadSettingsRet);

	if (m_dwGameCode == EGameCodes::GAMECODE_CSGO)
		m_cCSGOData = std::make_shared<CCSGOData>(dwTargetPID);
	else
		assert(1 && "unknown game code");
}

CHaxData::~CHaxData()
{
	m_dwTargetPID = 0;
	m_dwGameCode = 0;

	m_cSettings.reset();
	m_cSettings = nullptr;

	m_cCSGOData.reset();
	m_cCSGOData = nullptr;
}

bool CHaxData::ReloadSettings()
{
	auto bLoadSettingsRet = m_cSettings->Load();
	assert(bLoadSettingsRet);
	return bLoadSettingsRet;
}


int CHaxData::GetInteger(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto iValue = m_cSettings->Get_int32(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s: %d", szName.c_str(), iValue);
		return iValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return 0;
}
DWORD CHaxData::GetUnsignedLong(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto dwValue = m_cSettings->Get_uint32(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s: %u", szName.c_str(), DWORD(dwValue));
		return dwValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return 0UL;
}
double CHaxData::GetDouble(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto dValue = m_cSettings->GetReal(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s: %f", szName.c_str(), dValue);
		return dValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return 0.f;
}
std::string CHaxData::GetString(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto szValue = m_cSettings->GetString(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s: %s", szName.c_str(), szValue);
		return szValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return std::string("");
}
bool CHaxData::GetBoolean(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto bValue = m_cSettings->GetBoolean(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s: %d", szName.c_str(), bValue);
		return bValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return false;
}
std::vector <std::string> CHaxData::GetVector(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto vValue = m_cSettings->GetVector(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s", szName.c_str());
		return vValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return std::vector<std::string>();
}
std::map <std::string, std::string> CHaxData::GetMap(const std::string & szName)
{
	if (m_cSettings->IsExistKey(szName)) {
		auto mValue = m_cSettings->GetMap(szName);
		DEBUG_LOG(LL_SYS, "Config data found! %s", szName.c_str());
		return mValue;
	}

	DEBUG_LOG(LL_ERR, "%s is NOT exist config settings!", szName.c_str());
	return std::map<std::string, std::string>();
}

