#pragma once
#include "HttpServer.h"
//汇付宝的支付管理
#define HYPay_Result					"result"
#define HYPay_PayMessage				"pay_message"
#define HYPay_AgentID					"agent_id"
#define HYPay_JNetBillNo				"jnet_bill_no"
#define HYPay_AgentBillID				"agent_bill_id"
#define HYPay_PayType					"pay_type"
#define HYPay_PayAmt					"pay_amt"
#define HYPay_Remark					"remark"
#define HYPay_Sign						"sign"


struct HYPayCrcInfo
{
	DWORD		resultCrc;
	DWORD		payMessageCrc;
	DWORD		agentIDCrc;
	DWORD		jNetBillNoCrc;
	DWORD		agentBillIDCrc;
	DWORD		payTypeCrc;
	DWORD		payAmtCrc;
	DWORD		remarkCrc;
	DWORD       signCrc;

	HYPayCrcInfo()
	{
		resultCrc = AECrc32(HYPay_Result, strlen(HYPay_Result), 0);
		payMessageCrc = AECrc32(HYPay_PayMessage, strlen(HYPay_PayMessage), 0);
		agentIDCrc = AECrc32(HYPay_AgentID, strlen(HYPay_AgentID), 0);
		jNetBillNoCrc = AECrc32(HYPay_JNetBillNo, strlen(HYPay_JNetBillNo), 0);
		agentBillIDCrc = AECrc32(HYPay_AgentBillID, strlen(HYPay_AgentBillID), 0);
		payTypeCrc = AECrc32(HYPay_PayType, strlen(HYPay_PayType), 0);
		payAmtCrc = AECrc32(HYPay_PayAmt, strlen(HYPay_PayAmt), 0);
		remarkCrc = AECrc32(HYPay_Remark, strlen(HYPay_Remark), 0);
		signCrc = AECrc32(HYPay_Sign, strlen(HYPay_Sign), 0);
	}
};


struct tagHYPayRoleInfo
{
	UINT64		u64OrderID;
	DWORD		dwUserID;
	DWORD		dwShopID;
	DWORD		dwShopNum;
	BYTE		bClientID;
	UINT32		AgentType;
	std::string Token;
};
class HYPay : public HttpCallback
{
public:
	HYPay();
	~HYPay();

	//1.客户端发送命令请求支付令牌
	void	HandleRequestToken(DWORD dwUserID, DWORD dwUserIP, DWORD ShopID, DWORD ShopNum, BYTE bClientID, DWORD dwAgentType);
	void	HandleRequestTokenResult(unsigned __int64 Order64ID, char* pData, DWORD Length);
	virtual string RequestCallback(HttpRecvData *pRecvData);

	//void	__Test();
private:
	void	__SendRequestTokenFailed(DWORD dwUserID, BYTE bClientID);
	void	__SendRequestTokenSucess(DWORD dwUserID,UINT32 u32AgentID,UINT64 u64OrderID,UINT32 AgentType, std::string& Token, BYTE bClientID);
	void	__InitPayInfo(UINT64 OrderID, DWORD dwUserID, DWORD dwUserIP, DWORD ShopID, DWORD ShopNum, DWORD dwAgentType);
	tagHYPayRoleInfo* __FindOrderInfo(UINT64 orderID);
	void	__DelOrderInfo(UINT64 orderID);
	void	__SendRechargeSucess(UINT64 OrderID,UINT32 dwUserID, DWORD dwShopID,DWORD dwShopNum);
	string  __HandlePayResult(DWORD dwResult,string payMessage,string agentID,string jNetBillNo,UINT64 u64AgentBillID,DWORD dwPayType,double dPayAmt,string sign,UINT64 u64Remark);
	//string	__HandlePayResult(string Url);

	
private:
	HYPayCrcInfo						m_CrcMap;
	HashMap<UINT64, tagHYPayRoleInfo>	m_OrderMap;
};