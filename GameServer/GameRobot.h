//游戏机器人
#pragma once
#include "GameTable.h"
class GameRobotManager;
class CRoleEx;

struct tagGameRobotCharm
{
	DWORD		dwUserID;
	BYTE		CharmIndexID;
	WORD		CharmItemSum;
	DWORD		BeginTimeLog;
};
struct tagGameRobotCharmEvent
{
	DWORD		SrcUserID;
	int			CharmValue;
	DWORD		TimeLog;
};
struct tagGameRobotCharmEventList
{
	std::list<tagGameRobotCharmEvent>	 EventList;
	int									 NowChangeCharmValue;
	tagGameRobotCharmEventList()
	{
		EventList.clear();
		NowChangeCharmValue = 0;
	}
	void Clear()
	{
		EventList.clear();
		NowChangeCharmValue = 0;
	}
	int AddCharmEvent(DWORD SrcUserID, int ChanrmValue,DWORD DiffTime)
	{
		DWORD NowTime = timeGetTime();
		std::list<tagGameRobotCharmEvent>::iterator Iter = EventList.begin();
		for (; Iter != EventList.end();)
		{
			if (NowTime - Iter->TimeLog >= DiffTime)
			{
				NowChangeCharmValue -= Iter->CharmValue;
				Iter = EventList.erase(Iter);
			}
			else
				++Iter;
		}
		tagGameRobotCharmEvent pEvent;
		pEvent.CharmValue = ChanrmValue;
		pEvent.SrcUserID = SrcUserID;
		pEvent.TimeLog = NowTime;
		EventList.push_back(pEvent);
		NowChangeCharmValue += pEvent.CharmValue;
		return NowChangeCharmValue;
	}
};
struct tagGameRobotCharmChangeManager
{
	HashMap<DWORD, tagGameRobotCharmEventList>	EventMap;
	void Clear()
	{
		EventMap.clear();
	}
	void Clear(DWORD dwUserID)
	{
		EventMap.erase(dwUserID);
	}
	int AddCharmEvent(DWORD SrcUserID, int ChanrmValue, DWORD DiffTime)
	{
		HashMap<DWORD, tagGameRobotCharmEventList>::iterator Iter = EventMap.find(SrcUserID);
		if (Iter == EventMap.end())
		{
			tagGameRobotCharmEventList pInfo;
			int Value=pInfo.AddCharmEvent(SrcUserID, ChanrmValue, DiffTime);
			EventMap.insert(HashMap<DWORD, tagGameRobotCharmEventList>::value_type(SrcUserID,pInfo));
			return Value;
		}
		else
		{
			return Iter->second.AddCharmEvent(SrcUserID, ChanrmValue, DiffTime);
		}
	}
};
class GameRobot
{
public:
	GameRobot(GameRobotManager* pManager,CRoleEx* pRole);
	virtual ~GameRobot();

	void Update();
	void SetRobotUse(DWORD RobotID);
	bool IsRobotUsed(){ return m_IsUse; };
	void ResetRobot();
	DWORD GetRobotUserID();
	CRoleEx* GetRoleInfo();
private:
	void UpdateRobotRoom(DWORD tickNow);
	void UpdateRobotRate(DWORD tickNow);
	void UpdateRobotSkill();
	void UpdateRobotLauncher(DWORD tickNow);
	void UpdateRobotOpenFire(DWORD tickNow);
	void UpdateRobotIsCanOpenFire(DWORD tickNow);
	void UpdateRobotCharm();
	void UpdateRobotSendCharm();
private:
	GameRobotManager*   m_pManager;
	CRoleEx*			m_pRole;
	DWORD				m_RobotID; //配置的机器人类型ID
	bool				m_IsUse;
	//1.进出房间的记录
	DWORD				m_RoomTimeLog;
	DWORD				m_LeaveTableTimeLog;
	//2.机器人切换倍率
	DWORD				m_RateTimeLog;
	BYTE				m_NowRate;
	DWORD				m_ChangeRateTimeLog;
	//3.机器人使用技能
	DWORD				m_SkillTimeLog;
	DWORD				m_UseSkillTimeLog;
	//4.机器人切换炮台
	DWORD				m_LauncherTimeLog;
	DWORD				m_ChangeLauncherTimeLog;
	//5.机器人开炮
	DWORD				m_OpenFireTimeLog;
	WORD				m_LockFishID;
	DWORD				m_Angle;
	//
	DWORD				m_StopOpenFireTimeLog;
	//
	DWORD				m_RobotInfoTimeLog;
	//
	DWORD				m_RobotOpenFireTimeLog;
	bool				m_IsOpenFire;

