#include "stdafx.h"
#include "GameServerManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
GameServerManager::GameServerManager()
{

}
GameServerManager::~GameServerManager()
{
	OnDestroy();
}
void GameServerManager::SendNetCmdToLogon(BYTE LogonID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.find(LogonID);
	if (Iter == m_LogonMap.end())
	{
		ASSERT(false);
		return;
	}
	if (!Iter->second.m_pClient->IsConnected())
	{
		ASSERT(false);
		return;
	}
	Iter->second.m_pClient->Send(pCmd, false);
}
void GameServerManager::SendNetCmdToAllLogon(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.begin();
	for (; Iter != m_LogonMap.end(); ++Iter)
	{
		if (!Iter->second.m_pClient->IsConnected())
			continue;
		Iter->second.m_pClient->Send(pCmd, false);
	}
}
//void GameServerManager::OnRsgGameToCenter()
//{
//	//���Լ�ע�ᵽGameServerȥ
//	//GC_Cmd_RsgGame msg;
//	//SetMsgInfo(msg, GetMsgType(Main_Server, GC_RsgGame), sizeof(GC_Cmd_RsgGame));
//	//msg.GameConfigID = g_FishServer.GetGameIndex();
//	//g_FishServer.SendNetCmdToCenter(&msg);//������͵����������ȥ
//}
//void GameServerManager::OnRsgGameToCenterResult(CG_Cmd_RsgGame* pMsg)
//{
//	//GameServer  ע������������ķ���ֵ
//	if (!pMsg)
//	{
//		ASSERT(false);
//		return;
//	}
//	if ((pMsg->States & MsgBegin) != 0)
//	{
//		m_LogonMap.clear();
//	}
//	for (WORD i = 0; i < pMsg->Sum; ++i)
//	{
//		BYTE LogonConfigID = pMsg->Array[i];//Logon��Ӧ������ID
//		//���Ƿ������� ���ӵ�Logonȥ
//		TCPClientOnce pOnce;
//		LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(LogonConfigID);
//		if (!pLogonConfig)
//		{
//			ASSERT(false);
//			continue;
//		}
//		pOnce.m_pClient = new TCPClient();
//		ClientInitData pLogonData;
//		pLogonData.BuffSize = pLogonConfig->LogonServerBuffSize;
//		pLogonData.Port = pLogonConfig->LogonServerListenPort;
//		pLogonData.SleepTime = pLogonConfig->LogonServerSleepTime;
//		pLogonData.SocketRecvSize = pLogonConfig->LogonServerRecvBuffSize;
//		pLogonData.SocketSendSize = pLogonConfig->LogonServerSendBuffSize;
//		pLogonData.ThreadNum = 1;
//		pLogonData.Timeout = pLogonConfig->LogonServerTimeOut;
//		pLogonData.CmdHearbeat = 0;
//		pLogonData.SendArraySize = pLogonConfig->LogonServerMaxSendCmdCount;
//		pLogonData.RecvArraySize = pLogonConfig->LogonServerMaxRecvCmdCount;
//
//		m_LogonMap.insert(HashMap<BYTE, TCPClientOnce>::value_type(LogonConfigID, pOnce));
//
//		BYTE GameID = g_FishServer.GetGameIndex();
//
//		if (!pOnce.m_pClient->Init(pLogonConfig->LogonServerListenIP, pLogonData, (void*)&GameID, sizeof(BYTE)))
//		{
//			g_FishServer.ShowInfoToWin("��½����������ʧ��");
//			pOnce.m_ClientStates = false;
//		}
//		else
//		{
//			pOnce.m_ClientStates = true;
//
//			GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(g_FishServer.GetGameIndex());
//			if (!pGameConfig)
//				return;
//
//			//GL_Cmd_RsgGameServer msg;
//			//SetMsgInfo(msg, GetMsgType(Main_Logon, GL_RsgGameServer), sizeof(GL_Cmd_RsgGameServer));
//			////��ȡ������IP��ַ ���͸�LogonServer
//			//msg.GameConfigID = g_FishServer.GetGameIndex();
//			//SendNetCmdToLogon(LogonConfigID,&msg);
//
//			g_FishServer.ShowInfoToWin("�Ѿ����½���������ӳɹ���");
//		}
//		
//	}
//}
void GameServerManager::OnGameLeavCenter()
{
	//��GameServer������������Ͽ�;���ӵ�ʱ�� ���ǶϿ�ȫ����Logon������ ��GameServer�������ӵ�ʱ�� ���������ӵ�
	OnDestroy();
}
//void GameServerManager::OnLogonLeaveCenter(BYTE LogonID)
//{
//	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.find(LogonID);
//	if (Iter == m_LogonMap.end())
//	{
//		ASSERT(false);
//		return;
//	}
//	Iter->second.m_pClient->Shutdown();
//	delete Iter->second.m_pClient;
//	m_LogonMap.erase(Iter);
//}
//void GameServerManager::OnLogonRsgCenter(BYTE LogonID)
//{
//	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.find(LogonID);
//	if (Iter != m_LogonMap.end())
//	{
//		ASSERT(false);
//		return;
//	}
//	//һ���µ�Logon����
//	TCPClientOnce pOnce;
//	LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(LogonID);
//	if (!pLogonConfig)
//	{
//		ASSERT(false);
//		return;
//	}
//	pOnce.m_pClient = new TCPClient();
//	ClientInitData pLogonData;
//	pLogonData.BuffSize = pLogonConfig->LogonServerBuffSize;
//	pLogonData.Port = pLogonConfig->LogonServerListenPort;
//	pLogonData.SleepTime = pLogonConfig->LogonServerSleepTime;
//	pLogonData.SocketRecvSize = pLogonConfig->LogonServerRecvBuffSize;
//	pLogonData.SocketSendSize = pLogonConfig->LogonServerSendBuffSize;
//	pLogonData.ThreadNum = 1;
//	pLogonData.Timeout = pLogonConfig->LogonServerTimeOut;
//	pLogonData.CmdHearbeat = 0;
//	pLogonData.SendArraySize = pLogonConfig->LogonServerMaxSendCmdCount;
//	pLogonData.RecvArraySize = pLogonConfig->LogonServerMaxRecvCmdCount;
//
//	m_LogonMap.insert(HashMap<BYTE, TCPClientOnce>::value_type(LogonID, pOnce));
//
//	BYTE GameID = g_FishServer.GetGameIndex();
//	if (!pOnce.m_pClient->Init(pLogonConfig->LogonServerListenIP, pLogonData, (void*)&GameID, sizeof(BYTE)))
//	{
//		g_FishServer.ShowInfoToWin("��½����������ʧ��");
//		pOnce.m_ClientStates = false;
//	}
//	else
//	{
//		pOnce.m_ClientStates = true;
//
//		//GL_Cmd_RsgGameServer msg;
//		//SetMsgInfo(msg, GetMsgType(Main_Logon, GL_RsgGameServer), sizeof(GL_Cmd_RsgGameServer));
//		////��ȡ������IP��ַ ���͸�LogonServer
//		//GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(g_FishServer.GetGameIndex());
//		//if (!pGameConfig)
//		//	return;
//		//msg.GameConfigID = g_FishServer.GetGameIndex();
//		//SendNetCmdToLogon(LogonID, &msg);
//
//
//		g_FishServer.ShowInfoToWin("�Ѿ����½���������ӳɹ���");
//	}
//	
//}
//void GameServerManager::OnDestroy()
//{
//	if (m_LogonMap.empty())
//	{
//		return;
//	}
//	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.begin();
//	for (; Iter != m_LogonMap.end(); ++Iter)
//	{
//		Iter->second.m_pClient->Shutdown();
//		delete Iter->second.m_pClient;
//	}
//	m_LogonMap.clear();
//}
//void GameServerManager::OnHandleLogonMsg()
//{
//	if (m_LogonMap.empty())
//	{
//		return;
//	}
//	HashMap<BYTE, TCPClientOnce>::iterator Iter = m_LogonMap.begin();
//	for (; Iter != m_LogonMap.end(); ++Iter)
//	{
//		if (Iter->second.m_pClient->IsConnected())
//		{
//			if (!Iter->second.m_ClientStates)
//			{
//				Iter->second.m_ClientStates = true;
//				g_FishServer.ShowInfoToWin("���½���������ӳɹ�");
//				//����һ��ע�������
//				//GL_Cmd_RsgGameServer msg;
//				//SetMsgInfo(msg, GetMsgType(Main_Logon, GL_RsgGameServer), sizeof(GL_Cmd_RsgGameServer));
//				////��ȡ������IP��ַ ���͸�LogonServer
//				//GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(g_FishServer.GetGameIndex());
//				//if (!pGameConfig)
//				//	return;
//				//msg.GameConfigID = g_FishServer.GetGameIndex();
//				//SendNetCmdToLogon(Iter->first, &msg);//��GameServer��ע�ᷢ�͵�Logonȥ
//			}
//			NetCmd* pCmd = Iter->second.m_pClient->GetCmd();
//			int Sum = 0;
//			while (pCmd && Sum < Msg_OnceSum)
//			{
//				g_FishServer.HandleLogonMsg(Iter->first, pCmd);//��������д���
//				free(pCmd);
//				++Sum;
//				pCmd = Iter->second.m_pClient->GetCmd();
//			}
//			if (pCmd)
//			{
//				g_FishServer.HandleLogonMsg(Iter->first, pCmd);//��������д���
//				free(pCmd);
//			}
//		}
//		else
//		{
//			if (Iter->second.m_ClientStates)
//			{
//				g_FishServer.ShowInfoToWin("���½�������Ͽ�����");
//				Iter->second.m_ClientStates = false;
//			}
//			//��������DB
//			//���������ļ�ID ���д�������
//			BYTE LogonConfigID = Iter->first;
//
//			LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(LogonConfigID);
//			if (!pLogonConfig)
//			{
//				ASSERT(false);
//				continue;
//			}
//			ClientInitData pLogonData;
//			pLogonData.BuffSize = pLogonConfig->LogonServerBuffSize;
//			pLogonData.Port = pLogonConfig->LogonServerListenPort;
//			pLogonData.SleepTime = pLogonConfig->LogonServerSleepTime;
//			pLogonData.SocketRecvSize = pLogonConfig->LogonServerRecvBuffSize;
//			pLogonData.SocketSendSize = pLogonConfig->LogonServerSendBuffSize;
//			pLogonData.ThreadNum = 1;
//			pLogonData.Timeout = pLogonConfig->LogonServerTimeOut;
//			pLogonData.CmdHearbeat = 0;
//			pLogonData.SendArraySize = pLogonConfig->LogonServerMaxSendCmdCount;
//			pLogonData.RecvArraySize = pLogonConfig->LogonServerMaxRecvCmdCount;
//
//			BYTE GameID = g_FishServer.GetGameIndex();
//			if (!Iter->second.m_pClient->Init(pLogonConfig->LogonServerListenIP, pLogonData, (void*)&GameID, sizeof(BYTE)))
//			{
//				g_FishServer.ShowInfoToWin("��½����������ʧ��");
//				Iter->second.m_ClientStates = false;
//			}
//		}
//	}
//}