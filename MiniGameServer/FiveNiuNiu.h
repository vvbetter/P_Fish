#pragma once
/*
*@file   FiveNiuNiu.h
*@brief  通比牛牛实现类
*@author 徐达
*@data   2016-6-14 18:00
*/
#include "RoomManager.h"
class NiuNiuRoom;
class NiuNiuTable;
class NiuNiuTableRole;

//通比牛牛
enum FiveNiuNiuTableStates	//桌子的状态
{
	FNNTS_Begin = 1,		//下注阶段
	FNNTS_End = 2,
	FNNTS_WriteBegin = 3,	//等待开始
};

struct NiuNiuOnceBrandList//一套牌
{
private:
	std::vector<BYTE> BrandVec;//牌的集合
public:
	NiuNiuOnceBrandList()
	{
		OnClear();
	}
	/**
	*  功能描述:		从当前这套牌里随机抽取一张牌
	*
	*  @return BYTE		抽取的牌
	*/
	BYTE GetNewBrand()
	{
		BYTE Index = RandUInt() % BrandVec.size();
		BYTE BrandValue = BrandVec[Index];
		BrandVec[Index] = BrandVec[BrandVec.size() - 1];
		BrandVec.pop_back();
		return BrandValue;
	}
	/**
	*  功能描述:			将一张牌插会当前这套牌里面
	*  @param BrandValue	待插入的牌
	*
	*  @return void			无返回值
	*/
	void AddBrand(BYTE BrandValue)
	{
		BrandVec.push_back(BrandValue);
	}
	/**
	*  功能描述:		生成一套新的牌
	*
	*  @return void		无返回值
	*/
	void OnClear()
	{
		BrandVec.clear();
		for (BYTE i = 1; i <= 52; ++i)
			BrandVec.push_back(i);
	}
};

class NiuNiuHandleCommon : public IHandleCommon, public IRoomRoleManager  //命令处理核心类 用于处理房间和桌子上的逻辑
{
public:
	/**
	*  功能描述:			构造函数 explicit修饰
	*  @param eRoomType		房间的类型枚举
	*  @param pManager		房间管理器的接口 可能为nullptr
	*  @param pRoleManager	玩家缓存管理的接口 可能为Nullptr
	*
	*/
	explicit NiuNiuHandleCommon(RoomType eRoomType, IRoomManager* pManager, IRoomRoleManager* pRoleManager);
	~NiuNiuHandleCommon();

	NiuNiuHandleCommon(const NiuNiuHandleCommon&) = delete;
	NiuNiuHandleCommon& operator=(const NiuNiuHandleCommon&) = delete;

