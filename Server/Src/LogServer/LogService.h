#pragma once
#include <IBufferHandler.h>
#include <queue>

#include "LogMsgHandler.h"
#include "DBInterface/MySqlClient.h"

class LogService : public IPacketDispatcher
{
public:
	LogService(const LogService& other) = delete;
	LogService(LogService&& other) noexcept = delete;
	LogService& operator=(const LogService& other) = delete;
	LogService& operator=(LogService&& other) noexcept = delete;
	LogService();
	virtual ~LogService();
	static LogService& Instance();
	BOOL Init();
	BOOL UnInit();
	BOOL Run();
	void Test();
	bool OnNewConnect(INT32 nConnID) final;
	bool OnCloseConnect(INT32 nConnID) final;
	bool OnSecondTimer() final override;
	bool DispatchPacket( NetPacket* pNetPacket) final;
	void enqueue_log(
		std::string sqlTpl,
		std::vector<boost::mysql::field> params
	);
	CLogMsgHandler m_LogMsgHandler;
	std::thread m_ioThread;
	boost::asio::io_context m_ioContext;
	boost::mysql::connection_pool m_pool;
	std::mutex queue_mtx_;
	std::vector<std::pair<std::string, std::vector<boost::mysql::field>>> queue_;
};
#define sLogService LogService::Instance()