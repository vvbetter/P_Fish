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
	int AddCharmEvent(DWORD SrcUserID, int ChanrmValue, DWORD DiffTime)
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
			int Value = pInfo.AddCharmEvent(SrcUserID, ChanrmValue, DiffTime);
			EventMap.insert(HashMap<DWORD, tagGameRobotCharmEventList>::value_type(SrcUserID, pInfo));
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
	GameRobot(GameRobotManager* pManager, CRoleEx* pRole);
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
	BYTE                m_FishValue;
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
	void  OnLoadAllGameRobot(DWORD BeginUserID, DWORD EndUserID);
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
	bool JoinRobot(DWORD robotTypeID, GameTable* pTable);
private:
	volatile UINT				m_GetIndex;       //获取free
	bool						m_IsLoadFinish;   //加载完成
	DWORD						m_RobotConfigSum; //配置的机器人数量
	vector<GameRobot*>			m_Robots;      //空闲机器人
	list<tagRobotWrite>			m_WriteList;	  //重新回到桌子的机器人队列
};
#define MAX_ROBOTNAME_COUNT 600

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
	/*200*/    GBKToUTF8("独恋一枝春"),
	/*201*/		GBKToUTF8("游客30886"),
	/*202*/		GBKToUTF8("游客36915"),
	/*203*/		GBKToUTF8("游客38335"),
	/*204*/		GBKToUTF8("会员60492"),
	/*205*/		GBKToUTF8("游客41421"),
	/*206*/		GBKToUTF8("会员90027"),
	/*207*/		GBKToUTF8("会员20059"),
	/*208*/		GBKToUTF8("游客13926"),
	/*209*/		GBKToUTF8("会员83426"),
	/*210*/		GBKToUTF8("会员55736"),
	/*211*/		GBKToUTF8("游客95368"),
	/*212*/		GBKToUTF8("游客56429"),
	/*213*/		GBKToUTF8("会员21530"),
	/*214*/		GBKToUTF8("会员65123"),
	/*215*/		GBKToUTF8("游客3135"),
	/*216*/		GBKToUTF8("游客79802"),
	/*217*/		GBKToUTF8("会员23058"),
	/*218*/		GBKToUTF8("游客98167"),
	/*219*/		GBKToUTF8("游客18456"),
	/*220*/		GBKToUTF8("游客78042"),
	/*221*/		GBKToUTF8("游客77373"),
	/*222*/		GBKToUTF8("游客44919"),
	/*223*/		GBKToUTF8("会员98537"),
	/*224*/		GBKToUTF8("会员94324"),
	/*225*/		GBKToUTF8("游客64370"),
	/*226*/		GBKToUTF8("游客3526"),
	/*227*/		GBKToUTF8("游客68980"),
	/*228*/		GBKToUTF8("会员41873"),
	/*229*/		GBKToUTF8("会员99170"),
	/*230*/		GBKToUTF8("会员97281"),
	/*231*/		GBKToUTF8("游客20925"),
	/*232*/		GBKToUTF8("会员36327"),
	/*233*/		GBKToUTF8("会员26505"),
	/*234*/		GBKToUTF8("会员21729"),
	/*235*/		GBKToUTF8("游客25857"),
	/*236*/		GBKToUTF8("会员53895"),
	/*237*/		GBKToUTF8("会员545"),
	/*238*/		GBKToUTF8("会员33367"),
	/*239*/		GBKToUTF8("会员90364"),
	/*240*/		GBKToUTF8("游客13750"),
	/*241*/		GBKToUTF8("游客26808"),
	/*242*/		GBKToUTF8("会员47178"),
	/*243*/		GBKToUTF8("会员93584"),
	/*244*/		GBKToUTF8("游客2651"),
	/*245*/		GBKToUTF8("会员12399"),
	/*246*/		GBKToUTF8("会员95060"),
	/*247*/		GBKToUTF8("会员93368"),
	/*248*/		GBKToUTF8("游客10012"),
	/*249*/		GBKToUTF8("会员98586"),
	/*250*/		GBKToUTF8("会员97539"),
	/*251*/		GBKToUTF8("游客80570"),
	/*252*/		GBKToUTF8("会员60378"),
	/*253*/		GBKToUTF8("游客66601"),
	/*254*/		GBKToUTF8("游客12902"),
	/*255*/		GBKToUTF8("游客70492"),
	/*256*/		GBKToUTF8("会员60756"),
	/*257*/		GBKToUTF8("游客60280"),
	/*258*/		GBKToUTF8("会员9441"),
	/*259*/		GBKToUTF8("游客29689"),
	/*260*/		GBKToUTF8("会员46619"),
	/*261*/		GBKToUTF8("会员44729"),
	/*262*/		GBKToUTF8("游客8117"),
	/*263*/		GBKToUTF8("游客5771"),
	/*264*/		GBKToUTF8("游客90675"),
	/*265*/		GBKToUTF8("游客98927"),
	/*266*/		GBKToUTF8("游客77856"),
	/*267*/		GBKToUTF8("游客72353"),
	/*268*/		GBKToUTF8("会员76965"),
	/*269*/		GBKToUTF8("会员64683"),
	/*270*/		GBKToUTF8("游客28624"),
	/*271*/		GBKToUTF8("会员32871"),
	/*272*/		GBKToUTF8("会员48829"),
	/*273*/		GBKToUTF8("游客30019"),
	/*274*/		GBKToUTF8("会员63368"),
	/*275*/		GBKToUTF8("会员86715"),
	/*276*/		GBKToUTF8("会员18149"),
	/*277*/		GBKToUTF8("会员723"),
	/*278*/		GBKToUTF8("会员2245"),
	/*279*/		GBKToUTF8("会员93451"),
	/*280*/		GBKToUTF8("游客43555"),
	/*281*/		GBKToUTF8("游客97488"),
	/*282*/		GBKToUTF8("会员88228"),
	/*283*/		GBKToUTF8("游客92350"),
	/*284*/		GBKToUTF8("游客41500"),
	/*285*/		GBKToUTF8("会员87764"),
	/*286*/		GBKToUTF8("会员24914"),
	/*287*/		GBKToUTF8("游客75856"),
	/*288*/		GBKToUTF8("游客46491"),
	/*289*/		GBKToUTF8("游客48365"),
	/*290*/		GBKToUTF8("游客81936"),
	/*291*/		GBKToUTF8("会员52551"),
	/*292*/		GBKToUTF8("游客99228"),
	/*293*/		GBKToUTF8("游客75407"),
	/*294*/		GBKToUTF8("会员76121"),
	/*295*/		GBKToUTF8("会员94395"),
	/*296*/		GBKToUTF8("游客61237"),
	/*297*/		GBKToUTF8("游客73793"),
	/*298*/		GBKToUTF8("会员94428"),
	/*299*/		GBKToUTF8("游客31011"),
	/*300*/		GBKToUTF8("会员39529"),
	/*301*/		GBKToUTF8("会员22404"),
	/*302*/		GBKToUTF8("游客55763"),
	/*303*/		GBKToUTF8("游客54538"),
	/*304*/		GBKToUTF8("会员36840"),
	/*305*/		GBKToUTF8("会员44818"),
	/*306*/		GBKToUTF8("会员70688"),
	/*307*/		GBKToUTF8("游客67917"),
	/*308*/		GBKToUTF8("游客66996"),
	/*309*/		GBKToUTF8("会员87743"),
	/*310*/		GBKToUTF8("会员12183"),
	/*311*/		GBKToUTF8("会员95499"),
	/*312*/		GBKToUTF8("会员6725"),
	/*313*/		GBKToUTF8("会员55590"),
	/*314*/		GBKToUTF8("游客68139"),
	/*315*/		GBKToUTF8("会员69786"),
	/*316*/		GBKToUTF8("游客38082"),
	/*317*/		GBKToUTF8("会员88464"),
	/*318*/		GBKToUTF8("游客39507"),
	/*319*/		GBKToUTF8("游客28804"),
	/*320*/		GBKToUTF8("会员78611"),
	/*321*/		GBKToUTF8("会员27828"),
	/*322*/		GBKToUTF8("游客87343"),
	/*323*/		GBKToUTF8("会员35568"),
	/*324*/		GBKToUTF8("会员55422"),
	/*325*/		GBKToUTF8("游客13810"),
	/*326*/		GBKToUTF8("游客21801"),
	/*327*/		GBKToUTF8("游客73730"),
	/*328*/		GBKToUTF8("会员11305"),
	/*329*/		GBKToUTF8("会员78736"),
	/*330*/		GBKToUTF8("会员48626"),
	/*331*/		GBKToUTF8("会员3465"),
	/*332*/		GBKToUTF8("会员73416"),
	/*333*/		GBKToUTF8("会员13258"),
	/*334*/		GBKToUTF8("会员67637"),
	/*335*/		GBKToUTF8("会员5624"),
	/*336*/		GBKToUTF8("会员32036"),
	/*337*/		GBKToUTF8("会员11899"),
	/*338*/		GBKToUTF8("游客45550"),
	/*339*/		GBKToUTF8("会员90071"),
	/*340*/		GBKToUTF8("游客87131"),
	/*341*/		GBKToUTF8("游客84930"),
	/*342*/		GBKToUTF8("游客45894"),
	/*343*/		GBKToUTF8("会员70163"),
	/*344*/		GBKToUTF8("游客87981"),
	/*345*/		GBKToUTF8("游客52996"),
	/*346*/		GBKToUTF8("游客13773"),
	/*347*/		GBKToUTF8("游客39668"),
	/*348*/		GBKToUTF8("会员81095"),
	/*349*/		GBKToUTF8("会员16466"),
	/*350*/		GBKToUTF8("会员11340"),
	/*351*/		GBKToUTF8("会员27684"),
	/*352*/		GBKToUTF8("会员55542"),
	/*353*/		GBKToUTF8("会员79107"),
	/*354*/		GBKToUTF8("游客19756"),
	/*355*/		GBKToUTF8("游客18418"),
	/*356*/		GBKToUTF8("游客89412"),
	/*357*/		GBKToUTF8("会员32172"),
	/*358*/		GBKToUTF8("游客62009"),
	/*359*/		GBKToUTF8("会员25210"),
	/*360*/		GBKToUTF8("会员67587"),
	/*361*/		GBKToUTF8("会员19301"),
	/*362*/		GBKToUTF8("游客67372"),
	/*363*/		GBKToUTF8("游客1255"),
	/*364*/		GBKToUTF8("游客44599"),
	/*365*/		GBKToUTF8("游客29904"),
	/*366*/		GBKToUTF8("游客39811"),
	/*367*/		GBKToUTF8("会员15667"),
	/*368*/		GBKToUTF8("游客46228"),
	/*369*/		GBKToUTF8("游客29150"),
	/*370*/		GBKToUTF8("会员96658"),
	/*371*/		GBKToUTF8("会员89224"),
	/*372*/		GBKToUTF8("会员67269"),
	/*373*/		GBKToUTF8("会员54081"),
	/*374*/		GBKToUTF8("会员40084"),
	/*375*/		GBKToUTF8("会员11972"),
	/*376*/		GBKToUTF8("会员73850"),
	/*377*/		GBKToUTF8("游客5385"),
	/*378*/		GBKToUTF8("会员39299"),
	/*379*/		GBKToUTF8("会员6042"),
	/*380*/		GBKToUTF8("会员40713"),
	/*381*/		GBKToUTF8("会员56190"),
	/*382*/		GBKToUTF8("会员42590"),
	/*383*/		GBKToUTF8("游客8581"),
	/*384*/		GBKToUTF8("游客99336"),
	/*385*/		GBKToUTF8("会员71155"),
	/*386*/		GBKToUTF8("会员18004"),
	/*387*/		GBKToUTF8("游客14769"),
	/*388*/		GBKToUTF8("游客81776"),
	/*389*/		GBKToUTF8("会员47255"),
	/*390*/		GBKToUTF8("会员48142"),
	/*391*/		GBKToUTF8("游客45884"),
	/*392*/		GBKToUTF8("游客23205"),
	/*393*/		GBKToUTF8("游客79567"),
	/*394*/		GBKToUTF8("会员90613"),
	/*395*/		GBKToUTF8("游客62754"),
	/*396*/		GBKToUTF8("会员54259"),
	/*397*/		GBKToUTF8("会员28202"),
	/*398*/		GBKToUTF8("会员23506"),
	/*399*/		GBKToUTF8("会员2021"),
	/*400*/		GBKToUTF8("会员90868"),
	/*401*/		GBKToUTF8("会员35189"),
	/*402*/		GBKToUTF8("会员49908"),
	/*403*/		GBKToUTF8("会员10498"),
	/*404*/		GBKToUTF8("会员18808"),
	/*405*/		GBKToUTF8("游客86248"),
	/*406*/		GBKToUTF8("游客33333"),
	/*407*/		GBKToUTF8("游客21648"),
	/*408*/		GBKToUTF8("会员99754"),
	/*409*/		GBKToUTF8("游客51746"),
	/*410*/		GBKToUTF8("会员19529"),
	/*411*/		GBKToUTF8("会员38046"),
	/*412*/		GBKToUTF8("会员49797"),
	/*413*/		GBKToUTF8("游客86990"),
	/*414*/		GBKToUTF8("游客3033"),
	/*415*/		GBKToUTF8("游客12497"),
	/*416*/		GBKToUTF8("游客94892"),
	/*417*/		GBKToUTF8("会员19125"),
	/*418*/		GBKToUTF8("会员13996"),
	/*419*/		GBKToUTF8("游客9188"),
	/*420*/		GBKToUTF8("游客3729"),
	/*421*/		GBKToUTF8("会员32460"),
	/*422*/		GBKToUTF8("会员43921"),
	/*423*/		GBKToUTF8("会员26304"),
	/*424*/		GBKToUTF8("会员88027"),
	/*425*/		GBKToUTF8("会员66748"),
	/*426*/		GBKToUTF8("会员8902"),
	/*427*/		GBKToUTF8("会员97697"),
	/*428*/		GBKToUTF8("游客71043"),
	/*429*/		GBKToUTF8("游客32002"),
	/*430*/		GBKToUTF8("会员6403"),
	/*431*/		GBKToUTF8("会员681"),
	/*432*/		GBKToUTF8("游客8538"),
	/*433*/		GBKToUTF8("游客95151"),
	/*434*/		GBKToUTF8("游客82134"),
	/*435*/		GBKToUTF8("游客71692"),
	/*436*/		GBKToUTF8("游客16127"),
	/*437*/		GBKToUTF8("会员55629"),
	/*438*/		GBKToUTF8("游客90964"),
	/*439*/		GBKToUTF8("游客36429"),
	/*440*/		GBKToUTF8("游客76335"),
	/*441*/		GBKToUTF8("游客2900"),
	/*442*/		GBKToUTF8("会员7971"),
	/*443*/		GBKToUTF8("游客60289"),
	/*444*/		GBKToUTF8("游客17988"),
	/*445*/		GBKToUTF8("会员85795"),
	/*446*/		GBKToUTF8("游客53144"),
	/*447*/		GBKToUTF8("游客58390"),
	/*448*/		GBKToUTF8("会员55340"),
	/*449*/		GBKToUTF8("游客569"),
	/*450*/		GBKToUTF8("会员74232"),
	/*451*/		GBKToUTF8("游客56042"),
	/*452*/		GBKToUTF8("会员9117"),
	/*453*/		GBKToUTF8("游客66761"),
	/*454*/		GBKToUTF8("游客26309"),
	/*455*/		GBKToUTF8("会员95425"),
	/*456*/		GBKToUTF8("会员6367"),
	/*457*/		GBKToUTF8("游客4234"),
	/*458*/		GBKToUTF8("会员31626"),
	/*459*/		GBKToUTF8("会员26057"),
	/*460*/		GBKToUTF8("会员73168"),
	/*461*/		GBKToUTF8("游客90358"),
	/*462*/		GBKToUTF8("会员47386"),
	/*463*/		GBKToUTF8("会员4346"),
	/*464*/		GBKToUTF8("会员34994"),
	/*465*/		GBKToUTF8("会员56552"),
	/*466*/		GBKToUTF8("会员93529"),
	/*467*/		GBKToUTF8("会员32290"),
	/*468*/		GBKToUTF8("游客56970"),
	/*469*/		GBKToUTF8("游客19080"),
	/*470*/		GBKToUTF8("游客18593"),
	/*471*/		GBKToUTF8("游客18627"),
	/*472*/		GBKToUTF8("会员61886"),
	/*473*/		GBKToUTF8("会员88355"),
	/*474*/		GBKToUTF8("会员20090"),
	/*475*/		GBKToUTF8("会员59479"),
	/*476*/		GBKToUTF8("会员58969"),
	/*477*/		GBKToUTF8("游客52274"),
	/*478*/		GBKToUTF8("游客47641"),
	/*479*/		GBKToUTF8("会员25433"),
	/*480*/		GBKToUTF8("游客77888"),
	/*481*/		GBKToUTF8("会员24566"),
	/*482*/		GBKToUTF8("会员27284"),
	/*483*/		GBKToUTF8("会员90417"),
	/*484*/		GBKToUTF8("会员72260"),
	/*485*/		GBKToUTF8("游客237"),
	/*486*/		GBKToUTF8("游客73059"),
	/*487*/		GBKToUTF8("游客48518"),
	/*488*/		GBKToUTF8("游客90783"),
	/*489*/		GBKToUTF8("游客28458"),
	/*490*/		GBKToUTF8("游客67637"),
	/*491*/		GBKToUTF8("游客20483"),
	/*492*/		GBKToUTF8("游客70478"),
	/*493*/		GBKToUTF8("游客49314"),
	/*494*/		GBKToUTF8("游客45729"),
	/*495*/		GBKToUTF8("会员33459"),
	/*496*/		GBKToUTF8("会员89438"),
	/*497*/		GBKToUTF8("游客11388"),
	/*498*/		GBKToUTF8("会员31233"),
	/*499*/		GBKToUTF8("游客33681"),
	/*500*/		GBKToUTF8("游客60358"),
	/*501*/		GBKToUTF8("会员10699"),
	/*502*/		GBKToUTF8("游客1839"),
	/*503*/		GBKToUTF8("游客68363"),
	/*504*/		GBKToUTF8("会员28794"),
	/*505*/		GBKToUTF8("游客19847"),
	/*506*/		GBKToUTF8("游客17462"),
	/*507*/		GBKToUTF8("会员39390"),
	/*508*/		GBKToUTF8("会员45791"),
	/*509*/		GBKToUTF8("游客55115"),
	/*510*/		GBKToUTF8("游客96157"),
	/*511*/		GBKToUTF8("会员31491"),
	/*512*/		GBKToUTF8("游客62951"),
	/*513*/		GBKToUTF8("游客35021"),
	/*514*/		GBKToUTF8("游客93740"),
	/*515*/		GBKToUTF8("会员14030"),
	/*516*/		GBKToUTF8("会员35325"),
	/*517*/		GBKToUTF8("游客87516"),
	/*518*/		GBKToUTF8("会员33002"),
	/*519*/		GBKToUTF8("游客46139"),
	/*520*/		GBKToUTF8("会员85404"),
	/*521*/		GBKToUTF8("会员74580"),
	/*522*/		GBKToUTF8("会员39021"),
	/*523*/		GBKToUTF8("会员39862"),
	/*524*/		GBKToUTF8("会员35379"),
	/*525*/		GBKToUTF8("游客92685"),
	/*526*/		GBKToUTF8("会员99904"),
	/*527*/		GBKToUTF8("会员33483"),
	/*528*/		GBKToUTF8("游客99759"),
	/*529*/		GBKToUTF8("游客89744"),
	/*530*/		GBKToUTF8("游客69911"),
	/*531*/		GBKToUTF8("游客63950"),
	/*532*/		GBKToUTF8("会员77560"),
	/*533*/		GBKToUTF8("会员75105"),
	/*534*/		GBKToUTF8("游客40049"),
	/*535*/		GBKToUTF8("会员88711"),
	/*536*/		GBKToUTF8("游客19934"),
	/*537*/		GBKToUTF8("游客77375"),
	/*538*/		GBKToUTF8("游客93614"),
	/*539*/		GBKToUTF8("游客43768"),
	/*540*/		GBKToUTF8("游客44918"),
	/*541*/		GBKToUTF8("游客76882"),
	/*542*/		GBKToUTF8("会员76982"),
	/*543*/		GBKToUTF8("游客24030"),
	/*544*/		GBKToUTF8("游客11574"),
	/*545*/		GBKToUTF8("会员86593"),
	/*546*/		GBKToUTF8("会员50253"),
	/*547*/		GBKToUTF8("游客3074"),
	/*548*/		GBKToUTF8("会员34713"),
	/*549*/		GBKToUTF8("游客38377"),
	/*550*/		GBKToUTF8("会员15775"),
	/*551*/		GBKToUTF8("会员32919"),
	/*552*/		GBKToUTF8("会员6732"),
	/*553*/		GBKToUTF8("会员11017"),
	/*554*/		GBKToUTF8("会员60235"),
	/*555*/		GBKToUTF8("游客45691"),
	/*556*/		GBKToUTF8("游客23943"),
	/*557*/		GBKToUTF8("游客66328"),
	/*558*/		GBKToUTF8("游客49291"),
	/*559*/		GBKToUTF8("会员94018"),
	/*560*/		GBKToUTF8("游客36836"),
	/*561*/		GBKToUTF8("游客75055"),
	/*562*/		GBKToUTF8("会员63858"),
	/*563*/		GBKToUTF8("会员14904"),
	/*564*/		GBKToUTF8("游客72661"),
	/*565*/		GBKToUTF8("游客89685"),
	/*566*/		GBKToUTF8("游客13073"),
	/*567*/		GBKToUTF8("会员56851"),
	/*568*/		GBKToUTF8("游客49250"),
	/*569*/		GBKToUTF8("会员36503"),
	/*570*/		GBKToUTF8("游客39006"),
	/*571*/		GBKToUTF8("游客14639"),
	/*572*/		GBKToUTF8("游客21120"),
	/*573*/		GBKToUTF8("游客80226"),
	/*574*/		GBKToUTF8("游客87677"),
	/*575*/		GBKToUTF8("会员63981"),
	/*576*/		GBKToUTF8("游客87560"),
	/*577*/		GBKToUTF8("会员27955"),
	/*578*/		GBKToUTF8("会员33518"),
	/*579*/		GBKToUTF8("游客66342"),
	/*580*/		GBKToUTF8("会员45196"),
	/*581*/		GBKToUTF8("游客27321"),
	/*582*/		GBKToUTF8("会员94984"),
	/*583*/		GBKToUTF8("会员94427"),
	/*584*/		GBKToUTF8("会员52040"),
	/*585*/		GBKToUTF8("游客70072"),
	/*586*/		GBKToUTF8("会员45830"),
	/*587*/		GBKToUTF8("游客70347"),
	/*588*/		GBKToUTF8("会员82030"),
	/*589*/		GBKToUTF8("游客53714"),
	/*590*/		GBKToUTF8("游客57522"),
	/*591*/		GBKToUTF8("游客26924"),
	/*592*/		GBKToUTF8("会员89435"),
	/*593*/		GBKToUTF8("会员29204"),
	/*594*/		GBKToUTF8("会员30443"),
	/*595*/		GBKToUTF8("会员45486"),
	/*596*/		GBKToUTF8("会员84278"),
	/*597*/		GBKToUTF8("游客50262"),
	/*598*/		GBKToUTF8("会员89683"),
	/*599*/		GBKToUTF8("游客39848"),
	/*600*/		GBKToUTF8("游客8324"),
};