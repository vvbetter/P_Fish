//����������ϵı��������� ��������ȫ��GameServer�ϵ�ȫ������
//�����Ĺ�����  �Ա������й��� ��ҵı��� ��ҵļ��� �Ȳ���
#pragma once
#include "Stdafx.h"
#include <vector>
struct tagMonthConfig;
struct LastMonthRewardInfo
{
	DWORD				LoadSum;
	DWORD				m_NowMonthTimeID;
	//EndTime
	SYSTEMTIME pTime;
	//�ϴα����ļ�¼
	std::vector<tagRoleMonthCenterInfo*>		Lastm_RoleVec;//�����ּ�������
	HashMap<DWORD, tagRoleMonthCenterInfo*>		Lastm_MonthRoleMap;//��ҵļ���
	LastMonthRewardInfo()
	{
		LoadSum = 0;
		m_NowMonthTimeID = 0;
		Lastm_RoleVec.clear();
		Lastm_MonthRoleMap.clear();
	}
	void OnAddInfo(DWORD TimeID,std::vector<tagRoleMonthCenterInfo*>& pVec)
	{
		m_NowMonthTimeID = TimeID;
		GetLocalTime(&pTime);
		LoadSum = 0;
		if (pVec.empty())
			return;
		std::vector<tagRoleMonthCenterInfo*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			if (!(*Iter))
				continue;
			Lastm_RoleVec.push_back(*Iter);
			Lastm_MonthRoleMap.insert(HashMap<DWORD, tagRoleMonthCenterInfo*>::value_type((*Iter)->MonthInfo.dwUserID, *Iter));
		}
	}
	void UpdateRoleInfo(DWORD dwUserID, DWORD Point, DWORD Globel, DWORD SkillSum)
	{
		HashMap<DWORD, tagRoleMonthCenterInfo*>::iterator Iter = Lastm_MonthRoleMap.find(dwUserID);
		if (Iter == Lastm_MonthRoleMap.end() || !Iter->second)
		{
			ASSERT(false);
			return;
		}
		Iter->second->MonthInfo.dwMonthGlobel = Globel;
		Iter->second->MonthInfo.dwMonthScore = Point;
		Iter->second->MonthInfo.SkillSum = SkillSum;
	}
	bool OnAddGameServerFinish();
	void OnDestroy()
	{
		if (Lastm_RoleVec.empty())
			return;
		std::vector<tagRoleMonthCenterInfo*>::iterator Iter = Lastm_RoleVec.begin();
		for (; Iter != Lastm_RoleVec.end(); ++Iter)
		{
			if (!(*Iter))
				continue;
			delete (*Iter);
		}
		Lastm_RoleVec.clear();
		Lastm_MonthRoleMap.clear();

		LoadSum = 0;
		m_NowMonthTimeID = 0;
	}
};
struct tagRobotInfo
{
	DWORD	dwUserID;
	DWORD	Level;
	BYTE	SkillSum;
};
class FishMonth //һ������
{
public:
	FishMonth();
	virtual ~FishMonth();

	bool OnInit(BYTE MontID);
	void OnDestroy();
	//�����Ĵ�����
	void  UpdateByMin();//��ϵ���� 1���Ӹ���һ�� ��ϵ������״̬
	void  UpdateMonthIndex();
	void  SignUp(DWORD dwUserID);//�Ƿ���Ա�������
	void  SignUpResult(GC_Cmd_SignUpMonthResult* pMsg);
	void  JoinMonth(DWORD dwUserID, WORD TableID);//�Ƿ���Բμӱ���

	void OnChangeRolePoint(DWORD dwUserID, DWORD Point);
	void OnChangeRoleGlobel(DWORD dwUserID, DWORD Globel);
	void OnChangeRoleInfo(DWORD dwUserID, DWORD Point, DWORD Globel,DWORD SkillSum);
	void OnAddMonthGlobelNum(DWORD dwUserID);

	void OnLoadAllGameUserMonthInfo();

	bool RoleIsInMonth(DWORD dwUserID){ return m_MonthRoleMap.count(dwUserID) == 1; }
	BYTE GetMonthID();
	WORD GetSigupSum(){ return ConvertDWORDToWORD(m_RoleVec.size() /*+ m_MonthRankSum*/); }

	BYTE IsInSignUp();
	BYTE IsInStart();

	void ResetUserMonthInfo(DWORD dwUserID);//�û����Լ��ı�������


	bool MonthIsNeedSendPlayerSum(){ return m_IsInSignUp  && m_IsStart; }


	bool IsCanJoinMonth(DWORD dwUserID, tagRoleMonthInfo& MonthInfo);

	LastMonthRewardInfo& GetLastMonthInfo(){ return m_LastMonthInfo; }

	void OnUpdateRobotInfo();
	void OnLoadRobotFinish();

	void SendMonthStatesToGameServer(BYTE GameConfigID,BYTE MonthStates);
private:
	void  OnJoinSignUp();//�������뱨���׶�
	void  OnLeaveSignUp();//�����뿪�����׶�
	void  OnJoinStart();//������������׶�
	void  OnLeaveStart();//��������
	void  OnSaveMonthXml(SYSTEMTIME& pTime,std::vector<tagRoleMonthCenterInfo*>& pVec);

	void SaveFileToFtp(const char* FilePath,const char* FileName);

	void OnRoleSumChange();

	void OnLoadRobotInfo();
	
	void OnDestroyRobotInfo();
private:
	//tagMonthConfig*					m_FishMonthConfig;//��ǰ������������Ϣ
	BYTE							m_MonthID;
	bool							m_IsInSignUp;//�Ƿ��ڱ����׶�
	bool							m_IsStart;//�Ƿ��ڿ�ʼ�׶�
	//��ǰ�����ļ��� ����Ҫ���е����������� ��Ҫ�������� ����ʹ��MapVec
	std::vector<tagRoleMonthCenterInfo*>		m_RoleVec;//�����ּ�������
	HashMap<DWORD,tagRoleMonthCenterInfo*>		m_MonthRoleMap;//��ҵļ���

	DWORD										m_NowMonthTimeID;

	LastMonthRewardInfo							m_LastMonthInfo;

	//DWORD										m_MonthRankSum;

	HashMap<DWORD, tagRobotInfo>				m_RobotMap;//�����˵ļ��� 
};
class FishMonthManager //������
{
public:
	FishMonthManager();
	virtual ~FishMonthManager();

	bool	OnInit();
	void    Update(DWORD dwTimer);
	FishMonth* QueryMonth(BYTE MonthID);

	void SendRoleSignUpInfoToGameServer(DWORD dwUserID);
	void ResetUserMonthInfo(DWORD dwUserID, BYTE MonthID);

	void OnSendMonthDataToGame(BYTE GameSocketID);

	void OnLoadRobotFinish();

	bool RobotIsFinish(){ return m_RobotIsFinish; }

	void OnGameRsg(BYTE GameID);
	//void OnLogonToMonth(ServerClientData* pClient,DWORD dwUserID, BYTE MonthID);
private:
	void	UpdateByMin(DWORD dwTimer);
	void	UpdateMonthIndex(DWORD dwTimer);
	void	UpdateRobot(DWORD dwTimer);
	//void    UpdateMonthRoleSunToGameServer(DWORD dwTimer);
private:
	HashMap<BYTE, FishMonth*>			m_FishMonthMap;
	bool								m_RobotIsFinish;
};