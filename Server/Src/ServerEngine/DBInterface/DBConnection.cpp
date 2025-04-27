#include "CppMysql.h"
#include "DBConnection.h"
#include "DBStoredProc.h"
//#include <spdlog/spdlog.h>

CDBConnection::CDBConnection( void )
    : m_pMySql( NULL ),
      m_nPort( 3306 ),
      m_nErrno( 0 ),
      m_strError( "" )
{
}

CDBConnection::~CDBConnection( void )
{
    Close();
}

bool CDBConnection::Init()
{
    mysql_library_init( 0, NULL, NULL );
    return TRUE;
}

bool CDBConnection::Uninit()
{
    mysql_library_end();
    return TRUE;
}

// connect.
bool CDBConnection::Connect(char const* szHost, char const* szUser, char const* szPwd, char const* szDb, int nPort, char const* szCharSet)
{
    Close();

    m_pMySql = mysql_init( NULL );
    if (m_pMySql == NULL)
    {
        return FALSE;
    }

    if (NULL == mysql_real_connect(m_pMySql, szHost, szUser, szPwd, szDb, nPort, NULL, 0))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        //spdlog::error("CDBConnection::Connect Failed [mysql_real_connect], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        mysql_close(m_pMySql);
        m_pMySql = NULL;
        return FALSE;
    }

    if (0 != mysql_set_character_set(m_pMySql, szCharSet))
    {
        mysql_close(m_pMySql);
        m_pMySql = NULL;
        return FALSE;
    }

    m_strHost.assign(szHost);
    m_strUser.assign(szUser);
    m_strPwd.assign(szPwd);
    m_strDB.assign(szDb);
    m_nPort = nPort;
    m_strCharSet.assign(szCharSet);

    return FALSE;
}

bool CDBConnection::Reconnect( void )
{
    if ( NULL != m_pMySql && 0 == mysql_ping( m_pMySql ) )
    {
        return TRUE;
    }

    Close();
    m_pMySql = mysql_init(NULL);
    if (m_pMySql == NULL)
    {
        return FALSE;
    }


    if (NULL == mysql_real_connect(m_pMySql, m_strHost.c_str(), m_strUser.c_str(), m_strPwd.c_str(), m_strDB.c_str(), m_nPort, NULL, 0))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        //spdlog::error("CDBConnection::Connect Failed [mysql_real_connect], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        mysql_close(m_pMySql);
        m_pMySql = NULL;
        return FALSE;
    }

    if (0 != mysql_set_character_set(m_pMySql, m_strCharSet.c_str()))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        //spdlog::error("CDBConnection::Connect Failed [mysql_options], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        mysql_close(m_pMySql);
        m_pMySql = NULL;
        return FALSE;
    }


    return TRUE;
}

// close.
void CDBConnection::Close( void )
{
    if ( NULL != m_pMySql )
    {
        mysql_close( m_pMySql );

        m_pMySql = NULL;
    }

    return;
}

