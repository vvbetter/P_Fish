#pragma once
class CRoleEx;
class RoleVip
{
public:
	RoleVip();
	virtual ~RoleVip();
	bool OnInit(CRoleEx* pRole);
	BYTE GetLauncherReBoundNum();//��ȡ�ӵ������Ĵ���
private:
	CRoleEx*			m_pRole;
};