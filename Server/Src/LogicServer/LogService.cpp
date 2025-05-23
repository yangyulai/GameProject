﻿
#include "LogService.h"

#include "DataPool.h"
#include "StaticData.h"
#include "../Message/Msg_ID.pb.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"

#include "TimerManager.h"
#include "SimpleManager.h"
#include "GlobalDataMgr.h"
#include "MailManager.h"
#include "PayManager.h"
#include "GuildManager.h"
#include "ActivityManager.h"
#include "RankMananger.h"
#include "MsgHandlerManager.h"
#include "PacketHeader.h"
#include "TeamCopyMgr.h"
#include "WebCommandMgr.h"
#include "WatcherClient.h"
#include "SealManager.h"

//#include "LuaManager.h"
//#include "Lua_Script.h"

LogService::LogService(void)
{
    m_dwLogConnID    = 0;
    m_dwLoginConnID  = 0;
    m_dwDBConnID     = 0;
    m_dwCenterConnID = 0;   //中心服的连接ID
    m_uSvrOpenTime   = 0;
    m_dwDbErrorCount = 0;
    m_bRegSuccessed  = FALSE;
}

LogService::~LogService(void)
{
    m_dwLogConnID   = 0;
    m_dwLoginConnID = 0;
    m_dwDBConnID    = 0;
    m_dwCenterConnID  = 0;   //中心服的连接ID
    m_uSvrOpenTime  = 0;
    m_dwDbErrorCount = 0;
    m_bRegSuccessed = FALSE;
}

LogService* LogService::GetInstancePtr()
{
    static LogService _GameService;

    return &_GameService;
}

UINT32 LogService::GetLogSvrConnID()
{
    return m_dwLogConnID;
}

VOID LogService::RegisterMessageHanler()
{
    CMsgHandlerManager::GetInstancePtr()->RegisterMessageHandle(MSG_LOGIC_REGTO_LOGIN_ACK, &LogService::OnMsgRegToLoginAck, this);
    CMsgHandlerManager::GetInstancePtr()->RegisterMessageHandle(MSG_LOGIC_UPDATE_ACK,      &LogService::OnMsgUpdateInfoAck, this);
    CMsgHandlerManager::GetInstancePtr()->RegisterMessageHandle(MSG_DB_WRITE_ERROR_NTY,    &LogService::OnMsgDBWriteErrorNty, this);
    CMsgHandlerManager::GetInstancePtr()->RegisterMessageHandle(MSG_LOGIC_REGTO_CENTER_ACK, &LogService::OnMsgRegToCenterAck, this);
    CMsgHandlerManager::GetInstancePtr()->RegisterMessageHandle(MSG_LOGIC_REGTO_DBSVR_ACK, &LogService::OnMsgRegToDBSvrAck, this);
}

UINT64 LogService::GetSvrOpenTime()
{
    return m_uSvrOpenTime;
}

