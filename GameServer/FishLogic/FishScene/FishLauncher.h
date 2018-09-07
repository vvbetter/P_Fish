#pragma once
#include "RuntimeInfo.h"
#include "FishResData.h"
#include "PathLinearInterpolator.h"
#include "FishResManager.h"
#include "Fish.h"
#include "FishManager.h"
#include "FishSendInterface.h"
#include <list>
#include "../NetCmd.h"
using namespace std;
#define MAX_PATH_TIME  10000 //10 * 1000 //10��
#define SAMPLE_TIME  0.01666f
#define START_FISH_ID 1
struct FlowTimeInfo
{
	int		Count;
	float	LastTime;
	bool	Clear;
};
struct LaunchPathTime
{
	LaunchPathTime(int path)
	{
		Tick = GetTickCount();
		PathIndex = path;
	}
	UINT	Tick;
	int		PathIndex;
};
struct RepeatLaunchGroup
{
	ushort		m_PathGroupRepeat;
	ushort		m_PathGroupInterval;
	UINT		m_LastPathGroupLaunchTick;
	UINT		m_PathGroupID;
};
class FishLauncher
{
public:
	FishSendInterface *m_pSendAll;
	FlowData* m_FishFlow;
	FishManager *m_pFishMgr;
	vector<int> m_FlowGroupCountList;
	vector<FlowTimeInfo>	m_FlowTimeCountList;
	HashMap<int, LaunchPathTime>	m_PathIndexList;  //�Ѿ������·���б�,�����ظ����ֵĻ���
	USHORT  m_FishID;
	BYTE m_TableTypeID;
	int     m_MaxProbability;
	float   m_Time;
	int     m_LoopCount;
	float   m_LauncherTime;
	bool    m_bActive;
	bool    m_bClearState;//�����峡״̬
	vector<RepeatLaunchGroup> m_RepeatLaunchList;
	FishLauncher(FishManager *pMgr)
	{
		m_FishID = START_FISH_ID;
		m_FishFlow = NULL;
		m_pFishMgr = pMgr;
	}
	bool IsActive()
	{
		return m_bActive;
	}
	float GetFlowTime()
	{
		return m_Time;
	}
	void Shutdown()
	{
	}
	bool Init(FlowData *pFlow, FishSendInterface *pSend, BYTE TableTypeID)
	{
		m_pSendAll = pSend;
		m_FishFlow = pFlow;
		m_TableTypeID = TableTypeID;
		if (m_FishFlow != NULL)
			InitFlowData(true);
		return true;
	}
	bool Init(FlowData *pFlow)
	{
		m_FishFlow = pFlow;
		if (m_FishFlow != NULL)
			InitFlowData(true);
		return true;
	}
	void InitFlowData(bool bResetLancherTime)
	{
		if (bResetLancherTime)
		{
			if (m_FishFlow)
				m_LauncherTime = m_FishFlow->LauncherInterval;
			else
				m_LauncherTime = 0;
		}
		m_RepeatLaunchList.clear();
		m_PathIndexList.clear();
		m_bClearState = false;
		m_LoopCount = 0;
		m_Time = 0;
		m_bActive = true;
		m_FlowGroupCountList.clear();
		m_FlowTimeCountList.clear();
		if (m_FishFlow == NULL)
			return;
		
		for (int i = 0; i < m_FishFlow->GroupCount; ++i)
		{
			FlowGroupData &fg = m_FishFlow->pFlowGroupList[i];
			if (fg.MaxCount != -1)
				m_FlowGroupCountList.push_back(fg.MaxCount);
			else
				m_FlowGroupCountList.push_back(1);
		}
		for (int i = 0; i < m_FishFlow->FlowTimeCount; ++i)
		{
			FlowTimeData &ftd = m_FishFlow->pFlowTimeList[i];
			FlowTimeInfo fti;
			if (ftd.ClearAll)
				fti.Count = 1;
			else
				fti.Count = ftd.MaxCount;
			fti.LastTime = 1000000;
			m_FlowTimeCountList.push_back(fti);
		}
	}
	int GetPathIndex(int groupIndex, GroupData *pGroup, bool bforceValid)
	{
		//��ֹ�����ظ���·��
		int count = 0;
		int pathIndex;
		int *pathList = new int[pGroup->PathCount];
		for (int i = 0; i < pGroup->PathCount; ++i)
		{
			if (m_PathIndexList.find(pGroup->pPathList[i]) == m_PathIndexList.end())
				pathList[count++] = pGroup->pPathList[i];
		}
		if (count == 0)
		{
			if (bforceValid == false)
				return -1;
			//Log(L"δ�ҵ��ʺϵ�·��,·�������ظ����֣���ȺID:%d", groupIndex);
			pathIndex = pGroup->pPathList[0];
		}
		else
		{
			pathIndex = pathList[RandRange(0, count)];
			m_PathIndexList.insert(make_pair(pathIndex, LaunchPathTime(pathIndex)));
		}
		SAFE_DELETE_ARR(pathList);
		return pathIndex;
	}
	static float GetPathTimeByDist(float startX, float curX, int pathIdx)
	{
		PathLinearInterpolator pi;
		pi.InitFromPathData(FishResManager::Inst()->GetPath(pathIdx));
		float dist = startX - curX;
		float dist2 = pi.GetPos(0).x - pi.GetPos(SAMPLE_TIME).x;
		return dist / dist2 * SAMPLE_TIME;
	}
	UINT LaunchFish(int groupID, GroupData *pathgroup, UINT packageType)	//bforcelaunch·�����ظ�ʱ��Ҳǿ�Ʒ�����Ⱥ
	{
		PathGroupData *pPathGroup = FishResManager::Inst()->GetPathGroup(pathgroup->PathGroupIndex);
		int launchIdx = -1;
		UINT packageFishID = 0xffffffff;
		if (packageType != -1)
		{
			launchIdx = RandRange(0, pPathGroup->PathCount);
		}
		for (int i = 0; i < pPathGroup->PathCount; ++i)
		{
			Fish *pFish = new Fish;
			pFish->InitFishData(ConvertIntToWORD(groupID), m_FishID, pathgroup->FishIndex, pathgroup->PathGroupIndex, ConvertIntToWORD(i), pathgroup->FishScaling, pathgroup->Speed, Vector3(0, 0, 0), 0, &pPathGroup->pPathList[i], m_TableTypeID);
			if (launchIdx == i)
			{
				packageFishID = m_FishID;
			}

			if (++m_FishID == MAX_FISH_NUM)
				m_FishID = START_FISH_ID;

			if (!m_pFishMgr->AddFish(pFish))
			{
				delete pFish;
				return 0xffffffff;
			}
		}
		if (packageType == -1 && pathgroup->RepeatNum > 0)
		{
			RepeatLaunchGroup plg;
			plg.m_LastPathGroupLaunchTick = GetTickCount();
			plg.m_PathGroupID = groupID;
			plg.m_PathGroupRepeat = pathgroup->RepeatNum;
			plg.m_PathGroupInterval = pathgroup->Interval;
			m_RepeatLaunchList.push_back(plg);
		}
		return packageFishID;
	}

