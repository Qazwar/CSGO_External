#pragma once

class CConfigParser
{
	public:
		CConfigParser(const char* c_szFileName);
		~CConfigParser();

		bool	Load();
		bool	Save();

		bool	IsExistKey(const std::string & szKey);

		void	SetString(const std::string & szKey, const std::string & szValue);
		void	SetInteger(const std::string & szKey, uint64_t ullValue);
		void	SetBoolean(const std::string & szKey, bool bValue);
		void	SetReal(const std::string & szKey, double dValue);
		void	SetVector(const std::string & szKey, std::vector <std::string> vValues);
		void	SetMap(const std::string & szKey, std::map <std::string, std::string> mapValues);

		const char*			GetString(const std::string & szKey);
		unsigned long long	Get_uint64(const std::string & szKey);
		unsigned long		Get_uint32(const std::string & szKey);
		long long			Get_int64(const std::string & szKey);
		long				Get_int32(const std::string & szKey);
		bool				GetBoolean(const std::string & szKe);
		double				GetReal(const std::string & szKey);
		std::vector <std::string>				GetVector(const std::string & szKey);
		std::map	<std::string, std::string>	GetMap(const std::string & szKey);

	private:
		std::string															m_szFileName;
		std::string															m_szCurentSection;

		std::map <std::string /* szKey */, std::string /* szValue */>		m_dataMap;
};

