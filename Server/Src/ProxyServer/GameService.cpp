﻿#include "stdafx.h"
#include "GameService.h"
#include "Position.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"
#include "WatcherClient.h"
CGameService::CGameService(void)
{
    m_nLogicConnID     = 0;
    m_bLogicConnect = FALSE;
}

CGameService::~CGameService(void)
{
    m_nLogicConnID = 0;
    m_bLogicConnect = FALSE;
}

CGameService* CGameService::GetInstancePtr()
{
    static CGameService _GameService;

    return &_GameService;
}

BOOL CGameService::Init()
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("ProxyServer", "log"))
    {
        return FALSE;
    }

    spdlog::info("---------服务器开始启动-----------");

    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("配制文件加载失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("ProxyServer" + CConfigFile::GetInstancePtr()->GetStringValue("areaid")))
    {
        spdlog::error("ProxyServer己经在运行!");
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("proxy_log_level"));

    UINT16 nPort = CConfigFile::GetInstancePtr()->GetRealNetPort("proxy_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件proxy_svr_port配制错误!");
        return FALSE;
    }

    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("proxy_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    //开启包序号检查
    ERROR_RETURN_FALSE(m_ProxyMsgHandler.Init(0));

    spdlog::info("---------服务器启动成功!--------");

    return TRUE;
}

BOOL CGameService::OnNewConnect(INT32 nConnID)
{
    if (nConnID == m_nLogicConnID)
    {
        m_bLogicConnect = TRUE;
    }

    m_ProxyMsgHandler.OnNewConnect(nConnID);

    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnCloseConnect(INT32 nConnID)
{
    if(nConnID == m_nLogicConnID)
    {
        m_nLogicConnID = 0;
        spdlog::error("CGameService::OnCloseConnect Disconnect From Logic Server.");
        return TRUE;
    }

    m_ProxyMsgHandler.OnCloseConnect(nConnID);

    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL CGameService::OnSecondTimer()
{
    ConnectToLogicSvr();

    return TRUE;
}

BOOL CGameService::DispatchPacket(NetPacket* pNetPacket)
{
    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (m_ProxyMsgHandler.DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    return FALSE;
}

INT32 CGameService::GetLogicConnID()
{
    return m_nLogicConnID;
}

BOOL CGameService::ConnectToLogicSvr()
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
    pConn->SetConnectionData(1);
    return TRUE;
}

BOOL CGameService::Uninit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    m_ProxyMsgHandler.Uninit();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL CGameService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        ServiceBase::GetInstancePtr()->FixFrameNum(200);
    }

    return TRUE;
}


