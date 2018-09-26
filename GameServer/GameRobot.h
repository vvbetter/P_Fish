//��Ϸ������
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
	DWORD				m_RobotID; //���õĻ���������ID
	bool				m_IsUse;
	//1.��������ļ�¼
	DWORD				m_RoomTimeLog;
	DWORD				m_LeaveTableTimeLog;
	//2.�������л�����
	DWORD				m_RateTimeLog;
	BYTE				m_NowRate;
	DWORD				m_ChangeRateTimeLog;
	//3.������ʹ�ü���
	DWORD				m_SkillTimeLog;
	DWORD				m_UseSkillTimeLog;
	//4.�������л���̨
	DWORD				m_LauncherTimeLog;
	DWORD				m_ChangeLauncherTimeLog;
	//5.�����˿���
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
	list<tagGameRobotCharm> m_vecSendCharmArray;//�����͵���������
	tagGameRobotCharmChangeManager m_RobotCharmEvent;
	//
	DWORD				m_dwSendCharmTineLog;

};
struct tagRobotWrite
{
	WORD		TableID;
	DWORD		TimeLog;
};

class GameRobotManager//��Ϸ�����˹�����
{
public:
	GameRobotManager();
	virtual ~GameRobotManager();
	//����ȫ���Ļ�����
	void  OnLoadAllGameRobot(DWORD BeginUserID, DWORD EndUserID);
	void  OnAddRobot(DWORD dwUserID, CRoleEx* pRoleEx);
	//update
	void  Update();
	void  UpdateWriteList();
	GameRobot* GetFreeRobot(DWORD RobotID, GameTable* pTable);//��ȡһ�����еĻ�����
	void ResetGameRobot(DWORD dwUserID);//�黹һ��������
	void ResetAllGameRobot();//�黹���л�����
	bool GameRobotIsCanJoinTable(GameTable* pTable);//�жϻ������Ƿ���Խ��뵱ǰ������
	void  OnRoleCreateNormalRoom(GameTable* pTable);//����ͨ��Ҵ���һ���Ǳ��������ʱ��
	void  OnRoleLeaveNormalRoom(GameTable* pTable);//����ͨ����뿪һ���Ǳ��������ʱ��
	void  OnRoleJoinNormalRoom(GameTable* pTable);//����ͨ��ҽ���һ���Ǳ��������ʱ��
	void  AdddWriteRobot(WORD TableID, DWORD WriteTime); //��ӵ����¼��뷿��Ļ����˶���
private:
	bool JoinRobot(DWORD robotTypeID, GameTable* pTable);
private:
	volatile UINT				m_GetIndex;       //��ȡfree
	bool						m_IsLoadFinish;   //�������
	DWORD						m_RobotConfigSum; //���õĻ���������
	vector<GameRobot*>			m_Robots;      //���л�����
	list<tagRobotWrite>			m_WriteList;	  //���»ص����ӵĻ����˶���
};
#define MAX_ROBOTNAME_COUNT 600

