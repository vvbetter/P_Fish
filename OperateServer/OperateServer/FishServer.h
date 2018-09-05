#pragma once
#include "..\CommonFile\FishConfig.h"
#include "BuyEntityItemManager.h"
#include "RealNameVerificationManager.h"
#include "BindPhoneManager.h"
#include "BindEMailManager.h"
#include "UseRMBManager.h"
#include "ChannelLogonManager.h"
#include "PhonePayManager.h"
#include "PhoneLogon.h"
#include "NormalRechargeManager.h"
#include "IOSPayManager.h"
#include "OrderOnlyManager.h"
#include "WeiXinLogon.h"
#include "QQLogon.h"
#include "WeiXinUserInfoManager.h"
#include "QQUserInfoManager.h"
#include "HYPay.h"
#include "AlipayManager.h"
enum HttpType
{
	HT_Recharge = 1,//����ע��
	HT_PhonePay = 2,//�ֻ�����
	HT_NormalPay = 3,//��ͨ��ֵ
	HT_HYPay = 4,//�㸶����ֵ
};
enum HttpClientType
{
	HCT_Logon		= 1,//������½
	HCT_PhoneSMS	= 2,//�ֻ�����
	HCT_PhonePay	= 3,//�ֻ�����
};

class FishServer : public INetHandler
{
public:
	FishServer();
	virtual ~FishServer();

	bool InitServer();
	bool MainUpdate();
	bool OnDestroy();

	void SetServerClose(){ m_IsClose = true; }
	void SetReloadConfig(){ m_IsReloadConfig = true; }
	void ShowInfoToWin(const char *pcStr, ...);

	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize);
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize);
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt);

	void SendNetCmdToClient(ServerClientData* pClient, NetCmd* pCmd);
	void SendNetCmdToCenterServer(NetCmd* pCmd);
	void SendNetCmdToDB(NetCmd* pCmd);
	void SendNetCmdToControl(NetCmd*pCmd);
	void SendNetCmdToLogDB(NetCmd* pCmd);
	void SendNetCmdToFtp(NetCmd* pCmd);

	ServerClientData* GetUserClientDataByIndex(BYTE IndexID);
	FishConfig&		GetFishConfig(){ return m_FishConfig; }
	HttpClient&		GetHttpClient(){ return m_HttpClient; }
	PhoneSMSManager& GetPhoneSMSMananger(){ return m_PhoneSMSManager; }
	BindPhoneManager& GetBindPhoneManager(){ return m_BindPhoneManager; }
	PhoneLogon& GetPhoneLogon(){ return m_PhoneLogon; }
	NormalRechargeManager& GetNormalRechargeManager(){ return m_NormalRechargeManager; }
	IOSPayManager& GetIOSPayManager(){ return m_IOSPayManager; }
	OrderOnlyManager& GetOrderOnlyManager(){ return m_OrderOnlyManager; }
	AlipayManager&		GetAlipayManager(){ return m_AlipayManager; }

	void			OnFinish();

	WORD			GetPhonePayID(){ return m_PhonePayID; }
	WORD			GetPhoneSMSID(){ return m_PhoneSMSID; }
	WORD			GetChannelLogonID(){ return m_ChannelLogonID; }
	WORD			GetNormalRechargeID(){ return m_NormalRechargeID; }
	WORD			GetIOSPayID(){ return m_IOSPayID; }
	WORD			GetIOSPayTestID(){ return m_IOSPayTestID; }
	WORD			GetWeiXinTokenID(){ return m_WeiXinTokenID; }
	WORD			GetQQTokenID(){ return m_QQTokenID; }

	WORD			GetWeiXinUserInfoID(){ return m_WeiXinUserInfoID; }
	WORD			GetWeiXinFaceImgID(){ return m_WeiXinFaceImgID; }
	//WORD			GetWeiXinTokenID(){ return m_WeiXinUserTokenID; }

	WORD			GetQQUserInfoID(){ return m_QQUserInfoID; }
	WORD			GetQQFaceImgID(){ return m_QQFaceImgID; }

	WORD			GetHyTokenID(){ return m_HYTokenID; }
