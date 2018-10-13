#include "Stdafx.h"
#include "Role.h"
#include "RoleEx.h"
//#include "ServiceUnits.h"
#include"FishLogic\Config.h"
#include<timeapi.h>
#include "FishLogic\FishScene\FishResData.h"
#include"FishServer.h"
#include "FishLogic\NetCmd.h"

extern void SendLogDB(NetCmd* pCmd);

CRole::CRole()
{
	m_pTableRolemanager = NULL;
	ResetData();
	m_pConfig = g_FishServer.GetTableManager()->GetGameConfig();
	m_IsSendTableMsg = true;
	m_nTableRate = 0;
	m_dwLockEndTime = 0;
	m_dwKbEndTime = 0;
	m_dwLastFireTime = timeGetTime();
	m_dwLastUseSkillTime = timeGetTime();
	//CTraceService::TraceString(TEXT("一个玩家Role 已经创建"), TraceLevel_Normal);
}
CRole::~CRole()
{
	m_pRoleEx = NULL;
	//CTraceService::TraceString(TEXT("一个玩家Role 已经创建"), TraceLevel_Normal);
}
void CRole::ResetData()
{
	//Log("CRole ResetData");
	m_pRoleEx = NULL;
	m_JJCscore = 0;
	m_BulletIdx = 0;
	m_LauncherType = 0;
	m_nMultipleIndex = 0;
	m_vecLauncherInfo.clear();
	m_dwLastFireTime = timeGetTime();
	m_dwLastUseSkillTime = timeGetTime();
	ClearComb();
	m_byMinRate = 0;
	m_byMaxRate = 0;
	m_skill.Reset();
	m_dwLockEndTime = 0;
	m_dwKbEndTime = 0;
}

bool CRole::OnInit(WORD TableID, BYTE SeatID, CTableRoleManager* pManager)
{
	if (!pManager)
	{
		ASSERT(false);
		return false;
	}
	m_TableID = TableID;
	m_SeatID = SeatID;
	m_pTableRolemanager = pManager;
	m_pRoleEx = NULL;
	m_dwLockEndTime = 0;
	m_dwKbEndTime = 0;
	m_dwLastFireTime = timeGetTime();
	m_dwLastUseSkillTime = timeGetTime();
	return true;
}
void CRole::Clear()
{
	ResetData();
}
void CRole::SetUser(CRoleEx* pUser)
{
	if (!pUser)
	{
		ASSERT(false);
		return;
	}
	m_pRoleEx = pUser;
	//Log("SetUser,user id =%d", pUser->GetUserID());
	m_dwLastFireTime = timeGetTime();
	m_dwLastUseSkillTime = timeGetTime();
	for (int i = 0; i < MAX_LAUNCHER_NUM; i++)
	{
		LauncherInfo item = { 0 };
		m_vecLauncherInfo.push_back(item);
	}
}
DWORD CRole::GetID()
{
	if (!m_pRoleEx)
	{
		//ASSERT(false);
		return 0;
	}
	return m_pRoleEx->GetUserID();
}
char* CRole::GetNickName()
{
	if (!m_pRoleEx)
	{
		ASSERT(false);
		return NULL;
	}
	return m_pRoleEx->GetRoleInfo().NickName;
}
DWORD CRole::GetFaceID()
{
	if (!m_pRoleEx)
	{
		ASSERT(false);
		return 0;
	}
	return m_pRoleEx->GetFaceID();

}
bool CRole::CheckBulletIdx(BYTE& bulletID)
{
	if (!IsActionUser())
		return false;
	bulletID = (m_SeatID << 8) | m_BulletIdx;
	m_BulletIdx++;
	m_BulletIdx %= MAX_BULLET_NUM;
	bulletID = m_BulletIdx;
	return true;
}

