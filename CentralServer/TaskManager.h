//����������ϵ����������
#pragma once
#include "Stdafx.h"
class TaskManager//������������ڿ���ȫ�ֵ�Task������
{
public:
	TaskManager();
	virtual ~TaskManager();
	void OnInit();
	void OnGameServerRsg(BYTE SocketID);
	void UpdateByMin(bool IsHourChange, bool IsDayChange, bool IsMonthChange, bool IsYearChange);
private:
	void CreateGlobelTaskInfo();
	void SendTaskInfoToGameServer(BYTE SocketID);
private:
	std::vector<BYTE>			m_GlobelTaskVec;
};