	void LaunchPathGroupRepeat()
	{
		if (m_RepeatLaunchList.size() == 0)
			return ;
		for (UINT j = 0; j < m_RepeatLaunchList.size();)
		{
			RepeatLaunchGroup &plg = m_RepeatLaunchList[j];
			if (GetTickCount() - plg.m_LastPathGroupLaunchTick < plg.m_PathGroupInterval)
			{
				++j;
				continue;
			}

			USHORT startID = m_FishID;
			GroupData *pathgroup = FishResManager::Inst()->GetGroup(plg.m_PathGroupID);
			PathGroupData *pPathGroup = FishResManager::Inst()->GetPathGroup(pathgroup->PathGroupIndex);
			bool bOK = true;
			for (int i = 0; i < pPathGroup->PathCount; ++i)
			{
				Fish *pFish = new Fish;
				pFish->InitFishData(ConvertDWORDToWORD(plg.m_PathGroupID), m_FishID, pathgroup->FishIndex, pathgroup->PathGroupIndex, ConvertIntToWORD(i), pathgroup->FishScaling, pathgroup->Speed, Vector3(0, 0, 0), 0, &pPathGroup->pPathList[i], m_TableTypeID);
				if (++m_FishID == MAX_FISH_NUM)
					m_FishID = START_FISH_ID;
				if (!m_pFishMgr->AddFish(pFish))
				{
					delete pFish;
					bOK = false;
					break;
				}
			}
			if (bOK)
				m_pSendAll->SendAll(ConvertDWORDToWORD(plg.m_PathGroupID), startID);

			plg.m_LastPathGroupLaunchTick = GetTickCount();
			if (--plg.m_PathGroupRepeat == 0)
				ListRemoveAt(m_RepeatLaunchList, j);
			else
				++j;
		}
	}
	UINT  LaunchPackage(UINT package)
	{
		byte packageType = byte(package >> 16);
		ushort fishGroup = ushort(package & 0xffff);
		UINT fishID = LaunchFish(fishGroup, false, package);
		if (fishID != -1)
		{
			NetCmdFishPackage pa;
			SetMsgInfo(pa,CMD_FISH_PACKAGE, sizeof(NetCmdFishPackage));
			//pa.SetCmdType(CMD_FISH_PACKAGE);
			//pa.SetCmdSize(sizeof(pa));
			pa.FishID = (USHORT)fishID;
			pa.PackageType = (byte)packageType;
			m_pSendAll->SendAll(&pa);
			return (fishID << 16) | packageType;
		}
		else
			return 0xffffffff;
		
	}
	UINT LaunchFish(int fishGroup, bool bforceLaunch, UINT pcakgeType = -1)
	{
		//fishGroup = 158;
		USHORT startID = m_FishID;
		GroupData *pGroup = FishResManager::Inst()->GetGroup(fishGroup);
		if (pGroup->FishGroupByPathGroup)
		{
			m_pSendAll->SendAll(ConvertIntToWORD(fishGroup), startID);
			return LaunchFish(fishGroup, pGroup, pcakgeType);
		}
		UINT fishPackageID = 0xffffffff;
		bool bOK = true;
		int pathIndex = GetPathIndex(fishGroup, pGroup, bforceLaunch);
		//pathIndex = 92;
		if (pathIndex == -1)	//û���ʺϵ�·��
			return fishPackageID;
		float startX = pGroup->FrontPosition.x;
		PathData *pPathData = FishResManager::Inst()->GetPath(pathIndex);
		for (int n = 0; n < pGroup->GroupDataCount; ++n)
		{
			GroupData::GroupItem &gd = pGroup->pGroupDataArray[n];
			int fishIdx = -1;
			if (pcakgeType != -1 && fishPackageID == -1)
			{
				//���������һ��������
				fishIdx = RandRange(0, gd.FishNum);
			}
			for (int i = 0; i < gd.FishNum; ++i)
			{
				float time = GetPathTimeByDist(startX, gd.pFishPosList[i].x, pathIndex);
				Fish *pFish = new Fish;
				pFish->InitFishData(ConvertIntToWORD(fishGroup), m_FishID, gd.FishIndex, ConvertIntToWORD(pathIndex), ConvertIntToWORD((n << 8) | i), gd.FishScaling, gd.SpeedScaling, gd.pFishPosList[i], time, pPathData, m_TableTypeID);

				if (fishIdx == i)
					fishPackageID = m_FishID;

				if (++m_FishID == MAX_FISH_NUM)
					m_FishID = START_FISH_ID;
				if (!m_pFishMgr->AddFish(pFish))
				{
					delete pFish;
					bOK = false;
					Log(L"�����ʧ�ܣ��ͻ��˿��ܲ����ڴ���!");
				}
			}
		}
		if (bOK)
		{
			m_pSendAll->SendAll(ConvertIntToWORD(fishGroup), ConvertIntToWORD(pathIndex), startID);
		}
		return fishPackageID;
	}
	void LaunchByTime(float deltaTime)
	{
		bool bClearState = false;
		//ʱ�����ɵķ�ʽ
		for (int i = 0; i < m_FishFlow->FlowTimeCount; ++i)
		{
			FlowTimeInfo &fti = m_FlowTimeCountList[i];
			if (fti.Count <= 0)
				continue;
			if (m_Time <= m_FishFlow->pFlowTimeList[i].StartTime &&
				m_Time + deltaTime > m_FishFlow->pFlowTimeList[i].EndTime)
			{
				fti.LastTime += deltaTime;
				if (fti.LastTime < m_FishFlow->pFlowTimeList[i].LaunchInterval)
					continue;
				--fti.Count;
				fti.LastTime = 0;
				int rnd = RandRange(0, 100);
				if (rnd < m_FishFlow->pFlowTimeList[i].Probability)
				{
					//������Ⱥ
					LaunchFish(m_FishFlow->pFlowTimeList[i].GroupID, false);
				}
			}
			else if (m_Time >= m_FishFlow->pFlowTimeList[i].StartTime)
			{
				if (m_Time > m_FishFlow->pFlowTimeList[i].EndTime) 
					continue;
				fti.LastTime += deltaTime;
				if (fti.LastTime < m_FishFlow->pFlowTimeList[i].LaunchInterval) 
					continue;
				--fti.Count;
				fti.LastTime = 0;
				int rnd = RandRange(0, 100);
				if (rnd < m_FishFlow->pFlowTimeList[i].Probability)
				{
					//������Ⱥ
					LaunchFish(m_FishFlow->pFlowTimeList[i].GroupID, false);
				}
			}
			else
				break;
		}
		//m_bClearState = bClearState;
	}
	void RefreshFlowData()
	{
		m_MaxProbability = 0;
		for (int i = 0; i < m_FishFlow->GroupCount; ++i)
		{
			if (m_FlowGroupCountList[i] > 0)
				m_MaxProbability += m_FishFlow->pFlowGroupList[i].Probability;
		}
	}
	void LaunchByGroup()
	{
		//����������ɵķ�ʽ
		RefreshFlowData();
		if (m_MaxProbability == 0) return;
		int probability = RandRange(0, m_MaxProbability) + 1;
		int probabilityCount = 0;
		for (int i = 0; i < m_FishFlow->GroupCount; ++i)
		{
			if (m_FishFlow->pFlowGroupList[i].MaxCount != -1 && m_FlowGroupCountList[i] <= 0) continue;
			probabilityCount += m_FishFlow->pFlowGroupList[i].Probability;
			if (probabilityCount >= probability)
			{
				if (m_FishFlow->pFlowGroupList[i].MaxCount != -1)
					--m_FlowGroupCountList[i];
				LaunchFish(m_FishFlow->pFlowGroupList[i].GroupID, false);
				break;
			}
		}
	}
	void SendPackage(int fishID, int packageType)
	{
		//���ͺ������
		NetCmdFishPackage pa;
		pa.SetCmdType(CMD_FISH_PACKAGE);
		pa.SetCmdSize(sizeof(pa));
		pa.FishID = ConvertIntToWORD(fishID);
		pa.PackageType = (byte)packageType;
		m_pSendAll->SendAll(&pa);
	}
	UINT Update(float deltaTime, UINT package)
	{
		UINT FishID_Package = 0xffffffff;
		if (m_FishFlow == NULL || m_bActive == false)
			return FishID_Package;
		UINT tick = GetTickCount();
		HashMap<int, LaunchPathTime>::iterator it ;
		for (it = m_PathIndexList.begin(); it != m_PathIndexList.end();)
		{
			if (tick - it->second.Tick >= MAX_PATH_TIME)
			{
				it = m_PathIndexList.erase(it);
			}
			else
				++it;
		}
		m_LauncherTime += deltaTime;

		if (m_LauncherTime >= m_FishFlow->LauncherInterval && m_bClearState == false)
		{
			LaunchByGroup();
			m_LauncherTime -= m_FishFlow->LauncherInterval;
		}

		LaunchPathGroupRepeat();
		LaunchByTime(deltaTime);
		m_Time += deltaTime;
		if (package != -1)
		{
			//������·��
			FishID_Package = LaunchPackage(package);
		}
		
		if (m_Time >= m_FishFlow->MaxTime)
		{
			if (m_FishFlow->LoopCount != -1 && m_LoopCount >= m_FishFlow->LoopCount)
			{
				m_bActive = false;
			}
			else
			{
				InitFlowData(false);
				m_Time = 0;
				++m_LoopCount;
			}
		}
		return FishID_Package;
	}
	void SetDeactive()
	{
		m_bActive = false;
	}
	bool HasLaunchFish()
	{
		//ʱ�����ɵķ�ʽ
		bool has = false;
		for (int i = 0; i < m_FishFlow->FlowTimeCount; ++i)
		{
			FlowTimeInfo &fti = m_FlowTimeCountList[i];
			if (fti.Count <= 0)continue;
			if (m_Time <= m_FishFlow->pFlowTimeList[i].StartTime ||
				m_Time <= m_FishFlow->pFlowTimeList[i].EndTime)
			{
				has = true;
				break;
			}
		}
		return has || (m_RepeatLaunchList.size() != 0);
	}
};

