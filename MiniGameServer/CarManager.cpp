#include "Stdafx.h"
#include "CarManager.h"
#include "../CommonFile/DBLogManager.h"
#include "FishServer.h"
extern void SendLogDB(NetCmd* pCmd);
CarManager::CarManager()
{

}
CarManager::~CarManager()
{
	OnDestroy();
}
void CarManager::SetTableStates(CarTableStates States)
{
	m_TableStates = States;
	m_TableStatesLog = timeGetTime();
}
bool CarManager::OnInit()
{
	SetTableStates(CTS_WriteBegin);//���ó�ʼ����ʱ�� ����״̬Ϊ�ȴ���ʼ
	m_SystemGlobelSum = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.InitGlobelSum;//ϵͳ���
	//��ʼ��һЩ����
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		m_TableBetAreaGlobel[i] = 0;
	}
	for (BYTE i = 0; i < MAX_CARVIPSEAT_Sum; ++i)
	{
		m_VipSeatVec[i].dwUserID = 0;
		m_VipSeatVec[i].NonBetGlobelSum = 0;
	}
	m_TableBankerUserID = 0;
	m_TableBankerUseGameSum = 0;
	m_TableResult = 0;
	m_TableResultIndex = 0;
	m_TableWriteBankerVec.clear();
	m_TableUserMap.clear();
	m_IsNeedClearBankerInfo = false;
	return true;
}
void CarManager::OnUpdate(DWORD dwTimer)
{
	static DWORD CarUpdateTime = 0;
	if (CarUpdateTime == 0 || dwTimer - CarUpdateTime >= 1000) //1s����һ��
	{
		CarUpdateTime = dwTimer;
		UpdateTableIsCanBegin(timeGetTime());
		UpdateTableIsCanEnd(timeGetTime());
		UpdateTableInfo(timeGetTime());
		m_robotmanager.Update();
	}
}
void CarManager::OnDestroy()
{
}
void CarManager::OnRoleJoinCarTable(DWORD dwUserID)
{
	//��ҽ�����ͼ����ţţ����
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleJoinTable msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleJoinTable), sizeof(MG_Cmd_CarRoleJoinTable));
	msg.dwUserID = dwUserID;
	msg.Result = false;
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter != m_TableUserMap.end())
	{
		//����������뿪
		OnRoleLeaveCarTable(dwUserID);
	}
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.JoinGlobelSum)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	if (m_TableStates == CTS_WriteBegin)
	{
		//��Ϊ���˽����� 
		SetTableStates(CTS_Begin);
		OnClearTableInfo();//���������ϵ�����
	}
	msg.Result = true;
	tagCarRoleInfo pInfo;
	pInfo.UserID = dwUserID;
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		pInfo.BetGlobel[i] = 0;
	pInfo.IsBankerUser = false;
	pInfo.IsInWriteBanakerVec = false;
	pInfo.IsInVipSeat = false;
	pInfo.VipSeat = 0;
	pInfo.IsRobot = g_FishServer.GetRobotManager().IsRobot(dwUserID);

	m_TableUserMap.insert(HashMap<DWORD, tagCarRoleInfo>::value_type(dwUserID, pInfo));
	//�����������ݵ���������
	msg.TableStates = m_TableStates;
	DWORD NowTime = timeGetTime();
	if (m_TableStates == CTS_Begin)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.BeginWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else if (m_TableStates == CTS_End)
		msg.TableStatesUpdateSec = static_cast<BYTE>((g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.EndWriteSec * 1000 - (NowTime - m_TableStatesLog)) / 1000);
	else
		msg.TableStatesUpdateSec = 0;
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
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
	for (BYTE i = 0; i < MAX_CARSHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_CARSHOWBRAND_Sum;)
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
	for (BYTE i = 0; i < MAX_CARVIPSEAT_Sum; ++i)
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
	msg.TableGameSum = m_CarResultLog.LogSum;
	msg.TableResultLog = m_CarResultLog.ResultLog;
	msg.TableResultIndex = m_TableResultIndex;
	pRole->SendDataToGameServer(&msg);
}
void CarManager::OnRoleLeaveCarTable(DWORD dwUserID)
{
	//������뿪ţţ���ӵ�ʱ�� 
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
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
		if (m_TableStates == CTS_Begin)//���Ϊ��ע�׶�
		{
			//ׯ����ע�� ������ϵͳ����
			m_TableBankerUserID = 0;
			m_TableBankerUseGameSum = 0;
			OnBankerUserChange();//ׯ�ҷ����仯��
		}
		else if (m_TableStates == CTS_End)
		{
			m_IsNeedClearBankerInfo = true;//�ڿ��ƽ׶� ������ׯ��
		}
	}
	m_TableUserMap.erase(Iter);
}
bool CarManager::IsCanAddGlobel(DWORD AddGlobel, BYTE Index)
{
	UINT64 MaxGlobel = 0;
	for (BYTE i = 0; i <MAX_CAR_ClientSum; ++i)
	{
		UINT64 Globel = m_TableBetAreaGlobel[i];
		if (i == Index)
		{
			Globel += AddGlobel;
		}
		Globel = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.GetRateValue(i) * Globel;
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
void CarManager::OnRoleBetGlobelByLog(DWORD dwUserID, DWORD AddGlobelArray[MAX_CAR_ClientSum])
{
	//��ҽ��м���Ѻע
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}

	MG_Cmd_CarRoleBetGlobelByLog msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleBetGlobelByLog), sizeof(MG_Cmd_CarRoleBetGlobelByLog));
	msg.dwUserID = dwUserID;

	DWORD AllGlobel = 0; // ��ע���ܽ������
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		AllGlobel += AddGlobelArray[i];
		msg.betGlobel[i] = AddGlobelArray[i];
	}

	msg.AllGlobel = AllGlobel;
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
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
	if (m_TableStates != CTS_Begin)
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
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		RoleBetAllGlobel += Iter->second.BetGlobel[i];
	}

	if (RoleBetAllGlobel + AllGlobel> g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.MaxAddGlobel)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}

	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
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
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		Iter->second.BetGlobel[i] += AddGlobelArray[i];
		m_TableBetAreaGlobel[i] += AddGlobelArray[i];

		if (!Iter->second.IsRobot)
			g_DBLogManager.LogToDB(dwUserID, LogType::LT_Car, AddGlobelArray[i], i, TEXT("���ת����ע"), SendLogDB);//Param ��ʾ ��ע��
	}
	pRole->SendDataToGameServer(&msg);

	if (Iter->second.IsInVipSeat)
	{
		for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		{
			if (AddGlobelArray[i] == 0)
				continue;
			MG_Cmd_CarVipSeatGlobelChange msg;
			SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarVipSeatGlobelChange), sizeof(MG_Cmd_CarVipSeatGlobelChange));
			msg.VipSeat = Iter->second.VipSeat;
			msg.GlobelSum = AddGlobelArray[i];
			msg.Index = i;
			HashMap<DWORD, tagCarRoleInfo>::iterator IterRole = m_TableUserMap.begin();
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
void CarManager::OnRoleBetGlobel(DWORD dwUserID, BYTE BetIndex, DWORD AddGlobel)
{
	//�����ע
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleBetGlobel msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleBetGlobel), sizeof(MG_Cmd_CarRoleBetGlobel));
	msg.dwUserID = dwUserID;
	msg.AddGlobel = AddGlobel;
	msg.Index = BetIndex;
	msg.Result = false;
	if (BetIndex >= MAX_CAR_ClientSum)
	{
		pRole->SendDataToGameServer(&msg);
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
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
	if (m_TableStates != CTS_Begin)
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
	for (BYTE i = 0; i<MAX_CAR_ClientSum; ++i)
	{
		RoleBetAllGlobel += Iter->second.BetGlobel[i];
		TableBetAllGbel += m_TableBetAreaGlobel[i];
	}
	if (RoleBetAllGlobel + AddGlobel > g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.MaxAddGlobel)//��ǰ��ע���Ľ��̫��
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
		g_DBLogManager.LogToDB(dwUserID, LogType::LT_Car, AddGlobel, BetIndex, TEXT("���ת����ע"), SendLogDB);//Param ��ʾ ��ע��
	pRole->SendDataToGameServer(&msg);

	if (Iter->second.IsInVipSeat)
	{
		MG_Cmd_CarVipSeatGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarVipSeatGlobelChange), sizeof(MG_Cmd_CarVipSeatGlobelChange));
		msg.VipSeat = Iter->second.VipSeat;
		msg.GlobelSum = AddGlobel;
		msg.Index = BetIndex;
		HashMap<DWORD, tagCarRoleInfo>::iterator IterRole = m_TableUserMap.begin();
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
void CarManager::OnRoleGetWriteBankerList(DWORD dwUserID)
{
	//��ҽ���ׯ���Ŷ��б�
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	//��������ͻ���ȥ
	MG_Cmd_CarGetBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarGetBankerList), sizeof(MG_Cmd_CarGetBankerList));
	msg.dwUserID = dwUserID;
	msg.dwMySeatIndex = 0xffffffff;
	for (BYTE i = 0; i < MAX_CARSHOWBRAND_Sum; ++i)
	{
		msg.TableWriteBankerList[i].dwUserID = 0;
		msg.TableWriteBankerList[i].GlobelSum = 0;
		msg.TableWriteBankerList[i].NickName[0] = 0;
		msg.TableWriteBankerList[i].dwFaceID = 0;
	}
	if (!m_TableWriteBankerVec.empty())
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_CARSHOWBRAND_Sum;)
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
void CarManager::OnRoleJoinWriteBankerList(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	//�����ͼ������ׯ 
	MG_Cmd_CarRoleJoinBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleJoinBankerList), sizeof(MG_Cmd_CarRoleJoinBankerList));
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
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.JoinBankerGlobelSum)
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
	if (msg.SeatIndex < MAX_CARSHOWBRAND_Sum)
	{
		OnWriteBankerChange();
	}
	else
	{
		pRole->SendDataToGameServer(&msg);
	}
}
void CarManager::OnRoleGetWriteBankerFirstSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleGetBankerFirstSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleGetBankerFirstSeat), sizeof(MG_Cmd_CarRoleGetBankerFirstSeat));
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
	if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.GetNextBankerNeedGlobel)
	{
		pRole->SendDataToGameServer(&msg);
		return;
	}
	//������ׯ
	msg.Result = true;
	//1.�۳����
	pRole->OnChangeRoleGlobelSum(pRole->GetGlobelSum() - g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.GetNextBankerNeedGlobel);
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
void CarManager::OnRoleLeaveWriteBankerList(DWORD dwUserID)
{
	//����뿪��ׯ�Ŷ��б� 
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleLeaveBankerList msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleLeaveBankerList), sizeof(MG_Cmd_CarRoleLeaveBankerList));
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
	if (Index < MAX_CARSHOWBRAND_Sum)
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
void CarManager::OnRoleCanelBanker(DWORD dwUserID)
{
	//�����ׯ
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleCanelBankerSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleCanelBankerSeat), sizeof(MG_Cmd_CarRoleCanelBankerSeat));
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
	if (m_TableStates == CTS_Begin)
	{
		//Ӧ������ҵĽ�� ��ʱ����1���� 


		//��ע�׶� �����ׯ ������ϵͳ����
		m_TableBankerUserID = 0;
		m_TableBankerUseGameSum = 0;
		OnBankerUserChange();
	}
	else if (m_TableStates == CTS_End)
	{
		//���ƽ׶� �����ׯ ֱ����ע ����
		//���������ׯ�ɹ� 
		m_IsNeedClearBankerInfo = true;
	}
	if (pRole)
		pRole->SendDataToGameServer(&msg);
}
void CarManager::OnRoleLeaveMiniGameServer(DWORD dwUserID)
{
	OnRoleLeaveCarTable(dwUserID);
}
void CarManager::OnRoleGlobelChange(DWORD dwUserID, int ChangeSum)//�������û���ֵ��ʱ��
{
	if (m_TableBankerUserID != 0 && m_TableBankerUserID == dwUserID)
	{
		Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
		if (!pBankerRole)
			return;

		MG_Cmd_CarBankerUserGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarBankerUserGlobelChange), sizeof(MG_Cmd_CarBankerUserGlobelChange));
		msg.dwBankerGlobelSum = pBankerRole->GetGlobelSum();
		msg.dwBankerUserID = dwUserID;

		HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
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
		HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
		if (Iter == m_TableUserMap.end())
			return;
		if (!Iter->second.IsInVipSeat)
			return;
		//��������VIPλ���ϵ�ʱ�� 
		MG_Cmd_CarVipGlobelChange msg;
		SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarVipGlobelChange), sizeof(MG_Cmd_CarVipGlobelChange));
		msg.VipUserID = dwUserID;
		msg.VipGlobelSum = pRole->GetGlobelSum();
		HashMap<DWORD, tagCarRoleInfo>::iterator IterRole = m_TableUserMap.begin();
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
void CarManager::OnClearTableInfo()
{
	//ÿ�ֿ�ʼ��ע֮ǰ ������������
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		m_TableBetAreaGlobel[i] = 0;
	}
}
void CarManager::OnWriteBankerChange()
{
	MG_Cmd_CarBankerListChange msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarBankerListChange), sizeof(MG_Cmd_CarBankerListChange));
	//SendDataToAllTableRole(&msg);
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void CarManager::OnBankerUserChange()
{
	MG_Cmd_CarBankerUserChange msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarBankerUserChange), sizeof(MG_Cmd_CarBankerUserChange));
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
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void CarManager::UpdateTableIsCanBegin(DWORD dwTimer)
{
	if (m_TableStates == CTS_End)
	{
		//���ӵ�ǰ״̬Ϊ ���ƽ׶�
		if (m_TableStatesLog != 0 && dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.EndWriteSec * 1000)
			return;
		//���뿪�ƽ׶��Ѿ���30����
		if (m_TableUserMap.empty())
		{
			SetTableStates(CTS_WriteBegin);//���ӽ���ȴ���ʼ�׶� ��Ϊû�� �����������
			OnClearTableInfo();
		}
		else
		{
			OnTableJoinBegin();
		}
		return;
	}
	else if (m_TableStates == CTS_WriteBegin)
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
void CarManager::UpdateTableIsCanEnd(DWORD dwTimer)
{
	if (m_TableStates == CTS_Begin)
	{
		if (dwTimer - m_TableStatesLog < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.BeginWriteSec * 1000)
			return;
		//������ע�׶�30������ ���Խ��뿪�ƽ׶��� 
		OnTableJoinEnd();
		SetTableStates(CTS_End);//���ӽ��뿪�ƽ׶�
		return;
	}
	else
	{
		return;//�����׶β�������
	}
}
void CarManager::UpdateTableInfo(DWORD dwTimer)
{
	if (m_TableStates != CTS_Begin)
		return;
	static DWORD CarUpdateTableInfo = 0;
	if (CarUpdateTableInfo == 0 || dwTimer - CarUpdateTableInfo >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.UpdateSec * 1000)
	{
		CarUpdateTableInfo = dwTimer;
		OnUpdateTableInfo();
	}
}
tagCarRoleInfo* CarManager::GetRoleInfo(DWORD dwUserID)
{
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
		return NULL;
	else
		return &Iter->second;
}
void CarManager::SetNextBankerUserInfo()
{
	//����Ϸ��ʼ��ע��ʱ�� ����������ׯ�ҵ�����
	bool BankerIsChange = false;
	if (m_IsNeedClearBankerInfo)
	{
		tagCarRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
			if (m_TableBankerUseGameSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.BankerGameSum)
			{
				tagCarRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
				if (pUserInfo)
				{
					pUserInfo->IsBankerUser = false;
				}

				m_TableBankerUserID = 0;
				m_TableBankerUseGameSum = 0;
				BankerIsChange = true;
			}
			else if (pBankerRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.JoinBankerGlobelSum)
			{
				tagCarRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
			tagCarRoleInfo* pUserInfo = GetRoleInfo(m_TableBankerUserID);
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
				if (!pDestRole || pDestRole->GetGlobelSum() <  g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.JoinBankerGlobelSum)
				{
					tagCarRoleInfo* pUserInfo = GetRoleInfo(*Iter);
					if (pUserInfo)
					{
						pUserInfo->IsInWriteBanakerVec = false;
					}

					Iter = m_TableWriteBankerVec.erase(Iter);
				}
				else
				{
					tagCarRoleInfo* pUserInfo = GetRoleInfo(*Iter);
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
void CarManager::CreateNewCar()
{
	m_TableResult = 0;
	m_TableResultIndex = 0;
}
BYTE CarManager::GetTableResult(BYTE& Index)//�������һ�����  
{
	Index = 0;
	DWORD Value = RandUInt() % g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.MaxRate;
	vector<tagCarValue>::iterator Iter = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarValueVec.begin();
	for (; Iter != g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarValueVec.end(); ++Iter)
	{
		if (Value >= Iter->MinRate && Value < Iter->MaxRate)
		{
			Index = Iter->ID;
			return Iter->TypeID;
		}
	}
	return 0;
}
void CarManager::OnTableJoinBegin()
{
	//�����ӽ�����ע�׶ε�ʱ��
	//1.�ж�ׯ���Ƿ���Ҫ���б仯
	SetNextBankerUserInfo();
	//2.��������ϵ�����
	OnClearTableInfo();
	//3.֪ͨ��� �����µĽ׶���
	SetTableStates(CTS_Begin);//���ӽ�����ע�׶�
	//4.������������
	CreateNewCar();
	//5.
	MG_Cmd_CarTableJoinBegin msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarTableJoinBegin), sizeof(MG_Cmd_CarTableJoinBegin));
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
	m_robotmanager.StartGame();
}
void CarManager::OnUpdateTableInfo()
{
	//���������ϵ�����
	MG_Cmd_CarTableUpdate msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarTableUpdate), sizeof(MG_Cmd_CarTableUpdate));
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(Iter->first);
		if (!pRole)
			continue;
		msg.dwUserID = Iter->first;
		pRole->SendDataToGameServer(&msg);
	}
}
INT64 CarManager::GetBrandUserGlobelChange()
{
	INT64 GlobelChange = 0;
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
	{
		if (GetResultID() == i)
		{
			GlobelChange -= (GetResultRate() * m_TableBetAreaGlobel[i]);
		}
		else
		{
			GlobelChange += m_TableBetAreaGlobel[i];
		}
	}
	return GlobelChange;
}
INT64 CarManager::GetSystemGlobelChange()
{
	bool IsSystemBranker = false;
	if (m_TableBankerUserID == 0)//ׯ��Ϊϵͳ�Ļ� ���� ׯ��Ϊ�����˵�ʱ��
	{
		IsSystemBranker = true;
	}
	else
	{
		HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(m_TableBankerUserID);
		if (Iter != m_TableUserMap.end() && Iter->second.IsRobot)
		{
			IsSystemBranker = true;
		}
	}

	INT64 GlobelChange = 0;
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		{
			if (Iter->second.BetGlobel[i] == 0)
				continue;
			if (i == GetResultID())
			{
				if (Iter->second.IsRobot)
				{
					if (!IsSystemBranker)
						GlobelChange += (Iter->second.BetGlobel[i] * GetResultRate());
				}
				else
				{
					if (IsSystemBranker)
						GlobelChange -= (Iter->second.BetGlobel[i] * GetResultRate());
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
INT64 CarManager::ChangeResultBySystem()
{
	//1.�������ϵͳ������ ֱ��ϵͳ����Ǯ
	BYTE Index = 0;
	INT64 SystemGlobel = 0;
	do
	{
		//�������һ������
		m_TableResult = GetTableResult(m_TableResultIndex);
		SystemGlobel = GetSystemGlobelChange();
		++Index;
	} while (SystemGlobel < 0 && Index < 200);//������200����
	return SystemGlobel;
}
void CarManager::OnTableJoinEnd()
{
	//���������Ƶ�ʱ�� 
	//���н����ʱ�� 
	UINT64 TotalAllAddGlobel = 0;
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		TotalAllAddGlobel += m_TableBetAreaGlobel[i];


	//���ݸ��� ����һ�� ���
	m_TableResult = GetTableResult(m_TableResultIndex);
	//��������
	INT64 SystemGlobel = GetSystemGlobelChange();
	INT64 BrandUserGlobel = GetBrandUserGlobelChange();

	if (g_FishServer.GetCarControl().NeedControl())
	{
		//m_TableResult=
		for (int i = 0; i < 20; i++)
		{
			BYTE byPos = rand() % g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarValueVec.size();
			tagCarValue pItem = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarValueVec[byPos];
			if (pItem.TypeID == g_FishServer.GetCarControl().GetSuitResult())
			{
				m_TableResult = pItem.TypeID;
				m_TableResultIndex = byPos;
				break;
			}
		}	
		SystemGlobel = GetSystemGlobelChange();
		BrandUserGlobel = GetBrandUserGlobelChange();
		g_FishServer.GetCarControl().CompleteControl();
	}
	else
	{

		if (m_SystemGlobelSum < 0 || (m_SystemGlobelSum + SystemGlobel) < 0)
		{
			SystemGlobel = ChangeResultBySystem();
			BrandUserGlobel = GetBrandUserGlobelChange();//���ݽ������ׯ�ҽ�ҵı仯
		}
	}

	//3.������Ϻ� ���ǽ��д���
	MG_Cmd_CarTableJoinEnd msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarTableJoinEnd), sizeof(MG_Cmd_CarTableJoinEnd));
	for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
		msg.TableBetAreaGlobel[i] = m_TableBetAreaGlobel[i];
	msg.TableResultIndex = m_TableResultIndex;
	//m_CarResultLog.SetResult(m_TableAreaData[m_TableResult]);//���ݵ��ֵĽ����������

	//���ýṹ
	m_CarResultLog.SetResult(m_TableResult);

	msg.TableGameSum = m_CarResultLog.LogSum;
	msg.TableResultLog = m_CarResultLog.ResultLog;
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
		for (BYTE i = 0; i < MAX_CARSHOWBRAND_Sum; ++i)
		{
			msg.BankerListGlobelInfo[i].dwUserID = 0;
			msg.BankerListGlobelInfo[i].dwGlobelSum = 0;
		}
	}
	else
	{
		vector<DWORD>::iterator Iter = m_TableWriteBankerVec.begin();
		for (BYTE i = 0; Iter != m_TableWriteBankerVec.end() && i < MAX_CARSHOWBRAND_Sum;)
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
	for (BYTE i = 0; i < MAX_CARVIPSEAT_Sum; ++i)
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
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
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
			for (BYTE i = 0; i < MAX_CAR_ClientSum; ++i)
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
				if (m_VipSeatVec[Iter->second.VipSeat].NonBetGlobelSum >= g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.VipNonBetSum)
				{
					if ((pRole->GetRoleInfo().ParticularStates & ParticularState_CarVip) != 0)
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
		m_SystemGlobelSum = static_cast<INT64>(m_SystemGlobelSum * g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.GetChangeGlobelRateValue());

	if (m_TableBankerUserID != 0)
	{
		if (BrandUserGlobel >= 0)
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_Car, 1, static_cast<DWORD>(BrandUserGlobel), TEXT("�����ׯ�����ۻ�ý��"), SendLogDB);
		else
			g_DBLogManager.LogToDB(m_TableBankerUserID, LT_Car, 0, static_cast<DWORD>(BrandUserGlobel*-1), TEXT("�����ׯ������ʧȥ���"), SendLogDB);
	}
	//�洢�����ݿ�����ȥ
	g_DBLogManager.LogCarInfoToDB(m_TableBankerUserID, m_TableBetAreaGlobel, m_TableResult, SystemGlobel, m_SystemGlobelSum, m_TableUserMap.size(), SendLogDB);
	//�жϵ�ǰ���Ŷ��б������� 
	MG_Cmd_CarWriteBankerUserGlobelMsg msgSend;
	SetMsgInfo(msgSend, GetMsgType(Main_Car, MG_CarWriteBankerUserGlobelMsg), sizeof(MG_Cmd_CarWriteBankerUserGlobelMsg));
	vector<DWORD>::iterator IterVec = m_TableWriteBankerVec.begin();
	for (; IterVec != m_TableWriteBankerVec.end(); ++IterVec)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(*IterVec);
		if (!pRole)
			continue;
		if (pRole->GetGlobelSum() < g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.JoinBankerGlobelSum)
		{
			msgSend.dwUserID = *IterVec;
			pRole->SendDataToGameServer(&msgSend);
		}
	}
	m_robotmanager.EndGame();
}
SHORT CarManager::GetBrandRateValue(BYTE Index)
{
	if (Index >= MAX_CAR_ClientSum)
		return 1;
	if (GetResultID() == Index)
	{
		//��ׯ�Ҵ��ʱ��
		BYTE Rate = GetResultRate();
		return static_cast<SHORT>(Rate);
	}
	else
	{
		return 0;
	}
}
void CarManager::OnRoleJoinVipSeat(DWORD dwUserID, BYTE VipSeatIndex)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
	if (Iter == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}
	MG_Cmd_CarRoleJoinVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleJoinVipSeat), sizeof(MG_Cmd_CarRoleJoinVipSeat));
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
	if (VipSeatIndex >= MAX_CARVIPSEAT_Sum)
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
				MG_Cmd_CarRoleBeLeaveVipSeat msgLeave;
				SetMsgInfo(msgLeave, GetMsgType(Main_Car, MG_CarRoleBeLeaveVipSeat), sizeof(MG_Cmd_CarRoleBeLeaveVipSeat));
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
	HashMap<DWORD, tagCarRoleInfo>::iterator IterSend = m_TableUserMap.begin();//Vipϯλ�ĸı䷢�͸�ȫ�����
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void CarManager::OnRoleLeaveVipSeat(DWORD dwUserID)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.find(dwUserID);
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

	MG_Cmd_CarRoleLeaveVipSeat msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarRoleLeaveVipSeat), sizeof(MG_Cmd_CarRoleLeaveVipSeat));
	msg.dwDestUserID = dwUserID;
	msg.VipSeatIndex = Iter->second.VipSeat;
	HashMap<DWORD, tagCarRoleInfo>::iterator IterSend = m_TableUserMap.begin();
	for (; IterSend != m_TableUserMap.end(); ++IterSend)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(IterSend->first);
		if (!pRole)
			continue;
		msg.dwUserID = IterSend->first;
		pRole->SendDataToGameServer(&msg);
	}
}
void CarManager::OnRoleGetNormalInfoByPage(DWORD dwUserID, DWORD Page)
{
	Role* pRole = g_FishServer.GetRoleManager().QueryRole(dwUserID);
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator IterRole = m_TableUserMap.find(dwUserID);
	if (IterRole == m_TableUserMap.end())
	{
		ASSERT(false);
		return;
	}

	DWORD BeginIndex = Page * MAX_CARNORMAL_PAGESUM;
	DWORD EndIndex = (Page + 1)* MAX_CARNORMAL_PAGESUM - 1;


	MG_Cmd_CarGetNormalSeatInfo msg;
	SetMsgInfo(msg, GetMsgType(Main_Car, MG_CarGetNormalSeatInfo), sizeof(MG_Cmd_CarGetNormalSeatInfo));
	msg.dwUserID = dwUserID;
	msg.Page = Page;
	msg.TotalRoleSum = m_TableUserMap.size();

	for (BYTE i = 0; i < MAX_CARVIPSEAT_Sum; ++i)
	{
		if (m_VipSeatVec[i].dwUserID != 0)
			--msg.TotalRoleSum;
	}
	if (m_TableBankerUserID != 0)
		--msg.TotalRoleSum;

	for (BYTE i = 0; i < MAX_CARNORMAL_PAGESUM; ++i)
	{
		msg.Array[i].dwFaceID = 0;
		msg.Array[i].dwUserID = 0;
		msg.Array[i].GlobelSum = 0;
		msg.Array[i].NickName[0] = 0;
	}
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
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
BYTE CarManager::GetResultID()
{
	//��ע�� �� 0��ʼ �� 7  һ��8������
	//��ȡ��ע��������ID
	return m_TableResult;
}
BYTE CarManager::GetResultRate()
{
	//���ݽ�� ��ȡ����
	HashMap<BYTE, tagCarType>::iterator Iter =  g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarTypeMap.find(GetResultID());
	if (Iter == g_FishServer.GetFishConfig().GetFishMiNiGameConfig().carConfig.CarTypeMap.end())
	{
		return 0;
	}
	else
	{
		return  Iter->second.RateValue;
	}
}


DWORD CarManager::BankerId()
{
	return m_TableBankerUserID;
}

WORD CarManager::ApplyListLength()
{
	return m_TableWriteBankerVec.size();
}

DWORD CarManager::BankerGold()
{
	Role* pBankerRole = g_FishServer.GetRoleManager().QueryRole(m_TableBankerUserID);
	if (pBankerRole)
	{
		return pBankerRole->GetGlobelSum();
	}
	return 0;
}
bool	CarManager::IsGameEnd()
{
	return m_TableStates == CTS_End;
}

bool	CarManager::HaveApply(DWORD dwUserid)
{
	return find(m_TableWriteBankerVec.begin(), m_TableWriteBankerVec.end(), dwUserid) != m_TableWriteBankerVec.end();
}

void CarManager::QueryNames(DWORD ClientID)
{
	int nNamelength = (MAX_NICKNAME + 1);
	int nSize = sizeof(LC_CMD_CarAdmin_PlayerNames)+sizeof(TCHAR)*nNamelength*(m_TableUserMap.size() - m_robotmanager.RobotCount());
	LC_CMD_CarAdmin_PlayerNames *pResult = (LC_CMD_CarAdmin_PlayerNames*)malloc(nSize);
	pResult->SetCmdSize(nSize);
	pResult->SetCmdType((Main_Control << 8) | LC_CarAdminQueryName);
	pResult->ClientID = ClientID;
	int nIndex = 0;
	for (HashMap<DWORD, tagCarRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole&&!m_robotmanager.IsRobot(pRole->GetRoleID()))
		{
			TCHARCopy(&pResult->names[1 + nIndex*nNamelength], nNamelength, pRole->GetNickName(), MAX_NICKNAME);
			nIndex++;
		}
	}
	g_FishServer.SendNetCmdToControl(pResult);
	free(pResult);
}

