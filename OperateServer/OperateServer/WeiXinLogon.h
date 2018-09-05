//΢�ŵ�½����
//΢�Ż�����½����
//1.ͨ��code��ȡaccess_token  code �ͻ��˷��͹���
//2.ͨ�� access_token ��ȡ��һ�������
#pragma once
#include <string>

struct tagWeiXinUser
{
	UINT64		OnlyID;
	DWORD		ClientID;
	BYTE		LogonClientID;
	DWORD		TimeLog;//����Token��ʱ�� 1�����޷�Ӧ ʧ��
	BYTE		PlateFormID;//ƽ̨��ID
	TCHAR		MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
};

class WeiXinLogon
{
public:
	WeiXinLogon();
	virtual ~WeiXinLogon();

	void    HandleCode(ServerClientData* pClient, LO_Cmd_WeiXinLogon* pMsg);//����ͻ��˷�������code ����
	void	OnHandleWeiXinTokenResult(unsigned __int64 OnlyID, char* pData, DWORD Length);

	void	Update();
private:
	HashMap<UINT64, tagWeiXinUser>			m_UserMap;
	DWORD									m_dwOpenIPCrc32;
};