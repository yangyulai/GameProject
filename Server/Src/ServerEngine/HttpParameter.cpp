#include "HttpParameter.h"
#include "CommonConvert.h"

HttpParameter::HttpParameter()
{

}

HttpParameter::~HttpParameter(void)
{
	m_ParameterMap.clear();
}

bool HttpParameter::ParseStringToMap(const std::string& strParam)
{
	if(strParam.length() <= 0)
	{
		return false;
	}

	std::vector<std::string> strVector;
	CommonConvert::SpliteString(strParam, "&", strVector);
	std::vector<std::string>::iterator itend = strVector.end();
	for(std::vector<std::string>::iterator it = strVector.begin(); it != itend; it++)
	{
		std::vector<std::string> strVectorSub;
		CommonConvert::SpliteString(*it, "=", strVectorSub);
		if((strVectorSub.begin() + 1) != strVectorSub.end())
		{
			m_ParameterMap.insert(std::make_pair(strVectorSub[0], strVectorSub[1]));
		}
	}

	return true;
}

std::string HttpParameter::GetResultString()
{
	std::string strTemp;
	strTemp.reserve(1024);

	for (auto itor = m_ParameterMap.begin(); itor != m_ParameterMap.end(); itor++)
	{
		strTemp += itor->first;
		strTemp += "=";
		strTemp += itor->second;
		strTemp += "&";
	}

	return strTemp;
}

bool HttpParameter::HasKey(const std::string& strKey) const
{
	auto itor = m_ParameterMap.find(strKey);
	if(itor != m_ParameterMap.end())
	{
		return true;
	}

	return false;
}

int32_t HttpParameter::GetIntValue(const std::string& strKey) const
{
	auto itor = m_ParameterMap.find(strKey);
	if(itor != m_ParameterMap.end())
	{
		return CommonConvert::StringToInt(itor->second.c_str());
	}
	return 0;
}

std::string HttpParameter::GetStrValue(const std::string& strKey) const
{
	auto it = m_ParameterMap.find(strKey);
	if(it != m_ParameterMap.end())
	{
		return it->second;
	}
	return "";
}

int64_t HttpParameter::GetLongValue(const std::string& strKey) const
{
	auto it = m_ParameterMap.find(strKey);
	if(it != m_ParameterMap.end())
	{
		return CommonConvert::StringToInt64(it->second.c_str());
	}

	return 0;
}

float HttpParameter::GetFloatValue(const std::string& strKey) const
{
	auto it = m_ParameterMap.find(strKey);
	if(it != m_ParameterMap.end())
	{
		return CommonConvert::StringToFloat(it->second.c_str());
	}
	return 0.0f;
}

bool HttpParameter::SetKeyValue(const std::string& strKey, int32_t intValue)
{
	m_ParameterMap.insert(std::make_pair(strKey, CommonConvert::IntToString(intValue)));
	return true;
}

bool HttpParameter::SetKeyValue(const std::string& strKey, std::string& strValue)
{
	m_ParameterMap.insert(std::make_pair(strKey, strValue));
	return true;
}

bool HttpParameter::SetKeyValue(const std::string& strKey, int64_t longValue)
{
	m_ParameterMap.insert(std::make_pair(strKey, CommonConvert::IntToString(longValue)));
	return true;
}

bool HttpParameter::SetKeyValue(const std::string& strKey, float floatValue)
{
	m_ParameterMap.insert(std::make_pair(strKey, CommonConvert::FloatToString(floatValue)));
	return true;
}

