// CenterServer.cpp : 定义控制台应用程序的入口点。
//


#include "LogService.h"
#include "CrashReport.h"
#include "CommandLine.h"
#include "WatcherClient.h"

int main(int argc, char* argv[])
{
    SetCrashReport("CenterServer");

    CCommandLine cmdLine(argc, argv);

    if (!LogService::GetInstancePtr()->Init())
    {
        return 0;
    }

    LogService::GetInstancePtr()->Run();

    LogService::GetInstancePtr()->Uninit();

    UnSetCrashReport();

    return 0;
}

