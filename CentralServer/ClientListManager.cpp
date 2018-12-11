#include "Stdafx.h"
#include "ClientListManager.h"
#include "FishServer.h"
ClientListManager::ClientListManager()
{
	m_PlayerList.clear();
	m_GameServerMap.clear();
	m_LogonServerMap.clear();
}
ClientListManager::~ClientListManager()
{

}
void ClientListManager::SendNetCmdToGameServer(BYTE GameID, NetCmd* pCmd)
{
	//������͵�GameServerȥ
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(GameID);
	if (!pClient)
	{
		//ASSERT(false);
		return;
	}
	g_FishServer.SendNetCmdToClient(pClient, pCmd);
}
void ClientListManager::SendNetCmdToLogonServer(BYTE LogonID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(LogonID);
	if (!pClient)
	{
		//ASSERT(false);
		return;
	}
	g_FishServer.SendNetCmdToClient(pClient, pCmd);
}
void ClientListManager::SendNetCmdToAllGameServer(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (m_GameServerMap.empty())
		return;
	HashMap<BYTE,DWORD>::iterator Iter = m_GameServerMap.begin();
	for (; Iter != m_GameServerMap.end(); ++Iter)
	{
		SendNetCmdToGameServer(Iter->first, pCmd);
	}
}
DWORD ClientListManager::GetGameServerSum()
{
	return m_GameServerMap.size();
}
DWORD ClientListManager::GetLogonServerSum()
{
	return m_LogonServerMap.size();
}

