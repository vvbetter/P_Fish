#pragma once
class LogonCache
{
public:
	LogonCache();
	virtual ~LogonCache();

	void OnLoadAllAccountData();//�����ݿ��ȡȫ��������
	void OnLoadAllAccountDataResult(DBO_Cmd_LoadAllAccountInfo* msg);//�����ݿ���صĽ��
	//void OnLoadAllAccountDataResultFinish();
	bool CheckAccountData(const TCHAR* AccountName, DWORD PasswordCrc1, DWORD PasswordCrc2, DWORD PasswordCrc3, DWORD& UserID, bool& IsFreeze,__int64& FreezeEndTime);
	bool IsExistsAccount(const TCHAR* AccountName);
	void OnAccountRsg(AccountCacheInfo& pData);//��Logon���յ�һ�����ע���ʱ�� 

	//��������
	void OnAddTempAccountInfo(TempAccountCacheInfo& pData);//��δ��֤�����ݷ��뻺��֮��
	void OnCheckTempAccountInfo(DWORD ClientID, DWORD dwUserID, bool  Result);//ɾ������ת��Ϊ��ʽ�Ļ�������
	void OnCheckTempAccountInfo(DWORD ClientID,const TCHAR* AccountName, DWORD dwUserID, bool Result);

	void OnChangeAccountPassword(DWORD dwUserID, DWORD Pass1, DWORD Pass2, DWORD Pass3);
	void OnChangeAccountFreezeInfo(DWORD dwUserID, time_t FreezeEndTime);
	void OnResetAccount(DWORD dwUserID, const TCHAR* AccountName, DWORD Pass1, DWORD Pass2, DWORD Pass3);
	void OnAddAccountInfo(DWORD dwUserID, const TCHAR* AccountName, DWORD Pass1, DWORD Pass2, DWORD Pass3);
private :
	DWORD GetAccountNameCrc(const TCHAR* pAccountName);
	void OnDestroy();
private:
	HashMap<DWORD, AccountCacheInfo*>	m_AccountMap;
	HashMap<DWORD, AccountCacheInfo*>   m_UserMap;

	HashMap<DWORD, TempAccountCacheInfo>	m_TempAccountMap;//��������� //�ͻ��˶˵�CLientID
};