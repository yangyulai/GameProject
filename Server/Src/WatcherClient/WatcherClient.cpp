
#include "Platform.h"
#include "WatcherClient.h"
#include "../Message/Msg_Game.pb.h"
#include "../Message/Msg_RetCode.pb.h"
#include "../Message/Msg_ID.pb.h"

#ifdef WIN32
static bool        ExitRoutine(int32_t nSignal)
{
    switch (nSignal)
    {
        case CTRL_C_EVENT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case CTRL_CLOSE_EVENT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case CTRL_LOGOFF_EVENT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case CTRL_SHUTDOWN_EVENT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        default:
            break;
    }

    return true;
}
#else

static VOID        ExitRoutine(INT32 nSignal)
{
    switch (nSignal)
    {
        case SIGINT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case SIGQUIT:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case SIGKILL:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        case SIGTERM:
        {
            CWatcherClient::GetInstancePtr()->StopServer();
        }
        break;
        default:
            break;
    }
}
#endif;

CWatcherClient::CWatcherClient(void)
{
    m_bRun              = true;
}

CWatcherClient::~CWatcherClient(void)
{
}

CWatcherClient* CWatcherClient::GetInstancePtr()
{
    static CWatcherClient _WatcherClient;

    return &_WatcherClient;
}

bool CWatcherClient::IsRun()
{
    return m_bRun;
}

bool CWatcherClient::RegExitSignal()
{
#ifdef _WIN32
    return SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitRoutine, true);
#else
    signal(SIGINT, ExitRoutine);
    signal(SIGQUIT, ExitRoutine);
    signal(SIGKILL, ExitRoutine);
    signal(SIGTERM, ExitRoutine);
    return true;
#endif
}

bool CWatcherClient::StopServer()
{
    m_bRun = false;

    return true;
}

bool CWatcherClient::OnNewConnect(int32_t nConnID)
{

    return true;
}

bool CWatcherClient::OnCloseConnect(int32_t nConnID)
{
    return true;
}

bool CWatcherClient::OnSecondTimer()
{
    return true;
}

bool CWatcherClient::DispatchPacket(NetPacket* pNetPacket)
{

    return false;
}



