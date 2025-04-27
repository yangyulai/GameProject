﻿#pragma once

#include "DBInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "libmysql.lib")
#else
#pragma comment(lib, "libmysql.lib")
#endif

#define ERROR_SLEEP_TIME 3000 // 执行SQL语句时出错，再次尝试执行间隔时间（毫秒）

class CDBConnection  : public IDBInterface
{
public:
    CDBConnection( void );
    ~CDBConnection( void );

    bool    Init();

    bool    Uninit();

    bool    SetConnectParam(char const* szHost, char const* szUser, char const* szPwd, char const* szDb, int nPort, char const* szCharSet = "utf8mb4");

    bool    Connect(char const* szHost, char const* szUser, char const* szPwd, char const* szDb, int nPort, char const* szCharSet = "utf8mb4");

    void    Close(void);

    bool    Execute(CDBStoredProcedure* pDBStoredProcedure);

    bool    Query(CDBStoredProcedure* pDBStoredProcedure);

    bool    Execute(std::string sql);

    bool    Query(std::string sql);

    int     GetError(void) const;

    bool    Reconnect(void);

    bool    Ping();

private:
    MYSQL*       m_pMySql;
    std::string  m_strHost;
    std::string  m_strUser;
    std::string  m_strPwd;
    std::string  m_strDB;
    int          m_nPort;
    int          m_nErrno;
    std::string  m_strError;
    std::string  m_strCharSet;
};