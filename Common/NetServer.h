#pragma once
#define SERVER_CONNECT_MAGIC 0x1343316f
#define MAX_SOCKET_THREAD 16
#define IS_ANSWER(X) ((X & 0x80000000) != 0)
#define IS_GW_ANSWER(X) ((X & 0xc0000000) == 0xc0000000)
#define RESET_ANSWER(X) ((X) &= 0x7fffffff)
#define RESET_GW_ANSWER(X) ((X) &= 0x3fffffff)
#define SET_ANSWER(X) ((X) | 0x80000000)
#define SET_GW_ANSWER(X) ((X) | 0xc0000000)
#define CMD_ID_SIZE (sizeof(UINT))
#define HEARBEAT_ID		0xffffffff
#define PING_ID			0x8fffffff

#define UDP_RESEND_TICK		35	//UDP�ط���ʱms

inline void DebugMemeryData(void* pBuff, const UINT nSize)
{
#ifdef _DEBUG
	for (UINT i = 0; i < nSize; ++i)
	{
		std::cout << (int)*((char*)pBuff + i) << " ";
	}
	std::cout << endl;
#endif
	return;
}

inline bool IsDisconnect(INT ret, UINT code)
{
	return (ret == SOCKET_ERROR && (code == WSAECONNRESET || code == WSAECONNABORTED));
}
enum ServerClientState
{
	CLIENT_STATE_ACCEPT,
	CLIENT_STATE_CHECKED,
	CLIENT_STATE_JOINED,
	CLIENT_STATE_REMOVE,
};
enum RemoveType
{
	REMOVE_NONE,
	REMOVE_NORMAL,				//�ϲ��Ƴ�
	REMOVE_RECV_ERROR,			//���մ���
	REMOVE_SEND_ERROR,			//���ʹ���
	REMOVE_TIMEOUT,				//��ʱ
	REMOVE_CMD_SEND_OVERFLOW,	//���Ͷ����������������̫��
	REMOVE_CMD_RECV_OVERFLOW,	//���ն����������������̫��
	REMOVE_CMD_SIZE_ERROR,		//���յ������С����
	REMOVE_RECVBACK_NOT_SPACE,	//
	REMOVE_RELOGON,				//�ص�½
};
inline const char *GetRemoveTypeString(RemoveType rt)
{
	switch (rt)
	{
	case REMOVE_NONE:
		return "REMOVE_NONE";
	case REMOVE_NORMAL:
		return "REMOVE_NORMAL";
	case REMOVE_RECV_ERROR:
		return "REMOVE_RECV_ERROR";
	case REMOVE_SEND_ERROR:
		return "REMOVE_SEND_ERROR";
	case REMOVE_TIMEOUT:
		return "REMOVE_TIMEOUT";
	case REMOVE_CMD_SEND_OVERFLOW:
		return "REMOVE_CMD_SEND_OVERFLOW";
	case REMOVE_CMD_RECV_OVERFLOW:
		return "REMOVE_CMD_RECV_OVERFLOW";
	case REMOVE_CMD_SIZE_ERROR:
		return "REMOVE_CMD_SIZE_ERROR";
	case REMOVE_RECVBACK_NOT_SPACE:
		return "REMOVE_RECVBACK_NOT_SPACE";
	case REMOVE_RELOGON:
		return "REMOVE_RELOGON";
	default:
		return "REMOVE_OTHER";
	}
}
inline void GetIPString(UINT ip, char xx[30])
{
	sprintf_s(xx, 30, "%d.%d.%d.%d",
		(ip >> 0) & 0xff,
		(ip >> 8) & 0xff,
		(ip >> 16) & 0xff,
		(ip >> 24) & 0xff
		);
}
inline void GetIPString(UINT ip, USHORT port, char xx[30])
{
	sprintf_s(xx, 30, "%d.%d.%d.%d:%d",
		(ip >>  0) & 0xff,
		(ip >>  8) & 0xff,
		(ip >> 16) & 0xff,
		(ip >> 24) & 0xff,
		port
		);
}
//�ͻ���������Ϣ
struct ServerClientData
{
	ServerClientData(UINT recvCount) :RecvList(recvCount)
	{

	}
	//������Ϣ
	CACHELINE(1);
	volatile bool		Removed;			//�Ƿ���Ϊ�Ƴ�
	CACHELINE(2);
	volatile RemoveType	RemoveCode;			//�ͻ��˶Ͽ��ı��
	CACHELINE(3);
	volatile bool		IsInScene;			//�ڳ�����ΪTRUE������ΪFALSE
	CACHELINE(4);
	SOCKET				Socket;
	UINT				IP;
	USHORT				Port;
	SafeArray<NetCmd*>	RecvList;			//���յĶ���
	UINT_PTR			OutsideExtraData;	//�ⲿ���ӵ�����
	DWORD				dwChecked;			//��֤
	INT64				Uid;
	BYTE				ClientNetId;
};

