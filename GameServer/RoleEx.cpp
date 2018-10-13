#include "StdAfx.h"
#include "RoleEx.h"
#include "Role.h"
#include "RoleManager.h"
#include "FishServer.h"
#include "FishLogic\NetCmd.h"
extern void SendLogDB(NetCmd* pCmd);
CRoleEx::CRoleEx()
{
	m_IsNeedSave = false;
	m_LogonTimeByDay = 0;
	m_IsRobot = false;//是否为机器人
	m_IsAfk = false;//是否离线
	m_IsExit = false;
	m_IsOnline = false;//是否在线
}
CRoleEx::~CRoleEx()
{
	m_IsAfk = false;
	m_IsExit = false;
	m_IsOnline = false;
}
bool CRoleEx::OnInit(tagRoleInfo* pUserInfo, tagRoleServerInfo* pRoleServerInfo, RoleManager* pManager,
	DWORD dwSocketID, time_t pTime, bool IsRobot,const tagRoleGameData& gameData)//玩家登陆成功的时候 dwSocketID 对应的Gate的ID
{
	if (!pUserInfo || !pRoleServerInfo)
	{
		ASSERT(false);
		return false;
	}
	ServerClientData * pClient = g_FishServer.GetUserClientDataByIndex(dwSocketID);
	if (dwSocketID != 0 && !pClient)
	{
		ASSERT(false);
		return false;
	}
	m_IsRobot = IsRobot;
	m_dwGameSocketID = dwSocketID;
	m_RoleInfo = *pUserInfo;
	m_RoleServerInfo = *pRoleServerInfo;

	m_RoleManager = pManager;
	m_LastOnLineTime = pTime;
	m_LogonTime = time(NULL);
	m_LogonTimeByDay = m_LogonTime;
	if (!IsOnceDayOnline())
	{
		m_IsNeedSave = true;

	}
	bool ret = m_RoleGameData.OnInit(this, gameData)
		&& m_RoleVip.OnInit(this)
		&& m_RoleRate.OnInit(this)
		&& m_RoleLauncherManager.OnInit(this);
	if (ret)
	{
		if (m_IsRobot)
		{
			g_FishServer.GetRobotManager().OnAddRobot(GetUserID(), this);//如果当前玩家为机器人 加入到机器人的集合里面去
		}
	}
	return ret;
}
void CRoleEx::ChangeRoleSocketID(DWORD SocketID)
{
	if (!m_RoleManager)
	{
		ASSERT(false);
		return;
	}
	if (m_dwGameSocketID == 0)//机器人不允许修改
	{
		ASSERT(false);
		return;
	}
	m_RoleManager->ChangeRoleSocket(this, SocketID);
	m_dwGameSocketID = SocketID;
}

void CRoleEx::SendDataToClient(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (m_IsRobot)
		return;
	if (m_IsAfk)//玩家下线了 无须发送命令到客户端去
		return;
	//将玩家的命令发送到客户端去 并且将数据进行处理
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(m_dwGameSocketID);
	if (!pClient)
		return;
	g_FishServer.SendNewCmdToClient(pClient, pCmd);
}
void CRoleEx::SendUserLeaveToCenter()
{
	//玩家离开中央服务器
	//g_FishServer.DelRoleOnlineInfo(m_RoleInfo.dwUserID);
	CL_UserLeave msg;
	SetMsgInfo(msg,GetMsgType(Main_Center, CL_Sub_UserLeave), sizeof(CL_UserLeave));
	msg.dwUserID = m_RoleInfo.dwUserID;
	SendDataToCenter(&msg);
}
void CRoleEx::SendDataToCenter(NetCmd* pCmd)
{
	g_FishServer.SendNetCmdToCenter(pCmd);
}
bool CRoleEx::ChangeRoleGlobe(int64 AddGlobe, const BYTE TableTypeID, const USHORT FishType)
{
	if (TableTypeID >= 4)
		return false;
	if (AddGlobe == 0)
		return true;
	if (!CheckChangeDWORDValue(m_RoleInfo.money1 + m_RoleInfo.money2, AddGlobe))
		return false;

	if (AddGlobe > 0 && m_RoleInfo.money1 + m_RoleInfo.money2 + AddGlobe >= g_FishServer.GetFishConfig().GetSystemConfig().MaxGobelSum)//金币到达上限了 无法添加金币
		return false;
	//记录输赢的金币到游戏数据
	m_RoleGameData.GetGameData().turn_LoseWinGold[TableTypeID] += AddGlobe;
	//记录每分钟输赢金币，统计输赢次数
	m_RoleGameData.ChangeGameGold(AddGlobe);
	//处理击杀鱼事件
	m_RoleGameData.OnPlayerCatchFish(FishType);
	//优先消耗游戏奖励的金币，再消耗玩家充值的
	if (AddGlobe > 0)
	{
		m_RoleInfo.money2 += AddGlobe;
	}
	else
	{
		if (m_RoleInfo.money2 - abs(AddGlobe) >= 0)
		{
			m_RoleInfo.money2 += AddGlobe;
		}
		else
		{
			if (m_RoleInfo.money1 + m_RoleInfo.money2 - abs(AddGlobe) >= 0)
			{
				m_RoleInfo.money1 += (m_RoleInfo.money2 + AddGlobe);
				m_RoleInfo.money2 = 0;
			}
			else
			{
				m_RoleInfo.money1 = 0;
				m_RoleInfo.money2 = 0;
			}
		}
	}
	//Log("玩家剩余金币：money1=%lld, monney2=%lld", m_RoleInfo.money1,m_RoleInfo.money2);
	return true;
}
void CRoleEx::SendDataToTable(NetCmd* pCmd)
{
	if (!g_FishServer.GetTableManager())
	{
		free(pCmd);
		ASSERT(false);
		return;
	}
	g_FishServer.GetTableManager()->SendDataToTable(m_RoleInfo.dwUserID, pCmd);
}
bool CRoleEx::ChangeRoleIsOnline(bool States)
{
	//告诉玩家是否在线的状态 玩家进入AFK 状态 无须处理 直接将数据发送到中央服务器 与 数据库去
	if (m_IsOnline == States)
		return true;
	m_IsOnline = States;

	DBR_Cmd_RoleOnline msg;
	SetMsgInfo(msg, DBR_SetOnline, sizeof(DBR_Cmd_RoleOnline));
	msg.dwUserID = m_RoleInfo.dwUserID;
	msg.IsOnline = States;
	g_FishServer.SendNetCmdToSaveDB(&msg);//需要快速保存的

	//中央服务器
	CC_Cmd_ChangeRoleIsOnline msgCenter;	
	SetMsgInfo(msgCenter, GetMsgType(Main_Role, CC_ChangeRoleIsOnline), sizeof(CC_Cmd_ChangeRoleIsOnline));
	msgCenter.dwUserID = m_RoleInfo.dwUserID;
	msgCenter.IsOnline = States;
	SendDataToCenter(&msgCenter);

	//发送命令到迷你服务器去 玩家下线了
	if (!States)
	{
		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = GetUserID();
		msgDB.CurrceySum = GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = GetRoleInfo().money1 + GetRoleInfo().money2;
		msgDB.MedalSum = GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = false;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = 250;
		msgDB.TableMonthID = 0;
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
	}
	return true;
}
bool CRoleEx::ChangeRoleTotalFishGlobelSum(int64 AddSum)
{
	if (AddSum == 0)
		return true;
	m_RoleServerInfo.TotalFishGlobelSum += AddSum;
	m_IsNeedSave = true;
	return true;
}
int64 CRoleEx::GetScore()
{
	return m_RoleInfo.money1 + m_RoleInfo.money2;
}