	/**
	*  功能描述:			获取房间类型枚举
	*
	*  @return RoomType		房间类型枚举
	*/
	virtual RoomType	GetRoomType() const { return m_eRoomType; }
	/**
	*  功能描述:			处理管理器发送来的消息
	*  @param pCmd			待处理命令的指针  可能为nullptr
	*
	*  @return bool			是否可以处理 可以处理返回true  不可以处理返回false
	*/
	virtual bool		HandleMsg(NetCmd* pCmd); //处理命令
	/**
	*  功能描述:			当一个桌子进入开始阶段的时候触发
	*  @param bRoomID		指定的房间的ID
	*  @param wTableID		指定的桌子的ID
	*
	*  @return void			无返回值
	*/
	void				OnTableJoinBegin(BYTE bRoomID, WORD wTableID);
	/**
	*  功能描述:			当一个桌子进入等待开始阶段的时候触发
	*  @param bRoomID		指定的房间的ID
	*  @param wTableID		指定的桌子的ID
	*
	*  @return void			无返回值
	*/
	void				OnTableJoinWriteBegin(BYTE bRoomID, WORD wTableID);
	/**
	*  功能描述:			当一个桌子进入结束阶段的时候触发
	*  @param bRoomID		指定的房间的ID
	*  @param wTableID		指定的桌子的ID
	*
	*  @return void			无返回值
	*/
	void				OnTableJoinEnd(BYTE bRoomID, WORD wTableID);
	/**
	*  功能描述:			从配置文件获取指定房间ID的房间配置
	*  @param bRoomID		指定的房间的ID
	*
	*  @return tagNiuNiuRoomConfig* 配置文件对象指针
	*/
	const tagNiuNiuRoomConfig* GetNiuNiuRoomConfig(BYTE bRoomID) const;
	/**
	*  功能描述:			从配置文件获取指定房间ID的倍率配置
	*  @param bRoomID		指定的房间的ID
	*
	*  @return tagFiveNiuNiuRate* 配置文件倍率对象指针
	*/
	const tagFiveNiuNiuRate*   GetNiuNiuRateConfig(BYTE bRoomID) const;
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
	/**
	*  功能描述:			根据牌的数组 计算牌的大小 牌的数量为 MAX_NIUNIU_BrandSum
	*  @param pNiuNiuTable	桌子的指针  可能为nullptr
	*  @param dwUserID		玩家的ID
	*
	*  @return NiuNiuTableRole*	玩家的指针 可能为nullptr
	*/
	virtual WORD		HandleBrandValue(BYTE* BrandValue);
private:
	/**
	*  功能描述:		初始化函数 在构造函数中调用一次
	*
	*  @return void		无返回值
	*/
	void				__Init();
	/**
	*  功能描述:		销毁函数 在析构函数中调用一次
	*
	*  @return void		无返回值
	*/
	void				__Destory();
	/**
	*  功能描述:			具体消息处理函数 显示房间内全部的桌子
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleShowRoomTableInfo(GM_Cmd_FShowTableInfo* pMsg);
	/**
	*  功能描述:			具体消息处理函数 玩家进图房间内的桌子的时候
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleJoinRoomTable(GM_Cmd_FJoinRoomTable* pMsg);
	/**
	*  功能描述:			具体消息处理函数 玩家离开桌子的时候
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleLeaveTable(GM_Cmd_FRoleLeaveTable* pMsg);
	/**
	*  功能描述:			具体消息处理函数 玩家离开房间的时候
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleLeaveRoom(GM_Cmd_FRoleLeaveRoom* pMsg);
	/**
	*  功能描述:			具体消息处理函数 玩家信息变化的时候 一般为玩家充值的时候
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleRoleInfoChange(GM_Cmd_FRoleInfoChange* pMsg);
	/**
	*  功能描述:			具体消息处理函数 玩家摊牌
	*  @param pMsg			具体消息 可能为nullptr
	*
	*  @return void			无返回值
	*/
	void				__HandleShowBrandInfo(GM_Cmd_ShowBrandInfo* pMsg);
	/**
	*  功能描述:			根据房间ID 查询房间的指针 
	*  @param bRoomID		房间的ID
	*
	*  @return NiuNiuRoom*	房间的指针 可能为nullptr
	*/
	NiuNiuRoom*			__FindRoom(BYTE bRoomID) const;
	/**
	*  功能描述:			根据房间指针 和桌子ID 查询桌子的指针
	*  @param pNiuNiuRoom	房间的指针  可能为nullptr
	*  @param bRoomID		桌子的ID
	*
	*  @return NiuNiuRoom*	桌子的指针 可能为nullptr
	*/
	NiuNiuTable*		__FindTable(const NiuNiuRoom* pNiuNiuRoom, WORD wTableID) const;
	/**
	*  功能描述:			根据桌子指针 和玩家ID 查询玩家的指针
	*  @param pNiuNiuTable	桌子的指针  可能为nullptr
	*  @param dwUserID		玩家的ID
	*
	*  @return NiuNiuTableRole*	玩家的指针 可能为nullptr
	*/
	NiuNiuTableRole*	__FindTableRole(const NiuNiuTable* pNiuNiuTable, DWORD dwUserID) const;
	/**
	*  功能描述:			根据玩家指针里牌的数据 获取 倍率
	*  @param pTableRole	玩家的指针  可能为nullptr
	*
	*  @return BYTE			倍率
	*/
	BYTE				__GetBrandRateValue(NiuNiuTableRole* pTableRole);
	/**
	*  功能描述:			根据牌的面值 计算牌的价值
	*  @param Value			牌
	*
	*  @return BYTE			价值
	*/
	BYTE				__GetBrandHandleValue(BYTE Value);
	/**
	*  功能描述:			交换A B 2个数据
	*  @param A				待交换数
	*  @param B				待交换数
	*
	*  @return void			无返回值
	*/
	void				__ChangeValue(BYTE& A, BYTE& B);
	/**
	*  功能描述:			发送显示房间信息失败的消息
	*  @param pRole			玩家指针
	*
	*  @return void			无返回值
	*/
	void				__SendShowRoomTableInfoFailed(Role* pRole) const;
	/**
	*  功能描述:			发送显示房间信息的消息
	*  @param pRole			玩家指针
	*  @param pNiuNiuRoom	房间的指针
	*
	*  @return void			无返回值
	*/
	void				__SendShowRoomTableInfoSuccess(Role* pRole, NiuNiuRoom* pNiuNiuRoom) const;
	/**
	*  功能描述:			发送玩家进入指定桌子失败的消息
	*  @param pRole			玩家指针
	*
	*  @return void			无返回值
	*/
	void				__SendJoinTableFailed(Role* pRole) const;
	/**
	*  功能描述:			发送桌子上其他玩家数据同步给我
	*  @param pNiuNiuTableRole	玩家的指针(需要同步的玩家)
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SendTableOtherRoleInfoToMe(NiuNiuTableRole* pNiuNiuTableRole,NiuNiuTable* pNiuNiuTable) const;
	/**
	*  功能描述:			将我的信息同步给桌子上的其他玩家
	*  @param pNiuNiuTableRole	玩家的指针(需要被同步的玩家)
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SendMeRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const;
	/**
	*  功能描述:			将玩家离开桌子的信息发送给桌子上其他的玩家
	*  @param pNiuNiuTable	桌子指针
	*  @param dwUserID		玩家ID
	*  @param bSeatID		玩家位置索引
	*
	*  @return void			无返回值
	*/
	void				__SendRoleLeaveInfoToTableOtherRole(NiuNiuTable* pNiuNiuTable ,DWORD dwUserID, BYTE bSeatID) const;
	/**
	*  功能描述:			将一个玩家最新的数据重新同步给桌子上其他玩家
	*  @param pNiuNiuTableRole	需要同步的玩家的指针
	*  @param pNiuNiuTable		桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SendNewRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const;
	/**
	*  功能描述:			将所有金币少于指定数量的玩家剔除
	*  @param pNiuNiuTable	桌子指针
	*  @param dwMinGlobel	最少金币数量
	*
	*  @return void			无返回值
	*/
	void				__KickTableRoleByGlobel(NiuNiuTable* pNiuNiuTable, DWORD dwMinGlobel);
	/**
	*  功能描述:			设置桌子上全部玩家的牌的数据
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SetTableRoleBrandInfo(NiuNiuTable* pNiuNiuTable);
	/**
	*  功能描述:			获取桌子上牌最大的玩家ID
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return DWORD		玩家ID
	*/
	DWORD				__GetTableMaxBrandValueUserID(NiuNiuTable* pNiuNiuTable) const;
	/**
	*  功能描述:			根据桌子上玩家牌的数据 将结构计算出来 并且发送到客户端去
	*  @param pNiuNiuTable	桌子指针
	*  @param dwMaxBrandValueUserID	牌最大的玩家ID
	*  @param bRate	桌子指针 当前桌子的倍率
	*
	*  @return void			无返回值
	*/
	void				__SendTableRoleGameResult(NiuNiuTable* pNiuNiuTable, DWORD dwMaxBrandValueUserID, BYTE bRate);
	/**
	*  功能描述:			判断当前房间是否需要创建桌子 需要的话 直接创建
	*  @param pNiuNiuRoom	房间指针
	*
	*  @return void			无返回值
	*/
	void				__CheckIsNeedCreateTable(NiuNiuRoom* pNiuNiuRoom);
	/**
	*  功能描述:			在当前房间上创建一个桌子
	*  @param pNiuNiuRoom	房间指针
	*
	*  @return void			无返回值
	*/
	void				__CreateTable(NiuNiuRoom* pNiuNiuRoom);
	/**
	*  功能描述:			将一个桌子的最新数据发送给在当前房间但不在桌子里的玩家
	*  @param pNiuNiuRoom	房间指针
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SendTableInfoToClient(NiuNiuRoom* pNiuNiuRoom, NiuNiuTable* pNiuNiuTable);
	/**
	*  功能描述:			获取在房间里但是不在桌子里的玩家的列表
	*  @param pNiuNiuRoom	房间指针
	*  @param rRoomRoleVec	玩家列表引用集合
	*
	*  @return void			无返回值
	*/
	void				__GetNotInTableRoleVec(NiuNiuRoom* pNiuNiuRoom, vector<DWORD>& rRoomRoleVec);
	/**
	*  功能描述:			改变桌子上全部玩家指定的金币
	*  @param pNiuNiuTable	桌子指针
	*  @param nLostGlobel	改变金币数量
	*
	*  @return void			无返回值
	*/
	void				__LostAllTableRoleGlobel(NiuNiuTable* pNiuNiuTable, int nLostGlobel);
	/**
	*  功能描述:			告诉桌子上全部玩家进入开始状态了
	*  @param pNiuNiuTable	桌子指针
	*
	*  @return void			无返回值
	*/
	void				__SendTableJoinBeginInfo(NiuNiuTable* pNiuNiuTable) const;
	/**
	*  功能描述:			设置桌子上指定玩家牌的数据
	*  @param pNiuNiuTable	桌子指针
	*  @param pNiuNiuTableRole	桌子上玩家的指针
	*  @param wBrandValue	玩家牌的大小
	*
	*  @return void			无返回值
	*/
	void				__SetRoleBrandValue(NiuNiuTable* pNiuNiuTable, NiuNiuTableRole* pNiuNiuTableRole, WORD wBrandValue,BYTE bChangeIndex);
	void				__SendNonShowBrandRole(NiuNiuTable* pNiuNiuTable);
private:
	RoomType			m_eRoomType;//消息模块的房间类型枚举
	IRoomManager*		m_pRoomManager;//管理器的接口
	IRoomRoleManager*	m_pRoomRoleManager;//玩家管理器的接口

