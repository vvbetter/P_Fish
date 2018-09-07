#pragma once
#include "FishScene\FishCollider.h"
class FishManager;
class FishLauncher;
class BulletManager;
class FishSendInterface;
class FishMap;
class CTableRoleManager;
struct NetCmd;

#include <list>
class FishScene
{
public:
	FishScene();
	~FishScene();

	bool Init(CTableRoleManager *pm, FishSendInterface *pSend, BYTE TableTypeID);
	void Shutdown();
	void Update(float deltaTime);
	bool SetMap(const WCHAR *pcMapName);
	void Reset();
	BulletManager * GetBulletMgr()
	{
		return m_pBulletMgr;
	}
	FishManager * GetFishMgr()
	{
		return m_pFishMgr;
	}
	FishLauncher * GetFishLauncher()
	{
		return m_pFishLauncher;
	}
	bool IsEndScene()const
	{
		return m_bSceneEnd && m_pFishMgr->FishCount() == 0;
	}
	const WCHAR *GetMapName()const;
	void Clear();
	byte GetSceneBackground()const;

	USHORT GetAngleByFish(WORD& LoackFishID, BYTE SeatID, Vector2& Pos, Vector2& pCenter);
	void SetSceneStopEndTime(DWORD addTime/*����*/);
	bool IsSceneStopTime(){ return  m_IsSceneStopTime; }
protected:
	bool				m_bFishTide;
	CTableRoleManager*  m_pRoleMgr;
	bool				m_bFlowInterval; //�㳱�����3.5��
	float				m_SwapInterval;
	bool				m_SwapScene;
	UINT				m_FlowInterval;
	USHORT				m_FlowIndex;
	bool				m_bSceneEnd;	//�����е��㷢�������
	FishSendInterface	*m_pSender;
	FishManager			*m_pFishMgr;
	FishLauncher		*m_pFishLauncher;
	BulletManager		*m_pBulletMgr;
	FishMap				*m_pFishMap;
	DWORD				m_SceneStopTime; //������������ʱ��
	bool				m_IsSceneStopTime;
	BYTE				m_TableTypeID;
};
