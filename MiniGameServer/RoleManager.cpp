#include "stdafx.h"
#include "RoleManager.h"
#include "FishServer.h"
Role::Role(tagMiniGameRoleInfo* pInfo)
{
	if (!pInfo)
	{
		ASSERT(false);
		return;
	}
	m_RoleInfo = *pInfo;
}
Role::~Role()
{

}
DWORD	Role::GetRoleID()
{
	return m_RoleInfo.dwUserID;
}
DWORD	Role::GetFaceID()
{
	return m_RoleInfo.FaceID;
}
DWORD   Role::GetGlobelSum()
{
	return m_RoleInfo.dwGlobelSum;
}
DWORD   Role::GetMadleSum()
{
	return m_RoleInfo.dwMadleSum;
}
DWORD   Role::GetCurrceySum()
{
	return m_RoleInfo.dwCurrceySum;
}
BYTE    Role::GetVipLevel()
{
	return m_RoleInfo.VipLevel;
}
LPTSTR  Role::GetNickName()
{
	return m_RoleInfo.NickName;
}
BYTE	Role::GetGameConfigID()
{
	return m_RoleInfo.GameConfigID;
}
void	Role::SendDataToGameServer(NetCmd* pCmd)
{
	if (!pCmd)
	{
		ASSERT(false);
		return;
	}
	g_FishServer.SendNetCmdToClient(m_RoleInfo.GameConfigID, pCmd);
}
void    Role::OnChangeRoleGlobelSum(DWORD GlobelSum, bool IsServer )
{
	int ChangeSum = static_cast<int>(GlobelSum - m_RoleInfo.dwGlobelSum);

	m_RoleInfo.dwGlobelSum = GlobelSum;

	if (IsServer)
	{
		g_FishServer.GetNiuNiuControl().OnRoleGoldChange(m_RoleInfo.dwUserID, ChangeSum);
		g_FishServer.GetDialManager().OnRoleGlobelChange(m_RoleInfo.dwUserID, ChangeSum);
		g_FishServer.GetCarManager().OnRoleGlobelChange(m_RoleInfo.dwUserID, ChangeSum);

		{
			GM_Cmd_FRoleInfoChange msg;
			SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FRoleInfoChange), sizeof(GM_Cmd_FRoleInfoChange));
			msg.dwUserID = m_RoleInfo.dwUserID;
			g_FishServer.GetRoomManager()->HandleMsg(&msg);
		}
	}
}
void	Role::OnChangeRoleMadleSum(DWORD MadleSum)
{
	m_RoleInfo.dwMadleSum = MadleSum;
}
void    Role::OnChangeRoleCurrceySum(DWORD CurrceySum)
{
	m_RoleInfo.dwCurrceySum = CurrceySum;
}
void    Role::OnChangeRoleVipLevel(BYTE VipLevel)
{
	m_RoleInfo.VipLevel = VipLevel;
}
void    Role::OnChangeRoleFaceID(DWORD FaceID)
{
	m_RoleInfo.FaceID = FaceID;
}
void	Role::OnChangeRoleNickName(LPTSTR pNickName)
{
	TCHARCopy(m_RoleInfo.NickName, CountArray(m_RoleInfo.NickName), pNickName, _tcslen(pNickName));
}
void    Role::OnChangeRoleParticularStates(DWORD States)
{
	m_RoleInfo.ParticularStates = States;
}

RoleManager::RoleManager()
{

}
RoleManager::~RoleManager()
{
	if (!m_RoleMap.empty())
	{
		HashMap<DWORD, Role*>::iterator Iter = m_RoleMap.begin();
		for (; Iter != m_RoleMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_RoleMap.clear();
	}
}
Role* RoleManager::QueryRole(DWORD dwUserID)
{
	HashMap<DWORD, Role*>::iterator Iter =  m_RoleMap.find(dwUserID);
	if (Iter == m_RoleMap.end())
		return NULL;
	else
		return Iter->second;
}
void  RoleManager::OnRsgRole(tagMiniGameRoleInfo* pInfo,BYTE GameConfigID)
{
	if (!pInfo)
	{
		ASSERT(false);
		return;
	}
	pInfo->GameConfigID = GameConfigID;
	HashMap<DWORD, Role*>::iterator Iter = m_RoleMap.find(pInfo->dwUserID);
	if (Iter == m_RoleMap.end())
	{
		Role* pRole = new Role(pInfo);
		if (!pRole)
		{
			ASSERT(false);
			return;
		}
		m_RoleMap.insert(HashMap<DWORD, Role*>::value_type(pInfo->dwUserID, pRole));
	}
	else
	{
		//玩家已经在MINIGame上面了
		OnUnRsgRole(pInfo->dwUserID);
		Role* pRole = new Role(pInfo);
		if (!pRole)
		{
			ASSERT(false);
			return;
		}
		m_RoleMap.insert(HashMap<DWORD, Role*>::value_type(pInfo->dwUserID, pRole));
	}
}
void  RoleManager::OnUnRsgRole(DWORD dwUserID)
{
	HashMap<DWORD, Role*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter == m_RoleMap.end())
		return;
	else
	{
		g_FishServer.GetNiuNiuControl().OnRoleLeaveMiniGame(dwUserID);//离开牛牛服务器
		g_FishServer.GetDialManager().OnRoleLeaveMiniGameServer(dwUserID);
		g_FishServer.GetCarManager().OnRoleLeaveMiniGameServer(dwUserID);

		//玩家离开
		{
			GM_Cmd_FRoleLeaveRoom msg;
			SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FRoleLeaveRoom), sizeof(GM_Cmd_FRoleLeaveRoom));
			msg.dwUserID = dwUserID;
			g_FishServer.GetRoomManager()->HandleMsg(&msg);
		}

		delete Iter->second;
		m_RoleMap.erase(Iter);
	}
}