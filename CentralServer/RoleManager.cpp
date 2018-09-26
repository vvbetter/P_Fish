#include "Stdafx.h"
#include "RoleManager.h"
#include "FishServer.h"
CenterRoleManager::CenterRoleManager()
{
	m_LeaveOnlineUser = 0;
}
CenterRoleManager::~CenterRoleManager()
{
	Destroy();
}
void CenterRoleManager::OnInit()
{
	m_RoleMap.clear();
}
void CenterRoleManager::Destroy()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		delete Iter->second;
	}
	m_RoleMap.clear();
}
bool CenterRoleManager::OnCreateCenterRole(BYTE dwSocketID, tagCenterRoleInfo* pInfo, bool IsRobot)
{
	if (!pInfo)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.find(pInfo->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		return true;
	}
	else
	{
		CenterRole* pRole = new CenterRole();
		if (!pRole)
		{
			ASSERT(false);
			return false;
		}
		pRole->OnInit(dwSocketID, pInfo, this,IsRobot);
		//加入集合
		m_RoleMap.insert(std::map<DWORD, CenterRole*>::value_type(pInfo->dwUserID, pRole));
		return true;
	}
}
bool CenterRoleManager::OnDelCenterRole(DWORD dwUserID)
{
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end() && Iter->second)
	{
		Iter->second->OnRoleLeave();
		delete Iter->second;
		m_RoleMap.erase(Iter);
		return true;
	}
	else
	{
		return true;
	}
}
bool CenterRoleManager::OnPlazaLeave(BYTE dwSocketID)
{
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end();)
	{
		if (Iter->second && Iter->second->GetSocketID() == dwSocketID)
		{
			//Iter->second->ClearSocketID();
			Iter->second->OnRoleLeave();
			g_FishServer.GetCenterManager().UnRsgUser(Iter->first);//让指定玩家离线 当GameServer断开连接的时候 进行处理

			delete Iter->second;
			Iter = m_RoleMap.erase(Iter);
		}
		else
			++Iter;
	}
	return true;
}
void CenterRoleManager::SetCenterUserFinish(DWORD dwUserID)
{
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end() && Iter->second)
	{
		Iter->second->OnRoleEnterFinish();
	}
}
CenterRole* CenterRoleManager::QueryCenterUser(DWORD dwUserID)
{
	HashMap<DWORD, CenterRole*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end() && Iter->second)
		return Iter->second;
	else
		return NULL;
}

CenterRole::CenterRole()
{
	//CTraceService::TraceString(TEXT("创建一个CenterRole对象"), TraceLevel_Normal);
	m_IsRobot = false;
}
CenterRole::~CenterRole()
{
	//CTraceService::TraceString(TEXT("删除一个CenterRole对象"), TraceLevel_Normal);
	//g_FishServer.GetCenterManager().UnRsgUser(m_RoleInfo.dwUserID);//指定玩家离开了
}
void CenterRole::OnInit(BYTE dwSocketID, tagCenterRoleInfo* pInfo, CenterRoleManager* pManager, bool IsRobot)
{
	//当对象创建完毕的时候
	if (!pInfo || !pManager)
	{
		ASSERT(false);
		return;
	}
	m_dwSocketID = dwSocketID;
	m_RoleInfo = *pInfo;
	m_pRoleManager = pManager;
	m_IsRobot = IsRobot;
}
void CenterRole::OnRoleLeave()
{
	//当 一个玩家离开Center的时候 我们想要通知玩家 的被好友列表的全部玩家  xx玩家下线了
	//发送命令 Relation...
	//CTraceService::TraceString(TEXT("CenterRole 触发离开事件"), TraceLevel_Normal);
}
void CenterRole::OnRoleEnterFinish()
{
	//当中央服务器玩家对象创建完毕的时候 
	//1.将玩家上线的消息发送给全部的被好友玩家
	//...
	//CTraceService::TraceString(TEXT("CenterRole 触发进入事件"), TraceLevel_Normal);
}
void CenterRole::SendDataToGameServer(NetCmd* pCmd)
{
	g_FishServer.GetCenterManager().SendNetCmdToGameServer(m_dwSocketID, pCmd);
}

void CenterRole::OnChangeRoleParticularStates(DWORD States)
{
	m_RoleInfo.ParticularStates = States;
}