#include "StdAfx.h"
#include "Role.h"
#include "TableManager.h"
#include "FishLogic\FishScene\FishResManager.h"
#include "FishServer.h"
#include<atlconv.h>
#include <mmsystem.h>

#include "..\CommonFile\FishServerConfig.h"
#include<algorithm>

#pragma comment(lib, "winmm.lib")

extern void SendLogDB(NetCmd* pCmd);

static volatile long	g_UpdateCount = 0;
static volatile bool	g_bRun = true;
struct TableUpdateParam
{
	GameTable* pTable;
	bool m_bUpdateTime;
};
SafeList<TableUpdateParam*> g_SafeList[THREAD_NUM];
UINT WINAPI TableThreadUpdate(void *p)
{
	int idx = (int)p;
	SafeList<TableUpdateParam*> &safeList = g_SafeList[idx];
	while (g_bRun)
	{
		TableUpdateParam* pDesk = safeList.GetItem();
		while (pDesk)
		{
			//根据参数更新桌子
			if (!pDesk)
				continue;
			if (!pDesk->pTable)
			{
				free(pDesk);
				continue;
			}
			pDesk->pTable->Update(pDesk->m_bUpdateTime);
			::InterlockedIncrement(&g_UpdateCount);
			free(pDesk);

			pDesk = safeList.GetItem();
		}
		Sleep(1);
	}
	return 0;
}



