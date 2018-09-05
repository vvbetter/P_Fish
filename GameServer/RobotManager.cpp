#include "Stdafx.h"
#include "RobotManager.h"
#include "FishServer.h"
#include "FishLogic\NetCmd.h"
#include "RoleEx.h"
RobotManager::RobotManager()
{


}
RobotManager::~RobotManager()
{
	if (m_RobotMap.empty())
		return;
	HashMap<DWORD, Robot*>::iterator Iter = m_RobotMap.begin();
	for (; Iter != m_RobotMap.end(); ++Iter)
	{
		delete Iter->second;
	}
	m_RobotMap.clear();
}
void RobotManager::OnUpdate(DWORD dwTimer)
{
	if (m_RobotMap.empty())
		return;
	HashMap<DWORD, Robot*>::iterator Iter = m_RobotMap.begin();
	for (; Iter != m_RobotMap.end(); ++Iter)
	{
		if (!Iter->second->IsUse())
			continue;
		Iter->second->OnUpdate(dwTimer);
	}
}
void RobotManager::OnInit(DWORD BeginRobotUserID, DWORD EndRobotUserID)
{
	if (BeginRobotUserID == 0 || EndRobotUserID == 0)
		return;//���������
	//�����˵Ĺ����� ����ǰGameServer��ȫ���Ļ����� 
	//��ʼ��  ��Ҫ֪�������˵����� ���Ǵ��������Ļ����� ���д��� �����ݿ��ȡ����  
	DBR_Cmd_GetAccountInfoByUserID msg;
	SetMsgInfo(msg, DBR_GetAccountInfoByUserID, sizeof(DBR_Cmd_GetAccountInfoByUserID));
	msg.ClientID = 0;
	msg.LogonByGameServer = false;
	for (size_t i = BeginRobotUserID; i <= EndRobotUserID; ++i)
	{
		//�������������
		msg.dwUserID = i;
		g_FishServer.SendNetCmdToDB(&msg);//��������
	}
}
void RobotManager::OnAddRobot(DWORD dwUserID)
{
	CRoleEx* pRole = g_FishServer.GetRoleManager()->QueryUser(dwUserID);
	if (!pRole || !pRole->IsRobot())
	{
		ASSERT(false);
		return;
	}
	Robot* pRobot = new Robot;
	pRobot->OnInit(pRole);
	m_RobotMap.insert(HashMap<DWORD, Robot*>::value_type(pRole->GetUserID(), pRobot));//�������˱�������

	OnUseRobot(0, 0);
	//OnUseRobot(0xffff, 99);//�����˽��� ����
}
//������ ������� �� ���� ��ͨ����  �� ���� ָ�����ӵ�ID 
void RobotManager::OnUseRobot(WORD TableID, BYTE MonthID)
{
	Robot* pRobot = GetFreeRobot();
	if (!pRobot || pRobot->IsUse())
	{
		ASSERT(false);
		return;
	}
	pRobot->OnUseRobot(TableID, MonthID);
}
void RobotManager::OnDelRobot(DWORD dwUserID)
{
	HashMap<DWORD, Robot*>::iterator Iter = m_RobotMap.find(dwUserID);
	if (Iter == m_RobotMap.end())
	{
		ASSERT(false);
		return;
	}
	else
	{
		if (!Iter->second->IsUse())
			return;
		Iter->second->OnDelRobot();
		return;
	}
}
Robot* RobotManager::GetFreeRobot()
{
	//��ȡ һ�����еĻ�����
	HashMap<DWORD, Robot*>::iterator Iter = m_RobotMap.begin();
	for (; Iter != m_RobotMap.end(); ++Iter)
	{
		if (Iter->second->IsUse())
			continue;
		return Iter->second;
	}
	return null;
}


