﻿
#include "RankMananger.h"
#include "LogService.h"

CRankManager* CRankManager::GetInstancePtr()
{
	static CRankManager _StaticMgr;

	return &_StaticMgr;
}

CRankManager::CRankManager()
{
	m_LevelRanker.InitRanker(50, 200);

	m_FightRanker.InitRanker(50, 200);
}

CRankManager::~CRankManager()
{

}

BOOL CRankManager::LoadData(CppMySQL3DB& tDBConnection)
{

	return TRUE;
}


