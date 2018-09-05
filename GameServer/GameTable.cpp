#include "StdAfx.h"
#include "GameTable.h"
#include "FishLogic\FishScene\FishResManager.h"
#include "RoleEx.h"
#include "FishServer.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


GameTable::GameTable()
{
	//m_TableConfig = NULL;
}
GameTable::~GameTable()
{
	OnDestroy();
}
bool GameTable::OnInit(WORD TableID, BYTE TableTypeID, BYTE MonthID)
{
	HashMap<BYTE, TableInfo>::iterator Iter = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (Iter == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		ASSERT(false);
		return false;
	}
	m_TableID = TableID;//设置好桌子的ID 
	m_TableTypeID = TableTypeID;
	m_RoleManager.OnInit(TableID, Iter->second.wMaxPlayerSum);//初始化玩家管理器
	m_fishdesk.Init(m_TableID, this, Iter->second.cMapName, &m_RoleManager);
	m_LastUpdateTime = timeGetTime();
	m_MonthID = MonthID;
	return true;
}
bool GameTable::IsFull()
{
	return m_RoleManager.IsFull();
}
bool GameTable::IsCanAddRobot()
{
	return g_FishServer.GetRobotManager().GameRobotIsCanJoinTable(this);

	//判断当前桌子是否可以加入机器人
	//1.如果座子的空位置只有一个了 机器人无法加入
	/*HashMap<BYTE, TableInfo>::iterator Iter = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(m_TableTypeID);
	if (Iter == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		ASSERT(false);
		return false;
	}
	if (m_RoleManager.GetRoleSum() >= (Iter->second.wMaxPlayerSum - 1))
		return false;
	return true;*/
}
WORD GameTable::GetTablePlayerSum()
{
	return ConvertIntToWORD(m_RoleManager.GetRoleSum());
}
CRole* GameTable::SearchUser(DWORD dwUserID)
{
	return m_RoleManager.GetRoleByUserID(dwUserID);
}
void GameTable::OnDestroy()
{
	m_TableTypeID = 0;
	m_fishdesk.Shutdown();
	m_RoleManager.Destroy();
}
bool GameTable::IsCanJoinTable(CRoleEx* pRoleEx, BYTE MonthID)
{
	//判断玩家是否可以进入当前类型的桌子
	if (!pRoleEx)
	{
		ASSERT(false);
		return false;
	}
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(pRoleEx->GetUserID());
	if (pRole)
	{
		ASSERT(false);
		return false;
	}
	//判断玩家是否可以进入
	if (!IsCanJoinRoom(pRoleEx))//判断玩家是否可以进入桌子 
	{
		return false;
	}
	if (m_RoleManager.IsFull())
	{
		return false;
	}
	return true;
}
bool GameTable::OnRoleJoinTable(CRoleEx* pRoleEx, BYTE MonthID, bool IsSendToClient)
{
	if (!pRoleEx)
	{
		ASSERT(false);
		return false;
	}
	HashMap<BYTE, TableInfo>::iterator Iter = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(m_TableTypeID);
	if (Iter == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		ASSERT(false);
		return false;
	}
	//判断玩家是否可以进入
	if (!IsCanJoinRoom(pRoleEx))//判断玩家是否可以进入桌子 
	{
		ASSERT(false);
		return false;
	}
	if (!m_RoleManager.OnInitRole(pRoleEx))
	{
		ASSERT(false);
		return false;
	}
	CRole* pRole = m_RoleManager.GetRoleByUserID(pRoleEx->GetUserID());
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	pRole->SetTableType(m_TableTypeID);
	pRole->SetRoleIsCanSendTableMsg(false);//设置玩家是否可以发送命令到客户端去。等待客户端准备好的命令在同步
	//发送玩家成功进入桌子的命令
	pRole->SetBulletRate(Iter->second.TableRate, Iter->second.MinRate, Iter->second.MaxRate);
	pRole->SetRoomLauncher();//设置玩家的炮台
	//返回进入房间成功
	LC_JoinTableResult msgJoin;
	SetMsgInfo(msgJoin, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
	msgJoin.RoomID = GetTableTypeID();
	msgJoin.Result = true;
	msgJoin.BackgroundImage = m_fishdesk.GetSceneBackground();
	if (pRoleEx->GetRoleLauncherManager().IsCanUserLauncherByID(pRole->GetLauncherType()))
		msgJoin.PlayerData.launcherType = pRole->GetLauncherType() | 128;
	else
		msgJoin.PlayerData.launcherType = pRole->GetLauncherType();
	msgJoin.PlayerData.seat = pRole->GetSeatID();
	msgJoin.PlayerData.rateIndex = pRole->GetRateIndex();
	msgJoin.PlayerData.energy = pRole->GetEnergy();
	//其他信息
	msgJoin.PlayerData.playerId = pRoleEx->GetRoleInfo().Uid;
	memset(msgJoin.PlayerData.playerName, 0, MAX_NICKNAME + 1);
	memcpy_s(msgJoin.PlayerData.playerName, MAX_NICKNAME , pRoleEx->GetRoleInfo().NickName, MAX_NICKNAME);
	//msgJoin.PlayerData.playerName = pRoleEx->GetRoleInfo().NickName;
	msgJoin.PlayerData.sex = pRoleEx->GetRoleInfo().bGender;
	msgJoin.PlayerData.headicon = pRoleEx->GetRoleInfo().dwHeadIcon;
	//msgJoin.PlayerData.icon = pRoleEx->GetRoleInfo().icon;
	memcpy_s(msgJoin.PlayerData.icon, ICON_LENGTH, pRoleEx->GetRoleInfo().icon, ICON_LENGTH);
	msgJoin.PlayerData.vipLevel = pRoleEx->GetRoleInfo().VipLevel;
	msgJoin.PlayerData.goldNum = pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2;
	pRoleEx->SendDataToClient(&msgJoin);
	//保存玩家游戏开始记录
	DBR_Cmd_SaveRecord msg;
	SetMsgInfo(msg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
	msg.model = 0;
	msg.uid = pRoleEx->GetRoleInfo().Uid;
	msg.table_id = pRole->GetTableType();
	msg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
	msg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
	msg.leave_code = 0;
	g_FishServer.SendNetCmdToDB(&msg);
	return true;
}
void GameTable::DelaySyncDataToClient(CRoleEx* pRoleEx)
{
	if (m_RoleManager.GetRoleSum() == 1)
	{
		OnGameStart();
	}
	else
	{
		//人数大于1  我们想要进行玩家进入的同步函数处理 
		SendRoleJoinInfo(pRoleEx->GetUserID());
		//同步游戏内部的鱼
		byte SeatID = 0;
		m_fishdesk.PlayerJoin(pRoleEx->GetUserID(), SeatID);
	}
	return;
}
void GameTable::SendTableRoleInfoToClient(DWORD dwUserID)
{
	CRole* pUser = m_RoleManager.GetRoleByUserID(dwUserID);
	if (!pUser || !pUser->GetRoleExInfo())
	{
		ASSERT(false);
		return;
	}
	//直接的一些炮台的数据 发送到客户端去
	LC_JoinTableResult msgJoin;
	SetMsgInfo(msgJoin, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
	msgJoin.RoomID = GetTableTypeID();
	msgJoin.Result = true;
	msgJoin.BackgroundImage = m_fishdesk.GetSceneBackground();

	if (pUser->GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(pUser->GetLauncherType()))
		msgJoin.PlayerData.launcherType = pUser->GetLauncherType() | 128;
	else
		msgJoin.PlayerData.launcherType = pUser->GetLauncherType();

	msgJoin.PlayerData.seat = pUser->GetSeatID();
	msgJoin.PlayerData.rateIndex = pUser->GetRateIndex();
	msgJoin.PlayerData.energy = pUser->GetEnergy();
	pUser->GetRoleExInfo()->SendDataToClient(&msgJoin);
	//其他信息
	msgJoin.PlayerData.playerId = pUser->GetRoleExInfo()->GetRoleInfo().Uid;
	memcpy_s(msgJoin.PlayerData.playerName, MAX_NICKNAME + 1, pUser->GetNickName(), MAX_NICKNAME + 1);
	msgJoin.PlayerData.sex = pUser->GetRoleExInfo()->GetRoleInfo().bGender;
	msgJoin.PlayerData.headicon = pUser->GetRoleExInfo()->GetRoleInfo().dwHeadIcon;
	memcpy_s(msgJoin.PlayerData.icon, ICON_LENGTH, pUser->GetRoleExInfo()->GetRoleInfo().icon, ICON_LENGTH);
	//msgJoin.PlayerData.icon = pUser->GetRoleExInfo()->GetRoleInfo().dwFaceID;
	msgJoin.PlayerData.vipLevel = pUser->GetRoleExInfo()->GetRoleInfo().VipLevel;
	msgJoin.PlayerData.goldNum = pUser->GetRoleExInfo()->GetRoleInfo().money1 + pUser->GetRoleExInfo()->GetRoleInfo().money2;

	time_t pNow = time(null);

	for (BYTE i = 0; i < m_RoleManager.GetMaxPlayerSum(); ++i)
	{
		CRole* pOtherUser = m_RoleManager.GetRoleBySeatID(i);
		if (!pOtherUser || !pOtherUser->IsActionUser())
			continue;
		if (pOtherUser->GetID() != pUser->GetID())
		{
			LC_Cmd_ResetOtherUserInfo msg;
			SetMsgInfo(msg, GetMsgType(Main_Table, LC_ResetOtherUserInfo), sizeof(LC_Cmd_ResetOtherUserInfo));
			msg.UserInfo.dwUserID = pOtherUser->GetID();
			msg.UserInfo.dwFaceID = pOtherUser->GetFaceID();
			msg.UserInfo.bGender = pOtherUser->GetRoleExInfo()->GetRoleInfo().bGender;
			//msg.UserInfo.dwExp = pOtherUser->GetRoleExInfo()->GetRoleInfo().dwExp;
			msg.UserInfo.dwGlobeNum = pOtherUser->GetRoleExInfo()->GetRoleInfo().money1 + pOtherUser->GetRoleExInfo()->GetRoleInfo().money2;
			msg.UserInfo.dwAchievementPoint = pOtherUser->GetRoleExInfo()->GetRoleInfo().dwAchievementPoint;
			msg.UserInfo.TitleID = pOtherUser->GetRoleExInfo()->GetRoleInfo().TitleID;
			msg.UserInfo.GameID = pOtherUser->GetRoleExInfo()->GetRoleInfo().GameID;

			if (pOtherUser->GetRoleExInfo()->GetRoleInfo().IsShowIPAddress)
			{
				//TCHARCopy(msg.UserInfo.IPAddress, CountArray(msg.UserInfo.IPAddress), pOtherUser->GetRoleExInfo()->GetRoleInfo().IPAddress, _tcslen(pOtherUser->GetRoleExInfo()->GetRoleInfo().IPAddress));
				g_FishServer.GetAddressByIP(pOtherUser->GetRoleExInfo()->GetRoleInfo().ClientIP, msg.UserInfo.IPAddress, CountArray(msg.UserInfo.IPAddress));
			}
			else
			{
				TCHARCopy(msg.UserInfo.IPAddress, CountArray(msg.UserInfo.IPAddress), Defalue_Ip_Address, _tcslen(Defalue_Ip_Address));
			}

			for (int j = 0; j < MAX_CHARM_ITEMSUM; ++j)
				msg.UserInfo.CharmArray[j] = pOtherUser->GetRoleExInfo()->GetRoleInfo().CharmArray[j];
			//TCHARCopy(msg.UserInfo.NickName, CountArray(msg.UserInfo.NickName), pOtherUser->GetNickName(), _tcslen(pOtherUser->GetNickName()));
			memcpy_s(msg.UserInfo.NickName, MAX_NICKNAME + 1, pOtherUser->GetNickName(), MAX_NICKNAME + 1);
			//msg.UserInfo.NickName = pOtherUser->GetNickName();
			msg.UserInfo.VipLevel = pOtherUser->GetRoleExInfo()->GetRoleInfo().VipLevel;
			msg.UserInfo.IsInMonthCard = (pOtherUser->GetRoleExInfo()->GetRoleInfo().MonthCardID != 0 && pOtherUser->GetRoleExInfo()->GetRoleInfo().MonthCardEndTime >= pNow);
			msg.UserInfo.SeatID = pOtherUser->GetSeatID();
			msg.UserInfo.wLevel = pOtherUser->GetRoleExInfo()->GetRoleInfo().wLevel;

			pUser->GetRoleExInfo()->SendDataToClient(&msg);
		}
	}
	m_fishdesk.AsyncPlayerJoin(pUser, true);//玩家加入
}
void GameTable::SendRoleJoinInfo(DWORD dwUserID)
{
	
	//将新加入的玩家数据进行同步处理
	CRole* pUser = m_RoleManager.GetRoleByUserID(dwUserID);
	if (!pUser)
	{
		ASSERT(false);
		return;
	}

	time_t pNow = time(null);

	LC_OtherUserInfo msgUser;
	SetMsgInfo(msgUser,GetMsgType(Main_Table, LC_Sub_OtherUserInfo), sizeof(LC_OtherUserInfo));
	msgUser.UserInfo.playerId = pUser->GetRoleExInfo()->GetRoleInfo().Uid;
	memset(msgUser.UserInfo.playerName, 0, MAX_NICKNAME + 1);
	memcpy_s(msgUser.UserInfo.playerName, MAX_NICKNAME, pUser->GetNickName(), MAX_NICKNAME);
	msgUser.UserInfo.sex = pUser->GetRoleExInfo()->GetRoleInfo().bGender;
	msgUser.UserInfo.headicon = pUser->GetRoleExInfo()->GetRoleInfo().dwHeadIcon;
	memcpy_s(msgUser.UserInfo.icon, ICON_LENGTH, pUser->GetRoleExInfo()->GetRoleInfo().icon, ICON_LENGTH);
	//msgUser.UserInfo.icon = pUser->GetFaceID();
	msgUser.UserInfo.vipLevel = pUser->GetRoleExInfo()->GetRoleInfo().VipLevel;
	msgUser.UserInfo.goldNum = pUser->GetRoleExInfo()->GetRoleInfo().money1 + pUser->GetRoleExInfo()->GetRoleInfo().money2;
	msgUser.UserInfo.seat = pUser->GetSeatID();
	msgUser.UserInfo.launcherType = pUser->GetLauncherType();
	msgUser.UserInfo.rateIndex = pUser->GetRateIndex();
	msgUser.UserInfo.energy = pUser->GetEnergy();
	
	for (BYTE i = 0; i < m_RoleManager.GetMaxPlayerSum(); ++i)
	{
		CRole* pOtherUser = m_RoleManager.GetRoleBySeatID(i);
		if (!pOtherUser || !pOtherUser->IsActionUser())
			continue;
		if (pOtherUser->GetID() != pUser->GetID())
		{
			LC_OtherUserInfo msg;
			SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_OtherUserInfo), sizeof(LC_OtherUserInfo));
			msg.UserInfo.playerId = pOtherUser->GetRoleExInfo()->GetRoleInfo().Uid;
			memcpy_s(msg.UserInfo.playerName, MAX_NICKNAME + 1, pOtherUser->GetNickName(), MAX_NICKNAME + 1);
			msg.UserInfo.sex = pOtherUser->GetRoleExInfo()->GetRoleInfo().bGender;
			msg.UserInfo.headicon = pOtherUser->GetRoleExInfo()->GetRoleInfo().dwHeadIcon;
			//msg.UserInfo.icon = pOtherUser->GetFaceID();
			memcpy_s(msg.UserInfo.icon, ICON_LENGTH, pOtherUser->GetRoleExInfo()->GetRoleInfo().icon, ICON_LENGTH);
			msg.UserInfo.vipLevel = pOtherUser->GetRoleExInfo()->GetRoleInfo().VipLevel;
			msg.UserInfo.goldNum = pOtherUser->GetRoleExInfo()->GetRoleInfo().money1 + pOtherUser->GetRoleExInfo()->GetRoleInfo().money2;
			msg.UserInfo.seat = pOtherUser->GetSeatID();
			msg.UserInfo.launcherType = pOtherUser->GetLauncherType();
			msg.UserInfo.rateIndex = pOtherUser->GetRateIndex();
			msg.UserInfo.energy = pOtherUser->GetEnergy();

			pUser->GetRoleExInfo()->SendDataToClient(&msg);
			pOtherUser->GetRoleExInfo()->SendDataToClient(&msgUser);
		}
	}
}
bool GameTable::OnRoleLeaveTable(DWORD dwUserID)
{
	//玩家离开桌子
	CRole* pRole = m_RoleManager.GetRoleByUserID(dwUserID);
	if (!pRole || !m_fishdesk.PlayerLeave(dwUserID))
	{
		ASSERT(false);
		return false;
	}
	if (m_RoleManager.GetRoleSum() == 0)
	{
		OnGameStop();
	}
	return true;
}
bool GameTable::OnHandleTableMsg(DWORD dwUserID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return false;
	}
	return m_fishdesk.HandleNetCmd(dwUserID, pCmd);
}
void GameTable::Update(bool bUpdateTime)
{
	DWORD dwCurrent = timeGetTime();
	m_fishdesk.Update((dwCurrent - m_LastUpdateTime)*0.001f);
	m_LastUpdateTime = dwCurrent;

	if (bUpdateTime)
	{
		for (BYTE i = 0; i < m_RoleManager.GetMaxPlayerSum(); i++)
		{
			CRole *pRole = m_RoleManager.GetRoleBySeatID(i);
			if (pRole&&pRole->IsActionUser())
			{
				pRole->AddGameTime(GAME_TIME_SPACE);
			}
		}
	}
	DWORD dwTimer = timeGetTime();
}
void GameTable::OnGameStart()
{
	m_fishdesk.OnGameStar();
	m_LastUpdateTime = timeGetTime();
	//CTraceService::TraceString(TEXT("一个桌子开始游戏"), TraceLevel_Normal);
}
void GameTable::OnGameStop()
{
	m_fishdesk.OnGameEnd();
	//CTraceService::TraceString(TEXT("一个桌子结束游戏"), TraceLevel_Normal);
}
void GameTable::Send(PlayerID RoleID, NetCmd*pCmd)
{
	CRole* pRole = m_RoleManager.GetRoleByUserID(RoleID);
	if (pRole != NULL && pRole->IsActionUser())
		pRole->GetRoleExInfo()->SendDataToClient(pCmd);
	/*else
		free(pCmd);*/
}
void GameTable::SendBySeat(byte seatID, NetCmd*pCmd)
{
	CRole* pRole = m_RoleManager.GetRoleBySeatID(seatID);
	if (pRole != NULL && pRole->IsActionUser())
		pRole->GetRoleExInfo()->SendDataToClient(pCmd);
	/*else
		free(pCmd);*/
}
bool GameTable::IsUseSeat()const
{
	return true;
}
void GameTable::SendDataToTable(DWORD dwUserID, NetCmd* pData)
{
	if (!pData)
	{
		ASSERT(false);
		return;
	}
	if (m_RoleManager.GetRoleSum() == 0)
	{
		return;
	}
	for (BYTE i = 0; i < m_RoleManager.GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = m_RoleManager.GetRoleBySeatID(i);
		if (pRole->IsActionUser() && pRole->GetID() != dwUserID)
		{
			pRole->GetRoleExInfo()->SendDataToClient(pData);
		}
	}
}
void GameTable::SendDataToTableAllUser(NetCmd* pData)
{
	if (!pData)
	{
		ASSERT(false);
		return;
	}
	if (m_RoleManager.GetRoleSum() == 0)
	{
		return;
	}
	for (BYTE i = 0; i < m_RoleManager.GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = m_RoleManager.GetRoleBySeatID(i);
		if (pRole->IsActionUser())
		{
			pRole->GetRoleExInfo()->SendDataToClient(pData);
		}
	}
}
bool GameTable::ChangeRoleRate(BYTE Rate)
{
	//当前倍率是否正确
	HashMap<BYTE, TableInfo>::iterator Iter = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(m_TableTypeID);
	if (Iter == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		ASSERT(false);
		return false;
	}
	if (Rate >= Iter->second.MinRate && Rate <= Iter->second.MaxRate)
		return true;
	else
		return false;
}
bool GameTable::IsCanJoinRoom(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	if (pRole->IsRobot())
	{
		return IsCanAddRobot();
	}
	HashMap<BYTE, TableInfo>::iterator Iter = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(m_TableTypeID);
	if (Iter == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		ASSERT(false);
		return false;
	}
	return true;
}

const int64 GameTable::GetTablePlayerAvgMoney()
{
	return m_RoleManager.GetPlayerAvgMoney();
}