//全局桌子管理器
TableManager::TableManager() //:m_GameConfig(this)
{
	m_MaxTableID = 0;
	m_TableGlobeMap.clear();
	//初始化系统调控
	m_sysMinutes.clear();
}
TableManager::~TableManager()
{
	Destroy();
}
void TableManager::OnInit()
{
	//读取全局的配置文件
	m_pGameConfig = new CConfig(this);
	FishCallback::SetFishSetting(m_pGameConfig);
	if (!FishResManager::Inst() || !FishResManager::Inst()->Init(L"fishdata"))
	{
		//AfxMessageBox(TEXT("读取fishdata失败，请检查"));
		return;
	}

	{
		m_JJCTimer.StartTimer(JJC_GAME_TIME_SPACE, REPEATTIMER);
	}

	for (int i = 0; i < THREAD_NUM; ++i)//开启线程
	{
		::_beginthreadex(0, 0, (TableThreadUpdate), (void*)i, 0, 0);
	}
	m_LastUpdate = timeGetTime();
}
void TableManager::Destroy()
{
	//当销毁的时候 
	//1.将所有桌子对象全部销毁掉 
	g_bRun = false;
	if (!m_TableVec.empty())
	{
		std::vector<GameTable*>::iterator Iter = m_TableVec.begin();
		for (; Iter != m_TableVec.end(); ++Iter)
		{
			if (!(*Iter))
				continue;
			//CTraceService::TraceString(TEXT("一个桌子对象 已经被销毁"), TraceLevel_Normal);
			delete *Iter;
		}
		m_TableVec.clear();
	}
	//2.清理全部的配置文件
	if (FishResManager::Inst())
		FishResManager::Inst()->Shutdown();
}
void TableManager::OnStopService()
{
	//当服务器停止的时候
	Destroy();//桌子管理器停止服务器的时候 不是关闭进程
}
void TableManager::Update(DWORD dwTimeStep)
{
	if (m_TableVec.empty())
		return;
	float TimeStep = (dwTimeStep - m_LastUpdate) * 0.001f;
	m_LastUpdate = dwTimeStep;
	//更新竞技场桌子
	bool m_bUpdateTime = m_JJCTimer.Update(TimeStep);
	if (m_bUpdateTime)
	{
		UpdateJJC(dwTimeStep);
	}
	//对桌子数组进行更新
	g_UpdateCount = 0;
	int UpdateSum = 0;
	for (UINT i = 0; i < m_TableVec.size(); ++i)
	{
		if (!m_TableVec[i])
			continue;
		if (m_TableVec[i]->IsTableRunning() == false)
			continue;
		int idx = i % THREAD_NUM;
		TableUpdateParam* pParam = (TableUpdateParam*)malloc(sizeof(TableUpdateParam));
		if (!pParam)
		{
			ASSERT(false);
			return;
		}
		pParam->m_bUpdateTime = false;
		pParam->pTable = m_TableVec[i];
		g_SafeList[idx].AddItem(pParam);
		++UpdateSum;
	}
	while (g_UpdateCount != UpdateSum)
		Sleep(1);
}
void TableManager::UpdateMinutes()
{
	CostAndProduceMin();
}
void TableManager::UpdateJJC(DWORD dwTimeStep)
{
	const tagFishJJC& jjc_cfg = g_FishServer.GetFishConfig().GetFishJJC();
	const tagJJC_Time& jjc_time_cfg = jjc_cfg.time;
	for (auto it = m_JJCGameTables.begin(); it != m_JJCGameTables.end(); ++it)
	{
		vector<JJCGameTables>& jjc_tables = it->second;
		for (auto tableIt = jjc_tables.begin(); tableIt != jjc_tables.end(); ++tableIt)
		{
			//运行时间大于了最大竞技场持续时间 或者 大家都没有子弹了 关闭游戏
			if ((tableIt->table1->IsTableRunning()
				&& dwTimeStep - tableIt->table1->GetTableStartTime() > static_cast<DWORD>(jjc_time_cfg.maxTime * 60 * 1000))
				|| (tableIt->table1->IsCanEndMonthGame() && tableIt->table2->IsCanEndMonthGame()))
			{
				JJCRewardRank(tableIt->table1, tableIt->table2, true);
				tableIt->table1->OnGameStop();
				tableIt->table2->OnGameStop();
			}
			//竞技场人满，开始游戏
			else if (!tableIt->table1->IsTableRunning() && !tableIt->table2->IsTableRunning()
				&& tableIt->table1->IsFull() && tableIt->table2->IsFull())
			{
				msg_ArenaStartInfo msg;
				SetMsgInfo(msg, 6040, sizeof(msg_ArenaStartInfo));
				msg.isStart = true;
				msg.waitPlayers = 8;
				tableIt->table1->SendDataToTableAllUser(&msg);
				tableIt->table2->SendDataToTableAllUser(&msg);
				tableIt->table1->OnGameStart();
				tableIt->table2->OnGameStart();
			}
			//通知桌子上面的人现在等待人数
			else if (!tableIt->table1->IsTableRunning() && !tableIt->table2->IsTableRunning()
				&& (!tableIt->table1->IsFull() || !tableIt->table2->IsFull()))
			{
				BYTE WaitPlayerSum = tableIt->table1->GetTablePlayerSum() + tableIt->table2->GetTablePlayerSum();
				if (WaitPlayerSum != 0)
				{
					msg_ArenaStartInfo msg;
					SetMsgInfo(msg, 6040, sizeof(msg_ArenaStartInfo));
					msg.isStart = false;
					msg.waitPlayers = WaitPlayerSum;
					tableIt->table1->SendDataToTableAllUser(&msg);
					tableIt->table2->SendDataToTableAllUser(&msg);
				}
			}
			//游戏运行中，同步排行数据
			else if (tableIt->table1->IsTableRunning() && tableIt->table2->IsTableRunning())
			{
				JJCRewardRank(tableIt->table1, tableIt->table2, false);
			}
		}
	}
}
GameTable* TableManager::GetTable(WORD TableID)
{
	{
		if (TableID == 0xffff || TableID >= m_TableVec.size())
			//ASSERT(false);
			return NULL;
	}
	GameTable* pTable = m_TableVec[TableID];
	return pTable;
}
bool TableManager::OnPlayerJoinTable(WORD TableID, CRoleEx* pRoleEx, bool IsSendToClient)
{
	//玩家进入一张桌子 
	//判断玩家是否可以加入房间
	//首先发送玩家是否可以进入房间的命令 进入成功后 在发送其他玩家的数据
	if (!pRoleEx)
	{
		ASSERT(false);
		return false;
	}
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	BYTE TableTypeID = pTable->GetTableTypeID();
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(pRoleEx->GetUserID());
	if (Iter != m_RoleTableMap.end())
	{
		//玩家已经有桌子了无法继续进入桌子
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return false;
	}
	HashMap<BYTE, TableInfo>::iterator IterConfig = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (IterConfig == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return false;
	}

	if (pRoleEx->IsRobot() && !pTable->IsCanAddRobot())
	{
		return false;
	}
	if (pTable->OnRoleJoinTable(pRoleEx, pTable->GetTableMonthID(), IsSendToClient))
	{
		ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
		if (pData)
		{
			pData->IsInScene = true;
		}

		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRoleEx->GetUserID();
		msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2;
		msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = true;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = TableTypeID;
		msgDB.TableMonthID = pTable->GetTableMonthID();
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
		//用户成功加入桌子 我们想要发送命令到客户端去 告诉客户端玩家进入游戏成功
		m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), pTable->GetTableID()));

		if (!pRoleEx->IsRobot())
			g_FishServer.GetRobotManager().OnJoinRobotToTable(pTable);
		return true;
	}
	else
	{
		//非人满的情况 不可以进入桌子 我们无须继续判断
		//发送命令到客户端 告诉玩家进入桌子失败了
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		return false;
	}
}
bool TableManager::OnPlayerJoinTable(BYTE TableTypeID, CRoleEx* pRoleEx, BYTE MonthID, bool IsSendToClient)
{
	//玩家进入一张桌子 
	//判断玩家是否可以加入房间
	//首先发送玩家是否可以进入房间的命令 进入成功后 在发送其他玩家的数据
	if (!pRoleEx)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(pRoleEx->GetUserID());
	if (Iter != m_RoleTableMap.end())
	{
		//玩家已经有桌子了无法继续进入桌子
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		memset(msg.PlayerData.playerName, 0, MAX_NICKNAME + 1);
		memcpy_s(msg.PlayerData.playerName, MAX_NICKNAME, pRoleEx->GetRoleInfo().NickName, MAX_NICKNAME);
		memcpy_s(msg.PlayerData.icon, ICON_LENGTH, pRoleEx->GetRoleInfo().icon, ICON_LENGTH);
		pRoleEx->SendDataToClient(&msg);
		return false;
	}
	HashMap<BYTE, TableInfo>::iterator IterConfig = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (IterConfig == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		memset(msg.PlayerData.playerName, 0, MAX_NICKNAME + 1);
		memcpy_s(msg.PlayerData.playerName, MAX_NICKNAME, pRoleEx->GetRoleInfo().NickName, MAX_NICKNAME);
		memcpy_s(msg.PlayerData.icon, ICON_LENGTH, pRoleEx->GetRoleInfo().icon, ICON_LENGTH);
		pRoleEx->SendDataToClient(&msg);
		return false;
	}
	if (MonthID != 0 && CanPlayerJoinJJC(pRoleEx, MonthID) == false)
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		memset(msg.PlayerData.playerName, 0, MAX_NICKNAME + 1);
		memcpy_s(msg.PlayerData.playerName, MAX_NICKNAME, pRoleEx->GetRoleInfo().NickName, MAX_NICKNAME);
		memcpy_s(msg.PlayerData.icon, ICON_LENGTH, pRoleEx->GetRoleInfo().icon, ICON_LENGTH);
		pRoleEx->SendDataToClient(&msg);
		return false;
	}

	GameTable* joinTable = GetPlayerJoinTable(TableTypeID, MonthID);
	if (joinTable != NULL)
	{
		if (PlayerJoinTable(joinTable, pRoleEx, TableTypeID, MonthID, IsSendToClient) == false)
		{
			return false;
		}
	}
	else
	{
		GameTable* pNewTable = new GameTable();
		if (!pNewTable)
		{
			return false;
		}
		pNewTable->OnInit(m_MaxTableID, TableTypeID, MonthID);//将桌子初始化
		m_MaxTableID++;
		m_TableVec.push_back(pNewTable);
		//添加竞技场数据
		if (MonthID != 0)
		{
			AddJJCGameTable(TableTypeID, MonthID, pNewTable);
		}
		if (PlayerJoinTable(pNewTable, pRoleEx, TableTypeID, MonthID, IsSendToClient) == false)
		{
			return false;
		}
	}
	return true;
}

