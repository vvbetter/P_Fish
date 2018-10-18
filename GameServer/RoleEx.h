#pragma once
#include "Stdafx.h"
#include "RoleGameData.h"
#include "RoleLauncherManager.h"
#include "RoleVip.h"
#include "RoleRate.h"
#include"FishLogic\FishCallbak.h"
class RoleManager;
enum CatchType;
class CRoleEx
{
public:

	CRoleEx();
	~CRoleEx();

	bool OnInit(tagRoleInfo* pUserInfo, tagRoleServerInfo* pRoleServerInfo, RoleManager* pManager,
		DWORD dwSocketID, time_t pTime, bool IsRobot, const tagRoleGameData& gameData);//��ҵ�½�ɹ���ʱ����� �������ݿ�����
	DWORD GetUserID() { return m_RoleInfo.dwUserID; }
	DWORD GetGameSocketID() { return m_dwGameSocketID; }
	void ChangeRoleSocketID(DWORD SocketID);
	DWORD GetFaceID() { return m_RoleInfo.dwFaceID; }
	tagRoleInfo& GetRoleInfo() { return m_RoleInfo; }
	const FishKillDataType& GetRoleKillFishData() { return m_RoleGameData.GetFishKillData(); }
	tagRoleServerInfo& GetRoleServerInfo() { return m_RoleServerInfo; }
	time_t GetLastOnLineTime() { return m_LastOnLineTime; }//��ȡ���������ߵ�ʱ�� �п���Ϊ0 ��ʾ��ҵ�һ������
	bool IsOnceDayOnline();//�Ƿ���ͬһ�������µ�½
	bool IsOnceMonthOnline();
	RoleGameData& GetRoleGameData() { return m_RoleGameData; }
	RoleLauncherManager& GetRoleLauncherManager() { return m_RoleLauncherManager; }
	RoleVip& GetRoleVip() { return m_RoleVip; }
	RoleRate& GetRoleRate() { return m_RoleRate; }
	void SendDataToClient(NetCmd* pCmd);
	void SendDataToCenter(NetCmd* pCmd);
	void SendDataToTable(NetCmd* pCmd);
	void SendUserLeaveToCenter();
	bool ChangeRoleGlobe(int64 AddGlobe, const BYTE TableTypeID, const USHORT FishType = -1, const BYTE bulletType = -1);
	bool OnHitFish(const USHORT fishType,const BYTE bulletType);
	bool ChangeRoleIsOnline(bool States);
	//RoleServerInfo
	bool ChangeRoleTotalFishGlobelSum(int64 AddSum);
	int64 GetScore();
	void OnSaveInfoToDB();
	void UpdateByMin(DWORD dwMin);
	//��RoleEx ���� ������ �л�
	void SetRoleExLeaveServer();//����RoleEx �뿪�˷�����
	bool IsAFK() { return m_IsAfk; }
	void SetAfkStates(bool States);
	bool IsRobot() { return m_IsRobot; }
	bool IsExit() { return m_IsExit; }
	void SetIsExit(bool States);
	bool SaveAllRoleInfo(bool IsExit);
	void SetRoleIsNeedSave() { m_IsNeedSave = true; }//�޸���� ����Ҫ�����
private:
	RoleManager*				m_RoleManager;
	tagRoleInfo					m_RoleInfo;//��ҵ����� �����ݿ����ȡ�� ��һЩ����Ĳ��� ����������Ҫ���͵��ͻ���ȥ��
	tagRoleServerInfo			m_RoleServerInfo;
	bool					    m_IsNeedSave;
	//��ҵ����ݴ���
	DWORD						m_dwGameSocketID;//�����Game��Socket�϶�Ӧ��ID		
	time_t						m_LastOnLineTime;//�������ߵ�ʱ���¼
	time_t						m_LogonTime;//��ǰ��½��ʱ��
	time_t						m_LogonTimeByDay;//ÿ��Ŀ�ʼʱ��
	//��ҵĹ�����
	RoleGameData				m_RoleGameData;
	RoleLauncherManager			m_RoleLauncherManager;
	RoleVip						m_RoleVip;
	RoleRate					m_RoleRate;
	bool						m_IsAfk;
	//��½��صĻ�������
	bool						m_IsRobot;
	bool						m_IsExit;//����Ƿ������˳�
	bool						m_IsOnline;
};