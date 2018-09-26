#include "stdafx.h"
#include "Config.h"
#if !SIMPLE_SERVER_MODE
#include "..\Role.h"
#include "..\TableManager.h"
#endif
#include"FishScene\FishResManager.h"
#ifndef ASSERT
#define ASSERT assert
#endif
#include "FishScene\Bullet.h"
#include "FishScene\Fish.h"
#include"FishScene\tinyxml2.h"
#include"..\FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
const float SPEED_SCALE = 1.0f / 255;
enum CANNON_TYPE
{
	CANNON_1,  //pao1
	CANNON_2,  //pao 2
	CANNON_3,  //pao 3
	CANNON_4,  //bin dong 
	CANNON_5,  //chuan tou
};

float SystemControl::sysMaxWin = 0.0f;
float SystemControl::sysMinWin = 0.0f;
float SystemControl::minPerKillHp = 0.0f;
map<int, vector<float> > SystemControl::item = map<int, vector<float> >();

//float Con_Energy::s_energyratio = 0;
int Con_Combo::s_sustaintime = 0;
int Con_Combo::s_buff_cycle = 0;
float Con_Combo::s_bufferchance;

float Con_Cannon::s_ratiofactor = 0;
float Con_Cannon::s_maxratio = 0;
float Con_Cannon::s_finalratio = 0;
float Con_Cannon::s_unlockratio = 0;

float Con_Fish::s_revies_Activity = 0;
int Con_Fish::s_flashfishid;
int Con_Fish::s_flashfishCatch;
int Con_Fish::s_flashfishradius;

const float PRECISION = 0.0001f;//控制精度
const float ENLARGE = 100000.0f;

int Con_LuckItem::s_threshold;
float Con_LuckItem::s_base;
float Con_LuckItem::s_part;


using namespace tinyxml2;
bool Attribute(const XMLElement*pElement, const char*pName, int&nvalue)
{
	nvalue = 0;
	const char*pItem = pElement->Attribute(pName);
	if (pItem)
	{
		nvalue = atol(pItem);
		return true;
	}
	return false;
}
bool Attribute(const XMLElement*pElement, const char*pName, int64&nvalue)
{
	nvalue = 0;
	const char*pItem = pElement->Attribute(pName);
	if (pItem)
	{
		nvalue = atoll(pItem);
		return true;
	}
	return false;
}
bool Attribute(const XMLElement*pElement, const char*pName, float&fvalue)
{
	fvalue = 0;
	const char*pItem = pElement->Attribute(pName);
	if (pItem)
	{
		fvalue = static_cast<float>(atof(pItem));
		return true;
	}
	return false;
}

CConfig::CConfig(TableManager *pTableManager)
{
	{
		m_Energy.clear();
		m_Skill.clear();
		m_Rate.clear();
		m_Cannon.clear();
		m_Fish.clear();
		m_WinGlobe.clear();
	}
	m_nServerVersion = 0;
	m_pTableManager = pTableManager;
}

CConfig::~CConfig()
{
}