void CarManager::QueryAllJetton(DWORD ClientID)
{
	LC_CMD_CarAdmin_AllJetton result;
	ZeroMemory(&result, sizeof(result));
	result.SetCmdSize(sizeof(result));
	result.SetCmdType((Main_Control << 8) | LC_CarAdminQueryAllJetton);
	result.ClientID = ClientID;
	result.nStock = m_SystemGlobelSum;
	CalPlayerJetton(result.nJetton);
	g_FishServer.SendNetCmdToControl(&result);

}

void CarManager::QueryPlayerJetton(TCHAR *pNickame, DWORD ClientID)
{
	for (HashMap<DWORD, tagCarRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		Role* pRole = g_FishServer.GetRoleManager().QueryRole(it->first);
		if (pRole)
		{
			if (_tcscmp(pNickame, pRole->GetNickName()) == 0)
			{
				//HashMap<DWORD, tagDialRoleInfo>::iterator itJetton = m_TableUserMap.find(pRole->GetRoleID());
				//if (itJetton != m_TableUserMap.end())
				{
					LC_CMD_CarAdmin_PlayerJetton result;
					result.SetCmdSize(sizeof(result));
					result.SetCmdType((Main_Control << 8) | LC_CarAdminQueryPlayerJetton);
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

void CarManager::CalPlayerJetton(UINT64 uJetton[MAX_CAR_ClientSum])
{
	for (HashMap<DWORD, tagCarRoleInfo>::iterator it = m_TableUserMap.begin(); it != m_TableUserMap.end(); it++)
	{
		if (!m_robotmanager.IsRobot(it->first))
		{
			for (int i = 0; i < MAX_CAR_ClientSum; i++)
			{
				uJetton[i] += it->second.BetGlobel[i];
			}
		}
	}
}
bool CarManager::IsExitsNormalRole()
{
	HashMap<DWORD, tagCarRoleInfo>::iterator Iter = m_TableUserMap.begin();
	for (; Iter != m_TableUserMap.end(); ++Iter)
	{
		if (!Iter->second.IsRobot)
			return true;
	}
	return false;
}