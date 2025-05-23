﻿// WatchServer.cpp : 定义控制台应用程序的入口点。
//


#include "GameService.h"
#include "CrashReport.h"

int main(int argc, char* argv[])
{
    SetCrashReport("WatchServer");

    if (CGameService::GetInstancePtr()->Init())
    {
        CGameService::GetInstancePtr()->Run();
    }

    CGameService::GetInstancePtr()->Uninit();

    UnSetCrashReport();

    return 0;
}

