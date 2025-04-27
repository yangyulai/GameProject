#include <algorithm>
#include "CommonConvert.h"
#include <Windows.h>

int32_t CommonConvert::StringToInt(const char* pStr)
{
    if(pStr == NULL)
    {
        return 0;
    }

    return atoi(pStr);
}

int64_t CommonConvert::StringToInt64(const char* pStr)
{
    if(pStr == NULL)
    {
        return 0;
    }

#ifdef WIN32
    return _atoi64(pStr);
#else
    return atoll(pStr);
#endif
}

float  CommonConvert::StringToFloat(const char* pStr)
{
    if(pStr == NULL)
    {
        return 0;
    }

    return (float)atof(pStr);
}

double CommonConvert::StringToDouble(const char* pStr)
{
    if (pStr == NULL)
    {
        return 0;
    }

    return strtod(pStr, NULL);
}

std::string CommonConvert::DoubleToString(double dValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%f", dValue);

    return std::string(szValue);
}

std::string CommonConvert::IntToString(int32_t nValue)
{
    char szValue[64] = {0};

    snprintf(szValue, 64, "%d", nValue);

    return std::string(szValue);
}

std::string CommonConvert::IntToString(int64_t uValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%lld", uValue);

    return std::string(szValue);
}

std::string CommonConvert::IntToString(uint32_t nValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%d", nValue);

    return std::string(szValue);
}

std::string CommonConvert::IntToString(uint64_t uValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%lld", uValue);

    return std::string(szValue);
}

