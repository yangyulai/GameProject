#pragma once
#include <cstdint>
struct NetPacket;
class CWatcherClient
{
private:
    CWatcherClient(void);
    virtual ~CWatcherClient(void);

public:
    static CWatcherClient* GetInstancePtr();

    bool        OnNewConnect(int32_t nConnID);

    bool        OnCloseConnect(int32_t nConnID);

    bool        OnSecondTimer();

    bool        DispatchPacket( NetPacket* pNetPacket);

    bool        IsRun();

    bool        RegExitSignal();

    bool        StopServer();

    bool                    m_bRun;
public:
    //*********************消息处理定义开始******************************
    //*********************消息处理定义结束******************************
};
