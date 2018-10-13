#include "Stdafx.h"
#include "RoleManager.h"
#include "FishServer.h"
#include "RoleEx.h"
RoleManager::RoleManager()
{
	m_ResetGameData[0] = false;
	m_ResetGameData[1] = false;
	m_ResetGameData[2] = false;
	m_ResetGameData[3] = false;
}
RoleManager::~RoleManager()
{
	Destroy();
}
void RoleManager::Destroy()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		Iter->second->SaveAllRoleInfo(false);
		Iter->second->SetIsExit(true);//�����������
		SAFE_DELETE(Iter->second);
	}
	m_RoleMap.clear();
	m_RoleSocketMap.clear();
}
bool RoleManager::OnInit()
{
	m_RoleGameDataCache.clear();
	return true;
}
CRoleEx* RoleManager::QueryUser(DWORD dwUserID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
		return Iter->second;
	else
		return NULL;
}

CRoleEx* RoleManager::QuertUserBySocketID(DWORD dwSocketID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleSocketMap.find(dwSocketID);
	if (Iter != m_RoleSocketMap.end())
		return Iter->second;
	else
		return NULL;
}
CRoleEx* RoleManager::QuertUserByUid(int64 uid)
{
	for (auto it = m_RoleMap.begin(); it != m_RoleMap.end(); ++it)
	{
		if (it->second->GetRoleInfo().Uid == uid)
		{
			return it->second;
		}
	}
	return NULL;
}
void  RoleManager::OnDelUser(DWORD dwUserID, bool IsWriteSaveInfo, bool IsLeaveCenter)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
	{
		//��һ����ҵ��� ���� �˳�GameServer��ʱ�� ����Ӧ�û�����ҵ�RoleEx���� 
		//RoleEx���� ����ɾ�� ֱ������RoleEx ��������� ����SocketID Ϊ0  �޿ͻ��������� �ڱ����뿪���� ������ �뿪���ӵ� 
		//������Ҫ��Logon����ͨѶ ����ҽ��뵽GameServer��ʱ�� ������Ҫ��Logon�ҵ� ��� �ڶ��gameServer �ϵķֲ���
		if (IsWriteSaveInfo)
		{
			if (!Iter->second->SaveAllRoleInfo(true))
			{
				ASSERT(false);
			}
		}
		//���������Ϣ�����ݿ�
		DBR_Cmd_SaveRoleExInfo msg;
		SetMsgInfo(msg, DBR_SaveRoleExInfo, sizeof(DBR_Cmd_SaveRoleExInfo));
		memcpy_s(&msg.RoleInfo, sizeof(tagRoleInfo), &(Iter->second->GetRoleInfo()), sizeof(tagRoleInfo));
		g_FishServer.SendNetCmdToDB(&msg);
		//ֱ��ɾ����� �Ƿ���Ҫ�������������?
		//������ �� �������ʧ�� ������Ҫ�����������
		//���ֱ������ 
		//Iter->second->ChangeRoleIsOnline(false);
		if (IsLeaveCenter)
			Iter->second->SendUserLeaveToCenter();
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

		m_RoleSocketMap.erase(Iter->second->GetGameSocketID()/*GetGateSocketID*/);
		SAFE_DELETE(Iter->second);
		m_RoleMap.erase(Iter);
	}
}
void RoleManager::OnDelUserResult(DBO_Cmd_SaveRoleAllInfo* pResult)
{
	if (!pResult)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pResult->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		if (Iter->second->IsExit())//������˳�״̬
		{
			Iter->second->SendUserLeaveToCenter();
			g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

			m_RoleSocketMap.erase(Iter->second->GetGameSocketID());
			SAFE_DELETE(Iter->second);
			m_RoleMap.erase(Iter);
		}
		else
		{
			return;
		}
	}
	else
	{
		ASSERT(false);
		return;
	}
}