void ClientListManager::SendNetCmdToAllLogonServer(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (m_LogonServerMap.empty())
	{
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_LogonServerMap.begin();
	for (; Iter != m_LogonServerMap.end(); ++Iter)
	{
		SendNetCmdToLogonServer(Iter->first, pCmd);
	}
}
void ClientListManager::RsgLogon(BYTE LogonID)
{
	//��һ��Logonע�������ʱ��
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(LogonID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_LogonServerMap.find(LogonID);
	if (Iter != m_LogonServerMap.end())
	{
		ASSERT(false);
		return;
	}
	m_LogonServerMap.insert(HashMap<BYTE, DWORD>::value_type(LogonID,0));
	//��Ϊһ��Logon������ ������Ҫ��ȫ����ҵ��б��͸�Logon
	DWORD MsgPage = sizeof(CL_Cmd_RsgLogon)+(m_PlayerList.size() - 1) * sizeof(tagPlayerInfo);//5 ������  Ӧ�����зְ�����
	CL_Cmd_RsgLogon * msgCL = (CL_Cmd_RsgLogon*)malloc(MsgPage);
	if (!msgCL)
	{
		ASSERT(false);
		return;
	}
	msgCL->SetCmdType(GetMsgType(Main_Server, CL_RsgLogon));
	HashMap<DWORD, BYTE>::iterator IterRole = m_PlayerList.begin();
	for (int i = 0; IterRole != m_PlayerList.end(); ++IterRole, ++i)
	{
		msgCL->Array[i].dwUserID = IterRole->first;
		msgCL->Array[i].GameConfigID = IterRole->second;
	}

	std::vector<CL_Cmd_RsgLogon*> pVec;
	SqlitMsg(msgCL, MsgPage, m_PlayerList.size(),false, pVec);
	free(msgCL);
	if (!pVec.empty())
	{
		std::vector<CL_Cmd_RsgLogon*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			SendNetCmdToLogonServer(LogonID, *Iter);
			free(*Iter);
		}
		pVec.clear();
	}

	//��ȫ����GameServer������ ���͵�Logon����ȥ
	{
		DWORD MsgPage = sizeof(CL_Cmd_GameInfo)+(m_GameServerMap.size() - 1) * sizeof(BYTE);//5 ������  Ӧ�����зְ�����
		CL_Cmd_GameInfo * msgCL = (CL_Cmd_GameInfo*)malloc(MsgPage);
		if (!msgCL)
		{
			ASSERT(false);
			return;
		}
		msgCL->SetCmdType(GetMsgType(Main_Server, CL_GameInfo));
		HashMap<BYTE, DWORD>::iterator IterRole = m_GameServerMap.begin();
		for (int i = 0; IterRole != m_GameServerMap.end(); ++IterRole, ++i)
		{
			msgCL->Array[i] = IterRole->first;
		}

		std::vector<CL_Cmd_GameInfo*> pVec;
		SqlitMsg(msgCL, MsgPage, m_GameServerMap.size(), false, pVec);
		free(msgCL);
		if (!pVec.empty())
		{
			std::vector<CL_Cmd_GameInfo*>::iterator Iter = pVec.begin();
			for (; Iter != pVec.end(); ++Iter)
			{
				SendNetCmdToLogonServer(LogonID, *Iter);
				free(*Iter);
			}
			pVec.clear();
		}
	}

	//��Logon ������ ���͸� ����ȫ����GameServerȥ
	/*CG_Cmd_RsgLogon msgCG;
	SetMsgInfo(msgCG, GetMsgType(Main_Server, CG_RsgLogon), sizeof(CG_Cmd_RsgLogon));
	msgCG.LogonConfigID = LogonID;
	SendNetCmdToAllGameServer(&msgCG);*/
	Log("Logonע����� ID:%d",LogonID);
}
void ClientListManager::UnRsgLogon(BYTE LogonID)
{
	//��һ��Logon�뿪Center��ʱ�� ������δ�����?
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(LogonID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter= m_LogonServerMap.find(LogonID);
	if (Iter == m_LogonServerMap.end())
	{
		ASSERT(false);
		return;
	}

	//֪ͨȫ����GameServer Logon�뿪��Center GameServer�Ͽ���Logon������
	/*CG_Cmd_UnRsgLogon msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, CG_UnRsgLogon), sizeof(CG_Cmd_UnRsgLogon));
	msg.LogonConfigID = LogonID;
	SendNetCmdToAllGameServer(&msg);*/

	m_LogonServerMap.erase(Iter);

	Log("Logon�뿪 ID:%d",LogonID);
}
void ClientListManager::RsgGame(BYTE GameID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(GameID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_GameServerMap.find(GameID);
	if (Iter != m_GameServerMap.end())
	{
		ASSERT(false);
		return;
	}
	m_GameServerMap.insert(HashMap<BYTE,DWORD>::value_type(GameID,0));
	//��һ��GameServerע�������ʱ�� ������Ҫ��ȫ����Logon�����ݷ��͸�GameServer
	//g_FishServer.GetMonthManager().OnSendMonthDataToGame(GameID);//������������ͬ����Game

	//��Ϊһ��Gameע����� ��Ҫ����ǰGameע�ᵽLogon����ȥ
	CL_Cmd_GameInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, CL_GameInfo), sizeof(CL_Cmd_GameInfo));
	msg.Sum = 1;
	msg.Array[0] = GameID;
	msg.States = MsgBegin | MsgEnd;
	SendNetCmdToAllLogonServer(&msg);//���͸�ȫ����Logonȥ

	//g_FishServer.GetMonthManager().OnGameRsg(GameID);

	/*DWORD PageSize = sizeof(CG_Cmd_RsgGame)+(m_LogonServerMap.size() - 1)*sizeof(BYTE);
	CG_Cmd_RsgGame* msg = (CG_Cmd_RsgGame*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdType(GetMsgType(Main_Server, CG_RsgGame));
	Iter = m_LogonServerMap.begin();
	for (DWORD i = 0; Iter != m_LogonServerMap.end(); ++Iter, ++i)
	{
		msg->Array[i] = Iter->first;
	}
	std::vector<CG_Cmd_RsgGame*> pVec;
	SqlitMsg(msg, PageSize, m_LogonServerMap.size(), false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<CG_Cmd_RsgGame*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			SendNetCmdToGameServer(GameID, *Iter);
			free(*Iter);
		}
		pVec.clear();
	}*/
	Log("Gameע����� ID:%d",GameID);
}
void ClientListManager::UnRsgGame(BYTE GameID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(GameID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_GameServerMap.find(GameID);
	if (Iter == m_GameServerMap.end())
	{
		ASSERT(false);
		return;
	}
	//��һ��GameServer�뿪Center��ʱ�� ����ǰGameServer ���͸�ȫ����Logon ��Logonɾ����
	CL_Cmd_UnRsgGame msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, CL_UnRsgGame), sizeof(CL_Cmd_UnRsgGame));
	msg.GameConfigID = GameID;
	SendNetCmdToAllLogonServer(&msg);

	m_GameServerMap.erase(Iter);

	Log("Game�뿪 ID:%d",GameID);
}
void ClientListManager::RsgUser(DWORD UserID, BYTE GameID)
{
	//��һ����ҽ�����ʱ�� ���ǽ��д���
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(GameID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//һ����ҽ�������������� ���뵽����
	HashMap<DWORD,BYTE>::iterator IterRole = m_PlayerList.find(UserID);
	if (IterRole != m_PlayerList.end())
	{
		ASSERT(false);
		return;
	}
	m_PlayerList.insert(HashMap<DWORD, BYTE>::value_type(UserID, GameID));//��Ҷ��ڵ�gameServer
	//����ȫ����Logon ��ҽ��� ������
	CL_Cmd_RsgUser msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, CL_RsgUser), sizeof(CL_Cmd_RsgUser));
	msg.PlayerInfo.dwUserID = UserID;
	msg.PlayerInfo.GameConfigID = GameID;
	SendNetCmdToAllLogonServer(&msg);//����ȫ����Logon������
	//SendNetCmdToAllGameServer(&msg);

	//Log("��� ID: %d  ������������� ��GameServerIDΪ: %d",UserID,GameID);
}
void ClientListManager::UnRsgUser(DWORD UserID)
{
	HashMap<DWORD, BYTE>::iterator IterRole = m_PlayerList.find(UserID);
	if (IterRole == m_PlayerList.end())
	{
		ASSERT(false);
		return;
	}
	//����뿪��ʱ��
	CL_Cmd_UnRsgUser msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, CL_UnRsgUser), sizeof(CL_Cmd_UnRsgUser));
	msg.dwUserID = UserID;
	SendNetCmdToAllLogonServer(&msg);//����ȫ����Logon������
	m_PlayerList.erase(IterRole);

	//Log("��� ID: %d  �뿪���������", UserID);
}
void ClientListManager::OnClientLeave(BYTE CLientID)//����½������ ���� ����������뿪��ʱ��
{
	//��һ����Ϸ�������뿪��ʱ�� 
	HashMap<BYTE,DWORD>::iterator Iter = m_GameServerMap.find(CLientID);
	if (Iter != m_GameServerMap.end())
	{
		//GameServer �뿪
		UnRsgGame(CLientID);
		//�õ�ǰƽ̨�ϵ���
		g_FishServer.GetRoleManager().OnPlazaLeave(CLientID);
		return;
	}
	//�����뿪
	auto hallIt = m_GameHallMap.find(CLientID);
	if (hallIt != m_GameHallMap.end())
	{
		UnRsgGameHall(CLientID);
		return;
	}
}

