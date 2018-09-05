#include "stdafx.h"
#include "QQUserInfoManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include "json/json.h"
QQUserInfoManager::QQUserInfoManager()
{

}
QQUserInfoManager::~QQUserInfoManager()
{

}
void QQUserInfoManager::HandleGetQQUserInfo(const ServerClientData* pClient, DWORD dwUserID, const char* pchToken, const char* pchOpenID)
{
	if (nullptr == pClient || nullptr == pchToken || 0 == strlen(pchToken) || nullptr == pchOpenID || 0 == strlen(pchOpenID))
	{
		ASSERT(false);
		return;
	}
	UINT64 nOnlyID = dwUserID;
	nOnlyID = (nOnlyID << 32) + pClient->OutsideExtraData;
	HashMap<DWORD, tagQQUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter != m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}

	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "{6}?access_token=%s&oauth_consumer_key=%s&openid=%s", pchToken, g_FishServerConfig.GetOperateConfig()->QQAppID,pchOpenID);//�ַ�����ȡ���
	//���ͳ�ȥ

	WORD RequestID = g_FishServer.GetQQUserInfoID();
	if (g_FishServerConfig.GetIsOperateTest())
	{
		//���Ϊ����
		tagQQUserInfoOnce Once;
		Once.nOnlyID = nOnlyID;
		Once.dwUserID = dwUserID;
		Once.dwClientID = pClient->OutsideExtraData;
		Once.dwTimeLog = timeGetTime();
		m_HashMapUserInfo.insert(HashMap<DWORD, tagQQUserInfoOnce>::value_type(Once.dwUserID, Once));

		//{"ret":-1,"msg":"client request's parameters are invalid, invalid openid"} ����Ľ��
		string strTestUrlResult = "{\"ret\":0,\"msg\" : \"\",\"nickname\" : \"YOUR_NICK_NAME\",}";

		HandleQQUserInfoResult(nOnlyID, (char*)strTestUrlResult.c_str(), strTestUrlResult.length());
		return;
	}
	else
	{
		UINT64* pID = new UINT64(nOnlyID);
		if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, SignStr, true))//ֱ�ӷ���Get����
		{
			delete(pID);
			return;
		}
	}
	tagQQUserInfoOnce Once;
	Once.nOnlyID = nOnlyID;
	Once.dwUserID = dwUserID;
	Once.dwClientID = pClient->OutsideExtraData;
	Once.dwTimeLog = timeGetTime();
	Once.bGender = false;
	Once.dwFaceID = 0;
	m_HashMapUserInfo.insert(HashMap<DWORD, tagQQUserInfoOnce>::value_type(Once.dwUserID, Once));
}
void QQUserInfoManager::HandleQQUserInfoResult(UINT64 nOnlyID, char* pchData, DWORD dwLength)
{
	/*
	{"ret":-1,"msg":"client request's parameters are invalid, invalid openid"}
	*/
	if (0 == dwLength || nullptr == pchData)
	{
		ASSERT(false);
		return;
	}
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagQQUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	//�����յ������ݽ���ת�� ��ȡ�ض�������
	std::string strJosnInfo = pchData;

	std::istringstream is(strJosnInfo);
	Json::CharReaderBuilder jsonReader;
	Json::Value jsonRoot;
	string err;
	if (!Json::parseFromStream(jsonReader,is,&jsonRoot,&err))
	{
		m_HashMapUserInfo.erase(Iter);
		ASSERT(false);
		return;
	}
	if (!jsonRoot["ret"].asInt() != 0)
	{
		m_HashMapUserInfo.erase(Iter);
		ASSERT(false);
		return;
	}
	if (jsonRoot["nickname"].isNull() ||
		jsonRoot["gender"].isNull() ||
		jsonRoot["figureurl"].isNull()
		)
	{
		m_HashMapUserInfo.erase(Iter);
		ASSERT(false);
		return;
	}
	std::string strNickName = jsonRoot["nickname"].asString();
	std::string strGender = jsonRoot["gender"].asString();
	bool bGender = false;
	if (strcoll(strGender.c_str(), "��"))
		bGender = true;
	else
		bGender = false;
	std::string strFaceURL = jsonRoot["figureurl"].asString();

	Iter->second.bGender = bGender;
	UINT Size = 0;
	TCHAR* pNickName = CharToWChar(strNickName.c_str(), strNickName.length(), Size);
	TCHARCopy(Iter->second.chNickName, CountArray(Iter->second.chNickName), pNickName, Size);
	free(pNickName);
	//���ݻ�ȡ�ɹ��� ���Ǽ�����������
	HandleGetQQUserImg(nOnlyID, strFaceURL.c_str());
}
void QQUserInfoManager::HandleGetQQUserImg(UINT64 nOnlyID, const char* pchURL)
{
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagQQUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	//����URL��ַ ��ȡ��ҵ�ͷ��
	if (nullptr == pchURL || strlen(pchURL) == 0)
	{
		HandleQQUserImgResult(nOnlyID, nullptr, 0);//ֱ���������ͷ��δ��
		return;
	}
	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "{7}%s", pchURL);//�ַ�����ȡ���
	//���ͳ�ȥ
	WORD RequestID = g_FishServer.GetQQFaceImgID();
	UINT64* pID = new UINT64(nOnlyID);
	if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, SignStr, true))//ֱ�ӷ���Get����
	{
		delete(pID);
		HandleQQUserImgResult(nOnlyID, nullptr, 0);
		return;
	}
}
void QQUserInfoManager::HandleQQUserImgResult(UINT64 nOnlyID, char* pchData, DWORD dwLength)
{
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagQQUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.dwClientID));
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	//�����ͷ�����ݷ��͵�FTP������ȥ ��FTP������ȥ����
	Iter->second.dwFaceID = SendFaceImgToFTP(Iter->second.dwUserID, pchData, dwLength);
	if (Iter->second.dwFaceID == 0)
	{
		//g_FishServer.ShowInfoToWin("�������Ĭ��ͷ��");

		OL_Cmd_UpdateAccount msg;
		SetMsgInfo(msg, GetMsgType(Main_Role, OL_UpdateAccount), sizeof(OL_Cmd_UpdateAccount));
		msg.bGender = Iter->second.bGender;
		msg.dwFaceID = Iter->second.dwFaceID;
		msg.dwUserID = Iter->second.dwUserID;
		TCHARCopy(msg.NickName, CountArray(msg.NickName), Iter->second.chNickName, _tcslen(Iter->second.chNickName));
		g_FishServer.SendNetCmdToClient(pClient, &msg);
		m_HashMapUserInfo.erase(Iter);
	}
}
DWORD QQUserInfoManager::SendFaceImgToFTP(DWORD dwUserID, char* pFaceImgData, DWORD dwLength)
{
	if (nullptr == pFaceImgData || dwLength == 0 || dwLength >= 50000)
		return 0;
	if (strcoll(pFaceImgData, "none") == 0)
		return 0;
	//�������ID ��ȡ����ͼƬ��״̬
	static int MASK1 = GetTickCount();
	static int MASK2 = MASK1 + rand();

	UINT t = GetTickCount();
	UINT crc = AECrc32(&t, sizeof(t), 0);
	t = dwLength;
	crc = AECrc32(&t, sizeof(t), crc);
	t = ++MASK1;
	crc = AECrc32(&t, sizeof(t), crc);
	t = ++MASK2;
	crc = AECrc32(&t, sizeof(t), crc);

	char xx[50];
	sprintf_s(xx, sizeof(xx), "%u_%u.jpg", (UINT)dwUserID, crc);//����·��
	//�����ݷ��͵�FTPȥ
	DWORD cmdSize = sizeof(GF_Cmd_SaveImageData) + (dwLength - 1);
	CheckMsgSizeReturn(cmdSize);
	GF_Cmd_SaveImageData *pCmd = (GF_Cmd_SaveImageData*)malloc(cmdSize);
	pCmd->SetCmdSize(ConvertDWORDToWORD(cmdSize));
	pCmd->SetCmdType(GetMsgType(Main_Role, GF_SaveImageData));//��������
	pCmd->ID = dwUserID;
	strncpy_s(pCmd->ImgName, sizeof(pCmd->ImgName), xx, CountArray(xx));
	pCmd->ImgSize = static_cast<WORD>(dwLength);
	pCmd->Size = ConvertDWORDToWORD(cmdSize);
	pCmd->ImgCrc = crc;
	memcpy_s(pCmd->ImgData, dwLength, pFaceImgData, dwLength);
	g_FishServer.SendNetCmdToFtp(pCmd);
	free(pCmd);

	return crc;
}
void QQUserInfoManager::HandleSaveRoleFaceImgResult(DWORD dwUserID, bool Result, DWORD FaceID)
{
	HashMap<DWORD, tagQQUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.dwClientID));
	if (!pClient)
	{
		ASSERT(false);
		return;
	}
	Iter->second.dwFaceID = FaceID;
	OL_Cmd_UpdateAccount msg;
	SetMsgInfo(msg, GetMsgType(Main_Role, OL_UpdateAccount), sizeof(OL_Cmd_UpdateAccount));
	msg.bGender = Iter->second.bGender;
	msg.dwFaceID = Iter->second.dwFaceID;
	msg.dwUserID = Iter->second.dwUserID;
	TCHARCopy(msg.NickName, CountArray(msg.NickName), Iter->second.chNickName, _tcslen(Iter->second.chNickName));

	//g_FishServer.ShowInfoToWin("��������Զ���ͷ�� %u", msg.dwFaceID);

	g_FishServer.SendNetCmdToClient(pClient, &msg);
	m_HashMapUserInfo.erase(Iter);
}

