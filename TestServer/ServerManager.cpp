#include "stdafx.h"
#include "ServerManager.h"
#define CMD_HEARBEAT 100
#define CMD_QUEUE 101

void ServerManager::Init()
{
	ServerInitData sid;
	sid.BuffSize = 1024;
	sid.CmdHearbeat = CMD_HEARBEAT;
	sid.ListenCount = 512;
	sid.MaxRecvCmdCount = 16;
	sid.MaxSendCountPerFrame = 1;
	sid.Port = 14333;
	sid.SocketRecvSize = 1024;
	sid.SocketSendSize = 1024;
	sid.Timeout = 5000;
	sid.RecvThreadNum = 4;
	sid.SendThreadNum = 2;
	sid.SleepTime = 1;
	sid.AcceptSleepTime = 1;
	sid.MaxAcceptNumPerFrame = 512;
	
	m_Server.SetCmdHandler(this);
	m_Server.Init(sid);
}
void ServerManager::Close()
{
	for (UINT i = 0; i < m_ClientList.size(); ++i)
	{
		ServerClientData *pcsd = m_ClientList[i];
		m_Server.Kick(pcsd);
	}
	m_Server.Shutdown();
}
void ServerManager::Update()
{
	while (m_NewClientList.HasItem())
	{
		m_ClientList.push_back(m_NewClientList.GetItem());
	}

	for (UINT i = 0; i < m_ClientList.size();)
	{
		ServerClientData *pcsd = m_ClientList[i];
		if (pcsd->Removed)
		{
			m_Server.Kick(pcsd);
			m_ClientList.erase(m_ClientList.begin() + i);
			continue;
		}
		++i;
	}

}
