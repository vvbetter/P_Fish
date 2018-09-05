#include "Stdafx.h"
#include "RoleRate.h"
#include "FishServer.h"
RoleRate::RoleRate()
{

}
RoleRate::~RoleRate()
{

}
bool RoleRate::OnInit(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return false;
	}
	m_pRole = pRole;
	if (g_FishServer.GetTableManager()->GetGameConfig()->RateCount() == 0)
	{
		ASSERT(false);
		return false;
	}
	//初始化玩家的倍率
	m_MinRateIndex = 0;
	m_MaxRateIndex = g_FishServer.GetTableManager()->GetGameConfig()->RateCount();
	return true;
}
void RoleRate::GetInitRateReward()
{
}
bool RoleRate::OnChangeRoleRate(BYTE AddRateIndex)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return false;
	}
	//修改玩家的倍率
	if (AddRateIndex <= m_MaxRateIndex)
	{
		ASSERT(false);
		return false;
	}
	//判断是否可以使用
	if (IsCanUseRateIndex(AddRateIndex))
	{
		ASSERT(false);
		return false;
	}
	return true;
}
bool RoleRate::IsCanUseRateIndex(BYTE RateIndex)
{
	return (RateIndex <= m_MaxRateIndex&& RateIndex >= m_MinRateIndex) ? true : false;
}
BYTE RoleRate::GetCanShowMaxRate()
{
	return m_MaxRateIndex;
}
void RoleRate::ResetRateInfo()
{
	//倍率数据已经乱了重新设置下
	m_MinRateIndex = 0;
	m_MaxRateIndex = g_FishServer.GetTableManager()->GetGameConfig()->RateCount();
}