	HashMap<DWORD, BYTE>	m_RoleRoomIDHashMap;//玩家房间ID的缓存 当前类型房间的
	HashMap<DWORD, WORD>	m_RoleTableIDHashMap;//玩家桌子ID的缓存 当前类型的
};

class NiuNiuRoom :public IRoom //牛牛房间
{
public:
	/**
	*  功能描述:			构造函数 explicit修饰
	*  @param bRoomID		房间ID
	*  @param eRoomType		房间的类型枚举
	*  @param pHandleCommon	消息处理类的指针 可能为nullptr
	*
	*/
	explicit NiuNiuRoom(BYTE bRoomID, RoomType eRoomType, IHandleCommon* pHandleCommon);
	~NiuNiuRoom();
	/**
	*  功能描述:		获取当前房间的房间ID
	*
	*  @return BYTE		返回房间的RoomID
	*/
	virtual BYTE					GetRoomID() const { return m_bRoomID; }
	/**
	*  功能描述:		获取当前房间的房间类型的枚举
	*
	*  @return RoomType 返回房间的房间类型枚举
	*/
	virtual RoomType				GetRoomType() const { return m_eRoomType; }
	/**
	*  功能描述:		在当前房间下创建并且添加一个新的桌子
	*
	*  @return ITable*  返回创建的桌子的指针  有可能为 nullptr
	*/
	virtual ITable*					CreateTable();
	/**
	*  功能描述:		根据桌子ID 查询桌子的指针
	*  @param wTableID  桌子的ID
	*
	*  @return ITable*	查询的桌子的指针 有可能为nullptr
	*/
	virtual ITable*					FindTable(WORD wTableID) const;
	/**
	*  功能描述:		根据桌子ID 删除指定桌子
	*  @param wTableID  桌子的ID
	*
	*  @return void		无返回参数
	*/
	virtual void					DestoryTable(WORD wTableID);
	/**
	*  功能描述: 更新函数 用于更新房间内部数据
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual void					Update(DWORD dwTimer);
	/**
	*  功能描述:			获取房间内全部桌子的HashMap的引用
	*
	*  @return  HashMap		全部桌子的HashMap的引用
	*/
	const HashMap<WORD, ITable*>&	GetAllTableHashMap(){ return m_TableHashMap; }

