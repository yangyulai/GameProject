
#include "Platform.h"
#include "LogService.h"
#include "LogMsgHandler.h"
#include <sol/sol.hpp>
#include "CommonFunc.h"
#include "CommonTime.h"
#include "ConfigFile.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "Log.h"
#include "ServiceBase.h"
#pragma comment(lib, "Crypt32.lib")

namespace mysql = boost::mysql;
namespace asio = boost::asio;

LogService::LogService()
:m_pool([&]()
{
        sol::state lua;
        lua.open_libraries(sol::lib::base);
        auto result = lua.script_file("J:/github/GameProject/Server/Src/LuaScript/config.lua", [](lua_State*, sol::protected_function_result pfr) {
            return pfr;
            });
        sol::table db = lua["database"];
		std::string host = db["host"].get_or<std::string>("127.0.0.1");
        std::string user = db["user"].get_or<std::string>("root");
		std::string password = db["password"].get_or<std::string>("");
		std::string database = db["database"].get_or<std::string>("test");
		uint16_t port = db["port"].get_or<uint16_t>(3306);
		int pool_size = db["pool_size"].get_or<int>(4L);
        boost::mysql::pool_params params;
        params.server_address.emplace_host_and_port(host,port);
		params.username = user;
		params.retry_interval = std::chrono::seconds(1);   // 默认 1s，可改小
		params.connect_timeout = std::chrono::seconds(3);
		params.ping_interval = std::chrono::seconds(3);
		params.password = password;
		params.database = database;
		params.initial_size = pool_size;
        params.max_size = 2 * pool_size;
        params.thread_safe = true;
        return boost::mysql::connection_pool(
            m_ioContext.get_executor(),
            std::move(params)
        );
}())
{
    m_pool.async_run(boost::asio::detached);
	m_ioThread = std::thread([this]() {
		m_ioContext.run();
		});
}
LogService::~LogService()
{
    m_pool.cancel();
	m_ioContext.stop();
    if (m_ioThread.joinable()) m_ioThread.join();
}

LogService& LogService::Instance()
{
	static LogService inst;
	return inst;
}

BOOL LogService::Init()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    CommonFunc::SetCurrentWorkDir("");
    spdlog::info("---------服务器开始启动--------");
    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("LogServer" + CConfigFile::GetInstancePtr()->GetStringValue("areaid")))
    {
        spdlog::error("LogServer己经在运行!");
        return FALSE;
    }
    UINT16 nPort = CConfigFile::GetInstancePtr()->GetRealNetPort("log_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件log_svr_port配制错误!");
        return FALSE;
    }

    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("log_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, "127.0.0.1"))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }
    Test();

    ERROR_RETURN_FALSE(m_LogMsgHandler.Init(0));
    spdlog::info("---------服务器启动成功!--------");

    return TRUE;
}
bool LogService::OnNewConnect(INT32 nConnID)
{
	spdlog::info("新连接连接成功, ConnID:{}", nConnID);
    return TRUE;
}

bool LogService::OnCloseConnect(INT32 nConnID)
{
	spdlog::info("连接关闭, ConnID:{}", nConnID);
    return TRUE;
}

bool LogService::OnSecondTimer()
{

    return TRUE;
}

bool LogService::DispatchPacket(NetPacket* pNetPacket)
{
    if (m_LogMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

void LogService::enqueue_log(std::string sqlTpl, std::vector<boost::mysql::field> params)
{
	std::lock_guard lk(queue_mtx_);
	queue_.emplace_back(std::move(sqlTpl), std::move(params));
	if (queue_.size() > 1000) {
		spdlog::warn("日志队列过长，当前长度: {}", queue_.size());
	}
}

BOOL LogService::UnInit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    google::protobuf::ShutdownProtobufLibrary();
    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    size_t counter = 0;
    while (true)
    {
        ServiceBase::GetInstancePtr()->Update();
        m_LogMsgHandler.OnUpdate(CommonFunc::GetTickCount());
        ServiceBase::GetInstancePtr()->FixFrameNum(30);
    }
    return TRUE;
}

void LogService::Test()
{

    boost::asio::co_spawn(
        m_ioContext,
        [this]() -> boost::asio::awaitable<void> {
            boost::system::error_code ec;
            auto conn = co_await m_pool.async_get_connection(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
            boost::mysql::results res;
            co_await conn->async_execute("SELECT accountid, openid FROM account_login", res);
            for (const auto& row : res.rows())
            {
                int id = row.at(0).as_int64();
                std::string name = row.at(1).as_string();
                std::cout << "[Async] id=" << id << " name=" << name << std::endl;
            }
            co_return;
        },
        boost::asio::detached
    );
}