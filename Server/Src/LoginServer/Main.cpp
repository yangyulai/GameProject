﻿// LoginServer.cpp : 定义控制台应用程序的入口点。
//


#include "GameService.h"
#include "CrashReport.h"
#include "WatcherClient.h"

int main(int argc, char* argv[])
{
	SetCrashReport("LoginServer");

	if (CGameService::GetInstancePtr()->Init())
	{
		CGameService::GetInstancePtr()->Run();
	}

	CGameService::GetInstancePtr()->UnInit();

	UnSetCrashReport();

	return 0;
}

