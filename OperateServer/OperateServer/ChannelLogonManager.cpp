#include "stdafx.h"
#include "FishServer.h"
#include "ChannelLogonManager.h"
#include "..\CommonFile\FishServerConfig.h"
ChannelLogonManager::ChannelLogonManager()
{

}
ChannelLogonManager::~ChannelLogonManager()
{
	OnDestroy();
}
void ChannelLogonManager::OnHandleLogonInfo(ServerClientData* pClient, LO_Cmd_ChannelLogon* pMsg)
{
	if (!pMsg || !pClient)
	{
		ASSERT(false);
		return;
	}
	//����ͻ��˷������ı䳤����
	LO_Cmd_ChannelLogon* pNewChannelInfo = (LO_Cmd_ChannelLogon*)malloc(pMsg->CmdSize);
	memcpy_s(pNewChannelInfo, pMsg->CmdSize, pMsg, pMsg->CmdSize);

	std::vector<TCHAR*> pVec;
	GetStringArrayVecByData(pVec, &pMsg->channelInfo);
	if (pVec.size() != pMsg->channelInfo.HandleSum)
	{
		free(pNewChannelInfo);
		ASSERT(false);
		return;
	}
	
	TCHAR PostUrl[1024];
	swprintf_s(PostUrl, CountArray(PostUrl), TEXT("game_agent/checkLogin?userId=%s&channel=%s&token=%s&productCode=%s"), pVec[0], pVec[4], pVec[6], pVec[3]);
	UINT Count = 0;
	char* pPostUrl = WCharToChar(PostUrl, Count);
	//URL ������ �����첽 ����
	AE_CRC_PAIRS pThree;
	AECrc32(pThree, pVec[0], CountArray(pVec[0])*sizeof(TCHAR), 0, 0x73573);
	//����ת����CRC ����
	DWORD Crc1 = pThree.Crc1;
	DWORD Crc2 = pThree.Crc2;
	unsigned __int64 i64Value = Crc1;
	i64Value = (i64Value << 32);
	i64Value += Crc2;//Ψһ�Ķ�����
	
	WORD RequestID = g_FishServer.GetChannelLogonID();
	UINT64* pID = new UINT64(i64Value);
	
	HashMap < unsigned __int64, LogonTempInfo>::iterator Iter = m_RoleLogonMap.find(i64Value);
	if (Iter != m_RoleLogonMap.end())
	{
		free(Iter->second.pMsg);
		m_RoleLogonMap.erase(Iter);
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ ������ǰ����������"));
	}
	LogonTempInfo pInfo;
	pInfo.pMsg = pNewChannelInfo;
	pInfo.ClientID = static_cast<BYTE>(pClient->OutsideExtraData);
	pInfo.ChannelID = GetCrc32(pVec[0]);
	TCHARCopy(pInfo.MacAddress, CountArray(pInfo.MacAddress), pMsg->MacAddress, _tcslen(pMsg->MacAddress));
	m_RoleLogonMap.insert(HashMap<unsigned __int64, LogonTempInfo>::value_type(i64Value, pInfo));
	vector<TCHAR*>::iterator IterVec = pVec.begin();
	for (; IterVec != pVec.end(); ++IterVec)
	{
		free(*IterVec);
	}
	if (g_FishServerConfig.GetIsOperateTest())
	{
		OnHandleLogonResult(i64Value, "true", strlen("true"));
		delete pID;
	}
	else
	{
		if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, pPostUrl))
		{
			delete(pID);
			free(pNewChannelInfo);
			free(pPostUrl);
			return;
		}
		free(pPostUrl);
	}
}
void ChannelLogonManager::OnHandleLogonResult(unsigned __int64 OnlyID, char* pData, DWORD Length)
{
	if (!pData)
	{
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ ��������Ϊ��"));
		ASSERT(false);
		return;
	}
	HashMap < unsigned __int64, LogonTempInfo>::iterator Iter = m_RoleLogonMap.find(OnlyID);
	if (Iter == m_RoleLogonMap.end())
	{
		ASSERT(false);
		SYSTEMTIME pTime;
		GetLocalTime(&pTime);
		g_FishServer.ShowInfoToWin("���������½���ݲ�����  Time: %02d:%02d:%02d", pTime.wHour, pTime.wMinute, pTime.wSecond);
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ �������ݲ�����"));
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.ClientID));
	if (!pClient)
	{
		free(Iter->second.pMsg);
		m_RoleLogonMap.erase(Iter);
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ �ͻ��˲�����"));
		//ASSERT(false);
		return;
	}
	DWORD PageSize = sizeof(OL_Cmd_ChannelLogon)+(Iter->second.pMsg->channelInfo.Sum - 1)*sizeof(BYTE);
	OL_Cmd_ChannelLogon* msg = (OL_Cmd_ChannelLogon*)malloc(PageSize);
	msg->SetCmdType(GetMsgType(Main_Logon, OL_ChannelLogon));
	msg->SetCmdSize(ConvertDWORDToWORD(PageSize));
	DWORD DataLength = (Iter->second.pMsg->channelInfo.Sum - 1)*sizeof(BYTE)+sizeof(ChannelUserInfo);
	memcpy_s(&msg->channelInfo, DataLength, &Iter->second.pMsg->channelInfo, DataLength);
	msg->ClientID = Iter->second.pMsg->ClientID;
	msg->PlateFormID = Iter->second.pMsg->PlateFormID;
	msg->ChannelID = Iter->second.ChannelID;//��ҵ�ƽ̨ID
	TCHARCopy(msg->MacAddress, CountArray(msg->MacAddress), Iter->second.pMsg->MacAddress, _tcslen(Iter->second.pMsg->MacAddress));

	free(Iter->second.pMsg);
	m_RoleLogonMap.erase(Iter);

	
	if (Length == 0)
	{
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ ���س���Ϊ0"));
		msg->Result = false;
		g_FishServer.SendNetCmdToClient(pClient, msg);
		free(msg);
		return;
	}
	else if (strcmp(pData, "true") == 0)
	{
		msg->Result = true;
		g_FishServer.SendNetCmdToClient(pClient, msg);
		free(msg);
		return;
	}
	else if (strcmp(pData, "false") == 0)
	{
		msg->Result = false;
		g_FishServer.SendNetCmdToClient(pClient, msg);
		free(msg);
		return;
	}
	else
	{
		LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ �������������쳣"));
		ASSERT(false);
		msg->Result = false;
		g_FishServer.SendNetCmdToClient(pClient, msg);
		free(msg);
		return;
	}
}
void ChannelLogonManager::OnDestroy()
{
	if (m_RoleLogonMap.empty())
		return;
	LogInfoToFile("LogonError.txt", TEXT("�ͻ���������½ �����������ر� ����ȫ����½����"));
	HashMap < unsigned __int64, LogonTempInfo>::iterator Iter = m_RoleLogonMap.begin();
	for (; Iter != m_RoleLogonMap.end(); ++Iter)
	{
		free(Iter->second.pMsg);
	}
	m_RoleLogonMap.clear();
}