﻿#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "DBMsgHandler.h"
#include "DBWriterManager.h"
class CConnection;

class LogService  : public IPacketDispatcher
{
private:
    LogService(void);
    virtual ~LogService(void);

public:
    static LogService* GetInstancePtr();

public:
    BOOL        Init();

    BOOL        UnInit();

    BOOL        Run();

    BOOL        OnNewConnect(INT32 nConnID);

    BOOL        OnCloseConnect(INT32 nConnID);

    BOOL        OnSecondTimer();

    BOOL        DispatchPacket( NetPacket* pNetPacket);

    BOOL        SetLogicConnID(INT32 nConnID);

    INT32       GetLogicConnID();

    BOOL        SetLogicProcessID(INT32 nProcesssID);

    INT32       GetLogicProcessID();

    BOOL        CheckLogicServer();

public:
    CDBMsgHandler       m_DBMsgHandler;

    CDBWriterManager    m_DBWriterManger;

    INT32               m_nLogicConnID;

    INT32               m_nLogicProcessID;
public:
    //*********************消息处理定义开始******************************
    //*********************消息处理定义结束******************************
};

#endif
