//GameServer核心类
#pragma once
#include "UserLogonManager.h"
#include "..\CommonFile\FishConfig.h"
#include "RoleManager.h"
#include "TableManager.h"
#include "EventManager.h"
#include "TcpClientList.h"
#include "GameRobot.h"
#include "GameHallControl.h"
struct DelClient
{
	DWORD		LogTime;
	DWORD		SocketID;
};
typedef HashMap<INT64, tagClientUserData*> HallDataCatch;
class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer(int Index);
	bool MainUpdate();
	bool OnDestroy();

	void SetServerClose(){ m_IsClose = true; }
	void SetReloadConfig(){ m_IsReloadConfig = true; }

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);
	void OnAddClient();

	void SendNetCmdToDB(NetCmd* pCmd);
	void SendNetCmdToCenter(NetCmd* pCmd);
	void SendNetCmdToSaveDB(NetCmd* pCmd);
	void SendNetCmdToLogDB(NetCmd* pCmd);
	void SendNewCmdToClient(ServerClientData* pClient, NetCmd* pCmd);
	void SendNewCmdToAllClient(NetCmd* pCmd);

	ServerClientData* GetUserClientDataByIndex(DWORD IndexID);
	DWORD			GetOnlinePlayerSum(){ return m_ClintList.size(); }
	RoleManager*	GetRoleManager(){ return &m_RoleManager; }
	TableManager*	GetTableManager(){ return &m_TableManager; }
	FishConfig&		GetFishConfig(){ return m_FishConfig; }
	GameRobotManager& GetRobotManager(){ return m_RobotManager; }
	void UpdateByMin(DWORD dwTimer);
	void OnSaveInfoToDB(DWORD dwTimer);
	void CloseClientSocket(DWORD SocketID);
	BYTE  GetGameIndex(){ return m_GameNetworkID; }
	void GetAddressByIP(DWORD IP, TCHAR* pAddress,DWORD ArrayCount);
	bool HandleDataBaseMsg(NetCmd* pCmd);
	const INT GetRatioValue(){ return m_ratioValue; }
	bool OnRemoveHallCatchData(const int64 uid);
private:
	bool ConnectDB();
    bool ConnectSaveDB();
	bool ConnectLogDB();
	bool ConnectCenter();
	bool ConnectClient();

	void OnHandleAllMsg();

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);

	void OnLeaveCenterServer();

	bool HandleClientMsg(ServerClientData* pClient, NetCmd* pCmd);
	bool HandleCenterMsg(NetCmd* pCmd);
	//桌子进出
	bool OnHandleTCPNetworkTable(ServerClientData* pClient, NetCmd* pCmd);
	bool OnHandleRoleJoinTable(ServerClientData* pClient, NetCmd* pCmd);
	bool OnHandleRoleLeaveTable(ServerClientData* pClient);
	//游戏内部消息
	bool OnHandleTCPNetworkGame(ServerClientData* pClient, NetCmd* pCmd);
	//Role
	bool OnHandleTCPNetworkRole(ServerClientData* pClient, NetCmd* pCmd);
	bool OnHandleSaveRoleAllInfo(NetCmd* pCmd);
	bool OnHandDBLogonMsg(NetCmd* pCmd);
	//玩家GameData
	bool OnHandleDataBaseLoadGameData( NetCmd* pCmd);
	//
	bool OnHandleTCPNetworkPackage(ServerClientData* pClient, NetCmd* pCmd);
	//GameData
	bool OnHandleTCPNetworkGameData(ServerClientData* pClient, NetCmd* pCmd);
	void OnReloadConfig();
	void UpdateInfoToControl(DWORD dwTimer);
	void InitTestClientData();
private:
	Dump						m_pDump;
	volatile bool	 			m_IsClose;
	volatile bool				m_IsReloadConfig;
	BYTE						m_GameNetworkID;
	//网络库
	NewUDPServer				m_ClientTcp;//对客户端的网络库
	HashMap<DWORD, ServerClientData*> m_ClintList;

	TCPClient			m_CenterTcp;//连接到中央服务器的连接
	bool				m_CenterTcpStates;

	//DBServer
	TCPClient			m_DBTcp;//连接到数据库的连接
	bool				m_DBTcpStates;
	//DBSaveServer 专门用于保存的数据库连接
	TCPClient			m_DBSaveTcp;
	bool				m_DBSaveTcpStates;
	//Log
	TCPClient			m_DBLogTcp;
	bool				m_DBLogTcpStates;

	//具体的管理器
	DWORD				m_UserIndex;
	UserLogonManager	m_LogonManager;

	//配置文件
	FishConfig			m_FishConfig;

	RoleManager			m_RoleManager;
	TableManager		m_TableManager;
	SafeList<AfxNetworkClientOnce*>			m_AfxAddClient;
	GameRobotManager							m_RobotManager;
	//游戏大厅登录的玩家信息缓存
	HashMap<INT64,tagClientUserData*>				m_HalldataCach;
	//游戏大厅同步的玩家金币倍率
	static INT										m_ratioValue;
	//游戏大厅辅助控制
	GameHallControl						m_hallControl;
};
extern FishServer g_FishServer;