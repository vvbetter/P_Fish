//����������ϵĿͻ��˹���
//���浱ǰ�����������������Logon �� Game�Ļ�������  Logon ���� ID  Game ����ID
#pragma once
#include <set>
class ClientListManager
{
public:
	ClientListManager();
	virtual ~ClientListManager();

	void SendNetCmdToGameServer(BYTE GameID,NetCmd* pCmd);
	void SendNetCmdToLogonServer(BYTE LogonID, NetCmd* pCmd);
	void SendNetCmdToGameHall(BYTE LogonID, NetCmd* pCmd);
	void SendNetCmdToAllGameServer(NetCmd* pCmd);
	DWORD GetGameServerSum();
	DWORD GetLogonServerSum();
	void SendNetCmdToAllLogonServer(NetCmd* pCmd);

	void RsgLogon(BYTE LogonID);
	void UnRsgLogon(BYTE LogonID);

	void RsgGame(BYTE GameID);
	void UnRsgGame(BYTE GameID);

	void RsgGameHall(BYTE GameID);
	void UnRsgGameHall(BYTE GameID);

	void RsgUser(DWORD UserID, BYTE GameID);
	void UnRsgUser(DWORD UserID);

	void OnClientLeave(BYTE CLientID);
	DWORD GetFreeGameServer();

	void OnGameClientLeave(BYTE gameID);
	void OnGameClientJoin(BYTE gameID);
private:
	HashMap<BYTE,DWORD>					m_GameServerMap;
	HashMap<BYTE,DWORD>					m_LogonServerMap;
	HashMap<BYTE, DWORD>					m_GameHallMap;
	//��ҵ��б�
	HashMap<DWORD, BYTE>				m_PlayerList;//UserID->Config						
};