#pragma once
#include <vector>
#include <string>

namespace CommonConvert
{
int32_t StringToInt(const char* pStr);

int64_t StringToInt64(const char* pStr);

float StringToFloat(const char* pStr);

double StringToDouble(const char* pStr);

std::string DoubleToString(double dValue);

std::string IntToString(uint32_t nValue);

std::string IntToString(int32_t nValue);

std::string IntToString(uint64_t nValue);

std::string IntToString(int64_t nValue);

std::string FloatToString(float fValue, int32_t nPrecision = -1, bool bRound = false);

bool  StringToPos(const char* pStr, float& x, float& y, float& z);

bool  StringToBox(const char* pStr, float& left, float& top, float& right, float& bottom);

// std::wstring Utf8_To_Unicode(std::string strValue);
//
// std::string  Unicode_To_Uft8(std::wstring wstrValue);
//
// std::wstring Ansi_To_Unicode(std::string strValue);
//
// std::string Unicode_To_Ansi(std::wstring strValue);
//
// std::string Utf8_To_Ansi(std::string strValue);
//
std::string Ansi_To_Uft8(std::string strValue);

bool IsTextUTF8(const char* str, int32_t nLength);

int32_t GetValidUtf8Length(char* pStr, int32_t nLen);

std::string TruncateUtf8(char* pStr, int32_t nLen);

bool SpliteString(std::string strSrc,  std::string strDelim, std::vector<std::string>& vtStr);

bool SpliteStringByMuti(std::string strSrc, std::string strDelim, std::vector<std::string>& vtStr);

bool SpliteStringByBlank(std::string strSrc, std::vector<std::string>& vtStr);

bool SpliteString(std::string strSrc, char cDelim, std::vector<std::string>& vtStr);

bool ReplaceString(std::string& strSrc, const std::string& pattern, const std::string& newpat);

bool StringToVector(const char* pStrValue, std::vector<int32_t>& vtInt, char cDelim = ',');

bool StringToVector(const char* pStrValue, int32_t IntVector[], int32_t nSize, char cDelim = ',');

bool StringToVector(const char* pStrValue, float FloatVector[], int32_t nSize, char cDelim = ',');

int32_t VersionToInt(const std::string& strVersion);

int32_t CountSymbol(const char* pStr, char cSymbol);

bool HasSymbol(const char* pStr, const char* pszSymbol);

bool StringTrim(std::string& strValue);

bool StrCopy(char* pszDest, const char* pszSrc, int32_t nLen);

bool EscapeString(char* pszDest, int32_t nLen);

uint8_t FromHex(const uint8_t& x);

std::string UrlDecode(const std::string& strIn);
}
