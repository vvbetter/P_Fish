#include "stdafx.h"
#include "ClientManager.h"
#define CMD_HEARBEAT 100
#define CMD_QUEUE 101



UINT WINAPI ThreadFunc(void *p)
{
	((ClientManager*)p)->ThreadProcedure();
	return 0;
}
bool ClientManager::Init(const char *ip, int num)
{
	ClientInitData data;
	data.BuffSize = 1024;
	data.CmdHearbeat = CMD_HEARBEAT;
	data.Port = 14333;
	data.SocketRecvSize = 1024;
	data.SocketSendSize = 1024;
	data.Timeout = 1000;
	data.SleepTime = 1;
	data.ThreadNum = 2;
	//ip =  "115.159.27.236";
	num = 100;
	//g_Client.Init("115.159.27.236", 100, data);
	//m_Client.Init(ip, 10000, data);
	//return true;
	strcpy_s(m_IP, sizeof(m_IP), ip);
	m_Num = num;

	for (int i = 0; i < m_Num; ++i)
	{
		TCPClient *pClient = new TCPClient;
		m_ClientList.push_back(pClient);
	}
	::_beginthreadex(0, 0, ThreadFunc, this, 0, 0);
	return true;
}
void ClientManager::Close()
{
	//m_Client.Shutdown();
	for (uint i = 0; i < m_ClientList.size(); ++i)
	{
		m_ClientList[i]->Shutdown();
		delete m_ClientList[i];
	}
}
void ClientManager::ThreadProcedure()
{
	ClientInitData data;
	data.BuffSize = 1024;
	data.CmdHearbeat = CMD_HEARBEAT;
	data.Port = 14333;
	data.SocketRecvSize = 1024;
	data.SocketSendSize = 1024;
	data.Timeout = 5000;
	data.SleepTime = 10;
	int idx = 0;
	while (idx < m_Num)
	{
		for (int i = 0; idx < m_Num && i < 3000; ++idx, ++i)
		{
			TCPClient *pClient = m_ClientList[idx];
			if (!pClient->Init(m_IP, data))
			{
				Log(L"Á¬½ÓÊ§°Ü:%d", idx);
			}
			else
			{
				NetCmd *nc = (NetCmd*)malloc(sizeof(NetCmd));
				nc->SetCmdSize(sizeof(nc));
				nc->SetCmdType(0x00ff);
				pClient->Send(nc, false);
			}
		}
		Sleep(10);
	}
}