bool CConfig::LoadConfig(char szDir[])
{
	tinyxml2::XMLDocument xml_doc;
	char szConfig[MAX_PATH] = { 0 };
	sprintf_s(szConfig, sizeof(szConfig), "%s\\config.xml", szDir);

	if (xml_doc.LoadFile(szConfig) != XML_NO_ERROR) return false;

	XMLElement* xml_element = xml_doc.FirstChildElement("Config");
	if (xml_element == NULL) return false;

	const XMLElement* xml_child = NULL;
	for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement())
	{
		if (!strcmp(xml_child->Value(), "ServerVersion"))
		{
			Attribute(xml_child, "version", m_nServerVersion);

			int nMaxBullet;
			int nMaxFish;
			Attribute(xml_child, "maxbullet", nMaxBullet);
			Attribute(xml_child, "maxfish", nMaxFish);
		}
		else if (!strcmp(xml_child->Value(), "SystemControl"))
		{
			Attribute(xml_child, "sysMinWin", SystemControl::sysMinWin);
			Attribute(xml_child, "sysMaxWin", SystemControl::sysMaxWin);
			Attribute(xml_child, "minPercentKillHp", SystemControl::minPerKillHp);
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				int id = 0;
				vector<float> tempdata;
				float temp;
				Attribute(xml_data, "id", id);
				Attribute(xml_data, "min", temp);
				tempdata.push_back(temp);
				Attribute(xml_data, "max", temp);
				tempdata.push_back(temp);
				SystemControl::item[id] = tempdata;
			}
		}
		else if (!strcmp(xml_child->Value(), "Combo"))
		{
			Attribute(xml_child, "sustain_time", Con_Combo::s_sustaintime);
			Attribute(xml_child, "buff_cycle", Con_Combo::s_buff_cycle);
			Attribute(xml_child, "bufferchance", Con_Combo::s_bufferchance);						
		}
		else if (!strcmp(xml_child->Value(), "Energy"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Energy  energy_item = { 0 };
				Attribute(xml_data, "id", energy_item.nid);


				Attribute(xml_data, "cditme", energy_item.ncdtime);
				Attribute(xml_data, "mincatch", energy_item.nmincatch);
				Attribute(xml_data, "maxcatch", energy_item.nmaxcatch);
				Attribute(xml_data, "revise", energy_item.nrevise);
				Attribute(xml_data, "energyratio", energy_item.denergyratio);

				Attribute(xml_data, "radius", energy_item.nradius);

				//xml_data->Attribute("playtime", energy_item.nplaytime);
				Attribute(xml_data, "speed", energy_item.npseed);
				Attribute(xml_data, "time1", energy_item.ntime1);
				Attribute(xml_data, "time2", energy_item.ntime2);
				Attribute(xml_data, "time3", energy_item.ntime3);

				AddElemByid(m_Energy, energy_item);
				//m_Energy.push_back(energy_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Skill"))
		{

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Skill skill_item = { 0 };
				Attribute(xml_data, "id", skill_item.nid);

				Attribute(xml_data, "cditme", skill_item.ncdtime);
				Attribute(xml_data, "mincatch", skill_item.nmincatch);
				Attribute(xml_data, "maxcatch", skill_item.nmaxcatch);
				Attribute(xml_data, "revise", skill_item.nrevise);

				Attribute(xml_data, "goodsid", skill_item.ngoodsid);
				Attribute(xml_data, "multiple", skill_item.multiple);


				char *pInfo = const_cast<char*>(xml_data->Attribute("goodsconsume"));
				while (pInfo&&*pInfo)
				{
					SkillConsume consume = { 0 };
					consume.byorder = ConvertIntToBYTE(strtol(pInfo, &pInfo, 10));
					consume.byCount = ConvertIntToBYTE(strtol(pInfo, &pInfo, 10));
					skill_item.vecConsume.push_back(consume);
				}

				Attribute(xml_data, "playtime", skill_item.nplaytime);
				Attribute(xml_data, "speed", skill_item.npseed);
				Attribute(xml_data, "time1", skill_item.ntime1);
				Attribute(xml_data, "time2", skill_item.ntime2);
				Attribute(xml_data, "time3", skill_item.ntime3);

				AddElemByid(m_Skill, skill_item);
				//m_Skill.push_back(skill_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Rate"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Rate rate_item = { 0 };
				Attribute(xml_data, "id", rate_item.nid);
				Attribute(xml_data, "value", rate_item.nValue);
				Attribute(xml_data, "unlock", rate_item.nUnlock);
				Attribute(xml_data, "unlockreward", rate_item.unlockreward);
				Attribute(xml_data, "drop", rate_item.drop);
				//m_Rate.push_back(rate_item);
				AddElemByid(m_Rate, rate_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Cannon"))
		{
			Attribute(xml_child, "ratiofactor", Con_Cannon::s_ratiofactor);
			Attribute(xml_child, "maxratio", Con_Cannon::s_maxratio);
			Attribute(xml_child, "finalration", Con_Cannon::s_finalratio);
			Attribute(xml_child, "unlockratio", Con_Cannon::s_unlockratio);
			
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Cannon cannon_item = { 0 };
				Attribute(xml_data, "id", cannon_item.nid);

				Attribute(xml_data, "bulletspeed", cannon_item.bulletspeed);		

				Attribute(xml_data, "launcherinterval", cannon_item.dlauncherinterval);

				Attribute(xml_data, "consume", cannon_item.nconsume);
				Attribute(xml_data, "radius", cannon_item.nradius);
				Attribute(xml_data, "catchradius", cannon_item.ncatchradius);
				Attribute(xml_data, "maxcatch", cannon_item.nmaxcatch);

				Attribute(xml_data, "itemid", cannon_item.nItemId);
				Attribute(xml_data, "itemcount", cannon_item.nItemCount);
				Attribute(xml_data, "skill", cannon_item.nskill);			
				//
				Attribute(xml_data, "speed", cannon_item.npseed);
				Attribute(xml_data, "time1", cannon_item.ntime1);
				Attribute(xml_data, "time2", cannon_item.ntime2);
				Attribute(xml_data, "time3", cannon_item.ntime3);
				Attribute(xml_data, "power", cannon_item.power);

				//读取多次碰撞几率
				char* pCollision = const_cast<char*>(xml_data->Attribute("collisionrate"));
				while (pCollision&&*pCollision)
				{
					cannon_item.vCollision.push_back(static_cast<float>(strtod(pCollision, &pCollision)));
				}
				//m_Cannon.push_back(cannon_item);
				AddElemByid(m_Cannon, cannon_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Fish"))
		{
			Attribute(xml_child, "revies_Activity", Con_Fish::s_revies_Activity);
			Attribute(xml_child,"flashfishid", Con_Fish::s_flashfishid);
			Attribute(xml_child,"flashfishCatch", Con_Fish::s_flashfishCatch);
			Attribute(xml_child,"flashfishradius", Con_Fish::s_flashfishradius);

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Fish fish_item = { 0 };
				Attribute(xml_data, "id", fish_item.nid);
				Attribute(xml_data, "value", fish_item.nvalue);
				Attribute(xml_data, "maxcount", fish_item.maxcount);
				//Attribute(xml_data, "reviseratio", fish_item.reviseratio);
				Attribute(xml_data, "chance", fish_item.chance);
				Attribute(xml_data, "type", fish_item.type);
				Attribute(xml_data, "flashchance", fish_item.flashchance);			
				Attribute(xml_data, "SpecialRatio", fish_item.specialRate);
				char* pfishrate = const_cast<char*>(xml_data->Attribute("fishrate"));
				while (pfishrate&&*pfishrate)
				{
					fish_item.fishrate.push_back((float)strtod(pfishrate, &pfishrate));
				}

				//m_Fish.push_back(fish_item);
				AddElemByid(m_Fish, fish_item);

			}
		}
		else if (!strcmp(xml_child->Value(), "WinGlobe"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_WinGolbe item = { 0 };

				Attribute(xml_data, "id", item.nid);

				Attribute(xml_data, "money", item.money);
				Attribute(xml_data, "reviseratio", item.reviseratio);
				AddElemByid(m_WinGlobe, item);
			}
		}
		else  if (!strcmp(xml_child->Value(), "FishDeadTime"))//
		{
			DeadStayTime temp = { 0 };
			Attribute(xml_child, "TianZai_DouDong", temp.TianZai_DouDong_Time);           //天灾抖动时间

			Attribute(xml_child, "TianZai_Stay_Min", temp.TianZai_Stay_Time1);             //天灾等待陨石降落停留最短时间
			Attribute(xml_child, "TianZai_Stay_Max", temp.TianZai_Stay_Time2);             //天灾等待陨石降落停留最长时间

			Attribute(xml_child, "TianZai_Dead_Min", temp.TianZai_Dead_Time1);             //天灾死亡停留最短时间
			Attribute(xml_child, "TianZai_Dead_Max", temp.TianZai_Dead_Time2);             //天灾死亡停留最长时间

			Attribute(xml_child, "Bullet_BingDong_Dead_Min", temp.Bullet_BingDong_Dead_Time1);     //冰冻炮击中死亡停留最短时间
			Attribute(xml_child, "Bullet_BingDong_Dead_Max", temp.Bullet_BingDong_Dead_Time2);     //冰冻炮击中死亡停留长短时间

			Attribute(xml_child, "BingDong_Dead_Min", temp.BingDong_Dead_Time1);            //冰冻技能死亡最短时间
			Attribute(xml_child, "BingDong_Dead_Max", temp.BingDong_Dead_Time2);            //冰冻技能死亡最长时间

			Attribute(xml_child, "ShanDian_Dead_Min", temp.ShanDian_Dead_Time1);            //闪电技能死亡最短时间
			Attribute(xml_child, "ShanDian_Dead_Max", temp.ShanDian_Dead_Time2);            //闪电技能死亡最长时间

			Attribute(xml_child, "LongJuanFeng_Dead_Min", temp.LongJuanFeng_Dead_Time1);        //龙卷风技能死亡最短时间
			Attribute(xml_child, "LongJuanFeng_Dead_Max", temp.LongJuanFeng_Dead_Time2);        //龙卷风技能死亡最长时间

			Attribute(xml_child, "JiGuang_Stay_Time", temp.JiGuang_Stay_Time);
			Attribute(xml_child, "JiGuang_Dead_Min", temp.JiGuang_Dead_Time1);             //激光大招死亡最短时间
			Attribute(xml_child, "JiGuang_Dead_Max", temp.JiGuang_Dead_Time2);             //激光大招死亡最长时间

			Attribute(xml_child, "ShandDian_Speed", temp.ShandDian_Speed);
			Attribute(xml_child, "JiGuang_Speed", temp.JiGuang_Speed);
			Attribute(xml_child, "JiGuang_BingDong_Speed", temp.JiGuang_BingDong_Speed);
			Attribute(xml_child, "TianZai_Speed", temp.TianZai_Speed);
			Attribute(xml_child, "BingDong_Speed", temp.BingDong_Speed);

			FishResManager::Inst()->SetDeadStayTime(temp);

		}
	}
	if (m_Energy.size() != MAX_LAUNCHER_NUM || m_Cannon.size() != MAX_LAUNCHER_NUM)
	{
		cout << "MAX_LAUNCHER_NUM" << endl;
		return false;
	}
	if (m_Skill.size() != SKILL_MAX)
	{
		cout << "SKILL_MAX" << endl;
		return false;
	}
	/*if (m_Fish.size() != FISH_TYPES)
	{
		cout << "FISH_TYPES" << endl;
		return false;
	}*/	

	sort(m_Energy.begin(), m_Energy.end(), SortByid<Con_Energy>);
	sort(m_Skill.begin(), m_Skill.end(), SortByid<Con_Skill>);
	sort(m_Rate.begin(), m_Rate.end(), SortByid<Con_Rate>);
	sort(m_Cannon.begin(), m_Cannon.end(), SortByid<Con_Cannon>);
	sort(m_Fish.begin(), m_Fish.end(), SortByid<Con_Fish>);

	int nFishs = 0;
	int nFishValues = 0;
	for (Fish_Vec::iterator it = m_Fish.begin(); it != m_Fish.end(); it++)
	{
		nFishs += it->maxcount;
		nFishValues += it->maxcount*it->nvalue;
	}

	for (Energy_Vec::iterator it = m_Energy.begin(); it != m_Energy.end(); it++)
	{
		Con_Energy &item = *it;
		item.nincome = ConvertFloatToInt((item.nmincatch + item.nmaxcatch) / 2.0f / nFishs*nFishValues + item.nrevise);
		item.nthreshold = ConvertFloatToInt(item.nincome*item.denergyratio);//大招
	}

	for (Skill_Vec::iterator it = m_Skill.begin(); it != m_Skill.end(); it++)
	{
		Con_Skill &item = *it;
		item.nincome = ConvertFloatToInt((item.nmincatch + item.nmaxcatch) / 2.0f / nFishs*nFishValues + item.nrevise);
	}

	//read goods
	//if (!xml_doc.LoadFile(szGood, TIXML_ENCODING_UTF8)) return false;
	//xml_element = xml_doc.FirstChildElement("Goods");
	//if (xml_element == NULL) return false;
	//for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement())
	//{
	//	FISH_GOODS item = { 0 };
	//	xml_child->Attribute("id", &item.nid);
	//	//xml_child->Attribute("name", item.chname);
	//	xml_child->Attribute("price", &item.nprice);
	//	
	//	m_vecgood.push_back(item);
	//}
	//sort(m_vecgood.begin(), m_vecgood.end(), SortByid<FISH_GOODS>);

	return true;
}

//pei zhi
UINT  CConfig::GetVersion()
{
	return m_nServerVersion;
}

float CConfig::BulletRadius(USHORT bulletID)				//子弹基本半径
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].nradius;
}

float CConfig::BulletCatchRadius(USHORT bulletID)					//子弹碰撞之后的捕获半径
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].ncatchradius;
}

