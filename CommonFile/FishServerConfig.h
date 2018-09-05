#pragma once
#include "TinyXml\XmlReader.h"
//#define IP_MAX_LENGTH 32
#define FTP_PATH 256
#define MySQLLength 128
#define FileXmlStrLength 256
#define IApppayPrivateKeyLength 1024
struct CenterServerConfig 
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;
	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		TimeOut;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	char		CenterListenIP[IP_MAX_LENGTH+1];//IP
	WORD		CenterListenPort;//Port
	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID

	DWORD		BeginUserID;
	DWORD		EndUserID;

	//�Կ��ƶ˵�����
	//BYTE		ControlNetworkID;//����ID
	//DWORD		ControlPassword;
	//char		ControlListenIP[IP_MAX_LENGTH + 1];//IP
	//WORD		ControlListenPort;//Port
	//DWORD		ControlBuffSize;
	//WORD		ControlListenCount;
	//DWORD		ControlMaxSendCountPerFrame;
	//DWORD		ControlMaxAcceptNumPerFrame;
	//DWORD		ControlMaxRecvCmdCount;
	//DWORD		ControlMaxSendCmdCount;
	//WORD		ControlSceneHearbeatTick;
	//DWORD		ControlRecvBuffSize;
	//DWORD		ControlSendBuffSize;
	//WORD		ControlRecvThreadSum;
	//WORD		ControlSendThreadSum;
	//DWORD		ControlSleepTime;
	//DWORD		ControlAcceptSleepTime;
	//DWORD		ControlTimeOut;

	//other
	BYTE		DBNetworkID;
};
struct DBConnectionInfo
{
	//MySQL		��������
	char		MySQlIP[MySQLLength + 1];
	char		MySQlUserName[MySQLLength + 1];
	char		MySQlUserPassword[MySQLLength + 1];
	char		MySQlDBName[MySQLLength + 1];
};
struct DBServerConfig 
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	/*WORD		RecvMaxSize;
	WORD		SendMaxSize;*/
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;

	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		DBListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		DBListenPort;//Port
	vector<DBConnectionInfo> DBConnectionVec;
};
struct FTPServerConfig 
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	/*WORD		RecvMaxSize;
	WORD		SendMaxSize;*/
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;

	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		FTPListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		FTPListenPort;//Port
	char		FTPFilePath[FTP_PATH + 1];

	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID
};
struct LogonServerConfig
{
	BYTE		LogonID;
	//�Կͻ���
	BYTE		ClientNetworkID;
	char		LogonClientListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		LogonClientListenPort;//Port
	DWORD		LogonClientBuffSize;
	/*WORD		LogonClientRecvMaxSize;
	WORD		LogonClientSendMaxSize;*/
	WORD		LogonClientListenCount;
	DWORD		LogonClientMaxSendCountPerFrame;
	DWORD		LogonClientMaxAcceptNumPerFrame;
	DWORD		LogonClientMaxRecvCmdCount;
	DWORD		LogonClientMaxSendCmdCount;
	WORD		LogonClientSceneHearbeatTick;


	DWORD		LogonClientRecvBuffSize;
	DWORD		LogonClientSendBuffSize;
	WORD		LogonClientRecvThreadSum;
	WORD		LogonClientSendThreadSum;
	DWORD		LogonClientSleepTime;
	DWORD		LogonClientAcceptSleepTime;
	DWORD		LogonClientTimeOut;
	//��GameServer
	//BYTE		ServerNetworkID;//����ID
	//char		LogonServerListenIP[IP_MAX_LENGTH + 1];//IP
	//WORD		LogonServerListenPort;//Port
	//DWORD		LogonServerBuffSize;
	///*WORD		LogonServerRecvMaxSize;
	//WORD		LogonServerSendMaxSize;*/
	//WORD		LogonServerListenCount;
	//DWORD		LogonServerMaxSendCountPerFrame;
	//DWORD		LogonServerMaxAcceptNumPerFrame;
	//DWORD		LogonServerMaxRecvCmdCount;
	//DWORD		LogonServerMaxSendCmdCount;
	//WORD		LogonServerSceneHearbeatTick;

	//DWORD		LogonServerRecvBuffSize;
	//DWORD		LogonServerSendBuffSize;
	//WORD		LogonServerRecvThreadSum;
	//WORD		LogonServerSendThreadSum;
	//DWORD		LogonServerSleepTime;
	//DWORD		LogonServerAcceptSleepTime;
	//DWORD		LogonServerTimeOut;

