#include "stdafx.h"
#include "GameService.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"

CGameService::CGameService(void)
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
    spdlog::info("---------服务器启动成功!--------");

    return TRUE;
}


BOOL CGameService::OnNewConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnCloseConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnSecondTimer()
{

    return TRUE;
}

BOOL CGameService::DispatchPacket(NetPacket* pNetPacket)
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

    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL CGameService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        m_LogMsgHandler.OnUpdate(CommonFunc::GetTickCount());

        ServiceBase::GetInstancePtr()->FixFrameNum(30);
    }

    return TRUE;
}
#include <boost/mysql.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio.hpp> 
#include <iostream>

void CGameService::Test()
{
    namespace mysql = boost::mysql;
    namespace asio = boost::asio;

    // 1) I/O 上下文
    asio::io_context ctx;

    // 2) TCP 连接对象（使用默认 executor）
    mysql::tcp_connection conn(ctx);

    // 3) 用 resolver 得到一个 endpoint
    asio::ip::tcp::resolver resolver(ctx);
    auto endpoints = resolver.resolve("127.0.0.1", "3306");

    // 4) 构造握手参数： user, password, database
    mysql::handshake_params hs_params(
        "your_user",     // 用户名
        "your_password", // 密码
        "your_database"  // 要使用的数据库
    );

    // 5) 同步连接（也有 async_connect）
    conn.connect(*endpoints.begin(), hs_params);

    // 6) 发一条简单查询
    boost::mysql::results results;
    conn.async_execute("SELECT id, name FROM employees WHERE salary > 50000", results);


    // 8) 关闭连接
    conn.close();
}