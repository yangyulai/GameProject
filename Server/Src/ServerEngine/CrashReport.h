﻿#pragma once
#include <string>

void NewInvalidParamHandler(const wchar_t* expression,
                            const wchar_t* function,
                            const wchar_t* file,
                            unsigned int line,
                            uintptr_t pReserved);

void NewPureCallHandler(void);

/*安放crash定位*/
void SetCrashReport(std::string strAppName);

void UnSetCrashReport();