	NiuNiuRoom(const NiuNiuRoom&) = delete;
	NiuNiuRoom& operator=(const NiuNiuRoom&) = delete;
private:
	/**
	*  功能描述:		初始化函数 在构造函数中调用一次
	*
	*  @return void		无返回值
	*/
	void							__Init();
	/**
	*  功能描述:		销毁函数 在析构函数中调用一次
	*
	*  @return void		无返回值
	*/
	void							__Destory();
	/**
	*  功能描述:		清理房间内全部的桌子
	*
	*  @return void		无返回值
	*/
	void							__ClearAllTable();
private:
	RoomType				m_eRoomType;//房间的类型枚举
	BYTE					m_bRoomID;//房间ID
	WORD					m_wNowUseTableID;//房间的可用的桌子ID
	HashMap<WORD, ITable*>	m_TableHashMap;//桌子的HashMap
	NiuNiuHandleCommon*		m_pIHandleCommon;//消息处理接口
};

class NiuNiuTable : public ITable, public ITimer<NiuNiuTable> //牛牛桌子 核心功能区域
{
public:
	/**
	*  功能描述:			构造函数 explicit修饰
	*  @param bRoomID		房间ID
	*  @param wTableID		桌子ID
	*  @param pHandleCommon	消息处理类的指针 可能为nullptr
	*
	*/
	explicit NiuNiuTable(BYTE bRoomID, WORD wTableID, NiuNiuHandleCommon* pHandleCommon);
	~NiuNiuTable();