struct ServerInitData
{
	BYTE		ServerID;				//������ʶ����
	INT			Timeout;				//���ճ�ʱ(����)
	ushort		Port;					//�˿�
	ushort		ListenCount;			//�����Ŷӵ�����	//1 - 64
	uint		BuffSize;				//���ջ�������С, ����ܽ��յ������
	uint		MaxSendCountPerFrame;	//ÿ֡��෢�Ͷ��ٸ�����
	uint		MaxAcceptNumPerFrame;	//ÿ��Accept����ܽ��ܶ��ٸ�����;
	uint		SocketRecvSize;			//�ײ㻺������С
	uint		SocketSendSize;			//�ײ㻺������С
	uint		MaxRecvCmdCount;		//����ͬʱ���ն��ٸ�������˾��߳�
	uint		MaxSendCmdCount;		//����ͬʱ���Ͷ��ٸ�������˾��߳�
	uint		SleepTime;				//���ͺͽ����߳�˯�ߵļ��
	uint		AcceptSleepTime;		//Accept˯��ʱ��
	ushort		CmdHearbeat;			//����������
	ushort		RecvThreadNum;			//�����̵߳�����
	ushort		SendThreadNum;			//�����̵߳�����
	bool		AcceptRecvData;			//Accept�������ݲŴ�����
	ushort		SceneHearbeatTick;		//�ڳ����е�����TICK���˲�����Ҫ���Կͻ��˵�DelayACK���������ι��ܡ�ȡֵ��Χ:(50 - 100)
	ServerInitData()
	{
		memset(this, 0, sizeof(*this));
		SceneHearbeatTick = 100;
	}
};
#define CONNECT_CHECK_OK	 (0u)
#define CONNECT_CHECK_FAILED (0xffffffffu)
class INetHandler
{
public:
	//��Щ�ص������̴߳���
	//�Ƿ������µĿͻ��������������:
	//����CONNECT_CHECK_OK��ʾ����;
	//����CONNECT_CHECK_FAILED��ʾ������;
	//����>0��ʾ����������pData�е����ݷ��͸��ͻ��ˡ�
	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize) = 0;
	//�µĿͻ��˼��룬�Լ���ʼ���յ����������ݡ�
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize) = 0;
	//�ͻ��˶Ͽ����Ӽ��Ͽ����ӵ����͡�GetRemoveTypeString(rt)��ȡ�ַ���������
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
	{
		UNREFERENCED_PARAMETER(ServerID);
		UNREFERENCED_PARAMETER(pClient);
		UNREFERENCED_PARAMETER(rt);
	};
};

class NetServer
{
public:
	virtual ~NetServer(){};
	virtual bool Init(const ServerInitData &data, bool bTCP = true) = 0;
	virtual bool Kick(ServerClientData *pClient, RemoveType rt = REMOVE_NORMAL) = 0;
	virtual void Shutdown() = 0;
	virtual void SetCmdHandler(INetHandler *pHandler) = 0;
	virtual bool Send(ServerClientData *pClient, NetCmd *pCmd) = 0;
	virtual UINT JoinNum()const = 0;
	virtual void SwitchAccept(bool bEnable) = 0;
};