private:
	bool ConnectClient();
	bool ConnectCenter();
	bool ConnectHttp();
	bool ConnectHttpClient();
	bool ConnectDB();
	bool ConnectControl();
	bool ConnectLogDB();
	bool ConnectFTP();

	void HandleAllMsg();
	bool HandleCenterMsg(NetCmd* pCmd);
	bool HandleHttpMsg(HttpRecvData* pHttpData);
	bool HandleDBMsg(NetCmd* pCmd);
	bool HandleFtpMsg(NetCmd* pCmd);
	bool HandleHttpClientMsg(HttpClientResult* pResult);

	bool HandleControlMsg(NetCmd* pCmd);

	void OnTcpServerLeave(BYTE ServerID, ServerClientData* pClient);
	void OnTcpServerJoin(BYTE ServerID, ServerClientData* pClient);
	void OnTcpClientConnect(TCPClient* pClient);
	void OnTcpClientLeave(TCPClient* pClient);

	bool HandleGameServerMsg(ServerClientData* pClient, NetCmd* pCmd);

	void OnAddClient();

	void OnReloadConfig();

	void UpdateInfoToControl(DWORD dwTimer);
private:
	Dump								m_pDump;
	//��������������
	volatile bool	 					m_IsClose;
	volatile bool						m_IsReloadConfig;

	FishConfig							m_FishConfig;
	

	NewServer							m_ClientTcp;//��GameServer�������
	HashMap<BYTE, ServerClientData*>	m_ClintList;//GameServer���б�

	BYTE								m_OperateNetworkID;

	//���������		��Ӫ��������Ҫ���ӵ����������ȥ
	TCPClient							m_CenterTcp;//���ӵ����������������
	bool								m_CenterTcpStates;

	TCPClient							m_DBTcp;
	bool								m_DBTcpStates;

	TCPClient							m_FTPTcp;
	bool								m_FTPTcpStates;

	//����
	BindPhoneManager					m_BindPhoneManager;//�ֻ���֤
	RealNameVerificationManager			m_RealNameVerificationManager;//���֤��
	BuyEntityManager					m_BuyEntityManager;//ʵ����Ʒ����
	BindEmailManager					m_BindEmailManager;
	UseRMBManager						m_UseRMBManager;

	SafeList<AfxNetworkClientOnce*>      m_AfxAddClient;

	ChannelLogonManager					m_ChannelLogon;
	PhonePayManager						m_PhonePayManager;

	WeiXinLogon							m_WeiXinLogon;
	QQLogon								m_QQLogon;

	WeiXinUserInfoManager				m_WeiXinUserInfo;
	QQUserInfoManager					m_QQUserInfo;

	//Http
	HttpServer							m_HttpServer;
	HttpClient							m_HttpClient;

	//HashMap
	WORD								m_ChannelLogonID;
	WORD								m_PhoneSMSID;
	WORD								m_PhonePayID;
	WORD								m_NormalRechargeID;
	WORD								m_IOSPayID;
	WORD								m_IOSPayTestID;

	WORD								m_WeiXinTokenID;
	WORD								m_QQTokenID;

	WORD								m_WeiXinUserInfoID;
	WORD								m_WeiXinFaceImgID;
	//WORD								m_WeiXinUserTokenID;

	WORD								m_QQUserInfoID;
	WORD								m_QQFaceImgID;

	WORD								m_HYTokenID;

	WORD								m_HYPayID;

	TCPClient									m_ControlTcp;
	bool										m_ControlIsConnect;

	TCPClient									m_LogDBTcp;
	bool										m_LogDBIsConnect;

	//���Ź�����
	PhoneSMSManager								m_PhoneSMSManager;
	PhoneLogon									m_PhoneLogon;

	//��ͨ��ֵ����
	NormalRechargeManager						m_NormalRechargeManager;

	//IOS��ֵ
	IOSPayManager								m_IOSPayManager;

	//Ψһ����������
	OrderOnlyManager							m_OrderOnlyManager;

	//�㸶����ֵ
	HYPay										m_HYPay;

	//
	AlipayManager								m_AlipayManager;
};
extern FishServer g_FishServer;