
#pragma once

//��������Ϣ
struct tagRobotInfo
{
	int nChip[5];
	int nAreaChance[MAX_NIUNIU_ClientSum];										//������
														//�������

	tagRobotInfo()
	{
		int nTmpChip[5] = {1000,10000,100000,500000,1000000};
		int nTmpAreaChance[MAX_NIUNIU_ClientSum] = { 1, 1, 1, 1 };

		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));		
	}
};

struct OxTimer
{
	OxTimer(BYTE byTimerid, SHORT nTime)
	{
		m_byTimerid = byTimerid;
		m_nTime = nTime;
	}
	BYTE m_byTimerid;
	SHORT m_nTime;
};


class COxRobot
{
	//��Ϸ����
protected:
	bool							m_bAction;
	bool							m_bBnkerRobot;						//ׯ�һ�����
	LONGLONG						m_lMaxChipBanker;					//�����ע (ׯ��)
	LONGLONG						m_lMaxChipUser;						//�����ע (����)
	LONGLONG						m_lAreaChip[MAX_NIUNIU_ClientSum];			//������ע 	

	int								m_nChipLimit[2];					//��ע��Χ (0-MAX_NIUNIU_ClientSum)
	int								m_nChipTime;						//��ע���� (����)
	int								m_nChipTimeCount;					//���´��� (����)

	//��ׯ����
protected:
	//bool							m_bMeApplyBanker;					//�����ʶ
	int								m_nBankerCount;						//�������˵���ׯ����
	int								m_nWaitBanker;						//�ռ���

	//���ñ���  (ȫ������)
protected:
	tagRobotInfo					m_RobotInfo;						//ȫ������
	//���ñ���	(��Ϸ����)
protected:
	LONGLONG						m_lAreaLimitScore;					//��������
	LONGLONG						m_lUserLimitScore;					//��ע����	

	//���ñ���  (����������)
protected:
	int								m_nRobotBankerCount;				//��ׯ����
	int								m_nRobotWaitBanker;					//��������
	bool							m_bReduceJettonLimit;				//��������
	int								m_nMinRobotBankerCount;				//��С����	

	//�����˴�ȡ��
	LONGLONG						m_lRobotScoreRange[2];				//���Χ
	LONGLONG						m_lRobotBankGetScore;				//�������
	LONGLONG						m_lRobotBankGetScoreBanker;			//������� (ׯ��)
	int								m_nRobotBankStorageMul;				//����
//
	std::vector<OxTimer*>			m_VecOxTimer;	
	SHORT							m_nGameTime;
	DWORD							m_dwRobotid;
public:
	static DWORD					m_dwApplistChangeTime;
	static BYTE						m_byAppCountLimit;
	static DWORD					m_dwRobotCountChangeTime;
	static BYTE						m_byRobotCount;


	//�ؼ�����
protected:

	//��������
public:
	//���캯��
	COxRobot(DWORD dwRobotid);
	//��������
	virtual ~COxRobot();
	//��Ϸ�¼�
public:	
	//ʱ����Ϣ
	virtual bool  OnEventTimer(UINT nTimerID);
public:
	//��Ϣ����
public:
	DWORD URand()
	{
		return (rand() << 16) | (rand());
	}
	template<class T>
	T ClipNum(T t1, T t2)
	{
		if (t1 == t2)
		{
			return t1;
		}
		return t1 + URand() % (t2 - t1 + 1);
	}
	bool OnSubUserApplyBanker(const VOID * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker(const VOID * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker(const VOID * pBuffer, WORD wDataSize);

	//���ܺ���
public:
	//��ȡ����
	VOID ReadConfigInformation(TCHAR szFileName[], TCHAR szRoomName[], bool bReadFresh);
	//���㷶Χ
	bool CalcJettonRange(LONGLONG lMaxScore, LONGLONG lChipLmt[], int & nChipTime, int lJetLmt[]);	
	bool Update();
	DWORD UserId();
	DWORD UserGold();
	DWORD BankerId();
	DWORD BankerGold();
	WORD  ApplyListLength();
	void StartGame();
	void EndGame();
	void AdjustGold(DWORD dwGold);
	tagNiuNiuConfig GameConfig();
};

class COxRobotManager
{
public:
	COxRobotManager();
	~COxRobotManager();
	bool IsRobot(DWORD dwUserid);
	USHORT RobotCount();
	void Update();
	void StartGame();
	void EndGame();
private:	
	std::vector<COxRobot*>m_allrobot;
};