void TableManager::OnPlayerLeaveTable(DWORD dwUserID)
{
	//根据玩家找到当前的桌子 
	//根据桌子找到玩家 然后让玩家离开桌子
	//CWHDataLocker lock(m_CriticalSection);
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return;
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	//当玩家离开桌子的时候 将金币刷新下到客户端去
	CRoleEx* pRole = g_FishServer.GetRoleManager()->QueryUser(dwUserID);
	if (pRole)
	{
		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRole->GetUserID();
		msgDB.CurrceySum = pRole->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRole->GetRoleInfo().money1 + pRole->GetRoleInfo().money2;
		msgDB.MedalSum = pRole->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = false;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = pTable->GetTableTypeID();
		msgDB.TableMonthID = pTable->GetTableMonthID();
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
	}
	pTable->OnRoleLeaveTable(dwUserID);
	m_RoleTableMap.erase(Iter);//将玩家的数据删除掉

	if (!pRole->IsRobot())
		g_FishServer.GetRobotManager().OnRoleLeaveNormalRoom(pTable);
}
bool TableManager::OnHandleTableMsg(DWORD dwUserID, NetCmd* pData)
{
	//将Game消息传递到内部去
	//CWHDataLocker lock(m_CriticalSection);
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return false;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return false;
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	pTable->OnHandleTableMsg(dwUserID, pData);
	return true;
}
void TableManager::SendDataToTable(DWORD dwUserID, NetCmd* pData)
{
	//将命令发送到指定玩家的桌子上面去
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		return;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return;
	}
	GameTable* pTable = m_TableVec[Index];
	if (pTable)
	{
		pTable->SendDataToTable(dwUserID, pData);
	}
	else
	{
		ASSERT(false);
	}
}
CRole* TableManager::SearchUser(DWORD dwUserid)
{
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserid);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return NULL;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return NULL;
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
		return NULL;
	}
	return pTable->SearchUser(dwUserid);
}
CConfig* TableManager::GetGameConfig()
{
	return m_pGameConfig;
}
void TableManager::OnChangeTableGlobel(WORD TableID, int64 AddGlobel, USHORT uTableRate)
{

	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	//AddGlobel>0 玩家花费，AddGlobel<0 系统产出
	if (AddGlobel >= 0)
	{
		//记录桌子每分钟收益
		auto it = m_sysMinutes.find(pTable->GetTableTypeID());
		if (it == m_sysMinutes.end())
		{
			SysYieldControl nControl;
			nControl.lastMinCost = AddGlobel;
			m_sysMinutes[pTable->GetTableTypeID()] = nControl;
		}
		else
		{
			it->second.lastMinCost += AddGlobel;
		}
		//记录桌子总收益
		auto it2 = m_sysTotal.find(pTable->GetTableTypeID());
		if (it2 == m_sysTotal.end())
		{
			TableTotalGold tg;
			tg.total_earn = AddGlobel;
			m_sysTotal[pTable->GetTableTypeID()] = tg;
		}
		else
		{
			it2->second.total_earn += AddGlobel;
			auto itTbConfig = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pTable->GetTableTypeID());
			if (itTbConfig != g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
			{
				//玩家花费大于了配置的重置值。重置桌子的收益率
				if (it2->second.total_earn > itTbConfig->second.InitTurn * MONEY_RATIO*g_FishServer.GetRatioValue())
				{
					it2->second.total_earn = 0;
					it2->second.total_produce = 0;
					//重置桌子类型玩家的收益率
					g_FishServer.GetRoleManager()->SetResetGameDataTable(pTable->GetTableTypeID());
				}
			}
		}
	}
	else
	{
		auto it = m_sysMinutes.find(pTable->GetTableTypeID());
		if (it == m_sysMinutes.end())
		{
			SysYieldControl nControl;
			nControl.lastMinProduce = abs(AddGlobel);
			m_sysMinutes[pTable->GetTableTypeID()] = nControl;
		}
		else
		{
			it->second.lastMinProduce += abs(AddGlobel);
		}
		auto it2 = m_sysTotal.find(pTable->GetTableTypeID());
		if (it2 == m_sysTotal.end())
		{
			TableTotalGold tg;
			tg.total_produce = abs(AddGlobel);
			m_sysTotal[pTable->GetTableTypeID()] = tg;
		}
		else
		{
			it2->second.total_produce += abs(AddGlobel);
		}
	}
	int64 AllGlobelSum = 0;
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter == m_TableGlobeMap.end())
	{
		AllGlobelSum = AddGlobel;
		m_TableGlobeMap.insert(HashMap<BYTE, GoldPool>::value_type(pTable->GetTableTypeID(), GoldPool(AllGlobelSum)));
	}
	else
	{
		Iter->second.gold += AddGlobel;
		AllGlobelSum = Iter->second.gold;
	}

	Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter->second.open)
	{
		if (AllGlobelSum <= 0)
		{
			Iter->second.open = false;
			//Iter->second.gold = 0;			
		}
	}
}
void TableManager::OnResetTableGlobel(WORD TableID, int64 nValue)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter != m_TableGlobeMap.end())
	{
		Iter->second = nValue;
	}
}
int64 TableManager::GetTableGlobel(WORD TableID)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return 0;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter != m_TableGlobeMap.end())
		return Iter->second.gold;
	else
		return 0;
}

