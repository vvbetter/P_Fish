#include "Stdafx.h"
#include "RoleManager.h"
#include "FishServer.h"
#include "RoleEx.h"
RoleManager::RoleManager()
{
	m_ResetGameData[0] = false;
	m_ResetGameData[1] = false;
	m_ResetGameData[2] = false;
	m_ResetGameData[3] = false;
}
RoleManager::~RoleManager()
{
	Destroy();
}
void RoleManager::Destroy()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		Iter->second->SaveAllRoleInfo(false);
		Iter->second->SetIsExit(true);//设置玩家下线
		SAFE_DELETE(Iter->second);
	}
	m_RoleMap.clear();
	m_RoleSocketMap.clear();
}
bool RoleManager::OnInit()
{
	m_RoleGameDataCache.clear();
	return true;
}
CRoleEx* RoleManager::QueryUser(DWORD dwUserID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
		return Iter->second;
	else
		return NULL;
}

CRoleEx* RoleManager::QuertUserBySocketID(DWORD dwSocketID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleSocketMap.find(dwSocketID);
	if (Iter != m_RoleSocketMap.end())
		return Iter->second;
	else
		return NULL;
}
CRoleEx* RoleManager::QuertUserByUid(int64 uid)
{
	for (auto it = m_RoleMap.begin(); it != m_RoleMap.end(); ++it)
	{
		if (it->second->GetRoleInfo().Uid == uid)
		{
			return it->second;
		}
	}
	return NULL;
}
void  RoleManager::OnDelUser(DWORD dwUserID, bool IsWriteSaveInfo, bool IsLeaveCenter)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
	{
		//当一个玩家掉线 或者 退出GameServer的时候 我们应该缓存玩家的RoleEx对象 
		//RoleEx对象 不在删除 直接设置RoleEx 玩家离线了 设置SocketID 为0  无客户端数据了 在比赛离开比赛 在桌子 离开桌子等 
		//并且需要和Logon进行通讯 当玩家进入到GameServer的时候 我们想要让Logon找到 玩家 在多个gameServer 上的分布表
		if (IsWriteSaveInfo)
		{
			if (!Iter->second->SaveAllRoleInfo(true))
			{
				ASSERT(false);
			}
		}
		//保存玩家信息到数据库
		DBR_Cmd_SaveRoleExInfo msg;
		SetMsgInfo(msg, DBR_SaveRoleExInfo, sizeof(DBR_Cmd_SaveRoleExInfo));
		memcpy_s(&msg.RoleInfo, sizeof(tagRoleInfo), &(Iter->second->GetRoleInfo()), sizeof(tagRoleInfo));
		g_FishServer.SendNetCmdToDB(&msg);
		//直接删除玩家 是否需要设置玩家下线呢?
		//被顶掉 和 创建玩家失败 都不需要设置玩家下线
		//玩家直接离线 
		//Iter->second->ChangeRoleIsOnline(false);
		if (IsLeaveCenter)
			Iter->second->SendUserLeaveToCenter();
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

		m_RoleSocketMap.erase(Iter->second->GetGameSocketID()/*GetGateSocketID*/);
		SAFE_DELETE(Iter->second);
		m_RoleMap.erase(Iter);
	}
}
void RoleManager::OnDelUserResult(DBO_Cmd_SaveRoleAllInfo* pResult)
{
	if (!pResult)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pResult->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		if (Iter->second->IsExit())//玩家在退出状态
		{
			Iter->second->SendUserLeaveToCenter();
			g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

			m_RoleSocketMap.erase(Iter->second->GetGameSocketID());
			SAFE_DELETE(Iter->second);
			m_RoleMap.erase(Iter);
		}
		else
		{
			return;
		}
	}
	else
	{
		ASSERT(false);
		return;
	}
}

