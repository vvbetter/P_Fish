#pragma once
#include "Stdafx.h"
class CRoleEx;
class RoleManager
{
public:
	RoleManager();
	virtual ~RoleManager();
	void Destroy();

	bool OnInit();
	CRoleEx* QueryUser(DWORD dwUserID);
	CRoleEx* QueryUserByGameID(DWORD GameID);
	CRoleEx* QuertUserBySocketID(DWORD dwSocketID);
	CRoleEx* QuertUserByUid(int64 uid);
	void  OnDelUser(DWORD dwUserID,bool IsWriteSaveInfo,bool IsLeaveCenter);
	void OnDelUserResult(DBO_Cmd_SaveRoleAllInfo* pResult);
	bool CreateRole(tagRoleInfo* pUserInfo, tagRoleServerInfo* pUserServerInfo, DWORD dwSocketID, time_t pTime, bool LogobByGameServer, bool IsRobot);


	void OnUpdateByMin(bool IsHourChange, bool IsDayChange, bool IsMonthChange, bool IsYearChange);//每分钟更新
	void ChangeRoleSocket(CRoleEx* pRole, DWORD SocketID);
	void OnSaveInfoToDB();
	void OnKickOneUser(CRoleEx* pRole);
	void OnKickAllUser();
	void OnResetGameDataTurn();
	void SetResetGameDataTable(const BYTE tableTypeID);
private:
	bool m_ResetGameData[4];
	HashMap<DWORD, CRoleEx*>	m_RoleMap;       //userid,roleEx
	HashMap<DWORD, CRoleEx*>	m_RoleSocketMap; // socket,roleEx
	HashMap<DWORD, CRoleEx*>    m_RoleGameMap;   // gameid,roleEx
	HashMap<int64, tagRoleGameData> m_RoleGameDataCache; //uid,gamedata
 };
