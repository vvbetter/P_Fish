#include "Stdafx.h"
#include "LogonManager.h"
#include "FishServer.h"
LogonManager::LogonManager()
{
	m_GameIndex =1;
	m_GameServerMap.clear();
}
LogonManager::~LogonManager()
{
	
}
void LogonManager::OnLogonRsgToCenter()
{
	//Logon ���ӵ�Center
	/*LC_Cmd_RsgLogon msg;
	SetMsgInfo(msg, GetMsgType(Main_Server, LC_RsgLogon), sizeof(LC_Cmd_RsgLogon));
	msg.LogonConfigID = g_FishServer.GetLogonConfigID();
	g_FishServer.SendNetCmdToCenter(&msg);*/
}
void LogonManager::OnLogonRsgResult(CL_Cmd_RsgLogon* pMsg)
{
	if (!pMsg)
		return;
	if ((pMsg->States & MsgBegin) != 0)
		m_UserMap.clear();
	for (size_t i = 0; i < pMsg->Sum; ++i)//������ҵ��б�
	{
		m_UserMap.insert(HashMap<DWORD, BYTE>::value_type(pMsg->Array[i].dwUserID, pMsg->Array[i].GameConfigID));

		HashMap<BYTE, WORD>::iterator Iter = m_GamePlayerSum.find(pMsg->Array[i].GameConfigID);
		if (Iter == m_GamePlayerSum.end())
		{
			m_GamePlayerSum.insert(HashMap<BYTE, WORD>::value_type(pMsg->Array[i].GameConfigID, 1));
		}
		else
		{
			Iter->second += 1;
		}
	}
}
void LogonManager::OnLogonLeaveCenter()
{
	//��Logon�뿪�����������ʱ��
	//���Լ���Logon�Ͽ����ӵ�ʱ�� ��ʾ��ǰLogon��ͣ���� ���ڽ��ս���

	return;
}
void LogonManager::OnUserJoinCenter(DWORD dwUserID, BYTE GameConfigID)
{
	//��SocketID ת��Ϊ ConfigureID
	HashMap<DWORD, BYTE>::iterator Iter = m_UserMap.find(dwUserID);
	if (Iter != m_UserMap.end())
	{
		ASSERT(false);
		return;
	}
	m_UserMap.insert(HashMap<DWORD, BYTE>::value_type(dwUserID, GameConfigID));

	HashMap<BYTE, WORD>::iterator IterSum = m_GamePlayerSum.find(GameConfigID);
	if (IterSum == m_GamePlayerSum.end())
	{
		m_GamePlayerSum.insert(HashMap<BYTE, WORD>::value_type(GameConfigID, 1));
	}
	else
	{
		IterSum->second += 1;
	}

	m_TempLogonInfo.OnUserLogon(dwUserID);//�û���ʽ��½ ɾ����ʱ��¼������
}
void LogonManager::OnUserLeaveCenter(DWORD dwUserID)
{
	HashMap<DWORD, BYTE>::iterator Iter = m_UserMap.find(dwUserID);
	if (Iter == m_UserMap.end())
	{
		//ASSERT(false);
		return;
	}
	BYTE GameConfigID = Iter->second;
	m_UserMap.erase(Iter);

	HashMap<BYTE, WORD>::iterator IterSum = m_GamePlayerSum.find(GameConfigID);
	if (IterSum == m_GamePlayerSum.end())
	{
		ASSERT(false);
		return;
	}
	else
	{
		IterSum->second -= 1;
		if (IterSum->second == 0)
		{
			m_GamePlayerSum.erase(IterSum);
		}
	}
}
void LogonManager::OnGameRsgLogon(BYTE GameConfigID)
{
	//һ��GameServer���ӵ�Logon��������
	HashMap<BYTE,DWORD>::iterator Iter = m_GameServerMap.find(GameConfigID);
	if (Iter != m_GameServerMap.end())
	{
		ASSERT(false);
		return;
	}
	m_GameServerMap.insert(HashMap<BYTE, DWORD>::value_type(GameConfigID,0));

	std::cout << "GameServer ���� Logon\n";
}
void LogonManager::OnGameLeaveCenter(BYTE GameConfigID)
{
	HashMap<BYTE, DWORD>::iterator Iter = m_GameServerMap.find(GameConfigID);
	if (Iter == m_GameServerMap.end())
	{
		//ASSERT(false);
		return;
	}
	//��Ϊһ��GameServer���뿪 �������������GameServer�����ȫ���뿪��Ҽ���
	m_GameServerMap.erase(Iter);

	HashMap<DWORD, BYTE>::iterator IterRole = m_UserMap.begin();
	for (; IterRole != m_UserMap.end();)
	{
		if (IterRole->second == GameConfigID)
		{
			IterRole = m_UserMap.erase(IterRole);
		}
		else
			++IterRole;
	}
	m_GamePlayerSum.erase(GameConfigID);

	g_FishServer.ShowInfoToWin("GameServer �뿪Center ��Logon�Ͻ���ɾ��");
}
void LogonManager::OnGameLeaveLogon(BYTE GameConfigID)
{
	HashMap<BYTE, DWORD>::iterator Iter = m_GameServerMap.find(GameConfigID);
	if (Iter == m_GameServerMap.end())
	{
		//ASSERT(false);
		return;
	}
	m_GameServerMap.erase(Iter);

	g_FishServer.ShowInfoToWin("GameServer �뿪Logon");
}
void LogonManager::OnGameJoinLogon(BYTE GameConfigID)
{
}
BYTE LogonManager::GetGameServerConfigIDByUserID(DWORD dwUserID)//�ҵ��������ҵ�½��GameServer
{
	if (m_GameServerMap.empty())
	{
		ASSERT(false);
		return 0;
	}
	//�������ID ��ȡ���������GameServer��ID
	HashMap<DWORD, BYTE>::iterator Iter = m_UserMap.find(dwUserID);
	if (Iter != m_UserMap.end())
		return Iter->second;
	else
	{
		if (m_GamePlayerSum.empty())
		{
			BYTE GameConfigID = m_GameServerMap.begin()->first;
			m_TempLogonInfo.OnUserTempLogon(dwUserID, GameConfigID);//���ڽ�����ʱ�ļ�¼��
			return GameConfigID;
		}
		else
		{
			//�������ж�����Ƿ����ϴ���ʱ��½������
			BYTE GameConfigID = m_TempLogonInfo.GetUserTempGameConfigID(dwUserID);
			if (GameConfigID != 0)
				return GameConfigID;
			//������Ҫ��¼�� �������ٵ� 
			//���� 
			//���GameServer ���ǴӶ��GameServer��ѡ��һ�����е�½ 
			DWORD Index = m_GameIndex % m_GameServerMap.size();
			++m_GameIndex;

			HashMap<BYTE, DWORD>::iterator IterSum = m_GameServerMap.begin();
			for (DWORD i = 0; i < Index; ++i)
				++IterSum;
			return IterSum->first;
		}
	}
	return 0;
}