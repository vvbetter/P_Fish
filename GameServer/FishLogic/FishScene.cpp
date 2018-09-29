#include "stdafx.h"
#include "FishScene.h"
#include "../Role.h"
#include "FishScene\FishManager.h"
#include "FishScene\BulletManager.h"
#include "FishScene\FishLauncher.h"
#include "NetCmd.h"
#include "FishScene\FishCollider.h"
//#include "FishScene\Timer.h"
#include "FishScene\FishMap.h"
#include <math.h>
#define INTERVAL_FLOW 3500
FishScene::FishScene()
{
	m_pFishMgr = NULL;
	m_pFishLauncher = NULL;
	m_pBulletMgr = NULL;
	m_pFishMap = NULL;
	m_bSceneEnd = false;
	m_bFlowInterval = false;
	m_SceneStopTime = 0;
	m_IsSceneStopTime = false;
}
FishScene::~FishScene()
{

}
void FishScene::Reset()
{
	m_pFishMap->Reset();
	int idx = m_pFishMap->GetFlowIndex(m_bFishTide);
	FlowData *pFlow = FishResManager::Inst()->GetFlow(idx);
	m_pFishLauncher->Init(pFlow);
	m_pFishMgr->ClearAllFish(true);
	m_pBulletMgr->ClearAllBullet();
	m_bFlowInterval = false;
	m_FlowInterval = 0;
}
bool FishScene::Init(CTableRoleManager *pm, FishSendInterface *pSend, BYTE TableTypeID)
{
	m_bFishTide = false;
	m_pRoleMgr = pm;
	m_pFishMap = new FishMap();
	m_pFishMgr = new FishManager;
	m_pFishLauncher = new FishLauncher(m_pFishMgr);
	m_pBulletMgr = new BulletManager;
	m_pSender = pSend;
	m_TableTypeID = TableTypeID;
	bool bRet = true;
	bRet &= m_pFishMgr->Init();
	bRet &= m_pBulletMgr->Init();
	bRet &= m_pFishLauncher->Init(NULL, m_pSender, TableTypeID);
	m_SwapInterval = 0;
	m_SwapScene = false;
	if (!bRet)
	{
		Shutdown();
		return false;
	}
	else
		return true;
}
bool FishScene::SetMap(const WCHAR *pcMapName)
{
	if (m_pFishMap->LoadMap(pcMapName))
	{
		Reset();
		return true;
	}
	else
		return false;
}
void FishScene::Shutdown()
{
	m_pSender = NULL;
	m_pFishMgr->Shutdown();
	m_pBulletMgr->Shutdown();
	m_pFishLauncher->Shutdown();

	SAFE_DELETE(m_pFishMap);
	SAFE_DELETE(m_pFishLauncher);
	SAFE_DELETE(m_pFishMgr);
	SAFE_DELETE(m_pBulletMgr);
}
void FishScene::Update(float deltaTime)
{
	bool bClearScene = false;
	UINT package = 0xffffffff;
	//���Ƴ�������
	float updateTime = deltaTime;
	if (m_IsSceneStopTime == true)
	{
		m_SceneStopTime -= (DWORD)(deltaTime * 1000);
		updateTime = 0.0f;
	}
	if (m_SceneStopTime <= 0)
	{
		m_IsSceneStopTime = false;
		m_SceneStopTime = 0;
	}
	//��ʼ��������
	if (m_bFlowInterval == false)
	{
		package = m_pFishMap->Update(updateTime);
		//����ʹ������,�����µġ�
		if (m_pFishLauncher->IsActive() == false)
		{
			if (m_pFishMap->HasFlowIndex() == false)
			{
				if (m_pFishMap->IsRepeat())
					m_pFishMap->Reset();
				else
				{
					//�����ˡ�
					m_bSceneEnd = true;
				}
			}
			if (m_bSceneEnd == false)
			{
				int idx = m_pFishMap->GetFlowIndex(m_bFlowInterval);
				m_bFishTide = m_bFlowInterval;
				if (m_bFlowInterval)
				{
					m_pFishMgr->ClearAllFish(false);
					m_FlowIndex = ConvertIntToWORD(idx);
					m_FlowInterval = GetTickCount();
					//Log(L"�л����󣬼��3.5��:%d", idx);

					NetCmdClearScene nt;
					SetMsgInfo(nt, CMD_CLEAR_SCENE, sizeof(NetCmdClearScene));
					nt.ClearType = rand() % 2;
					m_pSender->SendAll(&nt);
					m_pRoleMgr->SwitchFishTide();
					bClearScene = true;
				}
				else
				{
					//Log(L"�л�����:%d", idx);
					m_pFishLauncher->Init(FishResManager::Inst()->GetFlow(idx));
				}
			}
		}
	}
	if (m_bFlowInterval == false)
	{
		if (m_bFishTide)
			package = -1;
		UINT fishID_PackageType = m_pFishLauncher->Update(updateTime, package);
		if (fishID_PackageType != -1)
		{
			//�����˺��
			USHORT fishID = fishID_PackageType >> 16;
			m_pFishMgr->GetFish(fishID)->SetPackage(fishID_PackageType & 0xff);
			//FishCallback::GetFishSetting()->LaunchPackage(fishID_PackageType);
		}
	}
	else
	{
		if (GetTickCount() - m_FlowInterval > INTERVAL_FLOW)
		{
			//Log(L"�л�����:%d", m_FlowIndex);
			m_pFishLauncher->Init(FishResManager::Inst()->GetFlow(m_FlowIndex));
			m_bFlowInterval = false;
		}
	}

	m_pFishMgr->Update(updateTime);
	m_pBulletMgr->Update(m_pFishMgr, deltaTime);
	UINT count = m_pFishMgr->GetFishCount();
	if (count == 0)
	{
		if (m_SwapScene == false)
		{
			if (m_pFishLauncher->HasLaunchFish() == false)
			{
				m_SwapScene = true;
				m_SwapInterval = 0;
			}
		}
		else
		{
			m_SwapInterval += updateTime;
			if (m_SwapInterval >= 5)
			{
				//���ó���
				//Log("���Ѿ��������ˣ��л�������!");
				m_SwapScene = false;
				m_SwapInterval = 0;
				m_pFishLauncher->SetDeactive();
			}
		}
	}
	FishCollider::Collide(m_pRoleMgr, m_pBulletMgr, m_pFishMgr, m_pSender);

}
void FishScene::Clear()
{
	m_pFishLauncher->InitFlowData(true);
	m_pFishMgr->ClearAllFish(true);
	m_pBulletMgr->ClearAllBullet();
}
byte FishScene::GetSceneBackground()const
{
	return m_pFishMap->BackgroundType();
}
const WCHAR *FishScene::GetMapName()const
{
	return m_pFishMap->GetMapName();
}

