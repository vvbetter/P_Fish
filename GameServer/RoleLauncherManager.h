//玩家炮台控制 也涉及到倍率的控制
//玩家炮台 与物品相关 也与VIP相关 也与月卡相关
#pragma once
class CRoleEx;
class CConfig;
class RoleLauncherManager
{
public:
	RoleLauncherManager();
	virtual ~RoleLauncherManager();
	bool			OnInit(CRoleEx* pRole);//当初始化的时候 也就是全部物品已经读取成功后 我们将炮台的数据重新设置下
	bool			IsCanUserLauncherByID(BYTE LauncherID);//是否可以使用指定的炮台 每次开炮都需要进行判断的
private:
	CRoleEx*		m_pRole;
	CConfig*		m_pConfig;
	DWORD			m_LauncherStates;//炮台的状态控制
};