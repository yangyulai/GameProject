#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "LoginMsgHandler.h"

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

    BOOL        SendCmdToAccountConnection(INT32 nMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata);

    BOOL        ConnectToAccountSvr();
public:
    CLoginMsgHandler    m_LoginMsgHandler;
    INT32               m_nAccountConnID;

public:
    //*********************消息处理定义开始******************************
    //*********************消息处理定义结束******************************
};

#endif
