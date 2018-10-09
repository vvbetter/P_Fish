#include "stdafx.h"
#include "GameRobot.h"
#include "FishServer.h"
#include "FishLogic\NetCmd.h"
GameRobotManager::GameRobotManager()
{
	m_Robots.clear();
	m_WriteList.clear();
	m_IsLoadFinish = false;
	m_GetIndex = 0;
}
GameRobotManager::~GameRobotManager()
{
	for (auto IterVec = m_Robots.begin() ; IterVec != m_Robots.end(); ++IterVec)
	{
		SAFE_DELETE(*IterVec);
	}
	m_Robots.clear();
	m_WriteList.clear();
}
void GameRobotManager::OnLoadAllGameRobot(DWORD BeginUserID, DWORD EndUserID)
{
	if (BeginUserID == 0 || EndUserID == 0)
		return;//���������
	Log("���ػ����ˣ�beginID=%d,endID=%d", BeginUserID, EndUserID);
	//�����˵Ĺ����� ����ǰGameServer��ȫ���Ļ����� 
	//��ʼ��  ��Ҫ֪�������˵����� ���Ǵ��������Ļ����� ���д��� �����ݿ��ȡ����  
	DBR_Cmd_GetAccountInfoByUserID msg;
	memset(&msg, 0, sizeof(DBR_Cmd_GetAccountInfoByUserID));
	SetMsgInfo(msg, DBR_GetAccountInfoByUserID, sizeof(DBR_Cmd_GetAccountInfoByUserID));
	//��ʼ����������Ϣ
	msg.ClientID = 0;
	msg.LogonByGameServer = false;
	msg.udata.money1 = 0;
	msg.udata.money2 = 0;
	for (size_t i = BeginUserID; i <= EndUserID; ++i)
	{
		uint RobotName_index = RandUInt() % MAX_ROBOTNAME_COUNT;
		memset(msg.udata.name, 0, MAX_NICKNAME + 1);
		memcpy_s(msg.udata.name, MAX_NICKNAME, g_RobotName[RobotName_index].c_str(), MAX_NICKNAME);
		uint iconIndex = RandUInt() % 12 + 1;
		memset(msg.udata.icon, 0, ICON_LENGTH);
		sprintf_s(msg.udata.icon, "%s%d", "head_female", iconIndex);
		msg.udata.sex = RandUInt() % 2;
		msg.udata.vip = RandUInt() % 6;
		msg.udata.headIcon = 0;
		//�����û���������
		msg.dwUserID = i;
		msg.Uid = i;
		g_FishServer.SendNetCmdToDB(&msg);//��������
	}
	m_RobotConfigSum = EndUserID - BeginUserID + 1;
}
void GameRobotManager::OnAddRobot(DWORD dwUserID, CRoleEx* pRoleEx)
{
	//�����˼������
	if (!pRoleEx)
	{
		ASSERT(false);
		return;
	}
	GameRobot* pRobot = new GameRobot(this, pRoleEx);
	if (!pRobot)
	{
		ASSERT(false);
		return;
	}
	m_Robots.push_back(pRobot);
	if (m_Robots.size() == m_RobotConfigSum && !m_IsLoadFinish)
	{
		m_IsLoadFinish = true;
		Log("�����˼������");
	}
	return;
}
GameRobot* GameRobotManager::GetFreeRobot(DWORD RobotID, GameTable* pTable)
{
	//��ȡһ�����еĻ�����
	if (m_Robots.empty())
		return NULL;
	GameRobot* pRobot = NULL;
	for (auto it = m_Robots.begin() + (m_GetIndex%m_RobotConfigSum);;)
	{
		if (it == m_Robots.end())
		{
			it = m_Robots.begin();
		}
		if (!(*it)->IsRobotUsed())
		{
			pRobot = *it;
			break;
		}
		++it;
		if (it == m_Robots.begin() + (m_GetIndex%m_RobotConfigSum))
		{
			break;
		}
	}
	if (!pRobot)
		return NULL;
	//��ȡ������ͽ���
	int64 tableMinGold = 0;
	const auto& tableInfo = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig;
	const auto tableInfoIt = tableInfo.find(pTable->GetTableTypeID());
	if (tableInfoIt != tableInfo.end())
	{
		tableMinGold = tableInfoIt->second.MinGlobelSum * g_FishServer.GetRatioValue() * MONEY_RATIO;
	}
	//��ʼ�������˵�Ǯ�������ƽ����0.8-1.5��
	int64 avgGold = pTable->GetTablePlayerAvgMoney();
	if (tableMinGold == 0 || tableMinGold > avgGold)
	{
		pRobot->GetRoleInfo()->GetRoleInfo().money1 = tableMinGold * ((RandUInt() % 10) + 8) / 10;
	}
	else
	{
		pRobot->GetRoleInfo()->GetRoleInfo().money1 = avgGold * ((RandUInt() % 10) + 8) / 10;
	}
	pRobot->GetRoleInfo()->GetRoleInfo().money2 = 0;
	pRobot->SetRobotUse(RobotID,pTable->GetTableMonthID());//���û������Ѿ�ʹ����
	m_GetIndex++;
	return pRobot;
}
void GameRobotManager::ResetGameRobot(DWORD dwUserID)
{
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		if ((*it)->GetRobotUserID() == dwUserID)
		{
			(*it)->ResetRobot();
			break;
		}
	}
	return;
}

