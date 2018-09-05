//ת�̹����� 
#pragma once
#include"niuniu/carrobot.h"
enum CarTableStates
{
	CTS_Begin = 1,	//ת����ע�׶�
	CTS_End = 2,	//ת�̿�ʼ�׶�
	CTS_WriteBegin = 3,	//�ȴ���ʼ
};

//12 ����ע�� ���ǽ��д��� 
// 12����ע�� 

struct tagCarRoleInfo  //ת�����
{
	DWORD			UserID;
	DWORD			BetGlobel[MAX_CAR_ClientSum];
	bool			IsInWriteBanakerVec;
	bool			IsBankerUser;
	bool			IsInVipSeat;
	BYTE			VipSeat;
	bool			IsRobot;
};
struct tagCarVipSeatInfo
{
	DWORD		dwUserID;
	BYTE		NonBetGlobelSum;
};
struct CarResultLog  //Log
{
	BYTE		LogSum;
	DWORD		ResultLog;
	CarResultLog()
	{
		LogSum = 0;
		ResultLog = 0;
	}
	void SetResult(BYTE ResultIndex)
	{
		//12��   F �͹���   8ge  FFFF FFFF
		if (ResultIndex > 0xf)
			return;
		if (LogSum >= 8)
		{
			LogSum = 7;
			//��������
			for (BYTE i = 0; i < 28; ++i)
			{
				BYTE Value = ((ResultLog & (1 << (i + 4))) != 0 ? 1 : 0);
				if (Value == 1)
					ResultLog |= (Value << i);
				else
				{
					if ((ResultLog & (1 << i)) != 0)
						ResultLog ^= (1 << i);
				}
			}
			for (BYTE i = 28; i < 32; ++i)
			{
				if ((ResultLog & (1 << i)) != 0)
					ResultLog ^= (1 << i);
			}
		}

		DWORD BeginIndex = LogSum * 4;
		DWORD EndIndex = LogSum * 4 + 3;

		for (DWORD i = BeginIndex, j = 1; i <= EndIndex; ++i, ++j)
		{
			//
			bool Result = ((ResultIndex & (1 << (i - BeginIndex))) != 0);
			if (Result)
				ResultLog = ResultLog | (1 << i);
		}

		++LogSum;
	}
};

class CarManager
{
public:
	CarManager();
	virtual ~CarManager();

	bool		OnInit();//��ʼ��
	void		OnUpdate(DWORD dwTimer);//���º���
	void		OnDestroy();//���ٺ���
	//��Ҳ���
	void		OnRoleJoinCarTable(DWORD dwUserID);
	void		OnRoleLeaveCarTable(DWORD dwUserID);
	void		OnRoleBetGlobel(DWORD dwUserID, BYTE BetIndex, DWORD AddGlobel);
	void		OnRoleBetGlobelByLog(DWORD dwUserID, DWORD AddGlobelArray[MAX_CAR_ClientSum]);
	void		OnRoleGetWriteBankerList(DWORD dwUserID);//��һ�ȡ�Ŷ��б�
	void		OnRoleJoinWriteBankerList(DWORD dwUserID);//�����ׯ
	void		OnRoleGetWriteBankerFirstSeat(DWORD dwUserID);//�����ׯ
	void		OnRoleLeaveWriteBankerList(DWORD dwUserID);//����뿪ׯ���Ŷ��б�
	void		OnRoleCanelBanker(DWORD dwUserID);//�����ׯ
	void		OnRoleJoinVipSeat(DWORD dwUserID, BYTE VipSeatIndex);//����������ϯ
	void		OnRoleLeaveVipSeat(DWORD dwUserID);//����뿪���ϯ
	void		OnRoleGetNormalInfoByPage(DWORD dwUserID, DWORD Page);//��һ�ȡ����ϯ������
	//�ⲿ����
	void		OnRoleLeaveMiniGameServer(DWORD dwUserID);//����뿪MiniGame��������
	void		OnRoleGlobelChange(DWORD dwUserID, int ChangeSum);
	UINT64      GetTableSystemGlobel(){ return m_SystemGlobelSum; }
private:
	void		SetTableStates(CarTableStates States);//��������״̬
	void		OnClearTableInfo();//ÿ��֮����������������
	//void		SendDataToAllTableRole(NetCmd* pCmd);
	void		SetNextBankerUserInfo();
	tagCarRoleInfo* GetRoleInfo(DWORD dwUserID);
	//�¼�
	void		OnWriteBankerChange();//���ȴ��б�ǰ8λ�����仯��ʱ�� 
	void		OnBankerUserChange();//����ǰר�ҷ����仯��ʱ��
	void		OnTableJoinBegin();//�����ӽ�����ע�׶ε�ʱ��
	void		OnUpdateTableInfo();//���������ϵ����ݵ��ͻ���
	void		OnTableJoinEnd();//���ӽ��뿪�ƽ׶�
	//���º���
	void		UpdateTableIsCanBegin(DWORD dwTimer);//�������� �Ƿ���Խ�����ע�׶�
	void		UpdateTableIsCanEnd(DWORD dwTimer);//�Ƿ���Խ��뿪�ƽ׶�
	void		UpdateTableInfo(DWORD dwTimer);//������������
	//˽�к���
	INT64		GetSystemGlobelChange();
	INT64		GetBrandUserGlobelChange();
	INT64		ChangeResultBySystem();
	void		CreateNewCar();
	BYTE		GetTableResult(BYTE& Index);
	SHORT		GetBrandRateValue(BYTE Index);
	bool		IsCanAddGlobel(DWORD AddGlobel, BYTE Index);
	BYTE		GetResultID();
	BYTE		GetResultRate();

	bool		IsExitsNormalRole();
public:
	DWORD		BankerId();
	DWORD		BankerGold();
	WORD		ApplyListLength();
	bool		IsGameEnd();
	bool		HaveApply(DWORD dwUserid);
	void		QueryNames(DWORD ClientID);
	void		QueryAllJetton(DWORD ClientID);
	void		QueryPlayerJetton(TCHAR *pNickame, DWORD ClientID);
	void		CalPlayerJetton(UINT64 uJetton[MAX_CAR_ClientSum]);
private:
	CarTableStates				m_TableStates;//���ӵ�״̬
	DWORD						m_TableStatesLog;//���ӽ��뵱ǰ״̬�ļ�¼ʱ��
	bool						m_IsNeedClearBankerInfo;
	//�����ϵ�һЩ����
	INT64						m_SystemGlobelSum;//ϵͳ�������
	UINT64						m_TableBetAreaGlobel[MAX_CAR_ClientSum];//8����ע���Ľ������
	BYTE						m_TableResult;//���̵Ľ��
	BYTE						m_TableResultIndex;//���̵�λ��
	//ׯ������
	DWORD						m_TableBankerUserID;//ׯ�ҵ�ID
	BYTE						m_TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	//�ȴ�ׯ���б������
	vector<DWORD>				m_TableWriteBankerVec;//�ȴ��б�
	//������ҵ�����
	HashMap<DWORD, tagCarRoleInfo> m_TableUserMap;//��Ҽ���
	//���ϯ
	tagCarVipSeatInfo				m_VipSeatVec[MAX_CARVIPSEAT_Sum];//���ϯ�ļ��� 8�����ϯ
	//
	CarResultLog					m_CarResultLog;
	CCarRobotManager				m_robotmanager;
};