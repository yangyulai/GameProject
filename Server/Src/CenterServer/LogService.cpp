
#include "LogService.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"
#include "LogicSvrMgr.h"

LogService::LogService(void)
{

}

LogService::~LogService(void)
{

}

LogService* LogService::GetInstancePtr()
{
    static LogService _GameService;

    return &_GameService;
}

BOOL LogService::Init()
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("CenterServer", "log"))
    {
        return FALSE;
    }
    spdlog::info("---------服务器开始启动--------");
    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("CenterServer"))
    {
        spdlog::error("CenterServer己经在运行!");
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("center_log_level"));


    UINT16 nPort = CConfigFile::GetInstancePtr()->GetIntValue("center_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件center_svr_port配制错误!");
        return FALSE;
    }
    INT32 nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("center_svr_max_con");
    std::string strListenIp = CConfigFile::GetInstancePtr()->GetStringValue("center_svr_ip");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, strListenIp))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    ERROR_RETURN_FALSE(m_CenterMsgHandler.Init(0));

    //AsyncMySQLDB::GetInstancePtr()->Init();

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
    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    CLogicSvrMgr::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnSecondTimer()
{
    return TRUE;
}

BOOL LogService::DispatchPacket(NetPacket* pNetPacket)
{
    if (m_CenterMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL LogService::Uninit()
{
    spdlog::info("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();


    m_CenterMsgHandler.Uninit();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::info("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        CommonFunc::Sleep(1);
    }

    return TRUE;
}


