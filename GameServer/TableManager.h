//��ǰ��Ϸ�ĺ����� 
//һ�����Ӵ��� ������ҵ�һ����Ϸ
//�����ڷ��俪ʼ��ʱ����Ѿ�������� �������ú����������
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

struct SysYieldControl //ϵͳ�������
{
	int64 TotalCost;	//��������ܻ���
	int64 TotalProduce;	//����ϵͳ�ܲ���
	int64 lastMinCost;	//��һ��������ܻ���
	int64 lastMinProduce;	//��һ����ϵͳ�ܲ���
	SysYieldControl()
	{
		TotalCost = 0;
		TotalProduce = 0;
		lastMinCost = 0;
		lastMinProduce = 0;
	}
};

class TableManager //�������� ��Ҫһ�����ӵ������ļ������� ������������������̳߳�ͻ
{
	//���ӵĹ�����
public:
	TableManager();
	virtual ~TableManager();

	void OnInit();//��ʼ��
	void Destroy();
	void OnStopService();
	void Update(DWORD dwTimeStep);
	bool OnHandleTableMsg(DWORD dwUserID,NetCmd* pData);
	bool OnPlayerJoinTable(BYTE TableTypeID, CRoleEx* pRoleEx,BYTE MonthID = 0,bool IsSendToClient = true);
	bool OnPlayerJoinTable(WORD TableID, CRoleEx* pRoleEx, bool IsSendToClient = true);
	void OnPlayerLeaveTable(DWORD dwUserID);
	void ResetTableInfo(DWORD dwUserID);
	//����������Ϣ
	void SendDataToTable(DWORD dwUserID, NetCmd* pData);
	
	CRole* SearchUser(DWORD dwUserid);
	CConfig *GetGameConfig();
	GameTable* GetTable(WORD TableID);
	DWORD GetTableSum(){ return m_TableVec.size(); }

	void OnChangeTableGlobel(WORD TableID, int64 AddGlobel, USHORT uTableRate);
	void OnResetTableGlobel(WORD TableID,int64 nValue);
	int64  GetTableGlobel(WORD TableID);

	void OnChangeTableTypePlayerSum(BYTE TableTypeID,bool IsAddOrDel);
	bool QueryPool(WORD TableID, int64 & nPoolGold);
	void QueryPool(BYTE TableTypeID, bool &bopen, int64&nPoolGold);
	std::list<DWORD> GetBlackList();
	bool SetBlackList(DWORD *pid,BYTE byCount);
	bool Isabhor(DWORD dwUserid);
	CRole* QueryRoleByRoleEx(CRoleEx* pRoleEx);
	void CostAndProduceMin(); //ÿ���Ӽ�¼ϵͳ��֧��־
	void NewDayForTable();//ÿ������
	const vector<float>& SysProduceRate(const BYTE tableType );//ϵͳ�������
	bool InitTableTotalGold(NetCmd* pData);
private:
	DWORD									m_LastUpdate;
	WORD						 			m_MaxTableID;
	//���Ӿ���Ķ�����
	HashMap<DWORD, WORD>					m_RoleTableMap;
	std::vector<GameTable*>					m_TableVec;

	CConfig                     *m_pGameConfig;

	CFishTimer                  m_TimerGameTime;//�������ʱ�����

	bool						m_bUpdateTime;

	HashMap<BYTE, GoldPool>		m_TableGlobeMap;//�������͵Ľ�ҳ�
	HashMap<BYTE, DWORD>        m_TableTypeSum;//��ǰ������ҵ�����
	std::list<DWORD>			m_blacklist;
	map<BYTE, SysYieldControl> m_sysMinutes;//ÿ������ÿ��Ľ�Ҽ�¼
	map<BYTE, TableTotalGold> m_sysTotal;//������ͳ��������Ľ�Һ�ϵͳ�������
};