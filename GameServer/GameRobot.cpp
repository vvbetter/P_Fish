#include "stdafx.h"
#include "GameRobot.h"
#include "FishServer.h"
#include "FishLogic\NetCmd.h"
GameRobotManager::GameRobotManager()
{
	m_Robots.clear();
	m_WriteList.clear();
	m_IsLoadFinish = false;
	m_GetIndex = 0;
}
GameRobotManager::~GameRobotManager()
{
	for (auto IterVec = m_Robots.begin() ; IterVec != m_Robots.end(); ++IterVec)
	{
		SAFE_DELETE(*IterVec);
	}
	m_Robots.clear();
	m_WriteList.clear();
}
void GameRobotManager::OnLoadAllGameRobot(DWORD BeginUserID, DWORD EndUserID)
{
	if (BeginUserID == 0 || EndUserID == 0)
		return;//无须机器人
	Log("加载机器人，beginID=%d,endID=%d", BeginUserID, EndUserID);
	//机器人的管理器 管理当前GameServer上全部的机器人 
	//初始化  需要知道机器人的容器 我们创建大量的机器人 进行处理 从数据库读取出来  
	DBR_Cmd_GetAccountInfoByUserID msg;
	memset(&msg, 0, sizeof(DBR_Cmd_GetAccountInfoByUserID));
	SetMsgInfo(msg, DBR_GetAccountInfoByUserID, sizeof(DBR_Cmd_GetAccountInfoByUserID));
	//初始化机器人信息
	msg.ClientID = 0;
	msg.LogonByGameServer = false;
	msg.udata.money1 = 0;
	msg.udata.money2 = 0;
	for (size_t i = BeginUserID; i <= EndUserID; ++i)
	{
		uint RobotName_index = RandUInt() % MAX_ROBOTNAME_COUNT;
		memset(msg.udata.name, 0, MAX_NICKNAME + 1);
		memcpy_s(msg.udata.name, MAX_NICKNAME, g_RobotName[RobotName_index].c_str(), MAX_NICKNAME);
		uint iconIndex = RandUInt() % 12 + 1;
		memset(msg.udata.icon, 0, ICON_LENGTH);
		sprintf_s(msg.udata.icon, "%s%d", "head_female", iconIndex);
		msg.udata.sex = RandUInt() % 2;
		msg.udata.vip = RandUInt() % 6;
		msg.udata.headIcon = 0;
		//我们让机器人上线
		msg.dwUserID = i;
		msg.Uid = i;
		g_FishServer.SendNetCmdToDB(&msg);//发送命令
	}
	m_RobotConfigSum = EndUserID - BeginUserID + 1;
}
void GameRobotManager::OnAddRobot(DWORD dwUserID, CRoleEx* pRoleEx)
{
	//机器人加载完毕
	if (!pRoleEx)
	{
		ASSERT(false);
		return;
	}
	GameRobot* pRobot = new GameRobot(this, pRoleEx);
	if (!pRobot)
	{
		ASSERT(false);
		return;
	}
	m_Robots.push_back(pRobot);
	if (m_Robots.size() == m_RobotConfigSum && !m_IsLoadFinish)
	{
		m_IsLoadFinish = true;
		Log("机器人加载完成");
	}
	return;
}
GameRobot* GameRobotManager::GetFreeRobot(DWORD RobotID, GameTable* pTable)
{
	//获取一个空闲的机器人
	if (m_Robots.empty())
		return NULL;
	GameRobot* pRobot = NULL;
	for (auto it = m_Robots.begin() + (m_GetIndex%m_RobotConfigSum);;)
	{
		if (it == m_Robots.end())
		{
			it = m_Robots.begin();
		}
		if (!(*it)->IsRobotUsed())
		{
			pRobot = *it;
			break;
		}
		++it;
		if (it == m_Robots.begin() + (m_GetIndex%m_RobotConfigSum))
		{
			break;
		}
	}
	if (!pRobot)
		return NULL;
	//初始化机器人的钱桌面玩家平均的0.5-1.5倍
	pRobot->GetRoleInfo()->GetRoleInfo().money1 =  pTable->GetTablePlayerAvgMoney()*((RandUInt() % 10) + 5) / 10;
	pRobot->GetRoleInfo()->GetRoleInfo().money2 = 0;
	pRobot->SetRobotUse(RobotID);//设置机器人已经使用了
	m_GetIndex++;
	return pRobot;
}
void GameRobotManager::ResetGameRobot(DWORD dwUserID)
{
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		if ((*it)->GetRobotUserID() == dwUserID)
		{
			(*it)->ResetRobot();
			break;
		}
	}
	return;
}

