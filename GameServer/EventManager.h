#pragma once
#include "Stdafx.h"
#define MAX_TASK_SUM 255
#define MAX_EVENT_SUM 255
#define MAX_BINDPARAM_SUM 255
class CRoleEx;
class EventManager //���ĵ��¼���������
{
public:
	EventManager();
	~EventManager();
	void OnInit();
	void OnLoadGlobelTask(CG_Cmd_GetGlobelTaskInfo* pMsg);//����ȫ�ֵ�����
	bool IsGlobleStop(BYTE TaskID);
private:
	void OnDestroy();
private:
	std::map<BYTE, BYTE>							m_TaskGlobelMap;//ȫ�ֵ������б�
};