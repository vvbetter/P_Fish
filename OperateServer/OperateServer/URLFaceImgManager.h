#pragma once
//处理网络图片的类
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

	//1.接收到下面服务器发送来的命令 需要加载指定位置的图片
	void OnHandleClientLoadURLImg(ServerClientData* pClient, DWORD dwUserID, const char* URL);
	//2.接收回来的消息
	void OnHandleWebInfo(UINT64 ID, char* pData, DWORD Length);
	//3.更新
	void OnUpdate(DWORD dwTimer);
private:
	HashMap<UINT64, tagURLImgOnce>		m_UserHashMap;
};
