/*
*@file   RoomManager.h
*@brief  管理器->房间->桌子->玩家 管理的接口模块
*@author 徐达
*@data   2016-6-14 10:00
*/
#pragma once
#include "RoleManager.h"

class IRoom;
class ITable;
class ITableRole;
class IHandleCommon;

enum RoomType //房间类型
{ 
	//枚举循环判断的 范围为 (RoomType_Init,RoomType_Max) 而不是[RoomType_Init,RoomType_Max]
	RoomType_Init		= 0, //开始的ID 枚举循环判断用的 无实际指定的房间类型
	RoomType_NiuNiu		= 1, //通比牛牛房间的类型ID
	RoomType_Max,			 //最大值 枚举循环判断用的 无实际指定的房间类型
};
//

template<typename T>
/**
*  功能描述:		用于BYTE 或者其他整形转化为 RoomType 如果失败 转化为 RoomType_Init
*  @param T			待转化的整形
*
*  @return RoomType 转化后的枚举
*/
inline RoomType ConvertToRoomType(T nT)
{
	T nMin = static_cast<T>(RoomType::RoomType_Init);
	T nMax = static_cast<T>(RoomType::RoomType_Max);
	if (nT >= nMax || nT <= nMin)
		return RoomType::RoomType_Init;
	else
		return static_cast<RoomType>(nT);
};

class ITableRobotManager
{
public:
	explicit ITableRobotManager(ITable* pTable, IHandleCommon* pHandleCommon){}
	virtual ~ITableRobotManager(){}

	//机器人进出操作
	virtual void OnCreateTableEvent() = 0;
	virtual void OnRoleJoinTableEvent(DWORD dwUserID) = 0;
	virtual void OnRoleLeaveTableEvent(DWORD dwUserID) = 0;
	virtual void OnTableDestoryEvent() = 0;

	//机器人流程控制
	virtual void OnBeginTableStateChange(ITable* pTable) = 0;
	virtual void OnEndTableStateChange(ITable* pTable) = 0;

	virtual void Update(DWORD dwTimer) = 0;
private:
	virtual	void __AddNewRobot() = 0;
	virtual void __DelRobot(DWORD dwUserID) = 0;
};

class IRoomManager
{
public:
	explicit IRoomManager(){}
	virtual ~IRoomManager(){}

	/**
	*  功能描述:		根据参数创建抽象房间接口指针
	*  @param bRoomID   房间ID
	*  @param eRoomType 房间的类型枚举
	*
	*  @return IRoom*   房间的抽象接口
	*/
	virtual	 IRoom*		CreateRoom(BYTE bRoomID,RoomType eRoomType) = 0;//创建一个房间
	/**
	*  功能描述: 根据房间ID 查询房间桌子 如果不存在 返回nullptr
	*  @param	bRoomID		房间ID 
	*
	*  @return	IRoom*		房间的抽象接口
	*/
	virtual  IRoom*		FindRoom(BYTE bRoomID) const = 0;//查询房间
	/**
	*  功能描述: 根据房间ID 将房间删除掉
	*  @param	bRoomID		房间ID
	*
	*  @return  void		无返回参数
	*/
	virtual  void		DestoryRoom(BYTE bRoomID) = 0;//删除一个房间
	/**
	*  功能描述: 更新函数 用于更新管理器内部数据 
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//管理器更新
	/**
	*  功能描述: 处理外部消息函数
	*  @param	pCmd		外部命令的指针
	*
	*  @return  void		无返回参数
	*/
	virtual  void		HandleMsg(NetCmd* pCmd) = 0;//管理器处理外部命令

	IRoomManager(const IRoomManager&) = delete;
	IRoomManager& operator=(const IRoomManager&) = delete;
};

class IRoomRoleManager
{
public:
	explicit IRoomRoleManager(){}
	virtual ~IRoomRoleManager(){}

