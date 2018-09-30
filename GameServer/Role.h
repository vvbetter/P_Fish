#pragma once
#include "Stdafx.h"
#include"SkillInfo.h"
#include "FishLogic\FishUtility.h"
#include "FishLogic\FishCallbak.h"
class CTableRoleManager;
class CRoleEx;
class CConfig;

class CRole
{
	struct ComboInfo
	{
		USHORT m_ComboBulletID;
		USHORT m_crit;
		DWORD m_dwEndTime;
		std::vector<WORD>	m_vecBulletid;
	};
	struct LauncherInfo //炮信息
	{
		int nEnergy;
		//bool nEnable;
	};
	typedef std::vector<LauncherInfo>VecLauncherInfo;

public:
	CRole();
	virtual ~CRole();

	bool		OnInit(WORD TableID, BYTE SeatID, CTableRoleManager* pManager);//初始化桌子上的玩家的数据
	bool		IsActionUser() { return m_pRoleEx != NULL; }//判断当前位置是否为空的
	void		Clear();
	void		SetUser(CRoleEx* pUser);

	WORD		GetTableID(){ return m_TableID; }
	BYTE		GetSeatID(){ return m_SeatID; }
	UINT		GetBulletCount() { return m_bulletCount; }
	void		SetBulletCount(const UINT n) { m_bulletCount = n; }
	INT64        GetJJCScore() { return m_JJCscore; }
	void		SetJJCScore(const INT64 n) { m_JJCscore = n; }

	DWORD		GetID();
	char*		GetNickName();
	DWORD		GetFaceID();
	CRoleEx*    GetRoleExInfo(){ return m_pRoleEx; }
	bool CheckBulletIdx(BYTE &bulletID);

	byte GetLauncherType()const
	{
		return m_LauncherType;
	}
	byte GetRateIndex()
	{
		return m_nMultipleIndex;
	}
	void AddLauncher(bool badd);
	void SetLauncher(BYTE LauncherType);
	void ResetData();

	void OnCatchFish(CatchType catchType, byte subType,WORD FishType, int nScore);
	bool CheckFire(BYTE byLauncher);
	bool IsFullEnergy();
	SkillFailedType UseSkill(SkillType skill);
	LaunchFailedType UseLaser(byte launcherType);
	int64 GetScore();
	USHORT Combo(WORD wCounter);
	void OnSkillUsed(float nCdtime);
	void SetBulletRate(USHORT uPoolRate,BYTE byMin, BYTE byMax);
	void SetRoomLauncher();
	void SetRoomLauncher(BYTE LancherType);

	BYTE GetBulletIdx(){ return m_BulletIdx; }
	ushort SkillInfo(SkillType skill);
	void SetRateIndex(byte idex);
	void ChangeRateIndex(byte idex);
	void OnChangeRateToIndex(bool UpOrDown, bool& IsCanUse);
	void OnChangeRateToIndex(byte RateIndex, bool& IsCanUse);
	int GetEnergy()
	{
		return m_vecLauncherInfo[m_LauncherType].nEnergy;
	}

	bool IsCanSendTableMsg(){ return m_IsSendTableMsg; }
	void SetRoleIsCanSendTableMsg(bool States){ m_IsSendTableMsg = States; }

	void OnResetRoleLauncher();
	void DelRoleLuckValue(DWORD Value);
	void AddRoleLuckValue(DWORD Value);

	USHORT TableRate();
	void ClearComb();

	bool IsNeedLeaveTable();
	void SetLockEndTime();
	void SetKbEndTime(DWORD addTime /*毫秒*/);
	bool IsInKbState();
	bool IsCanLock();
	ushort MinBulletCosume();	
	USHORT CombCount();
	void DelayCombo();
	void SetTableType(const BYTE table_type){ m_TableType = table_type; }
	const BYTE GetTableType(){ return m_TableType; }
	int GetTableRateRank();
private:
	WORD		m_TableID;//桌子号
	BYTE		m_SeatID;//位置号
	BYTE		m_TableType; //桌子的类型ID。初级、中级。。。
	CRoleEx*	m_pRoleEx;//外部的玩家的类
	byte		m_BulletIdx;//子弹 
	byte		m_LauncherType;//炮台
	byte        m_nMultipleIndex;//
	UINT		m_bulletCount; //竞技场子弹数量
	INT64		m_JJCscore;// 竞技场积分
	VecLauncherInfo				  m_vecLauncherInfo;
	DWORD						m_dwLastFireTime;
	BYTE						m_byMinRate;
	BYTE						m_byMaxRate;
	USHORT						m_nTableRate;
	CSkillInfo                  m_skill;
	ComboInfo                   m_Combo;
	CConfig                     *m_pConfig;
	CTableRoleManager			*m_pTableRolemanager;
	bool						m_IsSendTableMsg;
	DWORD						m_dwLastUseSkillTime;
	DWORD						m_dwLockEndTime;//锁定结束时间
	DWORD						m_dwKbEndTime; //使用狂暴鱼炮结束时间
};
class CTableRoleManager
{
public:
	CTableRoleManager();
	virtual ~CTableRoleManager();

	void		Destroy();
	void		OnInit(WORD TableID, BYTE TableMaxPlayerSum);

	bool		IsFull(){ return GetRoleSum() >= GetMaxPlayerSum(); }
	bool        IsAllUserHaveBullets();
	int			GetRoleSum();//获取用户数量 获取激活的用户数量
	bool		OnDelRole(PlayerID RoleID);
	bool		OnDleRole(WORD ChairID);
	void		OnDelAllRole();
	bool		OnInitRole(CRoleEx* pEx);
	BYTE		GetMaxPlayerSum(){ return m_MaxTableUserSum; }
	CRole*		GetRoleBySeatID(BYTE ChairID);
	CRole*		GetRoleByUserID(PlayerID RoleID);
	int			GetRoleRateRank(PlayerID RoleID);
	//桌子正在切换鱼阵
	void		SwitchFishTide();
	const int64 GetPlayerAvgMoney();
private:
	WORD			m_TableID;
	BYTE			m_MaxTableUserSum;
	CRole*			m_TableRoleArray;
};

