#include "Stdafx.h"
#include "RoleRelationManager.h"
#include "RoleManager.h"
#include "FishServer.h"
RoleRelationManager::RoleRelationManager()
{
	m_RelationMap.clear();
	m_BeRelationMap.clear();
	m_pRole = NULL;
	m_RoleManager = NULL;
}
RoleRelationManager::~RoleRelationManager()
{

}
bool RoleRelationManager::OnInit(CenterRole* pRole, CenterRoleManager* pManager)
{
	if (!pRole || !pManager)
	{
		ASSERT(false);
		return false;
	}
	m_pRole = pRole;
	m_RoleManager = pManager;
	return true;
}
void RoleRelationManager::OnLoadUserRelation(CC_Cmd_GetUserRelation* pMsg)
{
	//����ȫ������ҵĽṹ
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	if ((pMsg->States & MsgBegin) != 0)
	{
		m_RelationMap.clear();
	}
	for (size_t i = 0; i < pMsg->Sum; ++i)
	{
		m_RelationMap.insert(HashMap<DWORD, tagRoleRelation>::value_type(pMsg->Array[i].dwDestRoleID, pMsg->Array[i]));
	}
}
void RoleRelationManager::OnLoadUserBeRelation(CC_Cmd_LoadBeUserRelation* pMsg)
{
	//���ر�����ϵ
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	if ((pMsg->States & MsgBegin) != 0)
	{
		m_BeRelationMap.clear();
	}
	for (size_t i = 0; i < pMsg->Sum; ++i)
	{
		m_BeRelationMap.insert(HashMap<DWORD, tagBeRoleRelation>::value_type(pMsg->Array[i].dwUserID, pMsg->Array[i]));
	}
}
//void RoleRelationManager::OnLoadUSerRelationFinish(CC_Cmd_GetUserRelationFinish* pMsg)
//{
//
//}
void RoleRelationManager::OnAddUserRelation(CC_Cmd_AddUserRelation* pMsg)
{
	//�ͻ��˷��������һ����ϵ һ���ɹ�
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	if (m_RelationMap.size() > g_FishServer.GetFishConfig().GetRelation().MaxRelationSum)
		return;
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(pMsg->RelationInfo.dwDestRoleID);
	if (Iter != m_RelationMap.end())
	{
		ASSERT(false);
		return;
	}
	m_RelationMap.insert(HashMap<DWORD, tagRoleRelation>::value_type(pMsg->RelationInfo.dwDestRoleID, pMsg->RelationInfo));
	//m_RelationVec.push_back(pMsg->RelationInfo);
	//��ӳɹ��� ������Ҫת����ϵ��Ŀ������ȥ
	DWORD DestUserID = pMsg->RelationInfo.dwDestRoleID;
	CenterRole* pUser = m_RoleManager->QueryCenterUser(DestUserID);
	if (pUser)
	{
		//��Ŀ��������ߵ�ʱ�� ������Ҫ����ҵ����ݷ��͵�Ŀ������ȥ ����Ŀ����� ����
		tagBeRoleRelation pInfo;
		pInfo.dwUserID = m_pRole->GetRoleID();
		pInfo.bRelationType = pMsg->RelationInfo.bRelationType;
		//�ṹ׼����Ϻ� ������ҵļ�������
		pUser->GetRelationManager().OnAddBeUserRelation(&pInfo);
	}
}
void RoleRelationManager::OnDelUserRelation(CC_Cmd_DelUserRelation* pMsg)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	//���ɾ��һ����ϵ
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(pMsg->dwDestUserID);
	if (Iter != m_RelationMap.end())
	{
		DWORD dwDestUserID = pMsg->dwDestUserID;
		CenterRole* pUser = m_RoleManager->QueryCenterUser(dwDestUserID);
		if (pUser)
		{
			//ѡ��ɾ������
			pUser->GetRelationManager().OnDelBeUserRelation(m_pRole->GetRoleID());
		}
		m_RelationMap.erase(Iter);
		return;
	}
	else
	{
		ASSERT(false);
		return;
	}
}
void RoleRelationManager::OnChangeUserRelation(CC_Cmd_ChangeUserRelation* pMsg)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(pMsg->dwDestUserID);
	if (Iter != m_RelationMap.end())
	{
		DWORD dwDestUserID = pMsg->dwDestUserID;
		CenterRole* pUser = m_RoleManager->QueryCenterUser(dwDestUserID);
		if (pUser)
		{
			//ѡ���޸Ĳ���
			pUser->GetRelationManager().OnChangeBeUserRelation(m_pRole->GetRoleID(), pMsg->RelationType);
		}
		Iter->second.bRelationType = pMsg->RelationType;
		return;
	}
	else
	{
		ASSERT(false);
		return;
	}
}
void RoleRelationManager::OnAddBeUserRelation(tagBeRoleRelation* pInfo)
{
	if (!m_pRole || !pInfo)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagBeRoleRelation>::iterator Iter= m_BeRelationMap.find(pInfo->dwUserID);
	if (Iter != m_BeRelationMap.end())
	{
		ASSERT(false);
		return;
	}
	m_BeRelationMap.insert(HashMap<DWORD, tagBeRoleRelation>::value_type(pInfo->dwUserID, *pInfo));
	//�����Ϻ� ֪ͨ�ͻ��˽������ �������� ����ҵĿͻ���ȥ
	CC_Cmd_AddBeUserRelation msg;
	SetMsgInfo(msg,GetMsgType(Main_Relation, CC_AddBeUserRelation), sizeof(CC_Cmd_AddBeUserRelation));
	msg.dwUserID = m_pRole->GetRoleID();
	msg.RelationInfo = *pInfo;
	m_pRole->SendDataToGameServer(&msg);
}
void RoleRelationManager::OnDelBeUserRelation(DWORD dwSrcUserID)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagBeRoleRelation>::iterator Iter = m_BeRelationMap.find(dwSrcUserID);
	if (Iter != m_BeRelationMap.end())
	{
		m_BeRelationMap.erase(Iter);
		//��������ͻ���ȥ ������� ɾ��һ���ṹ
		CC_Cmd_DelBeUserRelation msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_DelBeUserRelation), sizeof(CC_Cmd_DelBeUserRelation));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwSrcUserID;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeBeUserRelation(DWORD dwDestUserID, BYTE bRelationType)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagBeRoleRelation>::iterator Iter = m_BeRelationMap.find(dwDestUserID);
	if (Iter != m_BeRelationMap.end())
	{
		Iter->second.bRelationType = bRelationType;
		//��������ͻ���
		CC_Cmd_ChangeBeUserRelation msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_ChangeBeUserRelation), sizeof(CC_Cmd_ChangeBeUserRelation));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwDestUserID;
		msg.RelationType = bRelationType;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnRoleOnlinChange(bool IsOnline)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	//��������߻������ߵ�ʱ�� ����֪ͨȫ�������ѵ���� 
	HashMap<DWORD, tagBeRoleRelation>::iterator Iter = m_BeRelationMap.begin();
	for (; Iter != m_BeRelationMap.end(); ++Iter)
	{
		if (Iter->second.bRelationType == RelationType::RT_Friend)
		{
			CenterRole* pRole = m_RoleManager->QueryCenterUser(Iter->second.dwUserID);
			if (pRole)
			{
				pRole->GetRelationManager().OnChangeBeRoleOnlineStateus(m_pRole->GetRoleID(), IsOnline);
			}
		}
	}
}
void RoleRelationManager::OnChangeBeRoleOnlineStateus(DWORD dwUserID, bool IsOnline)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		Iter->second.IsOnline = IsOnline;
		if (IsOnline)
		{
			Iter->second.LastLogonTime = time(NULL);
		}
		CC_Cmd_ChangeUserOline msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_RoleChangeOnline), sizeof(CC_Cmd_ChangeUserOline));
		msg.dwSrcUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.IsOnline = IsOnline;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserLevel(DWORD dwUserID, WORD wLevel)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (Iter->second.wLevel == wLevel)
			return;
		Iter->second.wLevel = wLevel;
		//�������GameServerȥ
		CC_Cmd_ChangeRelationLevel msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_ChangeRelationLevel), sizeof(CC_Cmd_ChangeRelationLevel));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.wLevel = wLevel;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeReltaionUserFaceID(DWORD dwUserID, DWORD FaceID)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (Iter->second.dwFaceID == FaceID)
			return;
		Iter->second.dwFaceID = FaceID;
		//�������GameServerȥ
		CC_Cmd_ChangeRelationFaceID msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_ChangeRelationFaceID), sizeof(CC_Cmd_ChangeRelationFaceID));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.dwFaceID = FaceID;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChanfeRelationUserNickName(DWORD dwUserID, LPTSTR pNickName)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (_tcscmp(pNickName, Iter->second.DestNickName) == 0)
			return;
		TCHARCopy(Iter->second.DestNickName, CountArray(Iter->second.DestNickName), pNickName, _tcslen(pNickName));
		//�������GameServerȥ
		CC_Cmd_ChangeRelationNickName msg;
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_ChangeRelationNickName), sizeof(CC_Cmd_ChangeRelationNickName));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		TCHARCopy(msg.cNickName, CountArray(msg.cNickName), pNickName, _tcslen(pNickName));
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChanfeRelationUserGender(DWORD dwUserID, bool bGender)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (Iter->second.bGender == bGender)
			return;
		Iter->second.bGender = bGender;
		//�������GameServerȥ
		CC_Cmd_ChangeRelationGender msg; 
		SetMsgInfo(msg,GetMsgType(Main_Relation, CC_ChangeRelationGender), sizeof(CC_Cmd_ChangeRelationGender));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.bGender = bGender;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserTitle(DWORD dwUserID, BYTE TitleID)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (Iter->second.TitleID == TitleID)
			return;
		Iter->second.TitleID = TitleID;
		//�������GameServerȥ
		CC_Cmd_ChangeRelationTitle msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationTitle), sizeof(CC_Cmd_ChangeRelationTitle));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.TitleID = TitleID;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserAchievementPoint(DWORD dwUserID, DWORD dwAchievementPoint)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		if (Iter->second.dwAchievementPoint == dwAchievementPoint)
			return;
		Iter->second.dwAchievementPoint = dwAchievementPoint;
		//�������GameServerȥ
		CC_Cmd_ChangeRelationAchievementPoint msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationAchievementPoint), sizeof(CC_Cmd_ChangeRelationAchievementPoint));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.dwAchievementPoint = dwAchievementPoint;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserCharmValue(DWORD dwUserID, DWORD CharmInfo[MAX_CHARM_ITEMSUM])
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{	
		CC_Cmd_ChangeRelationCharmValue msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationCharmValue), sizeof(CC_Cmd_ChangeRelationCharmValue));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		for (int i = 0; i < MAX_CHARM_ITEMSUM; ++i)
		{
			Iter->second.CharmArray[i] = CharmInfo[i];
			msg.CharmInfo[i] = CharmInfo[i];
		}
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserClientIP(DWORD dwUserID, DWORD ClientIP)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		CC_Cmd_ChangeRelationIP msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationIP), sizeof(CC_Cmd_ChangeRelationIP));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.ClientIP = ClientIP;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserIsShowIpAddress(DWORD dwUserID, bool States)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		CC_Cmd_ChangeRelationIsShowIpAddress msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationIsShowIpAddress), sizeof(CC_Cmd_ChangeRelationIsShowIpAddress));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.IsShowIpAddress = States;
		m_pRole->SendDataToGameServer(&msg);
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelationUserVipLevel(DWORD dwUserID, BYTE VipLevel)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		CC_Cmd_ChangeRelationVipLevel msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationVipLevel), sizeof(CC_Cmd_ChangeRelationVipLevel));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.VipLevel = VipLevel;
		return;
	}
	else
	{
		ASSERT(false);
	}
}
void RoleRelationManager::OnChangeRelatuonUserIsInMonthCard(DWORD dwUserID, bool IsInMonthCard)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagRoleRelation>::iterator Iter = m_RelationMap.find(dwUserID);
	if (Iter != m_RelationMap.end())
	{
		CC_Cmd_ChangeRelationIsInMonthCard msg;
		SetMsgInfo(msg, GetMsgType(Main_Relation, CC_ChangeRelationIsInMonthCard), sizeof(CC_Cmd_ChangeRelationIsInMonthCard));
		msg.dwUserID = m_pRole->GetRoleID();
		msg.dwDestUserID = dwUserID;
		msg.IsInMonthCard = IsInMonthCard;
		return;
	}
	else
	{
		ASSERT(false);
	}
}