#pragma once
class CRoleEx;
class RoleVip
{
public:
	RoleVip();
	virtual ~RoleVip();
	bool OnInit(CRoleEx* pRole);
	BYTE GetLauncherReBoundNum();//获取子弹反弹的次数
private:
	CRoleEx*			m_pRole;
};