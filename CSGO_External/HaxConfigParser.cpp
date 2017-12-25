#include "main.h"
#include "HaxConfigParser.h"
#include "TrimStr.h"
#include "Defines.h"

CConfigParser::CConfigParser(const char* c_szFileName) :
	m_szFileName(c_szFileName), m_szCurentSection("")
{
	m_dataMap.clear();
}
CConfigParser::~CConfigParser()
{
	m_szFileName.clear();
	m_szCurentSection.clear();
	m_dataMap.clear();
}

// TODO: Store as crypted
bool CConfigParser::Load()
{
	m_dataMap.clear();

	std::ifstream fInStream(m_szFileName);
	if (!fInStream)
		return false;

	std::size_t uiFixedSize;
	std::string szLine;
	while (std::getline(fInStream, szLine))
	{
		// Store fixed size
		uiFixedSize = szLine.size() - 1;

		// Empty line
		if (szLine.empty())
			continue;

		// Comment
		auto uiCommentAt = szLine.find("#");
		if (uiCommentAt != std::string::npos)
		{
			auto szUncommented = szLine.substr(0, uiCommentAt);
			if (szUncommented.empty())
				continue;

			szLine = szUncommented;
		}

		// Section
		if (szLine.find("[") == 0 && szLine.find("]") == uiFixedSize) {
			auto szSectionText = szLine.substr(szLine.find("[") + 1, uiFixedSize - 1);
			if (szSectionText != "~")
				m_szCurentSection = szSectionText;
		}

		// Parse
		std::string szKey = "";
		std::string szValue = "";

		auto separator = szLine.find("=");
		if (separator != std::string::npos)
		{
			szKey = szLine.substr(0, separator);
			szValue = szLine.substr(separator + 1, std::string::npos);
		}
		else
		{
			szKey = szLine;
		}

		str_trim::trim(szKey);
		str_trim::trim(szValue);
		std::transform(szKey.begin(), szKey.end(), szKey.begin(), tolower);

		// Key check
		if (szKey.empty() == false)
		{
			// Append section text
			if (m_szCurentSection.empty() == false)
				szKey = m_szCurentSection + "." + szKey;

			// Store on memory
			m_dataMap[szKey] = szValue;

			// if (g_haxApp->AppIsInitiliazed()) {
			// 	DEBUG_LOG(LL_SYS, "Cached config data(%d) %s:%s", m_dataMap.size(), szKey.c_str(), szValue.c_str());
			// }
		}
	}

	fInStream.close();
	return (m_dataMap.empty() == false);
}

bool CConfigParser::Save()
{
	// Read stream
	std::ifstream fInStream(m_szFileName); // Open input stream
	if (!fInStream)
		return false;

	// Copy old file data
	std::vector <std::string> vFileCopy;
	std::string szCopyLine;

	while (std::getline(fInStream, szCopyLine)) {
		// DEBUG_LOG(LL_SYS, "Copied file data: %s", szCopyLine.c_str());
		vFileCopy.push_back(szCopyLine);
	}

	// Compare routine
	bool bChanged = false;
	for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) // iterate current data map
	{
		// DEBUG_LOG(LL_SYS, "Current map data: %s-%s", iter->first.c_str(), iter->second.c_str());

		for (auto& szCurrentLine : vFileCopy) // Iterate File copy lines
		{
			std::string szKey = "";
			std::string szValue = "";

			auto separator = szCurrentLine.find("=");
			if (separator != std::string::npos)
			{
				szKey = szCurrentLine.substr(0, separator);
				szValue = szCurrentLine.substr(separator + 1, std::string::npos);

				str_trim::trim(szKey);
				str_trim::trim(szValue);

				if (szKey.empty() == false)
				{
					std::string szLowerCopy(szKey.begin(), szKey.end());
					std::transform(szLowerCopy.begin(), szLowerCopy.end(), szLowerCopy.begin(), tolower);

					if (szLowerCopy == iter->first)
					{
						// DEBUG_LOG(LL_SYS, "File data changed! Old: %s=%s New: %s=%s", szKey.c_str(), szValue.c_str(), iter->first.c_str(), iter->second.c_str());
						bChanged = true;
						szCurrentLine = iter->first + "=" + iter->second;
					}
				}
			}
		}
	}

	// Close
	fInStream.close();

	// Save new data
	if (bChanged)
	{
		std::ofstream fOutStream(m_szFileName); // Open output stream
		if (!fOutStream)
			return false;

		for (const auto& line : vFileCopy)
			fOutStream << line << '\n';

		fOutStream.close();
	}

	return true;
}

bool CConfigParser::IsExistKey(const std::string & szKey)
{
	auto szLowerKey = g_haxApp->GetFunctionsInstance()->szLower(szKey);
	return m_dataMap.find(szLowerKey) != m_dataMap.end();
}


