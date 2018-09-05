#pragma once
#include "Stdafx.h"
#include".\FishLogic\fishdesk.h"
#include "Role.h"
#include"FishTimer.h"
#include"SkillInfo.h"

const DWORD REPEATTIMER = (DWORD)0xFFFFFFFF;
const int GAME_TIME_SPACE = 5;//record game time;
const int MINUTE2SECOND = 60;

//���ӿ���
class GameTable : public NetSendInterface
{
public:
	GameTable();
	virtual ~GameTable();

	virtual bool				OnInit(WORD TableID, BYTE TableTypeID,BYTE MonthID = 0);
	virtual void				OnDestroy();
	virtual bool				IsCanJoinTable(CRoleEx* pRoleEx, BYTE MonthID);
	virtual bool				OnRoleJoinTable(CRoleEx* pRoleEx,BYTE MonthID,bool IsSendToClient);
	virtual bool				OnRoleLeaveTable(DWORD dwUserID);
	virtual bool				OnHandleTableMsg(DWORD dwUserID,NetCmd* pCmd);
	virtual void				Update(bool bUpdateTime);
	virtual void				OnGameStart();
	virtual void				OnGameStop();
	virtual WORD				GetTableID(){ return m_TableID; }
	virtual bool				IsFull();
	virtual WORD				GetTablePlayerSum();
	virtual BYTE				GetTableTypeID(){ return m_TableTypeID; }
	virtual BYTE				GetTableMonthID(){ return m_MonthID; }
	virtual CRole*              SearchUser(DWORD dwUserID);
	virtual void				SendDataToTable(DWORD dwUserID, NetCmd* pData);
	virtual void				SendDataToTableAllUser(NetCmd* pData);
	virtual void				SendRoleJoinInfo(DWORD dwUserID);

	virtual void				Send(PlayerID RoleID, NetCmd*);
	virtual void				SendBySeat(byte seatID, NetCmd*);
	virtual bool				IsUseSeat()const;
	//�����ϵ��л�����
	virtual bool				ChangeRoleRate(BYTE Rate);//����޸ı���
	virtual bool				IsCanJoinRoom(CRoleEx* pRole);
	//������
	virtual bool				IsCanAddRobot();
	//
	virtual CTableRoleManager&	GetRoleManager(){ return m_RoleManager; }

	FishDesk*					GetFishDesk(){ return &m_fishdesk; }

	void SendTableRoleInfoToClient(DWORD dwUserID);
	void DelaySyncDataToClient(CRoleEx* pData);
	const int64 GetTablePlayerAvgMoney();
private:
	DWORD						m_LastUpdateTime;
	WORD						m_TableID;
	BYTE						m_MonthID;//������ID  0��ʾ���Ǳ���������
	CTableRoleManager			m_RoleManager;
	FishDesk					m_fishdesk;//�������Ϸ��
	BYTE						m_TableTypeID;
};