#include "Stdafx.h"
#include "FishConfig.h"
FishConfig::FishConfig()
{

}
FishConfig::~FishConfig()
{
	OnDestroy();
}
bool FishConfig::LoadConfigFilePath()
{
	//1.加载配置文件
	WHXml pXml;
	if (!pXml.LoadXMLFilePath(TEXT("FishConfig.xml")))
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pFishConfig = pXml.GetChildNodeByName(TEXT("FishConfig"));
	if (!pFishConfig)
	{
		ASSERT(false);
		return false;
	}
	//读取物品
	if (!LoadFishItemConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	//奖励
	if (!LoadFishRewardConfig(pFishConfig))//奖励的读取应该放在前面
	{
		ASSERT(false);
		return false;
	}
	//11.桌子参数
	if (!LoadFishTableConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	//服务器更新时间
	if (!LoadFishUpdateConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	if (!LoadFishSystemConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	if (!LoadFishVipConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	if (!LoadFishGameRobotConfig(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	if (!LoadFishWhiteList(pFishConfig))
	{
		ASSERT(false);
		return false;
	}
	if (!LoadFishJJC(pFishConfig))
	{
		return false;
	}
	//过滤关键字
	if (!LoadFishErrorStringFile("ErrorString.txt"))
	{
		ASSERT(false);
		return false;
	}
	return true;
}
void FishConfig::OnDestroy()
{
}

bool FishConfig::LoadFishTableConfig(WHXmlNode* pFishConfig)
{
	WHXmlNode* pFishTables = pFishConfig->GetChildNodeByName(TEXT("FishTables"));
	if (!pFishTables)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pTable = pFishTables->GetChildNodeByName(TEXT("Table"));
	while (pTable)
	{
		TableInfo pTableInfo;

		if (!pTable->GetAttribute(TEXT("ID"), pTableInfo.TableTypeID))
		{
			ASSERT(false);
			return false;
		}

		int iValue = 0;
		if (!pTable->GetAttribute(TEXT("MinGlobelSum"), iValue))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MinGlobelSum = 0xffffffff;
		else
			pTableInfo.MinGlobelSum = ConvertIntToDWORD(iValue);

		if (!pTable->GetAttribute(TEXT("MaxGlobelSum"), iValue ))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MaxGlobelSum = 0xffffffff;
		else
			pTableInfo.MaxGlobelSum = ConvertIntToDWORD(iValue);

		double doubleValue = 0.0;
		if (!pTable->GetAttribute(TEXT("BasicRatio"), doubleValue))
		{
			ASSERT(false);
			return false;
		}
		pTableInfo.BasicRatio = ((int)((doubleValue + 0.00001) * 10000))*1.0 / 10000;
		if (!pTable->GetAttribute(TEXT("MinCurrey"), iValue ))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MinCurreySum = 0xffffffff;
		else
			pTableInfo.MinCurreySum = ConvertIntToDWORD(iValue);
		if (!pTable->GetAttribute(TEXT("MaxCurrey"), iValue ))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MaxCurreySum = 0xffffffff;
		else
			pTableInfo.MaxCurreySum = ConvertIntToDWORD(iValue);


		if (!pTable->GetAttribute(TEXT("MinLevel"), iValue))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MinLevel = 0xffffffff;
		else
			pTableInfo.MinLevel = ConvertIntToDWORD(iValue);
		if (!pTable->GetAttribute(TEXT("MaxLevel"), iValue))
		{
			ASSERT(false);
			return false;
		}
		if (iValue == -1)
			pTableInfo.MaxLevel = 0xffffffff;
		else
			pTableInfo.MaxLevel = ConvertIntToDWORD(iValue);

		if (!pTable->GetAttribute(TEXT("MaxPlayerSum"), pTableInfo.wMaxPlayerSum))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("MinRate"), pTableInfo.MinRate))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("MaxRate"), pTableInfo.MaxRate))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("TableRate"), pTableInfo.TableRate))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("InitProduce"), pTableInfo.InitProduce))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("InitEarn"), pTableInfo.InitEarn))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("InitTurn"), pTableInfo.InitTurn))
		{
			ASSERT(false);
			return false;
		}
		if (!pTable->GetAttribute(TEXT("MapName"), pTableInfo.cMapName,CountArray(pTableInfo.cMapName)))
		{
			ASSERT(false);
			return false;
		}

		WHXmlNode* pJoinItem = pTable->GetChildNodeByName(TEXT("pTable"));
		while (pJoinItem)
		{
			tagTableJoinItem pItem;
			if (!pJoinItem->GetAttribute(TEXT("ItemID"), pItem.ItemID))
			{
				ASSERT(false);
				return false;
			}
			if (!pJoinItem->GetAttribute(TEXT("ItemSum"), pItem.ItemSum))
			{
				ASSERT(false);
				return false;
			}
			pTableInfo.JoinItemVec.push_back(pItem);
			pJoinItem = pJoinItem->GetNextSignelNode();
		}

		m_TableConfig.m_TableConfig.insert(HashMap<BYTE, TableInfo>::value_type(pTableInfo.TableTypeID, pTableInfo));
		pTable = pTable->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::LoadFishUpdateConfig(WHXmlNode* pFishConfig)
{
	WHXmlNode* pFishUpdate = pFishConfig->GetChildNodeByName(TEXT("FishServerUpdateTime"));
	if (!pFishUpdate)
	{
		ASSERT(false);
		return false;
	}
	if (!pFishUpdate->GetAttribute(TEXT("UpdateHour"), m_FishUpdateConfig.UpdateHour))
		return false;
	if (!pFishUpdate->GetAttribute(TEXT("UpdateMin"), m_FishUpdateConfig.UpdateMin))
		return false;
	return true;
}
bool FishConfig::LoadFishItemConfig(WHXmlNode* pFishConfig)
{
	m_ItemMap.clear();
	WHXmlNode* pFishItems = pFishConfig->GetChildNodeByName(TEXT("FishItems"));
	if (!pFishItems)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pFishItem = pFishItems->GetChildNodeByName(TEXT("Item"));
	while (pFishItem)
	{
		tagItemConfig pItemConfig;

		if (!pFishItem->GetAttribute(TEXT("ID"), pItemConfig.ItemID))
		{
			return false;
		}
		if (!pFishItem->GetAttribute(TEXT("TypeID"), pItemConfig.ItemType))
		{
			return false;
		}
		if (!pFishItem->GetAttribute(TEXT("ItemParam"), pItemConfig.ItemParam))
		{
			pItemConfig.ItemParam = 0;
		}

		m_ItemMap.insert(HashMap<DWORD, tagItemConfig>::value_type(pItemConfig.ItemID, pItemConfig));
		pFishItem = pFishItem->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::LoadFishRewardConfig(WHXmlNode* pFishConfig)//游戏内部奖励的集合 货币 称号 成就的 以及物品(物品可能是宝箱等情况) 发送方式分为邮件 和 直接给予2种
{
	//加载玩家奖励的配置
	WHXmlNode* pFishRewards = pFishConfig->GetChildNodeByName(TEXT("FishRewardInfo"));
	if (!pFishRewards)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pFishReward = pFishRewards->GetChildNodeByName(TEXT("Reward"));
	while (pFishReward)
	{
		tagRewardOnce pReward;

		if (!pFishReward->GetAttribute(TEXT("RewardID"), pReward.RewardID))
			return false;	
		WHXmlNode* pFishItem = pFishReward->GetChildNodeByName(TEXT("Item"));
		while (pFishItem)
		{
			tagItemOnce pItem;
			if (!pFishItem->GetAttribute(TEXT("ItemID"), pItem.ItemID))
				return false;
			if (!ItemIsExists(pItem.ItemID))
				return false;
			if (!pFishItem->GetAttribute(TEXT("ItemSum"), pItem.ItemSum))
				return false;
			if (!pFishItem->GetAttribute(TEXT("LastMin"), pItem.LastMin))
				return false;
			pReward.RewardItemVec.push_back(pItem);
			pFishItem = pFishItem->GetNextSignelNode();
		}
		m_FishRewardConfig.RewardMap.insert(HashMap<WORD, tagRewardOnce>::value_type(pReward.RewardID, pReward));
		pFishReward = pFishReward->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::LoadFishSystemConfig(WHXmlNode* pFishConfig)
{
	//m_SystemConfig
	WHXmlNode* pFishSystem = pFishConfig->GetChildNodeByName(TEXT("FishSystem"));
	if (!pFishSystem)
	{
		ASSERT(false);
		return false;
	}
	if (!pFishSystem->GetAttribute(TEXT("Version"), m_SystemConfig.VersionID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("PathCrc"), m_SystemConfig.PathCrc))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("MaxAchievementRankSum"), m_SystemConfig.AchievementRankSum))
		return false;
	if (m_SystemConfig.AchievementRankSum > 12000)
		return false;
	if (!pFishSystem->GetAttribute(TEXT("CacheMin"), m_SystemConfig.CacheUserMin))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("RsgInitGlobel"), m_SystemConfig.RsgInitGlobelSum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("AnnouncementSum"), m_SystemConfig.AnnouncementSum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("FirstBindPhoneRewardID"), m_SystemConfig.FirstBindPhoneRewardID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("UserQueueHandleSumBySec"), m_SystemConfig.UserQueueHandleSumBySec))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("ChangeNickNameNeedItemID"), m_SystemConfig.ChangeNickNameNeedItemID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("ChangeNickNameNeedItemSum"), m_SystemConfig.ChangeNickNameNeedItemSum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("ChangeGenderNeedItemID"), m_SystemConfig.ChangeGenderNeedItemID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("ChangeGenderNeedItemSum"), m_SystemConfig.ChangeGenderNeedItemSum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("LeaveTableNonLauncherMin"), m_SystemConfig.LeaveTableNonLauncherMin))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("InitRateValue"), m_SystemConfig.InitRateValue))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("MaxGobelSum"), m_SystemConfig.MaxGobelSum))
	{
		m_SystemConfig.MaxGobelSum = 9223372036854775807;
	}
	if (m_SystemConfig.MaxGobelSum <= 0)
	{
		m_SystemConfig.MaxGobelSum = 9223372036854775807;
	}
	if (!pFishSystem->GetAttribute(TEXT("EmailGlobelRewardID"), m_SystemConfig.EmailGlobelRewardID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("EmailCurrceyRewardID"), m_SystemConfig.EmailCurrceyRewardID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("EmailMedalRewradID"), m_SystemConfig.EmailMedalRewradID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("RoleSaveMin"), m_SystemConfig.RoleSaveMin))
		return false;

	{
		TCHAR TimeLog[32] = { 0 };
		if (!pFishSystem->GetAttribute(TEXT("RateInitRewardTime"), TimeLog, CountArray(TimeLog)))
			return false;

		int Year = 0;
		int Month = 0;
		int Day = 0;
		int Hour = 0;
		int Min = 0;
		int Sec = 0;

		if (_stscanf_s(TimeLog, TEXT("%d-%d-%d %d:%d:%d"), &Year, &Month, &Day, &Hour, &Min, &Sec) != 6)
			return false;

		tm pTime;
		pTime.tm_year = Year - 1900;
		pTime.tm_mon = Month - 1;
		pTime.tm_mday = Day;
		pTime.tm_hour = Hour;
		pTime.tm_min = Min;
		pTime.tm_sec = Sec;

		m_SystemConfig.RateInitRewardTime = mktime(&pTime);
	}

	if (!pFishSystem->GetAttribute(TEXT("SendMessageItemID"), m_SystemConfig.SendMessageItemID))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("SendMessageItemSum"), m_SystemConfig.SendMessageItemSum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("CashGoldNum"), m_SystemConfig.CashGoldNum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("CashMinRMBSum"), m_SystemConfig.MinCashRMBNum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("OneDayCashNum"), m_SystemConfig.OneDayCashNum))
		return false;
	if (!pFishSystem->GetAttribute(TEXT("BindAccountReward"), m_SystemConfig.BindAccountReward))
		return false;
	return true;
}
bool FishConfig::LoadFishVipConfig(WHXmlNode* pFishConfig)
{
	//加载VIP配置数据
	WHXmlNode* pFishVips = pFishConfig->GetChildNodeByName(TEXT("FishVips"));
	if (!pFishVips)
	{
		ASSERT(false);
		return false;
	}
	if (!pFishVips->GetAttribute(TEXT("DefaultLauncherReBoundNum"), m_VipMap.DefaultLauncherReBoundNum))
		return false;
	if (!pFishVips->GetAttribute(TEXT("DefaultAlmsSum"), m_VipMap.DefaultAlmsSum))
		return false;
	if (!pFishVips->GetAttribute(TEXT("DefaultUseMedalSum"), m_VipMap.DefaultUseMedalSum))
		return false;

	WHXmlNode* pFishVip = pFishVips->GetChildNodeByName(TEXT("Vip"));
	tagVipOnce pTotalOnce;
	pTotalOnce.AddAlmsRate = 0;
	pTotalOnce.AddAlmsSum = 0;
	pTotalOnce.AddMonthScoreRate = 0;
	pTotalOnce.AddReChargeRate = 0;
	pTotalOnce.CanUseLauncherMap.clear();
	//pTotalOnce.IsCanLauncherLocking = false;
	pTotalOnce.LauncherReBoundNum = 0;
	pTotalOnce.NeedRechatgeRMBSum = 0;
	pTotalOnce.UpperVipLevel = 0;
	pTotalOnce.VipLevel = 0;
	while (pFishVip)
	{
		tagVipOnce pCurrceyOnce;

		if (!pFishVip->GetAttribute(TEXT("VipLevel"), pCurrceyOnce.VipLevel))
			return false;
		if (!pFishVip->GetAttribute(TEXT("UpperVipLevel"), pCurrceyOnce.UpperVipLevel))
			return false;
		if (!pFishVip->GetAttribute(TEXT("LauncherReBoundNum"), pCurrceyOnce.LauncherReBoundNum))
			return false;
		/*BYTE IsCanLauncherLocking = 0;
		if (!pFishVip->GetAttribute(TEXT("IsCanLauncherLocking"), IsCanLauncherLocking))
			return false;
		pCurrceyOnce.IsCanLauncherLocking = (IsCanLauncherLocking == 1 ? true : false);*/

		if (!pFishVip->GetAttribute(TEXT("AddAlmsSum"), pCurrceyOnce.AddAlmsSum))
			return false;
		if (!pFishVip->GetAttribute(TEXT("AddAlmsRate"), pCurrceyOnce.AddAlmsRate))
			return false;
		if (!pFishVip->GetAttribute(TEXT("AddMonthScoreRate"), pCurrceyOnce.AddMonthScoreRate))
			return false;
		if (!pFishVip->GetAttribute(TEXT("AddReChargeRate"), pCurrceyOnce.AddReChargeRate))
			return false;
		if (!pFishVip->GetAttribute(TEXT("RechargeRMBSum"), pCurrceyOnce.NeedRechatgeRMBSum))
			return false;
		if (!pFishVip->GetAttribute(TEXT("AddUseMedalSum"), pCurrceyOnce.AddUseMedalSum))
			return false;
		if (!pFishVip->GetAttribute(TEXT("AddCatchFishRate"), pCurrceyOnce.AddCatchFishRate))
			return false;
		BYTE AddLauncherID = 0;
		if (!pFishVip->GetAttribute(TEXT("AddLauncherID"), AddLauncherID))
			return false;

		//加入到总等级里面
		pTotalOnce.VipLevel = pCurrceyOnce.VipLevel;
		pTotalOnce.UpperVipLevel = pCurrceyOnce.UpperVipLevel;
		pTotalOnce.LauncherReBoundNum += pCurrceyOnce.LauncherReBoundNum;
		//pTotalOnce.IsCanLauncherLocking = (pTotalOnce.IsCanLauncherLocking || pCurrceyOnce.IsCanLauncherLocking);
		pTotalOnce.AddAlmsRate += pCurrceyOnce.AddAlmsRate;
		pTotalOnce.AddAlmsSum += pCurrceyOnce.AddAlmsSum;
		pTotalOnce.AddMonthScoreRate += pCurrceyOnce.AddMonthScoreRate;
		pTotalOnce.AddReChargeRate += pCurrceyOnce.AddReChargeRate;
		pTotalOnce.NeedRechatgeRMBSum = pCurrceyOnce.NeedRechatgeRMBSum;
		pTotalOnce.AddUseMedalSum += pCurrceyOnce.AddUseMedalSum;
		pTotalOnce.AddCatchFishRate += pCurrceyOnce.AddCatchFishRate;
		pTotalOnce.CanUseLauncherMap.insert(set<BYTE>::value_type(AddLauncherID));

		m_VipMap.VipMap.insert(HashMap<BYTE, tagVipOnce>::value_type(pTotalOnce.VipLevel,pTotalOnce));
		pFishVip = pFishVip->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::LoadFishWhiteList(WHXmlNode* pFishConfig)
{
	if (!pFishConfig)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pFishNode = pFishConfig->GetChildNodeByName(TEXT("WhiteList"));
	if (!pFishNode)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pItem = pFishNode->GetChildNodeByName(TEXT("Item"));
	DWORD TotalRate = 0;
	while (pItem)
	{
		int64 uid = 0;
		if (!pItem->GetAttribute(TEXT("uid"), uid))
			return false;
		m_WhiteList.uid.push_back(uid);
		pItem = pItem->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::LoadFishJJC(WHXmlNode * pFishConfig)
{
	if (!pFishConfig)
	{
		return false;
	}
	WHXmlNode* pFishNode = pFishConfig->GetChildNodeByName(TEXT("Arena"));
	if (!pFishNode)
	{
		return false;
	}
	if (!pFishNode->GetAttribute(TEXT("MaxPlayerSum"), m_jjc.maxPlayerSum))
		return false;

	WHXmlNode* pTable = pFishNode->GetChildNodeByName(TEXT("table"));
	if (!pTable)
	{
		return false;
	}
	m_jjc.jjcTable.clear();
	while (pTable)
	{
		tagJJCTableInfo tableInfo;
		BYTE tableTypeID = -1;
		pTable->GetAttribute(TEXT("id"), tableTypeID);
		INT64 admission = 0;
		pTable->GetAttribute(TEXT("admission"), tableInfo.admission);
		WHXmlNode* pReward = pTable->GetChildNodeByName(TEXT("reward"));
		if (!pReward)
		{
			return false;
		}
		WHXmlNode* pRewardItem = pReward->GetChildNodeByName(TEXT("Item"));
		while (pRewardItem)
		{
			BYTE rank = 0;
			INT64 gold = 0;
			pRewardItem->GetAttribute(TEXT("rank"), rank);
			pRewardItem->GetAttribute(TEXT("gold"), gold);
			tableInfo.reward.insert(make_pair(rank, gold));
			pRewardItem = pRewardItem->GetNextSignelNode();
		}
		m_jjc.jjcTable.insert(make_pair(tableTypeID, tableInfo));
		pTable = pTable->GetNextSignelNode();
	}
	WHXmlNode* pTime = pFishNode->GetChildNodeByName(TEXT("time"));
	if (!pTime)
	{
		return false;
	}
	if (!pTime->GetAttribute(TEXT("maxtime"), m_jjc.time.maxTime))
		return false;
	if (!pTime->GetAttribute(TEXT("remaintime"), m_jjc.time.remainTime))
		return false;
	WHXmlNode* pTimeItem = pTime->GetChildNodeByName(TEXT("Item"));
	m_jjc.time.openTime.clear();
	while (pTimeItem)
	{
		subJJC_Time tempTime;
		pTimeItem->GetAttribute(TEXT("year"), tempTime.year);
		pTimeItem->GetAttribute(TEXT("month"), tempTime.month);
		pTimeItem->GetAttribute(TEXT("day"), tempTime.day);
		pTimeItem->GetAttribute(TEXT("hour"), tempTime.hour);
		pTimeItem->GetAttribute(TEXT("minute"), tempTime.minute);
		m_jjc.time.openTime.push_back(tempTime);
		pTimeItem = pTimeItem->GetNextSignelNode();
	}
	return true;
}
bool FishConfig::CheckInWhiteList(const int64& uid)
{
	const vector<int64>& uids = m_WhiteList.uid;
	for (const int64& x : uids)
	{
		if (x == uid)
			return true;
	}
	return false;
}
bool FishConfig::LoadFishGameRobotConfig(WHXmlNode* pFishConfig)
{
	//加载游戏机器人的一些配置文件数据
	if (!pFishConfig)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pFishRobot = pFishConfig->GetChildNodeByName(TEXT("FishRobot"));
	if (!pFishRobot)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pGameRobot = pFishRobot->GetChildNodeByName(TEXT("GameRobot"));
	if (!pGameRobot)
	{
		ASSERT(false);
		return false;
	}
	WHXmlNode* pRobot = pGameRobot->GetChildNodeByName(TEXT("Robot"));
	while (pRobot)
	{
		tagGameRobotConfig pRobotConfig;

		if (!pRobot->GetAttribute(TEXT("RobotID"), pRobotConfig.RobotID))
			return false;
		if (!pRobot->GetAttribute(TEXT("MonthRobotSum"), pRobotConfig.MonthRobotSum))
			return false;
		if (!pRobot->GetAttribute(TEXT("AddGlobelSum"), pRobotConfig.AddGlobelSum))
			return false;
		if (!pRobot->GetAttribute(TEXT("InitGlobel"), pRobotConfig.InitGlobelSum))
			return false;
		if (!pRobot->GetAttribute(TEXT("InitCurrceySum"), pRobotConfig.InitCurrceySum))
			return false;
		if (!pRobot->GetAttribute(TEXT("AddCurrceySum"), pRobotConfig.AddCurrceySum))
			return false;
		if (!pRobot->GetAttribute(TEXT("MaxLevel"), pRobotConfig.MaxLevel))
			return false;
		if (!pRobot->GetAttribute(TEXT("MinLevel"), pRobotConfig.MinLevel))
			return false;
		if (!pRobot->GetAttribute(TEXT("GlobelRateF"), pRobotConfig.GlobelRateF))
			return false;
		if (!pRobot->GetAttribute(TEXT("CurrceyRateF"), pRobotConfig.CurrceyRateF))
			return false;
		//开炮控制
		WHXmlNode* pOpenFire = pRobot->GetChildNodeByName(TEXT("OpenFire"));
		if (!pOpenFire)
		{
			ASSERT(false);
			return false;
		}

		BYTE TempBool = 0;
		if (!pOpenFire->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.OpenFireInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pOpenFire->GetAttribute(TEXT("LastOpenFireMinSec"), pRobotConfig.OpenFireInfo.FireInfo.OpenMinSec))
			return false;
		if (!pOpenFire->GetAttribute(TEXT("LastOpenFireMaxSec"), pRobotConfig.OpenFireInfo.FireInfo.OpenMaxSec))
			return false;
		if (!pOpenFire->GetAttribute(TEXT("StopFireMinSec"), pRobotConfig.OpenFireInfo.FireInfo.StopMinSec))
			return false;
		if (!pOpenFire->GetAttribute(TEXT("StopFireMaxSec"), pRobotConfig.OpenFireInfo.FireInfo.StopMaxSec))
			return false;

		//切换炮台控制
		WHXmlNode* pLauncher = pRobot->GetChildNodeByName(TEXT("Launcher"));
		if (!pLauncher)
		{
			ASSERT(false);
			return false;
		}

		if (!pLauncher->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.LauncherInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pLauncher->GetAttribute(TEXT("ChangeLauncherMinSec"), pRobotConfig.LauncherInfo.LauncherInfo.OpenMinSec))
			return false;
		if (!pLauncher->GetAttribute(TEXT("ChangeLauncherMaxSec"), pRobotConfig.LauncherInfo.LauncherInfo.OpenMaxSec))
			return false;
		/*if (!pLauncher->GetAttribute(TEXT("ChangeLauncherCDMinSec"), pRobotConfig.LauncherInfo.LauncherInfo.StopMinSec))
			return false;
		if (!pLauncher->GetAttribute(TEXT("ChangeLauncherCDMaxSec"), pRobotConfig.LauncherInfo.LauncherInfo.StopMaxSec))
			return false;*/
		WHXmlNode*  pLauncherType = pLauncher->GetChildNodeByName(TEXT("Once"));
		pRobotConfig.LauncherInfo.MaxRateF = 0;
		while (pLauncherType)
		{
			tagRobotLauncherType pType;

			if (!pLauncherType->GetAttribute(TEXT("LauncherTypeID"), pType.LauncherType))
				return false;
			if (!pLauncherType->GetAttribute(TEXT("RateF"), pType.RateF))
				return false;

			pRobotConfig.LauncherInfo.MaxRateF += pType.RateF;
			pType.RateF = pRobotConfig.LauncherInfo.MaxRateF;
			pRobotConfig.LauncherInfo.LauncherVec.push_back(pType);
			pLauncherType = pLauncherType->GetNextSignelNode();
		}

		//Rate 倍率切换
		WHXmlNode* pRate = pRobot->GetChildNodeByName(TEXT("Rate"));
		if (!pRate)
		{
			ASSERT(false);
			return false;
		}

		if (!pRate->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.RateInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pRate->GetAttribute(TEXT("ChangeRateMinSec"), pRobotConfig.RateInfo.RateInfo.OpenMinSec))
			return false;
		if (!pRate->GetAttribute(TEXT("ChangeRateMaxSec"), pRobotConfig.RateInfo.RateInfo.OpenMaxSec))
			return false;
		/*if (!pRate->GetAttribute(TEXT("ChangeRateCDMinSec"), pRobotConfig.RateInfo.RateInfo.StopMinSec))
			return false;
		if (!pRate->GetAttribute(TEXT("ChangeRateCDMaxSec"), pRobotConfig.RateInfo.RateInfo.StopMaxSec))
			return false;*/
		WHXmlNode*  pRateType = pRate->GetChildNodeByName(TEXT("Once"));
		pRobotConfig.RateInfo.MaxRateF = 0;
		while (pRateType)
		{
			tagRobotRateType pType;

			if (!pRateType->GetAttribute(TEXT("RateTypeID"), pType.RateType))
				return false;
			if (!pRateType->GetAttribute(TEXT("RateF"), pType.RateF))
				return false;

			pRobotConfig.RateInfo.MaxRateF += pType.RateF;
			pType.RateF = pRobotConfig.RateInfo.MaxRateF;
			pRobotConfig.RateInfo.RateVec.push_back(pType);
			pRateType = pRateType->GetNextSignelNode();
		}

		//Skill
		WHXmlNode* pSkill = pRobot->GetChildNodeByName(TEXT("Skill"));
		if (!pSkill)
		{
			ASSERT(false);
			return false;
		}

		if (!pSkill->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.SkillInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pSkill->GetAttribute(TEXT("UseSkillMinSec"), pRobotConfig.SkillInfo.SkillInfo.OpenMinSec))
			return false;
		if (!pSkill->GetAttribute(TEXT("UseSkillMaxSec"), pRobotConfig.SkillInfo.SkillInfo.OpenMaxSec))
			return false;
	/*	if (!pSkill->GetAttribute(TEXT("SkillCDMinSec"), pRobotConfig.SkillInfo.SkillInfo.StopMinSec))
			return false;
		if (!pSkill->GetAttribute(TEXT("SkillCDMaxSec"), pRobotConfig.SkillInfo.SkillInfo.StopMaxSec))
			return false;*/

		//Room
		WHXmlNode* pRoom = pRobot->GetChildNodeByName(TEXT("Room"));
		if (!pRoom)
		{
			ASSERT(false);
			return false;
		}

		if (!pRoom->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.RoomInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pRoom->GetAttribute(TEXT("JoinTableMinSec"), pRobotConfig.RoomInfo.RoomInfo.OpenMinSec))
			return false;
		if (!pRoom->GetAttribute(TEXT("JoinTableMaxSec"), pRobotConfig.RoomInfo.RoomInfo.OpenMaxSec))
			return false;
		if (!pRoom->GetAttribute(TEXT("WriteMinSec"), pRobotConfig.RoomInfo.RoomInfo.StopMinSec))
			return false;
		if (!pRoom->GetAttribute(TEXT("WriteMaxSec"), pRobotConfig.RoomInfo.RoomInfo.StopMaxSec))
			return false;

		//charm
		WHXmlNode* pCharm = pRobot->GetChildNodeByName(TEXT("Charm"));
		if (!pCharm)
		{
			ASSERT(false);
			return false;
		}
		if (!pCharm->GetAttribute(TEXT("IsOpen"), TempBool))
			return false;
		pRobotConfig.CharmInfo.IsOpen = (TempBool == 1 ? true : false);

		if (!pCharm->GetAttribute(TEXT("SendCharmMinCDSec"), pRobotConfig.CharmInfo.SendCharmMinCDSec))
			return false;
		if (!pCharm->GetAttribute(TEXT("SendCharmMaxCDSec"), pRobotConfig.CharmInfo.SendCharmMaxCDSec))
			return false;
		if (!pCharm->GetAttribute(TEXT("BeChangeCharmValue"), pRobotConfig.CharmInfo.BeChangeCharmValue))
			return false;
		if (!pCharm->GetAttribute(TEXT("BeChangeCharmMaxSec"), pRobotConfig.CharmInfo.BeChangeCharmSec))
			return false;

		if (!pCharm->GetAttribute(TEXT("NonRateF"), pRobotConfig.CharmInfo.NonRateF))
			return false;

		WHXmlNode*  pCharmSend = pCharm->GetChildNodeByName(TEXT("Send"));
		pRobotConfig.CharmInfo.SendInfo.dwSendCharmArrayMaxRateF = 0;
		while (pCharmSend)
		{
			tagRobotSendCharmInfo pInfo;

			if (!pCharmSend->GetAttribute(TEXT("CharmIndexID"), pInfo.CharmIndexID))
				return false;
			if (!pCharmSend->GetAttribute(TEXT("MinItemSum"), pInfo.MinItemSum))
				return false;
			if (!pCharmSend->GetAttribute(TEXT("MaxItemSum"), pInfo.MaxItemSum))
				return false;
			if (!pCharmSend->GetAttribute(TEXT("RateF"), pInfo.RateF))
				return false;

			pRobotConfig.CharmInfo.SendInfo.dwSendCharmArrayMaxRateF += pInfo.RateF;
			pInfo.RateF = pRobotConfig.CharmInfo.SendInfo.dwSendCharmArrayMaxRateF;
			pRobotConfig.CharmInfo.SendInfo.vecSendCharmArray.push_back(pInfo);

			pCharmSend = pCharmSend->GetNextSignelNode();
		}

		WHXmlNode*  pBeSend = pCharm->GetChildNodeByName(TEXT("BeSend"));
		while (pBeSend)
		{
			tagRobotBeSendCharm pBeSendInfo;
			if (!pBeSend->GetAttribute(TEXT("CharmIndexID"), pBeSendInfo.CharmIndexID))
				return false;

			if (!pBeSend->GetAttribute(TEXT("NonRateF"), pBeSendInfo.NonRateF))
				return false;

			WHXmlNode*  pCharmSend = pBeSend->GetChildNodeByName(TEXT("Send"));
			pBeSendInfo.SendInfo.dwSendCharmArrayMaxRateF = 0;
			while (pCharmSend)
			{
				tagRobotSendCharmInfo pInfo;

				if (!pCharmSend->GetAttribute(TEXT("CharmIndexID"), pInfo.CharmIndexID))
					return false;
				if (!pCharmSend->GetAttribute(TEXT("MinItemSum"), pInfo.MinItemSum))
					return false;
				if (!pCharmSend->GetAttribute(TEXT("MaxItemSum"), pInfo.MaxItemSum))
					return false;
				if (!pCharmSend->GetAttribute(TEXT("RateF"), pInfo.RateF))
					return false;

				pBeSendInfo.SendInfo.dwSendCharmArrayMaxRateF += pInfo.RateF;
				pInfo.RateF = pBeSendInfo.SendInfo.dwSendCharmArrayMaxRateF;
				pBeSendInfo.SendInfo.vecSendCharmArray.push_back(pInfo);

				pCharmSend = pCharmSend->GetNextSignelNode();
			}

			pRobotConfig.CharmInfo.BeSendInfoMap.insert(HashMap<BYTE, tagRobotBeSendCharm>::value_type(pBeSendInfo.CharmIndexID, pBeSendInfo));

			pBeSend = pBeSend->GetNextSignelNode();
		}

		WHXmlNode*  pCharmChange = pCharm->GetChildNodeByName(TEXT("CharmChangeEvent"));
		while (pCharmChange)
		{
			if (!pCharmChange->GetAttribute(TEXT("LeaveRoomRateF"), pRobotConfig.CharmInfo.CharmChangeEvent.LeaveRoomRateF))
				return false;

			if (!pCharmChange->GetAttribute(TEXT("NonRateF"), pRobotConfig.CharmInfo.CharmChangeEvent.NonRateF))
				return false;

			WHXmlNode*  pCharmAdd = pCharmChange->GetChildNodeByName(TEXT("Add"));
			if (!pCharmAdd)
				return false;
			{
				WHXmlNode*  pCharmSend = pCharmAdd->GetChildNodeByName(TEXT("Send"));
				pRobotConfig.CharmInfo.CharmChangeEvent.AddSendInfo.dwSendCharmArrayMaxRateF = 0;
				while (pCharmSend)
				{
					tagRobotSendCharmInfo pInfo;

					if (!pCharmSend->GetAttribute(TEXT("CharmIndexID"), pInfo.CharmIndexID))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("MinItemSum"), pInfo.MinItemSum))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("MaxItemSum"), pInfo.MaxItemSum))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("RateF"), pInfo.RateF))
						return false;

					pRobotConfig.CharmInfo.CharmChangeEvent.AddSendInfo.dwSendCharmArrayMaxRateF += pInfo.RateF;
					pInfo.RateF = pRobotConfig.CharmInfo.CharmChangeEvent.AddSendInfo.dwSendCharmArrayMaxRateF;
					pRobotConfig.CharmInfo.CharmChangeEvent.AddSendInfo.vecSendCharmArray.push_back(pInfo);

					pCharmSend = pCharmSend->GetNextSignelNode();
				}
			}
			
			WHXmlNode*  pCharmDel = pCharmChange->GetChildNodeByName(TEXT("Del"));
			if (!pCharmDel)
				return false;
			{
				WHXmlNode*  pCharmSend = pCharmDel->GetChildNodeByName(TEXT("Send"));
				pRobotConfig.CharmInfo.CharmChangeEvent.DelSendInfo.dwSendCharmArrayMaxRateF = 0;
				while (pCharmSend)
				{
					tagRobotSendCharmInfo pInfo;

					if (!pCharmSend->GetAttribute(TEXT("CharmIndexID"), pInfo.CharmIndexID))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("MinItemSum"), pInfo.MinItemSum))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("MaxItemSum"), pInfo.MaxItemSum))
						return false;
					if (!pCharmSend->GetAttribute(TEXT("RateF"), pInfo.RateF))
						return false;

					pRobotConfig.CharmInfo.CharmChangeEvent.DelSendInfo.dwSendCharmArrayMaxRateF += pInfo.RateF;
					pInfo.RateF = pRobotConfig.CharmInfo.CharmChangeEvent.DelSendInfo.dwSendCharmArrayMaxRateF;
					pRobotConfig.CharmInfo.CharmChangeEvent.DelSendInfo.vecSendCharmArray.push_back(pInfo);

					pCharmSend = pCharmSend->GetNextSignelNode();
				}
			}

			pCharmChange = pCharmChange->GetNextSignelNode();
		}

		m_GameRobotConfig.RobotMap.insert(HashMap<DWORD, tagGameRobotConfig>::value_type(pRobotConfig.RobotID, pRobotConfig));
		pRobot = pRobot->GetNextSignelNode();
	}
	m_GameRobotConfig.RobotIndexMap.clear();
	//机器人映射表
	WHXmlNode* pOnceTable = pGameRobot->GetChildNodeByName(TEXT("OnceTable"));
	while (pOnceTable)
	{
		BYTE TableID = 0;
		BYTE MonthID = 0;
		if (!pOnceTable->GetAttribute(TEXT("TableID"), TableID))
			return false;
		if (!pOnceTable->GetAttribute(TEXT("MonthID"), MonthID))
			return false;
		DWORD Key = (TableID << 16) + MonthID;

		WHXmlNode* pRobot = pOnceTable->GetChildNodeByName(TEXT("Robot"));
		while (pRobot)
		{
			DWORD RobotID = 0;
			if (!pRobot->GetAttribute(TEXT("ID"), RobotID))
				return false;
			if (m_GameRobotConfig.RobotMap.count(RobotID) != 1)
				return false;
			m_GameRobotConfig.RobotIndexMap.insert(multimap<DWORD, DWORD>::value_type(Key, RobotID));
			pRobot = pRobot->GetNextSignelNode();
		}
		pOnceTable = pOnceTable->GetNextSignelNode();
	}
	return true;
}
DWORD FishConfig::GetWriteSec()
{
	return m_FishUpdateConfig.UpdateHour * 3600 + m_FishUpdateConfig.UpdateMin * 60;
}
BYTE FishConfig::GetItemType(DWORD ItemID)
{
	tagItemConfig* pConfig = GetItemInfo(ItemID);
	if (!pConfig)
		return 0;
	else
		return pConfig->ItemType;
}
DWORD FishConfig::GetItemParam(DWORD ItemID)
{
	tagItemConfig* pConfig = GetItemInfo(ItemID);
	if (!pConfig)
		return 0;
	else
		return pConfig->ItemParam;
}
tagItemConfig* FishConfig::GetItemInfo(DWORD ItemID)
{
	HashMap<DWORD, tagItemConfig>::iterator Iter = m_ItemMap.find(ItemID);
	if (Iter == m_ItemMap.end())
		return NULL;
	else
		return &Iter->second;
}
bool FishConfig::CheckVersionAndPathCrc(DWORD VersionID, DWORD PathCrc)
{
	if (GetSystemConfig().VersionID == 0 && GetSystemConfig().PathCrc == 0)
	{
		return true;
	}
	if (GetSystemConfig().VersionID > VersionID || GetSystemConfig().PathCrc > PathCrc)
		return false;
	else
		return true;
}
bool FishConfig::LoadFishErrorStringFile(const char* FilePath)
{
	//读取指定文件 将里面的屏蔽字全部读取出来
	m_ErrorStr.ErrorStrVec.clear();
	FILE* fp;
	errno_t Error= fopen_s(&fp, FilePath, "rb");
	if (Error != 0 || !fp)
	{
		ASSERT(false);
		return false;
	}
	fseek(fp, 0, SEEK_SET);

	char pSrcStr[512] = { 0 };

	while (!feof(fp))
	{
		fgets(pSrcStr, sizeof(pSrcStr), fp);//读取每一行
		//移除\n 换行符
		size_t Length = strlen(pSrcStr);
		if (pSrcStr[Length - 1] == '\n' || pSrcStr[Length - 1] == '\r')
		{
			pSrcStr[Length - 1] = '\0';
		}
		if (pSrcStr[Length - 2] == '\n' || pSrcStr[Length - 2] == '\r')
		{
			pSrcStr[Length - 2] = '\0';
		}
		//转化为 TCHAR
		UINT Count = 0;
		TCHAR* pDestChar = CharToWChar(pSrcStr,Count);
		if (Count == 0)
		{
			ASSERT(false);
			return false;
		}
		UINT uStrLength = _tcslen(pDestChar);
		if (uStrLength > MAX_ERRORSTR_LENGTH)
			continue;

		tagErrorStr pStr;
		TCHARCopy(pStr.ErrorStr, CountArray(pStr.ErrorStr), pDestChar, Count);
		m_ErrorStr.ErrorStrVec.push_back(pStr);

		free(pDestChar);
	}
	fclose(fp);
	return true;
}
bool FishConfig::CheckStringIsError(TCHAR* pStr,DWORD MinLength,DWORD MaxLength, StringCheckType pType)
{
	//空指针 和 长度过滤
	if (!pStr)
	{
		return false;
	}
	DWORD ArrayLength = _tcsclen(pStr);
	if (MinLength !=0 && ArrayLength == 0)
	{
		return false;
	}
	if (ArrayLength< MinLength || ArrayLength>MaxLength)
	{
		return false;
	}
	if (pType == SCT_Length)
		return true;
	if (pType != SCT_AccountName && pType != SCT_Password)
	{
		vector<tagErrorStr>::iterator Iter = m_ErrorStr.ErrorStrVec.begin();
		for (; Iter != m_ErrorStr.ErrorStrVec.end(); ++Iter)
		{
			if (GetStrIndeof(pStr, Iter->ErrorStr))
			{
				return false;
			}
		}
	}
	//根据类型在进行过滤
	if (pType == SCT_AccountName || pType == SCT_Password)
	{
		//只可以包含 0-9  A-Z  _  a-z 这些数据 
		for (size_t i = 0; i < ArrayLength; ++i)
		{
			if (
				pStr[i] == 95 ||
				(pStr[i] >= 48 && pStr[i] <= 57) ||
				(pStr[i] >= 65 && pStr[i] <= 90) ||
				(pStr[i] >= 97 && pStr[i] <= 122)
				)
			{
				continue;
			}
			return false;
		}
		return true;
	}
	else if (pType == SCT_Normal)
	{
		return true;
	}
	else
		return false;
}
bool FishConfig::GetStrIndeof(TCHAR* pStr, TCHAR* FindStr)
{
	if (!pStr || !FindStr)
	{
		ASSERT(false);
		return false;
	}
	if (_tcsstr(pStr, FindStr) != null)
		return true;
	else
		return false;
}
