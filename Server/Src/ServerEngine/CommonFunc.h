#pragma once
#include <string>
#include <vector>
#include <winnt.h>
#include <stdint.h>

#define GET_BIT(X,Y) (((X) >> (Y-1)) & 1)
#define SET_BIT(X,Y) ((X) |= (1 << (Y-1)))
#define CLR_BIT(X,Y) ((X) &= (~(1<<Y-1)))

namespace CommonFunc
{
int32_t           GetProcessorNum();

std::string     GetCurrentWorkDir();

bool            SetCurrentWorkDir(std::string strPath);

std::string     GetCurrentExeDir();

bool            CreateDir(std::string& strDir);

bool            GetDirFiles(const char* pszDir, char* pszFileType, std::vector<std::string>& vtFileList, bool bRecursion);

bool            GetSubDirNames(const char* pszDir, const char* pszBegin, std::vector<std::string>& vtDirList, bool bRecursion);

int32_t           GetCurThreadID();

int32_t           GetCurProcessID();

void            Sleep(int32_t nMilliseconds);

int32_t           GetFreePhysMemory();

int32_t           GetRandNum(int32_t nType);

int32_t           GetLastError();

std::string     GetLastErrorStr(int32_t nError);

// HANDLE       CreateShareMemory(std::string strName, int32_t nSize);
//
// HANDLE       OpenShareMemory(std::string strName);

HANDLE          CreateShareMemory(int32_t nModuleID, int32_t nPage, int32_t nSize);

HANDLE          OpenShareMemory(int32_t nModuleID, int32_t nPage);

CHAR*           GetShareMemory(HANDLE hShm);

bool            ReleaseShareMemory(CHAR* pMem);

bool            CloseShareMemory(HANDLE hShm);

bool            KillProcess(int32_t nPid);

bool            IsProcessExist(int32_t nPid);

int32_t           GetProcessID(const char* pszProcName);

bool            StartProcess(const char* pszProcName, const char* pszCommandLine = NULL, const char*  pszWorkPath = NULL);

bool            IsAlreadyRun(std::string strSignName);

bool            PrintColorText(CHAR* pSzText, int32_t nColor);

bool            GetBitValue(UINT64 nValue, int32_t nPos);

bool            SetBitValue(UINT64& nValue, int32_t nPos, bool bValue);
}
