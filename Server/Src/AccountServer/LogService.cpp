
#include "LogService.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"

LogService::LogService(void)
{
    m_nLogSvrConnID    = 0;
}

LogService::~LogService(void)
{
}

LogService* LogService::GetInstancePtr()
{
    static LogService _GameService;

    return &_GameService;
}

INT32 LogService::GetLogSvrConnID()
{
    return m_nLogSvrConnID;
}

BOOL LogService::Init()
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("AccountServer", "log"))
    {
        return FALSE;
    }

    spdlog::info("---------服务器开始启动-----------");

    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("AccountServer"))
    {
        spdlog::error("AccountServer己经在运行!");
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("account_log_level"));

    UINT16 nPort = CConfigFile::GetInstancePtr()->GetIntValue("account_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件account_svr_port配制错误!");
        return FALSE;
    }

    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("account_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, "127.0.0.1"))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    ERROR_RETURN_FALSE(m_AccountMsgHandler.Init(0));

    spdlog::info("---------服务器启动成功!--------");

    return TRUE;
}

BOOL LogService::OnNewConnect(INT32 nConnID)
{
    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnCloseConnect(INT32 nConnID)
{
    if (nConnID == m_nLogSvrConnID)
    {
        m_nLogSvrConnID = 0;
    }

    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnSecondTimer()
{
    ConnectToLogServer();

    return TRUE;
}

BOOL LogService::DispatchPacket(NetPacket* pNetPacket)
{
    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_AccountMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL LogService::UnInit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    m_AccountMsgHandler.Uninit();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        ServiceBase::GetInstancePtr()->FixFrameNum(100);
    }

    return TRUE;
}

BOOL LogService::ConnectToLogServer()
{
    if (m_nLogSvrConnID != 0)
    {
        return TRUE;
    }
    INT32 nLogPort = CConfigFile::GetInstancePtr()->GetRealNetPort("log_svr_port");
    ERROR_RETURN_FALSE(nLogPort > 0);
    std::string strStatIp = CConfigFile::GetInstancePtr()->GetStringValue("log_svr_ip");
    CConnection* pConnection = ServiceBase::GetInstancePtr()->ConnectTo(strStatIp, nLogPort);
    ERROR_RETURN_FALSE(pConnection != NULL);
    m_nLogSvrConnID = pConnection->GetConnectionID();
    return TRUE;
}
