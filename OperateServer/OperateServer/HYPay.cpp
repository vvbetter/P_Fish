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
	//1.组织数据 通过Http 进行发送出去
	//2.记录Log
	//3.记录数据到Map里面去
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(ShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{
		ASSERT(false);
		__SendRequestTokenFailed(dwUserID, bClientID);
		return;
	}	
	UINT64 OrderID = timeGetTime(); //唯一的订单ID
	OrderID = (OrderID << 32) | dwUserID;
	if (g_FishServer.GetOrderOnlyManager().IsExists(OrderID, OT_HY))
	{
		ASSERT(false);
		__SendRequestTokenFailed(dwUserID, bClientID);
		return;
	}
	g_FishServer.GetOrderOnlyManager().OnAddOrderInfo(dwUserID, OrderID, OT_HY);//申请记录一个唯一ID
	//将玩家数据组织起来
	tagHYPayRoleInfo pInfo;
	pInfo.u64OrderID = OrderID;
	pInfo.dwShopID = ShopID;
	pInfo.dwShopNum = ShopNum;
	pInfo.Token = "";
	pInfo.dwUserID = dwUserID;
	pInfo.bClientID = bClientID;
	pInfo.AgentType = dwAgentType;
	m_OrderMap.insert(HashMap<UINT64, tagHYPayRoleInfo>::value_type(OrderID,pInfo));
	__InitPayInfo(OrderID, dwUserID, dwUserIP, ShopID, ShopNum,dwAgentType);//发送命令去Http去
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
	//组织初始化字符串 发送到指定的Http端口去
	//使用https 的 处理 我们需要发送Https 
	DWORD version = 2;
	DWORD pay_type = dwAgentType;//30;
	DWORD agent_id = pConfig->HYAgentID;//商户的ID
	UINT64 agent_bill_id = OrderID;
	double pay_amt = Iter->second.dDisCountPrice * ShopNum;
	std::string return_url = "www.baidu.com";
	char notifuUrl[512] = { 0 };
	sprintf_s(notifuUrl, CountArray(notifuUrl), "http://%s:1680/%s", pConfig->OperateListenIP, "pay_callback_hy.clkj");
	std::string notify_url = notifuUrl;//从配置文件读取
	//将IP的4个值取出
	DWORD dwIp1 = dwUserIP & 0xff;
	DWORD dwIp2 = (dwUserIP >> 8) & 0xff;
	DWORD dwIp3 = (dwUserIP >> 16) & 0xff;
	DWORD dwIp4 = (dwUserIP >> 24);
	char ipStr[64] = { 0 };
	sprintf_s(ipStr, CountArray(ipStr), "%u.%u.%u.%u", dwIp1, dwIp2, dwIp3, dwIp4);
	std::string user_ip = ipStr;//将客户端的IP 转化为字符串
	DWORD user_identity = dwUserID;
	char timeStr[64] = { 0 };
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	sprintf_s(timeStr, CountArray(timeStr), "%04d%02d%02d%02d%02d%02d", pTime.wYear, pTime.wMonth, pTime.wDay, pTime.wHour, pTime.wMinute, pTime.wSecond);
	UINT64 agent_bill_time = strtoull(timeStr, null, 10);//当前时间 yyyymmddhhmmss
	char ItemName[12] = { 0 };
	_itoa_s(Iter->second.ID, ItemName, 12, 10);
	std::string goodsName = ItemName;
	DWORD goodsNum = ShopNum;
	UINT64 remark = OrderID;//自定义参数
	//std::string goods_note = "";//描述
	std::string Key = pConfig->HYKey;//密钥 从配置文件读取
	std::string goodsNote = "hypay";
	//签名验证
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
	if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, Url, true))//直接发送Get请求
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
	//将字符串解析成为XML 然后进行读取
	tinyxml2::XMLDocument pDoc;
	pDoc.Parse(pDestData, count);
	tinyxml2::XMLElement* pTokenNode = pDoc.FirstChildElement("token_id");
	tinyxml2::XMLElement* pErrorNode = pDoc.FirstChildElement("error");
	free(pDestData);
	if (nullptr != pTokenNode)
	{
		std::string strToken = pTokenNode->GetText();//获取令牌数据
		//将令牌数据设置起来
		pInfo->Token = strToken;
		//发送令牌数据到客户端去
		__SendRequestTokenSucess(pInfo->dwUserID, pConfig->HYAgentID, pInfo->u64OrderID, pInfo->AgentType,strToken, pInfo->bClientID);
		//发送Log记录到数据库去 一个玩家申请了进行充值记录 只是申请了 不是完成支付
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", Order64ID);
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay申请令牌成功", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
	}
	else if (nullptr != pErrorNode)
	{
		__SendRequestTokenFailed(pInfo->dwUserID, pInfo->bClientID);//提示玩家领取令牌失败了
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
		LogInfoToFile("LogRechargeInfo.txt", "HYPay充值异步回调 部分数据不存在");
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
	LogInfoToFile("LogRechargeInfo.txt", "HYPay 返回数据 result:[%u] pay_message:[%s] agent_id:[%s] jnet_bill_no:[%s] agent_bill_id:[%llu] pay_type:[%u] pay_amt:[%0.2f] remark:[%llu] sign:[%s]",
		dwResult, payMessage.c_str(), agentID.c_str(), jNetBillNo.c_str(), u64AgentBillID, dwPayType, dPayAmt, u64Remark, sign.c_str());
	if (dwResult != 1) //订单失败了
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
	std::string Key = pConfig->HYKey;//密钥 从配置文件读取
	//获取到数据后 如何处理呢
	//1.验证签名
	char TempSign[20480];
	sprintf_s(TempSign, CountArray(TempSign), "result=%u&agent_id=%s&jnet_bill_no=%s&agent_bill_id=%llu&pay_type=%u&pay_amt=%0.2f&remark=%llu&key=%s",
		dwResult, agentID.c_str(), jNetBillNo.c_str(), u64AgentBillID, dwPayType, dPayAmt, u64Remark, Key.c_str());
	string Md5Str = md5(TempSign);
	if (Md5Str.compare(sign) != 0)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPayMD5验证码失败", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//验证成功了
	//1.验证订单是否存在
	tagHYPayRoleInfo* pInfo = __FindOrderInfo(u64AgentBillID);
	if (nullptr == pInfo)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay订单不存在", OrderStr, 0, "", "", "", 0, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//2.验证价格是否正确
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(pInfo->dwShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())
	{

		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay订单物品不存在", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	if (Iter->second.dDisCountPrice * 100 * pInfo->dwShopNum != static_cast<UINT32>(dPayAmt)* 100)
	{
		char OrderStr[64] = { 0 };
		sprintf_s(OrderStr, CountArray(OrderStr), "%llu", u64AgentBillID);
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay订单价格不正确", OrderStr, pInfo->dwUserID, "", "", "", pInfo->dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);

		ASSERT(false);
		return "error";
	}
	//3.订单存在 价格正确 我们进行处理 
	__SendRechargeSucess(u64AgentBillID, pInfo->dwUserID, pInfo->dwShopID, pInfo->dwShopNum);
	__DelOrderInfo(u64AgentBillID);
	return "ok";
}
void HYPay::__SendRechargeSucess(UINT64 OrderID, UINT32 dwUserID, DWORD dwShopID, DWORD dwShopNum)
{
	char OrderStr[64] = { 0 };
	sprintf_s(OrderStr, CountArray(OrderStr), "%llu", OrderID);
	HashMap<DWORD, tagFishRechargeInfo>::iterator Iter = g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.find(dwShopID);
	if (Iter == g_FishServer.GetFishConfig().GetFishRechargesConfig().m_FishRechargeMap.end())//物品不存在
	{
		g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay充值验证失败:物品配置不存在", OrderStr, dwUserID, "", "", "", dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
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
	free(msg);//将玩家充值的数据发送到 GameServer去

	g_DBLogManager.LogUserRechargeLogToDB("运营服务器HYPay充值验证成功", OrderStr, dwUserID, "", "", "", dwShopID, 0, 0, 0, 0, 0, 0, 0, SendLogDB);
}