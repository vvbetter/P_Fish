#pragma once
struct BuyEntityOnce
{
	DWORD			OnlyID;
	DWORD			UserID;
	//��ַ �ֻ� ���� 
	TCHAR			Name[MAX_NAME_LENTH + 1];//����
	unsigned __int64 Phone;//�ֻ�����
	TCHAR			Addres[MAX_ADDRESS_LENGTH + 1];//��ʵ��ַ
	//��Ʒ����
	BYTE			ShopID;
	BYTE			ShopOnlyID;
	DWORD			ItemID;
	DWORD			ItemSum;

	DWORD			GameServerClientID;
};
class BuyEntityManager
{
public:
	BuyEntityManager();
	virtual ~BuyEntityManager();

	bool OnBuyEntityItem(ServerClientData* pClient, GO_Cmd_BuyEntityItem*pMsg);
	void OnBuyEntityItemResult(BYTE ErrorID,bool Result, DWORD dwUserID,DWORD OnlyID);
private:
	HashMap<DWORD,BuyEntityOnce>	m_BuyEntityMap;
	DWORD							m_OnlyID;
};