bool TableManager::QueryPool(WORD TableID, int64 & nPoolGold)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter == m_TableGlobeMap.end())
	{
		return false;
	}
	nPoolGold = Iter->second.gold;
	return 	Iter->second.open;
}

void TableManager::QueryPool(BYTE TableTypeID, bool & bopen, int64 & nPoolGold)
{
	bopen = false;
	nPoolGold = 0;

	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(TableTypeID);
	if (Iter == m_TableGlobeMap.end())
	{
		return;
	}
	bopen = Iter->second.open;
	nPoolGold = Iter->second.gold;
}

CRole* TableManager::QueryRoleByRoleEx(CRoleEx* pRoleEx)
{
	auto it = m_TableVec.begin();
	for (; it != m_TableVec.end(); ++it)
	{
		CRole* pRole = (*it)->GetRoleManager().GetRoleByUserID(pRoleEx->GetUserID());
		if (pRole != NULL)
		{
			return pRole;
		}
	}
	return NULL;
}
void TableManager::CostAndProduceMin()
{
	for (auto it = m_sysMinutes.begin(); it != m_sysMinutes.end(); ++it)
	{
		//计算系统总产出
		it->second.TotalProduce += it->second.lastMinProduce;
		//计算玩家总花费
		it->second.TotalCost += it->second.lastMinCost;
		if (it->second.lastMinCost != 0 || it->second.lastMinProduce != 0)
		{
			//记录每分钟产出日志
			LOG_Sys_Minute_Record msg;
			msg.table_id = it->first;
			msg.game_id = g_FishServer.GetGameIndex();
			msg.min_earn = it->second.lastMinCost / MONEY_RATIO;
			msg.min_produce = it->second.lastMinProduce / MONEY_RATIO;
			msg.total_earn = it->second.TotalCost / MONEY_RATIO;
			msg.total_produce = it->second.TotalProduce / MONEY_RATIO;
			msg.rate = 0;
			BYTE tableType = it->first;
			const int64& InitProduce = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(tableType)->second.InitProduce;
			const int64& InitEarn = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(tableType)->second.InitEarn;
			auto itSys = m_sysTotal.find(tableType);
			if (itSys != m_sysTotal.end())
			{
				float sysRand = ((itSys->second.total_earn + InitEarn * MONEY_RATIO*g_FishServer.GetRatioValue())
					- (itSys->second.total_produce + InitProduce * MONEY_RATIO *g_FishServer.GetRatioValue()))*1.0f
					/ (itSys->second.total_earn + InitEarn * MONEY_RATIO*g_FishServer.GetRatioValue());
				msg.rate = sysRand;
			}

			SetMsgInfo(msg, DBR_Save_System_Minute_Record, sizeof(LOG_Sys_Minute_Record));
			g_FishServer.SendNetCmdToLogDB(&msg);

			//记录总产出数据
			DBR_Cmd_SaveTotalGold totalMsg;
			SetMsgInfo(totalMsg, DBR_Save_Total_Gold, sizeof(DBR_Cmd_SaveTotalGold));
			totalMsg.server_id = g_FishServer.GetGameIndex();
			totalMsg.table_id = it->first;
			totalMsg.lastEarn = it->second.lastMinCost / MONEY_RATIO;
			totalMsg.lastProduce = it->second.lastMinProduce / MONEY_RATIO;
			totalMsg.table_totalEarn = (itSys != m_sysTotal.end() ? itSys->second.total_earn / MONEY_RATIO : 0);
			totalMsg.table_totalProduce = (itSys != m_sysTotal.end() ? itSys->second.total_produce / MONEY_RATIO : 0);
			g_FishServer.SendNetCmdToDB(&totalMsg);
		}
		//重置下一分钟的数据
		it->second.lastMinProduce = 0;
		it->second.lastMinCost = 0;
	}
}

