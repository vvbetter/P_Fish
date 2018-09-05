#include "stdafx.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include<algorithm>
FishServer::FishServer()
{
	m_IsClose = false;
	m_DBIsConnect = false;
	m_IsReloadConfig = false;
	m_ControlServerNetworkID = 0;
	m_ControlClientIndex = 0;
	m_ControlRankValue = 0;
	m_LogDBIsConnect = false;
}
FishServer::~FishServer()
{

}
bool FishServer::InitServer()
{
	m_pDump.OnInit();//������¼dump
	if (!g_FishServerConfig.LoadServerConfigFile())
	{
		ShowInfoToWin("�����������ļ���ȡʧ��");
		return false;
	}
	if (!m_FishConfig.LoadConfigFilePath())
	{
		ASSERT(false);
		return false;
	}
	ShowInfoToWin("�����ļ���ȡ�ɹ�");

	ControlServerConfig* pConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return false;
	}
	m_ControlServerNetworkID = pConfig->ControlServerNetworkID;
	m_ControlClientNetworkID = pConfig->ControlClientNetworkID;
	m_ControlID = pConfig->ControlID;
	m_ControlRankValue = pConfig->ControlClientPassword;
	if (!ConnectControlServer())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectControlServer())
				break;
		}
	}
	if (!ConnectDB())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectDB())
				break;
		}
	}
	if (!ConnectLogDB())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectLogDB())
				break;
		}
	}
	if (!ConnectControlClient())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectControlClient())
				break;
		}
	}
	ShowInfoToWin("�����������������ɹ�");
	return true;
}
bool FishServer::ConnectDB()
{
	ControlServerConfig* pControlServerConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pControlServerConfig)
	{
		ASSERT(false);
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pControlServerConfig->DBNetworkID);
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
	if (!m_DBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_ControlID, sizeof(BYTE)))
	{
		ShowInfoToWin("���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
bool FishServer::ConnectLogDB()
{
	ControlServerConfig* pControlServerConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pControlServerConfig)
	{
		ASSERT(false);
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pControlServerConfig->LogDBID);
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
	if (!m_LogDBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_ControlID, sizeof(BYTE)))
	{
		ShowInfoToWin("Log���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_LogDBTcp);
	return true;
}
bool FishServer::ConnectControlClient()
{
	ControlServerConfig* pControlServerConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pControlServerConfig)
	{
		ASSERT(false);
		return false;
	}
	ServerInitData pCLientData;
	pCLientData.ServerID = pControlServerConfig->ControlClientNetworkID;
	pCLientData.BuffSize = pControlServerConfig->ControlClientBuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.ListenCount = pControlServerConfig->ControlClientListenCount;
	pCLientData.MaxRecvCmdCount = pControlServerConfig->ControlClientMaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pControlServerConfig->ControlClientMaxSendCountPerFrame;
	pCLientData.Port = pControlServerConfig->ControlClientListenPort;
	pCLientData.SocketRecvSize = pControlServerConfig->ControlClientRecvBuffSize;
	pCLientData.SocketSendSize = pControlServerConfig->ControlClientSendBuffSize;
	pCLientData.Timeout = pControlServerConfig->ControlClientTimeOut;
	pCLientData.RecvThreadNum = pControlServerConfig->ControlClientRecvThreadSum;
	pCLientData.SendThreadNum = pControlServerConfig->ControlClientSendThreadSum;
	pCLientData.SleepTime = pControlServerConfig->ControlClientSleepTime;
	pCLientData.AcceptSleepTime = pControlServerConfig->ControlClientAcceptSleepTime;
	pCLientData.MaxAcceptNumPerFrame = pControlServerConfig->ControlClientMaxAcceptNumPerFrame;
	pCLientData.SceneHearbeatTick = pControlServerConfig->ControlClientSceneHearbeatTick;
	pCLientData.AcceptRecvData = false;
	pCLientData.SceneHearbeatTick = pControlServerConfig->ControlClientSceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pControlServerConfig->ControlClientMaxSendCmdCount;
	m_ClientTcp.SetCmdHandler(this);
	if (!m_ClientTcp.Init(pCLientData))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}

	return true;
}
bool FishServer::ConnectControlServer()
{
	//���Ӷ��ڵ�����
	ControlServerConfig* pControlServerConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pControlServerConfig)
	{
		ASSERT(false);
		return false;
	}
	ServerInitData pCLientData;
	pCLientData.ServerID = pControlServerConfig->ControlServerNetworkID;
	pCLientData.BuffSize = pControlServerConfig->ControlServerBuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.ListenCount = pControlServerConfig->ControlServerListenCount;
	pCLientData.MaxRecvCmdCount = pControlServerConfig->ControlServerMaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pControlServerConfig->ControlServerMaxSendCountPerFrame;
	pCLientData.Port = pControlServerConfig->ControlServerListenPort;
	pCLientData.SocketRecvSize = pControlServerConfig->ControlServerRecvBuffSize;
	pCLientData.SocketSendSize = pControlServerConfig->ControlServerSendBuffSize;
	pCLientData.Timeout = pControlServerConfig->ControlServerTimeOut;
	pCLientData.RecvThreadNum = pControlServerConfig->ControlServerRecvThreadSum;
	pCLientData.SendThreadNum = pControlServerConfig->ControlServerSendThreadSum;
	pCLientData.SleepTime = pControlServerConfig->ControlServerSleepTime;
	pCLientData.AcceptSleepTime = pControlServerConfig->ControlServerAcceptSleepTime;
	pCLientData.MaxAcceptNumPerFrame = pControlServerConfig->ControlServerMaxAcceptNumPerFrame;
	pCLientData.SceneHearbeatTick = pControlServerConfig->ControlServerSceneHearbeatTick;
	pCLientData.AcceptRecvData = true;
	pCLientData.SceneHearbeatTick = pControlServerConfig->ControlServerSceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pControlServerConfig->ControlServerMaxSendCmdCount;
	m_ServerTcp.SetCmdHandler(this);
	if (!m_ServerTcp.Init(pCLientData))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}
	return true;
}
void FishServer::OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�����������һ���ͻ����뿪��ʱ��
	if (ServerID == m_ControlServerNetworkID)
	{
		//ShowInfoToWin("һ���ڲ��������뿪�˿����� ID:%d",ServerID);
	}
	else if (ServerID == m_ControlClientNetworkID)
	{
		//һ���������뿪��
		//ShowInfoToWin("һ���ⲿ�ͻ����뿪�˿�����");
	}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�����������һ���ͻ��˼����ʱ��
	if (ServerID == m_ControlServerNetworkID)
	{
		//std::cout << "һ��Client������CentralServer\n";
		//ShowInfoToWin("һ���ڲ���������������� ID:%d",ServerID);
	}
	else if (ServerID == m_ControlClientNetworkID)
	{
		//һ��������������
		//ShowInfoToWin("һ���ⲿ���������������");
	}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::OnTcpClientConnect(TCPClient* pClient)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	if (pClient == &m_DBTcp)
	{
		m_DBIsConnect = true;
		ShowInfoToWin("�Ѿ���DB���ӳɹ���");
	}
	else if (pClient == &m_LogDBTcp)
	{
		m_LogDBIsConnect = true;
		ShowInfoToWin("�Ѿ���Log���ݿ����ӳɹ���");
	}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::OnTcpClientLeave(TCPClient* pClient)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�����Ͽ����ӵ�ʱ��
	if (pClient == &m_DBTcp)
	{
		ShowInfoToWin("�Ѿ���DB�Ͽ�������");
		m_DBIsConnect = false;
		m_DBTcp.Shutdown();
	}
	else if (pClient == &m_LogDBTcp)
	{
		ShowInfoToWin("�Ѿ���Log���ݿ�Ͽ�������");
		m_LogDBIsConnect = false;
		m_LogDBTcp.Shutdown();
	}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::OnReloadConfig()
{
	FishConfig pNewConfig;
	if (!pNewConfig.LoadConfigFilePath())
	{
		ShowInfoToWin("���¼���FishConfig.xmlʧ��");
		return;
	}
	m_FishConfig = pNewConfig;
	ShowInfoToWin("���¼���FishConfig.xml�ɹ�");
}
bool FishServer::MainUpdate()
{
	//��ѭ������
	while (!m_IsClose)
	{
		DWORD dwTimer = timeGetTime();

		if (m_IsReloadConfig)
		{
			m_IsReloadConfig = false;
			OnReloadConfig();//���¼��������ļ�
		}

		OnAddClient();

		//Center��������ÿһ֡���������  ������һ֡��ȫ�����������
		//1.����GameServer ������
		if (!m_ControlClientList.empty())
		{
			HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlClientList.begin();
			for (; Iter != m_ControlClientList.end();)
			{
				if (!Iter->second)
				{
					ASSERT(false);
					continue;
				}
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_ControlClientNetworkID, Iter->second);
					m_ClientTcp.Kick(Iter->second);
					Iter = m_ControlClientList.erase(Iter);
					continue;
				}
				int Sum = 0;
				while (Iter->second->RecvList.HasItem() && Sum < Client_Msg_OnceSum)//�������������
				{
					NetCmd* pCmd = Iter->second->RecvList.GetItem();
					if (!pCmd)
					{
						ASSERT(false);
						continue;
					}
					//������������ �ͻ��˷������ĵ�½ ע�� ������
					HandleClientMsg(Iter->second, pCmd);
					free(pCmd);
					++Sum;
				}
				++Iter;
			}
		}
		if (!m_ControlServerList.empty())
		{
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
			for (; Iter != m_ControlServerList.end();)
			{
				if (!Iter->second)
				{
					ASSERT(false);
					continue;
				}
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_ControlServerNetworkID, Iter->second);
					m_ServerTcp.Kick(Iter->second);
					Iter = m_ControlServerList.erase(Iter);
					continue;
				}
				int Sum = 0;
				while (Iter->second->RecvList.HasItem() && Sum < Msg_OnceSum)//�������������
				{
					NetCmd* pCmd = Iter->second->RecvList.GetItem();
					if (!pCmd)
					{
						ASSERT(false);
						continue;
					}
					//������������ �ͻ��˷������ĵ�½ ע�� ������
					HandleServerMsg(Iter->second, pCmd);
					free(pCmd);
					++Sum;
				}
				++Iter;
			}
		}

		//2.����DB������
		if (m_DBTcp.IsConnected())
		{
			if (!m_DBIsConnect)
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
			if (m_DBIsConnect)
				OnTcpClientLeave(&m_DBTcp);

			//��������DB
			ConnectDB();
		}

		if (m_LogDBTcp.IsConnected())//LogDB ��������
		{
			if (!m_LogDBIsConnect)
				OnTcpClientConnect(&m_LogDBTcp);
		}
		else
		{
			if (m_LogDBIsConnect)
				OnTcpClientLeave(&m_LogDBTcp);

			//��������DB
			ConnectLogDB();
		}

		Sleep(1);
	}
	return OnDestroy();
}
void FishServer::SendNetCmdToLogDB(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	m_LogDBTcp.Send(pCmd, false);
}
void FishServer::HandleClientMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	//�����ⲿ�������Ĵ�������
	if (!pClient || !pCmd)
	{
		ASSERT(false);
		return;
	}
	if (pCmd->CmdType != Main_Control)
	{
		ASSERT(false);
		return;
	}
	if (pCmd->SubCmdType == CL_CheckClientInfo)//����֤
	{
		CL_Cmd_CheckClientInfo* pMsg = (CL_Cmd_CheckClientInfo*)pCmd;
		if (!pMsg)
		{
			ASSERT(false);
			return;
		}
		/*bool bvaliddmac = false;
		tinyxml2::XMLDocument xml_doc;
		if (xml_doc.LoadFile("maclist.xml") == XML_SUCCESS)
		{
			XMLElement* xml_root = xml_doc.FirstChildElement("maclist");
			if (xml_root)
			{
				for (const XMLElement* xml_child = xml_root->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement())
				{
					const char* pItem = xml_child->Attribute("mac");
					if (pItem&&strcmp(pMsg->MachineCode, pItem) == 0)
					{
						bvaliddmac = true;
						break;
					}
				}
			}
		}*/

		LC_Cmd_CheckClientInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_Control, LC_CheckClientInfo), sizeof(LC_Cmd_CheckClientInfo));
		msg.Result = (pMsg->RankValue == m_ControlRankValue);//(pMsg->RankValue == m_ControlRankValue&&bvaliddmac);//��½�ɹ��� ���Խ���
		SendNetCmdToClient(pClient, &msg);
		if (!msg.Result)
		{
			pClient->Removed = true;
		}
		else
		{
			pClient->dwChecked = 1;
		}
		return;
	}
	if (pClient->dwChecked == 0)
	{
		return;
	}

	switch (pCmd->SubCmdType)
	{
		//case CL_CheckClientInfo:
		//	{
		//		CL_Cmd_CheckClientInfo* pMsg = (CL_Cmd_CheckClientInfo*)pCmd;
		//		if (!pMsg)
		//		{
		//			ASSERT(false);
		//			return;
		//		}
		//		LC_Cmd_CheckClientInfo msg;
		//		SetMsgInfo(msg, GetMsgType(Main_Control, LC_CheckClientInfo), sizeof(LC_Cmd_CheckClientInfo));
		//		msg.Result = (pMsg->RankValue == m_ControlRankValue);//��½�ɹ��� ���Խ���
		//		SendNetCmdToClient(pClient, &msg);
		//		if (!msg.Result)
		//		{
		//			pClient->Removed = true;
		//		}
		//		return;
		//	}
	case CL_SendMsgToAllGame:
	{
								CL_Cmd_SendMsgToAllGame* pMsg = (CL_Cmd_SendMsgToAllGame*)pCmd;
								if (!pMsg)
								{
									ASSERT(false);
									return;
								}
								SendNetCmdToCenter(pMsg);
								return;
	}
		break;
	case CL_SendSystemEmail:
	{
							   CL_Cmd_SendSystemEmail* pMsg = (CL_Cmd_SendSystemEmail*)pCmd;
							   if (!pMsg)
							   {
								   ASSERT(false);
								   return;
							   }
							   SendNetCmdToCenter(pMsg);
							   return;
	}
		break;
	case CL_ChangeaNickName:
	{
							   CL_Cmd_ChangeaNickName* pMsg = (CL_Cmd_ChangeaNickName*)pCmd;
							   if (!pMsg)
							   {
								   ASSERT(false);
								   return;
							   }
							   SendNetCmdToCenter(pMsg);
	}
		break;
	case CL_QueryOnlineUserInfo:
	{
								   CL_Cmd_QueryOnlineUserInfo* pMsg = (CL_Cmd_QueryOnlineUserInfo*)pCmd;
								   if (!pMsg)
								   {
									   ASSERT(false);
									   return;
								   }
								   pMsg->ClientID = pClient->OutsideExtraData;//��¼�¿ͻ���ID
								   SendNetCmdToCenter(pMsg);
	}
		break;
	case CL_KickUserByID:
	{
							CL_Cmd_KickUserByID* pMsg = (CL_Cmd_KickUserByID*)pCmd;
							pMsg->ClientID = pClient->OutsideExtraData;//��¼�¿ͻ���ID
							SendNetCmdToCenter(pMsg);
							//SendNetCmdToAllGame(pMsg);
							return;
	}
		break;
	case CL_ReloadConfig:
	{
							OnReloadConfig();
							SendNetCmdToAllServer(pCmd);
							return;
	}
		break;
	case CL_HandleEntityItem:
	{
								CL_Cmd_HandleEntityItem* pMsg = (CL_Cmd_HandleEntityItem*)pCmd;
								pMsg->ClientID = pClient->OutsideExtraData;
								pMsg->ClientIP = pClient->IP;
								SendNetCmdToCenter(pMsg);
								return;
	}
		break;
	case CL_ResetRolePassword:
	{
								 CL_Cmd_ResetRolePassword* pMsg = (CL_Cmd_ResetRolePassword*)pCmd;

								 DBR_Cmd_ResetUserPassword msg;
								 SetMsgInfo(msg, DBR_ResetUserPassword, sizeof(DBR_Cmd_ResetUserPassword));
								 msg.dwUserID = pMsg->dwUserID;
								 msg.Password1 = pMsg->Password1;
								 msg.Password2 = pMsg->Password2;
								 msg.Password3 = pMsg->Password3;
								 SendNetCmdToControlDB(&msg);

								 SendNetCmdToAllLogon(pMsg);
								 return;
	}
		break;
	case CL_FreezeAccount:
	{
							 CL_Cmd_FreezeAccount*pMsg = (CL_Cmd_FreezeAccount*)pCmd;

							 DBR_Cmd_FreezeUser msg;
							 SetMsgInfo(msg, DBR_FreezeUser, sizeof(DBR_Cmd_FreezeUser));
							 msg.dwUserID = pMsg->dwUserID;
							 msg.FreezeMin = pMsg->FreezeMin;
							 SendNetCmdToControlDB(&msg);

							 SendNetCmdToCenter(pMsg);
							 SendNetCmdToAllLogon(pMsg);
							 return;
	}
	case CL_ChangeParticularStates:
	{
									  CL_Cmd_ChangeParticularStates*pMsg = (CL_Cmd_ChangeParticularStates*)pCmd;
									  pMsg->ClientID = pClient->OutsideExtraData;//��¼�¿ͻ���ID
									  SendNetCmdToCenter(pMsg);
									  return;
	}
	case CL_QueryFishPool:
	{
							 if (pCmd->GetCmdSize() == sizeof(CL_CMD_QueryFishPool))
							 {
								 ((CL_CMD_QueryFishPool*)pCmd)->ClientID = pClient->OutsideExtraData;
								 SendNetCmdToAllGame(pCmd);
							 }
							 return;
	}
	case CL_QueryBlackList:
	{

							  if (pCmd->GetCmdSize() == sizeof(LC_CMD_QueryFishBlackList))
							  {
								  ((LC_CMD_QueryFishBlackList*)pCmd)->ClientID = pClient->OutsideExtraData;
								  SendNetCmdToAllGame(pCmd);
							  }
							  return;
	}
	case CL_SetBlackList:
	{
							if (pCmd->GetCmdSize() > sizeof(LC_CMD_SetFishBlackList))
							{
								((LC_CMD_SetFishBlackList*)pCmd)->ClientID = pClient->OutsideExtraData;
								SendNetCmdToAllGame(pCmd);
							}
							return;
	}
	case CL_OxAdmin:
	{
					   if (pCmd->GetCmdSize() == sizeof(CL_CMD_OxAdminReq))
					   {
						   ((CL_CMD_OxAdminReq*)pCmd)->ClientID = pClient->OutsideExtraData;
						   SendNetCmdToMiniGame(pCmd);
					   }
					   break;
	}
	case CL_AnimalAdmin:
	{
						   if (pCmd->GetCmdSize() == sizeof(CL_CMD_AnimalAdminReq))
						   {
							   ((CL_CMD_AnimalAdminReq*)pCmd)->ClientID = pClient->OutsideExtraData;
							   SendNetCmdToMiniGame(pCmd);
						   }
						   break;
	}
	case CL_CarAdmin:
	{
						if (pCmd->GetCmdSize() == sizeof(CL_CMD_CarAdminReq))
						{
							((CL_CMD_CarAdminReq*)pCmd)->ClientID = pClient->OutsideExtraData;
							SendNetCmdToMiniGame(pCmd);
						}
						break;
	}
	case CL_HandleCustMail:
	{
							  if (pCmd->GetCmdSize() == sizeof(CL_Cmd_HandleCustMail))
							  {
								  SendNetCmdToCenter(pCmd);
							  }
							  break;
	
	}
	case CL_ChangeIsShop:
	{
								SendNetCmdToCenter(pCmd);
								break;
	}
	}
	return;
}
void FishServer::HandleServerMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	//�����ڲ�����������������������
	if (!pClient || !pCmd)
	{
		ASSERT(false);
		return;
	}
	//����������ע�������
	if (pCmd->CmdType != Main_Control)
	{
		ASSERT(false);
		return;
	}
	switch (pCmd->SubCmdType)
	{
		//���յ����������������� ������ ֱ�ӽ���ת����ȥ
	case LC_CenterInfo:
	case LC_GameInfo:
	case LC_LogonInfo:
	case LC_DBInfo:
	case LC_RankInfo:
	case LC_FTPInfo:
	case LC_OperateInfo:
	case LC_MiNiGameInfo:
	{
							SendNetCmdToAllClient(pCmd);
							break;
	}
	case LC_KickUserResult:
	{
							  LC_Cmd_KickUserResult* pMsg = (LC_Cmd_KickUserResult*)pCmd;
							  HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
							  if (Iter == m_ControlClientList.end())
							  {
								  ASSERT(false);
								  return;
							  }
							  SendNetCmdToClient(Iter->second, pMsg);
							  break;
	}
	case LC_HandleEntityItem:
	{
								LC_Cmd_HandleEntityItem* pMsg = (LC_Cmd_HandleEntityItem*)pCmd;
								HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								if (Iter == m_ControlClientList.end())
								{
									ASSERT(false);
									return;
								}
								SendNetCmdToClient(Iter->second, pMsg);
								break;
	}

	case LC_QueryOnlineUserInfo:
	{
								   LC_Cmd_QueryOnlineUserInfo * pMsg = (LC_Cmd_QueryOnlineUserInfo*)pCmd;
								   HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								   if (Iter == m_ControlClientList.end())
								   {
									   ASSERT(false);
									   return;
								   }
								   SendNetCmdToClient(Iter->second, pMsg);
								   break;
	}
	case LC_ChangeParticularStates:
	{
									  LC_Cmd_ChangeParticularStates * pMsg = (LC_Cmd_ChangeParticularStates*)pCmd;
									  HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
									  if (Iter == m_ControlClientList.end())
									  {
										  ASSERT(false);
										  return;
									  }
									  SendNetCmdToClient(Iter->second, pMsg);
									  return;
	}
		break;

	case LC_QueryFishPoolResult:
	{
								   LC_CMD_QueryFishPoolResult* pMsg = (LC_CMD_QueryFishPoolResult*)pCmd;
								   HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								   if (Iter == m_ControlClientList.end())
								   {
									   ASSERT(false);
									   return;
								   }
								   SendNetCmdToClient(Iter->second, pMsg);
								   break;
	}
	case LC_QueryBlackListResult:
	{
									LC_CMD_QueryFishBlackListResult* pMsg = (LC_CMD_QueryFishBlackListResult*)pCmd;
									HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
									if (Iter == m_ControlClientList.end())
									{
										ASSERT(false);
										return;
									}
									SendNetCmdToClient(Iter->second, pMsg);
									break;
	}
	case LC_SetBlackListResult:
	{
								  LC_CMD_SetFishBlackListResult* pMsg = (LC_CMD_SetFishBlackListResult*)pCmd;
								  HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								  if (Iter == m_ControlClientList.end())
								  {
									  ASSERT(false);
									  return;
								  }
								  SendNetCmdToClient(Iter->second, pMsg);
								  break;
	}
	case LC_OxAdmin:
	{
					   LC_CMD_OxAdminReq* pMsg = (LC_CMD_OxAdminReq*)pCmd;
					   HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
					   if (Iter == m_ControlClientList.end())
					   {
						   ASSERT(false);
						   return;
					   }
					   SendNetCmdToClient(Iter->second, pMsg);
					   break;
	}
	case LC_OxAdminQueryName:
	{
								LC_CMD_OxAdmin_PlayerNames* pMsg = (LC_CMD_OxAdmin_PlayerNames*)pCmd;
								HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								if (Iter == m_ControlClientList.end())
								{
									ASSERT(false);
									return;
								}
								SendNetCmdToClient(Iter->second, pMsg);
								break;
	}
	case LC_OxAdminQueryAllJetton:
	{
									 LC_CMD_OxAdmin_AllJetton* pMsg = (LC_CMD_OxAdmin_AllJetton*)pCmd;
									 HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
									 if (Iter == m_ControlClientList.end())
									 {
										 ASSERT(false);
										 return;
									 }
									 SendNetCmdToClient(Iter->second, pMsg);
									 break;
	}
	case LC_OxAdminQueryPlayerJetton:
	{
										LC_CMD_OxAdmin_PlayerJetton* pMsg = (LC_CMD_OxAdmin_PlayerJetton*)pCmd;
										HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
										if (Iter == m_ControlClientList.end())
										{
											ASSERT(false);
											return;
										}
										SendNetCmdToClient(Iter->second, pMsg);
										break;
	}
	case LC_AnimalAdmin:
	{
						   LC_CMD_AnimalAdminReq* pMsg = (LC_CMD_AnimalAdminReq*)pCmd;
						   HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
						   if (Iter == m_ControlClientList.end())
						   {
							   ASSERT(false);
							   return;
						   }
						   SendNetCmdToClient(Iter->second, pMsg);
						   break;
	}
	case LC_AnimalAdminQueryName:
	{
									LC_CMD_AnimalAdmin_PlayerNames* pMsg = (LC_CMD_AnimalAdmin_PlayerNames*)pCmd;
									HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
									if (Iter == m_ControlClientList.end())
									{
										ASSERT(false);
										return;
									}
									SendNetCmdToClient(Iter->second, pMsg);
									break;
	}
	case LC_AnimalAdminQueryAllJetton:
	{
										 LC_CMD_AnimalAdmin_AllJetton* pMsg = (LC_CMD_AnimalAdmin_AllJetton*)pCmd;
										 HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
										 if (Iter == m_ControlClientList.end())
										 {
											 ASSERT(false);
											 return;
										 }
										 SendNetCmdToClient(Iter->second, pMsg);
										 break;
	}
	case LC_AnimalAdminQueryPlayerJetton:
	{
											LC_CMD_AnimalAdmin_PlayerJetton* pMsg = (LC_CMD_AnimalAdmin_PlayerJetton*)pCmd;
											HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
											if (Iter == m_ControlClientList.end())
											{
												ASSERT(false);
												return;
											}
											SendNetCmdToClient(Iter->second, pMsg);
											break;
	}

	case LC_CarAdmin:
	{
						LC_CMD_CarAdminReq* pMsg = (LC_CMD_CarAdminReq*)pCmd;
						HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
						if (Iter == m_ControlClientList.end())
						{
							ASSERT(false);
							return;
						}
						SendNetCmdToClient(Iter->second, pMsg);
						break;
	}
	case LC_CarAdminQueryName:
	{
								 LC_CMD_CarAdmin_PlayerNames* pMsg = (LC_CMD_CarAdmin_PlayerNames*)pCmd;
								 HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
								 if (Iter == m_ControlClientList.end())
								 {
									 ASSERT(false);
									 return;
								 }
								 SendNetCmdToClient(Iter->second, pMsg);
								 break;
	}
	case LC_CarAdminQueryAllJetton:
	{
									  LC_CMD_CarAdmin_AllJetton* pMsg = (LC_CMD_CarAdmin_AllJetton*)pCmd;
									  HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
									  if (Iter == m_ControlClientList.end())
									  {
										  ASSERT(false);
										  return;
									  }
									  SendNetCmdToClient(Iter->second, pMsg);
									  break;
	}
	case LC_CarAdminQueryPlayerJetton:
	{
										 LC_CMD_CarAdmin_PlayerJetton* pMsg = (LC_CMD_CarAdmin_PlayerJetton*)pCmd;
										 HashMap<DWORD, ServerClientData* >::iterator Iter = m_ControlClientList.find(pMsg->ClientID);
										 if (Iter == m_ControlClientList.end())
										 {
											 ASSERT(false);
											 return;
										 }
										 SendNetCmdToClient(Iter->second, pMsg);
										 break;
	}
	}
	return;
}
void FishServer::HandleDataBaseMsg(NetCmd* pCmd)
{
	//�������ݿⷢ����������
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}

	return;
}
bool FishServer::OnDestroy()//�����Ʒ������رյ�ʱ��
{
	HandleAllMsg();
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		delete pOnce;
	}
	return true;
}
void FishServer::HandleAllMsg()
{
	//��ȫ������ȫ������� 
	if (!m_ControlClientList.empty())
	{
		HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlClientList.begin();
		for (; Iter != m_ControlClientList.end();)
		{
			if (!Iter->second)
				continue;
			while (Iter->second->RecvList.HasItem())
			{
				NetCmd* pCmd = Iter->second->RecvList.GetItem();
				HandleClientMsg(Iter->second, pCmd);
				free(pCmd);
			}
			++Iter;
		}
	}
	if (!m_ControlServerList.empty())
	{
		HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
		for (; Iter != m_ControlServerList.end();)
		{
			if (!Iter->second)
				continue;
			while (Iter->second->RecvList.HasItem())
			{
				NetCmd* pCmd = Iter->second->RecvList.GetItem();
				HandleServerMsg(Iter->second, pCmd);
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
}
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//���������ֻ����FTP �� DB�����ӳɹ���Ź���
	return CONNECT_CHECK_OK;
}
bool FishServer::NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize)
{
	if (!pClient)
	{
		ASSERT(false);
		return false;
	}
	if (!pData || (SeverID != m_ControlServerNetworkID && SeverID != m_ControlClientNetworkID) || (SeverID == m_ControlServerNetworkID && recvSize != sizeof(BYTE)) || (SeverID == m_ControlClientNetworkID && recvSize != 0))
	{
		m_ClientTcp.Kick(pClient);
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
	if (recvSize > 0)
	{
		pOnce->pPoint = (void*)malloc(recvSize);
		if (!pOnce->pPoint)
		{
			m_ClientTcp.Kick(pClient);
			delete pOnce;
			ASSERT(false);
			return false;
		}
		memcpy_s(pOnce->pPoint, recvSize, pData, recvSize);
	}
	pOnce->Length = recvSize;
	m_AfxAddClient.AddItem(pOnce);
	return true;
}
void FishServer::Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
{
	//��� �ͻ����뿪��״̬
	switch (rt)
	{
	case REMOVE_NONE:
	{
						ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��ΪNone", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_NORMAL:
	{
						  ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ��ͨ", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_RECV_ERROR:
	{
							  ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���մ���", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_SEND_ERROR:
	{
							  ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���ʹ���", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_TIMEOUT:
	{
						   ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ��ʱ", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_CMD_SEND_OVERFLOW:
	{
									 ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_CMD_RECV_OVERFLOW:
	{
									 ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_CMD_SIZE_ERROR:
	{
								  ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ�����С", pClient->OutsideExtraData);
	}
		break;
	case REMOVE_RECVBACK_NOT_SPACE:
	{
									  ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���ջ��������", pClient->OutsideExtraData);
	}
		break;
	}
}
void FishServer::OnAddClient()
{
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		if (!pOnce)
		{
			continue;
		}
		if (pOnce->SeverID == m_ControlServerNetworkID)
		{
			if (!pOnce->pPoint || pOnce->Length != sizeof(BYTE))
			{
				m_ClientTcp.Kick(pOnce->pClient);
				//ASSERT(false);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			BYTE ClientID = *((BYTE*)pOnce->pPoint);
			if (ClientID == 0)
			{
				m_ClientTcp.Kick(pOnce->pClient);
				//ASSERT(false);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			ShowInfoToWin("�ڲ��ͻ��� ID:%d ����", ClientID);

			pOnce->pClient->OutsideExtraData = ClientID;
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(ClientID);
			if (Iter != m_ControlServerList.end())
			{
				ASSERT(false);
				m_ServerTcp.Kick(pOnce->pClient);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			m_ControlServerList.insert(HashMap<BYTE, ServerClientData*>::value_type(ClientID, pOnce->pClient));
			//������Ҽ�����¼�
			OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;
			continue;
		}
		else if (pOnce->SeverID == m_ControlClientNetworkID)
		{
			if (pOnce->pPoint || pOnce->Length != 0)
			{
				m_ClientTcp.Kick(pOnce->pClient);
				delete pOnce;
				continue;
			}
			pOnce->pClient->OutsideExtraData = ++m_ControlClientIndex;
			pOnce->pClient->dwChecked = 0;
			HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlClientList.find(pOnce->pClient->OutsideExtraData);
			if (Iter != m_ControlClientList.end())
			{
				ASSERT(false);
				m_ClientTcp.Kick(pOnce->pClient);
				delete pOnce;
				continue;
			}
			m_ControlClientList.insert(HashMap<DWORD, ServerClientData*>::value_type(pOnce->pClient->OutsideExtraData, pOnce->pClient));
			//������Ҽ�����¼�
			OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
			delete pOnce;
			continue;
		}
		else
		{
			delete pOnce;
			//ASSERT(false);
			continue;
		}
	}
}
void FishServer::ShowInfoToWin(const char *pcStr, ...)
{
	if (!pcStr)
	{
		ASSERT(false);
		return;
	}
	va_list var;
	va_start(var, pcStr);
	UINT nCount = _vscprintf(pcStr, var);
	char *pBuffer = new char[nCount + 1];
	vsprintf_s(pBuffer, nCount + 1, pcStr, var);
	std::cout << pBuffer;
	std::cout << "\n";
	SAFE_DELETE_ARR(pBuffer);
	va_end(var);
}
void FishServer::SendNetCmdToClient(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pCmd || !pClient)
	{
		ASSERT(false);
		return;
	}
	if (!m_ClientTcp.Send(pClient, pCmd))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToAllClient(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlClientList.begin();
	for (; Iter != m_ControlClientList.end(); ++Iter)
	{
		if (!m_ClientTcp.Send(Iter->second, pCmd))
		{
			ASSERT(false);
		}
	}
}
void  FishServer::SendNetCmdToControlDB(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!m_DBTcp.Send(pCmd, true))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToDB(BYTE DBID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(DBID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToAllDB(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
	for (; Iter != m_ControlServerList.end(); ++Iter)
	{
		if (g_FishServerConfig.GetDBServerConfig(Iter->first))
		{
			if (!m_ServerTcp.Send(Iter->second, pCmd))
			{
				ASSERT(false);
			}
		}
	}
}
void  FishServer::SendNetCmdToFtp(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!g_FishServerConfig.GetFTPServerConfig())
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(g_FishServerConfig.GetFTPServerConfig()->NetworkID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToRank(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!g_FishServerConfig.GetRankServerConfig())
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(g_FishServerConfig.GetRankServerConfig()->NetworkID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToLogon(BYTE LogonID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(LogonID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToAllLogon(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
	for (; Iter != m_ControlServerList.end(); ++Iter)
	{
		if (g_FishServerConfig.GetLogonServerConfig(Iter->first))
		{
			if (!m_ServerTcp.Send(Iter->second, pCmd))
			{
				ASSERT(false);
			}
		}
	}
}
void  FishServer::SendNetCmdToGame(BYTE GameID, NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(GameID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void  FishServer::SendNetCmdToAllGame(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
	for (; Iter != m_ControlServerList.end(); ++Iter)
	{
		if (g_FishServerConfig.GetGameServerConfig(Iter->first))
		{
			if (!m_ServerTcp.Send(Iter->second, pCmd))
			{
				ASSERT(false);
			}
		}
	}
}
void FishServer::SendNetCmdToAllServer(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.begin();
	for (; Iter != m_ControlServerList.end(); ++Iter)
	{
		if (!m_ServerTcp.Send(Iter->second, pCmd))
		{
			ASSERT(false);
		}
	}
}
void  FishServer::SendNetCmdToCenter(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!g_FishServerConfig.GetCenterServerConfig())
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(g_FishServerConfig.GetCenterServerConfig()->NetworkID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToMiniGame(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!g_FishServerConfig.GetMiniGameServerConfig())
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(g_FishServerConfig.GetMiniGameServerConfig()->NetworkID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToOperate(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!g_FishServerConfig.GetOperateConfig())
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ControlServerList.find(g_FishServerConfig.GetOperateConfig()->NetworkID);
	if (Iter == m_ControlServerList.end())
	{
		ASSERT(false);
		return;
	}
	if (!m_ServerTcp.Send(Iter->second, pCmd))
	{
		ASSERT(false);
	}
}
FishServer g_FishServer;