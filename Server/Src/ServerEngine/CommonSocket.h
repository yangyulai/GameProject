#pragma once
#define SOCKET_ERROR        (-1)
#include <WinSock2.h>
#include <string>

namespace CommonSocket
{
//设置套接字为可重用状态
bool        SetSocketReuseable(SOCKET hSocket);

//设置套接字为非阻塞状态
bool        SetSocketBlock(SOCKET hSocket, bool bBlock);

bool        SetSocketNoDelay(SOCKET hSocket);

bool        SetSocketKeepAlive( SOCKET hSocket, int nKeepInterval, int nKeepCount, int nKeepIdle );

bool        InitNetwork();

bool        UninitNetwork();

SOCKET      CreateSocket( int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);

bool        BindSocket( SOCKET hSocket, const struct sockaddr* pAddr, int nNamelen);

bool        ListenSocket( SOCKET hSocket, int nBacklog);

bool        ConnectSocket(SOCKET hSocket, const char* pAddr, short sPort);

INT32       GetSocketLastError();

bool        IsSocketValid(SOCKET hSocket);

void        ShutdownSend(SOCKET hSocket);

void        ShutdownRecv(SOCKET hSocket);

void        _ShutdownSocket(SOCKET hSocket);

void        CloseSocket(SOCKET hSocket);

std::string GetLocalIP();

UINT32      IpAddrStrToInt(CHAR* pszIpAddr);

UINT32      IpAddrStrToInt(const CHAR* pszIpAddr);

std::string IpAddrIntToStr(UINT32 dwIpAddr);

bool        SetSocketBuffSize(SOCKET hSocket, INT32 nRecvSize, INT32 nSendSize);

bool        SetSocketTimeOut(SOCKET hSocket, INT32 nSendTime, INT32 nRecvTime);

std::string GetRemoteIP(SOCKET hSocket);

UINT32      HostToNet(INT32 nValue);

UINT32      NetToHost(INT32 nValue);

VOID        IgnoreSignal();

std::string HttpGet(std::string strHost, INT32 nPort, std::string strPath, std::string strContent);

#ifdef WIN32
bool        ConnectSocketEx(SOCKET hSocket, const char* pAddr, short sPort, LPOVERLAPPED lpOverlapped);

bool        AcceptSocketEx(SOCKET hListenSocket, SOCKET hAcceptSocket, CHAR* pBuff, LPOVERLAPPED lpOverlapped);

bool        GetSocketAddress(SOCKET hSocket, CHAR* pDataBuffer, sockaddr_in*& pAddrClient, sockaddr_in*& pAddrLocal);

bool        DisconnectEx(SOCKET hSocket, LPOVERLAPPED lpOverlapped, bool bReuse);
#endif
}