BOOL LogService::Init()
{
    CommonFunc::SetCurrentWorkDir("");

    if(!CLog::GetInstancePtr()->Start("LogicServer", "log"))
    {
        return FALSE;
    }

    spdlog::info("---------服务器开始启动--------");

    if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
    {
        spdlog::error("加载 servercfg.ini文件失败!");
        return FALSE;
    }

    if (CommonFunc::IsAlreadyRun("LogicServer" + CConfigFile::GetInstancePtr()->GetStringValue("areaid")))
    {
        spdlog::error("LogicServer己经在运行!");
        return FALSE;
    }

    CLog::GetInstancePtr()->SetLogLevel(CConfigFile::GetInstancePtr()->GetIntValue("logic_log_level"));

    UINT16 nPort = CConfigFile::GetInstancePtr()->GetRealNetPort("logic_svr_port");
    if (nPort <= 0)
    {
        spdlog::error("配制文件logic_svr_port配制错误!");
        return FALSE;
    }

    std::string strListenIp = CConfigFile::GetInstancePtr()->GetStringValue("logic_svr_ip");

    INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("logic_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn, this, strListenIp))
    {
        spdlog::error("启动服务失败!");
        return FALSE;
    }

    if (!CDataPool::GetInstancePtr()->InitDataPool())
    {
        spdlog::error("初始化共享内存池失败!");
        return FALSE;
    }

    ///////////////////////////////////
    //服务器启动之前需要加载的数据
    if (!CStaticData::GetInstancePtr()->LoadConfigData("Config.db"))
    {
        spdlog::error("加载静态配制数据失败!");
        return FALSE;
    }

    //if (!CLuaManager::GetInstancePtr()->Init())
    //{
    //  spdlog::error("初始化Lua环境失败!");
    //  return FALSE;
    //}

    //if (!luaopen_LuaScript(CLuaManager::GetInstancePtr()->GetLuaState()))
    //{
    //  spdlog::error("导出Lua接口失败!");
    //  return FALSE;
    //}

    //if (!CLuaManager::GetInstancePtr()->LoadAllLua(".\\Lua"))
    //{
    //  spdlog::error("加载lua代码失败!");
    //  return FALSE;
    //}

    ///////////////////////////////////
    //服务器启动之前需要加载的数据
    std::string strHost = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_ip");
    nPort = CConfigFile::GetInstancePtr()->GetIntValue("mysql_game_svr_port");
    std::string strUser = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_user");
    std::string strPwd = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_pwd");
    std::string strDb = CConfigFile::GetInstancePtr()->GetStringValue("mysql_game_svr_db_name");

    CppMySQL3DB tDBConnection;
    if(!tDBConnection.open(strHost.c_str(), strUser.c_str(), strPwd.c_str(), strDb.c_str(), nPort))
    {
        spdlog::error("Error: Can not open mysql database! Reason:%s", tDBConnection.GetErrorMsg());
        return FALSE;
    }

    BOOL bRet = FALSE;

    bRet = CGlobalDataManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet);

    bRet = CSimpleManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet)

    bRet = CMailManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet)

    bRet = CGuildManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet)

    bRet = CActivityManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet)

    bRet = CRankManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet)

    bRet = CGameSvrMgr::GetInstancePtr()->Init();
    ERROR_RETURN_FALSE(bRet);

    bRet = CTeamCopyMgr::GetInstancePtr()->Init();
    ERROR_RETURN_FALSE(bRet);

    bRet = CPayManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet);

    bRet = CSealManager::GetInstancePtr()->LoadData(tDBConnection);
    ERROR_RETURN_FALSE(bRet);

    bRet = CWebCommandMgr::GetInstancePtr()->Init();
    ERROR_RETURN_FALSE(bRet);

    bRet = m_LogicMsgHandler.Init(0);
    ERROR_RETURN_FALSE(bRet);

    RegisterMessageHanler();

    spdlog::error("---------服务器启动成功!--------");

    return TRUE;
}

BOOL LogService::Uninit()
{
    spdlog::error("==========服务器开始关闭=======================");

    ServiceBase::GetInstancePtr()->StopNetwork();

    m_LogicMsgHandler.Uninit();

    CDataPool::GetInstancePtr()->ReleaseDataPool();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::error("==========服务器关闭完成=======================");

    return TRUE;
}

BOOL LogService::Run()
{
    while (CWatcherClient::GetInstancePtr()->IsRun())
    {
        ServiceBase::GetInstancePtr()->Update();

        m_LogicMsgHandler.OnUpdate(CommonFunc::GetTickCount());

        ServiceBase::GetInstancePtr()->FixFrameNum(200);
    }

    return TRUE;
}

BOOL LogService::SendCmdToDBConnection(INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata)
{
    ERROR_RETURN_FALSE(m_dwDBConnID != 0);
    ERROR_RETURN_FALSE(ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_dwDBConnID, nMsgID, u64TargetID, dwUserData, pdata));
    return TRUE;
}


