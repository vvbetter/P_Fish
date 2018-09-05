// DBServer.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "tinyxml2.h"
class MyHandler : public INetHandler
{
	virtual uint CanConnected(BYTE SeverID, uint ip, short port, void *pData, uint recvSize)
	{
		return CONNECT_CHECK_OK;
	}
	//新的客户端加入，以及初始接收到的连接数据。
	virtual bool NewClient(BYTE SeverID, ServerClientData *pClient, void *pData, uint recvSize)
	{
		return true;
	}
	//客户端断开连接及断开连接的类型。GetRemoveTypeString(rt)获取字符串描述。
	virtual void Disconnect(BYTE ServerID, ServerClientData *pClient, RemoveType rt)
	{
		UNREFERENCED_PARAMETER(ServerID);
		UNREFERENCED_PARAMETER(pClient);
		UNREFERENCED_PARAMETER(rt);
	};
};
MyHandler g_handler;
int _tmain(int argc, _TCHAR* argv[])
{
	tinyxml2::XMLDocument xml_doc;
	string LocalIP;
	string LogonIP;
	string OptIP;
	ushort GamePort;
	ushort LogonPort;
	ushort OptPort;
	uint   timeout = 1000 * 60;
	bool bOK = false;
	if (xml_doc.LoadFile("./FishServer.xml") == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* xml_root = xml_doc.FirstChildElement("FishServer")->FirstChildElement("GateWays");
		if (xml_root)
		{
			LocalIP = xml_root->Attribute("LocalIP");
			LogonIP = xml_root->Attribute("LogonServerIP");
			OptIP = xml_root->Attribute("OperationIP");
			GamePort = atoi(xml_root->Attribute("GamePort"));
			LogonPort = atoi(xml_root->Attribute("LogonPort"));
			OptPort = atoi(xml_root->Attribute("OperationPort"));
			timeout = atoi(xml_root->Attribute("Timeout"));
			bOK = true;
		}
	}
	if (!bOK)
	{
		Log("加载配置文件:FishServer.xml失败.");
		system("pause");
		return 0;
	}

	GWInitData data;
	data.ServerID = 0;
	data.Timeout = timeout;
	data.Port = GamePort;
	data.ListenCount = 100;
	data.BuffSize = 10240;
	data.MaxSendCountPerFrame = 1;
	data.MaxAcceptNumPerFrame = 100;
	data.SocketRecvSize = 10240;
	data.SocketSendSize = 10240;
	data.MaxRecvCmdCount = 256;
	data.MaxSendCmdCount = 256;
	data.SleepTime = 1;
	data.AcceptSleepTime = 1;
	data.CmdHearbeat = 0;
	data.RecvThreadNum = 8;
	data.SendThreadNum = 8;
	data.AcceptRecvData = false;
	data.SceneHearbeatTick = 100;

	strcpy_s(data.LogonServerIP, sizeof(data.LogonServerIP), LogonIP.c_str());
	strcpy_s(data.OperationServerIP, sizeof(data.OperationServerIP), OptIP.c_str());
	strcpy_s(data.LocalServerIP, sizeof(data.LocalServerIP), LocalIP.c_str());
	data.LogonClientPort = LogonPort;
	data.LogonServerPort = LogonPort;
	data.LogonThreadNum = 4;
	data.LogonAcceptPerFrame = 10;
	data.LogonTimeout = 5000;

	data.OperationAcceptPerFrame = 10;
	data.OperationClientPort = OptPort;
	data.OperationServerPort = OptPort;
	data.OperationTimeout    = 5000;
	NewGateServer ggg;
	ggg.SetCmdHandler(&g_handler);
	ggg.Init(data);
	while (1)
	{
		Sleep(100);
	}

	return 0;
}

