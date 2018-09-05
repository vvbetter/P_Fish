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
	void  OnLoadAllGameRobot(DWORD BeginUserID,DWORD EndUserID);
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
	volatile UINT				m_GetIndex;       //��ȡfree
	bool						m_IsLoadFinish;   //�������
	DWORD						m_RobotConfigSum; //���õĻ���������
	vector<GameRobot*>			m_Robots;      //���л�����
	list<tagRobotWrite>			m_WriteList;	  //���»ص����ӵĻ����˶���
};
#define MAX_ROBOTNAME_COUNT 200

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
	/*200*/    GBKToUTF8("����һ֦��")
};