///------------alg 3 ���������߶��Ƿ��ཻ------------  
/*�߶�1����̨���굽Ŀ������������  p-f
  �߶�2��������ͶӰ����Ļ�ľ������� m-n
  Ŀ�����ж���̨p��Ŀ����f֮���Ƿ�����������
	y        /f
	|	\n	/
	|    \ /
	|     \
	|	 / \
	|	/   \m
  --|--p----------------------- x
*/
double determinant(double v1, double v2, double v3, double v4)  // ����ʽ  
{
	return (v1*v3 - v2 * v4);
}
// aa,bb,cc,dd ��2���߶ε�4���˵�
bool intersect(const Vector2& aa, const Vector2& bb, const Vector2& cc, const Vector2& dd)
{
	double delta = determinant(bb.x - aa.x, cc.x - dd.x, bb.y - aa.y, cc.y - dd.y);
	if (delta <= (1e-6) && delta >= -(1e-6))  // delta=0����ʾ���߶��غϻ�ƽ��  
	{
		return false;
	}
	double namenda = determinant(cc.x - aa.x, cc.x - dd.x, cc.y - aa.y, cc.y - dd.y) / delta;
	if (namenda > 1 || namenda < 0)
	{
		return false;
	}
	double miu = determinant(bb.x - aa.x, cc.x - aa.x, bb.y - aa.y, cc.y - aa.y) / delta;
	if (miu > 1 || miu < 0)
	{
		return false;
	}
	return true;
}
///------------alg 3------------  

