#include "StdAfx.h"
#include "RoleLockManager.h"
#include "RoleEx.h"
#include "FishServer.h"
RoleLockManager::RoleLockManager()
{

}
RoleLockManager::~RoleLockManager()
{

}
bool RoleLockManager::OnInit(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	m_pRole = pRole;
	return true;
}
bool RoleLockManager::IsCanUseGlobel()
{
	return IsCanUseByType(RLS_Globel);
}
bool RoleLockManager::IsCanUseMedal()
{
	return IsCanUseByType(RLS_Medal);
}
bool RoleLockManager::IsCanUseCurrcey()
{
	return IsCanUseByType(RLS_Currcey);
}
bool RoleLockManager::IsCanUseByType(RoleLockStates states)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return false;
	}
	if ((m_pRole->GetRoleInfo().LockStates & states) != 0)
	{
		time_t Now = time(null);
		if (m_pRole->GetRoleInfo().LockEndTime >= Now)
			return false;
		else
			return true;
	}
	else
		return true;
}
void RoleLockManager::OnSetRoleLockInfo(DWORD States, DWORD LockSec)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	m_pRole->GetRoleInfo().LockStates = States;
	if (LockSec == 0)
	{
		m_pRole->GetRoleInfo().LockEndTime = 0;
	}
	else
	{
		m_pRole->GetRoleInfo().LockEndTime = time(null) + LockSec;
	}
	//修改完毕后 直接发送命令到数据库去
	DBR_Cmd_SaveRoleLockInfo msgDB;
	SetMsgInfo(msgDB, DBR_SaveRoleLockInfo, sizeof(DBR_Cmd_SaveRoleLockInfo));
	msgDB.dwUserID = m_pRole->GetUserID();
	msgDB.LockStates = m_pRole->GetRoleInfo().LockStates;
	msgDB.LockEndTime = m_pRole->GetRoleInfo().LockEndTime;
	g_FishServer.SendNetCmdToSaveDB(&msgDB);
	//发送到客户端去
	LC_Cmd_ChangeRoleLockInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Role, LC_ChangeRoleLockInfo), sizeof(LC_Cmd_ChangeRoleLockInfo));
	msg.LockEndTime = m_pRole->GetRoleInfo().LockEndTime;
	msg.LockStates = m_pRole->GetRoleInfo().LockStates;
	m_pRole->SendDataToClient(&msg);
}