	/**
	*  功能描述:		获取当前桌子的房间ID
	*
	*  @return BYTE		返回桌子的房间ID
	*/
	virtual BYTE						GetRoomID() const { return m_bRoomID; }
	/**
	*  功能描述:		获取当前桌子的桌子ID
	*
	*  @return WORD		返回桌子的桌子ID
	*/
	virtual WORD						GetTableID() const { return m_wTableID; }
	/**
	*  功能描述:		根据玩家ID 创建并且添加一个桌子玩家
	*  @param dwUserID  玩家ID
	*
	*  @return ITableRole*	创建的桌子玩家的指针 有可能为 nullptr
	*/
	virtual ITableRole*					CreateTableRole(DWORD dwUserID);
	/**
	*  功能描述:		根据玩家ID 查询桌子上的玩家指针
	*  @param dwUserID  玩家ID
	*
	*  @return ITableRole*	查询的桌子玩家的指针 有可能为 nullptr
	*/
	virtual ITableRole*					FindTableRole(DWORD dwUserID) const;
	/**
	*  功能描述:		根据玩家ID 删除指定桌子上的玩家
	*  @param dwUserID  玩家的ID
	*
	*  @return void		无返回参数
	*/
	virtual void						DestoryTableRole(DWORD dwUserID);
	/**
	*  功能描述: 更新函数 用于更新桌子内部数据
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return  void		无返回参数
	*/
	virtual void						Update(DWORD dwTimer);
	/**
	*  功能描述:		获取桌子上玩家的人数
	*
	*  @return BYTE		桌子上玩家人数
	*/
	virtual BYTE						GetTableRoleSum() const { return m_TableRoleHashMap.size(); }
	/**
	*  功能描述: 获取桌子玩家的状态  每个位置是否做人 的状态
	*
	*  @return  DWORD		状态值
	*/
	DWORD								GetTableInfoState() const;
	/**
	*  功能描述: 获取桌子上全部的玩家
	*
	*  @return  HashMap		全部玩家的HashMap
	*/
	const HashMap<DWORD, ITableRole*>&	GetAllRole() const { return m_TableRoleHashMap; }
	/**
	*  功能描述: 获取桌子当前的状态
	*
	*  @return  FiveNiuNiuTableStates		状态值
	*/
	FiveNiuNiuTableStates				GetTableState() const { return m_eTableState; }
	/**
	*  功能描述: 获取桌子当前的状态的结束时间
	*
	*  @return  DWORD		结束时间
	*/
	DWORD								GetTableStateTimeLog() const { return m_dwTableStateTimeLog; }