void RoleManager::OnKickOneUser(CRoleEx* pRole)
{
	if (pRole)
	{
		//������Ϸ����
		auto gdCacheIt = m_RoleGameDataCache.find(pRole->GetRoleInfo().Uid);
		if (gdCacheIt == m_RoleGameDataCache.end())
		{
			m_RoleGameDataCache.insert(make_pair(pRole->GetRoleInfo().Uid, pRole->GetRoleGameData().GetGameData()));
		}
		else
		{
			gdCacheIt->second = pRole->GetRoleGameData().GetGameData();
		}
		//ͬ�����ݵ���Ϸ����
		tagClientUserData* pUdata = g_FishServer.GetHallDataCache(pRole->GetRoleInfo().Uid);
		if (!pUdata) return;
		UINT32 msgSize = sizeof(GL_QuitSubGame) + pUdata->achSize * (sizeof(tagAchDataMap));
		GL_QuitSubGame* msg = (GL_QuitSubGame*)malloc(msgSize);
		SetMsgInfo((*msg), GetMsgType(Main_Hall, 70), msgSize);
		msg->uid = pRole->GetRoleInfo().Uid;
		msg->money1 = pRole->GetRoleInfo().money1*1.0 / MONEY_RATIO;
		msg->money2 = pRole->GetRoleInfo().money2*1.0 / MONEY_RATIO;
		msg->quitSubGame = false;
		msg->loseNum = pRole->GetRoleGameData().GetLoseNum();
		msg->winNum = pRole->GetRoleGameData().GetWinNum();
		msg->winMoney = pRole->GetRoleGameData().GetTotalWinGold()*1.0 / MONEY_RATIO;
		msg->loseMoney = pRole->GetRoleGameData().GetTotalLoseGold()*1.0 / MONEY_RATIO;
		msg->catchBossFishCount = pRole->GetRoleGameData().GetBossFishCount();
		msg->charmValue = pUdata->charmValue;
		msg->achSize = pUdata->achSize;
		memcpy_s(msg->achData, pUdata->achSize * sizeof(tagAchDataMap), pUdata->achDataMap, pUdata->achSize * sizeof(tagAchDataMap));
		g_FishServer.SendNetCmdToCenter(msg);
		free(msg);
		//�����������
		g_FishServer.OnRemoveHallCatchData(pRole->GetRoleInfo().Uid);
		//�ͻ�������ҷ�����Ϸ����
		GL_TickOut tickMsg;
		SetMsgInfo(tickMsg, 6032, sizeof(GL_TickOut));
		pRole->SendDataToClient(&tickMsg);
		//���������Ϸ��¼
		DBR_Cmd_SaveRecord recordMsg;
		SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
		recordMsg.model = 2;
		recordMsg.uid = pRole->GetRoleInfo().Uid;
		recordMsg.table_id = 0;
		recordMsg.enter_money = (pRole->GetRoleInfo().money1 + pRole->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_money = (pRole->GetRoleInfo().money1 + pRole->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_code = 2;
		g_FishServer.SendNetCmdToDB(&recordMsg);
		//ɾ����Ϸ����
		OnDelUser(pRole->GetUserID(), true, true);
	}
}

void RoleManager::OnKickAllUser()
{
	//ֱ���ߵ����е���� 
	for (auto Iter = m_RoleMap.begin(); Iter != m_RoleMap.end();)
	{
		auto delIt = Iter;
		++Iter;
		if (!g_FishServer.GetFishConfig().CheckInWhiteList(delIt->second->GetRoleInfo().Uid))
		{
			OnKickOneUser(delIt->second);
		}
	}
}
bool RoleManager::CreateRole(tagRoleInfo* pUserInfo,tagRoleServerInfo* pUserServerInfo, DWORD dwSocketID, time_t pTime, bool LogobByGameServer, bool IsRobot)
{
	if (!pUserInfo || !pUserServerInfo)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pUserInfo->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		return true;
	}
	else
	{
		CRoleEx * pUser = new CRoleEx();
		if (!pUser)
		{
			ASSERT(false);
			return false;
		}
		//��Ҷ����ʼ����Ϻ� ���Ǵ洢����
		m_RoleMap.insert(HashMap<DWORD, CRoleEx*>::value_type(pUserInfo->dwUserID, pUser));
		if (!IsRobot)//������������ӵ�SocketMap����
			m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(dwSocketID, pUser));
		//��gamedata���ص������������ȥ
		tagRoleGameData gameData;
		auto gdIt = m_RoleGameDataCache.find(pUserInfo->Uid);
		if (gdIt != m_RoleGameDataCache.end())
		{
			gameData = gdIt->second;
		}
		if (!pUser->OnInit(pUserInfo, pUserServerInfo, this, dwSocketID, pTime, IsRobot, gameData))
		{
			//��ʼ�����ʧ��
			OnDelUser(pUserInfo->dwUserID,false,false);
			return false;
		}
		//���سɹ���ɾ������
		if (gdIt != m_RoleGameDataCache.end())
		{
			m_RoleGameDataCache.erase(gdIt);
		}
		return true;
	}
}
void RoleManager::OnUpdateByMin(bool IsHourChange, bool IsDayChange, bool IsMonthChange, bool IsYearChange)
{
	OnResetGameDataTurn();
	if (m_RoleMap.empty())
		return;
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	for (HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin(); Iter != m_RoleMap.end();)
	{
		if (Iter->second->IsRobot())
		{
			++Iter;
			continue;
		}
		Iter->second->UpdateByMin(pTime.wMinute);
		auto delit = Iter;
		++Iter;
		//�ж����������ӵ� ����ʹ�ü��ܵ�ʱ�� ������ڶ��ٷ��� δ���� ������뿪����
		CRole* pTableRole = g_FishServer.GetTableManager()->SearchUser(delit->second->GetUserID());
		if (pTableRole && pTableRole->IsNeedLeaveTable())
		{
			CRoleEx* pRole = delit->second;
			if (pRole)
			{
				OnKickOneUser(pRole);
			}
			
		}
	}
}
void RoleManager::ChangeRoleSocket(CRoleEx* pRole, DWORD SocketID)
{
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	m_RoleSocketMap.erase(pRole->GetGameSocketID());
	if (SocketID != 0)
		m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(SocketID, pRole));
}
void RoleManager::OnSaveInfoToDB()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->OnSaveInfoToDB();
	}
}
void RoleManager::SetResetGameDataTable(const BYTE tableTypeID)
{
	if (tableTypeID >= 4)
	{
		return;
	}
	m_ResetGameData[tableTypeID] = true;
};

void RoleManager::OnResetGameDataTurn()
{
	for (BYTE i = 0; i < 4; ++i)
	{
		if (m_ResetGameData[i] == true)
		{
			//���������Ӯ��������������ҵģ�������������ҵ�
			for (auto it = m_RoleMap.begin(); it != m_RoleMap.end(); ++it)
			{
				it->second->GetRoleGameData().GetGameData().turn_LoseWinGold[i] = 0;
			}
			//����������ҵ�����
			for (auto it = m_RoleGameDataCache.begin(); it != m_RoleGameDataCache.end(); ++it)
			{
				it->second.turn_LoseWinGold[i] = 0;
				//�������ӵ����ݶ�����ˣ�ɾ������
				if (it->second.turn_LoseWinGold[0] == 0
					&& it->second.turn_LoseWinGold[1] == 0
					&& it->second.turn_LoseWinGold[2] == 0
					&& it->second.turn_LoseWinGold[3] == 0)
				{
					it = m_RoleGameDataCache.erase(it);
				}
			}
			m_ResetGameData[i] = false;
		}
	}
}