#include "Stdafx.h"
#include "NiuNiuManager.h"
#include "../CommonFile/DBLogManager.h"
#include "..\FishServer.h"
extern void SendLogDB(NetCmd* pCmd);
#define IsOnceBrandValue(a,b,c,d) a == b && a==c && a==d
NiuNiuManager::NiuNiuManager(DWORD niuniuID) :m_dwNiuNiuID(niuniuID), m_NiuNiuUpdateTime(0), m_NiuNiuUpdateTableInfo(0)
{

}
NiuNiuManager::~NiuNiuManager()
{
	OnDestroy();
}
void NiuNiuManager::SetTableStates(NiuNiuTableStates States)
{
	m_TableStates = States;
	m_TableStatesLog = timeGetTime();
}
bool NiuNiuManager::OnInit()
{
	SetTableStates(NNTS_WriteBegin);//���ó�ʼ����ʱ�� ����״̬Ϊ�ȴ���ʼ
	m_SystemGlobelSum = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.InitGlobelSum;//ϵͳ���
	//��ʼ��һЩ����
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		m_TableBetAreaGlobel[i] = 0;
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		m_TableBrandResult[i] = 0;
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			m_TableBrandArray[i][j] = 0;
	}
	for (BYTE i = 0; i < MAX_VIPSEAT_Sum; ++i)
	{
		m_VipSeatVec[i].dwUserID = 0;
		m_VipSeatVec[i].NonBetGlobelSum = 0;
	}
	m_TableBankerUserID = 0;
	m_TableBankerUseGameSum = 0;
	m_TableWriteBankerVec.clear();
	m_TableUserMap.clear();
	m_IsNeedClearBankerInfo = false;
	return true;
}
void NiuNiuManager::OnUpdate(DWORD dwTimer)
{
	if (m_NiuNiuUpdateTime == 0 || dwTimer - m_NiuNiuUpdateTime >= 1000) //1s����һ��
	{
		m_NiuNiuUpdateTime = dwTimer;
		UpdateTableIsCanBegin(timeGetTime());
		UpdateTableIsCanEnd(timeGetTime());
		UpdateTableInfo(timeGetTime());
		m_oxrobotmanager.Update();
	}
}
void NiuNiuManager::OnDestroy()
{
}
void NiuNiuManager::OnRoleJoinNiuNiuTable(DWORD dwUserID)
{
	//��ҽ�����ͼ����ţţ����
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleJoinTable msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleJoinTable), sizeof(MG_Cmd_RoleJoinTable));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter != m_TableUserMap.end())
	{
		/*ASSERT(false);
		pRole->SendDataToGameServer(&msg);*/

		//����������뿪
		//OnRoleLeaveNiuNiuTable(dwUserID);
		g_FishServer.GetNiuNiuControl().OnRoleLeave(dwUserID);

		//m_TableUserMap.erase(Iter);
	}
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinGlobelSum)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates == NNTS_WriteBegin)
	{
		//��Ϊ���˽����� 
		SetTableStates(NNTS_Begin);
		OnClearTableInfo();//���������ϵ�����
	}

	msg.Result = true;
	tagNiuNiuRoleInfo pInfo;
	pInfo.UserID = dwUserID;
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum;++i)
		pInfo.BetGlobel[i] = 0;
	pInfo.IsBankerUser = false;
	pInfo.IsInWriteBanakerVec = false;
	pInfo.IsInVipSeat = false;
	pInfo.VipSeat = 0;
	pInfo.IsRobot = g_FishServer.GetRobotManager().IsRobot(dwUserID);

	m_TableUserMap.insert(HashMap<DWORD, tagNiuNiuRoleInfo>::value_type(dwUserID,pInfo));
	//�����������ݵ���������
	msg.TableStates = m_TableStates;
	msg.IsNormalRoom = (GetNiuNiuID() == g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID());
	DWORD NowTime = timeGetTime();
	if (m_TableStates == NNTS_Begin)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.BeginWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else if (m_TableStates == NNTS_End)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.EndWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else
		msg.TableStatesUpdateSec = 0;
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		msg.TableBrandResult[i] = m_TableBrandResult[i];
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			msg.TableBrandArray[i][j] = m_TableBrandArray[i][j];
	}
	msg.TableBankerInfo.dwUserID = m_TableBankerUserID;
	msg.TableBankerUseGameSum = m_TableBankerUseGameSum;
	if (msg.TableBankerInfo.dwUserID != 0)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(msg.TableBankerInfo.dwUserID);
		if (!pBankerRole)
		{
			msg.TableBankerInfo.dwUserID = 0;
			msg.TableBankerInfo.dwFaceID = 0;
			msg.TableBankerUseGameSum = 0;
			msg.TableBankerInfo.NickName[0] = 0;
		}
		else
		{
			msg.TableBankerInfo.GlobelSum = pBankerRole->GetGlobelSum();
			msg.TableBankerInfo.dwFaceID = pBankerRole->GetFaceID();
			TCHARCopy(msg.TableBankerInfo.NickName, CountArray(msg.TableBankerInfo.NickName), pBankerRole->GetNickName(), _tcslen(pBankerRole->GetNickName()));
		}
	}
	for (BYTE i = 0; i < MAX_SHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_SHOWBRAND_Sum;)
		{
			Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(*Iter);
			if (!pDestRole)
			{
				//++i;
				++Iter;
				continue;
			}
			msg.TableWriteBankerList[i].dwUserID = *Iter;
			msg.TableWriteBankerList[i].GlobelSum = pDestRole->GetGlobelSum();
			msg.TableWriteBankerList[i].dwFaceID = pDestRole->GetFaceID();
			TCHARCopy(msg.TableWriteBankerList[i].NickName, CountArray(msg.TableWriteBankerList[i].NickName), pDestRole->GetNickName(), _tcslen(pDestRole->GetNickName()));
			++i;
			++Iter;
		}
	}
	//���ϯ������
	for (BYTE i = 0; i < MAX_VIPSEAT_Sum; ++i)
	{
		if (m_VipSeatVec[i].dwUserID == 0)
		{
			msg.VipSeatList[i].dwUserID = 0;
			msg.VipSeatList[i].GlobelSum = 0;
			msg.VipSeatList[i].NickName[0] = 0;
			msg.VipSeatList[i].dwFaceID = 0;
		}
		else
		{
			Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(m_VipSeatVec[i].dwUserID);
			if (pDestRole)
			{
				msg.VipSeatList[i].dwUserID = m_VipSeatVec[i].dwUserID;
				msg.VipSeatList[i].GlobelSum = pDestRole->GetGlobelSum();
				msg.VipSeatList[i].dwFaceID = pDestRole->GetFaceID();
				TCHARCopy(msg.VipSeatList[i].NickName, CountArray(msg.VipSeatList[i].NickName), pDestRole->GetNickName(), _tcslen(pDestRole->GetNickName()));
			}
			else
			{
				msg.VipSeatList[i].dwUserID = 0;
				msg.VipSeatList[i].GlobelSum = 0;
				msg.VipSeatList[i].NickName[0] = 0;
				msg.VipSeatList[i].dwFaceID = 0;
			}
		}
	}
	msg.TableGameSum = m_NiuNiuResultLog.LogSum;
	msg.TableResultLog = m_NiuNiuResultLog.ResultLog;

	pRole->SendDataToGameServer(&msg);
}
void NiuNiuManager::OnRoleLeaveNiuNiuTable(DWORD dwUserID)
{
	//������뿪ţţ���ӵ�ʱ�� 
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		//ASSERT(false);
		return;
	}
	if (Iter->second.IsInWriteBanakerVec)
	{
		vector<DWORD>::iterator IterFind = find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserID);
		if (IterFind != m_TableWriteBankerVec.end())
		{
			m_TableWriteBankerVec.erase(IterFind);
		}
	}
	if (Iter->second.IsInVipSeat && m_VipSeatVec[Iter->second.VipSeat].dwUserID == Iter->first)
	{
		/*m_VipSeatVec[Iter->second.VipSeat].dwUserID = 0;
		m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum = 0;*/
		OnRoleLeaveVipSeat(dwUserID);//ָ������뿪VIPϯ
	}
	if (Iter->second.IsBankerUser && dwUserID == m_TableBankerUserID)
	{
		if (m_TableStates == NNTS_Begin)//���Ϊ��ע�׶�
		{
			//ׯ����ע�� ������ϵͳ����
			m_TableBankerUserID = 0;
			m_TableBankerUseGameSum = 0;
			OnBankerUserChange();//ׯ�ҷ����仯��
		}
		else if (m_TableStates == NNTS_End)
		{
			m_IsNeedClearBankerInfo = true;//�ڿ��ƽ׶� ������ׯ��
		}
	}
	m_TableUserMap.erase(Iter);
}
void NiuNiuManager::OnRoleBetGlobel(DWORD dwUserID, BYTE BetIndex, DWORD AddGlobel)
{
	//�����ע
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleBetGlobel msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleBetGlobel), sizeof(MG_Cmd_RoleBetGlobel));
	msg.dwUserID = dwUserID;
	msg.AddGlobel = AddGlobel;
	msg.Index = BetIndex;
	msg.Result = false;
	if (BetIndex >= MAX_NIUNIU_ClientSum)
	{
		pRole->SendDataToGameServer(&msg);
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates != NNTS_Begin)
	{
		//���ƽ׶� �޷���ע
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableBankerUserID == dwUserID)
	{
		//ׯ�Ҳ�������ע
		pRole->SendDataToGameServer(&msg);
		return;
	}
	bool IsCanBetGlobel = true;

	DWORD RoleBetAllGlobel = 0;
	UINT64 TableBetAllGbel = 0;
	for (BYTE i = 0; i<MAX_NIUNIU_ClientSum; ++i)
	{
		RoleBetAllGlobel += Iter->second.BetGlobel[i];
		TableBetAllGbel += m_TableBetAreaGlobel[i];
	}
	if ((RoleBetAllGlobel + AddGlobel) * g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.MaxRate >(pRole->GetGlobelSum() + RoleBetAllGlobel))//��ҳ������������
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (RoleBetAllGlobel + AddGlobel > g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.MaxAddGlobel)//��ǰ��ע���Ľ��̫��
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//�����Բٳ�ׯ�ҵ����� 
	if (m_TableBankerUserID != 0 && GetNiuNiuID() == g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (pBankerRole && (TableBetAllGbel + AddGlobel)* g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.MaxRate > pBankerRole->GetGlobelSum())
		{
			pRole->SendDataToGameServer(&msg);
			return;
		}
	}

	//������ע��
	msg.Result = true;
	//1.�۳���ҽ��
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - AddGlobel);
	//2.�����ע������
	Iter->second.BetGlobel[BetIndex] += AddGlobel;
	m_TableBetAreaGlobel[BetIndex] += AddGlobel;
	if (!Iter->second.IsRobot)
		g_DBLogManager.LogToDB(dwUserID, LogType::LT_NiuNiu, AddGlobel, BetIndex, TEXT("���ţţ��ע"), SendLogDB);//Param ��ʾ ��ע��
	pRole->SendDataToGameServer(&msg);

	TableBetAllGbel += AddGlobel;
	if (m_TableBankerUserID != 0 && GetNiuNiuID() == g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (pBankerRole && (TableBetAllGbel + 1000)* g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.MaxRate > pBankerRole->GetGlobelSum())
		{
			IsCanBetGlobel = false;
		}
	}

	if (Iter->second.IsInVipSeat)
	{
		MG_Cmd_VipSeatGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_VipSeatGlobelChange), sizeof(MG_Cmd_VipSeatGlobelChange));
		msg.VipSeat = Iter->second.VipSeat;
		msg.GlobelSum = AddGlobel;
		msg.Index = BetIndex;	
		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterRole = m_TableUserMap.begin();
		for (; IterRole != m_TableUserMap.end(); ++IterRole)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterRole->first);
			if (!pRole)
				continue;
			msg.dwUserID = IterRole->first;
			pRole->SendDataToGameServer(&msg);
		}
	}
	if (!IsCanBetGlobel)
	{
		MG_Cmd_TableStopUpdate msgTable;
		SetMsgInfo(msgTable, GetMsgType(Main_NiuNiu, MG_TableStopUpdate), sizeof(MG_Cmd_TableStopUpdate));
		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterRole = m_TableUserMap.begin();
		for (; IterRole != m_TableUserMap.end(); ++IterRole)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterRole->first);
			if (!pRole)
				continue;
			msgTable.dwUserID = IterRole->first;

			pRole->SendDataToGameServer(&msgTable);
		}
	}
}

