#pragma once
#include "PhoneSMSManager.h"

struct PhoneLogonOnceInfo
{
	UINT64  PhoneNumber;//�ֻ���
	DWORD   LogonPassword;//��½������ ���������
	DWORD	HandleOnlyID;
	BYTE    GameConfigID;
	DWORD   ClientID;

	PhoneLogonOnceInfo()
	{
		PhoneNumber = 0;
		LogonPassword = 0;
		GameConfigID = 0;
		ClientID = 0;
		HandleOnlyID = 0;
	}
};

class PhoneLogon //ʹ���ֻ�������е�½����
{
public:
	PhoneLogon();
	virtual ~PhoneLogon();

	bool OnAddPhoneLogon(ServerClientData* pData,UINT64 PhoneNumber, BYTE GameConfigID, DWORD ClientID);//����ָ����ҵ�½��״̬
	void OnCheckPhoneResult(DBO_Cmd_CheckPhone* pMsg);

	void OnUpdate(DWORD dwTimer);

	void OnHandleSMSEvent(PhoneSMSOnceInfo pEvent, bool Result);

	void OnCheckPhoneLogonPassword(LO_Cmd_PhoneLogonCheck* pMsg);
private:
	void OnHandlePhoneLogon(PhoneLogonOnceInfo& pInfo);
private:
	HashMap<UINT64, PhoneLogonOnceInfo>  m_LogonMap;//������ĵ�½����
};