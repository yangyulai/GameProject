#include "Platform.h"
#include "LogMsgHandler.h"
#include <spdlog/spdlog.h>
#include "ConfigFile.h"
#include "GameService.h"
#include "../Message/Msg_ID.pb.h"
#include "../Message/Msg_Game.pb.h"
#include "../LogData/LogStruct.h"


CLogMsgHandler::CLogMsgHandler(): m_nWriteCount(0), m_nLastWriteTime(0)
{
}

CLogMsgHandler::~CLogMsgHandler()= default;

bool CLogMsgHandler::Init(INT32 nReserved)
{
    std::string strHost = CConfigFile::GetInstancePtr()->GetStringValue("mysql_log_svr_ip");
    INT32 nPort = CConfigFile::GetInstancePtr()->GetIntValue("mysql_log_svr_port");
    std::string strUser = CConfigFile::GetInstancePtr()->GetStringValue("mysql_log_svr_user");
    std::string strPwd = CConfigFile::GetInstancePtr()->GetStringValue("mysql_log_svr_pwd");
    std::string strDb = CConfigFile::GetInstancePtr()->GetStringValue("mysql_log_svr_db_name");

    m_nLastWriteTime = 0;

    return TRUE;
}

bool CLogMsgHandler::UnInit()
{
    return TRUE;
}

bool CLogMsgHandler::OnUpdate(UINT64 uTick)
{
    if (m_nLastWriteTime == 0)
    {
        m_nLastWriteTime = uTick;
    }

    if (m_nWriteCount == 0)
    {
        return TRUE;
    }

    if (uTick - m_nLastWriteTime > 1000)
    {
        m_nLastWriteTime = uTick;
        m_nWriteCount = 0;
    }

    return TRUE;
}

bool CLogMsgHandler::DispatchPacket(NetPacket* pNetPacket)
{
    switch(pNetPacket->m_nMsgID)
    {
        case MSG_LOG_DATA_NTF:
	        {
		        spdlog::info("---Receive Message:[%s]----", "MSG_LOG_DATA_NTF");
        		if(OnMsgLogDataNtf(pNetPacket))
        		{
        			return true;
        		}
	        }break;
    }
    return false;
}
bool CLogMsgHandler::OnMsgLogDataNtf(NetPacket* pNetPacket)
{
    Log_BaseData* pData = (Log_BaseData*)pNetPacket->m_pDataBuffer->GetData();
    CHAR szSql[4096] = {0};

    switch (pData->m_LogType)
    {
        case ELT_ACCOUNT_CREATE:
        {
            Log_AccountCreate* p = (Log_AccountCreate*)pData;
            p->GetLogSql(szSql);
        }
        break;
        case ELT_ACCOUNT_LOGIN:
        {
            Log_AccountLogin* p = (Log_AccountLogin*)pData;
            p->GetLogSql(szSql);
        }
        break;
        case ELT_ROLE_CREATE:
        {
            Log_RoleCreate* p = (Log_RoleCreate*)pData;
            p->GetLogSql(szSql);
        }
        break;
        case ELT_ROLE_LOGIN:
        {
            Log_RoleLogin* p = (Log_RoleLogin*)pData;
            p->GetLogSql(szSql);
        }
        break;
        case ELT_ROLE_LOGOUT:
        {
            Log_RoleLogout* p = (Log_RoleLogout*)pData;
            p->GetLogSql(szSql);
        }
        break;
        case ELT_ROLE_CHAT:
        {
            Log_RoleChat* p = (Log_RoleChat*)pData;
            p->GetLogSql(szSql);
        }
        break;
        default:
            break;
    }
    m_nWriteCount += 1;

    if (m_nWriteCount > 1000)
    {
        m_nWriteCount = 0;
    }
    return TRUE;
}

