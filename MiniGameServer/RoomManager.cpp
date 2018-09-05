/*
*@file   RoomManager.cpp
*@brief  ������->����->����->��� ����Ľӿ�ģ��
*@author ���
*@data   2016-6-14 10:00
*/
#include "Stdafx.h"
#include "RoomManager.h"
#include "FiveNiuNiu.h"
#include "FishServer.h"
RoomManager::RoomManager() :IRoomManager()
{
	__Init();
}
RoomManager::~RoomManager()
{
	__Destory();
}
/**
*  ��������:		���������������ȫ�������ָ��
*
*  @return void		�޷���ֵ
*/
void RoomManager::__ClearRoom()
{
	if (!m_RoomHashMap.empty())
	{
		HashMap<BYTE, IRoom*>::iterator Iter = m_RoomHashMap.begin();
		for (; Iter != m_RoomHashMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_RoomHashMap.clear();
	}
}
/**
*  ��������:		���������������ȫ����Ϣģ���ָ��
*
*  @return void		�޷���ֵ
*/
void RoomManager::__ClearHandleCommon()
{
	if (!m_HandleCommonHashMap.empty())
	{
		HashMap<RoomType, IHandleCommon*>::iterator Iter = m_HandleCommonHashMap.begin();
		for (; Iter != m_HandleCommonHashMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_HandleCommonHashMap.clear();
	}
}
/**
*  ��������:		������ҷ�������Ӽ�¼�Ļ���
*
*  @return void		�޷���ֵ
*/
void RoomManager::__ClearRoleCache()
{
	m_RoleRoomIDHashMap.clear();
	m_RoleTableIDHashMap.clear();
}
/**
*  ��������:		��ʼ������ �ڹ��캯���е���һ��
*
*  @return void		�޷���ֵ
*/
void RoomManager::__Init()
{
	__ClearRoom();
	__ClearHandleCommon(); 
	__ClearRoleCache();
	//1.���������ļ��������� ��ӷ��� ���handleCommon�ӿ�
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator CIter= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.cbegin();//ͨ��ţţ�ķ���
	for (; CIter != g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.cend(); ++CIter)
	{
		CreateRoom(CIter->second.bRoomID, ConvertToRoomType(CIter->second.bRoomType));//�����µķ���
	}
}
/**
*  ��������:		���ٺ��� �����������е���һ��
*
*  @return void		�޷���ֵ
*/
void RoomManager::__Destory()
{
	__ClearRoom();
	__ClearHandleCommon();
	__ClearRoleCache();
}
/**
*  ��������:		�ж�ָ�����͵���Ϣģ���Ƿ����
*  @param eRoomType �������͵�ö��
*
*  @return bool		���ڷ���true  �����ڷ���false
*/
bool RoomManager::__IsExistsHandleCommon(RoomType eRoomType) const
{
	HashMap<RoomType, IHandleCommon*>::const_iterator Iter = m_HandleCommonHashMap.find(eRoomType);
	if (Iter == m_HandleCommonHashMap.end())
		return false;
	else
		return true;
}
/**
*  ��������:		����µ���Ϣ����ģ��
*  @param pHandleCommon �µ���Ϣ����ģ���ָ��  �п���Ϊnullptr
*
*  @return void		�޷���ֵ
*/
void RoomManager::__AddHandleCommon(IHandleCommon* pHandleCommon)
{
	if (nullptr == pHandleCommon)
	{
		ASSERT(false);
		return;
	}
	m_HandleCommonHashMap.insert(HashMap<RoomType, IHandleCommon*>::value_type(pHandleCommon->GetRoomType(),pHandleCommon));
}
/**
*  ��������:		����µķ���
*  @param pRoom		�µķ����ָ��  �п���Ϊnullptr
*
*  @return void		�޷���ֵ
*/
void RoomManager::__AddRoom(IRoom* pRoom)
{
	if (nullptr == pRoom)
	{
		ASSERT(false);
		return;
	}
	m_RoomHashMap.insert(HashMap<BYTE, IRoom*>::value_type(pRoom->GetRoomID(),pRoom));
}
/**
*  ��������: ���ݷ���ID ��ѯ�������� ��������� ����nullptr
*  @param	bRoomID		����ID
*
*  @return	IRoom*		����ĳ���ӿ�
*/
IRoom* RoomManager::FindRoom(BYTE bRoomTypeID) const
{
	HashMap<BYTE, IRoom*>::const_iterator Iter = m_RoomHashMap.find(bRoomTypeID);
	if (m_RoomHashMap.end() == Iter)
		return nullptr;
	else
		return Iter->second;
}
/**
*  ��������: ���ݷ���ID ������ɾ����
*  @param	bRoomID		����ID
*
*  @return  void		�޷��ز���
*/
void RoomManager::DestoryRoom(BYTE bRoomTypeID)
{
	HashMap<BYTE, IRoom*>::iterator Iter = m_RoomHashMap.find(bRoomTypeID);
	if (Iter != m_RoomHashMap.end())
	{
		delete Iter->second;
		m_RoomHashMap.erase(Iter);
	}
}
/**
*  ��������: ���º��� ���ڸ��¹������ڲ�����
*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
*
*  @return  void		�޷��ز���
*/
void RoomManager::Update(DWORD dwTimer)
{
	HashMap<BYTE, IRoom*>::iterator Iter = m_RoomHashMap.begin();
	for (; Iter != m_RoomHashMap.end(); ++Iter)
	{
		if (nullptr != Iter->second)
			Iter->second->Update(dwTimer);
	}
}
/**
*  ��������: �����ⲿ��Ϣ����
*  @param	pCmd		�ⲿ�����ָ��
*
*  @return  void		�޷��ز���
*/
void RoomManager::HandleMsg(NetCmd* pCmd)
{
	HashMap<RoomType, IHandleCommon*>::iterator Iter = m_HandleCommonHashMap.begin();
	for (; Iter != m_HandleCommonHashMap.end(); ++Iter)
	{
		if (nullptr == Iter->second)
			continue;
		if (!Iter->second->HandleMsg(pCmd))
			continue;
		else
			return;
	}
	ASSERT(false);
	return;
}
/**
*  ��������:		��ѯ��ҵķ���ID ������ڷ��� ����0
*  @param dwUserID  ��ҵ�ID
*
*  @return BYTE		���ط����RoomID
*/
BYTE RoomManager::QueryRoleRoomID(DWORD dwUserID) const
{
	//���ݼ�¼������ ��ȡ��ҵķ���ID
	HashMap<DWORD, BYTE>::const_iterator Iter = m_RoleRoomIDHashMap.find(dwUserID);
	if (m_RoleRoomIDHashMap.end() == Iter)
		return 0;
	else
		return Iter->second;
}
/**
*  ��������:		��ѯ��ҵ�����ID ����������� ����0
*  @param dwUserID  ��ҵ�ID
*
*  @return WORD		�������ӵ�TableID
*/
WORD RoomManager::QueryRoleTableID(DWORD dwUserID) const
{
	//���ݼ�¼������ ��ȡ��ҵ�����ID
	HashMap<DWORD, WORD>::const_iterator Iter = m_RoleTableIDHashMap.find(dwUserID);
	if (m_RoleTableIDHashMap.end() == Iter)
		return 0;
	else
		return Iter->second;
}
/**
*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleRoomID ������ѯ
*  @param dwUserID  ���ID
*  @param bRoomID	����ID
*
*  @return void		�޷���ֵ
*/
void RoomManager::SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID)
{
	HashMap<DWORD, BYTE>::iterator Iter = m_RoleRoomIDHashMap.find(dwUserID);
	if (m_RoleRoomIDHashMap.end() == Iter)
	{
		m_RoleRoomIDHashMap.insert(HashMap<DWORD, BYTE>::value_type(dwUserID,bRoomID));
	}
	else
	{
		Iter->second = bRoomID;
	}
}
/**
*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleTableID ������ѯ
*  @param dwUserID  ���ID
*  @param wTableID	����ID
*
*  @return void		�޷���ֵ
*/
void RoomManager::SetRoleJoinTable(DWORD dwUserID, WORD wTableID)
{
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableIDHashMap.find(dwUserID);
	if (m_RoleTableIDHashMap.end() == Iter)
	{
		m_RoleTableIDHashMap.insert(HashMap<DWORD, WORD>::value_type(dwUserID, wTableID));
	}
	else
	{
		Iter->second = wTableID;
	}
}
/**
*  ��������:		ɾ�����ID�ͷ���ID ���Һ� ����ID ��ȫ������
*  @param dwUserID  ��ҵ�ID
*
*  @return void		�޷���ֵ
*/
void RoomManager::SetRoleLeaveRoom(DWORD dwUserID)
{
	//�뿪���� ����ͬʱ�뿪����
	m_RoleRoomIDHashMap.erase(dwUserID);
	m_RoleTableIDHashMap.erase(dwUserID);
}
/**
*  ��������:		ɾ�����ID������ID �Ļ���
*  @param dwUserID  ��ҵ�ID
*
*  @return void		�޷���ֵ
*/
void RoomManager::SetRoleLeaveTable(DWORD dwUserID)
{
	m_RoleTableIDHashMap.erase(dwUserID);
}
/**
*  ��������:		��ȡָ�����͵���Ϣ����ģ��
*  @param eRoomType	��������ö��
*
*  @return IHandleCommon* ģ��ָ��  ���ܷ���nullptr
*/
IHandleCommon* RoomManager::__GetHandleCommon(RoomType eRoomType) const
{
	HashMap<RoomType, IHandleCommon*>::const_iterator Iter = m_HandleCommonHashMap.find(eRoomType);
	if (Iter == m_HandleCommonHashMap.end())
		return nullptr;
	else
		return Iter->second;
}
/**
*  ��������:		���ݲ����������󷿼�ӿ�ָ��
*  @param bRoomID   ����ID
*  @param eRoomType ���������ö��
*
*  @return IRoom*   ����ĳ���ӿ�
*/
IRoom* RoomManager::CreateRoom(BYTE bRoomID, RoomType eRoomType) //Ψһ�ƻ����յĵط�
{
	switch (eRoomType)
	{
	case RoomType::RoomType_NiuNiu:
		{
			IHandleCommon* pHandleCommon = __GetHandleCommon(eRoomType);
			if (nullptr == pHandleCommon)
			{
				pHandleCommon = new NiuNiuHandleCommon(eRoomType, dynamic_cast<IRoomManager*>(this), dynamic_cast<IRoomRoleManager*>(this));
				__AddHandleCommon(pHandleCommon);
			}
			IRoom* pRoom = new NiuNiuRoom(bRoomID, eRoomType, pHandleCommon);
			if (nullptr == pRoom)
			{
				ASSERT(false);
				return nullptr;
			}
			__AddRoom(pRoom);
			return pRoom;
		}
		break;
	default:
		return nullptr;
		break;
	}
	return nullptr;
}

