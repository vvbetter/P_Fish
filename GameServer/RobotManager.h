//�����˵Ĺ���ѡ��
#pragma once
class CRoleEx;
class Robot//���������˵Ĵ���
{
public:
	Robot();
	virtual ~Robot();
	void OnInit(CRoleEx* pRole);
	void OnUpdate(DWORD dwTimer);
	void OnUseRobot(WORD TableID, BYTE MonthID);
	void OnDelRobot();

	bool IsUse(){ return m_IsUse; }
private:
	void		OnUpdateRoleChangeLauncher();//������ڿ���
	void		OnUpdateRoleOpenFire();//��ҿ��ڿ���
	void		OnUpdateRoleLauncherFace();//���������̨����
	void		OnUpdateRoleUseSkill();//�������ʹ�ü���
private:
	CRoleEx*			m_pRole;
	bool				m_IsUse;
	DWORD				m_UpdateTimeLog;

	DWORD				m_ChangeLauncherTimeLog;

	DWORD				m_ChangeAngleTimeLog;
	DWORD				m_Angle;//��̨����

	WORD				m_LockFishID;
};
class RobotManager
{
public:
	RobotManager();
	virtual ~RobotManager();

	void OnInit(DWORD BeginRobotUserID,DWORD EndRobotUserID);

	void OnAddRobot(DWORD dwUserID);
	void OnUseRobot(WORD TableID, BYTE MonthID);
	void OnDelRobot(DWORD dwUserID);

	void OnUpdate(DWORD dwTimer);
private:
	Robot* GetFreeRobot();//��ȡ һ�����еĻ�����
private:
	HashMap<DWORD, Robot*>		m_RobotMap;//�����˵ļ���
};