void ClientListManager::RsgGameHall(BYTE HallID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(HallID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_GameServerMap.find(HallID);
	if (Iter != m_GameServerMap.end())
	{
		ASSERT(false);
		return;
	}
	m_GameHallMap.insert(HashMap<BYTE, DWORD>::value_type(HallID, 0));
	Log("��Ϸ����������Ϸ IP:%d.%d.%d.%d", pClient->IP & 0xff, (pClient->IP >> 8) & 0xff, (pClient->IP >> 16) & 0xff, (pClient->IP >> 24) & 0xff);
}
void ClientListManager::UnRsgGameHall(BYTE HallID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(HallID);
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, DWORD>::iterator Iter = m_GameHallMap.find(HallID);
	if (Iter == m_GameHallMap.end())
	{
		ASSERT(false);
		return;
	}
	//����Ϸ�����뿪Center��ʱ��
	m_GameHallMap.erase(Iter);
	Log("��Ϸ�����뿪 IP:%d.%d.%d.%d", pClient->IP & 0xff, (pClient->IP >> 8) & 0xff, (pClient->IP >> 16) & 0xff, (pClient->IP >> 24) & 0xff);
}

void ClientListManager::SendNetCmdToGameHall(BYTE HallID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(HallID);
	if (!pClient)
	{
		//ASSERT(false);
		return;
	}
	g_FishServer.SendNetCmdToClient(pClient, pCmd);
}

DWORD ClientListManager::GetFreeGameServer()
{
	if (m_GameServerMap.size() == 0)
	{
		ASSERT(false);
		return -1;
	}
	DWORD gameID = 0xffffffff;
	DWORD sumPlayer = 0xffffffff;
	for (auto it = m_GameServerMap.begin(); it != m_GameServerMap.end(); ++it)
	{
		if (it->second < sumPlayer)
		{
			sumPlayer = it->second;
			gameID = it->first;
		}
	}
	return gameID;
}

void ClientListManager::OnGameClientLeave(BYTE gameID)
{
	auto it = m_GameServerMap.find(gameID);
	if (it != m_GameServerMap.end() && it->second > 0)
	{
		--it->second;
	}
}

void ClientListManager::OnGameClientJoin(BYTE gameID)
{
	auto it = m_GameServerMap.find(gameID);
	if (it != m_GameServerMap.end())
	{
		++it->second;
	}
}