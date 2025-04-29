// LogServer.cpp : 定义控制台应用程序的入口点。
#include "LogService.h"
#include "CrashReport.h"
#include "CommandLine.h"

int main(int argc, char* argv[])
{
	SetCrashReport("LogServer");
	CCommandLine cmdLine(argc, argv);
	if (!sLogService.Init())
	{
		return 0;
	}
	sLogService.Run();
	sLogService.UnInit();
	UnSetCrashReport();
	return 0;
}

