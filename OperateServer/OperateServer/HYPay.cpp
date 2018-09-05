#include "stdafx.h"
#include "HYPay.h"
#include "FishServer.h"
#include "..\CommonFile\DBLogManager.h"
#include "..\CommonFile\FishServerConfig.h"
#include "IApppay\SignUtils.h"
extern void SendLogDB(NetCmd* pCmd);
HYPay::HYPay()
{

}
HYPay::~HYPay()
{

}
void HYPay::HandleRequestToken(DWORD dwUserID, DWORD dwUserIP, DWORD ShopID,DWORD ShopNum, BYTE bClientID, DWORD dwAgentType)
{
	//1.��֯���� ͨ��Http ���з��ͳ�ȥ
	//2.��¼Log
	//3.��¼���ݵ�Map����ȥ
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{
		ASSERT(false);
		__SendRequestTokenFailed(dwUserID, bClientID);
		return;
	}	
	UINT64 OrderID = timeGetTime(); //Ψһ�Ķ���ID
	OrderID = (OrderID << 32) | dwUserID;
	if (g_FishServer.GetOrderOnlyManager().IsExists(OrderID, OT_HY))
	{
		ASSERT(false);
		__SendRequestTokenFailed(dwUserID, bClientID);
		return;
	}
	g_FishServer.GetOrderOnlyManager().OnAddOrderInfo(dwUserID, OrderID, OT_HY);//�����¼һ��ΨһID
	//�����������֯����
	tagHYPayRoleInfo pInfo;
	pInfo.u64OrderID = OrderID;
	pInfo.dwShopID = ShopID;
	pInfo.dwShopNum = ShopNum;
	pInfo.Token = "";
	pInfo.dwUserID = dwUserID;
	pInfo.bClientID = bClientID;
	pInfo.AgentType = dwAgentType;
	m_OrderMap.insert(HashMap<UINT64, tagHYPayRoleInfo>::value_type(OrderID,pInfo));
	__InitPayInfo(OrderID, dwUserID, dwUserIP, ShopID, ShopNum,dwAgentType);//��������ȥHttpȥ
}
void HYPay::__InitPayInfo(UINT64 OrderID, DWORD dwUserID, DWORD dwUserIP, DWORD ShopID,DWORD ShopNum, DWORD dwAgentType)
{
	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{
		ASSERT(false);
		return;
	}
	//��֯��ʼ���ַ��� ���͵�ָ����Http�˿�ȥ
	//ʹ��https �� ���� ������Ҫ����Https 
	DWORD version = 2;
	DWORD pay_type = dwAgentType;//30;
	DWORD agent_id = pConfig->HYAgentID;//�̻���ID
	UINT64 agent_bill_id = OrderID;
	double pay_amt = Iter->second.dDisCountPrice * ShopNum;
	std::string return_url = "www.baidu.com";
	char notifuUrl[512] = { 0 };
	sprintf_s(notifuUrl, CountArray(notifuUrl), "http://%s:1680/%s", pConfig->OperateListenIP, "pay_callback_hy.clkj");
	std::string notify_url = notifuUrl;//�������ļ���ȡ
	//��IP��4��ֵȡ��
	DWORD dwIp1 = dwUserIP & 0xff;
	DWORD dwIp2 = (dwUserIP >> 8) & 0xff;
	DWORD dwIp3 = (dwUserIP >> 16) & 0xff;
	DWORD dwIp4 = (dwUserIP >> 24);
	char ipStr[64] = { 0 };
	sprintf_s(ipStr, CountArray(ipStr), "%u.%u.%u.%u", dwIp1, dwIp2, dwIp3, dwIp4);
	std::string user_ip = ipStr;//���ͻ��˵�IP ת��Ϊ�ַ���
	DWORD user_identity = dwUserID;
	char timeStr[64] = { 0 };
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	sprintf_s(timeStr, CountArray(timeStr), "%04d%02d%02d%02d%02d%02d", pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond);
	UINT64 agent_bill_time = strtoull(timeStr, null, 10);//��ǰʱ�� yyyymmddhhmmss
	char ItemName[12] = { 0 };
	_itoa_s(Iter->second.ID, ItemName, 12, 10);
	std::string goodsName = ItemName;
	DWORD goodsNum = ShopNum;
	UINT64 remark = OrderID;//�Զ������
	//std::string goods_note = "";//����
	std::string Key = pConfig->HYKey;//��Կ �������ļ���ȡ
	std::string goodsNote = "hypay";
	//ǩ����֤
	char sign[20480];
	sprintf_s(sign, CountArray(sign), "version=%u&agent_id=%u&agent_bill_id=%llu&agent_bill_time=%llu&pay_type=%u&pay_amt=%0.2f&notify_url=%s&user_ip=%s&key=%s",
		version, agent_id, agent_bill_id, agent_bill_time, pay_type, pay_amt, notify_url.c_str(), user_ip.c_str(), Key.c_str());
	string Md5Str = md5(sign);
	char Url[20480];
	//Phone/SDK/PayInit.aspx
	sprintf_s(Url, CountArray(Url), "{8}?version=%u&pay_type=%u&agent_id=%u&agent_bill_id=%llu&pay_amt=%0.2f&return_url=%s&notify_url=%s&user_ip=%s&agent_bill_time=%llu&goods_name=%s&goods_num=%u&remark=%llu&goods_note=%s&sign=%s", 
		version, pay_type, agent_id, agent_bill_id, pay_amt, return_url.c_str(), notify_url.c_str(), user_ip.c_str(), agent_bill_time, goodsName.c_str(), goodsNum, remark,goodsNote.c_str(),Md5Str.c_str());
	WORD RequestID = g_FishServer.GetHyTokenID();
	UINT64* pID = new UINT64(agent_bill_id);
	if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, Url, true))//ֱ�ӷ���Get����
	{
		delete(pID);
		return;
	}
}
void HYPay::HandleRequestTokenResult(unsigned __int64 Order64ID, char* pData, DWORD Length)
{
	if (nullptr == pData || Length == 0)
	{
		ASSERT(false);
		return;
	}
	tagHYPayRoleInfo* pInfo = __FindOrderInfo(Order64ID);
	if (nullptr == pInfo)
	{
		ASSERT(false);
		return;
	}
	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return;
	}
	TCHAR* pStr = (TCHAR*)pData;
	UINT count = 0;
	char* pDestData = WCharToChar(pStr, count);
	//<token_id></token_id>  <error></error>
	//���ַ���������ΪXML Ȼ����ж�ȡ
	tinyxml2::XMLDocument pDoc;
	pDoc.Parse(pDestData, count);
	tinyxml2::XMLElement* pTokenNode = pDoc.FirstChildElement("token_id");
	tinyxml2::XMLElement* pErrorNode = pDoc.FirstChildElement("error");
	free(pDestData);
	if (nullptr != pTokenNode)
	{
		std::string strToken = pTokenNode->GetText();//��ȡ��������
		//������������������
		pInfo->Token = strToken;
		//�����������ݵ��ͻ���ȥ
		__SendRequestTokenSucess(pInfo->dwUserID, pConfig->HYAgentID, pInfo->u64OrderID, pInfo->AgentType,strToken, pInfo->bClientID);
		//����Log��¼�����ݿ�ȥ һ����������˽��г�ֵ��¼ ֻ�������� �������֧��
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", Order64ID);
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay�������Ƴɹ�", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
	}
	else if (nullptr != pErrorNode)
	{
		__SendRequestTokenFailed(pInfo->dwUserID, pInfo->bClientID);//��ʾ�����ȡ����ʧ����
		__DelOrderInfo(Order64ID);
		return;
	}
	else
	{
		ASSERT(false);
		return;
	}
}
tagHYPayRoleInfo* HYPay::__FindOrderInfo(UINT64 orderID)
{
	HashMap<UINT64, tagHYPayRoleInfo>::iterator Iter = m_OrderMap.find(orderID);
	if (Iter == m_OrderMap.end())
		return nullptr;
	else
		return &Iter->second;
}
void HYPay::__DelOrderInfo(UINT64 orderID)
{
	m_OrderMap.erase(orderID);
}

