#pragma once
#include <deque>
class AnnouncementManager
{
public:
	AnnouncementManager();
	virtual ~AnnouncementManager();

	//�����ݿ���ع���
	void OnLoadAllAnnouncementByDB();
	void OnLoadAllAnnouncementByDBResult(DBO_Cmd_LoadALlAnnouncement* pMsg);

	void OnSendAllAnnouncementToGameServer(BYTE GameID);//�����淢�͸�GameServer
	void OnAddNewAnnouncementOnce(AnnouncementOnce& pOnce);//����µĹ���
private:
	bool								m_IsFinish;
	std::vector<BYTE>					m_GameServerVec;
	std::deque<AnnouncementOnce>		m_AnnouncementList;//˫�˶���
};
