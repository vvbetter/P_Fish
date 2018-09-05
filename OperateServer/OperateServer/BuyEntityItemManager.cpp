#include "stdafx.h"
#include "FishServer.h"
#include "BuyEntityItemManager.h"
BuyEntityManager::BuyEntityManager()
{
	m_OnlyID = 0;
	m_BuyEntityMap.clear();
}
BuyEntityManager::~BuyEntityManager()
{
	m_BuyEntityMap.clear();
}
bool BuyEntityManager::OnBuyEntityItem(ServerClientData* pClient, GO_Cmd_BuyEntityItem*pMsg)
{
	if (!pClient || !pMsg)
	{
		ASSERT(false);
		return false;
	}
	DWORD OnlyID = m_OnlyID;
	++m_OnlyID;
	BuyEntityOnce pOnce;
	pOnce.OnlyID = OnlyID;
	pOnce.UserID = pMsg->dwUserID;
	pOnce.ItemID = pMsg->ItemID;
	pOnce.ItemSum = pMsg->ItemSum;
	pOnce.ShopID = pMsg->ShopID;
	pOnce.ShopOnlyID = pMsg->ShopOnlyID;
	pOnce.GameServerClientID = pClient->OutsideExtraData;
	TCHARCopy(pOnce.Addres, CountArray(pOnce.Addres), pMsg->Addres, _tcslen(pMsg->Addres));
	TCHARCopy(pOnce.Name, CountArray(pOnce.Name), pMsg->Name, _tcslen(pMsg->Name));
	pOnce.Phone = pMsg->Phone;
	m_BuyEntityMap.insert(HashMap<DWORD, BuyEntityOnce>::value_type(OnlyID,pOnce));//��ӽ��뼯��
	//��������ⲿ������ȥ
	//SendLink
	//�����ⲿ����  �������ܽ�����ӵ� �� ֱ�ӷ��͵��̵�ȥ �����

	//xuda
	OnBuyEntityItemResult(ROE_BuyEntityItem_Sucess,true, pMsg->dwUserID, OnlyID);//��ʾ��ҹ�����Ʒ�ɹ��� ʵ����Ʒ����ɹ���
	return true;
}
void BuyEntityManager::OnBuyEntityItemResult(BYTE ErrorID, bool Result, DWORD dwUserID, DWORD OnlyID)
{
	HashMap<DWORD, BuyEntityOnce>::iterator Iter = m_BuyEntityMap.find(OnlyID);//����Ѿ��ύ������
	if (Iter == m_BuyEntityMap.end())
	{
		ASSERT(false);
		return;//�����ڵĶ��� ���봦��
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(ConvertDWORDToBYTE(Iter->second.GameServerClientID));
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//��ȡ�������� ���ǿ�ʼ����
	OG_Cmd_BuyEntityItem msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OG_BuyEntityItem), sizeof(OG_Cmd_BuyEntityItem));
	msg.dwUserID = dwUserID;
	msg.ErrorID = ErrorID;
	msg.ShopID = Iter->second.ShopID;
	msg.ShopOnlyID = Iter->second.ShopOnlyID;
	msg.ItemSum = Iter->second.ItemSum;
	msg.Result = Result;
	g_FishServer.SendNetCmdToClient(pClient, &msg);
	m_BuyEntityMap.erase(Iter);//�Ƴ�����
}