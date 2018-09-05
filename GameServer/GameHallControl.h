#pragma once

enum GameHallState //��Ϸ��������״̬
{
	RUNING,		//������
	STOPED,		//ֹͣ
	STOPING,	//����-ֹͣ �л���
	STARTING,	//ֹͣ-���� �л���
};
//��Ϸ��������������
class GameHallControl
{
public:
	GameHallControl();
	~GameHallControl();
public:
	void UpdateMin(); //ÿ���Ӹ���
	bool IsHallRuning();
	bool ReqStopHall(INT64 delay);
	bool ReqStartHall(INT64 delay);
private:
	INT64 m_startServerDelay;
	INT64 m_stopServerDelay;
	bool  m_isHallRuning;
	GameHallState m_hallState;
};

