#include "stdafx.h"
#include "BindPhoneManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"

const static char PhoneSMSInfo[] = "�����β�������֤��Ϊ:%u��������Ϸ��";
BindPhoneManager::BindPhoneManager()
{
	m_BindPhoneMap.clear();
}
BindPhoneManager::~BindPhoneManager()
{
	m_BindPhoneMap.clear();
}
bool BindPhoneManager::CheckPhoneNumber(unsigned __int64  PhoneNumber)
{
	//����ֻ������Ƿ���ȷ
	return true;
}
bool BindPhoneManager::OnGetPhoneVerificationNumber(ServerClientData* pClient, GO_Cmd_GetPhoneVerificationNum* pMsg)
{
	if (!pClient || !pMsg)
	{
		ASSERT(false);
		return false;
	}
	if (!CheckPhoneNumber(pMsg->PhoneNumber))
	{
		OG_Cmd_BindPhone msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BindPhone), sizeof(OG_Cmd_BindPhone));
		msg.dwUserID = pMsg->dwUserID;
		msg.Result = false;
		msg.ErrorID = ROE_PhoneVerificationNum_PhoneError;
		msg.PhoneNumber = pMsg->PhoneNumber;
		g_FishServer.SendNetCmdToClient(pClient, &msg);
		ASSERT(false);
		return false;
	}
	//����ֻ�����
	DBR_Cmd_CheckPhone msgDB;
	SetMsgInfo(msgDB, DBR_CheckPhone, sizeof(DBR_Cmd_CheckPhone));
	msgDB.dwUserID = pMsg->dwUserID;
	msgDB.Phone = pMsg->PhoneNumber;
	msgDB.ClientID = pClient->OutsideExtraData;
	msgDB.IsBindOrLogon = true;
	g_FishServer.SendNetCmdToDB(&msgDB);
	return true;
}
void BindPhoneManager::OnCheckPhoneResult(DBO_Cmd_CheckPhone* pMsg)
{
	//����ֻ��Ƿ����ʹ��
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(ConvertDWORDToBYTE(pMsg->ClientID));
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	if (pMsg->Result)
	{
		//�����ҵĶ���
		HashMap<DWORD, BindPhoneOnce>::iterator Iter = m_BindPhoneMap.find(pMsg->dwUserID);
		if (Iter != m_BindPhoneMap.end())
		{
			//�����Ѿ������� ���ǽ��д��� 
			//1.������� ���Ѿ����͵� ���� ���ڵȴ����ŵ� ����  ʱ�����3���ӵ� ���ڶ��� ֱ��ɾ����
			if (Iter->second.SendPhoneNumberOnlyID != 0 /*&& timeGetTime() - Iter->second.beginTime >= 120000*/)
			{
				m_BindPhoneMap.erase(Iter);
			}
			else
			{
				//�Ѿ����ڵĶ��� �޷������ύ
				OG_Cmd_BindPhone msg;
				SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BindPhone), sizeof(OG_Cmd_BindPhone));
				msg.dwUserID = pMsg->dwUserID;
				msg.Result = false;
				msg.ErrorID = ROE_PhoneVerificationNum_IsExists;
				msg.PhoneNumber = pMsg->Phone;
				g_FishServer.SendNetCmdToClient(pClient, &msg);
				return;
			}
		}
		//���ⲿ�������ύ����
		BindPhoneOnce pOnce;
		pOnce.UserID = pMsg->dwUserID;
		pOnce.GameServerClientID = pMsg->ClientID;
		pOnce.VerificationNumber = 0;
		pOnce.PhoneNumber = pMsg->Phone;
		m_BindPhoneMap.insert(HashMap<DWORD, BindPhoneOnce>::value_type(pMsg->dwUserID, pOnce));

		//�뿪��������ͻ���ȥ
		OG_Cmd_GetPhoneVerificationNum msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OG_GetPhoneVerificationNum), sizeof(OG_Cmd_GetPhoneVerificationNum));
		msg.dwUserID = pMsg->dwUserID;
		msg.ErrorID = ROE_PhoneVerificationNum_WriteSend;
		msg.PhoneNumber = pMsg->Phone;
		g_FishServer.SendNetCmdToClient(pClient, &msg);
		//�ֻ�������֤����� ���Ƿ��Ͷ��Ÿ���� �����Ǽ��Ϸ��͵�
		m_SendPhoneList.UserVec.push_back(pMsg->dwUserID);
	}
	else
	{
		//�ֻ��Ѿ���ʹ����
		OG_Cmd_GetPhoneVerificationNum msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OG_GetPhoneVerificationNum), sizeof(OG_Cmd_GetPhoneVerificationNum));
		msg.dwUserID = pMsg->dwUserID;
		msg.ErrorID = ROE_PhoneVerificationNum_PhoneBeUse;
		msg.PhoneNumber = pMsg->Phone;
		g_FishServer.SendNetCmdToClient(pClient, &msg);
	}
}
void BindPhoneManager::OnGetPhoneVerificationNumberResult(bool Result,BYTE ErrorID, DWORD dwUserID)
{
	//�ⲿ�������������� ��ȡ�ֻ���֤��Ľ��
	HashMap<DWORD, BindPhoneOnce>::iterator Iter = m_BindPhoneMap.find(dwUserID);
	if (Iter == m_BindPhoneMap.end())
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(ConvertDWORDToBYTE(Iter->second.GameServerClientID));
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�������GameServerȥ
	OG_Cmd_GetPhoneVerificationNum msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OG_GetPhoneVerificationNum), sizeof(OG_Cmd_GetPhoneVerificationNum));
	msg.dwUserID = Iter->second.UserID;
	msg.ErrorID = ErrorID;
	msg.PhoneNumber = Iter->second.PhoneNumber;
	g_FishServer.SendNetCmdToClient(pClient, &msg);
	//if (!Result)
	//{
	//	//��ʾ��֤ʧ���������Ƴ�������
	//	m_BindPhoneMap.erase(Iter);
	//}
}
bool BindPhoneManager::OnBindPhontByVerificationNumber(ServerClientData* pClient, GO_Cmd_BindPhone* pMsg)
{
	if (!pClient || !pMsg)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, BindPhoneOnce>::iterator Iter = m_BindPhoneMap.find(pMsg->dwUserID);
	if (Iter == m_BindPhoneMap.end())
	{
		//ASSERT(false);
		OG_Cmd_BindPhone msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BindPhone), sizeof(OG_Cmd_BindPhone));
		msg.dwUserID = pMsg->dwUserID;
		msg.ErrorID = ROE_BindPhone_BindTimeOut;
		msg.Result = false;
		msg.PhoneNumber = 0;// pMsg->PhoneNumber;
		msg.SecPasswordCrc1 = pMsg->SecPasswordCrc1;
		msg.SecPasswordCrc2 = pMsg->SecPasswordCrc2;
		msg.SecPasswordCrc3 = pMsg->SecPasswordCrc3;
		g_FishServer.SendNetCmdToClient(pClient, &msg);
		return false;
	}
	if (Iter->second.SendPhoneNumberOnlyID == 0 || Iter->second.VerificationNumber != pMsg->BindNumber)
	{
		//ASSERT(false);
		OG_Cmd_BindPhone msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BindPhone), sizeof(OG_Cmd_BindPhone));
		msg.dwUserID = pMsg->dwUserID;
		msg.ErrorID = ROE_BindPhone_BindNumError;
		msg.Result = false;
		msg.PhoneNumber = 0;// pMsg->PhoneNumber;
		msg.SecPasswordCrc1 = pMsg->SecPasswordCrc1;
		msg.SecPasswordCrc2 = pMsg->SecPasswordCrc2;
		msg.SecPasswordCrc3 = pMsg->SecPasswordCrc3;
		g_FishServer.SendNetCmdToClient(pClient, &msg);
		return false;
	}
	//��֤��Ϻ� ���Ǽ�������
	OG_Cmd_BindPhone msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BindPhone), sizeof(OG_Cmd_BindPhone));
	msg.dwUserID = pMsg->dwUserID;
	msg.ErrorID = ROE_BindPhone_Sucess;
	msg.Result = true;
	msg.PhoneNumber = Iter->second.PhoneNumber;//ʹ�ö����ϼ�¼���ֻ�����
	msg.SecPasswordCrc1 = pMsg->SecPasswordCrc1;
	msg.SecPasswordCrc2 = pMsg->SecPasswordCrc2;
	msg.SecPasswordCrc3 = pMsg->SecPasswordCrc3;
	g_FishServer.SendNetCmdToClient(pClient, &msg);
	m_BindPhoneMap.erase(pMsg->dwUserID);
	return true;
}
void BindPhoneManager::OnUpdateSendPhoneSMS(DWORD dwTimer)
{
	//5����һ��
	static DWORD SendPhoneSMSLogTime = 0;
	if (SendPhoneSMSLogTime == 0 || dwTimer - SendPhoneSMSLogTime >= 5000)
	{
		SendPhoneSMSLogTime = dwTimer;
		if (m_SendPhoneList.UserVec.empty())
			return;
		DWORD RankValue = 123456;//������� ��ʾ ��֤��Ϊ 123456
		if (!g_FishServerConfig.GetIsOperateTest())
			RankValue = RandUInt() % 899999 + 100000;//�����
		PhoneSMSOnceInfo pEvent;
		sprintf_s(pEvent.SMSInfo, CountArray(pEvent.SMSInfo), PhoneSMSInfo, RankValue);
		pEvent.Type = ST_PhoneBind;
		vector<DWORD>::iterator Iter = m_SendPhoneList.UserVec.begin();
		for (; Iter != m_SendPhoneList.UserVec.end(); ++Iter)
		{
			DWORD dwUserID = *Iter;
			HashMap<DWORD, BindPhoneOnce>::iterator IterFind = m_BindPhoneMap.find(dwUserID);
			if (IterFind == m_BindPhoneMap.end())
				continue;
			pEvent.PhoneNumberVec.push_back(IterFind->second.PhoneNumber);
		}
		DWORD OnlyID = g_FishServer.GetPhoneSMSMananger().OnAddSMSEvent(pEvent);
		Iter = m_SendPhoneList.UserVec.begin();
		for (; Iter != m_SendPhoneList.UserVec.end(); ++Iter)
		{
			DWORD dwUserID = *Iter;
			HashMap<DWORD, BindPhoneOnce>::iterator IterFind = m_BindPhoneMap.find(dwUserID);
			if (IterFind == m_BindPhoneMap.end())
				continue;
			IterFind->second.SendPhoneNumberOnlyID = OnlyID;
			IterFind->second.VerificationNumber = RankValue;//��һ������ɹ�
		}
		m_SendPhoneList.UserVec.clear();
		return;
	}
}
void BindPhoneManager::OnHandleSMSEvent(PhoneSMSOnceInfo pEvent, bool Result)
{
	DWORD OnlyID = pEvent.OnlyID;
	HashMap<DWORD, BindPhoneOnce>::iterator Iter = m_BindPhoneMap.begin();
	for (; Iter != m_BindPhoneMap.end();)
	{
		if (Iter->second.SendPhoneNumberOnlyID == OnlyID)
		{
			if (Result)
			{
				OnGetPhoneVerificationNumberResult(true, ROE_PhoneVerificationNum_Sucess, Iter->first);
				++Iter;
			}
			else
			{
				OnGetPhoneVerificationNumberResult(false, ROE_BindPhone_SendSmsInfoError, Iter->first);
				Iter = m_BindPhoneMap.erase(Iter);
			}
		}
		else
			++Iter;
	}
}