void NiuNiuManager::OnRoleGetWriteBankerList(DWORD dwUserID)
{
	//��ҽ���ׯ���Ŷ��б�
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	//��������ͻ���ȥ
	MG_Cmd_GetBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_GetBankerList), sizeof(MG_Cmd_GetBankerList));
	msg.dwUserID = dwUserID;
	msg.dwMySeatIndex = 0xffffffff;
	for (BYTE i = 0; i < MAX_SHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_SHOWBRAND_Sum;)
		{
			Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(*Iter);
			if (!pDestRole)
			{
				//++i;
				++Iter;
				continue;
			}
			if (*Iter == dwUserID)
				msg.dwMySeatIndex = i;

			msg.TableWriteBankerList[i].dwUserID = *Iter;
			msg.TableWriteBankerList[i].GlobelSum = pDestRole->GetGlobelSum();
			msg.TableWriteBankerList[i].dwFaceID = pDestRole->GetFaceID();
			TCHARCopy(msg.TableWriteBankerList[i].NickName, CountArray(msg.TableWriteBankerList[i].NickName), pDestRole->GetNickName(), _tcslen(pDestRole->GetNickName()));
			++i;
			++Iter;
		}
	}
	pRole->SendDataToGameServer(&msg);
}
void NiuNiuManager::OnRoleJoinWriteBankerList(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	//�����ͼ������ׯ 
	MG_Cmd_RoleJoinBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleJoinBankerList), sizeof(MG_Cmd_RoleJoinBankerList));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	msg.SeatIndex = 0xffffffff;
	//1.�Ѿ����б�������
	/*vector<DWORD>::iterator IterFind = find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserID);
	if (IterFind != m_TableWriteBankerVec.end())
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}*/
	if (Iter->second.IsInWriteBanakerVec)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//2.�ж���ҽ���Ƿ��㹻
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//3.�ж��Ƿ�Ϊׯ��
	if (dwUserID == m_TableBankerUserID && !m_IsNeedClearBankerInfo)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//������ׯ��
	//
	msg.Result = true;
	msg.SeatIndex = m_TableWriteBankerVec.size();
	Iter->second.IsInWriteBanakerVec = true;
	//�۳����
	//pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum);
	//���뵽�б�����ȥ
	m_TableWriteBankerVec.push_back(dwUserID);//���뵽�б�����ȥ
	//
	if (msg.SeatIndex < MAX_SHOWBRAND_Sum)
	{
		OnWriteBankerChange();
	}
	else
	{
		pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuManager::OnRoleGetWriteBankerFirstSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleGetBankerFirstSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleGetBankerFirstSeat), sizeof(MG_Cmd_RoleGetBankerFirstSeat));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	//1.�����б�����
	if (!Iter->second.IsInWriteBanakerVec)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	vector<DWORD>::iterator IterFind = find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserID);
	if (IterFind == m_TableWriteBankerVec.end())
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//2.�Ƿ�Ϊͷ��
	if (*(m_TableWriteBankerVec.begin()) == dwUserID)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//3.����Ƿ��㹻
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetNextBankerNeedGlobel)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//������ׯ
	msg.Result = true;
	//1.�۳����
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetNextBankerNeedGlobel);
	//2.�ı�λ��
	m_TableWriteBankerVec.erase(IterFind);
	/*vector<DWORD> tempVec;
	tempVec.push_back(dwUserID);
	copy(tempVec.begin() + 1, m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end());
	m_TableWriteBankerVec = tempVec;*/

	m_TableWriteBankerVec.insert(m_TableWriteBankerVec.begin(), dwUserID);

	//3.
	OnWriteBankerChange();//�б�仯��
	pRole->SendDataToGameServer(&msg);
}
void NiuNiuManager::OnRoleLeaveWriteBankerList(DWORD dwUserID)
{
	//����뿪��ׯ�Ŷ��б� 
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleLeaveBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleLeaveBankerList), sizeof(MG_Cmd_RoleLeaveBankerList));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	//1.���ڶ�������
	if (!Iter->second.IsInWriteBanakerVec)
	{
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		return;
	}
	vector<DWORD>::iterator IterFind = find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserID);
	if (IterFind == m_TableWriteBankerVec.end())
	{
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		return;
	}
	//2.������뿪���� ������Ǯ
	//��Ϊ����뿪���� ���Ǵ�����������
	size_t Index = IterFind - m_TableWriteBankerVec.begin();
	if (Index < MAX_SHOWBRAND_Sum)
	{
		OnWriteBankerChange();
	}
	else
	{
		msg.Result = true;
		if (pRole)
			pRole->SendDataToGameServer(&msg);
	}
	m_TableWriteBankerVec.erase(IterFind);//�Ƴ�����
	Iter->second.IsInWriteBanakerVec = false;
	
}
void NiuNiuManager::OnRoleCanelBanker(DWORD dwUserID)
{
	//�����ׯ
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleCanelBankerSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleCanelBankerSeat), sizeof(MG_Cmd_RoleCanelBankerSeat));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	//����ׯ�Ҳ�������ׯ
	if (!Iter->second.IsBankerUser || dwUserID != m_TableBankerUserID)
	{
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		return;
	}
	if (GetNiuNiuID() != g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
	{
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		return;
	}
	//�������ׯ
	Iter->second.IsBankerUser = false;
	msg.Result = true;
	if (m_TableStates == NNTS_Begin)
	{
		//Ӧ������ҵĽ�� ��ʱ����1���� 


		//��ע�׶� �����ׯ ������ϵͳ����
		m_TableBankerUserID = 0;
		m_TableBankerUseGameSum = 0;
		OnBankerUserChange();
	}
	else if (m_TableStates == NNTS_End)
	{
		//���ƽ׶� �����ׯ ֱ����ע ����
		//���������ׯ�ɹ� 
		m_IsNeedClearBankerInfo = true;
	}
	if (pRole)
		pRole->SendDataToGameServer(&msg);
}
void NiuNiuManager::OnRoleLeaveMiniGameServer(DWORD dwUserID)
{
	OnRoleLeaveNiuNiuTable(dwUserID);
}
void NiuNiuManager::OnRoleGlobelChange(DWORD dwUserID,int ChangeSum)//�������û���ֵ��ʱ��
{
	if (m_TableBankerUserID != 0 && m_TableBankerUserID == dwUserID)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
		if (!pBankerRole)
			return;

		MG_Cmd_BankerUserGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_MiniGame, MG_BankerUserGlobelChange), sizeof(MG_Cmd_BankerUserGlobelChange));
		msg.dwBankerGlobelSum = pBankerRole->GetGlobelSum();
		msg.dwBankerUserID = dwUserID;

		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
		for (; Iter != m_TableUserMap.end(); ++Iter)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
			if (!pRole)
				continue;
			msg.dwUserID = Iter->first;
			pRole->SendDataToGameServer(&msg);
		}
	}
	else
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
		if (!pRole)
			return;
		//���ΪVIPϯλ�Ļ�
		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
		if (Iter == m_TableUserMap.end())
			return;
		if (!Iter->second.IsInVipSeat)
			return;
		//��������VIPλ���ϵ�ʱ�� 
		MG_Cmd_VipGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_VipGlobelChange), sizeof(MG_Cmd_VipGlobelChange));
		msg.VipUserID = dwUserID;
		msg.VipGlobelSum = pRole->GetGlobelSum();
		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterRole = m_TableUserMap.begin();
		for (; IterRole != m_TableUserMap.end(); ++IterRole)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterRole->first);
			if (!pRole)
				continue;
			msg.dwUserID = IterRole->first;
			pRole->SendDataToGameServer(&msg);
		}
	}
}
void NiuNiuManager::OnClearTableInfo()
{
	//ÿ�ֿ�ʼ��ע֮ǰ ������������
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		m_TableBetAreaGlobel[i] = 0;
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		m_TableBrandResult[i] = 0;
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			m_TableBrandArray[i][j] = 0;
	}
}
void NiuNiuManager::OnWriteBankerChange()
{
	MG_Cmd_BankerListChange msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_BankerListChange), sizeof(MG_Cmd_BankerListChange));
	//SendDataToAllTableRole(&msg);
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuManager::OnBankerUserChange()
{
	MG_Cmd_BankerUserChange msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_BankerUserChange), sizeof(MG_Cmd_BankerUserChange));
	if (m_TableBankerUserID == 0)
	{
		msg.BankerUserInfo.dwUserID = 0;
		msg.BankerUserInfo.GlobelSum = 0;
		msg.BankerUserInfo.NickName[0] = 0;
		msg.BankerUserInfo.dwFaceID = 0;
		msg.GameSum = 0;
	}
	else
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (!pBankerRole)
		{
			msg.BankerUserInfo.dwUserID = 0;
			msg.BankerUserInfo.GlobelSum = 0;
			msg.BankerUserInfo.NickName[0] = 0;
			msg.BankerUserInfo.dwFaceID = 0;
			msg.GameSum = 0;
		}
		else
		{
			msg.BankerUserInfo.dwUserID = m_TableBankerUserID;
			msg.BankerUserInfo.GlobelSum = pBankerRole->GetGlobelSum();
			TCHARCopy(msg.BankerUserInfo.NickName, CountArray(msg.BankerUserInfo.NickName), pBankerRole->GetNickName(), _tcslen(pBankerRole->GetNickName()));
			msg.BankerUserInfo.dwFaceID = pBankerRole->GetFaceID();
			msg.GameSum = m_TableBankerUseGameSum;
		}
	}
	//SendDataToAllTableRole(&msg);
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuManager::UpdateTableIsCanBegin(DWORD dwTimer)
{
	if (m_TableStates == NNTS_End)
	{
		//���ӵ�ǰ״̬Ϊ ���ƽ׶�
		if (m_TableStatesLog != 0 && dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.EndWriteSec * 1000)
			return;
		//���뿪�ƽ׶��Ѿ���30����
		if (m_TableUserMap.empty())
		{
			SetTableStates(NNTS_WriteBegin);//���ӽ���ȴ���ʼ�׶� ��Ϊû�� �����������
			OnClearTableInfo();
		}
		else
		{
			OnTableJoinBegin();
		}
		return;
	}
	else if (m_TableStates == NNTS_WriteBegin)
	{
		if (m_TableUserMap.empty())
			return;
		OnTableJoinBegin();
		return;
	}
	else
	{
		return;
	}
}
void NiuNiuManager::UpdateTableIsCanEnd(DWORD dwTimer)
{
	if (m_TableStates == NNTS_Begin)
	{
		if (dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.BeginWriteSec * 1000)
			return;
		//������ע�׶�30������ ���Խ��뿪�ƽ׶��� 
		OnTableJoinEnd();
		SetTableStates(NNTS_End);//���ӽ��뿪�ƽ׶�
		return;
	}
	else
	{
		return;//�����׶β�������
	}
}
void NiuNiuManager::UpdateTableInfo(DWORD dwTimer)
{
	if (m_TableStates != NNTS_Begin)
		return;
	if (m_NiuNiuUpdateTableInfo == 0 || dwTimer - m_NiuNiuUpdateTableInfo >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.UpdateSec * 1000)
	{
		m_NiuNiuUpdateTableInfo = dwTimer;
		OnUpdateTableInfo();
	}
}
tagNiuNiuRoleInfo* NiuNiuManager::GetRoleInfo(DWORD dwUserID)
{
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
		return NULL;
	else
		return &Iter->second;
}
void NiuNiuManager::SetNextBankerUserInfo()
{
	if (GetNiuNiuID() != g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
	{
		return;
	}
	//����Ϸ��ʼ��ע��ʱ�� ����������ׯ�ҵ�����
	bool BankerIsChange = false;
	if (m_IsNeedClearBankerInfo)
	{
		tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
		if (pUserInfo)
		{
			pUserInfo->IsBankerUser = false;
		}

		m_TableBankerUserID = 0;
		m_TableBankerUseGameSum = 0;
		m_IsNeedClearBankerInfo = false;
		BankerIsChange = true;
	}
	if (m_TableBankerUserID != 0)
	{
		Role * pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (pBankerRole)
		{
			++m_TableBankerUseGameSum;
			if (m_TableBankerUseGameSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.BankerGameSum)
			{
				tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
				if (pUserInfo)
				{
					pUserInfo->IsBankerUser = false;
				}

				m_TableBankerUserID = 0;
				m_TableBankerUseGameSum = 0;
				BankerIsChange = true;
			}
			else if (pBankerRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum)
			{
				tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
				if (pUserInfo)
				{
					pUserInfo->IsBankerUser = false;
				}

				m_TableBankerUserID = 0;
				m_TableBankerUseGameSum = 0;
				BankerIsChange = true;
			}
		}
		else
		{
			tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
			if (pUserInfo)
			{
				pUserInfo->IsBankerUser = false;
			}

			m_TableBankerUserID = 0;
			m_TableBankerUseGameSum = 0;
			BankerIsChange = true;
		}
	}
	if (m_TableBankerUserID == 0)
	{
		if (!m_TableWriteBankerVec.empty())
		{
			vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
			for (; Iter != m_TableWriteBankerVec.end();)
			{
				Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(*Iter);
				if (!pDestRole || pDestRole->GetGlobelSum() <  g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum)
				{
					tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(*Iter);
					if (pUserInfo)
					{
						pUserInfo->IsInWriteBanakerVec = false;
					}

					Iter = m_TableWriteBankerVec.erase(Iter);
				}
				else
				{
					tagNiuNiuRoleInfo* pUserInfo = GetRoleInfo(*Iter);
					if (pUserInfo)
					{
						//��Ϊ��ҳ�Ϊׯ�� �Զ��뿪VIPϯλ
						if (pUserInfo->IsInVipSeat)
							OnRoleLeaveVipSeat(*Iter);

						pUserInfo->IsInWriteBanakerVec = false;
						pUserInfo->IsBankerUser = true;
					}

					//��Ϊ�Ǵ��б�����ȡ��
					OnWriteBankerChange();

					m_TableBankerUserID = *Iter;
					m_TableBankerUseGameSum = 0;
					m_TableWriteBankerVec.erase(Iter);
					BankerIsChange = true;
					break;
				}
			}
		}
	}
	if (BankerIsChange)
	{
		//��ʼ��ע�׶�ǰ  ׯ�ҷ����仯��
		//if (m_TableBankerUserID != 0)
		//{
		//	Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		//	if (pDestRole)
		//	{
		//		pDestRole->OnChangeRoleGlobelSum(pDestRole->GetGlobelSum() + g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum);//�����ҵĽ��
		//	}
		//}
		OnBankerUserChange();
		BankerIsChange = false;
	}
}
void NiuNiuManager::OnTableJoinBegin()
{
	//�����ӽ�����ע�׶ε�ʱ��
	//1.�ж�ׯ���Ƿ���Ҫ���б仯
	SetNextBankerUserInfo();
	//2.��������ϵ�����
	OnClearTableInfo();
	//3.֪ͨ��� �����µĽ׶���
	SetTableStates(NNTS_Begin);//���ӽ�����ע�׶�
	MG_Cmd_TableJoinBegin msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_TableJoinBegin), sizeof(MG_Cmd_TableJoinBegin));
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
	m_oxrobotmanager.StartGame();
}
void NiuNiuManager::OnUpdateTableInfo()
{
	//���������ϵ�����
	MG_Cmd_TableUpdate msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_TableUpdate), sizeof(MG_Cmd_TableUpdate));
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
INT64 NiuNiuManager::GetBrandUserGlobelChange()
{
	INT64 GlobelChange = 0;
	for (BYTE i = 1; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		if (m_TableBrandResult[i] > m_TableBrandResult[0])
		{
			BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[i]);
			GlobelChange -= (m_TableBetAreaGlobel[i - 1] * Rate);
		}
		else
		{
			BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[0]);
			GlobelChange += (m_TableBetAreaGlobel[i - 1] * Rate);
		}
	}
	return GlobelChange;
}
INT64 NiuNiuManager::GetSystemGlobelChange()
{
	bool IsSystemBranker = false;
	if (m_TableBankerUserID == 0)//ׯ��Ϊϵͳ�Ļ� ���� ׯ��Ϊ�����˵�ʱ��
	{
		IsSystemBranker = true;
	}
	else
	{
		HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(m_TableBankerUserID);
		if (Iter != m_TableUserMap.end() && Iter->second.IsRobot)
		{
			IsSystemBranker = true;
		}
	}

	INT64 GlobelChange = 0;
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		{
			if (Iter->second.BetGlobel[i] == 0)
				continue;
			if (m_TableBrandResult[i+1] > m_TableBrandResult[0])
			{
				BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[i + 1]);
				if (Iter->second.IsRobot)
				{
					if (!IsSystemBranker)
						GlobelChange += (Iter->second.BetGlobel[i] * Rate);
				}
				else
				{
					if (IsSystemBranker)
						GlobelChange -= (Iter->second.BetGlobel[i] * Rate);
				}
			}
			else
			{
				BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[0]);
				if (Iter->second.IsRobot)
				{
					if (!IsSystemBranker)
						GlobelChange -= (Iter->second.BetGlobel[i] * Rate);
				}
				else
				{
					if (IsSystemBranker)
						GlobelChange += (Iter->second.BetGlobel[i] * Rate);
				}
			}
		}
	}
	return GlobelChange;
}
INT64 NiuNiuManager::ChangeBrandValueBySystem(OnceBrandList& pBrand)
{
	//1.�������ϵͳ������ ֱ��ϵͳ����Ǯ
	BYTE Index = 0;
	INT64 SystemGlobel = 0;
	do
	{
		if (m_TableBrandResult[0] != 0)
		{
			for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			{
				pBrand.AddBrand(m_TableBrandArray[0][j]);
				m_TableBrandArray[0][j] = 0;
			}
			m_TableBrandResult[0] = 0;
		}
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
		{
			m_TableBrandArray[0][j] = pBrand.GetNewBrand();//��ȡһ������
		}
		m_TableBrandResult[0] = HandleBrandValue(m_TableBrandArray[0]);
		SystemGlobel = GetSystemGlobelChange();
		++Index;
	} while (SystemGlobel < 0 && Index < 200);//������200����
	return SystemGlobel;
}
void NiuNiuManager::RestAllBrandInfo()
{
	//���ݵ�ǰ�˿� ���Ǵ�����
	//WORD MaxResult = 0;
	//BYTE MaxIndex = 0;
	//for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	//{
	//	if (m_TableBrandResult[i] > MaxResult)
	//	{
	//		MaxResult = m_TableBrandResult[i];
	//		MaxIndex = i;
	//	}
	//}
	//if (MaxIndex != 0)
	//{
	//	//0 �� MaxIndex ���ƽ��н���
	//	for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
	//	{
	//		BYTE tempValue = m_TableBrandArray[0][j];
	//		m_TableBrandArray[0][j] = m_TableBrandArray[MaxIndex][j];
	//		m_TableBrandArray[MaxIndex][j] = tempValue;
	//	}
	//	WORD temp = m_TableBrandResult[0];
	//	m_TableBrandResult[0] = m_TableBrandResult[MaxIndex];
	//	m_TableBrandResult[MaxIndex] = temp;
	//}
	//�̶����� ׯ��Ϊţ2 ����ȫ��Ϊ��ţ
	BYTE result[5][5] =
	{	
		{ 49, 41, 50, 1, 2 },
		{ 5, 33, 9, 13, 21 },
		{ 37, 10, 6, 45, 22 },
		{ 29, 25, 11, 12, 3 },
		{ 51, 38, 23, 24, 17 }
	};

	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
		{
			m_TableBrandArray[i][j] = result[i][j];
		}
		//5���Ʒ�����Ϻ� �����Ƶ�ֵ
		m_TableBrandResult[i] = HandleBrandValue(m_TableBrandArray[i]);
	}
}
void NiuNiuManager::OnTableJoinEnd()
{
	//���������Ƶ�ʱ�� 
	//���н����ʱ�� 
	UINT64 TotalAllAddGlobel = 0;
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		TotalAllAddGlobel += m_TableBetAreaGlobel[i];

	//������ 
	//1.��������ͼ���� ȫ�������� 
	INT64 SystemGlobel = 0;
	INT64 BrandUserGlobel = 0;
	if (g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID() != GetNiuNiuID())
	{
		//��ǰΪ������д����� ������Ҫ�Խ�������޸� ���ñ���Ϊ1 ���� ׯ��һ��ʤ��
		RestAllBrandInfo();//������ ׯ�����
		BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
		SystemGlobel = GetSystemGlobelChange();//����ϵͳ��ұ仯
	}
	else
	{
		OnceBrandList pBrand;//����һ���µ���
		for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
		{
			for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			{
				m_TableBrandArray[i][j] = pBrand.GetNewBrand();//��ȡһ������
			}
			//5���Ʒ�����Ϻ� �����Ƶ�ֵ
			m_TableBrandResult[i] = HandleBrandValue(m_TableBrandArray[i]);
		}

		//��������
		if (g_FishServer.GetOxControl().NeedControl())
		{
			//UINT64 nPlayerJettons[MAX_NIUNIU_ClientSum] = {0};
			g_FishServer.GetOxControl().GetSuitResult(m_TableBrandArray, m_TableBrandResult, m_TableBetAreaGlobel);

			g_FishServer.GetOxControl().CompleteControl();
			for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
			{
				m_TableBrandResult[i] = HandleBrandValue(m_TableBrandArray[i]);
			}
			BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
			SystemGlobel = GetSystemGlobelChange();//����ϵͳ��ұ仯
		}
		else
		{
			//2.�ж� ׯ���Ƿ���Ʋ� �Ʋ��Ļ� ���Ǹı� ׯ�ҵ�����  ֱ�����Ʋ�Ϊֹ
			BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
			SystemGlobel = GetSystemGlobelChange();//����ϵͳ��ұ仯

			if (m_SystemGlobelSum < 0 || (m_SystemGlobelSum + SystemGlobel) < 0)
			{
				//��Ҫ��������
				SystemGlobel = ChangeBrandValueBySystem(pBrand);
				BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
			}
			//if (m_TableBankerUserID == 0)//ׯ��Ϊϵͳ�Ļ� ���� ׯ��Ϊ�����˵�ʱ��
			//{
			//	if (m_SystemGlobelSum < 0 || (m_SystemGlobelSum + SystemGlobel) < 0)
			//	{
			//		SystemGlobel = ChangeBrandValueBySystem(pBrand);
			//		BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
			//	}
			//}
			//else
			//{
			//	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(m_TableBankerUserID);
			//	if (Iter != m_TableUserMap.end() && Iter->second.IsRobot)
			//	{
			//		if (m_SystemGlobelSum < 0 || (m_SystemGlobelSum + SystemGlobel) < 0)
			//		{
			//			SystemGlobel = ChangeBrandValueBySystem(pBrand);
			//			BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
			//		}
			//	}
			//}
		}
	}
	//3.������Ϻ� ���ǽ��д���
	MG_Cmd_TableJoinEnd msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_TableJoinEnd), sizeof(MG_Cmd_TableJoinEnd));
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	for (BYTE i = 0; i < MAX_NIUNIU_ClientSum + 1; ++i)
	{
		msg.TableBrandResult[i] = m_TableBrandResult[i];
		for (BYTE j = 0; j < MAX_NIUNIU_BrandSum; ++j)
			msg.TableBrandArray[i][j] = m_TableBrandArray[i][j];
	}

	m_NiuNiuResultLog.SetResult(m_TableBrandResult);//���ݵ��ֵĽ����������

	msg.TableGameSum = m_NiuNiuResultLog.LogSum;
	msg.TableResultLog = m_NiuNiuResultLog.ResultLog;

	//����ǰ��ҵĽ�ҷ��͵��ͻ���ȥ
	msg.BankerUserGlobelInfo.dwUserID = m_TableBankerUserID;
	if (m_TableBankerUserID != 0)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (pRole)
		{
			//����ׯ�ҵĽ��  Ӧ����֮��Ľ������
			if (pRole->GetGlobelSum() + BrandUserGlobel < 0)
				pRole->OnChangeRoleGlobelSum(0);
			else
				pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() + static_cast<DWORD>(BrandUserGlobel));

			msg.BankerUserGlobelInfo.dwGlobelSum = pRole->GetGlobelSum();
		}
		else
		{
			msg.BankerUserGlobelInfo.dwUserID = 0;
			msg.BankerUserGlobelInfo.dwGlobelSum = 0;
		}
	}
	if (m_TableWriteBankerVec.empty())
	{
		for (BYTE i = 0; i < MAX_SHOWBRAND_Sum; ++i)
		{
			msg.BankerListGlobelInfo[i].dwUserID = 0;
			msg.BankerListGlobelInfo[i].dwGlobelSum = 0;
		}
	}
	else
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_SHOWBRAND_Sum;)
		{
			Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(*Iter);
			if (!pDestRole)
			{
				//++i;
				++Iter;
				continue;
			}
			msg.BankerListGlobelInfo[i].dwUserID = *Iter;
			msg.BankerListGlobelInfo[i].dwGlobelSum = pDestRole->GetGlobelSum();
			++i;
			++Iter;
		}
	}
	for (BYTE i = 0; i < MAX_VIPSEAT_Sum; ++i)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(m_VipSeatVec[i].dwUserID);
		if (pRole)
		{
			msg.VipGlobelInfo[i].dwUserID = pRole->GetRoleID();
			msg.VipGlobelInfo[i].dwGlobelSum = pRole->GetGlobelSum();
		}
		else
		{
			msg.VipGlobelInfo[i].dwUserID = 0;
			msg.VipGlobelInfo[i].dwGlobelSum = 0;
		}
	}
	bool IsChangeSystemGlobel = false;

	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		if (m_TableBankerUserID != 0 && msg.dwUserID == m_TableBankerUserID)
		{
			/*if (pRole->GetGlobelSum() + ResultGlobel < 0)
				pRole->OnChangeRoleGlobelSum(0);
			else
				pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() + static_cast<DWORD>(ResultGlobel));*/

			msg.AddGlobelSum = BrandUserGlobel;

			//OnChangeSystemGlobel(msg.dwUserID, msg.AddGlobelSum);
			if (!Iter->second.IsRobot)
				IsChangeSystemGlobel = true;
		}
		else
		{
			//����ǰ��ҵ�����
			int AddGlobel = 0;
			for (BYTE i = 0; i < MAX_NIUNIU_ClientSum; ++i)
			{
				if (Iter->second.BetGlobel[i] == 0)
					continue;
				//���㵥����ҵĽ�ұ仯
				//����¹�ע
				SHORT Rate = GetBrandRateValue(i);
				AddGlobel += Iter->second.BetGlobel[i] * Rate;//������ҽ�Ҹı�

				Iter->second.BetGlobel[i] = 0;//��������ע�Ľ������
			}
			if (pRole->GetGlobelSum() + AddGlobel < 0)
				pRole->OnChangeRoleGlobelSum(0);
			else
				pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() + AddGlobel);
			msg.AddGlobelSum = AddGlobel;//�����Ӯ��ҵ�����

			//OnChangeSystemGlobel(msg.dwUserID, msg.AddGlobelSum);

			if (AddGlobel == 0 && Iter->second.IsInVipSeat && m_VipSeatVec[Iter->second.VipSeat].dwUserID == Iter->first)
			{
				//��ǰ���δ��ע
				++m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum;
				if (m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.VipNonBetSum)
				{
					//�ж�����Ƿ�Ϊ������� ���ǽ��д���
					if ((pRole->GetRoleInfo().ParticularStates & ParticularState_NiuNiuVip) != 0)
					{
						m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum = 0;
					}
					else
						OnRoleLeaveVipSeat(Iter->first);//����뿪VIPϯλ
				}
			}

			if (!Iter->second.IsRobot && AddGlobel != 0)
				IsChangeSystemGlobel = true;
		}
		pRole->SendDataToGameServer(&msg);
	}

	m_SystemGlobelSum += SystemGlobel;
	//ϵͳ��ҵݼ� ���� ����������ʵ��� ���� ��ʵ�����ʵ��ע��
	if (TotalAllAddGlobel != 0 && IsChangeSystemGlobel) //������ע��ʱ�� ������еݼ�
		m_SystemGlobelSum = static_cast<INT64>(m_SystemGlobelSum * g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetChangeGlobelRateValue());

	if (m_TableBankerUserID != 0)
	{
		if (BrandUserGlobel >= 0)
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_NiuNiu, 1, static_cast<DWORD>(BrandUserGlobel), TEXT("�����ׯ��ý��"), SendLogDB);
		else
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_NiuNiu, 0, static_cast<DWORD>(BrandUserGlobel*-1), TEXT("�����ׯʧȥ���"), SendLogDB);
	}
	//�洢�����ݿ�����ȥ
	g_DBLogManager.LogNiuNiuTableInfoToDB(m_TableBrandArray, SystemGlobel, m_SystemGlobelSum, m_TableUserMap.size(), SendLogDB);

	//�жϵ�ǰ���Ŷ��б������� 
	MG_Cmd_WriteBankerUserGlobelMsg msgSend;
	SetMsgInfo(msgSend, GetMsgType(Main_NiuNiu, MG_WriteBankerUserGlobelMsg), sizeof(MG_Cmd_WriteBankerUserGlobelMsg));
	vector<DWORD>::iterator IterVec = m_TableWriteBankerVec.begin();
	for (; IterVec != m_TableWriteBankerVec.end(); ++IterVec)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(*IterVec);
		if (!pRole)
			continue;
		if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.JoinBankerGlobelSum)
		{
			msgSend.dwUserID = *IterVec;
			pRole->SendDataToGameServer(&msgSend);
		}
	}
	m_oxrobotmanager.EndGame();
}
SHORT NiuNiuManager::GetBrandRateValue(BYTE Index)
{
	if (Index >= MAX_NIUNIU_ClientSum)
		return 1;
	if (m_TableBrandResult[Index + 1] > m_TableBrandResult[0])
	{
		//��ׯ�Ҵ��ʱ��
		BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[Index + 1]);
		//���Ӯׯ�� �˻����� ���㱶��
		Rate += 1;
		return static_cast<SHORT>(Rate);
	}
	else
	{
		//��ׯ��С��ʱ��
		BYTE Rate = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(m_TableBrandResult[0]);
		//ׯ��Ӯ �������һ�α���
		Rate -= 1;
		return static_cast<SHORT>(Rate*-1);
	}
}
WORD NiuNiuManager::HandleBrandValue(BYTE* BrandValue)
{
	if (MAX_NIUNIU_BrandSum != 5)
		return 0;
	//�õ���ǰ�Ƶ�ֵ 
	// ����  ����Ƶ��� ����ƻ�ɫ 
	BrandNiuNiuLevel bLevel = BrandNiuNiuLevel::BNNL_Non;//Ĭ����ţ
	BYTE bv = 0;//�����Ƶ�����
	BYTE HandleBrandValue[MAX_NIUNIU_BrandSum] = { 0 };
	BYTE BrandCardValue[MAX_NIUNIU_BrandSum] = { 0 };//�Ƶ���ֵ
	for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
	{
		HandleBrandValue[i] = GetBrandHandleValue(BrandValue[i]);
		BrandCardValue[i] = static_cast<BYTE>(((BrandValue[i] - 1) / 4) + 1);
		if (bv == 0 || bv < BrandValue[i])
			bv = BrandValue[i];
	}
	//
	//1.��Сţ
	if (BrandCardValue[0] + BrandCardValue[1] + BrandCardValue[2] + BrandCardValue[3] + BrandCardValue[4] < 10)
	{
		//&& BrandCardValue[0] < 5 && BrandCardValue[1] < 5 && BrandCardValue[2] < 5 && BrandCardValue[3] < 5 && BrandCardValue[4] < 5
		bLevel = BrandNiuNiuLevel::BNNL_WXN;//��Сţ
		return ((bLevel << 8) + bv);
	}
	//2.ը��
	if (
		IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[2], BrandCardValue[3]) ||
		IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[2], BrandCardValue[4]) ||
		IsOnceBrandValue(BrandCardValue[0], BrandCardValue[1], BrandCardValue[4], BrandCardValue[3]) ||
		IsOnceBrandValue(BrandCardValue[0], BrandCardValue[4], BrandCardValue[2], BrandCardValue[3]) ||
		IsOnceBrandValue(BrandCardValue[4], BrandCardValue[1], BrandCardValue[2], BrandCardValue[3])
		)
	{
		bLevel = BrandNiuNiuLevel::BNNL_ZD;//ը��
		return ((bLevel << 8) + bv);
	}
	//�����Ƶ�ţţ BUG  Ӧ��ȫ�������ж� ȡ����ֵ
	//�Ƚ���Ҫ���������ݼ�¼����
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
		//������
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
		bLevel = BrandNiuNiuLevel::BNNL_Non;//��ţ
		return ((bLevel << 8) + bv);
	}

	//�����л��ƵĲ���
	if (!m_ChangeMap.empty())
	{
		HashMap<BYTE, BYTE>::iterator Iter = m_ChangeMap.begin();
		for (; Iter != m_ChangeMap.end(); ++Iter)
		{
			ChangeValue(BrandValue[Iter->first], BrandValue[Iter->second]);
		}
		m_ChangeMap.clear();
	}


	switch (OtherValue)
	{
	case 10:
		{
			//�п���Ϊ�廨ţ
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
BYTE NiuNiuManager::GetBrandHandleValue(BYTE Value)
{
	// 1-52
	BYTE Type = (Value - 1) % 4;//0-3  
	BYTE BrandValue = ((Value - 1) / 4) + 1;//0-12 (A-10-K)
	if (BrandValue == 13 || BrandValue == 11 || BrandValue == 12)
		return 10;
	else
		return BrandValue;
}
void NiuNiuManager::ChangeValue(BYTE& A, BYTE& B)
{
	BYTE C = 0;
	C = A;
	A = B;
	B = C;
}
//void NiuNiuManager::OnChangeSystemGlobel(DWORD dwUserID, INT64 ChangeGlobel)
//{
//	if (ChangeGlobel == 0)
//		return;
//	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
//	if (Iter == m_TableUserMap.end())
//	{
//		ASSERT(false);
//		return;
//	}
//	if (Iter->second.IsRobot)
//	{
//		//�����˻�ý��
//		m_SystemGlobelSum += ChangeGlobel;
//		//g_FishServer.ShowInfoToWin("������ ��Ҹı� %d", ChangeGlobel);
//	}
//	else
//	{
//		m_SystemGlobelSum -= ChangeGlobel;
//
//		//g_FishServer.ShowInfoToWin("��ͨ�� ��Ҹı� %d", ChangeGlobel);
//	}
//}
void NiuNiuManager::OnRoleJoinVipSeat(DWORD dwUserID, BYTE VipSeatIndex)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_RoleJoinVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleJoinVipSeat), sizeof(MG_Cmd_RoleJoinVipSeat));
	msg.Result = false;
	msg.DestUserInfo.dwUserID = dwUserID;
	msg.DestUserInfo.dwFaceID = pRole->GetFaceID();
	msg.DestUserInfo.GlobelSum = pRole->GetGlobelSum();
	TCHARCopy(msg.DestUserInfo.NickName, CountArray(msg.DestUserInfo.NickName), pRole->GetNickName(), _tcslen(pRole->GetNickName()));
	msg.dwUserID = dwUserID;
	msg.VipSeatIndex = VipSeatIndex;
	if (Iter->second.IsBankerUser)//ׯ�Ҳ�������VIPϯλ
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (Iter->second.IsInVipSeat)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (pRole->GetVipLevel() == 0)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (VipSeatIndex >= MAX_VIPSEAT_Sum)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_VipSeatVec[VipSeatIndex].dwUserID != 0)
	{
		Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(m_VipSeatVec[VipSeatIndex].dwUserID);
		if (pDestRole)
		{
			if (pDestRole->GetVipLevel() >= pRole->GetVipLevel())
			{
				pRole->SendDataToGameServer(&msg);
				return;
			}
			else
			{
				//����Ŀ����� ������
				MG_Cmd_RoleBeLeaveVipSeat msgLeave;
				SetMsgInfo(msgLeave, GetMsgType(Main_NiuNiu, MG_RoleBeLeaveVipSeat), sizeof(MG_Cmd_RoleBeLeaveVipSeat));
				msgLeave.dwUserID = m_VipSeatVec[VipSeatIndex].dwUserID;
				msgLeave.DestRoleInfo.dwUserID = dwUserID;
				msgLeave.DestRoleInfo.dwFaceID = pRole->GetFaceID();
				msgLeave.DestRoleInfo.GlobelSum = pRole->GetGlobelSum();
				TCHARCopy(msgLeave.DestRoleInfo.NickName, CountArray(msgLeave.DestRoleInfo.NickName), pRole->GetNickName(), _tcslen(pRole->GetNickName()));
				pDestRole->SendDataToGameServer(&msgLeave);
			}
		}
	}
	//��ҿ�������Vipϯλ
	m_VipSeatVec[VipSeatIndex].dwUserID = dwUserID;
	m_VipSeatVec[VipSeatIndex].NonBetGlobelSum = 0;
	Iter->second.IsInVipSeat = true;
	Iter->second.VipSeat = VipSeatIndex;

	msg.Result = true;
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterSend = m_TableUserMap.begin();//Vipϯλ�ĸı䷢�͸�ȫ�����
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuManager::OnRoleLeaveVipSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	if (Iter->second.IsBankerUser)
	{
		ASSERT(false);
		return;
	}
	if (!Iter->second.IsInVipSeat)
	{
		ASSERT(false);
		return;
	}
	if (m_VipSeatVec[Iter->second.VipSeat].dwUserID != dwUserID)
	{
		ASSERT(false);
		return;
	}

	m_VipSeatVec[Iter->second.VipSeat].dwUserID = 0;
	m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum = 0;

	Iter->second.IsInVipSeat = false;
	Iter->second.VipSeat = 0;

	MG_Cmd_RoleLeaveVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_RoleLeaveVipSeat), sizeof(MG_Cmd_RoleLeaveVipSeat));
	msg.dwDestUserID = dwUserID;
	msg.VipSeatIndex = Iter->second.VipSeat;
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterSend = m_TableUserMap.begin();
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void NiuNiuManager::OnRoleGetNormalInfoByPage(DWORD dwUserID, DWORD Page)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator IterRole = m_TableUserMap.find(dwUserID);
	if (IterRole == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}

	DWORD BeginIndex = Page * MAX_NORMAL_PAGESUM;
	DWORD EndIndex = (Page + 1)* MAX_NORMAL_PAGESUM - 1;


	MG_Cmd_GetNormalSeatInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_NiuNiu, MG_GetNormalSeatInfo), sizeof(MG_Cmd_GetNormalSeatInfo));
	msg.dwUserID = dwUserID;
	msg.Page = Page;
	msg.TotalRoleSum = m_TableUserMap.size();

	for (BYTE i = 0; i < MAX_VIPSEAT_Sum; ++i)
	{
		if (m_VipSeatVec[i].dwUserID != 0)
			--msg.TotalRoleSum;
	}
	if (m_TableBankerUserID != 0 && GetNiuNiuID() == g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
		--msg.TotalRoleSum;

	for (BYTE i = 0; i < MAX_NORMAL_PAGESUM; ++i)
	{
		msg.Array[i].dwFaceID = 0;
		msg.Array[i].dwUserID = 0;
		msg.Array[i].GlobelSum = 0;
		msg.Array[i].NickName[0] = 0;
	}
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (DWORD Index = 0; Iter != m_TableUserMap.end(); ++Iter)
	{
		if (Iter->second.IsInVipSeat || Iter->second.IsBankerUser)
			continue;
		Role* pDestRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pDestRole)
			continue;
		if (Index >= BeginIndex)
		{
			//����Ҽ�¼����������ȥ
			msg.Array[Index - BeginIndex].dwUserID = pDestRole->GetRoleID();
			msg.Array[Index - BeginIndex].dwFaceID = pDestRole->GetFaceID();
			msg.Array[Index - BeginIndex].GlobelSum = pDestRole->GetGlobelSum();
			TCHARCopy(msg.Array[Index - BeginIndex].NickName, CountArray(msg.Array[Index - BeginIndex].NickName), pDestRole->GetNickName(), _tcslen(pDestRole->GetNickName()));

			if (Index == EndIndex)
			{
				pRole->SendDataToGameServer(&msg);
				return;
			}
		}
		++Index;
	}
	//Ŀǰ��û��׼���� 
	pRole->SendDataToGameServer(&msg);//�������� ���� ����Ϊ�յ�
	return;
}

