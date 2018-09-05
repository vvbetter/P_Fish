#include "Stdafx.h"
#include "DialManager.h"
#include "../CommonFile/DBLogManager.h"
#include "FishServer.h"
extern void SendLogDB(NetCmd* pCmd);
DialManager::DialManager()
{

}
DialManager::~DialManager()
{
	OnDestroy();
}
void DialManager::SetTableStates(DialTableStates States)
{
	m_TableStates = States;
	m_TableStatesLog = timeGetTime();
}
bool DialManager::OnInit()
{
	SetTableStates(DTS_WriteBegin);//���ó�ʼ����ʱ�� ����״̬Ϊ�ȴ���ʼ
	m_SystemGlobelSum = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.InitGlobelSum;//ϵͳ���
	//��ʼ��һЩ����
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		m_TableBetAreaGlobel[i] = 0;
		m_TableAreaRate[i] = 0;
	}
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		m_TableAreaData[i] = 0;
	}
	for (BYTE i = 0; i < MAX_DIALVIPSEAT_Sum; ++i)
	{
		m_VipSeatVec[i].dwUserID = 0;
		m_VipSeatVec[i].NonBetGlobelSum = 0;
	}
	m_TableBankerUserID = 0;
	m_TableBankerUseGameSum = 0;
	m_TableResult = 0;
	m_TableWriteBankerVec.clear();
	m_TableUserMap.clear();
	m_IsNeedClearBankerInfo = false;
	return true;
}
void DialManager::OnUpdate(DWORD dwTimer)
{
	static DWORD DialUpdateTime = 0;
	if (DialUpdateTime == 0 || dwTimer - DialUpdateTime >= 1000) //1s����һ��
	{
		DialUpdateTime = dwTimer;
		UpdateTableIsCanBegin(timeGetTime());
		UpdateTableIsCanEnd(timeGetTime());
		UpdateTableInfo(timeGetTime());
		m_animalrobotmanager.Update();
	}
}
void DialManager::OnDestroy()
{
}
void DialManager::OnRoleJoinDialTable(DWORD dwUserID)
{
	//��ҽ�����ͼ����ţţ����
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleJoinTable msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleJoinTable), sizeof(MG_Cmd_DialRoleJoinTable));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter != m_TableUserMap.end())
	{
		//����������뿪
		OnRoleLeaveDialTable(dwUserID);
	}
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.JoinGlobelSum)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates == DTS_WriteBegin)
	{
		//��Ϊ���˽����� 
		SetTableStates(DTS_Begin);
		OnClearTableInfo();//���������ϵ�����
		//CreateNewDial();//�����µ�����
		OnInitDial();//����ǵ�һ�εĻ� ��ʼ����������
	}
	msg.Result = true;
	tagDialRoleInfo pInfo;
	pInfo.UserID = dwUserID;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		pInfo.BetGlobel[i] = 0;
	pInfo.IsBankerUser = false;
	pInfo.IsInWriteBanakerVec = false;
	pInfo.IsInVipSeat = false;
	pInfo.VipSeat = 0;
	pInfo.IsRobot = g_FishServer.GetRobotManager().IsRobot(dwUserID);

	m_TableUserMap.insert(HashMap<DWORD, tagDialRoleInfo>::value_type(dwUserID, pInfo));
	//�����������ݵ���������
	msg.TableStates = m_TableStates;
	DWORD NowTime = timeGetTime();
	if (m_TableStates == DTS_Begin)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.BeginWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else if (m_TableStates == DTS_End)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.EndWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else
		msg.TableStatesUpdateSec = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		msg.TableAreaDataRate[i] = m_TableAreaRate[i];
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	}
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		msg.TableAreaData[i] = m_TableAreaData[i];
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
	for (BYTE i = 0; i < MAX_DIALSHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_DIALSHOWBRAND_Sum;)
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
	for (BYTE i = 0; i < MAX_DIALVIPSEAT_Sum; ++i)
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
	msg.TableGameSum = m_DialResultLog.LogSum;
	msg.TableResultLog = m_DialResultLog.ResultLog;
	msg.TableResultIndex = m_TableResult;
	pRole->SendDataToGameServer(&msg);
}
void DialManager::OnRoleLeaveDialTable(DWORD dwUserID)
{
	//������뿪ţţ���ӵ�ʱ�� 
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
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
		OnRoleLeaveVipSeat(dwUserID);//ָ������뿪VIPϯ
	}
	if (Iter->second.IsBankerUser && dwUserID == m_TableBankerUserID)
	{
		if (m_TableStates == DTS_Begin)//���Ϊ��ע�׶�
		{
			//ׯ����ע�� ������ϵͳ����
			m_TableBankerUserID = 0;
			m_TableBankerUseGameSum = 0;
			OnBankerUserChange();//ׯ�ҷ����仯��
		}
		else if (m_TableStates == DTS_End)
		{
			m_IsNeedClearBankerInfo = true;//�ڿ��ƽ׶� ������ׯ��
		}
	}
	m_TableUserMap.erase(Iter);
}
bool DialManager::IsCanAddGlobel(DWORD AddGlobel, BYTE Index)
{
	UINT64 MaxGlobel = 0;
	for (BYTE i = 0; i <MAX_DIAL_ClientSum; ++i)
	{
		UINT64 Globel = m_TableBetAreaGlobel[i];
		if (i == Index)
		{
			Globel += AddGlobel;
		}
		Globel = m_TableAreaRate[i] * Globel;
		if (Globel >= MaxGlobel)
			MaxGlobel = Globel;
	}
	if (m_TableBankerUserID != 0)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (!pBankerRole)
			return true;
		if (MaxGlobel > pBankerRole->GetGlobelSum())
			return false;
		else
			return true;
	}
	else
		return true;
}
void DialManager::OnRoleBetGlobelByLog(DWORD dwUserID, DWORD AddGlobelArray[MAX_DIAL_ClientSum])
{
	//��ҽ��м���Ѻע
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	

	MG_Cmd_DialRoleBetGlobelByLog msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleBetGlobelByLog), sizeof(MG_Cmd_DialRoleBetGlobelByLog));
	msg.dwUserID = dwUserID;

	DWORD AllGlobel = 0; // ��ע���ܽ������
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		AllGlobel += AddGlobelArray[i];
		msg.betGlobel[i] = AddGlobelArray[i];
	}

	msg.AllGlobel = AllGlobel;
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (pRole->GetGlobelSum() < AllGlobel)
	{
		ASSERT(false);
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates != DTS_Begin)
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

	DWORD RoleBetAllGlobel = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		RoleBetAllGlobel += Iter->second.BetGlobel[i];
	}

	if (RoleBetAllGlobel + AllGlobel> g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.MaxAddGlobel)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}

	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		if (!IsCanAddGlobel(AddGlobelArray[i], i))
		{
			pRole->SendDataToGameServer(&msg);
			return;
		}
	}

	//������ע��
	msg.Result = true;
	//1.�۳���ҽ��
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - AllGlobel);
	//2.�����ע������
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		Iter->second.BetGlobel[i] += AddGlobelArray[i];
		m_TableBetAreaGlobel[i] += AddGlobelArray[i];

		if (!Iter->second.IsRobot)
			g_DBLogManager.LogToDB(dwUserID, LogType::LT_Dial, AddGlobelArray[i], i, TEXT("���ת����ע"), SendLogDB);//Param ��ʾ ��ע��
	}
	pRole->SendDataToGameServer(&msg);

	if (Iter->second.IsInVipSeat)
	{
		for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			if (AddGlobelArray[i] == 0)
				continue;
			MG_Cmd_DialVipSeatGlobelChange msg;
			SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialVipSeatGlobelChange), sizeof(MG_Cmd_DialVipSeatGlobelChange));
			msg.VipSeat = Iter->second.VipSeat;
			msg.GlobelSum = AddGlobelArray[i];
			msg.Index = i;
			HashMap<DWORD, tagDialRoleInfo>::iterator IterRole = m_TableUserMap.begin();
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
}
void DialManager::OnRoleBetGlobel(DWORD dwUserID, BYTE BetIndex, DWORD AddGlobel)
{
	//�����ע
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleBetGlobel msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleBetGlobel), sizeof(MG_Cmd_DialRoleBetGlobel));
	msg.dwUserID = dwUserID;
	msg.AddGlobel = AddGlobel;
	msg.Index = BetIndex;
	msg.Result = false;
	if (BetIndex >= MAX_DIAL_ClientSum)
	{
		pRole->SendDataToGameServer(&msg);
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (pRole->GetGlobelSum() < AddGlobel)
	{
		ASSERT(false);
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates != DTS_Begin)
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
	for (BYTE i = 0; i<MAX_DIAL_ClientSum; ++i)
	{
		RoleBetAllGlobel += Iter->second.BetGlobel[i];
		TableBetAllGbel += m_TableBetAreaGlobel[i];
	}
	if (RoleBetAllGlobel + AddGlobel > g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.MaxAddGlobel)//��ǰ��ע���Ľ��̫��
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//�����Բٳ�ׯ�ҵ����� 
	if (!IsCanAddGlobel(AddGlobel, BetIndex))
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}

	//������ע��
	msg.Result = true;
	//1.�۳���ҽ��
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - AddGlobel);
	//2.�����ע������
	Iter->second.BetGlobel[BetIndex] += AddGlobel;
	m_TableBetAreaGlobel[BetIndex] += AddGlobel;
	if (!Iter->second.IsRobot)
		g_DBLogManager.LogToDB(dwUserID, LogType::LT_Dial, AddGlobel, BetIndex, TEXT("���ת����ע"), SendLogDB);//Param ��ʾ ��ע��
	pRole->SendDataToGameServer(&msg);


	/*TableBetAllGbel += AddGlobel;
	if (m_TableBankerUserID != 0)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
		if (pBankerRole && (TableBetAllGbel + 1000)* m_NowMaxRate > pBankerRole->GetGlobelSum())
		{
			IsCanBetGlobel = false;
		}
	}*/

	if (Iter->second.IsInVipSeat)
	{
		MG_Cmd_DialVipSeatGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialVipSeatGlobelChange), sizeof(MG_Cmd_DialVipSeatGlobelChange));
		msg.VipSeat = Iter->second.VipSeat;
		msg.GlobelSum = AddGlobel;
		msg.Index = BetIndex;
		HashMap<DWORD, tagDialRoleInfo>::iterator IterRole = m_TableUserMap.begin();
		for (; IterRole != m_TableUserMap.end(); ++IterRole)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterRole->first);
			if (!pRole)
				continue;
			msg.dwUserID = IterRole->first;
			pRole->SendDataToGameServer(&msg);
		}
	}
	/*if (!IsCanBetGlobel)
	{
		MG_Cmd_DialTableStopUpdate msgTable;
		SetMsgInfo(msgTable, GetMsgType(Main_Dial, MG_DialTableStopUpdate), sizeof(MG_Cmd_DialTableStopUpdate));
		HashMap<DWORD, tagDialRoleInfo>::iterator IterRole = m_TableUserMap.begin();
		for (; IterRole != m_TableUserMap.end(); ++IterRole)
		{
			Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterRole->first);
			if (!pRole)
				continue;
			msgTable.dwUserID = IterRole->first;

			pRole->SendDataToGameServer(&msgTable);
		}
	}*/
}
void DialManager::OnRoleGetWriteBankerList(DWORD dwUserID)
{
	//��ҽ���ׯ���Ŷ��б�
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	//��������ͻ���ȥ
	MG_Cmd_DialGetBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialGetBankerList), sizeof(MG_Cmd_DialGetBankerList));
	msg.dwUserID = dwUserID;
	msg.dwMySeatIndex = 0xffffffff;
	for (BYTE i = 0; i < MAX_DIALSHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_DIALSHOWBRAND_Sum;)
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
void DialManager::OnRoleJoinWriteBankerList(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	//�����ͼ������ׯ 
	MG_Cmd_DialRoleJoinBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleJoinBankerList), sizeof(MG_Cmd_DialRoleJoinBankerList));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	msg.SeatIndex = 0xffffffff;
	//1.�Ѿ����б�������
	if (Iter->second.IsInWriteBanakerVec)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//2.�ж���ҽ���Ƿ��㹻
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.JoinBankerGlobelSum)
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
	if (msg.SeatIndex < MAX_DIALSHOWBRAND_Sum)
	{
		OnWriteBankerChange();
	}
	else
	{
		pRole->SendDataToGameServer(&msg);
	}
}
void DialManager::OnRoleGetWriteBankerFirstSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleGetBankerFirstSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleGetBankerFirstSeat), sizeof(MG_Cmd_DialRoleGetBankerFirstSeat));
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
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.GetNextBankerNeedGlobel)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//������ׯ
	msg.Result = true;
	//1.�۳����
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.GetNextBankerNeedGlobel);
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
void DialManager::OnRoleLeaveWriteBankerList(DWORD dwUserID)
{
	//����뿪��ׯ�Ŷ��б� 
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleLeaveBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleLeaveBankerList), sizeof(MG_Cmd_DialRoleLeaveBankerList));
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
	if (Index < MAX_DIALSHOWBRAND_Sum)
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
void DialManager::OnRoleCanelBanker(DWORD dwUserID)
{
	//�����ׯ
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleCanelBankerSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleCanelBankerSeat), sizeof(MG_Cmd_DialRoleCanelBankerSeat));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	//����ׯ�Ҳ�������ׯ
	if (!Iter->second.IsBankerUser || dwUserID != m_TableBankerUserID)
	{
		if (pRole)
			pRole->SendDataToGameServer(&msg);
		return;
	}
	//�������ׯ
	Iter->second.IsBankerUser = false;
	msg.Result = true;
	if (m_TableStates == DTS_Begin)
	{
		//Ӧ������ҵĽ�� ��ʱ����1���� 


		//��ע�׶� �����ׯ ������ϵͳ����
		m_TableBankerUserID = 0;
		m_TableBankerUseGameSum = 0;
		OnBankerUserChange();
	}
	else if (m_TableStates == DTS_End)
	{
		//���ƽ׶� �����ׯ ֱ����ע ����
		//���������ׯ�ɹ� 
		m_IsNeedClearBankerInfo = true;
	}
	if (pRole)
		pRole->SendDataToGameServer(&msg);
}
void DialManager::OnRoleLeaveMiniGameServer(DWORD dwUserID)
{
	OnRoleLeaveDialTable(dwUserID);
}
void DialManager::OnRoleGlobelChange(DWORD dwUserID, int ChangeSum)//�������û���ֵ��ʱ��
{
	if (m_TableBankerUserID != 0 && m_TableBankerUserID == dwUserID)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
		if (!pBankerRole)
			return;

		MG_Cmd_DialBankerUserGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialBankerUserGlobelChange), sizeof(MG_Cmd_DialBankerUserGlobelChange));
		msg.dwBankerGlobelSum = pBankerRole->GetGlobelSum();
		msg.dwBankerUserID = dwUserID;

		HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
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
		HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
		if (Iter == m_TableUserMap.end())
			return;
		if (!Iter->second.IsInVipSeat)
			return;
		//��������VIPλ���ϵ�ʱ�� 
		MG_Cmd_DialVipGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialVipGlobelChange), sizeof(MG_Cmd_DialVipGlobelChange));
		msg.VipUserID = dwUserID;
		msg.VipGlobelSum = pRole->GetGlobelSum();
		HashMap<DWORD, tagDialRoleInfo>::iterator IterRole = m_TableUserMap.begin();
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
void DialManager::OnClearTableInfo()
{
	//ÿ�ֿ�ʼ��ע֮ǰ ������������
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		m_TableBetAreaGlobel[i] = 0;
		//m_TableAreaRate[i] = 0;
	}
	/*for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		m_TableAreaData[i] = 0;
	}*/
}
void DialManager::OnWriteBankerChange()
{
	MG_Cmd_DialBankerListChange msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialBankerListChange), sizeof(MG_Cmd_DialBankerListChange));
	//SendDataToAllTableRole(&msg);
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void DialManager::OnBankerUserChange()
{
	MG_Cmd_DialBankerUserChange msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialBankerUserChange), sizeof(MG_Cmd_DialBankerUserChange));
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
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void DialManager::UpdateTableIsCanBegin(DWORD dwTimer)
{
	if (m_TableStates == DTS_End)
	{
		//���ӵ�ǰ״̬Ϊ ���ƽ׶�
		if (m_TableStatesLog != 0 && dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.EndWriteSec * 1000)
			return;
		//���뿪�ƽ׶��Ѿ���30����
		if (m_TableUserMap.empty())
		{
			SetTableStates(DTS_WriteBegin);//���ӽ���ȴ���ʼ�׶� ��Ϊû�� �����������
			OnClearTableInfo();
		}
		else
		{
			OnTableJoinBegin();
		}
		return;
	}
	else if (m_TableStates == DTS_WriteBegin)
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
void DialManager::UpdateTableIsCanEnd(DWORD dwTimer)
{
	if (m_TableStates == DTS_Begin)
	{
		if (dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.BeginWriteSec * 1000)
			return;
		//������ע�׶�30������ ���Խ��뿪�ƽ׶��� 
		OnTableJoinEnd();
		SetTableStates(DTS_End);//���ӽ��뿪�ƽ׶�
		return;
	}
	else
	{
		return;//�����׶β�������
	}
}
void DialManager::UpdateTableInfo(DWORD dwTimer)
{
	if (m_TableStates != DTS_Begin)
		return;
	static DWORD DialUpdateTableInfo = 0;
	if (DialUpdateTableInfo == 0 || dwTimer - DialUpdateTableInfo >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.UpdateSec * 1000)
	{
		DialUpdateTableInfo = dwTimer;
		OnUpdateTableInfo();
	}
}
tagDialRoleInfo* DialManager::GetRoleInfo(DWORD dwUserID)
{
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
		return NULL;
	else
		return &Iter->second;
}
void DialManager::SetNextBankerUserInfo()
{
	//����Ϸ��ʼ��ע��ʱ�� ����������ׯ�ҵ�����
	bool BankerIsChange = false;
	if (m_IsNeedClearBankerInfo)
	{
		tagDialRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
			if (m_TableBankerUseGameSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.BankerGameSum)
			{
				tagDialRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
				if (pUserInfo)
				{
					pUserInfo->IsBankerUser = false;
				}

				m_TableBankerUserID = 0;
				m_TableBankerUseGameSum = 0;
				BankerIsChange = true;
			}
			else if (pBankerRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.JoinBankerGlobelSum)
			{
				tagDialRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
			tagDialRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
				if (!pDestRole || pDestRole->GetGlobelSum() <  g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.JoinBankerGlobelSum)
				{
					tagDialRoleInfo* pUserInfo = GetRoleInfo(*Iter);
					if (pUserInfo)
					{
						pUserInfo->IsInWriteBanakerVec = false;
					}

					Iter = m_TableWriteBankerVec.erase(Iter);
				}
				else
				{
					tagDialRoleInfo* pUserInfo = GetRoleInfo(*Iter);
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
		OnBankerUserChange();
		BankerIsChange = false;
	}
}
void DialManager::OnInitDial()
{
	if (m_TableResult == 0)
	{
		for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			m_TableAreaRate[i] = 0;
		}
		//��һ����������
		for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
		{
			BYTE Value = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[i];//0-3 ��Ȧ����
			BYTE Value2 = RandUInt() % 3;//0-2 ��Ȧ����
			BYTE Result = Value2 * 4 + Value;
			m_TableAreaData[i] = Result;
			m_TableAreaRate[Result] += 1;
		}

		for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			if (m_TableAreaRate[i] == 0)
			{
				//��ǰѡ����Ҫ�������
				BYTE Value = i / 4;//0-2 ��ɫ
				BYTE Rand = i % 4;//0-3 ����
				for (BYTE j = 0; j < MAX_DIAL_ClientSum; ++j)
				{
					if (m_TableAreaRate[j] > 1 && j % 4 == Rand)
					{
						//�޸�����
						for (BYTE k = 0; k < MAX_DIAL_GameSum; ++k)
						{
							if (m_TableAreaData[k] == j)
							{
								m_TableAreaData[k] = i;
								m_TableAreaRate[j] -= 1;
								m_TableAreaRate[i] += 1;
								break;
							}
						}
						break;
					}
				}
			}
		}

		BYTE ID = static_cast<BYTE>(RandUInt() % g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RateMap.size());
		for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			m_TableAreaRate[i] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RateMap[ID].RateArray[i];
		}

		/*for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			BYTE Rate = MAX_DIAL_GameSum / m_TableAreaRate[i];
			m_TableAreaRate[i] = Rate;
		}*/

		/*g_FishServer.ShowInfoToWin("��ʼ������");

		{
			string str = "";
			for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
			{
				BYTE Result = m_TableAreaData[i] / 4;
				BYTE Value = m_TableAreaData[i] % 4;
				string strValue = (Value == 0 ? "��è" : (Value == 1 ? "����" : (Value == 2 ? "����" : "ʨ��")));
				str += strValue;
				if (i != MAX_DIAL_GameSum - 1)
					str += ",";
			}
			g_FishServer.ShowInfoToWin(str.c_str());
		}*/
	}
}
void DialManager::OnMoveDialByResult()
{
	//ÿ�ֽ�����ʱ�� ˢ����������
	BYTE Index1 = (m_TableResult >> 8);
	BYTE Index2 = static_cast<BYTE>(m_TableResult);
	BYTE Array1[MAX_DIAL_GameSum] = { 0 };
	if (Index2 == Index1)
	{
		for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
		{
			Array1[i] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[i];
		}
	}
	else if (Index1 > Index2)
	{
		BYTE Change = Index1 - Index2;
		for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
		{
			BYTE Index = (i + Change) % MAX_DIAL_GameSum;
			Array1[Index] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[i];
		}
	}
	else
	{
		BYTE Change = Index2 - Index1;
		for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
		{
			BYTE Index = (MAX_DIAL_GameSum + i - Change) % MAX_DIAL_GameSum;
			Array1[Index] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[i];
		}
	}
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)//�������̵�����
	{
		m_TableAreaData[i] = (m_TableAreaData[i] / 4) * 4 + Array1[i];
	}
}
BYTE DialManager::GetIndexByParam(BYTE Param,bool States)
{
	if (States)
	{
		//��Ȧ
		vector<BYTE> pVec;
		for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
		{
			if (g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[j] % 4 == Param % 4)
			{
				pVec.push_back(j);
			}
		}
		size_t Index = RandUInt() % pVec.size();
		return pVec[Index];
	}
	else
	{
		//��Ȧ
		vector<BYTE> pVec;
		for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
		{
			if (m_TableAreaData[j] / 4 == Param / 4)
			{
				pVec.push_back(j);
			}
		}
		size_t Index = RandUInt() % pVec.size();
		return pVec[Index];
	}
}
void DialManager::CreateNewDial() //���������ϵ���ֵ
{
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		m_TableAreaRate[i] = 0;
	}
	//2.ȷ����Ȧ����  ��Ȧ ��֤ 0-1-2 ���ұ�֤ ȫ���� 12��ѡ�� ȫ����ֵ
	//��Ȧ�̶��� ����������Ȧ������
	BYTE Result[MAX_DIAL_GameSum] = { 0 };
	BYTE ID = static_cast<BYTE>(RandUInt() % g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RateMap.size());
	
	BYTE TempArray[MAX_DIAL_ClientSum] = { 0 };
	for (BYTE x = 0; x < MAX_DIAL_ClientSum; ++x)
	{
		BYTE Value = RandUInt() % 12;//0-11
		while (TempArray[Value] == 1)
		{
			Value = RandUInt() % 12;
		}
		TempArray[Value] = 1;
		//��������
		BYTE i = Value;
		for (BYTE j = 0; j < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RateMap[ID].SumArray[i]; ++j)
		{
			bool IsResult = false;
			//��һ���� ˳����� ���������
			for (BYTE k = 0; k < MAX_DIAL_GameSum; ++k)
			{
				if (/*m_TableAreaData[k] % 4 */g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[k] % 4 == i % 4 && Result[k] == 0)
				{
					m_TableAreaData[k] = i;
					Result[k] = 1;
					m_TableAreaRate[i] += 1;
					IsResult = true;
					break;
				}
			}
			//
			if (!IsResult)
				g_FishServer.ShowInfoToWin("���̵����� ���� �޷�������ɫ���� %d", ID);
		}
	}

	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		if (m_TableAreaRate[i] == 0)
		{
			//��ǰѡ����Ҫ�������
			BYTE Value = i / 4;//0-2 ��ɫ
			BYTE Rand = i % 4;//0-3 ����
			for (BYTE j = 0; j < MAX_DIAL_ClientSum; ++j)
			{
				if (m_TableAreaRate[j] > 1 && j % 4 == Rand)
				{
					//�޸�����
					for (BYTE k = 0; k < MAX_DIAL_GameSum; ++k)
					{
						if (m_TableAreaData[k] == j)
						{
							m_TableAreaData[k] = i;
							m_TableAreaRate[j] -= 1;
							m_TableAreaRate[i] += 1;
							break;
						}
					}
					break;
				}
			}
		}
	}
	
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		m_TableAreaRate[i] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.RateMap[ID].RateArray[i];
	}
}
void DialManager::OnTableJoinBegin()
{
	//�����ӽ�����ע�׶ε�ʱ��
	//1.�ж�ׯ���Ƿ���Ҫ���б仯
	SetNextBankerUserInfo();
	//2.��������ϵ�����
	OnClearTableInfo();
	//3.֪ͨ��� �����µĽ׶���
	SetTableStates(DTS_Begin);//���ӽ�����ע�׶�
	//4.������������
	CreateNewDial();
	//5.
	MG_Cmd_DialTableJoinBegin msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialTableJoinBegin), sizeof(MG_Cmd_DialTableJoinBegin));
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
		msg.TableAreaData[i] = m_TableAreaData[i];
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		msg.TableAreaDataRate[i] = m_TableAreaRate[i];
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
	m_animalrobotmanager.StartGame();
}
void DialManager::OnUpdateTableInfo()
{
	//���������ϵ�����
	MG_Cmd_DialTableUpdate msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialTableUpdate), sizeof(MG_Cmd_DialTableUpdate));
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
BYTE DialManager::GetAreaID()
{
	BYTE Index1 = (m_TableResult >> 8);
	BYTE Index2 = static_cast<BYTE>(m_TableResult);
	//0-11
	BYTE Color = m_TableAreaData[Index1] / 4;
	BYTE Item = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[Index2];//m_TableAreaData[Index2] % 4;
	BYTE Result = Color * 4 + Item;
	return Result;
}
void DialManager::ChangeAreaDataByResult()
{
	//�޸����̵����� 
	BYTE Index1 = (m_TableResult >> 8);
	BYTE Index2 = static_cast<BYTE>(m_TableResult);

	BYTE AreaData1[MAX_DIAL_GameSum] = { 0 };
	BYTE AreaData2[MAX_DIAL_GameSum] = { 0 };
	//��Ȧ���� ���� 
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		BYTE Index = (i >= Index1 ? i - Index1 : (MAX_DIAL_GameSum + i - Index1));
		AreaData1[Index] = m_TableAreaData[i] / 4;
	}
	//��Ȧ��������
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		BYTE Index = (i >= Index2 ? i - Index2 : (MAX_DIAL_GameSum + i - Index2));
		AreaData2[Index] = m_TableAreaData[i] % 4;
	}
	//
	for (BYTE i = 0; i < MAX_DIAL_GameSum; ++i)
	{
		m_TableAreaData[i] = AreaData1[i] * 4 + AreaData2[i];
	}
}
INT64 DialManager::GetBrandUserGlobelChange()
{
	INT64 GlobelChange = 0;
	BYTE ID = GetAreaID();
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		//��Ȧ������ɫ ��ȫ���� ���� 
		if (ID == i)
		{
			GlobelChange -= (m_TableAreaRate[i] * m_TableBetAreaGlobel[i]);
		}
		else
		{
			GlobelChange += m_TableBetAreaGlobel[i];
		}
	}
	return GlobelChange;
}
INT64 DialManager::GetSystemGlobelChange()
{
	bool IsSystemBranker = false;
	if (m_TableBankerUserID == 0)//ׯ��Ϊϵͳ�Ļ� ���� ׯ��Ϊ�����˵�ʱ��
	{
		IsSystemBranker = true;
	}
	else
	{
		HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(m_TableBankerUserID);
		if (Iter != m_TableUserMap.end() && Iter->second.IsRobot)
		{
			IsSystemBranker = true;
		}
	}

	INT64 GlobelChange = 0;
	BYTE ID = GetAreaID();
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		{
			if (Iter->second.BetGlobel[i] == 0)
				continue;
			if (i == ID)
			{
				if (Iter->second.IsRobot)
				{
					if (!IsSystemBranker)
						GlobelChange += (Iter->second.BetGlobel[i] * m_TableAreaRate[i]);
				}
				else
				{
					if (IsSystemBranker)
						GlobelChange -= (Iter->second.BetGlobel[i] * m_TableAreaRate[i]);
				}
			}
			else
			{
				if (Iter->second.IsRobot)
				{
					if (!IsSystemBranker)
						GlobelChange -= (Iter->second.BetGlobel[i]);
				}
				else
				{
					if (IsSystemBranker)
						GlobelChange += (Iter->second.BetGlobel[i]);
				}
			}
		}
	}
	return GlobelChange;
}
INT64 DialManager::ChangeResultBySystem(BYTE OldIndex2)
{
	//1.�������ϵͳ������ ֱ��ϵͳ����Ǯ
	BYTE Index = 0;
	INT64 SystemGlobel = 0;
	do
	{
		//�������һ������
		//m_TableResult = RandUInt() % MAX_DIAL_GameSum;
		BYTE Index1 = 0;// RandUInt() % MAX_DIAL_GameSum;
		BYTE Index2 = 0;// RandUInt() % MAX_DIAL_GameSum;
		CreateNewResult(Index1, Index2);//�������
		Index2 += OldIndex2;
		Index2 %= MAX_DIAL_GameSum;
		m_TableResult = (Index1 << 8) | Index2;
		//ChangeAreaDataByResult();
		SystemGlobel = GetSystemGlobelChange();
		++Index;
	} while (SystemGlobel < 0 && Index < 200);//������200����
	return SystemGlobel;
}
void DialManager::CreateNewResult(BYTE& Index1, BYTE& Index2)
{
	Index1 = 0;
	Index2 = 0;
	//�������̵ı��� ����һ������Ľ�� ���ϸ������� ������ĸ���С ��Ҫ��һ������
	DWORD MaxSum = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		if (MaxSum == 0)
			MaxSum = m_TableAreaRate[i];
		else if (MaxSum % m_TableAreaRate[i] != 0)
			MaxSum *= m_TableAreaRate[i];
	}
	DWORD TempArray[MAX_DIAL_ClientSum];
	DWORD MaxRateValue = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		TempArray[i] = MaxSum / m_TableAreaRate[i];//Ȩ��
		MaxRateValue += TempArray[i];
	}
	DWORD Value = RandUInt() % MaxRateValue;
	DWORD BeAdddValue = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
	{
		if (Value < BeAdddValue + TempArray[i])
		{
			//��� Ϊ i  ���ǽ��д���
			//���ݽ�� ����ȷ�� ת�̵�ת������
			//1.��Ȧ
			/*for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
			{
				if (m_TableAreaData[j] / 4 == i / 4)
				{
					Index1 = j;
					break;
				}
			}*/
			Index1 = GetIndexByParam(i, false);
			//2.��Ȧ
			Index2 = GetIndexByParam(i, true);
			/*for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
			{
				if (g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[j] % 4 == i % 4)
				{
					Index2 = j;
					break;
				}
			}*/
			return;
		}
		else
		{
			BeAdddValue += TempArray[i];
		}
	}
}
void DialManager::OnTableJoinEnd()
{
	//���������Ƶ�ʱ�� 
	//���н����ʱ�� 
	UINT64 TotalAllAddGlobel = 0;
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		TotalAllAddGlobel += m_TableBetAreaGlobel[i];

	//���ݸ��� ����һ�� ���
	BYTE Index1 = 0; //RandUInt() % MAX_DIAL_GameSum;
	BYTE Index2 = 0;// RandUInt() % MAX_DIAL_GameSum;
	INT64 SystemGlobel=0;
	INT64 BrandUserGlobel = 0;
	if (g_FishServer.GetAnimalControl().NeedControl())	
	{
		DWORD Value = g_FishServer.GetAnimalControl().GetSuitResult();		
		//1.��Ȧ
		for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
		{
			if (m_TableAreaData[j] / 4 == Value / 4)
			{
				Index1 = j;
				break;
			}
		}
		//2.��Ȧ
		for (BYTE j = 0; j < MAX_DIAL_GameSum; ++j)
		{
			if (g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[j] % 4 == Value % 4)
			{
				Index2 = j;
				break;
			}
		}
		m_TableResult = (Index1 << 8) | Index2;
		SystemGlobel = GetSystemGlobelChange();
		BrandUserGlobel = GetBrandUserGlobelChange();
		g_FishServer.GetAnimalControl().CompleteControl();
	}
	else
	{
		CreateNewResult(Index1, Index2);//�������
		BYTE OldIndex2 = (BYTE)m_TableResult;
		Index2 += OldIndex2;
		Index2 %= MAX_DIAL_GameSum;

		m_TableResult = (Index1 << 8) | Index2;
		//ChangeAreaDataByResult();//�޸�����
		//m_TableResult = RandUInt() % MAX_DIAL_GameSum;
		//��������
		 SystemGlobel = GetSystemGlobelChange();
		 BrandUserGlobel = GetBrandUserGlobelChange();
		if (m_SystemGlobelSum < 0 || (m_SystemGlobelSum + SystemGlobel) < 0)
		{
			SystemGlobel = ChangeResultBySystem(OldIndex2);
			BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
		}
	}

	{
		BYTE Index1 = (m_TableResult >> 8);
		BYTE Index2 = static_cast<BYTE>(m_TableResult);
		BYTE Rank = m_TableAreaData[Index1] / 4;
		BYTE Value = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.DialAreaData[Index2];
		string strRank = (Rank == 0 ? "��" : (Rank == 1 ? "��" : "��"));
		string strValue = (Value == 0 ? "��è" : (Value == 1 ? "����" : (Value ==2?"����":"ʨ��")));
		string strInfo = strValue + strRank;
		g_FishServer.ShowInfoToWin(strInfo.c_str());
	}

	/*{
		BYTE Rank = GetAreaID() / 4;
		BYTE Value = GetAreaID()%4;
		string strRank = (Rank == 0 ? "��" : (Rank == 1 ? "��" : "��"));
		string strValue = (Value == 0 ? "��è" : (Value == 1 ? "����" : (Value == 2 ? "����" : "ʨ��")));
		string strInfo = "ȷ�Ͻ��:" +strValue + strRank;
		g_FishServer.ShowInfoToWin(strInfo.c_str());
	}*/
	m_DialResultLog.SetResult(GetAreaID());//���ݵ��ֵĽ����������
	//CreateNewDial();//�������µĽ�� ˢ������������
	OnMoveDialByResult();

	//3.������Ϻ� ���ǽ��д���
	MG_Cmd_DialTableJoinEnd msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialTableJoinEnd), sizeof(MG_Cmd_DialTableJoinEnd));
	for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	msg.TableResultIndex = m_TableResult;
	
	msg.TableGameSum = m_DialResultLog.LogSum;
	msg.TableResultLog = m_DialResultLog.ResultLog;
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
		for (BYTE i = 0; i < MAX_DIALSHOWBRAND_Sum; ++i)
		{
			msg.BankerListGlobelInfo[i].dwUserID = 0;
			msg.BankerListGlobelInfo[i].dwGlobelSum = 0;
		}
	}
	else
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_DIALSHOWBRAND_Sum;)
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
	for (BYTE i = 0; i < MAX_DIALVIPSEAT_Sum; ++i)
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

	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		if (m_TableBankerUserID != 0 && msg.dwUserID == m_TableBankerUserID)
		{
			msg.AddGlobelSum = BrandUserGlobel;

			if (!Iter->second.IsRobot)
				IsChangeSystemGlobel = true;
		}
		else
		{
			//����ǰ��ҵ�����
			int AddGlobel = 0;
			for (BYTE i = 0; i < MAX_DIAL_ClientSum; ++i)
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

			if (AddGlobel == 0 && Iter->second.IsInVipSeat && m_VipSeatVec[Iter->second.VipSeat].dwUserID == Iter->first)
			{
				//��ǰ���δ��ע
				++m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum;
				if (m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.VipNonBetSum)
				{
					if ((pRole->GetRoleInfo().ParticularStates & ParticularState_DialVip) != 0)
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
	if (TotalAllAddGlobel != 0 && IsChangeSystemGlobel) //������ע��ʱ�� ������еݼ�
		m_SystemGlobelSum = static_cast<INT64>(m_SystemGlobelSum * g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.GetChangeGlobelRateValue());

	if (m_TableBankerUserID != 0)
	{
		if (BrandUserGlobel >= 0)
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_Dial, 1, static_cast<DWORD>(BrandUserGlobel), TEXT("�����ׯɭ������ý��"), SendLogDB);
		else
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_Dial, 0, static_cast<DWORD>(BrandUserGlobel*-1), TEXT("�����ׯɭ�����ʧȥ���"), SendLogDB);
	}
	//�洢�����ݿ�����ȥ
	g_DBLogManager.LogDialInfoToDB(m_TableBankerUserID, m_TableAreaData, m_TableBetAreaGlobel, m_TableResult, SystemGlobel, m_SystemGlobelSum, m_TableUserMap.size(), SendLogDB);
	//�жϵ�ǰ���Ŷ��б������� 
	MG_Cmd_DialWriteBankerUserGlobelMsg msgSend;
	SetMsgInfo(msgSend, GetMsgType(Main_Dial, MG_DialWriteBankerUserGlobelMsg), sizeof(MG_Cmd_DialWriteBankerUserGlobelMsg));
	vector<DWORD>::iterator IterVec = m_TableWriteBankerVec.begin();
	for (; IterVec != m_TableWriteBankerVec.end(); ++IterVec)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(*IterVec);
		if (!pRole)
			continue;
		if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().dialConfig.JoinBankerGlobelSum)
		{
			msgSend.dwUserID = *IterVec;
			pRole->SendDataToGameServer(&msgSend);
		}
	}
	m_animalrobotmanager.EndGame();
}
SHORT DialManager::GetBrandRateValue(BYTE Index)
{
	if (Index >= MAX_DIAL_ClientSum)
		return 1;
	if (GetAreaID() == Index)
	{
		//��ׯ�Ҵ��ʱ��
		BYTE Rate = m_TableAreaRate[Index];
		//���Ӯׯ�� �˻����� ���㱶��
		return static_cast<SHORT>(Rate);
	}
	else
	{
		return 0;
	}
}
void DialManager::OnRoleJoinVipSeat(DWORD dwUserID, BYTE VipSeatIndex)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_DialRoleJoinVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleJoinVipSeat), sizeof(MG_Cmd_DialRoleJoinVipSeat));
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
	if (VipSeatIndex >= MAX_DIALVIPSEAT_Sum)
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
				MG_Cmd_DialRoleBeLeaveVipSeat msgLeave;
				SetMsgInfo(msgLeave, GetMsgType(Main_Dial, MG_DialRoleBeLeaveVipSeat), sizeof(MG_Cmd_DialRoleBeLeaveVipSeat));
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
	HashMap<DWORD, tagDialRoleInfo>::iterator IterSend = m_TableUserMap.begin();//Vipϯλ�ĸı䷢�͸�ȫ�����
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void DialManager::OnRoleLeaveVipSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
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

	MG_Cmd_DialRoleLeaveVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialRoleLeaveVipSeat), sizeof(MG_Cmd_DialRoleLeaveVipSeat));
	msg.dwDestUserID = dwUserID;
	msg.VipSeatIndex = Iter->second.VipSeat;
	HashMap<DWORD, tagDialRoleInfo>::iterator IterSend = m_TableUserMap.begin();
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void DialManager::OnRoleGetNormalInfoByPage(DWORD dwUserID, DWORD Page)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator IterRole = m_TableUserMap.find(dwUserID);
	if (IterRole == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}

	DWORD BeginIndex = Page * MAX_DIALNORMAL_PAGESUM;
	DWORD EndIndex = (Page + 1)* MAX_DIALNORMAL_PAGESUM - 1;


	MG_Cmd_DialGetNormalSeatInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Dial, MG_DialGetNormalSeatInfo), sizeof(MG_Cmd_DialGetNormalSeatInfo));
	msg.dwUserID = dwUserID;
	msg.Page = Page;
	msg.TotalRoleSum = m_TableUserMap.size();

	for (BYTE i = 0; i < MAX_DIALVIPSEAT_Sum; ++i)
	{
		if (m_VipSeatVec[i].dwUserID != 0)
			--msg.TotalRoleSum;
	}
	if (m_TableBankerUserID != 0)
		--msg.TotalRoleSum;

	for (BYTE i = 0; i < MAX_DIALNORMAL_PAGESUM; ++i)
	{
		msg.Array[i].dwFaceID = 0;
		msg.Array[i].dwUserID = 0;
		msg.Array[i].GlobelSum = 0;
		msg.Array[i].NickName[0] = 0;
	}
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
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

