//DB 服务器的核心类
#pragma once
#include "..\CommonFile\FishConfig.h"
#include "SqlManager.h"
typedef HashMap<UINT, SqlTable*> TableHashMap;
struct RcveMsg
{
	NetCmd* pCmd;
	BYTE   ClientID;
};
class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer(BYTE DBNetworkID);
	bool MainUpdate();
	bool OnDestroy();

	void SetServerClose(){ m_bRun = true; }
	void SetReloadConfig(){ m_IsReloadConfig = true; }

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);
	void OnAddClient();

	//void SendNetCmdToClient(ServerClientData* pClient, NetCmd* pCmd);
	void SendNetCmdToControl(NetCmd*pCmd);

	FishConfig& GetFishConfig(){ return m_Config; }


	ServerClientData* GetClientData(BYTE ClientID);
	void HandleThreadMsg(BYTE ThreadIndex);//线程处理命令
private:
	bool ConnectClient();
	bool ConnectControl();

	void OnHandleAllMsg();
	void OnAddDBResult(BYTE Index, BYTE ClientID, NetCmd*pCmd);

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);

	bool HandleDataBaseMsg(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool HandleControlMsg(NetCmd* pCmd);

	void OnReloadConfig();


	//MySQl
	bool SelectTable(const SqlCondition &d, SqlTable *pMainTable, SqlTable &table){
		return pMainTable->Select(d, table);
	}
	bool CommitTable(SqlTable &table);
	bool CommitTable(SqlTable &mainTable, BYTE *pTableData, UINT size){
		return mainTable.CombineFromBytes(pTableData, size);
	}
private:
	//数据库具体的操作
	//具体数据库操作的函数
	//1.登陆相关
	bool OnHandleAccountLogon(BYTE Index, BYTE ClientID, NetCmd* pCmd);//账号登陆
	bool OnHandleQueryLogon(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAccountRsg(BYTE Index, BYTE ClientID, NetCmd* pCmd);//账号注册
	bool OnHandlePhoneSecPwdLogon(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAccountReset(BYTE Index, BYTE ClientID, NetCmd* pCmd);//游客账号转正式账号
	bool OnHandleChangeAccountPassword(BYTE Index, BYTE ClientID, NetCmd* pCmd);//修改账号密码
	bool OnHandleChangeAccountOnline(BYTE Index, BYTE ClientID, NetCmd* pCmd);//修改账号是否在线信息
	//bool OnHandleRoleAchievementIndex(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleRoleClientInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleGetNewAccount(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveChannelInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSetAccountFreeze(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandlePhoneLogon(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//2.玩家相关
	bool OnHandleGetRoleInfoByUserID(BYTE Index, BYTE ClientID, NetCmd* pCmd);//获取玩家数据根据玩家ID
	bool OnHandleLoadAllUserInfo(bool IsOnceDay, DWORD dwUserID, BYTE Index, BYTE ClientID);
	bool OnHandleSaveRoleInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);//保存玩家数据
	//bool OnHandleSaveRoleQueryAtt(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleNickName(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleLevel(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleGender(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAchievementPoint(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleCurTitle(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleCurency(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleFaceID(BYTE Index, BYTE ClientID, NetCmd* pCmd);

	bool OnHandleSaveRoleMedal(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleSendGiffSum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAcceptGiffSum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleTaskStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAchievementStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleActionStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleOnlineStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleCheckData(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleIsShopIpAddress(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAllInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleGlobel(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleOnLineMin(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleExChangeStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleTotalRecharge(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAddRoleTotalRecharge(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//bool OnHandleSaveRoleLockInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleIsFirstPayGlobel(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleIsFirstPayCurrcey(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleMoney(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleTableChange(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleRoleEntityItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleEntityItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleMonthCardInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleRateValue(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleMaxRate(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleGetMonthCardRewardTime(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleVipLevel(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleCashSum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleCashSum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleParticularStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleSecPassword(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleSecPassword(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleShareStates(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleGameIDConvertUserID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleZpnum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleIsShop(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleExchangeNum(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//3.玩家物品
	bool OnHandleGetUserItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAddUserItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelUserItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeUserItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//6.查询
	bool OnHandleQueryRoleInfoByNickName(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleQueryRoleInfoByUserID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleQueryRoleInfoByGameID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//7.签到
	/*bool OnHandleLoadRoleCheckInfo(BYTE Index,BYTE ClientID, NetCmd* pCmd);
	bool OnHandleUpdateRoleCheckInfo(BYTE Index,BYTE ClientID, NetCmd* pCmd);*/
	//8.任务
	bool OnHandleLoadRoleTask(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleTask(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleTask(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleClearRoleTask(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveAllTask(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//9.成就
	bool OnHandleLoadRoleAchievement(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAchievement(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleAchievement(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveAllAchievement(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//10.活动
	bool OnHandleLoadRoleAction(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleAction(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleAction(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveAllAction(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//11.称号
	bool OnHandleLoadRoleTitle(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAddRoleTitle(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelRoleTitle(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//12.商店
	bool OnHandleSaveRoleEntityItem(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//13.实体
	bool OnHandleLoadRoleEntity(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//bool OnHandleSaveRoleEntity(BYTE Index,BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityName(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityPhone(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityEmail(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityIdentityID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityItemUseName(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityItemUsePhone(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityItemUseAddress(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleEntityAlipayInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//17.GameData
	bool OnHandleLoadRoleGameData(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleSaveRoleGameData(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleChangeRoleGameDataByMonth(BYTE Index, BYTE ClientID, NetCmd* pCmd);

	bool OnHandleAchievementPointList(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//
	bool OnHandleLoadAllAccountInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//
	//Announcement
	bool OnHandleLoadAllAnnouncementInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAddAnnouncementInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//邮箱
	bool OnHandleChangeRoleEmail(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//Center
	bool OnHandleClearFishDB(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//ExChange
	bool OnHandleQueryExChange(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnDelExChange(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//Recharge
	bool OnAddRecharge(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnLoadRechargeOrderID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnAddRechargeOrderID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnCheckPhone(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnCheckEntityID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnLoadPhonePayOrderList(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnAddPhonePayOrderID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnGetRechargeOrderID(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnQueryRechargeOrderInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnDleRechargeOrderInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//Control
	bool OnResetUserPassword(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleFreezeUser(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//Robot
	bool OnHandelLoadRobotInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//Char
	bool OnHandleLoadCharInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleDelCharInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleAddCharInfo(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//
	bool OnHandleAddCash(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleLoadCash(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnHandleCash(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	//
	bool OnHandleRquestJoinNiuNiuRoom(BYTE Index, BYTE ClientID, NetCmd* pCmd);

	bool OnHandleCustMail(BYTE Index, BYTE ClientID, NetCmd* pCmd);

	void UpdateInfoToControl(DWORD dwTimer);
	//数据统计
	bool OnSaveFishBattleRecord(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnSaveMinuteSysRecord(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnSaveTotalGold(BYTE Index, BYTE ClientID, NetCmd* pCmd);
	bool OnGameserverStart(BYTE Index, BYTE ClientID, NetCmd* pCmd);
private:
	BYTE GetMinThreadID(){
		BYTE Index = 0;
		DWORD MsgSum = 0xFFFFFFFF;
		for (BYTE i = 0; i < DB_ThreadSum; ++i)
		{
			if (MsgSum > m_ThreadMsgSumArray[i])
			{
				Index = i;
				MsgSum = m_ThreadMsgSumArray[i];
				if (MsgSum == 0)
					break;
			}
		}
		Index = Index;
		return Index;
	}
	void AddThreadSum(BYTE ThreadID){
		if (ThreadID >= DB_ThreadSum)
			return;
		m_ThreadMsgSumArray[ThreadID] += 1;
	}
	void InitThreadMsgSum(){
		for (BYTE i = 0; i < DB_ThreadSum; ++i)
			m_ThreadMsgSumArray[i] = 0;
	}
	void UpdateThreadMsgSum()
	{
		for (BYTE i = 0; i < DB_ThreadSum; ++i)
			m_ThreadMsgSumArray[i] = m_RecvMsgList[i].Count();
	}
private:
	//dump
	Dump										m_pDump;
	BYTE										m_DBNetworkID;
	//服务器控制属性
	volatile bool								m_IsReloadConfig;
	volatile bool								m_bRun;
	volatile long								m_ExitCount;

	NewServer									m_Server;//对GameServer的网络库
	SafeList<AfxNetworkClientOnce*>				m_AfxAddClient;
	HashMap<BYTE, ServerClientData*>			m_ClintList;
	//SQL
	SqlManager									m_Sql[DB_ThreadSum];
	SafeList<RcveMsg*>							m_RecvMsgList[DB_ThreadSum];
	SafeList<RcveMsg*>							m_SendMsgList[DB_ThreadSum];
	SafeList<SqlUpdateStr*>						m_SqlList;
	//ControlServer
	//TCPClient									m_ControlTcp;
	bool										m_ControlIsConnect;
	//Config
	FishConfig									m_Config;

	//
	DWORD										m_ThreadMsgSumArray[DB_ThreadSum];
};
extern FishServer g_FishServer;