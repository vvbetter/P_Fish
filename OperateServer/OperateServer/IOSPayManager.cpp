#include "stdafx.h"
#include "IOSPayManager.h"
#include "FishServer.h"
#include "..\CommonFile\DBLogManager.h"
#include "..\CommonFile\FishServerConfig.h"
extern void SendLogDB(NetCmd* pCmd);
IOSPayManager::IOSPayManager()
{

}
IOSPayManager::~IOSPayManager()
{

}
void IOSPayManager::OnInit()
{
	//OnSendOrderToIOSServer(10000, "xudaxuda");
}
void IOSPayManager::OnSendOrderToIOSServer(DWORD dwUserID, string OrderInfo/*, bool IsTest*/)
{
	/*if (IsTest)
	{
		LogInfoToFile("LogRechargeInfo.txt", "IOS ɳ�в��Գ�ֵ OrderInfo=%s", OrderInfo.c_str());
	}
	else
	{
		LogInfoToFile("LogRechargeInfo.txt", "IOS ��ʽ��ֵ OrderInfo=%s", OrderInfo.c_str());
	}*/
	LogInfoToFile("LogRechargeInfo.txt", "IOS ��ʽ��ֵ OrderInfo=%s", OrderInfo.c_str());

	AE_CRC_PAIRS pThree;
	AECrc32(pThree, OrderInfo.c_str(), OrderInfo.length(), 0, 0x73373);
	//����ת����CRC ����
	DWORD Crc1 = pThree.Crc1;
	DWORD Crc2 = pThree.Crc2;
	unsigned __int64 i64Value = Crc1; //������
	i64Value = (i64Value << 32);
	i64Value += Crc2;

	//����Http���� �������ŷ��ͳ�ȥ
	char strURL[20480];
	//mobile/charge.json?
	sprintf_s(strURL, CountArray(strURL), "{1}{\"receipt-data\" : \"%s\"}", OrderInfo.c_str());
	
	IOSOrderInfo pInfo;
	pInfo.OrderID = i64Value;
	pInfo.dwUserID = dwUserID;
	//pInfo.IsTest = IsTest;
	pInfo.ItemID = 0;
	strncpy_s(pInfo.OrderInfo, CountArray(pInfo.OrderInfo), OrderInfo.c_str(), strlen(OrderInfo.c_str()));
	m_Map.insert(HashMap<UINT64, IOSOrderInfo>::value_type(pInfo.OrderID,pInfo));//�洢����

	UINT64* pID = new UINT64(pInfo.OrderID);
	WORD RequestID = g_FishServer.GetIOSPayID();
	if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pID, RequestID, "verifyReceipt", strURL,true))
	{
		delete(pID);
	}
	/*if (IsTest)
	{
		WORD RequestID = g_FishServer.GetIOSPayTestID();
		if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pID, RequestID, "verifyReceipt", strURL))
		{
			delete(pID);
		}
	}
	else
	{
		WORD RequestID = g_FishServer.GetIOSPayID();
		if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pID, RequestID, "verifyReceipt", strURL))
		{
			delete(pID);
		}
	}*/
}
void IOSPayManager::OnSendOrderToIOSResult(unsigned __int64 Order64ID, char* pData, DWORD Length)
{
	LogInfoToFile("LogRechargeInfo.txt", "IOS �������� ", pData);
	HashMap<UINT64, IOSOrderInfo>::iterator Iter = m_Map.find(Order64ID);
	if (Iter == m_Map.end())
	{
		ASSERT(false);
		return; 
	}
	UINT64 OrderSetID = GetOrderSetID(pData);
	char OrderStr[64] = {0};
	sprintf_s(OrderStr, CountArray(OrderStr), "%llu", OrderSetID);

	if (OrderSetID == 0)
	{
		ASSERT(false);
		return;
	}
	else if (Length != 0 && strstr(pData, "\"status\":0") != null)
	{
		//�ɹ�
		DWORD ItemID = GetItemID(pData);
		if (ItemID == 0)
		{
			//
			g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤ʧ��:��ƷIDΪ0", OrderStr, Iter->second.dwUserID, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		}
		else
		{
			//��ĿǰΪֹ ���ǳɹ��� ������Ҫ���д���  ѯ�����ݿ� �Ƿ���� �Ѿ������ID 100.0
			Iter->second.ItemID = ItemID;
			//���ú����ݺ� ���ǻ�ȡ������Ķ���ID
			//��������ȥ ��֤ �Ƿ���� �Ѿ�ʹ�õĶ�����
			if (g_FishServer.GetOrderOnlyManager().IsExists(OrderSetID, OT_IOS))
			{
				g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤ʧ��:�����ظ�", OrderStr, Iter->second.dwUserID, "", "", "", ItemID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
			}
			else
			{
				if (OnHandleRechargeResult(Order64ID, Iter->second.dwUserID, ItemID))
				{
					g_FishServer.GetOrderOnlyManager().OnAddOrderInfo(Iter->second.dwUserID, OrderSetID, OT_IOS);
					g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤�ɹ�:��֤�ɹ�", OrderStr, Iter->second.dwUserID, "", "", "", ItemID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
				}
			}
		}
	}
	else
	{
		//ʧ��
		//if (!Iter->second.IsTest)
		//{
		//	OnSendOrderToIOSServer(Iter->second.dwUserID, Iter->second.OrderInfo, true);
		//}
		//else
		//{
		//	//��ҳ�ֵʧ����
		//	g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤ʧ��:״̬ʧ��", OrderStr, Iter->second.dwUserID, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		//}

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤ʧ��:״̬ʧ��", OrderStr, Iter->second.dwUserID, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
	}
	m_Map.erase(Iter);
}
const char *product_id = "\"product_id\":\"";
const char *trans_id = "\"transaction_id\":\"";
const int product_size = strlen(product_id);
const int trans_size = strlen(trans_id);
DWORD IOSPayManager::GetItemID(string str)
{
	const char* pDest = strstr(str.c_str(), product_id);
	if (pDest == null)
		return 0;
	const char *pcStart = pDest + product_size;
	return strtoul(pcStart, null, 10);
}
UINT64 IOSPayManager::GetOrderSetID(string str)
{
	const char* pDest = strstr(str.c_str(), trans_id);
	if (pDest == null)
		return 0;
	const char *pcStart = pDest + trans_size;
	return strtoull(pcStart, null, 10);
}
bool IOSPayManager::OnHandleRechargeResult(UINT64 OrderID, DWORD dwUserID, DWORD ItemID)
{
	char OrderStr[64] = { 0 };
	sprintf_s(OrderStr, CountArray(OrderStr), "%llu", OrderID);
	ItemID = ItemID - 100;
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ItemID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������IOS��ֵ��֤ʧ��:��Ʒ���ò�����", OrderStr, dwUserID, "", "", "", ItemID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	//������������ 
	//������Ҫ��֯����ͻ���ȥ
	//����Щ�ַ��� ת��ΪTCHAR  ���� ���뵽��������ȥ

	string orderid = OrderStr;
	string ChannelCode = "";
	string channelOrderid = "";
	string channelLabel = "";

	UINT Count = 0;
	TCHAR* pOrderID = CharToWChar(orderid.c_str(), Count);
	TCHAR* pChannelCode = CharToWChar(ChannelCode.c_str(), Count);
	TCHAR* pChannelOrderid = CharToWChar(channelOrderid.c_str(), Count);
	TCHAR* pChannelLabel = CharToWChar(channelLabel.c_str(), Count);

	DWORD Length = sizeof(StringArrayData)+(orderid.length() - 1)*sizeof(TCHAR)+
		sizeof(StringArrayData)+(ChannelCode.length() - 1)*sizeof(TCHAR)+
		sizeof(StringArrayData)+(channelOrderid.length() - 1)*sizeof(TCHAR)+
		sizeof(StringArrayData)+(channelLabel.length() - 1)*sizeof(TCHAR);


	DWORD PageSize = sizeof(OC_Cmd_UseRMB)-sizeof(BYTE)+Length;
	OC_Cmd_UseRMB* msg = (OC_Cmd_UseRMB*)malloc(PageSize);
	CheckMsgSizeReturn(PageSize);
	msg->SetCmdType(GetMsgType(Main_Operate, OC_UseRMB));
	msg->SetCmdSize(static_cast<WORD>(PageSize));
	msg->rechargeInfo.Price = Iter->second.dDisCountPrice * 100;
	msg->rechargeInfo.UserID = dwUserID;
	msg->rechargeInfo.ShopItemID = ItemID;
	msg->rechargeInfo.ShopNum = 1;
	msg->rechargeInfo.HandleSum = 4;
	msg->rechargeInfo.Sum = Length;
	msg->rechargeInfo.FreePrice = 0;
	DWORD BeginIndex = 0;
	{
		StringArrayData* pString = (StringArrayData*)&msg->rechargeInfo.Array[BeginIndex];
		pString->Length = ConvertDWORDToBYTE(orderid.length()*sizeof(TCHAR));
		memcpy_s(pString->Array, orderid.length()*sizeof(TCHAR), pOrderID, orderid.length()*sizeof(TCHAR));
		BeginIndex += sizeof(StringArrayData)+(orderid.length() - 1)*sizeof(TCHAR);
	}
	{
		StringArrayData* pString = (StringArrayData*)&msg->rechargeInfo.Array[BeginIndex];
		pString->Length = ConvertDWORDToBYTE(ChannelCode.length()*sizeof(TCHAR));
		memcpy_s(pString->Array, ChannelCode.length() * sizeof(TCHAR), pChannelCode, ChannelCode.length() * sizeof(TCHAR));
		BeginIndex += sizeof(StringArrayData)+(ChannelCode.length() - 1)*sizeof(TCHAR);
	}
	{
		StringArrayData* pString = (StringArrayData*)&msg->rechargeInfo.Array[BeginIndex];
		pString->Length = ConvertDWORDToBYTE(channelOrderid.length()*sizeof(TCHAR));
		memcpy_s(pString->Array, channelOrderid.length() * sizeof(TCHAR), pChannelOrderid, channelOrderid.length() * sizeof(TCHAR));
		BeginIndex += sizeof(StringArrayData)+(channelOrderid.length() - 1)*sizeof(TCHAR);
	}
	{
		StringArrayData* pString = (StringArrayData*)&msg->rechargeInfo.Array[BeginIndex];
		pString->Length = ConvertDWORDToBYTE(channelLabel.length()*sizeof(TCHAR));
		memcpy_s(pString->Array, channelLabel.length() * sizeof(TCHAR), pChannelLabel, channelLabel.length() * sizeof(TCHAR));
		BeginIndex += sizeof(StringArrayData)+(channelLabel.length() - 1)*sizeof(TCHAR);
	}

	g_FishServer.SendNetCmdToCenterServer(msg);
	free(msg);//����ҳ�ֵ�����ݷ��͵� GameServerȥ
	return true;
}
//{
//	environment = Sandbox;
//	receipt = 
//	{
//		"adam_id" = 0;
//		"app_item_id" = 0;
//		"application_version" = 0;
//		"bundle_id" = "com.leduo.buyu3d";
//		"download_id" = 0;
//		"in_app" = (
//		{
//			"is_trial_period" = false;
//			"original_purchase_date" = "2016-02-26 02:11:40 Etc/GMT";
//			"original_purchase_date_ms" = 1456452700000;
//			"original_purchase_date_pst" = "2016-02-25 18:11:40 America/Los_Angeles";
//			"original_transaction_id" = 1000000196090055;
//			"product_id" = 106;
//			"purchase_date" = "2016-02-26 02:11:40 Etc/GMT";
//			"purchase_date_ms" = 1456452700000;
//			"purchase_date_pst" = "2016-02-25 18:11:40 America/Los_Angeles";
//			quantity = 1;
//			"transaction_id" = 1000000196090055;
//		}
//		);
//		"original_application_version" = "1.0";
//		"original_purchase_date" = "2013-08-01 07:00:00 Etc/GMT";
//		"original_purchase_date_ms" = 1375340400000;
//		"original_purchase_date_pst" = "2013-08-01 00:00:00 America/Los_Angeles";
//		"receipt_creation_date" = "2016-02-26 02:11:40 Etc/GMT";
//		"receipt_creation_date_ms" = 1456452700000;
//		"receipt_creation_date_pst" = "2016-02-25 18:11:40 America/Los_Angeles";
//		"receipt_type" = ProductionSandbox;
//		"request_date" = "2016-02-26 02:11:42 Etc/GMT";
//		"request_date_ms" = 1456452702952;
//		"request_date_pst" = "2016-02-25 18:11:42 America/Los_Angeles";
//		"version_external_identifier" = 0;
//	};
//	status = 0;
//}
