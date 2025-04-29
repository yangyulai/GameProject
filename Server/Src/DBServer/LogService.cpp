
#include "LogService.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"
LogService::LogService(void)
{
    m_nLogicConnID = 0;
    m_nLogicProcessID = 0;
}

LogService::~LogService(void)
{
    m_nLogicConnID = 0;
    m_nLogicProcessID = 0;
}

LogService* LogService::GetInstancePtr()
{
    static LogService _GameService;

    return &_GameService;
}

BOOL LogService::Init()
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("DBServer", "log"))
    {
        return FALSE;
    }

    spdlog::info("---------服务器开始启动-----------");

    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("DBServer" + CConfigFile::GetInstancePtr()->GetStringValue("areaid")))
    {
        spdlog::error("DBServer己经在运行!");
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("db_log_level"));

    UINT16 nPort = CConfigFile::GetInstancePtr()->GetRealNetPort("db_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件db_svr_port配制错误!");
        return FALSE;
    }

    INT32 nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("db_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, "127.0.0.1"))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    ERROR_RETURN_FALSE(m_DBMsgHandler.Init(0));

    ERROR_RETURN_FALSE(m_DBWriterManger.Init());

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

    return TRUE;
}

BOOL LogService::OnSecondTimer()
{
    CheckLogicServer();

    return TRUE;
}

BOOL LogService::DispatchPacket(NetPacket* pNetPacket)
{
    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_DBMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL LogService::SetLogicConnID(INT32 nConnID)
{
    m_nLogicConnID = nConnID;

    return TRUE;
}

INT32 LogService::GetLogicConnID()
{
    return m_nLogicConnID;
}

BOOL LogService::SetLogicProcessID(INT32 nProcesssID)
{
    m_nLogicProcessID = nProcesssID;

    return TRUE;
}

INT32 LogService::GetLogicProcessID()
{
    return m_nLogicProcessID;
}

BOOL LogService::CheckLogicServer()
{
    if (m_nLogicProcessID <= 0)
    {
        return TRUE;
    }

    if (CommonFunc::IsProcessExist(m_nLogicProcessID))
    {
        return TRUE;
    }

    CWatcherClient::GetInstancePtr()->StopServer();

    return TRUE;
}

BOOL LogService::UnInit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    m_DBMsgHandler.Uninit();

    m_DBWriterManger.Uninit();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        m_DBWriterManger.Update();

        ServiceBase::GetInstancePtr()->FixFrameNum(200);
    }

    return TRUE;
}