Robot::Robot()
{
	m_pRole = null;
	m_IsUse = false;
	m_UpdateTimeLog = 0;

	m_LockFishID = 0;
}
Robot::~Robot()
{

}
void Robot::OnInit(CRoleEx* pRole)
{
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	m_pRole = pRole;
	m_IsUse = false;
}
void Robot::OnUpdate(DWORD dwTimer)
{
	//�����˽��и��µ�ʱ��
	if (!m_pRole || !m_IsUse)
		return;
	if (m_pRole->GetRoleInfo().dwGlobeNum <= 100000)
	{
		//���С�� 10W 
		m_pRole->ChangeRoleGlobe(5000000, true);
	}
	OnUpdateRoleChangeLauncher();
	OnUpdateRoleLauncherFace();
	OnUpdateRoleOpenFire();
	OnUpdateRoleUseSkill();
}
void Robot::OnUpdateRoleChangeLauncher()//������ڿ���
{
	//����л��� 
	//����CD 1����  ÿ500S  �п��� �л�����
	if (m_ChangeLauncherTimeLog != 0 && timeGetTime() - m_ChangeLauncherTimeLog <= 60000)
		return;
	// 1/20 �����л���
	if (RandUInt() % 100 < 5)
	{
		m_ChangeLauncherTimeLog = timeGetTime();
		CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
		if (!pUser)
			return;
		//�л���
		NetCmdChangeLauncherType change;
		SetMsgInfo(change, GetMsgType(Main_Game, 17), sizeof(change));
		change.Seat = pUser->GetSeatID();
		change.LauncherType = (BYTE)(RandUInt() % 5);
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &change);
	}
}
void Robot::OnUpdateRoleOpenFire()//��ҿ��ڿ���
{
	if (m_UpdateTimeLog != 0 && timeGetTime() - m_UpdateTimeLog < 300)//200ms ����һ��
		return;
	m_UpdateTimeLog = timeGetTime();
	if (m_LockFishID == 0)
		return;
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
	if (!pUser)
		return;
	if (!pUser->IsFullEnergy())//�����ӵ�
	{
		if (pUser->CheckFire(0))
		{
			//1.�����ӵ�
			NetCmdBullet msg;
			SetMsgInfo(msg, GetMsgType(Main_Game, 11), sizeof(NetCmdBullet));
			msg.BulletID = 0;
			msg.Angle = m_Angle % 65535;
			if (pUser->IsCanLock())
				msg.LockFishID = 0;
			else
				msg.LockFishID = 0;
			msg.LauncherType = pUser->GetLauncherType();
			msg.Energy = (uint)pUser->GetEnergy();
			msg.ReboundCount = 0; pUser->GetRoleExInfo()->GetRoleVip().GetLauncherReBoundNum();
			if (msg.LauncherType == 4)
				msg.ReboundCount = 0;
			//���������õ�GameServerȥ
			g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msg);
		}
		else
		{
			//��Ҳ��� �����˲����� �ȴ�������ҽ��� �˳�
			return;
		}
	}
	else//�������
	{
		NetCmdSkillLaser msgSkill;
		SetMsgInfo(msgSkill, GetMsgType(Main_Game, 33), sizeof(msgSkill));
		msgSkill.Degree = m_Angle % 65535;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msgSkill);
	}
}
void Robot::OnUpdateRoleLauncherFace()//���������̨����
{
	if (m_ChangeAngleTimeLog != 0 && timeGetTime() - m_ChangeAngleTimeLog <= 200)
		return;
	m_ChangeAngleTimeLog = timeGetTime();

	//ʹ����������
	//3.������ڵķ���ֵ
	CRole* pUser = g_FishServer.GetTableManager()->SearchUser(m_pRole->GetUserID());//��ҽ������� �첽��
	if (!pUser)
		return;

	GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pUser->GetTableID());
	if (!pTable)
		return;

	m_Angle = pTable->GetFishDesk()->GetAngleByFish(m_LockFishID, pUser->GetSeatID());

	/*if (pUser->IsCanLock())
	{
		NetCmdUseSkill msgSkill;
		SetMsgInfo(msgSkill, GetMsgType(Main_Game, 26), sizeof(msgSkill));
		msgSkill.SkillID = SKILL_LOCK;
		g_FishServer.GetTableManager()->OnHandleTableMsg(m_pRole->GetUserID(), &msgSkill);
	}*/
	
}
void Robot::OnUpdateRoleUseSkill()//�������ʹ�ü���
{
	return;
}
void Robot::OnUseRobot(WORD TableID, BYTE MonthID)
{
	if (!m_pRole)
	{
		ASSERT(false);
		return;
	}
	if (MonthID == 0)
	{
		g_FishServer.GetTableManager()->OnPlayerJoinTable((BYTE)TableID, m_pRole, 0, false);
		m_IsUse = true;
	}
	else
	{
		GC_Cmd_JoinMonth  msg;
		SetMsgInfo(msg, GetMsgType(Main_Month, GC_JoinMonth), sizeof(GC_Cmd_JoinMonth));
		msg.dwUserID = m_pRole->GetUserID();
		msg.MonthID = MonthID;
		msg.TableID = TableID;
		m_pRole->SendDataToCenter(&msg);//���������������� �û����� ֱ�Ӽ��뵽����ȥ
		m_IsUse = true;
	}
}
void Robot::OnDelRobot()
{
	if (!m_IsUse)
		return;
	g_FishServer.GetTableManager()->OnPlayerLeaveTable(m_pRole->GetUserID());
	m_IsUse = false;
}