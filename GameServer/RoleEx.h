#pragma once
#include "Stdafx.h"
#include "RoleGameData.h"
#include "RoleLauncherManager.h"
#include "RoleVip.h"
#include "RoleRate.h"
#include"FishLogic\FishCallbak.h"
class RoleManager;
enum CatchType;
class CRoleEx
{
public:

	CRoleEx();
	~CRoleEx();

	bool OnInit(tagRoleInfo* pUserInfo, tagRoleServerInfo* pRoleServerInfo, RoleManager* pManager,
		DWORD dwSocketID, time_t pTime, bool IsRobot, const tagRoleGameData& gameData);//玩家登陆成功的时候调用 加载数据库数据
	DWORD GetUserID() { return m_RoleInfo.dwUserID; }
	DWORD GetGameSocketID() { return m_dwGameSocketID; }
	void ChangeRoleSocketID(DWORD SocketID);
	DWORD GetFaceID() { return m_RoleInfo.dwFaceID; }
	tagRoleInfo& GetRoleInfo() { return m_RoleInfo; }
	const FishKillDataType& GetRoleKillFishData() { return m_RoleGameData.GetFishKillData(); }
	tagRoleServerInfo& GetRoleServerInfo() { return m_RoleServerInfo; }
	time_t GetLastOnLineTime() { return m_LastOnLineTime; }//获取玩家最后上线的时间 有可能为0 表示玩家第一次上线
	bool IsOnceDayOnline();//是否在同一天内重新登陆
	bool IsOnceMonthOnline();
	RoleGameData& GetRoleGameData() { return m_RoleGameData; }
	RoleLauncherManager& GetRoleLauncherManager() { return m_RoleLauncherManager; }
	RoleVip& GetRoleVip() { return m_RoleVip; }
	RoleRate& GetRoleRate() { return m_RoleRate; }
	void SendDataToClient(NetCmd* pCmd);
	void SendDataToCenter(NetCmd* pCmd);
	void SendDataToTable(NetCmd* pCmd);
	void SendUserLeaveToCenter();
	bool ChangeRoleGlobe(int64 AddGlobe, const BYTE TableTypeID, const USHORT FishType = -1, const BYTE bulletType = -1);
	bool OnHitFish(const USHORT fishType,const BYTE bulletType);
	bool ChangeRoleIsOnline(bool States);
	//RoleServerInfo
	bool ChangeRoleTotalFishGlobelSum(int64 AddSum);
	int64 GetScore();
	void OnSaveInfoToDB();
	void UpdateByMin(DWORD dwMin);
	//对RoleEx 进行 上下线 切换
	void SetRoleExLeaveServer();//设置RoleEx 离开了服务器
	bool IsAFK() { return m_IsAfk; }
	void SetAfkStates(bool States);
	bool IsRobot() { return m_IsRobot; }
	bool IsExit() { return m_IsExit; }
	void SetIsExit(bool States);
	bool SaveAllRoleInfo(bool IsExit);
	void SetRoleIsNeedSave() { m_IsNeedSave = true; }//修改玩家 是需要保存的
private:
	RoleManager*				m_RoleManager;
	tagRoleInfo					m_RoleInfo;//玩家的数据 从数据库里读取的 做一些特殊的操作 部分数据需要发送到客户端去的
	tagRoleServerInfo			m_RoleServerInfo;
	bool					    m_IsNeedSave;
	//玩家的数据处理
	DWORD						m_dwGameSocketID;//玩家在Game和Socket上对应的ID		
	time_t						m_LastOnLineTime;//最后次上线的时间记录
	time_t						m_LogonTime;//当前登陆的时间
	time_t						m_LogonTimeByDay;//每天的开始时间
	//玩家的管理器
	RoleGameData				m_RoleGameData;
	RoleLauncherManager			m_RoleLauncherManager;
	RoleVip						m_RoleVip;
	RoleRate					m_RoleRate;
	bool						m_IsAfk;
	//登陆相关的缓存数据
	bool						m_IsRobot;
	bool						m_IsExit;//玩家是否正在退出
	bool						m_IsOnline;
};