static string g_RobotName[MAX_ROBOTNAME_COUNT] = {
	/*1*/    GBKToUTF8("ƽ��֮·1234"),
	/*2*/    GBKToUTF8("����ܽ22"),
	/*3*/    GBKToUTF8("����ĵ�"),
	/*4*/    GBKToUTF8("������"),
	/*5*/    GBKToUTF8("�������Ǻ�"),
	/*6*/    GBKToUTF8("����С��8868"),
	/*7*/    GBKToUTF8("��ڵ�õ��"),
	/*8*/    GBKToUTF8("����δ�벻��"),
	/*9*/    GBKToUTF8("��������"),
	/*10*/    GBKToUTF8("�Ұ����"),
	/*11*/    GBKToUTF8("Jerry��С��"),
	/*12*/    GBKToUTF8("ܰܰ��С��"),
	/*13*/    GBKToUTF8("����һֻèѽ"),
	/*14*/    GBKToUTF8("�Ҹ�������2015"),
	/*15*/    GBKToUTF8("��С��ABC"),
	/*16*/    GBKToUTF8("����һ��綹��"),
	/*17*/    GBKToUTF8("��Ϯ��2016"),
	/*18*/    GBKToUTF8("��Զ18�����Ů"),
	/*19*/    GBKToUTF8("���µ��ػ���"),
	/*20*/    GBKToUTF8("����������"),
	/*21*/    GBKToUTF8("��һ�����"),
	/*22*/    GBKToUTF8("������2016"),
	/*23*/    GBKToUTF8("lbiubiuily"),
	/*24*/    GBKToUTF8("��������"),
	/*25*/    GBKToUTF8("�����ɫ"),
	/*26*/    GBKToUTF8("���������"),
	/*27*/    GBKToUTF8("�������ͻ�"),
	/*28*/    GBKToUTF8("����"),
	/*29*/    GBKToUTF8("��ϯ��"),
	/*30*/    GBKToUTF8("��������˵"),
	/*31*/    GBKToUTF8("���׵�����"),
	/*32*/    GBKToUTF8("��ά123"),
	/*33*/    GBKToUTF8("�����Ϲ���"),
	/*34*/    GBKToUTF8("0��ʼ0"),
	/*35*/    GBKToUTF8("����2016"),
	/*36*/    GBKToUTF8("�ͳ��ڵ�M��"),
	/*37*/    GBKToUTF8("����è"),
	/*38*/    GBKToUTF8("����125"),
	/*39*/    GBKToUTF8("��ã�����к�"),
	/*40*/    GBKToUTF8("�߾�Ůyy"),
	/*41*/    GBKToUTF8("�˲˵�С����"),
	/*42*/    GBKToUTF8("�����˻��"),
	/*43*/    GBKToUTF8("��ƮƮ2016"),
	/*44*/    GBKToUTF8("���ﻨ��֪����we"),
	/*45*/    GBKToUTF8("��������2016"),
	/*46*/    GBKToUTF8("����2011"),
	/*47*/    GBKToUTF8("����һ��I"),
	/*48*/    GBKToUTF8("��̽���"),
	/*49*/    GBKToUTF8("���ѵ���2016"),
	/*50*/    GBKToUTF8("jinlai999"),
	/*51*/    GBKToUTF8("�������ֶ�"),
	/*52*/    GBKToUTF8("�����ҵ�����"),
	/*53*/    GBKToUTF8("������Ϯ"),
	/*54*/    GBKToUTF8("�簮���ײ�"),
	/*55*/    GBKToUTF8("СϲȵHD"),
	/*56*/    GBKToUTF8("������"),
	/*57*/    GBKToUTF8("�������"),
	/*58*/    GBKToUTF8("�Ҿ����Ǹ������"),
	/*59*/    GBKToUTF8("��������"),
	/*60*/    GBKToUTF8("�׺�һ��qwe"),
	/*61*/    GBKToUTF8("��������"),
	/*62*/    GBKToUTF8("�ܷ���1994"),
	/*63*/    GBKToUTF8("�����Ķ���2016"),
	/*64*/    GBKToUTF8("����������"),
	/*65*/    GBKToUTF8("С�ݱ���"),
	/*66*/    GBKToUTF8("�Ұ�����Ƥ���"),
	/*67*/    GBKToUTF8("����Ӧ��Ӧ����"),
	/*68*/    GBKToUTF8("iam���ǵ��"),
	/*69*/    GBKToUTF8("������������"),
	/*70*/    GBKToUTF8("����ʦ��"),
	/*71*/    GBKToUTF8("ůů������2016"),
	/*72*/    GBKToUTF8("A����Ժ�ķ��Է�"),
	/*73*/    GBKToUTF8("�ҵ���˹��D"),
	/*74*/    GBKToUTF8("�ҵ�δ��������"),
	/*75*/    GBKToUTF8("�۹�������"),
	/*76*/    GBKToUTF8("��������"),
	/*77*/    GBKToUTF8("�����Ǻ�ٰ"),
	/*78*/    GBKToUTF8("writers_"),
	/*79*/    GBKToUTF8("�����޹���Ů"),
	/*80*/    GBKToUTF8("Ү����"),
	/*81*/    GBKToUTF8("�Ѹ�ôô��"),
	/*82*/    GBKToUTF8("��欵�Сƨ��"),
	/*83*/    GBKToUTF8("���߸�Ь��С��ݮ"),
	/*84*/    GBKToUTF8("���һ�����Y"),
	/*85*/    GBKToUTF8("fangque123"),
	/*86*/    GBKToUTF8("���lonely"),
	/*87*/    GBKToUTF8("��ש������"),
	/*88*/    GBKToUTF8("ʣ��û����ʣ��"),
	/*89*/    GBKToUTF8("��ҽ�����"),
	/*90*/    GBKToUTF8("�����ܵ�С��	"),
	/*91*/    GBKToUTF8("��z��"),
	/*92*/    GBKToUTF8("��д����"),
	/*93*/    GBKToUTF8("��������2016	"),
	/*94*/    GBKToUTF8("Ŭ����С���Q"),
	/*95*/    GBKToUTF8("Ȼ����Ȼ"),
	/*96*/    GBKToUTF8("���˴�����"),
	/*97*/    GBKToUTF8("ľС��"),
	/*98*/    GBKToUTF8("���鲻�����	"),
	/*99*/    GBKToUTF8("����������wwl"),
	/*100*/    GBKToUTF8("ִ��ying"),
	/*101*/    GBKToUTF8("Zyydrw"),
	/*102*/    GBKToUTF8("��������"),
	/*103*/    GBKToUTF8("����������²�"),
	/*104*/    GBKToUTF8("�๢"),
	/*105*/    GBKToUTF8("���ܵ�������	"),
	/*106*/    GBKToUTF8("�չ�����"),
	/*107*/    GBKToUTF8("shaqt"),
	/*108*/    GBKToUTF8("���б�ɫ"),
	/*109*/    GBKToUTF8("���������1119"),
	/*110*/    GBKToUTF8("һ�����ֵĶ�����"),
	/*111*/    GBKToUTF8("��һ��������"),
	/*112*/    GBKToUTF8("�����Ǹ�С��"),
	/*113*/    GBKToUTF8("����Ե99"),
	/*114*/    GBKToUTF8("�������ĭ"),
	/*115*/    GBKToUTF8("�����520"),
	/*116*/    GBKToUTF8("����ˮ����"),
	/*117*/    GBKToUTF8("��޳�����"),
	/*118*/    GBKToUTF8("������"),
	/*119*/    GBKToUTF8("ʱ�⵹ת2016"),
	/*120*/    GBKToUTF8("����������"),
	/*121*/    GBKToUTF8("ˮ��˿��"),
	/*122*/    GBKToUTF8("΢΢С΢"),
	/*123*/    GBKToUTF8("һ������"),
	/*124*/    GBKToUTF8("��������"),
	/*125*/    GBKToUTF8("һ�Ź������С��"),
	/*126*/    GBKToUTF8("ͯʮ��"),
	/*127*/    GBKToUTF8("ż�θ��񰡰���"),
	/*128*/    GBKToUTF8("꼻�"),
	/*129*/    GBKToUTF8("������ƮƮ"),
	/*130*/    GBKToUTF8("�ƽ���������"),
	/*131*/    GBKToUTF8("С���ѳ���ëCXY"),
	/*132*/    GBKToUTF8("��������1940"),
	/*133*/    GBKToUTF8("�ҵ�ҢҢ"),
	/*134*/    GBKToUTF8("��֧����"),
	/*135*/    GBKToUTF8("���ӱ���"),
	/*136*/    GBKToUTF8("Ϧҹ1993"),
	/*137*/    GBKToUTF8("��ǿ�Ĺ����"),
	/*138*/    GBKToUTF8("����"),
	/*139*/    GBKToUTF8("��ֻ�Ǹ�Ů��ֽ"),
	/*140*/    GBKToUTF8("ѭ��2010"),
	/*141*/    GBKToUTF8("����"),
	/*142*/    GBKToUTF8("��ͷ��ͷ"),
	/*143*/    GBKToUTF8("ȨȨ�Ȱ�����"),
	/*144*/    GBKToUTF8("������pp"),
	/*145*/    GBKToUTF8("˵����N��ʹ��"),
	/*146*/    GBKToUTF8("ͼͼ������"),
	/*147*/    GBKToUTF8("����de��"),
	/*148*/    GBKToUTF8("���ι���"),
	/*149*/    GBKToUTF8("�㲻����"),
	/*150*/    GBKToUTF8("С��ôô"),
	/*151*/    GBKToUTF8("�������й�����"),
	/*152*/    GBKToUTF8("��������54"),
	/*153*/    GBKToUTF8("�ѵ�������"),
	/*154*/    GBKToUTF8("��������yi"),
	/*155*/    GBKToUTF8("�ֺǺ�5268"),
	/*156*/    GBKToUTF8("���ĳ���"),
	/*157*/    GBKToUTF8("�ջ�ŷ��"),
	/*158*/    GBKToUTF8("��Ī2015"),
	/*159*/    GBKToUTF8("����������"),
	/*160*/    GBKToUTF8("����ĳı"),
	/*161*/    GBKToUTF8("׷����������"),
	/*162*/    GBKToUTF8("һĨС���"),
	/*163*/    GBKToUTF8("��һ������ȥ��"),
	/*164*/    GBKToUTF8("��������"),
	/*165*/    GBKToUTF8("��Į��쳾"),
	/*166*/    GBKToUTF8("���ֻ���ɽ"),
	/*167*/    GBKToUTF8("��Ͱ������"),
	/*168*/    GBKToUTF8("�ܰ��ܰ���"),
	/*169*/    GBKToUTF8("�����Ȱ�����"),
	/*170*/    GBKToUTF8("Ҷ����"),
	/*171*/    GBKToUTF8("ȼ�յ���ˮ"),
	/*172*/    GBKToUTF8("��Ȼ�i"),
	/*173*/    GBKToUTF8("�������"),
	/*174*/    GBKToUTF8("70����������"),
	/*175*/    GBKToUTF8("׷����������"),
	/*176*/    GBKToUTF8("ƽ��л93"),
	/*177*/    GBKToUTF8("����������"),
	/*178*/    GBKToUTF8("�ɳǲ�ů������"),
	/*179*/    GBKToUTF8("д��δ����"),
	/*180*/    GBKToUTF8("Henkiki"),
	/*181*/    GBKToUTF8("�����è��"),
	/*182*/    GBKToUTF8("�������"),
	/*183*/    GBKToUTF8("׷����������"),
	/*184*/    GBKToUTF8("���Ʈ�仨��"),
	/*185*/    GBKToUTF8("������������"),
	/*186*/    GBKToUTF8("׷����ר��"),
	/*187*/    GBKToUTF8("ribendadan"),
	/*188*/    GBKToUTF8("chunyzi2016"),
	/*189*/    GBKToUTF8("С���ɺ���"),
	/*190*/    GBKToUTF8("С���򰮴���"),
	/*191*/    GBKToUTF8("����lovely"),
	/*192*/    GBKToUTF8("˧��С����"),
	/*193*/    GBKToUTF8("�ҽп׷�������"),
	/*194*/    GBKToUTF8("������1996"),
	/*195*/    GBKToUTF8("ʱ��׺�һ˲Ϣ"),
	/*196*/    GBKToUTF8("��һ��0��"),
	/*197*/    GBKToUTF8("���õ����õ�"),
	/*198*/    GBKToUTF8("���_1001"),
	/*199*/    GBKToUTF8("����"),
	/*200*/    GBKToUTF8("����һ֦��"),
	/*201*/		GBKToUTF8("�ο�30886"),
	/*202*/		GBKToUTF8("�ο�36915"),
	/*203*/		GBKToUTF8("�ο�38335"),
	/*204*/		GBKToUTF8("��Ա60492"),
	/*205*/		GBKToUTF8("�ο�41421"),
	/*206*/		GBKToUTF8("��Ա90027"),
	/*207*/		GBKToUTF8("��Ա20059"),
	/*208*/		GBKToUTF8("�ο�13926"),
	/*209*/		GBKToUTF8("��Ա83426"),
	/*210*/		GBKToUTF8("��Ա55736"),
	/*211*/		GBKToUTF8("�ο�95368"),
	/*212*/		GBKToUTF8("�ο�56429"),
	/*213*/		GBKToUTF8("��Ա21530"),
	/*214*/		GBKToUTF8("��Ա65123"),
	/*215*/		GBKToUTF8("�ο�3135"),
	/*216*/		GBKToUTF8("�ο�79802"),
	/*217*/		GBKToUTF8("��Ա23058"),
	/*218*/		GBKToUTF8("�ο�98167"),
	/*219*/		GBKToUTF8("�ο�18456"),
	/*220*/		GBKToUTF8("�ο�78042"),
	/*221*/		GBKToUTF8("�ο�77373"),
	/*222*/		GBKToUTF8("�ο�44919"),
	/*223*/		GBKToUTF8("��Ա98537"),
	/*224*/		GBKToUTF8("��Ա94324"),
	/*225*/		GBKToUTF8("�ο�64370"),
	/*226*/		GBKToUTF8("�ο�3526"),
	/*227*/		GBKToUTF8("�ο�68980"),
	/*228*/		GBKToUTF8("��Ա41873"),
	/*229*/		GBKToUTF8("��Ա99170"),
	/*230*/		GBKToUTF8("��Ա97281"),
	/*231*/		GBKToUTF8("�ο�20925"),
	/*232*/		GBKToUTF8("��Ա36327"),
	/*233*/		GBKToUTF8("��Ա26505"),
	/*234*/		GBKToUTF8("��Ա21729"),
	/*235*/		GBKToUTF8("�ο�25857"),
	/*236*/		GBKToUTF8("��Ա53895"),
	/*237*/		GBKToUTF8("��Ա545"),
	/*238*/		GBKToUTF8("��Ա33367"),
	/*239*/		GBKToUTF8("��Ա90364"),
	/*240*/		GBKToUTF8("�ο�13750"),
	/*241*/		GBKToUTF8("�ο�26808"),
	/*242*/		GBKToUTF8("��Ա47178"),
	/*243*/		GBKToUTF8("��Ա93584"),
	/*244*/		GBKToUTF8("�ο�2651"),
	/*245*/		GBKToUTF8("��Ա12399"),
	/*246*/		GBKToUTF8("��Ա95060"),
	/*247*/		GBKToUTF8("��Ա93368"),
	/*248*/		GBKToUTF8("�ο�10012"),
	/*249*/		GBKToUTF8("��Ա98586"),
	/*250*/		GBKToUTF8("��Ա97539"),
	/*251*/		GBKToUTF8("�ο�80570"),
	/*252*/		GBKToUTF8("��Ա60378"),
	/*253*/		GBKToUTF8("�ο�66601"),
	/*254*/		GBKToUTF8("�ο�12902"),
	/*255*/		GBKToUTF8("�ο�70492"),
	/*256*/		GBKToUTF8("��Ա60756"),
	/*257*/		GBKToUTF8("�ο�60280"),
	/*258*/		GBKToUTF8("��Ա9441"),
	/*259*/		GBKToUTF8("�ο�29689"),
	/*260*/		GBKToUTF8("��Ա46619"),
	/*261*/		GBKToUTF8("��Ա44729"),
	/*262*/		GBKToUTF8("�ο�8117"),
	/*263*/		GBKToUTF8("�ο�5771"),
	/*264*/		GBKToUTF8("�ο�90675"),
	/*265*/		GBKToUTF8("�ο�98927"),
	/*266*/		GBKToUTF8("�ο�77856"),
	/*267*/		GBKToUTF8("�ο�72353"),
	/*268*/		GBKToUTF8("��Ա76965"),
	/*269*/		GBKToUTF8("��Ա64683"),
	/*270*/		GBKToUTF8("�ο�28624"),
	/*271*/		GBKToUTF8("��Ա32871"),
	/*272*/		GBKToUTF8("��Ա48829"),
	/*273*/		GBKToUTF8("�ο�30019"),
	/*274*/		GBKToUTF8("��Ա63368"),
	/*275*/		GBKToUTF8("��Ա86715"),
	/*276*/		GBKToUTF8("��Ա18149"),
	/*277*/		GBKToUTF8("��Ա723"),
	/*278*/		GBKToUTF8("��Ա2245"),
	/*279*/		GBKToUTF8("��Ա93451"),
	/*280*/		GBKToUTF8("�ο�43555"),
	/*281*/		GBKToUTF8("�ο�97488"),
	/*282*/		GBKToUTF8("��Ա88228"),
	/*283*/		GBKToUTF8("�ο�92350"),
	/*284*/		GBKToUTF8("�ο�41500"),
	/*285*/		GBKToUTF8("��Ա87764"),
	/*286*/		GBKToUTF8("��Ա24914"),
	/*287*/		GBKToUTF8("�ο�75856"),
	/*288*/		GBKToUTF8("�ο�46491"),
	/*289*/		GBKToUTF8("�ο�48365"),
	/*290*/		GBKToUTF8("�ο�81936"),
	/*291*/		GBKToUTF8("��Ա52551"),
	/*292*/		GBKToUTF8("�ο�99228"),
	/*293*/		GBKToUTF8("�ο�75407"),
	/*294*/		GBKToUTF8("��Ա76121"),
	/*295*/		GBKToUTF8("��Ա94395"),
	/*296*/		GBKToUTF8("�ο�61237"),
	/*297*/		GBKToUTF8("�ο�73793"),
	/*298*/		GBKToUTF8("��Ա94428"),
	/*299*/		GBKToUTF8("�ο�31011"),
	/*300*/		GBKToUTF8("��Ա39529"),
	/*301*/		GBKToUTF8("��Ա22404"),
	/*302*/		GBKToUTF8("�ο�55763"),
	/*303*/		GBKToUTF8("�ο�54538"),
	/*304*/		GBKToUTF8("��Ա36840"),
	/*305*/		GBKToUTF8("��Ա44818"),
	/*306*/		GBKToUTF8("��Ա70688"),
	/*307*/		GBKToUTF8("�ο�67917"),
	/*308*/		GBKToUTF8("�ο�66996"),
	/*309*/		GBKToUTF8("��Ա87743"),
	/*310*/		GBKToUTF8("��Ա12183"),
	/*311*/		GBKToUTF8("��Ա95499"),
	/*312*/		GBKToUTF8("��Ա6725"),
	/*313*/		GBKToUTF8("��Ա55590"),
	/*314*/		GBKToUTF8("�ο�68139"),
	/*315*/		GBKToUTF8("��Ա69786"),
	/*316*/		GBKToUTF8("�ο�38082"),
	/*317*/		GBKToUTF8("��Ա88464"),
	/*318*/		GBKToUTF8("�ο�39507"),
	/*319*/		GBKToUTF8("�ο�28804"),
	/*320*/		GBKToUTF8("��Ա78611"),
	/*321*/		GBKToUTF8("��Ա27828"),
	/*322*/		GBKToUTF8("�ο�87343"),
	/*323*/		GBKToUTF8("��Ա35568"),
	/*324*/		GBKToUTF8("��Ա55422"),
	/*325*/		GBKToUTF8("�ο�13810"),
	/*326*/		GBKToUTF8("�ο�21801"),
	/*327*/		GBKToUTF8("�ο�73730"),
	/*328*/		GBKToUTF8("��Ա11305"),
	/*329*/		GBKToUTF8("��Ա78736"),
	/*330*/		GBKToUTF8("��Ա48626"),
	/*331*/		GBKToUTF8("��Ա3465"),
	/*332*/		GBKToUTF8("��Ա73416"),
	/*333*/		GBKToUTF8("��Ա13258"),
	/*334*/		GBKToUTF8("��Ա67637"),
	/*335*/		GBKToUTF8("��Ա5624"),
	/*336*/		GBKToUTF8("��Ա32036"),
	/*337*/		GBKToUTF8("��Ա11899"),
	/*338*/		GBKToUTF8("�ο�45550"),
	/*339*/		GBKToUTF8("��Ա90071"),
	/*340*/		GBKToUTF8("�ο�87131"),
	/*341*/		GBKToUTF8("�ο�84930"),
	/*342*/		GBKToUTF8("�ο�45894"),
	/*343*/		GBKToUTF8("��Ա70163"),
	/*344*/		GBKToUTF8("�ο�87981"),
	/*345*/		GBKToUTF8("�ο�52996"),
	/*346*/		GBKToUTF8("�ο�13773"),
	/*347*/		GBKToUTF8("�ο�39668"),
	/*348*/		GBKToUTF8("��Ա81095"),
	/*349*/		GBKToUTF8("��Ա16466"),
	/*350*/		GBKToUTF8("��Ա11340"),
	/*351*/		GBKToUTF8("��Ա27684"),
	/*352*/		GBKToUTF8("��Ա55542"),
	/*353*/		GBKToUTF8("��Ա79107"),
	/*354*/		GBKToUTF8("�ο�19756"),
	/*355*/		GBKToUTF8("�ο�18418"),
	/*356*/		GBKToUTF8("�ο�89412"),
	/*357*/		GBKToUTF8("��Ա32172"),
	/*358*/		GBKToUTF8("�ο�62009"),
	/*359*/		GBKToUTF8("��Ա25210"),
	/*360*/		GBKToUTF8("��Ա67587"),
	/*361*/		GBKToUTF8("��Ա19301"),
	/*362*/		GBKToUTF8("�ο�67372"),
	/*363*/		GBKToUTF8("�ο�1255"),
	/*364*/		GBKToUTF8("�ο�44599"),
	/*365*/		GBKToUTF8("�ο�29904"),
	/*366*/		GBKToUTF8("�ο�39811"),
	/*367*/		GBKToUTF8("��Ա15667"),
	/*368*/		GBKToUTF8("�ο�46228"),
	/*369*/		GBKToUTF8("�ο�29150"),
	/*370*/		GBKToUTF8("��Ա96658"),
	/*371*/		GBKToUTF8("��Ա89224"),
	/*372*/		GBKToUTF8("��Ա67269"),
	/*373*/		GBKToUTF8("��Ա54081"),
	/*374*/		GBKToUTF8("��Ա40084"),
	/*375*/		GBKToUTF8("��Ա11972"),
	/*376*/		GBKToUTF8("��Ա73850"),
	/*377*/		GBKToUTF8("�ο�5385"),
	/*378*/		GBKToUTF8("��Ա39299"),
	/*379*/		GBKToUTF8("��Ա6042"),
	/*380*/		GBKToUTF8("��Ա40713"),
	/*381*/		GBKToUTF8("��Ա56190"),
	/*382*/		GBKToUTF8("��Ա42590"),
	/*383*/		GBKToUTF8("�ο�8581"),
	/*384*/		GBKToUTF8("�ο�99336"),
	/*385*/		GBKToUTF8("��Ա71155"),
	/*386*/		GBKToUTF8("��Ա18004"),
	/*387*/		GBKToUTF8("�ο�14769"),
	/*388*/		GBKToUTF8("�ο�81776"),
	/*389*/		GBKToUTF8("��Ա47255"),
	/*390*/		GBKToUTF8("��Ա48142"),
	/*391*/		GBKToUTF8("�ο�45884"),
	/*392*/		GBKToUTF8("�ο�23205"),
	/*393*/		GBKToUTF8("�ο�79567"),
	/*394*/		GBKToUTF8("��Ա90613"),
	/*395*/		GBKToUTF8("�ο�62754"),
	/*396*/		GBKToUTF8("��Ա54259"),
	/*397*/		GBKToUTF8("��Ա28202"),
	/*398*/		GBKToUTF8("��Ա23506"),
	/*399*/		GBKToUTF8("��Ա2021"),
	/*400*/		GBKToUTF8("��Ա90868"),
	/*401*/		GBKToUTF8("��Ա35189"),
	/*402*/		GBKToUTF8("��Ա49908"),
	/*403*/		GBKToUTF8("��Ա10498"),
	/*404*/		GBKToUTF8("��Ա18808"),
	/*405*/		GBKToUTF8("�ο�86248"),
	/*406*/		GBKToUTF8("�ο�33333"),
	/*407*/		GBKToUTF8("�ο�21648"),
	/*408*/		GBKToUTF8("��Ա99754"),
	/*409*/		GBKToUTF8("�ο�51746"),
	/*410*/		GBKToUTF8("��Ա19529"),
	/*411*/		GBKToUTF8("��Ա38046"),
	/*412*/		GBKToUTF8("��Ա49797"),
	/*413*/		GBKToUTF8("�ο�86990"),
	/*414*/		GBKToUTF8("�ο�3033"),
	/*415*/		GBKToUTF8("�ο�12497"),
	/*416*/		GBKToUTF8("�ο�94892"),
	/*417*/		GBKToUTF8("��Ա19125"),
	/*418*/		GBKToUTF8("��Ա13996"),
	/*419*/		GBKToUTF8("�ο�9188"),
	/*420*/		GBKToUTF8("�ο�3729"),
	/*421*/		GBKToUTF8("��Ա32460"),
	/*422*/		GBKToUTF8("��Ա43921"),
	/*423*/		GBKToUTF8("��Ա26304"),
	/*424*/		GBKToUTF8("��Ա88027"),
	/*425*/		GBKToUTF8("��Ա66748"),
	/*426*/		GBKToUTF8("��Ա8902"),
	/*427*/		GBKToUTF8("��Ա97697"),
	/*428*/		GBKToUTF8("�ο�71043"),
	/*429*/		GBKToUTF8("�ο�32002"),
	/*430*/		GBKToUTF8("��Ա6403"),
	/*431*/		GBKToUTF8("��Ա681"),
	/*432*/		GBKToUTF8("�ο�8538"),
	/*433*/		GBKToUTF8("�ο�95151"),
	/*434*/		GBKToUTF8("�ο�82134"),
	/*435*/		GBKToUTF8("�ο�71692"),
	/*436*/		GBKToUTF8("�ο�16127"),
	/*437*/		GBKToUTF8("��Ա55629"),
	/*438*/		GBKToUTF8("�ο�90964"),
	/*439*/		GBKToUTF8("�ο�36429"),
	/*440*/		GBKToUTF8("�ο�76335"),
	/*441*/		GBKToUTF8("�ο�2900"),
	/*442*/		GBKToUTF8("��Ա7971"),
	/*443*/		GBKToUTF8("�ο�60289"),
	/*444*/		GBKToUTF8("�ο�17988"),
	/*445*/		GBKToUTF8("��Ա85795"),
	/*446*/		GBKToUTF8("�ο�53144"),
	/*447*/		GBKToUTF8("�ο�58390"),
	/*448*/		GBKToUTF8("��Ա55340"),
	/*449*/		GBKToUTF8("�ο�569"),
	/*450*/		GBKToUTF8("��Ա74232"),
	/*451*/		GBKToUTF8("�ο�56042"),
	/*452*/		GBKToUTF8("��Ա9117"),
	/*453*/		GBKToUTF8("�ο�66761"),
	/*454*/		GBKToUTF8("�ο�26309"),
	/*455*/		GBKToUTF8("��Ա95425"),
	/*456*/		GBKToUTF8("��Ա6367"),
	/*457*/		GBKToUTF8("�ο�4234"),
	/*458*/		GBKToUTF8("��Ա31626"),
	/*459*/		GBKToUTF8("��Ա26057"),
	/*460*/		GBKToUTF8("��Ա73168"),
	/*461*/		GBKToUTF8("�ο�90358"),
	/*462*/		GBKToUTF8("��Ա47386"),
	/*463*/		GBKToUTF8("��Ա4346"),
	/*464*/		GBKToUTF8("��Ա34994"),
	/*465*/		GBKToUTF8("��Ա56552"),
	/*466*/		GBKToUTF8("��Ա93529"),
	/*467*/		GBKToUTF8("��Ա32290"),
	/*468*/		GBKToUTF8("�ο�56970"),
	/*469*/		GBKToUTF8("�ο�19080"),
	/*470*/		GBKToUTF8("�ο�18593"),
	/*471*/		GBKToUTF8("�ο�18627"),
	/*472*/		GBKToUTF8("��Ա61886"),
	/*473*/		GBKToUTF8("��Ա88355"),
	/*474*/		GBKToUTF8("��Ա20090"),
	/*475*/		GBKToUTF8("��Ա59479"),
	/*476*/		GBKToUTF8("��Ա58969"),
	/*477*/		GBKToUTF8("�ο�52274"),
	/*478*/		GBKToUTF8("�ο�47641"),
	/*479*/		GBKToUTF8("��Ա25433"),
	/*480*/		GBKToUTF8("�ο�77888"),
	/*481*/		GBKToUTF8("��Ա24566"),
	/*482*/		GBKToUTF8("��Ա27284"),
	/*483*/		GBKToUTF8("��Ա90417"),
	/*484*/		GBKToUTF8("��Ա72260"),
	/*485*/		GBKToUTF8("�ο�237"),
	/*486*/		GBKToUTF8("�ο�73059"),
	/*487*/		GBKToUTF8("�ο�48518"),
	/*488*/		GBKToUTF8("�ο�90783"),
	/*489*/		GBKToUTF8("�ο�28458"),
	/*490*/		GBKToUTF8("�ο�67637"),
	/*491*/		GBKToUTF8("�ο�20483"),
	/*492*/		GBKToUTF8("�ο�70478"),
	/*493*/		GBKToUTF8("�ο�49314"),
	/*494*/		GBKToUTF8("�ο�45729"),
	/*495*/		GBKToUTF8("��Ա33459"),
	/*496*/		GBKToUTF8("��Ա89438"),
	/*497*/		GBKToUTF8("�ο�11388"),
	/*498*/		GBKToUTF8("��Ա31233"),
	/*499*/		GBKToUTF8("�ο�33681"),
	/*500*/		GBKToUTF8("�ο�60358"),
	/*501*/		GBKToUTF8("��Ա10699"),
	/*502*/		GBKToUTF8("�ο�1839"),
	/*503*/		GBKToUTF8("�ο�68363"),
	/*504*/		GBKToUTF8("��Ա28794"),
	/*505*/		GBKToUTF8("�ο�19847"),
	/*506*/		GBKToUTF8("�ο�17462"),
	/*507*/		GBKToUTF8("��Ա39390"),
	/*508*/		GBKToUTF8("��Ա45791"),
	/*509*/		GBKToUTF8("�ο�55115"),
	/*510*/		GBKToUTF8("�ο�96157"),
	/*511*/		GBKToUTF8("��Ա31491"),
	/*512*/		GBKToUTF8("�ο�62951"),
	/*513*/		GBKToUTF8("�ο�35021"),
	/*514*/		GBKToUTF8("�ο�93740"),
	/*515*/		GBKToUTF8("��Ա14030"),
	/*516*/		GBKToUTF8("��Ա35325"),
	/*517*/		GBKToUTF8("�ο�87516"),
	/*518*/		GBKToUTF8("��Ա33002"),
	/*519*/		GBKToUTF8("�ο�46139"),
	/*520*/		GBKToUTF8("��Ա85404"),
	/*521*/		GBKToUTF8("��Ա74580"),
	/*522*/		GBKToUTF8("��Ա39021"),
	/*523*/		GBKToUTF8("��Ա39862"),
	/*524*/		GBKToUTF8("��Ա35379"),
	/*525*/		GBKToUTF8("�ο�92685"),
	/*526*/		GBKToUTF8("��Ա99904"),
	/*527*/		GBKToUTF8("��Ա33483"),
	/*528*/		GBKToUTF8("�ο�99759"),
	/*529*/		GBKToUTF8("�ο�89744"),
	/*530*/		GBKToUTF8("�ο�69911"),
	/*531*/		GBKToUTF8("�ο�63950"),
	/*532*/		GBKToUTF8("��Ա77560"),
	/*533*/		GBKToUTF8("��Ա75105"),
	/*534*/		GBKToUTF8("�ο�40049"),
	/*535*/		GBKToUTF8("��Ա88711"),
	/*536*/		GBKToUTF8("�ο�19934"),
	/*537*/		GBKToUTF8("�ο�77375"),
	/*538*/		GBKToUTF8("�ο�93614"),
	/*539*/		GBKToUTF8("�ο�43768"),
	/*540*/		GBKToUTF8("�ο�44918"),
	/*541*/		GBKToUTF8("�ο�76882"),
	/*542*/		GBKToUTF8("��Ա76982"),
	/*543*/		GBKToUTF8("�ο�24030"),
	/*544*/		GBKToUTF8("�ο�11574"),
	/*545*/		GBKToUTF8("��Ա86593"),
	/*546*/		GBKToUTF8("��Ա50253"),
	/*547*/		GBKToUTF8("�ο�3074"),
	/*548*/		GBKToUTF8("��Ա34713"),
	/*549*/		GBKToUTF8("�ο�38377"),
	/*550*/		GBKToUTF8("��Ա15775"),
	/*551*/		GBKToUTF8("��Ա32919"),
	/*552*/		GBKToUTF8("��Ա6732"),
	/*553*/		GBKToUTF8("��Ա11017"),
	/*554*/		GBKToUTF8("��Ա60235"),
	/*555*/		GBKToUTF8("�ο�45691"),
	/*556*/		GBKToUTF8("�ο�23943"),
	/*557*/		GBKToUTF8("�ο�66328"),
	/*558*/		GBKToUTF8("�ο�49291"),
	/*559*/		GBKToUTF8("��Ա94018"),
	/*560*/		GBKToUTF8("�ο�36836"),
	/*561*/		GBKToUTF8("�ο�75055"),
	/*562*/		GBKToUTF8("��Ա63858"),
	/*563*/		GBKToUTF8("��Ա14904"),
	/*564*/		GBKToUTF8("�ο�72661"),
	/*565*/		GBKToUTF8("�ο�89685"),
	/*566*/		GBKToUTF8("�ο�13073"),
	/*567*/		GBKToUTF8("��Ա56851"),
	/*568*/		GBKToUTF8("�ο�49250"),
	/*569*/		GBKToUTF8("��Ա36503"),
	/*570*/		GBKToUTF8("�ο�39006"),
	/*571*/		GBKToUTF8("�ο�14639"),
	/*572*/		GBKToUTF8("�ο�21120"),
	/*573*/		GBKToUTF8("�ο�80226"),
	/*574*/		GBKToUTF8("�ο�87677"),
	/*575*/		GBKToUTF8("��Ա63981"),
	/*576*/		GBKToUTF8("�ο�87560"),
	/*577*/		GBKToUTF8("��Ա27955"),
	/*578*/		GBKToUTF8("��Ա33518"),
	/*579*/		GBKToUTF8("�ο�66342"),
	/*580*/		GBKToUTF8("��Ա45196"),
	/*581*/		GBKToUTF8("�ο�27321"),
	/*582*/		GBKToUTF8("��Ա94984"),
	/*583*/		GBKToUTF8("��Ա94427"),
	/*584*/		GBKToUTF8("��Ա52040"),
	/*585*/		GBKToUTF8("�ο�70072"),
	/*586*/		GBKToUTF8("��Ա45830"),
	/*587*/		GBKToUTF8("�ο�70347"),
	/*588*/		GBKToUTF8("��Ա82030"),
	/*589*/		GBKToUTF8("�ο�53714"),
	/*590*/		GBKToUTF8("�ο�57522"),
	/*591*/		GBKToUTF8("�ο�26924"),
	/*592*/		GBKToUTF8("��Ա89435"),
	/*593*/		GBKToUTF8("��Ա29204"),
	/*594*/		GBKToUTF8("��Ա30443"),
	/*595*/		GBKToUTF8("��Ա45486"),
	/*596*/		GBKToUTF8("��Ա84278"),
	/*597*/		GBKToUTF8("�ο�50262"),
	/*598*/		GBKToUTF8("��Ա89683"),
	/*599*/		GBKToUTF8("�ο�39848"),
	/*600*/		GBKToUTF8("�ο�8324"),
};