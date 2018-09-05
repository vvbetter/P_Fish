//�⾵��������½����
#pragma once
struct LogonTempInfo
{
	BYTE				 ClientID;
	DWORD				 ChannelID;
	TCHAR				 MacAddress[MAC_ADDRESS_LENGTH + 1];
	LO_Cmd_ChannelLogon* pMsg;
};
class ChannelLogonManager
{
public:
	ChannelLogonManager();
	virtual ~ChannelLogonManager();

	//���е�½��֤
	void  OnHandleLogonInfo(ServerClientData* pClient,LO_Cmd_ChannelLogon* pMsg);
	void OnHandleLogonResult(unsigned __int64 OnlyID, char* pData, DWORD Length);
private:
	void OnDestroy();
private:
	HashMap<unsigned __int64, LogonTempInfo> m_RoleLogonMap;
};