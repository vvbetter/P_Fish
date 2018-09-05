/*
*@file   FiveNiuNiu.cpp
*@brief  通比牛牛实现类
*@author 徐达
*@data   2016-6-14 18:00
*/
#include "Stdafx.h"
#include "FiveNiuNiu.h"
#include "FishServer.h"
//#define IsOnceBrandValue(a,b,c,d) a == b && a==c && a==d
//通比牛牛的处理类 (后续可以扩展的)
//核心消息处理
NiuNiuHandleCommon::NiuNiuHandleCommon(RoomType eRoomType, IRoomManager* pManager, IRoomRoleManager* pRoleManager)
	:IHandleCommon(eRoomType, pManager, pRoleManager), m_eRoomType(eRoomType), m_pRoomManager(pManager), m_pRoomRoleManager(pRoleManager)
{
	__Init();
}
NiuNiuHandleCommon::~NiuNiuHandleCommon()
{
	__Destory();
}
void NiuNiuHandleCommon::__Init()
{
	if (nullptr == m_pRoomManager || nullptr == m_pRoomRoleManager)
		ASSERT(false);
}
void NiuNiuHandleCommon::__Destory()
{
	m_pRoomManager = nullptr;
	m_pRoomRoleManager = nullptr;
	delete m_pRoomRoleManager;
	m_pRoomRoleManager = nullptr;
}
NiuNiuRoom* NiuNiuHandleCommon::__FindRoom(BYTE bRoomID) const
{
	IRoom* pRoom = m_pRoomManager->FindRoom(bRoomID);
	NiuNiuRoom* pNiuNiuRoom = dynamic_cast<NiuNiuRoom*>(pRoom);
	return pNiuNiuRoom;
}
NiuNiuTable* NiuNiuHandleCommon::__FindTable(const NiuNiuRoom* pNiuNiuRoom, WORD wTableID) const
{
	if (nullptr == pNiuNiuRoom)
		return nullptr;
	ITable* pTable = pNiuNiuRoom->FindTable(wTableID);
	NiuNiuTable* pNiuNiuTable = dynamic_cast<NiuNiuTable*>(pTable);
	return pNiuNiuTable;
}
NiuNiuTableRole* NiuNiuHandleCommon::__FindTableRole(const NiuNiuTable* pNiuNiuTable, DWORD dwUserID) const
{ 
	if (nullptr == pNiuNiuTable)
		return nullptr;
	ITableRole* pTableRole = pNiuNiuTable->FindTableRole(dwUserID);
	NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(pTableRole);
	return pNiuNiuTableRole;
}
bool NiuNiuHandleCommon::HandleMsg(NetCmd* pCmd)
{
	if (nullptr == pCmd)
		return false;
	if (Main_FiveNiuNiu != pCmd->GetMainType())
		return false;
	//具体的命令处理流程
	switch (pCmd->GetSubType())
	{
	case GM_FShowTableInfo:
		__HandleShowRoomTableInfo((GM_Cmd_FShowTableInfo*)pCmd);
		break;
	case GM_FJoinRoomTable:
		__HandleJoinRoomTable((GM_Cmd_FJoinRoomTable*)pCmd);
		break;
	case GM_FRoleLeaveTable:
		__HandleLeaveTable((GM_Cmd_FRoleLeaveTable*)pCmd);
		break;
	case GM_FRoleLeaveRoom:
		__HandleLeaveRoom((GM_Cmd_FRoleLeaveRoom*)pCmd);
		break;
	case GM_FRoleInfoChange:
		__HandleRoleInfoChange((GM_Cmd_FRoleInfoChange*)pCmd);
		break;
	case GM_ShowBrandInfo:
		__HandleShowBrandInfo((GM_Cmd_ShowBrandInfo*)pCmd);
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}
const tagNiuNiuRoomConfig* NiuNiuHandleCommon::GetNiuNiuRoomConfig(BYTE bRoomID) const
{
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator Iter = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.find(bRoomID);
	if (Iter == g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.end())
		return nullptr;
	else
		return &Iter->second;
}
const tagFiveNiuNiuRate* NiuNiuHandleCommon::GetNiuNiuRateConfig(BYTE bRoomID) const
{
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(bRoomID);
	if (nullptr == pNiuNiuRoomConfig)
		return nullptr;
	HashMap<BYTE, tagFiveNiuNiuRate>::const_iterator IterRate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRateMap.find(pNiuNiuRoomConfig->bRateID);
	if (g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRateMap.end() == IterRate)
		return nullptr;
	else
		return &IterRate->second;
}
void NiuNiuHandleCommon::__SendShowRoomTableInfoFailed(Role* pRole) const
{
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FShowTableInfo msgFailde;
	SetMsgInfo(msgFailde, GetMsgType(Main_FiveNiuNiu, MG_FShowTableInfo), sizeof(MG_Cmd_FShowTableInfo));
	msgFailde.dwUserID = pRole->GetRoleID();
	msgFailde.bResult = false;
	msgFailde.States = MsgBegin | MsgEnd;
	msgFailde.Sum = 0;
	pRole->SendDataToGameServer(&msgFailde);
}
void NiuNiuHandleCommon::__SendShowRoomTableInfoSuccess(Role* pRole, NiuNiuRoom* pNiuNiuRoom) const
{
	if (nullptr == pRole || nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	const HashMap<WORD, ITable*>& rTableHashMap = pNiuNiuRoom->GetAllTableHashMap();
	DWORD PageSize = sizeof(MG_Cmd_FShowTableInfo) + sizeof(tagFiveNiuNiuTableInfo) * (rTableHashMap.size() - 1);
	MG_Cmd_FShowTableInfo* msg = (MG_Cmd_FShowTableInfo*)malloc(PageSize);
	if (nullptr == msg)
	{
		ASSERT(false);
		__SendShowRoomTableInfoFailed(pRole);
		return;
	}
	msg->SetCmdType(GetMsgType(Main_FiveNiuNiu, MG_FShowTableInfo));
	msg->bResult = true;
	msg->dwUserID = pRole->GetRoleID();
	msg->Sum = rTableHashMap.size();
	HashMap<WORD, ITable*>::const_iterator CIter = rTableHashMap.cbegin();
	for (WORD i = 0; CIter != rTableHashMap.cend() && i < msg->Sum;)
	{
		NiuNiuTable* pNiuNiuTable = dynamic_cast<NiuNiuTable*>(CIter->second);
		if (nullptr == pNiuNiuTable)
		{
			++CIter;
		}
		msg->Array[i].TableID = pNiuNiuTable->GetTableID();
		msg->Array[i].TableStates = pNiuNiuTable->GetTableRoleSum();//pNiuNiuTable->GetTableInfoState();
		++CIter;
		++i;
	}
	//对命令进行切割
	std::vector<MG_Cmd_FShowTableInfo*> pVec;
	SqlitMsg(msg, PageSize, msg->Sum, true, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<MG_Cmd_FShowTableInfo*>::iterator IterVec = pVec.begin();
		for (; IterVec != pVec.end(); ++IterVec)
		{
			pRole->SendDataToGameServer(*IterVec);
			free(*IterVec);
		}
		pVec.clear();
	}
}
void NiuNiuHandleCommon::__HandleShowRoomTableInfo(GM_Cmd_FShowTableInfo* pMsg)
{
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(pMsg->dwUserID);
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	SetRoleLeaveRoom(pMsg->dwUserID);//玩家离开房间了
	//1.判断玩家是否可以进入房间 金币数量
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(pMsg->bRoomID);
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(pMsg->bRoomID);
	if (nullptr == pNiuNiuRoomConfig || nullptr == pNiuNiuRoom || pRole->GetGlobelSum() < pNiuNiuRoomConfig->dwJoinRoomGlobel)
	{
		__SendShowRoomTableInfoFailed(pRole);
		return;
	}
	SetRoleJoinRoom(pMsg->dwUserID, pMsg->bRoomID);//指定玩家进入房间了
	//发送当前房间全部桌子的命令
	__SendShowRoomTableInfoSuccess(pRole, pNiuNiuRoom);
}
void NiuNiuHandleCommon::__SendJoinTableFailed(Role* pRole) const
{
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FJoinRoomTable msgFailed;
	SetMsgInfo(msgFailed, GetMsgType(Main_FiveNiuNiu, MG_FJoinRoomTable), sizeof(MG_Cmd_FJoinRoomTable));
	msgFailed.dwUserID = pRole->GetRoleID();
	msgFailed.bResult = false;
	msgFailed.Sum = 0;
	msgFailed.States = MsgBegin | MsgEnd;
	msgFailed.bTableStates = 0;
	msgFailed.dwTableStatesLogTime = 0;
	pRole->SendDataToGameServer(&msgFailed);
}
void NiuNiuHandleCommon::__SendTableOtherRoleInfoToMe(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const
{
	if (nullptr == pNiuNiuTableRole || nullptr == pNiuNiuTableRole->GetRole() || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	//将桌子上其他玩家的数据同步给我
	DWORD PageSize = sizeof(MG_Cmd_FJoinRoomTable) + sizeof(tagFiveNiuNiuTableRoleInfo)*(pNiuNiuTable->GetTableRoleSum() - 2);//不包括自己
	MG_Cmd_FJoinRoomTable* msg = (MG_Cmd_FJoinRoomTable*)malloc(PageSize);
	if (nullptr == msg)
	{
		ASSERT(false);
		__SendJoinTableFailed(pNiuNiuTableRole->GetRole());
		return;
	}
	msg->SetCmdType(GetMsgType(Main_FiveNiuNiu, MG_FJoinRoomTable));
	msg->bResult = true;
	msg->dwUserID = pNiuNiuTableRole->GetRoleID();
	msg->bTableStates = static_cast<BYTE>(pNiuNiuTable->GetTableState());
	msg->dwTableStatesLogTime = pNiuNiuTable->GetTableStateTimeLog() - timeGetTime();
	msg->Sum = pNiuNiuTable->GetTableRoleSum() - 1;
	msg->wTableID = pNiuNiuTable->GetTableID();
	msg->bSeatID = pNiuNiuTableRole->GetSeatID();
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (BYTE i = 0; CIter != rRoleHashMap.cend() && i< msg->Sum;)
	{
		if (CIter->second->GetRoleID() == pNiuNiuTableRole->GetRoleID())
		{
			++CIter;
			continue;
		}
		Role* pRole = CIter->second->GetRole();
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(CIter->second);
		if (nullptr == pRole || nullptr == pNiuNiuTableRole)
		{
			++CIter;
			continue;
		}
		msg->Array[i].dwUserID = pRole->GetRoleID();
		msg->Array[i].dwFaceID = pRole->GetFaceID();
		msg->Array[i].dwGlobelSum = pRole->GetGlobelSum();
		msg->Array[i].SeatID = CIter->second->GetSeatID();
		pNiuNiuTableRole->GetBrandArray(msg->Array[i].bArrTableBrandArray);
		msg->Array[i].wBrandValue = pNiuNiuTableRole->GetBrandValue();
		TCHARCopy(msg->Array[i].chArrNickName, CountArray(msg->Array[i].chArrNickName), pRole->GetNickName(), _tcslen(pRole->GetNickName()));
		++i;
		++CIter;
	}
	std::vector<MG_Cmd_FJoinRoomTable*> pVec;
	SqlitMsg(msg, PageSize, msg->Sum, true, pVec);
	free(msg);
	if (!pVec.empty())
	{
		std::vector<MG_Cmd_FJoinRoomTable*>::iterator IterVec = pVec.begin();
		for (; IterVec != pVec.end(); ++IterVec)
		{
			pNiuNiuTableRole->GetRole()->SendDataToGameServer(*IterVec);
			free(*IterVec);
		}
		pVec.clear();
	}
}
void NiuNiuHandleCommon::__SendMeRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const
{
	//将我的数据发送给桌子上的其他玩家
	if (nullptr == pNiuNiuTableRole || nullptr == pNiuNiuTableRole->GetRole() || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	if (pNiuNiuTable->GetTableRoleSum() <= 1)
		return;
	MG_Cmd_FNewRoleJoinTable msgNewRole;
	SetMsgInfo(msgNewRole, GetMsgType(Main_FiveNiuNiu, MG_FNewRoleJoinTable), sizeof(MG_Cmd_FNewRoleJoinTable));
	msgNewRole.NewTableRoleInfo.dwUserID = pNiuNiuTableRole->GetRole()->GetRoleID();
	msgNewRole.NewTableRoleInfo.dwFaceID = pNiuNiuTableRole->GetRole()->GetFaceID();
	msgNewRole.NewTableRoleInfo.dwGlobelSum = pNiuNiuTableRole->GetRole()->GetGlobelSum();
	msgNewRole.NewTableRoleInfo.SeatID = pNiuNiuTableRole->GetSeatID();
	TCHARCopy(msgNewRole.NewTableRoleInfo.chArrNickName, CountArray(msgNewRole.NewTableRoleInfo.chArrNickName), pNiuNiuTableRole->GetRole()->GetNickName(), _tcslen(pNiuNiuTableRole->GetRole()->GetNickName()));
	for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
		msgNewRole.NewTableRoleInfo.bArrTableBrandArray[j] = 0;
	msgNewRole.NewTableRoleInfo.wBrandValue = pNiuNiuTableRole->GetBrandValue();
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (; CIter != rRoleHashMap.cend(); ++CIter)
	{
		if (CIter->first == pNiuNiuTableRole->GetRoleID())
			continue;
		msgNewRole.dwUserID = CIter->second->GetRoleID();
		if (nullptr != CIter->second->GetRole())
			CIter->second->GetRole()->SendDataToGameServer(&msgNewRole);
	}
}
void NiuNiuHandleCommon::__HandleJoinRoomTable(GM_Cmd_FJoinRoomTable* pMsg)
{
	//处理玩家进入桌子的命令
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(pMsg->dwUserID);
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	BYTE bRoomID = QueryRoleRoomID(pMsg->dwUserID);
	WORD wTableID = QueryRoleTableID(pMsg->dwUserID);
	if (wTableID != 0) //在桌子里面不可以重复进入桌子
	{
		__SendJoinTableFailed(pRole);
		return;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(bRoomID);
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, pMsg->wTableID);
	if (nullptr == pNiuNiuRoomConfig || nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable ||  pRole->GetGlobelSum() < pNiuNiuRoomConfig->dwJoinRoomGlobel
		|| pNiuNiuTable->GetTableRoleSum() >= pNiuNiuRoomConfig->bTableMaxRoleSum)
	{
		__SendJoinTableFailed(pRole);
		return;
	}
	SetRoleJoinTable(pMsg->dwUserID, pMsg->wTableID);//玩家进入桌子

	//设置玩家进入桌子
	ITableRole* pTableRole = pNiuNiuTable->CreateTableRole(pMsg->dwUserID); //让玩家进入到桌子里面去 但不涉及命令的同步
	NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(pTableRole);
	if (nullptr == pNiuNiuTableRole)
	{
		ASSERT(false);
		__SendJoinTableFailed(pRole);
		return;
	}
	//1.将其他玩家的数据同步给自己
	__SendTableOtherRoleInfoToMe(pNiuNiuTableRole, pNiuNiuTable);
	//2.将自己的数据同步给其他玩家
	__SendMeRoleInfoToTableOtherRole(pNiuNiuTableRole, pNiuNiuTable);
	//3.将变化数据发送出去
	__SendTableInfoToClient(pNiuNiuRoom, pNiuNiuTable);//将桌子最新的数据发送出去
	__CheckIsNeedCreateTable(pNiuNiuRoom);//检测是否需要创建桌子
}
void NiuNiuHandleCommon::__SendRoleLeaveInfoToTableOtherRole(NiuNiuTable* pNiuNiuTable ,DWORD dwUserID, BYTE bSeatID) const
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FRoleLeaveTable msgLeave;
	SetMsgInfo(msgLeave, GetMsgType(Main_FiveNiuNiu, MG_FRoleLeaveTable), sizeof(MG_Cmd_FRoleLeaveTable));
	msgLeave.bSeatID = bSeatID;
	msgLeave.dwLeaveUserID = dwUserID;
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.begin();
	for (; CIter != rRoleHashMap.end(); ++CIter)
	{
		msgLeave.dwUserID = CIter->second->GetRoleID();
		if (nullptr != CIter->second->GetRole())
			CIter->second->GetRole()->SendDataToGameServer(&msgLeave);
	}
}
void NiuNiuHandleCommon::__HandleLeaveTable(GM_Cmd_FRoleLeaveTable* pMsg)
{
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	//指定玩家离开桌子(只是离开桌子 不是离开房间)
	BYTE bRoomID= QueryRoleRoomID(pMsg->dwUserID);
	WORD wTableID = QueryRoleTableID(pMsg->dwUserID);
	if (bRoomID == 0 || wTableID == 0)
	{
		ASSERT(false);
		return;
	}
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	NiuNiuTableRole* pTableRole = __FindTableRole(pNiuNiuTable, pMsg->dwUserID);
	if (nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable || nullptr == pTableRole)
	{
		ASSERT(false);
		return;
	}
	//1.让玩家离开桌子 将玩家从集合里面删除掉
	BYTE bSeatID = pTableRole->GetSeatID();
	pNiuNiuTable->DestoryTableRole(pMsg->dwUserID);

	__SendTableInfoToClient(pNiuNiuRoom, pNiuNiuTable);//将桌子最新的数据发送出去

	SetRoleLeaveTable(pMsg->dwUserID);//玩家离开桌子
	pTableRole = nullptr;//因为玩家已经删除了 
	__SendRoleLeaveInfoToTableOtherRole(pNiuNiuTable, pMsg->dwUserID, bSeatID);

	//3.因为玩家离开房间了 我们将最新的桌子数据发送到客户端去
	GM_Cmd_FShowTableInfo msg;
	msg.bRoomID = bRoomID;
	msg.dwUserID = pMsg->dwUserID;
	__HandleShowRoomTableInfo(&msg);
}
void NiuNiuHandleCommon::__HandleLeaveRoom(GM_Cmd_FRoleLeaveRoom* pMsg)
{
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	//玩家离开房间 如果玩家在桌子里 就让玩家离开桌子
	BYTE bRoomID = QueryRoleRoomID(pMsg->dwUserID);
	if (bRoomID == 0)
	{
		//ASSERT(false);
		return;
	}
	NiuNiuRoom* pNiuNiuRoom = dynamic_cast<NiuNiuRoom*>(m_pRoomManager->FindRoom(bRoomID));
	if (nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	//1.先判断玩家是否在桌子了 如果在桌子了 让玩家离开桌子 并且通知其他玩家
	WORD wTableID = QueryRoleTableID(pMsg->dwUserID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	NiuNiuTableRole* pTableRole = __FindTableRole(pNiuNiuTable, pMsg->dwUserID);
	if (nullptr != pNiuNiuTable && nullptr != pTableRole)
	{
		BYTE bSeatID = pTableRole->GetSeatID();
		pNiuNiuTable->DestoryTableRole(pMsg->dwUserID);
		__SendRoleLeaveInfoToTableOtherRole(pNiuNiuTable, pMsg->dwUserID, bSeatID);

		__SendTableInfoToClient(pNiuNiuRoom, pNiuNiuTable);//将桌子最新的数据发送出去
	}
	//2.玩家不在桌子后 我们直接让玩家离开房间
	SetRoleLeaveRoom(pMsg->dwUserID);
}
void NiuNiuHandleCommon::__SendNewRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const
{
	if (nullptr == pNiuNiuTableRole || nullptr == pNiuNiuTableRole->GetRole() || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FRoleInfoChange msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_FRoleInfoChange), sizeof(MG_Cmd_FRoleInfoChange));
	msg.RoleInfo.dwUserID = pNiuNiuTableRole->GetRoleID();
	msg.RoleInfo.dwFaceID = pNiuNiuTableRole->GetRole()->GetFaceID();
	msg.RoleInfo.dwGlobelSum = pNiuNiuTableRole->GetRole()->GetGlobelSum();
	msg.RoleInfo.SeatID = pNiuNiuTableRole->GetSeatID();
	TCHARCopy(msg.RoleInfo.chArrNickName, CountArray(msg.RoleInfo.chArrNickName), pNiuNiuTableRole->GetRole()->GetNickName(), _tcslen(pNiuNiuTableRole->GetRole()->GetNickName()));
	pNiuNiuTableRole->GetBrandArray(msg.RoleInfo.bArrTableBrandArray);
	msg.RoleInfo.wBrandValue = pNiuNiuTableRole->GetBrandValue();
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (; CIter != rRoleHashMap.cend(); ++CIter)
	{
		msg.dwUserID = CIter->second->GetRoleID();
		if (nullptr != CIter->second->GetRole())
			CIter->second->GetRole()->SendDataToGameServer(&msg);
	}
}
void NiuNiuHandleCommon::__HandleRoleInfoChange(GM_Cmd_FRoleInfoChange* pMsg)
{
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	BYTE bRoomID = QueryRoleRoomID(pMsg->dwUserID);
	WORD wTableID = QueryRoleTableID(pMsg->dwUserID);
	if (bRoomID == 0 || wTableID == 0)
	{
		ASSERT(false);
		return;
	}
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	NiuNiuTableRole* pTableRole = __FindTableRole(pNiuNiuTable, pMsg->dwUserID);
	if (nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable || nullptr == pTableRole || nullptr == pTableRole->GetRole())
	{
		ASSERT(false);
		return;
	}
	//查询到后 我们更新玩家的数据 然后发送命令到客户端去
	__SendNewRoleInfoToTableOtherRole(pTableRole, pNiuNiuTable);
}
void NiuNiuHandleCommon::__HandleShowBrandInfo(GM_Cmd_ShowBrandInfo* pMsg)
{
	//玩家摊牌
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	BYTE bRoomID = QueryRoleRoomID(pMsg->dwUserID);
	WORD wTableID = QueryRoleTableID(pMsg->dwUserID);
	if (bRoomID == 0 || wTableID == 0)
	{
		ASSERT(false);
		return;
	}
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	NiuNiuTableRole* pTableRole = __FindTableRole(pNiuNiuTable, pMsg->dwUserID);
	if (nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable || nullptr == pTableRole || nullptr == pTableRole->GetRole())
	{
		ASSERT(false);
		return;
	}
	__SetRoleBrandValue(pNiuNiuTable, pTableRole, pMsg->wBrandValue,pMsg->bChangeIndex);
}
void NiuNiuHandleCommon::__SetRoleBrandValue(NiuNiuTable* pNiuNiuTable, NiuNiuTableRole* pNiuNiuTableRole, WORD wBrandValue, BYTE bChangeIndex)
{
	if (nullptr == pNiuNiuTable || nullptr == pNiuNiuTableRole || 0 == wBrandValue)
	{
		ASSERT(false);
		return;
	}
	//判断玩家的牌是否可以如此组合
	BYTE bArrayBrand[MAX_NIUNIU_BrandSum] = {0};
	pNiuNiuTableRole->GetBrandArray(bArrayBrand);

	//改变牌的位置
	if (bChangeIndex != 0)
	{
		BYTE bBeChangeIndex = 0;
		for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
		{
			if (((1 << i) & bChangeIndex) == 0)
				continue;
			__ChangeValue(bArrayBrand[i], bArrayBrand[bBeChangeIndex]);
			++bBeChangeIndex;
		}
		pNiuNiuTableRole->SetBrandArray(bArrayBrand);
	}

	WORD wMaxBrandValue = HandleBrandValue(bArrayBrand);
	if (wBrandValue > wMaxBrandValue)
	{
		MG_Cmd_ShowBrandInfoFailed msg;
		SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_ShowBrandInfoFailed), sizeof(MG_Cmd_ShowBrandInfoFailed));
		msg.dwUserID = pNiuNiuTableRole->GetRoleID();
		pNiuNiuTableRole->GetRole()->SendDataToGameServer(&msg);
	}
	else
	{
		pNiuNiuTableRole->SetBrandValue(wBrandValue);//设置玩家牌的数据
		pNiuNiuTableRole->SetIsShowBrand();
		MG_Cmd_ShowBrandInfo msg;
		SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_ShowBrandInfo), sizeof(MG_Cmd_ShowBrandInfo));
		msg.dwDestUserID = pNiuNiuTableRole->GetRoleID();
		msg.wBrandValue = wBrandValue;
		pNiuNiuTableRole->GetBrandArray(msg.bArrTableBrandArray);
		const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
		HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
		for (; IterRole != rRoleHashMap.cend(); ++IterRole)
		{
			msg.dwUserID = IterRole->first;
			IterRole->second->GetRole()->SendDataToGameServer(&msg);
		}
	}
}
void NiuNiuHandleCommon::__SendNonShowBrandRole(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
		return;
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(IterRole->second);
		if (nullptr == pNiuNiuTableRole || pNiuNiuTableRole->GetIsShowBrand())
			continue;
		__SetRoleBrandValue(pNiuNiuTable, pNiuNiuTableRole, pNiuNiuTableRole->GetBrandValue(),0);
	}
}
void NiuNiuHandleCommon::__KickTableRoleByGlobel(NiuNiuTable* pNiuNiuTable, DWORD dwMinGlobel)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	std::vector<DWORD> DelRoleIDVec;
	//将金币过少的玩家全部剔除掉
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		Role* pRole = IterRole->second->GetRole();
		if (nullptr == pRole || pRole->GetGlobelSum() >= dwMinGlobel)
			continue;
		DelRoleIDVec.push_back(pRole->GetRoleID());
	}
	if (!DelRoleIDVec.empty())
	{
		GM_Cmd_FRoleLeaveTable msg;
		SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FRoleLeaveTable), sizeof(GM_Cmd_FRoleLeaveTable));
		std::vector<DWORD>::iterator Iter = DelRoleIDVec.begin();
		for (; Iter != DelRoleIDVec.end(); ++Iter)
		{
			msg.dwUserID = *Iter;
			__HandleLeaveTable(&msg);//不要直接删除玩家 让玩家离开桌子 并且还有其他操作
		}
		DelRoleIDVec.clear();
	}
}
void NiuNiuHandleCommon::__SetTableRoleBrandInfo(NiuNiuTable* pNiuNiuTable)
{
	//给桌子上全部的玩家发牌
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	NiuNiuOnceBrandList oBrand;//一副新的牌 
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (BYTE i = 0; IterRole != rRoleHashMap.cend(); ++IterRole, ++i)
	{
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(IterRole->second);
		if (nullptr == pNiuNiuTableRole)
			continue;
		pNiuNiuTableRole->ClearBrandArray();//清理掉
		BYTE arrbBrandArray[MAX_NIUNIU_BrandSum] = { 0 };
		BYTE bMaxBradnValue = 0;
		for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
		{
			arrbBrandArray[i] = oBrand.GetNewBrand();
			if (arrbBrandArray[i] > bMaxBradnValue)
				bMaxBradnValue = arrbBrandArray[i];
		}
		//WORD wBrandValue = __HandleBrandValue(arrbBrandArray);
		pNiuNiuTableRole->SetBrandArray(arrbBrandArray);
		pNiuNiuTableRole->SetBrandValue((static_cast<WORD>(BrandNiuNiuLevel::BNNL_Non) << 8) + bMaxBradnValue);//无牛
	}
}
DWORD NiuNiuHandleCommon::__GetTableMaxBrandValueUserID(NiuNiuTable* pNiuNiuTable) const
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return 0;
	}
	DWORD dwMaxBrandValueUserID = 0;
	WORD wBrandValue = 0;

	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(IterRole->second);
		if (nullptr == pNiuNiuTableRole || pNiuNiuTableRole->GetBrandValue()==0xffff)
			continue;
		if (pNiuNiuTableRole->GetBrandValue() <= wBrandValue)
			continue;
		dwMaxBrandValueUserID = pNiuNiuTableRole->GetRoleID();
		wBrandValue = pNiuNiuTableRole->GetBrandValue();
	}
	return dwMaxBrandValueUserID;
}
void NiuNiuHandleCommon::__SendTableRoleGameResult(NiuNiuTable* pNiuNiuTable, DWORD dwMaxBrandValueUserID, BYTE bRate)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(pNiuNiuTable->GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return;
	}
	__int64 i64TotalGlobel = 0;
	MG_Cmd_FTableJoinEnd msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_FTableJoinEnd), sizeof(MG_Cmd_FTableJoinEnd));

	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (BYTE i = 0; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(IterRole->second);
		if (nullptr == pNiuNiuTableRole || pNiuNiuTableRole->GetRoleID() == dwMaxBrandValueUserID)
			continue;
		__int64 i64ChangeGlobel = 0;
		if (0xffff != pNiuNiuTableRole->GetBrandValue())
		{
			i64ChangeGlobel = static_cast<__int64>((bRate - 1) * pNiuNiuRoomConfig->dwOnceBetGlobel)*-1;
			i64TotalGlobel += bRate* pNiuNiuRoomConfig->dwOnceBetGlobel;
		}
		msg.dwUserID = pNiuNiuTableRole->GetRoleID();
		msg.i64GlobelChange = i64ChangeGlobel;
		if (0 != i64ChangeGlobel)
			pNiuNiuTableRole->GetRole()->GetRoleInfo().dwGlobelSum += static_cast<int>(i64ChangeGlobel);
		pNiuNiuTableRole->GetRole()->SendDataToGameServer(&msg);
	}
	i64TotalGlobel += pNiuNiuRoomConfig->dwOnceBetGlobel;
	msg.dwUserID = dwMaxBrandValueUserID;
	msg.i64GlobelChange = i64TotalGlobel;
	NiuNiuTableRole* pNiuNiuTableRole = __FindTableRole(pNiuNiuTable, dwMaxBrandValueUserID);
	if (nullptr == pNiuNiuTableRole)
	{
		ASSERT(false);
		return;
	}
	pNiuNiuTableRole->GetRole()->GetRoleInfo().dwGlobelSum += static_cast<int>(i64TotalGlobel);
	pNiuNiuTableRole->GetRole()->SendDataToGameServer(&msg);
}
void NiuNiuHandleCommon::__LostAllTableRoleGlobel(NiuNiuTable* pNiuNiuTable, int nLostGlobel)
{
	//改变桌子上玩家的金币
	if (nullptr == pNiuNiuTable || 0 == nLostGlobel)
	{
		ASSERT(false);
		return;
	}
	const HashMap<DWORD, ITableRole*>& rTableRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rTableRoleHashMap.cbegin();
	for (; CIter != rTableRoleHashMap.cend(); ++CIter)
	{
		Role* pRole = CIter->second->GetRole();
		if (nullptr == pRole)
			continue;
		pRole->GetRoleInfo().dwGlobelSum += nLostGlobel;
	}
}
void NiuNiuHandleCommon::__SendTableJoinBeginInfo(NiuNiuTable* pNiuNiuTable) const
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(pNiuNiuTable->GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FTableJoinBegin msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_FTableJoinBegin), sizeof(MG_Cmd_FTableJoinBegin));
	msg.AddGlobelSum = pNiuNiuRoomConfig->dwOnceBetGlobel;
	//将桌子进入开始阶段的消息发送到客户端去
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (BYTE i = 0; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(IterRole->second);
		if (nullptr == pNiuNiuTableRole)
			continue;
		msg.dwUserID = pNiuNiuTableRole->GetRoleID();
		pNiuNiuTableRole->GetBrandArray(msg.bArrTableBrandArray);
		pNiuNiuTableRole->GetRole()->SendDataToGameServer(&msg);
	}
}
void NiuNiuHandleCommon::OnTableJoinBegin(BYTE bRoomID, WORD wTableID)//因为没有下注阶段 所有直接开启
{
	//桌子进入开始阶段 
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(bRoomID);
	const tagFiveNiuNiuRate* pNiuNiuRateConfig = GetNiuNiuRateConfig(bRoomID);
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	if (nullptr == pNiuNiuRoomConfig || nullptr == pNiuNiuRateConfig || nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	DWORD MinGlobel = max(pNiuNiuRoomConfig->dwTableMinGlobel, pNiuNiuRateConfig->MaxRate * pNiuNiuRoomConfig->dwOnceBetGlobel);
	__KickTableRoleByGlobel(pNiuNiuTable, MinGlobel);//剔除部分金币不够的玩家
	__LostAllTableRoleGlobel(pNiuNiuTable, static_cast<int>(pNiuNiuRoomConfig->dwOnceBetGlobel)*-1);//扣除下注的金币
	__SetTableRoleBrandInfo(pNiuNiuTable);//给桌子上全部玩家发牌d
	__SendTableJoinBeginInfo(pNiuNiuTable);//将状态发送到客户端去
}
void NiuNiuHandleCommon::OnTableJoinEnd(BYTE bRoomID, WORD wTableID)
{
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(bRoomID);
	const tagFiveNiuNiuRate* pNiuNiuRateConfig = GetNiuNiuRateConfig(bRoomID);
	NiuNiuRoom* pNiuNiuRoom = __FindRoom(bRoomID);
	NiuNiuTable* pNiuNiuTable = __FindTable(pNiuNiuRoom, wTableID);
	if (nullptr == pNiuNiuRoomConfig || nullptr == pNiuNiuRateConfig || nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	NiuNiuTableRole* pMaxBrandValueRole = __FindTableRole(pNiuNiuTable, __GetTableMaxBrandValueUserID(pNiuNiuTable));//获取牌最大的玩家
	if (nullptr == pMaxBrandValueRole)
	{
		ASSERT(false);
		return;
	}
	__SendNonShowBrandRole(pNiuNiuTable);
	BYTE bRate = __GetBrandRateValue(pMaxBrandValueRole);//获取本局的赔率
	__SendTableRoleGameResult(pNiuNiuTable, pMaxBrandValueRole->GetRoleID(), bRate);//根据牌的结果结算 并且发送到客户端去
}
WORD NiuNiuHandleCommon::HandleBrandValue(BYTE* BrandValue)
{
	if (MAX_NIUNIU_BrandSum != 5)
		return 0;
	//得到当前牌的值 
	// 牌型  最大牌点数 最大牌花色 
	BrandNiuNiuLevel bLevel = BrandNiuNiuLevel::BNNL_Non;//默认无牛
	BYTE bv = 0;//最大的牌的数字
	BYTE HandleBrandValue[MAX_NIUNIU_BrandSum] = { 0 };
	BYTE BrandCardValue[MAX_NIUNIU_BrandSum] = { 0 };//牌的面值
	for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
	{
		HandleBrandValue[i] = __GetBrandHandleValue(BrandValue[i]);
		BrandCardValue[i] = static_cast<BYTE>(((BrandValue[i] - 1) / 4) + 1);
		if (bv == 0 || bv < BrandValue[i])
			bv = BrandValue[i];
	}
	//
	////1.五小牛
	//if (BrandCardValue[0] + BrandCardValue[1] + BrandCardValue[2] + BrandCardValue[3] + BrandCardValue[4] < 10)
	//{
	//	//&& BrandCardValue[0] < 5 && BrandCardValue[1] < 5 && BrandCardValue[2] < 5 && BrandCardValue[3] < 5 && BrandCardValue[4] < 5
	//	bLevel = BrandNiuNiuLevel::BNNL_WXN;//五小牛
	//	return ((bLevel << 8) + bv);
	//}
	////2.炸弹
	//if (
	//	IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[2], BrandCardValue[3]) ||
	//	IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[2], BrandCardValue[4]) ||
	//	IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[4], BrandCardValue[3]) ||
	//	IsOnceBrandValue(BrandCardValue[0], BrandCardValue[4], BrandCardValue[2], BrandCardValue[3]) ||
	//	IsOnceBrandValue(BrandCardValue[4], BrandCardValue[1], BrandCardValue[2], BrandCardValue[3])
	//	)
	//{
	//	bLevel = BrandNiuNiuLevel::BNNL_ZD;//炸弹
	//	return ((bLevel << 8) + bv);
	//}
	//处理牌的牛牛 BUG  应该全部进行判断 取最大的值
	//先将需要交换的数据记录下来
	HashMap<BYTE, BYTE> m_ChangeMap;
	BYTE OtherValue = 0;
	if ((HandleBrandValue[0] + HandleBrandValue[1] + HandleBrandValue[2]) % 10 == 0)
	{
		OtherValue = ((HandleBrandValue[3] + HandleBrandValue[4]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
	}
	if ((HandleBrandValue[0] + HandleBrandValue[1] + HandleBrandValue[3]) % 10 == 0 && ((HandleBrandValue[2] + HandleBrandValue[4]) % 10) >= OtherValue)
	{

		OtherValue = ((HandleBrandValue[2] + HandleBrandValue[4]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		//交换牌
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(2, 3));
	}
	if ((HandleBrandValue[0] + HandleBrandValue[1] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[2] + HandleBrandValue[3]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[3] + HandleBrandValue[2]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(2, 4));
	}
	if ((HandleBrandValue[0] + HandleBrandValue[2] + HandleBrandValue[3]) % 10 == 0 && ((HandleBrandValue[1] + HandleBrandValue[4]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[1] + HandleBrandValue[4]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(3, 1));
	}
	if ((HandleBrandValue[0] + HandleBrandValue[2] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[3] + HandleBrandValue[1]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[3] + HandleBrandValue[1]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(1, 4));
	}
	if ((HandleBrandValue[0] + HandleBrandValue[3] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[2] + HandleBrandValue[1]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[1] + HandleBrandValue[2]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(1, 3));
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(2, 4));
	}
	if ((HandleBrandValue[1] + HandleBrandValue[2] + HandleBrandValue[3]) % 10 == 0 && ((HandleBrandValue[0] + HandleBrandValue[4]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[0] + HandleBrandValue[4]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(0, 3));
	}
	if ((HandleBrandValue[1] + HandleBrandValue[2] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[0] + HandleBrandValue[3]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[3] + HandleBrandValue[0]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(0, 4));
	}
	if ((HandleBrandValue[1] + HandleBrandValue[3] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[2] + HandleBrandValue[0]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[0] + HandleBrandValue[2]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(0, 3));
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(2, 4));
	}
	if ((HandleBrandValue[2] + HandleBrandValue[3] + HandleBrandValue[4]) % 10 == 0 && ((HandleBrandValue[1] + HandleBrandValue[0]) % 10) >= OtherValue)
	{
		OtherValue = ((HandleBrandValue[0] + HandleBrandValue[1]) % 10);//0-9
		if (OtherValue == 0)
			OtherValue = 10;
		m_ChangeMap.clear();
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(0, 3));
		m_ChangeMap.insert(HashMap<BYTE, BYTE>::value_type(1, 4));
	}
	if (OtherValue == 0)
	{
		bLevel = BrandNiuNiuLevel::BNNL_Non;//无牛
		return ((bLevel << 8) + bv);
	}

	//进行切换牌的操作
	if (!m_ChangeMap.empty())
	{
		HashMap<BYTE, BYTE>::iterator Iter = m_ChangeMap.begin();
		for (; Iter != m_ChangeMap.end(); ++Iter)
		{
			__ChangeValue(BrandValue[Iter->first], BrandValue[Iter->second]);
		}
		m_ChangeMap.clear();
	}
	switch (OtherValue)
	{
	case 10:
	{
		//有可能为五花牛
		if (BrandCardValue[0] > 10 && BrandCardValue[1] > 10 && BrandCardValue[2] > 10 && BrandCardValue[3] > 10 && BrandCardValue[4] > 10 && BrandCardValue[5] > 10)
		{
			bLevel = BrandNiuNiuLevel::BNNL_WHN;
			return ((bLevel << 8) + bv);
		}
		else
		{
			bLevel = BrandNiuNiuLevel::BNNL_NN;
			return ((bLevel << 8) + bv);
		}
	}
	break;
	case 1:
		bLevel = BrandNiuNiuLevel::BNNL_1;
		return ((bLevel << 8) + bv);
		break;
	case 2:
		bLevel = BrandNiuNiuLevel::BNNL_2;
		return ((bLevel << 8) + bv);
		break;
	case 3:
		bLevel = BrandNiuNiuLevel::BNNL_3;
		return ((bLevel << 8) + bv);
		break;
	case 4:
		bLevel = BrandNiuNiuLevel::BNNL_4;
		return ((bLevel << 8) + bv);
		break;
	case 5:
		bLevel = BrandNiuNiuLevel::BNNL_5;
		return ((bLevel << 8) + bv);
		break;
	case 6:
		bLevel = BrandNiuNiuLevel::BNNL_6;
		return ((bLevel << 8) + bv);
		break;
	case 7:
		bLevel = BrandNiuNiuLevel::BNNL_7;
		return ((bLevel << 8) + bv);
		break;
	case 8:
		bLevel = BrandNiuNiuLevel::BNNL_8;
		return ((bLevel << 8) + bv);
		break;
	case 9:
		bLevel = BrandNiuNiuLevel::BNNL_9;
		return ((bLevel << 8) + bv);
		break;
	}
	return 0;
}
BYTE NiuNiuHandleCommon::__GetBrandRateValue(NiuNiuTableRole* pTableRole)
{
	if (nullptr == pTableRole)
		return 1;
	WORD wBrandValue = pTableRole->GetBrandValue();
	BYTE RoomID = QueryRoleRoomID(pTableRole->GetRoleID());
	BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.GetRateByBrandValue(RoomID, wBrandValue);
	return Rate;
}
BYTE NiuNiuHandleCommon::__GetBrandHandleValue(BYTE Value)
{
	BYTE Type = (Value - 1) % 4;//0-3  
	BYTE BrandValue = ((Value - 1) / 4) + 1;//0-12 (A-10-K)
	if (BrandValue == 13 || BrandValue == 11 || BrandValue == 12)
		return 10;
	else
		return BrandValue;
}
void NiuNiuHandleCommon::__ChangeValue(BYTE& A, BYTE& B)
{
	BYTE C = 0;
	C = A;
	A = B;
	B = C;
}
void NiuNiuHandleCommon::OnTableJoinWriteBegin(BYTE bRoomID, WORD wTableID) 
{
	//当前房间进入 暂停阶段 我们发送命令到客户端去
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(bRoomID);
	const tagFiveNiuNiuRate* pNiuNiuRateConfig = GetNiuNiuRateConfig(bRoomID);
	NiuNiuRoom* pRoom = __FindRoom(bRoomID);
	NiuNiuTable* pTable = __FindTable(pRoom, wTableID);
	if (nullptr == pNiuNiuRoomConfig || nullptr == pNiuNiuRateConfig || nullptr == pRoom || nullptr == pTable)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_FTableJoinWrite msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_FTableJoinWrite), sizeof(MG_Cmd_FTableJoinWrite));
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator IterRole = rRoleHashMap.cbegin();
	for (; IterRole != rRoleHashMap.cend(); ++IterRole)
	{
		msg.dwUserID = IterRole->second->GetRoleID();
		if (nullptr == IterRole->second->GetRole())
			IterRole->second->GetRole()->SendDataToGameServer(&msg);
	}
}
void NiuNiuHandleCommon::__CheckIsNeedCreateTable(NiuNiuRoom* pNiuNiuRoom)
{
	if (nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = GetNiuNiuRoomConfig(pNiuNiuRoom->GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return;
	}
	BYTE bMaxTableRoleSum = pNiuNiuRoomConfig->bTableMaxRoleSum;
	WORD wFrrTableSum = 0;
	//我们遍历全部的桌子
	const HashMap<WORD, ITable*>& rTableHashMap = pNiuNiuRoom->GetAllTableHashMap();
	HashMap<WORD, ITable*>::const_iterator CIter = rTableHashMap.cbegin();
	for (; CIter != rTableHashMap.cend(); ++CIter)
	{
		if (CIter->second->GetTableRoleSum() < bMaxTableRoleSum)
		{
			++wFrrTableSum;
		}
	}
	if (wFrrTableSum <= rTableHashMap.size() / 10)//空闲桌子数量少于1/10
	{
		//创建新的桌子
		WORD wCreateSum = (rTableHashMap.size() / 10) - wFrrTableSum;
		for (WORD i = 0; i < wCreateSum; ++i)
		{
			__CreateTable(pNiuNiuRoom);
		}
	}
}
void NiuNiuHandleCommon::__CreateTable(NiuNiuRoom* pNiuNiuRoom)
{
	if (nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	ITable* pTable = pNiuNiuRoom->CreateTable();
	NiuNiuTable* pNiuNiuTable = dynamic_cast<NiuNiuTable*>(pTable);
	if (!pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	__SendTableInfoToClient(pNiuNiuRoom, pNiuNiuTable);
}
void NiuNiuHandleCommon::__SendTableInfoToClient(NiuNiuRoom* pNiuNiuRoom, NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuRoom || nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	std::vector<DWORD> RoomRoleVec;
	__GetNotInTableRoleVec(pNiuNiuRoom, RoomRoleVec);
	if (RoomRoleVec.empty())
		return;
	MG_Cmd_FOnceTableChange msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, MG_FOnceTableChange), sizeof(MG_Cmd_FOnceTableChange));
	msg.oTableInfo.TableID = pNiuNiuTable->GetTableID();
	msg.oTableInfo.TableStates = pNiuNiuTable->GetTableRoleSum();
	std::vector<DWORD>::const_iterator CIter = RoomRoleVec.begin();
	for (; CIter != RoomRoleVec.cend(); ++CIter)
	{
		msg.dwUserID = *CIter;
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(msg.dwUserID);
		if (nullptr != pRole)
			pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuHandleCommon::__GetNotInTableRoleVec(NiuNiuRoom* pNiuNiuRoom, vector<DWORD>& rRoomRoleVec)
{
	rRoomRoleVec.clear();
	if (nullptr == pNiuNiuRoom)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, BYTE>::const_iterator CIterRoom = m_RoleRoomIDHashMap.cbegin();
	for (; CIterRoom != m_RoleRoomIDHashMap.cend(); ++CIterRoom)
	{
		if (CIterRoom->second != pNiuNiuRoom->GetRoomID())
			continue;
		HashMap<DWORD, WORD>::const_iterator CIterTable = m_RoleTableIDHashMap.find(CIterRoom->first);
		if (CIterTable != m_RoleTableIDHashMap.cend())
			continue;
		rRoomRoleVec.push_back(CIterRoom->first);//在指定房间里但是不在桌子了
	}
}
BYTE NiuNiuHandleCommon::QueryRoleRoomID(DWORD dwUserID) const
{
	return m_pRoomRoleManager->QueryRoleRoomID(dwUserID);
}
WORD NiuNiuHandleCommon::QueryRoleTableID(DWORD dwUserID) const
{
	return m_pRoomRoleManager->QueryRoleTableID(dwUserID);
}
void NiuNiuHandleCommon::SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID)
{
	m_pRoomRoleManager->SetRoleJoinRoom(dwUserID, bRoomID);
	HashMap<DWORD, BYTE>::iterator Iter = m_RoleRoomIDHashMap.find(dwUserID);
	if (m_RoleRoomIDHashMap.end() == Iter)
	{
		m_RoleRoomIDHashMap.insert(HashMap<DWORD, BYTE>::value_type(dwUserID, bRoomID));
	}
	else
	{
		Iter->second = bRoomID;
	}
}
void NiuNiuHandleCommon::SetRoleJoinTable(DWORD dwUserID, WORD wTableID)
{
	m_pRoomRoleManager->SetRoleJoinTable(dwUserID, wTableID);
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
void NiuNiuHandleCommon::SetRoleLeaveRoom(DWORD dwUserID)
{
	m_pRoomRoleManager->SetRoleLeaveRoom(dwUserID);
	m_RoleRoomIDHashMap.erase(dwUserID);
	m_RoleTableIDHashMap.erase(dwUserID);
}
void NiuNiuHandleCommon::SetRoleLeaveTable(DWORD dwUserID)
{
	m_pRoomRoleManager->SetRoleLeaveTable(dwUserID);
	m_RoleTableIDHashMap.erase(dwUserID);
}
//房间
NiuNiuRoom::NiuNiuRoom(BYTE bRoomID, RoomType eRoomType, IHandleCommon* pHandleCommon) 
	:IRoom(bRoomID, eRoomType, pHandleCommon), m_eRoomType(eRoomType), m_bRoomID(bRoomID), m_wNowUseTableID(1), m_pIHandleCommon(dynamic_cast<NiuNiuHandleCommon*>(pHandleCommon))
{
	__Init();
}
NiuNiuRoom::~NiuNiuRoom()
{
	__Destory();
}
void NiuNiuRoom::__Init()
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return;
	}
	__ClearAllTable();
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return;
	}
	for (WORD i = 0; i < pNiuNiuRoomConfig->wInitTableSum; ++i)
	{
		CreateTable();
	}
}
void NiuNiuRoom::__Destory()
{
	__ClearAllTable();
}
void NiuNiuRoom::__ClearAllTable()
{
	if (!m_TableHashMap.empty())
	{
		HashMap<WORD, ITable*>::iterator Iter = m_TableHashMap.begin();
		for (; Iter != m_TableHashMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_TableHashMap.clear();
	}
}
ITable* NiuNiuRoom::CreateTable()
{
	//创建一个桌子
	ITable* pTable = new NiuNiuTable(m_bRoomID, m_wNowUseTableID++, m_pIHandleCommon);
	m_TableHashMap.insert(HashMap<WORD, ITable*>::value_type(pTable->GetTableID(), pTable));
	return pTable;
}
ITable* NiuNiuRoom::FindTable(WORD wTableID) const
{
	HashMap<WORD, ITable*>::const_iterator Iter = m_TableHashMap.find(wTableID);
	if (Iter == m_TableHashMap.end())
		return nullptr;
	else
		return Iter->second;
}
void NiuNiuRoom::DestoryTable(WORD wTableID)
{
	HashMap<WORD, ITable*>::iterator Iter = m_TableHashMap.find(wTableID);
	if (Iter == m_TableHashMap.end())
		return;
	delete Iter->second;
	m_TableHashMap.erase(Iter);
}
void NiuNiuRoom::Update(DWORD dwTimer)
{
	if (m_TableHashMap.empty())
		return;
	HashMap<WORD, ITable*>::iterator Iter = m_TableHashMap.begin();
	for (; Iter != m_TableHashMap.end(); ++Iter)
	{
		if (nullptr != Iter->second)
			Iter->second->Update(dwTimer);
	}
}
//桌子
NiuNiuTable::NiuNiuTable(BYTE bRoomID, WORD wTableID, NiuNiuHandleCommon* pHandleCommon)
	:ITable(bRoomID, wTableID, dynamic_cast<IHandleCommon*>(pHandleCommon)), ITimer(), m_wTableID(wTableID), m_bRoomID(bRoomID), 
	m_eTableState(FNNTS_WriteBegin), m_dwTableStateTimeLog(0), m_pIHandleCommon(pHandleCommon), m_pTableRobotManager(nullptr)
{
	__Init();
}
NiuNiuTable::~NiuNiuTable()
{
	__Destory();
}
void NiuNiuTable::__Init()
{
	__ClearAllTableRole();

	AddTimer(1000, &NiuNiuTable::__UpdateTableState, true);//添加一个 1s触发一次的定时器
	AddTimer(1000, &NiuNiuTable::__UpdateTableRobot, true);
	

	m_pTableRobotManager = new NiuNiuTableRobotManager(this, m_pIHandleCommon);

	m_pTableRobotManager->OnCreateTableEvent();
}
void NiuNiuTable::__Destory()
{
	__ClearAllTableRole();
	Clear();

	delete m_pTableRobotManager;

	m_pTableRobotManager->OnTableDestoryEvent();
}
void NiuNiuTable::__ClearAllTableRole()
{
	if (!m_TableRoleHashMap.empty())
	{
		HashMap<DWORD, ITableRole*>::iterator Iter = m_TableRoleHashMap.begin();
		for (; Iter != m_TableRoleHashMap.end(); ++Iter)
		{
			delete Iter->second;
		}
		m_TableRoleHashMap.clear();
		m_TableRoleVec.clear();
	}
}
BYTE NiuNiuTable::__GetNullSeat() const
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return 0xff;
	}
	vector<ITableRole*>::const_iterator CIter = m_TableRoleVec.cbegin();
	for (BYTE i=0; CIter != m_TableRoleVec.cend(); ++CIter,++i)
	{
		if (nullptr == (*CIter))
			return i;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return 0xff;
	}
	return (m_TableRoleVec.size() >= pNiuNiuRoomConfig->bTableMaxRoleSum ? 0xff : m_TableRoleVec.size());
}
ITableRole* NiuNiuTable::CreateTableRole(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (nullptr == pRole)
	{
		ASSERT(false);
		return nullptr;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return nullptr;
	}
	BYTE SeatID = __GetNullSeat();
	if (0xff == SeatID)
	{
		ASSERT(false);
		return nullptr;
	}
	ITableRole* pTableRole = FindTableRole(dwUserID);
	if (nullptr != pTableRole)
	{
		ASSERT(false);
		return pTableRole;
	}
	pTableRole = new NiuNiuTableRole(SeatID, dwUserID);//设置玩家位置和ID
	m_TableRoleHashMap.insert(HashMap<DWORD, ITableRole*>::value_type(pTableRole->GetRoleID(), pTableRole));
	if (m_TableRoleVec.empty())
		m_TableRoleVec.resize(pNiuNiuRoomConfig->bTableMaxRoleSum);
	m_TableRoleVec[SeatID] = pTableRole;

	m_pTableRobotManager->OnRoleJoinTableEvent(dwUserID);

	return pTableRole;
}
ITableRole* NiuNiuTable::FindTableRole(DWORD dwUserID) const
{
	HashMap<DWORD, ITableRole*>::const_iterator Iter = m_TableRoleHashMap.find(dwUserID);
	if (Iter == m_TableRoleHashMap.end())
		return nullptr;
	else
		return Iter->second;
}
void NiuNiuTable::DestoryTableRole(DWORD dwUserID)
{
	HashMap<DWORD, ITableRole*>::iterator Iter = m_TableRoleHashMap.find(dwUserID);
	if (Iter == m_TableRoleHashMap.end())
		return;
	BYTE SeatID = Iter->second->GetSeatID();
	m_TableRoleVec[SeatID] = nullptr;
	delete Iter->second;
	m_TableRoleHashMap.erase(Iter);

	m_pTableRobotManager->OnRoleLeaveTableEvent(dwUserID);
}
void NiuNiuTable::Update(DWORD dwTimer)
{
	//进行状态更新
	OnUpdateTimer(dwTimer);
}
void NiuNiuTable::__UpdateTableRobot(DWORD dwTimer)
{
	m_pTableRobotManager->Update(dwTimer);
}
void NiuNiuTable::__UpdateTableState(DWORD dwTimer)
{
	//1s更新一次
	if (__IsCanJoinBegin(dwTimer))
	{
		__SetTableState(FiveNiuNiuTableStates::FNNTS_Begin);
		m_pTableRobotManager->OnBeginTableStateChange(this);
		__OnJoinBegin();
		m_pTableRobotManager->OnEndTableStateChange(this);
	}
	else if (__IsCanJoinEnd(dwTimer))
	{
		__SetTableState(FiveNiuNiuTableStates::FNNTS_End);
		m_pTableRobotManager->OnBeginTableStateChange(this);
		__OnJoinEnd();
		m_pTableRobotManager->OnEndTableStateChange(this);
	}
	else if (__IsCanJoinWriteBegin(dwTimer))
	{
		__SetTableState(FiveNiuNiuTableStates::FNNTS_WriteBegin);
		m_pTableRobotManager->OnBeginTableStateChange(this);
		__OnJoinWriteBegin();
		m_pTableRobotManager->OnEndTableStateChange(this);
	}

}
bool NiuNiuTable::__IsCanJoinBegin(DWORD dwTimer) const
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return false;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return false;
	}
	switch (m_eTableState)
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		{
			//从开始状态到开始状态
			return false;
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		{
			//从结束状态进入到开始状态
			if (GetTableRoleSum() >= pNiuNiuRoomConfig->bOpenGameMinRoleSum && dwTimer >= m_dwTableStateTimeLog)
				return true;
			else
				return false;
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		{
			//从暂停状态到开始状态
			if (GetTableRoleSum() >= pNiuNiuRoomConfig->bOpenGameMinRoleSum)
				return true;
			else
				return false;
		}
		break;
	default:
		return false;
	}
}
bool NiuNiuTable::__TableIsAllRoleShowBrand() const
{
	HashMap<DWORD, ITableRole*>::const_iterator Iter = m_TableRoleHashMap.cbegin();
	for (; Iter != m_TableRoleHashMap.cend(); ++Iter)
	{
		NiuNiuTableRole* pRole = dynamic_cast<NiuNiuTableRole*>(Iter->second);
		if (pRole == nullptr || pRole->GetBrandValue() == 0xffff)
			continue;
		if (!pRole->GetIsShowBrand())
			return false;
	}
	return true;
}
bool NiuNiuTable::__IsCanJoinEnd(DWORD dwTimer) const
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return false;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return false;
	}
	switch (m_eTableState)
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		{
			//从开始状态到结束状态
			//便利所有玩家  是否全部摊牌了
			if (dwTimer >= m_dwTableStateTimeLog)
				return true;
			else
				return __TableIsAllRoleShowBrand();
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		{
			//从结束状态到结束状态
			return false;
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		{
			//从暂停状态到结束状态
			return false;
		}
		break;
	default:
		return false;
	}
}
bool NiuNiuTable::__IsCanJoinWriteBegin(DWORD dwTimer) const
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return false;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return false;
	}
	switch (m_eTableState)
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		{
			//从开始状态到暂停状态
			return false;
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		{
			//从暂停状态到暂停状态
			return false;
		}
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		{
			//从结束状态到暂停状态
			if (GetTableRoleSum() < pNiuNiuRoomConfig->bOpenGameMinRoleSum && dwTimer >= m_dwTableStateTimeLog)
				return true;
			else
				return false;
		}
		break;
	default:
		return false;
	}
}
void NiuNiuTable::__SetTableState(FiveNiuNiuTableStates eTableState)
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return;
	}
	const tagNiuNiuRoomConfig* pNiuNiuRoomConfig = m_pIHandleCommon->GetNiuNiuRoomConfig(GetRoomID());
	if (nullptr == pNiuNiuRoomConfig)
	{
		ASSERT(false);
		return;
	}
	m_eTableState = eTableState;
	switch (m_eTableState)
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		m_dwTableStateTimeLog = timeGetTime() + pNiuNiuRoomConfig->wBeginWriteSec * 1000;
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		m_dwTableStateTimeLog = timeGetTime() + pNiuNiuRoomConfig->wEndWriteSec * 1000;
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		m_dwTableStateTimeLog = timeGetTime() + 600 * 1000;
		break;
	default:
		return;
	}
}
void NiuNiuTable::__OnJoinBegin()
{
	//将事件传递到HandleCommon去
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return;
	}
	m_pIHandleCommon->OnTableJoinBegin(GetRoomID(), GetTableID());
}
void NiuNiuTable::__OnJoinWriteBegin()
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return;
	}
	m_pIHandleCommon->OnTableJoinWriteBegin(GetRoomID(), GetTableID());
}
void NiuNiuTable::__OnJoinEnd()
{
	if (nullptr == m_pIHandleCommon)
	{
		ASSERT(false);
		return;
	}
	m_pIHandleCommon->OnTableJoinEnd(GetRoomID(), GetTableID());
}
DWORD NiuNiuTable::GetTableInfoState() const
{
	//处理桌子上的数据
	if (m_TableRoleHashMap.empty())
		return 0;
	DWORD State = 0;
	vector<ITableRole*>::const_iterator CIter = m_TableRoleVec.cbegin();
	for (; CIter != m_TableRoleVec.cend(); ++CIter)
	{
		if (nullptr == (*CIter))
			continue;
		DWORD NowState = 1;
		NowState = (NowState << (*CIter)->GetSeatID());

		State += NowState;
	}
	return State;
}
//桌子上的玩家
NiuNiuTableRole::NiuNiuTableRole(BYTE bSeatID, DWORD dwUserID) :ITableRole(bSeatID, dwUserID), m_dwUserID(dwUserID), m_pRole(nullptr), m_bSeatID(bSeatID), m_wBrandValue(0xffff), m_bIsShowBrand(false)
{
	__Init();
}
NiuNiuTableRole::~NiuNiuTableRole()
{
	__Destory();
}
void NiuNiuTableRole::__Init()
{
	m_pRole = g_FishServer.GetRoleManager().QueryRole(m_dwUserID);
	for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
		m_bArrTableBrandArray[i] = 0;
}
void NiuNiuTableRole::__Destory()
{

}
void NiuNiuTableRole::SetBrandArray(BYTE* parrBrandArray)
{
	if (nullptr == parrBrandArray)
	{
		ASSERT(false);
		return;
	}
	for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
		m_bArrTableBrandArray[i] = parrBrandArray[i];
}
//牛牛桌子机器人管理器 放入桌子里面去的
NiuNiuTableRobotManager::NiuNiuTableRobotManager(ITable* pTable, IHandleCommon* pHandleCommon) :ITableRobotManager(pTable, pHandleCommon)
			,m_pNiuNiuTable(dynamic_cast<NiuNiuTable*>(pTable)), m_NiuNiuHandleCommon(dynamic_cast<NiuNiuHandleCommon*>(pHandleCommon))
{

}
NiuNiuTableRobotManager::~NiuNiuTableRobotManager()
{

}
void NiuNiuTableRobotManager::OnCreateTableEvent()
{
}
void NiuNiuTableRobotManager::OnRoleJoinTableEvent(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	if (g_FishServer.GetRobotManager().IsRobot(dwUserID))
		return;
	//当玩家进入桌子的时候
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator CIter=g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.find(m_pNiuNiuTable->GetRoomID());
	if (CIter == g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.end())
	{
		ASSERT(false);
		return;
	}
	if (m_pNiuNiuTable->GetTableRoleSum() < CIter->second.bOpenGameMinRoleSum)
	{
		BYTE bAddRobotSum = CIter->second.bOpenGameMinRoleSum - m_pNiuNiuTable->GetTableRoleSum();
		for (BYTE i = 0; i < bAddRobotSum; ++i)
		{
			__AddNewRobot();
		}
	}
}
void NiuNiuTableRobotManager::OnRoleLeaveTableEvent(DWORD dwUserID)
{
	//当玩家离开桌子的时候
}
void NiuNiuTableRobotManager::OnTableDestoryEvent()
{
	//当桌子销毁的时候
	std::vector<DWORD> RobotVec;
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = m_pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (; CIter != rRoleHashMap.cend(); ++CIter)
	{
		if (!g_FishServer.GetRobotManager().IsRobot(CIter->first))
			continue;
		RobotVec.push_back(CIter->first);
	}
	if (!RobotVec.empty())
	{
		std::vector<DWORD>::const_iterator CIter = RobotVec.cbegin();
		for (; CIter != RobotVec.cend(); ++CIter)
		{
			__DelRobot(*CIter);
		}
		RobotVec.clear();
	}
}
void NiuNiuTableRobotManager::__AddNewRobot()
{
	//向桌子里添加一个新的机器人
	RobotUserInfo* pRobot = g_FishServer.GetRobotManager().GetNonUseRobot();
	if (nullptr == pRobot)
	{
		ASSERT(false);
		return;
	}
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(pRobot->dwUserID);
	if (pRole == nullptr)
	{
		ASSERT(false);
		return;
	}
	m_NiuNiuHandleCommon->SetRoleJoinRoom(pRole->GetRoleID(), m_pNiuNiuTable->GetRoomID());
	GM_Cmd_FJoinRoomTable msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FJoinRoomTable), sizeof(GM_Cmd_FJoinRoomTable));
	msg.dwUserID = pRole->GetRoleID();
	msg.wTableID = m_pNiuNiuTable->GetTableID();
	m_NiuNiuHandleCommon->HandleMsg(&msg);
}
void NiuNiuTableRobotManager::__DelRobot(DWORD dwUserID)
{
	//回收一个指定ID的机器人
	ITableRole* pRole = m_pNiuNiuTable->FindTableRole(dwUserID);
	if (nullptr == pRole)
	{
		ASSERT(false);
		return;
	}
	GM_Cmd_FRoleLeaveRoom msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FRoleLeaveRoom), sizeof(GM_Cmd_FRoleLeaveRoom));
	msg.dwUserID = dwUserID;
	m_NiuNiuHandleCommon->HandleMsg(&msg);
	g_FishServer.GetRobotManager().OnNonUseRobot(dwUserID);
}
void NiuNiuTableRobotManager::OnBeginTableStateChange(ITable* pTable)
{
	NiuNiuTable* pNiuNiuTable = dynamic_cast<NiuNiuTable*>(pTable);
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
	}
	switch (pNiuNiuTable->GetTableState())
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		__TableBeginJoinBegin(pNiuNiuTable);
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		__TableBeginJoinEnd(pNiuNiuTable);
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		__TableBeginJoinWriteBegin(pNiuNiuTable);
		break;
	}
}
void NiuNiuTableRobotManager::OnEndTableStateChange(ITable* pTable)
{
	NiuNiuTable* pNiuNiuTable = dynamic_cast<NiuNiuTable*>(pTable);
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
	}
	switch (pNiuNiuTable->GetTableState())
	{
	case FiveNiuNiuTableStates::FNNTS_Begin:
		__TableEndJoinBegin(pNiuNiuTable);
		break;
	case FiveNiuNiuTableStates::FNNTS_End:
		__TableEndJoinEnd(pNiuNiuTable);
		break;
	case FiveNiuNiuTableStates::FNNTS_WriteBegin:
		__TableEndJoinWriteBegin(pNiuNiuTable);
		break;
	}
}
void NiuNiuTableRobotManager::Update(DWORD dwTimer)
{
	//1s一次更新机器人的处理
	if (m_RobotEventVec.empty())
		return;
	std::vector<tagNiuNiuTableRobotEvent>::iterator Iter = m_RobotEventVec.begin();
	for (; Iter != m_RobotEventVec.end();)
	{
		if (dwTimer >= Iter->dwTimer)
		{
			__HandleEvent(dwTimer, *Iter);
			Iter = m_RobotEventVec.erase(Iter);
		}
		else
			++Iter;
	}
}
void NiuNiuTableRobotManager::__HandleEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent)
{
	switch (rEvent.bEventID)
	{
	case 1:
		__HandleJoinGlobelEvent(dwTimer, rEvent);
		break;
	case 2:
		__HandleShowBrandEvent(dwTimer, rEvent);
		break;
	}
}
void NiuNiuTableRobotManager::__HandleJoinGlobelEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent)
{
	if (rEvent.dwTimer > dwTimer)
		return;
	NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(m_pNiuNiuTable->FindTableRole(rEvent.dwUserID));
	if (nullptr == pNiuNiuTableRole)
	{
		ASSERT(false);
		return;
	}
	pNiuNiuTableRole->GetRole()->GetRoleInfo().dwGlobelSum += 200000;
	GM_Cmd_FRoleInfoChange msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_FRoleInfoChange), sizeof(GM_Cmd_FRoleInfoChange));
	msg.dwUserID = rEvent.dwUserID;
	m_NiuNiuHandleCommon->HandleMsg(&msg);
}
void NiuNiuTableRobotManager::__HandleShowBrandEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent)
{
	if (rEvent.dwTimer > dwTimer)
		return;
	NiuNiuTableRole* pNiuNiuTableRole = dynamic_cast<NiuNiuTableRole*>(m_pNiuNiuTable->FindTableRole(rEvent.dwUserID));
	if (nullptr == pNiuNiuTableRole)
	{
		ASSERT(false);
		return;
	}
	GM_Cmd_ShowBrandInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_FiveNiuNiu, GM_ShowBrandInfo), sizeof(GM_Cmd_ShowBrandInfo));
	msg.dwUserID = rEvent.dwUserID;
	BYTE bArrayBrand[MAX_NIUNIU_BrandSum] = { 0 };
	pNiuNiuTableRole->GetBrandArray(bArrayBrand);
	msg.wBrandValue = m_NiuNiuHandleCommon->HandleBrandValue(bArrayBrand);
	pNiuNiuTableRole->SetBrandArray(bArrayBrand);
	msg.bChangeIndex = 0; //设置玩家需要切换牌的数据 无须切牌 
	m_NiuNiuHandleCommon->HandleMsg(&msg);
}
void NiuNiuTableRobotManager::__TableBeginJoinBegin(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator Iter = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.find(pNiuNiuTable->GetRoomID());
	if (Iter == g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.end())
	{
		ASSERT(false);
		return;
	}
	//开始进入开始状态的时候 我们处理机器人金币问题
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (; CIter != rRoleHashMap.cend(); ++CIter)
	{
		if (g_FishServer.GetRobotManager().IsRobot(CIter->first))
		{
			//有机器人的话 我们进行处理
			NiuNiuTableRole* pRole = dynamic_cast<NiuNiuTableRole*>(CIter->second);
			if (pRole->GetRole()->GetRoleInfo().dwGlobelSum <= 200000)
			{
				//给机器人添加金币
				tagNiuNiuTableRobotEvent oEvent;
				oEvent.dwUserID = pRole->GetRoleID();
				oEvent.dwTimer = timeGetTime() + static_cast<WORD>(RandUInt() % (Iter->second.wBeginWriteSec - 1)) * 1000;
				oEvent.bEventID = 1;//添加金币事件
				m_RobotEventVec.push_back(oEvent);
			}
		}
	}
}
void NiuNiuTableRobotManager::__TableEndJoinBegin(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, tagNiuNiuRoomConfig>::const_iterator Iter = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.find(pNiuNiuTable->GetRoomID());
	if (Iter == g_FishServer.GetFishConfig().GetFishMiNiGameConfig().fiveniuniuConfig.NiuNiuRoomMap.end())
	{
		ASSERT(false);
		return;
	}
	const HashMap<DWORD, ITableRole*>& rRoleHashMap = pNiuNiuTable->GetAllRole();
	HashMap<DWORD, ITableRole*>::const_iterator CIter = rRoleHashMap.cbegin();
	for (; CIter != rRoleHashMap.cend(); ++CIter)
	{
		if (g_FishServer.GetRobotManager().IsRobot(CIter->first))
		{
			//有机器人的话 我们进行处理
			NiuNiuTableRole* pRole = dynamic_cast<NiuNiuTableRole*>(CIter->second);
			//让机器人进行翻牌处理
			tagNiuNiuTableRobotEvent oEvent;
			oEvent.dwUserID = pRole->GetRoleID();
			oEvent.dwTimer = timeGetTime() + static_cast<WORD>(RandUInt() % (Iter->second.wBeginWriteSec - 1)) * 1000;
			oEvent.bEventID = 2;//翻牌事件
			m_RobotEventVec.push_back(oEvent);
		}
	}
}
void NiuNiuTableRobotManager::__TableBeginJoinWriteBegin(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
}
void NiuNiuTableRobotManager::__TableEndJoinWriteBegin(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
}
void NiuNiuTableRobotManager::__TableBeginJoinEnd(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
}
void NiuNiuTableRobotManager::__TableEndJoinEnd(NiuNiuTable* pNiuNiuTable)
{
	if (nullptr == pNiuNiuTable)
	{
		ASSERT(false);
		return;
	}
}