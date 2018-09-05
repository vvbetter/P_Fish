#include "stdafx.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
FishServer g_FishServer;
FishServer::FishServer()
{
	m_IsClose = false;
	m_DBIsConnect = false;
	//m_ClientIndex = 0;
	m_RankNetworkID = 0;
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
bool FishServer::InitServer(bool IsNeedCreateRankXml)
{
	m_pDump.OnInit();
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
	

	//if (!ConnectFTP() || !ConnectDB() /*|| !ConnectClient()*/)
	//{
	//	ASSERT(false);
	//	return false;
	//}
	RankServerConfig* pRankConfig = g_FishServerConfig.GetRankServerConfig();
	if (!pRankConfig)
	{
		return false;
	}
	m_RankNetworkID = pRankConfig->NetworkID;

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
	if (!ConnectLogDB())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectLogDB())
				break;
		}
	}
	if (!ConnectFTP())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectFTP())
				break;
		}
	}
	

	m_RankManager.OnInit(IsNeedCreateRankXml);//��ʼ�����а�
	ShowInfoToWin("���а��������ʼ���ɹ�");

	return true;
}
bool FishServer::ConnectLogDB()
{
	RankServerConfig* pRankServerConfig = g_FishServerConfig.GetRankServerConfig();
	if (!pRankServerConfig)
	{
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pRankServerConfig->LogDBID);
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
	if (!m_LogDBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_RankNetworkID, sizeof(BYTE)))
	{
		ShowInfoToWin("Log���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_LogDBTcp);
	return true;
}
bool FishServer::ConnectFTP()
{
	FTPServerConfig* pFtpConfig = g_FishServerConfig.GetFTPServerConfig();
	if (!pFtpConfig)
	{
		return false;
	}
	ClientInitData pFtpData;
	pFtpData.BuffSize = pFtpConfig->BuffSize;
	pFtpData.Port = pFtpConfig->FTPListenPort;
	pFtpData.SleepTime = pFtpConfig->SleepTime ;
	pFtpData.SocketRecvSize = pFtpConfig->RecvBuffSize;
	pFtpData.SocketSendSize = pFtpConfig->SendBuffSize;
	pFtpData.ThreadNum = 1;
	pFtpData.Timeout = pFtpConfig->TimeOut;
	pFtpData.CmdHearbeat = 0;
	pFtpData.SendArraySize = pFtpConfig->MaxSendCmdCount;
	pFtpData.RecvArraySize = pFtpConfig->MaxRecvCmdCount;
	if (!m_FtpTcp.Init(pFtpConfig->FTPListenIP, pFtpData, (void*)&m_RankNetworkID, sizeof(BYTE)))
	{
		ShowInfoToWin("FtpServer����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_FtpTcp);
	return true;
}
bool FishServer::ConnectDB()
{
	RankServerConfig* pRankConfig = g_FishServerConfig.GetRankServerConfig();
	if (!pRankConfig)
	{
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pRankConfig->DBNetworkID);
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
	if (!m_DBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_RankNetworkID, sizeof(BYTE)))
	{
		ShowInfoToWin("���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
//bool FishServer::ConnectClient()
//{
//	RankServerConfig* pRankConfig = g_FishServerConfig.GetRankServerConfig();
//	if (!pRankConfig)
//	{
//		return false;
//	}
//	ServerInitData pCLientData;
//	pCLientData.ServerID = pRankConfig->NetworkID;
//	pCLientData.BuffSize = pRankConfig->BuffSize;
//	pCLientData.CmdHearbeat = 0;
//	pCLientData.ListenCount = 16;
//	pCLientData.MaxRecvCmdCount = pRankConfig->RecvMaxSize;
//	pCLientData.MaxSendCountPerFrame = pRankConfig->SendMaxSize;
//	pCLientData.Port = pRankConfig->RankListenPort;
//	pCLientData.SocketRecvSize = pRankConfig->RecvBuffSize;
//	pCLientData.SocketSendSize = pRankConfig->SendBuffSize;
//	pCLientData.Timeout = pRankConfig->TimeOut;
//	pCLientData.RecvThreadNum = pRankConfig->RecvThreadSum;
//	pCLientData.SendThreadNum = pRankConfig->SendThreadSum;
//	pCLientData.SleepTime = pRankConfig->SleepTime;
//	pCLientData.AcceptSleepTime = pRankConfig->AcceptSleepTime;
//	pCLientData.MaxAcceptNumPerFrame = 16;
//	if (!m_ClientTcp.Init(pCLientData))
//	{
//		ShowInfoToWin("�������������ʧ��");
//		return false;
//	}
//	m_ClientTcp.SetCmdHandler(this);
//	m_RankNetworkID = pRankConfig->NetworkID;
//	return true;
//}
//ServerClientData* FishServer::GetUserClientDataByIndex(DWORD IndexID)
//{
//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.find(IndexID);
//	if (Iter == m_ClintList.end())
//		return NULL;
//	else
//		return Iter->second;
//}
bool FishServer::OnDestroy()
{
	HandleAllMsg();
	return true;
}
//bool FishServer::CanConnected(BYTE SeverID, uint ip, short port)
//{
//	//if (SeverID == m_RankNetworkID)
//	//{
//	//	return m_DBTcp.IsConnected();//�ͻ���ֻ���ڷ������������ݿ�ɹ���ſ��Խ���ע��
//	//}
//	return true;
//}
//bool FishServer::NewClient(BYTE SeverID, ServerClientData *pClient)
//{
//	if (SeverID == m_RankNetworkID)
//	{
//		pClient->OutsideExtraData = ++m_ClientIndex;
//		m_ClintList.insert(HashMap<DWORD, ServerClientData*>::value_type(pClient->OutsideExtraData, pClient));
//		//������Ҽ�����¼�
//		OnTcpServerJoin(SeverID, pClient);
//		return true;
//	}
//	else
//		return false;
//}
//void FishServer::OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient)
//{
//	//�����������һ���ͻ����뿪��ʱ��
//	if (ServerID == m_RankNetworkID)
//	{
//		ShowInfoToWin("һ��GameServer�뿪��RanServer");
//	}
//	return;
//}
//void FishServer::OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient)
//{
//	//�����������һ���ͻ��˼����ʱ��
//	if (ServerID == m_RankNetworkID)
//	{
//		ShowInfoToWin("һ��GameServer������RanServer");
//	}
//	return;
//}
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
		return;
	}
	else if (pClient == &m_FtpTcp)
	{
		m_FtpIsConnect = true;
		ShowInfoToWin("�Ѿ���Ftp���������ӳɹ���");
		return;
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
		return;
	}
	else if (pClient == &m_FtpTcp)
	{
		m_FtpIsConnect = false;
		m_FtpTcp.Shutdown();
		ShowInfoToWin("�Ѿ���Ftp�������Ͽ�������");
		return;
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
}
void FishServer::SendNetCmdToDB(NetCmd* pCmd)
{
	if (!pCmd)
		return;
	if (!m_DBTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
//void FishServer::SendNetCmdToGameServer(ServerClientData* pClient, NetCmd* pCmd)
//{
//	if (!pCmd || !pClient)
//	{
//		ASSERT(false);
//		return;
//	}
//	m_ClientTcp.Send(pClient, pCmd);
//}
//void FishServer::SendNetCmdToAllGameServer(NetCmd* pCmd)
//{
//	if (!pCmd || m_ClintList.empty())
//		return;
//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
//	for (; Iter != m_ClintList.end(); ++Iter)
//	{
//		m_ClientTcp.Send(Iter->second, pCmd);
//	}
//}
void FishServer::SendNetCmdToFtpServer(NetCmd* pCmd)
{
	if (!pCmd)
		return;
	if (!m_FtpTcp.Send(pCmd, false))
	{
		ASSERT(false);
	}
}
void FishServer::HandleAllMsg()
{
	//if (!m_ClintList.empty())
	//{
	//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
	//	for (; Iter != m_ClintList.end();)
	//	{
	//		while (Iter->second->RecvList.HasItem())
	//		{
	//			NetCmd* pCmd = Iter->second->RecvList.GetItem();
	//			//������������ �ͻ��˷������ĵ�½ ע�� ������
	//			HandleGameServerMsg(Iter->second, pCmd);
	//			free(pCmd);
	//		}
	//		++Iter;
	//	}
	//}

	NetCmd* pCmd = m_FtpTcp.GetCmd();
	while (pCmd)
	{
		HandleFtpMsg(pCmd);
		free(pCmd);
		pCmd = m_FtpTcp.GetCmd();
	}
	
	pCmd = m_DBTcp.GetCmd();
	while (pCmd)
	{
		HandleDataBaseMsg(pCmd);
		free(pCmd);
		pCmd = m_DBTcp.GetCmd();
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
		UpdateByMin(dwTimer);
		//1.����GameServer ������
		//if (!m_ClintList.empty())
		//{
		//	HashMap<DWORD, ServerClientData*>::iterator Iter = m_ClintList.begin();
		//	for (; Iter != m_ClintList.end();)
		//	{
		//		if (Iter->second->Removed)
		//		{
		//			OnTcpServerLeave(m_RankNetworkID, Iter->second);
		//			m_ClientTcp.Kick(Iter->second);
		//			Iter = m_ClintList.erase(Iter);
		//			continue;
		//		}
		//		int Sum = 0;
		//		while (Iter->second->RecvList.HasItem() && Sum < Msg_OnceSum)
		//		{
		//			NetCmd* pCmd = Iter->second->RecvList.GetItem();
		//			//������������ �ͻ��˷������ĵ�½ ע�� ������
		//			HandleGameServerMsg(Iter->second, pCmd);
		//			free(pCmd);
		//			++Sum;
		//		}
		//		++Iter;
		//	}
		//}
		UpdateInfoToControl(dwTimer);
		//2.����DB������
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
			
		}
		else
		{
			if (m_DBIsConnect)
				OnTcpClientLeave(&m_DBTcp);

			//��������DB
			ConnectDB();
		}

		//3.����FTP������
		if (m_FtpTcp.IsConnected())
		{
			if (!m_FtpIsConnect)
				OnTcpClientConnect(&m_FtpTcp);
			NetCmd* pCmd = m_FtpTcp.GetCmd();
			int Sum = 0;
			while (pCmd && Sum < Msg_OnceSum)
			{
				HandleFtpMsg(pCmd);
				free(pCmd);
				++Sum;
				pCmd = m_FtpTcp.GetCmd();
			}
		}
		else
		{
			if (m_FtpIsConnect)
				OnTcpClientLeave(&m_FtpTcp);

			ConnectFTP();
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
void FishServer::UpdateByMin(DWORD dwTimer)
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

	//ÿ��4����и��´���
	//��ǰ10���ӽ��б��� 3:50���� �������а��ļ� 
	time_t WriteTime = time(NULL);
	WriteTime += Diff_Update_Sec;//���10���� 3:50+10  ->4:00
	tm pWriteTime;
	Error = localtime_s(&pWriteTime, &WriteTime);
	if (Error != 0)
	{
		ASSERT(false);
		return;
	}
	bool IsServerBeginNewDay = ((pWriteTime.tm_hour == GetFishConfig().GetFishUpdateConfig().UpdateHour) && (pWriteTime.tm_min == GetFishConfig().GetFishUpdateConfig().UpdateMin));
	if (IsServerBeginNewDay)
		m_RankManager.OnDayChange();//�����������仯��ʱ��
}
//bool FishServer::HandleGameServerMsg(ServerClientData* pClient, NetCmd* pCmd)
//{
//	if (!pClient || !pCmd)
//		return false;
//	if (pCmd->CmdType == Main_Rank)
//	{
//		switch (pCmd->SubCmdType)
//		{
//			case LR_GetRankReward:
//			{
//				LR_Cmd_GetRankReward * pMsg = CHECKMSG(pCmd, pCmd->CmdSize, LR_Cmd_GetRankReward);
//				if (!pMsg)
//				{
//					ASSERT(false);
//					return false;
//				}
//				m_RankManager.OnRoleGetWeekRankReward(pMsg->dwUserID, pMsg->RankID, pClient->OutsideExtraData);
//				return true;
//			}
//			case LR_GetRankInfo:
//			{
//				LR_Cmd_GetRankInfo* pMsg = CHECKMSG(pCmd, pCmd->CmdSize, LR_Cmd_GetRankInfo);
//				if (!pMsg)
//				{
//					ASSERT(false);
//					return false;
//				}
//				m_RankManager.OnRoleGetRankInfo(pMsg->dwUserID, pClient->OutsideExtraData);
//				return true;
//			}
//		}
//	}
//	return true;
//}
bool FishServer::HandleDataBaseMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	switch (pCmd->GetCmdType())
	{
	case DBO_LoadRankInfo:
		return HandleDataBaseLoadRankInfo(pCmd);
	/*case DBO_LoadWeekRankInfo:
		{
			DBO_Cmd_LoadWeekRankInfo* pMsg = (DBO_Cmd_LoadWeekRankInfo*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			m_RankManager.OnLoadWeekRankInfoResult(pMsg);
			return true;
		}
		break;*/
	}
	return true;
}
bool FishServer::HandleDataBaseLoadRankInfo(NetCmd* pCmd)
{
	DBO_Cmd_LoadRankInfo* pMsg = (DBO_Cmd_LoadRankInfo*)pCmd;
	if (!pMsg)
	{
		ASSERT(false);
		return false;
	}
	m_RankManager.OnLoadRankInfoResult(pMsg);
	return true;
}
bool FishServer::HandleFtpMsg(NetCmd* pCmd)
{
	if (!pCmd)
		return false;
	//��������FTP������ ���д���
	return true;
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

	if (!m_ControlTcp.Init(pConfig->ControlServerListenIP, pControlData, (void*)&m_RankNetworkID, sizeof(BYTE)))
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
		LC_Cmd_RankInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_Control, LC_RankInfo), sizeof(LC_Cmd_RankInfo));
		msg.RankID = m_RankNetworkID;
		SendNetCmdToControl(&msg);
	}
}