void TableManager::NewDayForTable()
{
	//重置每天的总产出
	for (auto it = m_sysMinutes.begin(); it != m_sysMinutes.end(); ++it)
	{
		it->second.TotalCost = 0;
		it->second.TotalProduce = 0;
	}
}
const vector<float>& TableManager::SysProduceRate(const BYTE tableType)
{
	auto it = m_sysTotal.find(tableType);
	if (it == m_sysTotal.end())
	{
		return SystemControl::item[1];
	}
	if (it->second.total_earn == 0 || it->second.total_produce == 0)
		return SystemControl::item[1];

	const int64& InitProduce = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(tableType)->second.InitProduce;
	const int64& InitEarn = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(tableType)->second.InitEarn;
	float sysRand = ((it->second.total_earn + InitEarn * MONEY_RATIO*g_FishServer.GetRatioValue())
		- (it->second.total_produce + InitProduce * MONEY_RATIO *g_FishServer.GetRatioValue()))*1.0f
		/ (it->second.total_earn + InitEarn * MONEY_RATIO*g_FishServer.GetRatioValue());

	if (sysRand < SystemControl::sysMinWin)
	{
		return SystemControl::item[2];
	}
	else if (sysRand > SystemControl::sysMaxWin)
	{
		return SystemControl::item[0];
	}
	return SystemControl::item[1];
}