UINT  CConfig::MaxCatch(USHORT bulletID)				//子弹的最大捕鱼数量
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].nmaxcatch;
}

float CConfig::CatchChance(PlayerID player, int maxCatch, Fish* pFish, Bullet* pBullet)
{
#if SIMPLE_SERVER_MODE
	return 0.5f;
#else
	USHORT bulletType = pBullet->BulletType;
	if (bulletType >= m_Cannon.size())
	{
		return 0;
	}
	if (pFish->FishType >= m_Fish.size())
	{
		return 0;
	}

	CRole *pPlayer = m_pTableManager->SearchUser(player);
	if (pPlayer)
	{
		//获取鱼血量信息
		BYTE seatID = pBullet->BulletID >> 8;
		const float& maxHp = pFish->fishHp[pBullet->RateIndex].maxHp;
		float& attackHp = pFish->fishHp[pBullet->RateIndex].attackHp;
		float& seatAttackHp = pFish->fishHp[pBullet->RateIndex].seatAttack[seatID];
		//增加伤害
		float addAttack = (1.0f * BulletMultiple(pBullet->RateIndex)  //子弹倍率
			* m_Cannon[bulletType].nconsume					          //大炮基础value
			* m_Cannon[bulletType].power / maxCatch		              //大炮威力
			* ((pBullet->LockFishID == 0 || pPlayer->GetRoleExInfo()->IsRobot()) ? Con_Cannon::s_unlockratio : 1) //不锁定增加威力
			* WinGolbeRate(pPlayer->GetRoleExInfo()->GetRoleGameData().GetTurnLoseWinGoldByTable(pPlayer->GetTableType()))//玩家收益控制
			* pBullet->ExtraPower      //狂暴额外攻击力加成
			);
		attackHp += addAttack;
		seatAttackHp += addAttack;
		//计算该玩家其他倍率总伤害
		float totalAttack = 0;
		for (int i = 0; i<RateCount(); ++i)
		{
			if (i != pBullet->RateIndex)
			{
				totalAttack += pFish->fishHp[i].seatAttack[seatID];
			}
		}
		//防止补刀，需要玩家伤害达到最小比率
		if (SystemControl::minPerKillHp >(totalAttack + seatAttackHp) / maxHp)
		{
			return 0;
		}
		//判定是否可以击杀
		if ((attackHp + totalAttack) < maxHp) //总伤害是否大于了鱼的最大血量
		{
			return 0;
		}
		else 
		{
			return 1;
		}
	}
	return 0;
#endif
}


