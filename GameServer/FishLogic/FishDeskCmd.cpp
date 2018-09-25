#include "stdafx.h"
#include "FishDesk.h"
#include "FishCallbak.h"
#include "NetCmd.h"
#include "FishScene.h"
#include "FishScene\BulletManager.h"
#include "FishScene\FishManager.h"
#include "FishScene\FishCollider.h"
#include "FishScene\FishResData.h"
//#include "FishScene\Crc32.h"
#include "FishScene\DeadStayTimer.h"
#include "..\Role.h"
#include "FishUtility.h"
#include "..\RoleEx.h"
const int  MAX_IMG_SIZE = 1024 * 12; //12k;
#define BULLET_LIFE 3.0f
bool FishDesk::IsEndScene()const
{
	return m_pScene->IsEndScene();
}
bool FishDesk::HandleNetCmd(PlayerID id, NetCmd *pCmd)
{
	//pCmd �ⲿ��������� ��Ӧ�����ڲ�����free
	CRole *player = GetPlayerFromID(id);
	if (player == NULL)
	{
		Log(L"������(%d)���������ID:%ld", m_nDeskIndex, id);
		return false;
	}
	IFishSetting *pSetting = FishCallback::GetFishSetting();
	switch (pCmd->GetCmdType())
	{
	case CMD_SKILL_DOUBLE_FIRE:
	{
		LG_UseGoods* pgoods = (LG_UseGoods*)pCmd;
		player->SetKbEndTime(static_cast<DWORD>(pgoods->keep_time));
		LG_UseGoodsKB msg;
		SetMsgInfo(msg, CMD_SKILL_DOUBLE_FIRE, sizeof(LG_UseGoodsKB));
		msg.seatID = player->GetSeatID();
		msg.keepTime = static_cast<UINT32>(pgoods->keep_time);
		SendAll(&msg);
		break;
	}
	case CMD_SKILL_TIMESTOP:
	{
		LG_UseGoods* pgoods = (LG_UseGoods*)pCmd;
		m_pScene->SetSceneStopEndTime(static_cast<DWORD>(pgoods->keep_time));
		LG_UseGoodsBD msg;
		SetMsgInfo(msg, CMD_SKILL_TIMESTOP, sizeof(LG_UseGoodsBD));
		msg.seatID = player->GetSeatID();
		msg.keepTime = static_cast<UINT32>(pgoods->keep_time);
		SendAll(&msg);
		break;
	}
	case CMD_PING:
		{
			SendTo(player->GetID(), pCmd);
		}
		break;
	case CMD_BULLET:
		{
			
			NetCmdBullet* pBullet = (NetCmdBullet*)pCmd;
			if (pCmd->GetCmdSize() != sizeof(NetCmdBullet))
			{
				return false;
			}
			Vector3 pos;// = BulletManager::GetBulletStartPos(pBullet->StartPosX, pBullet->StartPosY);
			Vector3 dir;// = BulletManager::GetBulletDir(pBullet->DirX, pBullet->DirY);
			GetBulletDirAndPos(pBullet->Angle, player->GetSeatID(), dir, pos);
			if (player->GetRoleExInfo()->IsRobot())
			{
				//�����˷����ӵ� ������ֱ�ӷ���
				//std::cout << "�����˷����ӵ���ʼ\n";
				BYTE bid = 0;
				if (!player->CheckBulletIdx(bid))
				{
					ASSERT(false);
					return false;
				}
				pBullet->BulletID = player->GetSeatID() << 8 | bid;
				pBullet->ReboundCount = player->GetRoleExInfo()->GetRoleVip().GetLauncherReBoundNum();
				if (pBullet->LauncherType == 4)
					pBullet->ReboundCount = 0;
				//pBullet->LockFishID = 0;
				float speed = FishCallback::GetFishSetting()->GetBulletSpeed(player->GetLauncherType());
				m_pScene->GetBulletMgr()->Launch(id, pBullet->BulletID, player->GetLauncherType(), player->GetRateIndex(), speed, pBullet->Angle, dir, pos, pBullet->ReboundCount, pBullet->LockFishID);
				SendAll(pBullet);
				//std::cout << "�����˷����ӵ��ɹ�\n";
			}
			else if (player->CheckFire(pBullet->LauncherType))
			{
				BYTE bid = 0;
				if (!player->CheckBulletIdx(bid))
				{
					ASSERT(false);
					return false;
				}
				pBullet->LauncherType = player->GetLauncherType();
				pBullet->Energy = (uint)player->GetEnergy();
				pBullet->ReboundCount =player->GetRoleExInfo()->GetRoleVip().GetLauncherReBoundNum();
				if (pBullet->LauncherType == 4)
					pBullet->ReboundCount = 0;
				pBullet->BulletID = player->GetSeatID() << 8 | bid;
				float speed = FishCallback::GetFishSetting()->GetBulletSpeed(player->GetLauncherType());
				float extraBulletPower = player->IsInKbState() ? KB_RATIO : 1.0f;
				m_pScene->GetBulletMgr()->Launch(id, pBullet->BulletID, player->GetLauncherType(), player->GetRateIndex(), speed, pBullet->Angle, dir, pos, pBullet->ReboundCount, pBullet->LockFishID, extraBulletPower);
				SendAll(pBullet);
			}
		}
		break;

	case CMD_CHANGE_LAUNCHER:
		{
			//nothing;
			//��֤����Ƿ��д���̨
			NetCmdChangeLauncher *change = (NetCmdChangeLauncher*)pCmd;
			player->AddLauncher(change->LauncherType != 0);

			if (player->GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(player->GetLauncherType()))
				change->LauncherType = byte(128 | player->GetLauncherType());
			else 
				change->LauncherType = byte(player->GetLauncherType());
			change->Seat = player->GetSeatID();
			SendAll(change);
		}
		break;
	case CMD_CHANGE_LAUNCHER_TYPE:
		{
			//nothing;
			//��֤����Ƿ��д���̨
			NetCmdChangeLauncherType *change = (NetCmdChangeLauncherType*)pCmd;
			if (!change || player->GetLauncherType() == change->LauncherType)
				return false;
			player->SetLauncher(change->LauncherType);
			if (player->GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(player->GetLauncherType()))
				change->LauncherType = byte(128 | player->GetLauncherType());
			else
				change->LauncherType = byte(player->GetLauncherType());
			change->Seat = player->GetSeatID();

			NetCmdChangeLauncher msg;
			SetMsgInfo(msg, CMD_CHANGE_LAUNCHER, sizeof(NetCmdChangeLauncher));
			msg.Seat = change->Seat;
			msg.LauncherType = change->LauncherType;
			SendAll(&msg);
		}
		break;
	case CMD_USE_SKILL:
		{
			NetCmdUseSkill *skill = (NetCmdUseSkill*)pCmd;
			SkillFailedType sft = pSetting->UseSkill(player->GetID(), (SkillType)skill->SkillID);
			if (sft == SFT_OK)
			{
				switch (skill->SkillID)
				{
				case SKILL_TORNADO:
					ProcessSkillTornado(player);
					break;
				case SKILL_FREEZE:
					ProcessSkillFreeze(player);
					break;
				case SKILL_LIGHTING:
					ProcessSkillLighting(player);
					break;
				case SKILL_DISASTER:
					ProcessSkillDisaster(player);
					break;
				case SKILL_LOCK:
					ProcessSkillLock(player);
					break;
				}
			}
			else
			{
				NetCmdSkillFailed nc ;
				nc.SetCmdSize(sizeof(nc));
				nc.SetCmdType(CMD_SKILL_FAILLED);
				nc.FailedType = static_cast<BYTE>(sft);
				nc.bySkillType = skill->SkillID;
				nc.uTimes = player->SkillInfo((SkillType)skill->SkillID);
				SendTo(player->GetID(), &nc);
			}
		}
		break;
	case CMD_SKILL_LASER_REQUEST:
		{
			LaunchFailedType lft = pSetting->UseLaser(player->GetID(), player->GetLauncherType());
			if (lft == LFT_OK)
			{
				ProcessSkillLaser((NetCmdSkillLaser*)pCmd, player);
			}
			else
			{
				NetCmdLaunchFailed nc;
				nc.SetCmdSize(sizeof(nc));
				nc.SetCmdType(CMD_LAUNCH_LASER_FAILED);
				nc.FailedType = static_cast<BYTE>(lft);
				nc.Energy = player->GetEnergy();
				SendTo(player->GetID(), &nc);
			}
		}
		break;
	case CMD_CHECK_FISH_POS:
		CheckFishPos((NetCmdCheckFishPos*)pCmd);
		break;
	case CMD_PLAYER_READY:
		break;
	case CMD_CHANGE_CLIENT_RATE:
		{
			//�ͻ��˷�����������
			NetCmdClientChangeRate* pMsg = (NetCmdClientChangeRate*)pCmd;
			bool IsCanUse = true;
			player->OnChangeRateToIndex(pMsg->UpRoDownRate, IsCanUse);//������ҵ�ǰ�ı���

			NetCmdChangeRate msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE, sizeof(NetCmdChangeRate));
			msg.Seat = player->GetSeatID();
			msg.IsCanUseRate = IsCanUse;
			msg.RateIndex = player->GetRateIndex();
			SendAll(&msg);
		}
		break;
	//case CMD_CHANGE_RATE:
	//	{
	//		NetCmdChangeRate *rate = (NetCmdChangeRate*)pCmd;
	//		memcpy_s(rate, pCmd->CmdSize, pCmd, pCmd->CmdSize);
	//		//player->ChangeRateIndex(rate->RateIndex);
	//		bool IsCanUse = true;
	//		player->OnChangeRateToIndex(rate->RateIndex, IsCanUse);//������ҵ�ǰ�ı���
	//		rate->RateIndex = player->GetRateIndex();
	//		rate->Seat = player->GetSeatID();
	//		rate->IsCanUseRate = IsCanUse;//�Ƿ����ʹ�õ�ǰ�ı���
	//		SendAll(rate);
	//	}
	//	break;
	case CMD_CHANGE_RATE_TYPE:
		{
			NetCmdChangeRateType *rate = (NetCmdChangeRateType*)pCmd;
			memcpy_s(rate, pCmd->GetCmdSize(), pCmd, pCmd->GetCmdSize());
			//player->SetRateIndex(rate->RateIndex);

			bool IsCanUse = true;
			player->OnChangeRateToIndex(rate->RateIndex, IsCanUse);//������ҵ�ǰ�ı���

			rate->RateIndex = player->GetRateIndex();
			rate->Seat = player->GetSeatID();

			NetCmdChangeRate msg;
			SetMsgInfo(msg, CMD_CHANGE_RATE, sizeof(NetCmdChangeRate));
			msg.Seat = rate->Seat;
			msg.IsCanUseRate = IsCanUse;
			msg.RateIndex = rate->RateIndex;
			SendAll(&msg);
		}
		break;
	case CMD_CHECK_BULLET_POS:
		CheckClientBulletPos((NetCmdCheckBulletPos*)pCmd);
		break;
	/*case CMD_UPLOADING_REQUEST:
		{
			NetCmdUploadImgRequest *pImg = (NetCmdUploadImgRequest*)pCmd;
			player->OnBeginUpLoadFaceData(pImg->Size);
			return true;
		}
		break;
	case CMD_UPLOADING_CHUNK:
		{
			NetCmdUploadImgChunk *pImg = (NetCmdUploadImgChunk*)pCmd;
			player->OnUpLoadFaceData(pImg->StartIndex, pImg->Size, pImg->ImgData);
			return true;
		}
		break;*/
	default:
		{
			Log(L"δ֪������:%d, ����:%d, ���ID:%ld", pCmd->GetCmdType(), m_nDeskIndex, id);
		}
		break;
	}
	return true;
}
void FishDesk::SendGlobalSetting(CRole *player)
{
	return;
}