BOOL LogService::ConnectToLogServer()
{
    if (m_dwLogConnID != 0)
    {
        return TRUE;
    }
    INT32 nLogPort = CConfigFile::GetInstancePtr()->GetRealNetPort("log_svr_port");
    ERROR_RETURN_FALSE(nLogPort > 0);
    std::string strLogIp = CConfigFile::GetInstancePtr()->GetStringValue("log_svr_ip");
    CConnection* pConnection = ServiceBase::GetInstancePtr()->ConnectTo(strLogIp, nLogPort);
    ERROR_RETURN_FALSE(pConnection != NULL);

    m_dwLogConnID = pConnection->GetConnectionID();
    return TRUE;
}

BOOL LogService::ConnectToLoginSvr()
{
    if (m_dwLoginConnID != 0)
    {
        return TRUE;
    }
    INT32 nLoginPort = CConfigFile::GetInstancePtr()->GetIntValue("login_svr_port");
    ERROR_RETURN_FALSE(nLoginPort > 0);
    std::string strLoginIp = CConfigFile::GetInstancePtr()->GetStringValue("login_svr_ip");
    CConnection* pConnection = ServiceBase::GetInstancePtr()->ConnectTo(strLoginIp, nLoginPort);
    ERROR_RETURN_FALSE(pConnection != NULL);
    m_dwLoginConnID = pConnection->GetConnectionID();
    return TRUE;
}

BOOL LogService::ConnectToDBSvr()
{
    if (m_dwDBConnID != 0)
    {
        return TRUE;
    }
    INT32 nDBPort = CConfigFile::GetInstancePtr()->GetRealNetPort("db_svr_port");
    ERROR_RETURN_FALSE(nDBPort > 0);
    std::string strDBIp = CConfigFile::GetInstancePtr()->GetStringValue("db_svr_ip");
    CConnection* pConnection = ServiceBase::GetInstancePtr()->ConnectTo(strDBIp, nDBPort);
    ERROR_RETURN_FALSE(pConnection != NULL);
    m_dwDBConnID = pConnection->GetConnectionID();
    return TRUE;
}


BOOL LogService::ConnectToCenterSvr()
{
    if (m_dwCenterConnID != 0)
    {
        return TRUE;
    }
    INT32 nCenterPort = CConfigFile::GetInstancePtr()->GetIntValue("center_svr_port");
    ERROR_RETURN_FALSE(nCenterPort > 0);
    std::string strCenterIp = CConfigFile::GetInstancePtr()->GetStringValue("center_svr_ip");
    CConnection* pConnection = ServiceBase::GetInstancePtr()->ConnectTo(strCenterIp, nCenterPort);
    ERROR_RETURN_FALSE(pConnection != NULL);
    m_dwCenterConnID = pConnection->GetConnectionID();
    return TRUE;
}

BOOL LogService::RegisterToLoginSvr()
{
    LogicRegToLoginReq Req;
    UINT32 dwServerID = CConfigFile::GetInstancePtr()->GetIntValue("areaid");
    std::string strSvrName = CConfigFile::GetInstancePtr()->GetStringValue("areaname");
    UINT32 dwPort  = CConfigFile::GetInstancePtr()->GetRealNetPort("proxy_svr_port");
    UINT32 dwHttpPort = CConfigFile::GetInstancePtr()->GetRealNetPort("logic_svr_port");
    std::string strIp = CConfigFile::GetInstancePtr()->GetStringValue("logic_svr_ip");
    Req.set_serverid(dwServerID);
    Req.set_serverport(dwPort);
    Req.set_svrinnerip(strIp);
    Req.set_servername(strSvrName);
    Req.set_httpport(dwHttpPort);
    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_dwLoginConnID, MSG_LOGIC_REGTO_LOGIN_REQ, 0, 0, Req);
}

BOOL LogService::RegisterToDBSvr()
{
    LogicRegToDbSvrReq Req;
    UINT32 dwServerID = CConfigFile::GetInstancePtr()->GetIntValue("areaid");
    std::string strSvrName = CConfigFile::GetInstancePtr()->GetStringValue("areaname");
    Req.set_serverid(dwServerID);
    Req.set_servername(strSvrName);
    Req.set_processid(CommonFunc::GetCurProcessID());
    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_dwDBConnID, MSG_LOGIC_REGTO_DBSVR_REQ, 0, 0, Req);
}