bool TableManager::InitTableTotalGold(NetCmd* pData)
{
	DBO_Cmd_GameserverStart* pmsg = (DBO_Cmd_GameserverStart*)pData;
	DWORD dataVec = (pData->GetCmdSize() - sizeof(DBO_Cmd_GameserverStart)) / sizeof(tagTableGold);
	for (DWORD i = 0; i < dataVec; ++i)
	{
		TableTotalGold tg;
		BYTE table_id = pmsg->data[i].table_id;
		tg.total_earn = pmsg->data[i].total_earn * MONEY_RATIO;
		tg.total_produce = pmsg->data[i].total_produce * MONEY_RATIO;
		m_sysTotal.insert(make_pair(table_id, tg));
	}
	return true;
}

bool TableManager::PlayerJoinTable(GameTable* pTable, CRoleEx* pRoleEx, BYTE TableTypeID, BYTE MonthID, bool IsSendToClient)
{
	//桌子插入后 我们开始进入
	if (pTable->OnRoleJoinTable(pRoleEx, MonthID, IsSendToClient))
	{
		//玩家进入桌子成功了
		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRoleEx->GetUserID();
		msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2;
		msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = true;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = TableTypeID;
		msgDB.TableMonthID = MonthID;
		g_FishServer.SendNetCmdToSaveDB(&msgDB);

		m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), pTable->GetTableID()));
		if (!pRoleEx->IsRobot())
		{
			if (pTable->GetTableMonthID() == 0)
			{
				g_FishServer.GetRobotManager().OnJoinRobotToTable(pTable);
			}
			else
			{
				AddJJCGameRobot(pTable);
			}
		}
		return true;
	}
	else
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.RoomID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		return false;
	}
}

GameTable * TableManager::GetPlayerJoinTable(BYTE tableTypeID, BYTE MonthID)
{
	if (MonthID == 0) //普通场
	{
		for (auto IterVec = m_TableVec.begin(); IterVec != m_TableVec.end(); ++IterVec)
		{
			if (!(*IterVec))
				continue;
			if ((*IterVec)->GetTableTypeID() == tableTypeID && MonthID == (*IterVec)->GetTableMonthID() && !(*IterVec)->IsFull())
			{
				return *IterVec;
			}
		}
	}
	else //竞技场
	{
		//找一个人最多的场次进去
		auto it = m_JJCGameTables.find(MonthID);
		if (it != m_JJCGameTables.end())
		{
			const vector<JJCGameTables>& jjcTables = it->second;
			BYTE maxTablePlayerSum = 0;
			auto retit = jjcTables.end();
			for (auto tableIt = jjcTables.begin(); tableIt != jjcTables.end(); ++tableIt)
			{
				if ((tableIt->table1 != NULL && tableIt->table1->IsTableRunning())
					|| (tableIt->table2 != NULL && tableIt->table2->IsTableRunning()))
				{
					continue;
				}
				BYTE playerSum = 0;
				if (tableIt->table1 != NULL && tableIt->table1->IsTableRunning() == false)
				{
					playerSum += tableIt->table1->GetTablePlayerSum();
				}
				if (tableIt->table2 != NULL && tableIt->table2->IsTableRunning() == false)
				{
					playerSum += tableIt->table2->GetTablePlayerSum();
				}
				if (playerSum < 8 && playerSum >= maxTablePlayerSum)
				{
					maxTablePlayerSum = playerSum;
					retit = tableIt;
				}
			}
			//如果找到一个竞技场场次
			if (retit != jjcTables.end())
			{
				if (retit->table1 && !retit->table1->IsFull()) return retit->table1;
				if (retit->table2 && !retit->table2->IsFull()) return retit->table2;
			}
		}
	}
	return NULL;
}