void GameRobotManager::ResetAllGameRobot()
{
	//先让桌子上面的机器人离开桌子,不直接删除，加入到删除队列中
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		g_FishServer.GetTableManager()->OnPlayerLeaveTable((*it)->GetRobotUserID());
		(*it)->ResetRobot();
	}
	m_WriteList.clear();
}
bool GameRobotManager::GameRobotIsCanJoinTable(GameTable* pTable)
{
	//判断机器人是否可以加入当前桌子
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	if (pTable->GetTableMonthID() == 0)
	{
		//修改规则 最后一个位置 机器人不可以进入
		if (pTable->GetRoleManager().GetRoleSum() >= 3)
			return false;
		return true;
	}
	else
	{
		//比赛房间 一个房间最多2个机器人
		BYTE RobotSum = 0;
		for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
		{
			CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
			if (!pRole || !pRole->IsActionUser())
				continue;
			if (pRole->GetRoleExInfo()->IsRobot())
				++RobotSum;
		}
		if (RobotSum >= 2)
			return false;
		else
			return true;
	}
}
void GameRobotManager::OnRoleCreateNormalRoom(GameTable* pTable)
{
	if (!pTable || pTable->GetTableMonthID() != 0)
		return;
	if (pTable->GetRoleManager().GetRoleSum() != 1)
		return;
	for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
			continue;
		if (pRole->GetRoleExInfo()->IsRobot())
			return;
	}

	//桌子ID 和 比赛ID 我们生成机器人的类型ID
	DWORD Key = (pTable->GetTableTypeID() << 16) + pTable->GetTableMonthID();
	multimap<DWORD, DWORD>::iterator Iter= g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.find(Key);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.end())
		return;
	
	DWORD RobotNum = (RandUInt() % 99999) % 3;
	for (DWORD i = 0; i < RobotNum; ++i)
	{
		GameRobot* pRobot = GetFreeRobot(Iter->second, pTable);//获取一个空闲的机器人
		if (!pRobot)
			return;
		CRoleEx* pRole = pRobot->GetRoleInfo();
		if (!pRole)
			return;
		if (!g_FishServer.GetTableManager()->OnPlayerJoinTable(pTable->GetTableID(), pRole))
		{
			ResetGameRobot(pRole->GetUserID());
		}
	}
}
void GameRobotManager::OnRoleLeaveNormalRoom(GameTable* pTable)
{
	//当普通玩家离开一个房间的时候 判断如果房间里没有普通玩家了 全部的机器人离开
	if (!pTable || pTable->GetTableMonthID() != 0)
		return;
	BYTE RoleSum = 0;
	for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
			continue;
		if (!pRole->GetRoleExInfo()->IsRobot())
			++RoleSum;
	}
	if (RoleSum == 0)
	{
		for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
		{
			CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
			if (!pRole || !pRole->IsActionUser())
				continue;
			if (pRole->GetRoleExInfo()->IsRobot())
			{
				//保存玩家游戏记录
				CRoleEx* pRoleEx = pRole->GetRoleExInfo();
				DBR_Cmd_SaveRecord recordMsg;
				SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
				recordMsg.model = 2;
				recordMsg.uid = pRoleEx->GetRoleInfo().Uid;
				recordMsg.table_id = 0;
				recordMsg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
				recordMsg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
				recordMsg.leave_code = 2;
				g_FishServer.SendNetCmdToDB(&recordMsg);
				//归还机器人
				DWORD dwUserID = pRole->GetID();
				g_FishServer.GetTableManager()->OnPlayerLeaveTable(dwUserID);
				ResetGameRobot(dwUserID);//归还机器人
			}
		}
	}
}
void GameRobotManager::OnRoleJoinNormalRoom(GameTable* pTable)
{
	//当普通玩家进入一个已经存在的房间的时候 如果玩家人数 小于等于2人 并且 全部为普通玩家的时候 添加一个机器人进来
	if (!pTable || pTable->GetTableMonthID() != 0)
		return;
	if (pTable->GetRoleManager().GetRoleSum() > 2)
		return;
	BYTE RobotSum = 0;
	for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
			continue;
		if (pRole->GetRoleExInfo()->IsRobot())
			++RobotSum;
	}
	if (RobotSum > 0)
		return;

	DWORD Key = (pTable->GetTableTypeID() << 16) + pTable->GetTableMonthID();
	multimap<DWORD, DWORD>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.find(Key);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.end())
		return;
	DWORD RobotID = Iter->second;

	DWORD RobotNum = (RandUInt() % 99999) % 3;
	for (DWORD i = 0; i < RobotNum; ++i)
	{
		GameRobot* pRobot = GetFreeRobot(RobotID, pTable);//获取一个空闲的机器人
		if (!pRobot)
			return;
		CRoleEx* pRole = pRobot->GetRoleInfo();
		if (!pRole)
			return;
		if (!g_FishServer.GetTableManager()->OnPlayerJoinTable(pTable->GetTableID(), pRole))
		{
			pRobot->ResetRobot();
		}
	}
}
void GameRobotManager::Update()
{
	if (!m_IsLoadFinish)
	{
		return;
	}
	UpdateWriteList();
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		if ((*it)->IsRobotUsed())
		{
			(*it)->Update();
		}
	}
}
void GameRobotManager::AdddWriteRobot(WORD TableID, DWORD WriteTime)
{
	tagRobotWrite pInfo;
	pInfo.TableID = TableID;
	pInfo.TimeLog = WriteTime;
	m_WriteList.push_back(pInfo);
}
void GameRobotManager::UpdateWriteList()
{
	if (m_WriteList.empty())
		return;
	DWORD Time = timeGetTime();
	list<tagRobotWrite>::iterator Iter = m_WriteList.begin();
	for (; Iter != m_WriteList.end();)
	{
		if (Time > Iter->TimeLog)
		{
			GameTable* pTable = g_FishServer.GetTableManager()->GetTable(Iter->TableID);
			if (!pTable)
			{
				//已经没有这个桌子了
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			if (pTable->GetTablePlayerSum() == 0)
			{
				//桌子空了
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			BYTE TableTypeID = pTable->GetTableTypeID();
			BYTE MonthID = pTable->GetTableMonthID();
			
			DWORD Key = (TableTypeID << 16) + MonthID;
			multimap<DWORD, DWORD>::iterator IterFind = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.find(Key);
			if (IterFind == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.end())
				return;
			DWORD RobotID = IterFind->second;

			GameRobot* pRobot = GetFreeRobot(RobotID, pTable);
			if (!pRobot)
			{
				++Iter;
				continue;
			}
			//加入成功通知客户端机器人进入，不然就释放机器人
			if (g_FishServer.GetTableManager()->OnPlayerJoinTable(Iter->TableID, pRobot->GetRoleInfo()))
			{
				g_FishServer.GetTableManager()->GetTable(Iter->TableID)->SendRoleJoinInfo(pRobot->GetRobotUserID());
			}
			Iter = m_WriteList.erase(Iter);
		}
		else
		{
			return;
		}
	}
}

//单个机器人处理
GameRobot::GameRobot(GameRobotManager* pManager, CRoleEx* pRole) :m_pManager(pManager), m_IsUse(false), m_pRole(pRole)
{
	if (!pRole || !m_pManager)
	{
		ASSERT(false);
		return;
	}
	m_RobotID = 0;
	//
	m_LeaveTableTimeLog = 0;
	m_RoomTimeLog = 0;
	//
	m_NowRate = 0xff;
	m_ChangeRateTimeLog = 0;
	m_RateTimeLog = 0;
	//
	m_UseSkillTimeLog = 0;
	m_SkillTimeLog = 0;
	//
	m_ChangeLauncherTimeLog = 0;
	m_LauncherTimeLog = 0;
	//
	m_OpenFireTimeLog = 0;
	//
	m_StopOpenFireTimeLog = 0;
	//
	m_RobotInfoTimeLog = 0;
	//
	m_IsOpenFire = false;
	m_RobotOpenFireTimeLog = 0;
	//
	m_RobotSendCharmItemTimeLog = 0;
	m_dwRobotSendCharmTimeLog = 0;
	m_vecSendCharmArray.clear();
	//
	m_dwSendCharmTineLog = 0;
	//
	m_RobotCharmEvent.Clear();
}
GameRobot::~GameRobot()
{

}
void GameRobot::SetRobotUse(DWORD RobotID)
{
	if (m_IsUse)
	{
		ASSERT(false);
		return;
	}
	m_RobotID = RobotID;
	m_IsUse = true;
}
void GameRobot::ResetRobot()
{
	if (!m_IsUse)
	{
		ASSERT(false);
		return;
	}
	m_RobotID = 0;
	m_IsUse = false;
	//
	m_LeaveTableTimeLog = 0;
	m_RoomTimeLog = 0;
	//
	m_NowRate = 0xff;
	m_ChangeRateTimeLog = 0;
	m_RateTimeLog = 0;
	//
	m_UseSkillTimeLog = 0;
	m_SkillTimeLog = 0;
	//
	m_ChangeLauncherTimeLog = 0;
	m_LauncherTimeLog = 0;
	//
	m_OpenFireTimeLog = 0;
	//
	m_StopOpenFireTimeLog = 0;
	//
	m_RobotInfoTimeLog = 0;
	//
	m_IsOpenFire = false;
	m_RobotOpenFireTimeLog = 0;

	//
	m_RobotSendCharmItemTimeLog = 0;
	m_dwRobotSendCharmTimeLog = 0;
	m_vecSendCharmArray.clear();
	//
	m_dwSendCharmTineLog = 0;
	//
	m_RobotCharmEvent.Clear();
}
DWORD GameRobot::GetRobotUserID()
{
	if (!m_pRole)
		return 0;
	return m_pRole->GetUserID();
}
CRoleEx* GameRobot::GetRoleInfo()
{
	return m_pRole;
}
void GameRobot::Update()
{
	DWORD tickNow = timeGetTime();
	UpdateRobotRoom(tickNow);
	UpdateRobotRate(tickNow);
	//UpdateRobotSkill();
	UpdateRobotLauncher(tickNow);
	UpdateRobotOpenFire(tickNow);
	UpdateRobotIsCanOpenFire(tickNow);
	//UpdateRobotCharm();
	//UpdateRobotSendCharm();
}
void GameRobot::UpdateRobotRoom(DWORD tickNow)
{
	if (m_RoomTimeLog != 0 && tickNow - m_RoomTimeLog < 5000)
		return;
	m_RoomTimeLog = tickNow;
	//更新机器人进出房间
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	//m_IsInTable = false;
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());
	if (!pRole)//玩家不在桌子上
		return;
	HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	//没有钱了让机器人离开桌子
	int64 robotMoney = m_pRole->GetRoleInfo().money1 + m_pRole->GetRoleInfo().money2;
	auto it = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pRole->GetTableType());
	double tableBasicRate = 0.00;
	if (it != g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		tableBasicRate = it->second.BasicRatio;
	}
	CConfig* m_pConfig = g_FishServer.GetTableManager()->GetGameConfig();
	int nConsume = (int)(m_pConfig->BulletConsume(pRole->GetLauncherType())	 // 每个炮的基础消耗
		*m_pConfig->BulletMultiple(pRole->GetRateIndex()) //玩家调整的倍率
		*g_FishServer.GetRatioValue()				 // 大厅的货币比例
		*tableBasicRate					 // 桌子的基础倍率 
		*MONEY_RATIO);
	//不够再发一炮让机器人延迟2秒离开桌子
	if (robotMoney < nConsume)
	{
		m_LeaveTableTimeLog = 1 ;
	}
	else
	{
		if (!Iter->second.RoomInfo.IsOpen)//为开启的换 表示玩家一直在桌子里面
			return;
	}
	if (m_LeaveTableTimeLog == 0)
	{
		DWORD JoinSec = RandUInt() % (Iter->second.RoomInfo.RoomInfo.OpenMaxSec - Iter->second.RoomInfo.RoomInfo.OpenMinSec) + Iter->second.RoomInfo.RoomInfo.OpenMinSec;//在房间待的秒数
		m_LeaveTableTimeLog = tickNow + JoinSec * 1000;
	}
	if (tickNow > m_LeaveTableTimeLog)
	{
		//保存玩家游戏记录
		CRoleEx* pRoleEx = pRole->GetRoleExInfo();
		DBR_Cmd_SaveRecord recordMsg;
		SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
		recordMsg.model = 2;
		recordMsg.uid = pRoleEx->GetRoleInfo().Uid;
		recordMsg.table_id = 0;
		recordMsg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_code = 2;
		g_FishServer.SendNetCmdToDB(&recordMsg);
		//机器人需要离开桌子了
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(GetRobotUserID());//让机器人离开桌子
		//设置缓存
		DWORD LeaveSec = RandUInt() % (Iter->second.RoomInfo.RoomInfo.StopMaxSec - Iter->second.RoomInfo.RoomInfo.StopMinSec) + Iter->second.RoomInfo.RoomInfo.StopMinSec;
		m_pManager->AdddWriteRobot(pRole->GetTableID(), tickNow + LeaveSec * 1000);
		//回收机器人
		ResetRobot();
	}
}
void GameRobot::UpdateRobotRate(DWORD tickNow)
{
	if (m_RateTimeLog != 0 && tickNow - m_RateTimeLog < 300)
		return;
	m_RateTimeLog = tickNow;
	//更新机器人切换倍率
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//玩家进入桌子 异步的
	if (!pUser /*|| m_pRole->GetRoleMonth().IsInMonthTable()*/)
		return;
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
	if (!pTable)
		return;
	if (m_NowRate == 0xff)
	{
		if (m_ChangeRateTimeLog == 0)
		{
			HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
			if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
				return;

			DWORD RateF = RandUInt() % Iter->second.RateInfo.MaxRateF;
			vector<tagRobotRateType>::iterator IterVec = Iter->second.RateInfo.RateVec.begin();
			for (; IterVec != Iter->second.RateInfo.RateVec.end(); ++IterVec)
			{
				if (RateF < IterVec->RateF)
				{
					//切换到当前倍率
					//不可以一次切换到指定倍率 必须 一级一级的切 中间间隔
					m_NowRate = IterVec->RateType;
					break;
				}
			}
			HashMap<BYTE, TableInfo>::iterator IterTable = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pTable->GetTableTypeID());
			if (IterTable == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
				return;
			if (m_NowRate < IterTable->second.MinRate)
				m_NowRate = IterTable->second.MinRate;
			if (m_NowRate > IterTable->second.MaxRate)
				m_NowRate = IterTable->second.MaxRate;
			NetCmdChangeRateType msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
			msg.Seat = pUser->GetSeatID();
			msg.RateIndex = m_NowRate;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
			m_NowRate = 0xff;


			DWORD JoinSec = RandUInt() % (Iter->second.RateInfo.RateInfo.OpenMaxSec - Iter->second.RateInfo.RateInfo.OpenMinSec) + Iter->second.RateInfo.RateInfo.OpenMinSec;//切换倍率的CD
			m_ChangeRateTimeLog = tickNow + JoinSec * 1000;
		}
		else if (tickNow >= m_ChangeRateTimeLog)
		{
			HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
			if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
				return;

			if (!Iter->second.RateInfo.IsOpen)
				return;

			DWORD RateF = RandUInt() % Iter->second.RateInfo.MaxRateF;
			vector<tagRobotRateType>::iterator IterVec = Iter->second.RateInfo.RateVec.begin();
			for (; IterVec != Iter->second.RateInfo.RateVec.end(); ++IterVec)
			{
				if (RateF < IterVec->RateF)
				{
					//切换到当前倍率
					//不可以一次切换到指定倍率 必须 一级一级的切 中间间隔
					m_NowRate = IterVec->RateType;
					break;
				}
			}

			//修改当前倍率
			HashMap<BYTE, TableInfo>::iterator IterTable = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pTable->GetTableTypeID());
			if (IterTable == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
				return;
			if (m_NowRate < IterTable->second.MinRate)
				m_NowRate = IterTable->second.MinRate;
			if (m_NowRate > IterTable->second.MaxRate)
				m_NowRate = IterTable->second.MaxRate;

			DWORD JoinSec = RandUInt() % (Iter->second.RateInfo.RateInfo.OpenMaxSec - Iter->second.RateInfo.RateInfo.OpenMinSec) + Iter->second.RateInfo.RateInfo.OpenMinSec;//切换倍率的CD
			m_ChangeRateTimeLog = tickNow + JoinSec * 1000;
		}
	}
	else
	{
		if (pUser->GetRateIndex() == m_NowRate)
		{
			m_NowRate = 0xff;
			return;
		}
		//将玩家当前倍率 往 指定倍率进行切换
		BYTE RateIndex = (m_NowRate < pUser->GetRateIndex() ? pUser->GetRateIndex() - 1 : pUser->GetRateIndex() + 1);
		NetCmdChangeRateType msg;
		SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
		msg.Seat = pUser->GetSeatID();
		msg.RateIndex = RateIndex;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
		return;
	}
}
void GameRobot::UpdateRobotSkill()
{
	if (m_SkillTimeLog != 0 && timeGetTime() - m_SkillTimeLog < 1000)
		return;
	m_SkillTimeLog = timeGetTime();
	//机器人使用技能 普通的
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//玩家进入桌子 异步的
	if (!pRole)
		return;
	if (m_UseSkillTimeLog == 0)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		DWORD JoinSec = RandUInt() % (Iter->second.SkillInfo.SkillInfo.OpenMaxSec - Iter->second.SkillInfo.SkillInfo.OpenMinSec) + Iter->second.SkillInfo.SkillInfo.OpenMinSec;//切换基恩能够的CD
		m_UseSkillTimeLog = timeGetTime() + JoinSec * 1000;//下次使用技能的CD
	}
	else if (timeGetTime() >= m_UseSkillTimeLog)
	{
		GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pRole->GetTableID());
		if (!pTable)
			return;
		if (pTable->GetFishDesk()->GetFishSum() < 80)
			return;
		if ( pTable->GetFishDesk()->GetFishSum() < 20)
			return;

		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;

		if (!Iter->second.SkillInfo.IsOpen)
			return;

		//使用技能
		BYTE SkillID = g_FishServer.GetTableManager()->GetGameConfig()->CannonOwnSkill(pRole->GetLauncherType());

		NetCmdUseSkill msg;
		SetMsgInfo(msg, CMD_USE_SKILL, sizeof(msg));
		msg.SkillID = SkillID;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);

		

		DWORD JoinSec = RandUInt() % (Iter->second.SkillInfo.SkillInfo.OpenMaxSec - Iter->second.SkillInfo.SkillInfo.OpenMinSec) + Iter->second.SkillInfo.SkillInfo.OpenMinSec;//切换基恩能够的CD
		m_UseSkillTimeLog = timeGetTime() + JoinSec * 1000;//下次使用技能的CD

		m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog,timeGetTime() + 3000);
	}
}
void GameRobot::UpdateRobotLauncher(DWORD tickNow)
{
	if (m_LauncherTimeLog != 0 && tickNow - m_LauncherTimeLog < 1000)
		return;
	m_LauncherTimeLog = tickNow;
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//玩家进入桌子 异步的
	if (!pUser)
		return;
	//切换炮的CD
	if (m_ChangeLauncherTimeLog == 0)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		DWORD LauncherCDSec = RandUInt() % (Iter->second.LauncherInfo.LauncherInfo.OpenMaxSec - Iter->second.LauncherInfo.LauncherInfo.OpenMinSec) + Iter->second.LauncherInfo.LauncherInfo.OpenMinSec;
		m_ChangeLauncherTimeLog = tickNow + LauncherCDSec * 1000;
		//玩家进入的随机产生一种可以使用的炮
		DWORD RateF = RandUInt() % Iter->second.LauncherInfo.MaxRateF;
		vector<tagRobotLauncherType>::iterator IterVec = Iter->second.LauncherInfo.LauncherVec.begin();
		for (; IterVec != Iter->second.LauncherInfo.LauncherVec.end(); ++IterVec)
		{
			if (RateF < IterVec->RateF)
			{
				BYTE LauncherType = IterVec->LauncherType;
				while (LauncherType >0 && !m_pRole->GetRoleLauncherManager().IsCanUserLauncherByID(LauncherType))
				{
					--LauncherType;
				}

				//切换到当前炮台
				NetCmdChangeLauncherType change;//切炮前2s 不可以发送子弹
				SetMsgInfo(change, CMD_CHANGE_LAUNCHER_TYPE, sizeof(change));
				change.Seat = pUser->GetSeatID();
				change.LauncherType = LauncherType;
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &change);

				return;
			}
		}
	}
	else if (tickNow >= m_ChangeLauncherTimeLog)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		if (!Iter->second.LauncherInfo.IsOpen)
			return;
		//可以切换炮台了
		DWORD LauncherCDSec = RandUInt() % (Iter->second.LauncherInfo.LauncherInfo.OpenMaxSec - Iter->second.LauncherInfo.LauncherInfo.OpenMinSec) + Iter->second.LauncherInfo.LauncherInfo.OpenMinSec;
		m_ChangeLauncherTimeLog = tickNow + LauncherCDSec * 1000;

		DWORD RateF = RandUInt() % Iter->second.LauncherInfo.MaxRateF;
		vector<tagRobotLauncherType>::iterator IterVec = Iter->second.LauncherInfo.LauncherVec.begin();
		for (; IterVec != Iter->second.LauncherInfo.LauncherVec.end(); ++IterVec)
		{
			if (RateF < IterVec->RateF)
			{
				BYTE LauncherType = IterVec->LauncherType;
				while (LauncherType >0 && !m_pRole->GetRoleLauncherManager().IsCanUserLauncherByID(LauncherType))
				{
					--LauncherType;
				}

				//切换到当前炮台
				NetCmdChangeLauncherType change;//切炮前2s 不可以发送子弹
				SetMsgInfo(change, CMD_CHANGE_LAUNCHER_TYPE, sizeof(change));
				change.Seat = pUser->GetSeatID();
				change.LauncherType = LauncherType;
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &change);

				m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog, tickNow + 1000);
				return;
			}
		}
	}
}
void GameRobot::UpdateRobotOpenFire(DWORD tickNow)
{
	if (m_OpenFireTimeLog == 0 || tickNow - m_OpenFireTimeLog > 200)
	{
		m_OpenFireTimeLog = tickNow;

		CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//玩家进入桌子 异步的
		if (!pUser)
			return;
		
		//判断玩家是否可以开炮
		if (m_NowRate != 0xff)
			return;
		if (!m_IsOpenFire)
			return;
		if (m_OpenFireTimeLog < m_StopOpenFireTimeLog)
			return;
		GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
		if (!pTable)
			return; 
		m_Angle = pTable->GetFishDesk()->GetAngleByFish(m_LockFishID, pUser->GetSeatID());
		if (m_LockFishID == 0 && m_Angle == 0xffff)
			return;
		//玩家开炮
		if (!pUser->IsFullEnergy())//发射子弹
		{
			if (pUser->CheckFire(0))
			{
				//1.发射子弹
				NetCmdBullet msg;
				SetMsgInfo(msg, CMD_BULLET, sizeof(NetCmdBullet));
				msg.BulletID = 0;
				msg.Angle = m_Angle % 65535;
				msg.LockFishID = (pUser->GetLauncherType() == 4 ? 0 : m_LockFishID);
				msg.LauncherType = pUser->GetLauncherType();
				msg.Energy = (uint)pUser->GetEnergy();
				msg.ReboundCount = 0; //pUser->GetRoleExInfo()->GetRoleVip().GetLauncherReBoundNum();
				if (msg.LauncherType == 4)
					msg.ReboundCount = 0;
				//将命令设置到GameServer去
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
			}
		}
		else//发射大招
		{
			NetCmdSkillLaser msgSkill;
			SetMsgInfo(msgSkill, CMD_SKILL_LASER_REQUEST, sizeof(msgSkill));
			msgSkill.Degree = m_Angle % 65535;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msgSkill);

			m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog, tickNow + 2000);
		}
	}
}
void GameRobot::UpdateRobotIsCanOpenFire(DWORD tickNow)
{
	//更新机器人是否可以开火
	//机器人连续开炮后 需要停止一段时间
	if (tickNow <= m_RobotOpenFireTimeLog)
		return;
	HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	if (!Iter->second.OpenFireInfo.IsOpen)
	{
		m_IsOpenFire = true;
		return;
	}
	if (m_IsOpenFire)
	{
		m_IsOpenFire = false;
		//设置停火 等待下次开始
		DWORD StarSec = RandUInt() % (Iter->second.OpenFireInfo.FireInfo.StopMaxSec - Iter->second.OpenFireInfo.FireInfo.StopMinSec) + Iter->second.OpenFireInfo.FireInfo.StopMinSec;
		m_RobotOpenFireTimeLog = tickNow + StarSec * 1000;
	}
	else
	{
		m_IsOpenFire = true;
		DWORD StarSec = RandUInt() % (Iter->second.OpenFireInfo.FireInfo.OpenMaxSec - Iter->second.OpenFireInfo.FireInfo.OpenMinSec) + Iter->second.OpenFireInfo.FireInfo.OpenMinSec;
		m_RobotOpenFireTimeLog = tickNow + StarSec * 1000;
	}
}
void GameRobot::UpdateRobotCharm()
{
	//关系机器人使用魅力物品
	if (m_RobotSendCharmItemTimeLog != 0 && timeGetTime() - m_RobotSendCharmItemTimeLog < 2000)
		return;
	m_RobotSendCharmItemTimeLog = timeGetTime();

	HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	if (!Iter->second.CharmInfo.IsOpen)
		return;
	if (m_dwRobotSendCharmTimeLog == 0)
	{
		m_dwRobotSendCharmTimeLog = timeGetTime() + (RandUInt() % (Iter->second.CharmInfo.SendCharmMaxCDSec - Iter->second.CharmInfo.SendCharmMinCDSec) + Iter->second.CharmInfo.SendCharmMinCDSec) * 1000;
		return;
	}
	if (timeGetTime() < m_dwRobotSendCharmTimeLog)
		return;

	BYTE RandF = static_cast<BYTE>(RandUInt() % 100);
	if (RandF < Iter->second.CharmInfo.NonRateF)
	{
		m_dwRobotSendCharmTimeLog = timeGetTime() + (RandUInt() % (Iter->second.CharmInfo.SendCharmMaxCDSec - Iter->second.CharmInfo.SendCharmMinCDSec) + Iter->second.CharmInfo.SendCharmMinCDSec) * 1000;
		return;
	}
	//每2秒更新一次
	//1.主动 处理 
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//玩家进入桌子 异步的
	if (!pUser /*|| m_pRole->GetRoleMonth().IsInMonthTable()*/)
		return;
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
	if (!pTable)
		return;
	if (pTable->GetRoleManager().GetRoleSum() <= 1)
		return;
	BYTE SeatIndex = RandUInt() % (pTable->GetRoleManager().GetRoleSum() -1);
	DWORD dwUserID = 0;
	BYTE NowIndex = 0;
	for (BYTE i = 0; i <= SeatIndex;)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
		{
			++i;
			continue;
		}
		else if (pRole->GetID() == GetRobotUserID())
		{
			++i;
			++NowIndex;
			continue;
		}
		else
		{
			if (NowIndex == SeatIndex)
			{
				dwUserID = pRole->GetID();
				break;
			}
			++i;
			++NowIndex;
		}
	}
	if (dwUserID == 0)
		return;
	//随机从桌子里挑选一位玩家 随机参数一种魅力 物品 随机产生魅力物品的数量 我们进行记录到集合里面去	
	DWORD RateF = RandUInt() % Iter->second.CharmInfo.SendInfo.dwSendCharmArrayMaxRateF;
	std::vector<tagRobotSendCharmInfo>::iterator IterVec = Iter->second.CharmInfo.SendInfo.vecSendCharmArray.begin();
	for (; IterVec != Iter->second.CharmInfo.SendInfo.vecSendCharmArray.end(); ++IterVec)
	{
		if (RateF < IterVec->RateF)
		{
			BYTE ItemID = IterVec->CharmIndexID;
			WORD Sum = RandUInt() % (IterVec->MaxItemSum - IterVec->MinItemSum) + IterVec->MinItemSum;
			if (Sum > 0)
			{
				tagGameRobotCharm pEvent;
				pEvent.dwUserID = dwUserID;
				pEvent.CharmIndexID = ItemID;
				pEvent.CharmItemSum = Sum;
				pEvent.BeginTimeLog = timeGetTime() + 2000;
				m_vecSendCharmArray.push_back(pEvent);
			}
			//加入到发送集合里面去
			m_dwRobotSendCharmTimeLog = timeGetTime() + (RandUInt() % (Iter->second.CharmInfo.SendCharmMaxCDSec - Iter->second.CharmInfo.SendCharmMinCDSec) + Iter->second.CharmInfo.SendCharmMinCDSec) * 1000;
			break;
		}
	}
}
void GameRobot::UpdateRobotSendCharm()
{
	//1s更新一次
	if (m_dwSendCharmTineLog != 0 && timeGetTime() - m_dwSendCharmTineLog < 500)
		return;
	m_dwSendCharmTineLog = timeGetTime();
	if (m_vecSendCharmArray.empty())
		return;

	HashMap<DWORD, tagGameRobotConfig>::iterator IterRobot = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (IterRobot == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	if (!IterRobot->second.CharmInfo.IsOpen)
		return;

	list<tagGameRobotCharm>::iterator Iter = m_vecSendCharmArray.begin();
	for (; Iter != m_vecSendCharmArray.end();)
	{
		//向指定玩家发送魅力物品
		CRoleEx* pDestRole = g_FishServer.GetRoleManager()->QueryUser(Iter->dwUserID);
		if (!pDestRole)
		{
			Iter = m_vecSendCharmArray.erase(Iter);
			continue;
		}
		if (Iter->BeginTimeLog > m_dwSendCharmTineLog)
			return;
		if (Iter->CharmItemSum == 0)
		{
			Iter = m_vecSendCharmArray.erase(Iter);
			continue;
		}

		//扣除玩家金币 或者 钻石
		HashMap<BYTE, tagCharmOnce>::iterator IterCharm = g_FishServer.GetFishConfig().GetCharmConfig().CharmIndexMap.find(Iter->CharmIndexID);
		if (IterCharm == g_FishServer.GetFishConfig().GetCharmConfig().CharmIndexMap.end())
		{
			Iter = m_vecSendCharmArray.erase(Iter);
			continue;
		}
		LC_Cmd_ChangeRoleCharmResult pResult;
		SetMsgInfo(pResult, GetMsgType(Main_Charm, LC_ChangeRoleCharmResult), sizeof(LC_Cmd_ChangeRoleCharmResult));
		pResult.Result = true;
		pResult.dwDestUserID = pDestRole->GetUserID();
		pResult.dwDestUserCharmValue = g_FishServer.GetFishConfig().GetCharmValue(pDestRole->GetRoleInfo().CharmArray);
		m_pRole->SendDataToClient(&pResult);
		LC_Cmd_TableRoleCharmInfo msgTable;
		SetMsgInfo(msgTable, GetMsgType(Main_Charm, LC_TableRoleCharmInfo), sizeof(LC_Cmd_TableRoleCharmInfo));
		msgTable.SrcUserID = m_pRole->GetUserID();
		msgTable.DestUserID = Iter->dwUserID;
		msgTable.CharmIndex = Iter->CharmIndexID;
		msgTable.ItemSum =1;
		m_pRole->SendDataToClient(&msgTable);
		m_pRole->SendDataToTable(&msgTable);


		Iter->CharmItemSum -= 1;
		if (Iter->CharmItemSum == 0)
			m_vecSendCharmArray.erase(Iter);

		m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog, timeGetTime() + 3000);
		return;
	}
}