	NiuNiuTable(const NiuNiuTable&) = delete;
	NiuNiuTable& operator=(const NiuNiuTable&) = delete;
private:
	/**
	*  功能描述:		初始化函数 在构造函数中调用一次
	*
	*  @return void		无返回值
	*/
	void								__Init();
	/**
	*  功能描述:		销毁函数 在析构函数中调用一次
	*
	*  @return void		无返回值
	*/
	void								__Destory();
	/**
	*  功能描述:		清理桌子上全部的玩家
	*
	*  @return void		无返回值
	*/
	void								__ClearAllTableRole();
	/**
	*  功能描述:		更新桌子的状态
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return void		无返回值
	*/
	void								__UpdateTableState(DWORD dwTimer);
	/**
	*  功能描述:		桌子是否可以进入开始状态
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return bool		可以进入为true  不可以进入为false
	*/
	bool								__IsCanJoinBegin(DWORD dwTimer) const;
	/**
	*  功能描述:		桌子是否可以进入等待开始状态
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return bool		可以进入为true  不可以进入为false
	*/
	bool								__IsCanJoinWriteBegin(DWORD dwTimer) const;
	/**
	*  功能描述:		桌子是否可以进入结束状态
	*  @param	dwTimer		当前时间 由timeGetTime()产生
	*
	*  @return bool		可以进入为true  不可以进入为false
	*/
	bool								__IsCanJoinEnd(DWORD dwTimer) const;
	/**
	*  功能描述:		改变桌子的当前状态 并且设置这个状态的结束时间(等待开始状态无结束时间)
	*  @param	eTableState		当前状态
	*
	*  @return void		无返回值
	*/
	void								__SetTableState(FiveNiuNiuTableStates eTableState);
	/**
	*  功能描述:		桌子进入开始状态
	*
	*  @return void		无返回值
	*/
	void								__OnJoinBegin();
	/**
	*  功能描述:		桌子进入等待开始状态
	*
	*  @return void		无返回值
	*/
	void								__OnJoinWriteBegin();
	/**
	*  功能描述:		桌子进入结束状态
	*
	*  @return void		无返回值
	*/
	void								__OnJoinEnd();
	/**
	*  功能描述:		获取桌子上的一个空的位置 如果没有 返回0xff
	*
	*  @return BYTE		返回空的位置 可能返回0xff 表示桌子满了
	*/
	BYTE								__GetNullSeat() const;

	bool								__TableIsAllRoleShowBrand() const;
	void								__UpdateTableRobot(DWORD dwTimer);
private:
	BYTE							m_bRoomID;//桌子所属的房间ID
	WORD							m_wTableID;//桌子ID
	HashMap<DWORD, ITableRole*>		m_TableRoleHashMap;//桌子玩家 按玩家ID为Key的HashMap
	vector<ITableRole*>				m_TableRoleVec;//按位置排序玩家
	//具体数据 状态模式
	FiveNiuNiuTableStates			m_eTableState; //桌子当前状态
	DWORD							m_dwTableStateTimeLog;//桌子当前状态结束时间
	//处理模板
	NiuNiuHandleCommon*				m_pIHandleCommon;//消息逻辑处理模块
	ITableRobotManager*				m_pTableRobotManager;
};

//基本机器人管理事件
struct tagNiuNiuTableRobotEvent
{
	DWORD		dwUserID;
	BYTE		bEventID;// 1 ->添加金币  2->翻牌 
	DWORD		dwTimer;//触发时间
};

class NiuNiuTableRobotManager : public ITableRobotManager
{
public:
	explicit NiuNiuTableRobotManager(ITable* pTable, IHandleCommon* pHandleCommon);
	virtual ~NiuNiuTableRobotManager();

	virtual void OnCreateTableEvent();
	virtual void OnRoleJoinTableEvent(DWORD dwUserID);
	virtual void OnRoleLeaveTableEvent(DWORD dwUserID);
	virtual void OnTableDestoryEvent();

	virtual void OnBeginTableStateChange(ITable* pTable);
	virtual void OnEndTableStateChange(ITable* pTable);

	virtual void Update(DWORD dwTimer);
private:
	void __AddNewRobot();
	void __DelRobot(DWORD dwUserID);

