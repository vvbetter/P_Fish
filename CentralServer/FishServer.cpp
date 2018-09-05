#include "stdafx.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"

FishServer g_FishServer;

void SendLogDB(NetCmd* pCmd)
{
	g_FishServer.SendNetCmdToLogDB(pCmd);
}
void SendDB(NetCmd* pCmd)
{
	g_FishServer.SendNetCmdToDB(pCmd);
}


FishServer::FishServer()
{
	m_IsClose = false;
	m_DBIsConnect = false;
	m_FtpIsConnect = false;
	m_LogDBIsConnect = false;

	m_CenterworkID = 0;
	//m_ControlworkID = 0;
	//m_ControlIndex = 0;

	m_IsReloadConfig = false;

	m_ControlIsConnect = false;
}
FishServer::~FishServer()
{

}

bool FishServer::InitServer(bool IsInit)
{
	m_pDump.OnInit();//������¼dump
	if (!g_FishServerConfig.LoadServerConfigFile())
	{
		Log("�����������ļ���ȡʧ��");
		return false;
	}
	if (!m_FishConfig.LoadConfigFilePath())
	{
		ASSERT(false);
		return false;
	}
	Log("�����ļ���ȡ�ɹ�");
	if (!m_MonthManager.OnInit())
	{
		ASSERT(false);
		return false;
	}
	Log("������������ʼ���ɹ�");
	m_GlobelTaskManager.OnInit();
	Log("����ȫ�����ݳ�ʼ���ɹ�");
	CenterServerConfig* pConfig= g_FishServerConfig.GetCenterServerConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return false;
	}
	m_CenterworkID = pConfig->NetworkID;
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
	OnCenterFinish();
	//DBR_Cmd_ClearFishDB msg;
	//SetMsgInfo(msg, DBR_ClearFishDB, sizeof(DBR_Cmd_ClearFishDB));
	//msg.IsInit = IsInit;
	//SendNetCmdToDB(&msg);

	return true;
}
void FishServer::OnCenterFinish()
{
	if (!ConnectClient())
	{
		ASSERT(false);
		return;
	}
	SendGetAchievementList();
	m_AnnouncementManager.OnLoadAllAnnouncementByDB();
	m_CashManager.Init();
	Log("��������������ɹ�");
	return;
}
bool FishServer::ConnectDB()
{
	CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
	if (!pCenterConfig)
	{
		ASSERT(false);
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pCenterConfig->DBNetworkID);
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
	rs.NetWorkID = m_CenterworkID;
	if (!m_DBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&rs, sizeof(Reg_Server)))
	{
		Log("���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
bool FishServer::ConnectLogDB()
{
	CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
	if (!pCenterConfig)
	{
		ASSERT(false);
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pCenterConfig->LogDBID);
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
	rs.NetWorkID = m_CenterworkID;
	if (!m_LogDBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&rs, sizeof(Reg_Server)))
	{
		Log("Log���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_LogDBTcp);
	return true;
}
bool FishServer::ConnectClient()
{
	CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
	if (!pCenterConfig)
	{
		ASSERT(false);
		return false;
	}
	ServerInitData pCLientData;
	pCLientData.ServerID = pCenterConfig->NetworkID;
	pCLientData.BuffSize = pCenterConfig->BuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.ListenCount = pCenterConfig->ListenCount;
	pCLientData.MaxRecvCmdCount = pCenterConfig->MaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pCenterConfig->MaxSendCountPerFrame;
	pCLientData.Port = pCenterConfig->CenterListenPort;
	pCLientData.SocketRecvSize = pCenterConfig->RecvBuffSize;
	pCLientData.SocketSendSize = pCenterConfig->SendBuffSize;
	pCLientData.Timeout = pCenterConfig->TimeOut;
	pCLientData.RecvThreadNum = pCenterConfig->RecvThreadSum;
	pCLientData.SendThreadNum = pCenterConfig->SendThreadSum;
	pCLientData.SleepTime = pCenterConfig->SleepTime;
	pCLientData.AcceptSleepTime = pCenterConfig->AcceptSleepTime;
	pCLientData.MaxAcceptNumPerFrame = pCenterConfig->MaxAcceptNumPerFrame;
	pCLientData.SceneHearbeatTick = pCenterConfig->SceneHearbeatTick;
	pCLientData.AcceptRecvData = true;
	pCLientData.SceneHearbeatTick = pCenterConfig->SceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pCenterConfig->MaxSendCmdCount;
	/*pCLientData.Version = 0;
	pCLientData.PathCrc = 0;*/
	m_ClientTcp.SetCmdHandler(this);
	if (!m_ClientTcp.Init(pCLientData))
	{
		Log("�������������ʧ��");
		return false;
	}
	
	return true;
}
ServerClientData* FishServer::GetUserClientDataByIndex(BYTE IndexID)
{
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.find(IndexID);
	if (Iter == m_ClintList.end() || !Iter->second)
		return NULL;
	else
		return Iter->second;
}
bool FishServer::OnDestroy()
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
	if (!m_ClintList.empty())
	{
		HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
		for (; Iter != m_ClintList.end();)
		{
			if (!Iter->second)
				continue;
			while (Iter->second->RecvList.HasItem())
			{
				NetCmd* pCmd = Iter->second->RecvList.GetItem();
				HandleGameServerMsg(Iter->first, Iter->second, pCmd);
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
	//pCmd = m_FtpTcp.GetCmd();
	//while (pCmd)
	//{
	//	HandleFtpMsg(pCmd);
	//	free(pCmd);
	//	pCmd = m_FtpTcp.GetCmd();
	//}
	//pCmd = m_ControlTcp.GetCmd();
	//while (pCmd)
	//{
	//	HandleControlMsg(pCmd);
	//	free(pCmd);
	//	pCmd = m_ControlTcp.GetCmd();
	//}
}
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//���������ֻ����FTP �� DB�����ӳɹ���Ź���
	if (m_DBTcp.IsConnected()/* && m_FtpTcp.IsConnected()*/ && m_LogDBTcp.IsConnected())
		return CONNECT_CHECK_OK;
	else
		return CONNECT_CHECK_FAILED;
}
bool FishServer::NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize)
{
	if (!pClient)
	{
		ASSERT(false);
		return false;
	}
	if (!pData || SeverID != m_CenterworkID  || recvSize != sizeof(Reg_Server))
	{
		m_ClientTcp.Kick(pClient);
		//ASSERT(false);
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
			Log("�ͻ��� ID:%d �뿪ԭ��ΪNone",pClient->OutsideExtraData);
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
void FishServer::OnAddClient()
{
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		if (!pOnce)
		{
			continue;
		}
		if (pOnce->SeverID == m_CenterworkID)
		{
			if (!pOnce->pPoint || pOnce->Length != sizeof(Reg_Server))
			{
				m_ClientTcp.Kick(pOnce->pClient);
				//ASSERT(false);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			BYTE ClientID = (((Reg_Server*)(pOnce->pPoint))->NetWorkID);
			if (ClientID == 0)
			{
				m_ClientTcp.Kick(pOnce->pClient);
				//ASSERT(false);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			pOnce->pClient->OutsideExtraData = ClientID;
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.find(ClientID);
			if (Iter != m_ClintList.end())
			{
				ASSERT(false);
				m_ClientTcp.Kick(pOnce->pClient);
				if (pOnce->pPoint)
					free(pOnce->pPoint);
				delete pOnce;
				continue;
			}
			m_ClintList.insert(HashMap<BYTE, ServerClientData*>::value_type(ClientID, pOnce->pClient));
			//������Ҽ�����¼�
			OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;

			//���ͻ��˼����ʱ�� ���Ǵ����¼� 
			if (g_FishServerConfig.IsLogonServerConfigID(ClientID))
			{
				m_CenterServerManager.RsgLogon(ClientID);
			}
			else if (g_FishServerConfig.IsGameServerConfigID(ClientID))
			{
				m_CenterServerManager.RsgGame(ClientID);
				m_GlobelTaskManager.OnGameServerRsg(ClientID);
				//SendUserAchievementToGameServer(ClientID);//GameServer�����ʱ�� ���ͳɾ͵�����������
			}
			else if (ClientID == 255) //��Ϸ����ID
			{
				m_CenterServerManager.RsgGameHall(ClientID);
			}
			else
			{
				Log("һ����Ӫ���������뵽Center  %d",ClientID);
			}
			continue;
		}
		//else if (pOnce->SeverID == m_ControlworkID)
		//{
		//	if (pOnce->pPoint || pOnce->Length !=0)
		//	{
		//		m_ClientTcp.Kick(pOnce->pClient);
		//		delete pOnce;
		//		continue;
		//	}
		//	pOnce->pClient->OutsideExtraData = ++m_ControlIndex;
		//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlList.find(pOnce->pClient->OutsideExtraData);
		//	if (Iter != m_ControlList.end())
		//	{
		//		ASSERT(false);
		//		m_ClientTcp.Kick(pOnce->pClient);
		//		delete pOnce;
		//		continue;
		//	}
		//	m_ControlList.insert(HashMap<DWORD, ServerClientData*>::value_type(pOnce->pClient->OutsideExtraData, pOnce->pClient));
		//	//������Ҽ�����¼�
		//	OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
		//	delete pOnce;
		//	continue;
		//}
		else
		{
			delete pOnce;
			//ASSERT(false);
			continue;
		}
	}
}
void FishServer::OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�����������һ���ͻ����뿪��ʱ��
	if (ServerID == m_CenterworkID)
	{
		//��GameServer�뿪��ʱ�� �õ�ǰGameServer��ȫ�����������
		//m_RoleManager.OnPlazaLeave(pClient->OutsideExtraData);
		m_CenterServerManager.OnClientLeave(ConvertDWORDToBYTE(pClient->OutsideExtraData));//һ���ͻ����뿪��
	}
	//else if (ServerID == m_ControlworkID)
	//{
	//	//һ���������뿪��

	//}
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
	if (ServerID == m_CenterworkID)
	{
		//std::cout << "һ��Client������CentralServer\n";
	}
	//else if (ServerID == m_ControlworkID)
	//{
	//	//һ��������������
	//}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::OnTcpClientConnect(TCPClient* pClient)
{
	//��������ӳɹ���ʱ��
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//if (pClient == &m_FtpTcp)
	//{
	//	m_FtpIsConnect = true;
	//	Log("�Ѿ���Ftp���ӳɹ���");
	//}
	if (pClient == &m_DBTcp)
	{
		m_DBIsConnect = true;
		Log("�Ѿ������ݿ����ӳɹ���");
	}
	else if (pClient == &m_LogDBTcp)
	{
		m_LogDBIsConnect = true;
		Log("�Ѿ���Log���ݿ����ӳɹ���");
	}
	//else if (pClient == &m_ControlTcp)
	//{
	//	m_ControlIsConnect = true;
	//	Log("�Ѿ���ControlServer���ӳɹ���");
	//}
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
	/*if (pClient == &m_FtpTcp)
	{
	Log("�Ѿ���Ftp�Ͽ�������");
	m_FtpIsConnect = false;
	m_FtpTcp.Shutdown();
	}
	else*/ if (pClient == &m_DBTcp)
	{
		Log("�Ѿ������ݿ�Ͽ�������");
		m_DBIsConnect = false;
		m_DBTcp.Shutdown();
	}
	else if (pClient == &m_LogDBTcp)
	{
		Log("�Ѿ���Log���ݿ�Ͽ�������");
		m_LogDBIsConnect = false;
		m_LogDBTcp.Shutdown();
	}
	//else if (pClient == &m_ControlTcp)
	//{
	//	m_ControlIsConnect = false;
	//	m_ControlTcp.Shutdown();
	//	Log("�Ѿ���ControlServer�Ͽ�������");
	//}
	else
	{
		ASSERT(false);
	}
	return;
}
void FishServer::SendNetCmdToDB(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!m_DBTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToLogDB(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!m_LogDBTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToFtp(NetCmd* pCmd)
{
	//if (!pCmd)
	//{
	//	ASSERT(false);
	//	return;
	//}
	//if (!m_FtpTcp.Send(pCmd, false))
	//{
	//	ASSERT(false);
	//}
}
void FishServer::SendNetCmdToClient (ServerClientData* pClient, NetCmd* pCmd)
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
void FishServer::SendNetCmdToControl( NetCmd*pCmd)
{
	/*if (!pCmd || !pClient)
	{
		ASSERT(false);
		return;
	}
	m_ClientTcp.Send(pClient, pCmd);*/

	//if (!pCmd)
	//{
	//	ASSERT(false);
	//	return;
	//}
	//if (!m_ControlTcp.Send(pCmd, false))
	//{
	//	ASSERT(false);
	//}
}
void FishServer::UpdateByMin(DWORD dwTimer)//�����������Ҫ����ѭ��
{
	static DWORD DayChageTimeUpdate = 0;
	if (DayChageTimeUpdate != 0 && dwTimer - DayChageTimeUpdate < 60000)
		return;
	DayChageTimeUpdate = dwTimer;
	SYSTEMTIME time;
	GetLocalTime(&time);
	bool IsMinChange = true;
	bool IsHourChange = (time.wMinute == GetFishConfig().GetFishUpdateConfig().UpdateMin);
	bool IsDayChange = ((time.wHour == GetFishConfig().GetFishUpdateConfig().UpdateHour) && IsHourChange);
	bool IsMonthChange = ((time.wDay == 1) && IsDayChange);
	bool IsYearChange = ((time.wMonth == 1) && IsMonthChange);
	bool IsNewDay = (time.wHour == 0 && time.wMinute == 0);
	m_GlobelTaskManager.UpdateByMin(IsHourChange, IsDayChange, IsMonthChange, IsYearChange);

	if (IsHourChange)
	{
		//SendGetAchievementList();//�ɾ͵����� ÿСʱһ��
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
		}

		OnAddClient();

		//m_MonthManager.Update(dwTimer);

		UpdateByMin(dwTimer);

		m_FishNoticeManager.OnUpdateByMin(dwTimer);

		UpdateInfoToControl(dwTimer);

		//m_CashManager.Update();

		//Center��������ÿһ֡���������  ������һ֡��ȫ�����������
		//1.����GameServer ������
		if (!m_ClintList.empty())
		{
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
			for (; Iter != m_ClintList.end();)
			{
				if (!Iter->second)
				{
					ASSERT(false);
					continue;
				}
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_CenterworkID, Iter->second);
					m_ClientTcp.Kick(Iter->second);
					Iter = m_ClintList.erase(Iter);
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
					HandleGameServerMsg(Iter->first, Iter->second, pCmd);
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

		

		////3.Ftp
		//if (m_FtpTcp.IsConnected())
		//{
		//	if (!m_FtpIsConnect)
		//		OnTcpClientConnect(&m_FtpTcp);
		//	NetCmd* pCmd = m_FtpTcp.GetCmd();
		//	int Sum = 0;
		//	while (pCmd && Sum < Msg_OnceSum)
		//	{
		//		HandleFtpMsg(pCmd);
		//		free(pCmd);
		//		++Sum;
		//		pCmd = m_FtpTcp.GetCmd();
		//	}
		//	if (pCmd)
		//	{
		//		HandleFtpMsg(pCmd);
		//		free(pCmd);
		//	}
		//}
		//else
		//{
		//	if (m_FtpIsConnect)
		//		OnTcpClientLeave(&m_FtpTcp);

		//	//��������DB
		//	ConnectFTP();
		//}
		//Control
		//if (!m_ControlList.empty())
		//{
		//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ControlList.begin();
		//	for (; Iter != m_ControlList.end();)
		//	{
		//		if (!Iter->second)
		//		{
		//			ASSERT(false);
		//			continue;
		//		}
		//		if (Iter->second->Removed)
		//		{
		//			OnTcpServerLeave(m_CenterworkID, Iter->second);
		//			m_ControlTcp.Kick(Iter->second);
		//			Iter = m_ControlList.erase(Iter);
		//			continue;
		//		}
		//		int Sum = 0;
		//		while (Iter->second->RecvList.HasItem() && Sum < Client_Msg_OnceSum)//�������������
		//		{
		//			NetCmd* pCmd = Iter->second->RecvList.GetItem();
		//			if (!pCmd)
		//			{
		//				ASSERT(false);
		//				continue;
		//			}
		//			//������������ �ͻ��˷������ĵ�½ ע�� ������
		//			HandleControlMsg(Iter->second, pCmd);
		//			free(pCmd);
		//			++Sum;
		//		}
		//		++Iter;
		//	}
		//}

		//if (m_ControlTcp.IsConnected())
		//{
		//	if (!m_ControlIsConnect)
		//		OnTcpClientConnect(&m_ControlTcp);
		//	NetCmd* pCmd = m_ControlTcp.GetCmd();
		//	int Sum = 0;
		//	while (pCmd && Sum < Msg_OnceSum)
		//	{
		//		HandleControlMsg(pCmd);
		//		free(pCmd);
		//		++Sum;
		//		pCmd = m_ControlTcp.GetCmd();
		//	}
		//	if (pCmd)
		//	{
		//		HandleControlMsg(pCmd);
		//		free(pCmd);
		//	}
		//}
		//else
		//{
		//	if (m_ControlIsConnect)
		//		OnTcpClientLeave(&m_ControlTcp);

		//	//��������DB
		//	ConnectControl();
		//}

		Sleep(1);
	}
	return OnDestroy();
}
bool FishServer::HandleGameServerMsg(BYTE serverID, ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return false;

	if (pCmd->GetMainType() == Main_Hall)
	{
		switch (pCmd->GetSubType())
		{
		case 63: //��Ҵ���Ϸ���������¼��Ϸ
		{
			if (pCmd == NULL)
			{
				ASSERT(false);
				return true;
			}
			uint gameID = m_CenterServerManager.GetFreeGameServer();
			m_CenterServerManager.OnGameClientJoin(gameID);
			m_CenterServerManager.SendNetCmdToGameServer(gameID, pCmd);
			return true;
		}
		case 67:// ��ҽ�ҸĶ�������֪ͨ������Ϸ
		{
			m_CenterServerManager.SendNetCmdToAllGameServer(pCmd);
			break;
		}
		case 70: //��ҴӲ�����Ϸ�������˳���֪ͨ��Ϸ����
		{
			m_CenterServerManager.OnGameClientLeave(serverID);
			pCmd->SetCmdType(GetMsgType(70, 24));
			m_CenterServerManager.SendNetCmdToGameHall(255, pCmd);
			break;
		}
		case 71: //�رղ��������
		{
			m_CenterServerManager.SendNetCmdToAllGameServer(pCmd);
			break;
		}
		case 72://֪ͨ���ز������ñ�
		{
			m_CenterServerManager.SendNetCmdToAllGameServer(pCmd);
			break;
		}
		case 73://֪ͨ��Ϸ����������ҽ���
		{
			m_CenterServerManager.SendNetCmdToAllGameServer(pCmd);
			break;
		}
		case 162://6306 ʹ����Ʒ
		{
			m_CenterServerManager.SendNetCmdToAllGameServer(pCmd);
			break;
		}
		case 164://��Ϸ���������������socket,ip�˿�
		{
			pCmd->SetCmdType(GetMsgType(64,24));
			m_CenterServerManager.SendNetCmdToGameServer(255, pCmd);
			return true;
		}
		case 255: //��Ϸ������ͬ�����ݵ���Ϸ����
			pCmd->SetCmdType(GetMsgType(68, 24));
			m_CenterServerManager.SendNetCmdToGameHall(255, pCmd);
			return true;
		}
	}
	else if (pCmd->GetMainType() == Main_HallHeartBeat)
	{
		m_CenterServerManager.SendNetCmdToGameHall(255, pCmd);
	}
	else if (pCmd->GetMainType() == Main_Center)
	{
		switch (pCmd->GetSubType())
		{
		case CL_Sub_UserEnter:
			{
				if (pCmd->GetCmdSize() != sizeof(CL_UserEnter) || !pCmd)
				{
					ASSERT(false);
					return true;
				}
				CL_UserEnter * pMsg = (CL_UserEnter*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return true;
				}
				if (m_RoleManager.QueryCenterUser(pMsg->RoleInfo.dwUserID))
				{
					//����Ѿ��ظ���½ ��������һ���������
					CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->RoleInfo.dwUserID);
					if (!pRole)
					{
						ASSERT(false);
						return true;
					}
					if (pRole->GetSocketID() != pClient->OutsideExtraData)
					{
						//����ظ���½ �����ڲ�ͬ��GameServer���� 
						//�������GameServerȥ
						CC_Cmd_UserLeaveGame msg;
						SetMsgInfo(msg, GetMsgType(Main_Center, CC_UserLeaveGame), sizeof(CC_Cmd_UserLeaveGame));
						msg.dwUserID = pMsg->RoleInfo.dwUserID;
						pRole->SendDataToGameServer(&msg);
					}
					else
					{
						//ֱ���쳣�� 
						ASSERT(false);
						return false;
					}
				}
				m_RoleManager.OnCreateCenterRole(ConvertDWORDToBYTE(pClient->OutsideExtraData), &pMsg->RoleInfo,pMsg->IsRobot);//����һ��������ҵ��˺�
				return true;
			}
		case CL_Sub_UserEnterFinish:
			{
				if (pCmd->GetCmdSize() != sizeof(CL_UserEnterFinish) || !pCmd)
				{
					ASSERT(false);
					return true;
				}
				CL_UserEnterFinish * pMsg = (CL_UserEnterFinish*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return true;
				}
				m_RoleManager.SetCenterUserFinish(pMsg->dwUserID);

				m_CenterServerManager.RsgUser(pMsg->dwUserID, ConvertDWORDToBYTE(pClient->OutsideExtraData));

				m_MonthManager.SendRoleSignUpInfoToGameServer(pMsg->dwUserID);

				return true;
			}
		case CL_Sub_UserLeave:
			{
				if (pCmd->GetCmdSize() != sizeof(CL_UserLeave) || !pCmd)
				{
					ASSERT(false);
					return true;
				}
				CL_UserLeave * pMsg = (CL_UserLeave*)pCmd;
				m_RoleManager.OnDelCenterRole(pMsg->dwUserID);

				m_CenterServerManager.UnRsgUser(pMsg->dwUserID);
				return true;
			}
		}
	}
	else if (pCmd->GetMainType() == Main_Char)
	{
		switch (pCmd->GetSubType())
		{
		case GC_SendCharInfo:
			{
				GC_Cmd_SendCharInfo* pMsg = (GC_Cmd_SendCharInfo*)pCmd;
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->MessageInfo.DestUserID);
				if (!pRole)
				{
					DBR_Cmd_AddCharInfo msg;
					SetMsgInfo(msg, DBR_AddCharInfo, sizeof(DBR_Cmd_AddCharInfo));
					msg.MessageInfo = pMsg->MessageInfo;
					SendNetCmdToDB(&msg);
				}
				else
				{
					CG_Cmd_SendCharInfo msg;
					SetMsgInfo(msg, GetMsgType(Main_Char, CG_SendCharInfo), sizeof(CG_Cmd_SendCharInfo));
					msg.MessageInfo = pMsg->MessageInfo;
					pRole->SendDataToGameServer(&msg);
				}
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_Relation)
	{
		//��ϵ�����������
		switch (pCmd->GetSubType())
		{
		case CC_LoadUserRelation:
		{
			CC_Cmd_GetUserRelation* pMsg = (CC_Cmd_GetUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			pRole->GetRelationManager().OnLoadUserRelation(pMsg);
			return true;
		}
		case CC_LoadBeUserRelation:
		{
			CC_Cmd_LoadBeUserRelation* pMsg = (CC_Cmd_LoadBeUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			pRole->GetRelationManager().OnLoadUserBeRelation(pMsg);
			return true;
		}
		case CC_AddUserRelation:
		{
			CC_Cmd_AddUserRelation* pMsg = (CC_Cmd_AddUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			pRole->GetRelationManager().OnAddUserRelation(pMsg);
			return true;
		}
		case CC_DelUserRelation:
		{
			CC_Cmd_DelUserRelation* pMsg = (CC_Cmd_DelUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			pRole->GetRelationManager().OnDelUserRelation(pMsg);
			return true;
		}
		case CC_ChangeUserRelation:
		{
			CC_Cmd_ChangeUserRelation* pMsg = (CC_Cmd_ChangeUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			pRole->GetRelationManager().OnChangeUserRelation(pMsg);
			return true;
		}
		}
	}
	else if (pCmd->GetMainType() == Main_Mail)
	{
		switch (pCmd->GetSubType())
		{
		case GC_SendUserMail://ֻ�ǽ���ת������
		{
			GC_Cmd_SendUserMail* pMsg = (GC_Cmd_SendUserMail*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->DestUserID);
			if (pRole)
			{
				//������� ֱ�ӷ������ȥ
				CG_Cmd_SendUserMail msg;
				SetMsgInfo(msg, GetMsgType(Main_Mail, CG_SendUserMail), sizeof(CG_Cmd_SendUserMail));
				msg.DestUserID = pMsg->DestUserID;
				msg.MailInfo = pMsg->MailInfo;
				pRole->SendDataToGameServer(&msg);
			}
			else
			{
				//��Ҳ����� ֱ�ӷ������ݿ�����
				DBR_Cmd_AddUserMail msg;
				SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
				msg.dwDestUserID = pMsg->DestUserID;//��ǰ���
				msg.MailInfo = pMsg->MailInfo;
				SendNetCmdToDB(&msg);
			}
			return true;
		}
		case GC_SendUserMailResult:
		{
			GC_Cmd_SendUserMailResult* pMsg = (GC_Cmd_SendUserMailResult*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->SrcUserID);
			if (!pRole)
			{
				ASSERT(false);
				return true;
			}
			//������ת�����ͻ���ȥ
			CG_Cmd_SendUserMailResult msg;
			SetMsgInfo(msg, GetMsgType(Main_Mail, CG_SendUserMailResult), sizeof(CG_Cmd_SendUserMailResult));
			msg.SrcUserID = pMsg->SrcUserID;
			msg.DestUserID = pMsg->DestUserID;
			msg.Result = pMsg->Result;
			pRole->SendDataToGameServer(&msg);
			return true;
		}
		case CC_SendSystemMail:
		{
			CC_Cmd_SendSystemMail* pMsg = (CC_Cmd_SendSystemMail*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			//������͵����ݿ�ȥ ϵͳ�ʼ��� DBR����ִ��
			DBR_Cmd_AddUserMail msg;
			SetMsgInfo(msg,DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
			msg.dwDestUserID = pMsg->dwDestUserID;
			msg.MailInfo = pMsg->MailInfo;
			SendNetCmdToDB(&msg);
			return true;
		}
		}
	}
	else if (pCmd->GetMainType() == Main_Role)
	{
		//�����ص�����
		switch (pCmd->GetSubType())
		{
		case CC_ChangeRoleLevel:
		{
			CC_Cmd_ChangeRoleLevel * pMsg = (CC_Cmd_ChangeRoleLevel*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			//��ҵȼ������仯��ʱ��
			pRole->OnChangeRoleLevel(pMsg->wLevel);
			return true;
		}
		case CC_ChangeRoleFaceID:
		{
			CC_Cmd_ChangeRoleFaceID * pMsg = (CC_Cmd_ChangeRoleFaceID*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleFaceID(pMsg->dwFaceID);
			return true;
		}
		case CC_ChangeRoleGender:
		{
			CC_Cmd_ChangeRoleGender * pMsg = (CC_Cmd_ChangeRoleGender*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleGender(pMsg->bGender);
			return true;
		}
		case CC_ChangeRoleNickName:
		{
			CC_Cmd_ChangeRoleNickName * pMsg = (CC_Cmd_ChangeRoleNickName*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return true;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleNickName(pMsg->NickName);
			return true;
		}
		case CC_ChangeRoleTitle:
		{
			CC_Cmd_ChangeRoleTitle* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleTitle);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleTitle(pMsg->TitleID);
			return true;
		}
		case CC_ChangeRoleAchievementPoint:
		{
			CC_Cmd_ChangeRoleAchievementPoint* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleAchievementPoint);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleAchievementPoint(pMsg->dwAchievementPoint);
			return true;
		}
		case CC_ChangeRoleCharmValue:
		{
			CC_Cmd_ChangeRoleCharmValue* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleCharmValue);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			pRole->OnChangeRoleCharmValue(pMsg->CharmInfo);
			return true;
		}
		case CC_ChangeRoleClientIP:
			{
				CC_Cmd_ChangeRoleClientIP* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleClientIP);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleClientIP( pMsg->ClientIP);
				return true;
			}
		case CC_ChangeRoleIsShowIpAddress:
			{
				CC_Cmd_ChangeRoleIsShowIpAddress* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleIsShowIpAddress);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleIsShowIpAddress(pMsg->IsShowIpAddress);
				return true;
			}
		case CC_ChangeRoleIsOnline:
			{
				CC_Cmd_ChangeRoleIsOnline* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleIsOnline);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleIsOnline(pMsg->IsOnline);
				return true;
			}
		case CC_ChangeRoleVipLevel:
			{
				CC_Cmd_ChangeRoleVipLevel* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleVipLevel);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleVipLevel(pMsg->VipLevel);
				return true;
			}
		case CC_ChangeRoleIsInMonthCard:
			{
				CC_Cmd_ChangeRoleIsInMonthCard* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleIsInMonthCard);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleIsInMonthCard(pMsg->IsInMonthCard);
				return true;
			}
		case CC_ChangeRoleParticularStates:
			{
				CC_Cmd_ChangeRoleParticularStates* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeRoleParticularStates);
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				pRole->OnChangeRoleParticularStates(pMsg->ParticularStates);
				return true;
			}
		}
	}
	else if (pCmd->GetMainType() == Main_Month)
	{
		switch (pCmd->GetSubType())
		{
		case GC_SignUpMonth:
		{
			GC_Cmd_SignUpMonth* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_SignUpMonth);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->MonthID);
			if (!pMonth)
			{
				ASSERT(false);
				return false;
			}
			pMonth->SignUp(pMsg->dwUserID);
			return true;
		}
		case GC_SignUpMonthResult:
		{
			GC_Cmd_SignUpMonthResult* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_SignUpMonthResult);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->MonthID);
			if (!pMonth)
			{
				ASSERT(false);
				return false;
			}
			pMonth->SignUpResult(pMsg);
			return true;
		}
		case GC_JoinMonth:
		{
			GC_Cmd_JoinMonth* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_JoinMonth);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->MonthID);
			if (!pMonth)
			{
				CG_Cmd_JoinMonth msg; 
				SetMsgInfo(msg,GetMsgType(Main_Month, CG_JoinMonth), sizeof(CG_Cmd_JoinMonth));
				msg.Result = false;
				msg.MonthInfo.dwUserID = pMsg->dwUserID;
				CenterRole* pUser = g_FishServer.GetRoleManager().QueryCenterUser(pMsg->dwUserID);
				if (pUser)
					pUser->SendDataToGameServer(&msg);
				return false;
			}
			pMonth->JoinMonth(pMsg->dwUserID,pMsg->TableID);
			return true;
		}
		case GC_ChangeUserMonthInfo:
		{
			GC_Cmd_ChangeUserMonthInfo* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_ChangeUserMonthInfo);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->MonthID);
			if (!pMonth)
			{
				ASSERT(false);
				return false;
			}
			if (pMsg->IsEnd)
				pMonth->GetLastMonthInfo().UpdateRoleInfo(pMsg->dwUserID, pMsg->dwPoint, pMsg->dwMonthGlobel,pMsg->SkillSum);
			else
				pMonth->OnChangeRoleInfo(pMsg->dwUserID, pMsg->dwPoint, pMsg->dwMonthGlobel,pMsg->SkillSum);
			return true;
		}
		case GC_GetUserMonthInfoFinish:
		{
			GC_Cmd_GetUserMonthInfoFinish* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_GetUserMonthInfoFinish);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->MonthID);
			if (!pMonth)
			{
				ASSERT(false);
				return false;
			}
			pMonth->OnLoadAllGameUserMonthInfo();
			return true;
		}
		case GC_ChangeUserAddMonthGlobelNum:
		{
			GC_Cmd_ChangeUserAddMonthGlobelNum* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_ChangeUserAddMonthGlobelNum);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			FishMonth* pMonth = GetMonthManager().QueryMonth(pMsg->Month);
			if (!pMonth)
			{
				ASSERT(false);
				return false;
			}
			pMonth->OnAddMonthGlobelNum(pMsg->dwUserID);
			return true;
		}
		case GC_LoadRoleSignUp:
		{
			GC_Cmd_LoadRoleSignUp* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_LoadRoleSignUp);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			GetMonthManager().SendRoleSignUpInfoToGameServer(pMsg->dwUserID);
			return true;
		}
		case GC_ResetMonthInfo:
		{
			GC_Cmd_ResetMonthInfo * pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), GC_Cmd_ResetMonthInfo);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			GetMonthManager().ResetUserMonthInfo(pMsg->dwUserID, pMsg->MonthID);
			return true;
		}
		}
	}
	else if (pCmd->GetMainType() == Main_RelationRequest)
	{
		switch (pCmd->GetSubType())
		{
		case GC_SendRelationRequest:
			{
				GC_Cmd_SendRelationRequest* pMsg = (GC_Cmd_SendRelationRequest*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->Info.DestUserID);
				if (!pRole)
					return false;
				CG_Cmd_SendRelationRequest msg;
				SetMsgInfo(msg, GetMsgType(Main_RelationRequest, CG_SendRelationRequest), sizeof(CG_Cmd_SendRelationRequest));
				msg.Info = pMsg->Info;
				pRole->SendDataToGameServer(&msg);
				return true;
			}
			break;
		case GC_HandleRelationRequest:
			{
				GC_Cmd_HandleRelationRequest* pMsg = (GC_Cmd_HandleRelationRequest*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->Info.SrcUserID);
				if (!pRole)
				{
					//Ŀ����Ҳ����ߵ�ʱ�� ����Ӧ���������Ӻ�������
					DBR_Cmd_AddUserRelation msg;
					SetMsgInfo(msg, DBR_AddUserRelation, sizeof(DBR_Cmd_AddUserRelation));
					msg.dwSrcUserID = pMsg->Info.SrcUserID;
					msg.dwDestUserID = pMsg->Info.DestUserID;
					msg.bRelationType = pMsg->Info.RelationType;
					g_FishServer.SendNetCmdToDB(&msg);
					return true;
				}
				else
				{
					CG_Cmd_HandleRelationRequest msg;
					SetMsgInfo(msg, GetMsgType(Main_RelationRequest, CG_HandleRelationRequest), sizeof(CG_Cmd_HandleRelationRequest));
					msg.Info = pMsg->Info;
					msg.Result = pMsg->Result;
					pRole->SendDataToGameServer(&msg);
					return true;
				}
			}
			break;
		case GC_DestDelRelation:
			{
				GC_Cmd_DestDelRelation* pMsg = (GC_Cmd_DestDelRelation*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->Info.DestUserID);
				if (!pRole)
				{
					DBR_Cmd_DelUserRelation msg;
					SetMsgInfo(msg, DBR_DelUserRelation, sizeof(DBR_Cmd_DelUserRelation));
					msg.dwSrcUserID = pMsg->Info.DestUserID;
					msg.dwDestUserID = pMsg->Info.SrcUserID;
					g_FishServer.SendNetCmdToDB(&msg);
				}
				else
				{
					CG_Cmd_DestDelRelation msg;
					SetMsgInfo(msg, GetMsgType(Main_RelationRequest, CG_DestDelRelation), sizeof(CG_Cmd_DestDelRelation));
					msg.Info = pMsg->Info;
					pRole->SendDataToGameServer(&msg);
				}
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_Charm)
	{
		switch (pCmd->GetSubType())
		{
		case CC_ChangeOtherUserCharm:
		{
			CC_Cmd_ChangeOtherUserCharm* pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), CC_Cmd_ChangeOtherUserCharm);
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pDestRole = m_RoleManager.QueryCenterUser(pMsg->dwDestUserID);
			if (!pDestRole)
			{
				//ֱ��ִ�д洢���� 
				HashMap<BYTE, tagCharmOnce>::iterator Iter = GetFishConfig().GetCharmConfig().CharmIndexMap.find(pMsg->CharmIndex);
				if (Iter == GetFishConfig().GetCharmConfig().CharmIndexMap.end())
				{
					ASSERT(false);
					return false;
				}
				DBR_Cmd_SaveRoleCharm msgDB;//��Ҳ����ߵ������ ֱ�ӱ��浽���ݿ�ȥ
				SetMsgInfo(msgDB, DBR_SaveRoleCharm, sizeof(DBR_Cmd_SaveRoleCharm));
				msgDB.dwUserID = pMsg->dwUserID;
				msgDB.dwDestUserID = pMsg->dwDestUserID;
				msgDB.bIndex = pMsg->CharmIndex;
				msgDB.ItemSum = pMsg->ItemSum;
				msgDB.AddCharmValue = Iter->second.ChangeCharmValue * pMsg->ItemSum;
				SendNetCmdToDB(&msgDB);

				//������� ������Ʒʹ�óɹ�
				/*CC_Cmd_ChangeRoleCharmResult msg;
				SetMsgInfo(msg, GetMsgType(Main_Charm, CC_ChangeRoleCharmResult), sizeof(CC_Cmd_ChangeRoleCharmResult));
				msg.dwUserID = pMsg->dwUserID;
				msg.Result = ErrorCode::EC_Sucess;
				msg.CharmIndex = pMsg->CharmIndex;
				msg.ItemSum = pMsg->ItemSum;
				pRole->SendDataToGameServer(&msg);*/

				return true;
			}
			else
			{
				//�������GameServerȥ
				//1.�������GameServer
				CC_Cmd_ChangeRoleCharmResult msg;
				SetMsgInfo(msg,GetMsgType(Main_Charm, CC_ChangeRoleCharmResult), sizeof(CC_Cmd_ChangeRoleCharmResult));
				msg.dwUserID = pMsg->dwUserID;
				msg.Result = true;
				msg.CharmIndex = pMsg->CharmIndex;
				msg.ItemSum = pMsg->ItemSum;
				pRole->SendDataToGameServer(&msg);
				//2.�������GameServerȥ �޸���ҵ�����
				CC_Cmd_ChangeUserCharm msgOther;
				SetMsgInfo(msgOther,GetMsgType(Main_Charm, CC_ChangeUserCharm), sizeof(CC_Cmd_ChangeUserCharm));
				msgOther.dwUserID = pMsg->dwDestUserID;
				msgOther.CharmIndex = pMsg->CharmIndex;
				msgOther.ItemSum = pMsg->ItemSum;
				pDestRole->SendDataToGameServer(&msgOther);
				return true;
			}
		}
		}
	}
	else if (pCmd->GetMainType() == Main_Giff)
	{
		switch (pCmd->GetSubType())
		{
		case GC_AddRoleGiff:
			{
				GC_Cmd_AddRoleGiff* pMsg = (GC_Cmd_AddRoleGiff*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pDestRole = m_RoleManager.QueryCenterUser(pMsg->dwDestUserID);
				if (pDestRole)
				{
					CG_Cmd_AddRoleGiff msg;
					SetMsgInfo(msg, GetMsgType(Main_Giff, CG_AddRoleGiff), sizeof(CG_Cmd_AddRoleGiff));
					msg.dwSrcUserID = pMsg->dwSrcUserID;
					msg.dwDestUserID = pMsg->dwDestUserID;
					pDestRole->SendDataToGameServer(&msg);
				}
				else
				{
					//��Ҳ����� 
					DBR_Cmd_AddRoleGiff msg;
					SetMsgInfo(msg, DBR_AddRoleGiff, sizeof(DBR_Cmd_AddRoleGiff));
					msg.SrcUserID = pMsg->dwSrcUserID;
					msg.DestUserID = pMsg->dwDestUserID;
					SendNetCmdToDB(&msg);
				}
				return true;
			}
		case GC_AddRoleGiffResult:
			{
				GC_Cmd_AddRoleGiffResult* pMsg = (GC_Cmd_AddRoleGiffResult*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pSrcRole = m_RoleManager.QueryCenterUser(pMsg->dwSrcUserID);
				if (!pSrcRole)
				{
					ASSERT(false);
					return false;
				}
				CG_Cmd_AddRoleGiffResult msg;
				SetMsgInfo(msg, GetMsgType(Main_Giff, CG_AddRoleGiffResult), sizeof(CG_Cmd_AddRoleGiffResult));
				msg.dwDestUserID = pMsg->dwDestUserID;
				msg.dwSrcUserID = pMsg->dwSrcUserID;
				msg.Result = pMsg->Result;
				pSrcRole->SendDataToGameServer(&msg);
				return true;
			}
		}
	}
	else if (pCmd->GetMainType() == Main_Message)
	{
		switch (pCmd->GetSubType())
		{
		case GC_SendMessage:
			{
				GC_Cmd_SendMessage* pMsg = (GC_Cmd_SendMessage*)pCmd;
				//����GameServer������������ ���ǽ����ж��� ��δ���
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				SendMessageByType(pMsg->Message,pMsg->MessageSize, pMsg->MessageType, pMsg->MessageColor, pMsg->StepNum, pMsg->StepSec, pMsg->Param);
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_Announcement)
	{
		switch (pCmd->GetSubType())
		{
		case GC_GetAllAnnouncement:
			{
				m_AnnouncementManager.OnSendAllAnnouncementToGameServer(ConvertDWORDToBYTE(pClient->OutsideExtraData));
				return true;
			}
		case GC_SendNewAnnouncementOnce:
			{
				GC_Cmd_SendNewAnnouncementOnce* pMsg = (GC_Cmd_SendNewAnnouncementOnce*)pCmd;
				m_AnnouncementManager.OnAddNewAnnouncementOnce(pMsg->pOnce);
				return true;
			}
		}
	}
	else if (pCmd->GetMainType() == Main_Control)
	{
		switch (pCmd->GetSubType())
		{
			case LC_KickUserResult:
			{
				LC_Cmd_KickUserResult* pMsg = (LC_Cmd_KickUserResult*)pCmd;
				SendNetCmdToControl(pMsg);
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_Operate)
	{
		//��Ӫ��������������
		switch (pCmd->GetSubType())
		{
		case OC_BindEmail:
			{
				OC_Cmd_BindEmail* pMsg = (OC_Cmd_BindEmail*)pCmd;
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					//����������ݿ�ȥ ֱ�ӱ���
					if (pMsg->Result)
					{
						DBR_Cmd_SaveRoleEmail msg;
						SetMsgInfo(msg, DBR_SaveRoleEmail, sizeof(DBR_Cmd_SaveRoleEmail));
						msg.dwUserID = pMsg->dwUserID;
						TCHARCopy(msg.Email, CountArray(msg.Email), pMsg->EMail, _tcslen(pMsg->EMail));
						SendNetCmdToDB(&msg);
					}
				}
				else
				{
					CG_Cmd_BindEmail msg;
					SetMsgInfo(msg, GetMsgType(Main_Operate, CG_BindEmail), sizeof(CG_Cmd_BindEmail));
					msg.dwUserID = pMsg->dwUserID;
					msg.ErrorID = pMsg->ErrorID;
					msg.Result = pMsg->Result;
					TCHARCopy(msg.EMail, CountArray(msg.EMail), pMsg->EMail, _tcslen(pMsg->EMail));
					pRole->SendDataToGameServer(&msg);
				}
				return true;
			}
		case OC_UseRMB:
			{
				//g_FishServer.Log("���յ��������������г�ֵ1");
				OC_Cmd_UseRMB* pMsg = (OC_Cmd_UseRMB*)pCmd;
				CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->rechargeInfo.UserID);
				if (pRole)
				{
					//��Ҵ��� ���ǽ����
					DWORD PageSize = sizeof(CG_Cmd_UseRMB)+sizeof(BYTE)*(pMsg->rechargeInfo.Sum - 1);
					CG_Cmd_UseRMB* msg = (CG_Cmd_UseRMB*)malloc(PageSize);
					msg->SetCmdType(GetMsgType(Main_Operate, CG_UseRMB));
					msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
					memcpy_s(&msg->rechargeInfo, (pMsg->rechargeInfo.Sum - 1)*sizeof(BYTE)+sizeof(RechageInfo), &pMsg->rechargeInfo, (pMsg->rechargeInfo.Sum - 1)*sizeof(BYTE)+sizeof(RechageInfo));
					pRole->SendDataToGameServer(msg);
					free(msg);
				}
				else
				{
					vector<TCHAR*> pVec;
					GetStringArrayVecByData(pVec, &pMsg->rechargeInfo);
					if (pVec.size() != pMsg->rechargeInfo.HandleSum || pVec.size()<4)
					{
						FreeVec(pVec);
						ASSERT(false);
						return false;
					}
					//TCHAR			orderid[32];
					//TCHAR			ChannelCode[32];//����������
					//TCHAR			channelOrderid[32];//�����û���ID
					//TCHAR			channelLabel[32];

					//ֱ��ִ�� ��ҽ��г�ֵ������ 
					HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = m_FishConfig.GetFishRechargesConfig().m_FishRechargeMap.find(pMsg->rechargeInfo.ShopItemID);
					if (Iter == m_FishConfig.GetFishRechargesConfig().m_FishRechargeMap.end() || Iter->second.dDisCountPrice*100*pMsg->rechargeInfo.ShopNum != pMsg->rechargeInfo.Price)
					{
						UINT Count = 0;
						char* OrderID = WCharToChar(pVec[0], Count);
						char* ChannelCode = WCharToChar(pVec[1], Count);
						char* ChannelOrderID = WCharToChar(pVec[2], Count);
						char* ChannelLabel = WCharToChar(pVec[3], Count);

						FreeVec(pVec);
						free(OrderID);
						free(ChannelCode);
						free(ChannelOrderID);
						free(ChannelLabel);			

						ASSERT(false);
						return false;
					}
					else
					{
						//���һ���µ����� ����������߳�ֵ������ DBR_Cmd_AddRecharge
						/*DWORD PageSize = sizeof(DBR_Cmd_AddRecharge)+sizeof(BYTE)*(pMsg->rechargeInfo.Sum - 1);
						DBR_Cmd_AddRecharge* msg = (DBR_Cmd_AddRecharge*)malloc(PageSize);
						msg->SetCmdType(DBR_AddRecharge);
						msg->SetCmdSize(ConvertDWORDToWORD(PageSize));*/
						if (Iter->second.IsAddCurrcey())
						{
							/*msg->AddCurrceySum = Iter->second.AddMoney;
							msg->AddGlobelSum = 0;*/

							tagRoleMail	MailInfo;
							MailInfo.bIsRead = false;
							//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
							TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���"), _tcslen(TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���")));
							MailInfo.RewardID = GetFishConfig().GetSystemConfig().EmailCurrceyRewardID;
							MailInfo.RewardSum = Iter->second.AddMoney * pMsg->rechargeInfo.ShopNum;
							MailInfo.MailID = 0;
							MailInfo.SendTimeLog = time(NULL);
							MailInfo.SrcFaceID = 0;
							TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
							MailInfo.SrcUserID = 0;//ϵͳ����
							MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
							DBR_Cmd_AddUserMail msg;
							SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
							msg.dwDestUserID = pMsg->rechargeInfo.UserID;
							msg.MailInfo = MailInfo;
							g_FishServer.SendNetCmdToDB(&msg);
						}
						else if (Iter->second.IsAddGlobel())
						{
							/*msg->AddCurrceySum = 0;
							msg->AddGlobelSum = Iter->second.AddMoney;*/

							tagRoleMail	MailInfo;
							MailInfo.bIsRead = false;
							//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
							TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���"), _tcslen(TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���")));
							MailInfo.RewardID = GetFishConfig().GetSystemConfig().EmailGlobelRewardID;
							MailInfo.RewardSum = Iter->second.AddMoney* pMsg->rechargeInfo.ShopNum;
							MailInfo.MailID = 0;
							MailInfo.SendTimeLog = time(NULL);
							MailInfo.SrcFaceID = 0;
							TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
							MailInfo.SrcUserID = 0;//ϵͳ����
							MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
							DBR_Cmd_AddUserMail msg;
							SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
							msg.dwDestUserID = pMsg->rechargeInfo.UserID;
							msg.MailInfo = MailInfo;
							g_FishServer.SendNetCmdToDB(&msg);
						}
						else if (Iter->second.IsAddReward())
						{
							//���Ϊ����ID�Ļ� ���ǰ��ʼ�����
							//�����ʼ����� ϵͳ�ʼ�ֱ��Я��RewardID ���д���
							tagRoleMail	MailInfo;
							MailInfo.bIsRead = false;
							//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
							TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���"), _tcslen(TEXT("��ϲ����ֵ�ɹ� ��Ϊ�������� ���г�ֵ����ͨ���ʼ����͸���")));
							MailInfo.RewardID = Iter->second.RewardID;
							MailInfo.RewardSum = pMsg->rechargeInfo.ShopNum;
							MailInfo.MailID = 0;
							MailInfo.SendTimeLog = time(NULL);
							MailInfo.SrcFaceID = 0;
							TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
							MailInfo.SrcUserID = 0;//ϵͳ����
							MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
							DBR_Cmd_AddUserMail msg;
							SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
							msg.dwDestUserID = pMsg->rechargeInfo.UserID;
							msg.MailInfo = MailInfo;
							g_FishServer.SendNetCmdToDB(&msg);
						}
						else
						{
							ASSERT(false);
							FreeVec(pVec);
							return false;
						}
					/*	msg->IsFirst = Iter->second.IsFirstPay;
						memcpy_s(&msg->rechargeInfo, (pMsg->rechargeInfo.Sum - 1)*sizeof(BYTE)+sizeof(RechageInfo), &pMsg->rechargeInfo, (pMsg->rechargeInfo.Sum - 1)*sizeof(BYTE)+sizeof(RechageInfo));
						SendNetCmdToDB(msg);
						free(msg);*/

						UINT Count = 0;
						char* OrderID = WCharToChar(pVec[0], Count);
						char* ChannelCode = WCharToChar(pVec[1], Count);
						char* ChannelOrderID = WCharToChar(pVec[2], Count);
						char* ChannelLabel = WCharToChar(pVec[3], Count);
						//�����ҵĳ�ֵ���� ���
						DBR_Cmd_AddRoleTotalRecharge msg;
						SetMsgInfo(msg, DBR_AddRoleTotalRecharge, sizeof(DBR_Cmd_AddRoleTotalRecharge));
						msg.dwUserID = pMsg->rechargeInfo.UserID;
						msg.Sum = pMsg->rechargeInfo.Price;
						g_FishServer.SendNetCmdToDB(&msg);

						FreeVec(pVec);
						free(OrderID);
						free(ChannelCode);
						free(ChannelOrderID);
						free(ChannelLabel);
					}
				}
				return true;
			}
		case OC_PhonePay:
			{
				OC_Cmd_PhonePay* pMsg = (OC_Cmd_PhonePay*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}

				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					if (pMsg->Result)
					{
						//����ҹ����ֻ���ֵ���ļ�¼�����ݿ�ȥ LOG
						HashMap<BYTE, tagShopConfig>::iterator IterShop = g_FishServer.GetFishConfig().GetShopConfig().ShopMap.find(pMsg->ShopID);
						if (IterShop == g_FishServer.GetFishConfig().GetShopConfig().ShopMap.end())
						{
							ASSERT(false);
							return false;
						}
						HashMap<BYTE, tagShopItemConfig>::iterator IterShopItem = IterShop->second.ShopItemMap.find(pMsg->ShopOnlyID);
						if (IterShopItem == IterShop->second.ShopItemMap.end())
						{
							ASSERT(false);
							return false;
						}
						if (IterShopItem->second.ShopType != SIT_PhonePay)
						{
							ASSERT(false);
							return false;
						}
						tagItemConfig* pItemConfig = GetFishConfig().GetItemInfo(IterShopItem->second.ItemInfo.ItemID);
						if (!pItemConfig)
						{
							ASSERT(false);
							return false;
						}

						DWORD FacePice = pItemConfig->ItemParam * IterShopItem->second.ItemInfo.ItemSum * pMsg->ShopSum;
						return true;
					}
					HashMap<BYTE, tagShopConfig>::iterator IterShop = g_FishServer.GetFishConfig().GetShopConfig().ShopMap.find(pMsg->ShopID);
					if (IterShop == g_FishServer.GetFishConfig().GetShopConfig().ShopMap.end())
					{
						ASSERT(false);
						return false;
					}
					HashMap<BYTE, tagShopItemConfig>::iterator IterShopItem = IterShop->second.ShopItemMap.find(pMsg->ShopOnlyID);
					if (IterShopItem == IterShop->second.ShopItemMap.end())
					{
						ASSERT(false);
						return false;
					}
					if (IterShopItem->second.ShopType != SIT_PhonePay)
					{
						ASSERT(false);
						return false;
					}

					//��Ϊ��ֵ����ʧ�� ������Ҫ���߹黹��ҵĽ��
					//DBR_Cmd_ChangeRoleMoney msg;
					//SetMsgInfo(msg, DBR_ChangeRoleMoney, sizeof(DBR_Cmd_ChangeRoleMoney));
					//msg.dwUserID = pMsg->dwUserID;
					////��ȡ��Ҫ�黹��Ǯ�ҵ�����
					//msg.GlobelSum = IterShopItem->second.PriceGlobel * pMsg->ShopSum;
					//msg.MedalSum = IterShopItem->second.PriceMabel * pMsg->ShopSum;
					//msg.CurrceySum = IterShopItem->second.PriceCurrey * pMsg->ShopSum;
					//SendNetCmdToDB(&msg);

					if (IterShopItem->second.PriceMabel * pMsg->ShopSum > 0)
					{
						tagRoleMail	MailInfo;
						MailInfo.bIsRead = false;
						//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
						TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ�����ͨ���ʼ��˻�����"), _tcslen(TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ�����ͨ���ʼ��˻�����")));
						MailInfo.RewardID = GetFishConfig().GetSystemConfig().EmailMedalRewradID;
						MailInfo.RewardSum = IterShopItem->second.PriceMabel * pMsg->ShopSum;
						MailInfo.MailID = 0;
						MailInfo.SendTimeLog = time(NULL);
						MailInfo.SrcFaceID = 0;
						TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
						MailInfo.SrcUserID = 0;//ϵͳ����
						MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
						DBR_Cmd_AddUserMail msg;
						SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
						msg.dwDestUserID = pMsg->dwUserID;
						msg.MailInfo = MailInfo;
						g_FishServer.SendNetCmdToDB(&msg);
					}
					if (IterShopItem->second.PriceGlobel * pMsg->ShopSum > 0)
					{
						tagRoleMail	MailInfo;
						MailInfo.bIsRead = false;
						//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
						TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ����ͨ���ʼ��˻�����"), _tcslen(TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ�����ͨ���ʼ��˻�����")));
						MailInfo.RewardID = GetFishConfig().GetSystemConfig().EmailGlobelRewardID;
						MailInfo.RewardSum = IterShopItem->second.PriceGlobel * pMsg->ShopSum;
						MailInfo.MailID = 0;
						MailInfo.SendTimeLog = time(NULL);
						MailInfo.SrcFaceID = 0;
						TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
						MailInfo.SrcUserID = 0;//ϵͳ����
						MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
						DBR_Cmd_AddUserMail msg;
						SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
						msg.dwDestUserID = pMsg->dwUserID;
						msg.MailInfo = MailInfo;
						g_FishServer.SendNetCmdToDB(&msg);
					}
					if (IterShopItem->second.PriceCurrey * pMsg->ShopSum > 0)
					{
						tagRoleMail	MailInfo;
						MailInfo.bIsRead = false;
						//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
						TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ���ʯͨ���ʼ��˻�����"), _tcslen(TEXT("�ֻ����ѳ�ֵʧ�� ���ǽ�����ͨ���ʼ��˻�����")));
						MailInfo.RewardID = GetFishConfig().GetSystemConfig().EmailCurrceyRewardID;
						MailInfo.RewardSum = IterShopItem->second.PriceCurrey * pMsg->ShopSum;
						MailInfo.MailID = 0;
						MailInfo.SendTimeLog = time(NULL);
						MailInfo.SrcFaceID = 0;
						TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
						MailInfo.SrcUserID = 0;//ϵͳ����
						MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
						DBR_Cmd_AddUserMail msg;
						SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
						msg.dwDestUserID = pMsg->dwUserID;
						msg.MailInfo = MailInfo;
						g_FishServer.SendNetCmdToDB(&msg);
					}

					//����ҽ���һ���������һ��
					DBR_Cmd_DelRoleCashSum msg;
					SetMsgInfo(msg, DBR_DelRoleCashSum, sizeof(DBR_Cmd_DelRoleCashSum));
					msg.UserID = pMsg->dwUserID;
					msg.DelCashSum = 1;
					SendNetCmdToDB(&msg);

					return true;
				}
				else
				{
					//��Ҵ��� ת����GameServerȥ
					CG_Cmd_PhonePay msg;
					SetMsgInfo(msg, GetMsgType(Main_Operate, CG_PhonePay), sizeof(CG_Cmd_PhonePay));
					msg.dwUserID = pMsg->dwUserID;
					msg.OrderID = pMsg->OrderID;
					msg.Phone = pMsg->Phone;
					msg.ErrorID = pMsg->ErrorID;
					msg.Result = pMsg->Result;
					msg.ShopID = pMsg->ShopID;
					msg.ShopOnlyID = pMsg->ShopOnlyID;
					msg.ShopSum = pMsg->ShopSum;
					pRole->SendDataToGameServer(&msg);
					return true;
				}
				return false;
			}
		case OC_AddNormalOrderID:
			{
				OC_Cmd_AddNormalOrderID* pMsg = (OC_Cmd_AddNormalOrderID*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return false;
				}
				if (!pMsg->Result)
				{
					DBR_Cmd_DelRechargeOrderInfo msg; //�Ƴ���������
					SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
					msg.OrderID = pMsg->OrderID;
					g_FishServer.SendNetCmdToDB(&msg);
				}
				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
				if (!pRole)
				{
					ASSERT(false);
					return false;
				}
				CG_Cmd_AddNormalOrderID msg;
				SetMsgInfo(msg, GetMsgType(Main_Operate, CG_AddNormalOrderID), sizeof(CG_Cmd_AddNormalOrderID));
				msg.dwUserID = pMsg->dwUserID;
				msg.ShopID = pMsg->ShopID;
				msg.OrderID = pMsg->OrderID;
				msg.Result = pMsg->Result;
				strncpy_s(msg.Transid, CountArray(msg.Transid), pMsg->Transid,strlen(pMsg->Transid));
				strncpy_s(msg.Sign, CountArray(msg.Sign), pMsg->Sign, strlen(pMsg->Sign));
				pRole->SendDataToGameServer(&msg);
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_NiuNiu)
	{
		switch (pCmd->GetSubType())
		{
		case GC_RequestJoinNiuNiuRoom:
			{
				GC_Cmd_RequestJoinNiuNiuRoom* pMsg = (GC_Cmd_RequestJoinNiuNiuRoom*)pCmd;
				if (nullptr == pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pSrcRole = GetRoleManager().QueryCenterUser(pMsg->dwSrcUserID);
				if (nullptr == pSrcRole)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwDestUserID);
				if (nullptr == pRole)
				{
					CG_Cmd_RequestJoinNiuNiuRoomResult msg;
					SetMsgInfo(msg, GetMsgType(Main_NiuNiu, CG_RequestJoinNiuNiuRoomResult), sizeof(CG_Cmd_RequestJoinNiuNiuRoomResult));
					msg.dwRequestUserID = pMsg->dwSrcUserID;
					msg.dwDestUserID = pMsg->dwDestUserID;
					msg.Result = false;
					pSrcRole->SendDataToGameServer(&msg);
				}
				else
				{
					CG_Cmd_RequestJoinNiuNiuRoom msg;
					SetMsgInfo(msg, GetMsgType(Main_NiuNiu, CG_RequestJoinNiuNiuRoom), sizeof(CG_Cmd_RequestJoinNiuNiuRoom));
					msg.dwSrcUserID = pMsg->dwSrcUserID;
					msg.dwDestUserID = pMsg->dwDestUserID;
					TCHARCopy(msg.strSrcNickName, CountArray(msg.strSrcNickName), pSrcRole->GetRoleInfo().NickName, _tcslen(pSrcRole->GetRoleInfo().NickName));
					pRole->SendDataToGameServer(&msg);
				}
				return true;
			}
			break;
		case GC_RequestJoinNiuNiuRoomResult:
			{
				GC_Cmd_RequestJoinNiuNiuRoomResult* pMsg = (GC_Cmd_RequestJoinNiuNiuRoomResult*)pCmd;
				if (nullptr == pMsg)
				{
					ASSERT(false);
					return false;
				}
				CenterRole* pSrcRole = GetRoleManager().QueryCenterUser(pMsg->dwRequestUserID);
				if (nullptr == pSrcRole)
				{
					ASSERT(false);
					return false;
				}
				CG_Cmd_RequestJoinNiuNiuRoomResult msg;
				SetMsgInfo(msg, GetMsgType(Main_NiuNiu, CG_RequestJoinNiuNiuRoomResult), sizeof(CG_Cmd_RequestJoinNiuNiuRoomResult));
				msg.dwRequestUserID = pMsg->dwRequestUserID;
				msg.dwDestUserID = pMsg->dwDestUserID;
				msg.Result = pMsg->Result;
				pSrcRole->SendDataToGameServer(&msg);
				return true;
			}
			break;
		}
	}
	else if (pCmd->GetMainType() == Main_Server)
	{
		switch (pCmd->GetSubType())
		{
		case SS_LogonToGame:
			{
				//��������װ������ȡ����
				SS_Cmd_LogonToGame* pMsg = (SS_Cmd_LogonToGame*)pCmd;
				NetCmd* pNewCmd = (NetCmd*)pMsg->Array;
				if (pMsg->GameID == 0)
				{
					m_CenterServerManager.SendNetCmdToAllGameServer(pNewCmd);
					return true;
				}
				else
				{
					m_CenterServerManager.SendNetCmdToGameServer(pMsg->GameID,pNewCmd);
					return true;
				}
			}
			break;
		case SS_GameToLogon:
			{
				SS_Cmd_GameToLogon* pMsg = (SS_Cmd_GameToLogon*)pCmd;
				NetCmd* pNewCmd = (NetCmd*)pMsg->Array;
				if (pMsg->LogonID == 0)
				{
					m_CenterServerManager.SendNetCmdToAllLogonServer(pNewCmd);
					return true;
				}
				else
				{
					m_CenterServerManager.SendNetCmdToLogonServer(pMsg->LogonID, pNewCmd);
					return true;
				}
			}
			break;
		case SS_GameToGame:
			{
				SS_Cmd_GameToGame* pMsg = (SS_Cmd_GameToGame*)pCmd;
				NetCmd* pNewCmd = (NetCmd*)pMsg->Array;
				if (pMsg->GameID == 0)
				{
					m_CenterServerManager.SendNetCmdToAllGameServer(pNewCmd);
					return true;
				}
				else
				{
					m_CenterServerManager.SendNetCmdToGameServer(pMsg->GameID, pNewCmd);
					return true;
				}
			}
			break;
		case SS_LogonToLogon:
			{
				SS_Cmd_LogonToLogon* pMsg = (SS_Cmd_LogonToLogon*)pCmd;
				NetCmd* pNewCmd = (NetCmd*)pMsg->Array;
				if (pMsg->LogonID == 0)
				{
					m_CenterServerManager.SendNetCmdToAllLogonServer(pNewCmd);
					return true;
				}
				else
				{
					m_CenterServerManager.SendNetCmdToLogonServer(pMsg->LogonID, pNewCmd);
					return true;
				}
			}
			break;
		}
	}
	return false;
}
void FishServer::SendMessageByType(TCHAR* pMessage, WORD MessageSize,BYTE MessageType, DWORD MessageColor, BYTE StepNum, BYTE StepSec, DWORD Param)
{
	if (MessageType == MT_Center || MessageType == MT_CenterMessage)
	{
		DWORD PageSize = sizeof(CG_Cmd_SendMessage)+sizeof(TCHAR)*MessageSize;
		CG_Cmd_SendMessage* msg = (CG_Cmd_SendMessage*)malloc(PageSize);
		if (!msg)
		{
			ASSERT(false);
			return;
		}
		//Center -> Game 0

		CheckMsgSize(PageSize);
		msg->SetCmdType(GetMsgType(Main_Message, CG_SendMessage));
		msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
		
		if (MessageType == MT_Center)
			msg->MessageType = MT_Game;
		else if (MessageType == MT_CenterMessage)
			msg->MessageType = MT_GameMessage;

		msg->MessageColor = MessageColor;
		msg->StepNum = StepNum;
		msg->StepSec = StepSec;
		msg->Param = Param;
		msg->MessageSize = MessageSize;
		memcpy_s(msg->Message, msg->MessageSize * sizeof(TCHAR), pMessage, MessageSize* sizeof(TCHAR));
		//TCHARCopy(msg->Message, msg->MessageSize, pMessage, _tcslen(pMessage));

		m_CenterServerManager.SendNetCmdToAllGameServer(msg);
		free(msg);
	}
	else if (MessageType == MT_Game || MessageType == MT_GameMessage)
	{
		DWORD PageSize = sizeof(CG_Cmd_SendMessage)+sizeof(TCHAR)*MessageSize;
		CG_Cmd_SendMessage* msg = (CG_Cmd_SendMessage*)malloc(PageSize);
		if (!msg)
		{
			ASSERT(false);
			return;
		}
		CheckMsgSize(PageSize);
		msg->SetCmdType(GetMsgType(Main_Message, CG_SendMessage));
		msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
		msg->MessageType = MessageType;
		msg->MessageColor = MessageColor;
		msg->StepNum = StepNum;
		msg->StepSec = StepSec;
		msg->Param = Param;
		/*msg->MessageSize = _tcslen(pMessage) + 1;
		TCHARCopy(msg->Message, msg->MessageSize, pMessage, _tcslen(pMessage));*/
		msg->MessageSize = MessageSize;
		memcpy_s(msg->Message, msg->MessageSize * sizeof(TCHAR), pMessage, MessageSize* sizeof(TCHAR));
		m_CenterServerManager.SendNetCmdToGameServer(ConvertDWORDToBYTE(msg->Param), msg);
		free(msg);
	}
	else if (MessageType == MT_User || MessageType == MT_UserMessage)
	{
		CenterRole * pRole = GetRoleManager().QueryCenterUser(Param);
		if (!pRole)
			return;
		DWORD PageSize = sizeof(CG_Cmd_SendMessage)+sizeof(TCHAR)*MessageSize;
		CG_Cmd_SendMessage* msg = (CG_Cmd_SendMessage*)malloc(PageSize);
		if (!msg)
		{
			ASSERT(false);
			return;
		}
		CheckMsgSize(PageSize);
		msg->SetCmdType(GetMsgType(Main_Message, CG_SendMessage));
		msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
		msg->MessageType = MessageType;
		msg->MessageColor = MessageColor;
		msg->StepNum = StepNum;
		msg->StepSec = StepSec;
		msg->Param = Param;
		//msg->MessageSize = _tcslen(pMessage) + 1;
		//TCHARCopy(msg->Message, msg->MessageSize, pMessage, _tcslen(pMessage));
		msg->MessageSize = MessageSize;
		memcpy_s(msg->Message, msg->MessageSize * sizeof(TCHAR), pMessage, MessageSize* sizeof(TCHAR));

		pRole->SendDataToGameServer(msg);
		free(msg);
	}
	else
	{
		ASSERT(false);
		return;
	}
}
bool FishServer::HandleDataBaseMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	switch (pCmd->GetCmdType())
	{
	case DBO_AddUserMail:
		HandleAddUserMailResult(pCmd);
		return true;
	case DBO_SaveRoleCharm:
		HandleSaveRoleCharmResult(pCmd);
		return true;
	case DBO_LoadAllUserAchievementPointList:
		HandleSendAchievementPointList(pCmd);
		return true;
	case DBO_LoadALlAnnouncement:
		{
			DBO_Cmd_LoadALlAnnouncement* pMsg = (DBO_Cmd_LoadALlAnnouncement*)pCmd;
			m_AnnouncementManager.OnLoadAllAnnouncementByDBResult(pMsg);
			return true;
		}
	/*case DBO_LoadAllAnnouncementFinish:
		{
			DBO_Cmd_LoadAllAnnouncementFinish* pMsg = (DBO_Cmd_LoadAllAnnouncementFinish*)pCmd;
			m_AnnouncementManager.OnLoadAllAnnouncementFinish();
			return true;
		}*/
	case DBO_AddRoleGiff:
		{
			DBO_Cmd_AddRoleGiff* pMsg = (DBO_Cmd_AddRoleGiff*)pCmd;
			CenterRole* pSrcRole = m_RoleManager.QueryCenterUser(pMsg->GiffInfo.dwUserID);
			if (!pSrcRole)
			{
				ASSERT(false);
				return false;
			}
			CG_Cmd_AddRoleGiffResult msg;
			SetMsgInfo(msg, GetMsgType(Main_Giff, CG_AddRoleGiffResult), sizeof(CG_Cmd_AddRoleGiffResult));
			msg.dwSrcUserID = pMsg->GiffInfo.dwUserID;
			msg.dwDestUserID = pMsg->dwDestUserID;
			msg.Result = pMsg->Result;
			pSrcRole->SendDataToGameServer(&msg);
			return true;
		}
		break;
	case DBO_ClearFishDB:
		{
			//OnCenterFinish();
			CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
			if (!pCenterConfig)
			{
				ASSERT(false);
				return false;
			}
			m_RobotManager.OnLoadRobotInfoBySum(pCenterConfig->BeginUserID, pCenterConfig->EndUserID, SendDB);//����1000��������
			return true;
		}
		break;
	case DBO_LoadRobotInfo:
		{
			DBO_Cmd_LoadRobotInfo* pMsg = (DBO_Cmd_LoadRobotInfo*)pCmd;
			m_RobotManager.OnLoadRobotInfoResult(pMsg);
			if (pMsg->States & MsgEnd)
			{
				OnCenterFinish();
				m_MonthManager.OnLoadRobotFinish();
			}
			return true;
		}
		break;
	case DBO_AddUserRelation:
		{
			//�����������ӹ�ϵ�Ľ��
			DBO_Cmd_AddUserRelation* pMsg = (DBO_Cmd_AddUserRelation*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			if (!pMsg->Result)
			{
				//��ӹ�ϵʧ����
				//1.����Dest ɾ�����ѹ�ϵ
				CenterRole* pCenterRole = GetRoleManager().QueryCenterUser(pMsg->RelationInfo.dwDestRoleID);
				if (pCenterRole)
				{
					CG_Cmd_DestDelRelation msg;
					SetMsgInfo(msg, GetMsgType(Main_RelationRequest, CG_DestDelRelation), sizeof(CG_Cmd_DestDelRelation));
					msg.Info.SrcUserID = pMsg->dwUserID;
					msg.Info.DestUserID = pMsg->RelationInfo.dwDestRoleID;
					pCenterRole->SendDataToGameServer(&msg);
				}
				else
				{
					DBR_Cmd_DelUserRelation msg;
					SetMsgInfo(msg, DBR_DelUserRelation, sizeof(DBR_Cmd_DelUserRelation));
					msg.dwSrcUserID = pMsg->RelationInfo.dwDestRoleID ;
					msg.dwDestUserID = pMsg->dwUserID;
					g_FishServer.SendNetCmdToDB(&msg);
				}
			}
			return true;
		}
		break;
	case DBO_RoleEntityItem:
		{
			DBO_Cmd_RoleEntityItem* pMsg = (DBO_Cmd_RoleEntityItem*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			if (pMsg->Result)
			{
				//��ȡ�����ɹ��� ���Ǵ�����
				UINT Count = 0;
				char* Address = WCharToChar(pMsg->Address, Count);
				char* Name = WCharToChar(pMsg->Name, Count);
				char* IDEntity = WCharToChar(pMsg->IDEntity, Count);
				//1.���浽Logȥ
				free(Address);
				free(Name);
				free(IDEntity);
				//2.ɾ������
				DBR_Cmd_DelRoleEntityItem msg;
				SetMsgInfo(msg, DBR_DelRoleEntityItem, sizeof(DBR_Cmd_DelRoleEntityItem));
				msg.ID = pMsg->ID;
				SendNetCmdToDB(&msg);
				//3.�����ʼ������ ��ʾ��� ʵ����Ʒ�Ѿ�������
				tagRoleMail	MailInfo;
				MailInfo.bIsRead = false;
				TCHAR* OrderNumber = CharToWChar(pMsg->OrderNumber, Count);
				//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� 
				_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("�������{ItemName:ItemID=%u} ����:%u �Ѿ����� ��ݵ���Ϊ:%s"), pMsg->ItemID, pMsg->ItemSum, OrderNumber);
				free(OrderNumber);
				MailInfo.RewardID =0;
				MailInfo.RewardSum = 0;
				MailInfo.MailID = 0;
				MailInfo.SendTimeLog = time(NULL);
				MailInfo.SrcFaceID = 0;
				TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
				MailInfo.SrcUserID = 0;//ϵͳ����
				MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
				DBR_Cmd_AddUserMail msgEmail;
				SetMsgInfo(msgEmail, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
				msgEmail.dwDestUserID = pMsg->UserID;
				msgEmail.MailInfo = MailInfo;
				g_FishServer.SendNetCmdToDB(&msgEmail);

				LC_Cmd_HandleEntityItem msgResult;
				SetMsgInfo(msgResult, GetMsgType(Main_Control, LC_HandleEntityItem), sizeof(LC_Cmd_HandleEntityItem));
				msgResult.ClientID = pMsg->ClientID;
				msgResult.Result = true;
				SendNetCmdToControl(&msgResult);
				return true;
			}
			else
			{
				LC_Cmd_HandleEntityItem msg;
				SetMsgInfo(msg, GetMsgType(Main_Control, LC_HandleEntityItem), sizeof(LC_Cmd_HandleEntityItem));
				msg.ClientID = pMsg->ClientID;
				msg.Result = false;
				SendNetCmdToControl(&msg);
				return true;
			}
		}
	case DBO_LoadCash:
		{
			DBO_Cmd_LoadCash* pMsg = (DBO_Cmd_LoadCash*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			m_CashManager.HandleCashOrderInfo(pMsg);
			return true;
		}
	case DBO_HandleCust:
		{
			DBO_Cmd_HandleCust* pMsg = (DBO_Cmd_HandleCust*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			//��ָ����ҷ���һ���ʼ�
			tagRoleMail	MailInfo;
			MailInfo.bIsRead = false;
			TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context),pMsg->strContext,_tcslen(pMsg->strContext));
			MailInfo.RewardID = 0;
			MailInfo.RewardSum =0;
			MailInfo.MailID = 0;
			MailInfo.SendTimeLog = time(NULL);
			MailInfo.SrcFaceID = 0;
			TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
			MailInfo.SrcUserID = 0;//ϵͳ����
			MailInfo.bIsExistsReward = false;
			DBR_Cmd_AddUserMail msg;
			SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
			msg.dwDestUserID = pMsg->dwUserID;
			msg.MailInfo = MailInfo;
			g_FishServer.SendNetCmdToDB(&msg);
			return true;
		}
	}
	return true;
}
bool FishServer::HandleAddUserMailResult(NetCmd* pCmd)
{
	DBO_Cmd_AddUserMail* pMsg = (DBO_Cmd_AddUserMail*)pCmd;
	if (!pMsg)
	{
		ASSERT(false);
		return true;
	}
	if (pMsg->Result && pMsg->MailInfo.SrcUserID == 0)
	{
		CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwDestUserID);
		if (pRole)
		{
			CC_Cmd_SendBeSystemMail msg;
			SetMsgInfo(msg, GetMsgType(Main_Mail, CC_SendBeSystemMail), sizeof(CC_Cmd_SendBeSystemMail));
			msg.dwDestUserID = pMsg->dwDestUserID;
			msg.MailInfo = pMsg->MailInfo;
			pRole->SendDataToGameServer(&msg);
			return true;
		}
	}
	CenterRole* pSrcRole = m_RoleManager.QueryCenterUser(pMsg->MailInfo.SrcUserID);
	if (pSrcRole)
	{
		CG_Cmd_SendUserMailResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Mail, CG_SendUserMailResult), sizeof(CG_Cmd_SendUserMailResult));
		msg.DestUserID = pMsg->dwDestUserID;
		msg.SrcUserID = pMsg->MailInfo.SrcUserID;
		msg.Result = pMsg->Result;
		pSrcRole->SendDataToGameServer(&msg);
	}
	return true;
}
bool FishServer::HandleSaveRoleCharmResult(NetCmd* pCmd)
{
	DBO_Cmd_SaveRoleCharm * pMsg = CHECKMSG(pCmd, pCmd->GetCmdSize(), DBO_Cmd_SaveRoleCharm);
	if (!pMsg)
	{
		ASSERT(false);
		return false;
	}
	if (pMsg->Result)
	{
		CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
		if (!pRole)
		{
			ASSERT(false);
			return false;
		}
		CC_Cmd_ChangeRoleCharmResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Charm, CC_ChangeRoleCharmResult), sizeof(CC_Cmd_ChangeRoleCharmResult));
		msg.dwUserID = pMsg->dwUserID;
		msg.CharmIndex = pMsg->bIndex;
		msg.ItemSum = pMsg->ItemSum;
		msg.Result = true;
		msg.dwDestUserCharmValue = pMsg->dwDestUserCharmValue;
		msg.dwDestUserID = pMsg->dwDestUserID;
		pRole->SendDataToGameServer(&msg);
		return true;
	}
	else
	{
		//������������������ʧ����
		CenterRole* pRole = m_RoleManager.QueryCenterUser(pMsg->dwUserID);
		if (!pRole)
		{
			ASSERT(false);
			return false;
		}
		CC_Cmd_ChangeRoleCharmResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Charm, CC_ChangeRoleCharmResult), sizeof(CC_Cmd_ChangeRoleCharmResult));
		msg.dwUserID = pMsg->dwUserID;
		msg.CharmIndex = pMsg->bIndex;
		msg.ItemSum = pMsg->ItemSum;
		msg.Result = false;
		msg.dwDestUserCharmValue = pMsg->dwDestUserCharmValue;
		msg.dwDestUserID = pMsg->dwDestUserID;
		pRole->SendDataToGameServer(&msg);
		return true;
	}
}
bool FishServer::HandleFtpMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	return true;
}
bool FishServer::HandleControlMsg(NetCmd* pCmd)
{
	//������Ӫ������������������
	if (!pCmd)
		return false;
	if (pCmd->GetMainType() != Main_Control)
	{
		ASSERT(false);
		return false;
	}
	switch (pCmd->GetSubType())
	{
	case CL_SendMsgToAllGame:
		{
			CL_Cmd_SendMsgToAllGame* pMsg = (CL_Cmd_SendMsgToAllGame*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			SendMessageByType(pMsg->CenterMessage,pMsg->MessageSize, MT_Center, pMsg->MessageColor, pMsg->StepNum, pMsg->StepSec, pMsg->Param);
			return true;
		}
		break;
	case CL_SendSystemEmail://�ⲿ����ϵͳ�ʼ�
		{
			CL_Cmd_SendSystemEmail* pMsg = (CL_Cmd_SendSystemEmail*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			//�����ʼ�
			tagRoleMail	MailInfo;
			MailInfo.bIsRead = false;
			//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� .
			TCHARCopy(MailInfo.Context, CountArray(MailInfo.Context), pMsg->EmailContext, pMsg->ContextSize);
			MailInfo.RewardID = static_cast<WORD>(pMsg->RewardID);
			MailInfo.RewardSum = pMsg->RewardSum;
			MailInfo.MailID = 0;
			MailInfo.SendTimeLog = time(NULL);
			MailInfo.SrcFaceID = 0;
			TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
			MailInfo.SrcUserID = 0;//ϵͳ����
			MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
			DBR_Cmd_AddUserMail msg;
			SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
			msg.dwDestUserID = pMsg->dwUserID;
			msg.MailInfo = MailInfo;
			g_FishServer.SendNetCmdToDB(&msg);
			return true;
		}
		break;
	case CL_KickUserByID:
		{
			CL_Cmd_KickUserByID* pMsg = (CL_Cmd_KickUserByID*)pCmd;//�޳������������
			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
			{
				GetCenterManager().SendNetCmdToAllLogonServer(pMsg);//����Logon �޸���ҵĶ���״̬

				DBR_Cmd_SetAccountFreeze msgDB;
				SetMsgInfo(msgDB, DBR_SetAccountFreeze, sizeof(DBR_Cmd_SetAccountFreeze));
				msgDB.dwUserID = pMsg->dwUserID;
				msgDB.FreezeMin = pMsg->FreezeMin;
				SendNetCmdToDB(&msgDB);

				//��������ظ�������
				LC_Cmd_KickUserResult msg;
				SetMsgInfo(msg, GetMsgType(Main_Control, LC_KickUserResult), sizeof(LC_Cmd_KickUserResult));
				msg.dwUserID = pMsg->dwUserID;
				msg.Result = 1;//��Ҳ����� �����˺ųɹ�
				msg.ClientID = pMsg->ClientID;
				SendNetCmdToControl(&msg);
				return true;
			}
			else
			{
				GetCenterManager().SendNetCmdToAllLogonServer(pMsg);
				pRole->SendDataToGameServer(pMsg);
				return true;
			}
		}
		break;
	case CL_QueryOnlineUserInfo:
		{
			CL_Cmd_QueryOnlineUserInfo* pMsg = (CL_Cmd_QueryOnlineUserInfo*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
			if (pRole)
			{
				pRole->SendDataToGameServer(pMsg);
				return true;
			}
			else
			{
				LC_Cmd_QueryOnlineUserInfo msg;
				SetMsgInfo(msg, GetMsgType(Main_Control, LC_QueryOnlineUserInfo), sizeof(LC_Cmd_QueryOnlineUserInfo));
				msg.ClientID = pMsg->ClientID;
				msg.Result = false;
				SendNetCmdToControl(&msg);
				return true;
			}
		}
		break;
	case CL_ChangeaNickName:
		{
			CL_Cmd_ChangeaNickName* pMsg = (CL_Cmd_ChangeaNickName*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
			if (pRole)
			{
				pRole->SendDataToGameServer(pMsg);
				return true;
			}
			else
			{
				DBR_Cmd_SaveRoleNickName msgDB;
				SetMsgInfo(msgDB, DBR_SaveRoleNickName, sizeof(DBR_Cmd_SaveRoleNickName));
				msgDB.dwUserID = pMsg->dwUserID;
				TCHARCopy(msgDB.NickName, CountArray(msgDB.NickName), pMsg->NickName, _tcslen(pMsg->NickName));
				g_FishServer.SendNetCmdToDB(&msgDB);
			}
		}
		break;
	case CL_ReloadConfig:
		{
			OnReloadConfig();
			return true;
		}
		break;
	case CL_HandleEntityItem:
		{
			//�ⲿ��Ҫ����һ����ҹ�����Ʒ�Ķ��� ������Ҫ���д�����
			//����DBR����ͻ���ȥ Я��ClientID
			CL_Cmd_HandleEntityItem* pMsg = (CL_Cmd_HandleEntityItem*)pCmd;
			DBR_Cmd_RoleEntityItem msg;
			SetMsgInfo(msg, DBR_RoleEntityItem, sizeof(DBR_Cmd_RoleEntityItem));
			msg.ClientID = pMsg->ClientID;
			msg.ClientIP = pMsg->ClientIP;
			msg.ID = pMsg->ID;
			strncpy_s(msg.OrderNumber, CountArray(msg.OrderNumber), pMsg->OrderNumber, strlen(pMsg->OrderNumber));
			SendNetCmdToDB(&msg);
			return true;
		}
		break;
	case CL_ChangeParticularStates:
		{
			CL_Cmd_ChangeParticularStates*pMsg = (CL_Cmd_ChangeParticularStates*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
			if (pRole)
			{
				pRole->SendDataToGameServer(pMsg);
				return true;
			}
			else
			{
				DBR_Cmd_SaveRoleParticularStates msgDB;
				SetMsgInfo(msgDB, DBR_SaveRoleParticularStates, sizeof(msgDB));
				msgDB.dwUserID = pMsg->dwUserID;
				msgDB.ParticularStates = pMsg->ParticularStates;
				g_FishServer.SendNetCmdToDB(&msgDB);

				LC_Cmd_ChangeParticularStates msg;
				SetMsgInfo(msg, GetMsgType(Main_Control, LC_ChangeParticularStates), sizeof(LC_Cmd_ChangeParticularStates));
				msg.dwUserID = pMsg->dwUserID;
				msg.ParticularStates = pMsg->ParticularStates;
				msg.ClientID = pMsg->ClientID;
				msg.Result = false;
				SendNetCmdToControl(&msg);
				return true;
			}
			return true;
		}
		break;
	case CL_FreezeAccount:
		{
			//�����˺�
			CL_Cmd_FreezeAccount* pMsg = (CL_Cmd_FreezeAccount*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
			if (!pRole)
				return false;
			pRole->SendDataToGameServer(pMsg);
			return true;
		}
		break;
	case CL_ChangeIsShop:
		{
			CL_Cmd_ChangeIsShop* pMsg = (CL_Cmd_ChangeIsShop*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			DWORD dwUserID = pMsg->dwUserID;
			bool bShop = pMsg->bShop;
			CenterRole* pRole = GetRoleManager().QueryCenterUser(dwUserID);
			if (nullptr == pRole)
			{
				DBR_Cmd_ChangeRoleIsShop msg;
				SetMsgInfo(msg, DBR_ChangeRoleIsShop, sizeof(DBR_Cmd_ChangeRoleIsShop));
				msg.bShop = bShop;
				msg.dwUserID = dwUserID;
				SendNetCmdToDB(&msg);
			}
			else
			{
				pRole->SendDataToGameServer(pMsg);
			}
		}
		break;
	case CL_HandleCustMail:
		{
			CL_Cmd_HandleCustMail* pMsg = (CL_Cmd_HandleCustMail*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			//�������ݿ� ����ָ���ʼ��Ѿ���ȡ��
			DWORD u32MailID = pMsg->u32MailID;//��������ʼ�
			//����������ݿ�ȥ
			DBR_Cmd_HandleCust msg;
			SetMsgInfo(msg, DBR_HandleCust, sizeof(msg));
			msg.dwMailID = u32MailID;
			TCHARCopy(msg.strContext, CountArray(msg.strContext), pMsg->strMailContext, _tcslen(pMsg->strMailContext));
			//����һ���µ��ʼ������ ������Ľ���������
			SendNetCmdToDB(&msg);
		}
		break;
	}
	return true;
}
bool FishServer::HandleSendAchievementPointList(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	DBO_Cmd_LoadAllUserAchievementPointList * pMsg = (DBO_Cmd_LoadAllUserAchievementPointList*)pCmd;
	if ((pMsg->States & MsgBegin) != 0)
	{
		m_AchievementList.clear();
	}
	for (int i = 0; i < pMsg->Sum; ++i)
	{
		HashMap<DWORD, WORD>::iterator Iter = m_AchievementList.find(pMsg->Array[i]);
		if (Iter == m_AchievementList.end())
		{
			m_AchievementList.insert(HashMap<DWORD, WORD>::value_type(pMsg->Array[i], i));
		}
		else
		{
			Iter->second = ConvertIntToWORD(i);
		}
	}
	if ((pMsg->States & MsgEnd) != 0)
	{
		SendUserAchievementToGameServer(0);
	}
	return true;
}
void FishServer::SendUserAchievementToGameServer(BYTE GameID)
{
	DWORD PageSize = sizeof(LC_Cmd_AchievementList)+(m_AchievementList.size() - 1)*sizeof(DWORD);
	LC_Cmd_AchievementList* msg = (LC_Cmd_AchievementList*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdType(GetMsgType(Main_Achievement, LC_AchievementList));
	HashMap<DWORD, WORD>::iterator Iter = m_AchievementList.begin();
	for (WORD i = 0; Iter != m_AchievementList.end(); ++Iter, ++i)
	{
		if (Iter->second >= m_AchievementList.size())
		{
			ASSERT(false);
			continue;
		}
		msg->Array[Iter->second] = Iter->first;
	}
	std::vector<LC_Cmd_AchievementList*> pVec;
	SqlitMsg(msg, PageSize, m_AchievementList.size(),false, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<LC_Cmd_AchievementList*>::iterator Iter = pVec.begin();
		for (; Iter != pVec.end(); ++Iter)
		{
			if (GameID == 0)
			{
				GetCenterManager().SendNetCmdToAllGameServer(*Iter);//������ͳ�ȥ
			}
			else
			{
				g_FishServer.GetCenterManager().SendNetCmdToGameServer(GameID, *Iter);
			}
			free(*Iter);
		}
		pVec.clear();
	}
}
void FishServer::SendGetAchievementList()
{
	DBR_Cmd_LoadAllUserAchievementPointList msg;
	msg.RankSum = GetFishConfig().GetSystemConfig().AchievementRankSum;
	SetMsgInfo(msg, DBR_LoadAllUserAchievementPointList, sizeof(DBR_Cmd_LoadAllUserAchievementPointList));
	SendNetCmdToDB(&msg);
}
//bool FishServer::HandleControlMsg(ServerClientData* pClient, NetCmd* pCmd)
//{
//	//����̨������
//	if (!pCmd || !pClient)
//	{
//		ASSERT(false);
//		return false;
//	}
//	if (pCmd->CmdType != Main_Control)
//	{
//		ASSERT(false);
//		return false;
//	}
//	switch (pCmd->SubCmdType)
//	{
//	case CL_CheckClientInfo:
//		{
//			CL_Cmd_CheckClientInfo* pMsg = (CL_Cmd_CheckClientInfo*)pCmd;
//			if (!pMsg)
//			{
//				ASSERT(false);
//				return false;
//			}
//			LC_Cmd_CheckClientInfo msg;
//			SetMsgInfo(msg, GetMsgType(Main_Control, LC_CheckClientInfo), sizeof(LC_Cmd_CheckClientInfo));
//			msg.Result = (pMsg->RankValue == m_ControlRankValue);
//			SendNetCmdToControl(pClient, &msg);
//			return true;
//		}
//	case CL_GetCenterPlayerSum:
//		{
//			CL_Cmd_GetCenterPlayerSum* pMsg = (CL_Cmd_GetCenterPlayerSum*)pCmd;
//			if (!pMsg)
//			{
//				ASSERT(false);
//				return false;
//			}
//			//���д���
//			LC_Cmd_GetCenterPlayerSum msg;
//			SetMsgInfo(msg, GetMsgType(Main_Control, LC_GetCenterPlayerSum), sizeof(LC_Cmd_GetCenterPlayerSum));
//			msg.PlayerSum = GetRoleManager().GetOnLinePlayerSum();
//			SendNetCmdToControl(pClient, &msg);
//			return true;
//		}
//		break;
//	case CL_SendMsgToAllGame:
//		{
//			CL_Cmd_SendMsgToAllGame* pMsg = (CL_Cmd_SendMsgToAllGame*)pCmd;
//			if (!pMsg)
//			{
//				ASSERT(false);
//				return false;
//			}
//			if (_tcslen(pMsg->CenterMessage) > MAX_MESSAGE_LENGTH)
//			{
//				return true;
//			}
//			//���д���
//			CG_Cmd_SendMessage msg;
//			SetMsgInfo(msg, GetMsgType(Main_Message, CG_SendMessage), sizeof(CG_Cmd_SendMessage));
//			msg.MessageType = MT_Center;
//			msg.MessageColor = pMsg->MessageColor;
//			msg.StepNum = pMsg->StepNum;
//			msg.StepSec = pMsg->StepSec;
//			TCHARCopy(msg.Message, CountArray(msg.Message), pMsg->CenterMessage, _tcslen(pMsg->CenterMessage));
//			m_CenterServerManager.SendNetCmdToAllGameServer(&msg);
//			return true;
//		}
//		break;
//	case CL_ResetNoticeConfig:
//		{
//			m_FishNoticeManager.OnReLoadFishNotcieConfig();
//			return true;
//		}
//		break;
//	case CL_KickUserByID:
//		{
//			CL_Cmd_KickUserByID* pMsg = (CL_Cmd_KickUserByID*)pCmd;
//			CenterRole* pRole = GetRoleManager().QueryCenterUser(pMsg->dwUserID);
//			if (!pRole)
//			{
//				LC_Cmd_KickUserResult msg;
//				SetMsgInfo(msg, GetMsgType(Main_Control, LC_KickUserResult), sizeof(LC_Cmd_KickUserResult));
//				msg.dwUserID = pMsg->dwUserID;
//				msg.Result = false;
//				msg.ClientID =0;
//				SendNetCmdToControl(pClient, &msg);
//				return true;
//			}
//			else
//			{
//				pMsg->ClientID = pClient->OutsideExtraData;
//				pRole->SendDataToGameServer(pMsg);
//				return true;
//			}
//		}
//		break;
//	case CL_ReloadConfig:
//		{
//			OnReloadConfig();
//			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
//			for (; Iter != m_ClintList.end(); ++Iter)
//			{
//				SendNetCmdToClient(Iter->second, pCmd);
//			}
//			return true;
//		}
//		break;
//	}
//	return true;
//}
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
void FishServer::UpdateInfoToControl(DWORD dwTimer)
{
	static DWORD LogUpdateCenterTime = 0;
	if (LogUpdateCenterTime == 0 || dwTimer - LogUpdateCenterTime >= 10000)
	{
		LogUpdateCenterTime = dwTimer;
		LC_Cmd_CenterInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_Control, LC_CenterInfo), sizeof(LC_Cmd_CenterInfo));
		msg.CenterID = m_CenterworkID;
		msg.AllGamePlayerSum = GetRoleManager().GetOnLinePlayerSum();
		msg.GameServerSum = m_CenterServerManager.GetGameServerSum();
		msg.LogonServerSum = m_CenterServerManager.GetLogonServerSum();
		SendNetCmdToControl(&msg);
	}
}