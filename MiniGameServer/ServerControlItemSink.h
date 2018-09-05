#pragma once

class CServerControlOx
{
	//��ҿ���						
protected:
	BYTE							m_cbExcuteTimes;				//ִ�д���
	BYTE							m_cbControlStyle;				//���Ʒ�ʽ
	bool							m_bWinArea[MAX_NIUNIU_ClientSum];		//Ӯ������	
public:
	CServerControlOx(void);
	virtual ~CServerControlOx(void);

public:

public:
	//����������
	virtual bool  Control(const void * pDataBuffer);

	//��Ҫ����
	virtual bool  NeedControl();

	//��ɿ���
	virtual bool  CompleteControl();

	
	//��ʼ����
	virtual void  GetSuitResult(BYTE cbTableCardArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum], WORD wTableBrandResult[MAX_NIUNIU_ClientSum + 1],UINT64 nAllJettonScore[MAX_NIUNIU_ClientSum]);

	//��Ӯ����
protected:

	//��������
	bool AreaWinCard(BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1]);

	//�������
	bool GetSuitCardCombine(BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1], UINT64 nAllJettonScore[]);

	//ׯ����Ӯ
	LONGLONG GetBankerWinScore(int nWinMultiple[], UINT64 nAllJettonScore[]);

	//ׯ��Ӯ��
	void BankerWinCard(bool bIsWin, BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1], UINT64 nAllJettonScore[]);
};

class CServerControlAnimal
{
protected:
	BYTE							m_cbExcuteTimes;				//ִ�д���
	BYTE							m_cbControlStyle;				//���Ʒ�ʽ
	BYTE							m_cbarea;
public:
	CServerControlAnimal();
	virtual bool  Control(const void * pDataBuffer);
	virtual bool  NeedControl();//��ɿ���
	virtual bool  CompleteControl();
	BYTE GetSuitResult();
};

class CServerControlCar
{
protected:
	BYTE							m_cbExcuteTimes;				//ִ�д���
	BYTE							m_cbControlStyle;				//���Ʒ�ʽ
	BYTE							m_cbarea;
public:
	CServerControlCar();
	virtual bool  Control(const void * pDataBuffer);
	virtual bool  NeedControl();//��ɿ���
	virtual bool  CompleteControl();
	BYTE GetSuitResult();
};