bool TableManager::CanPlayerJoinJJC(CRoleEx * pRoleEx, BYTE MonthID)
{
	if (IsJJCOpen())
	{
		BYTE tableTypeID = MonthID & 0x7f;
		const tagFishJJC& jjc_cfg = g_FishServer.GetFishConfig().GetFishJJC();
		auto it = jjc_cfg.jjcTable.find(tableTypeID);
		if (it == jjc_cfg.jjcTable.end()) return false;
		INT64 admission = -1 * it->second.admission * MONEY_RATIO;
		if (pRoleEx->ChangeRoleGlobe(admission, tableTypeID))
		{
			return true;
		}
	}
	return false;
}

bool TableManager::IsJJCOpen()
{
	//判断竞技场开始时间
	const tagFishJJC& jjc_cfg = g_FishServer.GetFishConfig().GetFishJJC();
	const tagJJC_Time& jjc_time_cfg = jjc_cfg.time;
	time_t NowTimeStamp = time(NULL);
	tm cfgTime;
	localtime_s(&cfgTime, &NowTimeStamp);
	for (auto it = jjc_time_cfg.openTime.begin(); it != jjc_time_cfg.openTime.end(); ++it)
	{
		if (it->year != 0) cfgTime.tm_year = it->year - 1900;
		if (it->month != 0) cfgTime.tm_mon = it->month - 1;
		if (it->day != 0) cfgTime.tm_mday = it->day;
		if (it->hour != 0) cfgTime.tm_hour = it->hour;
		if (it->minute != 0) cfgTime.tm_min = it->minute;
		cfgTime.tm_sec = 0;
		time_t cfgTimeStamp = mktime(&cfgTime);
		if (NowTimeStamp >= cfgTimeStamp && NowTimeStamp <= cfgTimeStamp + jjc_time_cfg.remainTime * 60)
		{
			return true;
		}
	}
	return false;
}

void TableManager::AddJJCGameTable(BYTE tableTypeID, BYTE monthTypeID, GameTable * pTable)
{
	//竞技场需要2个桌子
	GameTable* pNewTable = new GameTable();
	if (!pNewTable)
	{
		return;
	}
	pNewTable->OnInit(m_MaxTableID, tableTypeID, monthTypeID);//将桌子初始化
	m_MaxTableID++;
	m_TableVec.push_back(pNewTable);
	auto jjTablesIt = m_JJCGameTables.find(monthTypeID);
	if (jjTablesIt == m_JJCGameTables.end())
	{
		JJCGameTables tables(pTable, pNewTable);
		vector<JJCGameTables> temp;
		temp.push_back(tables);
		m_JJCGameTables.insert(make_pair(monthTypeID, temp));
	}
	else
	{
		vector<JJCGameTables>& jjcTables = jjTablesIt->second;
		JJCGameTables newtables(pTable, pNewTable);
		jjcTables.push_back(newtables);
	}
}

