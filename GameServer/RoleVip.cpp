#include "Stdafx.h"
#include "RoleVip.h"
#include "RoleEx.h"
#include "FishServer.h"
RoleVip::RoleVip()
{

}
RoleVip::~RoleVip()
{

}
bool RoleVip::OnInit(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	m_pRole = pRole;
	return true;
}
BYTE RoleVip::GetLauncherReBoundNum()
{
	if (!m_pRole)
	{
		ASSERT(false);
		return g_FishServer.GetFishConfig().GetVipConfig().DefaultLauncherReBoundNum;
	}
	if (m_pRole->GetRoleInfo().VipLevel == 0)
	{
		return g_FishServer.GetFishConfig().GetVipConfig().DefaultLauncherReBoundNum;
	}
	return g_FishServer.GetFishConfig().GetVipConfig().GetLauncherReBoundNum(m_pRole->GetRoleInfo().VipLevel);
}
