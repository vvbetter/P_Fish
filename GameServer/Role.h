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
	struct LauncherInfo //����Ϣ
	{
		int nEnergy;
		//bool nEnable;
	};
	typedef std::vector<LauncherInfo>VecLauncherInfo;

public:
	CRole();
	virtual ~CRole();

	bool		OnInit(WORD TableID, BYTE SeatID, CTableRoleManager* pManager);//��ʼ�������ϵ���ҵ�����
	bool		IsActionUser() { return m_pRoleEx != NULL; }//�жϵ�ǰλ���Ƿ�Ϊ�յ�
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
	void SetKbEndTime(DWORD addTime /*����*/);
	bool IsInKbState();
	bool IsCanLock();
	ushort MinBulletCosume();	
	USHORT CombCount();
	void DelayCombo();
	void SetTableType(const BYTE table_type){ m_TableType = table_type; }
	const BYTE GetTableType(){ return m_TableType; }
	int GetTableRateRank();
private:
	WORD		m_TableID;//���Ӻ�
	BYTE		m_SeatID;//λ�ú�
	BYTE		m_TableType; //���ӵ�����ID���������м�������
	CRoleEx*	m_pRoleEx;//�ⲿ����ҵ���
	byte		m_BulletIdx;//�ӵ� 
	byte		m_LauncherType;//��̨
	byte        m_nMultipleIndex;//
	UINT		m_bulletCount; //�������ӵ�����
	INT64		m_JJCscore;// ����������
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
	DWORD						m_dwLockEndTime;//��������ʱ��
	DWORD						m_dwKbEndTime; //ʹ�ÿ����ڽ���ʱ��
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
	int			GetRoleSum();//��ȡ�û����� ��ȡ������û�����
	bool		OnDelRole(PlayerID RoleID);
	bool		OnDleRole(WORD ChairID);
	void		OnDelAllRole();
	bool		OnInitRole(CRoleEx* pEx);
	BYTE		GetMaxPlayerSum(){ return m_MaxTableUserSum; }
	CRole*		GetRoleBySeatID(BYTE ChairID);
	CRole*		GetRoleByUserID(PlayerID RoleID);
	int			GetRoleRateRank(PlayerID RoleID);
	//���������л�����
	void		SwitchFishTide();
	const int64 GetPlayerAvgMoney();
private:
	WORD			m_TableID;
	BYTE			m_MaxTableUserSum;
	CRole*			m_TableRoleArray;
};

