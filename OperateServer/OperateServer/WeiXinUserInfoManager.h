//΢����������޸�
#pragma once
struct tagWeiXinUserInfoOnce
{
	UINT64  nOnlyID;
	DWORD	dwUserID;
	DWORD	dwClientID;
	DWORD	dwTimeLog;
	//�ⲿ����
	TCHAR	chNickName[MAX_NICKNAME + 1];
	DWORD	dwFaceID;
	bool	bGender;
	tagWeiXinUserInfoOnce()
	{
		nOnlyID = 0;
		dwUserID = 0;
		dwClientID = 0;
		dwTimeLog = 0;
		ZeroMemory(chNickName, sizeof(chNickName));
		dwFaceID = 0;
		bGender = false;
	}
};
class WeiXinUserInfoManager
{
public:
	WeiXinUserInfoManager();
	~WeiXinUserInfoManager();

	//1.
	void HandleGetWeiXinUserInfo(const ServerClientData* pClient, DWORD dwUserID, const char* pchToken, const char* pchOpenID);//�������ݷ������Web������ȥ
	//2.
	void HandleWeiXinUserInfoResult(UINT64 nOnlyID, char* pchData, DWORD dwLength);//����΢����ҵķ��ؽ��
	//3.
	void HandleGetWeiXinUserImg(UINT64 nOnlyID, const char* pchURL);
	//4.
	void HandleWeiXinUserImgResult(UINT64 nOnlyID, char* pchData, DWORD dwLength);//����ͼƬ�Ľ������
	//5.
	void HandleSaveRoleFaceImgResult(DWORD dwUserID, bool Result, DWORD FaceID);
private:
	DWORD SendFaceImgToFTP(DWORD dwUserID, char* pFaceImgData, DWORD dwLength);
private:
	HashMap<DWORD, tagWeiXinUserInfoOnce>		m_HashMapUserInfo;
};