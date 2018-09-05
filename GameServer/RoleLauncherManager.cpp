#include "Stdafx.h"
#include "RoleEx.h"
#include "FishServer.h"
#include "RoleLauncherManager.h"
RoleLauncherManager::RoleLauncherManager()
{
	m_LauncherStates = 0;
	m_pConfig = null;
	m_pRole = null;
}
RoleLauncherManager::~RoleLauncherManager()
{
	m_LauncherStates = 0;
	m_pConfig = null;
	m_pRole = null;
}
bool RoleLauncherManager::OnInit(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	m_LauncherStates = 0;
	m_pConfig = g_FishServer.GetTableManager()->GetGameConfig();
	if (!m_pConfig)
	{
		ASSERT(false);
		return false;
	}
	time_t pNow = time(null);
	m_pRole = pRole;
	for (int i = 0; i < MAX_LAUNCHER_NUM; ++i)
	{
		DWORD ItemID = 0;
		DWORD ItemSum = 0;
		m_pConfig->GoodsInfo(i, (int&)ItemID, (int&)ItemSum);
		if (ItemID == 0 || ItemSum == 0)
		{
			//当前炮台无须物品 直接设置为 可用
			m_LauncherStates |= (1 << (i + 1));//设置炮台的状态可用
			continue;
		}
		//判断玩家VIP等级
		if (m_pRole->GetRoleInfo().VipLevel != 0)
		{
			HashMap<BYTE, tagVipOnce>::iterator Iter = g_FishServer.GetFishConfig().GetVipConfig().VipMap.find(m_pRole->GetRoleInfo().VipLevel);
			if (Iter != g_FishServer.GetFishConfig().GetVipConfig().VipMap.end())
			{
				if (Iter->second.CanUseLauncherMap.count(i) == 1)
				{
					//可以使用炮
					m_LauncherStates |= (1 << (i + 1));//设置炮台的状态可用
					continue;
				}
			}
		}
	}
	return true;
}
bool RoleLauncherManager::IsCanUserLauncherByID(BYTE LauncherID)
{
	if (LauncherID >= 32 || LauncherID >= MAX_LAUNCHER_NUM)
	{
		ASSERT(false);
		return false;
	}
	return  ((m_LauncherStates & (1 << (LauncherID + 1))) != 0);
}