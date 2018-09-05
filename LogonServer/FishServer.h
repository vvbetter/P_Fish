//��½�������ĺ�����
#pragma once
#include "..\CommonFile\FishConfig.h"
#include "LogonManager.h"
#include "LogonCache.h"
struct DelClient
{
	DWORD		LogTime;
	DWORD		SocketID;
};
struct GameServerInfo
{
	DWORD	ID;
	DWORD	IP;
	WORD	Port;
	WORD	OnlineSum;

	GameServerInfo()
	{
		ID = 0;
		IP = 0;
		Port = 0;
		OnlineSum = 0;
	}
};
struct WriteGameServerUserInfo
{
	DWORD		dwUserID;
	DWORD		ClientID;
	DWORD		LogTime;
};
class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer(DWORD LogonID);
	bool MainUpdate();
	bool OnDestroy();

	void SetServerClose(){ m_IsClose = true; }
	void SetReloadConfig(){ m_IsReloadConfig = true; }
	void ShowInfoToWin(const char *pcStr, ...);

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);
	void OnAddClient();

	void SendNetCmdToDB(NetCmd* pCmd);
	void SendNetCmdToGameServer(BYTE GameID,NetCmd* pCmd);
	void SendNetCmdToOtherLogon(BYTE LogonID, NetCmd* pCmd);
	void SendNewCmdToClient(ServerClientData* pClient, NetCmd* pCmd);
	void SendNetCmdToCenter(NetCmd* pCmd);
	void SendNetCmdToOperate(NetCmd* pCmd);
	void SendNetCmdToControl(NetCmd*pCmd);
	void SendNetCmdToLogDB(NetCmd* pCmd);

	ServerClientData* GetClientServerData(DWORD ClientID);
	//ServerClientData* GetServerServerData(BYTE ServerID);
	//GameServerInfo* GetGameServerInfo(DWORD GameServerID);

	DWORD GetLogonConfigID(){ return m_LogonConfigID; }

	void OnLogonDBDataSucess();

	void AddDelRoleSocket(DelClient delCLient){ m_DelSocketVec.push_back(delCLient); }
private:
	bool ConnectDB();
	bool ConnectCenter();
	bool ConnectOperate();
	bool ConnectClient();
	bool ConnectControl();
	bool ConnectLogDB();

	void HandleAllMsg();

	bool HandleControlMsg(NetCmd* pCmd);

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);


	void HandleClientMsg(ServerClientData* pClient,NetCmd* pCmd);
	void HandleGameServerMsg(NetCmd* pCmd);
	void HandleDataBaseMsg(NetCmd* pCmd);
	void HandleCenterMsg(NetCmd* pCmd);
	void HandleOperateMsg(NetCmd* pCmd);

	//ServerClientData* GetMaxFreeGameServer();

	void CheckDelSocket(DWORD dwTimer);
	void CloseClientSocket(DWORD SocketID);

	void OnAddChannelInfo(DWORD ClientID, ChannelUserInfo* pInfo);
	void OnDelChannelInfo(DWORD ClientID);
	void OnSendChannelInfoToGameServer(DWORD dwUserID, DWORD ClientID, BYTE GameServerID);
	void OnClearAllChannelInfo();

	bool OnAddUserWriteGameServer(DWORD dwUserID,DWORD ClientID);
	bool OnCheckUserWriteGameServerIsExists(DWORD dwUserID);
	void OnUpdateWriteGameServerUserMap(DWORD dwTimer);

	void OnReloadConfig();

	void UpdateInfoToControl(DWORD dwTimer);
private:
	Dump						m_pDump;
	volatile bool	 	m_IsClose;
	volatile bool								m_IsReloadConfig;
	//�����
	NewServer			m_ClientTcp;//�Կͻ��˵������
	//NewServer			m_ServerTcp;//�Է������������
	TCPClient			m_DBTcp;//�����ݿ�������
	TCPClient			m_CenterTcp;//������������������
	TCPClient			m_OperateTcp;//��Ӫ������
	//vector<ServerClientData*>	m_ClintList;//�ͻ��˵�����
	//vector<ServerClientData*>	m_ServerList;//GameServer������

	SafeList<AfxNetworkClientOnce*>      m_AfxAddClient;

	HashMap<DWORD, ServerClientData*>	m_ClintList;
	DWORD								m_ClientIndex;

	//HashMap<BYTE, ServerClientData*>	m_ServerList;
	//DWORD								m_ServerIndex;

	bool											m_DBIsConnect;
	bool											m_CenterIsConnect;
	bool											m_OperateIsConnect;
	//�������� 
	BYTE								m_ClientNetworkID;
	//BYTE								m_ServerNetworkID;

	BYTE								m_LogonConfigID;

	LogonManager						m_LogonManager;
	FishConfig							m_FishConfig;

	LogonCache							m_LogonCacheManager;

	vector<DelClient>					m_DelSocketVec;//��ɾ���Ŀͻ���

	HashMap<DWORD, ChannelUserInfo*>	m_ChannelInfo;

	HashMap<DWORD, DWORD>				m_UserChannelMap;

	//��GameServer ���͵���ҵļ�¼ 
	HashMap<DWORD, WriteGameServerUserInfo>		m_SendGameUserMap;//�ȴ�GameServer���ص���ҵļ���

	TCPClient									m_ControlTcp;
	bool										m_ControlIsConnect;


	TCPClient									m_LogDBTcp;
	bool										m_LogDBIsConnect;
};
extern FishServer g_FishServer;