
#include "Platform.h"
#include "GameService.h"
#include "LogMsgHandler.h"

#include "CommonFunc.h"
#include "ConfigFile.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"
#include "Log.h"
#include "ServiceBase.h"
#pragma comment(lib, "Crypt32.lib")

CGameService::CGameService(void) : m_dbClient(std::make_unique<MySqlClient>(m_ioContext))
{

}

CGameService::~CGameService(void)
{

}

CGameService* CGameService::GetInstancePtr()
{
    static CGameService _GameService;

    return &_GameService;
}

BOOL CGameService::Init()
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
    bool ret = m_dbClient->connect_sync("127.0.0.1", "3306", "root", "367900", "db_log");
    if (!ret)
    {
        std::cerr << "Database connect failed!\n";
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

    ERROR_RETURN_FALSE(m_LogMsgHandler.Init(0));
    m_LogMsgHandler.Test();
    TestQuerySync();
    TestQueryAsync();
    spdlog::info("---------服务器启动成功!--------");

    return TRUE;
}


bool CGameService::OnNewConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

bool CGameService::OnCloseConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

bool CGameService::OnSecondTimer()
{

    return TRUE;
}

bool CGameService::DispatchPacket(NetPacket* pNetPacket)
{
    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_LogMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CGameService::Uninit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    google::protobuf::ShutdownProtobufLibrary();
    if (m_dbClient)
    {
        m_dbClient->close();
    }
    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL CGameService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();
        m_ioContext.run();
        m_LogMsgHandler.OnUpdate(CommonFunc::GetTickCount());

        ServiceBase::GetInstancePtr()->FixFrameNum(30);
    }

    return TRUE;
}

namespace mysql = boost::mysql;
namespace asio = boost::asio;
void CGameService::Test()
{


    // 1) I/O 上下文
    asio::io_context ctx;

    // 2) TCP 连接对象（使用默认 executor）
    mysql::tcp_connection conn(ctx);

    // 3) 用 resolver 得到一个 endpoint
    asio::ip::tcp::resolver resolver(ctx);
    auto endpoints = resolver.resolve("127.0.0.1", "3306");

    // 4) 构造握手参数： user, password, database
    mysql::handshake_params hs_params(
        "root",     // 用户名
        "367900", // 密码
        "db_log"  // 要使用的数据库
    );

    // 5) 同步连接（也有 async_connect）
    conn.connect(*endpoints.begin(), hs_params);

    // 6) 发一条简单查询
    boost::mysql::results results;
    conn.execute("SELECT accountid, openid FROM account_login", results);

    for (const auto& row : results.rows())
    {
        int id = row.at(0).as_int64();    // 假设 id 是 int
        std::string name = row.at(1).as_string();
        std::cout << "id = " << id << ", name = " << name << std::endl;
    }

    // 8) 关闭连接
    conn.close();
}

asio::awaitable<void> DoTest()
{
  
    mysql::tcp_connection conn(co_await asio::this_coro::executor);

    asio::ip::tcp::resolver resolver(co_await asio::this_coro::executor);
    auto endpoints = co_await resolver.async_resolve("127.0.0.1", "3306", asio::use_awaitable);

    mysql::handshake_params hs_params(
        "root",
        "367900",
        "db_log"
    );

    co_await conn.async_connect(*endpoints.begin(), hs_params, asio::use_awaitable);

    boost::mysql::results results;
    co_await conn.async_execute("SELECT accountid, openid FROM account_login", results, asio::use_awaitable);

    for (const auto& row : results.rows())
    {
        int id = row.at(0).as_int64();
        std::string name = row.at(1).as_string();
        std::cout << "id = " << id << ", name = " << name << std::endl;
    }

    co_await conn.async_close(asio::use_awaitable);
}

void CGameService::Test2()
{
    asio::io_context ctx;
    asio::co_spawn(ctx, DoTest(), asio::detached);
    ctx.run();
}
void CGameService::TestQuerySync()
{
    boost::mysql::results results;
    if (m_dbClient->query_sync("SELECT accountid, openid FROM account_login", results))
    {
        for (const auto& row : results.rows())
        {
            int id = row.at(0).as_int64();
            std::string name = row.at(1).as_string();
            std::cout << "id=" << id << " name=" << name << std::endl;
        }
    }
}

void CGameService::TestQueryAsync()
{
    boost::asio::co_spawn(m_ioContext,
        [this]() -> boost::asio::awaitable<void>
        {
            auto results = co_await m_dbClient->query_async("SELECT accountid, openid FROM account_login");
            for (const auto& row : results.rows())
            {
                int id = row.at(0).as_int64();
                std::string name = row.at(1).as_string();
                std::cout << "[Async] id=" << id << " name=" << name << std::endl;
            }
        },
        boost::asio::detached
    );
}