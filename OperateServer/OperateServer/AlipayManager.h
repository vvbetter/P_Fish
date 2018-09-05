#pragma once
#include "PhoneSMSManager.h"
struct tagRoleAlipay
{
	DWORD			dwUserID;
	UINT64			u64Phone;
	DWORD			dwPhoneNum;
	DWORD			HandleOnlyID;
	DWORD			dwGameConfigID;
};

class AlipayManager
{
public:
	AlipayManager(){}
	~AlipayManager(){}

	void		AddRoleInfo(DWORD dwUserID, UINT64 u64Phone, DWORD dwGameConfigID);
	void		ChangeAlipayInfo(DWORD dwUserID, DWORD PhoneNum, TCHAR AlipayAccount[MAX_AlipayAccount_LENGTH + 1], TCHAR AlipayName[MAX_AlipayName_LENGTH + 1]);
	void		OnHandleSMSEvent(PhoneSMSOnceInfo pEvent, bool Result);
private:
	void		__SendAddRoleInfoResult(ServerClientData* pClient,DWORD dwUserID, bool Result);
	void		__SendChangeAlipayInfoResult(ServerClientData* pClient, DWORD dwUserID, bool Result, TCHAR AlipayAccount[MAX_AlipayAccount_LENGTH + 1], TCHAR AlipayName[MAX_AlipayName_LENGTH + 1]);
private:
	HashMap<DWORD, tagRoleAlipay>  m_InfoMap;
};