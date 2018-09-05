#include "stdafx.h"
#include "AlipayManager.h"
#include "FishServer.h"

#include "..\CommonFile\FishServerConfig.h"

const static char AlipaySMSInfo[] = "您本次操作的验证码为:%u【欢快游戏】";

void AlipayManager::AddRoleInfo(DWORD dwUserID, UINT64 u64Phone, DWORD dwGameConfigID)
{
	HashMap<DWORD, tagRoleAlipay>::iterator Iter = m_InfoMap.find(dwUserID);
	if (Iter != m_InfoMap.end())
	{
		m_InfoMap.erase(Iter);
	}
	tagRoleAlipay info;
	info.dwUserID = dwUserID;
	info.u64Phone = u64Phone;
	DWORD RankValue = 123456;//如果测试 表示 验证码为 123456
	if (!g_FishServerConfig.GetIsOperateTest())
		RankValue = RandUInt() % 899999 + 100000;//随机码
	info.dwPhoneNum = RankValue;
	info.dwGameConfigID = dwGameConfigID;
	//发送短信到手机上面去
	PhoneSMSOnceInfo pSMS;
	pSMS.OnlyID = 0;
	pSMS.PhoneNumberVec.push_back(u64Phone);
	pSMS.Type = ST_Alipay;
	sprintf_s(pSMS.SMSInfo, CountArray(pSMS.SMSInfo), AlipaySMSInfo, RankValue);
	info.HandleOnlyID = g_FishServer.GetPhoneSMSMananger().OnAddSMSEvent(pSMS);
	m_InfoMap.insert(HashMap<DWORD, tagRoleAlipay>::value_type(info.dwUserID, info));
}
void AlipayManager::OnHandleSMSEvent(PhoneSMSOnceInfo pEvent, bool Result)
{
	//接收到回调的函数 如何处理呢
	HashMap<DWORD, tagRoleAlipay>::iterator Iter = m_InfoMap.begin();
	for (; Iter != m_InfoMap.end(); ++Iter)
	{
		if (Iter->second.HandleOnlyID != pEvent.OnlyID)
			continue;
		ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.dwGameConfigID));
		if (!pClient)
		{
			m_InfoMap.erase(Iter);
			ASSERT(false);
			return;
		}
		if (Result)
		{
			//手机验证码发送成功 进行登陆
			__SendAddRoleInfoResult(pClient,Iter->second.dwUserID, true);
		}
		else
		{
			//手机验证码发送失败 无法登陆
			__SendAddRoleInfoResult(pClient,Iter->second.dwUserID, false);
			m_InfoMap.erase(Iter);
		}
		return;
	}
}
void AlipayManager::ChangeAlipayInfo(DWORD dwUserID, DWORD PhoneNum, TCHAR AlipayAccount[MAX_AlipayAccount_LENGTH + 1], TCHAR AlipayName[MAX_AlipayName_LENGTH + 1])
{
	HashMap<DWORD, tagRoleAlipay>::iterator Iter = m_InfoMap.find(dwUserID);
	if (Iter == m_InfoMap.end())
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.dwGameConfigID));
	if (!pClient)
	{
		m_InfoMap.erase(Iter);
		ASSERT(false);
		return;
	}
	if (Iter->second.dwUserID != dwUserID || Iter->second.dwPhoneNum != PhoneNum)
	{
		__SendChangeAlipayInfoResult(pClient,dwUserID, false, AlipayAccount, AlipayName);
		return;
	}
	m_InfoMap.erase(Iter);
	__SendChangeAlipayInfoResult(pClient,dwUserID, true, AlipayAccount, AlipayName);
}
void AlipayManager::__SendAddRoleInfoResult(ServerClientData* pClient,DWORD dwUserID, bool Result)
{
	if (nullptr == pClient)
	{
		ASSERT(false);
		return;
	}
	//发送命令
	OL_Cmd_GetChangeAlipayPhoneNum msg;
	SetMsgInfo(msg, GetMsgType(Main_Entity, OL_GetChangeAlipayPhoneNum), sizeof(OL_Cmd_GetChangeAlipayPhoneNum));
	msg.dwUserID = dwUserID;
	msg.Result = Result;
	g_FishServer.SendNetCmdToClient(pClient, &msg);
}
void AlipayManager::__SendChangeAlipayInfoResult(ServerClientData* pClient, DWORD dwUserID, bool Result, TCHAR AlipayAccount[MAX_AlipayAccount_LENGTH + 1], TCHAR AlipayName[MAX_AlipayName_LENGTH + 1])
{
	if (nullptr == pClient)
	{
		ASSERT(false);
		return;
	}
	//发送命令
	OL_Cmd_ChangeAlipayByPhone msg;
	SetMsgInfo(msg, GetMsgType(Main_Entity, OL_ChangeAlipayByPhone), sizeof(OL_Cmd_ChangeAlipayByPhone));
	msg.dwUserID = dwUserID;
	msg.Result = Result;
	TCHARCopy(msg.EntityAlipayAccount, CountArray(msg.EntityAlipayAccount), AlipayAccount, _tcslen(AlipayAccount));
	TCHARCopy(msg.EntityAlipayName, CountArray(msg.EntityAlipayName), AlipayName, _tcslen(AlipayName));
	g_FishServer.SendNetCmdToClient(pClient, &msg);
}