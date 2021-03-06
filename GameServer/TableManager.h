//当前游戏的核心类 
//一个桌子代表 几个玩家的一场游戏
//桌子在房间开始的时候就已经创建玩家 并且设置好了相关数据
#pragma once
#include "Stdafx.h"
#include".\FishLogic\fishdesk.h"
#include "GameTable.h"
#include "RoleManager.h"
#include "RoleEx.h"
#include"FishLogic\Config.h"
#include "Role.h"

struct  GoldPool
{
	GoldPool(int64 ngold)
	{
		gold = ngold;
		open = false;
		byGive = 0;
	}
	int64 gold;
	bool open;
	byte byGive;
};

struct TableTotalGold
{
	int64 total_produce;
	int64 total_earn;
	TableTotalGold()
	{
		total_produce = 0;
		total_earn = 0;
	}
};

struct SysYieldControl //系统收益控制
{
	int64 TotalCost;	//当天玩家总花费
	int64 TotalProduce;	//当天系统总产出
	int64 lastMinCost;	//上一分钟玩家总花费
	int64 lastMinProduce;	//上一分钟系统总产出
	SysYieldControl()
	{
		TotalCost = 0;
		TotalProduce = 0;
		lastMinCost = 0;
		lastMinProduce = 0;
	}
};

struct JJCGameTables
{
	GameTable* table1;
	GameTable* table2;
	JJCGameTables(GameTable* t1 = NULL, GameTable* t2 = NULL) :table1(t1), table2(t2) {}
};

class TableManager //管理桌子 需要一个桌子的配置文件控制器 网络命令与更新器的线程冲突
{
	//桌子的管理器
public:
	TableManager();
	virtual ~TableManager();

	void OnInit();//初始化
	void Destroy();
	void OnStopService();
	void Update(DWORD dwTimeStep);
	void UpdateMinutes();
	void UpdateJJC(DWORD dwTimeStep); //竞技场更新
	//
	bool OnHandleTableMsg(DWORD dwUserID, NetCmd* pData);
	bool OnPlayerJoinTable(BYTE TableTypeID, CRoleEx* pRoleEx, BYTE MonthID = 0, bool IsSendToClient = true);
	bool OnPlayerJoinTable(WORD TableID, CRoleEx* pRoleEx, bool IsSendToClient = true);
	void OnPlayerLeaveTable(DWORD dwUserID);
	//发送桌子消息
	void SendDataToTable(DWORD dwUserID, NetCmd* pData);
	CRole* SearchUser(DWORD dwUserid);
	CConfig *GetGameConfig();
	GameTable* GetTable(WORD TableID);
	DWORD GetTableSum() { return m_TableVec.size(); }
	void OnChangeTableGlobel(WORD TableID, int64 AddGlobel, USHORT uTableRate);
	void OnResetTableGlobel(WORD TableID, int64 nValue);
	int64  GetTableGlobel(WORD TableID);
	bool QueryPool(WORD TableID, int64 & nPoolGold);
	void QueryPool(BYTE TableTypeID, bool &bopen, int64&nPoolGold);
	CRole* QueryRoleByRoleEx(CRoleEx* pRoleEx);
	void CostAndProduceMin(); //每分钟记录系统收支日志
	void NewDayForTable();//每天重置
	const vector<float>& SysProduceRate(const BYTE tableType);//系统收益比率
	bool InitTableTotalGold(NetCmd* pData);
private:
	//
	bool PlayerJoinTable(GameTable* pTable, CRoleEx* pRoleEx,BYTE tableTypeID, BYTE MonthID, bool IsSendToClient = true);
	GameTable* GetPlayerJoinTable(BYTE tableTypeID, BYTE MonthID);
	//竞技场消息
	bool CanPlayerJoinJJC(CRoleEx* pRoleEx, BYTE MonthID);
	bool IsJJCOpen();
	void AddJJCGameTable(BYTE tableTypeID, BYTE monthTypeID, GameTable* pTable);
	void AddJJCGameRobot(GameTable* p1);
	void JJCRewardRank(GameTable* p1, GameTable* p2,bool isReward = false/*是否发放奖励*/);
private:
	DWORD									m_LastUpdate;
	WORD						 			m_MaxTableID;
	//桌子具体的对象处理
	HashMap<DWORD, WORD>					m_RoleTableMap;
	std::vector<GameTable*>					m_TableVec;
	CConfig                     *m_pGameConfig;
	CFishTimer					m_JJCTimer;// 竞技场计时器

	bool						m_bUpdateTime;
	HashMap<BYTE, vector<JJCGameTables> > m_JJCGameTables; //竞技场桌子类型-竞技场桌子
	HashMap<BYTE, GoldPool>		m_TableGlobeMap;//桌子类型的金币池
	map<BYTE, SysYieldControl> m_sysMinutes;//每个桌子每天的金币记录
	map<BYTE, TableTotalGold> m_sysTotal;//分桌子统计玩家消耗金币和系统产出金币
};