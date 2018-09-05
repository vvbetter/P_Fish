// DBServer.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
int _tmain(int argc, _TCHAR* argv[])
{
	FTPClient client;
	client.Init("61.153.110.167", 45046, 5);
	for (int i = 0; i < 500; ++i)
	client.GetFile("stdafx.h");
	while (1)
	{
		Sleep(1000);
	}

	return 0;
}