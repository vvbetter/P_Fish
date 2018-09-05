#include "stdafx.h"
#include "UseRMBManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include "..\CommonFile\DBLogManager.h"
extern void SendLogDB(NetCmd* pCmd);
UseRMBManager::UseRMBManager()
{
}
UseRMBManager::~UseRMBManager()
{

}
void UseRMBManager::OnInit()
{
	/*DBR_Cmd_LoadRechageOrderID msg;
	SetMsgInfo(msg, DBR_LoadRechageOrderID, sizeof(DBR_Cmd_LoadRechageOrderID));
	msg.DiffDay = 2;
	g_FishServer.SendNetCmdToDB(&msg);*/
}
//void UseRMBManager::OnLoadAllOrderID(DBO_Cmd_LoadRechageOrderID* pMsg)
//{
//	if (!pMsg)
//	{
//		ASSERT(false);
//		return;
//	}
//	if (pMsg->States & MsgBegin)
//	{
//		m_OrderIDSet.clear();
//	}
//	for (WORD i = 0; i < pMsg->Sum; ++i)
//	{
//		m_OrderIDSet.insert(std::set<unsigned __int64>::value_type(pMsg->Array[i]));
//	}
//	if (pMsg->States & MsgEnd)
//	{
//		//Operate ���Ե�½��
//		g_FishServer.OnFinish();//Operate ���Կ�����
//	}
//}
bool UseRMBManager::OnHandleHttpInfoByUserRecharge(string orderid, string price, string ChannelCode, string channelOrderid, string channelLabel, string callbackInfo, string sign)
{
	//�ɰ汾
	LogInfoToFile("LogRechargeInfo.txt", "��ֵ�첽�ص�: orderid=%s, price=%s,ChannelCode=%s, channelOrderid =%s, channelLabel =%s, callbackInfo=%s, sign=%s", orderid.c_str(), price.c_str(), ChannelCode.c_str(), channelOrderid.c_str(), channelLabel.c_str(), callbackInfo.c_str(), sign.c_str());

	//����Ӧ�ÿ�������������ڵ�Center����
	//CustParam Ӧ�ð���  UserID  /ShopID 
	DWORD dPrice = strtoul(price.c_str(), null, 10);
	string BackInfo = Base64Decode(callbackInfo.c_str(), callbackInfo.length());
	UINT64 Param = _strtoui64(BackInfo.c_str(), null, 10);//_atoi64(BackInfo.c_str());
	DWORD UserID = static_cast<DWORD>(Param >> 32);
	DWORD ShopItemID = (DWORD)Param;

	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopItemID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:��Ʒ������", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, 0, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	if (Iter->second.dDisCountPrice * 100 != dPrice)//�۸���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�۸���ȷ", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, 0, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	//�Ե�ǰ�Ķһ����ݽ����жϴ���  ��֤ �һ����Ƿ�����Ч�� 
	//��֤ǩ��
	OperateConfig* pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:ϵͳ����", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, 0, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	//g_FishServer.ShowInfoToWin("�������������г�ֵ3");
	string Product = pConfig->ProductSecret;
	string CheckStr = orderid + price + callbackInfo + Product;
	string Md5Str = md5(CheckStr);//���ܺ��ǩ��
	if (Md5Str.compare(sign) != 0)//��֤ǩ���Ƿ���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:MD5��֤ʧ��", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, 0, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}

	//g_FishServer.ShowInfoToWin("�������������г�ֵ4");
	//��֤�һ��� �Ƿ����
	//1.������תΪ3��DWORD 3��DWORD һ�� ���д���
	AE_CRC_PAIRS pThree;
	AECrc32(pThree, orderid.c_str(), orderid.length(), 0, 0x73573);
	//����ת����CRC ����
	DWORD Crc1 = pThree.Crc1;
	DWORD Crc2 = pThree.Crc2;
	unsigned __int64 i64Value = Crc1;
	i64Value = (i64Value << 32);
	i64Value += Crc2;
	//��ö�����Ψһ��� ����д�����ݿ� ���߽����ж�
	if (g_FishServer.GetOrderOnlyManager().IsExists(i64Value, OT_SDK))
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�ظ��Ķ���", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, 0, 0, 0, 0, 0, 0, SendLogDB);
		//LogInfoToFile("LogRechargeInfo.txt", "��ֵ�첽�ص� �����ظ�ִ��: orderid=%s", orderid.c_str());
		return false;//�����Ŵ���
	}
	//g_FishServer.ShowInfoToWin("�������������г�ֵ5");
	//��Ϊ�һ�������ظ����� ������Ҫ����֤�������֤ ����
	//1.����֤�뱣�浽���ݿ�ȥ
	g_FishServer.GetOrderOnlyManager().OnAddOrderInfo(UserID, i64Value, OT_SDK);
	//DBR_Cmd_AddRechageOrderID msgDB;
	//SetMsgInfo(msgDB, DBR_AddRechageOrderID, sizeof(DBR_Cmd_AddRechageOrderID));
	//msgDB.dwUserID = UserID;
	//msgDB.OrderOnlyID = i64Value;
	//g_FishServer.SendNetCmdToDB(&msgDB);
	////2.����֤�뱣�浽�ڴ���ȥ
	//m_OrderIDSet.insert(std::set<unsigned __int64>::value_type(i64Value));
	//������������ 
	//������Ҫ��֯����ͻ���ȥ
	//����Щ�ַ��� ת��ΪTCHAR  ���� ���뵽��������ȥ
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
	msg->rechargeInfo.Price = dPrice;
	msg->rechargeInfo.UserID = UserID;
	msg->rechargeInfo.ShopItemID = ShopItemID;
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
bool UseRMBManager::OnHandleHttpInfoByUserRecharge(string orderid, string price, string ChannelCode, string channelOrderid, string channelLabel, string callbackInfo, string sign, string sign2, string version, string freePrice, string sdkCode)
{
	// string sign2, string version, string freePrice, string sdkCode
	LogInfoToFile("LogRechargeInfo.txt", "��ֵ�첽�ص�: orderid=%s, price=%s,freeprice=%s,version=%s,sdkCode=%s, ChannelCode=%s, channelOrderid =%s, channelLabel =%s, callbackInfo=%s, sign=%s ,sign2=%s", orderid.c_str(), price.c_str(), freePrice.c_str(), version.c_str(), sdkCode.c_str(), ChannelCode.c_str(), channelOrderid.c_str(), channelLabel.c_str(), callbackInfo.c_str(), sign.c_str(), sign2.c_str());

	//����Ӧ�ÿ�������������ڵ�Center����
	//CustParam Ӧ�ð���  UserID  /ShopID 
	DWORD dPrice = strtoul(price.c_str(), null, 10);
	DWORD FreePrice = strtoul(freePrice.c_str(), null, 10);
	string BackInfo = Base64Decode(callbackInfo.c_str(), callbackInfo.length());
	UINT64 Param = _strtoui64(BackInfo.c_str(), null, 10);//_atoi64(BackInfo.c_str());
	DWORD UserID = static_cast<DWORD>(Param >> 32);
	DWORD ShopItemID = (DWORD)Param;
	/*if (dPrice + FreePrice == 2800)//��ʱ��
	{
	UserID = (DWORD)Param;
	ShopItemID = 13;
	}*/
	/*if (sdkCode.length() == 0 || strlen(sdkCode.c_str()) == 0)
	sdkCode = channelLabel;
	if (freePrice.length() == 0 || strlen(freePrice.c_str()) == 0)
	freePrice = "0";
	if (version.compare("v2.0") != 0)
	{
	g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�汾����", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
	return false;
	}
	if (FreePrice != 0 && sdkCode.compare("anzhi") != 0)
	{
	g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�ǰ��������������", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
	return false;
	}*/
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopItemID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:��Ʒ������", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	if (Iter->second.dDisCountPrice * 100 != dPrice)//�۸���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�۸���ȷ", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	//�Ե�ǰ�Ķһ����ݽ����жϴ���  ��֤ �һ����Ƿ�����Ч�� 
	//��֤ǩ��
	OperateConfig* pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:ϵͳ����", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}
	//g_FishServer.ShowInfoToWin("�������������г�ֵ3");
	string Product = pConfig->ProductSecret;
	string CheckStr = orderid + price + callbackInfo + Product;
	string Md5Str = md5(CheckStr);//���ܺ��ǩ��
	if (Md5Str.compare(sign) != 0)//��֤ǩ���Ƿ���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:MD5��֤ʧ��", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		return false;
	}

	string CheckSign2Str = "callbackInfo=" + callbackInfo + "&channelCode=" + ChannelCode + "&channelLabel=" + channelLabel + "&channelOrderId=" + channelOrderid + "&freePrice=" + freePrice + "&orderId=" + orderid + "&price=" + price + "&sdkCode=" + sdkCode + "&version=" + version + "&" + Product;
	string Md5Str2 = md5(CheckSign2Str);//���ܺ��ǩ��
	if (Md5Str2.compare(sign2) != 0)//��֤ǩ���Ƿ���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:MD5��֤2ʧ��", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		//LogInfoToFile("LogRechargeInfo.txt", "��ֵ�첽�ص� ����ʧ�� MD5У��ʧ��: orderid=%s ChannelMD5=%s,ConfigMD5=%s", orderid.c_str(), sign.c_str(), Md5Str.c_str());
		return false;
	}

	//g_FishServer.ShowInfoToWin("�������������г�ֵ4");
	//��֤�һ��� �Ƿ����
	//1.������תΪ3��DWORD 3��DWORD һ�� ���д���
	AE_CRC_PAIRS pThree;
	AECrc32(pThree, orderid.c_str(), orderid.length(), 0, 0x73573);
	//����ת����CRC ����
	DWORD Crc1 = pThree.Crc1;
	DWORD Crc2 = pThree.Crc2;
	unsigned __int64 i64Value = Crc1;
	i64Value = (i64Value << 32);
	i64Value += Crc2;
	//��ö�����Ψһ��� ����д�����ݿ� ���߽����ж�
	//std::set<unsigned __int64>::iterator IterFind = m_OrderIDSet.find(i64Value);
	if (g_FishServer.GetOrderOnlyManager().IsExists(i64Value, OT_SDK))
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������֤ʧ��:�ظ��Ķ���", orderid, UserID, ChannelCode, channelOrderid, channelLabel, ShopItemID, dPrice, FreePrice, 0, 0, 0, 0, 0, SendLogDB);
		//LogInfoToFile("LogRechargeInfo.txt", "��ֵ�첽�ص� �����ظ�ִ��: orderid=%s", orderid.c_str());
		return false;//�����Ŵ���
	}
	//g_FishServer.ShowInfoToWin("�������������г�ֵ5");
	//��Ϊ�һ�������ظ����� ������Ҫ����֤�������֤ ����
	//1.����֤�뱣�浽���ݿ�ȥ
	//DBR_Cmd_AddRechageOrderID msgDB;
	//SetMsgInfo(msgDB, DBR_AddRechageOrderID, sizeof(DBR_Cmd_AddRechageOrderID));
	//msgDB.dwUserID = UserID;
	//msgDB.OrderOnlyID = i64Value;
	//g_FishServer.SendNetCmdToDB(&msgDB);
	////2.����֤�뱣�浽�ڴ���ȥ
	//m_OrderIDSet.insert(std::set<unsigned __int64>::value_type(i64Value));
	g_FishServer.GetOrderOnlyManager().OnAddOrderInfo(UserID, i64Value, OT_SDK);
	//������������ 
	//������Ҫ��֯����ͻ���ȥ
	//����Щ�ַ��� ת��ΪTCHAR  ���� ���뵽��������ȥ
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
	msg->rechargeInfo.Price = dPrice;
	msg->rechargeInfo.UserID = UserID;
	msg->rechargeInfo.ShopItemID = ShopItemID;
	msg->rechargeInfo.ShopNum = 1;
	msg->rechargeInfo.HandleSum = 4;
	msg->rechargeInfo.Sum = Length;
	msg->rechargeInfo.FreePrice = FreePrice;
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
