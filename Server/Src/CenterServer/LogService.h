﻿#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "CenterMsgHandler.h"

class LogService : public IPacketDispatcher
{
private:
	LogService(void);
	virtual ~LogService(void);

public:
	static LogService* GetInstancePtr();

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnect(INT32 nConnID);

	BOOL		OnCloseConnect(INT32 nConnID);

	BOOL		OnSecondTimer();

	BOOL		DispatchPacket( NetPacket* pNetPacket);

public:
	CCenterMsgHandler		m_CenterMsgHandler;

	//*********************消息处理定义开始******************************
	//*********************消息处理定义结束******************************
};

#endif
