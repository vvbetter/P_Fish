#pragma once

struct ClientInitData
{
	uint		Timeout;	//��ʱ(����)
	ushort		Port;		//�˿�
	uint		BuffSize;	//��������С
	uint		SocketRecvSize;
	uint		SocketSendSize;
	uint		CmdHearbeat;
	uint		SleepTime;
	UINT		RecvArraySize;
	UINT		SendArraySize;
	uint		ThreadNum;	//����ʹ��
};

class NetClient
{
public:
	virtual bool Init(const char *ip, const ClientInitData &data, void *pData = NULL, int sendSize = 0) = 0;
	virtual bool Send(NetCmd *pCmd, bool bNoneUse) = 0;
	virtual void Shutdown() = 0;
	virtual bool IsConnected()const = 0;
	virtual NetCmd *GetCmd() = 0;
};