//#pragma once
//#include "NetServer.h"
//
////������Ӧ�շ������м���������Ӱ��Ƚϴ�
////1.SleepTime,�շ��̵߳�˯�߼��ʱ�䡣
////2.RecvThreadNum �����߳�����  
////3.SendThreadNum �����߳����� 
////4.�����������ܴ�ʱ���߳�Խ�࣬ÿ���̴߳������������Խ�٣�ÿ֡�ķѵ�ʱ���Խ�١����ķѵ�ʱ�����˯��ʱ��ʱ���̼߳�����һֱ��ѭ����������CPUռ����������
//
//struct TCPClientData : public ServerClientData
//{
//	SafeArray<NetCmd*, 512>	SendList; //���͵Ķ���
//	volatile long RefCount;
//	UINT	SendTick;
//	UINT	RecvTick;
//	UINT	Offset;
//	UINT	RecvSize;
//	BYTE    Buff[1];
//};
//struct ThreadRuntimeData
//{
//	volatile long	Num;
//	volatile float	Delta1;
//	volatile float  Delta2;
//};
//
//class TCPServerOld :public NetServer
//{
//public:
//	typedef vector<TCPClientData*> ClientVector;
//	typedef SafeArray<TCPClientData*>  ClientList;
//	TCPServerOld();
//	~TCPServerOld();
//
//	virtual bool Init(const ServerInitData &data);
//	virtual void Shutdown();
//	virtual bool Send(ServerClientData *pClient, NetCmd *pCmd);
//	virtual void SetCmdHandler(INetHandler *pHandler)
//	{
//		m_CmdHandler = pHandler;
//	}
//	virtual bool Kick(ServerClientData *pClient);
//	virtual UINT JoinNum()const
//	{
//		return (UINT)m_JoinNum;
//	}
//	virtual void SwitchAccept(bool bEnable)
//	{
//		m_bEnableAccept = bEnable;
//	}
//	void RecvThread();
//	void SendThread();
//	void AcceptThread();
//	void RecvThread2();
//	const ServerInitData& GetInitData()
//	{
//		return m_InitData;
//	}
//	const ThreadRuntimeData& GetRecvThreadData(ushort idx)
//	{
//		return m_RecvThreadData[idx];
//	}
//	const ThreadRuntimeData& GetSendThreadData(ushort idx)
//	{
//		return m_SendThreadData[idx];
//	}
//protected:
//	void AddRecvThreadNum(int idx)
//	{
//		::InterlockedIncrement(&m_RecvThreadData[idx].Num);
//	}
//	void RemoveRecvThreadNum(int idx)
//	{
//		::InterlockedDecrement(&m_RecvThreadData[idx].Num);
//	}
//	void AddSendThreadNum(int idx)
//	{
//		::InterlockedIncrement(&m_SendThreadData[idx].Num);
//	}
//	void RemoveSendThreadNum(int idx)
//	{
//		::InterlockedDecrement(&m_SendThreadData[idx].Num);
//	}
//	void	AddJoinNum()
//	{
//		::InterlockedIncrement(&m_JoinNum);
//	}
//	void	RemoveJoinNum()
//	{
//		::InterlockedDecrement(&m_JoinNum);
//	}
//	bool	RecvData(TCPClientData *pscd, int nRecvSize);
//	void	RemoveClient(TCPClientData *pscd);
//	void	RemoveClientImmediate(TCPClientData *pscd);
//	ushort	GetRecvThreadMin();
//	ushort	GetSendThreadMin();
//
//	volatile bool			m_bEnableAccept;
//	volatile long			m_RecvIndex;
//	volatile long			m_SendIndex;
//	volatile long			m_ExitCount;
//	volatile bool			m_bRun;
//	volatile long			m_JoinNum;
//
//	ThreadRuntimeData		m_RecvThreadData[MAX_SOCKET_THREAD];
//	ThreadRuntimeData		m_SendThreadData[MAX_SOCKET_THREAD];
//	ClientList				m_NewRecvList[MAX_SOCKET_THREAD];
//	ClientList				m_NewSendList[MAX_SOCKET_THREAD];
//
//	INetHandler				*m_CmdHandler;
//	SOCKET					 m_Socket;
//	ServerInitData			 m_InitData;
//	NetCmd					 m_HeartCmd;
//};