void FishDesk::AsyncPlayerJoin(CRole* player, bool bAsyncScene)
{
	//ȫ������ͬ����ȥ
	SendGlobalSetting(player);

	////ͬ�����������
	//NetCmdPlayerJoin ncp;
	//SetMsgInfo(ncp,CMD_PLAYER_JOIN, sizeof(NetCmdPlayerJoin));
	//ncp.SetCmdType(CMD_PLAYER_JOIN);
	//if (bAsyncScene == false)
	//{
	//	ncp.PlayerInfo.ID = player->GetID();
	//	ncp.PlayerInfo.GoldNum = 0;
	//	ncp.PlayerInfo.ImgCrc = player->GetFaceID();
	//	ncp.PlayerInfo.Lvl = 0;
	//	ncp.PlayerInfo.Sex = 0;
	//	ncp.Seat = player->GetSeatID();
	//	ncp.rateIndex = player->GetRateIndex();
	//	ncp.LauncherType = (128 | player->GetLauncherType());
	//	SendFrom(player->GetID(), &ncp);
	//}

	////���������ͬ�����Լ�
	//for (BYTE i = 0; i < m_PlayerManager->GetMaxPlayerSum()/*m_nMaxSeatNum*/; ++i)
	//{
	//	CRole* pRole = m_PlayerManager->GetRoleBySeatID(i);
	//	if (pRole && pRole->IsActionUser() && pRole->GetID() != player->GetID())
	//	{
	//		ncp.Seat = i;
	//		ncp.PlayerInfo.ID = pRole->GetID();
	//		ncp.LauncherType = byte(128 | pRole->GetLauncherType());
	//		ncp.PlayerInfo.ImgCrc = pRole->GetFaceID();	
	//		ncp.rateIndex = pRole->GetRateIndex();
	//		SendTo(player->GetID(), &ncp);
	//	}
	//}
	SendFishToClient(player);
}
void FishDesk::SendFishToClient(CRole* player)
{
	//ͬ�������е���
	const float DELAY_TIME = 0.005f; //���������ӳٵ�ʱ��
	FishMapList *pFishMap = m_pScene->GetFishMgr()->GetFishMapList();
	if (pFishMap->size() > 0)
	{
		for (FishMapIt it = pFishMap->begin(); it != pFishMap->end(); ++it)
		{
			NetCmdSyncFish fishData;
			SetMsgInfo(fishData,CMD_SYNC_FISH, sizeof(NetCmdSyncFish));
			Fish *pFish = it->second;
			fishData.FishID = pFish->FishID;
			fishData.GroupID = pFish->GroupID;
			fishData.FishTime = pFish->GetTime();
			fishData.PathGroup = pFish->PathGroup;
			fishData.PathIdx =  pFish->PathIndex;
			fishData.IsActiveEvent = pFish->Controller.IsActiveEvent();
			fishData.ElapsedTime = ushort((pFish->Controller.EventElapsedTime() + DELAY_TIME) * 1000);
			fishData.Package = pFish->GetPackage();
			//����Ч��
			{
				fishData.DelayType = (byte)pFish->Controller.GetDelayData().DelayType;
				fishData.DelayCurrentTime = ushort((pFish->Controller.GetDelayData().CurrentTime + DELAY_TIME) * 1000);
				fishData.DelayDuration1 = byte(pFish->Controller.GetDelayData().Duration[0] / FISH_DURATION_TIME + 0.5f);
				fishData.DelayDuration2 = byte(pFish->Controller.GetDelayData().Duration[1] / FISH_DURATION_TIME + 0.5f);
				fishData.DelayDuration3 = byte(pFish->Controller.GetDelayData().Duration[2] / FISH_DURATION_TIME + 0.5f);
				fishData.DelayScaling = byte(pFish->Controller.GetDelayData().Scaling * 255);
			}
			if (player)
				SendTo(player->GetID(), &fishData);
			else
				SendAll(&fishData);
			//Log(L"�ֽڴ�С:%d" + size);
		}
	}
	BulletVecList * pBulletMap = m_pScene->GetBulletMgr()->GetBulletMapList();
	if (pBulletMap->size() > 0)
	{
		UINT count = pBulletMap->size();
		uint size = sizeof(NetCmdSyncBullet)+(count - 1) * sizeof(tagSyncBulletData);
		CheckMsgSize(size);
		NetCmdSyncBullet *pCmd = (NetCmdSyncBullet*)malloc(size);
		pCmd->SetCmdSize(ConvertDWORDToWORD(size));
		pCmd->SetCmdType(CMD_SYNC_BULLET);

		for (UINT i = 0; i < count; ++i)
		{
			Bullet *pBullet = pBulletMap->operator[](i);
			pCmd->Bullets[i].BulletID = pBullet->BulletID;
			pCmd->Bullets[i].Degree = pBullet->Degree;
			pCmd->Bullets[i].Time = ushort(pBullet->Time * 1000);
			pCmd->Bullets[i].BulletType = (byte)pBullet->BulletType;
			pCmd->Bullets[i].RateIdx = pBullet->RateIndex;
			pCmd->Bullets[i].ReboundCount = pBullet->ReboundCount;
			pCmd->Bullets[i].LockFishID = pBullet->LockFishID;
			//pCmd->Bullets[i].CollideCount = pBullet->CollideFrameCount;
			//pCmd->Bullets[i].PauseTime = ushort(pBullet->PauseTime * 1000);
			//pCmd->Bullets[i].SpeedScaling = ushort(pBullet->SpeedScaling * 1000);

		}
		if (player)
			SendTo(player->GetID(), pCmd);
		else
			SendAll(pCmd);
		free(pCmd);
	}
}
void FishDesk::CheckFishPos(NetCmdCheckFishPos *pos)
{
	return;
}