USHORT FishScene::GetAngleByFish(WORD& LoackFishID, BYTE SeatID, Vector2& pCenter, BYTE& FishValue)
{
	if (!m_pFishMgr)
	{
		LoackFishID = 0;
		return 0xffff;
	}
	//��׼һ���� ��ȡ�ڵ���̨����
	Fish *pFish = m_pFishMgr->GetFish(LoackFishID);
	if (!pFish || !pFish->IsInFrustum || !pFish->IsActive || pFish->IsDead)
	{
		if (LoackFishID != (WORD)-1)// LoackFishID -1 ��ȡ��һ���㡣
		{
			LoackFishID = 0;
			return 0xffff;
		}
		FishVecList* pVec = m_pFishMgr->GetFishVecList();
		if (pVec->empty())
		{
			LoackFishID = 0;
			return 0xffff;
		}
		BYTE BulletRate;
		pFish = GetRobotFish(pCenter, BulletRate);
		if (!pFish)
		{
			LoackFishID = 0;
			return 0xffff;
		}
		if (!pFish->IsInFrustum || !pFish->IsActive || pFish->IsDead)
		{
			LoackFishID = 0;
			return 0xffff;
		}
	}
	LoackFishID = pFish->FishID;
	FishValue = pFish->FishValue;

	Vector2 UpDir(0.0f, SeatID > 1 ? -1.0f : 1.0f);
	Vector2 pos = pFish->ScreenPos;//�������
	Vector2 Direction = (pos - pCenter);
	if (SeatID <= 1 && Direction.y < 0.1f)
		Direction.y = 0.1f;
	else if (SeatID > 1 && Direction.y > -0.1f)
		Direction.y = 0.1f;
	if (Direction.x < 0.1f && Direction.x > 0.0f)
		Direction.x = 0.1f;
	else if (Direction.x > -0.1f && Direction.x < 0.0f)
		Direction.x = -0.1f;
	float dirlen = Vec2Length(Direction);
	Direction /= dirlen;
	float dot = D3DXVec2Dot(&UpDir, &Direction);
	float Angle = acos(Clamp(dot, 0.0f, 1.0f)) * 57.2958f;
	if (Direction.x >= 0)
		Angle = -Angle;
	Angle = Clamp(Angle, -85, 85);
	short TempAngle = (short)(Angle / 90.0f * 32767);
	if (SeatID == 1 || SeatID == 0)
		TempAngle = TempAngle;
	else
		TempAngle = TempAngle * -1;
	return TempAngle;
}

void FishScene::SetSceneStopEndTime(DWORD addTime)
{
	m_SceneStopTime = addTime;
	m_IsSceneStopTime = true;
}

Fish* FishScene::GetRobotFish(const Vector2& pCenter, BYTE& BulletRate)
{
	Fish* tagetFish = NULL;
	CConfig* gameConfig = g_FishServer.GetTableManager()->GetGameConfig();
	FishVecList* pVec = m_pFishMgr->GetFishVecList();
	//��ѯʣ��Ѫ�����ٵ���
	float minFishHpRate = 1.0f, tempHpRate = 0.0f;
	UINT fishIndex = -1;
	//��������̨�������
	UINT nearFishIndex = -1;
	float length = 1000.0f, tempLength = 0.0f;
	//////////////
	for (int i = 0; i < pVec->size()*0.8; ++i)
	{
		Fish* oneFish = pVec->at(i);
		//�����������
		Vector2 l = oneFish->ScreenPos - pCenter;
		tempLength = D3DXVec2LengthSq(&l);
		if (tempLength < length)
		{
			length = tempLength;
			nearFishIndex = i;
		}
		//����ʣ��Ѫ�����ٵ���
		if (oneFish->FishValue < 10 || oneFish->FishValue>45)
			continue;
		for (int r = 0; r < gameConfig->RateCount(); ++r)
		{
			const float& maxHp = oneFish->fishHp[r].maxHp;
			const float& attackHp = oneFish->fishHp[r].attackHp;
			const float remainHp_P = (maxHp - attackHp) / maxHp;
			if (remainHp_P < SystemControl::minPerKillHp * 2)
				continue;
			if (remainHp_P < minFishHpRate)
			{
				minFishHpRate = remainHp_P;
				fishIndex = i;
				BulletRate = r;
			}
		}
	}
	if (fishIndex != (UINT)-1 && RandFloat() < 0.5f) //�ҵ�һ���Ƚ����״������㣬��50%����ȥ����
	{
		tagetFish = pVec->at(fishIndex);
	}
	else if (nearFishIndex != (UINT)-1)
	{
		tagetFish = pVec->at(nearFishIndex);
	}
	return tagetFish;
}