SkillFailedType  CConfig::UseSkill(PlayerID playerID, SkillType skill)
{
#if SIMPLE_SERVER_MODE
	return SFT_OK;
#else
	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (!pUser)
	{
		return SFT_INVALID;
	}

	if ((size_t)skill >= m_Skill.size())
	{
		return SFT_INVALID;
	}
	return pUser->UseSkill(skill);
#endif
}

LaunchFailedType  CConfig::UseLaser(PlayerID playerID, byte launcherType)
{
#if SIMPLE_SERVER_MODE
	return true;
#else
	ASSERT(launcherType < m_Energy.size());

	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (!pUser)
	{
		return LFT_INVALID;
	}
	return pUser->UseLaser(launcherType);
#endif
}

void  CConfig::GetSkillRange(SkillType skill, int &minNum, int &maxNum)
{
	ASSERT(static_cast<DWORD>(skill) < m_Skill.size());
	minNum = m_Skill[skill].nmincatch;
	maxNum = m_Skill[skill].nmaxcatch;
}

void  CConfig::GetLaserRange(byte laser, int &minNum, int &maxNum)
{
	ASSERT(laser<m_Energy.size());
	minNum = m_Energy[laser].nmincatch;
	maxNum = m_Energy[laser].nmaxcatch;
}