void NiuNiuManager::QueryNames(DWORD ClientID)
{
	int nNamelength = (MAX_NICKNAME+1);
	int nSize = sizeof(LC_CMD_OxAdmin_PlayerNames)+sizeof(TCHAR)*nNamelength*(m_TableUserMap.size() - m_oxrobotmanager.RobotCount());
	LC_CMD_OxAdmin_PlayerNames *pResult = (LC_CMD_OxAdmin_PlayerNames*)malloc(nSize);
	pResult->SetCmdSize(nSize);
	pResult->SetCmdType((Main_Control << 8) | LC_OxAdminQueryName);
	pResult->ClientID = ClientID;
	int nIndex = 0;
	for (HashMap<DWORD, tagNiuNiuRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end();it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole&&!m_oxrobotmanager.IsRobot(pRole->GetRoleID()))
		{
			TCHARCopy(&pResult->names[1 + nIndex*nNamelength], nNamelength, pRole->GetNickName(), MAX_NICKNAME);
			nIndex++;
		}
	}
	g_FishServer.SendNetCmdToControl(pResult);
	free(pResult);	
}
void NiuNiuManager::QueryAllJetton(DWORD ClientID)
{
	LC_CMD_OxAdmin_AllJetton result;
	ZeroMemory(&result, sizeof(result));
	result.SetCmdSize(sizeof(result));
	result.SetCmdType((Main_Control << 8) | LC_OxAdminQueryAllJetton);
	result.ClientID = ClientID;
	result.nStock = m_SystemGlobelSum;
	CalPlayerJetton(result.nJetton);
	g_FishServer.SendNetCmdToControl(&result);

}
void NiuNiuManager::QueryPlayerJetton(TCHAR *pNickame,DWORD ClientID)
{
	for (HashMap<DWORD, tagNiuNiuRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole)
		{
			if (_tcscmp(pNickame, pRole->GetNickName()) == 0)
			{
				HashMap<DWORD, tagNiuNiuRoleInfo>::iterator itJetton = m_TableUserMap.find(pRole->GetRoleID());
				if (itJetton != m_TableUserMap.end())
				{
					LC_CMD_OxAdmin_PlayerJetton result;
					result.SetCmdSize(sizeof(result));
					result.SetCmdType((Main_Control << 8) | LC_OxAdminQueryPlayerJetton);
					result.ClientID = ClientID;
					//TCHARCopy(result.name, MAX_NICKNAME + 1, pNickame, MAX_NICKNAME);
					memcpy(result.dwJetton, itJetton->second.BetGlobel, sizeof(result.dwJetton));				
					g_FishServer.SendNetCmdToControl(&result);
					break;
				}
			}
		}
	}

}