	BYTE		DBNetworkID;
	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID
};
struct RankServerConfig
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	/*WORD		RecvMaxSize;
	WORD		SendMaxSize;*/
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;

	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		RankListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		RankListenPort;//Port

	BYTE		DBNetworkID;
	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID
};
struct GameServerConfig
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	/*WORD		RecvMaxSize;
	WORD		SendMaxSize;*/
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;

	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		GameListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		GameListenPort;//Port
	BYTE		DBNetworkID;
	BYTE		SaveDBNetworkID;
	BYTE		LogDBNetworkID;//��¼LOG�����ݿ�
	DWORD		TestRobotNumber;
	DWORD		BeginRobotUserID;
	DWORD		EndRobotUserID;
};
struct RobotConfig
{
	//�����˵�����
	BYTE	UserLogonID;//ʹ�õ�Logon��ID
	DWORD	OpenRobotSum;//���������˵�����
	DWORD	BeginRobotSum;
};
struct OperateConfig
{
	//��Ӫ������
	BYTE		NetworkID;
	DWORD		BuffSize;
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;

	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		OperateListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		OperateListenPort;//Portd
	//���ݿ�
	BYTE		DBNetworkID;
	//���������
	WORD		HttpPort;
	DWORD		HttpSleepTime;
	DWORD		HttpThreadSum;
	DWORD		HttpTimeOut;
	char		ProductSecret[FileXmlStrLength];//����Ʒ����
	//Sms ���ŵ�����
	char		SMSUserName[FileXmlStrLength];
	char		SMSPassword[FileXmlStrLength];
	char		SMSInfo[FileXmlStrLength];
	//���ѳ�ֵ
	char		PhonePayPartNerID[FileXmlStrLength];
	char        PhonePayOrgcode[FileXmlStrLength];
	char        PhonePayAgentId[FileXmlStrLength];
	char		PhonePayPassword[FileXmlStrLength];
	char		PhonePayPayPassword[FileXmlStrLength];
	char		PhonePayIP[FileXmlStrLength];
	char		PhonePayKey[FileXmlStrLength];
	WORD		PhonePayPort;
	//HttpClient
	DWORD		HttpClientThreadNum;
	DWORD		HttpClientTimeout;
	DWORD		HttpClientSleepTime;
	//IApppay
	char		ApppayID[FileXmlStrLength];
	char		ApppayKey[FileXmlStrLength];
	char		ApppayPrvKey[IApppayPrivateKeyLength];
	//WeiXin
	char		WeiXinAppID[FileXmlStrLength];
	char		WeiXinSecret[FileXmlStrLength];
	//QQ
	char		QQAppID[FileXmlStrLength];
	char		QQKey[FileXmlStrLength];
	//HYType
	DWORD		HYAgentID;
	char		HYKey[FileXmlStrLength];
	//
	bool		OpenTest;

	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID
};
//struct GateConfig
//{
//	//���ط�����
//	BYTE		NetworkID;
//
//	WORD		PingPort;
//	DWORD		SleepTime;
//	DWORD		RecvBuffSize;
//	DWORD		SendBuffSize;
//	//Logon
//	BYTE		LogonNetworkID;
//	char		LogonListenIP[IP_MAX_LENGTH + 1];//IP
//	DWORD		LogonBuffSize;
//	DWORD		LogonThreadSum;
//	WORD		LogonServerPort;
//	WORD		LogonClientPort;
//	DWORD		LogonTimeOut;
//	WORD		LogonAcceptPerSec;
//	//Game 
//	BYTE		GameNetworkID;
//	char		GameListenIP[IP_MAX_LENGTH + 1];//IP
//	DWORD		GameBuffSize;
//	DWORD		GameThreadSum;
//	WORD		GameServerPort;
//	WORD		GameClientPort;
//	DWORD		GameTimeOut;
//	WORD		GameAcceptPerSec;
//};
struct GateWay
{
	DWORD		ChannelID;
	DWORD		Ip;
	WORD		Port;
};
struct ControlServerConfig
{
	BYTE		ControlID;

