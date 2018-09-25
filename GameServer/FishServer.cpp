#include "stdafx.h"
#include "FishServer.h"
#include "FishLogic\NetCmd.h"
#include "..\CommonFile\FishServerConfig.h"
#include "..\CommonFile\ip.h"
#include<list>
FishServer g_FishServer;
void SendLogDB(NetCmd* pCmd)
{
	g_FishServer.SendNetCmdToLogDB(pCmd);
}
INT FishServer::m_ratioValue = 0;
FishServer::FishServer()
{
	m_IsClose = false;
	m_CenterTcpStates = false;
	m_DBTcpStates = false;
	m_DBSaveTcpStates = false;
	m_UserIndex = 0;
	m_GameNetworkID = 0;

	m_DBLogTcpStates = false;
	m_IsReloadConfig = false;
}
FishServer::~FishServer()
{
	m_ClintList.clear();
	m_UserIndex = 0;
	
}

bool FishServer::InitServer(int Index)
{
	m_pDump.OnInit();
	//1.��ȡ�����ļ� ���������õ������ļ�
	if (!g_FishServerConfig.LoadServerConfigFile())
	{
		Log("�����������ļ���ȡʧ��");
		return false;
	}

	m_GameNetworkID = ConvertIntToBYTE(Index);
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}

	//4.���ֹ��������г�ʼ��
	if (!m_FishConfig.LoadConfigFilePath())
	{
		ASSERT(false);
		return false;
	}
	//5.
	if (!m_RoleManager.OnInit())
	{
		ASSERT(false);
		return false;
	}
	Log("��ҹ�������ʼ���ɹ�");
	m_TableManager.OnInit();
	Log("���ӹ�������ʼ���ɹ�");
	if (!ConnectDB())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectDB())
				break;
		}
	}
	if (!ConnectSaveDB())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectSaveDB())
				break;
		}
	}
	m_RobotManager.OnLoadAllGameRobot(pGameConfig->BeginRobotUserID, pGameConfig->EndRobotUserID);//�����µ����
	if (!ConnectCenter())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectCenter())
				break;
		}
	}
	if (!ConnectClient())
	{
		ASSERT(false);
		return false;
	}
	//������������
	{
		DBR_Cmd_GameserverStart msg;
		SetMsgInfo(msg, DBR_GameserverStart, sizeof(DBR_Cmd_GameserverStart));
		msg.game_id = m_GameNetworkID;
		g_FishServer.SendNetCmdToDB(&msg);
	}
	Log("GameServer�����ɹ�");
	InitTestClientData();
	return true;
}

