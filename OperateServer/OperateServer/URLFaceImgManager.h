#pragma once
//��������ͼƬ����
struct tagURLImgOnce
{
	UINT64		OnlyID;
	DWORD		dwUserID;
	DWORD		ClientID;
	std::string URL;
	DWORD		TimeLog;
};
class URLFaceImgManager
{
public:
	URLFaceImgManager();
	~URLFaceImgManager();

	//1.���յ���������������������� ��Ҫ����ָ��λ�õ�ͼƬ
	void OnHandleClientLoadURLImg(ServerClientData* pClient, DWORD dwUserID, const char* URL);
	//2.���ջ�������Ϣ
	void OnHandleWebInfo(UINT64 ID, char* pData, DWORD Length);
	//3.����
	void OnUpdate(DWORD dwTimer);
private:
	HashMap<UINT64, tagURLImgOnce>		m_UserHashMap;
};
