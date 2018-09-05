//Ftp ֻ��Ҫ���ӵ�GameServer�Ϳ����� �������ӵ�DBȥ
#pragma once
#include "..\CommonFile\FishConfig.h"
struct TempFileData
{
	DWORD		FileSize;
	DWORD		WriteSize;
	BYTE*		FileData;
};
class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer();
	bool MainUpdate();
	bool OnDestroy();

	void SetServerClose(){ m_IsClose = true; }
	void ShowInfoToWin(const char *pcStr, ...);

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);
	void OnAddClient();

	void SendNetCmdToGameServer(ServerClientData* pClient, NetCmd* pCmd);
	void SendNetCmdToAllGameServer(NetCmd* pCmd);
	void SendNetCmdToControl(NetCmd*pCmd);
	void SendNetCmdToLogDB(NetCmd* pCmd);

	ServerClientData* GetUserClientDataByIndex(BYTE IndexID);
private:
	bool ConnectClient();
	bool ConnectControl();
	bool ConnectLogDB();

	void HandleAllMsg();

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);

	bool HandleGameServerMsg(ServerClientData* pClient, NetCmd* pCmd);
	bool HandleControlMsg(NetCmd* pCmd);

	void UpdateInfoToControl(DWORD dwTimer);
private:
	//��������������
	Dump				m_pDump;
	volatile bool	 					m_IsClose;
	BYTE								m_FtpNetworkID;
	NewServer							m_ClientTcp;//��GameServer�������
	HashMap<BYTE, ServerClientData*>	m_ClintList;//GameServer���б�
	SafeList<AfxNetworkClientOnce*>     m_AfxAddClient;

	HashMap<DWORD, TempFileData>				m_FileArray;

	//���ӵ�������
	TCPClient									m_ControlTcp;
	bool										m_ControlIsConnect;

	TCPClient									m_LogDBTcp;
	bool										m_LogDBIsConnect;
};
extern FishServer g_FishServer;