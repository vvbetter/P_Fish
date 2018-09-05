#pragma once

enum GameHallState //游戏大厅运行状态
{
	RUNING,		//运行中
	STOPED,		//停止
	STOPING,	//运行-停止 切换中
	STARTING,	//停止-运行 切换中
};
//游戏大厅辅助控制类
class GameHallControl
{
public:
	GameHallControl();
	~GameHallControl();
public:
	void UpdateMin(); //每分钟更新
	bool IsHallRuning();
	bool ReqStopHall(INT64 delay);
	bool ReqStartHall(INT64 delay);
private:
	INT64 m_startServerDelay;
	INT64 m_stopServerDelay;
	bool  m_isHallRuning;
	GameHallState m_hallState;
};

