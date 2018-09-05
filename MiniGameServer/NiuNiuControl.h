#pragma once
#include "niuniu\NiuNiuManager.h"
class NiuNiuControl
{
public:
	NiuNiuControl();
	~NiuNiuControl();

	bool	Init();
	void	Destory();
	void	Update();

	void	OnRoleJoin(DWORD dwUserID);//普通命令触发的
	void	OnRoleJoin(DWORD dwSrcUserID, DWORD deDestUserID); //特殊命令触发的
	void	OnRoleLeave(DWORD dwUserID);
	NiuNiuManager* GetNiuNiuManager(DWORD dwUserID);
	NiuNiuManager* GetNiuNiuManager();
	void	CreateNiuNiuManager(DWORD dwUserID);//特殊命令触发的
	void	OnRoleGoldChange(DWORD dwUserID,int ChangeSum);
	void	OnRoleLeaveMiniGame(DWORD dwUserID);

	DWORD	GetNiuNiuNormalID(){ return m_NormalID; }
private:
	DWORD	__GetRoleNiuNiuID(DWORD dwUserID);
private:
	DWORD							m_NiuNiuBeginIndex;
	DWORD							m_NormalID;
	HashMap<DWORD, DWORD>			m_UserLogMap;
	HashMap<DWORD, NiuNiuManager*>	m_NiuNiuMap;
};