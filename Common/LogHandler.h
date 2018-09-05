#pragma once
#include "Common.h"
class LogHandler :public ILogHandler
{
public:
	virtual void LogHandlerW(const WCHAR *pcStr)
	{
		wcout << GetTimeStrW();
		wcout << pcStr << endl;
		OutputDebugStringW(pcStr);
		//MessageBoxW(0, pcStr, 0, 0);
	}
	virtual void LogHandlerA(const CHAR *pcStr)
	{
		cout << GetTimeStr();
		cout << pcStr << endl;
		OutputDebugStringA(pcStr);
		//MessageBoxA(0, pcStr, 0, 0);
	}
};