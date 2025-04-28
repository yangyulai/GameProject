#pragma once
#include <stdint.h>
#include <string>
#include <map>
class CCommandLine
{
public:
	CCommandLine(int32_t argc, char* argv[]);

	~CCommandLine();

	int32_t		GetIntValue(const std::string& strKey) const;

	std::string GetStrValue(const std::string& strKey) const;

	int64_t		GetLongValue(const std::string& strKey) const;

	float		GetFloatValue(const std::string& strKey) const;
protected:
	std::map<std::string, std::string> m_ParameterMap;
};