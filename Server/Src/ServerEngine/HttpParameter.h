#pragma once
#include <map>
#include <string>
#include <cstdint>

class HttpParameter
{
public:
	HttpParameter();

	~HttpParameter(void);

	bool		ParseStringToMap(const std::string& strParam);

	std::string GetResultString();

	bool		HasKey(const std::string& strKey) const;

	int32_t		GetIntValue(const std::string& strKey) const;

	std::string GetStrValue(const std::string& strKey) const;

	int64_t		GetLongValue(const std::string& strKey) const;

	float		GetFloatValue(const std::string& strKey) const;

	bool		SetKeyValue(const std::string& strKey, int32_t intValue);

	bool		SetKeyValue(const std::string& strKey, std::string& strValue);

	bool		SetKeyValue(const std::string& strKey, int64_t longValue);

	bool		SetKeyValue(const std::string& strKey, float floatValue);

protected:
	std::map<std::string, std::string> m_ParameterMap;
};