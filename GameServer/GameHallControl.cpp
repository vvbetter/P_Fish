#include "stdafx.h"
#include "GameHallControl.h"

#define MINUTE_MILLISECONDS 60000

GameHallControl::GameHallControl()
{
	m_startServerDelay = 0;
	m_stopServerDelay = 0;
	m_isHallRuning = true;
	m_hallState = RUNING;
}


GameHallControl::~GameHallControl()
{
}

void GameHallControl::UpdateMin()
{
	//�Ѿ�����
	if (m_hallState == RUNING) return;
	//�ӳ�����
	if (m_hallState == STARTING)
	{
		m_startServerDelay -= MINUTE_MILLISECONDS;
		if (m_startServerDelay <= 0)
		{
			m_hallState = RUNING;
			m_isHallRuning = true;
		}
		return;
	}
	//�ӳ�ͣ��
	if (m_hallState == STOPING)
	{
		m_stopServerDelay -= MINUTE_MILLISECONDS;
		if (m_stopServerDelay <= MINUTE_MILLISECONDS)//��ǰһ���ӹر���ҽ���
		{
			m_hallState = STOPED;
			m_isHallRuning = false;
		}
		return;
	}
	//�Ѿ�ֹͣ
	if (m_hallState == STOPED) return;
}

bool GameHallControl::IsHallRuning()
{
	return m_isHallRuning;
}

//�Ѿ��������ڵ����ʼ
//����������ٵ�ط�
//����ط����ٵ㿪��
//��ο������߹ط�����
bool GameHallControl::ReqStartHall(INT64 delay)
{
	m_startServerDelay = delay;
	m_hallState = STARTING;
	return true;
}

bool GameHallControl::ReqStopHall(INT64 delay)
{
	m_stopServerDelay = delay;
	m_hallState = STOPING;
	return true;
}