#include "stdafx.h"
#include "PhonePayManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include "..\CommonFile\DBLogManager.h"
#include "..\CommonFile\DBLogManager.h"
extern void SendLogDB(NetCmd* pCmd);

PhonePayManager::PhonePayManager()
{
	m_PhonePayMap.clear();
}
PhonePayManager::~PhonePayManager()
{

}
void PhonePayManager::OnInit()
{
	//��������ǰ�����ݿ����ȫ�����ֻ�������
	DBR_Cmd_LoadPhonePayOrderID msg;
	SetMsgInfo(msg, DBR_LoadPhonePayOrderID, sizeof(DBR_Cmd_LoadPhonePayOrderID));
	msg.DiffDay = 2;
	g_FishServer.SendNetCmdToDB(&msg);
}
void PhonePayManager::OnLoadAllPhonePayResult(DBO_Cmd_LoadPhonePayOrderID* pMsg)
{
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	if (pMsg->States & MsgBegin)
	{
		m_PhoneOrderIDSet.clear();
	}
	for (WORD i = 0; i < pMsg->Sum; ++i)
	{
		m_PhoneOrderIDSet.insert(std::set<unsigned __int64>::value_type(pMsg->Array[i]));
	}
}
void PhonePayManager::OnHandleRolePhonePay(ServerClientData* pClient, GO_Cmd_PhonePay* pMsg)
{
	if (!pClient || !pMsg)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, tagShopConfig>::iterator IterShop = g_FishServer.GetFishConfig().GetShopConfig().ShopMap.find(pMsg->ShopID);
	if (IterShop == g_FishServer.GetFishConfig().GetShopConfig().ShopMap.end())
	{
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID = pMsg->dwUserID;
		msg.Phone = pMsg->Phone;
		msg.Result = false;
		msg.ShopID = pMsg->ShopID;
		msg.ShopOnlyID = pMsg->ShopOnlyID;
		msg.ShopSum = pMsg->ShopSum;
		msg.ErrorID = ROE_PhonePay_System;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		ASSERT(false);
		return;
	}
	HashMap<BYTE, tagShopItemConfig>::iterator IterShopItem= IterShop->second.ShopItemMap.find(pMsg->ShopOnlyID);
	if (IterShopItem == IterShop->second.ShopItemMap.end())
	{
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID = pMsg->dwUserID;
		msg.Phone = pMsg->Phone;
		msg.Result = false;
		msg.ShopID = pMsg->ShopID;
		msg.ShopOnlyID = pMsg->ShopOnlyID;
		msg.ShopSum = pMsg->ShopSum;
		msg.ErrorID = ROE_PhonePay_System;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		ASSERT(false);
		return;
	}
	//�ж���Ʒ�Ƿ�Ϊ������Ʒ
	if (IterShopItem->second.ShopType != SIT_PhonePay)
	{
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID = pMsg->dwUserID;
		msg.Phone = pMsg->Phone;
		msg.Result = false;
		msg.ShopID = pMsg->ShopID;
		msg.ShopOnlyID = pMsg->ShopOnlyID;
		msg.ShopSum = pMsg->ShopSum;
		msg.ErrorID = ROE_PhonePay_System;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		ASSERT(false);
		return;
	}
	DWORD ItemID = IterShopItem->second.ItemInfo.ItemID;
	DWORD ItemParam = g_FishServer.GetFishConfig().GetItemParam(ItemID);
	if (ItemParam == 0)
	{
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID = pMsg->dwUserID;
		msg.Phone = pMsg->Phone;
		msg.Result = false;
		msg.ShopID = pMsg->ShopID;
		msg.ShopOnlyID = pMsg->ShopOnlyID;
		msg.ShopSum = pMsg->ShopSum;
		msg.ErrorID = ROE_PhonePay_System;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		ASSERT(false);
		return;
	}
	PhonePayOnce pOrder;
	pOrder.ClientID = pClient->OutsideExtraData;
	pOrder.UserID = pMsg->dwUserID;
	pOrder.PhoneNumber = pMsg->Phone;
	pOrder.ShopID = pMsg->ShopID;
	pOrder.ShopOnlyID = pMsg->ShopOnlyID;
	pOrder.ShopSum = pMsg->ShopSum;
	pOrder.PayRMBSum = ItemParam * pMsg->ShopSum * IterShopItem->second.ItemInfo.ItemSum;//��ֵ�Ļ���
	//����� ��Ӷ�����
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	char OrderStr[256];
	sprintf_s(OrderStr, CountArray(OrderStr), "%d%04d%02d%02d%02d%02d%02d%llu%u%u", pMsg->dwUserID, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, pMsg->Phone, pOrder.PayRMBSum, RandUInt());
	AE_CRC_PAIRS pThree;
	AECrc32(pThree, OrderStr, strlen(OrderStr), 0, 0x73573);
	//����ת����CRC ����
	DWORD Crc1 = pThree.Crc1;
	DWORD Crc2 = pThree.Crc2;
	unsigned __int64 i64Value = Crc1;
	i64Value = (i64Value << 32);
	i64Value += Crc2;//Ψһ�Ķ�����
	pOrder.OrderOnlyID = i64Value;
	if (g_FishServerConfig.GetIsOperateTest())
	{
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.OrderID = i64Value;
		msg.dwUserID = pOrder.UserID;
		msg.Phone = pMsg->Phone;
		msg.Result = true;
		msg.ShopID = pOrder.ShopID;
		msg.ShopOnlyID = pOrder.ShopOnlyID;
		msg.ShopSum = pOrder.ShopSum;
		msg.ErrorID = ROE_PhonePay_Sucess;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		//���Ѿ�����Ķ����洢�����ݿ�ȥ
		DBR_Cmd_AddPhonePayOrderID msgDB;
		SetMsgInfo(msgDB, DBR_AddPhonePayOrderID, sizeof(DBR_Cmd_AddPhonePayOrderID));
		msgDB.dwUserID = pOrder.UserID;
		msgDB.OrderOnlyID = pOrder.OrderOnlyID;
		g_FishServer.SendNetCmdToDB(&msgDB);
		//��ŵ���������ȥ
		m_PhoneOrderIDSet.insert(set<unsigned __int64>::value_type(pOrder.OrderOnlyID));
	}
	else
	{
		m_PhonePayMap.insert(HashMap<unsigned __int64, PhonePayOnce>::value_type(pOrder.OrderOnlyID, pOrder));
		//��������ǰ����ֵ 
		//��֯URL ���� ��ֵ  ���ҵȴ���ֵ�Ĳ���
		OnSendPhoePayInfoToHttp(pOrder.OrderOnlyID);//����ΨһID ���ж�������
	}
}
void PhonePayManager::OnSendPhoePayInfoToHttp(unsigned __int64 OrderID)
{
	HashMap<unsigned __int64, PhonePayOnce>::iterator Iter = m_PhonePayMap.find(OrderID);
	if (Iter == m_PhonePayMap.end())
	{
		ASSERT(false);
		return;
	}
	OperateConfig* pOperateConfig = g_FishServerConfig.GetOperateConfig();
	if (!pOperateConfig)
	{
		ASSERT(false);
		return;
	}
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	//���ɶ����� 
	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "account_no=%llu&agent_id=%s&face_price=%u&order_no=%llu&orgcode=%s&partner_id=%s&password=%s&pay_password=%s&sign_type=MD5&timestamp=%04d%02d%02d%02d%02d%02d%s",
		Iter->second.PhoneNumber, pOperateConfig->PhonePayAgentId, Iter->second.PayRMBSum, OrderID, pOperateConfig->PhonePayOrgcode, pOperateConfig->PhonePayPartNerID, pOperateConfig->PhonePayPassword, pOperateConfig->PhonePayPayPassword, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, pOperateConfig->PhonePayKey);
	string Md5Str = md5(SignStr);
	char strURL[2048];
	//mobile/charge.json?
	sprintf_s(strURL, CountArray(strURL),
		"{\"header\":{\"partner_id\":\"%s\",\"orgcode\":\"%s\",\"agent_id\":\"%s\",\"password\":\"%s\",\"timestamp\":\"%04d%02d%02d%02d%02d%02d\",\"sign_type\":\"MD5\",\"sign\":\"%s\"},\"body\":{\"pay_password\":\"%s\",\"order_no\":\"%llu\",\"account_no\":\"%llu\",\"face_price\":\"%u\"}}", 
		pOperateConfig->PhonePayPartNerID, pOperateConfig->PhonePayOrgcode, pOperateConfig->PhonePayAgentId, pOperateConfig->PhonePayPassword, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, Md5Str.c_str(), pOperateConfig->PhonePayPayPassword, OrderID, Iter->second.PhoneNumber, Iter->second.PayRMBSum
		);
	//UTF-8 
	string UTF8URL = GBKToUTF8(strURL);//��תGBK Ȼ���� URL ����
	WORD RequestID = g_FishServer.GetPhonePayID();
	UINT64* pID = new UINT64(OrderID);
	if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pID, RequestID, "mobile/charge.json", UTF8URL.c_str()))
	{
		delete(pID);
	}

	//��¼�����ݿ�
	g_DBLogManager.LogRolePhonePayLogToDB("���Ͷ�����SMS", OrderID, Iter->second.UserID, Iter->second.PhoneNumber, Iter->second.PayRMBSum, SendLogDB);
}
void PhonePayManager::OnSendPhonePayResult(unsigned __int64 OrderID, char* pData, DWORD Length)
{
	if (!pData)
	{
		ASSERT(false);
		return;
	}
	/*if (Length!=0)
		g_FishServer.ShowInfoToWin("���յ����ѳ�ֵ���ַ���: %s", pData);*/
	HashMap<unsigned __int64, PhonePayOnce>::iterator Iter = m_PhonePayMap.find(OrderID);//ָ������������
	if (Iter == m_PhonePayMap.end())
	{
		ASSERT(false);
		return;
	}
	set<unsigned __int64>::iterator IterFind = m_PhoneOrderIDSet.find(OrderID);//�����Ѿ���������� �ظ���
	if (IterFind != m_PhoneOrderIDSet.end())
	{
		//�ظ��Ķ��� ���ڴ���
		m_PhonePayMap.erase(Iter);
		ASSERT(false);
		return;
	}
	//�жϷ��ص��ַ������Ƿ��к��ʵ�����  
	if (Length != 0 && strstr(pData, "\"ret_code\":\"10000000\"") != null)
	{
		//һ�м�������
		g_DBLogManager.LogRolePhonePayLogToDB("SMS���ն����ɹ� ���ڴ���", OrderID, Iter->second.UserID, Iter->second.PhoneNumber, Iter->second.PayRMBSum, SendLogDB);
		return;
	}
	else if (Length != 0 && strstr(pData, "\"ret_code\":\"10990001\"") != null )
	{
		//�����ѯ��������
		g_DBLogManager.LogRolePhonePayLogToDB("SMS���ն���ʧ��[10990001] �����ѯ", OrderID, Iter->second.UserID, Iter->second.PhoneNumber, Iter->second.PayRMBSum, SendLogDB);
		OnQueryPhonePayOrder(Iter->second.OrderOnlyID);
	}
	else if (Length != 0 && strstr(pData, "\"ret_code\":\"11610051\"") != null)
	{
		//�����ѯ��������
		g_DBLogManager.LogRolePhonePayLogToDB("SMS���ն���ʧ��[11610051] �����ѯ", OrderID, Iter->second.UserID, Iter->second.PhoneNumber, Iter->second.PayRMBSum, SendLogDB);
		OnQueryPhonePayOrder(Iter->second.OrderOnlyID);
	}
	else
	{
		g_DBLogManager.LogRolePhonePayLogToDB("SMS���ն���ʧ��", OrderID, Iter->second.UserID, Iter->second.PhoneNumber, Iter->second.PayRMBSum, SendLogDB);
		//ASSERT(false);
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID = Iter->second.UserID;
		msg.Phone = Iter->second.PhoneNumber;
		msg.Result = false;
		msg.ShopID = Iter->second.ShopID;
		msg.ShopOnlyID = Iter->second.ShopOnlyID;
		msg.ShopSum = Iter->second.ShopSum;
		msg.ErrorID = ROE_PhonePay_HttpError;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		m_PhonePayMap.erase(Iter);
	}
}
void PhonePayManager::OnQueryPhonePayOrder(UINT64 order_no)
{
	HashMap<unsigned __int64, PhonePayOnce>::iterator Iter = m_PhonePayMap.find(order_no);
	if (Iter == m_PhonePayMap.end())
	{
		ASSERT(false);
		return;
	}
	OperateConfig* pOperateConfig = g_FishServerConfig.GetOperateConfig();
	if (!pOperateConfig)
	{
		ASSERT(false);
		return;
	}
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	//��ȡǩ��
	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "agent_id=%s&order_no=%llu&order_type=01&orgcode=%s&partner_id=%s&password=%s&sign_type=MD5&timestamp=%04d%02d%02d%02d%02d%02d%s",
		pOperateConfig->PhonePayAgentId, order_no, pOperateConfig->PhonePayOrgcode, pOperateConfig->PhonePayPartNerID, pOperateConfig->PhonePayPassword, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, pOperateConfig->PhonePayKey);
	string Md5Str = md5(SignStr);

	char strURL[2048];
	sprintf_s(strURL, CountArray(strURL),
		"{\"header\":{\"partner_id\":\"%s\",\"orgcode\":\"%s\",\"agent_id\":\"%s\",\"password\":\"%s\",\"timestamp\":\"%04d%02d%02d%02d%02d%02d\",\"sign_type\":\"MD5\",\"sign\":\"%s\"},\"body\":{\"order_no\":\"%llu\",\"order_type\":\"01\"}",
		pOperateConfig->PhonePayPartNerID, pOperateConfig->PhonePayOrgcode, pOperateConfig->PhonePayAgentId, pOperateConfig->PhonePayPassword, pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond, Md5Str.c_str(), order_no);
	//��ѯָ���Ķ���
	//ƴ���ַ��� ���Ͳ�ѯ����
	string UTF8URL = GBKToUTF8(strURL);//��תGBK Ȼ���� URL ����
	WORD RequestID = g_FishServer.GetPhonePayID();
	UINT64* pID = new UINT64(order_no);
	if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pID, RequestID, "common/query.json", UTF8URL.c_str()))
	{
		delete(pID);
	}
}
void PhonePayManager::OnHandleHttpResult(string account_no, string face_price, string order_no, string order_time, string order_type, string partner_id, string pay_money, string profit, string sign_type, string status, string stream_id, string sign)
{
	LogInfoToFile("LogPhonePay.txt", "�ֻ���ֵ�첽�ص�: account_no=%s,face_price=%s,order_no=%s,order_time=%s,order_type=%s,partner_id=%s,pay_money=%s,profit=%s,sign_type=%s,status=%s,stream_id=%s,sign=%s"
		,account_no.c_str(), face_price.c_str(), order_no.c_str(), order_time.c_str(), order_type.c_str(), partner_id.c_str(), pay_money.c_str(), profit.c_str(), sign_type.c_str(), status.c_str(), stream_id.c_str(), sign.c_str());

	unsigned __int64 orderID = _strtoui64(order_no.c_str(), null, 10);//�ַ���תUInt64
	DWORD Price = strtoul(face_price.c_str(), null, 10);//static_cast<DWORD>(atoi(face_price.c_str()));

	OperateConfig* pOperateConfig = g_FishServerConfig.GetOperateConfig();
	if (!pOperateConfig)
	{
		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:ϵͳ����", orderID, 0, 0, Price, SendLogDB);
		ASSERT(false);
		return;
	}
	//���մ�Http ������������
	//ǩ���ļ��� account_no face_price order_no order_time order_type pay_money profit status sign_type stream_id
	//string str2 = "account_no=15088290546&face_price=1.00&order_no=15541734533254851416&order_time=20151225165707&order_type=01&partner_id=10330028&pay_money=1.30&profit=0.00&sign_type=MD5&status=SUCCESS&stream_id=260457121F75BDC0E16701F76";
	char SignStr[2048]; 
	sprintf_s(SignStr, CountArray(SignStr), "account_no=%s&face_price=%s&order_no=%s&order_time=%s&order_type=%s&partner_id=%s&pay_money=%s&profit=%s&sign_type=%s&status=%s&stream_id=%s%s",
		account_no.c_str(), face_price.c_str(), order_no.c_str(), order_time.c_str(), order_type.c_str(), partner_id.c_str(), pay_money.c_str(), profit.c_str(), sign_type.c_str(), status.c_str(), stream_id.c_str(), pOperateConfig->PhonePayKey);

	//g_FishServer.ShowInfoToWin("���յ����ѳ�ֵ���첽���� : %s", SignStr);

	string Md5Str = md5(SignStr);
	if (Md5Str.compare(sign) != 0)
	{
		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:MD5��֤ʧ��", orderID, 0, 0, Price, SendLogDB);

		//g_DBLogManager.LogErrorPhonePayInfoTODB(1, static_cast<unsigned __int64>(_atoi64(order_no.c_str())),order_no,"",)

		//LogInfoToFile("LogPhonePay.txt", "�ֻ���ֵ�첽�ص� MD5У��ʧ�� order_no=%s, ChannelMd5=%s,ConfigMD5=%s", order_no.c_str(), sign.c_str(), Md5Str.c_str());
		//g_FishServer.ShowInfoToWin("���ѳ�ֵʧ�� MD5����֤���� %s : %s", Md5Str.c_str(), sign.c_str());
		//��ǰ���յ������ݿ����Ǵ����
		//ASSERT(false);
		return;
	}
	
	HashMap<unsigned __int64, PhonePayOnce>::iterator Iter = m_PhonePayMap.find(orderID);
	if (Iter == m_PhonePayMap.end())
	{
		//ASSERT(false);
		//LogInfoToFile("LogPhonePay.txt", "�ֻ���ֵ�첽�ص� �����Ų����� order_no=%s", order_no.c_str());
		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:����������", orderID, 0, 0, Price, SendLogDB);
		return;
	}
	set<unsigned __int64>::iterator IterFind= m_PhoneOrderIDSet.find(orderID);
	if (IterFind != m_PhoneOrderIDSet.end())
	{
		//�ظ��Ķ��� ���ڴ���
		m_PhonePayMap.erase(Iter);
		//ASSERT(false);
		//LogInfoToFile("LogPhonePay.txt", "�ֻ���ֵ�첽�ص� �����ظ� order_no=%s", order_no.c_str());
		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:�����ظ�", orderID,Iter->second.UserID,Iter->second.PhoneNumber, Price, SendLogDB);
		return;
	}
	
	if (Iter->second.PayRMBSum != Price)
	{
		m_PhonePayMap.erase(Iter);
		//ASSERT(false);
		//LogInfoToFile("LogPhonePay.txt", "�ֻ���ֵ�첽�ص� ��ֵ�������Ϸ���ý����� order_no=%s", order_no.c_str());

		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:�۸�ƥ��", orderID, Iter->second.UserID, Iter->second.PhoneNumber, Price, SendLogDB);
		return;
	}
	if (status.compare("SUCCESS") == 0)
	{
		//��������ɹ���
		//1.�ж϶������Ƿ��ظ��� ���ǽ��д���
		//�����ظ��Ķ��� ����ֱ������
		ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(Iter->second.ClientID);
		if (!pClient)
		{
			ASSERT(false);
			return;
		}
		OC_Cmd_PhonePay msg;
		msg.OrderID = orderID;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.dwUserID =Iter->second.UserID;
		msg.Phone = Iter->second.PhoneNumber;
		msg.Result = true;
		msg.ShopID = Iter->second.ShopID;
		msg.ShopOnlyID = Iter->second.ShopOnlyID;
		msg.ShopSum = Iter->second.ShopSum;
		msg.ErrorID = ROE_PhonePay_Sucess;
		g_FishServer.SendNetCmdToCenterServer(&msg);	
		//���Ѿ�����Ķ����洢�����ݿ�ȥ
		DBR_Cmd_AddPhonePayOrderID msgDB;
		SetMsgInfo(msgDB, DBR_AddPhonePayOrderID, sizeof(DBR_Cmd_AddPhonePayOrderID));
		msgDB.dwUserID = Iter->second.UserID;
		msgDB.OrderOnlyID = orderID;
		g_FishServer.SendNetCmdToDB(&msgDB);
		//��ŵ���������ȥ
		m_PhoneOrderIDSet.insert(set<unsigned __int64>::value_type(Iter->second.OrderOnlyID));
		m_PhonePayMap.erase(Iter);
	}
	else if (status.compare("FAILED") == 0)
	{
		g_DBLogManager.LogRolePhonePayLogToDB("��Ӫ���������ն���ʧ��:��ֵʧ��", orderID, Iter->second.UserID, Iter->second.PhoneNumber, Price, SendLogDB);
		OC_Cmd_PhonePay msg;
		SetMsgInfo(msg, GetMsgType(Main_Operate, OC_PhonePay), sizeof(OC_Cmd_PhonePay));
		msg.OrderID = orderID;
		msg.dwUserID = Iter->second.UserID;
		msg.Phone = Iter->second.PhoneNumber;
		msg.Result = false;
		msg.ShopID = Iter->second.ShopID;
		msg.ShopOnlyID = Iter->second.ShopOnlyID;
		msg.ShopSum = Iter->second.ShopSum;
		msg.ErrorID = ROE_PhonePay_HttpError;
		g_FishServer.SendNetCmdToCenterServer(&msg);
		m_PhonePayMap.erase(Iter);
	}
}