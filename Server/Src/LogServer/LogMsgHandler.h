#pragma once
#include <stdint.h>
struct NetPacket;

class CLogMsgHandler
{
public:
	CLogMsgHandler(const CLogMsgHandler& other) = delete;
	CLogMsgHandler(CLogMsgHandler&& other) noexcept = delete;
	CLogMsgHandler& operator=(const CLogMsgHandler& other) = delete;
	CLogMsgHandler& operator=(CLogMsgHandler&& other) noexcept = delete;

	CLogMsgHandler();

	~CLogMsgHandler();
	bool		Init(int32_t nReserved);
	bool		UnInit();
	bool		OnUpdate(uint64_t uTick);
	bool		DispatchPacket(NetPacket* pNetPacket);
	bool Test();
public:
	bool		OnMsgLogDataNtf(NetPacket* pNetPacket);
	uint32_t			m_nWriteCount;
	uint64_t          m_nLastWriteTime;
};