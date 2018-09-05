#include "stdafx.h"
#include "NormalRechargeManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include "..\CommonFile\DBLogManager.h"
#include "IApppay\SignUtils.h"
#include "IApppay\CryptHelper.h"
extern void SendLogDB(NetCmd* pCmd);
NormalRechargeManager::NormalRechargeManager()
{

}
NormalRechargeManager::~NormalRechargeManager()
{

}
//void NormalRechargeManager::Test()
//{
//	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
//	if (!pConfig)
//	{
//		return;
//	}
//	string xx = "{transid:\"32031602171911251517\",redirecturl:\"http://ipay.test.happyapk.com:9002/test/iapppay_pc_online.jsp\",cpurl:\"http://www.sina.com.cn/cpurl\"}";
//	string prvKey = pConfig->ApppayPrvKey;
//	EVP_PKEY* vkey = CryptHelper::getKeyByPKCS1(prvKey, 1);
//	if (!vkey)
//	{
//		return;
//	}
//	std::string reqSign;
//	CryptHelper::md5WithRsa(xx, reqSign, vkey);
//
//	bool xs = false;
//}
bool NormalRechargeManager::OnHandleAddOrder(DWORD dwUserID, DWORD OrderID, DWORD ShopID)
{
	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return false;
	}
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{
		ASSERT(false);
		return false;
	}
	DWORD waresid = 0;
	if (Iter->second.RechargeType == 1 || Iter->second.RechargeType == 3)
		waresid = 2;
	else if (Iter->second.RechargeType == 2 || Iter->second.RechargeType == 4)
		waresid = 1;
	else
		waresid = 3;
	UINT64 cporderid = OrderID;
	cporderid = (cporderid << 32) | ShopID;
	//���ⲿ������ֱ���µ� 
	//���ݿͻ��˷������Ĳ��� ���������ַ��� ���Ҽ��� ���ͳ�ȥ
	string transdata = "{\"appid\":\"%s\",\"waresid\":%d,\"cporderid\":\"%u\",\"cpprivateinfo\":\"%llu\",\"price\":%d.00,\"currency\":\"RMB\",\"appuserid\":\"%u\",\"notifyurl\":\"http://%s:1680/%s\"}";
	string UrlData = "transdata=%s&sign=%s&signtype=RSA";
	char PostUrl[1024];
	sprintf_s(PostUrl, CountArray(PostUrl), transdata.c_str(), pConfig->ApppayID, waresid, OrderID,cporderid, Iter->second.dDisCountPrice, dwUserID, pConfig->OperateListenIP,"pay_callback_self.clkj?");
	//���м��ܴ���
	string prvKey = pConfig->ApppayPrvKey;
	EVP_PKEY* vkey = CryptHelper::getKeyByPKCS1(prvKey, 1);
	if (!vkey)
	{
		ASSERT(false);
		return false;
	}
	std::string reqSign;
	CryptHelper::md5WithRsa(PostUrl, reqSign, vkey);
	//����ƴװ�ַ���
	char DestUrl[2048];
	sprintf_s(DestUrl, CountArray(DestUrl), UrlData.c_str(),PostUrl, reqSign.c_str());
	//�ַ���������� ���ǽ��м���
	//string Url = UrlEncode(DestUrl);
	//���ַ������ͳ�ȥ Post
	WORD RequestID = g_FishServer.GetNormalRechargeID();
	UINT64* pInt = new UINT64(cporderid);
	if (!g_FishServer.GetHttpClient().AddPostRequest((UINT_PTR)pInt, RequestID, "payapi/order", DestUrl))
	{
		delete(pInt);
		return false;
	}
	OrderInfo orderInfo;
	orderInfo.OnlyID = cporderid;
	orderInfo.dwUserID = dwUserID;
	orderInfo.OrderID = OrderID;
	orderInfo.ShopID = ShopID;
	m_OrderMap.insert(HashMap<UINT64, OrderInfo>::value_type(orderInfo.OnlyID, orderInfo));
	return true;
}
void NormalRechargeManager::OnHandleAddOrderResult(UINT64 ID, char* pData, DWORD Length)
{
	HashMap<UINT64, OrderInfo>::iterator Iter =  m_OrderMap.find(ID);
	if (Iter == m_OrderMap.end())
	{
		ASSERT(false);
		return;
	}
	OC_Cmd_AddNormalOrderID msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OC_AddNormalOrderID), sizeof(OC_Cmd_AddNormalOrderID));
	msg.dwUserID = Iter->second.dwUserID;
	msg.OrderID = Iter->second.OrderID;
	msg.ShopID = Iter->second.ShopID;
	msg.Transid[0] = 0;
	msg.Sign[0] = 0;
	msg.Result = false;

	//�����ַ��� ���ǽ��н�������
	string Url = pData;// UrlDecode(pData);
	size_t BeginIndex = Url.find_first_of("{");
	size_t EndIndex = Url.find_first_of("}");
	if (BeginIndex == -1 || EndIndex == -1 || EndIndex <= BeginIndex)
	{
		ASSERT(false);
		g_FishServer.SendNetCmdToCenterServer(&msg);
		return;
	}
	string transdata = Url.substr(BeginIndex, EndIndex - BeginIndex + 1);
	//���ж��ǳɹ�����ʧ��
	HashMap<UINT, std::string> pMap;
	SplitStrByIpay(Url.c_str(), Url.length(), pMap);
	if (pMap.count(m_NormalRechargeAddOrderCrcInfo.codeCrc) == 1)
	{
		//ʧ����
		if (strstr(Url.c_str(), "3005") != 0)
			Log("**�������µ�ʧ��:�̻����������쳣:%s", Url.c_str());
		ASSERT(false);
		g_FishServer.SendNetCmdToCenterServer(&msg);
		return;
	}
	else if (pMap.count(m_NormalRechargeAddOrderCrcInfo.transidCrc) == 1 && pMap.count(m_NormalRechargeAddOrderCrcInfo.signCrc) == 1)
	{
		//��֤ǩ��
		string sign = pMap[m_NormalRechargeAddOrderCrcInfo.signCrc];
		if (sign.length() == 0)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
		if (!pConfig)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		//2.���ַ��������и�ΪMap ��Ҫ�ṩ��Կ ���н��� ����
		string platPkey = pConfig->ApppayKey;//ƽ̨��Կ д�������ļ�ȥ
		EVP_PKEY* pkey = CryptHelper::getKeyByPKCS1(platPkey, 0);
		if (!pkey)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		if (CryptHelper::verifyMd5WithRsa(transdata, sign, pkey) != 0)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		//��֤�ɹ� ��������ͻ���ȥ
		string transid = pMap[m_NormalRechargeAddOrderCrcInfo.transidCrc];
		if (transid.length() == 0)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		msg.Result = true;
		strncpy_s(msg.Transid, CountArray(msg.Transid), transid.c_str(), transid.length());
		//��Ϊ�ͻ���û��ǩ������ �������˰����ͻ��˼����ǩ�� ���͵��ͻ���ȥ
		char ClientSign[1024] = { 0 };
		sprintf_s(ClientSign, CountArray(ClientSign),"{transid:\"%s\",redirecturl:\"http://%s:1680/%s\",cpurl:\"%s\"}", msg.Transid, pConfig->OperateListenIP, "pay_callback_self.clkj","");
		Log(ClientSign);
		//����ַ�������м���
		string prvKey = pConfig->ApppayPrvKey;
		EVP_PKEY* vkey = CryptHelper::getKeyByPKCS1(prvKey, 1);
		if (!vkey)
		{
			ASSERT(false);
			g_FishServer.SendNetCmdToCenterServer(&msg);
			return;
		}
		std::string reqSign;
		CryptHelper::md5WithRsa(ClientSign, reqSign, vkey);
		//ǩ������ //reqSign
		strncpy_s(msg.Sign, CountArray(msg.Sign), reqSign.c_str(), reqSign.length());
		g_FishServer.SendNetCmdToCenterServer(&msg);
		return;	
	}
	else
	{
		ASSERT(false);
		g_FishServer.SendNetCmdToCenterServer(&msg);
		return;
	}
}
void NormalRechargeManager::OnHandleNormalRechargeInfo(string UrlStr)
{
	UrlStr = UrlDecode(UrlStr);

	LogInfoToFile("LogRechargeInfo.txt", "��ͨ��ֵ�첽�ص�: %s", UrlStr.c_str());
	//���յ��ַ���
	if (UrlStr.length() == 0)
	{
		ASSERT(false);
		return;
	}
	size_t BeginIndex = UrlStr.find_first_of("{");
	size_t EndIndex = UrlStr.find_first_of("}");
	if (BeginIndex == -1 || EndIndex == -1 || EndIndex <= BeginIndex)
	{
		ASSERT(false);
		LogInfoToFile("LogRechargeInfo.txt", "��ͨ��ֵ�첽�ص� ���ݴ���");
		return;
	}
	string transdata = UrlStr.substr(BeginIndex, EndIndex - BeginIndex + 1);
	if (transdata.length() == 0)
	{
		LogInfoToFile("LogRechargeInfo.txt", "��ͨ��ֵ�첽�ص� ��������Ϊ��");
		ASSERT(false);
		return;
	} 
	HashMap<UINT, std::string> pMap;
	SplitStrByIpay(UrlStr.c_str(), UrlStr.length(), pMap);
	if (pMap.count(m_NormalRechargeCrcInfo.signCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.transtypeCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.transidCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.appuseridCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.appidCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.waresidCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.feetypeCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.moneyCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.currencyCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.resultCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.transtimeCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.cpprivateCrc) != 1 ||
		pMap.count(m_NormalRechargeCrcInfo.cporderidCrc) != 1
		)
	{
		LogInfoToFile("LogRechargeInfo.txt", "��ͨ��ֵ�첽�ص� �������ݲ�����");
		ASSERT(false);
		return;
	}
	string cporderid = pMap[m_NormalRechargeCrcInfo.cporderidCrc];
	DWORD OrderID = strtoul(cporderid.c_str(), null, 10);
	//2.���ǩ���ַ�
	string sign = pMap[m_NormalRechargeCrcInfo.signCrc];
	if (sign.length() == 0)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:ǩ������Ϊ��", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return;
	}
	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:ϵͳ����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return;
	}
	//2.���ַ��������и�ΪMap ��Ҫ�ṩ��Կ ���н��� ����
	string platPkey = pConfig->ApppayKey;//ƽ̨��Կ д�������ļ�ȥ
	EVP_PKEY* pkey = CryptHelper::getKeyByPKCS1(platPkey, 0);
	if (!pkey)
	{
		//������Կʧ��
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:��ȡ��Կʧ��", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return;
	}
	if (CryptHelper::verifyMd5WithRsa(transdata, sign, pkey) != 0)
	{
		//��֤ʧ�� 
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:ǩ����֤ʧ��", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return;
	}
	//��֤�ɹ��� ������Ҫ���ַ��������и� Ϊ �����Ĳ���
	string transtype = pMap[m_NormalRechargeCrcInfo.transtypeCrc];
	string transid = pMap[m_NormalRechargeCrcInfo.transidCrc];
	string appuserid = pMap[m_NormalRechargeCrcInfo.appuseridCrc];
	string appid = pMap[m_NormalRechargeCrcInfo.appidCrc];
	string waresid = pMap[m_NormalRechargeCrcInfo.waresidCrc];
	string feetype = pMap[m_NormalRechargeCrcInfo.feetypeCrc];
	string money = pMap[m_NormalRechargeCrcInfo.moneyCrc];
	string currency = pMap[m_NormalRechargeCrcInfo.currencyCrc];
	string result = pMap[m_NormalRechargeCrcInfo.resultCrc];
	string transtime = pMap[m_NormalRechargeCrcInfo.transtimeCrc];
	//��ѡ����
	string cpprivate = pMap[m_NormalRechargeCrcInfo.cpprivateCrc];
	string paytype = pMap[m_NormalRechargeCrcInfo.paytypeCrc];

	OnHandleRechargeByParam(transtype, cporderid, transid, appuserid, appid, waresid, feetype, money, currency, result, transtime, cpprivate, paytype);
}
void NormalRechargeManager::OnHandleRechargeByParam(string transtype, string cporderid, string transid, string appuserid, string appid, string waresid,
	string feetype, string money, string currency, string result, string transtime, string cpprivate, string paytype)
{
	DWORD OrderID = strtoul(cporderid.c_str(), null, 10);
	OperateConfig* pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:ϵͳ����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	//������֤
	//appid
	if (appid.compare(pConfig->ApppayID) != 0)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:appid����", cporderid,0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	DWORD Type = strtoul(transtype.c_str(), null, 10);
	if (Type != 0)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:transtype����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	DWORD UserID = strtoul(appuserid.c_str(), null, 10);
	//DWORD ShopID = strtoul(waresid.c_str(), null, 10);
	DWORD Price = strtoul(money.c_str(), null, 10);
	UINT64 Param = _strtoui64(cpprivate.c_str(), null, 10);
	DWORD CheckOrderID = static_cast<DWORD>(Param >> 32);
	DWORD CheckShopItemID = (DWORD)Param;
	bool Result = (strtoul(result.c_str(), null, 10) == 0?true:false);
	if (CheckOrderID != OrderID)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:����ID У�����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	if (!Result)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:������ش���", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	/*if (ShopID != CheckShopItemID)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:��ƷID У�����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}*/
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(CheckShopItemID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:��Ʒ������ У�����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	if (Iter->second.dDisCountPrice != Price)//�۸���ȷ
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:�۸���ȷ У�����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	if (currency.compare("RMB") != 0)
	{
		DBR_Cmd_DelRechargeOrderInfo msg;
		SetMsgInfo(msg, DBR_DelRechargeOrderInfo, sizeof(DBR_Cmd_DelRechargeOrderInfo));
		msg.OrderID = OrderID;
		g_FishServer.SendNetCmdToDB(&msg);

		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:�������ʹ��� У�����", cporderid, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	//ȫ����֤��� ���Ƿ��͵����ݿ�ȥ
	DBR_Cmd_QueryRechargeOrderInfo msg;
	SetMsgInfo(msg, DBR_QueryRechargeOrderInfo, sizeof(DBR_Cmd_QueryRechargeOrderInfo));
	msg.OrderID = OrderID;
	msg.Price = Price;
	msg.CheckShopID = CheckShopItemID;
	msg.CheckUserID = UserID;
	g_FishServer.SendNetCmdToDB(&msg);
}
void NormalRechargeManager::OnHandleNormalRechargeInfoResult(DBO_Cmd_QueryRechargeOrderInfo* pMsg)
{
	if (!pMsg)
	{
		ASSERT(false);
		return;
	}
	char OrderStr[32] = { 0 };
	_itoa_s(pMsg->OrderID, OrderStr, CountArray(OrderStr), 10);
	if (pMsg->CheckUserID != pMsg->dwUserID)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:���ID У�����", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	if (pMsg->CheckShopID != pMsg->ShopID)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵʧ��:��ƷID У�����", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	if (pMsg->OrderID == 0 || pMsg->dwUserID == 0)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ:�����Ż�����Ҳ�����", OrderStr, pMsg->dwUserID, "", "", "", pMsg->ShopID, pMsg->Price * 100, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}
	//�жϼ۸��Ƿ���ȷ
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(pMsg->ShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:��Ʒ������", OrderStr, pMsg->dwUserID, "", "", "", pMsg->ShopID, pMsg->Price * 100, 0, 0, 0, 0, 0, 0, SendLogDB);
		return ;
	}
	if (Iter->second.dDisCountPrice != pMsg->Price)//�۸���ȷ
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:�۸���ȷ", OrderStr, pMsg->dwUserID, "", "", "", pMsg->ShopID, pMsg->Price * 100, 0, 0, 0, 0, 0, 0, SendLogDB);
		return ;
	}
	OperateConfig* pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ��������ͨ��ֵ��֤ʧ��:ϵͳ����", OrderStr, pMsg->dwUserID, "", "", "", pMsg->ShopID, pMsg->Price * 100, 0, 0, 0, 0, 0, 0, SendLogDB);
		return;
	}

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
	CheckMsgSize(PageSize);
	msg->SetCmdType(GetMsgType(Main_Operate, OC_UseRMB));
	msg->SetCmdSize(static_cast<WORD>(PageSize));
	msg->rechargeInfo.Price = pMsg->Price * 100;//��ǰ���� �� �۸� ��λΪ��
	msg->rechargeInfo.UserID = pMsg->dwUserID;
	msg->rechargeInfo.ShopItemID = pMsg->ShopID;
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
}