	/**
	*  功能描述:		查询玩家的房间ID 如果不在房间 返回0
	*  @param dwUserID  玩家的ID
	*
	*  @return BYTE		返回房间的RoomID 
	*/
	virtual BYTE QueryRoleRoomID(DWORD dwUserID) const = 0;//查询玩家房间ID
	/**
	*  功能描述:		查询玩家的桌子ID 如果不在桌子 返回0
	*  @param dwUserID  玩家的ID
	*
	*  @return WORD		返回桌子的TableID
	*/
	virtual WORD QueryRoleTableID(DWORD dwUserID) const = 0;//查询玩家桌子ID
	/**
	*  功能描述:		设置玩家ID 和 房间ID的缓存 供 QueryRoleRoomID 函数查询
	*  @param dwUserID  玩家ID
	*  @param bRoomID	房间ID
	*
	*  @return void		无返回值
	*/
	virtual void SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID) = 0;//设置玩家房间ID
	/**
	*  功能描述:		设置玩家ID 和 桌子ID的缓存 供 QueryRoleTableID 函数查询
	*  @param dwUserID  玩家ID
	*  @param wTableID	桌子ID
	*
	*  @return void		无返回值
	*/
	virtual void SetRoleJoinTable(DWORD dwUserID, WORD wTableID) = 0;//设置玩家桌子ID
	/**
	*  功能描述:		删除玩家ID和房间ID 并且和 桌子ID 的全部缓存 
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回值
	*/
	virtual void SetRoleLeaveRoom(DWORD dwUserID) = 0;//玩家离开房间
	/**
	*  功能描述:		删除玩家ID和桌子ID 的缓存
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回值
	*/
	virtual void SetRoleLeaveTable(DWORD dwUserID) = 0;//玩家离开桌子

	IRoomRoleManager(const IRoomRoleManager&) = delete;
	IRoomRoleManager& operator=(const IRoomRoleManager&) = delete;
};

class IRoom
{
public:
	/**
	*  功能描述:		构造函数  explicit 修饰
	*  @param bRoomID   房间的ID
	*  @param eRoomType 房间的类型枚举
	*  @param pHandleCommon 消息处理类的指针
	*/
	explicit IRoom(BYTE bRoomID, RoomType eRoomType, IHandleCommon* pHandleCommon) {}
	virtual ~IRoom(){}

	/**
	*  功能描述:		获取当前房间的房间ID
	*
	*  @return BYTE		返回房间的RoomID
	*/
	virtual BYTE		GetRoomID() const = 0; //获取房间ID
	/**
	*  功能描述:		获取当前房间的房间类型的枚举
	*
	*  @return RoomType 返回房间的房间类型枚举
	*/
	virtual RoomType	GetRoomType() const = 0;//获取房间类型
	/**
	*  功能描述:		在当前房间下创建并且添加一个新的桌子
	*
	*  @return ITable*  返回创建的桌子的指针  有可能为 nullptr
	*/
	virtual ITable*		CreateTable() = 0;//创建桌子
	/**
	*  功能描述:		根据桌子ID 查询桌子的指针
	*  @param wTableID  桌子的ID
	*
	*  @return ITable*	查询的桌子的指针 有可能为nullptr
	*/
	virtual ITable*		FindTable(WORD wTableID) const = 0;//查询桌子
	/**
	*  功能描述:		根据桌子ID 删除指定桌子
	*  @param wTableID  桌子的ID
	*
	*  @return void		无返回参数
	*/
	virtual void		DestoryTable(WORD wTableID) = 0;//删除桌子
	/**
	*  功能描述: 更新函数 用于更新房间内部数据
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//房间更新

	IRoom(const IRoom&) = delete;
	IRoom& operator=(const IRoom&) = delete;
};
class ITable
{
public:
	/**
	*  功能描述:		构造函数  explicit 修饰
	*  @param bRoomID   所属房间的ID
	*  @param wTableID  当前桌子ID
	*/
	explicit ITable(BYTE bRoomID, WORD wTableID, IHandleCommon* pHandleCommon){}
	virtual ~ITable(){}

