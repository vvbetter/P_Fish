#pragma once
#include"oxrobot.h"
enum BrandNiuNiuLevel
{
	BNNL_Non	= 1,		//��ţ
	BNNL_1		= 2,		//ţһ 
	BNNL_2		= 3,
	BNNL_3		= 4,
	BNNL_4		= 5,
	BNNL_5		= 6,
	BNNL_6		= 7,
	BNNL_7		= 8,
	BNNL_8		= 9,
	BNNL_9		= 10,		//ţ9
	BNNL_NN		= 11,		//ţţ
	BNNL_WHN	= 12,		//�廨ţ
	BNNL_ZD		= 13,		//ը��
	BNNL_WXN	= 14,		//��Сţ
};
enum NiuNiuTableStates
{
	NNTS_Begin			= 1,	//��ע�׶�
	NNTS_End			= 2,	//���ƽ׶�
	NNTS_WriteBegin		= 3,	//�ȴ���ʼ
};
struct tagNiuNiuRoleInfo
{
	DWORD			UserID;
	DWORD			BetGlobel[MAX_NIUNIU_ClientSum];
	bool			IsInWriteBanakerVec;
	bool			IsBankerUser;
	bool			IsInVipSeat;
	bool			IsRobot;
	BYTE			VipSeat;
};
struct OnceBrandList//һ����
{
	std::vector<BYTE> BrandVec;//�Ƶļ���

	OnceBrandList()
	{
		for (BYTE i = 1; i <= 52; ++i)
			BrandVec.push_back(i);
	}

	BYTE    GetNewBrand()
	{
		//��ȡһ������
		BYTE Index = RandUInt() % BrandVec.size();
		BYTE BrandValue = BrandVec[Index];//��ȡָ���±����
		BrandVec.erase(BrandVec.begin() + Index);
		return BrandValue;
	}
	void  AddBrand(BYTE BrandValue)
	{
		BrandVec.push_back(BrandValue);
	}
	void    OnClear()
	{
		BrandVec.clear();
		for (BYTE i = 1; i <= 52; ++i)
			BrandVec.push_back(i);
	}
};
struct VipSeatInfo
{
	DWORD		dwUserID;
	BYTE		NonBetGlobelSum;
};
struct NiuNiuResultLog  //Log
{
	BYTE		LogSum;
	DWORD		ResultLog;
	NiuNiuResultLog()
	{
		LogSum = 0;
		ResultLog = 0;
	}
	void SetResult(WORD TableResult[MAX_NIUNIU_ClientSum + 1])
	{
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
		
		for (DWORD i = BeginIndex, j = 1; i <= EndIndex; ++i,++j)
		{
			bool Result = (TableResult[0] >= TableResult[j]);//ׯ��Ӯ
			if (Result)
				ResultLog = ResultLog | (1 << i);
		}

		++LogSum;
	}
};
class NiuNiuManager
{
public:
	NiuNiuManager(DWORD niuniuID);
	virtual ~NiuNiuManager();
	//��������
	bool		OnInit();//��ʼ������
	void		OnUpdate(DWORD dwTimer);//���º���
	void		OnDestroy();//���ٺ���
	//��ҵĲ���
	void		OnRoleJoinNiuNiuTable(DWORD dwUserID);//��ҽ���ţţ����
	void		OnRoleLeaveNiuNiuTable(DWORD dwUserID);//����뿪ţţ����
	void		OnRoleBetGlobel(DWORD dwUserID, BYTE BetIndex, DWORD AddGlobel);//�����ע
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
	void		OnRoleGlobelChange(DWORD dwUserID,int ChangeSum);

	UINT64      GetTableSystemGlobel(){ return m_SystemGlobelSum; }

	DWORD		GetRoleSum(){ return m_TableUserMap.size(); }

	DWORD		GetNiuNiuID(){ return m_dwNiuNiuID; }

	void		SetBrandInfo(DWORD dwUserID);
private:
	//�ڲ�����
	void		SetTableStates(NiuNiuTableStates States);//��������״̬
	void		OnClearTableInfo();//ÿ��֮����������������
	//void		SendDataToAllTableRole(NetCmd* pCmd);
	void		SetNextBankerUserInfo();
	tagNiuNiuRoleInfo* GetRoleInfo(DWORD dwUserID);
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
	INT64		ChangeBrandValueBySystem(OnceBrandList& pBrand);
	SHORT		GetBrandRateValue(BYTE Index);
	WORD		HandleBrandValue(BYTE* BrandValue);
	BYTE		GetBrandHandleValue(BYTE Value);
	void		ChangeValue(BYTE& A, BYTE& B);
	bool		IsExitsNormalRole();

	//void		OnChangeSystemGlobel(DWORD dwUserID, INT64 ChangeGlobel);
public:
	void		QueryNames(DWORD ClientID);
	void		QueryAllJetton(DWORD ClientID);
	void		QueryPlayerJetton(TCHAR *pNickame, DWORD ClientID);
	DWORD		BankerId();
	DWORD		BankerGold();
	WORD		ApplyListLength();
	bool		IsGameEnd();
	bool		HaveApply(DWORD dwUserid);
	void		CalPlayerJetton(UINT64 uJetton[MAX_NIUNIU_ClientSum]);

	void		RestAllBrandInfo();
private:
	NiuNiuTableStates			m_TableStates;//���ӵ�״̬
	DWORD						m_TableStatesLog;//���ӽ��뵱ǰ״̬�ļ�¼ʱ��
	bool						m_IsNeedClearBankerInfo;
	//�����ϵ�һЩ����
	INT64						m_SystemGlobelSum;//ϵͳ�������
	UINT64						m_TableBetAreaGlobel[MAX_NIUNIU_ClientSum];//4����ע���Ľ������
	BYTE						m_TableBrandArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//������ 5x5 ��
	WORD						m_TableBrandResult[MAX_NIUNIU_ClientSum + 1];//�ƵĽ��
	//ׯ������
	DWORD						m_TableBankerUserID;//ׯ�ҵ�ID
	BYTE						m_TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	//�ȴ�ׯ���б������
	vector<DWORD>				m_TableWriteBankerVec;//�ȴ��б�
	//������ҵ�����
	HashMap<DWORD, tagNiuNiuRoleInfo> m_TableUserMap;//��Ҽ���
	//���ϯ
	VipSeatInfo					m_VipSeatVec[MAX_VIPSEAT_Sum];//���ϯ�ļ��� 8�����ϯ
	//��������
	NiuNiuResultLog				m_NiuNiuResultLog;
	COxRobotManager				m_oxrobotmanager;
	//
	DWORD						m_dwNiuNiuID;
	//
	DWORD						m_NiuNiuUpdateTime;
	DWORD						m_NiuNiuUpdateTableInfo;
};