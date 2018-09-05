// CentralServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "FishServer.h"
bool strlHandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
	case CTRL_CLOSE_EVENT://ֱ�ӹرճ���
		g_FishServer.SetServerClose();
		return true;
	}
	return true;
}
DWORD WINAPI MainUpdate(LPVOID pParam)
{
	g_FishServer.MainUpdate();
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1)
	{
#ifdef _DEBUG
		std::cout << "Debugģʽ�� �����������...";
		getchar();
#endif
	}

	DWORD Index = 0;
	if (argc == 2)
	{
		Index = _ttoi(argv[1]);
	}
	else
	{
		std::cout << "�������Ƿ������������а�:";
		std::cin >> Index;
	}

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)strlHandler, true))
	{
		ASSERT(false);
		return 0;
	}
	if (!g_FishServer.InitServer(Index == 1))
	{
		ASSERT(false);
		return 0;
	}
	DWORD nThreadID;
	CreateThread(0, 0, &MainUpdate, NULL, 0, &nThreadID);
	//�ȴ����� ��������Ϊ "exit"  ��ִ���˳����� ���ҹر��߳�
	std::string str;
	do
	{
		std::cout << "����̨ �������� exit Ϊ �˳����� :\n";
		std::cin >> str;
		if (str.compare("exit") == 0)
		{
			g_FishServer.SetServerClose();//���ڹر��߳�
			return 0;
		}
		else
		{
			std::cout << "δ֪�Ŀ���̨���� �޷����д���\n";
		}

	} while (true);
	return 0;
}