	/**
	*  功能描述:		获取当前桌子的桌子ID
	*
	*  @return WORD		返回桌子的桌子ID
	*/
	virtual WORD		GetTableID() const = 0;//获取桌子ID
	/**
	*  功能描述:		获取当前桌子的房间ID
	*
	*  @return BYTE		返回桌子的房间ID
	*/
	virtual BYTE		GetRoomID() const = 0;//或者桌子的房间ID
	/**
	*  功能描述:		根据玩家ID 创建并且添加一个桌子玩家
	*  @param dwUserID  玩家ID
	*
	*  @return ITableRole*	创建的桌子玩家的指针 有可能为 nullptr
	*/
	virtual ITableRole* CreateTableRole(DWORD dwUserID) = 0;//创建桌子上一个玩家
	/**
	*  功能描述:		根据玩家ID 查询桌子上的玩家指针
	*  @param dwUserID  玩家ID
	*
	*  @return ITableRole*	查询的桌子玩家的指针 有可能为 nullptr
	*/
	virtual ITableRole* FindTableRole(DWORD dwUserID) const = 0;//查询玩家
	/**
	*  功能描述:		根据玩家ID 删除指定桌子上的玩家
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回参数
	*/
	virtual void		DestoryTableRole(DWORD dwUserID) = 0;//删除桌子上一个玩家
	/**
	*  功能描述:		获取桌子上玩家的人数
	*
	*  @return BYTE		桌子上玩家人数
	*/
	virtual BYTE		GetTableRoleSum() const = 0;//获取桌子上人数
	/**
	*  功能描述: 更新函数 用于更新桌子内部数据
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//桌子更新

	ITable(const ITable&) = delete;
	ITable& operator=(const ITable&) = delete;
};

class ITableRole
{
public:
	/**
	*  功能描述:		构造函数  explicit 修饰
	*  @param bSeatID   所属桌子上的位置
	*  @param dwUserID  当前玩家ID
	*/
	explicit ITableRole(BYTE bSeatID,DWORD dwUserID){}
	virtual ~ITableRole(){}

	/**
	*  功能描述:		获取玩家的ID
	*
	*  @return DWORD	玩家ID
	*/
	virtual DWORD	GetRoleID() const = 0;//获取玩家ID
	/**
	*  功能描述:		获取玩家的外部对象指针
	*
	*  @return Role*	玩家指针 有可能为 nullptr
	*/
	virtual	Role*	GetRole() const = 0;//获取玩家数据指针
	/**
	*  功能描述:		获取玩家的位置索引(从0开始)
	*
	*  @return BYTE		玩家的位置索引(从0开始)
	*/
	virtual BYTE	GetSeatID() const = 0;//获取玩家座位ID

	ITableRole(const ITableRole&) = delete;
	ITableRole& operator=(const ITableRole&) = delete;
};

class IHandleCommon
{
public:
	/**
	*  功能描述:		构造函数  explicit 修饰
	*  @param eRoomType 房间类型
	*  @param pManager  管理器的桌子 用于查询房间
	*  @param pRoleManager  玩家数据缓存桌子 用于设置和获取玩家的桌子ID 和房间ID
	*/
	explicit IHandleCommon(RoomType eRoomType, IRoomManager* pManager, IRoomRoleManager* pRoleManager){}
	virtual ~IHandleCommon(){}

	/**
	*  功能描述:		获取当前接口的房间类型枚举
	*
	*  @return RoomType	房间类型枚举
	*/
	virtual RoomType GetRoomType() const = 0;//获取当前处理类的所属的房间类型ID
	/**
	*  功能描述: 用于处理管理器发送来的消息 如果可以处理 返回true  不可以处理返回false
	*  @param	NetCmd*		命令的指针  与可能为nullptr
	*
	*  @return  bool		是否可以处理
	*/
	virtual bool HandleMsg(NetCmd* pCmd) = 0; //处理命令

	IHandleCommon(const IHandleCommon&) = delete;
	IHandleCommon& operator=(const IHandleCommon&) = delete;
};


//具体的实现类
class RoomManager : public IRoomManager, public IRoomRoleManager
{
public:
	explicit RoomManager();
	virtual ~RoomManager();

