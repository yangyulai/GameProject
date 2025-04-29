#pragma once
#include <stdint.h>
#include "CommonConvert.h"
#include "CommonFunc.h"
#include "CommonTime.h"
#include "CommonSocket.h"
#include "spdlog/spdlog.h"
enum ELogType
{
    ELT_LOG_TYPE_NONE = 0,
    ELT_ACCOUNT_CREATE = 1,
    ELT_ACCOUNT_LOGIN,
    ELT_ROLE_CREATE,
    ELT_ROLE_LOGIN,
    ELT_ROLE_LOGOUT, //离线
    ELT_ROLE_EXP,
    ELT_ROLE_DIAMOND,
    ELT_ROLE_GOLD,
    ELT_ROLE_LEVEL,
    ELT_ROLE_CHAT, //聊天
};
template<>
struct fmt::formatter<ELogType> {
    // 默认解析，无需额外格式
    constexpr auto parse(fmt::format_parse_context& ctx) {
        return ctx.begin();
    }

    // 根据枚举值返回对应名称
    template <typename FormatContext>
    auto format(ELogType e, FormatContext& ctx) const  {
        const char* name = nullptr;
        switch (e) {
        case ELT_LOG_TYPE_NONE:      name = "NONE";        break;
        case ELT_ACCOUNT_CREATE:     name = "ACCOUNT_CREATE"; break;
        case ELT_ACCOUNT_LOGIN:      name = "ACCOUNT_LOGIN";  break;
        case ELT_ROLE_CREATE:        name = "ROLE_CREATE";    break;
        case ELT_ROLE_LOGIN:         name = "ROLE_LOGIN";     break;
        case ELT_ROLE_LOGOUT:        name = "ROLE_LOGOUT";    break;
        case ELT_ROLE_EXP:           name = "ROLE_EXP";       break;
        case ELT_ROLE_DIAMOND:       name = "ROLE_DIAMOND";   break;
        case ELT_ROLE_GOLD:          name = "ROLE_GOLD";      break;
        case ELT_ROLE_LEVEL:         name = "ROLE_LEVEL";     break;
        case ELT_ROLE_CHAT:          name = "ROLE_CHAT";      break;
        default:                     name = "UNKNOWN";        break;
        }
        // 输出名字到 fmt 的缓冲区
        return fmt::format_to(ctx.out(), "{}", name);
    }
};
//角色日志
struct Log_BaseData
{
    ELogType    m_LogType   = ELT_LOG_TYPE_NONE; //日志类型
    int64_t      m_uAccountID = 0; //账号ID
    int64_t      m_uRoleID    = 0; //角色ID
    int64_t      m_uOpTime    = 0; //日志发生时间
    int32_t       m_nChannel   = 0; //渠道
    int32_t       m_nAreaID    = 0; //区服ID
    char        m_szIdfa[64] = { 0 };  //客户端idfa
    char        m_szRoleName[64] = { 0 };
    //以下两条仅角色日志有效
    int32_t       m_nLevel     = 0;//角色等级
    int32_t       m_nVipLevel  = 0;//角色VIP等级
    int32_t       m_nAddWay    = 0;//
        virtual ~Log_BaseData() = default;
    // 返回 SQL 模板和对应参数包
    virtual std::pair<std::string, std::vector<boost::mysql::field>> 
    make_sql_and_params() const = 0;
};

struct Log_AccountCreate : public Log_BaseData
{
    int32_t       m_dwVersion      = 0;   //客户端版本号
    int32_t       m_dwIpAddr       = 0;    //登录IP
    char        m_szImei[64]    = { 0 };  //手机的MEI
    char        m_szModel[64]   = { 0 }; //手机的机型
    char        m_szUuid[64]    = { 0 };  //手机的uuid;
    char        m_szOpenID[64]  = { 0 };//第三方平台ID

    Log_AccountCreate()
    {
        m_LogType = ELT_ACCOUNT_CREATE;
    }

    bool GetLogSql(char* pBuff)
    {
        snprintf(pBuff, 4096, "insert into account_create(accountid, channel, version, optime, ip, uuid, idfa, imei, imodel, openid) values(%lld, %u, %d, '%s', '%s','%s','%s','%s', '%s', '%s')",
                 m_uAccountID, m_nChannel, m_dwVersion, CommonFunc::TimeToString(m_uOpTime).c_str(), CommonSocket::IpAddrIntToStr(m_dwIpAddr).c_str(), m_szUuid, m_szIdfa, m_szImei, m_szModel, m_szOpenID);
        return true;
    }
};

struct Log_AccountLogin : public Log_BaseData
{
    int32_t       m_dwVersion = 0;  //客户端版本号
    int32_t       m_dwIpAddr = 0;   //登录IP
    char        m_szImei[64] = { 0 }; //手机的MEI
    char        m_szModel[64] = { 0 };//手机的机型
    char        m_szUuid[64] = { 0 }; //手机的uuid;
    char        m_szOpenID[64] = { 0 };//第三方平台ID
    Log_AccountLogin()
    {
        m_LogType = ELT_ACCOUNT_LOGIN;
    }

