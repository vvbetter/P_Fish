#pragma once
#include "Bullet.h"
#include "TArray.h"
#include "../FishCallbak.h"
typedef HashMap<byte, Bullet*>	BulletMapList;	//��ǰ��ҵ��ӵ��б�
typedef BulletMapList::iterator BulletMapIt;
typedef vector<Bullet*> BulletVecList;
class BulletManager
{
public:
	bool Init()
	{
		return true;
	}
	void Shutdown()
	{
		ClearAllBullet();
	}
	void ClearAllBullet()
	{
		m_BulletVecList.clear();
		for (int i = 0; i < MAX_SEAT_NUM; ++i)
		{
			for (BulletMapIt it = m_BulletList[i].begin(); it != m_BulletList[i].end();)
			{
				delete it->second;
				it = m_BulletList[i].erase(it);
			}
		}
	}

	static Vector3 GetBulletDir(short x, short y)
	{
		return Vector3(x * InvShortMaxValue, y * InvShortMaxValue, 0);
	}
	static Vector3 GetBulletStartPos(short x, short y)
	{
		Vector3 pos;
		pos.x = (x * InvShortMaxValue) * RuntimeInfo::NearRightBottomPoint.x;
		pos.y = (y * InvShortMaxValue) * RuntimeInfo::NearLeftTopPoint.y;
		pos.z = RuntimeInfo::NearLeftTopPoint.z;
		return pos;
	}
	static void BulletPosToShort(const Vector3& pos , short &x, short &y)
	{
		float a = pos.x / RuntimeInfo::NearRightBottomPoint.x;
		float b = pos.y / RuntimeInfo::NearLeftTopPoint.y;
		a = Clamp(a, -1.0f, 1.0f);
		b = Clamp(b, -1.0f, 1.0f);
		x = short(a * SHRT_MAX);
		y = short(b * SHRT_MAX);
	}
	static void BulletDirToShort(const Vector3& dir, short &x, short &y)
	{
		x = short(dir.x  * SHRT_MAX);
		y = short(dir.y  * SHRT_MAX);
	}
	void Update(FishManager *pmgr, float delta)
	{
		m_BulletVecList.clear();
		for (int i = 0; i < MAX_SEAT_NUM; ++i)
		for (BulletMapIt it = m_BulletList[i].begin(); it != m_BulletList[i].end();)
		{
			Bullet *pBullet = it->second;
			if (!pBullet->Update(pmgr, delta))
			{
				USHORT id = pBullet->BulletID;
				//Log("update ɾ���ӵ�%d, lockfish=%d,ReboundCount=%d,runtime=%f", id, pBullet->LockFishID, pBullet->ReboundCount,pBullet->Time);
				delete pBullet;
				it = m_BulletList[i].erase(it);
			}
			else
			{
				m_BulletVecList.push_back(it->second);
				++it;
			}
		}
	}
	void RemoveBySeat(byte seat)
	{
		for (BulletMapIt it = m_BulletList[seat].begin(); it != m_BulletList[seat].end(); ++it)
		{
			it->second->CanBeUsed = false;
			it->second->LockFishID = 0;
			//delete it->second;
			//it = m_BulletList[seat].erase(it);
		}
	}
	bool AddBullet(Bullet *pBullet)
	{
		BYTE seat = GetSeat(pBullet);
		BYTE idx = GetIdx(pBullet);
		if (idx >= MAX_BULLET_NUM)
		{
			Log(L"�ӵ�����������ȷ:%d", idx);
			return false;
		}
		//ֱ���滻֮ǰ���ӵ�
		m_BulletList[seat][idx] = pBullet;
		return true;
	}
	bool FindByID(USHORT bulletID)
	{
		BYTE seat = GetSeat(bulletID);
		BYTE idx = GetIdx(bulletID);
		return m_BulletList[seat].find(idx) != m_BulletList[seat].end();
	}
	Bullet *FindBullet(USHORT bulletID)
	{
		BYTE seat = GetSeat(bulletID);
		BYTE idx = GetIdx(bulletID);
		BulletMapIt it = m_BulletList[seat].find(idx);
		if (it != m_BulletList[seat].end())
			return it->second;
		else
			return NULL;
	}
	void RemoveBullet(USHORT bullet)
	{
		byte seat = GetSeat(bullet);
		BulletMapList & bml = m_BulletList[seat];
		BulletMapIt it = bml.find(GetIdx(bullet));
		if (it == bml.end())
		{
			Log(L"�ӵ��Ƴ�ʧ��, Seat:%d, Idx:%d", GetSeat(bullet), GetIdx(bullet));
		}
		else
		{
			//Log(L"�Ƴ��ӵ�, Seat:%d, Idx:%d", GetSeat(bullet), GetIdx(bullet));
			delete it->second;
			bml.erase(it);
		}
	}
	bool Launch(UINT playerID, USHORT bulletID, USHORT bulletType, byte rateIndex, float speed, short degree, const Vector3 &dir, const Vector3 &startPos, BYTE ReboundCount = 0, USHORT lockFishID = 0,float e_pwer = 1.0f)
	{

		Bullet *pBullet = new Bullet;
		pBullet->CollideCount = 0;
		pBullet->CatchedNum = 0;
		pBullet->MaxCatchNum = 0;
		pBullet->CatchGold = 0;
		pBullet->BulletID = bulletID;
		pBullet->Player = playerID; 
		pBullet->BulletType = bulletType;
		pBullet->RateIndex = rateIndex;
		pBullet->Dir = dir;
		pBullet->Pos = pBullet->StartPos = startPos;
		pBullet->Degree = degree;
		pBullet->Speed = speed;
		pBullet->Time = 0;
		pBullet->ReboundCount = ReboundCount;
		pBullet->LockFishID = lockFishID;
		pBullet->OrgDir = pBullet->Dir;
		pBullet->ExtraPower = e_pwer;
		bool bRet = AddBullet(pBullet);
		if (!bRet)
			delete pBullet;
		return bRet;
	}
	BulletVecList* GetBulletMapList()
	{
		return &m_BulletVecList;
	}
	BYTE GetIdx(USHORT BulletID)
	{
		return BulletID & 0xff;
	}
	BYTE GetIdx(Bullet *pBullet)
	{
		return pBullet->BulletID & 0xff;
	}
	BYTE GetSeat(Bullet *pBullet)
	{
		return pBullet->BulletID >> 8;
	}
	BYTE GetSeat(USHORT bulletID)
	{
		return bulletID >> 8;
	}
protected:
	BulletMapList m_BulletList[MAX_SEAT_NUM];		//��ǰ���ӵ���ID��Ӧ
	BulletVecList m_BulletVecList;
};