	void __TableBeginJoinBegin(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinBegin(NiuNiuTable* pNiuNiuTable);

	void __TableBeginJoinWriteBegin(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinWriteBegin(NiuNiuTable* pNiuNiuTable);

	void __TableBeginJoinEnd(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinEnd(NiuNiuTable* pNiuNiuTable);

	void __HandleEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent);
	void __HandleJoinGlobelEvent(DWORD dwTimer,const tagNiuNiuTableRobotEvent& rEvent);
	void __HandleShowBrandEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent);
private:
	NiuNiuTable*							m_pNiuNiuTable;
	NiuNiuHandleCommon*						m_NiuNiuHandleCommon;
	std::vector<tagNiuNiuTableRobotEvent>	m_RobotEventVec;
};

class NiuNiuTableRole : public ITableRole
{
public:
	/**
	*  功能描述:			构造函数 explicit修饰
	*  @param bSeatID		座位ID 从0开始
	*  @param dwUserID		玩家ID
	*
	*/
	explicit NiuNiuTableRole(BYTE bSeatID,DWORD dwUserID);
	~NiuNiuTableRole();
	/**
	*  功能描述:		获取玩家的ID
	*
	*  @return DWORD	玩家ID
	*/
	virtual DWORD	GetRoleID() const { return m_dwUserID; }
	/**
	*  功能描述:		获取玩家的外部对象指针
	*
	*  @return Role*	玩家指针 有可能为 nullptr
	*/
	virtual	Role*	GetRole() const { return m_pRole; }
	/**
	*  功能描述:		获取玩家的位置索引(从0开始)
	*
	*  @return BYTE		玩家的位置索引(从0开始)
	*/
	virtual BYTE	GetSeatID() const { return m_bSeatID; }
	/**
	*  功能描述:		获取玩家的牌的数据
	*  @param rBrandArray	牌的引用数组指针
	*
	*  @return void		无返回值
	*/
	const void		GetBrandArray(BYTE* rBrandArray) const{
		for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
			rBrandArray[i] = m_bArrTableBrandArray[i];
	}
	/**
	*  功能描述:			设置玩家牌的数据  长度为 MAX_NIUNIU_BrandSum
	*  @param dwUserID		牌的数组 长度为 MAX_NIUNIU_BrandSum
	*
	*  @return void			无返回值
	*/
	void			SetBrandArray(BYTE* parrBrandArray);
	/**
	*  功能描述:			清空玩家牌的数据
	*
	*  @return void			无返回值
	*/
	void			ClearBrandArray(){ ZeroMemory(&m_bArrTableBrandArray, sizeof(BYTE)*MAX_NIUNIU_BrandSum); m_wBrandValue = 0; m_bIsShowBrand = false; }
	/**
	*  功能描述:			获取玩家牌的统计权重值
	*
	*  @return WORD			牌的权重值 (用于比较牌的大小)
	*/
	WORD			GetBrandValue() const { return m_wBrandValue; }
	/**
	*  功能描述:			设置玩家牌的权重值
	*  @param wBrandValue	牌的权重值
	*
	*  @return void			无返回值
	*/
	void			SetBrandValue(WORD wBrandValue){ m_wBrandValue = wBrandValue; }

	void			SetIsShowBrand(){ m_bIsShowBrand = true; }
	bool			GetIsShowBrand(){ return m_bIsShowBrand; }

	NiuNiuTableRole(const NiuNiuTableRole&) = delete;
	NiuNiuTableRole& operator=(const NiuNiuTableRole&) = delete;
private:
	/**
	*  功能描述:		初始化函数 在构造函数中调用一次
	*
	*  @return void		无返回值
	*/
	void			__Init();
	/**
	*  功能描述:		销毁函数 在析构函数中调用一次
	*
	*  @return void		无返回值
	*/
	void			__Destory();
private:
	BYTE			m_bSeatID;//玩家的座位ID 从0开始
	DWORD			m_dwUserID;//玩家ID
	BYTE			m_bArrTableBrandArray[MAX_NIUNIU_BrandSum];//桌子上玩家牌的数据
	WORD			m_wBrandValue;//玩家牌的权重值
	Role*			m_pRole;//玩家的指针
	bool			m_bIsShowBrand;
};

