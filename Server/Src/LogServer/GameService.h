#pragma once
#include <IBufferHandler.h>
#include "LogMsgHandler.h"
#include "DBInterface/MySqlClient.h"

class CGameService : public IPacketDispatcher
{
public:
	CGameService(const CGameService& other) = delete;
	CGameService(CGameService&& other) noexcept = delete;
	CGameService& operator=(const CGameService& other) = delete;
	CGameService& operator=(CGameService&& other) noexcept = delete;

private:
	CGameService(void);
	virtual ~CGameService(void);
public:
	static CGameService* GetInstancePtr();
	BOOL		Init();
	BOOL		UnInit();
	BOOL		Run();
	void Test();
	void Test2();
	void TestQuerySync();
	void TestQueryAsync();
	bool OnNewConnect(INT32 nConnID) override	;
	bool OnCloseConnect(INT32 nConnID) override;
	bool OnSecondTimer() override;
	bool DispatchPacket( NetPacket* pNetPacket) override;
	CLogMsgHandler		m_LogMsgHandler;
	boost::asio::io_context m_ioContext;
	std::unique_ptr<MySqlClient> m_dbClient;
};
