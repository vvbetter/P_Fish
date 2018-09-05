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
	//已经开启
	if (m_hallState == RUNING) return;
	//延迟启动
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
	//延迟停服
	if (m_hallState == STOPING)
	{
		m_stopServerDelay -= MINUTE_MILLISECONDS;
		if (m_stopServerDelay <= MINUTE_MILLISECONDS)//提前一分钟关闭玩家进入
		{
			m_hallState = STOPED;
			m_isHallRuning = false;
		}
		return;
	}
	//已经停止
	if (m_hallState == STOPED) return;
}

bool GameHallControl::IsHallRuning()
{
	return m_isHallRuning;
}

//已经开服有在点击开始
//点击开服后再点关服
//点击关服后再点开服
//多次开服或者关服操作
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