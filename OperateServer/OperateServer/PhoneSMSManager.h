//�����ֻ����ŵĹ�����
#pragma once
#define MAX_SMS_Length 128
#define SMS_TimeOut 60000
enum SMSType
{
	ST_Init = 0,
	ST_PhoneBind  = 1,
	ST_PhoneLogon = 2,
	ST_Alipay	  = 3,
};
struct  PhoneSMSOnceInfo //һ�η����¼�
{
	DWORD				OnlyID;
	SMSType				Type;
	vector<UINT64>		PhoneNumberVec;//�ֻ��б�
	DWORD				LogTime;
	char				SMSInfo[MAX_SMS_Length+1];//������Ϣ

	PhoneSMSOnceInfo()
	{
		OnlyID = 0;
		Type = ST_Init;
		PhoneNumberVec.clear();
		LogTime = 0;
		ZeroMemory(&SMSInfo, sizeof(char)*(MAX_SMS_Length + 1));
	}
};
class PhoneSMSManager
{
public:
	PhoneSMSManager();
	virtual ~PhoneSMSManager();

	DWORD OnAddSMSEvent(PhoneSMSOnceInfo pEvent);
	void OnUpdate(DWORD dwTimer);

	void OnSendPhoneSMSResult(UINT64 ID, char* pData, DWORD Length);//�첽�Ľ�� ���ⲿ���е���
private:
	bool HandleSMSEvent(PhoneSMSOnceInfo pEvent);
	void HandleSMSResult(PhoneSMSOnceInfo pEvent, bool Result);

private:
	DWORD									m_HandleIndex;

	HashMap<DWORD, PhoneSMSOnceInfo>		m_SMSHandleMap;//������Ķ����¼�
};