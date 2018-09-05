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
	m_BossFishCount = 0;
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

void RoleGameData::OnPlayerCatchFish(const USHORT FishType)
{
	if (FishType == -1)
	{
		return;
	}
	tagClientUserData* pUdata = g_FishServer.GetHallDataCache(m_pRole->GetRoleInfo().Uid);
	if (pUdata != NULL)
	{
		if (25 == FishType)
		{
			OnCatchFihs_25(pUdata);
		}
		if (1 == FishType || 3 == FishType || 19 == FishType)
		{
			OnCatchFish_1_3_19(pUdata);
		}
	}
}

void RoleGameData::OnFishEvent(tagClientUserData* udata, INT eventType)
{
	for (int i = 0; i < udata->achSize; ++i)
	{
		if (eventType == udata->achDataMap[i].achtype 
			&& udata->achDataMap[i].qachfinishnum < udata->achDataMap[i].qachNum)
		{
			//完成度 +1
			udata->achDataMap[i].qachfinishnum += 1;
			//完成成就
			if (udata->achDataMap[i].qachfinishnum == udata->achDataMap[i].qachNum)
			{
				udata->achDataMap[i].finishtime = timeGetTime();
				//TODO 通知客服端
			}
		}
	}
}

void RoleGameData::OnCatchFihs_25(tagClientUserData* udata)
{
	OnFishEvent(udata, 61);//61成就ID就是击杀电鳗
}

void RoleGameData::OnCatchFish_1_3_19(tagClientUserData* udata)
{
	OnFishEvent(udata, 60); //60 击杀BOSS鱼（金色鱼）
	m_BossFishCount++;
}