void CRole::AddLauncher(bool badd)
{
	if (!m_pConfig)
	{
		ASSERT(false);
		return;
	}
	m_LauncherType = ConvertIntToBYTE((m_LauncherType + (badd ? 1 : -1) + m_pConfig->CannonCount()) % m_pConfig->CannonCount());
}
void CRole::SetLauncher(BYTE LauncherType)
{
	if (LauncherType >= m_vecLauncherInfo.size())
		return;
	m_LauncherType = LauncherType;
}
SkillFailedType CRole::UseSkill(SkillType skill)
{
	if (!m_pConfig || !m_pRoleEx || !m_pTableRolemanager)
	{
		ASSERT(false);
		return SFT_INVALID;
	}

	if (!m_pRoleEx->GetRoleLauncherManager().IsCanUserLauncherByID(m_LauncherType))
	{
		return SFT_UNLOCK;
	}
	int nCannonIndex;
	if (m_pConfig->FindCannon(skill, nCannonIndex) && nCannonIndex != m_LauncherType)
	{
		return SFT_BIND;
	}

	if (!m_skill.IsCoolDown())//cd time
	{
		return SFT_CD;
	}
	int nGoodId;
	int nGoodConsume;
	//int nPrice;

	m_pConfig->GoodsInfo(skill, m_skill.UsedTimes(static_cast<BYTE>(skill)), nGoodId, nGoodConsume);   // xuda
	//机器人使用技能 无须消耗物品
	if (nGoodConsume>0 && !m_pRoleEx->IsRobot())//fail 
	{
		//notify client
		return SFT_COUNT;
	}

	if (skill != SKILL_LOCK)
	{
		for (BYTE i = 0; i < m_pTableRolemanager->GetMaxPlayerSum(); i++)
		{
			CRole *pRole = m_pTableRolemanager->GetRoleBySeatID(i);
			if (pRole)
			{
				pRole->OnSkillUsed(m_pConfig->SkillCdTime(static_cast<BYTE>(skill)));
			}
		}
		m_skill.RecordSkill(static_cast<BYTE>(skill));		
	}
	return SFT_OK;
}

LaunchFailedType CRole::UseLaser(byte launcherType)
{
	if (!m_skill.IsCoolDown())//cd time
	{
		return LFT_CD;
	}
	ASSERT(m_LauncherType<m_vecLauncherInfo.size());

	if (!m_pConfig)
	{
		ASSERT(false);
		return LFT_INVALID;
	}
	LauncherInfo&  launcher = m_vecLauncherInfo[m_LauncherType];//launcherType==m_LauncherType

	if (launcher.nEnergy < m_pConfig->LaserThreshold(launcherType)*m_pConfig->BulletMultiple(m_nMultipleIndex))//*rate
	{
		return LFT_ENERGY;
	}

	launcher.nEnergy = 0;
	for (BYTE i = 0; i < m_pTableRolemanager->GetMaxPlayerSum(); i++)
	{
		CRole *pRole = m_pTableRolemanager->GetRoleBySeatID(i);
		if (pRole)
		{
			pRole->OnSkillUsed(m_pConfig->LaserCdTime(launcherType));
		}
	}
	return LFT_OK;
}