void HYPay::__SendRequestTokenFailed(DWORD dwUserID, BYTE bClientID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(bClientID);
	if (nullptr == pClient)
	{
		ASSERT(false);
		return;
	}
	OG_Cmd_HYPay msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OG_HYPay), sizeof(OG_Cmd_HYPay));
	msg.bResult = false;
	msg.dwUserID = dwUserID;
	msg.AgentID = 0;
	msg.OrderID = 0;
	msg.AgentType = 0;
	msg.wTokenLength = 0;
	g_FishServer.SendNetCmdToClient(pClient, &msg);
}
void HYPay::__SendRequestTokenSucess(DWORD dwUserID, UINT32 u32AgentID, UINT64 u64OrderID, UINT32 AgentType, std::string& Token, BYTE bClientID)
{
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(bClientID);
	if (nullptr == pClient)
	{
		ASSERT(false);
		return;
	}
	UINT count = 0;
	TCHAR* tcToken = CharToWChar(Token.c_str(), count);
	if (nullptr == tcToken)
	{
		ASSERT(false);
		return;
	}
	OG_Cmd_HYPay* msg = (OG_Cmd_HYPay*)CreateCmd(GetMsgType(Main_Operate, OG_HYPay), sizeof(OG_Cmd_HYPay)+(count - 1)*sizeof(TCHAR));
	msg->bResult = true;
	msg->dwUserID = dwUserID;
	msg->AgentID = u32AgentID;
	msg->OrderID = u64OrderID;
	msg->AgentType = AgentType;
	msg->wTokenLength = Token.length();
	memcpy_s(msg->strToken, count*sizeof(TCHAR), tcToken, count*sizeof(TCHAR));
	g_FishServer.SendNetCmdToClient(pClient, msg);
	free(tcToken);
	free(msg);
}
string HYPay::RequestCallback(HttpRecvData *pRecvData)
{
	if (nullptr == pRecvData)
		return "error";
	if (pRecvData->KeyValues.count(m_CrcMap.resultCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.payMessageCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.agentIDCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.jNetBillNoCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.agentBillIDCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.payTypeCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.payAmtCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.remarkCrc) != 1 ||
		pRecvData->KeyValues.count(m_CrcMap.signCrc) != 1
		)
	{
		LogInfoToFile("LogRechargeInfo.txt", "HYPay��ֵ�첽�ص� �������ݲ�����");
		ASSERT(false);
		return "error";
	}
	string result = pRecvData->KeyValues[m_CrcMap.resultCrc];
	DWORD dwResult = strtoul(result.c_str(), nullptr, 10);
	string pay_message = pRecvData->KeyValues[m_CrcMap.payMessageCrc];
	string agent_id = pRecvData->KeyValues[m_CrcMap.agentIDCrc];
	string jnet_bill_no = pRecvData->KeyValues[m_CrcMap.jNetBillNoCrc];
	string agent_bull_id = pRecvData->KeyValues[m_CrcMap.agentBillIDCrc];
	UINT64 u64AgentBillID = strtoull(agent_bull_id.c_str(), nullptr, 10);
	string pay_type = pRecvData->KeyValues[m_CrcMap.payTypeCrc];
	DWORD dwpayType = strtoul(pay_type.c_str(), nullptr, 10);
	string pay_amt = pRecvData->KeyValues[m_CrcMap.payAmtCrc];
	double dPayAmt = strtod(pay_amt.c_str(), nullptr);
	string remark = pRecvData->KeyValues[m_CrcMap.remarkCrc];
	UINT64 u64Remark = strtoull(remark.c_str(), nullptr, 10);
	string sign = pRecvData->KeyValues[m_CrcMap.signCrc];
	return __HandlePayResult(dwResult, pay_message, agent_id, jnet_bill_no, u64AgentBillID, dwpayType, dPayAmt, sign, u64Remark);
}
string HYPay::__HandlePayResult(DWORD dwResult, string payMessage, string agentID, string jNetBillNo, UINT64 u64AgentBillID, DWORD dwPayType, double dPayAmt, string sign, UINT64 u64Remark)
{
	LogInfoToFile("LogRechargeInfo.txt", "HYPay �������� result:[%u] pay_message:[%s] agent_id:[%s] jnet_bill_no:[%s] agent_bill_id:[%llu] pay_type:[%u] pay_amt:[%0.2f] remark:[%llu] sign:[%s]",
		dwResult, payMessage.c_str(), agentID.c_str(), jNetBillNo.c_str(), u64AgentBillID, dwPayType, dPayAmt, u64Remark, sign.c_str());
	if (dwResult != 1) //����ʧ����
	{
		__DelOrderInfo(u64AgentBillID);
		return "error";
	}
	OperateConfig * pConfig = g_FishServerConfig.GetOperateConfig();
	if (!pConfig)
	{
		ASSERT(false);
		return "error";
	}
	std::string Key = pConfig->HYKey;//��Կ �������ļ���ȡ
	//��ȡ�����ݺ� ��δ�����
	//1.��֤ǩ��
	char TempSign[20480];
	sprintf_s(TempSign, CountArray(TempSign), "result=%u&agent_id=%s&jnet_bill_no=%s&agent_bill_id=%llu&pay_type=%u&pay_amt=%0.2f&remark=%llu&key=%s",
		dwResult, agentID.c_str(), jNetBillNo.c_str(), u64AgentBillID, dwPayType, dPayAmt, u64Remark, Key.c_str());
	string Md5Str = md5(TempSign);
	if (Md5Str.compare(sign) != 0)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPayMD5��֤��ʧ��", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//��֤�ɹ���
	//1.��֤�����Ƿ����
	tagHYPayRoleInfo* pInfo = __FindOrderInfo(u64AgentBillID);
	if (nullptr == pInfo)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay����������", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//2.��֤�۸��Ƿ���ȷ
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(pInfo->dwShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{

		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay������Ʒ������", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	if (Iter->second.dDisCountPrice * 100 * pInfo->dwShopNum != static_cast<UINT32>(dPayAmt)* 100)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay�����۸���ȷ", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//3.�������� �۸���ȷ ���ǽ��д��� 
	__SendRechargeSucess(u64AgentBillID, pInfo->dwUserID, pInfo->dwShopID, pInfo->dwShopNum);
	__DelOrderInfo(u64AgentBillID);
	return "ok";
}
void HYPay::__SendRechargeSucess(UINT64 OrderID, UINT32 dwUserID, DWORD dwShopID, DWORD dwShopNum)
{
	char OrderStr[64] = { 0 };
	sprintf_s(OrderStr, CountArray(OrderStr), "%llu", OrderID);
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(dwShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//��Ʒ������
	{
		g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay��ֵ��֤ʧ��:��Ʒ���ò�����", OrderStr, dwUserID, "", "", "", dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
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
	msg->rechargeInfo.Price = Iter->second.dDisCountPrice * 100 * dwShopNum;
	msg->rechargeInfo.UserID = dwUserID;
	msg->rechargeInfo.ShopItemID = dwShopID;
	msg->rechargeInfo.ShopNum = dwShopNum;
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

	g_DBLogManager.LogUserRechargeLogToDB("��Ӫ������HYPay��ֵ��֤�ɹ�", OrderStr, dwUserID, "", "", "", dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
}