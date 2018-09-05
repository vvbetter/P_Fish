#include "stdafx.h"
#include "BindEmailManager.h"
#include "FishServer.h"
BindEmailManager::BindEmailManager()
{

}
BindEmailManager::~BindEmailManager()
{

}
bool BindEmailManager::OnBindEmail(ServerClientData* pClient, GO_Cmd_BindEmail* pMsg)
{
	//�����������.
	if (!pClient || !pMsg)
	{
		ASSERT(false);
		return false;
	}
	//����һ��ȷ���ʼ����Է�����ȥ �ȴ�����

	//xuda 
	OnBindEmailResult(ROE_BindMail_Sucess,true, pMsg->dwUserID, pMsg->EMail);
	return true;
}
void BindEmailManager::OnBindEmailResult(BYTE ErrorID, bool Result, DWORD dwUserID, const TCHAR* pEmail)
{
	//UserID Email  ����п��ܲ��� �������� ������Ҫ���д��� ������ת�������������ȥ ������ 
	OC_Cmd_BindEmail msg;
	SetMsgInfo(msg, GetMsgType(Main_Operate, OC_BindEmail), sizeof(OC_Cmd_BindEmail));
	msg.dwUserID = dwUserID;
	msg.ErrorID = ErrorID;
	msg.Result = Result;
	TCHARCopy(msg.EMail, CountArray(msg.EMail), pEmail, _tcslen(pEmail));
	g_FishServer.SendNetCmdToCenterServer(&msg);
}