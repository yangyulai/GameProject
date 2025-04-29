
#include "LogService.h"
#include "DataBuffer.h"
#include "../Message/Msg_ID.pb.h"
#include "StaticData.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "HttpParameter.h"
#include "CommonConvert.h"
#include "WebActionDef.h"
#include "WatcherClient.h"

LogService::LogService(void)
{
    m_nLogicConnID     = 0;
    m_nProxyConnID     = 0;
    m_nServerID        = 0;
}

LogService::~LogService(void)
{
}

LogService* LogService::GetInstancePtr()
{
    static LogService _GameService;

    return &_GameService;
}

BOOL LogService::Init(INT32 nServerID, INT32 nPort)
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("GameServer", "log"))
    {
        return FALSE;
    }

    spdlog::info("---------服务器开始启动-ServerID:%d--Port:%d--------", nServerID, nPort);

    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    CHAR szSignName[128] = { 0 };
    snprintf(szSignName, 128, "GameServer%d_%d", nServerID, nPort);
    if (CommonFunc::IsAlreadyRun(szSignName))
    {
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("game_log_level"));

    m_nServerID = nServerID;

    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("game_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    CStaticData::GetInstancePtr()->LoadConfigData("Config.db");

    if(!m_SceneManager.Init(TRUE))
    {
        spdlog::error("启动场景管理器失败!");
        return FALSE;
    }

    spdlog::info("---------服务器启动成功!--------");
    return TRUE;
}

BOOL LogService::OnNewConnect(INT32 nConnID)
{
    if(nConnID == m_nLogicConnID)
    {
        RegisterToLogicSvr();
        m_SceneManager.SendCityReport();
        return TRUE;
    }

    if(nConnID == m_nProxyConnID)
    {
        RegisterToProxySvr();
        return TRUE;
    }

    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnCloseConnect(INT32 nConnID)
{
    if(m_nLogicConnID == nConnID)
    {
        m_nLogicConnID = 0;
    }

    if(m_nProxyConnID == nConnID)
    {
        m_nProxyConnID = 0;
    }

    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnSecondTimer()
{
    ConnectToLogicSvr();

    ConnectToProxySvr();

    CWatcherClient::GetInstancePtr()->OnSecondTimer();

    return TRUE;
}

BOOL LogService::DispatchPacket(NetPacket* pNetPacket)
{
    switch(pNetPacket->m_nMsgID)
    {
            PROCESS_MESSAGE_ITEM(MSG_GASVR_REGTO_PROXY_ACK, OnMsgRegToProxyAck)
            PROCESS_MESSAGE_ITEM(MSG_PHP_GM_COMMAND_REQ,    OnMsgWebCommandReq)
    }

    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_SceneManager.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

BOOL LogService::UnInit()
{
    spdlog::info("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    m_SceneManager.Uninit();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::info("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        m_SceneManager.OnUpdate(CommonFunc::GetTickCount());

        ServiceBase::GetInstancePtr()->FixFrameNum(200);
    }

    return TRUE;
}

BOOL LogService::SetLogicConnID( INT32 nConnID )
{
    m_nLogicConnID = nConnID;

    return TRUE;
}

UINT32 LogService::GetLogicConnID()
{
    return m_nLogicConnID;
}

UINT32 LogService::GetProxyConnID()
{
    return m_nProxyConnID;
}

BOOL LogService::ConnectToLogicSvr()
{
    if (m_nLogicConnID != 0)
    {
        return TRUE;
    }
    INT32 nLogicPort = CConfigFile::GetInstancePtr()->GetRealNetPort("logic_svr_port");
    ERROR_RETURN_FALSE(nLogicPort > 0);
    std::string strLogicIp = CConfigFile::GetInstancePtr()->GetStringValue("logic_svr_ip");
    CConnection* pConn = ServiceBase::GetInstancePtr()->ConnectTo(strLogicIp, nLogicPort);
    ERROR_RETURN_FALSE(pConn != NULL);
    m_nLogicConnID = pConn->GetConnectionID();

    return TRUE;
}

BOOL LogService::ConnectToProxySvr()
{
    if (m_nProxyConnID != 0)
    {
        return TRUE;
    }
    INT32 nProxyPort = CConfigFile::GetInstancePtr()->GetRealNetPort("proxy_svr_port");
    ERROR_RETURN_FALSE(nProxyPort > 0);
    std::string strProxyIp = CConfigFile::GetInstancePtr()->GetStringValue("proxy_svr_ip");
    CConnection* pConn = ServiceBase::GetInstancePtr()->ConnectTo(strProxyIp, nProxyPort);
    ERROR_RETURN_FALSE(pConn != NULL);
    m_nProxyConnID = pConn->GetConnectionID();
    return TRUE;
}

BOOL LogService::RegisterToLogicSvr()
{
    SvrRegToSvrReq Req;
    Req.set_serverid(m_nServerID);
    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_nLogicConnID, MSG_GAME_REGTO_LOGIC_REQ, 0, 0, Req);
}

BOOL LogService::RegisterToProxySvr()
{
    SvrRegToSvrReq Req;
    Req.set_serverid(m_nServerID);
    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_nProxyConnID, MSG_GASVR_REGTO_PROXY_REQ, 0, 0, Req);
}

UINT32 LogService::GetServerID()
{
    return m_nServerID;
}

BOOL LogService::OnMsgDefautReq(NetPacket* pNetPacket)
{
    return TRUE;
}

BOOL LogService::OnMsgRegToProxyAck(NetPacket* pNetPacket)
{
    SvrRegToSvrAck Ack;

    Ack.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());

    return TRUE;
}

BOOL LogService::OnMsgWebCommandReq(NetPacket* pNetPacket)
{
    CHAR szMsgBuf[1024] = { 0 };
    strncpy(szMsgBuf, pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());

    HttpParameter Params;
    Params.ParseStringToMap(szMsgBuf);
    std::string strAction = Params.GetStrValue("Action");
    spdlog::info("Web Action :%s", strAction.c_str());

    EWebAction eWebAction = (EWebAction)CommonConvert::StringToInt(strAction.c_str());
    switch (eWebAction)
    {
        case EWA_RELOAD_TABLE:
        {
            std::string strName = Params.GetStrValue("TableName");
            CStaticData::GetInstancePtr()->ReloadConfigData(strName);
        }
        break;
    }

    return TRUE;
}
