/*
*@file   RoomManager.cpp
*@brief  管理器->房间->桌子->玩家 管理的接口模块
*@author 徐达
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
*  功能描述:		清理房间管理器管理全部房间的指针
*
*  @return void		无返回值
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
*  功能描述:		清理房间管理器管理全部消息模块的指针
*
*  @return void		无返回值
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
*  功能描述:		清理玩家房间和桌子记录的缓存
*
*  @return void		无返回值
*/
void RoomManager::__ClearRoleCache()
{
	m_RoleRoomIDHashMap.clear();
	m_RoleTableIDHashMap.clear();
}
/**
*  功能描述:		初始化函数 在构造函数中调用一次
*
*  @return void		无返回值
*/
void RoomManager::__Init()
{
	__ClearRoom();
	__ClearHandleCommon(); 
	__ClearRoleCache();
	//1.根据配置文件创建房间 添加房间 添加handleCommon接口
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator CIter= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.cbegin();//通比牛牛的房间
	for (; CIter != g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.cend(); ++CIter)
	{
		CreateRoom(CIter->second.bRoomID, ConvertToRoomType(CIter->second.bRoomType));//创建新的房间
	}
}
/**
*  功能描述:		销毁函数 在析构函数中调用一次
*
*  @return void		无返回值
*/
void RoomManager::__Destory()
{
	__ClearRoom();
	__ClearHandleCommon();
	__ClearRoleCache();
}
/**
*  功能描述:		判断指定类型的消息模块是否存在
*  @param eRoomType 房间类型的枚举
*
*  @return bool		存在返回true  不存在返回false
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
*  功能描述:		添加新的消息处理模块
*  @param pHandleCommon 新的消息处理模块的指针  有可能为nullptr
*
*  @return void		无返回值
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
*  功能描述:		添加新的房间
*  @param pRoom		新的房间的指针  有可能为nullptr
*
*  @return void		无返回值
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
*  功能描述: 根据房间ID 查询房间桌子 如果不存在 返回nullptr
*  @param	bRoomID		房间ID
*
*  @return	IRoom*		房间的抽象接口
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
*  功能描述: 根据房间ID 将房间删除掉
*  @param	bRoomID		房间ID
*
*  @return  void		无返回参数
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
*  功能描述: 更新函数 用于更新管理器内部数据
*  @param	dwTimer		当前时间 由timeGetTime()产生
*
*  @return  void		无返回参数
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
*  功能描述: 处理外部消息函数
*  @param	pCmd		外部命令的指针
*
*  @return  void		无返回参数
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
*  功能描述:		查询玩家的房间ID 如果不在房间 返回0
*  @param dwUserID  玩家的ID
*
*  @return BYTE		返回房间的RoomID
*/
BYTE RoomManager::QueryRoleRoomID(DWORD dwUserID) const
{
	//根据记录的数据 获取玩家的房间ID
	HashMap<DWORD, BYTE>::const_iterator Iter = m_RoleRoomIDHashMap.find(dwUserID);
	if (m_RoleRoomIDHashMap.end() == Iter)
		return 0;
	else
		return Iter->second;
}
/**
*  功能描述:		查询玩家的桌子ID 如果不在桌子 返回0
*  @param dwUserID  玩家的ID
*
*  @return WORD		返回桌子的TableID
*/
WORD RoomManager::QueryRoleTableID(DWORD dwUserID) const
{
	//根据记录的数据 获取玩家的桌子ID
	HashMap<DWORD, WORD>::const_iterator Iter = m_RoleTableIDHashMap.find(dwUserID);
	if (m_RoleTableIDHashMap.end() == Iter)
		return 0;
	else
		return Iter->second;
}
/**
*  功能描述:		设置玩家ID 和 房间ID的缓存 供 QueryRoleRoomID 函数查询
*  @param dwUserID  玩家ID
*  @param bRoomID	房间ID
*
*  @return void		无返回值
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
*  功能描述:		设置玩家ID 和 桌子ID的缓存 供 QueryRoleTableID 函数查询
*  @param dwUserID  玩家ID
*  @param wTableID	桌子ID
*
*  @return void		无返回值
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
*  功能描述:		删除玩家ID和房间ID 并且和 桌子ID 的全部缓存
*  @param dwUserID  玩家的ID
*
*  @return void		无返回值
*/
void RoomManager::SetRoleLeaveRoom(DWORD dwUserID)
{
	//离开房间 等于同时离开桌子
	m_RoleRoomIDHashMap.erase(dwUserID);
	m_RoleTableIDHashMap.erase(dwUserID);
}
/**
*  功能描述:		删除玩家ID和桌子ID 的缓存
*  @param dwUserID  玩家的ID
*
*  @return void		无返回值
*/
void RoomManager::SetRoleLeaveTable(DWORD dwUserID)
{
	m_RoleTableIDHashMap.erase(dwUserID);
}
/**
*  功能描述:		获取指定类型的消息处理模块
*  @param eRoomType	房间类型枚举
*
*  @return IHandleCommon* 模块指针  可能返回nullptr
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
*  功能描述:		根据参数创建抽象房间接口指针
*  @param bRoomID   房间ID
*  @param eRoomType 房间的类型枚举
*
*  @return IRoom*   房间的抽象接口
*/
IRoom* RoomManager::CreateRoom(BYTE bRoomID, RoomType eRoomType) //唯一破坏开闭的地方
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