DWORD DialManager::BankerId()
{
	return m_TableBankerUserID;
}

WORD DialManager::ApplyListLength()
{
	return m_TableWriteBankerVec.size();
}

DWORD DialManager::BankerGold()
{
	Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
	if (pBankerRole)
	{
		return pBankerRole->GetGlobelSum();
	}
	return 0;
}
bool	DialManager::IsGameEnd()
{
	return m_TableStates == DTS_End;
}

bool	DialManager::HaveApply(DWORD dwUserid)
{
	return find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserid) != m_TableWriteBankerVec.end();
}

void DialManager::QueryNames(DWORD ClientID)
{
	int nNamelength = (MAX_NICKNAME + 1);
	int nSize = sizeof(LC_CMD_AnimalAdmin_PlayerNames)+sizeof(TCHAR)*nNamelength*(m_TableUserMap.size() - m_animalrobotmanager.RobotCount());
	LC_CMD_AnimalAdmin_PlayerNames *pResult = (LC_CMD_AnimalAdmin_PlayerNames*)malloc(nSize);
	pResult->SetCmdSize(nSize);
	pResult->SetCmdType((Main_Control << 8) | LC_AnimalAdminQueryName);
	pResult->ClientID = ClientID;
	int nIndex = 0;
	for (HashMap<DWORD, tagDialRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole&&!m_animalrobotmanager.IsRobot(pRole->GetRoleID()))
		{
			TCHARCopy(&pResult->names[1 + nIndex*nNamelength], nNamelength, pRole->GetNickName(), MAX_NICKNAME);
			nIndex++;
		}
	}
	g_FishServer.SendNetCmdToControl(pResult);
	free(pResult);
}

