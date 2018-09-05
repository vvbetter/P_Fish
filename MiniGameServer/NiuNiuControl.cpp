#include "stdafx.h"
#include "NiuNiuControl.h"
NiuNiuControl::NiuNiuControl() :m_NiuNiuBeginIndex(1)
{

}
NiuNiuControl::~NiuNiuControl()
{

}
bool NiuNiuControl::Init()
{
	m_NiuNiuMap.clear();
	m_NormalID = m_NiuNiuBeginIndex;
	++m_NiuNiuBeginIndex;
	NiuNiuManager* pNiuNiu = new NiuNiuManager(m_NormalID);
	pNiuNiu->OnInit();
	m_NiuNiuMap.insert(HashMap<DWORD, NiuNiuManager*>::value_type(m_NormalID, pNiuNiu));

	return true;
}
void NiuNiuControl::Destory()
{
	if (m_NiuNiuMap.empty())
		return;
	HashMap<DWORD, NiuNiuManager*>::iterator Iter = m_NiuNiuMap.begin();
	for (; Iter != m_NiuNiuMap.end(); ++Iter)
	{
		delete Iter->second;
	}
	m_NiuNiuMap.clear();
}
void NiuNiuControl::Update()
{
	if (m_NiuNiuMap.empty())
		return;
	DWORD dwTimer = timeGetTime();
	HashMap<DWORD, NiuNiuManager*>::iterator Iter = m_NiuNiuMap.begin();
	for (; Iter != m_NiuNiuMap.end(); ++Iter)
	{
		Iter->second->OnUpdate(dwTimer);
	}
}
NiuNiuManager* NiuNiuControl::GetNiuNiuManager(DWORD dwUserID)
{
	DWORD NiuNiuID = __GetRoleNiuNiuID(dwUserID);
	if (NiuNiuID == 0)
		return nullptr;
	HashMap<DWORD, NiuNiuManager*>::iterator Iter = m_NiuNiuMap.find(NiuNiuID);
	if (Iter == m_NiuNiuMap.end())
		return nullptr;
	else
		return Iter->second;
}
NiuNiuManager* NiuNiuControl::GetNiuNiuManager()
{
	return GetNiuNiuManager(m_NormalID);
}
DWORD NiuNiuControl::__GetRoleNiuNiuID(DWORD dwUserID)
{
	HashMap<DWORD, DWORD>::iterator Iter = m_UserLogMap.find(dwUserID);
	if (Iter == m_UserLogMap.end())
		return 0;
	else
		return Iter->second;
}
void NiuNiuControl::OnRoleJoin(DWORD dwUserID)
{
	m_UserLogMap.insert(HashMap<DWORD, DWORD>::value_type(dwUserID, m_NormalID));
	//指定玩家进入牛牛桌子
	NiuNiuManager* pNiuNiu = GetNiuNiuManager(dwUserID);
	if (nullptr != pNiuNiu)
		pNiuNiu->OnRoleJoinNiuNiuTable(dwUserID);
}
void NiuNiuControl::OnRoleJoin(DWORD dwSrcUserID, DWORD deDestUserID)
{
	NiuNiuManager* pNiuNiuRoom = GetNiuNiuManager(dwSrcUserID);
	if (nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	m_UserLogMap.insert(HashMap<DWORD, DWORD>::value_type(deDestUserID, pNiuNiuRoom->GetNiuNiuID()));
	pNiuNiuRoom->OnRoleJoinNiuNiuTable(deDestUserID);
}
void NiuNiuControl::OnRoleLeave(DWORD dwUserID)
{
	DWORD dwNiuNiuID = __GetRoleNiuNiuID(dwUserID);
	NiuNiuManager* pNiuNiu = GetNiuNiuManager(dwUserID);
	if (nullptr != pNiuNiu)
	{
		pNiuNiu->OnRoleLeaveNiuNiuTable(dwUserID);
		if (pNiuNiu->GetRoleSum() == 0 && dwNiuNiuID != m_NormalID)
		{
			delete pNiuNiu;
			m_NiuNiuMap.erase(dwNiuNiuID);
		}
	}
	m_UserLogMap.erase(dwUserID);
}
void NiuNiuControl::CreateNiuNiuManager(DWORD dwUserID) //特殊的命令触发的
{
	//创建一个牛牛房间 特殊的命令 创建成功后 立刻让玩家进入房间
	m_UserLogMap.erase(dwUserID);
	DWORD niuNiuID = m_NiuNiuBeginIndex;
	++m_NiuNiuBeginIndex;
	NiuNiuManager* pNiuNiu = new NiuNiuManager(niuNiuID);
	pNiuNiu->OnInit();
	m_NiuNiuMap.insert(HashMap<DWORD, NiuNiuManager*>::value_type(niuNiuID, pNiuNiu));
	pNiuNiu->SetBrandInfo(dwUserID);
	m_UserLogMap.insert(HashMap<DWORD, DWORD>::value_type(dwUserID, niuNiuID));
	pNiuNiu->OnRoleJoinNiuNiuTable(dwUserID);
}
void NiuNiuControl::OnRoleGoldChange(DWORD dwUserID, int ChangeSum)
{
	NiuNiuManager* pNiuNiu = GetNiuNiuManager(dwUserID);
	if (nullptr == pNiuNiu)
		return;
	pNiuNiu->OnRoleGlobelChange(dwUserID, ChangeSum);
}
void NiuNiuControl::OnRoleLeaveMiniGame(DWORD dwUserID)
{
	OnRoleLeave(dwUserID);
	/*NiuNiuManager* pNiuNiu = GetNiuNiuManager(dwUserID);
	if (nullptr == pNiuNiu)
		return;
	pNiuNiu->OnRoleLeaveMiniGameServer(dwUserID);*/
}