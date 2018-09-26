#pragma once
#include "Stdafx.h"
class CenterRoleManager;
class CenterRole
{
public:
	CenterRole();
	virtual ~CenterRole();

	void     OnInit(BYTE dwSocketID, tagCenterRoleInfo* pInfo, CenterRoleManager* pManager,bool IsRobot);

	DWORD	 GetRoleID(){ return m_RoleInfo.dwUserID; }
	BYTE	 GetSocketID(){ return m_dwSocketID; }
	void     SetSocketID(BYTE dwSocketID){ m_dwSocketID = dwSocketID; }
	tagCenterRoleInfo& GetRoleInfo(){ return m_RoleInfo; }

	void     OnRoleLeave();
	void	 OnRoleEnterFinish();

	void	SendDataToGameServer(NetCmd* pCmd);

	void    OnChangeRoleParticularStates(DWORD States);

	bool	IsRobot(){ return m_IsRobot; }

private:
	CenterRoleManager*			m_pRoleManager;
	BYTE						m_dwSocketID;//玩家对于的SocketID
	tagCenterRoleInfo			m_RoleInfo;//玩家的中央服务器上的信息
	bool						m_IsRobot;
};
class CenterRoleManager
{
public:
	CenterRoleManager();
	virtual ~CenterRoleManager();

	void OnInit();
	void Destroy();
	bool OnCreateCenterRole(BYTE dwSocketID, tagCenterRoleInfo* pInfo, bool IsRobot);
	bool OnDelCenterRole(DWORD dwUserID);
	bool OnPlazaLeave(BYTE dwSocketID);
	void SetCenterUserFinish(DWORD dwUserID);

	CenterRole* QueryCenterUser(DWORD dwUserID);

	DWORD	GetOnLinePlayerSum(){ return m_RoleMap.size(); }

	DWORD	GetLeaveOnlinePlayerSum(){ return m_LeaveOnlineUser; }
	void	SetAddOrDelLeaveOnlineUser(bool States) { m_LeaveOnlineUser += (States ? 1 : -1); }
private:
	HashMap<DWORD, CenterRole*>		m_RoleMap;
	DWORD							m_LeaveOnlineUser;
};