void CRole::OnCatchFish(CatchType catchType, byte subType,WORD FishType, int nScore)
{
	if (!m_pRoleEx)
	{
		ASSERT(false);
		return;
	}
	auto tableRateit = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(GetTableType());
	double tableBasicRate = 0.0;
	if (tableRateit != g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		tableBasicRate = tableRateit->second.BasicRatio;
	}
	int64 catchScore = nScore*static_cast<int64>(MONEY_RATIO*g_FishServer.GetRatioValue()*tableBasicRate);
	
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(m_TableID);
	if (pTable == NULL || !pTable->IsTableRunning())
	{
		return;
	}
	if (pTable->GetTableMonthID() != 0)
	{
		m_JJCscore += catchScore;
	}
	else
	{
		m_pRoleEx->ChangeRoleGlobe(catchScore, m_TableType, FishType);
		if (!m_pRoleEx->IsRobot())
		{
			g_FishServer.GetTableManager()->OnChangeTableGlobel(GetTableID(), -catchScore, TableRate());
		}
		m_pRoleEx->ChangeRoleTotalFishGlobelSum(catchScore);
	}
}
bool CRole::IsFullEnergy()
{
	return false;
	ASSERT(m_LauncherType<m_vecLauncherInfo.size());
	ASSERT(m_pConfig != null);
	ASSERT(m_pRoleEx != null);
	LauncherInfo&  launcher = m_vecLauncherInfo[m_LauncherType]; //暂时先屏蔽掉 xuda  后期开启
	if (launcher.nEnergy >= m_pConfig->LaserThreshold(m_LauncherType)*m_pConfig->BulletMultiple(m_nMultipleIndex))// *rate 
		return true;
	else
		return false;
}
bool CRole::CheckFire(BYTE byLauncher)
{
	ASSERT(m_LauncherType<m_vecLauncherInfo.size());
	ASSERT(m_pConfig != null);
	ASSERT(m_pRoleEx != null);

	if (!m_pRoleEx->GetRoleLauncherManager().IsCanUserLauncherByID(m_LauncherType))
	{
		//Log(L"大炮未购买:%s, 炮台:%d", m_pRoleEx->GetRoleInfo().NickName, m_LauncherType);
		return false;
	}

	if (!m_pRoleEx->GetRoleRate().IsCanUseRateIndex(m_nMultipleIndex))//当前倍率不可以使用
	{
		return false;
	}

	if (timeGetTime() - m_dwLastFireTime<m_pConfig->LauncherInterval(m_LauncherType)*1000)//speed up
	{
		//Log(L"大炮发射间隔:%s, 炮台:%d", m_pRoleEx->GetRoleInfo().NickName, m_LauncherType);
		return false;
	}
	//游戏桌子是否运行
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(m_TableID);
	if (pTable == NULL || !pTable->IsTableRunning())
	{
		return false;
	}
	if (pTable->GetTableMonthID() != 0)
	{
		//校验竞技场子弹和倍率
		if (m_LauncherType != 0) return false;
		if (m_bulletCount <= 0) return false;
		if (m_bulletCount > 400 && m_bulletCount <= 600 && m_nMultipleIndex != 2) return false;
		if (m_bulletCount > 200 && m_bulletCount <= 400 && m_nMultipleIndex != 5) return false;
		if (m_bulletCount > 0 && m_bulletCount <= 200 && m_nMultipleIndex != 8) return false;
		m_bulletCount--;
	}
	else
	{
		auto it = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(m_TableType);
		double tableBasicRate = 0.00;
		if (it != g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
		{
			tableBasicRate = it->second.BasicRatio;
		}
		int nConsume = (int)(m_pConfig->BulletConsume(m_LauncherType)	 // 每个炮的基础消耗
			*m_pConfig->BulletMultiple(m_nMultipleIndex) //玩家调整的倍率
			*g_FishServer.GetRatioValue()				 // 大厅的货币比例
			*tableBasicRate					 // 桌子的基础倍率 
			*MONEY_RATIO					// 游戏服务器处理货币的倍率值
			* (IsInKbState() ? KB_RATIO : 1) //是否在狂暴状态
			);
		if (!m_pRoleEx->ChangeRoleGlobe(nConsume*-1, m_TableType))
		{
			//Log(L"大炮金币不够:%s, 炮台:%d", m_pRoleEx->GetRoleInfo().NickName, m_LauncherType);
			return false;
		}
		if (!m_pRoleEx->IsRobot())
		{
			g_FishServer.GetTableManager()->OnChangeTableGlobel(GetTableID(), nConsume, TableRate());
		}
		//成就 终极炮台
		BYTE all_tables = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.size();
		if (m_TableType == all_tables - 1 && m_nMultipleIndex == m_pConfig->RateCount() - 1)
		{
			m_pRoleEx->GetRoleGameData().OnPlayerUseMaxRate();
		}
		//end
	}
	m_dwLastFireTime = timeGetTime();
	return true;
}

int64 CRole::GetScore()
{
	if (!m_pRoleEx)
	{
		return 0;
	}
	return m_pRoleEx->GetScore();
}

USHORT CRole::Combo(WORD wCounter)
{
	if (!m_pConfig || !m_pTableRolemanager)
	{
		ASSERT(false);
		return 0;
	}
	for (size_t i = 0; i < m_Combo.m_vecBulletid.size(); i++)
	{
		if (m_Combo.m_vecBulletid[i] == wCounter)
		{
			return 0;
		}
	}
	m_Combo.m_dwEndTime = timeGetTime() + ConvertDoubleToDWORD(m_pConfig->ComboSustainTime() * 1000.0);
	m_Combo.m_vecBulletid.push_back(wCounter);
	if (m_Combo.m_vecBulletid.size() >= 2)
	{
		byte byHigh = (m_Combo.m_vecBulletid.size()-1) % m_pConfig->ComboBuffCycle() == 0;
		byte byLow = ConvertDWORDToBYTE(m_Combo.m_vecBulletid.size() - 1);
		return (byHigh << 15) | (byLow);		
	}
	return 0;
}

void CRole::OnSkillUsed(float nCdtime)
{
	m_skill.EmitSkill(nCdtime);
}
bool CRole::IsNeedLeaveTable()
{
	if (m_pRoleEx->IsRobot())//机器人不踢
		return false;
	DWORD LeaveTableTime = g_FishServer.GetFishConfig().GetSystemConfig().LeaveTableNonLauncherMin * 60000;
	DWORD NowTime = timeGetTime();
	if (NowTime - m_dwLastUseSkillTime >= LeaveTableTime && NowTime - m_dwLastFireTime >= LeaveTableTime)
	{
		//破产情况下不踢
		return true;
	}
	else
		return false;
}
void CRole::SetBulletRate(USHORT uPoolRate,BYTE byMin, BYTE byMax)
{
	if (!m_pConfig)
	{
		ASSERT(false);
		return;
	}
	if (byMin > byMax)
	{
		return;
	}
	if (byMin >= m_pConfig->RateCount() || byMax >= m_pConfig->RateCount())
	{
		return;
	}
	m_nMultipleIndex = byMin;
	m_byMinRate = byMin;
	m_byMaxRate = byMax;
	m_nTableRate = uPoolRate;
	
	//重新设置玩家的倍率为当前桌子 和 玩家可以承受的最大值
	//if(m_pRoleEx->GetRoleMonth().IsInMonthTable())
	//	m_nMultipleIndex = max(GetRoleExInfo()->GetRoleRate().GetCanUseMinRate(), m_byMinRate);
	//else
	//	m_nMultipleIndex  = min(GetRoleExInfo()->GetRoleRate().GetCanUseMaxRate(), m_byMaxRate);
}
void CRole::SetRoomLauncher()
{
	//切换到玩家可以使用的最高的炮
	for (BYTE i = MAX_LAUNCHER_NUM - 1; i >= 0; --i)
	{
		if (i == 0)
		{
			break;
		}
		if (GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(i))
		{
			m_LauncherType = i;
			return;
		}
	}
	m_LauncherType = 0;
	return;
}
void CRole::SetRoomLauncher(BYTE LancherType)
{
	if (GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(LancherType))
	{
		m_LauncherType = LancherType;
		return;
	}
	m_LauncherType = 0;
	return;
}
void CRole::ClearComb()
{
	m_Combo.m_ComboBulletID = MAX_BULLET_NUM;
	m_Combo.m_dwEndTime = 0;
	m_Combo.m_vecBulletid.clear();
	m_Combo.m_crit=0;
	
}

ushort CRole::SkillInfo(SkillType skill)
{
	if (skill < SKILL_MAX)
	{		
		return m_skill.UsedTimes(static_cast<BYTE>(skill));
	}
	return 0;
}
void CRole::OnResetRoleLauncher()
{
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(m_TableID);
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	pTable->GetFishDesk()->ResetLauncher(GetID());
}

void CRole::DelRoleLuckValue(DWORD Value)
{
	//if (m_LuckyValue < Value)
	//	m_LuckyValue = 0;
	//else
	//	m_LuckyValue -= Value;	
}

void CRole::AddRoleLuckValue(DWORD Value)
{
	//if (m_LuckyValue != 0)//触过底了
	//{
	//	m_LuckyValue += Value;
	//}
}

USHORT CRole::TableRate()
{
	return m_nTableRate;
}

void CRole::SetRateIndex(byte idex)
{	
	if (idex<m_byMinRate)
	{
		idex = m_byMinRate;
	}
	if (idex > m_byMaxRate)
	{
		idex = m_byMaxRate;
	}
	m_nMultipleIndex = idex;
	ClearComb();
}

void CRole::ChangeRateIndex(byte idex)
{
	m_nMultipleIndex++;
	if (m_nMultipleIndex > m_byMaxRate)
	{
		m_nMultipleIndex = m_byMinRate;
	}
	ClearComb();
}
void CRole::OnChangeRateToIndex(bool UpOrDown,bool& IsCanUse)
{
	IsCanUse = true;
	BYTE RoleRateMin = max(GetRoleExInfo()->GetRoleRate().GetCanUseMinRate(), m_byMinRate);
	BYTE RoleRateMax = min(GetRoleExInfo()->GetRoleRate().GetCanUseMaxRate(), m_byMaxRate);
	//获得范围后 我们进行处理
	if (UpOrDown)
		m_nMultipleIndex += 1;
	else
		m_nMultipleIndex -= 1;
	if (m_nMultipleIndex > RoleRateMax || m_nMultipleIndex < RoleRateMin)
	{
		if (UpOrDown)
			m_nMultipleIndex = RoleRateMin;
		else
			m_nMultipleIndex = RoleRateMax;
	}
	ClearComb();
	IsCanUse = GetRoleExInfo()->GetRoleRate().IsCanUseRateIndex(m_nMultipleIndex);
}
void CRole::OnChangeRateToIndex(byte RateIndex, bool& IsCanUse)
{
	IsCanUse = true;
	//玩家需要进行选择倍率 
	//1.获得玩家倍率 的 上限 和下限
	BYTE RoleRateMin = max(GetRoleExInfo()->GetRoleRate().GetCanUseMinRate(), m_byMinRate);
	BYTE RoleRateMax = min(GetRoleExInfo()->GetRoleRate().GetCanUseMaxRate(), m_byMaxRate);
	//获得范围后 我们进行处理
	if (RateIndex <= RoleRateMax && RateIndex >= RoleRateMin)
	{
		m_nMultipleIndex = RateIndex;
		ClearComb();
	}
	IsCanUse = GetRoleExInfo()->GetRoleRate().IsCanUseRateIndex(m_nMultipleIndex);
}
void CRole::SetKbEndTime(DWORD addTime)
{
	m_dwKbEndTime = timeGetTime() + addTime;
}
bool CRole::IsInKbState()
{
	return timeGetTime() > m_dwKbEndTime ? false : true;
}
void  CRole::SetLockEndTime()
{ 
	DWORD NowTime = timeGetTime();
	m_dwLockEndTime = (NowTime + m_pConfig->GetSkillCDTime(SkillType::SKILL_LOCK) * 1000);
}
bool CRole::IsCanLock()
{ 
	DWORD NowTime = timeGetTime();
	if (m_dwLockEndTime == 0 || m_dwLockEndTime < NowTime)
	{ 
		m_dwLockEndTime = 0; 
		return true;
	} 
	else
	{
		return false; 
	} 
}
ushort CRole::MinBulletCosume()
{
	return 	m_pConfig->BulletConsume(m_LauncherType)*m_pConfig->BulletMultiple(m_byMinRate);//最小消耗的子弹
}

USHORT CRole::CombCount()
{
	return m_Combo.m_vecBulletid.size();
}

void CRole::DelayCombo()
{
	m_Combo.m_dwEndTime += 6500;
}
int CRole::GetTableRateRank()
{
	return m_pTableRolemanager->GetRoleRateRank(GetID());
}

void CRole::SaveBattleRecord(BYTE model, BYTE leaveCode)
{
	//modle 0, 进入游戏，1，同步数据，2退出游戏
	//leaveCode 0:游戏中、1：超时提出 2：正常退出
	if (m_pRoleEx == null)
	{
		return;
	}
	//保存玩家游戏记录
	DBR_Cmd_SaveRecord recordMsg;
	SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
	recordMsg.model = model;
	recordMsg.uid = m_pRoleEx->GetRoleInfo().Uid;
	recordMsg.table_id = m_TableType;
	recordMsg.enter_money = (m_pRoleEx->GetRoleInfo().money1 + m_pRoleEx->GetRoleInfo().money2);
	recordMsg.leave_money = (m_pRoleEx->GetRoleInfo().money1 + m_pRoleEx->GetRoleInfo().money2);
	recordMsg.leave_code = leaveCode;
	g_FishServer.SendNetCmdToDB(&recordMsg);
}

CTableRoleManager::CTableRoleManager()
{

}
CTableRoleManager::~CTableRoleManager()
{
	Destroy();
}
void CTableRoleManager::OnInit(WORD TableID, BYTE TableMaxPlayerSum)
{
	if (TableMaxPlayerSum == 0)
	{
		ASSERT(false);
		return;
	}
	m_MaxTableUserSum = TableMaxPlayerSum;
	m_TableRoleArray = new CRole[m_MaxTableUserSum];
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return;
	}
	//CTraceService::TraceString(TEXT("一个桌子玩家数组 已经创建"), TraceLevel_Normal);
	m_TableID = TableID;
	for (BYTE i = 0; i < m_MaxTableUserSum; ++i)
	{
		m_TableRoleArray[i].OnInit(m_TableID, i,this);
	}
}
void CTableRoleManager::Destroy()
{
	//CTraceService::TraceString(TEXT("一个桌子玩家数组 已经销毁"), TraceLevel_Normal);
	SAFE_DELETE_ARR(m_TableRoleArray);
	m_TableRoleArray = NULL;
}
bool CTableRoleManager::IsAllUserHaveBullets()
{
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (!m_TableRoleArray[i].IsActionUser())
			continue;
		if (m_TableRoleArray[i].GetBulletCount() != 0)
		{
			return false;
		}
	}
	return true;
}
int CTableRoleManager::GetRoleSum()
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return 0;
	}
	int Count = 0;
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser())
			++Count;
	}
	return Count;
}
void CTableRoleManager::OnDelAllRole()
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return;
	}
	for (BYTE i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser())
		{
			OnDleRole(i);
		}
	}
}
bool CTableRoleManager::OnDelRole(PlayerID RoleID)
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return false;
	}
	for (BYTE i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser() && m_TableRoleArray[i].GetID() == RoleID)
		{
			OnDleRole(i);
			return true;
		}
	}
	return false;
}
bool CTableRoleManager::OnDleRole(WORD ChairID)
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return false;
	}
	if (ChairID >= m_MaxTableUserSum)
		return false;
	m_TableRoleArray[ChairID].Clear();
	//CTraceService::TraceString(TEXT("一个玩家已经离开了桌子"), TraceLevel_Normal);
	return true;
}
bool CTableRoleManager::OnInitRole(CRoleEx* pEx)
{
	//网桌子里添加一位玩家
	if (!m_TableRoleArray || !pEx) 
	{
		ASSERT(false);
		return false;
	}
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (!m_TableRoleArray[i].IsActionUser())
		{
			m_TableRoleArray[i].SetUser(pEx);
			//Log("一个玩家已经进入桌子 user id =%d",pEx->GetUserID());
			return true;
		}
	}
	return false;
}
CRole* CTableRoleManager::GetRoleBySeatID(BYTE ChairID)
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return NULL;
	}
	if (ChairID >= m_MaxTableUserSum)
		return NULL;
	else
		return &m_TableRoleArray[ChairID];
}
CRole* CTableRoleManager::GetRoleByUserID(PlayerID RoleID)
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return NULL;
	}
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].GetID() == RoleID &&  m_TableRoleArray[i].IsActionUser())
			return &m_TableRoleArray[i];
	}
	return NULL;
}
void CTableRoleManager::SwitchFishTide()
{
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser())
			m_TableRoleArray[i].DelayCombo();
	}
}

