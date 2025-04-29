#include "Platform.h"
#include "LogMsgHandler.h"
#include <spdlog/spdlog.h>
#include "ConfigFile.h"
#include "LogService.h"
#include "../Message/Msg_ID.pb.h"
#include "../Message/Msg_Game.pb.h"
#include "../LogData/LogStruct.h"


CLogMsgHandler::CLogMsgHandler()
{
}

CLogMsgHandler::~CLogMsgHandler()= default;

bool CLogMsgHandler::Init(INT32 nReserved)
{
    return true;
}

bool CLogMsgHandler::UnInit()
{
    return true;
}

bool CLogMsgHandler::OnUpdate(UINT64 uTick)
{
    return true;
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

    boost::asio::co_spawn(
        sLogService.m_ioContext,
        [sql = std::string(szSql)]() -> boost::asio::awaitable<void> {
            boost::system::error_code ec;
            auto conn = co_await sLogService.m_pool.async_get_connection(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
            boost::mysql::results res;
            co_await conn->async_execute(sql, res);
            co_return;
        },
        boost::asio::detached
    );
    return true;
}