BOOL LogService::RegisterToCenterSvr()
{
    SvrRegToSvrReq Req;
    UINT32 dwServerID = CConfigFile::GetInstancePtr()->GetIntValue("areaid");
    std::string strSvrName = CConfigFile::GetInstancePtr()->GetStringValue("areaname");
    UINT32 dwPort  = CConfigFile::GetInstancePtr()->GetRealNetPort("logic_svr_port");
    std::string strIp = CConfigFile::GetInstancePtr()->GetStringValue("logic_svr_ip");
    Req.set_serverid(dwServerID);
    Req.set_serverport(dwPort);
    Req.set_serverip(strIp);
    Req.set_servername(strSvrName);
    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_dwCenterConnID, MSG_LOGIC_REGTO_CENTER_REQ, 0, 0, Req);
}

BOOL LogService::OnNewConnect(INT32 nConnID)
{
    if(nConnID == m_dwLoginConnID)
    {
        RegisterToLoginSvr();

        return TRUE;
    }

    if(nConnID == m_dwCenterConnID)
    {
        RegisterToCenterSvr();

        return TRUE;
    }

    if (nConnID == m_dwDBConnID)
    {
        RegisterToDBSvr();

        return TRUE;
    }

    CWatcherClient::GetInstancePtr()->OnNewConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnCloseConnect(INT32 nConnID)
{
    if(m_dwLoginConnID == nConnID)
    {
        m_dwLoginConnID = 0;
        return TRUE;
    }

    if(m_dwLogConnID == nConnID)
    {
        m_dwLogConnID = 0;
        return TRUE;
    }

    if(m_dwDBConnID == nConnID)
    {
        m_dwDBConnID = 0;
        return TRUE;
    }

    if(m_dwCenterConnID == nConnID)
    {
        m_dwCenterConnID = 0;
        return TRUE;
    }

    CWatcherClient::GetInstancePtr()->OnCloseConnect(nConnID);

    CGameSvrMgr::GetInstancePtr()->OnCloseConnect(nConnID);

    return TRUE;
}

BOOL LogService::OnSecondTimer()
{
    ConnectToLogServer();

    ConnectToLoginSvr();

    ConnectToDBSvr();

    ConnectToCenterSvr();

    ReportServerStatus();

    CWatcherClient::GetInstancePtr()->OnSecondTimer();

    return TRUE;
}

BOOL LogService::DispatchPacket(NetPacket* pNetPacket)
{
    if (pNetPacket->m_nMsgID != MSG_LOGIC_UPDATE_ACK)
    {
        spdlog::info("Receive Message:[%s]", MessageID_Name((MessageID)pNetPacket->m_nMsgID).c_str());
    }

    if (CWatcherClient::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (CWebCommandMgr::GetInstancePtr()->DispatchPacket(pNetPacket))
    {
        return TRUE;
    }

    if (CMsgHandlerManager::GetInstancePtr()->FireMessage(pNetPacket->m_nMsgID, pNetPacket))
    {
        return TRUE;
    }

    PacketHeader* pHeader = (PacketHeader*)pNetPacket->m_pDataBuffer->GetBuffer();
    CPlayerObject* pPlayer = CPlayerManager::GetInstancePtr()->GetPlayer(pHeader->u64TargetID);
    if (pPlayer == NULL)
    {
        spdlog::error("CGameService::DispatchPacket Error Invalid u64TargetID:%lld, MessageID:%d", pHeader->u64TargetID, pNetPacket->m_nMsgID);
        return TRUE;
    }

    if (pPlayer->FireMessage(pNetPacket->m_nMsgID, pNetPacket))
    {
        return TRUE;
    }

    return TRUE;
}


UINT32 LogService::GetDBConnID()
{
    return m_dwDBConnID;
}

UINT32 LogService::GetLoginConnID()
{
    return m_dwLoginConnID;
}

UINT32 LogService::GetServerID()
{
    return CConfigFile::GetInstancePtr()->GetIntValue("areaid");
}

UINT32 LogService::GetCenterConnID()
{
    return m_dwCenterConnID;
}

BOOL LogService::ReportServerStatus()
{
    if (m_dwLoginConnID <= 0)
    {
        return TRUE;
    }

    if (!m_bRegSuccessed)
    {
        return TRUE;
    }

    static INT32 nCount = 30;
    nCount++;
    if (nCount < 59)
    {
        return TRUE;
    }

    nCount = 0;

    if (CGlobalDataManager::GetInstancePtr()->GetMaxOnline() < CPlayerManager::GetInstancePtr()->GetOnlineCount())
    {
        CGlobalDataManager::GetInstancePtr()->SetMaxOnline(CPlayerManager::GetInstancePtr()->GetOnlineCount());
    }

    LogicUpdateInfoReq Req;

    Req.set_maxonline(CGlobalDataManager::GetInstancePtr()->GetMaxOnline()); //最大在线人数
    Req.set_curonline(CPlayerManager::GetInstancePtr()->GetOnlineCount());   //当前在线人数
    Req.set_totalnum(CSimpleManager::GetInstancePtr()->GetTotalCount());     //总注册人数
    Req.set_cachenum(CPlayerManager::GetInstancePtr()->GetCount());          //当前缓存人数
    Req.set_serverid(CConfigFile::GetInstancePtr()->GetIntValue("areaid"));  //区服ID
    Req.set_servername(CConfigFile::GetInstancePtr()->GetStringValue("areaname")); //区服名字
    Req.set_dberrcnt(m_dwDbErrorCount);                                      //db写错误数

    return ServiceBase::GetInstancePtr()->SendMsgProtoBuf(m_dwLoginConnID, MSG_LOGIC_UPDATE_REQ, 0, 0, Req);
}

BOOL LogService::OnMsgRegToLoginAck(NetPacket* pNetPacket)
{
    LogicRegToLoginAck Ack;
    Ack.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());
    PacketHeader* pHeader = (PacketHeader*)pNetPacket->m_pDataBuffer->GetBuffer();

    ERROR_RETURN_TRUE(Ack.retcode() == MRC_SUCCESSED);
    //表示注册成功
    m_bRegSuccessed = TRUE;

    m_uSvrOpenTime = Ack.svropentime();

    spdlog::error("---------开服时间:%s--------", CommonFunc::TimeToString(m_uSvrOpenTime).c_str());

    return TRUE;
}

BOOL LogService::OnMsgRegToCenterAck(NetPacket* pNetPacket)
{
    SvrRegToSvrAck Ack;
    Ack.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());
    PacketHeader* pHeader = (PacketHeader*)pNetPacket->m_pDataBuffer->GetBuffer();
    ERROR_RETURN_TRUE(Ack.retcode() == MRC_SUCCESSED);
    return TRUE;
}

BOOL LogService::OnMsgUpdateInfoAck(NetPacket* pNetPacket)
{
    LogicUpdateInfoAck Ack;
    Ack.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());
    PacketHeader* pHeader = (PacketHeader*)pNetPacket->m_pDataBuffer->GetBuffer();
    ERROR_RETURN_TRUE(Ack.retcode() == 0);

    if (m_uSvrOpenTime == Ack.svropentime())
    {
        return TRUE;
    }

    m_uSvrOpenTime = Ack.svropentime();

    //这里处理开服时间发生改变有事件
    spdlog::error("---------开服时间:%s--------", CommonFunc::TimeToString(m_uSvrOpenTime).c_str());

    return TRUE;
}

BOOL LogService::OnMsgDBWriteErrorNty(NetPacket* pNetPacket)
{
    Msg_DbErrorCountNty Nty;
    Nty.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth());
    PacketHeader* pHeader = (PacketHeader*)pNetPacket->m_pDataBuffer->GetBuffer();

    m_dwDbErrorCount = Nty.errorcount();

    return TRUE;
}

BOOL LogService::OnMsgRegToDBSvrAck(NetPacket* pNetPacket)
{
    return TRUE;
}