void  CConfig::GetSkillFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	ASSERT(SKILL_FREEZE<m_Skill.size());
	speedScaling = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime3 / FISH_DURATION_TIME);
}

float CConfig::GetSkillChance(SkillType skill, USHORT fishIndex, BYTE byPackageType)
{
	ASSERT(ConvertIntToDWORD(skill)<m_Skill.size());
	ASSERT(fishIndex<m_Fish.size());
	return  1.0f*m_Skill[skill].nincome / (m_Fish[fishIndex].nvalue* m_Skill[skill].nmaxcatch)*PackageFactor(byPackageType);
}

float CConfig::GetLaserChance(byte launcherType, USHORT fishIndex, BYTE byPackageType)
{
	ASSERT(launcherType<m_Energy.size());
	ASSERT(fishIndex<m_Fish.size());
	return  1.0f*m_Energy[launcherType].nincome / (m_Fish[fishIndex].nvalue* m_Energy[launcherType].nmaxcatch) *PackageFactor(byPackageType);
}

float CConfig::GetLaserRadius(byte launcherType)
{
	ASSERT(launcherType<m_Energy.size());
	return m_Energy[launcherType].nradius * 1.0f;
}

BulletType CConfig::GetBulletType(USHORT bulletID)
{
	if (bulletID == CANNON_5)
	{
		return BULLET_TYPE_PENETRATION;
	}

	if (bulletID == CANNON_4)//???
	{
		return BULLET_TYPE_FREEZE;
	}

	return BULLET_TYPE_NORMAL;
}

