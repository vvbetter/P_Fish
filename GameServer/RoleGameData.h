//玩家在游戏中的 一些数据的统计
//玩家参加比赛 获得名次的次数
//玩家参加比赛 获得前三名的次数 (分别123)
//玩家捕获特殊鱼 的次数 (特殊鱼)
//玩家获得金币的总数量
//玩家开炮消耗金币的数量等数据统计
#pragma once
#include "Stdafx.h"
class CRoleEx;
struct tagRoleGameData;
class RoleGameData
{
public:
	RoleGameData();
	virtual ~RoleGameData();

	bool OnInit(CRoleEx* pRole,const tagRoleGameData& gameData);
	tagRoleGameData& GetGameData(){ return m_RoleGameData; }
	const int64 GetTurnLoseWinGoldByTable(const BYTE TableTypeID);
	void UpdateMinWinLose();					//统计一分钟内的输赢
	void ChangeGameGold(const INT64 gold);	//每次输赢统计金币
	void OnPlayerCatchFish(const USHORT FishType); //处理击杀鱼事件

	const INT32 GetWinNum(){ return m_winNum; }
	const INT32 GetLoseNum(){ return m_loseNum; }
	const INT64 GetTotalWinGold(){ return m_TotalWinGold; }
	const int64 GetTotalLoseGold(){ return m_TotalLoseGold; }
	const INT32 GetBossFishCount(){ return m_BossFishCount; }
private:
	void OnFishEvent(tagClientUserData* udata, INT eventType);

	void OnCatchFihs_25(tagClientUserData* udata); //击杀电鳗
	void OnCatchFish_1_3_19(tagClientUserData* udata);//击杀BOSS鱼
private:
	CRoleEx*				m_pRole;
	tagRoleGameData			m_RoleGameData;
	INT64					m_lastMinWinGold;//上一分钟输赢金币
	INT64					m_lastMinLoseGold;//上一分钟输金币
	INT32					m_winNum;  //统计每分钟的输赢，退出时返回输赢次数
	INT32					m_loseNum; //统计每分钟的输赢，退出时返回输赢次数 
	INT64					m_TotalWinGold;	//玩家一局游戏中总获得金币
	INT64					m_TotalLoseGold; //玩家一局游戏中总输掉金币

	INT32					m_BossFishCount; //击杀的BOSS鱼数量
};