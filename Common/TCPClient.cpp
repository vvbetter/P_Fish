#include "stdafx.h"
#pragma warning(disable:4365) 
#include <WS2tcpip.h>
#include "NetClient.h"
#include "ProtoBufferCoder.h"

#define SLEEP_TIME 1
static UINT WINAPI ThreadFunc1(void *p)
{
	((TCPClient*)p)->RecvThread();
	return 0;
}
static UINT WINAPI ThreadFunc2(void *p)
{
	((TCPClient*)p)->SendThread();
	return 0;
}
TCPClient::TCPClient()
{
	m_ExitCount = 0;
	m_bConnected = false;

	m_Offset = 0;
	m_RecvSize = 0;
	m_Buff = NULL;
	m_Socket = NULL;
}
TCPClient::~TCPClient()
{

}
bool TCPClient::Init(const char *ip, const ClientInitData &data, void *pData, int sendSize)
{
	m_InitData = data;
	m_Buff = (BYTE*)malloc(m_InitData.BuffSize);
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_Socket == INVALID_SOCKET)
	{
		SAFE_FREE(m_Buff);
		return false;
	}

	m_RecvList = new SafeCmdList(data.RecvArraySize);
	m_SendList = new SafeCmdList(data.SendArraySize);

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	inet_pton(AF_INET, ip, &addr.sin_addr.s_addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_InitData.Port);
	timeval time = { 0, 3 * 1000 };
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&data.SocketRecvSize, sizeof(uint));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&data.SocketSendSize, sizeof(uint));
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
	long noDelay = 1;
	int len = sizeof(long);
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay, len);
	bool bcheck = (sendSize > 0 && pData != NULL);
	int ret = connect(m_Socket, (const struct sockaddr *) &addr, sizeof(addr));
	if (ret == 0)
	{
		if (bcheck)
		{
			int ret = send(m_Socket, (char*)pData, sendSize, 0);
			if (ret != sendSize)
			{
				closesocket(m_Socket);

				SAFE_FREE(m_Buff);
				SAFE_DELETE(m_RecvList);
				SAFE_DELETE(m_SendList);
				return false;
			}
			//�ȴ����
			char buff[128];
			ret = recv(m_Socket, buff, sizeof(buff), 0);
			if (ret > 0)
			{
				UINT result = *((UINT*)buff);
				if (result != SERVER_CONNECT_MAGIC)
				{
					closesocket(m_Socket);

					SAFE_FREE(m_Buff);
					SAFE_DELETE(m_RecvList);
					SAFE_DELETE(m_SendList);
					return false;
				}
			}
			else
			{
				Log("Client recv ERROR! recv size:%d error code:%d", ret, WSAGetLastError());
				closesocket(m_Socket);

				SAFE_FREE(m_Buff);
				SAFE_DELETE(m_RecvList);
				SAFE_DELETE(m_SendList);
				return false;
			}
		}
		Log("�ͻ���������(SendBuff:%d, RecvBuff:%d, SendCmdCount:%d, RecvCmdCount:%d, Timeout:%d",
			m_InitData.SocketSendSize,
			m_InitData.SocketRecvSize,
			m_InitData.SendArraySize,
			m_InitData.RecvArraySize,
			m_InitData.Timeout
			);
		timeval time = { 0, m_InitData.Timeout * 1000 };
		setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
		m_bConnected = true; 
		m_RecvTick = timeGetTime();
		::_beginthreadex(0, 0, (ThreadFunc1), this, 0, 0);
		::_beginthreadex(0, 0, (ThreadFunc2), this, 0, 0);
		return true;
	}
	else
	{
		Log("���� %s:%d ʧ��:%d", ip, m_InitData.Port, WSAGetLastError());
		SAFE_FREE(m_Buff);
		SAFE_DELETE(m_RecvList);
		SAFE_DELETE(m_SendList);

		return false;
	}
}
void TCPClient::SendThread()
{
	UINT tick = timeGetTime();
	bool b10035;
	UINT hearbeat = HEARBEAT_ID;
	int timeout = m_InitData.Timeout >> 1;
	while (m_bConnected)
	{
		bool bSend = false;
		uint curTick = timeGetTime();
		while (m_SendList->HasItem())
		{
			NetCmd *pCmd = m_SendList->GetItemNoRemove();
			//Log("Client SendCmd, size:%d, type:%d", pCmd->GetCmdSize(), pCmd->GetCmdType());
			if (!SendImmediate(pCmd, pCmd->GetCmdSize(), b10035))
			{
				if (!b10035)
				{
					m_bConnected = false;
					break;
				}
			}
			else
			{
				tick = curTick;
				free(m_SendList->GetItem());
				bSend = true;
			}
		}
		if (bSend)
			tick = curTick;
		else if (int(curTick - tick) > timeout)
		{
			bool bret = uint(send(m_Socket, (char*)&hearbeat, sizeof(hearbeat), 0) == sizeof(hearbeat));
			if (!bret)
			{
				Log("client hearbeat send error,err code %d", WSAGetLastError());
			}
			tick = curTick;
		}
		Sleep(m_InitData.SleepTime);
	}
	::InterlockedIncrement(&m_ExitCount);
}
void TCPClient::RecvThread()
{
	while (m_bConnected)
	{
		int curPos = m_Offset + m_RecvSize;
		int nRet = recv(m_Socket, (char*)m_Buff + curPos, m_InitData.BuffSize - curPos, 0);
		if (nRet == 0 || (nRet == SOCKET_ERROR && (WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAECONNABORTED)))
		{
			Log(L"Recv size Error:%d, Error code:%d", nRet, WSAGetLastError());
			m_bConnected = false;
			break;
		}
		m_RecvTick = timeGetTime();
		m_RecvSize += nRet;
		while (m_RecvSize >= sizeof(UINT))
		{
			BYTE *pBuff = m_Buff + m_Offset;
			USHORT cmdSize = ((NetCmd*)pBuff)->GetCmdSize();
			USHORT recvID = ((NetCmd*)pBuff)->GetCmdType();
			//Log(L"TCPClient Recv data: cmdSize=%d,recvID=%d",cmdSize,recvID);
			if (*((UINT*)(pBuff)) == HEARBEAT_ID)
			{
				m_Offset += sizeof(UINT);
				m_RecvSize -= sizeof(UINT);
			}
			else if (*((UINT*)(pBuff)) == PING_ID)
			{
				m_Offset += sizeof(UINT);
				m_RecvSize -= sizeof(UINT);
			}
			else if (m_RecvSize >= sizeof(NetCmd))
			{
				if (cmdSize > m_InitData.BuffSize)
				{
					Log(L"RecvSize Error:%d", cmdSize);
					m_bConnected = false;
					break;
				}
				if (m_RecvSize >= cmdSize)
				{
					bool isPBC = true;
					NetCmd* pCmdRecv = PBC_Decode(recvID, (char*)pBuff, cmdSize, isPBC);
					if (pCmdRecv != NULL && pCmdRecv->GetCmdType() != m_InitData.CmdHearbeat)
					{
						if (m_RecvList->HasSpace())
							m_RecvList->AddItem(pCmdRecv);
						else
							Log("*���ն�������:Size;%d, Count:%d, CmdSize:%d*", m_RecvList->ArraySize(), m_RecvList->Count(), cmdSize);
					}
					m_Offset += (isPBC == true ? cmdSize + sizeof(NetCmd) : cmdSize);
					m_RecvSize -= (isPBC == true ? cmdSize + sizeof(NetCmd) : cmdSize);
				}
				else
					break;
			}
			else
				break;
		}
		UINT freeBuffSize = m_InitData.BuffSize - (m_Offset + m_RecvSize);
		if (freeBuffSize < 128)
		{
			memmove(m_Buff, m_Buff + m_Offset, m_RecvSize);
			m_Offset = 0;
		}
		Sleep(m_InitData.SleepTime);
	}
	::InterlockedIncrement(&m_ExitCount);
}
bool TCPClient::Send(NetCmd *pCmd, bool bNoneUse)
{
	UNREFERENCED_PARAMETER(bNoneUse);
	if (!IsConnected())
		return false;
	if (m_SendList->HasSpace() == false)
	{
		Log("��������ʧ�ܣ����Ͷ�������:%d, %d", m_SendList->ArraySize(), m_SendList->Count());
		return false;
	}
	NetCmd *pNewCmd = CreateCmd(pCmd->GetCmdType(),pCmd->GetCmdSize());
	if (!pNewCmd)
		return false;
	memcpy_s(pNewCmd, pCmd->GetCmdSize(), pCmd, pCmd->GetCmdSize());
	m_SendList->AddItem(pNewCmd);
	return true;
}
bool TCPClient::SendImmediate(void *pData, UINT size, bool &b10035)
{
	NetCmd* pCmd = (NetCmd*)pData;
	//Log(L"Send data:size %d, type:%d", pCmd->GetCmdSize(), pCmd->GetCmdType());
	uint datalen = 0;
	bool isPBC = true;
	char* sendData = PBC_Encode(pCmd, datalen, isPBC);
	bool bret = uint(send(m_Socket, sendData, datalen, 0) == datalen);
	if (isPBC == true)
	{
		free(sendData);
	}
	if (!bret)
	{
		UINT code = WSAGetLastError();
		b10035 = code == 10035;
		Log(L"Send err:%d, LastError:%d", bret, code);
		return false;
	}
	return true;
}
void TCPClient::Shutdown()
{
	if (m_bConnected)
	{
		m_ExitCount = 0;
		m_bConnected = false;
		closesocket(m_Socket);
		while (m_ExitCount != 2)
			Sleep(SLEEP_TIME);
	}
	ReleaseData();
	SAFE_FREE(m_Buff);

}
void TCPClient::ReleaseData()
{
	while (m_RecvList->HasItem())
		free(m_RecvList->GetItem());
	while (m_SendList->HasItem())
		free(m_SendList->GetItem());
	SAFE_DELETE(m_RecvList);
	SAFE_DELETE(m_SendList);
}