	BYTE		ControlServerNetworkID;//����ID
	DWORD		ControlServerBuffSize;
	WORD		ControlServerListenCount;
	DWORD		ControlServerMaxSendCountPerFrame;
	DWORD		ControlServerMaxAcceptNumPerFrame;
	DWORD		ControlServerMaxRecvCmdCount;
	DWORD		ControlServerMaxSendCmdCount;
	WORD		ControlServerSceneHearbeatTick;
	DWORD		ControlServerRecvBuffSize;
	DWORD		ControlServerSendBuffSize;
	WORD		ControlServerRecvThreadSum;
	WORD		ControlServerSendThreadSum;
	DWORD		ControlServerTimeOut;
	DWORD		ControlServerSleepTime;
	DWORD		ControlServerAcceptSleepTime;
	char		ControlServerListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		ControlServerListenPort;//Port
	//�Կ��ƶ˵�Control����
	BYTE		ControlClientNetworkID;//����ID
	DWORD		ControlClientPassword;
	char		ControlClientListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		ControlClientListenPort;//Port
	DWORD		ControlClientBuffSize;
	WORD		ControlClientListenCount;
	DWORD		ControlClientMaxSendCountPerFrame;
	DWORD		ControlClientMaxAcceptNumPerFrame;
	DWORD		ControlClientMaxRecvCmdCount;
	DWORD		ControlClientMaxSendCmdCount;
	WORD		ControlClientSceneHearbeatTick;
	DWORD		ControlClientRecvBuffSize;
	DWORD		ControlClientSendBuffSize;
	WORD		ControlClientRecvThreadSum;
	WORD		ControlClientSendThreadSum;
	DWORD		ControlClientSleepTime;
	DWORD		ControlClientAcceptSleepTime;
	DWORD		ControlClientTimeOut;
	//other
	BYTE		DBNetworkID;
	BYTE		LogDBID;//��Ӧ��LOG���ݿ��ID
};
struct MiniGameServerConfig
{
	BYTE		NetworkID;//����ID
	DWORD		BuffSize;
	WORD		ListenCount;
	DWORD		MaxSendCountPerFrame;
	DWORD		MaxAcceptNumPerFrame;
	DWORD		MaxRecvCmdCount;
	DWORD		MaxSendCmdCount;
	WORD		SceneHearbeatTick;
	DWORD		RecvBuffSize;
	DWORD		SendBuffSize;
	WORD		RecvThreadSum;
	WORD		SendThreadSum;
	DWORD		SleepTime;
	DWORD		AcceptSleepTime;
	DWORD		TimeOut;
	char		GameListenIP[IP_MAX_LENGTH + 1];//IP
	WORD		GameListenPort;//Port
	BYTE		DBNetworkID;
	BYTE		LogDBNetworkID;//��¼LOG�����ݿ�

	DWORD		BeginUserID;
	DWORD		EndUserID;
};
class FishServerConfig
{
public:
	FishServerConfig();
	virtual ~FishServerConfig();

	bool LoadServerConfigFile();

	CenterServerConfig* GetCenterServerConfig(){ return &m_CenterConfig; }
	FTPServerConfig* GetFTPServerConfig(){ return &m_FtpConfig; }

	RankServerConfig* GetRankServerConfig(){ return &m_RankConfig; }

	DBServerConfig*	GetDBServerConfig(BYTE DBNetworkID);
	GameServerConfig* GetGameServerConfig(BYTE GameNetworkID);
	LogonServerConfig* GetLogonServerConfig(BYTE LogonID);
	RobotConfig* GetRobotConfig(){ return &m_RobotConfig; }
	OperateConfig* GetOperateConfig(){ return &m_OperateConfig; }
	//GateConfig* GetGateConfig(BYTE GateConfigID);
	GateWay* GetGateWay(DWORD ChannelID);

	ControlServerConfig* GetControlServerConfig(){ return &m_ControlServerConfig; }
	MiniGameServerConfig* GetMiniGameServerConfig(){ return &m_MiniGameServerConfig; }

	bool IsLogonServerConfigID(BYTE ConfigID);
	bool IsGameServerConfigID(BYTE ConfigID);

	bool GetIsOperateTest();
private:
	bool LoadCenterConfig(WHXmlNode* pFishConfig);
	bool LoadDBConfig(WHXmlNode* pFishConfig);
	bool LoadGameServerConfig(WHXmlNode* pFishConfig);
	bool LoadLogonConfig(WHXmlNode* pFishConfig);
	bool LoadRankConfig(WHXmlNode* pFishConfig);
	bool LoadFTPConfig(WHXmlNode* pFishConfig);
	bool LoadRobotConfig(WHXmlNode* pFishConfig);
	bool LoadOperateConfig(WHXmlNode* pFishConfig);
	//bool LoadGateConfig(WHXmlNode* pFishConfig);
	bool LoadGateWayConfig(WHXmlNode* pFishConfig);
	bool LoadControlServerConfig(WHXmlNode* pFishConfig);
	bool LoadMiniGameServerConfig(WHXmlNode* pFishConfig);
private:
	CenterServerConfig			m_CenterConfig;
	HashMap<BYTE, DBServerConfig> m_DBConfigMap;
	FTPServerConfig				m_FtpConfig;
	HashMap<BYTE, LogonServerConfig>	m_LogonConfigMap;
	RankServerConfig			m_RankConfig;
	HashMap<BYTE, GameServerConfig> m_GameConfigMap;
	RobotConfig					m_RobotConfig;
	OperateConfig				m_OperateConfig;
	//HashMap<BYTE, GateConfig>	m_GateConfigMap;//���صļ���
	HashMap<DWORD, GateWay>		m_GateWayMap;
	ControlServerConfig			m_ControlServerConfig;
	MiniGameServerConfig		m_MiniGameServerConfig;
};
extern FishServerConfig g_FishServerConfig;