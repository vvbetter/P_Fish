#pragma once
#include "../../FishServer.h"
#include "FishResData.h"
#include "PathController.h"
#include "FishResManager.h"
#define INFRUSTUM_OFFSET 0.97f
#define INFRUSTUM_OFFSET_SCR 7.0f
#include "Timer.h"
#define START_POS -3000
#define FISH_HP_RANDRANGE_N 10000
class Fish;

extern FishServer g_FishServer;

struct FishInfo
{
	Fish*  pFish;
};
enum SideType
{
	SIDE_LEFT,
	SIDE_RIGHT,
	SIDE_TOP,
	SIDE_BOTTOM,
	SIDE_MAX
};

struct FishHpInfo 
{
	float maxHp;			//ÿ�����Ӧ���ʵ����Ѫ��
	float attackHp;			//�Ѿ��˺�Ѫ��
	float seatAttack[4];	//4��λ���˺�Ѫ��
	FishHpInfo()
	{
		maxHp = 0;
		attackHp = 0;
		seatAttack[0] = 0;
		seatAttack[1] = 0;
		seatAttack[2] = 0;
		seatAttack[3] = 0;
	}
};

class Fish
{
public:
	void InitFishData(USHORT groupID, USHORT id, USHORT fishType, USHORT pathGroup, USHORT pathIndex, float fishScaling, float speed, const Vector3 &offset, float time, PathData *pPathData,BYTE TableTypeID)
	{
		//Log("Fish: id:%d,GroupID:%d,FishType:%d,pathGroup:%d,pathIndex:%d", id, groupID, FishType, pathGroup, pathIndex);
		PackageType = 255;
		GroupID = groupID;
		FishID = id;
		FishType = fishType;
		Speed = speed;
		FishScaling = fishScaling;
		PathGroup = pathGroup;
		PathIndex = pathIndex;
		Offset = offset;
		Offset.x = 0;
		FishBoxSize = FishResManager::Inst()->GetFishData(FishType)->Size * FishScaling;
		HasAttackAction = FishResManager::Inst()->HasAttackAction(FishType);
		Controller.ResetController(pPathData, speed, time, FishResManager::Inst()->GetFishData(FishType)->ClipLength[CLIP_CHAOFENG]);
		IsInFrustum = false;
		IsActive = true;
		IsDead = false;
		WorldPos = Vector3(START_POS, 0, 0);

		const vector<float>& sysRate = g_FishServer.GetTableManager()->SysProduceRate(TableTypeID);
		CConfig* gameConfig = g_FishServer.GetTableManager()->GetGameConfig();
		for (int i = 0; i < gameConfig->RateCount(); ++i)
		{
			float maxHp = 1.0f * gameConfig->GetFishValue(FishType) * gameConfig->BulletMultiple(i)*gameConfig->GetFishSpecialRate(fishType);//���Ѫ��
			FishHpInfo oneRateHp;
			oneRateHp.maxHp = maxHp* (RandRange(static_cast<UINT>(sysRate[0] * FISH_HP_RANDRANGE_N), static_cast<UINT>(sysRate[1] * FISH_HP_RANDRANGE_N))*1.0f / FISH_HP_RANDRANGE_N);
			fishHp.insert(make_pair(i, oneRateHp));
		}
	}
	bool IsValidTime()
	{
		return GetTime() >= 0.0f;
	}
	float GetTime()
	{
		return Controller.GetTime();
	}
	bool UpdateFish(float deltaTime)
	{
		//·������
		bool bRet = Controller.Update(deltaTime);

		if (Controller.GetTime() >= 1.f)
			return false;

		if (!bRet)
			return true;

		//����λ��
		Quaternion rot = Controller.GetRotation() * Controller.GetWorldRotation();
		Vector3 pos = Controller.GetPosition() + Offset;

		D3DXVec3TransformCoord(&WorldPos, &pos, &Controller.GetWorldMatrix());
		D3DXQuaternionMultiply(&WorldRot, &FishResManager::Inst()->GetFishData(FishType)->Rot, &rot);

		if(WorldPos.z < CAMERA_NEAR || WorldPos.z > CAMERA_FAR)
			IsActive = false;
		else
			IsActive = true;
		
		//������Ļ��
		ProjScreenPos = WorldToProjection(WorldPos);
		ScreenPos = ProjectionToViewport(ProjScreenPos);

		IsInFrustum = InFrustum(ProjScreenPos, INFRUSTUM_OFFSET);

		//����Ļ�ڣ������8����Χ�е�
		//if (IsInFrustum)
		{
			RotateBoxPoint();
		}
		return true;
	}
	void RotateBoxPoint()
	{
		Matrix mat;
		D3DXMatrixTransformation(&mat, NULL, NULL, NULL, &WorldPos, &WorldRot, NULL);
		Vector3 halfSize = FishBoxSize * 0.5f;
		Vector3 maxpoint = WorldPos + halfSize;
		Vector3 minpoint = WorldPos - halfSize;

		Vector3 BoxPoint[8];

		BoxPoint[0] = Vector3(maxpoint.x, maxpoint.y, maxpoint.z);
		BoxPoint[1] = Vector3(minpoint.x, maxpoint.y, maxpoint.z);
		BoxPoint[2] = Vector3(minpoint.x, minpoint.y, maxpoint.z);
		BoxPoint[3] = Vector3(maxpoint.x, minpoint.y, maxpoint.z);

		BoxPoint[4] = Vector3(maxpoint.x, maxpoint.y, minpoint.z);
		BoxPoint[5] = Vector3(minpoint.x, maxpoint.y, minpoint.z);
		BoxPoint[6] = Vector3(minpoint.x, minpoint.y, minpoint.z);
		BoxPoint[7] = Vector3(maxpoint.x, minpoint.y, minpoint.z);

		mat *= RuntimeInfo::ProjectMatrix;
		float maxradius = 0;

		for (int i = 0; i < 8; ++i)
		{
			Vector3 pos;
			D3DXVec3TransformCoord(&pos, &BoxPoint[i], &mat);
			Vector2 viewpos = ScreenBoxPoint[i] = ProjectionToViewport(pos);
			Vector2 dist = viewpos - ScreenPos;
			float d = Vec2LengthSq(dist);
			if (d > maxradius)
				maxradius = d;

			//��������Ļ�⣬���жϸ������Ƿ�����Ļ��
			if (IsInFrustum == false)
			{
				float w = RuntimeInfo::NearX - fabsf(viewpos.x);
				float h = RuntimeInfo::NearY - fabsf(viewpos.y);
				if (w > INFRUSTUM_OFFSET_SCR && h > INFRUSTUM_OFFSET_SCR)
				{
					IsInFrustum = true;
				}
			}

		}
		ScreenRadius = sqrtf(maxradius);
		
	}
	bool UpdateDeadTime(float delta)
	{
		DeadTime += delta;
		if (DeadTime >= DeadStayTime)
			return true;
		UpdateFish(delta);
		return false;
	}
	void SetDead(float stayTime)
	{
		DeadStayTime = stayTime;
		IsDead = true;
		DeadTime = 0;
	}
	void SetPackage(byte package)
	{
		PackageType = package;
	}
	BYTE GetPackage()
	{
		return PackageType;
	}
protected:
	bool InFrustum(Vector3 pos, float padding)
	{
		bool bret = (pos.x > -padding && pos.x < padding && pos.y > -padding && pos.y < padding);
		if (bret == false)
		{
			////�����������Ļ�ߵľ���
			//if (fabsf(pos.x) < fabsf(pos.y))
			//{
			//	if (pos.x < 0)
			//		sideType = SIDE_LEFT;
			//	else
			//		sideType = SIDE_RIGHT;
			//}
			//else
			//{
			//	if (pos.y > 0)
			//		sideType = SIDE_TOP;
			//	else
			//		sideType = SIDE_BOTTOM;
			//}
		}
		
		return bret;
	}
	
public:
	float			DeadStayTime;
	bool			IsDead;
	float			DeadTime;
	float			ScreenRadius;
	Vector2			ScreenBoxPoint[8];	//���Χ������Ļ�ϵ�8���㡣
	Vector2			ScreenPos;
	Vector3			ProjScreenPos;
	Vector3			WorldPos;
	Quaternion		WorldRot;
	Matrix			InvWorldMat;	//�ӵ����߼��ʹ��
	PathController  Controller;
	Vector3			FishBoxSize;	//��Χ��ֱ��
	USHORT			GroupID;
	USHORT			FishID;			//���ID
	USHORT			FishType;		//�������
	bool			IsActive;		//����״̬
	float			Speed;			//�ٶ�
	float			FishScaling;	//ģ������
	USHORT			PathGroup;		//����·��Ⱥ(����û��·��Ⱥ)
	USHORT 			PathIndex;		//·��������
	Vector3			Offset;			//ƫ��
	bool			HasAttackAction;
	bool			IsInFrustum;
	BYTE			PackageType;
	map<USHORT, FishHpInfo> fishHp; // �ӵ���ÿ�ֱ��ʶ�Ӧ��Ѫ����Ϣ
};