bool CDBConnection::Execute(CDBStoredProcedure* pDBStoredProcedure)
{
    assert((NULL != m_pMySql) && (pDBStoredProcedure != NULL) && (pDBStoredProcedure->m_pMybind != NULL));

    static CDBStoredProcedure* pLastProcedure = NULL;
    static MYSQL_STMT* pLastStmt = NULL;

    if (pLastProcedure != NULL && pLastStmt != NULL)
    {
        //如果是同一个存储需求，则直接用上一次的预处理
        if (pLastProcedure == pDBStoredProcedure)
        {
            if (0 == mysql_stmt_execute(pLastStmt))
            {
                return TRUE;
            }

            m_nErrno = mysql_errno(m_pMySql);
            m_strError = mysql_error(m_pMySql);

            //spdlog::error("WriteDB1 Failed, ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());

            if (pDBStoredProcedure->m_pMybind[0].buffer_type == MYSQL_TYPE_LONGLONG)
            {
               // spdlog::error("WriteDB1 Failed key:%lld, Sql:[{}]", *(UINT64*)pDBStoredProcedure->m_pMybind[0].buffer, pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
            }
            else if (pDBStoredProcedure->m_pMybind[0].buffer_type == MYSQL_TYPE_LONG)
            {
               // spdlog::error("WriteDB1 Failed key:%ld, Sql:[{}]", *(INT32 *)pDBStoredProcedure->m_pMybind[0].buffer, pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
            }
            else
            {
               // spdlog::error("WriteDB1 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
            }

            mysql_stmt_close(pLastStmt);
            pLastStmt = NULL;
            pLastProcedure = NULL;
            return FALSE;
        }
        else
        {
            if (pLastStmt != NULL)
            {
                mysql_stmt_close(pLastStmt);
                pLastStmt = NULL;
            }

            pLastProcedure = NULL;
        }
    }


    MYSQL_STMT* pMySqlStmt = mysql_stmt_init(m_pMySql);
    if (pMySqlStmt == NULL)
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        //spdlog::error("WriteDB2 Failed, ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        //spdlog::error("WriteDB2 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        return FALSE;
    }

    if (0 != mysql_stmt_prepare(pMySqlStmt, pDBStoredProcedure->m_strSql.c_str(), (unsigned long)pDBStoredProcedure->m_strSql.size()))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        mysql_stmt_close(pMySqlStmt);
        pMySqlStmt = NULL;
        //spdlog::error("WriteDB3 Failed, ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        //spdlog::error("WriteDB3 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        return FALSE;
    }

    my_bool _bl = 1;
    mysql_stmt_attr_set(pMySqlStmt, STMT_ATTR_UPDATE_MAX_LENGTH, &_bl);

    if (mysql_stmt_param_count(pMySqlStmt) != pDBStoredProcedure->m_nCount)
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
        mysql_stmt_close(pMySqlStmt);
        pMySqlStmt = NULL;
       // spdlog::error("WriteDB4 Failed, ParamCount not equal ProcedureCount!");
       // spdlog::error("WriteDB4 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        return FALSE;
    }

    if (0 != mysql_stmt_bind_param(pMySqlStmt, pDBStoredProcedure->m_pMybind))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);
       // spdlog::error("WriteDB5 Failed, ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
       // spdlog::error("WriteDB5 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        mysql_stmt_close(pMySqlStmt);
        pMySqlStmt = NULL;
        return FALSE;
    }

    if (0 != mysql_stmt_execute(pMySqlStmt))
    {
        m_nErrno = mysql_errno(m_pMySql);
        m_strError = mysql_error(m_pMySql);


       // spdlog::error("WriteDB6 Failed, ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());

        if (pDBStoredProcedure->m_pMybind[0].buffer_type == MYSQL_TYPE_LONGLONG)
        {
            //spdlog::error("WriteDB6 Failed key:%lld, Sql:[{}]", *(UINT64*)pDBStoredProcedure->m_pMybind[0].buffer, pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        }
        else if (pDBStoredProcedure->m_pMybind[0].buffer_type == MYSQL_TYPE_LONG)
        {
            //spdlog::error("WriteDB6 Failed key:%ld, Sql:[{}]", *(INT32 *)pDBStoredProcedure->m_pMybind[0].buffer, pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        }
        else
        {
           // spdlog::error("WriteDB6 Failed Sql:[{}]", pDBStoredProcedure->m_strSql.substr(0, 100).c_str());
        }

        mysql_stmt_close(pMySqlStmt);
        pMySqlStmt = NULL;
        pLastProcedure = NULL;
        pLastStmt = NULL;
        return FALSE;
    }

    pLastProcedure = pDBStoredProcedure;
    pLastStmt = pMySqlStmt;

    return TRUE;
}

// query.
bool CDBConnection::Query(CDBStoredProcedure* pDBStoredProcedure)
{
    assert(pDBStoredProcedure->m_pMybind != NULL);
    assert(NULL != m_pMySql && NULL != pDBStoredProcedure);

    MYSQL_STMT* pMySqlStmt = mysql_stmt_init( m_pMySql );
    if(NULL == pMySqlStmt)
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
       // spdlog::error("CDBConnection::Execute Failed [mysql_stmt_init], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        return FALSE;
    }

    if(0 != mysql_stmt_prepare(pMySqlStmt, pDBStoredProcedure->m_strSql.c_str(), (unsigned long)pDBStoredProcedure->m_strSql.size()))
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
        mysql_stmt_close( pMySqlStmt );
        pMySqlStmt = NULL;
        //spdlog::error("CDBConnection::Execute Failed [mysql_stmt_prepare], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        return FALSE;
    }

    my_bool _bl = 1;
    mysql_stmt_attr_set( pMySqlStmt, STMT_ATTR_UPDATE_MAX_LENGTH, &_bl );

    if ( 0 != mysql_stmt_bind_param( pMySqlStmt, pDBStoredProcedure->m_pMybind ) )
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
        mysql_stmt_close( pMySqlStmt );
        pMySqlStmt = NULL;
        //spdlog::error("CDBConnection::Execute Failed [mysql_stmt_bind_param], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        return FALSE;
    }

    if(pMySqlStmt == NULL)
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
        return FALSE;
    }

    if (0 != mysql_stmt_execute( pMySqlStmt ))
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
        mysql_stmt_close( pMySqlStmt );
        pMySqlStmt = NULL;
        //spdlog::error("CDBConnection::Execute Failed [mysql_stmt_execute], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        return FALSE;
    }

    // 检查是否有结果集
    MYSQL_RES* pMySqlResult = mysql_stmt_result_metadata( pMySqlStmt );
    if(pMySqlResult == NULL)
    {
        pMySqlResult  = NULL;
        mysql_stmt_close( pMySqlStmt );
        pMySqlStmt = NULL;
        return FALSE;
    }

    unsigned int server_status = m_pMySql->server_status;

    if(0 != mysql_stmt_store_result( pMySqlStmt ))
    {
        m_nErrno = mysql_errno( m_pMySql );
        m_strError = mysql_error( m_pMySql );
        mysql_stmt_close( pMySqlStmt );
        pMySqlStmt = NULL;
        //spdlog::error("CDBConnection::Execute Failed [mysql_stmt_store_result], ErrorNo:%d, ErrorMsg:{}", m_nErrno, m_strError.c_str());
        return FALSE;
    }

    if((int)mysql_stmt_num_rows(pMySqlStmt) <= 0)
    {
        return FALSE;
    }

    pDBStoredProcedure->m_DBRecordSet.InitRecordSet(pMySqlStmt, pMySqlResult);

    return TRUE;
}

bool CDBConnection::Query( std::string sql )
{
    return TRUE;
}

bool CDBConnection::Ping()
{
    if (m_pMySql == NULL)
    {
        return FALSE;
    }

    if (mysql_ping(m_pMySql) == 0)
    {
        return TRUE;
    }

    return FALSE;
}

int CDBConnection::GetError( void ) const
{
    return m_nErrno;
}

bool CDBConnection::Execute( std::string sql )
{
    int nRet = mysql_real_query(m_pMySql, sql.c_str(), static_cast<unsigned long>(sql.size()));
    if (nRet == 0)
    {
        return TRUE;
    }

    m_nErrno = mysql_errno(m_pMySql);
    m_strError = mysql_error(m_pMySql);

    return FALSE;
}

bool CDBConnection::SetConnectParam( char const* szHost, char const* szUser, char const* szPwd, char const* szDb, int nPort, char const* szCharSet)
{
    m_strHost.assign( szHost );

    m_strUser.assign( szUser );

    m_strPwd.assign( szPwd );

    m_strDB.assign( szDb );

    m_nPort = nPort;

    m_strCharSet.assign(szCharSet);

    return TRUE;
}