void FishDesk::ProcessSkillTornado(CRole *player)
{
	m_pScene->GetFishMgr()->RandVecList();
	FishVecList*pFishMap = m_pScene->GetFishMgr()->GetFishVecList();
	/*if (pFishMap->size() == 0)
	return;*/

	int numMin, numMax;
	FishCallback::GetFishSetting()->GetSkillRange(SKILL_TORNADO, numMin, numMax);

	int count = RandRange(numMin, numMax);
	int size = sizeof(NetCmdSkillTornado)+(count - 1) * sizeof(FishDeadTime);
	CheckMsgSize(size);
	NetCmdSkillTornado *pSkill = (NetCmdSkillTornado*)malloc(size);
	if (!pSkill)
	{
		ASSERT(false);
		return;
	}
	pSkill->Seat = player->GetSeatID();
	int validCount = 0;
	UINT goldNum = 0;
	for (size_t i = 0; i < pFishMap->size();)
	{
		Fish *pFish = pFishMap->operator[](i);
		float change = FishCallback::GetFishSetting()->GetSkillChance(SKILL_TORNADO, pFish->FishType, pFish->PackageType);
		//������޷�����������
		if (FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType) || !pFish->IsInFrustum || RandFloat() > change)
		{
			++i;
			continue;
		}
		pFish->Controller.StopLaugh();
		goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, CATCH_SKILL, SKILL_TORNADO, pFish->GetPackage(), player->GetRateIndex());
		pSkill->FishID[validCount].fishID = pFish->FishID;
		pSkill->FishID[validCount].nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(), pFish->FishType);
		m_pScene->GetFishMgr()->RemoveFishImmediate(pFish->FishID);
		ListRemoveAt(*pFishMap, i);

		if (++validCount >= count)
			break;
	}
	pSkill->GoldNum = goldNum;
	pSkill->TotalNum = player->GetRoleExInfo()->GetRoleInfo().money2;
	size = (validCount - 1) * sizeof(FishDeadTime)+sizeof(NetCmdSkillTornado);
	pSkill->SetCmdSize(ConvertIntToWORD(size));
	pSkill->SetCmdType(CMD_SKILL_TORNADO_RESPONSE);
	SendAll(pSkill);
	free(pSkill);
}
static float LIGHTING_DEAD_TIME[3] = { 0, 2, 0 };
typedef map<ushort, FishDeadTime> CatchFishMap;
int CatchLightingFish(FishManager * pMgr, CatchFishMap &fm, Fish *pCatchFish, FishVecList* pFishMapList, CRole *player, UINT &goldNum, int& num, BYTE catchType, BYTE skillType)
{
	
	for (UINT i = 0; i < pFishMapList->size() && num > 0;)
	{
		Fish *pFish = pFishMapList->operator[](i);
		float dist = Vec2Length(pCatchFish->ScreenPos - pFish->ScreenPos);
		if (FishCallback::GetFishSetting()->CanCatchLightingFish(pFish->FishType, player->GetID(), dist) == false)
		{
			++i;
			continue;
		}
		
		goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, (CatchType)catchType, skillType, pFish->GetPackage(), player->GetRateIndex());
		FishDeadTime fc;
		fc.deadTime = 2;
		fc.nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(), pFish->FishType);
		fc.lightingFishID = pCatchFish->FishID;
		fc.fishID = pFish->FishID;
		fm[fc.fishID] = fc;
		ListRemoveAt(*pFishMapList, i);
		pMgr->RemoveFish(pFish->FishID, 0, LIGHTING_DEAD_TIME);
		pFish->Controller.StopLaugh();
		--num;
	}
	return num;
}
void FishDesk::ProcessSkillFreeze(CRole *player)
{
	m_pScene->GetFishMgr()->RandVecList();
	FishVecList*pFishMap = m_pScene->GetFishMgr()->GetFishVecList();

	int numMin, numMax;
	byte speedScl, duration1, duration2, duration3;
	FishCallback::GetFishSetting()->GetSkillRange(SKILL_FREEZE, numMin, numMax);
	FishCallback::GetFishSetting()->GetSkillFreezeReduction(speedScl, duration1, duration2, duration3);

	float fSpdScl;
	float fDurationList[3];
	ReductionToFloat(speedScl, duration1, duration2, duration3, fSpdScl, fDurationList);
	int count = RandRange(numMin, numMax);
	count = min(count, 64);//���64������Ӱ��
	
	int validCount = 0;
	float speed;
	float deadTime[3];
	UINT goldNum = 0;
	CatchFishMap fishList;
	for (size_t i = 0; i < pFishMap->size();)
	{
		Fish *pFish = pFishMap->operator[](i);
		if (FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType) || fishList.find(pFish->FishID) != fishList.end())
		{
			++i;
			continue;
		}
		FishDeadTime fdt;
		fdt.fishID = pFish->FishID;
		fdt.lightingFishID = 0;
		fdt.nReward = 0;
		fdt.deadTime = 0;
		
		float change = FishCallback::GetFishSetting()->GetSkillChance(SKILL_FREEZE, pFish->FishType, pFish->PackageType);
		if (pFish->IsInFrustum && RandFloat() < change)	//��������
		{
			goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, CATCH_SKILL, SKILL_FREEZE, pFish->GetPackage(), player->GetRateIndex());
			byte bt = DeadStayTimer::GetSkillDeadTime(SKILL_FREEZE, speed, deadTime);			
			int lightNum = FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType, player->GetID());
			if (lightNum > 0)
			{
				//������
				memcpy(deadTime, LIGHTING_DEAD_TIME, sizeof(deadTime));
			}
			m_pScene->GetFishMgr()->RemoveFish(pFish->FishID, speed, deadTime);
			ListRemoveAt(*pFishMap, i);
			fdt.deadTime = bt;
			fdt.nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(),pFish->FishType);
			pFish->Controller.StopLaugh();
			if (lightNum > 0)
			{
				//����������
				int xx = CatchLightingFish(m_pScene->GetFishMgr(), fishList, pFish, pFishMap, player, goldNum, lightNum, CATCH_SKILL, SKILL_FREEZE);
				if (xx != lightNum)
					i = 0;
			}
		}
		else
		{
			++i;
			if (pFish->Controller.IsActiveEvent() == false)
				pFish->Controller.AddSkillTimeScaling(fSpdScl, fDurationList, DELAY_SKILL);
		}
		fishList.insert(make_pair(fdt.fishID, fdt));
		if (++validCount >= count)
			break;
	}
	count = (int)fishList.size();
	int size = sizeof(NetCmdSkillFreeze)+(count - 1) * sizeof(FishDeadTime);
	CheckMsgSize(size);
	NetCmdSkillFreeze *pSkill = (NetCmdSkillFreeze*)malloc(size);
	if (!pSkill)
	{
		ASSERT(false);
		return;
	}
	int kk = 0;
	for (auto it = fishList.begin(); it != fishList.end(); ++it)
		pSkill->FishID[kk++] = it->second;
	pSkill->Seat = player->GetSeatID();
	pSkill->GoldNum = goldNum;
	pSkill->TotalNum = player->GetRoleExInfo()->GetRoleInfo().money2;
	pSkill->SetCmdSize(ConvertIntToWORD(size));
	pSkill->SetCmdType(CMD_SKILL_FREEZE_RESPONSE);
	SendAll(pSkill);
	free(pSkill);
}
void FishDesk::ProcessSkillLighting(CRole *player)
{
	m_pScene->GetFishMgr()->RandVecList();
	FishVecList*pFishMap = m_pScene->GetFishMgr()->GetFishVecList();
	/*if (pFishMap->size() == 0)
	return;*/

	int numMin, numMax;
	FishCallback::GetFishSetting()->GetSkillRange(SKILL_LIGHTING, numMin, numMax);

	int count = RandRange(numMin, numMax);
	
	int validCount = 0;
	float deadTime[3];
	float speedScl;
	UINT goldNum = 0;
	CatchFishMap fishList;
	for (size_t i = 0; i < pFishMap->size();)
	{
		Fish *pFish = pFishMap->operator[](i);
		FishDeadTime fdt;
		float change = FishCallback::GetFishSetting()->GetSkillChance(SKILL_LIGHTING, pFish->FishType, pFish->PackageType);
		if (FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType) || !pFish->IsInFrustum || RandFloat() > change)
		{
			++i;
			continue;
		}
		pFish->Controller.StopLaugh();
		byte bt = DeadStayTimer::GetSkillDeadTime(SKILL_LIGHTING, speedScl, deadTime);
		goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, CATCH_SKILL, SKILL_LIGHTING, pFish->GetPackage(), player->GetRateIndex());
		fdt.fishID = pFish->FishID;
		fdt.deadTime = bt;
		fdt.lightingFishID = 0;
		fdt.nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(), pFish->FishType);
		fishList.insert(make_pair(fdt.fishID, fdt));
		int lightNum = FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType, player->GetID());
		if (lightNum > 0)
		{
			memcpy(deadTime, LIGHTING_DEAD_TIME, sizeof(deadTime));
		}
		m_pScene->GetFishMgr()->RemoveFish(pFish->FishID, speedScl, deadTime);
		ListRemoveAt(*pFishMap, i);
		if (lightNum > 0)
		{
			int xx = CatchLightingFish(m_pScene->GetFishMgr(), fishList, pFish, pFishMap, player, goldNum, lightNum, CATCH_SKILL, SKILL_LIGHTING);
			if (xx != lightNum)
				i = 0;
		}
		if (++validCount >= count)
			break;
	}
	count = (int)fishList.size();
	int size = sizeof(NetCmdSkillLighting)+(count - 1) * sizeof(FishDeadTime);
	CheckMsgSize(size);
	NetCmdSkillLighting *pSkill = (NetCmdSkillLighting*)malloc(size);
	if (!pSkill)
	{
		ASSERT(false);
		return;
	}
	int kk = 0;
	for (auto it = fishList.begin(); it != fishList.end(); ++it)
		pSkill->FishID[kk++] = it->second;
	pSkill->Seat = player->GetSeatID();
	pSkill->GoldNum = goldNum;
	pSkill->TotalNum = player->GetRoleExInfo()->GetRoleInfo().money2;
	pSkill->SetCmdSize(ConvertIntToWORD(size));
	pSkill->SetCmdType(CMD_SKILL_LIGHTING_RESPONSE);
	SendAll(pSkill);
	free(pSkill);
}