DWORD NiuNiuManager::BankerId()
{
	return m_TableBankerUserID;
}

WORD NiuNiuManager::ApplyListLength()
{
	return m_TableWriteBankerVec.size();
}

DWORD NiuNiuManager::BankerGold()
{
	Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
	if (pBankerRole)
	{
		return pBankerRole->GetGlobelSum();
	}
	return 0;
}
bool	NiuNiuManager::IsGameEnd()
{
	return m_TableStates == NNTS_End;
}

bool	NiuNiuManager::HaveApply(DWORD dwUserid)
{
	return find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserid) != m_TableWriteBankerVec.end();
}
void NiuNiuManager::CalPlayerJetton(UINT64 uJetton[MAX_NIUNIU_ClientSum])
{
	for (HashMap<DWORD, tagNiuNiuRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		if (!m_oxrobotmanager.IsRobot(it->first))
		{
			for (int i = 0; i < MAX_NIUNIU_ClientSum; i++)
			{
				uJetton[i] += it->second.BetGlobel[i];
			}
		}
	}
}
bool NiuNiuManager::IsExitsNormalRole()
{
	HashMap<DWORD, tagNiuNiuRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		if (!Iter->second.IsRobot)
			return true;
	}
	return false;
}
void NiuNiuManager::SetBrandInfo(DWORD dwUserID)
{
	if (GetNiuNiuID() == g_FishServer.GetNiuNiuControl().GetNiuNiuNormalID())
		return;
	m_TableBankerUserID = dwUserID;
	m_TableBankerUseGameSum = 250;
}