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
	//1.读取配置文件 服务器配置的配置文件
	if (!g_FishServerConfig.LoadServerConfigFile())
	{
		Log("服务器配置文件读取失败");
		return false;
	}

	m_GameNetworkID = ConvertIntToBYTE(Index);
	GameServerConfig* pGameConfig = g_FishServerConfig.GetGameServerConfig(m_GameNetworkID);
	if (!pGameConfig)
	{
		ASSERT(false);
		return false;
	}

	//4.各种管理器进行初始化
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
	Log("玩家管理器初始化成功");
	m_TableManager.OnInit();
	Log("桌子管理器初始化成功");
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
	m_RobotManager.OnLoadAllGameRobot(pGameConfig->BeginRobotUserID, pGameConfig->EndRobotUserID);//加载新的玩家
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
	//加载桌子数据
	{
		DBR_Cmd_GameserverStart msg;
		SetMsgInfo(msg, DBR_GameserverStart, sizeof(DBR_Cmd_GameserverStart));
		msg.game_id = m_GameNetworkID;
		g_FishServer.SendNetCmdToDB(&msg);
	}
	Log("GameServer启动成功");
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
		//Log("一个客户端离开游戏");
		return;//客户端离开不做处理
	}
	//输出 客户端离开的状态
	switch (rt)
	{
	case REMOVE_NONE:
		{
			Log("客户端 ID:%d 离开原因为None", pClient->OutsideExtraData);
		}
		break;
	case REMOVE_NORMAL:
		{
			Log("客户端 ID:%d 离开原因为普通",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECV_ERROR:
		{
			Log("客户端 ID:%d 离开原因为接收错误",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_SEND_ERROR:
		{
			Log("客户端 ID:%d 离开原因为发送错误",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_TIMEOUT:
		{
			Log("客户端 ID:%d 离开原因为超时",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SEND_OVERFLOW:
		{
			Log("客户端 ID:%d 离开原因为发送太多",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_RECV_OVERFLOW:
		{
			Log("客户端 ID:%d 离开原因为接收太多",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_CMD_SIZE_ERROR:
		{
			Log("客户端 ID:%d 离开原因为命令大小",pClient->OutsideExtraData);
		}
		break;
	case REMOVE_RECVBACK_NOT_SPACE:
		{
			Log("客户端 ID:%d 离开原因为接收缓冲区溢出",pClient->OutsideExtraData);
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
		Log("DB服务器连接失败");
		return false;
	}
	OnTcpClientConnect(&m_DBTcp);
	return true;
}
bool FishServer::ConnectSaveDB()
{
	//连接到保存数据库 和 普通的读取 修改数据库不是一个数据库
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
		Log("SaveDB服务器连接失败");
		return false;
	}
	OnTcpClientConnect(&m_DBSaveTcp);
	return true;
}
bool FishServer::ConnectLogDB()
{
	//连接到保存数据库 和 普通的读取 修改数据库不是一个数据库
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
		Log("LogDB服务器连接失败");
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
		Log("中央服务器连接失败");
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
	pCLientData.AcceptRecvData = false;		//表示接收初始数据

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
		Log("对内网络库启动失败");
		return false;
	}
	
	
	return true;
}
uint FishServer::CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
{
	//中央服务器只有在FTP 和 DB都连接成功后才工作
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
			pOnce->pClient->OutsideExtraData = ClientID;//玩家的ID
			m_ClintList.insert(HashMap<DWORD, ServerClientData*>::value_type(pOnce->pClient->OutsideExtraData, pOnce->pClient));
			DWORD tick = timeGetTime();
			//让玩家进行数据库查询加载数据
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
				Log("未找到大厅发送的数据缓存uid=%lld", Uid);
			}
			//触发玩家加入的事件
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
	//往网络库中有一个客户端离开的时候
	if (ServerID == m_GameNetworkID)
	{
		//Log("一个Client离开了GameServer");
		OnHandleRoleLeaveTable(pClient);
	}
	return;
}
void FishServer::OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient)
{
	//往网络库中有一个客户端加入的时候
	if (ServerID == m_GameNetworkID)
	{
		//Log("一个Client加入了GameServer");
	}
	return;
}
void FishServer::OnTcpClientConnect(TCPClient* pClient)
{
	if (!pClient)
		return;
	//网络库连接成功的时候
	if (pClient == &m_CenterTcp)
	{
		m_CenterTcpStates = true;
		Log("已经与中央服务器连接成功了");
	}
	else if (pClient == &m_DBTcp)
	{
		m_DBTcpStates = true;

		Log("已经与数据库连接成功了");
	}
	else if (pClient == &m_DBSaveTcp)
	{
		m_DBSaveTcpStates = true;
		Log("已经与Save数据库连接成功了");
	}
	else if (pClient == &m_DBLogTcp)
	{
		m_DBLogTcpStates = true;
		Log("已经与Log数据库连接成功了");
	}
	else
		return;
}
void FishServer::OnLeaveCenterServer()
{
	//当GameServer服务不可用的时候 我们让玩家全部离线
	//让全部玩家离线
	m_RoleManager.OnKickAllUser();
	//断开全部的网络
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
		Log("已经与中央服务器断开连接了");
		m_CenterTcp.Shutdown();
		//m_GameServerManager.OnGameLeavCenter();//与中央服务器断开连接
		OnLeaveCenterServer();
	}
	else if (pClient == &m_DBTcp)
	{
		m_DBTcpStates = false;
		m_DBTcp.Shutdown();
		Log("已经与数据库断开连接了");
	}
	else if (pClient == &m_DBSaveTcp)
	{
		m_DBSaveTcpStates = false;
		m_DBSaveTcp.Shutdown();
		Log("已经与Save数据库断开连接了");
	}
	else if (pClient == &m_DBLogTcp)
	{
		m_DBLogTcpStates = false;
		m_DBLogTcp.Shutdown();
		Log("已经与Log数据库断开连接了");
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
				//处理网络命令 客户端发送来的登陆 注册 等命令
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
			OnReloadConfig();//重新加载配置文件
			if (!FishCallback::GetFishSetting()->LoadConfig("fishdata/Config"))
			{
				Log(L"重新加载config.xml失败!");
				return false;
			}
			Log(L"重新加载config.xml成功!");
		}

		OnAddClient();
		UpdateByMin(dwTimer);
		OnSaveInfoToDB(dwTimer);

		m_RobotManager.Update();
		//1.处理Client 的命令
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
		//2.处理数据库的命令
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

			//重新连接DB
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

			//重新连接DB
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

			//重新连接DB
			ConnectLogDB();
		}
		//3.中央服务器
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
		//管理器更新
		m_LogonManager.OnUpdate(dwTimer);
		//命令处理完毕后 我们开始处理 其他的更新函数 其实就是 不断的Send命令到其他服务器或者客户端去
		m_TableManager.Update(dwTimer); //更新桌子
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
//开始命令处理类了 
bool FishServer::HandleClientMsg(ServerClientData* pClient, NetCmd* pCmd)
{
	if (!pClient || !pCmd)
		return false;
	//客户端发送来的命令
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
	//中央服务器发送来的数据
	switch (pCmd->GetMainType())
	{
	case Main_Hall:
	{
		switch (pCmd->GetSubType())
		{ 
		case 63://玩家从游戏大厅请求登录游戏
		{
			//大厅没有启动，返回
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
		case 67:// 玩家金币改动，大厅通知捕鱼游戏
		{
			LG_UpdateMoney* pmsg = (LG_UpdateMoney*)pCmd;
			//Log("update money uid = %lld,money1=%f,money2=%f", uid, pmsg->money1, pmsg->money2);
			CRoleEx* pRole = m_RoleManager.QuertUserByUid(pmsg->uid);
			if (!pRole)
			{
				Log("玩家金币改动，未找到玩家：%lld, money1=%lf, money2=%lf", pmsg->uid, pmsg->money1, pmsg->money2);
				ASSERT(false);
				return false;
			}
			pRole->GetRoleInfo().money1 += static_cast<int64>(pmsg->money1 * MONEY_RATIO);
			pRole->GetRoleInfo().money2 += static_cast<int64>(pmsg->money2 * MONEY_RATIO);
			//通知客户端
			LG_SyncAddMoney msg;
			SetMsgInfo(msg, 6034, sizeof(LG_SyncAddMoney));
			msg.add_goldNum = static_cast<int64>(pmsg->money1 * MONEY_RATIO) + static_cast<int64>(pmsg->money2 * MONEY_RATIO);
			msg.reason = pmsg->reason;
			pRole->SendDataToClient(&msg);
			break;
		}
		case 71://关闭捕鱼服务器
		{
			LG_StopFishServer* pmsg = (LG_StopFishServer*)pCmd;
			Log("游戏大厅离线倒计时！%lld ms", pmsg->closeTime);
			m_hallControl.ReqStopHall(pmsg->closeTime);
			break;
		}
		case 72://通知加载捕鱼配置表
		{
			//OnReloadConfig();//重新加载配置文件
			//if (!FishCallback::GetFishSetting()->LoadConfig("fishdata/Config"))
			//{
			//	Log(L"重新加载config.xml失败!");
			//}
			//Log(L"重新加载config.xml成功!");
			//解析配置
			CL_Cmd_ReloadConfig* pmsg = (CL_Cmd_ReloadConfig*)pCmd;
			std::string tableStr;
			tableStr.assign(pmsg->tableStr);
			Log("收到更新配置：%s", tableStr.c_str());
			vector<string> allFields;
			SplitString2Vector(tableStr, allFields, ",");
			if (allFields.size() != 6)
			{
				Log("解析发送配置错误，节点数据量=%d", allFields.size());
				return true;
			}
			//系统收益控制节点
			float winMin = static_cast<float>(atof(allFields[0].c_str()));
			if (winMin != -1) SystemControl::sysMinWin = winMin;
			float winMax = static_cast<float>(atof(allFields[1].c_str()));
			if (winMax != -1) SystemControl::sysMaxWin = winMax;
			if (allFields[2].length() > 2)
			{
				const string& field = allFields[2];
				string temp = allFields[2].substr(1, field.length() - 2);//删除[]
				vector<string> vecControl;
				SplitString2Vector(temp, vecControl, "|");
				if (vecControl.size() % 3 != 0)
				{
					Log("系统收益节点解析错误，单元数量配置错误");
					return false;
				}
				UINT i = 0;
				while (i < vecControl.size())
				{
					int id = atoi(vecControl[i].c_str());
					auto it = SystemControl::item.find(id);
					if (it == SystemControl::item.end())
					{
						Log("系统收益item ID配置错误,id=%d", id);
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
			//鱼死亡控制节点
			if (allFields[3].length() > 2)
			{
				const string& field = allFields[3];
				string temp = allFields[3].substr(1, field.length() - 2);//删除[]
				vector<string> vecFishDead;
				SplitString2Vector(temp, vecFishDead, "|");
				if (vecFishDead.size() % 2 != 0)
				{
					Log("鱼死亡控制节点解析错误，单元数量配置错误");
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
			//桌子初始化配置
			if (allFields[4].length()>2)
			{
				const string& field = allFields[4];
				string temp = allFields[4].substr(1, field.length() - 2);//删除[]
				vector<string> vecTableInit;
				SplitString2Vector(temp, vecTableInit, "|");
				if (vecTableInit.size() % 4 != 0)
				{
					Log("桌子初始化配置节点错误");
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
						Log("桌子更新配置ID错误:桌子ID = %d", tableID);
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
			//子弹威力配置更新
			if (allFields[5].length() > 2)
			{
				const string& field = allFields[5];
				string temp = allFields[5].substr(1, field.length() - 2);//删除[]
				vector<string> vecConnonPower;
				SplitString2Vector(temp, vecConnonPower, "|");
				if (vecConnonPower.size() % 2 != 0)
				{
					Log("子弹威力解析错误，单元数量配置错误");
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
		case 73://通知游戏大厅允许进入
		{
			CL_HallStartFishServer* pmsg = (CL_HallStartFishServer*)pCmd;
			Log("游戏大厅开服倒计时！%lld ms", pmsg->starTime);
			m_hallControl.ReqStartHall(pmsg->starTime);
			break;
		}
		case 162://LG 使用物品
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
				//传入到游戏内部去
				return m_TableManager.OnHandleTableMsg(pRole->GetUserID(), pCmd);
			}
			else
			{
				Log("使用物品,未知玩家：%lld, 物品ID=%d, 物品数量=%d", pmsg->uid, pmsg->itemId, pmsg->itemNum);
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
void FishServer::UpdateByMin(DWORD dwTimer)//按分钟进行更新 不受中央服务器控制
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
	//具体的关系函数
	m_RoleManager.OnUpdateByMin(IsHourChange, IsDayChange, IsMonthChange, IsYearChange);
	m_TableManager.CostAndProduceMin();
	m_hallControl.UpdateMin();
	if (m_hallControl.IsHallRuning() == false)
	{
		OnLeaveCenterServer();
	}
	if (IsNewDay)
	{
		//告诉客户端 新的一天了 可以进行处理些数据了
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
	if (pMsg->ClientID != 0 && !pClient)//pMsg->ClientID == 0 为机器人
	{
		ASSERT(false);
		return false;
	}
	if (pMsg->ClientID == 0 && pMsg->IsRobot == false)
	{
		ASSERT(false);
		return false;//非机器人 被加载被加载为机器人了
	}
	DWORD SocketID = (pClient ? pClient->OutsideExtraData : 0);
	if (pMsg->Result)
	{
		//玩家登陆成功
		//玩家登陆成功了 我们进行处理
		//1.生成玩家对象
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
				//玩家Socket进行切换
				DWORD SocketID = pRole->GetGameSocketID();
				pRole->ChangeRoleSocketID(pClient->OutsideExtraData);
			}
			else
			{
				m_TableManager.OnPlayerLeaveTable(pRole->GetUserID());//先离开桌子
				pRole->ChangeRoleSocketID(pClient->OutsideExtraData);//修改缓存玩家的Socket
			}
			if (pRole->IsExit())
			{
				//Log("玩家进行离线保存 在回发确认命令之前 重新登陆了  回发确认命令 失效 ");
				pRole->SetIsExit(false);
			}
		}
		else if (!m_RoleManager.CreateRole(&pMsg->RoleInfo, &pMsg->RoleServerInfo, SocketID, pMsg->LastOnlineTime, pMsg->LogonByGameServer, pMsg->IsRobot))
		{
			//创建玩家失败后 让玩家离开
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
			//账号被冻结了 提示失败
			LC_Cmd_AccountIsFreeze msg;
			SetMsgInfo(msg, GetMsgType(Main_Logon, LC_AccountIsFreeze), sizeof(LC_Cmd_AccountIsFreeze));
			msg.EndTime = pMsg->FreezeEndTime;
			SendNewCmdToClient(pClient, &msg);
			return true;
		}
		//玩家登陆失败了
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
		//当玩家发送登陆行为的时候
		switch (pCmd->GetSubType())
		{
		case CL_Sub_JoinTable:
		{
			bool ret = OnHandleRoleJoinTable(pClient, pCmd);
			if (ret == false)
			{
				//进入大厅失败，让玩家退出
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
				Log("未找到请求查看玩家信息：id = %lld", pMsg->playerid);
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
	//玩家进入桌子
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
	return m_TableManager.OnPlayerJoinTable(pMsg->bTableTypeID, pRole);//内部处理玩家进入桌子 会处理命令的 不带比赛参数
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
	//玩家管理器删除玩家数据
	m_RoleManager.OnKickOneUser(pRole);
	//标记移除
	pClient->Removed = true;
	return true;
}
//------------------------游戏------------------------------------------------------------------------
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
		//玩家上传头像 非桌子内部命令 无须放入内部 外部处理就OK
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
	//接收客户端发送来的 玩家数据 修改
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
	//接收到保存
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
	//将IP地址进行 取反
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
	//对TCHAR 进行截取
	size_t len = _tcsclen(pResult);
	size_t beginIndex = 0;
	//字符串生成成功后 进行处理
	for (size_t i = 0; i < len; ++i)
	{
		if (pResult[i] == TEXT('省'))
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
		Log("重新加载FishConfig.xml失败");
		return;
	}
	m_FishConfig = pNewConfig;
	Log("重新加载FishConfig.xml成功");
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
		//Log("清除缓存数据：%lld", uid);
	}
	return true;
}