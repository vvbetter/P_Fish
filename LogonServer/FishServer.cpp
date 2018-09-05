#include "stdafx.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
FishServer g_FishServer;
FishServer::FishServer()
{
	m_IsClose = false;
	m_DBIsConnect = false;
	m_CenterIsConnect = false;
	m_OperateIsConnect = false;
	m_ClientIndex = 0;
	//m_ServerIndex = 0;

	m_ClientNetworkID = 0;
	//m_ServerNetworkID = 0;

	m_IsReloadConfig = false;
	m_ControlIsConnect = false;

	m_LogDBIsConnect = false;
}
FishServer::~FishServer()
{
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
void FishServer::Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
{
	if (!pClient)
	{
		ASSERT(false);
		return; 
	}
	if (ServerID == m_ClientNetworkID)
	{
		return;//�ͻ����뿪��������
	}
	//��� �ͻ����뿪��״̬
	switch (rt)
	{
	case REMOVE_NONE:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��ΪNone",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_NORMAL:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ��ͨ",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECV_ERROR:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���մ���",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_SEND_ERROR:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���ʹ���",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_TIMEOUT:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ��ʱ",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SEND_OVERFLOW:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_RECV_OVERFLOW:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ����̫��",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SIZE_ERROR:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ�����С",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECVBACK_NOT_SPACE:
		{
			ShowInfoToWin("�ͻ��� ID:%d �뿪ԭ��Ϊ���ջ��������",pClient->OutsideExtraData);
		}
		break;
	}
}
bool FishServer::InitServer(DWORD LogonID)
{
	m_pDump.OnInit();
	//��ʼ����½������ ���ǽ������� 
	//1.�������ļ��� ���� Logon �� һЩ���� ������Ҫ�������� FishServer.xml ����ж�ȡ ��������˿� ���ڼ����˿� ���ݿ�Ķ˿��Ƕ��� �� ��Ҫ���д���
	m_LogonConfigID = ConvertDWORDToBYTE(LogonID);
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

	if (!ConnectControl())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectControl())
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
	if (!ConnectCenter())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectCenter())
				break;
		}
	}
	if (!ConnectOperate())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectOperate())
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
	

 	m_LogonCacheManager.OnLoadAllAccountData();//�����ݿ���������
	return true;
}
bool FishServer::ConnectLogDB()
{
	LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(m_LogonConfigID);
	if (!pLogonConfig)
	{
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pLogonConfig->LogDBID);
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
	if (!m_LogDBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_LogonConfigID, sizeof(BYTE)))
	{
		ShowInfoToWin("Log���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_LogDBTcp);
	return true;
}
bool FishServer::ConnectDB()
{
	LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(m_LogonConfigID);
	if (!pLogonConfig)
	{
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pLogonConfig->DBNetworkID);
	if (!pDBConfig)
	{
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
	if (!m_DBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_LogonConfigID, sizeof(BYTE)))
	{
		ShowInfoToWin("���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
bool FishServer::ConnectCenter()
{
	CenterServerConfig* pCenterConfig = g_FishServerConfig.GetCenterServerConfig();
	if (!pCenterConfig)
	{
		return false;
	}

	ClientInitData pDBData;
	pDBData.BuffSize = pCenterConfig->BuffSize;
	pDBData.Port = pCenterConfig->CenterListenPort;
	pDBData.SleepTime = pCenterConfig->SleepTime;
	pDBData.SocketRecvSize = pCenterConfig->RecvBuffSize;
	pDBData.SocketSendSize = pCenterConfig->SendBuffSize;
	pDBData.ThreadNum = 1;
	pDBData.Timeout = pCenterConfig->TimeOut;
	pDBData.CmdHearbeat = 0;
	pDBData.SendArraySize = pCenterConfig->MaxSendCmdCount;
	pDBData.RecvArraySize = pCenterConfig->MaxRecvCmdCount;
	if (!m_CenterTcp.Init(pCenterConfig->CenterListenIP, pDBData, (void*)&m_LogonConfigID, sizeof(BYTE)))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_CenterTcp);
	return true;
}
bool FishServer::ConnectOperate()
{
	OperateConfig* pOperateConfig = g_FishServerConfig.GetOperateConfig();
	if (!pOperateConfig)
	{
		return false;
	}
	ClientInitData pDBData;
	pDBData.BuffSize = pOperateConfig->BuffSize;
	pDBData.Port = pOperateConfig->OperateListenPort;
	pDBData.SleepTime = pOperateConfig->SleepTime;
	pDBData.SocketRecvSize = pOperateConfig->RecvBuffSize;
	pDBData.SocketSendSize = pOperateConfig->SendBuffSize;
	pDBData.ThreadNum = 1;
	pDBData.Timeout = pOperateConfig->TimeOut;
	pDBData.CmdHearbeat = 0;
	pDBData.SendArraySize = pOperateConfig->MaxSendCmdCount;
	pDBData.RecvArraySize = pOperateConfig->MaxRecvCmdCount;
	if (!m_OperateTcp.Init(pOperateConfig->OperateListenIP, pDBData, (void*)&m_LogonConfigID, sizeof(BYTE)))
	{
		ShowInfoToWin("��Ӫ����������ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_OperateTcp);
	return true;
}
bool FishServer::ConnectClient()
{
	LogonServerConfig* pLogonConfig = g_FishServerConfig.GetLogonServerConfig(m_LogonConfigID);
	if (!pLogonConfig)
	{
		return false;
	}

	//���͵��ͻ��˵�
	ServerInitData pCLientData;
	pCLientData.ServerID = pLogonConfig->ClientNetworkID;
	pCLientData.BuffSize = pLogonConfig->LogonClientBuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.Port = pLogonConfig->LogonClientListenPort;
	pCLientData.SocketRecvSize = pLogonConfig->LogonClientRecvBuffSize;
	pCLientData.SocketSendSize = pLogonConfig->LogonClientSendBuffSize;
	pCLientData.Timeout = pLogonConfig->LogonClientTimeOut;
	pCLientData.RecvThreadNum = pLogonConfig->LogonClientRecvThreadSum;
	pCLientData.SendThreadNum = pLogonConfig->LogonClientSendThreadSum;
	pCLientData.SleepTime = pLogonConfig->LogonClientSleepTime;
	pCLientData.AcceptSleepTime = pLogonConfig->LogonClientAcceptSleepTime;
	pCLientData.AcceptRecvData = false;	//��ʾ���ճ�ʼ����

	pCLientData.SceneHearbeatTick = pLogonConfig->LogonClientSceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pLogonConfig->LogonClientMaxSendCmdCount;
	pCLientData.MaxAcceptNumPerFrame = pLogonConfig->LogonClientMaxAcceptNumPerFrame;
	pCLientData.MaxRecvCmdCount = pLogonConfig->LogonClientMaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pLogonConfig->LogonClientMaxSendCountPerFrame;
	pCLientData.ListenCount = pLogonConfig->LogonClientListenCount;

	m_ClientTcp.SetCmdHandler(this);
	m_ClientNetworkID = pLogonConfig->ClientNetworkID;
	if (!m_ClientTcp.Init(pCLientData))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}
	//��GameServer �� 
	/*ServerInitData pServerData;
	pServerData.ServerID = pLogonConfig->ServerNetworkID;
	pServerData.BuffSize = pLogonConfig->LogonServerBuffSize;
	pServerData.CmdHearbeat = 0;
	pServerData.Port = pLogonConfig->LogonServerListenPort;
	pServerData.SocketRecvSize = pLogonConfig->LogonServerRecvBuffSize;
	pServerData.SocketSendSize = pLogonConfig->LogonServerSendBuffSize;
	pServerData.Timeout = pLogonConfig->LogonServerTimeOut;
	pServerData.RecvThreadNum = pLogonConfig->LogonServerRecvThreadSum;
	pServerData.SendThreadNum = pLogonConfig->LogonServerSendThreadSum;
	pServerData.SleepTime = pLogonConfig->LogonServerSleepTime;
	pServerData.AcceptSleepTime = pLogonConfig->LogonServerAcceptSleepTime;
	pServerData.AcceptRecvData = true;

	pServerData.SceneHearbeatTick = pLogonConfig->LogonServerSceneHearbeatTick;
	pServerData.MaxSendCmdCount = pLogonConfig->LogonServerMaxSendCmdCount;
	pServerData.MaxAcceptNumPerFrame = pLogonConfig->LogonServerMaxAcceptNumPerFrame;
	pServerData.MaxRecvCmdCount = pLogonConfig->LogonServerMaxRecvCmdCount;
	pServerData.MaxSendCountPerFrame = pLogonConfig->LogonServerMaxSendCountPerFrame;
	pServerData.ListenCount = pLogonConfig->LogonServerListenCount;

	m_ServerTcp.SetCmdHandler(this);
	m_ServerNetworkID = pLogonConfig->ServerNetworkID;
	if (!m_ServerTcp.Init(pServerData))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}*/
	return true;
}
bool FishServer::OnDestroy()
{
	HandleAllMsg();
	OnClearAllChannelInfo();
	while (m_AfxAddClient.HasItem())
	{
		AfxNetworkClientOnce* pOnce = m_AfxAddClient.GetItem();
		delete pOnce;
	}
	return true;
}
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//���������ֻ����FTP �� DB�����ӳɹ���Ź���
	bool IsNeed = m_DBTcp.IsConnected() && m_CenterTcp.IsConnected();
	if (IsNeed)
	{
		/*if (SeverID == m_ServerNetworkID)
			return CONNECT_CHECK_OK;
		else */
		if (SeverID == m_ClientNetworkID)
		{
			if (recvSize != 2*sizeof(DWORD) || !pData)
				return CONNECT_CHECK_FAILED;
			DWORD* pInfo = (DWORD*)pData;
			if (!m_FishConfig.CheckVersionAndPathCrc(pInfo[0], pInfo[1]))
			{
				pInfo[0] = m_FishConfig.GetSystemConfig().VersionID;
				pInfo[1] = m_FishConfig.GetSystemConfig().PathCrc;
				return 2 * sizeof(DWORD);
			}
			return (CONNECT_CHECK_OK);
		}
		else
			return (CONNECT_CHECK_FAILED);
	}
	else
		return (CONNECT_CHECK_FAILED);
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
		/*if (SeverID == m_ServerNetworkID)
		{
			m_ServerTcp.Kick(pClient);
		}
		else*/
		if (SeverID == m_ClientNetworkID)
		{
			m_ClientTcp.Kick(pClient);
		}
		ASSERT(false);
		return false;
	}
	pOnce->SeverID = SeverID;
	pOnce->pClient = pClient;
	pOnce->pPoint = (void*)malloc(recvSize);
	if (!pOnce->pPoint)
	{
		/*if (SeverID == m_ServerNetworkID)
		{
			m_ServerTcp.Kick(pClient);
		}
		else */
		if (SeverID == m_ClientNetworkID)
		{
			m_ClientTcp.Kick(pClient);
		}
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
			/*if (pOnce->SeverID == m_ServerNetworkID)
			{
				m_ServerTcp.Kick(pOnce->pClient);
			}
			else */
			if (pOnce->SeverID == m_ClientNetworkID)
			{
				m_ClientTcp.Kick(pOnce->pClient);
			}
			//ASSERT(false);
			continue;
		}
		if (pOnce->SeverID == m_ClientNetworkID)
		{
			pOnce->pClient->OutsideExtraData = ++m_ClientIndex;
			m_ClintList.insert(HashMap<DWORD, ServerClientData*>::value_type(pOnce->pClient->OutsideExtraData, pOnce->pClient));
			//������Ҽ�����¼�
			OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;
		}
		//else if (pOnce->SeverID == m_ServerNetworkID)
		//{
		//	if (!pOnce->pPoint || pOnce->Length != sizeof(BYTE))
		//	{
		//		m_ServerTcp.Kick(pOnce->pClient);
		//		//ASSERT(false);
		//		if (pOnce->pPoint)
		//			free(pOnce->pPoint);
		//		delete pOnce;
		//	}
		//	BYTE ClientID = *((BYTE*)pOnce->pPoint);
		//	if (ClientID == 0)
		//	{
		//		m_ServerTcp.Kick(pOnce->pClient);
		//		//ASSERT(false);
		//		if (pOnce->pPoint)
		//			free(pOnce->pPoint);
		//		delete pOnce;
		//		continue;
		//	}
		//	pOnce->pClient->OutsideExtraData = ClientID;//��ҵ�ID
		//	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ServerList.find(ClientID);
		//	if (Iter != m_ServerList.end())
		//	{
		//		ASSERT(false);
		//		m_ServerTcp.Kick(pOnce->pClient);
		//		if (pOnce->pPoint)
		//			free(pOnce->pPoint);
		//		delete pOnce;
		//		continue;
		//	}
		//	m_ServerList.insert(HashMap<BYTE, ServerClientData*>::value_type(ClientID, pOnce->pClient));
		//	//������Ҽ�����¼�
		//	OnTcpServerJoin(pOnce->SeverID, pOnce->pClient);
		//	if (pOnce->pPoint)
		//		free(pOnce->pPoint);
		//	delete pOnce;

		//	m_LogonManager.OnGameRsgLogon(ClientID);
		//}
		else
		{
			//ASSERT(false);
			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;
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
	//if (ServerID == m_ServerNetworkID)
	//{
	//	/*GameServerInfo* pGameServerInfo = GetGameServerInfo(pClient->OutsideExtraData);
	//	if (pGameServerInfo)
	//		delete pGameServerInfo;*/
	//	//m_GameServerOnlineSumMap.erase((DWORD)pClient->OutsideExtraData);
	//	m_LogonManager.OnGameLeaveLogon(ConvertDWORDToBYTE(pClient->OutsideExtraData));
	//	ShowInfoToWin("һ��GameServer�뿪��LogonServer");
	//}
	//else 
	if (ServerID == m_ClientNetworkID)
	{
		//ShowInfoToWin("һ��Client�뿪��LogonServer");
	}
	else
	{
		ASSERT(false);
		return;
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
	//if (ServerID == m_ServerNetworkID)
	//{
	//	ShowInfoToWin("һ��GameServer������LogonServer");
	//	//��һ��GameServer���뵽��������ȥ��ʱ�� 
	//	m_LogonManager.OnGameJoinLogon(ConvertDWORDToBYTE(pClient->OutsideExtraData));
	//}
	//else 
	if (ServerID == m_ClientNetworkID)
	{
		//ShowInfoToWin("һ��Client������LogonServer");
	}
	else
	{
		ASSERT(false);
		return;
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
	//��������ӳɹ���ʱ��
	if (pClient == &m_DBTcp)
	{
		m_DBIsConnect = true;
		ShowInfoToWin("�Ѿ������ݿ����ӳɹ���");
	}
	else if (pClient == &m_CenterTcp)
	{
		m_CenterIsConnect = true;

		m_LogonManager.OnLogonRsgToCenter();//ע��Logon��Centerȥ
		ShowInfoToWin("�Ѿ���������������ӳɹ���");
	}
	else if (pClient == &m_OperateTcp)
	{
		m_OperateIsConnect = true;
		ShowInfoToWin("�Ѿ�����Ӫ���������ӳɹ���");
	}
	else if (pClient == &m_ControlTcp)
	{
		m_ControlIsConnect = true;
		ShowInfoToWin("�Ѿ���ControlServer���ӳɹ���");
	}
	else if (pClient == &m_LogDBTcp)
	{
		m_LogDBIsConnect = true;
		ShowInfoToWin("�Ѿ���Log���ݿ����ӳɹ���");
	}
	else
	{
		ASSERT(false);
		return;
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
		ShowInfoToWin("�Ѿ������ݿ�Ͽ�������");
		m_DBIsConnect = false;
		m_DBTcp.Shutdown();
	}
	else if (pClient == &m_CenterTcp)
	{
		ShowInfoToWin("�Ѿ�������������Ͽ�������");
		m_CenterIsConnect = false;
		m_CenterTcp.Shutdown();
		m_LogonManager.OnLogonLeaveCenter();
	}
	else if (pClient == &m_OperateTcp)
	{
		m_OperateIsConnect = false;
		m_OperateTcp.Shutdown();
		ShowInfoToWin("�Ѿ�����Ӫ�������Ͽ�������");
	}
	else if (pClient == &m_ControlTcp)
	{
		m_ControlIsConnect = false;
		m_ControlTcp.Shutdown();
		ShowInfoToWin("�Ѿ���ControlServer�Ͽ�������");
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
		return;
	}
	return;
}
void FishServer::SendNetCmdToDB(NetCmd* pCmd)
{
	if (!pCmd || !m_DBTcp.IsConnected())
		return;
	if (!m_DBTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToCenter(NetCmd* pCmd)
{
	if (!pCmd || !m_CenterTcp.IsConnected())
		return;
	if (!m_CenterTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToOperate(NetCmd* pCmd)
{
	if (!pCmd || !m_OperateTcp.IsConnected())
		return;
	if (!m_OperateTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::SendNetCmdToGameServer(BYTE GameID, NetCmd* pCmd)
{
	if (!pCmd)
		return;
	DWORD PageSize = sizeof(SS_Cmd_LogonToGame)+sizeof(BYTE)* (pCmd->CmdSize - 1);
	CheckMsgSize(PageSize);
	SS_Cmd_LogonToGame* msg = (SS_Cmd_LogonToGame*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdSize(static_cast<WORD>(PageSize));
	msg->SetCmdType(GetMsgType(Main_Server, SS_LogonToGame));
	memcpy_s(msg->Array, pCmd->CmdSize, pCmd, pCmd->CmdSize);
	msg->Length = pCmd->CmdSize;
	msg->GameID = GameID;
	SendNetCmdToCenter(msg);
	free(msg);
	//m_ServerTcp.Send(pClient, pCmd);
}
void FishServer::SendNetCmdToOtherLogon(BYTE LogonID, NetCmd* pCmd)
{
	if (!pCmd)
		return;
	DWORD PageSize = sizeof(SS_Cmd_GameToLogon)+sizeof(BYTE)* (pCmd->CmdSize - 1);
	CheckMsgSize(PageSize);
	SS_Cmd_GameToLogon* msg = (SS_Cmd_GameToLogon*)malloc(PageSize);
	if (!msg)
	{
		ASSERT(false);
		return;
	}
	msg->SetCmdSize(static_cast<WORD>(PageSize));
	msg->SetCmdType(GetMsgType(Main_Server, SS_GameToLogon));
	memcpy_s(msg->Array, pCmd->CmdSize, pCmd, pCmd->CmdSize);
	msg->Length = pCmd->CmdSize;
	msg->LogonID = LogonID;
	SendNetCmdToCenter(msg);
	free(msg);
}
void FishServer::SendNewCmdToClient(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return;
	if (!m_ClientTcp.Send(pClient, pCmd))
	{
		ASSERT(false);
	}
}
void FishServer::HandleAllMsg()
{
	if (!m_ClintList.empty())
	{
		HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
		for (; Iter != m_ClintList.end();)
		{
			if (!Iter->second)
				continue;
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

	//if (!m_ServerList.empty())
	//{
	//	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ServerList.begin();
	//	for (; Iter != m_ServerList.end();)
	//	{
	//		if (!Iter->second)
	//			continue;
	//		while (Iter->second->RecvList.HasItem())
	//		{
	//			NetCmd* pCmd = Iter->second->RecvList.GetItem();
	//			//������������ GameServer������������
	//			HandleGameServerMsg(Iter->second, pCmd);
	//			free(pCmd);
	//		}
	//		++Iter;
	//	}
	//}

	NetCmd* pCmd = m_DBTcp.GetCmd();
	while (pCmd)
	{
		HandleDataBaseMsg(pCmd);
		free(pCmd);
		pCmd = m_DBTcp.GetCmd();
	}

	pCmd = m_OperateTcp.GetCmd();
	while (pCmd)
	{
		HandleOperateMsg(pCmd);
		free(pCmd);
		pCmd = m_OperateTcp.GetCmd();
	}

	pCmd = m_ControlTcp.GetCmd();
	while (pCmd)
	{
		HandleControlMsg(pCmd);
		free(pCmd);
		pCmd = m_ControlTcp.GetCmd();
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
		//0.
		OnAddClient();
		//1.
		OnUpdateWriteGameServerUserMap(dwTimer);

		UpdateInfoToControl(dwTimer);

		//1.����Client ������
		if (!m_ClintList.empty())
		{
			HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
			for (; Iter != m_ClintList.end();)
			{
				if (!Iter->second)
					continue;
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_ClientNetworkID, Iter->second);
					m_ClientTcp.Kick(Iter->second);
					Iter = m_ClintList.erase(Iter);

					//ShowInfoToWin("�Ƴ�һ���ͻ���");

					continue;
				}
				int Sum = 0;
				while (Iter->second->RecvList.HasItem() && Sum < Client_Msg_OnceSum)
				{
					NetCmd* pCmd = Iter->second->RecvList.GetItem();
					//������������ �ͻ��˷������ĵ�½ ע�� ������
					HandleClientMsg(Iter->second, pCmd);
					free(pCmd);
					++Sum;
				}
				++Iter;
			}
		}
		//2.����Server������
		//if (!m_ServerList.empty())
		//{
		//	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ServerList.begin();
		//	for (; Iter != m_ServerList.end();)
		//	{
		//		if (!Iter->second)
		//			continue;
		//		if (Iter->second->Removed)
		//		{
		//			OnTcpServerLeave(m_ServerNetworkID, Iter->second);
		//			m_ServerTcp.Kick(Iter->second);
		//			Iter = m_ServerList.erase(Iter);
		//			continue;
		//		}
		//		int Sum = 0;
		//		while (Iter->second->RecvList.HasItem() && Sum < Msg_OnceSum)
		//		{
		//			NetCmd* pCmd = Iter->second->RecvList.GetItem();
		//			//������������ GameServer������������
		//			HandleGameServerMsg(Iter->second, pCmd);
		//			free(pCmd);
		//			++Sum;
		//		}
		//		++Iter;
		//	}
		//}
		//3.�������ݿ������
		if (m_DBTcp.IsConnected()) 
		{
			if (!m_DBIsConnect)
				OnTcpClientConnect(&m_DBTcp);
			NetCmd* pCmd = m_DBTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				//�������ݿⷢ����������
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

		//4.�������������������
		if (m_CenterTcp.IsConnected())
		{
			if (!m_CenterIsConnect)
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
			if (m_CenterIsConnect)
				OnTcpClientLeave(&m_CenterTcp);

			//��������DB
			ConnectCenter();
		}

		//5.������Ӫ������������
		if (m_OperateTcp.IsConnected())
		{
			if (!m_OperateIsConnect)
				OnTcpClientConnect(&m_OperateTcp);
			NetCmd* pCmd = m_OperateTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleOperateMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_OperateTcp.GetCmd();
			}
			if (pCmd)
			{
				HandleOperateMsg(pCmd);
				free(pCmd);
			}
		}
		else
		{
			if (m_OperateIsConnect)
				OnTcpClientLeave(&m_OperateTcp);

			//��������DB
			ConnectOperate();
		}

		if (m_ControlTcp.IsConnected())
		{
			if (!m_ControlIsConnect)
				OnTcpClientConnect(&m_ControlTcp);
			NetCmd* pCmd = m_ControlTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleControlMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_ControlTcp.GetCmd();
			}
			if (pCmd)
			{
				HandleControlMsg(pCmd);
				free(pCmd);
			}
		}
		else
		{
			if (m_ControlIsConnect)
				OnTcpClientLeave(&m_ControlTcp);

			//��������DB
			ConnectControl();
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

void FishServer::HandleClientMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return;
	if (pCmd->CmdType != Main_Logon)
		return;
	switch (pCmd->SubCmdType)
	{
	case CL_AccountLogon:
		{
			CL_Cmd_AccountLogon* pMsg = (CL_Cmd_AccountLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			//�Ƚ��� �汾 ·�� ����֤  ��֤�ɹ� �� ��������ҽ��е�½ ��½����Я�� ƽ̨��ID
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				//ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ��˰汾���� ��ͨ��½ʧ�� %u, %u  vs %u, %u"), pMsg->VersionID, pMsg->PathCrc, m_FishConfig.GetSystemConfig().VersionID, m_FishConfig.GetSystemConfig().PathCrc);

				return;
			}
			if (!m_FishConfig.CheckStringIsError(pMsg->AccountName, ACCOUNT_MIN_LENGTH, ACCOUNT_LENGTH, SCT_AccountName))
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 1;
				SendNewCmdToClient(pClient, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);


				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺����ƴ��� ��ͨ��½ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);

				return;
			}
			//���򻺴�����
			DWORD dwUserID = 0;
			bool IsFreeze = false;
			__int64 FreezeEndTime = 0;
			if (!m_LogonCacheManager.CheckAccountData(pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3, dwUserID, IsFreeze, FreezeEndTime))
			{
				DBR_Cmd_AccountLogon msg;
				SetMsgInfo(msg, DBR_AccountLogon, sizeof(DBR_Cmd_AccountLogon));
				msg.ClientID = pClient->OutsideExtraData;
				TCHARCopy(msg.AccountName, CountArray(msg.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				msg.PasswordCrc1 = pMsg->PasswordCrc1;
				msg.PasswordCrc2 = pMsg->PasswordCrc2;
				msg.PasswordCrc3 = pMsg->PasswordCrc3;
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				SendNetCmdToDB(&msg);

				//�����¼
				TempAccountCacheInfo pData;
				pData.ClientID = pClient->OutsideExtraData;
				TCHARCopy(pData.AccountInfo.AccountName, CountArray(pData.AccountInfo.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				TCHARCopy(pData.AccountInfo.MacAddress, CountArray(pData.AccountInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				pData.AccountInfo.PasswordCrc1 = pMsg->PasswordCrc1;
				pData.AccountInfo.PasswordCrc2 = pMsg->PasswordCrc2;
				pData.AccountInfo.PasswordCrc3 = pMsg->PasswordCrc3;
				m_LogonCacheManager.OnAddTempAccountInfo(pData);
			}
			else
			{
				//������֤����� ���ǿ��Խ��д����� 
				if (dwUserID == 0)
				{
					//������� �޷���½
					LC_Cmd_AccountOnlyID msgClient;
					SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
					msgClient.dwOnlyID = 0;
					msgClient.dwUserID = 0;
					msgClient.GameIp = 0;
					msgClient.GamePort = 0;
					msgClient.GateIp = 0;
					msgClient.GatePort = 0;
					msgClient.LogonType = 1;
					SendNewCmdToClient(pClient, &msgClient);

					DelClient pDel;
					pDel.LogTime = timeGetTime();
					pDel.SocketID = pClient->OutsideExtraData;
					m_DelSocketVec.push_back(pDel);

					LogInfoToFile("LogonError.txt", TEXT("�ͻ����˺Ų����ڻ���������� ��ͨ��½ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				}
				else
				{
					if (IsFreeze)
					{
						LC_Cmd_AccountIsFreeze msg;
						SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
						msg.EndTime = FreezeEndTime;
						SendNewCmdToClient(pClient, &msg);

						DelClient pDel;
						pDel.LogTime = timeGetTime();
						pDel.SocketID = pClient->OutsideExtraData;
						m_DelSocketVec.push_back(pDel);
						LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
						return;
					}
					GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(dwUserID));//�ҵ�һ������ʵ�GameServer
					if (!pConfig)
					{
						ASSERT(false);
						return;
					}
					/*ServerClientData* pGameServer = GetServerServerData(pConfig->NetworkID);
					if (!pGameServer)
					{
						ASSERT(false);
						return;
					}*/
					if (!OnAddUserWriteGameServer(dwUserID, pClient->OutsideExtraData))
					{
						ASSERT(false);
						return;
					}
					DWORD OnlyID = RandUInt();
					//1.�������GameServerȥ
					OnSendChannelInfoToGameServer(dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);
					LG_Cmd_AccountOnlyID msgGameServer;
					SetMsgInfo(msgGameServer, GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
					msgGameServer.dwOnlyID = OnlyID;
					msgGameServer.dwUserID = dwUserID;
					msgGameServer.ClientID = pClient->OutsideExtraData;
					msgGameServer.GameServerID = pConfig->NetworkID;
					msgGameServer.LogonTypeID = 1;
					msgGameServer.LogonID = m_LogonConfigID;
					SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);//������ͨ��������������͵�Logonȥ
				}
			}
			return;
		}
		break;
	case CL_AccountRsg:
		{
			CL_Cmd_AccountRsg* pMsg = (CL_Cmd_AccountRsg*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ��˰汾���� ��ͨע��ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			if (!m_FishConfig.CheckStringIsError(pMsg->AccountName, ACCOUNT_MIN_LENGTH, ACCOUNT_LENGTH, SCT_AccountName))
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 2;
				SendNewCmdToClient(pClient, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺����ƴ��� ��ͨע��ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			if (m_LogonCacheManager.IsExistsAccount(pMsg->AccountName))
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 2;
				SendNewCmdToClient(pClient, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺��Ѿ����� ��ͨע��ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}

			DBR_Cmd_AccountRsg msg;
			SetMsgInfo(msg,DBR_AccountRsg, sizeof(DBR_Cmd_AccountRsg));
			msg.ClientID = pClient->OutsideExtraData;
			TCHARCopy(msg.AccountName, CountArray(msg.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			msg.PasswordCrc1 = pMsg->PasswordCrc1;
			msg.PasswordCrc2 = pMsg->PasswordCrc2;
			msg.PasswordCrc3 = pMsg->PasswordCrc3;
			msg.ClientIP = pClient->IP;
			SendNetCmdToDB(&msg);

			TempAccountCacheInfo pData;
			pData.ClientID = pClient->OutsideExtraData;
			TCHARCopy(pData.AccountInfo.AccountName, CountArray(pData.AccountInfo.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
			TCHARCopy(pData.AccountInfo.MacAddress, CountArray(pData.AccountInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			pData.AccountInfo.PasswordCrc1 = pMsg->PasswordCrc1;
			pData.AccountInfo.PasswordCrc2 = pMsg->PasswordCrc2;
			pData.AccountInfo.PasswordCrc3 = pMsg->PasswordCrc3;
			m_LogonCacheManager.OnAddTempAccountInfo(pData);

			return;
		}
		break;
	case CL_GetNewAccount:
		{
			CL_Cmd_GetNewAccount* pMsg = (CL_Cmd_GetNewAccount*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				//ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ��˰汾���� ��ȡ���˺�ʧ��"));
				return;
			}
			//������ת�������ݿ�ȥ
			DBR_Cmd_GetNewAccount msg;
			SetMsgInfo(msg, DBR_GetNewAccount, sizeof(DBR_Cmd_GetNewAccount));
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			msg.ClientID = pClient->OutsideExtraData;
			msg.PasswordCrc1 = pMsg->PasswordCrc1;
			msg.PasswordCrc2 = pMsg->PasswordCrc2;
			msg.PasswordCrc3 = pMsg->PasswordCrc3;
			msg.ClientIP = pClient->IP;
			SendNetCmdToDB(&msg);
			return;
		}
		break;
	case CL_ChannelLogon:
		{
			//g_FishServer.ShowInfoToWin("���յ���ҽ���������½1");
			CL_Cmd_ChannelLogon* pMsg = (CL_Cmd_ChannelLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			//g_FishServer.ShowInfoToWin("���յ���ҽ���������½2");
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ��˰汾���� ������½ʧ��"));
				return;
			}
			//g_FishServer.ShowInfoToWin("���յ���ҽ���������½3");
			//���յ�������½ ����ֱ�ӽ�����ת���� Operater  ��Ӫ������ȥ
			DWORD Page = sizeof(LO_Cmd_ChannelLogon)+(pMsg->channelInfo.Sum - 1)*sizeof(BYTE);
			LO_Cmd_ChannelLogon* msg = (LO_Cmd_ChannelLogon*)malloc(Page);
			msg->SetCmdSize(ConvertDWORDToWORD(Page));
			msg->SetCmdType(GetMsgType(Main_Logon, LO_ChannelLogon));
			msg->PlateFormID = pMsg->PlateFormID;
			msg->ClientID = pClient->OutsideExtraData;
			DWORD DataLength = (pMsg->channelInfo.Sum - 1)*sizeof(BYTE)+sizeof(ChannelUserInfo);
			memcpy_s(&msg->channelInfo, DataLength, &pMsg->channelInfo, DataLength);
			TCHARCopy(msg->MacAddress, CountArray(msg->MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			SendNetCmdToOperate(msg);
			free(msg);
			//g_FishServer.ShowInfoToWin("���յ���ҽ���������½4");

			ShowInfoToWin("����������½");

			return;
		}	
		break;
	case CL_WeiXinLogon:
		{
			//�����ͼͨ��΢�ŵ�½
			CL_Cmd_WeiXinLogon* pMsg = (CL_Cmd_WeiXinLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ��˰汾���� ΢�ŵ�½ʧ��"));
				return;
			}

			HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pClient->OutsideExtraData);
			if (Iter != m_UserChannelMap.end())
			{
				ASSERT(false);
				m_UserChannelMap.erase(Iter);
			}
			std::vector<char*> pVec;
			GetStringArrayVecByData(pVec, &pMsg->CodeInfo);
			if (pVec.size() != pMsg->CodeInfo.HandleSum)
			{
				ASSERT(false);
				return;
			}
			string OpenID = pVec[0];
			FreeVec(pVec);

			//��������������ӽ�ȥ

			//ת��ΪTCHAR
			UINT Count = 0;
			TCHAR* pOpenID = CharToWChar(OpenID.c_str(), Count);
			DWORD dwOpenID= GetCrc32(pOpenID);

			CL_Cmd_QueryLogon msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, CL_QueryLogon), sizeof(CL_Cmd_QueryLogon));
			//1.�˺�����
			_stprintf_s(msg.AccountName, CountArray(msg.AccountName), TEXT("WeiXin%u"), dwOpenID);
			TCHAR Password[32];
			int length = _stprintf_s(Password, CountArray(Password), TEXT("WeiXinPass%u"), dwOpenID);
			for (int i = length + 1; i < 32; ++i)
				Password[i] = 0xfefe;
			free(pOpenID);

			AE_CRC_THREE pTree;
			AECrc32(pTree, Password, CountArray(Password) * sizeof(TCHAR), 0, 0x735739, 0x79387731);
			msg.PasswordCrc1 = pTree.Crc1;
			msg.PasswordCrc2 = pTree.Crc2;
			msg.PasswordCrc3 = pTree.Crc3;
			msg.PlateFormID = pMsg->PlateFormID;
			msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
			msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			msg.LogonType = 2;


			/*LC_Cmd_SaveAccountInfo msgSave;
			SetMsgInfo(msgSave, GetMsgType(Main_Logon, LC_SaveAccountInfo), sizeof(LC_Cmd_SaveAccountInfo));
			TCHARCopy(msgSave.AccountName, CountArray(msgSave.AccountName), msg.AccountName, _tcslen(msg.AccountName));
			msgSave.PasswordCrc1 = pTree.Crc1;
			msgSave.PasswordCrc2 = pTree.Crc2;
			msgSave.PasswordCrc3 = pTree.Crc3;
			SendNewCmdToClient(pClient, &msgSave);*/

			HandleClientMsg(pClient, &msg);

			//���յ�������½ ����ֱ�ӽ�����ת���� Operater  ��Ӫ������ȥ
			/*DWORD Page = sizeof(LO_Cmd_WeiXinLogon)+(pMsg->CodeInfo.Sum - 1)*sizeof(BYTE);
			LO_Cmd_WeiXinLogon* msg = (LO_Cmd_WeiXinLogon*)malloc(Page);
			msg->SetCmdSize(ConvertDWORDToWORD(Page));
			msg->SetCmdType(GetMsgType(Main_Logon, LO_WeiXinLogon));
			msg->PlateFormID = pMsg->PlateFormID;
			msg->ClientID = pClient->OutsideExtraData;
			DWORD DataLength = (pMsg->CodeInfo.Sum - 1)*sizeof(BYTE)+sizeof(WeiXinUserCodeInfo);
			memcpy_s(&msg->CodeInfo, DataLength, &pMsg->CodeInfo, DataLength);
			TCHARCopy(msg->MacAddress, CountArray(msg->MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			SendNetCmdToOperate(msg);
			free(msg);
			ShowInfoToWin("����΢�ŵ�½");*/
			return;
		}
		break;
	case CL_QQLogon:
		{
			//�����ͼͨ��΢�ŵ�½
			CL_Cmd_QQLogon* pMsg = (CL_Cmd_QQLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ��˰汾���� QQ��½ʧ��"));
				return;
			}

			HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pClient->OutsideExtraData);
			if (Iter != m_UserChannelMap.end())
			{
				ASSERT(false);
				m_UserChannelMap.erase(Iter);
			}
			std::vector<char*> pVec;
			GetStringArrayVecByData(pVec, &pMsg->TokenInfo);
			if (pVec.size() != pMsg->TokenInfo.HandleSum)
			{
				ASSERT(false);
				return;
			}
			string OpenID = pVec[0];
			FreeVec(pVec);

			//��������������ӽ�ȥ

			//ת��ΪTCHAR
			UINT Count = 0;
			TCHAR* pOpenID = CharToWChar(OpenID.c_str(), Count);
			DWORD dwOpenID = GetCrc32(pOpenID);

			CL_Cmd_QueryLogon msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, CL_QueryLogon), sizeof(CL_Cmd_QueryLogon));
			//1.�˺�����
			_stprintf_s(msg.AccountName, CountArray(msg.AccountName), TEXT("QQ%u"), dwOpenID);
			TCHAR Password[32];
			int length = _stprintf_s(Password, CountArray(Password), TEXT("QQPass%u"), dwOpenID);
			for (int i = length + 1; i < 32; ++i)
				Password[i] = 0xfefe;

			free(pOpenID);

			AE_CRC_THREE pTree;
			AECrc32(pTree, Password, CountArray(Password) * sizeof(TCHAR), 0, 0x735739, 0x79387731);
			msg.PasswordCrc1 = pTree.Crc1;
			msg.PasswordCrc2 = pTree.Crc2;
			msg.PasswordCrc3 = pTree.Crc3;
			msg.PlateFormID = pMsg->PlateFormID;
			msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
			msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			msg.LogonType = 3;

			/*LC_Cmd_SaveAccountInfo msgSave;
			SetMsgInfo(msgSave, GetMsgType(Main_Logon, LC_SaveAccountInfo), sizeof(LC_Cmd_SaveAccountInfo));
			TCHARCopy(msgSave.AccountName, CountArray(msgSave.AccountName), msg.AccountName, _tcslen(msg.AccountName));
			msgSave.PasswordCrc1 = pTree.Crc1;
			msgSave.PasswordCrc2 = pTree.Crc2;
			msgSave.PasswordCrc3 = pTree.Crc3;
			SendNewCmdToClient(pClient, &msgSave);*/


			HandleClientMsg(pClient, &msg);


			//���յ�������½ ����ֱ�ӽ�����ת���� Operater  ��Ӫ������ȥ
			/*DWORD Page = sizeof(LO_Cmd_QQLogon)+(pMsg->TokenInfo.Sum - 1)*sizeof(BYTE);
			LO_Cmd_QQLogon* msg = (LO_Cmd_QQLogon*)malloc(Page);
			msg->SetCmdSize(ConvertDWORDToWORD(Page));
			msg->SetCmdType(GetMsgType(Main_Logon, LO_QQLogon));
			msg->PlateFormID = pMsg->PlateFormID;
			msg->ClientID = pClient->OutsideExtraData;
			DWORD DataLength = (pMsg->TokenInfo.Sum - 1)*sizeof(BYTE)+sizeof(QQUserCodeInfo);
			memcpy_s(&msg->TokenInfo, DataLength, &pMsg->TokenInfo, DataLength);
			TCHARCopy(msg->MacAddress, CountArray(msg->MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			SendNetCmdToOperate(msg);
			free(msg);
			ShowInfoToWin("����QQ��½");*/
			return;
		}
		break;
	case CL_QueryLogon:
		{
			//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½1");
			CL_Cmd_QueryLogon* pMsg = (CL_Cmd_QueryLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ��˰汾���� ���ٵ�½ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½2");
			if (!m_FishConfig.CheckStringIsError(pMsg->AccountName, ACCOUNT_MIN_LENGTH, ACCOUNT_LENGTH, SCT_AccountName))
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 1;
				SendNewCmdToClient(pClient, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺����ƴ��� ���ٵ�½ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½3");
			//���򻺴�����
			DWORD dwUserID = 0;
			bool IsFreeze = false;
			__int64 FreezeEndTime = 0;
			if (!m_LogonCacheManager.CheckAccountData(pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3, dwUserID, IsFreeze, FreezeEndTime))
			{
				//�����ݿ���в�ѯ
				DBR_Cmd_QueryLogon msg;
				SetMsgInfo(msg, DBR_QueryLogon, sizeof(DBR_Cmd_QueryLogon));
				TCHARCopy(msg.AccountName, CountArray(msg.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				msg.ClientID = pClient->OutsideExtraData;
				msg.PasswordCrc1 = pMsg->PasswordCrc1;
				msg.PasswordCrc2 = pMsg->PasswordCrc2;
				msg.PasswordCrc3 = pMsg->PasswordCrc3;
				msg.ClientIP = pClient->IP;
				msg.LogonType = pMsg->LogonType;
				SendNetCmdToDB(&msg);
				ShowInfoToWin("���Ϳ��ٵ�½");

				//�����¼
				TempAccountCacheInfo pData;
				pData.ClientID = pClient->OutsideExtraData;
				TCHARCopy(pData.AccountInfo.AccountName, CountArray(pData.AccountInfo.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				TCHARCopy(pData.AccountInfo.MacAddress, CountArray(pData.AccountInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				pData.AccountInfo.PasswordCrc1 = pMsg->PasswordCrc1;
				pData.AccountInfo.PasswordCrc2 = pMsg->PasswordCrc2;
				pData.AccountInfo.PasswordCrc3 = pMsg->PasswordCrc3;
				m_LogonCacheManager.OnAddTempAccountInfo(pData);

				//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½4");
			}
			else
			{
				//������֤����� ���ǿ��Խ��д����� 
				if (dwUserID == 0)
				{
					//������� �޷���½
					LC_Cmd_AccountOnlyID msgClient;
					SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
					msgClient.dwOnlyID = 0;
					msgClient.dwUserID = 0;
					msgClient.GameIp = 0;
					msgClient.GamePort = 0;
					msgClient.GateIp = 0;
					msgClient.GatePort = 0;
					msgClient.LogonType = 1;
					SendNewCmdToClient(pClient, &msgClient);

					DelClient pDel;
					pDel.LogTime = timeGetTime();
					pDel.SocketID = pClient->OutsideExtraData;
					m_DelSocketVec.push_back(pDel);

					//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½5");

					LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺�������� ���ٵ�½ʧ�� ����˺�:%s ����:%u,%u,%u"), pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				}
				else
				{
					if (IsFreeze)
					{
						LC_Cmd_AccountIsFreeze msg;
						SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
						msg.EndTime = FreezeEndTime;
						SendNewCmdToClient(pClient, &msg);

						DelClient pDel;
						pDel.LogTime = timeGetTime();
						pDel.SocketID = pClient->OutsideExtraData;
						m_DelSocketVec.push_back(pDel);
						LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
						return;
					}
					GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(dwUserID));//�ҵ�һ������ʵ�GameServer
					if (!pConfig)
					{
						ASSERT(false);
						return;
					}
					/*ServerClientData* pGameServer = GetServerServerData(pConfig->NetworkID);
					if (!pGameServer)
					{
						ASSERT(false);
						return;
					}*/
					if (!OnAddUserWriteGameServer(dwUserID, pClient->OutsideExtraData))
					{
						ASSERT(false);
						return;
					}
					DWORD OnlyID = RandUInt();
					//1.�������GameServerȥ
					OnSendChannelInfoToGameServer(dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

					LG_Cmd_AccountOnlyID msgGameServer;
					SetMsgInfo(msgGameServer, GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
					msgGameServer.dwOnlyID = OnlyID;
					msgGameServer.dwUserID = dwUserID;
					msgGameServer.ClientID = pClient->OutsideExtraData;
					msgGameServer.GameServerID = pConfig->NetworkID;
					msgGameServer.LogonTypeID = 1;
					msgGameServer.LogonID = m_LogonConfigID;
					SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);

					//g_FishServer.ShowInfoToWin("�ͻ��˽��п��ٵ�½6");
				}
			}
			return;
		}
	case CL_PhoneLogon:
		{
			CL_Cmd_PhoneLogon* pMsg = (CL_Cmd_PhoneLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			//�����ͼ�����ֻ���½
			LO_Cmd_PhoneLogon msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LO_PhoneLogon), sizeof(LO_Cmd_PhoneLogon));
			msg.ClientID = pClient->OutsideExtraData;
			msg.PhoneNumber = pMsg->PhoneNumber;
			SendNetCmdToOperate(&msg);
			return;
		}
		break;
	case CL_PhoneLogonCheck:
		{
			CL_Cmd_PhoneLogonCheck * pMsg = (CL_Cmd_PhoneLogonCheck*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			LO_Cmd_PhoneLogonCheck msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LO_PhoneLogonCheck), sizeof(LO_Cmd_PhoneLogonCheck));
			msg.ClientID = pClient->OutsideExtraData;
			msg.Password = pMsg->Password;
			msg.PhoneNumber = pMsg->PhoneNumber;
			msg.ClientIP = pClient->IP;
			msg.Crc1 = pMsg->Crc1;
			msg.Crc2 = pMsg->Crc2;
			msg.Crc3 = pMsg->Crc3;
			msg.IsFirOrSec = pMsg->IsFirOrSec;
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress,_tcslen(pMsg->MacAddress));
			SendNetCmdToOperate(&msg);
			return;
		}
		break;
	case CL_PhoneSecPwd:
		{
			CL_Cmd_PhoneSecPwd* pMsg = (CL_Cmd_PhoneSecPwd*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			if (!m_FishConfig.CheckVersionAndPathCrc(pMsg->VersionID, pMsg->PathCrc))
			{
				ASSERT(false);
				LC_Cmd_CheckVersionError msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_CheckVersionError), sizeof(LC_Cmd_CheckVersionError));
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				SendNewCmdToClient(pClient, &msg);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ��˰汾���� �ֻ����������½ʧ�� ����ֻ���:%llu ����:%u,%u,%u"), pMsg->PhoneNumber, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			//��ҽ��ж��������½
			TempAccountCacheInfo pData;
			pData.ClientID = pClient->OutsideExtraData;
			_i64tot_s(pMsg->PhoneNumber, pData.AccountInfo.AccountName, CountArray(pData.AccountInfo.AccountName), 10);
			TCHARCopy(pData.AccountInfo.MacAddress, CountArray(pData.AccountInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			pData.AccountInfo.PasswordCrc1 = pMsg->PasswordCrc1;
			pData.AccountInfo.PasswordCrc2 = pMsg->PasswordCrc2;
			pData.AccountInfo.PasswordCrc3 = pMsg->PasswordCrc3;
			m_LogonCacheManager.OnAddTempAccountInfo(pData);

			DBR_Cmd_PhoneSecPwd msg;
			SetMsgInfo(msg, DBR_PhoneSecPwd, sizeof(DBR_Cmd_PhoneSecPwd));
			msg.ClientID = pClient->OutsideExtraData;
			msg.PhoneNumber = pMsg->PhoneNumber;
			TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
			msg.PasswordCrc1 = pMsg->PasswordCrc1;
			msg.PasswordCrc2 = pMsg->PasswordCrc2;
			msg.PasswordCrc3 = pMsg->PasswordCrc3;
			SendNetCmdToDB(&msg);
			return;
		}
		break;
	}
}

void FishServer::HandleGameServerMsg(NetCmd* pCmd)//����GameServer������������
{
	if (!pCmd)
		return;
	if (pCmd->CmdType == Main_Logon)
	{
		switch (pCmd->SubCmdType)
		{
		case GL_AccountOnlyID:
			{
				GL_Cmd_AccountOnlyID* pMsg = (GL_Cmd_AccountOnlyID*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				ServerClientData* pClientData = GetClientServerData(pMsg->ClientID);
				if (!pClientData)
				{
					ASSERT(false);
					return;
				}
			/*	GameServerInfo* pGameServerInfo = GetGameServerInfo(pMsg->GameServerID);
				if (!pGameServerInfo)
					return;*/
				GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(pMsg->GameServerID);
				if (!pConfig)
				{
					ASSERT(false);
					return;
				}

				if (!OnCheckUserWriteGameServerIsExists(pMsg->dwUserID))
				{
					ASSERT(false);
					return;
				}

				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = pMsg->dwOnlyID;
				msgClient.dwUserID = pMsg->dwUserID;
				HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pMsg->ClientID);
				if (Iter == m_UserChannelMap.end())
				{
					msgClient.GameIp = 0;
					msgClient.GamePort = 0;
					msgClient.GateIp = inet_addr(pConfig->GameListenIP);
					msgClient.GatePort = pConfig->GameListenPort;
				}
				else
				{
					DWORD ChannelID = Iter->second;
					m_UserChannelMap.erase(Iter);
					//��������
					GateWay* pGateWay = g_FishServerConfig.GetGateWay(ChannelID);
					if (!pGateWay)
					{
						msgClient.GameIp = 0;
						msgClient.GamePort = 0;
						msgClient.GateIp = inet_addr(pConfig->GameListenIP);
						msgClient.GatePort = pConfig->GameListenPort;
					}
					else
					{
						msgClient.GameIp = inet_addr(pConfig->GameListenIP);
						msgClient.GamePort = pConfig->GameListenPort;
						msgClient.GateIp = pGateWay->Ip;
						msgClient.GatePort = pGateWay->Port;
					}
				}
				msgClient.HallTimeOut = pConfig->TimeOut;//�����ĳ�ʱʱ��
				msgClient.LogonType = pMsg->LogonTypeID;
				SendNewCmdToClient(pClientData, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClientData->OutsideExtraData;//ɾ���ͻ���
				m_DelSocketVec.push_back(pDel);
				return;
			}
			break;
		case GL_ResetPassword:
			{
				//����޸����� Logon��Ҳ��Ҫ�����޸�
				GL_Cmd_ResetPassword* pMsg = (GL_Cmd_ResetPassword*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				m_LogonCacheManager.OnChangeAccountPassword(pMsg->dwUserID, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			break;
		case GL_ResetAccount:
			{
				//����޸����� Logon��Ҳ��Ҫ�����޸�
				GL_Cmd_ResetAccount* pMsg = (GL_Cmd_ResetAccount*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				m_LogonCacheManager.OnResetAccount(pMsg->dwUserID, pMsg->NewAccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);
				return;
			}
			break;
		}
	}
}
ServerClientData* FishServer::GetClientServerData(DWORD ClientID)
{
	HashMap<DWORD, ServerClientData*>::iterator IterFind = m_ClintList.find(ClientID);
	if (IterFind == m_ClintList.end() || !IterFind->second)
		return NULL;
	return IterFind->second;
}
//ServerClientData* FishServer::GetServerServerData(BYTE ServerID)
//{
//	HashMap<BYTE, ServerClientData*>::iterator IterFind = m_ServerList.find(ServerID);
//	if (IterFind == m_ServerList.end() || !IterFind->second)
//		return NULL;
//	return IterFind->second;
//}
void FishServer::OnLogonDBDataSucess()
{
	/*if ( !ConnectClient())
	{
		ASSERT(false);
		return;
	}*/
	while (!ConnectClient())
	{
		Sleep(5);
	}
	ShowInfoToWin("LogonServer ��ʼ���ɹ�");
	return;
}
//GameServerInfo* FishServer::GetGameServerInfo(DWORD GameServerID)
//{
//	HashMap<DWORD, GameServerInfo*>::iterator IterFind = m_GameServerOnlineSumMap.find(GameServerID);
//	if (IterFind == m_GameServerOnlineSumMap.end())
//		return NULL;
//	return IterFind->second;
//}
//ServerClientData* FishServer::GetMaxFreeGameServer()
//{
//	//��ȡ���е�GameServer
//	if (m_GameServerOnlineSumMap.empty())
//		return NULL;
//
//	DWORD pClientID = 0;
//	WORD PlayerSum = 65535;//һ��GameServer����������ô���� �����൱�����ֵ
//
//	HashMap<DWORD, GameServerInfo*>::iterator Iter = m_GameServerOnlineSumMap.begin();
//	for (; Iter != m_GameServerOnlineSumMap.end(); ++Iter)
//	{
//		if (Iter->second->OnlineSum < PlayerSum)
//		{
//			pClientID = Iter->first;
//			PlayerSum = Iter->second->OnlineSum;
//		}
//	}
//	if (pClientID == 0)
//		return NULL;
//	return GetServerServerData(pClientID);
//}
void FishServer::HandleDataBaseMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return;
	switch (pCmd->GetCmdType())
	{
	case DBO_AccountLogon:
		{
			DBO_Cmd_AccountLogon* pMsg = (DBO_Cmd_AccountLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);//�ͻ��� 
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			if (pMsg->dwUserID == 0)
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient,GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 1;
				SendNewCmdToClient(pClient, &msgClient);
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, false);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺�������� ���ݿ���ͨ��½ʧ��"));
				return;
			}
			else if (pMsg->IsFreeze)
			{
				//��ǰ�˺��Ѿ��������� ��Ҫ����
				LC_Cmd_AccountIsFreeze msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
				msg.EndTime = pMsg->FreezeEndTime;
				SendNewCmdToClient(pClient, &msg);
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, false);
				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
				return;
			}
			/*ServerClientData* pGameServer = GetMaxFreeGameServer();
			if (!pGameServer)
				return;*/

			m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, true);

			GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(pMsg->dwUserID));//�ҵ�һ������ʵ�GameServer
			if (!pConfig)
			{
				ASSERT(false);
				return;
			}
			/*ServerClientData* pGameServer = GetServerServerData(pConfig->NetworkID);
			if (!pGameServer)
			{
				ASSERT(false);
				return;
			}*/
			if (!OnAddUserWriteGameServer(pMsg->dwUserID, pClient->OutsideExtraData))
			{
				ASSERT(false);
				return;
			}
			DWORD OnlyID = RandUInt();
			//1.�������GameServerȥ
			OnSendChannelInfoToGameServer(pMsg->dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

			LG_Cmd_AccountOnlyID msgGameServer;
			SetMsgInfo(msgGameServer,GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
			msgGameServer.dwOnlyID = OnlyID;
			msgGameServer.dwUserID = pMsg->dwUserID;
			msgGameServer.ClientID = pClient->OutsideExtraData;
			msgGameServer.GameServerID = pConfig->NetworkID;
			msgGameServer.LogonTypeID = 1;
			msgGameServer.LogonID = m_LogonConfigID;
			SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);
			return;
		}
		break;
	case DBO_AccountRsg:
		{
			DBO_Cmd_AccountRsg* pMsg = (DBO_Cmd_AccountRsg*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)//����Ѿ������� �޷�������
			{
				ASSERT(false);
				return;
			}
			if (pMsg->dwUserID == 0)
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient,GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = pMsg->dwUserID;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 2;
				SendNewCmdToClient(pClient, &msgClient);

				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, false);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ����˺�������� ���ݿ���ͨע��ʧ��"));
				return;
			}
			m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, true);

			GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(pMsg->dwUserID));
			if (!pConfig)
			{
				ASSERT(false);
				return;
			}
			/*ServerClientData* pGameServer = GetServerServerData(pConfig->NetworkID);
			if (!pGameServer)
			{
				ASSERT(false);
				return;
			}*/
			if (!OnAddUserWriteGameServer(pMsg->dwUserID, pClient->OutsideExtraData))
			{
				ASSERT(false);
				return;
			}
			DWORD OnlyID = RandUInt();
			//1.�������GameServerȥ
			OnSendChannelInfoToGameServer(pMsg->dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

			LG_Cmd_AccountOnlyID msgGameServer;
			SetMsgInfo(msgGameServer,GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
			msgGameServer.dwOnlyID = OnlyID;
			msgGameServer.dwUserID = pMsg->dwUserID;
			msgGameServer.ClientID = pClient->OutsideExtraData;
			msgGameServer.GameServerID = pConfig->NetworkID;
			msgGameServer.LogonTypeID = 2;
			msgGameServer.LogonID = m_LogonConfigID;
			SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);

			return;
		}
		break;
	case DBO_GetNewAccount:
		{
			DBO_Cmd_GetNewAccount* pMsg = (DBO_Cmd_GetNewAccount*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			if (pMsg->Result)
				m_LogonCacheManager.OnAddAccountInfo(pMsg->dwUserID, pMsg->AccountName, pMsg->PasswordCrc1, pMsg->PasswordCrc2, pMsg->PasswordCrc3);//���Ѿ��������˺ű�������
			else
			{
				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺�������� ���ݿ��ȡ���˺�ʧ��"));
			}
			//������ת�����ͻ���ȥ
			LC_Cmd_GetNewAccount msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LC_GetNewAccount), sizeof(LC_Cmd_GetNewAccount));
			msg.Result = pMsg->Result;
			if (msg.Result)
			{
				TCHARCopy(msg.NewAccountName, CountArray(msg.NewAccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				msg.PasswordCrc1 = pMsg->PasswordCrc1;
				msg.PasswordCrc2 = pMsg->PasswordCrc2;
				msg.PasswordCrc3 = pMsg->PasswordCrc3;
			}
			SendNewCmdToClient(pClient, &msg);
			if (pMsg->Result)
			{
				CL_Cmd_AccountLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, CL_AccountLogon), sizeof(CL_Cmd_AccountLogon));
				TCHARCopy(msg.AccountName, CountArray(msg.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				msg.PasswordCrc1 = pMsg->PasswordCrc1;
				msg.PasswordCrc2 = pMsg->PasswordCrc2;
				msg.PasswordCrc3 = pMsg->PasswordCrc3;
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				HandleClientMsg(pClient, &msg);
			}
			return;
		}
		break;
	case DBO_LoadAllAcoountInfo:
		{
			DBO_Cmd_LoadAllAccountInfo* pMsg = (DBO_Cmd_LoadAllAccountInfo*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			m_LogonCacheManager.OnLoadAllAccountDataResult(pMsg);
			return;
		}
		break;
	case DBO_QueryLogon:
		{
			//��ҽ��п��ٵ�½�ķ��ؽ��
			DBO_Cmd_QueryLogon* pMsg = (DBO_Cmd_QueryLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);//�ͻ��� 
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			//�Ѿ���ȡ���˺��� ���ǽ��е�½����
			//1.���˺����ݻ�������
			if (pMsg->dwUserID == 0)
			{
				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 1;
				SendNewCmdToClient(pClient, &msgClient);

				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, false);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ����˺�������� ���ݿ���ٵ�½ʧ��"));
				return;
			}
			else if (pMsg->IsFreeze)
			{
				//��ǰ�˺��Ѿ��������� ��Ҫ����
				LC_Cmd_AccountIsFreeze msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
				msg.EndTime = pMsg->FreezeEndTime;
				SendNewCmdToClient(pClient, &msg);
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, false);
				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
				return;
			}
			m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, true);
			GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(pMsg->dwUserID));//�ҵ�һ������ʵ�GameServer
			if (!pConfig)
			{
				ASSERT(false);
				return;
			}
			/*ServerClientData* pGameServer = GetServerServerData(pConfig->NetworkID);
			if (!pGameServer)
			{
				ASSERT(false);
				return;
			}*/
			if (!OnAddUserWriteGameServer(pMsg->dwUserID, pClient->OutsideExtraData))
			{
				ASSERT(false);
				return;
			}
			DWORD OnlyID = RandUInt();
			//1.�������GameServerȥ
			OnSendChannelInfoToGameServer(pMsg->dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

			LG_Cmd_AccountOnlyID msgGameServer;
			SetMsgInfo(msgGameServer, GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
			msgGameServer.dwOnlyID = OnlyID;
			msgGameServer.dwUserID = pMsg->dwUserID;
			msgGameServer.ClientID = pClient->OutsideExtraData;
			msgGameServer.GameServerID = pConfig->NetworkID;
			msgGameServer.LogonTypeID = 1;
			msgGameServer.LogonID = m_LogonConfigID;
			SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);

			if (pMsg->IsNewRsg)
			{
				LC_Cmd_RsgNewAccount msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_RsgNewAccount), sizeof(LC_Cmd_RsgNewAccount));
				msg.LogonType = pMsg->LogonType;
				SendNewCmdToClient(pClient, &msg);//���߿ͻ��� ��ǰ�˺�����ע���
			}
			return;
							  
		}
		break;
	case DBO_PhoneSecPwd:
		{
			DBO_Cmd_PhoneSecPwd* pMsg = (DBO_Cmd_PhoneSecPwd*)pCmd;
			//��ҽ����ֻ����������½���
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)//����Ѿ������� �޷�������
			{
				ASSERT(false);
				return;
			}
			if (pMsg->dwUserID == 0)
			{
				//��ʾ����ֻ���½ʧ����
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);

				LC_Cmd_AccountOnlyID msgClient;
				SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
				msgClient.dwOnlyID = 0;
				msgClient.dwUserID = 0;
				msgClient.GameIp = 0;
				msgClient.GamePort = 0;
				msgClient.GateIp = 0;
				msgClient.GatePort = 0;
				msgClient.LogonType = 1;
				SendNewCmdToClient(pClient, &msgClient);

				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ֻ����������½����"));
				return;
			}
			else if (pMsg->IsFreeze)
			{
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);

				//��ǰ�˺��Ѿ��������� ��Ҫ����
				LC_Cmd_AccountIsFreeze msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
				msg.EndTime = pMsg->FreezeEndTime;
				SendNewCmdToClient(pClient, &msg);
				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
				return;
			}
			GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(pMsg->dwUserID));//�ҵ�һ������ʵ�GameServer
			if (!pConfig)
			{
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);

				ASSERT(false);
				return;
			}
			if (!OnAddUserWriteGameServer(pMsg->dwUserID, pClient->OutsideExtraData))
			{
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);

				ASSERT(false);
				return;
			}
			DWORD OnlyID = RandUInt();
			//1.�������GameServerȥ
			OnSendChannelInfoToGameServer(pMsg->dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

			m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, true);

			LG_Cmd_AccountOnlyID msgGameServer;
			SetMsgInfo(msgGameServer, GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
			msgGameServer.dwOnlyID = OnlyID;
			msgGameServer.dwUserID = pMsg->dwUserID;
			msgGameServer.ClientID = pClient->OutsideExtraData;
			msgGameServer.GameServerID = pConfig->NetworkID;
			msgGameServer.LogonTypeID = 1;
			msgGameServer.LogonID = m_LogonConfigID;
			SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);

			//�����˺Ž��뻺�� �����˺��Ѿ�ʹ����
			//m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->dwUserID, true);

			return;
		}
		break;
	case DBO_PhoneLogon:
		{
			DBO_Cmd_PhoneLogon* pMsg = (DBO_Cmd_PhoneLogon*)pCmd;
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)//����Ѿ������� �޷�������
			{
				ASSERT(false);
				return;
			}
			//��ҽ��������½�Ľ������
			if (pMsg->dwUserID == 0)
			{
				//�޷���½ UserID Ϊ 0 ��ʾ��½ʧ����
				LC_Cmd_PhoneLogonCheck msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_PhoneLogonCheck), sizeof(LC_Cmd_PhoneLogonCheck));
				msg.dwUserID = 0;
				msg.Result = false;
				msg.ErrorID = PLE_SystemError;
				SendNewCmdToClient(pClient, &msg);
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);
				LogInfoToFile("LogonError.txt", TEXT("�ͻ����ֻ������½ UserIDΪ0 ��½ʧ��"));
				return;
			}
			else if (pMsg->IsFreeze)
			{
				LC_Cmd_SaveAccountInfo msgSave;
				SetMsgInfo(msgSave, GetMsgType(Main_Logon, LC_SaveAccountInfo), sizeof(LC_Cmd_SaveAccountInfo));
				TCHARCopy(msgSave.AccountName, CountArray(msgSave.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				msgSave.PasswordCrc1 = pMsg->PasswordCrc1;
				msgSave.PasswordCrc2 = pMsg->PasswordCrc2;
				msgSave.PasswordCrc3 = pMsg->PasswordCrc3;
				SendNewCmdToClient(pClient, &msgSave);

				//�˺��Ѿ���������
				LC_Cmd_AccountIsFreeze msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
				msg.EndTime = pMsg->FreezeEndTime;
				SendNewCmdToClient(pClient, &msg);
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, false);
				DelClient pDel;
				pDel.LogTime = timeGetTime();
				pDel.SocketID = pClient->OutsideExtraData;
				m_DelSocketVec.push_back(pDel);

				LogInfoToFile("LogonError.txt", TEXT("�ͻ����Ѿ������� ��½ʧ��"));
				return;
			}
			else
			{
				//���˺����ݷ��͵��ͻ���ȥ ��������
				LC_Cmd_SaveAccountInfo msgSave;
				SetMsgInfo(msgSave, GetMsgType(Main_Logon, LC_SaveAccountInfo), sizeof(LC_Cmd_SaveAccountInfo));
				TCHARCopy(msgSave.AccountName, CountArray(msgSave.AccountName), pMsg->AccountName, _tcslen(pMsg->AccountName));
				msgSave.PasswordCrc1 = pMsg->PasswordCrc1;
				msgSave.PasswordCrc2 = pMsg->PasswordCrc2;
				msgSave.PasswordCrc3 = pMsg->PasswordCrc3;
				SendNewCmdToClient(pClient, &msgSave);

				//����UserID ����ҽ���������½
				//����޸������� ���ǽ���ʱ�����ݽ��д���
				m_LogonCacheManager.OnCheckTempAccountInfo(pClient->OutsideExtraData, pMsg->AccountName, pMsg->dwUserID, true);
				//��Ϊ����޸������� ����֪ͨȫ����Logon ����޸�����
				{
					GL_Cmd_ResetPassword msgGL;
					SetMsgInfo(msgGL, GetMsgType(Main_Logon, GL_ResetPassword), sizeof(GL_Cmd_ResetPassword));
					msgGL.dwUserID = pMsg->dwUserID;
					msgGL.PasswordCrc1 = pMsg->PasswordCrc1;
					msgGL.PasswordCrc2 = pMsg->PasswordCrc2;
					msgGL.PasswordCrc3 = pMsg->PasswordCrc3;
					SendNetCmdToOtherLogon(0, &msgGL);
				}

				GameServerConfig* pConfig = g_FishServerConfig.GetGameServerConfig(m_LogonManager.GetGameServerConfigIDByUserID(pMsg->dwUserID));
				if (!pConfig)
				{
					ASSERT(false);
					return;
				}
				if (!OnAddUserWriteGameServer(pMsg->dwUserID, pClient->OutsideExtraData))
				{
					ASSERT(false);
					return;
				}
				DWORD OnlyID = RandUInt();
				//1.�������GameServerȥ
				OnSendChannelInfoToGameServer(pMsg->dwUserID, pClient->OutsideExtraData, pConfig->NetworkID);

				LG_Cmd_AccountOnlyID msgGameServer;
				SetMsgInfo(msgGameServer, GetMsgType(Main_Logon, LG_AccountOnlyID), sizeof(LG_Cmd_AccountOnlyID));
				msgGameServer.dwOnlyID = OnlyID;
				msgGameServer.dwUserID = pMsg->dwUserID;
				msgGameServer.ClientID = pClient->OutsideExtraData;
				msgGameServer.GameServerID = pConfig->NetworkID;
				msgGameServer.LogonTypeID = 2;
				msgGameServer.LogonID = m_LogonConfigID;
				SendNetCmdToGameServer(pConfig->NetworkID, &msgGameServer);

				return;
			}
		}
		break;
	}
}
void FishServer::HandleCenterMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return;
	//�����������������������
	if (pCmd->CmdType == Main_Server)
	{
		switch (pCmd->SubCmdType)
		{
		case CL_RsgLogon:
			{
				CL_Cmd_RsgLogon* pMsg = (CL_Cmd_RsgLogon*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				m_LogonManager.OnLogonRsgResult(pMsg);
				return;
			}
			break;
		case CL_UnRsgGame:
			{
				CL_Cmd_UnRsgGame* pMsg = (CL_Cmd_UnRsgGame*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				/*	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ServerList.find(pMsg->GameConfigID);
				if (Iter != m_ServerList.end())
				{
					Iter->second->Removed = true;
				}*/
				m_LogonManager.OnGameLeaveCenter(pMsg->GameConfigID);
				return;
			}
			break;
		case CL_RsgUser:
			{
				CL_Cmd_RsgUser* pMsg = (CL_Cmd_RsgUser*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				m_LogonManager.OnUserJoinCenter(pMsg->PlayerInfo.dwUserID, pMsg->PlayerInfo.GameConfigID);
				return;
			}
			break;
		case CL_UnRsgUser:
			{
				CL_Cmd_UnRsgUser* pMsg = (CL_Cmd_UnRsgUser*)pCmd;
				if (!pMsg)
				{
					ASSERT(false);
					return;
				}
				m_LogonManager.OnUserLeaveCenter(pMsg->dwUserID);
				return;
			}
			break;
		case CL_GameInfo:
			{
				CL_Cmd_GameInfo* pMsg = (CL_Cmd_GameInfo*)pCmd;
				for (WORD i = 0; i < pMsg->Sum; ++i)
				{
					m_LogonManager.OnGameRsgLogon(pMsg->Array[i]);
				}
				return;
			}
		}
	}
	HandleGameServerMsg(pCmd);//��������ת����GameServer������
	return;
}
void FishServer::CheckDelSocket(DWORD dwTimer)
{
	if (m_DelSocketVec.empty())
		return;
	vector<DelClient>::iterator Iter = m_DelSocketVec.begin();
	for (; Iter != m_DelSocketVec.end();)
	{
		if (dwTimer - Iter->LogTime >= 50000)
		{
			CloseClientSocket(Iter->SocketID);
			Iter = m_DelSocketVec.erase(Iter);
		}
		else
			++Iter;
	}
}
void FishServer::CloseClientSocket(DWORD SocketID)
{
	ServerClientData* pClient = GetClientServerData(SocketID);
	if (pClient)
		pClient->Removed = true;
	else
	{
		ASSERT(false);
		return;
	}
}
void FishServer::HandleOperateMsg(NetCmd* pCmd)
{
	//������Ӫ����������������Ϣ
	if (!pCmd || pCmd->CmdType != Main_Logon)
	{
		ASSERT(false);
		return;
	}
	switch (pCmd->SubCmdType)
	{
	case OL_WeiXinLogon:
		{
			OL_Cmd_WeiXinLogon* pMsg = (OL_Cmd_WeiXinLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData * pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			if (pMsg->Result)
			{
				//�������������� ΪWeiXin
				HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pMsg->ClientID);
				if (Iter != m_UserChannelMap.end())
				{
					ASSERT(false);
					m_UserChannelMap.erase(Iter);
				}
				std::vector<char*> pVec;
				GetStringArrayVecByData(pVec, &pMsg->OpenInfo);
				if (pVec.size() != pMsg->OpenInfo.HandleSum)
				{
					ASSERT(false);
					return;
				}
				string OpenID = pVec[0];
				FreeVec(pVec);

				//��������������ӽ�ȥ

				//ת��ΪTCHAR
				UINT Count = 0;
				TCHAR* pOpenID=CharToWChar(OpenID.c_str(), Count);


				CL_Cmd_QueryLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, CL_QueryLogon), sizeof(CL_Cmd_QueryLogon));
				//1.�˺�����
				_stprintf_s(msg.AccountName, CountArray(msg.AccountName), TEXT("WeiXin%s"), pOpenID);
				TCHAR Password[32];
				int length = _stprintf_s(Password, CountArray(Password), TEXT("WeiXinPass%s"),pOpenID);
				for (int i = length + 1; i < 32; ++i)
					Password[i] = 0xfefe;

				free(pOpenID);

				AE_CRC_THREE pTree;
				AECrc32(pTree, Password, CountArray(Password) * sizeof(TCHAR), 0, 0x735739, 0x79387731);
				msg.PasswordCrc1 = pTree.Crc1;
				msg.PasswordCrc2 = pTree.Crc2;
				msg.PasswordCrc3 = pTree.Crc3;
				msg.PlateFormID = pMsg->PlateFormID;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				msg.LogonType = 2;
				HandleClientMsg(pClient, &msg);
			}
			else
			{
				//�����������Ľ�����͵��ͻ���ȥ
				LC_Cmd_WeiXinLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_WeiXinLogon), sizeof(LC_Cmd_WeiXinLogon));
				msg.Result = pMsg->Result;
				SendNewCmdToClient(pClient, &msg);
				LogInfoToFile("LogonError.txt", TEXT("�ͻ���΢�ŵ�½ʧ��"));
			}
			return;
		}
		break;
	case OL_QQLogon:
		{
			OL_Cmd_QQLogon* pMsg = (OL_Cmd_QQLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData * pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			if (pMsg->Result)
			{
				//�������������� ΪQQ
				HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pMsg->ClientID);
				if (Iter != m_UserChannelMap.end())
				{
					ASSERT(false);
					m_UserChannelMap.erase(Iter);
				}
				std::vector<char*> pVec;
				GetStringArrayVecByData(pVec, &pMsg->OpenInfo);
				if (pVec.size() != pMsg->OpenInfo.HandleSum)
				{
					ASSERT(false);
					return;
				}
				string OpenID = pVec[0];
				FreeVec(pVec);

				//��������������ӽ�ȥ

				//ת��ΪTCHAR
				UINT Count = 0;
				TCHAR* pOpenID=CharToWChar(OpenID.c_str(), Count);


				CL_Cmd_QueryLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, CL_QueryLogon), sizeof(CL_Cmd_QueryLogon));
				//1.�˺�����
				_stprintf_s(msg.AccountName, CountArray(msg.AccountName), TEXT("QQ%s"), pOpenID);
				TCHAR Password[32];
				int length = _stprintf_s(Password, CountArray(Password), TEXT("QQPass%s"),pOpenID);
				for (int i = length + 1; i < 32; ++i)
					Password[i] = 0xfefe;

				free(pOpenID);

				AE_CRC_THREE pTree;
				AECrc32(pTree, Password, CountArray(Password) * sizeof(TCHAR), 0, 0x735739, 0x79387731);
				msg.PasswordCrc1 = pTree.Crc1;
				msg.PasswordCrc2 = pTree.Crc2;
				msg.PasswordCrc3 = pTree.Crc3;
				msg.PlateFormID = pMsg->PlateFormID;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				msg.LogonType = 3;
				HandleClientMsg(pClient, &msg);

			}
			else
			{
				//�����������Ľ�����͵��ͻ���ȥ
				LC_Cmd_QQLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_QQLogon), sizeof(LC_Cmd_QQLogon));
				msg.Result = pMsg->Result;
				SendNewCmdToClient(pClient, &msg);
				LogInfoToFile("LogonError.txt", TEXT("�ͻ���QQ��½ʧ��"));
			}
			return;
		}
		break;
	case OL_ChannelLogon:
		{
			//g_FishServer.ShowInfoToWin("���յ���ҽ��������������1");
			OL_Cmd_ChannelLogon* pMsg = (OL_Cmd_ChannelLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData * pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			//g_FishServer.ShowInfoToWin("���յ���ҽ��������������2");
			//���յ����� ���ǽ��д���
			if (pMsg->Result)
			{
				OnAddChannelInfo(pMsg->ClientID, &pMsg->channelInfo);//��ӵ���������ȥ

				HashMap<DWORD, DWORD>::iterator Iter = m_UserChannelMap.find(pMsg->ClientID);
				if (Iter != m_UserChannelMap.end())
				{
					ASSERT(false);
					m_UserChannelMap.erase(Iter);
				}
				m_UserChannelMap.insert(HashMap<DWORD, DWORD>::value_type(pMsg->ClientID, pMsg->ChannelID));

				std::vector<StringArrayData*> pVec;
				GetStringArrayVecByData(pVec, &pMsg->channelInfo);
				if (pVec.size() != pMsg->channelInfo.HandleSum)
				{
					ASSERT(false);
					return;
				}
				//����һ���˺���Ϣ  Ȼ����п��ٵ�½(���˺ž͵�½ ���˺� ע���˺ŵ�½ )
				CL_Cmd_QueryLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, CL_QueryLogon), sizeof(CL_Cmd_QueryLogon));
				
				//1.�˺�����
				DWORD uidCrc32 = GetCrc32(pVec[0]->Array, pVec[0]->Length/sizeof(TCHAR));
				DWORD channelLabelCrc32 = GetCrc32(pVec[1]->Array, pVec[1]->Length / sizeof(TCHAR));//�����ܱ仯��
				DWORD channeluidCrc32 = GetCrc32(pVec[2]->Array, pVec[2]->Length / sizeof(TCHAR));//�����ܱ仯��
				_stprintf_s(msg.AccountName, CountArray(msg.AccountName), TEXT("ChannelUser%u%u"), uidCrc32,channelLabelCrc32);
				TCHAR Password[32];
				int length = _stprintf_s(Password, CountArray(Password), TEXT("Pass%u%u"), uidCrc32, channeluidCrc32);
				for (int i = length+1; i < 32; ++i)
					Password[i] = 0xfefe;

				AE_CRC_THREE pTree;
				AECrc32(pTree, Password, CountArray(Password) * sizeof(TCHAR), 0, 0x735739, 0x79387731);
				msg.PasswordCrc1 = pTree.Crc1;
				msg.PasswordCrc2 = pTree.Crc2;
				msg.PasswordCrc3 = pTree.Crc3;
				msg.PlateFormID = pMsg->PlateFormID;
				msg.VersionID = m_FishConfig.GetSystemConfig().VersionID;
				msg.PathCrc = m_FishConfig.GetSystemConfig().PathCrc;
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				msg.LogonType = 1;
				HandleClientMsg(pClient, &msg);

				//Log(TEXT("������¼ �û���:%s Password:%s  %u  %u  %u"), msg.AccountName, Password, msg.PasswordCrc1, msg.PasswordCrc2, msg.PasswordCrc3);

				/*vector<TCHAR*>::iterator Iter = pVec.begin();
				for (; Iter != pVec.end(); ++Iter)
				{
					free(*Iter);
				}
				pVec.clear();*/

				//g_FishServer.ShowInfoToWin("���յ���ҽ��������������4");
			}
			else
			{
				//�����������Ľ�����͵��ͻ���ȥ
				LC_Cmd_ChannelLogon msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_ChannelLogon), sizeof(LC_Cmd_ChannelLogon));
				msg.Result = pMsg->Result;
				SendNewCmdToClient(pClient, &msg);

				LogInfoToFile("LogonError.txt",TEXT("�ͻ���������½ʧ��"));

				//g_FishServer.ShowInfoToWin("���յ���ҽ��������������5");
			}
			return;
		}
		break;
	case OL_PhoneLogon:
		{
			OL_Cmd_PhoneLogon* pMsg = (OL_Cmd_PhoneLogon*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			//��ҽ��л�ȡ�ֻ���½����Ľ��
			LC_Cmd_PhoneLogon msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LC_PhoneLogon), sizeof(LC_Cmd_PhoneLogon));
			msg.ErrorID = pMsg->ErrorID;
			msg.PhoneNumber = pMsg->PhoneNumber;
			SendNewCmdToClient(pClient, &msg);
			return;
		}
		break;
	case OL_PhoneLogonCheck:
		{
			OL_Cmd_PhoneLogonCheck* pMsg = (OL_Cmd_PhoneLogonCheck*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return;
			}
			ServerClientData* pClient = GetClientServerData(pMsg->ClientID);
			if (!pClient)
			{
				ASSERT(false);
				return;
			}
			if (pMsg->Result)
			{
				//��������ݼ��뵽����ȥ
				TempAccountCacheInfo pData;
				pData.ClientID = pMsg->ClientID;
				_i64tot_s(pMsg->PhoneNumber, pData.AccountInfo.AccountName, CountArray(pData.AccountInfo.AccountName), 10);
				TCHARCopy(pData.AccountInfo.MacAddress, CountArray(pData.AccountInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				pData.AccountInfo.PasswordCrc1 = pMsg->Crc1;
				pData.AccountInfo.PasswordCrc2 = pMsg->Crc2;
				pData.AccountInfo.PasswordCrc3 = pMsg->Crc3;
				m_LogonCacheManager.OnAddTempAccountInfo(pData);

				DBR_Cmd_PhoneLogon msg;
				SetMsgInfo(msg, DBR_PhoneLogon, sizeof(DBR_Cmd_PhoneLogon));
				msg.ClientID = pMsg->ClientID;
				msg.PhoneNumber = pMsg->PhoneNumber;
				msg.ClientIP = pMsg->ClientIP;
				msg.PasswordCrc1 = pMsg->Crc1;
				msg.PasswordCrc2 = pMsg->Crc2;
				msg.PasswordCrc3 = pMsg->Crc3;
				msg.IsFirOrSec = pMsg->IsFirOrSec;
				TCHARCopy(msg.MacAddress, CountArray(msg.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
				SendNetCmdToDB(&msg);
				return;
			}
			else
			{
				LC_Cmd_PhoneLogonCheck msg;
				SetMsgInfo(msg, GetMsgType(Main_Logon, LC_PhoneLogonCheck), sizeof(LC_Cmd_PhoneLogonCheck));
				msg.dwUserID = 0;
				msg.ErrorID = pMsg->ErrorID;
				msg.Result = pMsg->Result;
				SendNewCmdToClient(pClient, &msg);
				return;
			}
		}
		break;
	}
}
void FishServer::OnAddChannelInfo(DWORD ClientID, ChannelUserInfo* pInfo)
{
	HashMap<DWORD, ChannelUserInfo*>::iterator Iter = m_ChannelInfo.find(ClientID);
	if (Iter != m_ChannelInfo.end())
	{
		free(Iter->second);
		m_ChannelInfo.erase(Iter);
	}
	//�������
	DWORD InfoSize = sizeof(ChannelUserInfo)+sizeof(BYTE)*(pInfo->Sum - 1);
	ChannelUserInfo* pNewInfo = (ChannelUserInfo*)malloc(InfoSize);
	memcpy_s(pNewInfo, InfoSize, pInfo, InfoSize);
	m_ChannelInfo.insert(HashMap<DWORD, ChannelUserInfo*>::value_type(ClientID, pNewInfo));
}
void FishServer::OnDelChannelInfo(DWORD ClientID)
{
	HashMap<DWORD, ChannelUserInfo*>::iterator Iter = m_ChannelInfo.find(ClientID);
	if (Iter != m_ChannelInfo.end())
	{
		free(Iter->second);
		m_ChannelInfo.erase(Iter);
	}
}
void FishServer::OnSendChannelInfoToGameServer(DWORD dwUserID, DWORD ClientID, BYTE GameServerID)
{
	/*ServerClientData* pServer = GetServerServerData(GameServerID);
	if (!pServer)
		return;*/
	HashMap<DWORD, ChannelUserInfo*>::iterator Iter = m_ChannelInfo.find(ClientID);
	if (Iter != m_ChannelInfo.end())
	{
		//���ҵ��� ���з���
		DWORD PageSize = sizeof(LG_Cmd_ChannelInfo)+sizeof(BYTE)*(Iter->second->Sum - 1);
		DWORD InfoSize = sizeof(ChannelUserInfo)+sizeof(BYTE)*(Iter->second->Sum - 1);
		LG_Cmd_ChannelInfo * msg = (LG_Cmd_ChannelInfo*)malloc(PageSize);
		CheckMsgSize(PageSize);
		msg->SetCmdSize(static_cast<WORD>(PageSize));
		msg->SetCmdType(GetMsgType(Main_Logon, LG_ChannelInfo));
		msg->ClientID = ClientID;
		msg->dwUserID = dwUserID;
		memcpy_s(&msg->channelUserInfo, InfoSize, Iter->second, InfoSize);
		SendNetCmdToGameServer(GameServerID, msg);
		free(msg);
		free(Iter->second);
		m_ChannelInfo.erase(Iter);
	}
}
void FishServer::OnClearAllChannelInfo()
{
	if (m_ChannelInfo.empty())
		return;
	HashMap<DWORD, ChannelUserInfo*>::iterator Iter = m_ChannelInfo.begin();
	for (; Iter != m_ChannelInfo.end(); ++Iter)
	{
		free(Iter->second);
	}
	m_ChannelInfo.clear();
}
bool FishServer::OnAddUserWriteGameServer(DWORD dwUserID,DWORD ClientID)
{
	HashMap<DWORD, WriteGameServerUserInfo>::iterator Iter = m_SendGameUserMap.find(dwUserID);
	if (Iter == m_SendGameUserMap.end())
	{
		WriteGameServerUserInfo pOnce;
		pOnce.dwUserID = dwUserID;
		pOnce.ClientID = ClientID;
		pOnce.LogTime = timeGetTime();
		m_SendGameUserMap.insert(HashMap<DWORD, WriteGameServerUserInfo>::value_type(dwUserID, pOnce));
		return true;
	}
	else
	{
		ASSERT(false);
		return false;
	}
}
bool FishServer::OnCheckUserWriteGameServerIsExists(DWORD dwUserID)
{
	HashMap<DWORD, WriteGameServerUserInfo>::iterator Iter = m_SendGameUserMap.find(dwUserID);
	if (Iter == m_SendGameUserMap.end())
	{
		ASSERT(false);
		return false;
	}
	else
	{
		m_SendGameUserMap.erase(Iter);
		return true;
	}
}
void FishServer::OnUpdateWriteGameServerUserMap(DWORD dwTimer)
{
	HashMap<DWORD, WriteGameServerUserInfo>::iterator Iter = m_SendGameUserMap.begin();
	for (; Iter != m_SendGameUserMap.end();)
	{
		//10s
		if (dwTimer - Iter->second.LogTime >= 10000)
		{
			//��������ͻ���ȥ ������� ��½ʧ����
			ServerClientData* pClient = GetClientServerData(Iter->second.ClientID);
			if (!pClient)
			{
				ASSERT(false);
				Iter = m_SendGameUserMap.erase(Iter);
				continue;
			}
			LC_Cmd_AccountOnlyID msgClient;
			SetMsgInfo(msgClient, GetMsgType(Main_Logon, LC_AccountOnlyID), sizeof(LC_Cmd_AccountOnlyID));
			msgClient.dwOnlyID = 0;
			msgClient.dwUserID = 0;
			msgClient.GameIp = 0;
			msgClient.GamePort = 0;
			msgClient.GateIp = 0;
			msgClient.GatePort = 0;
			msgClient.LogonType = 1;
			SendNewCmdToClient(pClient, &msgClient);

			DelClient pDel;
			pDel.LogTime = timeGetTime();
			pDel.SocketID = pClient->OutsideExtraData;
			m_DelSocketVec.push_back(pDel);

			LogInfoToFile("LogonError.txt",TEXT("�ͻ�������GameServer��ʱ ��½ʧ��"));

			Iter = m_SendGameUserMap.erase(Iter);
		}
		else
		{
			++Iter;
			continue;
		}	
	}
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

void FishServer::SendNetCmdToControl(NetCmd*pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	if (!m_ControlTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
bool FishServer::ConnectControl()
{
	//�Կ��Ʒ������Ķ˿� �����ⲿ���ӵ���������������е�½���� Я��DWORD ��ΨһID
	ControlServerConfig* pConfig = g_FishServerConfig.GetControlServerConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return false;
	}
	ClientInitData pControlData;
	pControlData.BuffSize = pConfig->ControlServerBuffSize;
	pControlData.Port = pConfig->ControlServerListenPort;
	pControlData.SleepTime = pConfig->ControlServerSleepTime;
	pControlData.SocketRecvSize = pConfig->ControlServerRecvBuffSize;
	pControlData.SocketSendSize = pConfig->ControlServerSendBuffSize;
	pControlData.ThreadNum = 1;
	pControlData.Timeout = pConfig->ControlServerTimeOut;
	pControlData.CmdHearbeat = 0;
	pControlData.SendArraySize = pConfig->ControlServerMaxSendCmdCount;
	pControlData.RecvArraySize = pConfig->ControlServerMaxRecvCmdCount;

	if (!m_ControlTcp.Init(pConfig->ControlServerListenIP, pControlData, (void*)&m_LogonConfigID, sizeof(BYTE)))
	{
		ShowInfoToWin("ControlServer����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_ControlTcp);
	return true;
}
bool FishServer::HandleControlMsg(NetCmd* pCmd)
{
	//������Ӫ������������������
	if (!pCmd)
		return false;
	if (pCmd->CmdType != Main_Control)
	{
		ASSERT(false);
		return false;
	}
	switch (pCmd->SubCmdType)
	{
	case CL_ReloadConfig:
		{
			OnReloadConfig();
			return true;
		}
		break;
	case CL_ResetRolePassword:
		{
			CL_Cmd_ResetRolePassword* pMsg = (CL_Cmd_ResetRolePassword*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			m_LogonCacheManager.OnChangeAccountPassword(pMsg->dwUserID, pMsg->Password1, pMsg->Password2, pMsg->Password3);
			return true;
		}
		break;
	//case CL_FreezeAccount:
	//	{
	//		CL_Cmd_FreezeAccount* pMsg = (CL_Cmd_FreezeAccount*)pCmd;
	//		if (!pMsg)
	//		{
	//			ASSERT(false);
	//			return false;
	//		}
	//		//�޸�ָ����ҵĶ���ʱ��
	//		time_t FreezeEndTime = time(null) + pMsg->FreezeMin * 60;
	//		m_LogonCacheManager.OnChangeAccountFreezeInfo(pMsg->dwUserID, FreezeEndTime);
	//		return true;
	//	}
	//	break;
	case CL_KickUserByID:
		{
			CL_Cmd_KickUserByID* pMsg = (CL_Cmd_KickUserByID*)pCmd;
			time_t FreezeEndTime = time(null) + pMsg->FreezeMin * 60;
			m_LogonCacheManager.OnChangeAccountFreezeInfo(pMsg->dwUserID, FreezeEndTime);
			return true;
		}
		break;
	}
	return true;
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
void FishServer::UpdateInfoToControl(DWORD dwTimer)
{
	static DWORD LogUpdateCenterTime = 0;
	if (LogUpdateCenterTime == 0 || dwTimer - LogUpdateCenterTime >= 10000)
	{
		LogUpdateCenterTime = dwTimer;
		LC_Cmd_LogonInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_Control, LC_LogonInfo), sizeof(LC_Cmd_LogonInfo));
		msg.LogonID = m_LogonConfigID;
		msg.LogonTablePlayerSum = m_LogonManager.LogonRoleSum();
		SendNetCmdToControl(&msg);
	}
}