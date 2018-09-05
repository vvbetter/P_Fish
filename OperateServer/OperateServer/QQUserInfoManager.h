#pragma once
//QQ ��ȡQQ��ҵ�����
struct tagQQUserInfoOnce
{
	UINT64  nOnlyID;
	DWORD	dwUserID;
	DWORD	dwClientID;
	DWORD	dwTimeLog;
	//�ⲿ����
	TCHAR	chNickName[MAX_NICKNAME + 1];
	DWORD	dwFaceID;
	bool	bGender;
	tagQQUserInfoOnce()
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
class QQUserInfoManager
{
public:
	QQUserInfoManager();
	~QQUserInfoManager();

	//1.
	void HandleGetQQUserInfo(const ServerClientData* pClient, DWORD dwUserID, const char* pchToken, const char* pchOpenID);//�������ݷ������Web������ȥ
	//2.
	void HandleQQUserInfoResult(UINT64 nOnlyID, char* pchData, DWORD dwLength);//����΢����ҵķ��ؽ��
	//3.
	void HandleGetQQUserImg(UINT64 nOnlyID, const char* pchURL);
	//4.
	void HandleQQUserImgResult(UINT64 nOnlyID, char* pchData, DWORD dwLength);//����ͼƬ�Ľ������
	//5.
	void HandleSaveRoleFaceImgResult(DWORD dwUserID, bool Result, DWORD FaceID);
private:
	DWORD SendFaceImgToFTP(DWORD dwUserID, char* pFaceImgData, DWORD dwLength);
private:
	HashMap<DWORD, tagQQUserInfoOnce>		m_HashMapUserInfo;
};