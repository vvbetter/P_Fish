//ͨ�û����˹�����
//�����˻��ڻ������ IsRobot = 1
//���ݿ� ���������� ���ǽ��� ���� ���� RsgIP �Ȳ���
#pragma once
typedef void(SendMsgToDBFun)(NetCmd* pCmd);
class RobotManager
{
public:
	RobotManager();
	virtual ~RobotManager();
	
	void  OnLoadRobotInfoBySum(DWORD BeginUserID,DWORD EndUserID, SendMsgToDBFun pSend);//����ָ�������Ļ�����
	void  OnLoadRobotInfoResult(DBO_Cmd_LoadRobotInfo* msg);
	RobotUserInfo* GetNonUseRobot();
	void  OnNonUseRobot(DWORD dwUserID);//�黹������
	vector<RobotUserInfo*>& GetAllNonRobotVec(){ return m_NonUseRobotInfo; }

	bool IsRobot(DWORD dwUserID);
private:
	HashMap<DWORD, RobotUserInfo*> m_UseRobotInfo;
	vector<RobotUserInfo*> m_NonUseRobotInfo;
};