bool CommonConvert::StringToPos(const char* pStr, float& x, float& y, float& z)
{
    if(pStr == NULL)
    {
        return false;
    }

    char szTempBuf[256] = { 0 };
    strncpy(szTempBuf, pStr, strlen(pStr));

    char* pPos = strchr(szTempBuf, ',');
    if(pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    x = CommonConvert::StringToFloat(szTempBuf + 1);

    char* pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if(pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    y = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ')');
    if(pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    z = CommonConvert::StringToFloat(pOldPos);

    return true;
}

bool CommonConvert::StringToBox(const char* pStr, float& left, float& top, float& right, float& bottom)
{
    if (pStr == NULL)
    {
        return false;
    }

    char szTempBuf[256] = { 0 };
    strncpy(szTempBuf, pStr, strlen(pStr));

    char* pPos = strchr(szTempBuf, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    left = CommonConvert::StringToFloat(szTempBuf + 1);

    char* pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    top = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    right = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ')');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    bottom = CommonConvert::StringToFloat(pOldPos);

    return true;
}

std::string CommonConvert::FloatToString(float fValue, int32_t nPrecision, bool bRound)
{
    char szValue[64] = {0};

    if((bRound) && (nPrecision > 0) && (nPrecision < 6))
    {
        float fRoundValue = 5;

        for(int i = 0; i < nPrecision + 1; i++)
        {
            fRoundValue *= 0.1f;
        }

        fValue += fRoundValue;
    }

    snprintf(szValue, 64, "%f", fValue);

    char* pChar = strchr(szValue, '.');
    if(pChar == NULL)
    {
        return std::string(szValue);
    }

    *(pChar + nPrecision + 1) = '\0';

    return std::string(szValue);
}

bool CommonConvert::SpliteString(std::string strSrc, std::string strDelim, std::vector<std::string>& vtStr)
{
    vtStr.clear();
    if (strDelim.empty())
    {
        vtStr.push_back(strSrc);
        return true;
    }

    if (strSrc.size() <= 0)
    {
        return true;
    }

    std::string::iterator subStart, subEnd;
    subStart = strSrc.begin();
    while (true)
    {
        subEnd = std::search(subStart, strSrc.end(), strDelim.begin(), strDelim.end());
        std::string temp(subStart, subEnd);
        if (!temp.empty())
        {
            vtStr.push_back(temp);
        }
        if (subEnd == strSrc.end())
        {
            break;
        }
        subStart = subEnd + strDelim.size();
    }

    return true;
}

bool CommonConvert::SpliteStringByMuti(std::string strSrc, std::string strDelim, std::vector<std::string>& vtStr)
{
    int32_t posStart = -1;

    for (int32_t i = 0; i < strSrc.size(); i++)
    {
        if (strDelim.find(strSrc.at(i)) != std::string::npos)
        {
            if (posStart < 0)
            {
                continue;
            }

            vtStr.emplace_back(std::string(strSrc, posStart, i - posStart));
            posStart = -1;
        }
        else if (posStart < 0)
        {
            posStart = i;
        }
    }

    if (posStart >= 0 && posStart < (int32_t)strSrc.size())
    {
        vtStr.emplace_back(std::string(strSrc, posStart, strSrc.size() - posStart));
    }

    return true;
}


bool CommonConvert::SpliteStringByBlank(std::string strSrc, std::vector<std::string>& vtStr)
{
    vtStr.clear();
    bool bWordStart = false;
    std::string::size_type posStart;
    std::string::size_type posCur = 0;

    while (posCur < strSrc.size())
    {
        if (strSrc.at(posCur) == ' ' || strSrc.at(posCur) == '\t')
        {
            if (!bWordStart)
            {
                posCur += 1;
                continue;
            }

            if (posCur > posStart)
            {
                vtStr.push_back(strSrc.substr(posStart, posCur - posStart));
            }

            bWordStart = false;
            posStart = 0;
            posCur += 1;
        }
        else
        {
            if (!bWordStart)
            {
                posStart = posCur;
                bWordStart = true;
            }
            posCur += 1;
        }
    }

    return true;
}

bool CommonConvert::ReplaceString(std::string& str, const std::string& pattern, const std::string& newpat)
{
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for (size_t pos = str.find(pattern, 0); pos != std::string::npos; pos = str.find(pattern, pos + nsize))
    {
        str.replace(pos, psize, newpat);
    }

    return true;
}

bool CommonConvert::StringToVector(const char* pStrValue, int32_t IntVector[], int32_t nSize, char cDelim)
{
    if (pStrValue == NULL)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    int32_t nIndex = 0;
    while (pEndPos != NULL)
    {
        //*pEndPos = 0;
        IntVector[nIndex++] = StringToInt(pBeginPos);
        if (nIndex >= nSize)
        {
            return true;
        }

        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0 && nIndex < nSize)
    {
        IntVector[nIndex++] = StringToInt(pBeginPos);
    }

    return true;
}

bool CommonConvert::StringToVector(const char* pStrValue, std::vector<int32_t>& vtInt, char cDelim /*= ','*/)
{
    if (pStrValue == NULL)
    {
        return false;
    }

    if (strlen(pStrValue) > 1024)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    while (pEndPos != NULL)
    {
        int32_t nValue = StringToInt(pBeginPos);
        vtInt.push_back(nValue);
        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0)
    {
        int32_t nValue = StringToInt(pBeginPos);
        vtInt.push_back(nValue);
    }

    return true;
}

bool CommonConvert::StringToVector(const char* pStrValue, float FloatVector[], int32_t nSize, char cDelim /*= ','*/)
{
    if (pStrValue == NULL)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    int32_t nIndex = 0;
    while (pEndPos != NULL)
    {
        FloatVector[nIndex++] = StringToFloat(pBeginPos);
        if (nIndex >= nSize)
        {
            return true;
        }

        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0 && nIndex < nSize)
    {
        FloatVector[nIndex++] = StringToFloat(pBeginPos);
    }

    return true;
}

bool CommonConvert::SpliteString(std::string strSrc,  char cDelim, std::vector<std::string>& vtStr)
{
    vtStr.clear();

    std::string::size_type posStart, posEnd;
    posEnd = strSrc.find(cDelim);
    posStart = 0;
    while(std::string::npos != posEnd)
    {
        vtStr.emplace_back(strSrc.substr(posStart, posEnd - posStart));

        posStart = posEnd + 1;
        posEnd = strSrc.find(cDelim, posStart);
    }

    if(posStart != strSrc.length())
    {
        vtStr.emplace_back(strSrc.substr(posStart));
    }

    return true;
}

/*
std::wstring CommonConvert::Utf8_To_Unicode( std::string strSrc )
{
#ifdef WIN32
    wchar_t wBuff[102400] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), -1, wBuff, 102400);
    std::wstring strRet = wBuff;
    return strRet;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(strSrc.c_str());
#endif
}

std::string CommonConvert::Unicode_To_Uft8( std::wstring wstrValue )
{
#ifdef WIN32
    char sBuff[102400] = { 0 };
    WideCharToMultiByte(CP_UTF8, 0, wstrValue.c_str(), -1, sBuff, 102400, NULL, NULL);
    std::string strRet = sBuff;
    return strRet;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstrValue.c_str());

#endif
}

std::wstring CommonConvert::Ansi_To_Unicode( std::string strSrc )
{
    wchar_t wBuff[102400] = {0};
#ifdef WIN32
    MultiByteToWideChar(CP_ACP,  0, strSrc.c_str(), -1, wBuff, 102400);
    std::wstring strRet = wBuff;
    return strRet;
#else
    setlocale(LC_CTYPE, "chs");
    std::mbstowcs(wBuff, strSrc.c_str(), 102400);
#endif
}

std::string CommonConvert::Unicode_To_Ansi( std::wstring strValue )
{
    char sBuff[102400] = { 0 };
#ifdef WIN32
    WideCharToMultiByte(CP_ACP, 0, strValue.c_str(), -1, sBuff, 102400, NULL, NULL);
    return std::string(sBuff);
#else
    setlocale(LC_CTYPE, "chs");
    std::wcstombs(sBuff, strValue.c_str(), 102400);
#endif
}

std::string CommonConvert::Utf8_To_Ansi( std::string strSrc )
{
    wchar_t wBuff[102400] = {0};
    char sBuff[102400] = {0};
#ifdef WIN32
    MultiByteToWideChar(CP_UTF8, 0, strSrc.c_str(), -1, wBuff, 102400);
    WideCharToMultiByte(CP_ACP, 0, wBuff, -1, sBuff, 102400, NULL, NULL);
    std::string strRet = sBuff;
    return strRet;
#else

#endif
}
*/
std::string CommonConvert::Ansi_To_Uft8( std::string strSrc )
{
    wchar_t wBuff[102400] = {0};
    char sBuff[102400] = {0};
#ifdef WIN32
    MultiByteToWideChar(CP_ACP,  0, strSrc.c_str(), -1, wBuff, 102400);
    WideCharToMultiByte(CP_UTF8, 0, wBuff, -1, sBuff, 102400, NULL, NULL);
    std::string strRet = sBuff;
    return strRet;
#else
#endif
}


bool CommonConvert::IsTextUTF8(const char* str, int32_t nLength)
{
    int32_t i;
    int32_t nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    uint8_t chr;
    bool bAllAscii = true; //如果全部都是ASCII, 说明不是UTF-8
    for (i = 0; i < nLength; i++)
    {
        chr = *(str + i);
        if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
        {
            bAllAscii = false;
        }
        if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                {
                    nBytes = 6;
                }
                else if (chr >= 0xF8)
                {
                    nBytes = 5;
                }
                else if (chr >= 0xF0)
                {
                    nBytes = 4;
                }
                else if (chr >= 0xE0)
                {
                    nBytes = 3;
                }
                else if (chr >= 0xC0)
                {
                    nBytes = 2;
                }
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else //多字节符的非首字节,应为 10xxxxxx
        {
            if ((chr & 0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }
    if (nBytes > 0) //违返规则
    {
        return false;
    }
    if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
    {
        return true;
    }
    return true;
}

int32_t CommonConvert::GetValidUtf8Length(char* pStr, int32_t nLen)
{
    int32_t nCurPos = 0;
    bool bUtfStart = false;
    int32_t nUtfLen = 0;
    int32_t nBkLen = 0;
    for (int i = 0; i < nLen; i++)
    {
        uint8_t nCurChar = *(pStr + i);
        if (!bUtfStart)
        {
            if ((nCurChar & 0x80) == 0)
            {
                nUtfLen = 0;
                nCurPos += 1;
            }
            else if (nCurChar >= 0xFC && nCurChar <= 0xFD)
            {
                nBkLen = nUtfLen = 6;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xF8)
            {
                nBkLen = nUtfLen = 5;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xF0)
            {
                nBkLen = nUtfLen = 4;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xE0)
            {
                nBkLen = nUtfLen = 3;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xC0)
            {
                nBkLen = nUtfLen = 2;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else
            {
                return nCurPos;
            }
        }
        else
        {
            if ((nCurChar & 0xC0) != 0x80)
            {
                return nCurPos;
            }
            else
            {
                nUtfLen -= 1;
                if (nUtfLen <= 0)
                {
                    bUtfStart = false;
                    nCurPos += nBkLen;
                    nBkLen = nUtfLen = 0;
                }
            }
        }
    }

    return nCurPos;
}

std::string CommonConvert::TruncateUtf8(char* pStr, int32_t nLen)
{
    int32_t nUtf8Len = GetValidUtf8Length(pStr, nLen);

    return std::string(pStr, nUtf8Len);
}

int32_t CommonConvert::VersionToInt(const std::string& strVersion )
{
    int32_t nValue[3] = { 0 };
    StringToVector(strVersion.c_str(), nValue, 3, '.');
    return nValue[0] * 1000000 + nValue[1] * 1000 + nValue[2];
}

int32_t CommonConvert::CountSymbol(const char* pStr, char cSymbol )
{
    if(pStr == NULL)
    {
        return 0;
    }

    int32_t nCount = 0;

    const char* pTemp = pStr;
    while(*pTemp != '\0')
    {
        if(*pTemp == cSymbol)
        {
            nCount += 1;
        }

        pTemp += 1;
    }

    return nCount;
}

bool CommonConvert::HasSymbol(const char* pStr, const char* pszSymbol)
{
    if (pStr == NULL || pszSymbol == NULL)
    {
        return false;
    }

    const char* pSym = pszSymbol;
    while (*pSym != '\0')
    {
        const char* pTemp = pStr;
        while (*pTemp != '\0')
        {
            if (*pTemp == *pSym)
            {
                return true;
            }

            pTemp += 1;
        }

        pSym += 1;
    }

    return false;
}

bool CommonConvert::StringTrim(std::string& strValue)
{
    if(!strValue.empty())
    {
        strValue.erase(0, strValue.find_first_not_of((" \n\r\t\v\f")));
        strValue.erase(strValue.find_last_not_of((" \n\r\t\v\f")) + 1);
    }
    return true;
}

/*      nLen的大小必须小于等于pszDest的大小                      */
bool CommonConvert::StrCopy(char* pszDest, const char* pszSrc, int32_t nLen)
{
    if (pszDest == NULL || pszSrc == NULL)
    {
        return false;
    }

    if (nLen <= 0)
    {
        return false;
    }

    int32_t nSrcLen = (int32_t)strlen(pszSrc);

    int32_t nCpyLen = 0;
    if ((nLen - 1) > nSrcLen)
    {
        nCpyLen = nSrcLen;
    }
    else
    {
        nCpyLen = nLen - 1;
    }

    //这个方法并不会自动加上终止符
    strncpy(pszDest, pszSrc, nCpyLen);
    pszDest[nCpyLen] = '\0';

    return true;
}

bool CommonConvert::EscapeString(char* pszDest, int32_t nLen)
{
    for (int i = 0; i < nLen; i++)
    {
        if (pszDest[i] == '\'')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\\')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\"')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\0')
        {
            break;
        }
    }

    return true;
}

BYTE CommonConvert::FromHex(const BYTE& x)
{
    return isdigit(x) ? x - '0' : x - 'A' + 10;
}

std::string CommonConvert::UrlDecode(const std::string& strIn)
{
    std::string strOut;
    for (size_t ix = 0; ix < strIn.size(); ix++)
    {
        BYTE ch = 0;
        if (strIn[ix] == '%')
        {
            ch = (FromHex(strIn[ix + 1]) << 4);
            ch |= FromHex(strIn[ix + 2]);
            ix += 2;
        }
        else if (strIn[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = strIn[ix];
        }
        strOut += (char)ch;
    }
    return strOut;
}
