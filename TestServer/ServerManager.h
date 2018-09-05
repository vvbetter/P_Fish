#pragma once
#include "Common.h"

class ServerManager : public INetHandler
{
public:
	vector<ServerClientData*> m_ClientList;
	virtual bool CanConnected(uint ip, short port)
	{
		return true;
	}
	virtual bool CheckClientData(NetCmd *pCmd, uint ip, short port)
	{
		return true;
	}
	virtual bool NewClient(ServerClientData *pClient)
	{
		m_NewClientList.AddItem(pClient);
		return true;
	}
	
	void Init();
	void Update();
	TCPServer &GetServer()
	{
		return m_Server;
	}
	void Close();
protected:
	SafeList<ServerClientData*> m_NewClientList;

	
	TCPServer m_Server;
};