void CConfig::GetBulletFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	// san hao pao shi bin dong  ?????
	ASSERT(CANNON_4<m_Cannon.size());
	speedScaling = ConvertFloatToBYTE(m_Cannon[CANNON_4].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime3 / FISH_DURATION_TIME);
}

//
//notify 
//
UINT   CConfig::CatchFish(PlayerID playerID, USHORT fishIndx, CatchType catchType, byte subType, byte byPackageType, byte byRateIndex)
{
#if SIMPLE_SERVER_MODE
	return fishIndx;
#else
	ASSERT(fishIndx<m_Fish.size());

	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (pUser&&fishIndx<m_Fish.size())
	{
		int nIncome = (fishIndx == 25 ? 0 : m_Fish[fishIndx].nvalue);//电鳗不算钱
		int nMultiple = BulletMultiple(byRateIndex);
		if (catchType == CATCH_SKILL)//技能特有倍率
		{
			nMultiple = SkillMultiple(subType);
		}
		pUser->OnCatchFish(catchType, subType, fishIndx, nIncome*nMultiple);
		return nIncome * nMultiple;
	}
	return 0;
#endif
}

USHORT    CConfig::GetCombo(PlayerID playerID, USHORT bulletID)
{
#if SIMPLE_SERVER_MODE
	return 0;
#else
	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (pUser)
	{
		return pUser->Combo(bulletID);
	}
	return 0;
#endif
}
bool CConfig::CanUploadImg(PlayerID playerID, USHORT imgSize)
{
	return true;
}
byte  CConfig::GetSkillCDTime(SkillType st)
{
	ASSERT(ConvertIntToDWORD(st)<m_Skill.size());
	return ConvertFloatToBYTE(m_Skill[st].ncdtime);
}

void  CConfig::GetLaserReduction(byte LauncherType, byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	ASSERT(LauncherType<m_Energy.size());
	speedScaling = ConvertFloatToBYTE(m_Energy[LauncherType].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime3 / FISH_DURATION_TIME);
}

byte  CConfig::GetBulletSpeed(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertIntToBYTE(m_Cannon[LauncherType].bulletspeed); //Con_Cannon::s_bulletspeed;
}

