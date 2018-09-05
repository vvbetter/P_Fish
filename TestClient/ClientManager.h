#pragma once

class ClientManager
{
public:
	TestClient m_Client;
	vector<TCPClient*> m_ClientList;
	bool Init(const char *ip, int num); 
	void ThreadProcedure();
	void Close();

protected:
	int m_Num;
	char m_IP[20];


};