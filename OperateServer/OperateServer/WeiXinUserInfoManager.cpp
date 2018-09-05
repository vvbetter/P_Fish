#include "stdafx.h"
#include "WeiXinUserInfoManager.h"
#include "FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
#include "json/json.h"
WeiXinUserInfoManager::WeiXinUserInfoManager()
{

}
WeiXinUserInfoManager::~WeiXinUserInfoManager()
{

}
void WeiXinUserInfoManager::HandleGetWeiXinUserInfo(const ServerClientData* pClient, DWORD dwUserID, const char* pchToken, const char* pchOpenID)
{
	if (nullptr == pClient || nullptr == pchToken || 0 == strlen(pchToken) || nullptr == pchOpenID || 0 == strlen(pchOpenID))
	{
		ASSERT(false);
		return;
	}
	UINT64 nOnlyID = dwUserID;
	nOnlyID = (nOnlyID << 32) + pClient->OutsideExtraData;
	HashMap<DWORD, tagWeiXinUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter != m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}

	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "{4}?access_token=%s&openid=%s", pchToken, pchOpenID);//�ַ�����ȡ���
	//���ͳ�ȥ
	//https://api.weixin.qq.com/sns/userinfo?access_token=%s&openid=%s
	WORD RequestID = g_FishServer.GetWeiXinUserInfoID();
	if (g_FishServerConfig.GetIsOperateTest())
	{
		//���Ϊ����
		tagWeiXinUserInfoOnce Once;
		Once.nOnlyID = nOnlyID;
		Once.dwUserID = dwUserID;
		Once.dwClientID = pClient->OutsideExtraData;
		Once.dwTimeLog = timeGetTime();
		m_HashMapUserInfo.insert(HashMap<DWORD, tagWeiXinUserInfoOnce>::value_type(Once.dwUserID, Once));

		string strTestUrlResult = "{\"openid\":\"OPENID\",\"nickname\" : \"NICKNAME\",\"sex\" : 1,\"province\" : \"PROVINCE\",\"city\" : \"CITY\",\"country\" : \"COUNTRY\",\"headimgurl\" : \"http://wx.qlogo.cn/mmopen/g3MonUZtNHkdmzicIlibx6iaFqAc56vxLSUfpb6n5WKSYVY0ChQKkiaJSgQ1dZuTOgvLLrhJbERQQ4eMsv84eavHiaiceqxibJxCfHe/0\",\"privilege\" : [\"PRIVILEGE1\",\"PRIVILEGE2\"],\"unionid\" : \" o6_bmasdasdsad6_2sgVt7hMZOPfL\"}";

		HandleWeiXinUserInfoResult(nOnlyID, (char*)strTestUrlResult.c_str(), strTestUrlResult.length());
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
	tagWeiXinUserInfoOnce Once;
	Once.nOnlyID = nOnlyID;
	Once.dwUserID = dwUserID;
	Once.dwClientID = pClient->OutsideExtraData;
	Once.dwTimeLog = timeGetTime();
	Once.bGender = false;
	Once.dwFaceID = 0;
	m_HashMapUserInfo.insert(HashMap<DWORD, tagWeiXinUserInfoOnce>::value_type(Once.dwUserID, Once));
}
void WeiXinUserInfoManager::HandleWeiXinUserInfoResult(UINT64 nOnlyID, char* pchData, DWORD dwLength)
{
	/*
	{
	"openid":"OPENID",
	"nickname" : "NICKNAME",
	"sex" : 1,
	"province" : "PROVINCE",
	"city" : "CITY",
	"country" : "COUNTRY",
	"headimgurl" : "http://wx.qlogo.cn/mmopen/g3MonUZtNHkdmzicIlibx6iaFqAc56vxLSUfpb6n5WKSYVY0ChQKkiaJSgQ1dZuTOgvLLrhJbERQQ4eMsv84eavHiaiceqxibJxCfHe/0",
	"privilege" : [
	"PRIVILEGE1",
	"PRIVILEGE2"
	],
	"unionid" : " o6_bmasdasdsad6_2sgVt7hMZOPfL"

	}
	{"errcode":41001,"errmsg":"access_token missing, hints: [ req_id: l5m0085ns54 ]"}
	*/
	if (0 == dwLength || nullptr == pchData)
	{
		ASSERT(false);
		return;
	}
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagWeiXinUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	//�����յ������ݽ���ת�� ��ȡ�ض�������
	std::string strJosnInfo = pchData;
	
	std::istringstream ijson(strJosnInfo);
	Json::CharReaderBuilder jsonReader;
	Json::Value jsonRoot;
	string err;
	if (!Json::parseFromStream(jsonReader, ijson, &jsonRoot, &err))
	{
		ASSERT(false);
		return;
	}
	if (!jsonRoot["errcode"].isNull())
	{
		m_HashMapUserInfo.erase(Iter);
		ASSERT(false);
		return;
	}
	if (jsonRoot["nickname"].isNull() ||
		jsonRoot["sex"].isNull() ||
		jsonRoot["headimgurl"].isNull()
		)
	{
		m_HashMapUserInfo.erase(Iter);
		ASSERT(false);
		return;
	}
	std::string strNickName = jsonRoot["nickname"].asString();
	bool bGender = !jsonRoot["sex"].asBool();
	std::string strFaceURL = jsonRoot["headimgurl"].asString();
	//std::string strOpenID = jsonRoot["openid"].asString();

	Iter->second.bGender = bGender;
	UINT Size = 0;
	TCHAR* pNickName = CharToWChar(strNickName.c_str(), strNickName.length(), Size);
	TCHARCopy(Iter->second.chNickName, CountArray(Iter->second.chNickName), pNickName, Size);
	free(pNickName);
	//���ݻ�ȡ�ɹ��� ���Ǽ�����������
	HandleGetWeiXinUserImg(nOnlyID, strFaceURL.c_str());
}
void WeiXinUserInfoManager::HandleGetWeiXinUserImg(UINT64 nOnlyID, const char* pchURL)
{
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagWeiXinUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	//����URL��ַ ��ȡ��ҵ�ͷ��
	if (nullptr == pchURL || strlen(pchURL) == 0)
	{
		HandleWeiXinUserImgResult(nOnlyID, nullptr, 0);//ֱ���������ͷ��δ��
		return;
	}
	char SignStr[2048];
	sprintf_s(SignStr, CountArray(SignStr), "{5}%s", pchURL);//�ַ�����ȡ���
	//���ͳ�ȥ
	WORD RequestID = g_FishServer.GetWeiXinFaceImgID();
	UINT64* pID = new UINT64(nOnlyID);
	if (!g_FishServer.GetHttpClient().AddRequest((UINT_PTR)pID, RequestID, SignStr, true))//ֱ�ӷ���Get����
	{
		delete(pID);
		HandleWeiXinUserImgResult(nOnlyID, nullptr, 0);
		return;
	}
}
void WeiXinUserInfoManager::HandleWeiXinUserImgResult(UINT64 nOnlyID, char* pchData, DWORD dwLength)
{
	DWORD dwUserID = static_cast<DWORD>(nOnlyID >> 32);
	HashMap<DWORD, tagWeiXinUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
	if (Iter == m_HashMapUserInfo.end())
	{
		ASSERT(false);
		return;
	}
	ServerClientData* pClient = g_FishServer.GetUserClientDataByIndex(static_cast<BYTE>(Iter->second.dwClientID));
	if (!pClient)
	{
		m_HashMapUserInfo.erase(Iter);
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
DWORD WeiXinUserInfoManager::SendFaceImgToFTP(DWORD dwUserID, char* pFaceImgData, DWORD dwLength)
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
void WeiXinUserInfoManager::HandleSaveRoleFaceImgResult(DWORD dwUserID, bool Result, DWORD FaceID)
{
	HashMap<DWORD, tagWeiXinUserInfoOnce>::iterator Iter = m_HashMapUserInfo.find(dwUserID);
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

	//g_FishServer.ShowInfoToWin("��������Զ���ͷ�� %u",msg.dwFaceID);

	g_FishServer.SendNetCmdToClient(pClient, &msg);
	m_HashMapUserInfo.erase(Iter);
}