byte  CConfig::GetLauncherInterval(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertFloatToBYTE(m_Cannon[LauncherType].dlauncherinterval / FISH_DURATION_TIME);
}

byte  CConfig::GetLaserCDTime(byte LauncherType)
{
	ASSERT(LauncherType<m_Energy.size());
	return ConvertFloatToBYTE(m_Energy[LauncherType].ncdtime / FISH_DURATION_TIME);
}
UINT  CConfig::LaunchPackage(UINT fishid_packageid)
{
	return 1;
}
byte  CConfig::GetRateIndex(byte seat, PlayerID id)
{
#if SIMPLE_SERVER_MODE
	return 0;
#else
	CRole *pPlayer = m_pTableManager->SearchUser(id);
	if (pPlayer)
	{
		return pPlayer->GetRateIndex();
	}
	return 0;
#endif
}

USHORT CConfig::FishRewardDrop(PlayerID id, BYTE byPackageType,USHORT byType)
{
	return 0;
}


byte  CConfig::BulletConsume(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertIntToBYTE(m_Cannon[LauncherType].nconsume);
}

ushort  CConfig::BulletMultiple(byte byIndex)
{
	ASSERT(byIndex<m_Rate.size());
	return ConvertIntToWORD(m_Rate[byIndex].nValue);
}

float CConfig::CalBaseRatio(BYTE cbCannoIndex, BYTE cbFishIndex)
{
	ASSERT(cbCannoIndex<m_Cannon.size());
	ASSERT(cbFishIndex<m_Fish.size());

	float fratio = 1.0f*m_Cannon[cbCannoIndex].nconsume / m_Fish[cbFishIndex].nvalue*Con_Cannon::s_ratiofactor;
	if (fratio > Con_Cannon::s_maxratio)
	{
		fratio = Con_Cannon::s_maxratio;
	}

	if (cbCannoIndex < m_Fish[cbFishIndex].fishrate.size())
	{
		fratio *= m_Fish[cbFishIndex].fishrate[cbCannoIndex];
	}

	//fratio *= m_Fish[cbFishIndex].reviseratio;
	fratio *= Con_Fish::s_revies_Activity;
	return fratio;
}
float CConfig::CalRandom(float d1, float d2)
{
	if (d1 == d2)
		return d1;
	return d1 + rand() % ((int)((d2 - d1)*ENLARGE)) / ENLARGE;
}



int CConfig::FishCount()
{
	return m_Fish.size();
}

int CConfig::CannonCount()
{
	return m_Cannon.size();
}

int CConfig::SkillCount()
{
	return m_Skill.size();
}

int CConfig::RateCount()
{
	return m_Rate.size();
}

float CConfig::WinGolbeRate(int64 nMoney)
{
	float ret = 1.0f;
	for (WinGolbe_Vec::reverse_iterator it = m_WinGlobe.rbegin(); it != m_WinGlobe.rend(); it++)
	{
		if (nMoney >= it->money * g_FishServer.GetRatioValue() * MONEY_RATIO)
		{
			return it->reviseratio;
		}
		ret = it->reviseratio;
	}
	return ret;
}

int  CConfig::LaserThreshold(byte byIndex)
{
	ASSERT(byIndex<m_Energy.size());
	return m_Energy[byIndex].nthreshold;
}

float  CConfig::LauncherInterval(byte byIndex)
{
	ASSERT(byIndex<m_Cannon.size());
	return m_Cannon[byIndex].dlauncherinterval;
}
float  CConfig::LaserCdTime(byte byIndex)
{
	ASSERT(byIndex<m_Energy.size());
	return m_Energy[byIndex].ncdtime;
}
float  CConfig::SkillCdTime(byte byIndex)
{
	ASSERT(byIndex<m_Skill.size());
	return m_Skill[byIndex].ncdtime;
}