	//
	DWORD				m_RobotSendCharmItemTimeLog;
	DWORD				m_dwRobotSendCharmTimeLog;
	list<tagGameRobotCharm> m_vecSendCharmArray;//待发送的魅力集合
	tagGameRobotCharmChangeManager m_RobotCharmEvent;
	//
	DWORD				m_dwSendCharmTineLog;

};
struct tagRobotWrite
{
	WORD		TableID;
	DWORD		TimeLog;
};

class GameRobotManager//游戏机器人管理器
{
public:
	GameRobotManager();
	virtual ~GameRobotManager();
	//加载全部的机器人
	void  OnLoadAllGameRobot(DWORD BeginUserID,DWORD EndUserID);
	void  OnAddRobot(DWORD dwUserID, CRoleEx* pRoleEx);
	//update
	void  Update();
	void  UpdateWriteList();
	GameRobot* GetFreeRobot(DWORD RobotID, GameTable* pTable);//获取一个空闲的机器人
	void ResetGameRobot(DWORD dwUserID);//归还一个机器人
	void ResetAllGameRobot();//归还所有机器人
	bool GameRobotIsCanJoinTable(GameTable* pTable);//判断机器人是否可以进入当前的桌子
	void  OnRoleCreateNormalRoom(GameTable* pTable);//当普通玩家创建一个非比赛房间的时候
	void  OnRoleLeaveNormalRoom(GameTable* pTable);//当普通玩家离开一个非比赛房间的时候
	void  OnRoleJoinNormalRoom(GameTable* pTable);//当普通玩家进入一个非比赛房间的时候
	void  AdddWriteRobot(WORD TableID, DWORD WriteTime); //添加到重新加入房间的机器人队列
private:
	volatile UINT				m_GetIndex;       //获取free
	bool						m_IsLoadFinish;   //加载完成
	DWORD						m_RobotConfigSum; //配置的机器人数量
	vector<GameRobot*>			m_Robots;      //空闲机器人
	list<tagRobotWrite>			m_WriteList;	  //重新回到桌子的机器人队列
};
#define MAX_ROBOTNAME_COUNT 200