void FishServer::InitTestClientData()
{
//	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
//	DWORD  MAX_TEST_CLIENT = pGameConfig->TestRobotNumber;
//#define  UDP_SOCKET_PORT_START 50000
//	for (int64 i = 1; i <= MAX_TEST_CLIENT; ++i)
//	{
//		CreateSocketData csd;
//		memset(&csd, 0, sizeof(CreateSocketData));
//		csd.uid = _HTONLL_(i);
//		bool ret = m_ClientTcp.CreateNewClientSocket(csd, 0, static_cast<USHORT>(UDP_SOCKET_PORT_START + i));
//		m_ratioValue = 1000;
//		tagClientUserData udata;
//		memset(&udata, 0, sizeof(tagClientUserData));
//		udata.money1 = 10000000;
//		sprintf_s(udata.name, "%s%d", "test ", i);
//		sprintf_s(udata.icon, "%s", "head_female1");
//		udata.sex = 2;
//		udata.vip = i % 5;
//		udata.headIcon = 1;
//		m_HalldataCach[_NTOHLL(csd.uid)] = udata;
//		if (i == 1 || i == MAX_TEST_CLIENT)
//		{
//			Log("Test Client Socket ip = %s, Prot = %d", g_FishServerConfig.GetGameServerConfig(m_GameNetworkID)->GameListenIP, csd.Port);
//		}
//	}
}
void FishServer::Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	if (ServerID == m_GameNetworkID)
	{
		//Log("һ���ͻ����뿪��Ϸ");
		return;//�ͻ����뿪��������
	}
	//��� �ͻ����뿪��״̬
	switch (rt)
	{
	case REMOVE_NONE:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��ΪNone", pClient->OutsideExtraData);
		}
		break;
	case REMOVE_NORMAL:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ��ͨ",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECV_ERROR:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ���մ���",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_SEND_ERROR:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ���ʹ���",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_TIMEOUT:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ��ʱ",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SEND_OVERFLOW:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_RECV_OVERFLOW:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SIZE_ERROR:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ�����С",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECVBACK_NOT_SPACE:
		{
			Log("�ͻ��� ID:%d �뿪ԭ��Ϊ���ջ��������",pClient->OutsideExtraData);
		}
		break;
	}
}
bool FishServer::ConnectDB()
{
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}


	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pGameConfig->DBNetworkID);
	if (!pDBConfig)
	{
		ASSERT(false);
		return false;
	}
	ClientInitData pDBData;
	pDBData.BuffSize = pDBConfig->BuffSize;
	pDBData.Port = pDBConfig->DBListenPort;
	pDBData.SleepTime = pDBConfig->SleepTime;
	pDBData.SocketRecvSize = pDBConfig->RecvBuffSize;
	pDBData.SocketSendSize = pDBConfig->SendBuffSize;
	pDBData.ThreadNum = 1;
	pDBData.Timeout = pDBConfig->TimeOut;
	pDBData.CmdHearbeat = 0;
	pDBData.SendArraySize = pDBConfig->MaxSendCmdCount;
	pDBData.RecvArraySize = pDBConfig->MaxRecvCmdCount;
	Reg_Server rs;
	SetMsgInfo(rs, GetMsgType(Main_Server, SR_ServerInfo), sizeof(Reg_Server));
	rs.NetWorkID = m_GameNetworkID;
	if (!m_DBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&rs,sizeof(Reg_Server)))
	{
		Log("DB����������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
bool FishServer::ConnectSaveDB()
{
	//���ӵ��������ݿ� �� ��ͨ�Ķ�ȡ �޸����ݿⲻ��һ�����ݿ�
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}

	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pGameConfig->SaveDBNetworkID);
	if (!pDBConfig)
	{
		ASSERT(false);
		return false;
	}
	ClientInitData pDBData;
	pDBData.BuffSize = pDBConfig->BuffSize;
	pDBData.Port = pDBConfig->DBListenPort;
	pDBData.SleepTime = pDBConfig->SleepTime;
	pDBData.SocketRecvSize = pDBConfig->RecvBuffSize;
	pDBData.SocketSendSize = pDBConfig->SendBuffSize;
	pDBData.ThreadNum = 1;
	pDBData.Timeout = pDBConfig->TimeOut;
	pDBData.CmdHearbeat = 0;
	pDBData.SendArraySize = pDBConfig->MaxSendCmdCount;
	pDBData.RecvArraySize = pDBConfig->MaxRecvCmdCount;
	Reg_Server rs;
	SetMsgInfo(rs, GetMsgType(Main_Server, SR_ServerInfo), sizeof(Reg_Server));
	rs.NetWorkID = m_GameNetworkID;
	if (!m_DBSaveTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&rs, sizeof(Reg_Server)))
	{
		Log("SaveDB����������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBSaveTcp);
	return true;
}
bool FishServer::ConnectLogDB()
{
	//���ӵ��������ݿ� �� ��ͨ�Ķ�ȡ �޸����ݿⲻ��һ�����ݿ�
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}

	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pGameConfig->LogDBNetworkID);
	if (!pDBConfig)
	{
		ASSERT(false);
		return false;
	}
	ClientInitData pDBData;
	pDBData.BuffSize = pDBConfig->BuffSize;
	pDBData.Port = pDBConfig->DBListenPort;
	pDBData.SleepTime = pDBConfig->SleepTime;
	pDBData.SocketRecvSize = pDBConfig->RecvBuffSize;
	pDBData.SocketSendSize = pDBConfig->SendBuffSize;
	pDBData.ThreadNum = 1;
	pDBData.Timeout = pDBConfig->TimeOut;
	pDBData.CmdHearbeat = 0;
	pDBData.SendArraySize = pDBConfig->MaxSendCmdCount;
	pDBData.RecvArraySize = pDBConfig->MaxRecvCmdCount;
	Reg_Server rs;
	SetMsgInfo(rs, GetMsgType(Main_Server, SR_ServerInfo), sizeof(Reg_Server));
	rs.NetWorkID = m_GameNetworkID;
	if (!m_DBLogTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&rs, sizeof(Reg_Server)))
	{
		Log("LogDB����������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBLogTcp);
	return true;
}
bool FishServer::ConnectCenter()
{
	CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
	if (!pCenterConfig)
	{
		ASSERT(false);
		return false;
	}
	ClientInitData pCenterData;
	pCenterData.BuffSize = pCenterConfig->BuffSize;
	pCenterData.Port = pCenterConfig->CenterListenPort;
	pCenterData.SleepTime = pCenterConfig ->SleepTime ;
	pCenterData.SocketRecvSize = pCenterConfig->RecvBuffSize;
	pCenterData.SocketSendSize = pCenterConfig->SendBuffSize;
	pCenterData.ThreadNum = 1;
	pCenterData.Timeout = pCenterConfig->TimeOut;
	pCenterData.CmdHearbeat = 0;
	pCenterData.SendArraySize = pCenterConfig->MaxSendCmdCount;
	pCenterData.RecvArraySize = pCenterConfig->MaxRecvCmdCount;
	Reg_Server rs;
	SetMsgInfo(rs, GetMsgType(Main_Server, SR_ServerInfo), sizeof(Reg_Server));
	rs.NetWorkID = m_GameNetworkID;
	if (!m_CenterTcp.Init(pCenterConfig->CenterListenIP, pCenterData, (void*)&rs, sizeof(Reg_Server)))
	{
		Log("�������������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_CenterTcp);
	return true;
}
bool FishServer::ConnectClient()
{
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}
	ServerInitData pCLientData;
	pCLientData.ServerID = pGameConfig->NetworkID;
	pCLientData.BuffSize = pGameConfig->BuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.Port = pGameConfig->GameListenPort;
	pCLientData.SocketRecvSize = pGameConfig->RecvBuffSize;
	pCLientData.SocketSendSize = pGameConfig->SendBuffSize;
	pCLientData.Timeout = pGameConfig->TimeOut;
	pCLientData.RecvThreadNum = pGameConfig->RecvThreadSum;
	pCLientData.SendThreadNum = pGameConfig->SendThreadSum;
	pCLientData.SleepTime = pGameConfig->SleepTime;
	pCLientData.AcceptSleepTime = pGameConfig->AcceptSleepTime;
	pCLientData.AcceptRecvData = false;		//��ʾ���ճ�ʼ����

	pCLientData.SceneHearbeatTick = pGameConfig->SceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pGameConfig->MaxSendCmdCount;
	pCLientData.MaxAcceptNumPerFrame = pGameConfig->MaxAcceptNumPerFrame;
	pCLientData.MaxRecvCmdCount = pGameConfig->MaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pGameConfig->MaxSendCountPerFrame;
	pCLientData.ListenCount = pGameConfig->ListenCount;

	m_ClientTcp.SetCmdHandler(this);

	m_GameNetworkID = pGameConfig->NetworkID;

	if (!m_ClientTcp.Init(pCLientData, false))
	{
		Log("�������������ʧ��");
		return false;
	}
	
	
	return true;
}
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//���������ֻ����FTP �� DB�����ӳɹ���Ź���
	bool IsNeed = m_DBTcp.IsConnected() && m_CenterTcp.IsConnected()/* && m_OperatorTcp.IsConnected()*/ && m_DBSaveTcp.IsConnected() & m_DBLogTcp.IsConnected()/* && m_FtpTcp.IsConnected()*/;
	if (IsNeed)
	{
		if (recvSize != 8 || !pData)
			return ConvertIntToDWORD(CONNECT_CHECK_FAILED);
		DWORD* pInfo = (DWORD*)pData;
		if (!m_FishConfig.CheckVersionAndPathCrc(pInfo[0], pInfo[1]))
		{
			pInfo[0] = m_FishConfig.GetSystemConfig().VersionID;
			pInfo[1] = m_FishConfig.GetSystemConfig().PathCrc;
			return 8;
		}
		return ConvertIntToDWORD(CONNECT_CHECK_OK);
	}
	else
		return ConvertIntToDWORD(CONNECT_CHECK_FAILED);
}
bool FishServer::NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize)
{
	if (!pClient)
	{
		ASSERT(false);
		return false;
	}
	AfxNetworkClientOnce* pOnce = new AfxNetworkClientOnce();
	if (!pOnce)
	{
		m_ClientTcp.Kick(pClient);
		ASSERT(false);
		return false;
	}
	pOnce->SeverID = SeverID;
	pOnce->pClient = pClient;
	pOnce->pPoint = (void*)malloc(recvSize);
	if (!pOnce->pPoint)
	{
		m_ClientTcp.Kick(pClient);
		ASSERT(false);
		delete pOnce;
		return false;
	}
	memcpy_s(pOnce->pPoint, recvSize, pData, recvSize);
	pOnce->Length = recvSize;
	m_AfxAddClient.AddItem(pOnce);
	return true;
}
void FishServer::OnAddClient()
{
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		if (!pOnce)
		{
			//ASSERT(false);
			continue;
		}
		if (pOnce->SeverID == m_GameNetworkID)
		{
			++m_UserIndex;
			DWORD ClientID = m_UserIndex;
			INT64 Uid = pOnce->pClient->Uid;
			pOnce->pClient->OutsideExtraData = ClientID;//��ҵ�ID
			m_ClintList.insert(HashMap<DWORD, ServerClientData*>::value_type(pOnce->pClient->OutsideExtraData, pOnce->pClient));
			DWORD tick = timeGetTime();
			//����ҽ������ݿ��ѯ��������
			auto it = m_HalldataCach.find(Uid);
			if (it != m_HalldataCach.end())
			{
				DBR_Cmd_GetAccountInfoByUserID msg;
				msg.udata.money1 = it->second->money1;
				msg.udata.money2 = it->second->money2;
				memcpy_s(msg.udata.name, MAX_NICKNAME + 1, it->second->name, MAX_NICKNAME + 1);
				memcpy_s(msg.udata.icon, ICON_LENGTH, it->second->icon, ICON_LENGTH);
				memcpy_s(msg.udata.signature, MAX_SIGNATURE + 1, it->second->signature, MAX_SIGNATURE + 1);
				msg.udata.sex = it->second->sex;
				msg.udata.vip = it->second->vip;
				msg.udata.headIcon = it->second->headIcon;
				SetMsgInfo(msg, DBR_GetAccountInfoByUserID, sizeof(DBR_Cmd_GetAccountInfoByUserID));
				msg.Uid = Uid;
				msg.ClientID = ClientID;
				msg.LogonByGameServer = true;
				g_FishServer.SendNetCmdToDB(&msg);
			}
			else
			{
				Log("δ�ҵ��������͵����ݻ���uid=%lld", Uid);
			}
			//������Ҽ�����¼�
			OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);

			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;
		}
		else
		{
			delete pOnce;
			//ASSERT(false);
		}
	}
}
void FishServer::OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient)
{
	//�����������һ���ͻ����뿪��ʱ��
	if (ServerID == m_GameNetworkID)
	{
		//Log("һ��Client�뿪��GameServer");
		OnHandleRoleLeaveTable(pClient);
	}
	return;
}
void FishServer::OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient)
{
	//�����������һ���ͻ��˼����ʱ��
	if (ServerID == m_GameNetworkID)
	{
		//Log("һ��Client������GameServer");
	}
	return;
}
void FishServer::OnTcpClientConnect(TCPClient* pClient)
{
	if (!pClient)
		return;
	//��������ӳɹ���ʱ��
	if (pClient == &m_CenterTcp)
	{
		m_CenterTcpStates = true;
		Log("�Ѿ���������������ӳɹ���");
	}
	else if (pClient == &m_DBTcp)
	{
		m_DBTcpStates = true;

		Log("�Ѿ������ݿ����ӳɹ���");
	}
	else if (pClient == &m_DBSaveTcp)
	{
		m_DBSaveTcpStates = true;
		Log("�Ѿ���Save���ݿ����ӳɹ���");
	}
	else if (pClient == &m_DBLogTcp)
	{
		m_DBLogTcpStates = true;
		Log("�Ѿ���Log���ݿ����ӳɹ���");
	}
	else
		return;
}
void FishServer::OnLeaveCenterServer()
{
	//��GameServer���񲻿��õ�ʱ�� ���������ȫ������
	//��ȫ���������
	m_RoleManager.OnKickAllUser();
	//�Ͽ�ȫ��������
	for (auto Iter = m_ClintList.begin(); Iter != m_ClintList.end();)
	{
		if (!GetFishConfig().CheckInWhiteList(Iter->second->Uid))
		{
			m_ClientTcp.Kick(Iter->second);
			Iter = m_ClintList.erase(Iter);
			continue;
		}
		++Iter;
	}
}
void FishServer::OnTcpClientLeave(TCPClient* pClient)
{
	if (pClient == &m_CenterTcp)
	{
		m_CenterTcpStates = false;
		Log("�Ѿ�������������Ͽ�������");
		m_CenterTcp.Shutdown();
		//m_GameServerManager.OnGameLeavCenter();//������������Ͽ�����
		OnLeaveCenterServer();
	}
	else if (pClient == &m_DBTcp)
	{
		m_DBTcpStates = false;
		m_DBTcp.Shutdown();
		Log("�Ѿ������ݿ�Ͽ�������");
	}
	else if (pClient == &m_DBSaveTcp)
	{
		m_DBSaveTcpStates = false;
		m_DBSaveTcp.Shutdown();
		Log("�Ѿ���Save���ݿ�Ͽ�������");
	}
	else if (pClient == &m_DBLogTcp)
	{
		m_DBLogTcpStates = false;
		m_DBLogTcp.Shutdown();
		Log("�Ѿ���Log���ݿ�Ͽ�������");
	}
	else
		return;
}
void FishServer::CloseClientSocket(DWORD SocketID)
{
	ServerClientData* pClient = GetUserClientDataByIndex(SocketID);
	if (pClient)
		pClient->Removed = true;
}
void FishServer::SendNetCmdToDB(NetCmd* pCmd)
{
	if (!m_DBTcp.IsConnected() || !pCmd)
		return;
	if (!m_DBTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToCenter(NetCmd* pCmd)
{
	if (!m_CenterTcp.IsConnected() || !pCmd)
		return;
	if (!m_CenterTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}

void FishServer::SendNetCmdToSaveDB(NetCmd* pCmd)
{
	if (!m_DBSaveTcp.IsConnected() || !pCmd)
		return;
	if (!m_DBSaveTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToLogDB(NetCmd* pCmd)
{
	if (!m_DBLogTcp.IsConnected() || !pCmd)
		return;
	if (!m_DBLogTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
//void FishServer::SendNetCmdToLogon(NetCmd* pCmd)
//{
//	if (!m_LogonTcp.IsConnected() || !pCmd)
//		return;
//	m_LogonTcp.Send(pCmd, false);
//}
void FishServer::SendNewCmdToClient(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return;
	//LogSendCmdMsg(pClient->OutsideExtraData,pCmd->GetMainType(), pCmd->GetSubType(), pCmd->CmdSize);
	if (!m_ClientTcp.Send(pClient, pCmd))
	{
		ASSERT(false);
	}
}
void FishServer::SendNewCmdToAllClient(NetCmd* pCmd)
{
	if (!pCmd)
		return;
	if (!m_ClintList.empty())
	{
		HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
		for (; Iter != m_ClintList.end(); ++Iter)
		{
			if (!m_ClientTcp.Send(Iter->second, pCmd))
			{
				ASSERT(false);
			}
		}
	}
}
void FishServer::OnHandleAllMsg()
{
	if (!m_ClintList.empty())
	{
		HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
		for (; Iter != m_ClintList.end();)
		{
			while (Iter->second->RecvList.HasItem())
			{
				NetCmd* pCmd = Iter->second->RecvList.GetItem();
				//������������ �ͻ��˷������ĵ�½ ע�� ������
				HandleClientMsg(Iter->second, pCmd);
				free(pCmd);
			}
			++Iter;
		}
	}
	NetCmd* pCmd = m_DBTcp.GetCmd();
	while (pCmd)
	{
		HandleDataBaseMsg(pCmd);
		free(pCmd);
		pCmd = m_DBTcp.GetCmd();
	}

	pCmd = m_DBSaveTcp.GetCmd();
	while (pCmd)
	{
		HandleDataBaseMsg(pCmd);
		free(pCmd);
		pCmd = m_DBSaveTcp.GetCmd();
	}
	pCmd = m_CenterTcp.GetCmd();
	while (pCmd)
	{
		HandleCenterMsg(pCmd);
		free(pCmd);
		pCmd = m_CenterTcp.GetCmd();
	}
}

bool FishServer::MainUpdate()
{
	while (!m_IsClose)
	{
		DWORD dwTimer = timeGetTime();

		if (m_IsReloadConfig)
		{
			m_IsReloadConfig = false;
			OnReloadConfig();//���¼��������ļ�
			if (!FishCallback::GetFishSetting()->LoadConfig("fishdata/Config"))
			{
				Log(L"���¼���config.xmlʧ��!");
				return false;
			}
			Log(L"���¼���config.xml�ɹ�!");
		}

		OnAddClient();
		UpdateByMin(dwTimer);
		OnSaveInfoToDB(dwTimer);

		m_RobotManager.Update();
		//1.����Client ������
		if (!m_ClintList.empty())
		{
			HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
			for (; Iter != m_ClintList.end();)
			{
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_GameNetworkID, Iter->second);
					m_ClientTcp.Kick(Iter->second);
					Iter = m_ClintList.erase(Iter);
					continue;
				}
				int Sum = 0;
				while (Iter->second->RecvList.HasItem() && Sum < Client_Msg_OnceSum)
				{
					NetCmd* pCmd = Iter->second->RecvList.GetItem();
					HandleClientMsg(Iter->second, pCmd);
					free(pCmd);
					++Sum;
				}
				++Iter;
			}
		}
		//2.�������ݿ������
		if (m_DBTcp.IsConnected())
		{
			if (!m_DBTcpStates)
				OnTcpClientConnect(&m_DBTcp);
			NetCmd* pCmd = m_DBTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleDataBaseMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_DBTcp.GetCmd();
			}
			if (pCmd)
			{
				HandleDataBaseMsg(pCmd);
				free(pCmd);
			}
		}
		else
		{
			if (m_DBTcpStates)
				OnTcpClientLeave(&m_DBTcp);

			//��������DB
			ConnectDB();
		}

		if (m_DBSaveTcp.IsConnected())
		{
			if (!m_DBSaveTcpStates)
				OnTcpClientConnect(&m_DBSaveTcp);
			NetCmd* pCmd = m_DBSaveTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleDataBaseMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_DBSaveTcp.GetCmd();
			}
			if (pCmd)
			{
				HandleDataBaseMsg(pCmd);
				free(pCmd);
			}
		}
		else
		{
			if (m_DBSaveTcpStates)
				OnTcpClientLeave(&m_DBSaveTcp);

			//��������DB
			ConnectSaveDB();
		}

		if (m_DBLogTcp.IsConnected())
		{
			if (!m_DBLogTcpStates)
				OnTcpClientConnect(&m_DBLogTcp);
		}
		else
		{
			if (m_DBLogTcpStates)
				OnTcpClientLeave(&m_DBLogTcp);

			//��������DB
			ConnectLogDB();
		}
		//3.���������
		if (m_CenterTcp.IsConnected())
		{
			if (!m_CenterTcpStates)
				OnTcpClientConnect(&m_CenterTcp);
			NetCmd* pCmd = m_CenterTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleCenterMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_CenterTcp.GetCmd();
			}
			if (pCmd)
			{
				HandleCenterMsg(pCmd);
				free(pCmd);
			}
		}
		else
		{
			if (m_CenterTcpStates)
				OnTcpClientLeave(&m_CenterTcp);

			ConnectCenter();
		}
		//����������
		m_LogonManager.OnUpdate(dwTimer);
		//�������Ϻ� ���ǿ�ʼ���� �����ĸ��º��� ��ʵ���� ���ϵ�Send����������������߿ͻ���ȥ
		m_TableManager.Update(dwTimer); //��������
		Sleep(1);
	}
	return OnDestroy();
}
bool FishServer::OnDestroy()
{
	m_TableManager.OnStopService();
	m_RoleManager.Destroy();

	OnHandleAllMsg();
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		delete pOnce;
	}
	return true;
}
//��ʼ��������� 
bool FishServer::HandleClientMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return false;
	//�ͻ��˷�����������
	switch (pCmd->GetMainType())
	{
	case Main_Table:
		return OnHandleTCPNetworkTable(pClient, pCmd);
	case Main_Game:
		return OnHandleTCPNetworkGame(pClient, pCmd);
	case Main_Role:
		return OnHandleTCPNetworkRole(pClient, pCmd);
	case Main_Package:
		return OnHandleTCPNetworkPackage(pClient, pCmd);
	case Main_GameData:
		return OnHandleTCPNetworkGameData(pClient, pCmd);
	default:
		Log(" ERROR: Unkown HandleClientMsg :%d", pCmd->GetMainType());
	}
	return true;
}
bool FishServer::HandleDataBaseMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	switch (pCmd->GetCmdType())
	{
	case DBO_GameserverStart:
		return m_TableManager.InitTableTotalGold(pCmd);
	case DBO_GetAccountInfoByUserID:
		return OnHandDBLogonMsg(pCmd);
	case DBO_LoadGameData:
		return OnHandleDataBaseLoadGameData(pCmd);
	case DBO_SaveRoleAllInfo:
		return OnHandleSaveRoleAllInfo(pCmd);
	}
	return true;
}
bool FishServer::HandleCenterMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	//���������������������
	switch (pCmd->GetMainType())
	{
	case Main_Hall:
	{
		switch (pCmd->GetSubType())
		{ 
		case 63://��Ҵ���Ϸ���������¼��Ϸ
		{
			//����û������������
			GL_LoginSubGame msg;
			SetMsgInfo(msg, GetMsgType(Main_Hall, 164), sizeof(GL_LoginSubGame));
			msg.loginSubGame = false;
			CreateSocketData csd;
			memset(&csd, 0, sizeof(CreateSocketData));
			csd.uid = ((LG_UDPClientConnect*)pCmd)->uid;
			if (m_hallControl.IsHallRuning() == true || m_FishConfig.CheckInWhiteList(csd.uid))
			{
				if (m_ClientTcp.CreateNewClientSocket(csd, 0))// inet_addr(g_FishServerConfig.GetGameServerConfig(m_GameNetworkID)->GameListenIP));
				{
					m_ratioValue = ((LG_UDPClientConnect*)pCmd)->udata.ratioValue;
					UINT32 uDataSize = sizeof(tagClientUserData) + ((LG_UDPClientConnect*)pCmd)->udata.achSize*sizeof(tagAchDataMap);
					tagClientUserData* udata = (tagClientUserData*)malloc(uDataSize);
					memcpy_s(udata, uDataSize, &(((LG_UDPClientConnect*)pCmd)->udata), uDataSize);
					m_HalldataCach[csd.uid] = udata;
					msg.loginSubGame = true;
				}
			}
			msg.fishPort = csd.Port;
			msg.fishIp = inet_addr(g_FishServerConfig.GetGameServerConfig(m_GameNetworkID)->GameListenIP);
			msg.rpcId = ((LG_UDPClientConnect*)pCmd)->rpcid;
			msg.uid = ((LG_UDPClientConnect*)pCmd)->uid;
			//Log("new client ip = %d,prot = %d", msg.Addr, msg.Port);
			SendNetCmdToCenter(&msg);
			break;
		}
		case 67:// ��ҽ�ҸĶ�������֪ͨ������Ϸ
		{
			LG_UpdateMoney* pmsg = (LG_UpdateMoney*)pCmd;
			//Log("update money uid = %lld,money1=%f,money2=%f", uid, pmsg->money1, pmsg->money2);
			CRoleEx* pRole = m_RoleManager.QuertUserByUid(pmsg->uid);
			if (!pRole)
			{
				Log("��ҽ�ҸĶ���δ�ҵ���ң�%lld, money1=%lf, money2=%lf", pmsg->uid, pmsg->money1, pmsg->money2);
				ASSERT(false);
				return false;
			}
			pRole->GetRoleInfo().money1 += static_cast<int64>(pmsg->money1 * MONEY_RATIO);
			pRole->GetRoleInfo().money2 += static_cast<int64>(pmsg->money2 * MONEY_RATIO);
			//֪ͨ�ͻ���
			LG_SyncAddMoney msg;
			SetMsgInfo(msg, 6034, sizeof(LG_SyncAddMoney));
			msg.add_goldNum = static_cast<int64>(pmsg->money1 * MONEY_RATIO) + static_cast<int64>(pmsg->money2 * MONEY_RATIO);
			msg.reason = pmsg->reason;
			pRole->SendDataToClient(&msg);
			break;
		}
		case 71://�رղ��������
		{
			LG_StopFishServer* pmsg = (LG_StopFishServer*)pCmd;
			Log("��Ϸ�������ߵ���ʱ��%lld ms", pmsg->closeTime);
			m_hallControl.ReqStopHall(pmsg->closeTime);
			break;
		}
		case 72://֪ͨ���ز������ñ�
		{
			//OnReloadConfig();//���¼��������ļ�
			//if (!FishCallback::GetFishSetting()->LoadConfig("fishdata/Config"))
			//{
			//	Log(L"���¼���config.xmlʧ��!");
			//}
			//Log(L"���¼���config.xml�ɹ�!");
			//��������
			CL_Cmd_ReloadConfig* pmsg = (CL_Cmd_ReloadConfig*)pCmd;
			std::string tableStr;
			tableStr.assign(pmsg->tableStr);
			Log("�յ��������ã�%s", tableStr.c_str());
			vector<string> allFields;
			SplitString2Vector(tableStr, allFields, ",");
			if (allFields.size() != 6)
			{
				Log("�����������ô��󣬽ڵ�������=%d", allFields.size());
				return true;
			}
			//ϵͳ������ƽڵ�
			float winMin = static_cast<float>(atof(allFields[0].c_str()));
			if (winMin != -1) SystemControl::sysMinWin = winMin;
			float winMax = static_cast<float>(atof(allFields[1].c_str()));
			if (winMax != -1) SystemControl::sysMaxWin = winMax;
			if (allFields[2].length() > 2)
			{
				const string& field = allFields[2];
				string temp = allFields[2].substr(1, field.length() - 2);//ɾ��[]
				vector<string> vecControl;
				SplitString2Vector(temp, vecControl, "|");
				if (vecControl.size() % 3 != 0)
				{
					Log("ϵͳ����ڵ�������󣬵�Ԫ�������ô���");
					return false;
				}
				UINT i = 0;
				while (i < vecControl.size())
				{
					int id = atoi(vecControl[i].c_str());
					auto it = SystemControl::item.find(id);
					if (it == SystemControl::item.end())
					{
						Log("ϵͳ����item ID���ô���,id=%d", id);
						i += 3;
						continue;
					}
					float f1 = static_cast<float>(atof(vecControl[i + 1].c_str()));
					if (f1 != -1) it->second[0] = f1;
					float f2 = static_cast<float>(atof(vecControl[i + 2].c_str()));
					if (f2 != -1) it->second[1] = f2;
					i += 3;
				}
			}
			//���������ƽڵ�
			if (allFields[3].length() > 2)
			{
				const string& field = allFields[3];
				string temp = allFields[3].substr(1, field.length() - 2);//ɾ��[]
				vector<string> vecFishDead;
				SplitString2Vector(temp, vecFishDead, "|");
				if (vecFishDead.size() % 2 != 0)
				{
					Log("���������ƽڵ�������󣬵�Ԫ�������ô���");
					return false;
				}
				UINT i = 0;
				while (i < vecFishDead.size())
				{
					USHORT fishType = static_cast<USHORT>(atoi(vecFishDead[i].c_str()));
					float rate = static_cast<float>(atof(vecFishDead[i + 1].c_str()));
					FishCallback::GetFishSetting()->FishSpecialRateChange(fishType, rate);
					i += 2;
				}
			}
			//���ӳ�ʼ������
			if (allFields[4].length()>2)
			{
				const string& field = allFields[4];
				string temp = allFields[4].substr(1, field.length() - 2);//ɾ��[]
				vector<string> vecTableInit;
				SplitString2Vector(temp, vecTableInit, "|");
				if (vecTableInit.size() % 4 != 0)
				{
					Log("���ӳ�ʼ�����ýڵ����");
					return false;
				}
				UINT i = 0;
				tagTableConfig& tableCfg = m_FishConfig.GetTableConfig();
				while (i< vecTableInit.size())
				{
					BYTE tableID = static_cast<BYTE>(atoi(vecTableInit[i].c_str()));
					auto it = tableCfg.m_TableConfig.find(tableID);
					if (it == tableCfg.m_TableConfig.end())
					{
						Log("���Ӹ�������ID����:����ID = %d", tableID);
						i += 4;
						continue;
					}
					int64 initProduce = atoll(vecTableInit[i + 1].c_str());
					if (initProduce != -1) it->second.InitProduce = initProduce;
					int64 initEarn = atoll(vecTableInit[i + 2].c_str());
					if (initEarn != -1) it->second.InitEarn = initEarn;
					int64 initTurn = atoll(vecTableInit[i + 3].c_str());
					if (initTurn != -1) it->second.InitTurn = initTurn;
					i += 4;
				}
			}
			//�ӵ��������ø���
			if (allFields[5].length() > 2)
			{
				const string& field = allFields[5];
				string temp = allFields[5].substr(1, field.length() - 2);//ɾ��[]
				vector<string> vecConnonPower;
				SplitString2Vector(temp, vecConnonPower, "|");
				if (vecConnonPower.size() % 2 != 0)
				{
					Log("�ӵ������������󣬵�Ԫ�������ô���");
					return false;
				}
				UINT i = 0;
				while (i < vecConnonPower.size())
				{
					UINT id = static_cast<UINT>(atoi(vecConnonPower[i].c_str()));
					float power = static_cast<float>(atof(vecConnonPower[i + 1].c_str()));
					FishCallback::GetFishSetting()->FishConnonPowerChange(id, power);
					i += 2;
				}
			}
			break;
		}
		case 73://֪ͨ��Ϸ�����������
		{
			CL_HallStartFishServer* pmsg = (CL_HallStartFishServer*)pCmd;
			Log("��Ϸ������������ʱ��%lld ms", pmsg->starTime);
			m_hallControl.ReqStartHall(pmsg->starTime);
			break;
		}
		case 162://LG ʹ����Ʒ
		{
			LG_UseGoods* pmsg = (LG_UseGoods*)pCmd;
			CRoleEx* pRole = m_RoleManager.QuertUserByUid(pmsg->uid);
			if (pRole)
			{
				switch (pmsg->itemId)
				{
				case 300001:
					pCmd->SetCmdType(CMD_SKILL_TIMESTOP);
				case 300002:
					pCmd->SetCmdType(CMD_SKILL_DOUBLE_FIRE);
				default:
					return false;
				}
				//���뵽��Ϸ�ڲ�ȥ
				return m_TableManager.OnHandleTableMsg(pRole->GetUserID(), pCmd);
			}
			else
			{
				Log("ʹ����Ʒ,δ֪��ң�%lld, ��ƷID=%d, ��Ʒ����=%d", pmsg->uid, pmsg->itemId, pmsg->itemNum);
			}
		}
		default:
			break;
		}
		return true;
	}
	default:
		Log("unkown Center NetCmd MainType=%d,SubType=%d", pCmd->GetMainType(), pCmd->GetSubType());
	}
	return true;
}
void FishServer::UpdateByMin(DWORD dwTimer)//�����ӽ��и��� �����������������
{
	static DWORD DayChageTimeUpdate = 0;
	if (DayChageTimeUpdate != 0 && dwTimer - DayChageTimeUpdate < 60000)
		return;
	DayChageTimeUpdate = dwTimer;
	time_t NowTime = time(NULL);
	tm pNowTime;
	errno_t Error = localtime_s(&pNowTime, &NowTime);
	if (Error != 0)
	{
		ASSERT(false);
		return;
	}

	bool IsMinChange = true;
	bool IsHourChange = (pNowTime.tm_min == GetFishConfig().GetFishUpdateConfig().UpdateMin);
	bool IsDayChange = ((pNowTime.tm_hour == GetFishConfig().GetFishUpdateConfig().UpdateHour) && IsHourChange);
	bool IsMonthChange = ((pNowTime.tm_mday == 1) && IsDayChange);
	bool IsYearChange = ((pNowTime.tm_mon == 0) && IsMonthChange);
	bool IsNewDay = (pNowTime.tm_hour == 0 && pNowTime.tm_min == 0);
	//����Ĺ�ϵ����
	m_RoleManager.OnUpdateByMin(IsHourChange, IsDayChange, IsMonthChange, IsYearChange);
	m_TableManager.CostAndProduceMin();
	m_hallControl.UpdateMin();
	if (m_hallControl.IsHallRuning() == false)
	{
		OnLeaveCenterServer();
	}
	if (IsNewDay)
	{
		//���߿ͻ��� �µ�һ���� ���Խ��д���Щ������
		m_TableManager.NewDayForTable();
	}
}
void FishServer::OnSaveInfoToDB(DWORD dwTimer)
{

	static DWORD SaveInfoDB = timeGetTime();
	if (dwTimer - SaveInfoDB >= static_cast<DWORD>(GetFishConfig().GetSystemConfig().RoleSaveMin * 60000))
	{
		SaveInfoDB = dwTimer;
		m_RoleManager.OnSaveInfoToDB();
		//m_RoleManager.OnGetRoleAchievementIndex();
	}
}
bool FishServer::OnHandDBLogonMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;

	DBO_Cmd_GetAccountInfoByUserID* pMsg = (DBO_Cmd_GetAccountInfoByUserID*)pCmd;
	ServerClientData* pClient = GetUserClientDataByIndex(pMsg->ClientID);
	if (pMsg->ClientID != 0 && !pClient)//pMsg->ClientID == 0 Ϊ������
	{
		ASSERT(false);
		return false;
	}
	if (pMsg->ClientID == 0 && pMsg->IsRobot == false)
	{
		ASSERT(false);
		return false;//�ǻ����� �����ر�����Ϊ��������
	}
	DWORD SocketID = (pClient ? pClient->OutsideExtraData : 0);
	if (pMsg->Result)
	{
		//��ҵ�½�ɹ�
		//��ҵ�½�ɹ��� ���ǽ��д���
		//1.������Ҷ���
		CRoleEx* pRole = m_RoleManager.QueryUser(pMsg->RoleInfo.dwUserID);
		if (pRole)
		{
			ASSERT(false);
			if (pRole->IsRobot())
			{
				ASSERT(false);
				return false;
			}
			if (!pRole->IsAFK())
			{
				NetCmd msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_ServerChangeSocket), sizeof(NetCmd));
				pRole->SendDataToClient(&msg);
				//���Socket�����л�
				DWORD SocketID = pRole->GetGameSocketID();
				pRole->ChangeRoleSocketID(pClient->OutsideExtraData);
			}
			else
			{
				m_TableManager.OnPlayerLeaveTable(pRole->GetUserID());//���뿪����
				pRole->ChangeRoleSocketID(pClient->OutsideExtraData);//�޸Ļ�����ҵ�Socket
			}
			if (pRole->IsExit())
			{
				//Log("��ҽ������߱��� �ڻط�ȷ������֮ǰ ���µ�½��  �ط�ȷ������ ʧЧ ");
				pRole->SetIsExit(false);
			}
		}
		else if (!m_RoleManager.CreateRole(&pMsg->RoleInfo, &pMsg->RoleServerInfo, SocketID, pMsg->LastOnlineTime, pMsg->LogonByGameServer, pMsg->IsRobot))
		{
			//�������ʧ�ܺ� ������뿪
			ASSERT(false);

			LC_Cmd_AccountOnlyIDFailed msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountOnlyIDFailed), sizeof(LC_Cmd_AccountOnlyIDFailed));
			SendNewCmdToClient(pClient, &msg);
			return false;
		}
		else
		{
			LC_LoadFinish msg;
			SetMsgInfo(msg, 6030, sizeof(LC_LoadFinish));
			SendNewCmdToClient(pClient, &msg);
		}
		return true;
	}
	else
	{
		if (pMsg->IsFreeze)
		{
			//�˺ű������� ��ʾʧ��
			LC_Cmd_AccountIsFreeze msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
			msg.EndTime = pMsg->FreezeEndTime;
			SendNewCmdToClient(pClient, &msg);
			return true;
		}
		//��ҵ�½ʧ����
		LC_Cmd_AccountOnlyIDFailed msg;
		SetMsgInfo(msg,GetMsgType(Main_Logon, LC_AccountOnlyIDFailed), sizeof(LC_Cmd_AccountOnlyIDFailed));
		SendNewCmdToClient(pClient, &msg);
		return true;
	}
}
ServerClientData* FishServer::GetUserClientDataByIndex(DWORD IndexID)
{
	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.find(IndexID);
	if (Iter == m_ClintList.end())
		return NULL;
	else
		return Iter->second;
}
bool FishServer::OnHandleTCPNetworkTable(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pCmd || !pClient)
		return false;
	if (pCmd->GetMainType() == Main_Table)
	{
		//����ҷ��͵�½��Ϊ��ʱ��
		switch (pCmd->GetSubType())
		{
		case CL_Sub_JoinTable:
		{
			bool ret = OnHandleRoleJoinTable(pClient, pCmd);
			if (ret == false)
			{
				//�������ʧ�ܣ�������˳�
				OnHandleRoleLeaveTable(pClient);
			}
			return ret;
		}
		case CL_Sub_LeaveTable:
			OnHandleRoleLeaveTable(pClient);
			return true;
		case LC_Sub_StartSyncFish:
		{

			if (!pCmd || !pClient)
				return false;
			CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			CRole* pR = m_TableManager.QueryRoleByRoleEx(pRole);
			if (pR == NULL)
			{
				Log("QueryRoleByRoleEx does not get role,user id = %d",pRole->GetUserID());
				ASSERT(false);
				return false;
			}
			pR->SetRoleIsCanSendTableMsg(true);
			m_TableManager.GetTable(pR->GetTableID())->DelaySyncDataToClient(pRole);
			return true;
		}
		case LC_Sub_ShowOthers:
		{
			if (!pCmd || !pClient)
				return false;
			NetCmdClientShowOthers* pMsg = (NetCmdClientShowOthers*)pCmd;
			NetCmdOtherInfo msg;
			memset(&msg, 0, sizeof(NetCmdOtherInfo));
			SetMsgInfo(msg, 6028, sizeof(NetCmdOtherInfo));
			auto it = m_HalldataCach.find(pMsg->playerid);
			if (it != m_HalldataCach.end())
			{
				msg.playerId = it->first;
				msg.vipLevel = it->second->vip;
				memcpy_s(msg.icon, ICON_LENGTH, it->second->icon, ICON_LENGTH);
				memcpy_s(msg.name, MAX_NICKNAME, it->second->name, MAX_NICKNAME);
				msg.sex = it->second->sex;
				msg.titleId = it->second->titleIndex;
				msg.gameWinMaxMoney = it->second->gameMaxWinMoney;
				msg.money = (it->second->money1 + it->second->money2)*1.0 / MONEY_RATIO;
				msg.charmValue = it->second->charmValue;
				memcpy_s(msg.signature, MAX_SIGNATURE, it->second->signature, MAX_SIGNATURE);
				msg.headIcon = it->second->headIcon;
			}
			else
			{
				Log("δ�ҵ�����鿴�����Ϣ��id = %lld", pMsg->playerid);
				msg.playerId = 0;
			}
			SendNewCmdToClient(pClient, &msg);
			return true;
		}
		}
		return true;
	}
	return true;
}
bool FishServer::OnHandleRoleJoinTable(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pCmd || !pClient)
		return false;
	//��ҽ�������
	CL_JoinTable* pMsg = (CL_JoinTable*)pCmd;
	if (!pMsg)
	{
		ASSERT(false);
		return false;
	}
	CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	return m_TableManager.OnPlayerJoinTable(pMsg->bTableTypeID, pRole);//�ڲ�������ҽ������� �ᴦ������� ������������
}
bool FishServer::OnHandleRoleLeaveTable(ServerClientData* pClient)
{
	if (!pClient)
		return false;
	CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	//��ҹ�����ɾ���������
	m_RoleManager.OnKickOneUser(pRole);
	//����Ƴ�
	pClient->Removed = true;
	return true;
}
//------------------------��Ϸ------------------------------------------------------------------------
bool FishServer::OnHandleTCPNetworkGame(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pCmd || !pClient)
		return false;
	CRoleEx* pUser = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pUser)
	{
		//ASSERT(false);
		return true;
	}
	if (pCmd->GetMainType() == Main_Game || pCmd->GetMainType() == Main_Table)
	{
		//����ϴ�ͷ�� �������ڲ����� ��������ڲ� �ⲿ�����OK
		return m_TableManager.OnHandleTableMsg(pUser->GetUserID(), pCmd);
		//switch (pCmd->GetCmdType())
		//{
		//default:
		//	
		//}
	}
	return true;
}
//Role--------------------------------------------------------------------------------------------------------------------------------------------
bool FishServer::OnHandleTCPNetworkRole(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return false;
	//���տͻ��˷������� ������� �޸�
	CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pRole)
	{
		ASSERT(false);
		return true;
	}
	//switch (pCmd->GetSubType())
	//{
	//	
	//}
	return true;
}
bool FishServer::OnHandleSaveRoleAllInfo(NetCmd* pCmd)
{
	DBO_Cmd_SaveRoleAllInfo* pMsg = (DBO_Cmd_SaveRoleAllInfo*)pCmd;
	if (!pMsg)
	{
		ASSERT(false);
		return false;
	}
	//���յ�����
	GetRoleManager()->OnDelUserResult(pMsg);
	return true;
}
//GameData---------------------------------------------------------------------------------
bool FishServer::OnHandleDataBaseLoadGameData( NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	DBO_Cmd_LoadGameData* pMsg = (DBO_Cmd_LoadGameData*)pCmd;
	CRoleEx* pRole = m_RoleManager.QueryUser(pMsg->dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	return true;
}
//Package
bool FishServer::OnHandleTCPNetworkPackage(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
	{
		ASSERT(false);
		return false;
	}
	CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	/*switch (pCmd->GetSubType())
	{
		case CL_OpenPackage:
		{
			CL_Cmd_OpenPackage* pMsg = (CL_Cmd_OpenPackage*)pCmd;
			m_PackageManager.OnOpenFishPackage(pRole, pMsg->PackageItemID);
			return true;
		}
		break;
	}
	return true;*/

	return false;
}
//GameData
bool FishServer::OnHandleTCPNetworkGameData(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
	{
		ASSERT(false);
		return false;
	}
	CRoleEx* pRole = m_RoleManager.QuertUserBySocketID(pClient->OutsideExtraData);
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	switch (pCmd->GetSubType())
	{
	case CL_GetGameData:
		{
			return true;
		}
		break;
	}
	return true;
}

void FishServer::GetAddressByIP(DWORD IP, TCHAR* pAddress,DWORD ArrayCount)
{
	if (!pAddress)
	{
		ASSERT(false);
		return;
	}
	//��IP��ַ���� ȡ��
	IP = htonl(IP);
	char Result[MAX_ADDRESS_LENGTH_IP] = { 0 };
	IpLocating(IP, Result, MAX_ADDRESS_LENGTH_IP);
	UINT Count = 0;
	TCHAR* pResult = CharToWChar(Result, Count);
	if (!pResult)
	{
		ASSERT(false);
		return;
	}
	//��TCHAR ���н�ȡ
	size_t len = _tcsclen(pResult);
	size_t beginIndex = 0;
	//�ַ������ɳɹ��� ���д���
	for (size_t i = 0; i < len; ++i)
	{
		if (pResult[i] == TEXT('ʡ'))
		{
			beginIndex = i + 1;
			break;
		}
	}
	if (beginIndex < len && beginIndex != 0)
		TCHARCopy(pAddress, ArrayCount, &pResult[beginIndex], _tcslen(pResult) - beginIndex);
	else
		TCHARCopy(pAddress, ArrayCount, pResult, _tcslen(pResult));
	free(pResult);
}
void FishServer::OnReloadConfig()
{
	FishConfig pNewConfig;
	if (!pNewConfig.LoadConfigFilePath())
	{
		Log("���¼���FishConfig.xmlʧ��");
		return;
	}
	m_FishConfig = pNewConfig;
	Log("���¼���FishConfig.xml�ɹ�");
}
tagClientUserData* FishServer::GetHallDataCache(const int64 uid)
{
	auto it = m_HalldataCach.find(uid);
	if (it == m_HalldataCach.end())
	{
		return NULL;
	}
	return it->second;
}

bool FishServer::OnRemoveHallCatchData(const int64 uid)
{
	auto it = m_HalldataCach.find(uid);
	if (it != m_HalldataCach.end())
	{
		free(it->second);
		m_HalldataCach.erase(it);
		//Log("����������ݣ�%lld", uid);
	}
	return true;
}