void DialManager::QueryAllJetton(DWORD ClientID)
{
	LC_CMD_AnimalAdmin_AllJetton result;
	ZeroMemory(&result, sizeof(result));
	result.SetCmdSize(sizeof(result));
	result.SetCmdType((Main_Control << 8) | LC_AnimalAdminQueryAllJetton);
	result.ClientID = ClientID;
	result.nStock = m_SystemGlobelSum;
	CalPlayerJetton(result.nJetton);
	g_FishServer.SendNetCmdToControl(&result);

}

void DialManager::QueryPlayerJetton(TCHAR *pNickame, DWORD ClientID)
{
	for (HashMap<DWORD, tagDialRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole)
		{
			if (_tcscmp(pNickame, pRole->GetNickName()) == 0)
			{
				//HashMap<DWORD, tagDialRoleInfo>::iterator itJetton = m_TableUserMap.find(pRole->GetRoleID());
				//if (itJetton != m_TableUserMap.end())
				{
					LC_CMD_AnimalAdmin_PlayerJetton result;
					result.SetCmdSize(sizeof(result));
					result.SetCmdType((Main_Control << 8) | LC_AnimalAdminQueryPlayerJetton);
					result.ClientID = ClientID;
					//TCHARCopy(result.name, MAX_NICKNAME + 1, pNickame, MAX_NICKNAME);
					memcpy(result.dwJetton, it->second.BetGlobel, sizeof(result.dwJetton));
					g_FishServer.SendNetCmdToControl(&result);
					break;
				}
			}
		}
	}
}

void DialManager::CalPlayerJetton(UINT64 uJetton[MAX_DIAL_ClientSum])
{
	for (HashMap<DWORD, tagDialRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		if (!m_animalrobotmanager.IsRobot(it->first))
		{
			for (int i = 0; i < MAX_DIAL_ClientSum; i++)
			{
				uJetton[i] += it->second.BetGlobel[i];
			}
		}
	}
}

bool DialManager::IsExitsNormalRole()
{
	HashMap<DWORD, tagDialRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		if (!Iter->second.IsRobot)
			return true;
	}
	return false;
}