	/**
	*  功能描述:		根据参数创建抽象房间接口指针
	*  @param bRoomID   房间ID
	*  @param eRoomType 房间的类型枚举
	*
	*  @return IRoom*   房间的抽象接口
	*/
	virtual	 IRoom*		CreateRoom(BYTE bRoomID,RoomType eRoomType);
	/**
	*  功能描述: 根据房间ID 查询房间桌子 如果不存在 返回nullptr
	*  @param	bRoomID		房间ID
	*
	*  @return	IRoom*		房间的抽象接口
	*/
	virtual  IRoom*		FindRoom(BYTE bRoomID) const;
	/**
	*  功能描述: 根据房间ID 将房间删除掉
	*  @param	bRoomID		房间ID
	*
	*  @return  void		无返回参数
	*/
	virtual  void		DestoryRoom(BYTE bRoomID);
	/**
	*  功能描述: 更新函数 用于更新管理器内部数据
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual  void		Update(DWORD dwTimer);
	/**
	*  功能描述: 处理外部消息函数
	*  @param	pCmd		外部命令的指针
	*
	*  @return  void		无返回参数
	*/
	virtual  void		HandleMsg(NetCmd* pCmd);
	/**
	*  功能描述:		查询玩家的房间ID 如果不在房间 返回0
	*  @param dwUserID  玩家的ID
	*
	*  @return BYTE		返回房间的RoomID
	*/
	virtual  BYTE		QueryRoleRoomID(DWORD dwUserID) const;
	/**
	*  功能描述:		查询玩家的桌子ID 如果不在桌子 返回0
	*  @param dwUserID  玩家的ID
	*
	*  @return WORD		返回桌子的TableID
	*/
	virtual  WORD		QueryRoleTableID(DWORD dwUserID) const;
	/**
	*  功能描述:		设置玩家ID 和 房间ID的缓存 供 QueryRoleRoomID 函数查询
	*  @param dwUserID  玩家ID
	*  @param bRoomID	房间ID
	*
	*  @return void		无返回值
	*/
	virtual	 void		SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID);
	/**
	*  功能描述:		设置玩家ID 和 桌子ID的缓存 供 QueryRoleTableID 函数查询
	*  @param dwUserID  玩家ID
	*  @param wTableID	桌子ID
	*
	*  @return void		无返回值
	*/
	virtual  void		SetRoleJoinTable(DWORD dwUserID, WORD wTableID);
	/**
	*  功能描述:		删除玩家ID和房间ID 并且和 桌子ID 的全部缓存
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回值
	*/
	virtual  void		SetRoleLeaveRoom(DWORD dwUserID);
	/**
	*  功能描述:		删除玩家ID和桌子ID 的缓存
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回值
	*/
	virtual  void		SetRoleLeaveTable(DWORD dwUserID);

	RoomManager(const RoomManager&) = delete;
	RoomManager& operator=(const RoomManager&) = delete;
private:
	/**
	*  功能描述:		初始化函数 在构造函数中调用一次
	*
	*  @return void		无返回值
	*/
	void				__Init();//初始化
	/**
	*  功能描述:		销毁函数 在析构函数中调用一次
	*
	*  @return void		无返回值
	*/
	void				__Destory();//销毁
	/**
	*  功能描述:		清理房间管理器管理全部房间的指针
	*
	*  @return void		无返回值
	*/
	void				__ClearRoom();//清理房间
	/**
	*  功能描述:		清理房间管理器管理全部消息模块的指针
	*
	*  @return void		无返回值
	*/
	void				__ClearHandleCommon();//清理处理模块
	/**
	*  功能描述:		清理玩家房间和桌子记录的缓存
	*
	*  @return void		无返回值
	*/
	void				__ClearRoleCache();//清理玩家房间和桌子记录的缓存
	/**
	*  功能描述:		判断指定类型的消息模块是否存在
	*  @param eRoomType 房间类型的枚举
	*
	*  @return bool		存在返回true  不存在返回false
	*/
	bool				__IsExistsHandleCommon(RoomType eRoomType) const;//判断是否已经存在指定房间类型的消息处理模块
	/**
	*  功能描述:		添加新的消息处理模块
	*  @param pHandleCommon 新的消息处理模块的指针  有可能为nullptr
	*
	*  @return void		无返回值
	*/
	void				__AddHandleCommon(IHandleCommon* pHandleCommon);//添加新的消息处理模块
	/**
	*  功能描述:		添加新的房间
	*  @param pRoom		新的房间的指针  有可能为nullptr
	*
	*  @return void		无返回值
	*/
	void				__AddRoom(IRoom* pRoom);//添加房间
	/**
	*  功能描述:		获取指定类型的消息处理模块
	*  @param eRoomType	房间类型枚举
	*
	*  @return IHandleCommon* 模块指针  可能返回nullptr
	*/
	IHandleCommon*		__GetHandleCommon(RoomType eRoomType) const;
private:
	HashMap<BYTE, IRoom*>					m_RoomHashMap;//全部房间的HashMap
	HashMap<RoomType,IHandleCommon*>		m_HandleCommonHashMap;//全部消息处理模块的HashMap
	HashMap<DWORD, BYTE>					m_RoleRoomIDHashMap;//玩家房间ID的缓存
	HashMap<DWORD, WORD>					m_RoleTableIDHashMap;//玩家桌子ID的缓存
};