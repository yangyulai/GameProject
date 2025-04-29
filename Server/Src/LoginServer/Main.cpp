// LoginServer.cpp : 定义控制台应用程序的入口点。
//


#include "LogService.h"
#include "CrashReport.h"
#include "WatcherClient.h"

int main(int argc, char* argv[])
{
	SetCrashReport("LoginServer");

	if (LogService::GetInstancePtr()->Init())
	{
		LogService::GetInstancePtr()->Run();
	}

	LogService::GetInstancePtr()->UnInit();

	UnSetCrashReport();

	return 0;
}