void FishDesk::ProcessSkillLaser(NetCmdSkillLaser* pCmd, CRole *player)
{
	Vector3 pos;
	Vector3 dir;
	GetBulletDirAndPos(pCmd->Degree, player->GetSeatID(), dir, pos);
	vector<FishInfo> fishList;
	FishCollider::CollideByDir(fishList, player->GetLauncherType(), pCmd, pos, dir, m_pScene->GetFishMgr());

	
	float deadTime[3];
	float speedScl;
	UINT goldNum = 0;
	FishVecList*pFishMap = m_pScene->GetFishMgr()->GetFishVecList();
	CatchFishMap catchFishList;
	for (UINT i = 0; i < fishList.size(); ++i)
	{
		Fish *pFish = fishList[i].pFish;
		pFish->Controller.StopLaugh();
		goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, CATCH_LASER, player->GetLauncherType(), pFish->GetPackage(), player->GetRateIndex());
		byte bt = DeadStayTimer::GetLaserDeadTime(player->GetLauncherType(), speedScl, deadTime);
		FishDeadTime fdt;
		fdt.fishID = pFish->FishID;
		fdt.deadTime = bt;
		fdt.nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(), pFish->FishType);
		fdt.lightingFishID = 0;
		catchFishList.insert(make_pair(fdt.fishID, fdt));

		int lightNum = FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType, player->GetID());
		
		if (lightNum > 0)
		{
			memcpy(deadTime, LIGHTING_DEAD_TIME, sizeof(deadTime));
		}
		m_pScene->GetFishMgr()->RemoveFish(pFish->FishID, speedScl, deadTime);
		if (lightNum > 0)
		{
			int xx = CatchLightingFish(m_pScene->GetFishMgr(), catchFishList, pFish, pFishMap, player, goldNum, lightNum, CATCH_LASER, player->GetLauncherType());
		}
	}
	int fishcount = (int)catchFishList.size();
	int size = sizeof(FishDeadTime)* (fishcount - 1) + sizeof(NetCmdSkillLaserResponse);
	CheckMsgSize(size);
	NetCmdSkillLaserResponse *cmd = (NetCmdSkillLaserResponse*)malloc(size);
	if (!cmd)
		return;
	int kk = 0;
	for (auto it = catchFishList.begin(); it != catchFishList.end(); ++it)
		cmd->FishID[kk++] = it->second;
	cmd->SetCmdType(CMD_SKILL_LASER_RESPONSE);
	cmd->SetCmdSize(ConvertIntToWORD(size));
	cmd->Seat = player->GetSeatID();
	cmd->LaserType = player->GetLauncherType();
	cmd->Degree = pCmd->Degree;
	cmd->GoldNum = goldNum;
	cmd->TotalNum = player->GetRoleExInfo()->GetRoleInfo().money2;
	SendAll(cmd);
	free(cmd);


	FishVecList *pFishList = m_pScene->GetFishMgr()->GetFishVecList();
	int count = pFishList->size();
	if (count > 0)
	{
		byte scal;
		byte d1, d2, d3;
		FishCallback::GetFishSetting()->GetLaserReduction(player->GetLauncherType(), scal, d1, d2, d3);
		int size = (count - 1) * sizeof(FISHID)+sizeof(NetCmdReduction);
		CheckMsgSize(size);
		NetCmdReduction *pRed = (NetCmdReduction*)malloc(size);
		if (!pRed)
		{
			ASSERT(false);
			return;
		}
		pRed->SetCmdType(CMD_REDUCTION);
		pRed->SetCmdSize(ConvertIntToWORD(size));
		pRed->ReductionType = 1;
		pRed->LaserType = player->GetLauncherType();

		float fSpdScl;
		float fDurationList[3];
		ReductionToFloat(scal, d1, d2, d3, fSpdScl, fDurationList);
		int validCount = 0;
		for (int i = 0; i < count; ++i)
		{
			Fish *pFish = pFishList->operator[](i);
			if (pFish->Controller.IsActiveEvent())
				continue;

			pFish->Controller.AddSkillTimeScaling(fSpdScl, fDurationList, DELAY_SKILL);
			pRed->FishID[validCount++] = pFish->FishID;
		}
		if (validCount > 0)
		{
			pRed->SetCmdSize(ConvertDWORDToWORD((validCount - 1) * sizeof(FISHID)+sizeof(NetCmdReduction)));
			SendAll(pRed);
		}
		::free(pRed);
	}
}

