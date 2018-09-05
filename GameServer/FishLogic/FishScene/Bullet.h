#pragma once
#include "FishResData.h"
#include "RuntimeInfo.h"
#include "TArray.h"
#include "FishManager.h"
class Bullet
{
public:
	Bullet()
	{
		ReboundCount = 0;
		LockFishID = 0;
		CanBeUsed = true;
	}
	bool UpdateLockFish(FishManager *pmgr)
	{
		Fish *pfish = pmgr->GetFish(LockFishID);
		if (pfish == NULL)
			return false;

		Vector2 dir = pfish->ScreenPos - ScreenPos;
		float dirlen = Vec2Length(dir);
		dir /= dirlen;
		Dir.x = dir.x;
		Dir.y = dir.y;
		//Log("id = %d; ReboundCount=%d; time=%f; ScreenPos.x=%f; ScreenPos.y=%f; dir.x=%f; dir.y=%f;fish_pos.x=%f fish_pos.y=%f", BulletID, ReboundCount, Time, ScreenPos.x, ScreenPos.y, Dir.x, Dir.y, pfish->ScreenPos.x, pfish->ScreenPos.y);
		return true;
	}
	bool Update(FishManager *pmgr, float deltaTime)
	{
		if (LockFishID != 0)
		{
			if (!UpdateLockFish(pmgr))
			{
				ClearLockFishID();
			}
		}
		Time += deltaTime;
		Pos += Dir * deltaTime * Speed;
		Vector3 p = WorldToProjection(Pos);
		ScreenPos = ProjectionToViewport(p);
		return CheckBoundary();
	}
	bool CheckBoundary()
	{
		//�ü�����Ļ����ӵ�
		if (Pos.x > RuntimeInfo::NearRightBottomPoint.x || Pos.x < RuntimeInfo::NearLeftTopPoint.x ||
			Pos.y > RuntimeInfo::NearLeftTopPoint.y || Pos.y < RuntimeInfo::NearRightBottomPoint.y)
		{
			LockFishID = 0;
			if (ReboundCount > 0)
			{
				Vector2 dir = Dir;
				if (Pos.x > RuntimeInfo::NearRightBottomPoint.x)
				{
					Pos.x = RuntimeInfo::NearRightBottomPoint.x;
					Dir.x = -Dir.x;
				}
				else if (Pos.x < RuntimeInfo::NearLeftTopPoint.x)
				{
					Pos.x = RuntimeInfo::NearLeftTopPoint.x;
					Dir.x = -Dir.x;
				}
				if (Pos.y > RuntimeInfo::NearLeftTopPoint.y)
				{
					Pos.y = RuntimeInfo::NearLeftTopPoint.y;
					Dir.y = -Dir.y;

				}
				else if (Pos.y < RuntimeInfo::NearRightBottomPoint.y)
				{
					Pos.y = RuntimeInfo::NearRightBottomPoint.y;
					Dir.y = -Dir.y;
				}
				StartPos = Pos;
				Time = 0;
				--ReboundCount;
				return true;
			}
			else
				return false;
		}
		return true;
	} 
	void AddCollideFishID(USHORT id, bool bCatched)
	{
		if(CheckCollideFish(id))
		{
			Log(L"������ͬ����ײ��ID:%d", id);
			return;
		}
		CatchedNum += bCatched;
		CollideFishList.insert(make_pair(id, bCatched));
	}
	bool CheckCollideFish(USHORT id)
	{
		return CollideFishList.find(id) != CollideFishList.end();
	}
	void ClearLockFishID()
	{
		LockFishID = 0;
		Dir = OrgDir;
	}
	HashMap<USHORT, byte> CollideFishList;
	
	Vector3		OrgDir;
	USHORT		LockFishID;
	BYTE		ReboundCount;	//��������
	BYTE		CollideCount;	//����ײ����
	BYTE		CatchedNum;		//��������
	BYTE		MaxCatchNum;	//�����ײ����
	PlayerID	Player;			//�ӵ��������ID;
	USHORT		BulletID;		//�ӵ���ID
	USHORT		BulletType;		//�ӵ�������
	Vector2		ScreenPos;
	Vector3		Dir;			//���з���
	Vector3		StartPos;		//��ʼλ��
	Vector3		Pos;
	float		Speed;			//�ٶ�
	float		Time;			//����ʱ��
	short		Degree;
	byte		RateIndex;
	UINT		CatchGold;
	bool		CanBeUsed;		//�ӵ��Ƿ�ʧЧ
	float		ExtraPower;     //�ӵ���������ϵ��
};