void GameRobotManager::ResetAllGameRobot()
{
	//������������Ļ������뿪����,��ֱ��ɾ�������뵽ɾ��������
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		g_FishServer.GetTableManager()->OnPlayerLeaveTable((*it)->GetRobotUserID());
		(*it)->ResetRobot();
	}
	m_WriteList.clear();
}
bool GameRobotManager::GameRobotIsCanJoinTable(GameTable* pTable)
{
	//�жϻ������Ƿ���Լ��뵱ǰ����
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	if (pTable->IsFull())
		return false;
	return true;
}
void GameRobotManager::OnJoinRobotToTable(GameTable * pTable, INT robotNum)
{
	//����ID �� ����ID �������ɻ����˵�����ID
	DWORD Key = (pTable->GetTableTypeID() << 16) + (pTable->GetTableMonthID() & 0x7f);
	multimap<DWORD, DWORD>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.find(Key);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.end())
		return;
	DWORD RobotID = Iter->second;
	JoinRobot(RobotID, pTable, robotNum);
}
void GameRobotManager::OnRoleLeaveNormalRoom(GameTable* pTable)
{
	//����ͨ����뿪һ�������ʱ�� �ж����������û����ͨ����� ȫ���Ļ������뿪
	if (!pTable || pTable->GetTableMonthID() != 0)
		return;
	BYTE RoleSum = 0;
	for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
			continue;
		if (!pRole->GetRoleExInfo()->IsRobot())
			++RoleSum;
	}
	if (RoleSum == 0)
	{
		for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
		{
			CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
			if (!pRole || !pRole->IsActionUser())
				continue;
			if (pRole->GetRoleExInfo()->IsRobot())
			{
				//���������Ϸ��¼
				CRoleEx* pRoleEx = pRole->GetRoleExInfo();
				DBR_Cmd_SaveRecord recordMsg;
				SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
				recordMsg.model = 2;
				recordMsg.uid = pRoleEx->GetRoleInfo().Uid;
				recordMsg.table_id = 0;
				recordMsg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
				recordMsg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
				recordMsg.leave_code = 2;
				g_FishServer.SendNetCmdToDB(&recordMsg);
				//�黹������
				DWORD dwUserID = pRole->GetID();
				g_FishServer.GetTableManager()->OnPlayerLeaveTable(dwUserID);
				ResetGameRobot(dwUserID);//�黹������
			}
		}
	}
}
void GameRobotManager::Update()
{
	if (!m_IsLoadFinish)
	{
		return;
	}
	UpdateWriteList();
	for (auto it = m_Robots.begin(); it != m_Robots.end(); ++it)
	{
		if ((*it)->IsRobotUsed())
		{
			(*it)->Update();
		}
	}
}
void GameRobotManager::AdddWriteRobot(WORD TableID, DWORD WriteTime)
{
	tagRobotWrite pInfo;
	pInfo.TableID = TableID;
	pInfo.TimeLog = WriteTime;
	m_WriteList.push_back(pInfo);
}
void GameRobotManager::OnAllRobotLeaveTable(GameTable * pTable)
{
	for (BYTE i = 0; i < pTable->GetRoleManager().GetMaxPlayerSum(); ++i)
	{
		CRole* pRole = pTable->GetRoleManager().GetRoleBySeatID(i);
		if (!pRole || !pRole->IsActionUser())
			continue;
		if (pRole->GetRoleExInfo()->IsRobot())
		{
			//���������Ϸ��¼
			CRoleEx* pRoleEx = pRole->GetRoleExInfo();
			DBR_Cmd_SaveRecord recordMsg;
			SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
			recordMsg.model = 2;
			recordMsg.uid = pRoleEx->GetRoleInfo().Uid;
			recordMsg.table_id = 0;
			recordMsg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
			recordMsg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
			recordMsg.leave_code = 2;
			g_FishServer.SendNetCmdToDB(&recordMsg);
			//�黹������
			DWORD dwUserID = pRole->GetID();
			g_FishServer.GetTableManager()->OnPlayerLeaveTable(dwUserID);
			ResetGameRobot(dwUserID);//�黹������
		}
	}
}
bool GameRobotManager::JoinRobot(DWORD robotTypeID, GameTable * pTable, INT robotNum)
{
	if (robotNum == -1)
	{
		robotNum = (RandUInt() % 99999) % 2 + 1;
	}
	for (INT i = 0; i < robotNum; ++i)
	{
		GameRobot* pRobot = GetFreeRobot(robotTypeID, pTable);//��ȡһ�����еĻ�����
		if (!pRobot)
			return false;
		CRoleEx* pRole = pRobot->GetRoleInfo();
		if (!pRole)
			return false;
		if (!g_FishServer.GetTableManager()->OnPlayerJoinTable(pTable->GetTableID(), pRole))
		{
			pRobot->ResetRobot();
			return false;
		}
	}
	return true;
}
void GameRobotManager::UpdateWriteList()
{
	if (m_WriteList.empty())
		return;
	DWORD Time = timeGetTime();
	list<tagRobotWrite>::iterator Iter = m_WriteList.begin();
	for (; Iter != m_WriteList.end();)
	{
		if (Time > Iter->TimeLog)
		{
			GameTable* pTable = g_FishServer.GetTableManager()->GetTable(Iter->TableID);
			if (!pTable)
			{
				Log("�Ѿ�û����������� id = %d", Iter->TableID);
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			if ( pTable->GetTablePlayerSum() == 4)
			{
				Log("�������� id = %d", Iter->TableID);
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			if (!pTable->IsTableRunning() && pTable->GetTableMonthID() == 0)
			{
				Log("��ͨ���ӿյ� id = %d", Iter->TableID);
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			if (pTable->GetTableMonthID() != 0 && pTable->IsTableRunning())
			{
				Log("�������������Ѿ���ʼ��Ϸ�� id =%d", Iter->TableID);
				Iter = m_WriteList.erase(Iter);
				continue;
			}
			DWORD Key = (pTable->GetTableTypeID() << 16) + (pTable->GetTableMonthID() & 0x7f);
			multimap<DWORD, DWORD>::iterator IterFind = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.find(Key);
			if (IterFind == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotIndexMap.end())
				return;
			DWORD RobotID = IterFind->second;

			GameRobot* pRobot = GetFreeRobot(RobotID, pTable);
			if (pRobot)
			{
				//����ɹ�֪ͨ�ͻ��˻����˽��룬��Ȼ���ͷŻ�����
				if (g_FishServer.GetTableManager()->OnPlayerJoinTable(Iter->TableID, pRobot->GetRoleInfo()))
				{
					pTable->SendRoleJoinInfo(pRobot->GetRobotUserID());
					//����ɹ���50%���ʿ��Լ������������
					if (RandFloat() > 0.5f && pTable->GetTableMonthID() == 0 )
					{
						Iter->TimeLog = Time + ((RandUInt() % 99999) % 60) * 1000;
					}
					else
					{
						Iter = m_WriteList.erase(Iter);
						continue;
					}
				}
				else //����ʧ�ܣ����û�����
				{
					pRobot->ResetRobot();
					Iter = m_WriteList.erase(Iter);
					continue;
				}
			}
		}
		++Iter;
	}
}

//���������˴���
GameRobot::GameRobot(GameRobotManager* pManager, CRoleEx* pRole) :m_pManager(pManager), m_IsUse(false), m_pRole(pRole)
{
	if (!pRole || !m_pManager)
	{
		ASSERT(false);
		return;
	}
	m_FishValue = 0;
	m_LockFishID = -1;
	m_RobotID = 0;
	//
	m_LeaveTableTimeLog = 0;
	m_RoomTimeLog = 0;
	//
	m_NowRate = 0xff;
	m_ChangeRateTimeLog = 0;
	m_RateTimeLog = 0;
	//
	m_UseSkillTimeLog = 0;
	m_SkillTimeLog = 0;
	//
	m_ChangeLauncherTimeLog = 0;
	m_LauncherTimeLog = 0;
	//
	m_OpenFireTimeLog = 0;
	//
	m_StopOpenFireTimeLog = 0;
	//
	m_RobotInfoTimeLog = 0;
	//
	m_IsOpenFire = false;
	m_RobotOpenFireTimeLog = 0;
	//
	m_RobotSendCharmItemTimeLog = 0;
	m_dwRobotSendCharmTimeLog = 0;
	m_vecSendCharmArray.clear();
	//
	m_dwSendCharmTineLog = 0;
	//
	m_RobotCharmEvent.Clear();
}
GameRobot::~GameRobot()
{

}
void GameRobot::SetRobotUse(DWORD RobotID, BYTE MonthID)
{
	if (m_IsUse)
	{
		ASSERT(false);
		return;
	}
	m_RobotID = RobotID;
	m_monthID = MonthID;
	m_IsUse = true;
}
void GameRobot::ResetRobot()
{
	if (!m_IsUse)
	{
		ASSERT(false);
		return;
	}
	m_FishValue = 0;
	m_LockFishID = -1;
	m_RobotID = 0;
	m_IsUse = false;
	m_monthID = 0;
	//
	m_LeaveTableTimeLog = 0;
	m_RoomTimeLog = 0;
	//
	m_NowRate = 0xff;
	m_ChangeRateTimeLog = 0;
	m_RateTimeLog = 0;
	//
	m_UseSkillTimeLog = 0;
	m_SkillTimeLog = 0;
	//
	m_ChangeLauncherTimeLog = 0;
	m_LauncherTimeLog = 0;
	//
	m_OpenFireTimeLog = 0;
	//
	m_StopOpenFireTimeLog = 0;
	//
	m_RobotInfoTimeLog = 0;
	//
	m_IsOpenFire = false;
	m_RobotOpenFireTimeLog = 0;

	//
	m_RobotSendCharmItemTimeLog = 0;
	m_dwRobotSendCharmTimeLog = 0;
	m_vecSendCharmArray.clear();
	//
	m_dwSendCharmTineLog = 0;
	//
	m_RobotCharmEvent.Clear();
}
DWORD GameRobot::GetRobotUserID()
{
	if (!m_pRole)
		return 0;
	return m_pRole->GetUserID();
}
CRoleEx* GameRobot::GetRoleInfo()
{
	return m_pRole;
}
void GameRobot::Update()
{
	DWORD tickNow = timeGetTime();
	UpdateRobotRoom(tickNow);
	UpdateRobotRate(tickNow);
	//UpdateRobotSkill();
	UpdateRobotLauncher(tickNow);
	UpdateRobotOpenFire(tickNow);
	UpdateRobotIsCanOpenFire(tickNow);
}
void GameRobot::UpdateRobotRoom(DWORD tickNow)
{
	if (m_RoomTimeLog != 0 && tickNow - m_RoomTimeLog < 5000)
		return;
	m_RoomTimeLog = tickNow;
	//���»����˽�������
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	//�������䲻�뿪
	if (m_monthID != 0)
	{
		return;
	}
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());
	if (!pRole)//��Ҳ���������
		return;
	HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	//û��Ǯ���û������뿪����
	int64 robotMoney = m_pRole->GetRoleInfo().money1 + m_pRole->GetRoleInfo().money2;
	auto it = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pRole->GetTableType());
	double tableBasicRate = 0.00;
	if (it != g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		tableBasicRate = it->second.BasicRatio;
	}
	CConfig* m_pConfig = g_FishServer.GetTableManager()->GetGameConfig();
	int nConsume = (int)(m_pConfig->BulletConsume(pRole->GetLauncherType())	 // ÿ���ڵĻ�������
		*m_pConfig->BulletMultiple(pRole->GetRateIndex()) //��ҵ����ı���
		*g_FishServer.GetRatioValue()				 // �����Ļ��ұ���
		*tableBasicRate					 // ���ӵĻ������� 
		*MONEY_RATIO);
	//�����ٷ�һ���û������ӳ�2���뿪����
	if (robotMoney < nConsume)
	{
		m_LeaveTableTimeLog = 1 ;
	}
	else
	{
		if (!Iter->second.RoomInfo.IsOpen)//Ϊ�����Ļ� ��ʾ���һֱ����������
			return;
	}
	if (m_LeaveTableTimeLog == 0)
	{
		DWORD JoinSec = RandUInt() % (Iter->second.RoomInfo.RoomInfo.OpenMaxSec - Iter->second.RoomInfo.RoomInfo.OpenMinSec) + Iter->second.RoomInfo.RoomInfo.OpenMinSec;//�ڷ����������
		m_LeaveTableTimeLog = tickNow + JoinSec * 1000;
	}
	if (tickNow > m_LeaveTableTimeLog)
	{
		//���������Ϸ��¼
		CRoleEx* pRoleEx = pRole->GetRoleExInfo();
		DBR_Cmd_SaveRecord recordMsg;
		SetMsgInfo(recordMsg, DBR_Save_battle_Record, sizeof(DBR_Cmd_SaveRecord));
		recordMsg.model = 2;
		recordMsg.uid = pRoleEx->GetRoleInfo().Uid;
		recordMsg.table_id = 0;
		recordMsg.enter_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_money = (pRoleEx->GetRoleInfo().money1 + pRoleEx->GetRoleInfo().money2) / MONEY_RATIO;
		recordMsg.leave_code = 2;
		g_FishServer.SendNetCmdToDB(&recordMsg);
		//��������Ҫ�뿪������
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(GetRobotUserID());//�û������뿪����
		//���û���
		DWORD LeaveSec = RandUInt() % (Iter->second.RoomInfo.RoomInfo.StopMaxSec - Iter->second.RoomInfo.RoomInfo.StopMinSec) + Iter->second.RoomInfo.RoomInfo.StopMinSec;
		m_pManager->AdddWriteRobot(pRole->GetTableID(), tickNow + LeaveSec * 1000);
		//���ջ�����
		ResetRobot();
	}
}
void GameRobot::UpdateRobotRate(DWORD tickNow)
{
	if (m_RateTimeLog != 0 && tickNow - m_RateTimeLog < 300)
		return;
	m_RateTimeLog = tickNow;
	//���»������л�����
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
	if (!pUser)
		return;
	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
	if (!pTable)
		return;
	//���������ʵ���Ϊ3-6-9 , index = 2,5,8
	if (m_monthID != 0)
	{
		//TODO
		if (pUser->GetBulletCount() <= 400 && pUser->GetBulletCount() > 200 && pUser->GetRateIndex() != 5)
		{
			NetCmdChangeRateType msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
			msg.Seat = pUser->GetSeatID();
			msg.RateIndex = 5;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
		}
		else if (pUser->GetBulletCount() <= 200 && pUser->GetBulletCount() > 0 && pUser->GetRateIndex() != 8)
		{
			NetCmdChangeRateType msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
			msg.Seat = pUser->GetSeatID();
			msg.RateIndex = 8;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
		}
		return;
	}
	if (m_NowRate == 0xff)
	{
		if (m_ChangeRateTimeLog == 0)
		{
			HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
			if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
				return;

			DWORD RateF = RandUInt() % Iter->second.RateInfo.MaxRateF;
			vector<tagRobotRateType>::iterator IterVec = Iter->second.RateInfo.RateVec.begin();
			for (; IterVec != Iter->second.RateInfo.RateVec.end(); ++IterVec)
			{
				if (RateF < IterVec->RateF)
				{
					//�л�����ǰ����
					//������һ���л���ָ������ ���� һ��һ������ �м���
					m_NowRate = IterVec->RateType;
					break;
				}
			}
			HashMap<BYTE, TableInfo>::iterator IterTable = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pTable->GetTableTypeID());
			if (IterTable == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
				return;
			if (m_NowRate < IterTable->second.MinRate)
				m_NowRate = IterTable->second.MinRate;
			if (m_NowRate > IterTable->second.MaxRate)
				m_NowRate = IterTable->second.MaxRate;
			NetCmdChangeRateType msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
			msg.Seat = pUser->GetSeatID();
			msg.RateIndex = m_NowRate;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
			m_NowRate = 0xff;


			DWORD JoinSec = RandUInt() % (Iter->second.RateInfo.RateInfo.OpenMaxSec - Iter->second.RateInfo.RateInfo.OpenMinSec) + Iter->second.RateInfo.RateInfo.OpenMinSec;//�л����ʵ�CD
			m_ChangeRateTimeLog = tickNow + JoinSec * 1000;
		}
		else if (tickNow >= m_ChangeRateTimeLog)
		{
			HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
			if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
				return;

			if (!Iter->second.RateInfo.IsOpen)
				return;

			DWORD RateF = RandUInt() % Iter->second.RateInfo.MaxRateF;
			vector<tagRobotRateType>::iterator IterVec = Iter->second.RateInfo.RateVec.begin();
			for (; IterVec != Iter->second.RateInfo.RateVec.end(); ++IterVec)
			{
				if (RateF < IterVec->RateF)
				{
					//�л�����ǰ����
					//������һ���л���ָ������ ���� һ��һ������ �м���
					m_NowRate = IterVec->RateType;
					break;
				}
			}

			//�޸ĵ�ǰ����
			HashMap<BYTE, TableInfo>::iterator IterTable = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(pTable->GetTableTypeID());
			if (IterTable == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
				return;
			if (m_NowRate < IterTable->second.MinRate)
				m_NowRate = IterTable->second.MinRate;
			if (m_NowRate > IterTable->second.MaxRate)
				m_NowRate = IterTable->second.MaxRate;

			DWORD JoinSec = RandUInt() % (Iter->second.RateInfo.RateInfo.OpenMaxSec - Iter->second.RateInfo.RateInfo.OpenMinSec) + Iter->second.RateInfo.RateInfo.OpenMinSec;//�л����ʵ�CD
			m_ChangeRateTimeLog = tickNow + JoinSec * 1000;
		}
	}
	else
	{
		if (pUser->GetRateIndex() == m_NowRate)
		{
			m_NowRate = 0xff;
			return;
		}
		//����ҵ�ǰ���� �� ָ�����ʽ����л�
		BYTE RateIndex = (m_NowRate < pUser->GetRateIndex() ? pUser->GetRateIndex() - 1 : pUser->GetRateIndex() + 1);
		NetCmdChangeRateType msg;
		SetMsgInfo(msg, CMD_CHANGE_RATE_TYPE, sizeof(msg));
		msg.Seat = pUser->GetSeatID();
		msg.RateIndex = RateIndex;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
		return;
	}
}
void GameRobot::UpdateRobotSkill()
{
	if (m_SkillTimeLog != 0 && timeGetTime() - m_SkillTimeLog < 1000)
		return;
	m_SkillTimeLog = timeGetTime();
	//������ʹ�ü��� ��ͨ��
	CRole* pRole = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
	if (!pRole)
		return;
	if (m_UseSkillTimeLog == 0)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		DWORD JoinSec = RandUInt() % (Iter->second.SkillInfo.SkillInfo.OpenMaxSec - Iter->second.SkillInfo.SkillInfo.OpenMinSec) + Iter->second.SkillInfo.SkillInfo.OpenMinSec;//�л������ܹ���CD
		m_UseSkillTimeLog = timeGetTime() + JoinSec * 1000;//�´�ʹ�ü��ܵ�CD
	}
	else if (timeGetTime() >= m_UseSkillTimeLog)
	{
		GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pRole->GetTableID());
		if (!pTable)
			return;
		if (pTable->GetFishDesk()->GetFishSum() < 80)
			return;
		if ( pTable->GetFishDesk()->GetFishSum() < 20)
			return;

		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;

		if (!Iter->second.SkillInfo.IsOpen)
			return;

		//ʹ�ü���
		BYTE SkillID = g_FishServer.GetTableManager()->GetGameConfig()->CannonOwnSkill(pRole->GetLauncherType());

		NetCmdUseSkill msg;
		SetMsgInfo(msg, CMD_USE_SKILL, sizeof(msg));
		msg.SkillID = SkillID;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);

		

		DWORD JoinSec = RandUInt() % (Iter->second.SkillInfo.SkillInfo.OpenMaxSec - Iter->second.SkillInfo.SkillInfo.OpenMinSec) + Iter->second.SkillInfo.SkillInfo.OpenMinSec;//�л������ܹ���CD
		m_UseSkillTimeLog = timeGetTime() + JoinSec * 1000;//�´�ʹ�ü��ܵ�CD

		m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog,timeGetTime() + 3000);
	}
}
void GameRobot::UpdateRobotLauncher(DWORD tickNow)
{
	if (m_LauncherTimeLog != 0 && tickNow - m_LauncherTimeLog < 1000)
		return;
	if (m_monthID != 0)
		return;
	m_LauncherTimeLog = tickNow;
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
	if (!pUser)
		return;
	//�л��ڵ�CD
	if (m_ChangeLauncherTimeLog == 0)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		DWORD LauncherCDSec = RandUInt() % (Iter->second.LauncherInfo.LauncherInfo.OpenMaxSec - Iter->second.LauncherInfo.LauncherInfo.OpenMinSec) + Iter->second.LauncherInfo.LauncherInfo.OpenMinSec;
		m_ChangeLauncherTimeLog = tickNow + LauncherCDSec * 1000;
		//��ҽ�����������һ�ֿ���ʹ�õ���
		DWORD RateF = RandUInt() % Iter->second.LauncherInfo.MaxRateF;
		vector<tagRobotLauncherType>::iterator IterVec = Iter->second.LauncherInfo.LauncherVec.begin();
		for (; IterVec != Iter->second.LauncherInfo.LauncherVec.end(); ++IterVec)
		{
			if (RateF < IterVec->RateF)
			{
				BYTE LauncherType = IterVec->LauncherType;
				while (LauncherType >0 && !m_pRole->GetRoleLauncherManager().IsCanUserLauncherByID(LauncherType))
				{
					--LauncherType;
				}

				//�л�����ǰ��̨
				NetCmdChangeLauncherType change;//����ǰ2s �����Է����ӵ�
				SetMsgInfo(change, CMD_CHANGE_LAUNCHER_TYPE, sizeof(change));
				change.Seat = pUser->GetSeatID();
				change.LauncherType = LauncherType;
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &change);

				return;
			}
		}
	}
	else if (tickNow >= m_ChangeLauncherTimeLog)
	{
		HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
		if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
			return;
		if (!Iter->second.LauncherInfo.IsOpen)
			return;
		//�����л���̨��
		DWORD LauncherCDSec = RandUInt() % (Iter->second.LauncherInfo.LauncherInfo.OpenMaxSec - Iter->second.LauncherInfo.LauncherInfo.OpenMinSec) + Iter->second.LauncherInfo.LauncherInfo.OpenMinSec;
		m_ChangeLauncherTimeLog = tickNow + LauncherCDSec * 1000;

		DWORD RateF = RandUInt() % Iter->second.LauncherInfo.MaxRateF;
		vector<tagRobotLauncherType>::iterator IterVec = Iter->second.LauncherInfo.LauncherVec.begin();
		for (; IterVec != Iter->second.LauncherInfo.LauncherVec.end(); ++IterVec)
		{
			if (RateF < IterVec->RateF)
			{
				BYTE LauncherType = IterVec->LauncherType;
				while (LauncherType >0 && !m_pRole->GetRoleLauncherManager().IsCanUserLauncherByID(LauncherType))
				{
					--LauncherType;
				}

				//�л�����ǰ��̨
				NetCmdChangeLauncherType change;//����ǰ2s �����Է����ӵ�
				SetMsgInfo(change, CMD_CHANGE_LAUNCHER_TYPE, sizeof(change));
				change.Seat = pUser->GetSeatID();
				change.LauncherType = LauncherType;
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &change);

				m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog, tickNow + 1000);
				return;
			}
		}
	}
}
void GameRobot::UpdateRobotOpenFire(DWORD tickNow)
{
	if (m_OpenFireTimeLog == 0 || tickNow - m_OpenFireTimeLog > 120)
	{
		m_OpenFireTimeLog = tickNow;

		CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
		if (!pUser)
			return;
		
		//�ж�����Ƿ���Կ���
		if (m_NowRate != 0xff)
			return;
		if (!m_IsOpenFire)
			return;
		if (m_OpenFireTimeLog < m_StopOpenFireTimeLog)
			return;
		GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
		if (!pTable)
			return; 
		m_Angle = pTable->GetFishDesk()->GetAngleByFish(m_LockFishID, pUser->GetSeatID(), m_FishValue);
		if (m_LockFishID == 0 && m_Angle == 0xffff)
		{
			if (m_FishValue > 10 && RandFloat() > 0.750f)
			{
				m_StopOpenFireTimeLog = tickNow + 600 + static_cast<DWORD>(RandFloat() * 1000); //���������Ѿ������ˣ��ӳ�1-4��
			}
			m_LockFishID = -1; //��һ����Ҫ����ѡ��һ����
			return;
		}
		//��ҿ���
		if (!pUser->IsFullEnergy())//�����ӵ�
		{
			if (pUser->CheckFire(0))
			{
				//1.�����ӵ�
				NetCmdBullet msg;
				SetMsgInfo(msg, CMD_BULLET, sizeof(NetCmdBullet));
				msg.BulletID = 0;
				msg.Angle = m_Angle % 65535;
				msg.LockFishID = (pUser->GetLauncherType() == 4 ? 0 : m_LockFishID);
				msg.LauncherType = pUser->GetLauncherType();
				msg.Energy = (uint)pUser->GetEnergy();
				msg.ReboundCount = 0; //pUser->GetRoleExInfo()->GetRoleVip().GetLauncherReBoundNum();
				if (msg.LauncherType == 4)
					msg.ReboundCount = 0;
				//���������õ�GameServerȥ
				g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
			}
		}
		else//�������
		{
			NetCmdSkillLaser msgSkill;
			SetMsgInfo(msgSkill, CMD_SKILL_LASER_REQUEST, sizeof(msgSkill));
			msgSkill.Degree = m_Angle % 65535;
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msgSkill);

			m_StopOpenFireTimeLog = max(m_StopOpenFireTimeLog, tickNow + 2000);
		}
	}
}
void GameRobot::UpdateRobotIsCanOpenFire(DWORD tickNow)
{
	//���»������Ƿ���Կ���
	//�������������ں� ��Ҫֹͣһ��ʱ��
	if (tickNow <= m_RobotOpenFireTimeLog)
		return;
	HashMap<DWORD, tagGameRobotConfig>::iterator Iter = g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.find(m_RobotID);
	if (Iter == g_FishServer.GetFishConfig().GetFishGameRobotConfig().RobotMap.end())
		return;
	if (!Iter->second.OpenFireInfo.IsOpen)
	{
		m_IsOpenFire = true;
		return;
	}
	if (m_IsOpenFire)
	{
		m_IsOpenFire = false;
		//����ͣ�� �ȴ��´ο�ʼ
		DWORD StarSec = RandUInt() % (Iter->second.OpenFireInfo.FireInfo.StopMaxSec - Iter->second.OpenFireInfo.FireInfo.StopMinSec) + Iter->second.OpenFireInfo.FireInfo.StopMinSec;
		m_RobotOpenFireTimeLog = tickNow + StarSec * 1000;
	}
	else
	{
		m_IsOpenFire = true;
		DWORD StarSec = RandUInt() % (Iter->second.OpenFireInfo.FireInfo.OpenMaxSec - Iter->second.OpenFireInfo.FireInfo.OpenMinSec) + Iter->second.OpenFireInfo.FireInfo.OpenMinSec;
		m_RobotOpenFireTimeLog = tickNow + StarSec * 1000;
	}
}