void TableManager::AddJJCGameRobot(GameTable * p1)
{
	if (!p1)
	{
		return;
	}
	//竞技场有2个桌子。先找到另一个桌子
	GameTable* pOther = NULL;
	auto jjcIt = m_JJCGameTables.find(p1->GetTableMonthID());
	if (jjcIt == m_JJCGameTables.end()) return;
	vector<JJCGameTables>& tables = jjcIt->second;
	for (auto tableIt = tables.begin(); tableIt != tables.end(); ++tableIt)
	{
		if (pOther && p1) break;
		if (tableIt->table1 == p1) pOther = tableIt->table2;
		if (tableIt->table2 == p1) pOther = tableIt->table1;
	}
	if (p1 && pOther)
	{
		//竞技场需要增加机器人 5-10s增加一个机器人
		DWORD tick = timeGetTime();
		DWORD lastAddTime1 = 0;
		DWORD lastAddTime2 = 0;
		for (int i = 0; i < 6; ++i)
		{
			DWORD time1 = RandInt() % 5000 + 6000 + lastAddTime1;
			lastAddTime1 = time1;
			g_FishServer.GetRobotManager().AdddWriteRobot(p1->GetTableID(), time1 + tick);
			DWORD time2 = RandInt() % 5000 + 6000 + lastAddTime2;
			lastAddTime2 = time2;
			g_FishServer.GetRobotManager().AdddWriteRobot(pOther->GetTableID(), time2 + tick);
		}
	}
}

void TableManager::JJCRewardRank(GameTable * p1, GameTable * p2, bool isReward)
{
	if (!p1 || !p2) return;
	UINT msgLength = sizeof(msg_ArenaReward) + 8 * sizeof(tagArenaRewardRank);
	msg_ArenaReward* msg = (msg_ArenaReward*) new char[msgLength];
	memset(msg, 0, msgLength);
	for (BYTE i = 0; i < 4; ++i)
	{
		CRole* pRole = p1->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole->IsActionUser()) continue;
		INT64 nJJCScore = pRole->GetJJCScore();
		msg->reward[i].nScore = nJJCScore;
		memcpy_s(msg->reward[i].nickname, MAX_NICKNAME, pRole->GetRoleExInfo()->GetRoleInfo().NickName, MAX_NICKNAME);
		msg->reward[i].rank = 0;
		msg->reward[i].uid = pRole->GetRoleExInfo()->GetRoleInfo().Uid;
	}
	for (BYTE i = 0; i < 4; ++i)
	{
		CRole* pRole = p2->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole->IsActionUser()) continue;
		INT64 nJJCScore = pRole->GetJJCScore();
		msg->reward[i + 4].nScore = nJJCScore;
		memcpy_s(msg->reward[i + 4].nickname, MAX_NICKNAME, pRole->GetRoleExInfo()->GetRoleInfo().NickName, MAX_NICKNAME);
		msg->reward[i + 4].rank = 0;
		msg->reward[i + 4].uid = pRole->GetRoleExInfo()->GetRoleInfo().Uid;
	}
	//计算排行发放奖励
	UINT rank = 1;
	tagArenaRewardRank temp;
	UINT tmpSize = sizeof(tagArenaRewardRank);
	const tagFishJJC& jjc_cfg = g_FishServer.GetFishConfig().GetFishJJC();
	auto it = jjc_cfg.jjcTable.find(p1->GetTableTypeID());
	if (it == jjc_cfg.jjcTable.end()) return;
	const map<BYTE, INT64>& reward = it->second.reward;
	for (UINT i = 0; i < 8; ++i)
	{
		for (UINT j = i + 1; j < 8; ++j)
		{
			if (msg->reward[j].nScore > msg->reward[i].nScore)
			{
				memcpy_s(&temp, tmpSize, &msg->reward[i], tmpSize);
				memcpy_s(&msg->reward[i], tmpSize, &msg->reward[j], tmpSize);
				memcpy_s(&msg->reward[j], tmpSize, &temp, tmpSize);
			}
		}
		//设置排行和发放奖励
		msg->reward[i].rank = rank++;
		if (isReward)
		{
			auto it = reward.find(msg->reward[i].rank);
			if (it != reward.end())
			{
				CRoleEx* pRoleEx = g_FishServer.GetRoleManager()->QuertUserByUid(msg->reward[i].uid);
				if (pRoleEx)
				{
					pRoleEx->ChangeRoleGlobe(it->second*MONEY_RATIO, p1->GetTableMonthID() & 0x7f, 0);
				}
			}
		}
	}
	if (isReward) //结束发放奖励
	{
		SetMsgInfo((*msg), 6042, msgLength);
		p1->SendDataToTableAllUser(msg);
		p2->SendDataToTableAllUser(msg);
	}
	else //只是发送排行信息
	{
		SetMsgInfo((*msg), 6044, msgLength);
		p1->SendDataToTableAllUser(msg);
		p2->SendDataToTableAllUser(msg);
	}
	SAFE_DELETE_ARR(msg);
}
