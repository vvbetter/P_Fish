//�������Ϸ�е� һЩ���ݵ�ͳ��
//��Ҳμӱ��� ������εĴ���
//��Ҳμӱ��� ���ǰ�����Ĵ��� (�ֱ�123)
//��Ҳ��������� �Ĵ��� (������)
//��һ�ý�ҵ�������
//��ҿ������Ľ�ҵ�����������ͳ��
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
	void UpdateMinWinLose();					//ͳ��һ�����ڵ���Ӯ
	void ChangeGameGold(const INT64 gold);	//ÿ����Ӯͳ�ƽ��

	const INT32 GetWinNum(){ return m_winNum; }
	const INT32 GetLoseNum(){ return m_loseNum; }
	const INT64 GetTotalWinGold(){ return m_TotalWinGold; }
	const int64 GetTotalLoseGold(){ return m_TotalLoseGold; }
private:
	CRoleEx*				m_pRole;
	tagRoleGameData			m_RoleGameData;
	INT64					m_lastMinWinGold;//��һ������Ӯ���
	INT64					m_lastMinLoseGold;//��һ��������
	INT32					m_winNum;  //ͳ��ÿ���ӵ���Ӯ���˳�ʱ������Ӯ����
	INT32					m_loseNum; //ͳ��ÿ���ӵ���Ӯ���˳�ʱ������Ӯ���� 
	INT64					m_TotalWinGold;	//���һ����Ϸ���ܻ�ý��
	INT64					m_TotalLoseGold; //���һ����Ϸ����������
};