//��Ӫ�������� ʵ����֤�Ĺ��ܹ���
//ʵ����֤���� GameServerConfigID
#pragma once
struct RealNameVerificationOnce
{
	DWORD			UserID;
	DWORD			GameServerClientID;
	TCHAR			Name[MAX_NAME_LENTH + 1];//ʵ������
	TCHAR			IDCard[MAX_IDENTITY_LENGTH + 1];//���֤����

	//��������֤
	DWORD			beginTime;//�������� 1�������޴��� ��ʾ����������
};
class RealNameVerificationManager
{
public:
	RealNameVerificationManager();
	virtual ~RealNameVerificationManager();

	bool OnAddRealNameVerificationOnce(ServerClientData* pClient, GO_Cmd_RealNameVerification* pMsg);//���һ������
	//void OnCheckEntityResult(DBO_Cmd_CheckEntityID* pMsg);
	void OnAddRealNameVerificationOnceResult(BYTE ErrorID,bool Result, DWORD dwUserID);//���ⲿ���������صĽ��
	void OnUpdate(DWORD dwTimer);//���и��´���

	bool CheckEntityID(LPCTSTR EntityID);
private:
	HashMap<DWORD, RealNameVerificationOnce>			m_RealNameVerificationMap;//ȫ���ȴ�ʵ����֤�Ķ���
};