int CTableRoleManager::GetRoleRateRank(PlayerID RoleID)
{
	if (!m_TableRoleArray)
	{
		ASSERT(false);
		return 0;
	}
	int myRate = 0;
	list<int> tempRate;
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser())
		{
			int rate = m_TableRoleArray[i].GetRateIndex();
			tempRate.push_back(rate);
			if (m_TableRoleArray[i].GetID() == RoleID)
			{
				myRate = rate;
			}
		}
	}
	tempRate.sort();
	int rank = 1;
	for (list<int>::iterator it = tempRate.begin(); it != tempRate.end(); ++it, ++rank)
	{
		if (*it == myRate)
		{
			return rank;
		}
	}
	return 0;
}
 
const int64 CTableRoleManager::GetPlayerAvgMoney()
{
	int64 totalMoney = 0;
	UINT nPlayers = 0;
	for (int i = 0; i < m_MaxTableUserSum; ++i)
	{
		if (m_TableRoleArray[i].IsActionUser() && !m_TableRoleArray[i].GetRoleExInfo()->IsRobot())
		{
			nPlayers++;
			totalMoney += m_TableRoleArray[i].GetRoleExInfo()->GetRoleInfo().money1 
				+ m_TableRoleArray[i].GetRoleExInfo()->GetRoleInfo().money2;
		}
	}
	if (nPlayers == 0 || totalMoney == 0) return 0;
	return totalMoney / nPlayers;
}