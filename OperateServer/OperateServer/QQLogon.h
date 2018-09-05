//QQ��½
//�ϴ��û���token���� ��ȡ OpenID �û���Ψһ���

#pragma once
#include <string>

struct tagQQUser
{
	UINT64		OnlyID;
	DWORD		ClientID;
	BYTE		LogonClientID;
	DWORD		TimeLog;//����Token��ʱ�� 1�����޷�Ӧ ʧ��
	BYTE		PlateFormID;//ƽ̨��ID
	TCHAR		MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
};

class QQLogon
{
public:
	QQLogon();
	virtual ~QQLogon();

	void    HandleCode(ServerClientData* pClient, LO_Cmd_QQLogon* pMsg);//����ͻ��˷�������code ����
	void	OnHandleQQTokenResult(unsigned __int64 OnlyID, char* pData, DWORD Length);

	void	Update();
private:
	HashMap<UINT64, tagQQUser>			m_UserMap;
};