void CConfigParser::SetString(const std::string & szKey, const std::string & szValue)
{
	auto szLowerKey = g_haxApp->AppIsInitiliazed() ? g_haxApp->GetFunctionsInstance()->szLower(szKey) : szKey;

	auto iter = m_dataMap.find(szLowerKey);
	if (iter == m_dataMap.end())
		return;

	// DEBUG_LOG(LL_SYS, "Set config data: %s-%s\n", szLowerKey.c_str(), szValue.c_str());
	m_dataMap[szLowerKey] = szValue;
}
void CConfigParser::SetInteger(const std::string & szKey, uint64_t ullValue)
{
	std::stringstream ss;
	ss << std::hex << ullValue;
	SetString(szKey, ss.str().c_str());
}
void CConfigParser::SetReal(const std::string & szKey, double dValue)
{
	std::stringstream ss;
	ss << dValue;
	SetString(szKey, ss.str().c_str());
}
void CConfigParser::SetBoolean(const std::string & szKey, bool bValue)
{
	SetString(szKey, bValue ? "true" : "false");
}
void CConfigParser::SetVector(const std::string & szKey, std::vector <std::string> vValues)
{
	std::stringstream ss;

	for (auto& line : vValues)
	{
		ss << line;

		auto pos = std::distance(vValues.begin(), std::find(vValues.begin(), vValues.end(), line));
		if (pos != vValues.size() - 1)
			ss << ",";
	}

	SetString(szKey, ss.str().c_str());
}
void CConfigParser::SetMap(const std::string & szKey, std::map <std::string, std::string> mapValues)
{
	std::stringstream ss;

	std::vector <std::string> vAddedEntries;
	for (auto& line : mapValues)
	{
		if (std::find(vAddedEntries.begin(), vAddedEntries.end(), line.first) != vAddedEntries.end())
			continue; // block duplicate entry

		ss << line.first << ":" << line.second;

		auto pos = std::distance(mapValues.begin(), std::find(mapValues.begin(), mapValues.end(), line));
		if (pos != mapValues.size() - 1)
			ss << ",";

		vAddedEntries.push_back(line.first);
	}

	SetString(szKey, ss.str().c_str());
}

const char* CConfigParser::GetString(const std::string & szKey)
{
	auto szLowerKey = g_haxApp->AppIsInitiliazed() ? g_haxApp->GetFunctionsInstance()->szLower(szKey) : szKey;

	auto iter = m_dataMap.find(szLowerKey);
	if (iter == m_dataMap.end())
		return nullptr;

	return iter->second.c_str();
}

unsigned long long CConfigParser::Get_uint64(const std::string & szKey)
{
	unsigned long long ullValue = 0;
	std::stringstream ss;

	ss << std::hex << ullValue;
	ss.str(GetString(szKey));
	ss >> std::hex >> ullValue;

	return ullValue;
}
unsigned long CConfigParser::Get_uint32(const std::string & szKey)
{
	auto ullValue = Get_uint64(szKey);
	return static_cast<unsigned long>(ullValue);
}
long long CConfigParser::Get_int64(const std::string & szKey)
{
	auto ullValue = Get_uint64(szKey);
	return static_cast<long long>(ullValue);
}
long CConfigParser::Get_int32(const std::string & szKey)
{
	auto ullValue = Get_uint64(szKey);
	return static_cast<long>(ullValue);
}

bool CConfigParser::GetBoolean(const std::string & szKey)
{
	bool bValue = false;
	auto szValue = GetString(szKey);

	auto szLowerValue = g_haxApp->GetFunctionsInstance()->szLower(szValue);

	return szLowerValue == "true" ? true : false;
}
double CConfigParser::GetReal(const std::string & szKey)
{
	double dValue = 0.0;
	std::stringstream ss;

	ss << dValue;
	ss.str(GetString(szKey));
	ss >> dValue;

	return dValue;
}
std::vector <std::string> CConfigParser::GetVector(const std::string & szKey)
{
	std::vector <std::string> vResult;
	std::string szValue = GetString(szKey);
	std::stringstream ss(szValue);

	while (ss.good())
	{
		std::string szSubstr = "";
		std::getline(ss, szSubstr, ',');
		vResult.push_back(szSubstr);
	}

	return vResult;
}
std::map <std::string, std::string> CConfigParser::GetMap(const std::string & szKey)
{
	std::map <std::string, std::string> mapResult;
	std::string szValue = GetString(szKey);
	std::stringstream ss(szValue);

	while (ss.good())
	{
		std::string szSubstr = "";
		std::getline(ss, szSubstr, ',');

		if (szSubstr.empty() == false)
		{
			std::string szMapKey = "";
			std::string szMapValue = "";

			auto separator = szSubstr.find(":");
			if (separator != std::string::npos)
			{
				szMapKey = szSubstr.substr(0, separator);
				szMapValue = szSubstr.substr(separator + 1, std::string::npos);

				if (szMapKey.empty() == false)
				{
					if (mapResult[szMapKey].empty() == true) // block duplicate entry
					{
						mapResult[szMapKey] = szMapValue;
					}
				}
			}
		}
	}

	return mapResult;
}
