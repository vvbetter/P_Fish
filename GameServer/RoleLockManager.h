#pragma once
enum RoleLockStates
{
	RLS_Globel = 1,
	RLS_Currcey = 2,
	RLS_Medal = 4,
};

class RoleLockManager
{
public:
	RoleLockManager();
	virtual ~RoleLockManager();

	bool OnInit(CRoleEx* pRole);

	//提供的函数
	bool IsCanUseGlobel();
	bool IsCanUseMedal();
	bool IsCanUseCurrcey();

	//操作函数
	void OnSetRoleLockInfo(DWORD States,DWORD LockSec);

private:
	bool IsCanUseByType(RoleLockStates states);
private:
	CRoleEx*			m_pRole;
};