    bool GetLogSql(char* pBuff)
    {

        snprintf(pBuff, 4096, "insert into account_login(accountid, channel, version, optime, ip, uuid, idfa, imei, imodel, openid) values(%lld, %u, %d, '%s', '%s','%s', '%s','%s', '%s', '%s')",
                 m_uAccountID, m_nChannel, m_dwVersion, CommonFunc::TimeToString(m_uOpTime).c_str(), CommonSocket::IpAddrIntToStr(m_dwIpAddr).c_str(), m_szUuid, m_szIdfa, m_szImei, m_szModel, m_szOpenID);
        return true;
    }
};

struct Log_RoleCreate : public Log_BaseData
{
    Log_RoleCreate()
    {
        m_LogType = ELT_ROLE_CREATE;
    }

    bool GetLogSql(char* pBuff)
    {
        snprintf(pBuff, 4096, "insert into role_create(roleid, accountid, areaid, channel, optime, rolename, idfa) values(%lld, %lld, %d, %d, '%s','%s', '%s')",
                 m_uRoleID, m_uAccountID, m_nAreaID, m_nChannel, CommonFunc::TimeToString(m_uOpTime).c_str(), m_szRoleName, m_szIdfa);
        return true;
    }
};

struct Log_RoleLogin : public Log_BaseData
{
    int32_t  m_nIpAddr = 0;   //登录IP
    Log_RoleLogin()
    {
        m_LogType = ELT_ROLE_LOGIN;
    }

    bool GetLogSql(char* pBuff)
    {
        snprintf(pBuff, 2048, "insert into role_login(roleid, accountid, areaid, channel, level, viplevel, optime, rolename, idfa) values(%lld, %lld, %d, %d, %d, %d, '%s', '%s', '%s')",
                 m_uRoleID, m_uAccountID, m_nAreaID, m_nChannel, m_nLevel, m_nVipLevel, CommonFunc::TimeToString(m_uOpTime).c_str(), m_szRoleName, m_szIdfa);
        return true;
    }
};

struct Log_RoleLogout : public Log_BaseData
{
    int64_t m_uLastTime = 0;
    Log_RoleLogout()
    {
        m_LogType = ELT_ROLE_LOGOUT;
    }

    bool GetLogSql(char* pBuff)
    {
        snprintf(pBuff, 2048, "insert into role_logout(roleid, accountid, areaid, channel, level, viplevel, optime, rolename, idfa) values(%lld, %lld, %d, %d, %d, %d, '%s', '%s', '%s')",
                 m_uRoleID, m_uAccountID, m_nAreaID, m_nChannel, m_nLevel, m_nVipLevel, CommonFunc::TimeToString(m_uOpTime).c_str(), m_szRoleName, m_szIdfa);
        return true;
    }
};

//经验获取
struct Log_RoleExp : public Log_BaseData
{
    int64_t m_uPreValue;
    int64_t m_uAfterValue;

    Log_RoleExp()
    {
        m_LogType = ELT_ROLE_EXP;
    }

    bool GetLogSql(char* pBuff)
    {
        return true;
    }
};

//钻石获取(充值币)
struct Log_RoleDiamond : public Log_BaseData
{
    int64_t m_uPreValue;
    int64_t m_uAfterValue;

    Log_RoleDiamond()
    {
        m_LogType = ELT_ROLE_DIAMOND;
    }

    bool GetLogSql(char* pBuff)
    {
        return true;
    }
};

//金币获取(游戏币)
struct Log_RoleGold : public Log_BaseData
{
    int64_t m_uPreValue;
    int64_t m_uAfterValue;

    Log_RoleGold()
    {
        m_LogType = ELT_ROLE_GOLD;
    }

    bool GetLogSql(char* pBuff)
    {
        return true;
    }
};

//聊天日志
struct Log_RoleChat : public Log_BaseData
{
    char   m_szText[256] = { 0 };
    char   m_szTargetName[64] = { 0 };
    int64_t m_uTargetID = 0;
    uint32_t m_nTargetVip = 0;
    int32_t  m_nChatChl = 0;

    Log_RoleChat()
    {
        m_LogType = ELT_ROLE_CHAT;
    }

    bool GetLogSql(char* pBuff)
    {
        CommonConvert::EscapeString(m_szText, 256);

        snprintf(pBuff, 2048, "insert into role_chat(roleid,rolename, areaid, channel, optime, level, viplevel, chatchl, text, targetid, targetvip, targetname) values(%lld, '%s', %d ,%d, '%s', %d, %d,%d, '%s', %lld, %ld, '%s')",
                 m_uRoleID, m_szRoleName, m_nAreaID, m_nChannel, CommonFunc::TimeToString(m_uOpTime).c_str(), m_nLevel, m_nVipLevel, m_nChatChl, m_szText, m_uTargetID, m_nTargetVip, m_szTargetName);
        return true;
    }
};

//等级变化日志
struct Log_RoleLevel : public Log_BaseData
{
    uint32_t m_uPreValue = 0;
    Log_RoleLevel()
    {
        m_LogType = ELT_ROLE_LEVEL;
    }

    bool GetLogSql(char* pBuff)
    {
        return true;
    }
};