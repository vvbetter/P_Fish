#include "Stdafx.h"
#include "CashManager.h"
#include "FishServer.h"
CashManager::CashManager()
{

}
CashManager::~CashManager()
{

}
void CashManager::Init()
{
	//向数据库发送命令读取全部的 未发送邮件 但是可以发送邮件的提取订单数据
	__SendLoadAllCashOrderInfoToDB();
}
void CashManager::Update()
{
	DWORD u32NowTime = timeGetTime();
	if (u32NowTime - m_LogTime >= 60000)
	{
		__SendLoadAllCashOrderInfoToDB();
	}
}
void CashManager::__SendLoadAllCashOrderInfoToDB()
{
	m_LogTime = timeGetTime();
	//发送数据库命令
	DBR_Cmd_LoadCash msg;
	SetMsgInfo(msg, DBR_LoadCash, sizeof(DBR_Cmd_LoadCash));
	g_FishServer.SendNetCmdToDB(&msg);
}
void CashManager::HandleCashOrderInfo(DBO_Cmd_LoadCash* pMsg)
{
	//处理全部发送来的订单数据
	//订单数据可能是分包发送的
	if (nullptr == pMsg)
	{
		ASSERT(false);
		return;
	}
	for (WORD i = 0; i < pMsg->Sum; ++i)
	{
		tagCashOrderInfo* pInfo = &pMsg->Array[i];
		__HandleCashOrderInfo(pInfo);
	}
}
void CashManager::__HandleCashOrderInfo(tagCashOrderInfo* pInfo)
{
	if (nullptr == pInfo)
	{
		ASSERT(false);
		return;
	}
	//根据订单数据发送邮件
	tagRoleMail	MailInfo;
	MailInfo.bIsRead = false;
	//比赛的内容需要特殊的处理 我们想要一个 特殊的转义字符串 客户端 和 服务器通用的 
	if (pInfo->bFinish)
	{
		_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("您提交的订单:%u 已经成功处理了"), pInfo->dwOrderID);
		MailInfo.RewardID = 0;
		MailInfo.RewardSum = 0;
	}
	else
	{
		_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("您提交的订单:%u 处理失败了 退还金币:%u"), pInfo->dwOrderID, pInfo->dwOrderGold);
		MailInfo.RewardID = g_FishServer.GetFishConfig().GetSystemConfig().EmailGlobelRewardID;
		MailInfo.RewardSum = pInfo->dwOrderGold;
	}
	MailInfo.MailID = 0;
	MailInfo.SendTimeLog = time(NULL);
	MailInfo.SrcFaceID = 0;
	TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
	MailInfo.SrcUserID = 0;//系统发送
	MailInfo.bIsExistsReward = (MailInfo.RewardID != 0 && MailInfo.RewardSum != 0);
	DBR_Cmd_AddUserMail msg;
	SetMsgInfo(msg, DBR_AddUserMail, sizeof(DBR_Cmd_AddUserMail));
	msg.dwDestUserID = pInfo->dwUserID;
	msg.MailInfo = MailInfo;
	g_FishServer.SendNetCmdToDB(&msg);
	//
	DBR_Cmd_HandleCash msgDB;
	SetMsgInfo(msgDB, DBR_HandleCash, sizeof(DBR_Cmd_HandleCash));
	msgDB.OrderID = pInfo->dwOrderID;
	g_FishServer.SendNetCmdToDB(&msgDB);
}