bool CRoleEx::IsOnceDayOnline()
{
	if (g_FishServer.GetFishConfig().GetFishUpdateConfig().IsChangeUpdate(m_LastOnLineTime, m_LogonTime))
		return false;
	else
		return true;
}
bool CRoleEx::IsOnceMonthOnline()
{
	time_t DestOnLineTime = m_LastOnLineTime - g_FishServer.GetFishConfig().GetWriteSec();
	if (DestOnLineTime < 0)
		DestOnLineTime = 0;
	time_t DestLogonTime = m_LogonTime - g_FishServer.GetFishConfig().GetWriteSec();
	if (DestLogonTime < 0)
		DestLogonTime = 0;
	tm pLogonTime;
	errno_t Error = localtime_s(&pLogonTime, &DestOnLineTime);
	if (Error != 0)
	{
		ASSERT(false);
		return false;
	}

	tm pNowTime;
	Error = localtime_s(&pNowTime, &DestLogonTime);
	if (Error != 0)
	{
		ASSERT(false);
		return false;
	}
	return pNowTime.tm_mon == pLogonTime.tm_mon;
}
void CRoleEx::OnSaveInfoToDB()
{
	SaveAllRoleInfo(false);//平时的普通的保存 15分钟进行一次的
}
void CRoleEx::UpdateByMin(DWORD dwMin)
{
	//统计游戏每分钟输赢，记录输赢次数
	m_RoleGameData.UpdateMinWinLose();
	//每分钟同步数据到游戏大厅
	GL_SyncDataToHall msg;
	SetMsgInfo(msg, GetMsgType(Main_Hall, 255), sizeof(GL_SyncDataToHall));
	msg.money1 = m_RoleInfo.money1*1.0 / MONEY_RATIO;
	msg.money2 = m_RoleInfo.money2*1.0 / MONEY_RATIO;
	msg.subGameId = 80002;
	msg.uid = m_RoleInfo.Uid;
	SendDataToCenter(&msg);
	//保存玩家游戏记录
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_RoleInfo.dwUserID);
	if (pRole)
	{
		pRole->SaveBattleRecord(1, 0);
	}
}
void CRoleEx::SetRoleExLeaveServer()
{
	//1.玩家进入暂离状态
	SetAfkStates(true);
	//2.如果玩家在房间里面 将玩家大招的能量清空
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_RoleInfo.dwUserID);
	if (pRole)
	{
		pRole->SetRoleIsCanSendTableMsg(false);//不可以向客户端发送命令
	}
}

bool CRoleEx::SaveAllRoleInfo(bool IsExit)
{
	//每次保存的时候 保存下玩家的在线时间
	DBR_Cmd_SaveRoleOnlineMin msgDB;
	SetMsgInfo(msgDB, DBR_SaveRoleOnlineMin, sizeof(DBR_Cmd_SaveRoleOnlineMin));
	msgDB.dwUserID = m_RoleInfo.dwUserID;
	msgDB.OnLineMin = 0;//保存玩家的在线分钟
	g_FishServer.SendNetCmdToSaveDB(&msgDB);

	if (!m_IsNeedSave && !IsExit)//无须保存
	{
		SetIsExit(IsExit);
		return true;
	}
	//保存玩家的全部的数据
	SetIsExit(IsExit);
	return true;
}
void CRoleEx::SetAfkStates(bool States) 
{ 
	if (m_IsAfk == States)
		return;
	m_IsAfk = States; 
	ChangeRoleIsOnline(!States); 
}
void CRoleEx::SetIsExit(bool States)
{ 
	if (m_IsExit == States)
		return;
	m_IsExit = States; 
	ChangeRoleIsOnline(!States);
}