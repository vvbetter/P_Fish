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
	//�����ݿⷢ�������ȡȫ���� δ�����ʼ� ���ǿ��Է����ʼ�����ȡ��������
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
	//�������ݿ�����
	DBR_Cmd_LoadCash msg;
	SetMsgInfo(msg, DBR_LoadCash, sizeof(DBR_Cmd_LoadCash));
	g_FishServer.SendNetCmdToDB(&msg);
}
void CashManager::HandleCashOrderInfo(DBO_Cmd_LoadCash* pMsg)
{
	//����ȫ���������Ķ�������
	//�������ݿ����Ƿְ����͵�
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
	//���ݶ������ݷ����ʼ�
	tagRoleMail	MailInfo;
	MailInfo.bIsRead = false;
	//������������Ҫ����Ĵ��� ������Ҫһ�� �����ת���ַ��� �ͻ��� �� ������ͨ�õ� 
	if (pInfo->bFinish)
	{
		_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("���ύ�Ķ���:%u �Ѿ��ɹ�������"), pInfo->dwOrderID);
		MailInfo.RewardID = 0;
		MailInfo.RewardSum = 0;
	}
	else
	{
		_sntprintf_s(MailInfo.Context, CountArray(MailInfo.Context), TEXT("���ύ�Ķ���:%u ����ʧ���� �˻����:%u"), pInfo->dwOrderID, pInfo->dwOrderGold);
		MailInfo.RewardID = g_FishServer.GetFishConfig().GetSystemConfig().EmailGlobelRewardID;
		MailInfo.RewardSum = pInfo->dwOrderGold;
	}
	MailInfo.MailID = 0;
	MailInfo.SendTimeLog = time(NULL);
	MailInfo.SrcFaceID = 0;
	TCHARCopy(MailInfo.SrcNickName, CountArray(MailInfo.SrcNickName), TEXT(""), 0);
	MailInfo.SrcUserID = 0;//ϵͳ����
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