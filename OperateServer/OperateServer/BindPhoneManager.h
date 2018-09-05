#pragma once
#include "PhoneSMSManager.h"
struct BindPhoneOnce
{
	DWORD				UserID;
	DWORD				GameServerClientID;
	unsigned __int64	PhoneNumber;
	DWORD				SendPhoneNumberOnlyID;//�Ƿ��Ѿ��ɹ�������֤����
	DWORD				VerificationNumber;//��֤��
	BindPhoneOnce()
	{
		UserID = 0;
		GameServerClientID = 0;
		PhoneNumber = 0;
		SendPhoneNumberOnlyID = 0;
		VerificationNumber = 0;
	}
};
struct SendPhoneList
{
	vector<DWORD>   UserVec; //��ǰ���ڽ��е����
};
class BindPhoneManager
{
public:
	BindPhoneManager();
	virtual ~BindPhoneManager();

	bool  OnGetPhoneVerificationNumber(ServerClientData* pClient, GO_Cmd_GetPhoneVerificationNum* pMsg);//��ȡ�ֻ�����֤��
	void  OnCheckPhoneResult(DBO_Cmd_CheckPhone* pMsg);
	void  OnGetPhoneVerificationNumberResult(bool Result,BYTE ErrorID,DWORD dwUserID);//��ȡ�ֻ���֤��Ľ��

	bool OnBindPhontByVerificationNumber(ServerClientData* pClient, GO_Cmd_BindPhone* pMsg);//���ֻ�������֤ 

	bool CheckPhoneNumber(unsigned __int64  PhoneNumber);

	void OnUpdateSendPhoneSMS(DWORD dwTimer);

	void OnHandleSMSEvent(PhoneSMSOnceInfo pEvent, bool Result);
private:
	HashMap<DWORD, BindPhoneOnce>		m_BindPhoneMap;//�ֻ��󶨵Ķ���
	SendPhoneList						m_SendPhoneList;//��������֤����ֻ�����
};