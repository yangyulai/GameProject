#pragma once
#include <Platform.h>
#include <IBufferHandler.h>
#include "LogMsgHandler.h"
#include "DBInterface/MySqlClient.h"

class CGameService : public IPacketDispatcher
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();
	void Test();
	void Test2();
	void TestQuerySync();
	void TestQueryAsync();

	bool		OnNewConnect(INT32 nConnID);

	bool		OnCloseConnect(INT32 nConnID);

	bool		OnSecondTimer();

	bool		DispatchPacket( NetPacket* pNetPacket);

public:
	CLogMsgHandler		m_LogMsgHandler;
	boost::asio::io_context m_ioContext;
	std::unique_ptr<MySqlClient> m_dbClient;
public:
	//*********************消息处理定义开始******************************
	//*********************消息处理定义结束******************************
};