static string g_RobotName[MAX_ROBOTNAME_COUNT] = {
	/*1*/    GBKToUTF8("平凡之路1234"),
	/*2*/    GBKToUTF8("丹妮芙22"),
	/*3*/    GBKToUTF8("心碎的蛋"),
	/*4*/    GBKToUTF8("何青蓝"),
	/*5*/    GBKToUTF8("公马真是好"),
	/*6*/    GBKToUTF8("无名小卒8868"),
	/*7*/    GBKToUTF8("忏悔的玫瑰"),
	/*8*/    GBKToUTF8("八月未央不央"),
	/*9*/    GBKToUTF8("跳海躲鱼"),
	/*10*/    GBKToUTF8("灰白马甲"),
	/*11*/    GBKToUTF8("Jerry和小宝"),
	/*12*/    GBKToUTF8("馨馨会小心"),
	/*13*/    GBKToUTF8("我是一只猫呀"),
	/*14*/    GBKToUTF8("幸福花开啦2015"),
	/*15*/    GBKToUTF8("汪小雨ABC"),
	/*16*/    GBKToUTF8("那是一阵风豆豆"),
	/*17*/    GBKToUTF8("逆袭男2016"),
	/*18*/    GBKToUTF8("永远18岁的少女"),
	/*19*/    GBKToUTF8("岁月的守护者"),
	/*20*/    GBKToUTF8("大明二宫主"),
	/*21*/    GBKToUTF8("第一个零点"),
	/*22*/    GBKToUTF8("心乱了2016"),
	/*23*/    GBKToUTF8("lbiubiuily"),
	/*24*/    GBKToUTF8("跳海躲鱼"),
	/*25*/    GBKToUTF8("阿鬼好色"),
	/*26*/    GBKToUTF8("善念伴我行"),
	/*27*/    GBKToUTF8("火炎焱焱火"),
	/*28*/    GBKToUTF8("莲婷"),
	/*29*/    GBKToUTF8("周席周"),
	/*30*/    GBKToUTF8("发达了再说"),
	/*31*/    GBKToUTF8("纯白的世界"),
	/*32*/    GBKToUTF8("禾维123"),
	/*33*/    GBKToUTF8("胖胖南瓜仔"),
	/*34*/    GBKToUTF8("0开始0"),
	/*35*/    GBKToUTF8("感心2016"),
	/*36*/    GBKToUTF8("低潮期的M子"),
	/*37*/    GBKToUTF8("躲鱼猫"),
	/*38*/    GBKToUTF8("麦麦125"),
	/*39*/    GBKToUTF8("迷茫的老男孩"),
	/*40*/    GBKToUTF8("蛇精女yy"),
	/*41*/    GBKToUTF8("菜菜的小锤锤"),
	/*42*/    GBKToUTF8("跳进了火坑"),
	/*43*/    GBKToUTF8("雨飘飘2016"),
	/*44*/    GBKToUTF8("梦里花落知多少we"),
	/*45*/    GBKToUTF8("马拉个币2016"),
	/*46*/    GBKToUTF8("闽子2011"),
	/*47*/    GBKToUTF8("烂人一个I"),
	/*48*/    GBKToUTF8("柴禾金子"),
	/*49*/    GBKToUTF8("冬已到来2016"),
	/*50*/    GBKToUTF8("jinlai999"),
	/*51*/    GBKToUTF8("天天乐乐儿"),
	/*52*/    GBKToUTF8("先生家的星星"),
	/*53*/    GBKToUTF8("菜神来袭"),
	/*54*/    GBKToUTF8("风爱花易残"),
	/*55*/    GBKToUTF8("小喜鹊HD"),
	/*56*/    GBKToUTF8("温玉兰"),
	/*57*/    GBKToUTF8("大大的马甲"),
	/*58*/    GBKToUTF8("我就是那个零点五"),
	/*59*/    GBKToUTF8("枫林主人"),
	/*60*/    GBKToUTF8("沧海一粟qwe"),
	/*61*/    GBKToUTF8("沁如心扉"),
	/*62*/    GBKToUTF8("周防尊1994"),
	/*63*/    GBKToUTF8("家在哪儿呢2016"),
	/*64*/    GBKToUTF8("北岸浮生高"),
	/*65*/    GBKToUTF8("小草被踩"),
	/*66*/    GBKToUTF8("我爱狐狸皮大氅"),
	/*67*/    GBKToUTF8("婆婆应不应该来"),
	/*68*/    GBKToUTF8("iam繁星点点"),
	/*69*/    GBKToUTF8("心随风飞心随风飞"),
	/*70*/    GBKToUTF8("灵悟师傅"),
	/*71*/    GBKToUTF8("暖暖的阳光2016"),
	/*72*/    GBKToUTF8("A疯人院的疯言疯"),
	/*73*/    GBKToUTF8("我的密斯特D"),
	/*74*/    GBKToUTF8("我的未来不靠谱"),
	/*75*/    GBKToUTF8("桔梗的眼泪"),
	/*76*/    GBKToUTF8("安臣若好"),
	/*77*/    GBKToUTF8("冬宝呵呵侔"),
	/*78*/    GBKToUTF8("writers_"),
	/*79*/    GBKToUTF8("风流无关男女"),
	/*80*/    GBKToUTF8("耶里雅"),
	/*81*/    GBKToUTF8("佳哥么么哒"),
	/*82*/    GBKToUTF8("妖娆的小屁股"),
	/*83*/    GBKToUTF8("穿高跟鞋的小草莓"),
	/*84*/    GBKToUTF8("把我还给我Y"),
	/*85*/    GBKToUTF8("fangque123"),
	/*86*/    GBKToUTF8("杨光lonely"),
	/*87*/    GBKToUTF8("搬砖的妹子"),
	/*88*/    GBKToUTF8("剩得没法再剩了"),
	/*89*/    GBKToUTF8("中医朱进忠"),
	/*90*/    GBKToUTF8("有秘密的小可	"),
	/*91*/    GBKToUTF8("海z润"),
	/*92*/    GBKToUTF8("续写春秋"),
	/*93*/    GBKToUTF8("马兰花开2016	"),
	/*94*/    GBKToUTF8("努力的小蹦跶"),
	/*95*/    GBKToUTF8("然后释然"),
	/*96*/    GBKToUTF8("男人带宝宝"),
	/*97*/    GBKToUTF8("木小臻"),
	/*98*/    GBKToUTF8("亲情不可替代	"),
	/*99*/    GBKToUTF8("曾经多美好wwl"),
	/*100*/    GBKToUTF8("执着ying"),
	/*101*/    GBKToUTF8("Zyydrw"),
	/*102*/    GBKToUTF8("超级灰侠"),
	/*103*/    GBKToUTF8("换个马甲想吐槽"),
	/*104*/    GBKToUTF8("皓亨耿"),
	/*105*/    GBKToUTF8("奔跑的跳蚤蚤	"),
	/*106*/    GBKToUTF8("苏姑娘晚安"),
	/*107*/    GBKToUTF8("shaqt"),
	/*108*/    GBKToUTF8("阿男本色"),
	/*109*/    GBKToUTF8("温软的子罗1119"),
	/*110*/    GBKToUTF8("一个欢乐的逗比星"),
	/*111*/    GBKToUTF8("你一定会来吗"),
	/*112*/    GBKToUTF8("我是那个小米"),
	/*113*/    GBKToUTF8("爱佳缘99"),
	/*114*/    GBKToUTF8("洋葱与泡沫"),
	/*115*/    GBKToUTF8("爱与诚520"),
	/*116*/    GBKToUTF8("民间风水先生"),
	/*117*/    GBKToUTF8("最恨出轨妻"),
	/*118*/    GBKToUTF8("左梦竹"),
	/*119*/    GBKToUTF8("时光倒转2016"),
	/*120*/    GBKToUTF8("海上月蒙蒙"),
	/*121*/    GBKToUTF8("水心丝语"),
	/*122*/    GBKToUTF8("微微小微"),
	/*123*/    GBKToUTF8("一华独秀"),
	/*124*/    GBKToUTF8("七月天热"),
	/*125*/    GBKToUTF8("一颗孤零零的小草"),
	/*126*/    GBKToUTF8("童十七"),
	/*127*/    GBKToUTF8("偶滴个神啊啊啊"),
	/*128*/    GBKToUTF8("昙花"),
	/*129*/    GBKToUTF8("无名氏飘飘"),
	/*130*/    GBKToUTF8("黄金马铃薯派"),
	/*131*/    GBKToUTF8("小酒窝长睫毛CXY"),
	/*132*/    GBKToUTF8("柳树成荫1940"),
	/*133*/    GBKToUTF8("我的尧尧"),
	/*134*/    GBKToUTF8("二支访蕊"),
	/*135*/    GBKToUTF8("橙子被抢"),
	/*136*/    GBKToUTF8("夕夜1993"),
	/*137*/    GBKToUTF8("坚强的鬼点子"),
	/*138*/    GBKToUTF8("谷育"),
	/*139*/    GBKToUTF8("窝只是个女孩纸"),
	/*140*/    GBKToUTF8("循环2010"),
	/*141*/    GBKToUTF8("凉尘"),
	/*142*/    GBKToUTF8("芋头馒头"),
	/*143*/    GBKToUTF8("权权娜爱吃鱼"),
	/*144*/    GBKToUTF8("敏公子pp"),
	/*145*/    GBKToUTF8("说不出N多痛苦"),
	/*146*/    GBKToUTF8("图图述心事"),
	/*147*/    GBKToUTF8("鸵鸟de我"),
	/*148*/    GBKToUTF8("右盼顾左"),
	/*149*/    GBKToUTF8("你不像你"),
	/*150*/    GBKToUTF8("小晶么么"),
	/*151*/    GBKToUTF8("至少你有过真心"),
	/*152*/    GBKToUTF8("何来何往54"),
	/*153*/    GBKToUTF8("难道不懂爱"),
	/*154*/    GBKToUTF8("万里无云yi"),
	/*155*/    GBKToUTF8("乐呵呵5268"),
	/*156*/    GBKToUTF8("西夏出凉"),
	/*157*/    GBKToUTF8("烧货欧巴"),
	/*158*/    GBKToUTF8("沉莫2015"),
	/*159*/    GBKToUTF8("侯多多王闹闹"),
	/*160*/    GBKToUTF8("若初某谋"),
	/*161*/    GBKToUTF8("追忆岁月悠悠"),
	/*162*/    GBKToUTF8("一抹小橄榄"),
	/*163*/    GBKToUTF8("让一切随风而去吧"),
	/*164*/    GBKToUTF8("飞天龙蛋"),
	/*165*/    GBKToUTF8("大漠醉红尘"),
	/*166*/    GBKToUTF8("欢乐花果山"),
	/*167*/    GBKToUTF8("马桶惊呆了"),
	/*168*/    GBKToUTF8("很爱很爱你"),
	/*169*/    GBKToUTF8("猪肉卷比爱永恒"),
	/*170*/    GBKToUTF8("叶御寒"),
	/*171*/    GBKToUTF8("燃烧的弱水"),
	/*172*/    GBKToUTF8("安然豬"),
	/*173*/    GBKToUTF8("天涯情感"),
	/*174*/    GBKToUTF8("70后大叔的困惑"),
	/*175*/    GBKToUTF8("追忆岁月悠悠"),
	/*176*/    GBKToUTF8("平凡谢93"),
	/*177*/    GBKToUTF8("过来过来人"),
	/*178*/    GBKToUTF8("旧城不暖少年心"),
	/*179*/    GBKToUTF8("写手未成年"),
	/*180*/    GBKToUTF8("Henkiki"),
	/*181*/    GBKToUTF8("阳光的猫精"),
	/*182*/    GBKToUTF8("百里夫人"),
	/*183*/    GBKToUTF8("追忆岁月悠悠"),
	/*184*/    GBKToUTF8("随风飘落花瓣"),
	/*185*/    GBKToUTF8("你若死我怎活"),
	/*186*/    GBKToUTF8("追男神专用"),
	/*187*/    GBKToUTF8("ribendadan"),
	/*188*/    GBKToUTF8("chunyzi2016"),
	/*189*/    GBKToUTF8("小巫仙忽忽"),
	/*190*/    GBKToUTF8("小萝莉爱大叔"),
	/*191*/    GBKToUTF8("秋天lovely"),
	/*192*/    GBKToUTF8("帅哥小白龙"),
	/*193*/    GBKToUTF8("我叫孔飞龙乐乐"),
	/*194*/    GBKToUTF8("哈哈哈1996"),
	/*195*/    GBKToUTF8("时间沧海一瞬息"),
	/*196*/    GBKToUTF8("差一点0错爱"),
	/*197*/    GBKToUTF8("灿烂的美好的"),
	/*198*/    GBKToUTF8("肉包_1001"),
	/*199*/    GBKToUTF8("汪忻"),
	/*200*/    GBKToUTF8("独恋一枝春")
};