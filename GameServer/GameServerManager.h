#pragma once
//struct TCPClientOnce//������
//{
//	TCPClient*		m_pClient;
//	bool			m_ClientStates;
//};
class GameServerManager //���ӵ�ȫ����Logon�� �������������ȡ��ȫ����Logon�б��
{
public:
	GameServerManager();
	virtual ~GameServerManager();
	void OnDestroy();

	void SendNetCmdToLogon(BYTE LogonID, NetCmd* pCmd);
	void SendNetCmdToAllLogon(NetCmd* pCmd);

	//void OnRsgGameToCenter();//���Լ�ע�ᵽCenterȥ
	//void OnRsgGameToCenterResult(CG_Cmd_RsgGame* pMsg);
	void OnGameLeavCenter();//���Լ��Ͽ���Center�����ӵ�ʱ��
	void OnLogonLeaveCenter(BYTE LogonID);//�������������gameServer һ��Logon�뿪����������� gameServer�� ���޳���
	void OnLogonRsgCenter(BYTE LogonID);

	//����Logon������Ĵ��� ��Ϊ�ж�� ���Խ��д���
	void OnHandleLogonMsg();
private:
	//HashMap<BYTE, TCPClientOnce>		m_LogonMap;//logon�ļ��� 
};