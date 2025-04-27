#pragma once
struct NetPacket;
#include "DBInterface/CppMysql.h"

class CLogMsgHandler
{
public:
	CLogMsgHandler();

	~CLogMsgHandler();

	BOOL		Init(INT32 nReserved);

	BOOL		Uninit();

	BOOL		OnUpdate(UINT64 uTick);

	BOOL		DispatchPacket(NetPacket* pNetPacket);
	bool Test();

public:
	//*********************消息处理定义开始******************************
	BOOL		OnMsgLogDataNtf(NetPacket* pNetPacket);
	//*********************消息处理定义结束******************************

	CppMySQL3DB     m_DBConnection;

	UINT32			m_nWriteCount;

	UINT64          m_nLastWriteTime;

};