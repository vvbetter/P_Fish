#pragma once
#include "..\CommonFile\FishConfig.h"
#include "RoleManager.h"
#include "RoleRelationManager.h"
#include "FishMonthManager.h"
#include "TaskManager.h"
#include "ClientListManager.h"
#include "AnnouncementManager.h"
#include "FishNoticeManager.h"
#include "..\CommonFile\RobotManager.h"
#include "CashManager.h"

class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer(bool IsInit);
	bool MainUpdate();
	bool OnDestroy();

	void UpdateByMin(DWORD dwTimer);

	void SetServerClose(){ m_IsClose = true; }
	void SetReloadConfig(){ m_IsReloadConfig = true; }

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);
	void OnAddClient();

	void SendNetCmdToDB(NetCmd* pCmd);
	void SendNetCmdToClient(ServerClientData* pClient, NetCmd* pCmd);
	void SendNetCmdToFtp(NetCmd* pCmd);
	void SendNetCmdToControl(NetCmd*pCmd);
	void SendNetCmdToLogDB(NetCmd* pCmd);

	FishConfig& GetFishConfig(){ return m_FishConfig; }
	CenterRoleManager& GetRoleManager(){ return m_RoleManager; }
	FishMonthManager&	GetMonthManager(){ return m_MonthManager; }
	ClientListManager& GetCenterManager(){ return m_CenterServerManager; }
	RobotManager& GetRobotManager(){ return m_RobotManager; }
	CashManager& GetCashManager(){ return m_CashManager; }
	AnnouncementManager& GetAnnouncenment(){ return m_AnnouncementManager; }

	ServerClientData* GetUserClientDataByIndex(BYTE IndexID);

	void SendUserAchievementToGameServer(BYTE GameID);

	void SendMessageByType(TCHAR* pMessage,WORD MessageSize, BYTE MessageType, DWORD MessageColor, BYTE StepNum, BYTE StepSec, DWORD Param);
private:
	void OnCenterFinish();

	bool ConnectDB();
	bool ConnectClient();
	bool ConnectLogDB();

	void HandleAllMsg();

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);

	bool HandleGameServerMsg(BYTE serverID, ServerClientData* pClient, NetCmd* pCmd);
	bool HandleDataBaseMsg(NetCmd* pCmd);
	bool HandleFtpMsg(NetCmd* pCmd);
	bool HandleControlMsg(NetCmd* pCmd);

	bool HandleAddUserMailResult(NetCmd* pCmd);
	bool HandleSaveRoleCharmResult(NetCmd* pCmd);
	bool HandleSendAchievementPointList(NetCmd* pCmd);

	void SendGetAchievementList();

	void OnReloadConfig();
	void UpdateInfoToControl(DWORD dwTimer);
private:
	//dump
	Dump										m_pDump;
	//��������������
	volatile bool	 							m_IsClose;
	volatile bool								m_IsReloadConfig;
	NewServer									m_ClientTcp;//��GameServer�������
	TCPClient									m_DBTcp;//�����ݿ�������
	TCPClient									m_LogDBTcp;//Log ���ݿ�
	//TCPClient									m_FtpTcp;//��FTP�������
	//NewServer									m_ControlTcp;//�ⲿ���Ƶ�TCP ���� 
	//DWORD										m_ControlRankValue;
	//HashMap<DWORD, ServerClientData*>			m_ControlList;//Control���б�
	//BYTE										m_ControlworkID;
	//DWORD										m_ControlIndex;

	BYTE										m_CenterworkID;
	HashMap<BYTE, ServerClientData*>			m_ClintList;//GameServer���б�
	SafeList<AfxNetworkClientOnce*>				m_AfxAddClient;
	bool										m_DBIsConnect;
	bool										m_FtpIsConnect;
	bool										m_LogDBIsConnect;
	//��ҹ�����
	CenterRoleManager							m_RoleManager;
	//�����ļ�
	FishConfig									m_FishConfig;
	//����������
	FishMonthManager							m_MonthManager;
	//���������
	TaskManager									m_GlobelTaskManager;
	//GameServer �� LogonServer �� ������ 
	ClientListManager							m_CenterServerManager;
	//�ɾ͵�����
	HashMap<DWORD, WORD>						m_AchievementList;
	//����
	AnnouncementManager							m_AnnouncementManager;

	FishNoticeManager							m_FishNoticeManager;

	//���ӵ�������
	//TCPClient									m_ControlTcp;
	bool										m_ControlIsConnect;

	//������
	RobotManager								m_RobotManager;
	//
	CashManager									m_CashManager;
};
extern FishServer g_FishServer;