void FishDesk::ProcessSkillDisaster(CRole *player)
{
	m_pScene->GetFishMgr()->RandVecList();
	FishVecList*pFishMap = m_pScene->GetFishMgr()->GetFishVecList();
	/*if (pFishMap->size() == 0)
	return;*/

	int numMin, numMax;
	FishCallback::GetFishSetting()->GetSkillRange(SKILL_DISASTER, numMin, numMax);

	int count = RandRange(numMin, numMax);
	
	int validCount = 0;
	float deadTime[3];
	float speedScl;
	UINT goldNum = 0;
	CatchFishMap fishList;
	for (size_t i = 0; i < pFishMap->size();)
	{
		Fish *pFish = pFishMap->operator[](i);
		float change = FishCallback::GetFishSetting()->GetSkillChance(SKILL_DISASTER, pFish->FishType, pFish->PackageType);
		if (FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType) || !pFish->IsInFrustum || RandFloat() > change)
		{
			++i;
			continue;
		}
		pFish->Controller.StopLaugh();
		goldNum += FishCallback::GetFishSetting()->CatchFish(player->GetID(), pFish->FishType, CATCH_SKILL, SKILL_DISASTER, pFish->GetPackage(), player->GetRateIndex());
		byte bt = DeadStayTimer::GetSkillDeadTime(SKILL_DISASTER, speedScl, deadTime);
		FishDeadTime fdt;
		fdt.fishID = pFish->FishID;
		fdt.deadTime = bt;
		fdt.nReward = FishCallback::GetFishSetting()->FishRewardDrop(player->GetID(), pFish->GetPackage(), pFish->FishType);
		fdt.lightingFishID = 0;
		int lightNum = FishCallback::GetFishSetting()->IsLightingFish(pFish->FishType, player->GetID());
		if (lightNum > 0)
		{
		}
		m_pScene->GetFishMgr()->RemoveFish(pFish->FishID, speedScl, deadTime);
		ListRemoveAt(*pFishMap, i);
		fishList.insert(make_pair(fdt.fishID, fdt));
		if (lightNum > 0)
		{
			CatchLightingFish(m_pScene->GetFishMgr(), fishList, pFish, pFishMap, player, goldNum, lightNum, CATCH_SKILL, SKILL_DISASTER);
		}
		if (++validCount >= count)
			break;
	}
	count = (int)fishList.size();
	int size = sizeof(NetCmdSkillDisaster)+(count - 1) * sizeof(FishDeadTime);
	CheckMsgSize(size);
	NetCmdSkillDisaster *pSkill = (NetCmdSkillDisaster*)malloc(size);

	int kk = 0;
	for (auto it = fishList.begin(); it != fishList.end(); ++it)
		pSkill->FishID[kk++] = it->second;

	pSkill->Seat = player->GetSeatID();
	pSkill->GoldNum = goldNum;
	pSkill->TotalNum = player->GetRoleExInfo()->GetRoleInfo().money2;
	size = (validCount - 1) * sizeof(FishDeadTime)+sizeof(NetCmdSkillDisaster);
	pSkill->SetCmdSize(ConvertIntToWORD(size));
	pSkill->SetCmdType(CMD_SKILL_DISASTER_RESPONSE);
	SendAll(pSkill);
	free(pSkill);
}
void FishDesk::ProcessSkillLock(CRole *player)
{
	//��ǰ���ʹ����������
	if (!player)
	{
		ASSERT(false);
		return;
	}
	if (!player->IsCanLock())
		return;
	//�����������
	player->SetLockEndTime();
	//��������ͻ���ȥ
	NetCmdSkillLock msg;
	SetMsgInfo(msg, CMD_SKILL_LOCK_RESPONSE, sizeof(NetCmdSkillLock));
	msg.Seat = player->GetSeatID();
	SendAll(&msg);
}
void FishDesk::CheckClientBulletPos(NetCmdCheckBulletPos *pos)
{
	for (int i = 0; i < pos->Count; ++i)
	{
		Bullet* pBullet = m_pScene->GetBulletMgr()->FindBullet(pos->Bullets[i].id);
		if (pBullet == NULL)
		{
			Log(L"�����ڵ��ӵ�:%d", pos->Bullets[i].id);
			continue;
		}
		float dist = Length(pBullet->Pos - pos->Bullets[i].pos);
		if (dist > 0.5f)
		{
			Log(L"�ӵ�λ�ò���� :%f,server: %f, %f, %f, client: %f, %f, %f",
				dist,
				pBullet->Pos.x,
				pBullet->Pos.y,
				pBullet->Pos.z,
				pos->Bullets[i].pos.x,
				pos->Bullets[i].pos.y,
				pos->Bullets[i].pos.z);
		}
	}

}
void FishDesk::ResetLauncher(PlayerID id)
{
	CRole* player = GetPlayerFromID(id);
	if (!player)
	{
		//ASSERT(false);
		return;
	}
	NetCmdChangeLauncher msg;
	SetMsgInfo(msg, CMD_CHANGE_LAUNCHER, sizeof(NetCmdChangeLauncher));
	if (player->GetRoleExInfo()->GetRoleLauncherManager().IsCanUserLauncherByID(player->GetLauncherType()))
		msg.LauncherType = byte(128 | player->GetLauncherType());
	else
		msg.LauncherType = byte(player->GetLauncherType());
	msg.Seat = player->GetSeatID();
	SendAll(&msg);
}
WORD FishDesk::GetAngleByFish(WORD& LoackFishID, BYTE SeatID, BYTE& FishValue)
{
	BulletStartPosData & pd = RuntimeInfo::BulletCenterList[SeatID];
	Vector3 pPos = pd.Pos;
	pPos.z = RuntimeInfo::NearLeftTopPoint.z;
	Vector3 pCenter = pd.Center;
	pCenter.z = RuntimeInfo::NearLeftTopPoint.z;
	return m_pScene->GetAngleByFish(LoackFishID, SeatID, ProjectionToViewport(WorldToProjection(pCenter)), FishValue);
}
DWORD FishDesk::GetFishSum()
{
	return m_pScene->GetFishMgr()->GetFishCount();
}