void CConfig::GoodsInfo(int nSkill, int norder, int &nGoodsid, int &nGoodsConsume)
{
	nGoodsid = m_Skill[nSkill].ngoodsid;

	if (m_Skill[nSkill].vecConsume.size() == 0)
	{
		nGoodsConsume = 0;
		return;
	}
	for (size_t i = 0; i < m_Skill[nSkill].vecConsume.size(); i++)
	{
		if (norder < m_Skill[nSkill].vecConsume[i].byorder)
		{
			nGoodsConsume = m_Skill[nSkill].vecConsume[i].byCount; //ngoodsconsume;
			return;
		}
	}

	nGoodsConsume = m_Skill[nSkill].vecConsume.back().byCount;//the last one
}
void CConfig::GoodsInfo(int nCannon, int &nGoodsid, int &nGoodsCount)
{
	ASSERT(ConvertIntToDWORD(nCannon)<m_Cannon.size());
	nGoodsid = m_Cannon[nCannon].nItemId;
	nGoodsCount = m_Cannon[nCannon].nItemCount;
}

bool CConfig::FindCannon(byte nSkill, int& nCannon)
{
	for (Cannon_Vec::iterator it = m_Cannon.begin(); it != m_Cannon.end(); it++)
	{
		if (it->nskill == nSkill)
		{
			nCannon = it->nid;
			return true;
		}
	}
	return false;
}

float CConfig::ComboSustainTime()
{
	return Con_Combo::s_sustaintime* 1.0f;
}
int CConfig::ComboBuffCycle()
{
	return Con_Combo::s_buff_cycle;
}

float CConfig::PackageFactor(BYTE cbPackage)
{	
	return 0.0f;
}
USHORT CConfig::RateUnlock(BYTE byIndex)
{
	ASSERT(byIndex<m_Rate.size());
	return m_Rate[byIndex].nUnlock;
}

ushort CConfig::SkillMultiple(byte byIndex)
{
	ASSERT(byIndex<m_Skill.size());
	return m_Skill[byIndex].multiple;
}
USHORT CConfig::UnlockRateReward(BYTE byIndex)
{
	if (byIndex < m_Rate.size())
	{
		return m_Rate[byIndex].unlockreward;
	}
	return 0;
}

bool CConfig::IsLightingFish(USHORT fishIndex)
{
	return fishIndex == Con_Fish::s_flashfishid;
}
int  CConfig::IsLightingFish(USHORT fishIndex, PlayerID id)
{
	if (IsLightingFish(fishIndex))
	{
		return Con_Fish::s_flashfishCatch;
	}
	return false;
	//如果是闪电鱼，返回大于0的捕获数量
	//return (fishIndex < 8) ? 5 : 0;
}
//某一条鱼是否可以被闪电捕获，fishIndex鱼ID，玩家ID，dist:屏幕距离。
bool CConfig::CanCatchLightingFish(USHORT fishIndex, PlayerID id, float dist)
{
	if (fishIndex >= m_Fish.size())
	{
		return false;		
	}
	return dist<Con_Fish::s_flashfishradius&&RandFloat()<m_Fish[fishIndex].flashchance;
	//return RandRange(0, 10) > 7 && dist < 25;
}
BYTE CConfig::CannonOwnSkill(BYTE byIndex)
{
	if (byIndex < m_Cannon.size())
	{
		return m_Cannon[byIndex].nskill;
	}
	return 255;
}

INT CConfig::GetFishValue(const USHORT fishType)
{
	if (fishType >= m_Fish.size())
	{
		return -1;
	}
	return m_Fish[fishType].nvalue;
}

float CConfig::GetFishSpecialRate(const USHORT fishType)
{
	if (fishType >= m_Fish.size())
	{
		return 1.0f;
	}
	return m_Fish[fishType].specialRate;
}

bool CConfig::FishSpecialRateChange(const USHORT fishType, float rate)
{
	if (fishType >= m_Fish.size())
	{
		Log("鱼死亡概率更新配置错误。fishid = %d", fishType);
		return false;
	}
	if (rate != -1)
	{
		m_Fish[fishType].specialRate = rate;
	}
	return true;
}

bool CConfig::FishConnonPowerChange(const UINT id, float power)
{
	if (id >= m_Cannon.size())
	{
		Log("更新大炮威力错误！大炮id=%d", id);
		return false;
	}
	if (power != -1)
	{
		m_Cannon[id].power = power;
	}
	return true;
}