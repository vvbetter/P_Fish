#include "Stdafx.h"
#include "RoleGameData.h"
#include "RoleEx.h"
#include "FishServer.h"
RoleGameData::RoleGameData()
{
	m_lastMinWinGold = 0;
	m_lastMinLoseGold = 0;
	m_loseNum = 0;
	m_winNum = 0;
	m_TotalLoseGold = 0;
	m_TotalWinGold = 0;
}
RoleGameData::~RoleGameData()
{
	//SaveRoleGameData();
}
bool RoleGameData::OnInit(CRoleEx* pRole, const tagRoleGameData& gameData)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	m_pRole = pRole;
	m_RoleGameData = gameData;
	return true;
}

const int64 RoleGameData::GetTurnLoseWinGoldByTable(const BYTE TableTypeID)
{
	if (TableTypeID >= 4) return 0;
	return m_RoleGameData.turn_LoseWinGold[TableTypeID];
}

void RoleGameData::ChangeGameGold(const INT64 gold)
{
	if (gold >= 0)
	{
		m_TotalWinGold += gold;
		m_lastMinWinGold += gold;
	}
	else
	{
		m_TotalLoseGold += gold;
		m_lastMinLoseGold += gold;
	}
}

void RoleGameData::UpdateMinWinLose()
{
	if (m_lastMinLoseGold != 0 || m_lastMinWinGold != 0)
	{
		m_lastMinWinGold > (-m_lastMinLoseGold) ? (++m_winNum) : (++m_loseNum);
	}
	m_lastMinLoseGold = 0;
	m_lastMinWinGold = 0;
}