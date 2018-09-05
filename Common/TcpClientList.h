//��װ���ʵ���
#pragma once
#include "TCPClient.h"
#include "NetClient.h"
//#define IP_MAX_LENGTH 16
struct TcpClientConnectOnce
{
	DWORD			TcpID;
	char			IpStr[IP_MAX_LENGTH];
	ClientInitData	ClientData;
};
struct TcpClientOnce
{
	DWORD				TcpID;
	TCPClient			m_Tcp;//���ӵ����ݿ������
	bool				m_TcpStates;
};
class TcpClientList
{
public:
	TcpClientList();
	virtual ~TcpClientList();
	//��ʼ��
	bool Init(std::vector<TcpClientConnectOnce>& pList);//ͬʱ���Ӷ��
	//��������
	bool Send(NetCmd *pCmd, bool bNoneUse);
	//�ر�
	void Shutdown();
	//�Ƿ�����
	bool IsConnected();
	//�ڲ��Զ����� ���� 
	NetCmd** CheckAllTcpClient(bool CheckNetStates);

	DWORD GetTcoClientSum(){ return m_TcpClientMap.size(); }
private:
	vector<TcpClientOnce*>					m_TcpClientMap;//��Ӧ��Map
	DWORD									m_SendIndex;
	NetCmd**								m_GetCmdArray;
};