void RoleManager::OnKickOneUser(CRoleEx* pRole)
{
	if (pRole)
	{
		//缓存游戏数据
		auto gdCacheIt = m_RoleGameDataCache.find(pRole->GetRoleInfo().Uid);
		if (gdCacheIt == m_RoleGameDataCache.end())
		{
			m_RoleGameDataCache.insert(make_pair(pRole->GetRoleInfo().Uid, pRole->GetRoleGameData().GetGameData()));
		}
		else
		{
			gdCacheIt->second = pRole->GetRoleGameData().GetGameData();
		}
		//同步数据到游戏大厅
		tagClientUserData* pUdata = g_FishServer.GetHallDataCache(pRole->GetRoleInfo().Uid);
		if (!pUdata) return;
		UINT32 msgSize = sizeof(GL_QuitSubGame) + pUdata->achSize * (sizeof(tagAchDataMap));
		GL_QuitSubGame* msg = (GL_QuitSubGame*)malloc(msgSize);
		SetMsgInfo((*msg), GetMsgType(Main_Hall, 70), msgSize);
		msg->uid = pRole->GetRoleInfo().Uid;
		msg->money1 = pRole->GetRoleInfo().money1*1.0 / MONEY_RATIO;
		msg->money2 = pRole->GetRoleInfo().money2*1.0 / MONEY_RATIO;
		msg->quitSubGame = false;
		msg->loseNum = pRole->GetRoleGameData().GetLoseNum();
		msg->winNum = pRole->GetRoleGameData().GetWinNum();
		msg->winMoney = pRole->GetRoleGameData().GetTotalWinGold()*1.0 / MONEY_RATIO;
		msg->loseMoney = pRole->GetRoleGameData().GetTotalLoseGold()*1.0 / MONEY_RATIO;
		msg->catchBossFishCount = pRole->GetRoleGameData().GetBossFishCount();
		msg->charmValue = pUdata->charmValue;
		msg->achSize = pUdata->achSize;
		memcpy_s(msg->achData, pUdata->achSize * sizeof(tagAchDataMap), pUdata->achDataMap, pUdata->achSize * sizeof(tagAchDataMap));
		g_FishServer.SendNetCmdToCenter(msg);
		free(msg);
		//清除缓存数据
		g_FishServer.OnRemoveHallCatchData(pRole->GetRoleInfo().Uid);
		//客户端让玩家返回游戏大厅
		GL_TickOut tickMsg;
		SetMsgInfo(tickMsg, 6032, sizeof(GL_TickOut));
		pRole->SendDataToClient(&tickMsg);
		//保存玩家游戏记录
		DBR_Cmd_SaveRecord recordMsg;
		SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
		recordMsg.model = 2;
		recordMsg.uid = pRole->GetRoleInfo().Uid;
		recordMsg.table_id = 0;
		recordMsg.enter_money = (pRole->GetRoleInfo().money1 + pRole->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_money = (pRole->GetRoleInfo().money1 + pRole->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_code = 2;
		g_FishServer.SendNetCmdToDB(&recordMsg);
		//删除游戏数据
		OnDelUser(pRole->GetUserID(), true, true);
	}
}

void RoleManager::OnKickAllUser()
{
	//直接踢掉所有的玩家 
	for (auto Iter = m_RoleMap.begin(); Iter != m_RoleMap.end();)
	{
		auto delIt = Iter;
		++Iter;
		if (!g_FishServer.GetFishConfig().CheckInWhiteList(delIt->second->GetRoleInfo().Uid))
		{
			OnKickOneUser(delIt->second);
		}
	}
}
bool RoleManager::CreateRole(tagRoleInfo* pUserInfo,tagRoleServerInfo* pUserServerInfo, DWORD dwSocketID, time_t pTime, bool LogobByGameServer, bool IsRobot)
{
	if (!pUserInfo || !pUserServerInfo)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pUserInfo->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		return true;
	}
	else
	{
		CRoleEx * pUser = new CRoleEx();
		if (!pUser)
		{
			ASSERT(false);
			return false;
		}
		//玩家对象初始化完毕后 我们存储起来
		m_RoleMap.insert(HashMap<DWORD, CRoleEx*>::value_type(pUserInfo->dwUserID, pUser));
		if (!IsRobot)//机器人无须添加到SocketMap里面
			m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(dwSocketID, pUser));
		//把gamedata加载到玩家数据里面去
		tagRoleGameData gameData;
		auto gdIt = m_RoleGameDataCache.find(pUserInfo->Uid);
		if (gdIt != m_RoleGameDataCache.end())
		{
			gameData = gdIt->second;
		}
		if (!pUser->OnInit(pUserInfo, pUserServerInfo, this, dwSocketID, pTime, IsRobot, gameData))
		{
			//初始化玩家失败
			OnDelUser(pUserInfo->dwUserID,false,false);
			return false;
		}
		//加载成功，删除缓存
		if (gdIt != m_RoleGameDataCache.end())
		{
			m_RoleGameDataCache.erase(gdIt);
		}
		return true;
	}
}
void RoleManager::OnUpdateByMin(bool IsHourChange, bool IsDayChange, bool IsMonthChange, bool IsYearChange)
{
	OnResetGameDataTurn();
	if (m_RoleMap.empty())
		return;
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	for (HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin(); Iter != m_RoleMap.end();)
	{
		if (Iter->second->IsRobot())
		{
			++Iter;
			continue;
		}
		Iter->second->UpdateByMin(pTime.wMinute);
		auto delit = Iter;
		++Iter;
		//判断玩家最后发送子弹 或者使用技能的时间 如果大于多少分钟 未操作 让玩家离开桌子
		CRole* pTableRole = g_FishServer.GetTableManager()->SearchUser(delit->second->GetUserID());
		if (pTableRole && pTableRole->IsNeedLeaveTable())
		{
			CRoleEx* pRole = delit->second;
			if (pRole)
			{
				OnKickOneUser(pRole);
			}
			
		}
	}
}
void RoleManager::ChangeRoleSocket(CRoleEx* pRole, DWORD SocketID)
{
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	m_RoleSocketMap.erase(pRole->GetGameSocketID());
	if (SocketID != 0)
		m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(SocketID, pRole));
}
void RoleManager::OnSaveInfoToDB()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->OnSaveInfoToDB();
	}
}
void RoleManager::SetResetGameDataTable(const BYTE tableTypeID)
{
	if (tableTypeID >= 4)
	{
		return;
	}
	m_ResetGameData[tableTypeID] = true;
};

void RoleManager::OnResetGameDataTurn()
{
	for (BYTE i = 0; i < 4; ++i)
	{
		if (m_ResetGameData[i] == true)
		{
			//重置玩家输赢，先重置线上玩家的，在重置离线玩家的
			for (auto it = m_RoleMap.begin(); it != m_RoleMap.end(); ++it)
			{
				it->second->GetRoleGameData().GetGameData().turn_LoseWinGold[i] = 0;
			}
			//重置离线玩家的数据
			for (auto it = m_RoleGameDataCache.begin(); it != m_RoleGameDataCache.end(); ++it)
			{
				it->second.turn_LoseWinGold[i] = 0;
				//所有桌子的数据都清除了，删除缓存
				if (it->second.turn_LoseWinGold[0] == 0
					&& it->second.turn_LoseWinGold[1] == 0
					&& it->second.turn_LoseWinGold[2] == 0
					&& it->second.turn_LoseWinGold[3] == 0)
				{
					it = m_RoleGameDataCache.erase(it);
				}
			}
			m_ResetGameData[i] = false;
		}
	}
}