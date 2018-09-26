//捕鱼的配置文件的集中读取
#pragma once
#include "TinyXml\XmlReader.h"
#include <set>
//#define MAX_Group 256
enum StringCheckType
{
	SCT_AccountName = 1,
	SCT_Password    = 2,
	SCT_Normal		= 3,
	SCT_Length		= 4,
};

class FishConfig
{
public:
	FishConfig();
	virtual ~FishConfig();

	bool LoadConfigFilePath();
	void OnDestroy();

	tagTableConfig& GetTableConfig(){ return m_TableConfig; }
	tagFishServerUpdate& GetFishUpdateConfig(){ return m_FishUpdateConfig; }
	//tagGlobelShop& GetGlobelShopConfig(){ return m_GlobelShopConfig; }
	bool ItemIsExists(DWORD ItemID){ return m_ItemMap.count(ItemID) == 1; }
	BYTE GetItemType(DWORD ItemID);
	DWORD GetItemParam(DWORD ItemID);
	tagItemConfig* GetItemInfo(DWORD ItemID);
	tagRewardMap& GetFishRewardConfig(){ return m_FishRewardConfig; }
	tagFishSystemInfo& GetSystemConfig(){ return m_SystemConfig; }
	tagVipConfig& GetVipConfig(){ return m_VipMap; }
	tagGameRobotInfo& GetFishGameRobotConfig(){ return m_GameRobotConfig; }
	DWORD GetWriteSec();
	bool CheckVersionAndPathCrc(DWORD VersionID, DWORD PathCrc);
	bool CheckStringIsError(TCHAR* pStr, DWORD MinLength, DWORD MaxLength, StringCheckType pType);
	bool CheckInWhiteList(const int64& uid);
private:
	bool LoadFishTableConfig(WHXmlNode* pFishConfig);
	bool LoadFishUpdateConfig(WHXmlNode* pFishConfig);
	bool LoadFishItemConfig(WHXmlNode* pFishConfig);
	bool LoadFishRewardConfig(WHXmlNode* pFishConfig);
	bool LoadFishSystemConfig(WHXmlNode* pFishConfig);
	bool LoadFishErrorStringFile(const char* FilePath);
	bool GetStrIndeof(TCHAR* pStr, TCHAR* FindStr);
	bool LoadFishVipConfig(WHXmlNode* pFishConfig);
	bool LoadFishGameRobotConfig(WHXmlNode* pFishConfig);
	bool LoadFishWhiteList(WHXmlNode* pFishConfig);
private:
	tagTableConfig				m_TableConfig;
	tagFishServerUpdate			m_FishUpdateConfig;
	//tagGlobelShop				m_GlobelShopConfig;
	tagRewardMap				m_FishRewardConfig;
	HashMap<DWORD, tagItemConfig>	m_ItemMap;
	tagFishSystemInfo			m_SystemConfig;
	//std::vector<TCHAR*>			m_ErrorStr;//屏蔽字
	tagErrorMap					m_ErrorStr;

	tagVipConfig				m_VipMap;
	tagGameRobotInfo			m_GameRobotConfig;
	tagFishWhiteList			m_WhiteList; //白名单
};