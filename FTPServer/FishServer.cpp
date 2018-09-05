#include "stdafx.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include <io.h>
#define IMG_DIR "FishPublish\\images"
#define XML_DIR "FishPublish\\xml"
FishServer g_FishServer;
FishServer::FishServer()
{
	m_IsClose = false;
	m_FtpNetworkID = 0;
	m_FileArray.clear();
	m_ControlIsConnect = false;
	m_LogDBIsConnect = false;
}
FishServer::~FishServer()
{
	if (!m_FileArray.empty())
	{
		HashMap<DWORD, TempFileData>::iterator Iter = m_FileArray.begin();
		for (; Iter != m_FileArray.end(); ++Iter)
		{
			free(Iter->second.FileData);
		}
		m_FileArray.clear();
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
bool FishServer::InitServer()
{
	m_pDump.OnInit();
	if (!g_FishServerConfig.LoadServerConfigFile())
	{
		ShowInfoToWin("�����������ļ���ȡʧ��");
		return false;
	}
	if (!g_FishServerConfig.GetFTPServerConfig())
	{
		ASSERT(false);
		return false;
	}
	if (!g_FishServerConfig.GetFTPServerConfig())
	{
		ASSERT(false);
		return false;
	}
	m_FtpNetworkID = g_FishServerConfig.GetFTPServerConfig()->NetworkID;

	//1.�ж�FTP ·���Ƿ���� �����ڵĻ� �ر�FTP������
	if (_access(g_FishServerConfig.GetFTPServerConfig()->FTPFilePath, 0) != 0)
	{
		if (ENOENT == GetLastError())
		{
			return false;
		}
	}

	if (!ConnectControl())
	{
		while (true)
		{
			Sleep(5);
			if (ConnectControl())
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

	if (!ConnectClient())
	{
		ASSERT(false);
		return false;
	}
	return true;
}
bool FishServer::ConnectLogDB()
{
	FTPServerConfig* pFTPServerConfig = g_FishServerConfig.GetFTPServerConfig();
	if (!pFTPServerConfig)
	{
		ASSERT(false);
		return false;
	}
	DBServerConfig* pDBConfig = g_FishServerConfig.GetDBServerConfig(pFTPServerConfig->LogDBID);
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
	if (!m_LogDBTcp.Init(pDBConfig->DBListenIP, pDBData, (void*)&m_FtpNetworkID, sizeof(BYTE)))
	{
		ShowInfoToWin("Log���ݿ�����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_LogDBTcp);
	return true;
}
bool FishServer::ConnectClient()
{
	FTPServerConfig* pFTPConfig = g_FishServerConfig.GetFTPServerConfig();
	if (!pFTPConfig)
	{
		ASSERT(false);
		return false;
	}

	ServerInitData pCLientData;
	pCLientData.ServerID = pFTPConfig->NetworkID;
	pCLientData.BuffSize = pFTPConfig->BuffSize;
	pCLientData.CmdHearbeat = 0;
	pCLientData.Port = pFTPConfig->FTPListenPort;
	pCLientData.SocketRecvSize = pFTPConfig->RecvBuffSize;
	pCLientData.SocketSendSize = pFTPConfig->SendBuffSize;
	pCLientData.Timeout = pFTPConfig->TimeOut;
	pCLientData.RecvThreadNum = pFTPConfig->RecvThreadSum;
	pCLientData.SendThreadNum = pFTPConfig->SendThreadSum;
	pCLientData.SleepTime = pFTPConfig->SleepTime;
	pCLientData.AcceptSleepTime = pFTPConfig->AcceptSleepTime;
	pCLientData.AcceptRecvData = true;

	pCLientData.SceneHearbeatTick = pFTPConfig->SceneHearbeatTick;
	pCLientData.MaxSendCmdCount = pFTPConfig->MaxSendCmdCount;
	pCLientData.MaxAcceptNumPerFrame = pFTPConfig->MaxAcceptNumPerFrame;
	pCLientData.MaxRecvCmdCount = pFTPConfig->MaxRecvCmdCount;
	pCLientData.MaxSendCountPerFrame = pFTPConfig->MaxSendCountPerFrame;
	pCLientData.ListenCount = pFTPConfig->ListenCount;

	m_ClientTcp.SetCmdHandler(this);

	m_FtpNetworkID = pFTPConfig->NetworkID;

	if (!m_ClientTcp.Init(pCLientData))
	{
		ShowInfoToWin("�������������ʧ��");
		return false;
	}
	
	return true;
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

	if (!m_ControlTcp.Init(pConfig->ControlServerListenIP, pControlData, (void*)&m_FtpNetworkID, sizeof(BYTE)))
	{
		ShowInfoToWin("ControlServer����ʧ��");
		return false;
	}
	OnTcpClientConnect(&m_ControlTcp);
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
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//���������ֻ����FTP �� DB�����ӳɹ���Ź���
	if (SeverID == m_FtpNetworkID)
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
	if (!pData || recvSize != sizeof(BYTE))
	{
		m_ClientTcp.Kick(pClient);
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
		delete pOnce;
		ASSERT(false);
		return false;
	}
	memcpy_s(pOnce->pPoint, recvSize, pData, recvSize);
	pOnce->Length = recvSize;
	m_AfxAddClient.AddItem(pOnce);
	return true;
}
void FishServer::Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
{
	if (!pClient)
	{
		ASSERT(false);
		return;
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
		if (pOnce->SeverID == m_FtpNetworkID)
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
			pOnce->pClient->OutsideExtraData = ClientID;
			
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.find(ClientID);
			if (Iter != m_ClintList.end())
			{
				ASSERT(false);
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
		}
		else
		{
			if (pOnce->pPoint)
				free(pOnce->pPoint);
			delete pOnce;
			//ASSERT(false);
		}
	}
}
void FishServer::OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient)
{
	//�����������һ���ͻ����뿪��ʱ��
	if (ServerID == m_FtpNetworkID)
	{
		ShowInfoToWin("һ��GameServer�뿪��FtpServer");
	}
	return;
}
void FishServer::OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient)
{
	//�����������һ���ͻ��˼����ʱ��
	if (ServerID == m_FtpNetworkID)
	{
		ShowInfoToWin("һ��GameServer������FtpServer");
	}
	return;
}
void FishServer::SendNetCmdToGameServer(ServerClientData* pClient, NetCmd* pCmd)
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
void FishServer::SendNetCmdToAllGameServer(NetCmd* pCmd)
{
	if (!pCmd || m_ClintList.empty())
		return;
	HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
	for (; Iter != m_ClintList.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		if (!m_ClientTcp.Send(Iter->second, pCmd))
		{
			ASSERT(false);
		}
	}
}
bool FishServer::MainUpdate()
{
	while (!m_IsClose)
	{
		OnAddClient();

		DWORD dwTimer = timeGetTime();
		UpdateInfoToControl(dwTimer);
		//1.����GameServer ������
		if (!m_ClintList.empty())
		{
			HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
			for (; Iter != m_ClintList.end();)
			{
				if (!Iter->second)
				{
					++Iter;
					continue;
				}
				if (Iter->second->Removed)
				{
					OnTcpServerLeave(m_FtpNetworkID, Iter->second);
					m_ClientTcp.Kick(Iter->second);
					Iter = m_ClintList.erase(Iter);
					continue;
				}
				int Sum = 0;
				while (Iter->second->RecvList.HasItem() && Sum < Msg_OnceSum)
				{
					NetCmd* pCmd = Iter->second->RecvList.GetItem();
					HandleGameServerMsg(Iter->second, pCmd);
					free(pCmd);
					++Sum;
				}
				++Iter;
			}
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
bool FishServer::HandleGameServerMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
	{
		ASSERT(false);
		return false;
	}
	if (!g_FishServerConfig.GetFTPServerConfig())
	{
		ASSERT(false);
		return false;
	}
	//����GameServer�������� �Զ���ͷ��Ĵ�����
	if (pCmd->CmdType == Main_Role)
	{
		if (pCmd->SubCmdType == GF_SaveImageData)
		{
			GF_Cmd_SaveImageData* pMsg = (GF_Cmd_SaveImageData*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			char FilePath[FTP_PATH + 1] = { 0 };
			sprintf_s(FilePath, sizeof(FilePath), "%s\\%s", g_FishServerConfig.GetFTPServerConfig()->FTPFilePath, IMG_DIR);
			CreateDirectoryA(FilePath, 0);

			char dir[MAX_PATH + 1];
			//�����ݽ��д���

			sprintf_s(dir, sizeof(dir), "%s\\%s", FilePath, pMsg->ImgName);
			FILE *pFile;
			fopen_s(&pFile, dir, "wb");
			bool bRet = pFile != NULL;
			if (bRet)
			{
				fwrite(pMsg->ImgData, 1, pMsg->ImgSize, pFile);
				fclose(pFile);
				//cout << "" << dir << endl;
				LogInfoToFile("FTPFileSaveLog.txt", "�ļ�����ɹ�: %s", dir);
			}
			else
			{
				ASSERT(false);
				return false;
				//cout << "*�ļ�����ʧ��:" << dir << endl;
				LogInfoToFile("FTPFileSaveLog.txt", "�ļ�����ʧ��: %s", dir);
			}

			FG_Cmd_SaveImageData msg;
			SetMsgInfo(msg, GetMsgType(Main_Role, FG_SaveImageData), sizeof(FG_Cmd_SaveImageData));
			msg.Size = sizeof(FG_Cmd_SaveImageData);
			msg.Result = bRet;
			msg.ID = pMsg->ID;
			msg.Crc = pMsg->ImgCrc;
			SendNetCmdToGameServer(pClient, &msg);
			return true;
		}
	}
	else if (pCmd->CmdType == Main_Ftp)
	{
		if (pCmd->SubCmdType == CF_UpLoadFile)
		{	
			CF_Cmd_UpLoadFile* pMsg = (CF_Cmd_UpLoadFile*)pCmd;
			if (!pMsg)
			{
				ASSERT(false);
				return false;
			}
			char FilePath[FTP_PATH] = { 0 };
			sprintf_s(FilePath, sizeof(FilePath), "%s\\%s", g_FishServerConfig.GetFTPServerConfig()->FTPFilePath, XML_DIR);
			CreateDirectoryA(FilePath, 0);

			if ((pMsg->States & MsgBegin) != 0)//�����ļ����б�
			{
				//��ʼ���յ����� ���������ڴ��д���һ������ �ȱ�������
				char FileName[64];
				strncpy_s(FileName, CountArray(FileName), pMsg->FileName, CountArray(pMsg->FileName));
				errno_t err = _strlwr_s(FileName, 64);
				if (err != 0)
				{
					ASSERT(false);
					return false;
				}
				DWORD FileCrc = AECrc32(FileName, sizeof(char)*64, 0);

				BYTE* pFileData = (BYTE*)malloc(pMsg->FileSum);
				if (!pFileData)
				{
					ASSERT(false);
					return false;
				}

				TempFileData pFile;
				pFile.FileSize = pMsg->FileSum;
				pFile.FileData = pFileData;
				pFile.WriteSize = 0;

				HashMap<DWORD, TempFileData>::iterator Iter = m_FileArray.find(FileCrc);
				if (Iter != m_FileArray.end())
				{
					ASSERT(false);
					return false;
				}
				m_FileArray.insert(HashMap<DWORD, TempFileData>::value_type(FileCrc, pFile));
			}
			//���ļ�д�뵽 ����
			char FileName[64];
			strncpy_s(FileName, CountArray(FileName), pMsg->FileName, CountArray(pMsg->FileName));
			errno_t err = _strlwr_s(FileName, 64);
			if (err != 0)
			{
				ASSERT(false);
				return false;
			}
			DWORD FileCrc = AECrc32(FileName, sizeof(char)* 64, 0);

			HashMap<DWORD, TempFileData>::iterator Iter = m_FileArray.find(FileCrc);
			if (Iter == m_FileArray.end())
			{
				ASSERT(false);
				return false;
			}
			memcpy_s(&Iter->second.FileData[Iter->second.WriteSize], pMsg->Sum, pMsg->Array, pMsg->Sum);
			
			if ((pMsg->States & MsgEnd) != 0)
			{
				//�ļ��Ѿ�д������� 
				char dir[MAX_PATH +1];
				sprintf_s(dir, sizeof(dir), "%s\\%s", FilePath, pMsg->FileName);
				FILE* pFile = null;
				errno_t Error = fopen_s(&pFile, dir, "wb");
				if (Error != 0 || !pFile)
				{
					ASSERT(false);
					return false;
				}
				fwrite(Iter->second.FileData, 1, pMsg->FileSum, pFile);//������д�뵽�ļ�����ȥ
				fclose(pFile);
				//ShowInfoToWin("�ļ�����ɹ�: %s", dir);

				free(Iter->second.FileData);
				m_FileArray.erase(Iter);

				LogInfoToFile("FTPFileSaveLog.txt", "ͼƬ�ļ�����ɹ�: %s", dir);
			}
			return true;
		}
	}
	return true;
}
void FishServer::HandleAllMsg()
{
	if (!m_ClintList.empty())
	{
		HashMap<BYTE, ServerClientData*>::iterator Iter = m_ClintList.begin();
		for (; Iter != m_ClintList.end();)
		{
			while (Iter->second->RecvList.HasItem())
			{
				NetCmd* pCmd = Iter->second->RecvList.GetItem();
				if (!pCmd)
				{
					continue;
				}
				//������������ �ͻ��˷������ĵ�½ ע�� ������
				HandleGameServerMsg(Iter->second, pCmd);
				free(pCmd);
			}
			++Iter;
		}
	}
	NetCmd* pCmd = m_ControlTcp.GetCmd();
	while (pCmd)
	{
		HandleControlMsg(pCmd);
		free(pCmd);
		pCmd = m_ControlTcp.GetCmd();
	}
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
bool FishServer::HandleControlMsg(NetCmd* pCmd)
{
	//������Ӫ������������������
	if (!pCmd)
		return false;


	return true;
}
void FishServer::OnTcpClientConnect(TCPClient* pClient)
{
	//��������ӳɹ���ʱ��
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	if (pClient == &m_ControlTcp)
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
	if (pClient == &m_ControlTcp)
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
	}
	return;
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
		LC_Cmd_FTPInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_Control, LC_FTPInfo), sizeof(LC_Cmd_FTPInfo));
		msg.FTPID = m_FtpNetworkID;
		SendNetCmdToControl(&msg);
	}
}