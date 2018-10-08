//��ǰ��Ŀ��  ���ĵĽṹ�� 
#pragma once
#pragma warning(disable:4200)  //ʹ���˷Ǳ�׼��չ : �ṹ/�����е����С����
#pragma pack(push)
#pragma pack(1)
#include <time.h>
#include <set>
//һЩ�����Ķ���
#define KB_RATIO     2.0f    //�񱩱���
#define  MONEY_RATIO 100
#define  ICON_LENGTH 16 
#define  MAX_USER_ID 8
#define MAX_NICKNAME 23   //����ǳƵĳ���
#define MAX_SIGNATURE 23  //���ǩ������
#define MIN_NICKNAME 2

#define MAX_ERRORSTR_LENGTH 16
#define MAX_ROLECHAR_LENGTH 32
//#define MAX_MAIL_ITEM 3	 //�ʼ����Я������Ʒ����

#define MAX_MAIL_CONTEXT 128  // �ʼ�������󳤶�
#define MIN_MAIL_CONTEXT 1

#define ACCOUNT_LENGTH 32  //�˻��ĳ���
#define ACCOUNT_MIN_LENGTH 4

#define	MAC_ADDRESS_LENGTH 56

#define MAX_CHARM_ITEMSUM 8  //��������ĳ���

#define MAX_NAME_LENTH 10  //�����ʵ���Ƶĳ���
#define MIN_NAME_LENTH 2

#define MAX_ADDRESS_LENGTH 128  //�����ʵ��ַ�ĳ���
#define MIN_ADDRESS_LENGTH 1

#define MAX_PHONE_LENGTH 32 //����ֻ��ĳ���
#define MIN_PHONE_LENGTH 8 //����ֻ��ĳ���

#define MAX_IDENTITY_LENGTH 32 //������֤�ĳ���
#define MIN_IDENTITY_LENGTH 15 //������֤�ĳ���

#define MAX_EMAIL_LENGTH 64 //��������ַ�ĳ���
#define MIN_EMAIL_LENGTH 12

#define MAX_MAP_NAME	9 //��Ϸ������ͼ���Ƴ���
#define QUERY_ROLE_SUM 6 //һ�β�ѯ��ҷ�ҳ������
//#define SOCKET_TCP_PACKET 1000 //�����������ĳ�������
#define THREAD_NUM 8  //�������ӵ��߳�����
#define DB_ThreadSum 8 //DB���߳�����
//#define DB_Msg_PageSum 60000
#define Diff_Update_Sec 600  //���а���ǰʱ����
#define CHECKMSG(NowMSG,LENGTH,TYPE)		\
	(NowMSG == NULL || sizeof(TYPE) != LENGTH) ? NULL : static_cast<TYPE*>(NowMSG);
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))
#define SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 
#define GetMsgType(Main,Sub) (Main<<8)+Sub
#define SetMsgInfo(Object,Type,Size) Object.SetCmdSize(Size); Object.SetCmdType(Type);
#define CheckMsgSize(Size) if(Size >= 0xffff) {ASSERT(false);return;}
#define CheckMsgSizeReturn(Size) if(Size >= 0xffff) {ASSERT(false);return false;}
#define Msg_OnceSum 200  //�ڲ������
#define Client_Msg_OnceSum 1
#define GameServer_Max_UserSum 1000

#define MAX_ADDRESS_LENGTH_IP 16
#define Defalue_Ip_Address TEXT("���ص�ַ")

#define MAX_MESSAGE_LENGTH 128
#define MIN_MESSAGE_LENGTH 1

//#define MAX_LOGINFO_LENGTH  128
#define MAX_Announcement_Length 64

#define IP_MAX_LENGTH 16

#define  DB_Log_Str_Length 256

#define  RMB_OnceOnlyID_Length 32

#define  ExChangeCode_Length 16

#define PhoneIsCanUse(PhoneNumber) (PhoneNumber <=99999999999 && PhoneNumber>=10000000000)


#define MAX_NIUNIU_ClientSum 4 //�ͻ�����ע��������
#define MAX_NIUNIU_BrandSum 5  //һ�η��Ƶ�����
#define MAX_VIPSEAT_Sum 8 //���ϯ������
#define MAX_SHOWBRAND_Sum 8
#define MAX_NORMAL_PAGESUM 9


#define MAX_DIAL_GameSum 24 //�ͻ���ת�̶��������
#define MAX_DIAL_ClientSum 12 //�ͻ�����ע��������
#define MAX_DIALVIPSEAT_Sum 8 //���ϯ������
#define MAX_DIALSHOWBRAND_Sum 8
#define MAX_DIALNORMAL_PAGESUM 9



#define MAX_CAR_ClientSum 8 //�ͻ�����ע��������
#define MAX_CARVIPSEAT_Sum 8 //���ϯ������
#define MAX_CARSHOWBRAND_Sum 8
#define MAX_CARNORMAL_PAGESUM 9


#define MAX_MonthList_Length 4

#define MAX_RelationRequestLength 32


#define MAX_AlipayAccount_LENGTH 128
#define MIN_AlipayAccount_LENGTH 1
#define MAX_AlipayName_LENGTH 32
#define MIN_AlipayName_LENGTH 1


const int MAC_LENGTH = 18;

struct AfxNetworkClientOnce
{
	BYTE SeverID;
	ServerClientData *pClient;
	void* pPoint;
	DWORD Length;
};





//static void LogTaskToFile(DWORD AddGlobelSum,DWORD AllGlobelSum)
//{
//	//д�뵽�ļ�����ȥ Log.txt
//	FILE* pFile = NULL;
//	errno_t Error = fopen_s(&pFile, "LogTask.txt", "a");
//	if (!pFile)
//	{
//		Error = fopen_s(&pFile, "LogTask.txt", "w+");
//	}
//	if (pFile)
//	{
//		SYSTEMTIME time;
//		GetLocalTime(&time);
//		fprintf_s(pFile, "%d %d\n", AddGlobelSum, AllGlobelSum);
//		fclose(pFile);
//	}
//}


struct int256  //����Ľṹ 
{
	//BYTE 8 λ
	BYTE Value[32];
};
struct int256Handle
{
	static void Clear(int256& p)
	{
		for (int i = 0; i < CountArray(p.Value); ++i)
			p.Value[i] = 0;
	}
	static void SetValue(int256& p, BYTE pArray[], BYTE Size)
	{
		//���������õ�����ȥ
		for (int i = 0; i < Size && i<CountArray(p.Value); ++i)
		{
			p.Value[i] = pArray[i];
		}
	}
	static void SetBitStates(int256& p, BYTE BitIndex, bool States)
	{
		BYTE ArrayIndex = static_cast<BYTE>(BitIndex / 8);
		BYTE ChangeIndex = static_cast<BYTE>(BitIndex % 8);
		BYTE BitValue = static_cast<BYTE>(1 << ChangeIndex);
		if (States)
			p.Value[ArrayIndex] |= BitValue;
		else
			p.Value[ArrayIndex] ^= BitValue;
	}
	static bool GetBitStates(int256& p, BYTE BitIndex)
	{
		BYTE ArrayIndex = static_cast<BYTE>(BitIndex / 8);
		BYTE ChangeIndex = static_cast<BYTE>(BitIndex % 8);
		return ((p.Value[ArrayIndex] & (1 << ChangeIndex)) == (1 << ChangeIndex));
	}
};

//enum ErrorCode
//{
//	EC_Init = 0,
//	EC_Sucess = 0,
//	//������� �����������ͬ
//	//GlobelShop
//	EC_GlobelShop_ItemNotExites,//ָ������Ʒ�ڻ����̵겻����
//	EC_GlobelShop_CurreyError,//���Ҳ�����������̵��ָ����Ʒ
//	EC_Globel_Shop_Sucess,//������Ʒ�ɹ���
//	//RoleAchievement
//	EC_RoleAchievement_NotExites,//�ɾͲ�����
//	EC_RoleAchievement_NotFinih,//�ɾ�����δ��� �޷��ύ���
//	//RoleAction
//	EC_RoleAction_NotExites,//�������
//	EC_RoleAction_NotFinih,//�����δ��� �޷��ύ���
//	//RoleCharm
//	EC_RoleCharm_NonMe,//�����Ը��Լ����������Ʒ
//	EC_RoleCharm_NonItem,//ָ����������Ʒ������
//	EC_RoleCharm_GlobelError,//ָ��������Ʒ����������
//	EC_RoleCharm_Sucess,//����������Ʒ�ɹ���
//	EC_RoleCharm_SystemError,//ϵͳ����
//	EC_RoleCharm_CenterFiled,//δ���������������
//	//RoleCheck
//	EC_RoleCheck_CheckNowDayError,//�����Ѿ�ǩ������
//	EC_RoleCheck_CheckOtherMoneyError,//���Ҳ��� �޷���ǩ
//	EC_RoleCheck_CheckOtherError,//�޷���ǩ ��Ϊû����Ҫ��ǩ������
//	//RoleEntity
//	EC_RoleEntity_NameError,//ʵ�����Ƴ��Ȳ���ȷ �޷��޸�
//	EC_RoleEntity_Phone,//ʵ���ֻ����벻��ȷ �޷��޸�
//	EC_RoleEntity_EMail,//ʵ�������ַ����ȷ �޷��޸�
//	EC_RoleEntity_Address,//ʵ���û���ַ����ȷ �޷��޸�
//	EC_RoleEntity_EntityID,//ʵ���û������֤���벻��ȷ �޷��޸�
//	//RoleGiff
//	EC_RoleGiff_NotMe,//�����Ը��Լ���������
//	EC_RoleGiff_SendMaxSum,//�����Ѿ�����������ʹ��� �޷���������
//	EC_RoleGiff_SendOnceUserSum,//����Ե�ǰ��ҵ����ʹ����Ѿ�������� �޷���������
//	EC_RoleGiff_RecvMaxSum,//�����Ѿ��������Ľ������ʹ��� �޷���������
//	EC_RoleGiff_GiffNotExites,//��ǰ���Ͳ����� �޷�����
//	EC_RoleGiff_GiffNull,//�����ڿ��Խ��յ�����
//	EC_RoleGiff_TargetGiffMax,//Ŀ��������б��Ѿ����� �޷�������������
//	EC_RoleGiff_CenterFiled,//Ϊ�������������
//	//RoleItem
//	//RoleMail
//	EC_RoleMail_NotMe,//�����Ը��Լ������ʼ�
//	EC_RoleMail_MailNull,//Ŀ���ʼ�������
//	EC_RoleMail_ItemNonExites,//��ǰ�ʼ��޸��� �޷���ȡ
//	EC_RoleMail_ContextError,//�ʼ����峤�Ȳ���ȷ �޷�����
//	EC_RoleMail_SendTargetMailFull,//�ʼ�����ʧ�� Ŀ�������Ѿ����� 
//	EC_RoleMail_DelMailError,//Ŀ���ʼ������� �޷�ɾ��
//	EC_RoleMail_CenterField,//�޷��������������
//	EC_RoleMail_SendSucess,//�ʼ����ͳɹ�
//	//RoleMonth
//	EC_RoleMonth_SigUpMonthNotExists,//��ǰ���������� �޷�����
//	EC_RoleMonth_SigUpMonthTimeError,//��ǰ�������ڱ���ʱ�� �޷�����
//	EC_RoleMonth_SigUpMonthError,//�Ѿ�������ǰ������ �޷���������
//	EC_RoleMonth_SigUpMonthTableError,//�޷�����������������ӵ����� �޷���������
//	EC_RoleMonth_SigUpMonthMoneyError,//���Ҳ��� �޷���������
//	EC_RoleMonth_SigUpMonthItemError,//��Ʒ���� �޷���������
//	EC_RoleMonth_JoinMonthNotExists,//��ǰ���������� �޷�����
//	EC_RoleMonth_JoinMonthTimeError,//��ǰ�������ڲμ�ʱ�� �޷��μ�
//	EC_RoleMonth_JoinMonthError,//δ������ǰ������ �޷������μ�
//	EC_RoleMonth_JoinMonthTableError,//�޷�����������������ӵ����� �޷��μӱ���
//	EC_RoleMonth_AddMonthGlobelMonthError,//��ǰ���������� �޷�����
//	EC_RoleMonth_AddMonthGlobelSumError,//�޷����������� ������������
//	EC_RoleMonth_AddMonthGlobelMoneyError,//�޷����� ���Ҳ���
//	EC_RoleMonth_ResetMonthMonthError,//���ñ���ʧ�� ��ǰ����������
//	EC_RoleMonth_ResetMonthMoneyError,//���Ҳ��� �޷����ñ���
//	EC_RoleMonth_ResetMonthItemError,//��Ʒ���� �޷����ñ���
//	//RoleOnlirneReward
//	EC_RoleOnlineReward_RewardIDError,//��ǰ���߽��������� �޷���ȡ
//	EC_RoleOnlineReward_OnlineMinError,//�޷���ȡ���߽��� ����ʱ�䲻��
//	EC_RoleOnlineReward_Error,//�Ѿ���ȡ�����߽����� �޷�������ȡ
//	EC_RoleOnlineReward_Sucess,//��ȡ���߽����ɹ�
//	//RoleQuery
//	EC_QueryRole_RoleNameError,//��ѯ���Ƴ��Ȳ���ȷ �޷���ѯ
//	EC_QueryRole_QueryMeError,//��Ҳ����Բ�ѯ�Լ�
//	//RoleRank
//	EC_RoleRank_WeekReward_RankNotExites,//ָ�����а񲻴���
//	EC_RoleRank_WeekReward_NotInRank,//����Ϊ�ϰ� �޷���ȡ����
//	EC_RoleRank_WeekReward_Error,//�Ѿ���ȡ������ �޷�������ȡ
//	//RoleRelation
//	EC_RoleRelation_Add_NotMe,//����������Լ�
//	EC_RoleRelation_Add_RelationExists,//�Ѿ����ڹ�ϵ �޷��������
//	EC_RoleRelation_Add_RelationIsFull,//��ϵ�б��Ѿ����� �޷��������
//	EC_RoleRelation_Del_RelationNotExists,//��ϵ������ �޷�ɾ��
//	EC_RoleRelation_Change_RelationNotExists,//ָ����ϵ������ �޷��޸�
//	EC_RoleRelation_Change_Error,//��ǰ��ϵΪ�����仯 �޷��޸�
//	//RoleShop
//	EC_RoleShop_ShopError,//ָ���̵겻���� �޷�����
//	EC_RoleShop_ItemNotExists,//ָ���̵����Ʒ���� �޷�����
//	EC_RoleShop_ItemNotinTime,//ָ����ƷΪ���� �޷�����
//	EC_RoleShop_EntityItemError,//����ʵ����Ʒ ����������ʾ��ַ������ 
//	EC_RoleShop_MoneyError,//������Ʒʧ�� ���Ҳ���
//	//RoleTable
//	EC_RoleTable_TableError,//ָ�����Ӳ����� �޷�����
//	EC_RoleTable_GlobelMin,//�������̫�� �޷���������
//	EC_RoleTable_GlobelMax,//�������̫�� �޷���������
//	EC_RoleTable_CurreyMin,//�ֱ�����̫�� �޷���������
//	EC_RoleTable_CurreyMax,//�ֱ�����̫�� �޷���������
//	EC_RoleTable_ItemError,//����������Ҫ����Ʒ������ �޷���������
//	//RoleTask
//	EC_RoleTask_NotExites,//�������
//	EC_RoleTask_NotFinih,//��������δ��� �޷��ύ���
//	//RoleTitle
//	EC_RoleTitle_NonTitle,//�ƺŲ����� �޷��л��ƺ�
//	//Package
//	EC_Package_NonItem,//���������
//	EC_Package_BagNonItem,//������û���������
//	EC_Package_Null,//���Ϊ��
//	EC_Package_Succes,//����򿪳ɹ�
//	//Message
//	EC_Message_Length,//������Ϣ���Ȳ���ȷ �޷�����
//	EC_Message_Type,//��Ϣ���ʹ���
//	//ʵ����֤
//	EC_Verification_Sucess,//��֤�ɹ�
//	EC_Verification_IsExists,//�ظ��ύ����
//	EC_Verification_Filed,//��֤ʧ��
//	//�ֻ���
//	EC_BindPhone_PhoneNumberSucess,//�ֻ���֤���Ѿ�����
//	EC_BindPhone_PhoneNumberFailed,//�ֻ���֤�뷢��ʧ��
//	EC_BindPhone_PhoneNumberIsExists,//�ֻ��Ѿ�ע�����
//	EC_BindPhone_Sucess,
//	EC_BindPhone_Failed,
//	//ʵ����Ʒ����
//	EC_EntityItem_Failed,
//	EC_EntityItem_Sucess,
//	//������
//	EC_BindEMail_Failed,
//	EC_BindEMail_Sucess,
//	EC_BindEmail_IsUse,
//	//��ֵ
//	EC_Recharge_Sucess,
//	EC_Recharge_Failed,
//	//�޸�����
//	EC_ResetPassword_Sucess,
//	EC_ResetPassword_Failed,
//	//ʹ����Ʒ
//	EC_UseItem_Sucess,
//	EC_UseItem_Failed,
//	//ʹ��RMB
//	EC_UseRMB_Sucess,
//	EC_UseRMB_Failed,
//	//
//	EC_NickName_Error,
//	EC_AccountName_Error,
//	EC_Account_IsExists,
//	EC_Password_Error,
//	EC_Logon_Error,
//
//	//��½�����µ���ʾ
//	EC_Logon_AccountNotExists,//�˺Ų�����
//	EC_Logon_PasswordError,//������������
//	EC_Rsg_AccountExists,//�˺��Ѿ����� �޷�ע��
//	//�ַ���������ʾ
//	EC_AccountName_StringError,//�˺��ַ�������
//	EC_Password_StringError,//�������
//	EC_NickName_StringError,//�û�������
//	EC_NickName_IsExists,//�û����Ѿ�����
//	EC_MailContext_StringError,//�ʼ����ݴ���
//
//	EC_ResetAccount_Error,//���ο��˺� �޷������˺�
//	EC_ResetAccount_ServerError,//���������߿ͻ��� �޷������˺�
//
//	EC_Max = 0xff,
//};

struct StringArrayData
{
	//����䳤��string�Ĵ���
	USHORT		Length;
	TCHAR		Array[1];//�ַ���
};
struct StringArrayDataByChar
{
	USHORT		Length;
	char		Array[1];//�ַ���
};
struct ChannelUserInfo
{
	//TCHAR			uid[32];
	//TCHAR			channelLabel[32];//����������
	//TCHAR			channeluid[32];//�����û���ID
	//TCHAR			productCode[32];
	//TCHAR			channelCode[32];
	//TCHAR			UserName[32];//�û�����
	//TCHAR			token[32];//��������
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};

template<typename T>
void GetStringArrayVecByData(vector<StringArrayData*>& pVec, T* pData)
{
	pVec.clear();
	if (pData->Sum == 0)
		return;
	DWORD BeginIndex = 0;
	while (pData->Sum - BeginIndex >= (sizeof(StringArrayData)-sizeof(TCHAR)) && pVec.size() < pData->HandleSum)
	{
		StringArrayData* p = (StringArrayData*)&pData->Array[BeginIndex];
		USHORT Length = p->Length;
		pVec.push_back(p);
		BeginIndex += (Length + sizeof(USHORT));
	}
};
template<typename T>
void GetStringArrayVecByData(vector<TCHAR*>& pVec, T* pData)
{
	pVec.clear();
	if (pData->Sum == 0)
		return;
	DWORD BeginIndex = 0;
	while (pData->Sum - BeginIndex >= (sizeof(StringArrayData)-sizeof(TCHAR)) && pVec.size() < pData->HandleSum)
	{
		StringArrayData* p = (StringArrayData*)&pData->Array[BeginIndex];
		USHORT Length = p->Length;

		TCHAR* pStr = (TCHAR*)malloc(Length+sizeof(TCHAR));
		memcpy_s(pStr, Length, p->Array, Length);
		pStr[Length / sizeof(TCHAR)] = 0;
		pVec.push_back(pStr);
		BeginIndex += (Length + sizeof(USHORT));
	}
};
template<typename T>
void GetStringArrayVecByData(vector<char*>& pVec, T* pData)
{
	pVec.clear();
	if (pData->Sum == 0)
		return;
	DWORD BeginIndex = 0;
	while (pData->Sum - BeginIndex >= (sizeof(StringArrayDataByChar)-sizeof(char)) && pVec.size() < pData->HandleSum)
	{
		StringArrayDataByChar* p = (StringArrayDataByChar*)&pData->Array[BeginIndex];
		USHORT Length = p->Length;

		char* pStr = (char*)malloc(Length + sizeof(char));
		memcpy_s(pStr, Length, p->Array, Length);
		pStr[Length / sizeof(char)] = 0;
		pVec.push_back(pStr);
		BeginIndex += (Length + sizeof(USHORT));
	}
};
template<typename T>
void FreeVec(vector<T>& pVec)
{
	if (pVec.empty())
		return; 
	vector<T>::iterator Iter = pVec.begin();
	for (; Iter != pVec.end(); ++Iter)
	{
		free(*Iter);
	}
	pVec.clear();
}


struct tagRoleServerInfo
{
	//������ϵ����� ���ǲ����͵��ͻ���ȥ
	//�ܲ���������
	int64			TotalFishGlobelSum;

	//���ֱ�������
	BYTE			RoleProtectSum;//��������
	__int64			RoleProtectLogTime;//����ʱ��

	//��Ҷ�������
	DWORD			SecPasswordCrc1;
	DWORD			SecPasswordCrc2;
	DWORD			SecPasswordCrc3;
};
struct tagRoleInfo
{
	DWORD			dwUserID;
	INT64			Uid;
	char			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	DWORD			dwExp;//��Ҿ���
	DWORD			dwFaceID;//���ͷ��ID
	char			icon[ICON_LENGTH];
	DWORD			dwHeadIcon; //���ͷ���
	bool			bGender; //����Ա�
	volatile int64			money1;  //��ҳ�ֵ��Ǯ
	volatile int64			money2;  //��Ϸ������Ǯ
	DWORD           dwMedalNum;
	DWORD			dwCurrencyNum;
	BYTE			TitleID;
	char			signature[MAX_SIGNATURE + 1];  //���ǩ��
	//Vip����
	BYTE			VipLevel;//���漰���ݿ��
};
//�����ʵ��ַ
struct tagRoleAddressInfo
{
	TCHAR			Name[MAX_NAME_LENTH + 1];//����
	unsigned __int64			Phone;//�ֻ�����
	TCHAR			Email[MAX_EMAIL_LENGTH + 1];//����
	TCHAR			IdentityID[MAX_IDENTITY_LENGTH + 1];//���֤����
	TCHAR			EntityItemUseName[MAX_NAME_LENTH + 1];//�ջ��˵�����
	unsigned __int64 EntityItemUsePhone;//�ջ��˵��ֻ�����
	TCHAR			EntityItemUseAddres[MAX_ADDRESS_LENGTH + 1];//�ջ��˵���ʵ��ַ
	TCHAR			AlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR			AlipayName[MAX_AlipayName_LENGTH+1];
};

struct tagRemoteRoleInfo
{
	BYTE			SeatID;
	DWORD			dwUserID;
	char			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	//DWORD			dwExp;//��Ҿ���
	DWORD			dwFaceID;//���ͷ��ID
	bool			bGender; //����Ա�
	int64			dwGlobeNum;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ

	DWORD			dwAchievementPoint;//��ҵĳɾ͵���
	BYTE			TitleID;
	//DWORD			ClientIP;//xuda
	TCHAR			IPAddress[MAX_ADDRESS_LENGTH_IP+1];
	BYTE			VipLevel;
	bool			IsInMonthCard;
	DWORD			GameID;
};

struct tagCenterRoleInfo
{
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	//DWORD			dwExp;//��Ҿ���
	DWORD			dwFaceID;//���ͷ��ID
	BYTE			TitleID;
	DWORD			dwAchievementPoint;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	bool			bGender; //����Ա�
	DWORD			ClientIP;//���IP
	bool			IsShowIpAddress;
	BYTE			VipLevel;
	bool			IsInMonthCard;
	bool			IsOnline;
	DWORD			ParticularStates;
	DWORD			GameID;
};
struct tagQueryRoleInfo
{
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	DWORD			dwFaceID;//���ͷ��ID
	bool			bGender; //����Ա� n
	bool			bIsOnline;
	DWORD			dwAchievementPoint;//��ҵĳɾ͵���
	BYTE			TitleID;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	DWORD			ClientIP;//���IP
	bool			IsShowIpAddress;
	BYTE			VipLevel;
	bool			IsInMonthCard;
	DWORD			GameID;
};
struct tagQueryRoleInfoClient
{
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	DWORD			dwFaceID;//���ͷ��ID
	bool			bGender; //����Ա� n
	bool			bIsOnline;
	DWORD			dwAchievementPoint;//��ҵĳɾ͵���
	BYTE			TitleID;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	TCHAR			IPAddress[MAX_ADDRESS_LENGTH_IP+1];
	BYTE			VipLevel;
	bool			IsInMonthCard;
	DWORD			GameID;
};

enum ParticularStates
{
	ParticularState_NiuNiuVip = 1,
	ParticularState_DialVip   = 2,
	ParticularState_CarVip    = 4,
};

struct tagMiniGameRoleInfo
{
	DWORD		dwUserID;
	TCHAR		NickName[MAX_NICKNAME + 1];
	DWORD       FaceID;
	DWORD		dwGlobelSum;
	DWORD       dwMadleSum;
	DWORD		dwCurrceySum;
	BYTE		VipLevel;
	BYTE		GameConfigID;
	DWORD		ParticularStates;
	DWORD		GameID;
};

struct RechageInfo
{
	DWORD			UserID;//���ID
	DWORD			Price;//�۸� ��λ��
	DWORD			FreePrice;
	DWORD			ShopItemID;//���� ��ǰ���� �������Ʒ��ID
	DWORD			ShopNum;//��������
	//TCHAR			orderid[32];
	//TCHAR			ChannelCode[32];//����������
	//TCHAR			channelOrderid[32];//�����û���ID
	//TCHAR			channelLabel[32];
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};

enum EItemType
{
	IT_Globel   = 1,//��� 
	IT_Medal    = 2,//����
	IT_AchievementPoint = 3,//�ɾ͵�
	IT_Title    = 4,//�ƺ�
	IT_Currey   = 5,//�ֱ�
	IT_MonthSocre = 6,//��������
	IT_MonthGlobel=7,//�������
	IT_Normal	  =8,//��ͨ��Ʒ
	IT_Skill      =9,//������Ʒ
	IT_Package    =10,//�����Ʒ
	IT_Charm      =11,//������Ʒ
	IT_Giff		  =12,//������Ʒ
	IT_Cannon     =13,//��̨��Ʒ
	IT_OsCar	  =14,//Ǳͧ��Ʒ
	IT_Entity	  =15,//ʵ����Ʒ ��ʵ����Ʒ
	IT_MonthCard  =16,//�¿���Ʒ
	IT_GlobelBag  =17,//�۱�����Ʒ
	IT_Horn		  =18,//����

};
struct tagItemConfig
{
	DWORD	ItemID;//��ƷID
	BYTE    ItemType;//��Ʒ����
	DWORD   ItemParam;//��Ʒ����ֵ  ���⴦���õ� ����û�� û��Ϊ0
};

struct tagItemOnce  //�����ļ�ʹ�õ����� 
{
	DWORD		ItemID;
	DWORD		ItemSum;//��Ʒ�����޸�
	DWORD		LastMin;//��Ʒ����ķ���
};
struct tagItemInfo//��Ҿ��������
{
	DWORD		ItemOnlyID;//��Ʒ��ΨһID  ����ID
	DWORD		ItemID;//��ƷID
	DWORD		ItemSum;//��Ʒ����
	__int64     EndTime;//��Ʒ����ʱ��
};
enum RelationType
{
	RT_Friend = 1,
	RT_Black = 2,
};
struct tagRoleRelation
{
	//һ����ϵ����
	DWORD			dwDestRoleID;
	DWORD			dwFaceID;
	WORD			wLevel;
	bool			bGender;
	//bool			bIsBe;
	//BYTE			bLastLogonTime;//��������ʱ��
	__int64			LastLogonTime;
	bool			IsOnline;
	TCHAR			DestNickName[MAX_NICKNAME + 1];
	BYTE			bRelationType;//��ϵ����
	BYTE			TitleID;
	DWORD			dwAchievementPoint;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	DWORD			ClientIP;//���IP
	bool			IsShowIpAddress;
	BYTE			VipLevel;
	bool			IsInMonthCard;
	DWORD			GameID;
};
struct tagBeRoleRelation//������ϵ����
{
	DWORD			dwUserID;
	BYTE			bRelationType;
};
struct tagRoleRelationClient
{ 
	DWORD			dwDestRoleID;
	DWORD			dwFaceID;
	WORD			wLevel;
	bool			bGender;
	BYTE			bDiffDay;//����������
	TCHAR			DestNickName[MAX_NICKNAME + 1];
	BYTE			bRelationType;//��ϵ����
	BYTE			TitleID;
	DWORD			dwAchievementPoint;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	//DWORD			ClientIP;//���IP //xuda
	TCHAR			IPAddress[MAX_ADDRESS_LENGTH_IP+1];
	BYTE			VipLevel;
	bool			IsInMonthCard;
	DWORD			GameID;
};
//����������Ҫһ��ͳһ���ʼ���ʽ
struct tagRoleMail   //�����ݿ��ȡ�������ʼ���Ϣ
{
	DWORD		MailID;
	DWORD		SrcUserID;//���ͷ���ID
	DWORD		SrcFaceID;
	TCHAR		SrcNickName[MAX_NICKNAME + 1];
	__int64		SendTimeLog;//����ʱ��
	TCHAR		Context[MAX_MAIL_CONTEXT + 1];//�ʼ�����
	WORD		RewardID;//���� ����
	DWORD		RewardSum;//����������
	bool		bIsRead;//�Ƿ��Ѿ��Ķ�
	bool		bIsExistsReward;//�Ƿ���ڽ���
};
struct tagNormalMailTitle
{
	DWORD		MailID;
	DWORD		SrcUserID;//���ͷ���ID
	DWORD		SrcFaceID;
	TCHAR		SrcNickName[MAX_NICKNAME + 1];
	BYTE		bDiffTime;//����������
	bool		bIsRead;
};
struct tagNormalMail
{
	DWORD		MailID;
	DWORD		SrcUserID;//���ͷ���ID
	DWORD		SrcFaceID;
	TCHAR		SrcNickName[MAX_NICKNAME + 1];
	TCHAR		Context[MAX_MAIL_CONTEXT + 1];//�ʼ�����
	BYTE		bDiffTime;//����������
	bool		bIsRead;
};
struct tagSystemMailTitle
{
	DWORD		MailID;
	BYTE		bDiffTime;//����������
	bool		bIsRead;//�Ƿ��Ѿ��Ķ�
	bool		bIsExistsItem;//�Ƿ���ڸ���
};
struct tagSystemMail
{
	DWORD		MailID;
	TCHAR		Context[MAX_MAIL_CONTEXT + 1];//�ʼ�����
	WORD		RewardID;
	DWORD		RewardSum;//����������
	BYTE		bDiffTime;//����������
	bool		bIsRead;//�Ƿ��Ѿ��Ķ�
	bool		bIsExistsReward;//�Ƿ���ڽ���
};
//ǩ��
//struct tagRoleCheckInfo
//{
//	//BYTE		bMontheCheckSum;//����ǩ������Ϣ��¼ 0-30��  ÿ���Ƿ�ǩ��
//	//BYTE		bCheckSum;//����ǩ���Ĵ��� ���汾�µ���Ϣ�޸�
//	DWORD		MonthCheckInfo;//����ǩ��������
//	BYTE		bLastLogYear;//����ǩ���� �� �� ��
//	BYTE		bLastLogMonth;
//	BYTE		bLastLogDay;
//};
//������¼�����
enum EventTargetType  //�������ط����õĹ��� �����������ص�����
{
	ET_GetGlobel = 1,//��ý�� �ۼӵ�
	ET_GetMadel = 2,//��ý��� �ۼ�
	ET_GetCurren = 3,//����ֱ� �ۼ�
	ET_UpperLevel = 4,//�����ȼ� �ۼӵȼ�
	ET_CatchFish = 5,//������   �󶨲���0 ��ʾ������
	ET_SendGiff = 6,//�������� 
	ET_UseSkill = 7,//ʹ�ü���
	ET_LauncherGlobel = 8,//��̨��ý��
	ET_MaxGlobelSum = 9,//ӵ�е���������� ��Ҫ�޸� ��������� Ӧ�ô����ȵ�
	ET_ToLevel = 10,//��ָ���ȼ�
	ET_MaxCurren = 11,//���ӵ���ֱ�  
	ET_Recharge  = 12,//��ֵ������¼�
	ET_Max,
};

struct EventInfo  //�¼� �Ѿ��¼����Ĳ���  ���� ���� �� �ɾ�
{
	BYTE	OnlyID;//ΨһID �����¼�ID ��ͬһ���ڵ��²������ظ�
	BYTE	EventID;
	DWORD	BindParam;//��ǰ�¼��󶨵Ĳ��� ����Ϊ��
	DWORD	FinishValue;//��ǰ�¼�����ɲ���
};
struct tagRoleTaskInfo//����Ľṹ����
{
	BYTE		TaskID;//����ID ��0�� 256  ����ֱ�ӽ�������
	DWORD		TaskValue;
};
//�ɾ͵�����
struct tagRoleAchievementInfo
{
	BYTE		AchievementID;
	DWORD		AchievementValue;
};

//�������
struct tagRoleActionInfo
{
	BYTE		ActionID;
	DWORD		ActionValue;
	DWORD		ActionStates;
};
//����
struct tagRoleMonthInfo //��ҵı�����Ϣ
{
	DWORD		dwUserID;
	DWORD		dwIndex;
	BYTE		bMonthID;
	DWORD		dwMonthGlobel;
	DWORD		dwMonthScore;
	BYTE		wUserAddGlobelNum;//���ҵĴ���
	DWORD		SkillSum;//ʹ�ü��ܵĴ���
};
struct tagRoleMonthCenterInfo
{
	tagRoleMonthInfo		MonthInfo;
	DWORD					FaceID;
	float					VipAddScore;
	TCHAR					NickName[MAX_NICKNAME + 1];	
};
//Rank
struct tagRankInfo
{
	DWORD		dwUserID;
	BYTE		bIndex;
	DWORD		dwFaceID;
	BYTE		TitleID;
	bool		Gender;
	TCHAR		NickName[MAX_NICKNAME + 1];
	__int64		Param;
};

//���� giff
struct tagGiffInfo
{
	DWORD		OnlyID;
	DWORD		dwUserID;
	DWORD		dwFaceID;
	TCHAR		NickName[MAX_NICKNAME + 1];
	__int64		SendTime;
};
struct tagGiffClientInfo
{
	DWORD		OnlyID;
	DWORD		dwUserID;
	DWORD		dwFaceID;
	TCHAR		NickName[MAX_NICKNAME + 1];
	BYTE		DiffTime;//0-250
};


//�����ļ��ṹ��
//ǩ��
struct RoleCheckConfig
{
	DWORD			CheckOtherUser;//��ǩ��Ҫ�Ļ���
	DWORD			MaxCheckDaySum;
	HashMap<DWORD, WORD> CheckDayReward;
	HashMap<DWORD, WORD>  MonthCheckReward;
};

//����������ļ�����
struct tagTaskConfig
{
	BYTE		TaskID;	//����ID 
	//���������Ǵӷ�������ȡ�� TASK_MAXEVENTSUM ����������� 
	EventInfo	EventInfo;
	BYTE		UpperTaskID; //�ϼ�����ID
	bool		IsDayClear; //�Ƿ�Ϊÿ������
	//DWORD		RewardGlobel;//�����Ľ�Ǯ
	//DWORD		RewardMabel;//�����Ľ���
	//DWORD		RewardCurreny;//�������ֱ�
	WORD		RewardID;
	BYTE		GroupID;//ͬһ�����µ����񻥳� ���� ֻ��ÿ������ſ��Խ��з���
	BYTE		JoinLevel;
	vector<BYTE> LowerTaskVec;//�¼�������ID�б�
};
struct tagTaskMap
{
	BYTE							m_MaxJoinTaskSum;
	HashMap<BYTE, tagTaskConfig>  m_TaskMap;
	HashMap<BYTE, std::vector<BYTE>> m_TaskGroup;
};
//�ɾ�����
struct tagAchievementConfig
{
	BYTE		AchievementID;
	EventInfo	AchievementEventInfo;
	BYTE		UpperAchievementID;
	WORD		RewardID;
	BYTE		JoinLevel;
	vector<BYTE> LowerAchievementVec;
};
struct tagAchievementMap
{
	BYTE								m_MaxJoinAchievementSum;
	HashMap<BYTE, tagAchievementConfig>	m_AchievementMap;
};
//�
struct tagActionEventConfig
{
	DWORD			ID;//�׶ε�ΨһID
	//DWORD			BindParam;
	DWORD			FinishValue;
	WORD			RewardID;
};
struct tagActionConfig //һ��ϵ�еĻ  ��������¼� �Ѿ���ͬ�Ľ���
{
	BYTE		ActionID;
	BYTE		EventID;//һ���  һ���¼�
	DWORD		BindParam;
	DWORD		FinishValue;//�������ɵ��ܵ�ֵ
	HashMap<DWORD, tagActionEventConfig> EventMap;

	__int64		BeginTime;
	WORD		LastHour;

	bool IsInTime()
	{
		if (LastHour == 0)//����СʱΪ0��ʱ�� ��Զ���ּ���״̬
			return true;
		time_t now = time(NULL);
		if (now >= BeginTime && now <= BeginTime + LastHour * 3600)
			return true;
		else
			return false;
	}
};
struct tagActionGroupMap
{
	BYTE									m_MaxJoinActionSum;
	HashMap<BYTE, tagActionConfig>			m_ActionMap;
};
//��������
struct tagMonthConvert
{
	BYTE		ConvertSum;
	DWORD		LostGlobel;
	DWORD		LostMadle;
	DWORD		LostCurrey;
	DWORD		AddMonthGlobel;
};
struct tagMonthTime
{
	BYTE		ID;//��ǵ�ǰ�Ľ׶�
	//��ʼ�׶�
	BYTE		BeginWeek;
	BYTE		BeginHour;
	BYTE		BeginMin;
	//WORD		LastMin;
	//�׶εĳ���ʱ��
	WORD		AllLastMin;//����ڵ����ʱ��
	//�׶εĵȴ�ʱ��
	WORD		WriteLastMin;//��һ���׶εĿ�ʼʱ��
	//�����׶�
	WORD		SignWriteMin;
	WORD		SignLastMin;
	//��ʼ�׶�
	WORD		StarWriteMin;
	WORD		SratLastMin;
	//
	bool		IsContinue;
	WORD		RunSum;//ѭ���Ĵ���

	__int64     GetBeginTime()
	{
		//��ȡ��ʼʱ��
		time_t Now = time(NULL);
		tm NowTime;
		errno_t Err = localtime_s(&NowTime, &Now);
		if (Err != 0)
		{
			ASSERT(false);
			return 0;
		}
		if (BeginWeek == 8)
		{
			//8 ��ʾÿһ�� ������ 
			int DiffHour = NowTime.tm_hour - BeginHour;
			int DiffMin = NowTime.tm_min - BeginMin;
			int DiffSec = DiffHour * 3600 + DiffMin * 60;
			if (DiffSec >= 0)
				return (Now - DiffSec);//��ȡ�ϴεĿ�ʼʱ��
			else
			{
				//����� 
				DiffSec += 24 * 3600;
				return (Now - DiffSec);//��ȡ�ϴεĿ�ʼʱ��
			}
		}
		else
		{
			int DiffDay = NowTime.tm_wday - BeginWeek;//( < 0) ? (7 + NowTime.tm_wday - BeginWeek) : (NowTime.tm_wday - BeginWeek);//��������
			int DiffHour = NowTime.tm_hour - BeginHour;
			int DiffMin = NowTime.tm_min - BeginMin;
			int DiffSec = DiffDay * 24 * 3600 + DiffHour * 3600 + DiffMin * 60;
			//�޸��� 
			if (DiffSec < 0)
			{
				DiffSec = 7 * 3600 * 24 + DiffSec;
			}
			return (Now - DiffSec);//��ȡ�ϴεĿ�ʼʱ��
		}
		
	}
	DWORD		IsInMonthTime()
	{
		time_t Now = time(NULL);//��ǰʱ��
		time_t BeginTime = GetBeginTime();//�����Ŀ�ʼʱ��
		if (BeginTime > Now)
		{
			ASSERT(false);
			return 0;
		}
		//��Щ��������ѭ���� ������Ҫ�ж�
		if (!IsContinue)
		{
			//һ���Ե�ʱ���жϺ���
			if (Now < (BeginTime + AllLastMin*60))
			{
				return static_cast<DWORD>(ID << 16);
			}
			else
				return 0u;
		}
		DWORD DiffSec = static_cast<DWORD>(Now - BeginTime);
		//һ�������׶ε�ʱ�� AllLastMin + WriteLastMin
		DWORD nValue = (DiffSec/60) % (AllLastMin + WriteLastMin);

		//DWORD ContinueSum = (nValue == 0 ? ((DiffSec / 60) / (AllLastMin + WriteLastMin)) : ((DiffSec / 60) / (AllLastMin + WriteLastMin) + 1));//ѭ���Ĵ���
		DWORD ContinueSum = (DiffSec / 60) / (AllLastMin + WriteLastMin);
		if (ContinueSum >= RunSum)
		{
			//�Ѿ������� ����ѭ��
			return false;
		}
		else if (nValue < AllLastMin)
		{
			WORD Sum = static_cast<WORD>((DiffSec / 60) / (AllLastMin + WriteLastMin));
			return static_cast<DWORD>((ID << 16) + Sum);
		}
		else if (nValue >= static_cast<DWORD>(AllLastMin) && nValue < static_cast<DWORD>(AllLastMin + WriteLastMin))
			return 0;
		else
		{
			ASSERT(false);
			return 0;
		}
	}
	bool		IsInSignTime()
	{
		//����ʱ�� Ϊ [) ������ ���һ����
		//�Ƿ��ڱ����׶�
		time_t Now = time(NULL);//��ǰʱ��
		time_t BeginTime = GetBeginTime();//�����Ŀ�ʼʱ��
		if (BeginTime > Now)
		{
			ASSERT(false);
			return false;
		}
		if (!IsContinue)
		{
			//һ���Ե�ʱ���жϺ���
			if (Now < (BeginTime + AllLastMin * 60))
			{
				if (Now >= (BeginTime + SignWriteMin * 60) && Now < (BeginTime + SignWriteMin * 60 + SignLastMin * 60))
					return true;
				else
					return false;
			}
			else
				return false;
		}
		DWORD DiffSec = static_cast<DWORD>(Now - BeginTime);
		//һ�������׶ε�ʱ�� AllLastMin + WriteLastMin
		DWORD nValue = (DiffSec / 60) % (AllLastMin + WriteLastMin);
		//DWORD ContinueSum = (nValue == 0 ? ((DiffSec / 60) / (AllLastMin + WriteLastMin)) : ((DiffSec / 60) / (AllLastMin + WriteLastMin) + 1));//ѭ���Ĵ���
		DWORD ContinueSum = (DiffSec / 60) / (AllLastMin + WriteLastMin);
		if (ContinueSum >= RunSum)
		{
			//�Ѿ������� ����ѭ��
			return false;
		}
		else if (nValue < AllLastMin)
		{
			//�ڱ����׶� �����ж�
			if (nValue >= static_cast<DWORD>(SignWriteMin) && nValue < static_cast<DWORD>(SignWriteMin + SignLastMin))
				return true;
			else
				return false;
		}
		else if (nValue >= static_cast<DWORD>(AllLastMin) && nValue < static_cast<DWORD>(AllLastMin + WriteLastMin))
			return false;
		else
		{
			ASSERT(false);
			return false;
		}
	}
	bool		IsInStarTime()
	{
		//�Ƿ��ڱ����׶�
		time_t Now = time(NULL);//��ǰʱ��
		time_t BeginTime = GetBeginTime();//�����Ŀ�ʼʱ��
		if (BeginTime > Now)
		{
			ASSERT(false);
			return false;
		}
		if (!IsContinue)
		{
			//һ���Ե�ʱ���жϺ���
			if (Now < (BeginTime + AllLastMin * 60))
			{
				if (Now >= (BeginTime + StarWriteMin * 60) && Now < (BeginTime + StarWriteMin * 60 + SratLastMin * 60))
					return true;
				else
					return false;
			}
			else
				return false;
		}
		DWORD DiffSec = static_cast<DWORD>(Now - BeginTime);
		//һ�������׶ε�ʱ�� AllLastMin + WriteLastMin
		DWORD nValue = (DiffSec / 60) % (AllLastMin + WriteLastMin);
		//DWORD ContinueSum = (nValue == 0 ? ((DiffSec / 60) / (AllLastMin + WriteLastMin)) : ((DiffSec / 60) / (AllLastMin + WriteLastMin) + 1));//ѭ���Ĵ���
		DWORD ContinueSum = (DiffSec / 60) / (AllLastMin + WriteLastMin);
		if (ContinueSum >= RunSum)
		{
			//�Ѿ������� ����ѭ��
			return false;
		}
		else if (nValue < AllLastMin)
		{
			//�ڱ����׶� �����ж�
			if (nValue >= static_cast<DWORD>(StarWriteMin) && nValue < static_cast<DWORD>(StarWriteMin + SratLastMin))
				return true;
			else
				return false;
		}
		else if (nValue >= static_cast<DWORD>(AllLastMin) && nValue < static_cast<DWORD>(AllLastMin + WriteLastMin))
			return false;
		else
		{
			ASSERT(false);
			return false;
		}
	}
};
struct tagMonthReward
{
	WORD		Index;
	WORD		RewardID;
};
struct tagMonthRobot
{
	DWORD		Level;
	DWORD		RobotSum;
	DWORD		AddScore;
	DWORD		Rate;
	DWORD		MaxScore;
	DWORD		SumRate;
	//���ܵ�����
	BYTE		SkillSum;//����ʹ�ô��� 
	DWORD		SkillUseRate;//ÿ��ʹ�ü��ܵĸ���
	DWORD		SkillScore;//ÿ�μ������ӵĻ���
	DWORD		SkillScoreRate;//���ܻ��ֲ�������
};
struct tagMonthConfig
{
	BYTE		MonthID;
	DWORD		SignGlobel;
	DWORD       SignMadel;
	DWORD		SignCurrey;
	tagItemOnce SignItem;
	DWORD		InitMonthGlobel;
	BYTE		TableTypeID;
	BYTE		RankSum;
	BYTE		MinRateLimit;
	BYTE		TypeID;
	HashMap<DWORD, tagMonthRobot> m_RobotMap;
	HashMap<BYTE, tagMonthConvert> m_ConvertMap;//���ҵļ���
	//std::vector<tagMonthTime> m_MonthTime;
	HashMap<BYTE, tagMonthTime> m_MonthTime;
	HashMap<WORD, tagMonthReward> m_MonthReward;

	DWORD GetMonthTimeID()
	{
		//��ȡ��ǰ���ڽ��еı���ʱ���ΨһID
		if (m_MonthTime.empty())
			return 0;
		HashMap<BYTE, tagMonthTime>::iterator Iter = m_MonthTime.begin();
		for (; Iter != m_MonthTime.end(); ++Iter)
		{
			DWORD nValue = Iter->second.IsInMonthTime();
			if (nValue != 0)
				return nValue;
		}
		return 0;
	}
};
struct tagMonthMap
{
	BYTE								m_MonthBeginLevel;
	HashMap<BYTE, tagMonthConfig>		m_MonthMap;
};

//���а�
struct tagRankReward
{
	BYTE		Index;
	WORD		RewardID;
	//tagItemOnce ItemInfo;
};

struct tagRankConfig
{
	BYTE		RankID;
	BYTE		RowCount;
	HashMap<BYTE, tagRankReward> RewardMap;
};
struct tagRankMap
{
	BYTE						   RankWeekDay;
	HashMap<BYTE, tagRankConfig>  m_RankMap;
};
//����
struct tagChestsCost
{
	BYTE					MaxCostNum;
	HashMap<BYTE, DWORD>	CostMap;//�鱦��Ļ���
};
struct tagChestsRewardOnce
{
	BYTE					OnceID;
	DWORD					Chances;
	WORD					RewardID;
};
struct tagChestsReward
{
	BYTE					RewardID;
	DWORD					Chances;
	DWORD					MaxOnceChances;
	std::vector<tagChestsRewardOnce> RewardMap;
};
struct tagChestsAllReward
{
	BYTE				 SpecialRewardTypeID;
	BYTE				 SpecialRewardUseRp;
	BYTE				 OtherAddRp;
	DWORD				 MaxRewardChances;
	//std::vector<tagChestsReward> RewardVec;
	HashMap<BYTE, tagChestsReward> RewardMap;
};
struct tagChestConfig
{
	BYTE				ChestID;
	WORD				CatchChance;
	BYTE				ExisteSec;
	WORD				ImmediateRewardid;
	tagChestsCost		CostInfo;
	tagChestsAllReward	RewardInfo;
};
struct tagChestMap
{
	HashMap<BYTE, tagChestConfig>  ChestMap;
};
//���������ļ�
struct tagCharmOnce
{
	BYTE			Index;
	DWORD			UserGlobel;//���ĵĽ��
	DWORD			UserMedal;
	DWORD			UserCurrcey;
	DWORD			ItemID;
	DWORD			ItemSum;
	int				ChangeCharmValue;
};
struct tagCharmConfig
{
	BYTE				 CharmMaxItem;
	HashMap<BYTE, tagCharmOnce> CharmIndexMap;
};
//�̵�������ļ�
struct tagShopItemTimeConfig
{
	__int64 BeginTime;
	WORD	LastHour;
};

enum ShopItemType
{
	SIT_Normal = 0,//��ͨ��Ʒ
	SIT_Entity = 1,//ʵ����Ʒ 
	SIT_PhonePay = 2,//�ֻ�����
	SIT_MonthCard=3,//�¿���Ʒ
	SIT_GlobelBag = 4,//�۱���
};

struct tagShopItemConfig//��Ʒ������
{
	BYTE		ShopItemIndex;
	tagItemOnce ItemInfo;
	DWORD		PriceGlobel;
	DWORD		PriceMabel;
	DWORD		PriceCurrey;
	//bool		IsEntityItem;//�Ƿ�Ϊʵ�����Ʒ
	BYTE		ShopType;//��Ʒ����������
	bool		IsCanPile;//�Ƿ����һ�ι�����
	std::vector<tagShopItemTimeConfig> TimeVec;//��Ʒ��ʱ������ �޵Ļ���ʾ������
};
struct tagShopConfig
{
	BYTE		ShopID;
	HashMap<BYTE, tagShopItemConfig>  ShopItemMap;
	std::vector<tagShopItemConfig> ShopItemVec;
};
struct tagShopConfigMap
{
	HashMap<BYTE, tagShopConfig>		ShopMap;
	std::vector<tagShopConfig>			ShopVec;
};
//Game
struct tagShopItemRunTimeData
{
	BYTE									ShopID;
	std::vector<DWORD>						ItemVec;
	HashMap<DWORD, bool>					ShopItem;
};
struct tagShopRunTimeData
{
	std::vector<tagShopItemRunTimeData*>		ShopVec;
	HashMap<BYTE, tagShopItemRunTimeData*>		ShopMap;
};
//Giff
struct tagGiffConfig
{
	BYTE			LimitDay;
	BYTE			SendSumLimitByDay;
	BYTE			AcceptSubLimitByDay;
	BYTE			SendOnceUserLimitByDay;
	//DWORD			RewardGlobel;
	//tagItemOnce		ItemInfo;
	WORD			RewardID;
	BYTE			MaxUserGiffSum;
};
//TableConfig
struct tagTableJoinItem
{
	DWORD		ItemID;
	DWORD		ItemSum;
};
struct TableInfo
{
	BYTE		TableTypeID;//���ӵ�ID
	DWORD		MinGlobelSum;
	DWORD		MaxGlobelSum;
	double       BasicRatio;   //���ӻ�������
	DWORD		MinCurreySum;
	DWORD		MaxCurreySum;
	DWORD		MinLevel;
	DWORD	    MaxLevel;
	BYTE		MinRate;
	BYTE		MaxRate;
	USHORT      TableRate;
	BYTE		wMaxPlayerSum;//�����������
	TCHAR		cMapName[MAX_MAP_NAME + 1];//��ͼ����
	int64		InitProduce;
	int64		InitEarn;
	int64		InitTurn;
	vector<tagTableJoinItem> JoinItemVec;//���뷿����Ҫ������
};
struct tagTableConfig
{
	HashMap<BYTE, TableInfo> m_TableConfig;
};
//Relation
struct tagRelationConfig
{
	BYTE		MaxRelationSum;
	BYTE		MaxRelationRequestSum;
	BYTE		RelationRequestLimitDay;
};
//��Ʒ����ҵ�ֱ��ת��
struct tagItemConvert
{
	DWORD		 ItemID;
	DWORD		 Globel;
	DWORD		 Medal;
	DWORD		 Currey;
};
struct tagItemConvertMap
{
	HashMap<DWORD, tagItemConvert>		m_ConvertMap;
};
//��������ϵ����
struct tagFishServerUpdate
{
	BYTE		UpdateHour;
	BYTE		UpdateMin;

	bool  IsChangeUpdate(__int64 LogTime)
	{
		DWORD WriteSec = static_cast<DWORD>(UpdateHour * 3600 + UpdateMin * 60);
		//��ʱ����д���
		time_t Now = time(NULL);
		Now -= WriteSec;
		if (Now < 0)
			Now = 0;
		LogTime -= WriteSec;
		if (LogTime < 0)
			LogTime = 0;

		tm NowTime;
		errno_t Err = localtime_s(&NowTime, &Now);
		if (Err != 0)
		{
			ASSERT(false);
			return false; 
		}
		tm LogTm;
		Err = localtime_s(&LogTm, &LogTime);
		if (Err != 0)
		{
			ASSERT(false);
			return false;
		}
		return (LogTm.tm_mday != NowTime.tm_mday || LogTm.tm_mon != NowTime.tm_mon || LogTm.tm_year != NowTime.tm_year);
	}
	bool IsChangeUpdate(__int64 LogTime, __int64 OnLineTime)
	{
		//�жϵ�ǰʱ�� �� ��¼ʱ�� �Ƿ񴩹��˸���ʱ�� �жϼ�¼�� ��ǰ�Ƿ�Ϊ2��
		DWORD WriteSec = static_cast<DWORD>(UpdateHour * 3600 + UpdateMin * 60);
		//�ж��޸ĺ�������Ƿ�Ϊͬһ�� 
		OnLineTime -= WriteSec;
		if (OnLineTime < 0)
			OnLineTime = 0;
		LogTime -= WriteSec;
		if (LogTime < 0)
			LogTime = 0;

		tm NowTime;
		errno_t Err = localtime_s(&NowTime, &OnLineTime);
		if (Err != 0)
		{
			ASSERT(false);
			return false;
		}
		tm LogTm;
		Err = localtime_s(&LogTm, &LogTime);
		if (Err != 0)
		{
			ASSERT(false);
			return false;
		}
		return (LogTm.tm_mday != NowTime.tm_mday || LogTm.tm_mon != NowTime.tm_mon || LogTm.tm_year != NowTime.tm_year);
	}
};
struct tagMailConfig
{
	BYTE MailLimitDay;
	//BYTE MailMaxJoinSum;
	BYTE MaxUserMailSum;
	BYTE ClientShowMailSum;
};
//����̵�
//struct tagGlobelShopItem
//{
//	BYTE	ID;
//	DWORD   LostCurrey;
//	DWORD	AddGlobel;
//};
//struct tagGlobelShop
//{
//	HashMap<BYTE, tagGlobelShopItem> m_ShopItem;
//};
//���߽���
struct tagOnceOnlienReward
{
	BYTE	ID;
	WORD	OnlineMin;
	WORD	RewardID;
	//tagItemOnce ItemInfo;
};
struct tagOnlineReward
{
	HashMap<BYTE, tagOnceOnlienReward> m_OnlineRewardMap;//���߽������б�
};

//RoleGameData
struct tagRoleGameData
{
	int64 turn_LoseWinGold[4]; //����ǰÿ��������Ӯ�������

	tagRoleGameData()
	{
		turn_LoseWinGold[0] = 0;
		turn_LoseWinGold[1] = 0;
		turn_LoseWinGold[2] = 0;
		turn_LoseWinGold[3] = 0;
	}
};

//��� 
struct tagFishPackageItem
{
	WORD		RewardID;
	DWORD		RankValue;

	tagFishPackageItem()
	{
		RewardID = 0;
		RankValue = 0;
	}
};
struct tagFishPackageGroup
{
	BYTE			GroupID;
	DWORD			MaxRankValue;
	//�������  ��� ���� ��ʯ ��Ʒ 4������
	vector<tagFishPackageItem> PackageItemVec;//ֻ���Դ��в���һ����Ʒ
};
struct tagFishPackage
{
	DWORD			ItemID;
	bool			IsAutoOpen;
	vector<tagFishPackageGroup>	ItemVec;//�������������Ʒ
};
struct tagFishPckageMap
{
	HashMap<DWORD, tagFishPackage>	m_PackageMap;//ȫ������ļ���
};
//ȫ�ֽ�������
struct tagRewardOnce
{
	WORD		RewardID;
	vector<tagItemOnce> RewardItemVec;
};
struct tagRewardMap
{
	HashMap<WORD, tagRewardOnce> RewardMap;
};
struct tagFishSystemInfo//ϵͳ��Ϣ
{
	//������ϵͳ����
	DWORD		VersionID;//�汾ID
	DWORD		PathCrc;//·��CRC
	DWORD		CacheUserMin;//����ķ���
	DWORD		AchievementRankSum;//�ɾ����а������
	DWORD		RsgInitGlobelSum;//��ʼ����ҽ����
	DWORD		AnnouncementSum;//������������
	WORD		FirstBindPhoneRewardID;//�״ΰ��ֻ��Ľ���
	DWORD		UserQueueHandleSumBySec;//������ÿ�봦�����ҵ�½�ĸ���
	INT64		MaxGobelSum;//42Y ������������
	DWORD		ChangeNickNameNeedItemID;//�޸�������Ʒ
	DWORD		ChangeNickNameNeedItemSum;
	DWORD		ChangeGenderNeedItemID;//�޸��Ա���Ʒ
	DWORD		ChangeGenderNeedItemSum;
	BYTE		LeaveTableNonLauncherMin;//�߳����ӵ�δ����ʱ��
	DWORD		InitRateValue;//��ʼ����ʱ�� ���Ĭ�ϵı���״̬
	WORD		EmailGlobelRewardID;
	WORD		EmailCurrceyRewardID;
	WORD		EmailMedalRewradID;
	BYTE		RoleSaveMin;
	__int64		RateInitRewardTime;
	DWORD		SendMessageItemID;
	WORD		SendMessageItemSum;
	DWORD		CashGoldNum;
	DWORD		MinCashRMBNum;
	DWORD		OneDayCashNum;
	WORD		BindAccountReward;
};
enum RechargeType
{
	RT_FirstCurrcey = 1,//�׳���ʯ
	RT_FirstGlobel = 2,//�׳���
	RT_Currcey = 3,//��ֵ��ʯ
	RT_Globel = 4,//��ֵ���
	RT_Reward = 5,//��ֵ����ID
};
//�����ת �ֱҲ���
struct tagFishRechargeInfo
{
	DWORD		ID;
	DWORD		dDisCountPrice;//��ֵ ��λԪ
	DWORD		dPreDisCountPrice;//�ۿ�ǰ�ļ۸� ��λԪ
	WORD		RewardID;
	BYTE		RechargeType;//��ֵ����
	DWORD		AddMoney;//��ӻ�������
	/*bool		IsCurreyOrGlobel;
	bool		IsFirstPay;*/

	bool IsAddGlobel()
	{
		if (RechargeType == RT_FirstGlobel || RechargeType == RT_Globel)
			return true;
		else
			return false;
	}
	bool IsAddCurrcey()
	{
		if (RechargeType == RT_FirstCurrcey || RechargeType == RT_Currcey)
			return true;
		else
			return false;
	}
	bool IsAddReward()
	{
		if (RechargeType == RT_Reward)
			return true;
		else
			return false;
	}
	bool IsFirstAdd()
	{
		if (RechargeType == RT_FirstCurrcey || RechargeType == RT_FirstGlobel)
			return true;
		else
			return false;
	}
};
struct tagFishRechargesMap
{
	HashMap<DWORD, tagFishRechargeInfo> m_FishRechargeMap;
};
struct tagErrorStr
{
	TCHAR		ErrorStr[MAX_ERRORSTR_LENGTH + 1];
};
struct tagErrorMap
{
	vector<tagErrorStr>	ErrorStrVec;
};
//Announcement
struct AnnouncementOnce //����ֺܶ�����
{
	//һ������
	//TCHAR    AnnouncementStr[MAX_Announcement_Length]; //������ַ���
	TCHAR		NickName[MAX_NAME_LENTH + 1];
	BYTE		ShopID;
	BYTE		ShopOnlyID;
};

//LevelReward
struct LevelRewardMap
{
	HashMap<WORD, WORD>  m_LevelRewardMap;
};

//ExChange
struct tagExChange
{
	BYTE			ID;
	BYTE			TypeID;
	DWORD			ChannelID;
	WORD			RewardID;
};
struct tagExChangeMap
{
	HashMap<BYTE, tagExChange> m_ExChangeMap;
};

struct tagRoleProtectConfig
{
	BYTE			ProtectSum;
	BYTE			ProtectCDMin;
	DWORD			AddGlobel;
	DWORD			MaxGlobelSum;
	DWORD			OpenProtectGlobelSum;
	HashMap<BYTE, BYTE> TableMap;//��������ӵ����͵ļ���
};

struct tagNotice
{
	DWORD			ID;
	time_t			BeginTime;
	DWORD			StepNum;
	DWORD			StepMin;
	BYTE			OnceStepNum;
	BYTE			OnceStepSec;
	DWORD			MessageInfoColor;
	TCHAR			MessageInfo[MAX_MESSAGE_LENGTH+1];
};

struct tagLotteryReward
{
	DWORD		Rate;
	WORD		RewardID;
};
struct tagLotteryOnce
{
	BYTE  LotteryID;
	BYTE  UpperLotteryID;
	DWORD NeedUseScore;
	vector<tagLotteryReward> RewardVec;
	DWORD TotalRate;
};
struct tagLotteryConfig
{
	BYTE						  MaxLotteryFishSum;//������
	HashMap<BYTE, tagLotteryOnce> LotteryMap;
	HashMap<BYTE, DWORD>	      FishScore;

	DWORD GetFishLotteryScore(BYTE FishTypeID,BYTE LotteryFishSum)
	{
		if (LotteryFishSum >= MaxLotteryFishSum)
			return 0;
		HashMap<BYTE, DWORD>::iterator Iter= FishScore.find(FishTypeID);
		if (Iter == FishScore.end())
			return 0;
		else
			return Iter->second;
	}
	
};

struct tagVipOnce
{
	BYTE VipLevel;//VIP�ȼ�
	BYTE UpperVipLevel;//��һ����VIP�ȼ�����
	DWORD NeedRechatgeRMBSum;//��ǰ�ȼ���Ҫ�����������
	BYTE LauncherReBoundNum;//�����ӵ���������
	//bool IsCanLauncherLocking;//�Ƿ����ʹ����̨����
	BYTE AddAlmsSum;//���Ӿȼý�Ĵ���
	DWORD AddAlmsRate;//���Ӿȼý�İٷֱ���
	DWORD AddMonthScoreRate;//���ӱ������ֵİٷֱ���
	DWORD AddReChargeRate;//��ֵ�����İٷֱ�
	DWORD AddCatchFishRate;//�����������
	BYTE  AddUseMedalSum;//�һ���������
	std::set<BYTE> CanUseLauncherMap;//VIP����ʹ�õ��ڵļ���
};
struct tagVipConfig
{
	//Ĭ��ֵ
	BYTE DefaultLauncherReBoundNum;//�����ӵ���������
	BYTE DefaultAlmsSum;//���Ӿȼý�Ĵ���
	BYTE DefaultUseMedalSum;//Ĭ�϶һ�����
	HashMap<BYTE, tagVipOnce> VipMap;

	BYTE GetLauncherReBoundNum(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return DefaultLauncherReBoundNum;
		}
		return DefaultLauncherReBoundNum + Iter->second.LauncherReBoundNum;
	}
	BYTE GetUseMedalSum(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return DefaultUseMedalSum;
		}
		return DefaultUseMedalSum + Iter->second.AddUseMedalSum;
	}
	/*bool  IsCanLauncherLocking(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return false;
		}
		return Iter->second.IsCanLauncherLocking;
	}*/
	BYTE AddAlmsSum(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return DefaultAlmsSum;
		}
		return DefaultAlmsSum + Iter->second.AddAlmsSum;
	}
	float AddAlmsRate(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return 1.0f;
		}
		return (Iter->second.AddAlmsRate + 100) / 100.0f;
	}
	float AddMonthScoreRate(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return 1.0f;
		}
		return (Iter->second.AddMonthScoreRate + 100) / 100.0f;
	}
	float AddReChargeRate(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return 1.0f;
		}
		return (Iter->second.AddReChargeRate + 100) / 100.0f;
	}
	float AddCatchFishRate(BYTE VipLevel)
	{
		HashMap<BYTE, tagVipOnce>::iterator Iter = VipMap.find(VipLevel);
		if (Iter == VipMap.end())
		{
			return 1.0f;
		}
		return (Iter->second.AddCatchFishRate + 100) / 100.0f;
	}
};

struct tagMonthCardOnce
{
	BYTE	MonthCardID;
	DWORD	UseLastMin;
	WORD	OnceDayRewardID;
	bool	IsCanAutoFire;//�Ƿ�����Զ�����
	DWORD	AddLotteryRate;//�齱���������İٷֱ�
};
struct tagMonthCardConfig
{
	HashMap<BYTE, tagMonthCardOnce> MonthCardMap;
};

struct DropInfo
{
	WORD	RewardID;
	DWORD	RateValue;
};
struct tagFishDropOnce
{
	BYTE		FishTypeID;
	DWORD		TotalRateValue;
	vector<DropInfo> DroupVec;

	WORD GetFishDrop()
	{
		if (DroupVec.empty())
			return 0;
		DWORD RandValue = RandUInt() % TotalRateValue;
		vector<DropInfo>::iterator Iter = DroupVec.begin();
		//WORD UpperRewardID = 0;
		for (; Iter != DroupVec.end(); ++Iter)
		{
			if (RandValue < Iter->RateValue)
			{
				return Iter->RewardID;
			}
		}
		return 0;
	}
};
struct tagFishDropConfig
{
	HashMap<BYTE, tagFishDropOnce> FishDropMap;
};

//MiNiGame
//struct tagDialLevelInfo
//{
//	BYTE		ID;
//	DWORD		MinRate;
//	DWORD		MaxRate;
//};
struct RobotAnimalCountControl
{
	BYTE StartTime;
	BYTE EndTime;
	BYTE RobotCountMin;
	BYTE RobotCountMax;
};
struct tagDialRate
{
	BYTE			ID;
	DWORD			MaxRateSum;
	BYTE			RateArray[MAX_DIAL_ClientSum];
	BYTE			SumArray[MAX_DIAL_ClientSum];
};
struct tagDialConfig
{
	DWORD  BeginWriteSec;
	DWORD  EndWriteSec;
	DWORD  UpdateSec;
	DWORD  InitGlobelSum;
	DWORD  JoinGlobelSum;
	DWORD  MaxAddGlobel;
	BYTE   ChangeGlobelRate;//˥��ֵ
	DWORD  JoinBankerGlobelSum;
	DWORD  GetNextBankerNeedGlobel;
	DWORD  BankerGameSum;
	DWORD  VipNonBetSum;
	BYTE	MaxRate;
	vector<BYTE> DialAreaData;//�ȼ�����
	vector<tagDialRate> RateMap;

	vector<RobotAnimalCountControl>VecRobotCount;
	BYTE  RobotCountInterval;
	BYTE  RobotMinGameTime;
	BYTE  RobotMaxGameTime;
	BYTE  RobotActionRate;
	BYTE  RobotBankerRate;
	BYTE RobotWaitBankerMin;
	BYTE RobotWaitBankerMax;
	BYTE RobotBankerCountMin;
	BYTE RobotBankerCountMax;
	BYTE RobotListCountMin;
	BYTE RobotListCountMax;
	BYTE RobotListCountInterval;

	BYTE RobotMinBetTime;
	BYTE RobotMaxBetTime;

	DWORD RobotMinJetton;
	DWORD RobotMaxJetton;
	DWORD RobotAreaLimit;


	DWORD RobotBankerScoreMin;
	DWORD RobotBankerScoreMax;
	DWORD RobotPlayerScoreMin;
	DWORD RobotPlayerScoreMax;

	float GetChangeGlobelRateValue()
	{
		return (1000 - ChangeGlobelRate) / 1000.0f;
	}
};
struct tagCarValue
{
	BYTE		ID;
	BYTE		TypeID;
	//���������ļ����������ȫ������
	DWORD		MinRate;
	DWORD		MaxRate;//��ǰ��Ȩ������ 
};
struct tagCarType
{
	BYTE		TypeID;//����ID
	BYTE		RateValue;//���ͱ���
	DWORD		RateF;//�������Ȩ��
};

struct RobotCarCountControl
{
	BYTE StartTime;
	BYTE EndTime;
	BYTE RobotCountMin;
	BYTE RobotCountMax;
};

struct tagCarConfig
{
	DWORD  BeginWriteSec;
	DWORD  EndWriteSec;
	DWORD  UpdateSec;
	DWORD  InitGlobelSum;
	DWORD  JoinGlobelSum;
	DWORD  MaxAddGlobel;
	BYTE   ChangeGlobelRate;//˥��ֵ
	DWORD  JoinBankerGlobelSum;
	DWORD  GetNextBankerNeedGlobel;
	DWORD  BankerGameSum;
	DWORD  VipNonBetSum;
	DWORD  MaxRate;
	DWORD  MaxMultiple;

	vector<RobotCarCountControl>VecRobotCount;
	BYTE  RobotCountInterval;
	BYTE  RobotMinGameTime;
	BYTE  RobotMaxGameTime;
	BYTE  RobotActionRate;
	BYTE  RobotBankerRate;
	BYTE RobotWaitBankerMin;
	BYTE RobotWaitBankerMax;
	BYTE RobotBankerCountMin;
	BYTE RobotBankerCountMax;
	BYTE RobotListCountMin;
	BYTE RobotListCountMax;
	BYTE RobotListCountInterval;

	BYTE RobotMinBetTime;
	BYTE RobotMaxBetTime;

	DWORD RobotMinJetton;
	DWORD RobotMaxJetton;
	DWORD RobotAreaLimit;


	DWORD RobotBankerScoreMin;
	DWORD RobotBankerScoreMax;
	DWORD RobotPlayerScoreMin;
	DWORD RobotPlayerScoreMax;



	//������������  2��  ����  �� ���������
	HashMap<BYTE, tagCarType>   CarTypeMap;//���͵�����
	vector<tagCarValue>  CarValueVec;//��������

	float GetChangeGlobelRateValue()
	{
		return (1000 - ChangeGlobelRate) / 1000.0f;
	}

	BYTE GetRateValue(BYTE Index)
	{
		if (Index >= CarValueVec.size())
			return 0;
		HashMap<BYTE, tagCarType>::iterator Iter = CarTypeMap.find(CarValueVec[Index].TypeID);
		if (Iter == CarTypeMap.end())
			return 0;
		else
			return Iter->second.RateValue;
	}
};
struct RobotOxCountControl
{
	BYTE StartTime;
	BYTE EndTime;
	BYTE RobotCountMin;
	BYTE RobotCountMax;
};
struct tagNiuNiuConfig
{
	DWORD  BeginWriteSec;
	DWORD  EndWriteSec;
	DWORD  UpdateSec;
	DWORD  InitGlobelSum;
	DWORD  JoinGlobelSum;
	DWORD  MaxAddGlobel;
	BYTE   MaxRate;
	BYTE   ChangeGlobelRate;//˥��ֵ
	DWORD  JoinBankerGlobelSum;
	DWORD  GetNextBankerNeedGlobel;
	DWORD  BankerGameSum;
	DWORD  VipNonBetSum;
	HashMap<BYTE, BYTE> NiuNiuLevelRate;//�ȼ�����

	vector<RobotOxCountControl>VecRobotCount;
	BYTE  RobotCountInterval;
	BYTE  RobotMinGameTime;
	BYTE  RobotMaxGameTime;
	BYTE  RobotActionRate;
	BYTE  RobotBankerRate;
	BYTE RobotWaitBankerMin;
	BYTE RobotWaitBankerMax;
	BYTE RobotBankerCountMin;
	BYTE RobotBankerCountMax;
	BYTE RobotListCountMin;
	BYTE RobotListCountMax;
	BYTE RobotListCountInterval;

	BYTE RobotMinBetTime;
	BYTE RobotMaxBetTime;
	
	DWORD RobotMinJetton;
	DWORD RobotMaxJetton;
	DWORD RobotAreaLimit;
	

	DWORD RobotBankerScoreMin;
	DWORD RobotBankerScoreMax;
	DWORD RobotPlayerScoreMin;
	DWORD RobotPlayerScoreMax;


	

	float GetChangeGlobelRateValue()
	{
		return (1000 - ChangeGlobelRate) / 1000.0f;
	}

	BYTE GetRateByBrandValue(WORD Value)
	{
		BYTE Level = (BYTE)(Value >> 8);
		HashMap<BYTE, BYTE>::iterator Iter= NiuNiuLevelRate.find(Level);
		if (Iter != NiuNiuLevelRate.end())
			return Iter->second;
		else
			return 1;
	}
};
struct tagRoomConfig
{
	BYTE		bRoomID;
	BYTE		bRoomType;
	DWORD		dwJoinRoomGlobel;
	WORD		wInitTableSum;
	DWORD		dwTableMinGlobel;
	DWORD		dwOnceBetGlobel;
	BYTE		bOpenGameMinRoleSum;
	BYTE		bTableMaxRoleSum;
	WORD		wBeginWriteSec;
	WORD		wEndWriteSec;
};
struct tagNiuNiuRoomConfig : public tagRoomConfig
{
	BYTE		bRateID;
};
struct tagFiveNiuNiuRate
{
	BYTE				RateTypeID;
	BYTE				MaxRate;
	HashMap<BYTE, BYTE> NiuNiuLevelRate;//�ȼ�����
};
struct tagFiveNiuNiuConfig
{
	//ͨ��ţţ����
	HashMap<BYTE, tagNiuNiuRoomConfig>	NiuNiuRoomMap;//ţţ��������
	HashMap<BYTE, tagFiveNiuNiuRate>    NiuNiuRateMap;

	BYTE GetRateByBrandValue(BYTE bRoomID,WORD wValue)
	{
		BYTE bLevel = (BYTE)(wValue >> 8);
		HashMap<BYTE, tagNiuNiuRoomConfig>::iterator IterRoom = NiuNiuRoomMap.find(bRoomID);
		if (IterRoom == NiuNiuRoomMap.end())
			return 1;
		HashMap<BYTE, tagFiveNiuNiuRate>::iterator IterRate = NiuNiuRateMap.find(IterRoom->second.bRateID);
		if (IterRate == NiuNiuRateMap.end())
			return 1;
		HashMap<BYTE, BYTE>::iterator Iter = IterRate->second.NiuNiuLevelRate.find(bLevel);
		if (Iter == IterRate->second.NiuNiuLevelRate.end())
			return 1;
		return Iter->second;
	}
};
struct tagMiNiGameConfig
{
	tagNiuNiuConfig		niuniuConfig;
	tagDialConfig		dialConfig;
	tagCarConfig		carConfig;
	tagFiveNiuNiuConfig	fiveniuniuConfig;
};

struct tagRobotControlTime
{
	DWORD		OpenMinSec;
	DWORD		OpenMaxSec;
	DWORD		StopMinSec;
	DWORD		StopMaxSec;
};
struct tagRobotOpenFireInfo
{
	bool					IsOpen;
	tagRobotControlTime		FireInfo;

};
struct tagRobotLauncherType
{
	BYTE				LauncherType;
	DWORD				RateF;
};
struct tagRobotLauncherInfo
{
	bool					IsOpen;
	tagRobotControlTime    LauncherInfo;
	vector<tagRobotLauncherType> LauncherVec;
	DWORD						 MaxRateF;
};
struct tagRobotRateType
{
	BYTE		RateType;
	DWORD		RateF;
};
struct tagRobotRateInfo
{
	bool					IsOpen;
	tagRobotControlTime		RateInfo;
	vector<tagRobotRateType> RateVec;
	DWORD					 MaxRateF;
};
struct tagRobotSkillInfo
{
	bool					IsOpen;
	tagRobotControlTime   SkillInfo;
};
struct tagRobotRoomInfo
{
	bool					IsOpen;
	tagRobotControlTime	  RoomInfo;
};

struct tagRobotSendCharmInfo
{
	BYTE			CharmIndexID;
	WORD			MinItemSum;
	WORD			MaxItemSum;
	DWORD			RateF;
};
struct tagRobotSendCharm
{
	std::vector<tagRobotSendCharmInfo> vecSendCharmArray;
	DWORD				dwSendCharmArrayMaxRateF;
};
struct tagRobotBeSendCharm
{
	BYTE			CharmIndexID;
	BYTE			NonRateF;
	tagRobotSendCharm SendInfo;
};
struct tagRobotCharmChangeEvent
{
	DWORD			LeaveRoomRateF;
	BYTE			NonRateF;
	tagRobotSendCharm AddSendInfo;
	tagRobotSendCharm DelSendInfo;
};
struct tagRobotCharmInfo
{
	bool				IsOpen;
	DWORD				SendCharmMinCDSec;//��������������Ʒ��CD
	DWORD				SendCharmMaxCDSec;//��������������Ʒ��CD
	DWORD				BeChangeCharmValue;
	DWORD				BeChangeCharmSec;
	BYTE				NonRateF;
	tagRobotSendCharm	SendInfo;
	HashMap<BYTE, tagRobotBeSendCharm>	BeSendInfoMap;
	tagRobotCharmChangeEvent CharmChangeEvent;
};
struct tagGameRobotConfig
{
	DWORD				RobotID;
	DWORD				MonthRobotSum;
	BYTE				TableTypeID;
	BYTE				MonthID;
	DWORD				InitGlobelSum;
	DWORD				AddGlobelSum;
	DWORD				InitCurrceySum;
	DWORD				AddCurrceySum;
	DWORD				MinLevel;
	DWORD				MaxLevel;
	BYTE				GlobelRateF;
	BYTE				CurrceyRateF;
	tagRobotOpenFireInfo OpenFireInfo;
	tagRobotLauncherInfo LauncherInfo;
	tagRobotRateInfo     RateInfo;
	tagRobotSkillInfo	 SkillInfo;
	tagRobotRoomInfo     RoomInfo;
	tagRobotCharmInfo    CharmInfo;
};
struct tagGameRobotInfo
{
	HashMap<DWORD, tagGameRobotConfig>	RobotMap;
	multimap<DWORD, DWORD>				RobotIndexMap;
};

struct tagZPItemInfo
{
	WORD		wRewardID;
	DWORD		dwRateValue;
};
struct tagFishZPInfo
{
	DWORD		freeNum;
	DWORD		vipFreeNum;
	DWORD		useGoldNum;
	DWORD		dwMaxRate;
	vector<tagZPItemInfo> zpItemVec;
};

struct tagFishWhiteList
{
	vector<int64> uid;
};

struct subJJC_Time
{
	INT year;
	BYTE month;
	BYTE day;
	BYTE hour;
	BYTE minute;
};
struct tagJJC_Time
{
	INT remainTime;//����ʱ��
	INT maxTime; //�������ʱ��
	vector<subJJC_Time> openTime;
};

struct tagJJCTableInfo
{
	INT64 admission; //��������
	map<BYTE, INT64> reward;//����-����
};

struct tagFishJJC
{
	BYTE maxPlayerSum; //����������
	tagJJC_Time time;//����ʱ��
	map<BYTE, tagJJCTableInfo> jjcTable; //��������ID - ������Ϣ
};


//��������������� 
//���ݿ�����
enum FishDBCmd
{
	//LogOn
	DB_Logon_Init = 0xFF00,

	DBR_AccountLogon = DB_Logon_Init + 1,
	DBO_AccountLogon = DB_Logon_Init + 1,

	DBR_AccountRsg = DB_Logon_Init + 2,
	DBO_AccountRsg = DB_Logon_Init + 2,

	DBR_ResetAccount = DB_Logon_Init + 3,
	DBO_ResetAccount = DB_Logon_Init + 3,

	DBR_ChangeAccountPassword = DB_Logon_Init + 4,
	DBO_ChangeAccountPassword = DB_Logon_Init + 4,

	DBR_GetAccountInfoByUserID = DB_Logon_Init + 5,
	DBO_GetAccountInfoByUserID = DB_Logon_Init + 5,

	DBR_SetOnline = DB_Logon_Init + 6,
	DBO_SetOnline = DB_Logon_Init + 6,

	//DBR_LoadAllInfo = DB_Logon_Init + 7,

	DBR_SetRoleClientInfo = DB_Logon_Init + 7,

	DBR_GetNewAccount = DB_Logon_Init + 8,
	DBO_GetNewAccount = DB_Logon_Init + 8,

	DBR_QueryLogon = DB_Logon_Init+9,
	DBO_QueryLogon = DB_Logon_Init+9,

	DBR_SaveChannelInfo = DB_Logon_Init +10,
	
	DBR_SetAccountFreeze = DB_Logon_Init + 11,

	DBR_PhoneLogon = DB_Logon_Init + 12,
	DBO_PhoneLogon = DB_Logon_Init + 12,

	DBR_PhoneSecPwd = DB_Logon_Init + 13,
	DBO_PhoneSecPwd = DB_Logon_Init + 13,

	//Item
	DB_Item_Init,

	DBR_LoadUserItem = DB_Item_Init + 1,
	DBO_LoadUserItem = DB_Item_Init + 1,

	//DBO_LoadUserItemFinish = DB_Item_Init + 2,

	DBR_AddUserItem = DB_Item_Init + 3,
	DBO_AddUserItem = DB_Item_Init + 3,

	DBR_DelUserItem = DB_Item_Init + 4,

	DBR_ChangeUserItem = DB_Item_Init + 5,
	//Relation
	DB_Relation_Init,

	DBR_LoadUserRelation = DB_Relation_Init + 1,
	DBO_LoadUserRelation = DB_Relation_Init + 1,

	DBO_LoadBeUserRelation = DB_Relation_Init + 2,

	//DBO_LoadBeUserRelationFinish = DB_Relation_Init + 3,

	DBR_AddUserRelation = DB_Relation_Init + 4,
	DBO_AddUserRelation = DB_Relation_Init + 4,

	DBR_DelUserRelation = DB_Relation_Init + 5,
	//Mail
	DB_Main_Init,
	DBR_LoadUserMail = DB_Main_Init + 1,
	DBO_LoadUserMail = DB_Main_Init + 1,

	//DBO_LoadUserMailFinish = DB_Main_Init + 2,

	DBR_AddUserMail = DB_Main_Init + 3,
	DBO_AddUserMail = DB_Main_Init + 3,

	DBR_DelUserMail = DB_Main_Init + 4,

	DBR_GetUserMailItem = DB_Main_Init + 5,

	DBR_ChangeUserMailIsRead = DB_Main_Init + 6,

	//Role
	DB_Role_Init,

	DBR_SaveRoleExInfo = DB_Role_Init + 1,
	DBR_SaveRoleNickName = DB_Role_Init + 2,
	DBO_SaveRoleNickName = DB_Role_Init + 2,
	DBR_SaveRoleLevel = DB_Role_Init + 3,
	DBR_SaveRoleGender = DB_Role_Init + 4,
	DBR_SaveRoleAchievementPoint = DB_Role_Init + 5,
	DBR_SaveRoleCurTitle = DB_Role_Init + 6,
	DBR_SaveRoleCharmArray = DB_Role_Init + 7,
	DBR_SaveRoleCurrency = DB_Role_Init + 8,
	DBR_SaveRoleFaceID = DB_Role_Init + 9,
	DBR_SaveRoleMedal = DB_Role_Init + 10,
	DBR_SaveRoleSendGiffSum = DB_Role_Init + 11,
	DBR_SaveRoleAcceptGiffSum = DB_Role_Init + 12,
	DBR_SaveRoleTaskStates = DB_Role_Init + 13,
	DBR_SaveRoleAchievementStates = DB_Role_Init + 14,
	DBR_SaveRoleActionStates = DB_Role_Init + 15,
	DBR_SaveRoleOnlineStates = DB_Role_Init + 16,
	DBR_SaveRoleCheckData = DB_Role_Init + 17,
	DBR_SaveRoleIsShowIpAddress = DB_Role_Init + 18,


	DBR_SaveRoleAllInfo = DB_Role_Init + 19,
	DBO_SaveRoleAllInfo = DB_Role_Init + 20,

	DBR_SaveRoleGlobel = DB_Role_Init + 21,
	DBR_SaveRoleOnlineMin = DB_Role_Init + 22,

	DBR_SaveRoleExChangeStates = DB_Role_Init + 23,
	DBR_SaveRoleTotalRecharge = DB_Role_Init + 24,
	DBR_SaveRoleIsFirstPayGlobel = DB_Role_Init + 25,
	DBR_SaveRoleIsFirstPayCurrcey = DB_Role_Init + 26,

	DBR_ChangeRoleMoney = DB_Role_Init + 27,

	DBR_TableChange = DB_Role_Init + 28,

	DBR_RoleEntityItem = DB_Role_Init + 29,
	DBO_RoleEntityItem = DB_Role_Init + 29,

	DBR_DelRoleEntityItem = DB_Role_Init + 30,

	DBR_SaveRoleMonthCardInfo = DB_Role_Init + 31,
	DBR_SaveRoleRateValue = DB_Role_Init + 32,
	DBR_SaveRoleGetMonthCardRewardTime = DB_Role_Init + 33,
	DBR_SaveRoleVipLevel = DB_Role_Init + 34,
	DBR_SaveRoleCashSum = DB_Role_Init + 35,
	DBR_DelRoleCashSum = DB_Role_Init + 36,

	DBR_AddRoleTotalRecharge = DB_Role_Init + 37,

	DBR_SaveRoleMaxRateValue = DB_Role_Init + 38,

	//DBR_SaveRoleLockInfo        = DB_Role_Init + 38,

	DBR_SaveRoleParticularStates = DB_Role_Init + 39,

	DBR_SaveRoleSecPassword = DB_Role_Init + 40,

	DBR_ChangeRoleSecPassword = DB_Role_Init + 41,
	DBO_ChangeRoleSecPassword = DB_Role_Init + 41,

	DBR_ChangeRoleShareStates = DB_Role_Init + 42,

	DBR_GameIDConvertToUserID = DB_Role_Init + 43,
	DBO_GameIDConvertToUserID = DB_Role_Init + 43,

	DBR_ChangeRoleZPNum = DB_Role_Init + 44,

	DBR_ChangeRoleIsShop = DB_Role_Init + 45,

	DBR_ChangeRoleExchangeNum = DB_Role_Init + 46,
	//Query
	DB_Query_Init,

	DBR_Query_RoleInfoByNickName = DB_Query_Init+1,
	DBO_Query_RoleInfo = DB_Query_Init+1,

	DBR_Query_RoleInfoByUserID = DB_Query_Init+2,
	DBO_Query_RoleInfoByUserID = DB_Query_Init+2,

	DBR_Query_RoleInfoByGameID = DB_Query_Init + 3,
	DBO_Query_RoleInfoByGameID = DB_Query_Init + 3,

	//TASK
	DB_Task_Init,

	DBR_LoadRoleTask = DB_Task_Init+1,
	DBO_LoadRoleTask = DB_Task_Init + 1,

	DBR_SaveRoleTask = DB_Task_Init + 2,//������ҵ�һ������

	DBR_DelRoleTask = DB_Task_Init + 3,

	DBR_ClearRoleTask = DB_Task_Init + 4,//���һ�����ȫ��������

	DBR_SaveAllTask = DB_Task_Init+5,

	//�ɾ�
	DB_Achievement_Init,
	DBR_LoadRoleAchievement = DB_Achievement_Init + 1,
	DBO_LoadRoleAchievement = DB_Achievement_Init + 1,

	DBR_SaveRoleAchievement = DB_Achievement_Init + 2,//������ҵ�һ������

	DBR_DelRoleAchievement = DB_Achievement_Init + 3,

	DBR_SaveAllAchievement = DB_Achievement_Init + 4,
	//�ƺ�
	DB_Title_Init,
	DBR_LoadRoleTitle = DB_Title_Init+1,
	DBO_LoadRoleTitle = DB_Title_Init + 1,

	DBR_AddRoleTitle = DB_Title_Init + 2,

	DBR_DelRoleTitle = DB_Title_Init + 3,

	//���а�
	DB_Rank_Init,

	DBR_LoadRankInfo = DB_Rank_Init+1,
	DBO_LoadRankInfo = DB_Rank_Init + 1,

	DBR_LoadWeekRankInfo = DB_Rank_Init + 2,
	DBO_LoadWeekRankInfo = DB_Rank_Init + 2,

	DBR_SaveWeekRankInfo = DB_Rank_Init + 3,

	DBR_ChangeWeekInfo = DB_Rank_Init +4,
	//���� Ŀǰ����ʹ�� 
	DB_Charm_Init,
	DBR_SaveRoleCharm = DB_Charm_Init+1,
	DBO_SaveRoleCharm = DB_Charm_Init+1,

	//�̵�
	DB_Shop_Init,
	DBR_AddRoleEntityItem = DB_Shop_Init+1,

	//��ʵ��ַ
	DB_Enetity_Init,
	DBR_LoadRoleEntity = DB_Enetity_Init+1,
	DBO_LoadRoleEntity = DB_Enetity_Init + 1,

	DBR_SaveRoleEntityName = DB_Enetity_Init + 2,
	DBR_SaveRoleEntityPhone = DB_Enetity_Init + 3,
	DBR_SaveRoleEntityEmail = DB_Enetity_Init + 4,
	DBR_SaveRoleEntityIdentityID = DB_Enetity_Init + 5,
	DBR_SaveRoleEntityItemUseName = DB_Enetity_Init + 6,
	DBR_SaveRoleEntityItemUsePhone = DB_Enetity_Init + 7,
	DBR_SaveRoleEntityItemUseAddress = DB_Enetity_Init + 8,
	DBR_SaveRoleEntityAlipayInfo	= DB_Enetity_Init + 9,

	//DBR_SaveRoleEntity = 94,
	//�
	DB_Acion_Init,
	DBR_LoadRoleAction = DB_Acion_Init+1,
	DBO_LoadRoleAction = DB_Acion_Init + 1,

	DBR_SaveRoleAction = DB_Acion_Init + 2,

	DBR_DelRoleAction = DB_Acion_Init + 3,

	DBR_SaveAllAction = DB_Acion_Init +4,

	//����
	DB_Giff_Init,
	DBR_LoadRoleGiff = DB_Giff_Init+1,
	DBO_LoadRoleGiff = DB_Giff_Init + 1,

	DBR_AddRoleGiff = DB_Giff_Init + 2,
	DBO_AddRoleGiff = DB_Giff_Init + 2,

	DBR_DelRoleGiff = DB_Giff_Init + 3,

	DBR_GetNowDayGiff = DB_Giff_Init+4,
	DBO_GetNowDayGiff = DB_Giff_Init + 4,//��ȡ�������������

	DBR_ClearNowDayGiff = DB_Giff_Init+ 5,
	//GameData
	DB_GameData_Init,
	DBR_LoadGameData = DB_GameData_Init+1,
	DBO_LoadGameData = DB_GameData_Init + 1,

	DBR_SaveGameData = DB_GameData_Init + 2,

	DBR_ChangeGameDataByMonth = DB_GameData_Init + 3,

	DBR_LoadAllUserAchievementPointList = DB_GameData_Init + 4,//��ȡȫ���ĳɾ͵�����
	DBO_LoadAllUserAchievementPointList = DB_GameData_Init + 4,//���������������


	DBR_LoadAllAccountInfo = DB_GameData_Init + 5,
	DBO_LoadAllAcoountInfo = DB_GameData_Init + 5,

	//Announcement
	DB_Announcement_Init,
	DBR_LoadAllAnnouncement = DB_Announcement_Init+1,
	DBO_LoadALlAnnouncement = DB_Announcement_Init + 1,
	//DBO_LoadAllAnnouncementFinish = DB_Announcement_Init + 2,

	DBR_AddAnnouncement = DB_Announcement_Init + 3,

	//Operate
	DB_Operate_Init,
	DBR_SaveRoleEmail = DB_Operate_Init+1,

	//Center
	DB_Center_Init,
	DBR_ClearFishDB = DB_Center_Init+1,
	DBO_ClearFishDB = DB_Center_Init+1,

	//ExChange
	DBR_ExChange_Init,

	DBR_QueryExChange = DBR_ExChange_Init + 1,
	DBO_QueryExChange = DBR_ExChange_Init + 1,
	DBR_DelExChange   = DBR_ExChange_Init + 2,

	//Recharge
	DBR_Recharge_Init,
	DBR_AddRecharge = DBR_Recharge_Init +1,

	DBR_LoadRechageOrderID = DBR_Recharge_Init +2,
	DBO_LoadRechageOrderID = DBR_Recharge_Init +2,

	DBR_AddRechageOrderID = DBR_Recharge_Init+3,

	DBR_CheckEntityID = DBR_Recharge_Init+4,
	DBO_CheckEntityID = DBR_Recharge_Init+4,

	DBR_CheckPhone = DBR_Recharge_Init + 5,
	DBO_CheckPhone = DBR_Recharge_Init + 5,

	DBR_LoadPhonePayOrderID = DBR_Recharge_Init+6,
	DBO_LoadPhonePayOrderID = DBR_Recharge_Init+6,
	DBR_AddPhonePayOrderID = DBR_Recharge_Init+7,

	DBR_GetRechargeOrderID = DBR_Recharge_Init+8,
	DBO_GetRechargeOrderID = DBR_Recharge_Init+8,

	DBR_QueryRechargeOrderInfo = DBR_Recharge_Init + 9,
	DBO_QueryRechargeOrderInfo = DBR_Recharge_Init + 9,

	DBR_DelRechargeOrderInfo = DBR_Recharge_Init + 10,

	//Control
	DBR_Control_init,
	DBR_ResetUserPassword	= DBR_Control_init +1,
	DBR_FreezeUser			= DBR_Control_init+2,

	//Robot
	DBR_Robot_Init,
	DBR_LoadRobotInfo = DBR_Robot_Init + 1,
	DBO_LoadRobotInfo = DBR_Robot_Init + 1,

	//Char
	DBR_Char_Init,
	DBR_LoadCharInfo = DBR_Char_Init + 1,
	DBO_LoadCharInfo = DBR_Char_Init + 1,
	DBR_DelCharInfo = DBR_Char_Init + 2,
	DBR_AddCharInfo = DBR_Char_Init+3,

	//Relationrequest
	DBR_RelationRequest_Init,

	DBR_LoadRelationRequest = DBR_RelationRequest_Init + 1,
	DBO_LoadRelationRequest = DBR_RelationRequest_Init + 1,

	DBR_AddRelationRequest = DBR_RelationRequest_Init + 2,
	DBO_AddRelationRequest = DBR_RelationRequest_Init + 2,

	DBR_DelRelationRequest = DBR_RelationRequest_Init + 3,

	//Cash
	DBR_Cash_Init,
	DBR_LoadCash = DBR_Cash_Init +1,
	DBO_LoadCash = DBR_Cash_Init + 1,

	DBR_HandleCash = DBR_Cash_Init + 2,

	DBR_AddCash = DBR_Cash_Init +3,
	DBO_AddCash = DBR_Cash_Init + 3,

	//NiuNiu
	DBR_NiuNiu_Init,
	DBR_RequestJoinNiuNiuRoom = DBR_NiuNiu_Init+1,
	DBO_RequestJoinNiuNiuRoom = DBR_NiuNiu_Init + 1,

	//Cust
	DBR_Cust_Init,
	DBR_HandleCust = DBR_Cust_Init +1,
	DBO_HandleCust = DBR_Cust_Init + 1,

	//����ͳ��
	DBR_Save_battle_Record,
	DBR_Save_System_Minute_Record,
	DBR_Save_Total_Gold,
	DBR_GameserverStart,
	DBO_GameserverStart,
};

struct msg_ArenaStartInfo :public NetCmd
{
	bool isStart;
	INT waitPlayers;
};
struct tagArenaRewardRank
{
	INT rank;
	char nickname[MAX_NICKNAME + 1];
	INT64 nScore;
	INT64 uid;
};
struct msg_ArenaReward :public NetCmd
{
	tagArenaRewardRank reward[0];
};

struct Reg_Server : public NetCmd 
{
	BYTE NetWorkID;
};

struct DBR_Cmd_AccountLogon : public NetCmd
{
	//��ҽ��е�½����
	DWORD ClientID;
	TCHAR	AccountName[ACCOUNT_LENGTH+1];
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct DBO_Cmd_AccountLogon : public NetCmd
{
	DWORD ClientID;
	DWORD	dwUserID;
	bool	IsFreeze;
	__int64 FreezeEndTime;
};
struct DBR_Cmd_AccountRsg : public NetCmd
{
	DWORD	ClientID;
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	//ע������
	DWORD	ClientIP;
};
struct DBO_Cmd_AccountRsg : public NetCmd
{
	DWORD ClientID;
	DWORD	dwUserID;
};

struct DBR_Cmd_PhoneSecPwd : public NetCmd
{
	DWORD	ClientID;
	UINT64	PhoneNumber;
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct DBO_Cmd_PhoneSecPwd : public NetCmd
{
	DWORD	ClientID;
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	dwUserID;
	bool	IsFreeze;
	__int64 FreezeEndTime;
};
struct DBR_Cmd_ResetAccount : public NetCmd
{
	DWORD	dwUserID;
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct DBO_Cmd_ResetAccount : public NetCmd
{
	DWORD	dwUserID;
	bool	Result;
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct DBR_Cmd_ChangeAccountPassword : public NetCmd
{
	DWORD	dwUserID;
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	DWORD	OldPasswordCrc1;
	DWORD	OldPasswordCrc2;
	DWORD	OldPasswordCrc3;
};
struct DBO_Cmd_ChangeAccountPassword : public NetCmd
{
	DWORD	dwUserID;
	bool	Result;
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};


struct tagPlayerGameInfo
{
	double money1;
	double money2;
	char name[MAX_NICKNAME + 1];
	char icon[ICON_LENGTH];
	char signature[MAX_SIGNATURE + 1]; //����ǩ��
	INT sex;
	INT vip; //vip
	INT headIcon; //ͷ���
};

struct DBR_Cmd_GetAccountInfoByUserID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	ClientID;
	bool	LogonByGameServer;
	INT64	Uid;
	tagPlayerGameInfo udata;
};

struct DBR_Cmd_GameserverStart:public NetCmd
{
	int game_id;
};

struct tagTableGold
{
	BYTE table_id;
	int64 total_earn;
	int64 total_produce;
};
struct DBO_Cmd_GameserverStart :public NetCmd
{
	tagTableGold data[0];
};
struct DBO_Cmd_GetAccountInfoByUserID : public NetCmd
{
	DWORD		ClientID;
	bool		Result;
	bool		IsFreeze;
	__int64     FreezeEndTime;
	tagRoleInfo RoleInfo;
	tagRoleServerInfo RoleServerInfo;
	__int64		LastOnlineTime;//����¼��ʱ��
	bool		LogonByGameServer;
	bool		IsRobot;
};
struct DBR_Cmd_RoleOnline : public NetCmd
{
	DWORD			dwUserID;
	bool			IsOnline;
};
struct DBO_Cmd_RoleOnline : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	bool			IsOnline;
};
struct DBR_Cmd_GetNewAccount : public NetCmd
{
	DWORD			ClientID;
	TCHAR			MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
	DWORD			ClientIP;
};
struct DBO_Cmd_GetNewAccount : public NetCmd
{
	bool			Result;
	DWORD			ClientID;
	DWORD			dwUserID;
	TCHAR			AccountName[ACCOUNT_LENGTH + 1];
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
};
struct DBR_Cmd_QueryLogon : public NetCmd
{
	DWORD			ClientID;
	TCHAR			AccountName[ACCOUNT_LENGTH + 1];
	TCHAR			MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
	DWORD			ClientIP;
	BYTE			LogonType;
};
struct DBO_Cmd_QueryLogon : public NetCmd
{
	DWORD			ClientID;
	DWORD			dwUserID;
	bool			IsFreeze;
	__int64			FreezeEndTime;
	bool			IsNewRsg;
	BYTE			LogonType;
};
struct DBR_Cmd_SaveChannelInfo : public NetCmd
{
	DWORD			dwUserID;
	ChannelUserInfo pInfo;
};
struct DBR_Cmd_SetAccountFreeze : public NetCmd
{
	DWORD			dwUserID;
	DWORD			FreezeMin;
};

struct DBR_Cmd_PhoneLogon : public NetCmd
{
	UINT64			PhoneNumber;
	DWORD			ClientID;
	TCHAR			MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD			ClientIP;
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
	bool			IsFirOrSec;
};
struct DBO_Cmd_PhoneLogon : public NetCmd
{
	DWORD			ClientID;
	DWORD			dwUserID;
	//�����ɵ��˺� ��������ҵ�½
	TCHAR			AccountName[ACCOUNT_LENGTH + 1];
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
	bool			IsFreeze;
	__int64			FreezeEndTime;
};
//struct DBR_Cmd_LoadAllInfo : public NetCmd
//{
//	DWORD			dwUserID;
//};
//struct DBR_Cmd_GetRoleAchievementIndex : public NetCmd
//{
//	DWORD			dwUserID;
//};
//struct DBO_Cmd_GetRoleAchievementIndex : public NetCmd
//{
//	DWORD			dwUserID;
//	DWORD			AchievementIndex;
//};
struct DBR_Cmd_SetRoleClientInfo : public NetCmd
{
	DWORD			dwUserID;
	BYTE			PlateFormID;
	DWORD			ScreenPoint;
};

//Item
struct DBR_Cmd_LoadUserItem : public NetCmd
{
	DWORD			dwUserID;
};
struct DBO_Cmd_LoadUserItem : public NetCmd
{
	DWORD			dwUserID; 
	BYTE			States;
	WORD			Sum;
	tagItemInfo		Array[1];
};
//struct DBO_Cmd_LoadUserItemFinish : public NetCmd
//{
//	DWORD			dwUserID;
//};
struct DBR_Cmd_AddUserItem : public NetCmd
{
	DWORD			dwUserID;
	tagItemOnce		ItemInfo;
};
struct DBO_Cmd_AddUserItem : public NetCmd
{
	DWORD			dwUserID;
	tagItemInfo		ItemInfo;
};
struct DBR_Cmd_DelUserItem : public NetCmd
{
	DWORD			ItemOnlyID;
};
struct DBR_Cmd_ChangeUserItem : public NetCmd
{
	DWORD			ItemOnlyID;
	DWORD			ItemSum;
};
//Relation
struct DBR_Cmd_LoadUserRelation : public NetCmd
{
	DWORD			dwUserID;
};
struct DBO_Cmd_LoadUserRelation : public NetCmd
{
	DWORD			dwUserID;
	BYTE			States;
	WORD			Sum;
	tagRoleRelation Array[1];
};
struct DBO_Cmd_LoadBeUserRelation : public NetCmd
{
	DWORD				dwUserID;
	BYTE				States;
	WORD				Sum;
	tagBeRoleRelation	Array[1];
};
//struct DBO_Cmd_LoadBeUserRelationFinish : public NetCmd
//{
//	DWORD			dwUserID;
//};

struct DBR_Cmd_AddUserRelation : public NetCmd
{
	DWORD			dwSrcUserID;
	DWORD			dwDestUserID;
	BYTE			bRelationType;
};
struct DBO_Cmd_AddUserRelation : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	tagRoleRelation	RelationInfo;
};
struct DBR_Cmd_DelUserRelation : public NetCmd
{
	DWORD			dwSrcUserID;
	DWORD			dwDestUserID;
};
//struct DBR_Cmd_ChangeUserRelation : public NetCmd
//{
//	DWORD			dwSrcUserID;
//	DWORD			dwDestUserID;
//	BYTE			bRelationType;
//};
//Mail
struct DBR_Cmd_LoadUserMail : public NetCmd
{
	DWORD			dwUserID;
};
struct DBO_Cmd_LoadUserMail : public NetCmd
{
	DWORD			dwUserID;
	BYTE			States;
	WORD			Sum;
	tagRoleMail     Array[1];
};
//struct DBO_Cmd_LoadUserMailFinish : public NetCmd
//{
//	DWORD			dwUserID;
//};
struct DBR_Cmd_AddUserMail : public NetCmd
{
	DWORD			dwDestUserID;//Ŀ����ҵ�ID
	tagRoleMail		MailInfo;//����ӵ��ʼ�
};
struct DBO_Cmd_AddUserMail : public NetCmd
{
	DWORD			dwDestUserID;
	bool			Result;
	tagRoleMail		MailInfo;
};
struct DBR_Cmd_DelUserMail : public NetCmd
{
	DWORD			dwMailID;
};
struct DBR_Cmd_GetUserMailItem : public NetCmd
{
	DWORD			dwMailID;
	//tagItemOnce		ItemArray[MAX_MAIL_ITEM];//��Ʒ������ ���ĺ����Ʒ������
};
struct DBR_Cmd_ChangeUserMailIsRead : public NetCmd
{
	DWORD			dwMailID;
	bool			bIsRead;
};
//Role
struct DBR_Cmd_SaveRoleExInfo : public NetCmd
{
	int			 RoleCharmValue;
	tagRoleInfo  RoleInfo;
};
//struct DBR_Cmd_SaveRoleQueryAtt : public NetCmd
//{
//	//������Ҫ�����������ݿ�����Խ��б���
//	DWORD			dwUserID;
//	TCHAR			NickName[MAX_NICKNAME + 1];
//	WORD			wLevel;//��ҵ��
//	DWORD			dwFaceID;//���ͷ��ID
//	bool			bGender; //����Ա�
//	DWORD			dwAchievementPoint;//��ҵĳɾ͵���
//	BYTE			TitleID;
//	bool			IsShowIpAddress;
//};

struct DBR_Cmd_SaveRoleNickName : public NetCmd
{
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
};
struct DBO_Cmd_SaveRoleNickName : public NetCmd
{
	bool			Result;
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
};
struct DBR_Cmd_SaveRoleLevel : public NetCmd
{
	DWORD			dwUserID;
	WORD			wLevel;
};
struct DBR_Cmd_SaveRoleGender : public NetCmd
{
	DWORD			dwUserID;
	bool			bGender;
};
struct DBR_Cmd_SaveRoleAchievementPoint : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwAchievementPoint;
};
struct DBR_Cmd_SaveRoleCurTitle : public NetCmd
{
	DWORD			dwUserID;
	BYTE			TitleID;
};
struct DBR_Cmd_SaveRoleCharmArray : public NetCmd
{
	DWORD			dwUserID;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
};
struct DBR_Cmd_SaveRoleCurrency : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwCurrencyNum;
};
struct DBR_Cmd_SaveRoleFaceID : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwFaceID;
};
struct DBR_Cmd_SaveRoleMedal : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwMedalSum;
	DWORD			TotalUseMedal;
};
struct DBR_Cmd_SaveRoleSendGiffSum : public NetCmd
{
	DWORD			dwUserID;
	BYTE			SendSum;
};
struct DBR_Cmd_SaveRoleAcceptGiffSum : public NetCmd
{
	DWORD			dwUserID;
	BYTE			AcceptSum;
};
struct DBR_Cmd_SaveRoleTaskStates : public NetCmd
{
	DWORD			dwUserID;
	int256			States;
};
struct DBR_Cmd_SaveRoleAchievementStates : public NetCmd
{
	DWORD			dwUserID;
	int256			States;
};
struct DBR_Cmd_SaveRoleActionStates : public NetCmd
{
	DWORD			dwUserID;
	int256			States;
};
struct DBR_Cmd_SaveRoleOnlineStates : public NetCmd
{
	DWORD			dwUserID;
	DWORD			OnlineStates;
};
struct DBR_Cmd_SaveRoleCheckData : public NetCmd
{
	DWORD			dwUserID;
	DWORD			CheckData;
};
struct DBR_Cmd_SaveRoleIsShowIpAddress : public NetCmd
{
	DWORD			dwUserID;
	bool			IsShowIP;
};
struct DBR_Cmd_SaveRoleExChangeStates : public NetCmd
{
	DWORD			dwUserID;
	DWORD			States;
};
struct DBR_Cmd_SaveRoleTotalRecharge : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Sum;
};
struct DBR_Cmd_AddRoleTotalRecharge : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Sum;
};
struct DBR_Cmd_SaveRoleMaxRateValue : public NetCmd
{
	DWORD			dwUserID;
	BYTE			MaxRate;
};
struct DBR_Cmd_SaveRoleParticularStates : public NetCmd
{
	DWORD			dwUserID;
	DWORD			ParticularStates;
};
struct DBR_Cmd_SaveRoleSecPassword : public NetCmd
{
	DWORD			dwUserID;
	DWORD			SecPasswordCrc1;
	DWORD			SecPasswordCrc2;
	DWORD			SecPasswordCrc3;
};
struct DBR_Cmd_ChangeRoleSecPassword : public NetCmd
{
	DWORD			dwUserID;

	DWORD			dwOldSecPasswordCrc1;
	DWORD			dwOldSecPasswordCrc2;
	DWORD			dwOldSecPasswordCrc3;

	DWORD			dwNewSecPasswordCrc1;
	DWORD			dwNewSecPasswordCrc2;
	DWORD			dwNewSecPasswordCrc3;
};
struct DBO_Cmd_ChangeRoleSecPassword : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	DWORD			dwNewSecPasswordCrc1;
	DWORD			dwNewSecPasswordCrc2;
	DWORD			dwNewSecPasswordCrc3;
};
/*struct DBR_Cmd_SaveRoleLockInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			LockStates;
	time_t			LockEndTime;
};*/
struct DBR_Cmd_SaveRoleIsFirstPayGlobel : public NetCmd
{
	DWORD			dwUserID;
};
struct DBR_Cmd_SaveRoleIsFirstPayCurrcey : public NetCmd
{
	DWORD			dwUserID;
};
struct DBR_Cmd_ChangeRoleMoney : public NetCmd
{
	DWORD			dwUserID;
	int				GlobelSum;
	int				MedalSum;
	int				CurrceySum;
};
struct DBR_Cmd_TableChange : public NetCmd
{
	DWORD			dwUserID;
	BYTE			TableTypeID;
	BYTE			TableMonthID;
	int64				GlobelSum;
	int				MedalSum;
	int				CurrceySum;
	time_t			LogTime;
	bool			JoinOrLeave;
};
struct DBR_Cmd_RoleEntityItem : public NetCmd
{
	DWORD			ClientID;
	DWORD			ClientIP;
	DWORD			ID;
	char			OrderNumber[32];//��ݵ���
};
struct DBO_Cmd_RoleEntityItem : public NetCmd
{
	DWORD			ClientID;
	DWORD			ClientIP;
	char			OrderNumber[32];//��ݵ���
	bool			Result;
	//����������
	DWORD			ID;
	DWORD			ItemID;
	DWORD			ItemSum;
	DWORD			MedalNum;
	DWORD			NowMedalNum;
	DWORD			UserID;
	UINT64			Phone;
	time_t			ShopTime;
	TCHAR			Address[MAX_ADDRESS_LENGTH + 1];
	TCHAR			Name[MAX_NAME_LENTH + 1];
	TCHAR			IDEntity[MAX_IDENTITY_LENGTH + 1];
};
struct DBR_Cmd_DelRoleEntityItem : public NetCmd
{
	DWORD			ID;
};
struct DBR_Cmd_SaveRoleMonthCardInfo : public NetCmd
{
	DWORD			UserID;
	BYTE			MonthCardID;
	__int64			MonthCardEndTime;
};
struct DBR_Cmd_SaveRoleRateValue : public NetCmd
{
	DWORD			UserID;
	int256			RateValue;//��ұ�������
	DWORD			MaxRateValue;
};
struct DBR_Cmd_SaveRoleGetMonthCardRewardTime : public NetCmd
{
	DWORD			UserID;
	__int64			LogTime;
};
struct DBR_Cmd_SaveRoleVipLevel : public NetCmd
{
	DWORD			UserID;
	BYTE			VipLevel;
};
struct DBR_Cmd_SaveRoleCashSum : public NetCmd
{
	DWORD			UserID;
	BYTE			CashSum;
	DWORD			TotalCashSum;
};
struct DBR_Cmd_ChangeRoleShareStates : public NetCmd
{
	DWORD			dwUserID;
	bool			States;
};

struct DBR_Cmd_GameIDConvertToUserID : public NetCmd
{
	DWORD			SrcUserID;
	DWORD			dwGameID;
	BYTE			NetCmd;
};
struct DBO_Cmd_GameIDConvertToUserID : public NetCmd
{
	DWORD			SrcUserID;
	DWORD			dwGameID;
	DWORD			dwUserID;
	BYTE			NetCmd;
};
struct DBR_Cmd_ChangeRoleZPNum : public NetCmd
{
	DWORD			dwUserID;
	DWORD			zpNum;
};
struct DBR_Cmd_ChangeRoleIsShop : public NetCmd
{
	DWORD			dwUserID;
	bool			bShop;
};
struct DBR_Cmd_ChangeRoleExchangeNum : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwExchangeNum;
};

struct DBR_Cmd_DelRoleCashSum : public NetCmd
{
	DWORD			UserID;
	BYTE			DelCashSum;
};
union UnionEventInfo
{
	tagRoleTaskInfo	TaskInfo;
	tagRoleAchievementInfo AchievementInfo;
	tagRoleActionInfo ActionInfo;
};
struct SaveEventInfo
{
	BYTE			InfoStates;//1 ����  2 �ɾ� 3 �
	UnionEventInfo	EventInfo;
};
struct DBR_Cmd_SaveRoleAllInfo : public NetCmd//�����Ҫ�����ȫ��������
{
	DWORD			dwUserID;
	bool			IsNeedResult;//�Ƿ���Ҫ��������
	//���ݴ���������� 
	//1.RoleEx �е�
	DWORD			dwExp;//��Ҿ���
	DWORD			dwGlobeNum;
	DWORD			dwProduction;//�����õĽ������
	DWORD			dwGameTime;//�������Ϸʱ��
	WORD			OnlineMin;
	DWORD			ClientIP;//���IP
	DWORD			dwLotteryScore;
	BYTE			bLotteryFishSum;
	//int64			TotalFishGlobelSum;
	tagRoleServerInfo RoleServerInfo;
	//2.GameData
	tagRoleGameData GameData;
	//3.
	BYTE			States;
	WORD			Sum;
	SaveEventInfo   Array[1];
};
struct DBO_Cmd_SaveRoleAllInfo : public NetCmd
{
	DWORD			dwUserID;//����ڱ�����Ϻ�Ļط�����
};
struct DBR_Cmd_SaveRoleGlobel : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwGlobel;
};
struct DBR_Cmd_SaveRoleOnlineMin : public NetCmd
{
	DWORD			dwUserID;
	WORD			OnLineMin;
};
//Query
struct DBR_Cmd_QueryRoleInfoByNickName : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		QueryNickName[MAX_NICKNAME + 1];
	BYTE		Page;
	bool		IsOnline;
};
struct DBO_Cmd_QueryRoleInfo : public NetCmd
{
	DWORD		dwUserID;
	WORD		Page;
	BYTE		States;
	WORD		Sum;
	tagQueryRoleInfo Array[1];
};
struct DBR_Cmd_Query_RoleInfoByUserID : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwDestUserID;
};
struct DBO_Cmd_Query_RoleInfoByUserID : public NetCmd
{
	DWORD		dwUserID;
	tagQueryRoleInfo RoleInfo;
};

struct DBR_Cmd_Query_RoleInfoByGameID : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwDestGameID;
};
struct DBO_Cmd_Query_RoleInfoByGameID : public NetCmd
{
	DWORD		dwUserID;
	tagQueryRoleInfo RoleInfo;
};
//Check
//struct DBR_Cmd_LoadRoleCheckInfo : public NetCmd
//{
//	DWORD	dwUserID;
//};
//struct DBO_Cmd_LoadRoleCheckInfo : public NetCmd
//{
//	DWORD	dwUserID;
//	tagRoleCheckInfo	CheckInfo;
//};
//struct DBR_Cmd_ChangeRoleCheckInfo : public NetCmd
//{
//	DWORD	dwUserID;
//	tagRoleCheckInfo	CheckInfo;
//};
//Task
struct DBR_Cmd_LoadRoleTask : public NetCmd
{
	DWORD  dwUserID;
};
struct DBO_Cmd_LoadRoleTask : public NetCmd
{
	DWORD  dwUserID;
	BYTE   States;
	WORD   Sum;
	tagRoleTaskInfo Array[1];
};
struct DBR_Cmd_SaveRoleTask : public NetCmd
{
	DWORD  dwUserID;
	tagRoleTaskInfo TaskInfo;
};
struct DBR_Cmd_DelRoleTask : public NetCmd
{
	DWORD	dwUserID;
	BYTE	bTaskID;
};
struct DBR_Cmd_ClearRoleTask : public NetCmd
{
	DWORD	dwUserID;
};
struct DBR_Cmd_SaveAllTask : public NetCmd
{
	DWORD			dwUserID;
	BYTE			States;
	WORD			Sum;
	tagRoleTaskInfo Array[1];
};
//Achievement
struct DBR_Cmd_LoadRoleAchievement : public NetCmd
{
	DWORD  dwUserID;
};
struct DBO_Cmd_LoadRoleAchievement : public NetCmd
{
	DWORD  dwUserID;
	BYTE   States;
	WORD   Sum;
	tagRoleAchievementInfo Array[1];
};
struct DBR_Cmd_SaveRoleAchievement : public NetCmd
{
	DWORD  dwUserID;
	tagRoleAchievementInfo AchievementInfo;
};
struct DBR_Cmd_DelRoleAchievement : public NetCmd
{
	DWORD	dwUserID;
	BYTE	bAchievementID;
};
struct DBR_Cmd_SaveAllAchievement : public NetCmd
{
	DWORD			dwUserID;
	BYTE			States;
	WORD			Sum;
	tagRoleAchievementInfo Array[1];
};
//�ƺ�
struct DBR_Cmd_LoadRoleTitle : public NetCmd
{
	DWORD  dwUserID;
};
struct DBO_Cmd_LoadRoleTitle : public NetCmd
{
	DWORD  dwUserID;
	BYTE   States;
	WORD   Sum;
	BYTE   Array[1];
};
struct DBR_Cmd_AddRoleTitle : public NetCmd
{
	DWORD  dwUserID;
	BYTE   TitleID;
};
struct DBR_Cmd_DelRoleTitle : public NetCmd
{
	DWORD  dwUserID;
	BYTE   TitleID;
};
//Rank
struct DBR_Cmd_LoadRankInfo : public NetCmd
{
	BYTE  RankID;
	BYTE  RandRowCount;
};
struct DBO_Cmd_LoadRankInfo : public NetCmd
{
	BYTE  RankID;
	BYTE  States;
	WORD  Sum;
	tagRankInfo Array[1];
};
//struct DBR_Cmd_LoadWeekRankInfo : public NetCmd
//{
//	DWORD	VersionID;
//};
//struct RankIndexInfo
//{
//	BYTE		RankID;
//	BYTE		RankIndex;
//};
//struct tagRankWeekReward
//{
//	RankIndexInfo   IndexInfo;
//	bool			IsReward;//�Ƿ��Ѿ���ȡ������
//};
//struct tagDBWeekReward
//{
//	DWORD				dwUserID;
//	tagRankWeekReward	WeekRank;
//};
//struct DBO_Cmd_LoadWeekRankInfo : public NetCmd
//{
//	WORD				WeekRankSum;
//	tagDBWeekReward		WeekRankArray[1];
//};
//struct DBR_Cmd_SaveWeekRankInfo : public NetCmd//�����������
//{
//	DWORD				VersionID;
//	tagDBWeekReward		SaveInfo;
//};
struct tagRankWeekRankInfo
{
	DWORD			dwUserID;
	DWORD			VersionID;
	BYTE			RankID;
	BYTE			RankIndex;
	bool			IsReward;
};
struct tagRankWeekRankClient
{
	BYTE			RankID;
	BYTE			RankIndex;
	bool			IsReward;
};
struct DBR_Cmd_SaveWeekRankInfo : public NetCmd
{
	BYTE					States;
	WORD					Sum;
	tagRankWeekRankInfo		Array[1];
};
struct DBR_Cmd_LoadWeekRankInfo : public NetCmd
{
	DWORD					dwUserID;
	bool					IsInit;
};
struct DBO_Cmd_LoadWeekRankInfo : public NetCmd
{
	DWORD					dwUserID;
	bool					IsInit;
	BYTE					States;
	WORD					Sum;
	tagRankWeekRankClient	Array[1];
};
struct DBR_Cmd_ChangeWeekInfo : public NetCmd
{
	DWORD					dwUserID;
	tagRankWeekRankClient	RankInfo;
};
//����
struct DBR_Cmd_SaveRoleCharm : public NetCmd
{
	DWORD	dwUserID;
	DWORD   dwDestUserID;
	BYTE	bIndex;
	int		AddCharmValue;
	DWORD	ItemSum;
};
struct DBO_Cmd_SaveRoleCharm : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	bIndex;
	DWORD	ItemSum;
	bool	Result;
	int		dwDestUserCharmValue;
};
struct DBR_Cmd_AddRoleEntityItem : public NetCmd
{
	DWORD	dwUserID;
	DWORD	ItemID;
	DWORD	ItemSum;
	DWORD	UseMedal;
	DWORD	NowMedalSum;
	TCHAR	Name[MAX_NAME_LENTH + 1];//����
	unsigned __int64	Phone;//�ֻ�����
	TCHAR	Addres[MAX_ADDRESS_LENGTH + 1];//��ʵ��ַ
	TCHAR	IDEntity[MAX_IDENTITY_LENGTH+1];
};
//Entity
struct DBR_Cmd_LoadRoleEntity : public NetCmd
{
	DWORD	dwUserID;
};
struct DBO_Cmd_LoadRoleEntity : public NetCmd
{
	DWORD	dwUserID;
	tagRoleAddressInfo RoleInfo;
};
struct DBR_Cmd_SaveRoleEntityName : public NetCmd
{
	DWORD			dwUserID;
	TCHAR			Name[MAX_NAME_LENTH + 1];//����
};
struct DBR_Cmd_SaveRoleEntityPhone : public NetCmd
{
	DWORD			dwUserID;
	unsigned __int64	Phone;//�ֻ�����
};
struct DBR_Cmd_SaveRoleEntityEmail : public NetCmd
{
	DWORD			dwUserID;
	TCHAR			Email[MAX_EMAIL_LENGTH + 1];//����
};
struct DBR_Cmd_SaveRoleEntityIdentityID : public NetCmd
{
	DWORD	dwUserID;
	TCHAR			IdentityID[MAX_IDENTITY_LENGTH + 1];//���֤����
	
};
struct DBR_Cmd_SaveRoleEntityItemUseName : public NetCmd
{
	DWORD	dwUserID;
	TCHAR			EntityItemUseName[MAX_NAME_LENTH + 1];//�ջ��˵�����
	
};
struct DBR_Cmd_SaveRoleEntityItemUsePhone : public NetCmd
{
	DWORD	dwUserID;
	unsigned __int64 EntityItemUsePhone;//�ջ��˵��ֻ�����
	
};
struct DBR_Cmd_SaveRoleEntityItemUseAddress : public NetCmd
{
	DWORD	dwUserID;
	TCHAR			EntityItemUseAddres[MAX_ADDRESS_LENGTH + 1];//�ջ��˵���ʵ��ַ
};
struct DBR_Cmd_SaveRoleEntityAlipayInfo : public NetCmd
{
	DWORD dwUserID;
	TCHAR EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};
//struct DBR_Cmd_SaveRoleEntity : public NetCmd
//{
//	DWORD	dwUserID;
//	tagRoleAddressInfo RoleInfo;
//};
//Action
struct DBR_Cmd_LoadRoleAction : public NetCmd
{
	DWORD  dwUserID;
};
struct DBO_Cmd_LoadRoleAction : public NetCmd
{
	DWORD  dwUserID;
	BYTE	States;
	WORD   Sum;
	tagRoleActionInfo Array[1];
};
struct DBR_Cmd_SaveRoleAction : public NetCmd
{
	DWORD  dwUserID;
	tagRoleActionInfo ActionInfo;
};
struct DBR_Cmd_DelRoleAction : public NetCmd
{
	DWORD	dwUserID;
	BYTE	bActionID;
};
struct DBR_Cmd_SaveAllAction : public NetCmd
{
	DWORD			dwUserID;
	BYTE			States;
	WORD			Sum;
	tagRoleActionInfo Array[1];
};
//����
struct DBR_Cmd_LoadRoleGiff : public NetCmd
{
	DWORD	dwUserID;
	DWORD	LimitDay;
};
struct DBO_Cmd_LoadRoleGiff : public NetCmd
{
	DWORD	dwUserID;
	BYTE	States;
	WORD    Sum;
	tagGiffInfo Array[1];
};
struct DBR_Cmd_AddRoleGiff : public NetCmd
{
	DWORD	SrcUserID;
	DWORD	DestUserID;
};
struct DBO_Cmd_AddRoleGiff : public NetCmd
{
	DWORD		dwDestUserID;
	tagGiffInfo GiffInfo;
	bool	Result;
};
struct DBR_Cmd_DelRoleGiff : public NetCmd
{
	DWORD	GiffID;
};
struct tagNowDaySendGiffInfo
{
	DWORD		dwUserID;
	BYTE		SendSum;
};
struct DBR_Cmd_GetNowDayGiff : public NetCmd
{
	DWORD		dwUserID;
};
struct DBO_Cmd_GetNowDayGiff : public NetCmd
{
	//��ȡ������ҷ��͵����͵����� ���ID ����
	DWORD   dwUserID;
	BYTE	States;
	WORD	Sum;
	tagNowDaySendGiffInfo Array[1];
};
struct DBR_Cmd_ClearNowDayGiff : public NetCmd
{
	DWORD		dwUserID;
};
struct DBR_Cmd_LoadGameData : public NetCmd
{
	DWORD	dwUserID;
};
struct DBO_Cmd_LoadGameData : public NetCmd
{
	DWORD	dwUserID;
	tagRoleGameData GameData;
};
struct DBR_Cmd_SaveGameData : public NetCmd
{
	DWORD	dwUserID;
	tagRoleGameData GameData;
};
struct DBR_Cmd_ChangeGameDataByMonth :public NetCmd
{
	DWORD	dwUserID;
	BYTE	MonthIndex;
};
struct DBR_Cmd_LoadAllUserAchievementPointList :public NetCmd
{
	DWORD   RankSum;
};
struct DBO_Cmd_LoadAllUserAchievementPointList :public NetCmd
{
	BYTE		States;
	WORD		Sum;
	DWORD		Array[1];
};
//��½
struct AccountCacheInfo
{
	DWORD	dwUserID;
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	TCHAR   MacAddress[MAC_ADDRESS_LENGTH + 1];
	bool	IsFreeze;
	__int64 FreezeEndTime;
};
struct TempAccountCacheInfo
{
	DWORD			 ClientID;
	AccountCacheInfo AccountInfo;
};
struct DBR_Cmd_LoadAllAccountInfo :public NetCmd
{

};
struct DBO_Cmd_LoadAllAccountInfo :public NetCmd//�䳤�ְ�����
{
	BYTE				States;
	WORD				Sum;
	AccountCacheInfo	Array[1];
};
//struct DBO_Cmd_LoadAllAccountInfoFinish :public NetCmd
//{
//
//};
struct DBR_Cmd_LogInfo :public NetCmd
{
	DWORD			UserID;
	BYTE			Type;
	int				TypeSum;
	DWORD			Param;
	TCHAR			Info[DB_Log_Str_Length+1];
};
struct LogRechargeInfo
{
	//char			OrderString
	//char			OrderID
	//char			OrderCode
	//char			ChannelOrderID
	//char			channelLabel
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct DBR_Cmd_LogRecharge :public NetCmd
{
	DWORD				UserID;
	DWORD				ShopItemID;
	DWORD				Price;
	DWORD				FreePrice;
	DWORD				OldGlobelNum;
	DWORD				OldCurrceyNum;
	DWORD				AddGlobel;
	DWORD				AddCurrcey;
	WORD				AddRewardID;
	LogRechargeInfo		RechargeInfo;
};
struct DBR_Cmd_LogOnlineInfo :public NetCmd
{
	//��¼������ߵ�����
	DWORD			dwUserID;
	bool			IsOnline;
	int64			GlobelSum;
	DWORD			CurrceySum;
	DWORD			MadelSum;
	char			MacAddress[MAC_ADDRESS_LENGTH+1];
	char			IpAddress[IP_MAX_LENGTH + 1];
};
struct DBR_Cmd_LogRoleTableInfo :public NetCmd
{
	DWORD			dwUserID;
	int64			GlobelSum;
	DWORD			MedalSum;
	DWORD			CurrceySum;
	BYTE			TableID;
	BYTE			TableMonthID;
	bool			JionOrLeave;
};
struct LogEntityItemInfo
{
	//char				OrderStates
	//char				Address
	//char				IDEntity
	//char				Name
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct DBR_Cmd_LogEntityItemInfo :public NetCmd
{
	DWORD ID;
	DWORD ItemID;
	DWORD ItemSum;
	time_t ShopLogTime;
	DWORD UserID;
	UINT64 Phone;
	DWORD Medal;
	DWORD NowMedal;
	DWORD HandleIP;
	LogEntityItemInfo EntityInfo;
};
struct LogPhonePayInfo
{
	//char			OrderStates
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct DBR_Cmd_LogPhonePay :public NetCmd
{
	UINT64 OrderID;
	DWORD UserID;
	UINT64 PhoneNumber;
	DWORD FacePrice;
	LogPhonePayInfo PhonePayInfo;
};

struct DBR_Cmd_LogNiuNiuTableInfo :public NetCmd
{
	BYTE		BrandValue[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//25���Ƶ�����
	INT64		BrandGlobelSum;//ÿ����Ӯ�Ľ��
	DWORD		UserSum;
	INT64		SystemGlobelSum;
	time_t		NowTime;
};
struct DBR_Cmd_LogExChangeInfo :public NetCmd
{
	DWORD		dwUserID;
	BYTE		ExChangeTypeID;
	TCHAR		ExChangeCode[ExChangeCode_Length + 1];
};
struct DBR_Cmd_LogLotteryInfo :public NetCmd
{
	DWORD		dwUserID;
	BYTE		LotteryID;
	WORD		RewardID;
};
struct DBR_Cmd_LogMonthInfo :public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
	DWORD		MonthIndex;
	DWORD		MonthScore;
	DWORD		SkillSum;
	BYTE		AddGlobelSum;
	WORD		RewardID;
};
struct DBR_Cmd_LogDialInfo :public NetCmd
{
	DWORD BanderUserID;
	BYTE AreaData[MAX_DIAL_GameSum];
	UINT64 AreaGlobel[MAX_DIAL_ClientSum];
	WORD ResultIndex;
	INT64 BrandGlobel;
	INT64 SystemGlobel;
	DWORD RoleSum;
};
struct DBR_Cmd_LogCarInfo :public NetCmd
{
	DWORD BanderUserID;
	UINT64 AreaGlobel[MAX_CAR_ClientSum];
	BYTE ResultIndex;
	INT64 BrandGlobel;
	INT64 SystemGlobel;
	DWORD RoleSum;
};

struct DBR_Cmd_LoadAllAnnouncement :public NetCmd
{

};
struct DBO_Cmd_LoadALlAnnouncement :public NetCmd
{
	BYTE			States;
	WORD			Sum;
	AnnouncementOnce Array[1];
};
//struct DBO_Cmd_LoadAllAnnouncementFinish :public NetCmd
//{
//
//};
struct DBR_Cmd_AddAnnouncement :public NetCmd
{
	AnnouncementOnce AnnouncementInfo;
};
struct DBR_Cmd_SaveRoleEmail :public NetCmd
{
	DWORD		dwUserID;
	TCHAR		Email[MAX_EMAIL_LENGTH+1];
};
struct DBR_Cmd_ClearFishDB : public NetCmd
{
	bool		IsInit;
};
struct DBO_Cmd_ClearFishDB : public NetCmd
{
	
};

//ExChange
struct DBR_Cmd_QueryExChange : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		ExChangeCode[ExChangeCode_Length+1];
};
struct DBO_Cmd_QueryExChange : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		ExChangeCode[ExChangeCode_Length + 1];
	BYTE		ID;
};
struct DBR_Cmd_DelExChange : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		ExChangeCode[ExChangeCode_Length + 1];
};

//Recharge
struct DBR_Cmd_AddRecharge : public NetCmd
{
	//��ҽ��в���  ������ߵ�����½���
	DWORD		AddGlobelSum;
	DWORD		AddCurrceySum;
	bool		IsFirst;
	//Log
	RechageInfo	rechargeInfo;//��ֵ�Ĳ���
};
struct DBR_Cmd_LoadRechageOrderID : public NetCmd
{
	//����ȫ���Ķ�����
	BYTE			DiffDay;
};
struct OrderSetInfo
{
	UINT64   OrderID;
	BYTE	 OrderType;
};
struct DBO_Cmd_LoadRechageOrderID : public NetCmd
{
	BYTE						States;
	WORD						Sum;
	OrderSetInfo				Array[1];
};
struct DBR_Cmd_AddRechageOrderID : public NetCmd
{
	DWORD						dwUserID;
	OrderSetInfo				OrderOnlyID;
};
struct DBR_Cmd_CheckEntityID : public NetCmd
{
	//������֤
	DWORD						dwUserID;
	DWORD						EntityCrcID;
	TCHAR						EntityID[MAX_IDENTITY_LENGTH+1];
};
struct DBO_Cmd_CheckEntityID : public NetCmd
{
	DWORD						dwUserID;
	DWORD						EntityCrcID;
	bool						Result;
};
struct DBR_Cmd_CheckPhone : public NetCmd
{
	bool						IsBindOrLogon;
	DWORD						dwUserID;
	unsigned __int64			Phone;
	DWORD						ClientID;
};
struct DBO_Cmd_CheckPhone : public NetCmd
{
	bool						IsBindOrLogon;
	DWORD						dwUserID;
	bool						Result;
	unsigned __int64			Phone;
	DWORD						ClientID;
};
struct DBR_Cmd_LoadPhonePayOrderID : public NetCmd
{
	BYTE			DiffDay;
};
struct DBO_Cmd_LoadPhonePayOrderID : public NetCmd
{
	BYTE						States;
	WORD						Sum;
	unsigned __int64			Array[1];
};
struct DBR_Cmd_AddPhonePayOrderID : public NetCmd
{
	DWORD						dwUserID;
	unsigned __int64			OrderOnlyID;
};
struct DBR_Cmd_GetRechargeOrderID : public NetCmd
{
	DWORD						dwUserID;
	DWORD						ShopID;
	bool						IsPC;
};
struct DBO_Cmd_GetRechargeOrderID : public NetCmd
{
	DWORD						OrderID;
	DWORD						dwUserID;
	DWORD						ShopID;
	bool						IsPC;
};
struct DBR_Cmd_QueryRechargeOrderInfo : public NetCmd
{
	DWORD						OrderID;
	DWORD						Price;
	DWORD						CheckUserID;
	DWORD						CheckShopID;
};
struct DBO_Cmd_QueryRechargeOrderInfo : public NetCmd
{
	DWORD						OrderID;
	DWORD						dwUserID;
	DWORD						ShopID;
	DWORD						Price;
	DWORD						CheckUserID;
	DWORD						CheckShopID;
};
struct DBR_Cmd_DelRechargeOrderInfo : public NetCmd
{
	DWORD						OrderID;
};
//Control
struct DBR_Cmd_ResetUserPassword : public NetCmd
{
	DWORD						dwUserID;
	DWORD						Password1;
	DWORD						Password2;
	DWORD						Password3;
};
struct DBR_Cmd_FreezeUser : public NetCmd
{
	DWORD						dwUserID;
	DWORD						FreezeMin;
};

struct RobotUserInfo  //�����˵Ļ�������
{
	DWORD			dwUserID;
	TCHAR			NickName[MAX_NICKNAME + 1];
	WORD			wLevel;//��ҵ��
	DWORD			dwExp;//��Ҿ���
	DWORD			dwFaceID;//���ͷ��ID
	bool			bGender; //����Ա�
	DWORD			dwGlobeNum;
	DWORD           dwMedalNum;
	DWORD			dwCurrencyNum;
	DWORD			dwAchievementPoint;//��ҵĳɾ͵���
	BYTE			TitleID;
	DWORD			CharmArray[MAX_CHARM_ITEMSUM];//����ֵ��Ӧ����Ʒ
	BYTE			VipLevel;//���漰���ݿ��
	bool			IsInMonthCard;//�Ƿ�Ϊ�¿��û�
};

struct DBR_Cmd_LoadRobotInfo : public NetCmd
{
	DWORD						BeginUserID;
	DWORD						EndUserID;
};
struct DBO_Cmd_LoadRobotInfo : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	RobotUserInfo Array[1];
};
//char
struct tagRoleCharInfo  //һ��������Ϣ ���ѵ����� ���� ����Я�����������ص�����
{
	DWORD		SrcUserID;//���ͷ�
	DWORD		DestUserID;//���շ�
	TCHAR		MessageInfo[MAX_ROLECHAR_LENGTH + 1];//��Ϣ������
	__int64     LogTime;//���͵�ʱ��
};

struct DBR_Cmd_LoadCharInfo : public NetCmd
{
	DWORD		dwUserID;
};
struct DBO_Cmd_LoadCharInfo : public NetCmd
{
	DWORD				dwUserID;
	BYTE				States;
	WORD				Sum;
	tagRoleCharInfo		Array[1];
};
struct DBR_Cmd_DelCharInfo : public NetCmd
{
	DWORD				dwSrcUserID;
	DWORD				dwDestUserID;
};
struct DBR_Cmd_AddCharInfo : public NetCmd
{
	tagRoleCharInfo		MessageInfo;
};


struct tagRelationRequest
{
	DWORD		ID;
	DWORD		SrcUserID;
	DWORD		SrcFaceID;
	TCHAR		SrcNickName[MAX_NICKNAME + 1];
	WORD		SrcLevel;
	BYTE		SrcVipLevel;
	bool		SrcGender;
	TCHAR		SrcIPAddress[MAX_ADDRESS_LENGTH_IP + 1];
	DWORD		DestUserID;
	TCHAR		MessageInfo[MAX_RelationRequestLength + 1];
	BYTE		RelationType;
	__int64     SendTime;
};
struct DBR_Cmd_LoadRelationRequest : public NetCmd
{
	DWORD				dwUserID;
};
struct DBO_Cmd_LoadRelationRequest : public NetCmd
{
	DWORD				dwUserID;
	BYTE				States;
	WORD				Sum;
	tagRelationRequest	Array[1];
};
struct DBR_Cmd_AddRelationRequest : public NetCmd
{
	DWORD				dwUserID;
	tagRelationRequest	Info;
};
struct DBO_Cmd_AddRelationRequest : public NetCmd
{
	DWORD				dwUserID;
	bool				Result;
	BYTE				ErrorID;
	tagRelationRequest	Info;
};
struct DBR_Cmd_DelRelationRequest : public NetCmd
{
	DWORD				dwUserID;
	DWORD				ID;
};

struct tagCashOrderInfo
{
	DWORD	dwOrderID;//����ID
	DWORD	dwUserID;//�������ID
	DWORD	dwOrderGold;//�����漰���Ľ������
	bool	bFinish;//���������Ƿ�ɹ� ���ʧ����Ҫ�˻����
};
struct DBR_Cmd_LoadCash : public NetCmd
{

};
struct DBO_Cmd_LoadCash : public NetCmd
{
	BYTE				States;
	WORD				Sum;
	tagCashOrderInfo	Array[1];
};
struct DBR_Cmd_HandleCash : public NetCmd
{
	DWORD				OrderID;
};
struct DBR_Cmd_AddCash : public NetCmd
{
	DWORD				dwUserID;
	DWORD				dwGoldNum;
	DWORD				dwOriginalRMB;
	DWORD				dwLoseRMB;
};
struct DBO_Cmd_AddCash : public NetCmd
{
	DWORD				dwUserID;
	DWORD				dwGoldNum;
	bool				Result;
};
struct DBR_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD				dwUserID;
	TCHAR				DestNickName[MAX_NICKNAME + 1];
};
struct DBO_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD				dwUserID;
	DWORD				dwDestUserID;
};
struct DBR_Cmd_HandleCust : public NetCmd
{
	DWORD				dwMailID;
	TCHAR				strContext[128];
};
struct DBO_Cmd_HandleCust : public NetCmd
{
	DWORD				dwUserID;
	TCHAR				strContext[128];
};
//��������ģ�� 
//������
enum MainCmd
{

	Main_HallHeartBeat = 101,//��Ϸ��������
	Main_CmdInit = 128,
	Main_Logon = Main_CmdInit + 1,//��½����
	Main_Table = Main_CmdInit + 2,//�����ϵ�����
	Main_Game = Main_CmdInit + 3,//��ϷID
	Main_Center = Main_CmdInit + 4,//�������������ͨѶ
	Main_Balanced = Main_CmdInit + 5,//Gate�븺�ؾ���֮���ͨѶ

	Main_Item = Main_CmdInit + 6,//�����Ʒ��صĲ���
	Main_Relation = Main_CmdInit + 7,
	Main_Mail = Main_CmdInit + 8,//�ʼ� ϵͳ�ʼ� ���� ���˽��
	Main_Role = Main_CmdInit + 9,//��ҵĻ��������޸�
	Main_Query = Main_CmdInit + 10,//��ѯ�������
	Main_Check = Main_CmdInit + 11,//���ǩ��
	Main_Task = Main_CmdInit + 12,//�������
	Main_Achievement = Main_CmdInit + 13,//��ҳɾ�
	Main_Title = Main_CmdInit + 14,//��ҳƺ�
	Main_Month = Main_CmdInit + 15,//����
	Main_Rank = Main_CmdInit + 16,//���а�
	Main_Chest = Main_CmdInit + 17,//����
	Main_Charm = Main_CmdInit + 18,//����
	Main_Shop = Main_CmdInit + 19,//�̵�
	Main_Entity = Main_CmdInit + 20,//�����ʵ����
	Main_Action = Main_CmdInit + 21,//�
	Main_Giff = Main_CmdInit + 22,//����
	//Main_GlobelShop = Main_CmdInit+ 23,
	Main_Hall = Main_CmdInit + 24,//��Ϸ����
	Main_Ftp = Main_CmdInit + 25,
	Main_GameData = Main_CmdInit + 26,
	Main_Package = Main_CmdInit + 27,//���
	Main_Launcher = Main_CmdInit + 28,
	Main_Message = Main_CmdInit + 29,
	Main_Recharge = Main_CmdInit + 30,
	Main_Announcement = Main_CmdInit + 31,
	Main_Operate = Main_CmdInit + 32,//��Ӫ ����ķ���
	Main_RoleMessage = Main_CmdInit + 33,
	Main_Exchange = Main_CmdInit + 34,//�һ�����
	Main_Lottery = Main_CmdInit + 35,//�齱
	Main_Extra_Down = Main_CmdInit + 36,//FTP��������
	Main_MiniGame = Main_CmdInit + 37,
	Main_NiuNiu = Main_CmdInit + 38,
	Main_Char = Main_CmdInit + 39,
	Main_RelationRequest = Main_CmdInit + 40,
	Main_Dial = Main_CmdInit + 41,
	Main_Car = Main_CmdInit + 42,
	Main_FiveNiuNiu = Main_CmdInit + 43,
	Main_Cash = Main_CmdInit + 44,
	Main_OnlineReward = Main_CmdInit + 45,

	Main_Server		  = 200,//���������ܵ�����
	Main_Control	  = 201,//������
};
enum LogonCmd
{
	CL_AccountLogon = 1,
	CL_AccountRsg = 2,
	//CL_AccountQueryRsg = 3,

	LG_AccountOnlyID = 4,
	LC_AccountOnlyID = 5,

	CL_AccountOnlyID = 6,
	LC_AccountOnlyIDFailed = 7,
	LC_AccountOnlyIDSuccess = 8,

	/*CL_AccountChangePassword = 6,
	LC_AccountChangePassword = 7,

	CL_AccountResetAccount = 8,
	LC_AccountResetAccount = 9,*/

	//GameServer ֮������ͬ��
	GL_RsgGameServer = 9,
	//GL_OnlinePlayerSum = 10,

	GL_AccountOnlyID = 11,

	CL_ResetLogonGameServer = 12,
	LC_ResetLogonGameServer = 13,

	LC_AccountLogonToScreen = 14,//���ֱ�ӵ�½������

	LC_ServerChangeSocket	= 15,//�������Ϸ�б�������

	CL_ResetPassword		=16,
	LC_ResetPassword		=17,

	CL_AccountResetAccount	=18,
	LC_AccountResetAccount  = 19,

	LC_LogonQueueWrite		= 20,

	GL_ResetPassword		= 21,
	GL_ResetAccount			= 22,


	CL_GetNewAccount		=23,
	LC_GetNewAccount		=24,

	LC_CheckVersionError    =25,
	LC_AccountIsFreeze	    = 26,

	//�����ĵ�½
	CL_ChannelLogon			= 30,//�ͻ��˷����½
	LO_ChannelLogon			= 31,
	OL_ChannelLogon			= 32,
	LC_ChannelLogon			= 33,

	//���ٵ�½
	CL_QueryLogon			= 40,
	LG_ChannelInfo			= 41,

	//�ֻ������½
	CL_PhoneLogon			= 50,
	LO_PhoneLogon			= 51,
	OL_PhoneLogon			= 52,
	LC_PhoneLogon			= 53,
	CL_PhoneLogonCheck      = 54,
	LO_PhoneLogonCheck		= 55,
	OL_PhoneLogonCheck		= 56,
	LC_PhoneLogonCheck		= 57,
	LC_SaveAccountInfo      = 58,

	//�ֻ����������½
	CL_PhoneSecPwd			= 70,

	//WeiXin
	CL_WeiXinLogon			= 80,
	LO_WeiXinLogon			= 81,
	OL_WeiXinLogon			= 82,
	LC_WeiXinLogon			= 83,

	//QQ
	CL_QQLogon				= 84,
	LO_QQLogon				= 85,
	OL_QQLogon				= 86,
	LC_QQLogon				= 87,

	//
	LC_RsgNewAccount		= 88,

};

struct DBR_Cmd_SaveRecord :public NetCmd
{
	int model; //0,������Ϸ��1��ͬ�����ݣ�2�˳���Ϸ
	int64 uid;//���ID
	int table_id; //����ID
	int64 enter_money; //�����ǽ��
	int64 leave_money; //�뿪ʱ���
	int leave_code; //0�������˳���1��ǿ���˳���2��ϵͳ��ʱ�߳�
};
struct DBR_Cmd_SaveTotalGold :public NetCmd
{
	int server_id;
	int table_id;
	int64 lastEarn;
	int64 lastProduce;
	int64 table_totalEarn;
	int64 table_totalProduce;
};

struct CL_Cmd_QueryLogon : public NetCmd
{
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	BYTE	PlateFormID;//ƽ̨��ID
	DWORD   VersionID;
	DWORD	PathCrc;
	BYTE	LogonType;//1:Channel  2:WeiXin  3:QQ
};
struct CL_Cmd_AccountLogon : public NetCmd
{
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	BYTE	PlateFormID;//ƽ̨��ID
	DWORD   VersionID;
	DWORD	PathCrc;
};
struct CL_Cmd_PhoneSecPwd : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				PasswordCrc1;
	DWORD				PasswordCrc2;
	DWORD				PasswordCrc3;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				VersionID;
	DWORD				PathCrc;
};
struct CL_Cmd_AccountRsg : public NetCmd
{
	TCHAR	AccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	/*TCHAR	NickName[MAX_NICKNAME + 1];
	bool	Gender;*/
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	BYTE	PlateFormID;//ƽ̨��ID
	DWORD   VersionID;
	DWORD	PathCrc;
};
//struct CL_Cmd_AccountQueryRsg : public NetCmd
//{
//	DWORD	PasswordCrc1;
//	DWORD	PasswordCrc2;
//	DWORD	PasswordCrc3;
//	/*TCHAR	NickName[MAX_NICKNAME + 1];
//	bool	Gender;*/
//	BYTE	PlateFormID;//ƽ̨��ID
//	DWORD   VersionID;
//	DWORD	PathCrc;
//};
struct LG_Cmd_AccountOnlyID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwOnlyID;//���ɵ�ΨһID��֤��
	DWORD	ClientID;
	BYTE	GameServerID;
	BYTE	LogonID;
	BYTE	LogonTypeID;//��½1 ע��2
};
struct GL_Cmd_AccountOnlyID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwOnlyID;//���ɵ�ΨһID��֤��
	DWORD	ClientID;
	BYTE	GameServerID;
	BYTE	LogonTypeID;//��½1 ע��2
};
struct LC_Cmd_AccountOnlyID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwOnlyID;
	DWORD	GateIp;
	WORD	GatePort;
	DWORD	GameIp;
	WORD	GamePort;
	DWORD	HallTimeOut;
	BYTE	LogonType;//��½1  ע��2
};
struct CL_Cmd_AccountOnlyID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwOnlyID;
	BYTE	PlateFormID;//���ƽ̨ID
	DWORD	ScreenPoint;//�����Ļ�ֱ���
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
};
struct LC_Cmd_AccountOnlyIDFailed : public NetCmd
{
};
struct LC_LoadFinish : public NetCmd
{
};
struct LC_Cmd_AccountOnlyIDSuccess : public NetCmd
{
	DWORD			RandID;//�ͻ���Ψһ��ID
	DWORD			OperateIp;//Я����Ӫ��������IP��ַ
	tagRoleInfo		RoleInfo;//�������
};
struct LC_Cmd_AccountLogonToScreen : public NetCmd
{
	DWORD			RandID;//�ͻ���Ψһ��ID
	//tagRoleInfo		RoleInfo;//�������
	BYTE			bTableTypeID;
	BYTE			SeatID;
	BYTE			LauncherType;
	BYTE			BackgroundImage;
	BYTE			RateIndex;
	int				Energy;
};
struct CL_Cmd_ResetLogonGameServer : public NetCmd
{
	DWORD			UserID;
	DWORD			RandID;
	DWORD			VersionID;
	DWORD			PathCrc;
};
struct LC_Cmd_ResetLogonGameServer : public NetCmd
{
};
//struct CL_Cmd_AccountChangePassword : public NetCmd
//{
//	DWORD	dwUserID;
//	DWORD	PasswordCrc1;
//	DWORD	PasswordCrc2;
//	DWORD	PasswordCrc3;
//	DWORD	OldPasswordCrc1;
//	DWORD	OldPasswordCrc2;
//	DWORD	OldPasswordCrc3;
//};
//struct LC_Cmd_AccountChangePassword : public NetCmd
//{
//	DWORD	dwUserID;
//	bool	Result;
//};
struct GL_Cmd_RsgGameServer : public NetCmd
{
	//DWORD	Ip;//ע�ᵱǰGameServerȥLogon����
	//WORD	Port;
	DWORD GameConfigID;
};
//struct GL_Cmd_OnlinePlayerSum : public NetCmd
//{
//	WORD	PlayerSum;//�����������
//};
struct CL_Cmd_ResetPassword : public NetCmd//�޸�����
{
	DWORD		OldPasswordCrc1;
	DWORD		OldPasswordCrc2;
	DWORD		OldPasswordCrc3;
	DWORD		NewPasswordCrc1;
	DWORD		NewPasswordCrc2;
	DWORD		NewPasswordCrc3;
};
struct LC_Cmd_ResetPassword : public NetCmd//�޸�����Ľ��
{
	bool		Result;
	DWORD		NewPasswordCrc1;
	DWORD		NewPasswordCrc2;
	DWORD		NewPasswordCrc3;
};
struct CL_Cmd_AccountResetAccount : public NetCmd
{
	TCHAR	NewAccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct LC_Cmd_AccountResetAccount : public NetCmd
{
	bool	Result;
};
struct LC_Cmd_LogonQueueWrite : public NetCmd
{
	DWORD	WriteIndex;
	DWORD	WriteSec;
};
struct GL_Cmd_ResetPassword : public NetCmd
{
	DWORD	dwUserID;
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct GL_Cmd_ResetAccount : public NetCmd
{
	DWORD	dwUserID;
	TCHAR	NewAccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct CL_Cmd_GetNewAccount : public NetCmd
{
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
	TCHAR	MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	BYTE	PlateFormID;//ƽ̨��ID
	DWORD   VersionID;
	DWORD	PathCrc;
};
struct LC_Cmd_GetNewAccount : public NetCmd
{
	bool	Result;
	TCHAR	NewAccountName[ACCOUNT_LENGTH + 1];
	DWORD	PasswordCrc1;
	DWORD	PasswordCrc2;
	DWORD	PasswordCrc3;
};
struct LC_Cmd_CheckVersionError : public NetCmd
{
	DWORD   VersionID;
	DWORD	PathCrc;
};
struct LC_Cmd_AccountIsFreeze : public NetCmd
{
	__int64 EndTime;
};
struct CL_Cmd_ChannelLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				VersionID;
	DWORD				PathCrc;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	ChannelUserInfo		channelInfo;
};
struct LO_Cmd_ChannelLogon : public NetCmd
{
	DWORD				ClientID;
	BYTE				PlateFormID;//ƽ̨��ID
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	ChannelUserInfo		channelInfo;
};
struct OL_Cmd_ChannelLogon : public NetCmd
{
	//����Ӫ�������� ����򽻵� 
	DWORD				ClientID;
	BYTE				PlateFormID;//ƽ̨��ID
	bool				Result;//��½�Ľ��
	DWORD				ChannelID;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	ChannelUserInfo		channelInfo;
};
struct LC_Cmd_ChannelLogon : public NetCmd
{
	bool				Result;
};

struct WeiXinUserCodeInfo
{
	//CHAR			Code[xx];//����Ϊ OpenID
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct CL_Cmd_WeiXinLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				VersionID;
	DWORD				PathCrc;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	WeiXinUserCodeInfo  CodeInfo;
};
struct LO_Cmd_WeiXinLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				ClientID;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	WeiXinUserCodeInfo  CodeInfo;
};
struct OL_Cmd_WeiXinLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				ClientID;
	bool				Result;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	WeiXinUserCodeInfo  OpenInfo;
};
struct LC_Cmd_WeiXinLogon : public NetCmd
{
	bool				Result;
};

struct QQUserCodeInfo
{
	//CHAR			Token[xx];//����Ϊ OpenID
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct CL_Cmd_QQLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				VersionID;
	DWORD				PathCrc;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	QQUserCodeInfo		TokenInfo;
};
struct LO_Cmd_QQLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				ClientID;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	QQUserCodeInfo		TokenInfo;
};
struct OL_Cmd_QQLogon : public NetCmd
{
	BYTE				PlateFormID;//ƽ̨��ID
	DWORD				ClientID;
	bool				Result;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];//Mac��ַ
	QQUserCodeInfo		OpenInfo;
};
struct LC_Cmd_QQLogon : public NetCmd
{
	bool				Result;
};

struct LC_Cmd_RsgNewAccount : public NetCmd
{
	//����ǰ�˺�Ϊ��ע���ʱ��
	BYTE				LogonType;//1:Channel 2:WeiXin 3:QQ
};

enum PhoneLogonError
{
	PLE_Success = 0,//
	PLE_LogonExists = 1,//�ֻ������½�Ѿ����� 
	PLE_PasswordFailed = 2,//�ֻ���֤�뷢��ʧ��
	PLE_PasswordError = 3,//�ֻ���֤�����
	PLE_SystemError = 4,//ϵͳ����
};

struct CL_Cmd_PhoneLogon : public NetCmd
{
	UINT64				PhoneNumber;
};
struct LO_Cmd_PhoneLogon : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				ClientID;
};
struct OL_Cmd_PhoneLogon : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				ClientID;
	BYTE				ErrorID;
};
struct LC_Cmd_PhoneLogon : public NetCmd
{
	UINT64				PhoneNumber;
	BYTE				ErrorID;
};
struct CL_Cmd_PhoneLogonCheck : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				Password;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD				Crc1;
	DWORD				Crc2;
	DWORD				Crc3;
	bool				IsFirOrSec;
};
struct LO_Cmd_PhoneLogonCheck : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				Password;
	DWORD				ClientID;
	DWORD				ClientIP;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD				Crc1;
	DWORD				Crc2;
	DWORD				Crc3;
	bool				IsFirOrSec;
};
struct OL_Cmd_PhoneLogonCheck : public NetCmd
{
	UINT64				PhoneNumber;
	DWORD				ClientID;
	bool				Result;
	BYTE				ErrorID;
	DWORD				ClientIP;
	TCHAR				MacAddress[MAC_ADDRESS_LENGTH + 1];
	DWORD				Crc1;
	DWORD				Crc2;
	DWORD				Crc3;
	bool				IsFirOrSec;
};
struct LC_Cmd_PhoneLogonCheck : public NetCmd
{
	bool				Result;
	DWORD				dwUserID;
	BYTE				ErrorID;
};
struct LC_Cmd_SaveAccountInfo : public NetCmd
{
	TCHAR			AccountName[ACCOUNT_LENGTH + 1];
	DWORD			PasswordCrc1;
	DWORD			PasswordCrc2;
	DWORD			PasswordCrc3;
};
enum TableCmd
{
	//��ҵ�½�ɹ��� ����ѡ���������
	CL_Sub_JoinTable = 1,//��������
	LC_Sub_JoinTable = 2,//�������ӵķ���
	CL_Sub_LeaveTable = 3,
	//��������Ϣͬ��
	LC_Sub_OtherUserInfo = 4,//������ҵ���Ϣ
	LC_Sub_UserLeave = 5,//����뿪����

	LC_Sub_MeSeatID = 6,
	LC_Sub_StartSyncFish = 7,//����ͬ������
	LC_Sub_ShowOthers = 8, //�鿴���������Ϣ
	LC_ResetOtherUserInfo = 30,

	LC_LeaveTableByServer = 31,//������������뿪����

};
struct CL_JoinTable : public NetCmd
{
	BYTE		bTableTypeID;//ѡ��������ӵ�����
	bool        isArena; //�Ƿ��Ǿ�����
};

struct tagJonPlayerData
{
	int64 			playerId ;
	char 			playerName[MAX_NICKNAME + 1];
	INT				sex;
	INT 			headicon;//ͷ���
	char 			icon[ICON_LENGTH];//ͷ��
	INT				vipLevel;
	int64 			goldNum;
	INT 			seat;//��λ��
	INT				launcherType;//��̨����
	INT				rateIndex;//����
	INT 			energy;//��ǰ��ŭ�����ܵ�ֵ
	INT				bulletCount;// �������ӵ�����
	INT64				score; //����������
};

struct LC_JoinTableResult : public NetCmd
{
	bool		Result;
	INT			RoomID;
	BYTE		BackgroundImage;
	tagJonPlayerData PlayerData;
};
struct CL_LeaveTable : public NetCmd
{
};
struct LC_ReqStartSyncFish :public NetCmd
{

};
struct LC_OtherUserInfo : public NetCmd
{
	tagJonPlayerData UserInfo;
};
struct LC_OtherUserLeave : public NetCmd
{
	DWORD		dwUserID;
};

struct CL_Req_Connect : public NetCmd //����UDP������Ϸ������
{
	TCHAR userid[MAX_USER_ID + 1];
};
struct LC_Cmd_MeSeatID : public NetCmd
{
	BYTE		SeatID;
};
struct LC_Cmd_TableChangeRoleLevel : public NetCmd
{
	DWORD		dwDestUserID;
	WORD		wLevel;
};
struct LC_Cmd_TableChangeRoleGender : public NetCmd
{
	DWORD		dwDestUserID;
	bool		bGender;
};
struct LC_Cmd_TableChangeRoleNickName : public NetCmd
{
	DWORD       dwDestUserID;
	TCHAR		NickName[MAX_NICKNAME + 1];
};
struct LC_Cmd_TableChangeRoleFaceID : public NetCmd
{
	DWORD		dwDestUserID;
	DWORD		dwDestFaceID;
};
struct LC_Cmd_TableChangeRoleGlobe : public NetCmd
{
	DWORD		dwDestUserID;
	int			dwGlobelSum;
};
struct LC_Cmd_TableChangeRoleExp : public NetCmd
{
	DWORD		dwDestUserID;
	DWORD		dwExp;
};
struct LC_Cmd_TableChangeRoleTitle : public NetCmd
{
	DWORD		dwDestUserID;
	BYTE		TitleID;
};
struct LC_Cmd_TableChangeRoleAchievementPoint : public NetCmd
{
	DWORD		dwDestUserID;
	DWORD		dwAchievementPoint;
};
struct LC_Cmd_TableChangeRoleCharmValue : public NetCmd
{
	DWORD		dwDestUserID;
	DWORD		CharmInfo[MAX_CHARM_ITEMSUM];
};
struct LC_Cmd_TableChangeRoleIpAddress : public NetCmd
{
	DWORD		dwDestUserID;
	TCHAR		IpAddress[MAX_ADDRESS_LENGTH_IP+1];
};
struct LC_Cmd_ResetOtherUserInfo : public NetCmd
{
	tagRemoteRoleInfo UserInfo;
};
struct LG_Cmd_ChannelInfo : public NetCmd
{
	DWORD				dwUserID;
	DWORD				ClientID;
	ChannelUserInfo		channelUserInfo;
};
struct LC_Cmd_LeaveTableByServer : public NetCmd
{
	bool				IsReturnLogon;
};
struct LC_Cmd_TableChangeRoleVipLevel : public NetCmd
{
	DWORD				dwDestUserID;
	BYTE				VipLevel;
};
struct LC_Cmd_TableChangeRoleIsInMonthCard : public NetCmd
{
	DWORD				dwDestUserID;
	bool				IsInMonthCard;
};
//---------------------------------------------------------------game
enum GameSub
{
	GAME_SUB_COMBO = 50,
};

struct Game_Combo : public NetCmd
{
	BYTE seat;
	BYTE byOrder;
	bool bbuff;
};
//---------------------------------------------------------------���������
enum CenterSub
{
	CL_Sub_UserEnter = 1,
	CL_Sub_UserEnterFinish = 5,
	CL_Sub_RsgFinish = 6,
	CL_Sub_UserLeave = 10,
	CC_UserLeaveGame = 11,
	CG_CenterTimeChange = 12,
	CL_Sub_ReqConnect = 13,
};
struct CL_UserEnter : public NetCmd
{
	//�����û���½Center����Ϣ ��Ϣ������ҵ�Զ������ 
	tagCenterRoleInfo RoleInfo;
	bool			  IsRobot;
};

struct CL_HallStartFishServer :public NetCmd
{
	INT64 starTime;
};
struct CL_UserEnterFinish : public NetCmd
{
	DWORD			dwUserID;
};
struct CL_UserLeave : public NetCmd
{
	DWORD				dwUserID;
};

enum BalancedSub
{
	CL_Sub_GateInfo = 1,

	CL_Sub_GetGate = 2,
	LC_Sub_GetGate = 3,
};
struct CL_GateInfo : public NetCmd
{
	__int64		GroupID;
	DWORD		Ip;
	WORD		Port;
	WORD		UserSum;
	WORD		UserMaxSum;
};
struct LC_GetGate : public NetCmd
{
	bool		Result;
	DWORD		Ip;
	WORD		Port;
};
struct CC_Cmd_UserLeaveGame : public NetCmd
{
	//������� ����
	DWORD		dwUserID;
};
//struct CG_Cmd_DayChange
//{
//	bool		MonthIsChange;
//};
struct CG_Cmd_CenterTimeChange : public NetCmd
{
	//���������������ȫ���ķ��������и��²��� 1���ӽ���һ�θ���
	__int64	TimeLog;//�����������ʱ��
};
//-----------------------------------------------------��Ʒ	

enum ItemSub
{
	CL_GetUserItem = 1,
	LC_GetUserItem = 2,
	//LC_GetUserItemFinish = 3,

	LC_AddUserItem = 4,
	LC_DelUserItem = 5,
	LC_ChangeUserItem = 6,

	CL_OnUseItem = 7,
	LC_OnUseItem = 8,

	CL_OnAcceptItem = 9,
	LC_OnAcceptItem = 10,
};
struct CL_Cmd_GetUserItem : public NetCmd
{

};
struct LC_Cmd_GetUserItem : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	tagItemInfo Array[1];
};
//struct LC_Cmd_GetUserItemFinish : public NetCmd
//{
//};
struct LC_Cmd_AddUserItem : public NetCmd
{
	tagItemInfo ItemInfo;
};
struct LC_Cmd_DelUserItem : public NetCmd
{
	DWORD		ItemOnlyID;
};
struct LC_Cmd_ChangeUserItem : public NetCmd
{
	DWORD		ItemOnlyID;
	DWORD		ItemSum;
};
struct CL_Cmd_OnUseItem : public NetCmd
{
	DWORD		ItemOnlyID;
	DWORD		ItemID;
	DWORD		ItemSum;
};
struct LC_Cmd_OnUseItem : public NetCmd
{
	bool		Result;
	DWORD		ItemOnlyID;
	DWORD		ItemID;
	DWORD		ItemSum;
};
struct CL_Cmd_OnAcceptItem : public NetCmd
{
	DWORD		dwID;
	DWORD		ItemOnlyID;
	DWORD		ItemID;
	DWORD		ItemSum;
	bool		GameIDorUserID;
};
struct LC_Cmd_OnAcceptItem : public NetCmd
{
	bool		Result;
	DWORD		dwDestUserID;
	DWORD		ItemOnlyID;
	DWORD		ItemID;
	DWORD		ItemSum;
};
//------------------------------------------------Relation
enum RelationSub
{
	CL_GetUserRelation = 1,
	LC_GetUserRelation = 2,
	//LC_GetUserRelationFinish = 3,

	CL_AddUserRelation = 4,
	LC_AddUserRelation = 5,
	CL_DelUserRelation = 6,
	LC_DelUserRelation = 7,
	//CL_ChangeUserRelation = 8,
	LC_ChangeUserRelation = 9,

	//Center
	CC_LoadUserRelation = 10,
	CC_LoadBeUserRelation = 11,
	//CC_LoadUserRelationFinish = 12,
	CC_AddUserRelation = 13,
	CC_DelUserRelation = 14,
	CC_ChangeUserRelation = 15,

	CC_AddBeUserRelation = 16,
	CC_DelBeUserRelation = 17,
	CC_ChangeBeUserRelation = 18,

	CC_RoleChangeOnline = 19,
	LC_RoleChangeOnline = 20,

	//
	CC_ChangeRelationLevel = 22,
	CC_ChangeRelationFaceID = 23,
	CC_ChangeRelationNickName = 24,
	CC_ChangeRelationGender = 25,
	CC_ChangeRelationTitle = 26,
	CC_ChangeRelationAchievementPoint = 27,
	CC_ChangeRelationCharmValue = 28,
	CC_ChangeRelationIP	 = 29,
	 
	//
	LC_ChangeRelationLevel = 30,
	LC_ChangeRelationFaceID = 31,
	LC_ChangeRelationNickName = 32,
	LC_ChangeRelationGender = 33,
	LC_ChangeRelationTitle = 34,
	LC_ChangeRelationAchievementPoint = 35,
	LC_ChangeRelationCharmValue = 36,
	LC_ChangeRelationIP = 37,

	CC_ChangeRelationIsShowIpAddress = 40,

	CC_ChangeRelationVipLevel		= 41,
	LC_ChangeRelationVipLevel		= 42,
	CC_ChangeRelationIsInMonthCard	= 43,
	LC_ChangeRelationIsInMonthCard	= 44,
};
struct CL_Cmd_GetUserRelation : public NetCmd
{
};
struct LC_Cmd_GetUserRelation : public NetCmd
{
	BYTE	States;
	WORD	Sum;
	tagRoleRelationClient Array[1];
};
//struct LC_Cmd_GetUserRelationFinish : public NetCmd
//{
//};
struct CL_Cmd_AddUserRelation : public NetCmd
{
	DWORD  dwDestUserID;
	BYTE	bRelationType;
};
struct LC_Cmd_AddUserRelation : public NetCmd
{
	bool	Result;
	tagRoleRelationClient RelationInfo;
};
struct CL_Cmd_DelUserRelation : public NetCmd
{
	DWORD	dwDestUserID;
};
struct LC_Cmd_DelUserRelation : public NetCmd
{
	DWORD	dwDestUserID;
	bool	Result;
};
//struct CL_Cmd_ChangeUserRelation : public NetCmd
//{
//	DWORD	dwDestUserID;
//	BYTE	RelationType;
//};
struct LC_Cmd_ChangeUserRelation : public NetCmd
{
	DWORD   dwDestUserID;
	bool	Result;
	BYTE	RelationType;
};
struct CC_Cmd_GetUserRelation : public NetCmd
{
	DWORD	dwUserID;
	BYTE	States;
	WORD	Sum;
	tagRoleRelation Array[1];
};
struct CC_Cmd_LoadBeUserRelation : public NetCmd
{
	DWORD	dwUserID;
	BYTE	States;
	WORD	Sum;
	tagBeRoleRelation Array[1];
};
//struct CC_Cmd_GetUserRelationFinish : public NetCmd
//{
//	DWORD	dwUserID;
//};
struct CC_Cmd_AddUserRelation : public NetCmd
{
	DWORD  dwUserID;
	tagRoleRelation RelationInfo;
};
struct CC_Cmd_DelUserRelation : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
};
struct CC_Cmd_ChangeUserRelation : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	RelationType;
};
struct CC_Cmd_AddBeUserRelation : public NetCmd
{
	DWORD	dwUserID;
	tagBeRoleRelation RelationInfo;
};
struct CC_Cmd_DelBeUserRelation : public NetCmd
{
	DWORD  dwUserID;
	DWORD  dwDestUserID;
};
struct CC_Cmd_ChangeBeUserRelation : public NetCmd
{
	DWORD dwUserID;
	DWORD dwDestUserID;
	BYTE  RelationType;
};
struct CC_Cmd_ChangeUserOline : public NetCmd
{
	DWORD dwSrcUserID;
	DWORD dwDestUserID;
	bool  IsOnline;
};
struct LC_Cmd_ChangeUserOline : public NetCmd
{
	//DWORD dwSrcUserID;
	DWORD dwDestUserID;
	bool  IsOnline;
};
struct CC_Cmd_ChangeRelationLevel : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	WORD	wLevel;
};
struct CC_Cmd_ChangeRelationFaceID : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	DWORD	dwFaceID;
};
struct CC_Cmd_ChangeRelationNickName : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	TCHAR	cNickName[MAX_NICKNAME + 1];
};
struct CC_Cmd_ChangeRelationGender : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	bool	bGender;
};
struct CC_Cmd_ChangeRelationTitle : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	TitleID;
};
struct CC_Cmd_ChangeRelationAchievementPoint : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	DWORD   dwAchievementPoint;
};
struct CC_Cmd_ChangeRelationCharmValue : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	DWORD	CharmInfo[MAX_CHARM_ITEMSUM];
};
struct CC_Cmd_ChangeRelationIP : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	DWORD	ClientIP;
};
struct LC_Cmd_ChangeRelationLevel : public NetCmd
{
	DWORD	dwDestUserID;
	WORD	wLevel;
};
struct LC_Cmd_ChangeRelationFaceID : public NetCmd
{
	DWORD	dwDestUserID;
	DWORD	dwFaceID;
};
struct LC_Cmd_ChangeRelationNickName : public NetCmd
{
	DWORD	dwDestUserID;
	TCHAR	cNickName[MAX_NICKNAME + 1];
};
struct LC_Cmd_ChangeRelationGender : public NetCmd
{
	DWORD	dwDestUserID;
	bool	bGender;
};
struct LC_Cmd_ChangeRelationTitle : public NetCmd
{
	DWORD	dwDestUserID;
	BYTE	TitleID;
};
struct LC_Cmd_ChangeRelationAchievementPoint : public NetCmd
{
	DWORD	dwDestUserID;
	DWORD   dwAchievementPoint;
};
struct LC_Cmd_ChangeRelationCharmValue : public NetCmd
{
	DWORD	dwDestUserID;
	DWORD	CharmInfo[MAX_CHARM_ITEMSUM];
};
struct LC_Cmd_ChangeRelationIP : public NetCmd
{
	DWORD	dwDestUserID;
	//DWORD	ClientIP; //xuda
	TCHAR			IPAddress[MAX_ADDRESS_LENGTH_IP+1];
};
struct CC_Cmd_ChangeRelationIsShowIpAddress: public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	bool	IsShowIpAddress;
};
struct CC_Cmd_ChangeRelationVipLevel : public NetCmd
{
	DWORD  dwUserID;
	DWORD  dwDestUserID;
	BYTE   VipLevel;
};
struct LC_Cmd_ChangeRelationVipLevel : public NetCmd
{
	DWORD  dwDestUserID;
	BYTE   VipLevel;
};
struct CC_Cmd_ChangeRelationIsInMonthCard : public NetCmd
{
	DWORD dwUserID;
	DWORD  dwDestUserID;
	bool  IsInMonthCard;
};
struct LC_Cmd_ChangeRelationIsInMonthCard : public NetCmd
{
	DWORD  dwDestUserID;
	bool  IsInMonthCard;
};
//-----------------------------------------------------mail
enum MailSub //ϵͳ�ʼ�Ӧ������������������з��͵� Ȼ��ͬ����GameServerȥ
{
	CL_GetUserMail = 1,
	LC_GetUserSystemMail = 2,
	LC_GetUserNormalMail = 3,
	//LC_GetUserMailFinish = 4,

	//CL_SendUserMail = 5,//�ͻ�����������˷���һ���ʼ�
	//LC_SendUserMail = 6,//�ͻ��˽��յ�һ���ʼ���Ϣ ��ʾ ����ʧ�ܻ��߳ɹ�
	//LC_SendBeUserMail = 7,

	CL_DelUserMail = 8,
	LC_DelUserMail = 9,

	CL_GetMailItem = 10,
	LC_GetMailItem = 11,

	//LC_ChangeMailIsRead = 12,

	CL_ReadMail = 13,
	LC_ReadNormalMail = 14,
	LC_ReadSystemMail = 15,
	LC_ReadMailError = 16,

	//�����ʼ������������������ת
	//CC_AddUserMail = 17,//��ҷ����ʼ� ���� ϵͳ�����ʼ���ʱ�� ����
	//CC_AddBeUserMain = 18,// ��������������յ������ʼ���ʱ�� ��������������߾�ֱ�Ӵ��� ת����ȥ

	LC_AddNormalTitle = 19,
	LC_AddSystemTitle = 20,

	CC_SendSystemMail = 21,
	CC_SendBeSystemMail = 22,

	//�����ʼ������������� 7������
	CL_SendUserMail = 23,
	GC_SendUserMail = 24,
	CG_SendUserMail = 25,
	GC_SendUserMailResult = 26,
	CG_SendUserMailResult = 27,
	LC_SendUserMailResult = 28,
};

struct CL_Cmd_GetUserMail : public NetCmd
{
	//DWORD  dwUserID;
};
struct LC_Cmd_NormalMail : public NetCmd
{
	BYTE				States;
	WORD				Sum;
	tagNormalMailTitle	Array[1];
};
struct LC_Cmd_SystemMail : public NetCmd
{
	BYTE				States;
	WORD				Sum;
	tagSystemMailTitle  Array[1];
};
//struct LC_Cmd_GetMailFinish : public NetCmd
//{
//};


struct CL_Cmd_SendUserMail : public NetCmd
{
	DWORD				DestUserID;//���ͷ���ID
	TCHAR				Context[MAX_MAIL_CONTEXT + 1];//�ʼ�����
};
struct GC_Cmd_SendUserMail : public NetCmd
{
	DWORD				DestUserID;
	tagRoleMail			MailInfo;
};
struct CG_Cmd_SendUserMail : public NetCmd
{
	DWORD				DestUserID;
	tagRoleMail			MailInfo;
};
struct GC_Cmd_SendUserMailResult : public NetCmd
{
	DWORD				SrcUserID;
	DWORD				DestUserID;
	bool				Result;
};
struct CG_Cmd_SendUserMailResult : public NetCmd
{
	DWORD				SrcUserID;
	DWORD				DestUserID;
	bool				Result;
};
struct LC_Cmd_SendUserMailResult : public NetCmd
{
	DWORD				DestUserID;
	bool				Result;
};


//struct LC_Cmd_SendBeNormalMail
//{
//	DWORD				dwUserID;
//	tagNormalMailTitle	MailInfo;
//};

struct CL_Cmd_DelMail : public NetCmd
{
	DWORD				dwMailID;
};
struct LC_Cmd_DelMail : public NetCmd
{
	DWORD				dwMailID;
	bool				Result;
};
struct CL_Cmd_GetMailItem : public NetCmd
{
	DWORD				dwMailID;
};
struct LC_Cmd_GetMailItem : public NetCmd
{
	DWORD				dwMailID;
	bool				Result;
};
//struct LC_Cmd_ChangeMailIsRead
//{
//	DWORD				dwUserID;
//	DWORD				dwMailID;
//};
struct CL_Cmd_GetMailContext : public NetCmd
{
	DWORD				dwMailID;
};
struct LC_Cmd_GetNormalMail : public NetCmd
{
	tagNormalMail		MailInfo;
};
struct LC_Cmd_GetSystemMail : public NetCmd
{
	//DWORD				dwUserID;
	tagSystemMail		MailInfo;
};
struct LC_Cmd_GetMailError : public NetCmd
{
	//DWORD				dwUserID;
	DWORD				MailID;
};
//struct CC_Cmd_AddUserMail : public NetCmd
//{
//	DWORD				dwUserID;
//	tagRoleMail			MailInfo;
//};
//struct CC_Cmd_AddBeUserMail : public NetCmd
//{
//	DWORD				dwDestRoleID;
//	tagRoleMail			MailInfo;
//};
struct LC_Cmd_AddNormalMail : public NetCmd
{
	tagNormalMailTitle	MailInfo;
};
struct LC_Cmd_AddSystemMail : public NetCmd
{
	tagSystemMailTitle	MailInfo;
};
struct CC_Cmd_SendSystemMail : public NetCmd
{
	DWORD				dwDestUserID;
	tagRoleMail			MailInfo;
};
struct CC_Cmd_SendBeSystemMail : public NetCmd
{
	DWORD				dwDestUserID;
	tagRoleMail			MailInfo;
};
//Role---------------------------------------------------------------
enum RoleSub
{
	LC_ChangeRoleExp = 1,
	LC_ChangeRoleLevel = 2,
	LC_ChangeRoleGender = 3,
	LC_ChangeRoleNickName = 4,
	LC_ChangeRoleFaceID = 5,
	LC_ChangeRoleGlobe = 6,
	LC_ChangeRoleMedal = 7,
	LC_ChangeRoleCurrency = 8,
	LC_ChangeRoleTitle = 9,
	LC_ChangeRoleAchievementPoint = 10,
	LC_ChangeRoleCharmValue = 11,
	LC_ChangeRoleSendGiffSum = 12,
	LC_ChangeRoleAcceptGiffSum = 13,
	LC_ChangeRoleTaskStates = 14,
	LC_ChangeRoleAchievementStates = 15,
	LC_ChangeRoleActionStates = 16,
	LC_ChangeRoleIsShop	= 17,

	//������ ���������͵�
	//CC_ChangeRoleExp = 20,
	CC_ChangeRoleLevel = 21,
	CC_ChangeRoleGender = 22,
	CC_ChangeRoleNickName = 23,
	CC_ChangeRoleFaceID = 24,
	CC_ChangeRoleTitle = 25,
	CC_ChangeRoleAchievementPoint = 26,
	CC_ChangeRoleCharmValue = 27,
	CC_ChangeRoleClientIP = 28,
	/*CC_ChangeRoleGlobe = 25,
	CC_ChangeRoleMedal = 26,
	CC_ChangeRoleCurrency = 27,*/

	//�ͻ������������ �����޸ĵ�����
	CL_ChangeRoleGender = 40,
	CL_ChangeRoleNickName = 41,
	CL_ChangeRoleNormalFaceID = 42,
	//ͷ����޸� ��Ҫע��� 
	//CL_RequestUserFaceID = 43,//�������ͷ��
	//LC_ResponseUserFaceID = 44,
	//CL_UpLoadingchunk = 45,
	//LC_UpLoadingResponse = 46,
	//LC_UpLoadingFinish = 47,
	//LC_UpLoadFtpError = 48,//�޸�ͷ��ʧ����

	LC_DayChange = 49,

	GF_SaveImageData = 50,
	FG_SaveImageData = 51,

	LC_ChangeRoleAchievementIndex = 52,

	LC_ResetRoleGlobel = 53,


	CL_ResetRoleInfo = 54,
	LC_ResetRoleInfo = 55,

	LC_ChangeRoleCheckData = 56,

	CL_ChangeRoleIsShowIpAddress = 57,
	LC_ChangeRoleIsShowIpAddress = 58,
	CC_ChangeRoleIsShowIpAddress = 59,

	LC_NewDay = 60,

	CC_ChangeRoleIsOnline = 61,

	LC_ChangeRoleExChangeStates = 62,
	LC_ChangeRoleTotalRecharge = 63,

	LC_RoleProtect  = 64,

	LC_ChangeRoleIsFirstPayGlobel = 65,
	LC_ChangeRoleIsFirstPayCurrcey = 66,

	LC_ChangeRoleMonthCardInfo	= 67,
	LC_ChangeRoleRateValue		= 68,
	CL_ChangeRoleRateValue	    = 69,

	CL_GetMonthCardReward	    = 70,
	LC_GetMonthCardReward       = 71,

	LC_ChangeRoleVipLevel       = 72,

	CC_ChangeRoleVipLevel       = 73,
	CC_ChangeRoleIsInMonthCard  = 74,

	LC_ChangeRoleCashSum		= 75,
	CL_RoleProtect = 76,
	LC_RoleProtectStatus = 77,

	CC_ChangeRoleParticularStates = 78,
	LC_ChangeRoleParticularStates = 79,

	//LC_ChangeRoleLockInfo = 78,
	LC_SetSecondPassword		= 80,

	CL_ChangeSecondPassword     = 81,
	LC_ChangeSecondPassword     = 82,

	CL_ChangeRoleShareStates	= 83,
	LC_ChangeRoleShareStates    = 84,

	LC_OpenShareUI				= 85,//��������

	//�����˺�
	CL_UpdateAccount			= 86,
	LO_UpdateAccount			= 87,
	OL_UpdateAccount			= 88,

	LC_ChangeRoleZPNum			= 89,
	

	//�������ת��
	CL_OpenZP					= 90,
	LC_OpenZP					= 91,

	//
	CL_UpdatePay				= 100,
	LC_UpdatePay				= 101,

	LC_ChangeRoleExchangeNum	= 111 ,
};
struct LC_Cmd_ChangRoleExp : public NetCmd
{
	//DWORD		dwUserID;
	DWORD		dwExp;
};
struct LC_Cmd_ChangeRoleLevel : public NetCmd
{
	//DWORD		dwUserID;
	WORD		wLevel;
};
struct LC_Cmd_ChangeRoleGender : public NetCmd
{
	//DWORD		dwUserID;
	bool		bGender;
};
struct LC_Cmd_ChangeRoleNickName : public NetCmd
{
	//DWORD		dwUserID;
	bool		Result;
	TCHAR		NickName[MAX_NICKNAME + 1];
};
struct LC_Cmd_ChangeRoleFaceID : public NetCmd
{
	//DWORD		dwUserID;
	DWORD		dwFaceID;
};
struct LC_Cmd_ChangeRoleGlobe : public NetCmd
{
	//DWORD		dwUserID;
	int		dwGlobeNum;
};
struct LC_Cmd_ChangeRoleMedal : public NetCmd
{
	//DWORD		dwUserID;
	DWORD       dwMedalNum;
	DWORD		TotalUseMedal;
};
struct LC_Cmd_ChangeRoleCurrency : public NetCmd
{
	//DWORD		dwUserID;
	DWORD		dwCurrencyNum;
};
struct LC_Cmd_ChangeRoleTitle : public NetCmd
{
	BYTE		TitleID;
};
struct LC_Cmd_ChangeRoleAchievementPoint : public NetCmd
{
	DWORD		dwAchievementPoint;
};
struct LC_Cmd_ChangeRoleCharmValue : public NetCmd
{
	DWORD		CharmInfo[MAX_CHARM_ITEMSUM];
};
struct LC_Cmd_ChangeRoleSendGiffSum : public NetCmd
{
	BYTE	SendGiffSum;
};
struct LC_Cmd_ChangeRoleAcceptGiffSum : public NetCmd
{
	BYTE	AcceptGiffSum;
};
struct LC_Cmd_ChangeRoleTaskStates : public NetCmd
{
	BYTE	Index;
	bool	States;
};
struct LC_Cmd_ChangeRoleAchievementStates : public NetCmd
{
	BYTE	Index;
	bool	States;
};
struct LC_Cmd_ChangeRoleActionStates : public NetCmd
{
	BYTE	Index;
	bool	States;
};
struct LC_Cmd_ChangeRoleIsShop : public NetCmd
{
	bool	bShop;
};
//struct CC_Cmd_ChangRoleExp : public NetCmd
//{
//	DWORD		dwUserID;
//	DWORD		dwExp;
//};
struct CC_Cmd_ChangeRoleLevel : public NetCmd
{
	DWORD		dwUserID;
	WORD		wLevel;
};
struct CC_Cmd_ChangeRoleGender : public NetCmd
{
	DWORD		dwUserID;
	bool		bGender;
};
struct CC_Cmd_ChangeRoleNickName : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		NickName[MAX_NICKNAME + 1];
};
struct CC_Cmd_ChangeRoleFaceID : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwFaceID;
};
struct CC_Cmd_ChangeRoleTitle : public NetCmd
{
	DWORD		dwUserID;
	BYTE		TitleID;
};
struct CC_Cmd_ChangeRoleAchievementPoint : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwAchievementPoint;
};
struct CC_Cmd_ChangeRoleCharmValue : public NetCmd
{
	DWORD		dwUserID;
	DWORD		CharmInfo[MAX_CHARM_ITEMSUM];
};
struct CC_Cmd_ChangeRoleClientIP : public NetCmd
{
	DWORD		dwUserID;
	DWORD		ClientIP;
};

struct LC_Cmd_ChangeRoleAchievementIndex : public NetCmd
{
	DWORD		AchievementIndex;
};
//struct CC_Cmd_ChangeRoleGlobe
//{
//	DWORD		dwUserID;
//	DWORD		dwGlobeNum;
//};
//struct CC_Cmd_ChangeRoleMedal
//{
//	DWORD		dwUserID;
//	DWORD       dwMedalNum;
//};
//struct CC_Cmd_ChangeRoleCurrency
//{
//	DWORD		dwUserID;
//	DWORD		dwCurrencyNum;
//};
struct CL_Cmd_ChangeRoleGender : public NetCmd
{
	//��������޸��Ա�
	//DWORD		dwUserID;
	bool		bGender;
};
struct CL_Cmd_ChangeRoleNickName : public NetCmd
{
	//DWORD		dwUserID;
	TCHAR		NickName[MAX_NICKNAME + 1];
};
struct CL_Cmd_ChangeRoleNormalFaceID : public NetCmd
{
	//DWORD		dwUserID;
	DWORD		dwFaceID;
};

//struct CL_Cmd_RequestUserFaceID : public NetCmd
//{
//	//�ͻ���������� �ϴ�ͷ��
//	//DWORD		dwUserID;
//	WORD		Size;//�����ϴ���ͷ���С
//};
//struct LC_Cmd_ResponseUserFaceID : public NetCmd
//{
//	//��Ӧ�ͻ��˵�����
//	BYTE		AllowUpload;//�Ƿ�����ϴ�
//};
//struct CL_Cmd_UpLoadingchunk : public NetCmd//�䳤����
//{
//	WORD		StartIndex;
//	WORD		Size;
//	BYTE		ImgData[1];
//};
//struct LC_Cmd_UpLoadingResponse : public NetCmd
//{
//	BYTE		UpdateResult;
//};
//struct LC_Cmd_UpLoadingFinish : public NetCmd
//{
//	//DWORD		dwUserID;
//	DWORD		dwFaceID;
//};
struct LC_Cmd_DayChange : public NetCmd
{
	BYTE			Year;
	BYTE			Month;
	BYTE			Day;
	BYTE			Hour;
	BYTE			Min;
	BYTE			Sec;
	bool			IsDayUpdate;
};
struct GF_Cmd_SaveImageData : public NetCmd
{
	USHORT		Size;
	UINT_PTR	ID;
	UINT		ImgCrc;
	USHORT		ImgSize;
	char		ImgName[50];
	BYTE		ImgData[1];
};
struct FG_Cmd_SaveImageData : public NetCmd
{
	USHORT		Size;
	UINT_PTR	ID;
	UINT		Crc;
	UINT		Result;
};
struct LC_Cmd_ResetRoleGlobel : public NetCmd
{
	DWORD		dwTotalGlobel;
	DWORD		dwLotteryScore;
};
struct LC_Cmd_ResetRoleInfo : public NetCmd
{
	DWORD		  OperateIP;
	tagRoleInfo   RoleInfo;
};
struct LC_Cmd_ChangeRoleCheckData : public NetCmd
{
	DWORD			CheckData;
};
struct CL_Cmd_ChangeRoleIsShowIpAddress : public NetCmd
{
	bool			IsShowIpAddress;
};
struct LC_Cmd_ChangeRoleIsShowIpAddress : public NetCmd
{
	bool			IsShowIpAddress;
};
struct CC_Cmd_ChangeRoleIsShowIpAddress : public NetCmd
{
	DWORD			dwUserID;
	bool			IsShowIpAddress;
};
struct CC_Cmd_ChangeRoleIsOnline : public NetCmd
{
	DWORD			dwUserID;
	bool			IsOnline;
};
struct LC_Cmd_ChangeRoleExChangeStates : public NetCmd
{
	DWORD			States;
};
struct LC_Cmd_ChangeRoleTotalRecharge : public NetCmd
{
	DWORD			Sum;
};
struct LC_Cmd_ChangeRoleIsFirstPayGlobel : public NetCmd
{

};
struct LC_Cmd_ChangeRoleIsFirstPayCurrcey : public NetCmd
{

};
struct LC_Cmd_RoleProtect : public NetCmd
{

};

struct LC_Cmd_RoleProtectStatus : public NetCmd
{
	//BYTE setat;
	//bool poor;
};
struct CC_Cmd_ChangeRoleParticularStates : public NetCmd
{
	DWORD		dwUserID;
	DWORD		ParticularStates;
};
struct LC_Cmd_ChangeRoleParticularStates : public NetCmd
{
	DWORD		ParticularStates;
};
struct CL_Cmd_RoleProtect : public NetCmd
{

};

struct LC_Cmd_ChangeRoleMonthCardInfo : public NetCmd
{
	BYTE		MonthCardID;
	__int64     MonthCardEndTime;
};
struct CL_Cmd_ChangeRoleRateValue : public NetCmd
{
	BYTE		RateIndex;//��Ҫ�����ı���
};
struct LC_Cmd_ChangeRoleRateValue : public NetCmd
{
	int256		RateValue;
	BYTE		OpenRateIndex;
};
struct CL_Cmd_GetMonthCardReward : public NetCmd
{

};
struct LC_Cmd_GetMonthCardReward : public NetCmd
{
	bool	Result;
};
struct LC_Cmd_ChangeRoleVipLevel : public NetCmd
{
	BYTE	VipLevel;
};
struct CC_Cmd_ChangeRoleVipLevel : public NetCmd
{
	DWORD  dwUserID;
	BYTE   VipLevel;
};
struct CC_Cmd_ChangeRoleIsInMonthCard : public NetCmd
{
	DWORD  dwUserID;
	bool   IsInMonthCard;
};
struct LC_Cmd_ChangeRoleCashSum : public NetCmd
{
	BYTE	CashSum;
	DWORD   TotalCashSum;
};

struct CL_Cmd_ChangeRoleShareStates : public NetCmd
{
	bool  ShareStates;
};
struct LC_Cmd_ChangeRoleShareStates : public NetCmd
{
	bool  ShareStates;
};

struct LC_Cmd_OpenShareUI : public NetCmd
{

};


struct AccountSDKInfo
{
	//CHAR			Token/OpenID
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};

struct CL_Cmd_UpdateAccount	 : public NetCmd
{
	BYTE			bType;
	AccountSDKInfo  Info;
};
struct LO_Cmd_UpdateAccount	 : public NetCmd
{
	DWORD			dwUserID;
	BYTE			bType;
	AccountSDKInfo  Info;
};
struct OL_Cmd_UpdateAccount	 : public NetCmd
{
	DWORD			dwUserID;
	bool			bGender;
	TCHAR			NickName[MAX_NICKNAME + 1];
	DWORD			dwFaceID;
};
struct LC_Cmd_ChangeRoleZPNum : public NetCmd
{
	DWORD			dwZpNum;
};
struct LC_Cmd_ChangeRoleExchangeNum : public NetCmd
{
	DWORD			dwExchangeNum;
};
struct CL_Cmd_OpenZP : public NetCmd
{

};
struct LC_Cmd_OpenZP : public NetCmd
{
	bool		Result;
	DWORD		Index;
	WORD		RewardID;
};
struct CL_Cmd_UpdatePay : public NetCmd
{

};
struct LC_Cmd_UpdatePay : public NetCmd
{
	DWORD		dwCurrcey;
	DWORD		dwGold;
	BYTE		MonthCardID;
	__int64		MonthCardEndTime;
};
/*struct LC_Cmd_ChangeRoleLockInfo : public NetCmd
{
	DWORD	LockStates;
	__int64 LockEndTime;
};*/
struct LC_Cmd_SetSecondPassword : public NetCmd
{
	//����Ҳ�ѯ���ö�������
};
struct CL_Cmd_ChangeSecondPassword : public NetCmd
{
	DWORD	dwOldCrc1;
	DWORD	dwOldCrc2;
	DWORD	dwOldCrc3;

	DWORD	dwNewCrc1;
	DWORD	dwNewCrc2;
	DWORD	dwNewCrc3;
};
struct LC_Cmd_ChangeSecondPassword : public NetCmd
{
	bool	Result;
};
//-------------------------------------------------------------------------��Ҳ�ѯ
enum QuerySub
{
	//��ѯ����
	CL_QueryUserByNickName = 1,//�������ƽ���ģ����ѯ ��ҳ��ѯ
	CL_QueryUserByUserID = 2,
	CL_QueryUserByGameID = 3,

	LC_QueryUserInfo = 10,
	//LC_QueryUserInfoFinish = 11,

	LC_QueryUserOnce = 12,
};
struct CL_Cmd_QueryUserByNickName : public NetCmd
{
	TCHAR		NickName[MAX_NICKNAME + 1];
	BYTE		Page;//ҳ����
	bool		IsOnline;//�Ƿ��ѯ�����ߵ���� (���Խ��п��Ƶ�)
};
struct CL_Cmd_QueryUserByUserID : public NetCmd
{
	DWORD		dwDestUserID;
};
struct CL_Cmd_QueryUserByGameID : public NetCmd
{
	DWORD		GameID;
};
struct LC_Cmd_QueryUserInfo : public NetCmd
{
	WORD		Page;
	BYTE		States;
	WORD		Sum;
	tagQueryRoleInfoClient  Array[1];
};
struct LC_Cmd_QueryUserOnce : public NetCmd
{
	tagQueryRoleInfoClient  RoleInfo;
};
//struct LC_Cmd_QueryUserInfoFinish
//{
//
//};

//-------------------------------------------------------------------check
enum CheckSub
{
	/*CL_GetRoleCheckInfo = 1,
	LC_GetRoleCheckInfo = 2,*/

	CL_CheckNowDay = 4,
	LC_CheckNowDay = 5,

	CL_CheckOtherDay = 6,
	LC_CheckOtherDay = 7,

	//LC_CheckChange = 8,
};
struct LC_Cmd_CheckNowDay : public NetCmd
{
	bool		Result;
	WORD		DayRewardID;
	WORD		MonthRewardID;
};
struct CL_Cmd_CheckOtherDay : public NetCmd
{
	BYTE	   Day;
};
struct LC_Cmd_CheckOtherDay : public NetCmd
{
	bool       Result;
	BYTE	   Day;
	WORD	   DayRewardID;
	WORD	   MonthRewardID;
};
//--------------------------------------------------------------------Task
enum TaskSub
{
	CL_GetRoleTaskInfo = 1,
	LC_GetRoleTaskInfo = 2,
	//LC_GetRoleTaskInfoFinish = 3,

	LC_TaskAllEventFinish = 5,

	CL_GetTaskReward = 6,
	LC_GetTaskReward = 7,

	//LC_ResetTask  = 8,

	CL_GetOnceTaskInfo = 9,
	LC_GetOnceTaskInfo = 10,


	CG_GetGlobelTaskInfo = 11,

	LC_JoinTask = 12,
	LC_ClearTask = 13,
	LC_DelTask = 14,

};
struct CL_Cmd_GetRoleTaskInfo : public NetCmd
{
	
};
struct LC_Cmd_GetRoleTaskInfo : public NetCmd
{
	BYTE			States;
	WORD			Sum;
	tagRoleTaskInfo Array[1];
};
//struct LC_Cmd_GetRoleTaskInfoFinish : public NetCmd
//{
//
//};
struct LC_Cmd_TaskAllEventFinish : public NetCmd
{
	BYTE			TaskID;
};
struct CL_Cmd_GetTaskReward : public NetCmd
{
	BYTE			TaskID;
};
struct LC_Cmd_GetTaskReward : public NetCmd
{
	BYTE	TaskID;
};
struct CL_Cmd_GetOnceTaskInfo : public NetCmd
{
	BYTE	TaskID;
};
struct LC_Cmd_GetOnceTaskInfo : public NetCmd
{
	tagRoleTaskInfo		TaskInfo;
};
struct CG_Cmd_GetGlobelTaskInfo : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	BYTE		Array[1];
};
struct LC_Cmd_JoinTask : public NetCmd
{
	BYTE		TaskID;//��ȡ��������
};
struct LC_Cmd_DelTask : public NetCmd
{
	BYTE		TaskID;//��ɾ��������
};
//Achievement --------------------------------------------------------------------
enum AchievementSub
{
	CL_GetRoleAchievementInfo = 1,
	LC_GetRoleAchievementInfo = 2,
	//LC_GetRoleAchievementInfoFinish = 3,

	LC_AchievementAllEventFinish = 5,

	CL_GetAchievementReward = 6,
	LC_GetAchievementReward = 7,

	CL_GetOnceAchievementInfo = 9,
	LC_GetOnceAchievementInfo = 10,

	LC_JoinAchievement = 11,
	LC_DelAchievement = 12,

	LC_AchievementList = 13,
};
struct CL_Cmd_GetRoleAchievementInfo : public NetCmd
{

};
struct LC_Cmd_GetRoleAchievementInfo : public NetCmd
{
	BYTE			States;
	WORD			Sum;
	tagRoleAchievementInfo Array[1];
};
struct LC_Cmd_GetRoleAchievementInfoFinish : public NetCmd
{

};
struct LC_Cmd_AchievementAllEventFinish : public NetCmd
{
	BYTE			AchievementID;
};
struct CL_Cmd_GetAchievementReward : public NetCmd
{
	BYTE			AchievementID;
};
struct LC_Cmd_GetAchievementReward : public NetCmd
{
	BYTE			AchievementID;
};
struct CL_Cmd_GetOnceAchievementInfo : public NetCmd
{
	BYTE	AchievementID;
};
struct LC_Cmd_GetOnceAchievementInfo : public NetCmd
{
	tagRoleAchievementInfo		AchievementInfo;
};
struct LC_Cmd_JoinAchievement : public NetCmd
{
	BYTE		AchievementID;
};
struct LC_Cmd_DelAchievement : public NetCmd
{
	BYTE		AchievementID;
};
struct LC_Cmd_AchievementList : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	DWORD		Array[1];
};
//�ƺ�--------------------------------------------
enum TitleSub
{
	CL_LoadRoleTitle = 1,
	LC_LoadRoleTitle = 2,
	//LC_LoadRoleTitleFinish = 3,

	LC_AddRoleTitle = 4,
	LC_DelRoleTitle = 5,

	CL_ChangeRoleCurrTitle = 6,
};
struct CL_Cmd_LoadRoleTitle : public NetCmd
{

};
struct LC_Cmd_LoadRoleTitle : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	BYTE		Array[1];
};
struct LC_Cmd_LoadRoleTitleFinish : public NetCmd
{

};
struct LC_Cmd_AddRoleTitle : public NetCmd
{
	BYTE		AddTitleID;
};
struct LC_Cmd_DelRoleTitle : public NetCmd
{
	BYTE		DelTitleID;
};
struct CL_Cmd_ChangeRoleCurrTitle : public NetCmd
{
	BYTE		TitleID;
};
//---------------------------Month-----------------
enum MonthSub
{
	//����
	CL_SignUpMonth = 1,//�������������
	GC_SignUpMonth = 2,//ת�������������
	CG_SignUpMonth = 3,//���������ѯ����Ϸ����ǰ��
	GC_SignUpMonthResult = 4,//��Ϸ����������Ʒɾ���Ĳ��� ���ҳɹ�����������������
	CG_SignUpMonthSucess = 5,
	LC_SignUpMonthSucess = 6,
	LC_SignUpMonthFiled = 7,
	//�μӱ���
	CL_JoinMonth = 8,
	GC_JoinMonth = 9,
	CG_JoinMonth = 10,
	LC_JoinMonthSucess = 11,
	LC_JoinMonthFiled = 12,
	//��һ��� ������ҵı仯
	/*GC_ChangeUserPoint = 13,
	GC_ChangeUserGlobel = 14,*/
	GC_ChangeUserMonthInfo = 13,

	CL_ChangeUserAddMonthGlobelNum = 15,//�����������
	GC_ChangeUserAddMonthGlobelNum = 16,//��ҽ������Ҳ���
	LC_ChangeUserAddMonthGlobelNum = 17,//

	//�����Center�������仯��ʱ�� ������δ���?
	CG_UserChangeIndex = 18,
	LC_UserChangeIndex = 19,

	CG_UserMonthEnd = 20,
	LC_UserMonthEnd = 21,

	//
	GC_LoadRoleSignUp = 30,
	CG_LoadRoleSignUp = 31,
	CL_LoadRoleSignUpInfo = 32,
	LC_LoadRoleSignUpInfo = 33,

	//CG_SendMonthRoleSumBegin = 34,//����
	CG_SendMonthRoleSum = 35,
	//CG_SendMonthRoleSumFinish = 36,//����
	//CL_GetMonthRoleSum = 37,
	LC_GetMonthRoleSum = 38,
	//LC_GetMonthRoleSumFinish = 39,

	CL_ResetMonthInfo = 40,
	GC_ResetMonthInfo = 41,
	CG_ResetMonthInfo = 42,
	LC_ResetMonthInfo = 43,

	LC_SetRoleMonthInfo = 44,

	LC_ResetRoleMonthInfo = 45,

	CG_GetUserMonthInfo = 46,
	GC_GetUserMonthInfoFinish = 47,

	/*GC_LogonToMonthTable = 50,
	CG_LogonToMonthTable = 51,*/

	CG_MonthList		= 48,
	LC_MonthList		= 49,

	CG_MonthStates      = 50,
};
struct CL_Cmd_SignUpMonth : public NetCmd
{
	BYTE		MonthID;
};
struct GC_Cmd_SignUpMonth : public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
};
struct CG_Cmd_SignUpMonth : public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
	bool		Result;
};
struct LC_Cmd_SignUpMonthFiled : public NetCmd
{
	BYTE		MonthID;
};
struct GC_Cmd_SignUpMonthResult : public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
};
struct CG_Cmd_SignUpMonthSucess : public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
};
struct LC_Cmd_SignUpMonthSucess : public NetCmd
{
	BYTE		MonthID;
};
struct CL_Cmd_JoinMonth : public NetCmd
{
	BYTE		MonthID;
};
struct GC_Cmd_JoinMonth : public NetCmd
{
	DWORD		dwUserID;
	//BYTE		TableTypeID;
	BYTE		MonthID;
	WORD		TableID;
};
struct CG_Cmd_JoinMonth : public NetCmd
{
	bool		Result;
	tagRoleMonthInfo MonthInfo;
	WORD		TableID;
};
struct LC_Cmd_JoinMonthSucess : public NetCmd
{
	tagRoleMonthInfo MonthInfo;
};
struct LC_Cmd_ChangeUserAddMonthGlobelNum : public NetCmd
{
	DWORD			dwUserID;
};
//struct LC_Cmd_JoinMonthFiled
//{
//};
//struct GC_Cmd_ChangeUserPoint : public NetCmd
//{
//	BYTE		MonthID;
//	DWORD		dwUserID;
//	DWORD		dwPoint;
//};
//struct GC_Cmd_ChangeUserGlobel : public NetCmd
//{
//	BYTE		MonthID;
//	DWORD		dwUserID;
//	DWORD		dwMonthGlobel;
//};
struct GC_Cmd_ChangeUserMonthInfo : public NetCmd
{
	bool		IsEnd;
	BYTE		MonthID;
	DWORD		dwUserID;
	DWORD		dwMonthGlobel;
	DWORD		dwPoint;
	DWORD		SkillSum;
};
struct CG_Cmd_GetUserMonthInfo : public NetCmd
{
	BYTE		MonthID;
};
struct GC_Cmd_GetUserMonthInfoFinish : public NetCmd
{
	BYTE		MonthID;
};
struct tagMonthRoleInfo
{
	DWORD		dwUserID;
	DWORD		Index;
	TCHAR		NickName[MAX_NICKNAME+1];
	DWORD		Score;
	DWORD		VipScore;
	DWORD		FaceID;
};
struct CG_Cmd_MonthList : public NetCmd
{
	BYTE					MonthID;
	//MAX_MonthList_Length
	tagMonthRoleInfo		MonthInfo[MAX_MonthList_Length];
};
struct LC_Cmd_MonthList : public NetCmd
{
	BYTE					MonthID;
	//MAX_MonthList_Length
	tagMonthRoleInfo		MonthInfo[MAX_MonthList_Length];
};

//struct CL_Cmd_ChangeUserAddMonthGlobelNum
//{
//	
//};
struct GC_Cmd_ChangeUserAddMonthGlobelNum : public NetCmd
{
	DWORD			dwUserID;
	BYTE			Month;
};
//struct LC_Cmd_ChangeUserAddMonthGlobelNum
//{
//
//};
struct CG_Cmd_UserChangeIndex : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwIndex;
	DWORD	dwUpperSocre;
};
struct LC_Cmd_UserChangeIndex : public NetCmd
{
	DWORD   dwUserID;
	DWORD	dwIndex;
	DWORD	dwUpperSocre;
};
struct CG_Cmd_UserMonthEnd : public NetCmd
{
	DWORD		dwUserID;
	BYTE		MonthID;
	DWORD		MonthIndex;
	DWORD		MonthScores;
	DWORD		VipScores;
};
struct LC_Cmd_UserMonthEnd : public NetCmd
{
	BYTE		MonthID;
	DWORD		MonthIndex;
	DWORD		MonthScores;
	DWORD		VipScores;
};
struct GC_Cmd_LoadRoleSignUp : public NetCmd
{
	DWORD dwUserID;
};
struct CG_Cmd_LoadRoleSignUp : public NetCmd
{
	DWORD dwUserID;
	BYTE   States;
	WORD   Sum;
	BYTE   Array[1];
};
//struct CL_Cmd_LoadRoleSignUpInfo
//{
//
//};
struct LC_Cmd_LoadRoleSignUpInfo : public NetCmd
{
	BYTE   States;
	WORD   Sum;
	BYTE   Array[1];
};
struct tagMonthRoleSum
{
	BYTE  MonthID;
	WORD  MonthUserSum;
};
struct CG_Cmd_SendMonthRoleSum : public NetCmd
{
	BYTE	States;
	WORD	Sum;
	tagMonthRoleSum Array[1];
};
struct LC_Cmd_GetMonthRoleSum : public NetCmd
{
	BYTE	States;
	WORD	Sum;
	tagMonthRoleSum Array[1];
};
struct CL_Cmd_ResetMonthInfo : public NetCmd
{
	BYTE	MonthID;
};
struct GC_Cmd_ResetMonthInfo : public NetCmd
{
	DWORD	dwUserID;
	BYTE	MonthID;
};
struct CG_Cmd_ResetMonthInfo : public NetCmd
{
	DWORD	dwUserID;
	BYTE	MonthID;
	bool	Result;
};
struct LC_Cmd_ResetMonthInfo : public NetCmd
{
	DWORD	dwUserID;
	BYTE	MonthID;
	bool	Result;
};
struct LC_Cmd_SetRoleMonthInfo : public NetCmd
{
	DWORD dwUserID;
	BYTE  MonthID;
	int MonthSocre;
	int MonthGlobel;
};
struct LC_Cmd_ResetRoleMonthInfo : public NetCmd
{
	tagRoleMonthInfo MonthInfo;
};
struct CG_Cmd_MonthStates : public NetCmd
{
	BYTE		MonthID;
	BYTE		MonthStates;//1 ������ʼ  2����δ��ʼ  4������ʼ 8 ����δ��ʼ
};
//struct GC_Cmd_LogonToMonthTable : public NetCmd
//{
//	DWORD			dwUserID;
//	BYTE			MonthID;
//};
//struct CG_Cmd_LogonToMonthTable : public NetCmd
//{
//	DWORD			dwUserID;
//	BYTE			MonthID;
//	bool			Result;
//	tagRoleMonthInfo MonthInfo;
//};
//���а� Rank --------------------------------------------
enum RankSub
{
	CL_GetRankReward = 1,
	/*LR_GetRankReward = 2,
	RL_GetRankReward = 3,*/
	LC_GetRankReward = 4,

	//��ȡ���а���ȡ��������̬
	CL_GetRankInfo = 5,
	/*LR_GetRankInfo = 6,
	RL_GetWeekRankInfo = 7,
	RL_GetRankInfoFinish = 8,*/
	LC_GetWeekRankInfo = 9,
	//LC_GetRankInfoFinish = 10,

};
struct CL_Cmd_GetRankReward : public NetCmd
{
	BYTE		RankID;
};
//struct LR_Cmd_GetRankReward : public NetCmd
//{
//	DWORD		dwUserID;
//	BYTE		RankID;
//};
//struct RL_Cmd_GetRankReward : public NetCmd
//{
//	DWORD		dwUserID;
//	BYTE		RankID;
//	BYTE		Index;
//};
struct LC_Cmd_GetRankReward : public NetCmd
{
	BYTE		RankID;
};
//struct LR_Cmd_GetRankInfo : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct RL_Cmd_GetWeekRankInfo : public NetCmd
//{
//	//��ȡ��ҵ���������
//	DWORD		dwUserID;
//	BYTE		RankSum;
//	tagRankWeekReward RankArray[1];
//};
//struct RL_Cmd_GetRankInfoFinish : public NetCmd
//{
//	DWORD		dwUserID;
//};
struct LC_Cmd_GetWeekRankInfo : public NetCmd
{
	BYTE					States;
	WORD					Sum;
	tagRankWeekRankClient	Array[1];
};
//Chest ----------------------------------------------------------------------
enum ChestSub
{
	//LC_AddChest = 1,
	LC_ActionChest = 2,
	LC_ChestEnd = 3,

	CL_GetChestReward = 4,
	LC_GetChestReward = 5,

	CL_CloseChest = 6,

	LC_ResetChest = 7,
};
struct ChestOnceStates
{
	BYTE	Index;
	BYTE	RewardID;
	BYTE	RewardOnlyID;
};
struct LC_Cmd_ActionChest : public NetCmd
{
	bool			IsReset;
	BYTE			ChestOnlyID;
	BYTE			ChestTypeID;
	BYTE			ChestSum;
	BYTE			OpenSum;
	DWORD			EndSec;
	ChestOnceStates	ChestArray[1];
};
struct LC_Cmd_ChestEnd : public NetCmd
{
	BYTE			ChestOnlyID;
};
struct CL_Cmd_GetChestReward : public NetCmd
{
	BYTE			ChestOnlyID;
	BYTE			ChestIndex;//������λ������ 
};
struct LC_Cmd_GetChestReward : public NetCmd
{
	bool		Result;
	BYTE		ChestOnlyID;
	ChestOnceStates ChestStates;
};
struct CL_Cmd_CloseChest : public NetCmd
{
	BYTE			ChestOnlyID;
};
struct LC_Cmd_ResetChest : public NetCmd
{
};
//Charm
enum CharmSub
{
	CL_ChangeOtherUserCharm = 1,
	CC_ChangeOtherUserCharm = 2,
	CC_ChangeRoleCharmResult = 3,
	CC_ChangeUserCharm = 4,
	LC_ChangeRoleCharmResult = 5,

	LC_TableRoleCharmInfo = 6,
};
struct CL_Cmd_ChangeOtherUserCharm : public NetCmd
{
	DWORD		dwDestUserID;
	BYTE		CharmIndex;
	DWORD		ItemSum;
};
struct CC_Cmd_ChangeOtherUserCharm : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwDestUserID;
	BYTE		CharmIndex;
	DWORD		ItemSum;
};
struct CC_Cmd_ChangeRoleCharmResult : public NetCmd
{
	DWORD		dwUserID;
	BYTE		CharmIndex;
	DWORD		ItemSum;
	bool		Result;
	DWORD		dwDestUserID;
	int			dwDestUserCharmValue;
};
struct CC_Cmd_ChangeUserCharm : public NetCmd
{
	DWORD		dwUserID;
	BYTE		CharmIndex;
	DWORD		ItemSum;
};
struct LC_Cmd_ChangeRoleCharmResult : public NetCmd
{
	bool		Result;
	DWORD		dwDestUserID;
	int			dwDestUserCharmValue;
};
struct LC_Cmd_TableRoleCharmInfo : public NetCmd
{
	DWORD		SrcUserID;
	DWORD		DestUserID;
	BYTE		CharmIndex;
	DWORD		ItemSum;
};
//�̵�
enum ShopSub
{
	CL_ShopItem = 1,
	LC_ShopItemResult = 2,
};
struct CL_Cmd_ShopItem : public NetCmd
{
	BYTE		ShopID;
	//tagItemOnce pItem;
	BYTE		ShopItemIndex;
	DWORD		ItemSum;
};
struct LC_Cmd_ShopItemResult : public NetCmd
{
	bool		Result;
};
//Entity
enum EntitySub
{
	CL_LoadUserEntity = 1,
	LC_LoadUserEntity = 2,

	//CL_ChangeRoleName = 3,
	LC_ChangeRoleName = 4,

	//CL_ChangeRolePhone = 5,
	LC_ChangeRolePhone = 6,

	//CL_ChangeRoleEmail = 7,
	LC_ChangeRoleEmail = 8,

	/*CL_ChangeRoleAddress = 9,
	LC_ChangeRoleAddress = 10,*/

	//CL_ChangeRoleEntityID = 11,
	LC_ChangeRoleEntityID = 12,

	CL_ChangeRoleEntityItemUseName = 15,
	LC_ChangeRoleEntityItemUseName = 16,

	CL_ChagneRoleEntityItemUsePhone = 17,
	LC_ChangeRoleEntityItemUsePhone = 18,

	CL_ChangeRoleEntityItemUseAddress = 19,
	LC_ChangeRoleEntityItemUseAddress = 20,

	CL_ChangeRoleEntityAlipayInfo	= 21,
	LC_ChangeRoleEntityAlipayInfo	= 22,

	//��Ұ�֧�����޸�
	CL_GetChangeAlipayPhoneNum		= 31,
	LO_GetChangeAlipayPhoneNum		= 32,
	OL_GetChangeAlipayPhoneNum	    = 33,
	LC_GetChangeAlipayPhoneNum		= 34,

	CL_ChangeAlipayByPhone			= 41,
	LO_ChangeAlipayByPhone			= 42,
	OL_ChangeAlipayByPhone			= 43,
	LC_ChangeAlipayByPhone			= 44,
};
struct LC_Cmd_LoadUserEntity : public NetCmd
{
	tagRoleAddressInfo		UserEntityInfo;
};
//struct CL_Cmd_ChangeRoleName : public NetCmd
//{
//	TCHAR			Name[MAX_NAME_LENTH + 1];//����
//};
struct LC_Cmd_ChangeRoleName : public NetCmd
{
	TCHAR			Name[MAX_NAME_LENTH + 1];//����
};
//
//struct CL_Cmd_ChangeRolePhone : public NetCmd
//{
//	TCHAR		Phone[MAX_PHONE_LENGTH + 1];
//};
struct LC_Cmd_ChangeRolePhone : public NetCmd
{
	unsigned __int64		Phone;
};

//struct CL_Cmd_ChangeRoleEmail : public NetCmd
//{
//	TCHAR			Email[MAX_EMAIL_LENGTH + 1];
//};
struct LC_Cmd_ChangeRoleEmail : public NetCmd
{
	TCHAR			Email[MAX_EMAIL_LENGTH + 1];
};

//struct CL_Cmd_ChangeRoleAddress : public NetCmd
//{
//	TCHAR			Addres[MAX_ADDRESS_LENGTH + 1];
//};
//struct LC_Cmd_ChangeRoleAddress : public NetCmd
//{
//	TCHAR			Addres[MAX_ADDRESS_LENGTH + 1];
//};

//struct CL_Cmd_ChangeRoleEntityID : public NetCmd
//{
//	TCHAR			IdentityID[MAX_IDENTITY_LENGTH + 1];//���֤����
//};
struct LC_Cmd_ChangeRoleEntityID : public NetCmd
{
	TCHAR			IdentityID[MAX_IDENTITY_LENGTH + 1];//���֤����
};

struct CL_Cmd_ChangeRoleEntityItemUseName : public NetCmd
{
	TCHAR			EntityItemUseName[MAX_NAME_LENTH + 1];
};
struct LC_Cmd_ChangeRoleEntityItemUseName: public NetCmd
{
	TCHAR			EntityItemUseName[MAX_NAME_LENTH + 1];
};
struct CL_Cmd_ChagneRoleEntityItemUsePhone : public NetCmd
{
	unsigned __int64		EntityItemUsePhone;
};
struct LC_Cmd_ChangeRoleEntityItemUsePhone : public NetCmd
{
	unsigned __int64		EntityItemUsePhone;
};
struct CL_Cmd_ChangeRoleEntityItemUseAddress : public NetCmd
{
	TCHAR			EntityItemUseAddres[MAX_ADDRESS_LENGTH + 1];
};
struct LC_Cmd_ChangeRoleEntityItemUseAddress : public NetCmd
{
	TCHAR			EntityItemUseAddres[MAX_ADDRESS_LENGTH + 1];
};
struct CL_Cmd_ChangeRoleEntityAlipayInfo : public NetCmd
{
	TCHAR			EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR			EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};
struct LC_Cmd_ChangeRoleEntityAlipayInfo : public NetCmd
{
	TCHAR			EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR			EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};

struct CL_Cmd_GetChangeAlipayPhoneNum : public NetCmd
{

};
struct LO_Cmd_GetChangeAlipayPhoneNum : public NetCmd
{
	DWORD		dwUserID;
	UINT64		u64Phone;
};
struct OL_Cmd_GetChangeAlipayPhoneNum : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_GetChangeAlipayPhoneNum : public NetCmd
{
	bool		Result;
};
struct CL_Cmd_ChangeAlipayByPhone : public NetCmd
{
	DWORD		dwPhoneNum;
	TCHAR			EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR			EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};
struct LO_Cmd_ChangeAlipayByPhone : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwPhoneNum;
	TCHAR		EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR		EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};
struct OL_Cmd_ChangeAlipayByPhone : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
	TCHAR		EntityAlipayAccount[MAX_AlipayAccount_LENGTH + 1];
	TCHAR		EntityAlipayName[MAX_AlipayName_LENGTH + 1];
};
struct LC_Cmd_ChangeAlipayByPhone : public NetCmd
{
	bool		Result;
};

//�

enum ActionSub
{
	CL_GetRoleActionInfo = 1,
	LC_GetRoleActionInfo = 2,
	//LC_GetRoleActionInfoFinish = 3,

	LC_ActionAllEventFinish = 5,

	CL_GetActionReward = 6,
	LC_GetActionReward = 7,

	CL_GetOnceActionInfo = 9,
	LC_GetOnceActionInfo = 10,

	LC_JoinAction = 11,
	LC_DelAction = 12,
};
struct CL_Cmd_GetRoleActionInfo : public NetCmd
{

};
struct LC_Cmd_GetRoleActionInfo : public NetCmd
{
	BYTE			States;
	WORD			Sum;
	tagRoleActionInfo Array[1];
};
//struct LC_Cmd_GetRoleActionInfoFinish : public NetCmd
//{
//
//};
struct LC_Cmd_ActionAllEventFinish : public NetCmd
{
	BYTE			ActionID;
};
struct CL_Cmd_GetActionReward : public NetCmd
{
	BYTE			ActionID;
	DWORD			OnceID;
};
struct LC_Cmd_GetActionReward : public NetCmd
{
	BYTE			ActionID;
};
struct CL_Cmd_GetOnceActionInfo : public NetCmd
{
	BYTE	ActionID;
};
struct LC_Cmd_GetOnceActionInfo : public NetCmd
{
	tagRoleActionInfo		ActionInfo;
};
struct LC_Cmd_DelAction : public NetCmd
{
	BYTE	ActionID;
};
struct LC_Cmd_JoinAction : public NetCmd
{
	BYTE	ActionID;
};
//Giff --------------------------------------------------------------------------
enum GiffSub
{
	CL_GetRoleGiff = 1,
	LC_GetRoleGiff = 2,
	//LC_GetRoleGiffFinish = 3,

	/*CL_AddRoleGiff = 4,
	LC_AddRoleGiffResult = 5,
	CC_AddRoleGiff = 6,
	CC_AddBeRoleGiff = 7,
	LC_AddBeRoleGiff = 8,*/

	CL_GetRoleGiffReward = 9,
	LC_GetRoleGiffReward = 10,

	CL_GetAllRoleGiffReward = 11,
	LC_GetAllRoleGiffReward = 12,

	CL_GetRoleNowDaySendGiff = 13,
	LC_GetRoleNowDaySendGiff = 14,
	//LC_GetRoleNowDaySendGiffFinish = 15,

	LC_DelRoleGiff			=16,

	CL_AddRoleGiff			= 17,
	GC_AddRoleGiff			= 18,
	CG_AddRoleGiff			= 19,
	GC_AddRoleGiffResult	= 20,
	CG_AddRoleGiffResult	= 21,
	LC_AddRoleGiffResult	= 22,
	LC_AddBeRoleGiff		= 23,
};
struct LC_Cmd_GetRoleGiff : public NetCmd
{
	BYTE			  States;
	WORD			  Sum;
	tagGiffClientInfo Array[1];
};

struct CL_Cmd_AddRoleGiff : public NetCmd
{
	DWORD		dwDestUserID;
};
struct GC_Cmd_AddRoleGiff : public NetCmd
{
	DWORD		dwSrcUserID;
	DWORD		dwDestUserID;
};
struct CG_Cmd_AddRoleGiff : public NetCmd
{
	DWORD		dwSrcUserID;
	DWORD		dwDestUserID;
};
struct GC_Cmd_AddRoleGiffResult : public NetCmd
{
	DWORD		dwSrcUserID;
	DWORD		dwDestUserID;
	bool		Result;
};
struct CG_Cmd_AddRoleGiffResult : public NetCmd
{
	DWORD		dwSrcUserID;
	DWORD		dwDestUserID;
	bool		Result;
};
struct LC_Cmd_AddRoleGiffResult : public NetCmd
{
	DWORD		dwDestUserID;
	bool		Result;
};
struct LC_Cmd_AddBeRoleGiff : public NetCmd
{
	tagGiffClientInfo GiffInfo;
};
//struct CL_Cmd_AddRoleGiff : public NetCmd
//{
//	DWORD		dwDestUserID;
//};
struct LC_Cmd_GetRoleNowDaySendGiff : public NetCmd
{
	BYTE				States;
	WORD				Sum;
	tagNowDaySendGiffInfo Array[1];
};
//struct LC_Cmd_AddRoleGiffResult : public NetCmd
//{
//	DWORD		dwDestUserID;
//	BYTE		Result;
//};
//struct CC_Cmd_AddRoleGiff : public NetCmd
//{
//	/*DWORD		dwSrcUserID;
//	DWORD		dwDestUserID;
//	DWORD		OnlyID;*/
//	DWORD		dwDestUserID;
//	tagGiffInfo GiffInfo;
//};
//struct CC_Cmd_AddBeRoleGiff : public NetCmd
//{
//	/*DWORD		dwSrcUserID;
//	DWORD		dwDestUserID;
//	DWORD		OnlyID;*/
//	DWORD		dwDestUserID;
//	tagGiffInfo GiffInfo;
//};
//struct LC_Cmd_AddBeRoleGiff : public NetCmd
//{
//	/*DWORD		dwSrcUserID;
//	DWORD		OnlyID;*/
//	tagGiffClientInfo GiffInfo;
//};
struct CL_Cmd_GetRoleGiffReward : public NetCmd
{
	DWORD		GiffID;
};
struct LC_Cmd_GetRoleGiffReward : public NetCmd
{
	DWORD		GiffID;
};
struct LC_Cmd_GetAllRoleGiffReward : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	DWORD		Array[1];
};
struct LC_Cmd_DelRoleGiff : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	DWORD		Array[1];
};

//GlobelShop
//enum GlobelShopSub
//{
//	CL_GlobelShopItem = 1,
//	LC_GlobelShopItem = 2,
//};
//struct CL_Cmd_GlobelShopItem : public NetCmd
//{
//	BYTE		ID;
//	WORD		Sum;
//};
//struct LC_Cmd_GlobelShopItem : public NetCmd
//{
//	BYTE		Result;
//};
//OnlineReward 
enum OnlineRewardSub
{
	CL_GetOnlineReward = 1,
	LC_GetOnlineReward = 2,
};
struct CL_Cmd_GetOnlineReward : public NetCmd
{
	BYTE		ID;
};
struct LC_Cmd_GetOnlineReward : public NetCmd
{
	BYTE		RewardID;
	DWORD		States;
	bool		Result;//�Ƿ�ɹ�
};

enum FtpShopSub
{
	CF_UpLoadFile = 1,
};
struct CF_Cmd_UpLoadFile : public NetCmd
{
	char			FileName[64+1];
	DWORD			FileSum;
	BYTE			States;
	WORD			Sum;
	BYTE			Array[1];
};

//Main_GameData
enum GameDataSub
{
	CL_GetGameData  = 1,
	LC_GendGameData = 2,
};
struct CL_Cmd_GetGameData : public NetCmd
{
};
struct LC_Cmd_GendGameData : public NetCmd
{
	tagRoleGameData GameData;
};


//Main_Package
enum PackageSub
{
	//CL_OpenPackage = 1,
	LC_OpenPackage = 2,
};
//struct CL_Cmd_OpenPackage : public NetCmd
//{
//	DWORD		PackageItemID;
//};
struct LC_Cmd_OpenPackage : public NetCmd
{
	bool		Result;
	BYTE		RewardSum;
	WORD		RewardID[1];
};

//Main_Server
enum ServerSub
{
	//���������� ֮���໥ע������̴���
	//Logon->Center
	LC_RsgLogon   = 1,//Logon��Centerע���Լ� ��ʾ�Լ����Ե�½�� Я���Լ���ID Logon�������ļ�FishServer�е�Ψһ���Logon��ID
	CL_RsgLogon	  = 2,//Center ��Logon��Ҫ�� ��ǰ������ҵ��б��ظ�Logon �䳤�� UserID:GameID
	CG_RsgLogon	  = 3,//����GameServer һ���µ�Logon������
	CG_UnRsgLogon = 4,//����ȫ����GameServer ָ����Logon�뿪��


	GC_RsgGame	  = 5,//Game��Center ע���Լ� ����Я����ǰ�Ѿ���½��GameServer�ϵ�ȫ����� ������ ���������ע��
	CG_RsgGame	  = 6,//Center��ȫ����Logon���б��͸��ռ����Game ������ ȥ������Щ��Logon 
	CL_UnRsgGame  = 7,//GameServer�뿪Center��

	CL_RsgUser    = 8,
	CL_UnRsgUser  = 9,

	CL_GameInfo   = 10,
	SR_ServerInfo = 11,//����������ע��
	//CC_ClientToServer = 10,

	//����������֮���ת��
	SS_LogonToGame = 20,//LogonToGameServer
	SS_GameToLogon = 21,//GameServerToLogon
	SS_GameToGame  = 22,//GameToGame
	SS_LogonToLogon= 23,//LogonToLogon
};
struct LC_Cmd_RsgLogon : public NetCmd
{
	BYTE		LogonConfigID;//��ǰLogon������ID
};
struct tagPlayerInfo
{
	DWORD		dwUserID;
	BYTE		GameConfigID;
};
struct CL_Cmd_RsgLogon : public NetCmd//���ݻ�ܳ�
{
	BYTE			States;
	WORD			Sum;//��ҵ�����
	//��ҵ��б�
	tagPlayerInfo   Array[1];//��ҵ��б�
};
struct CG_Cmd_RsgLogon : public NetCmd
{
	BYTE		LogonConfigID;
};
struct CG_Cmd_UnRsgLogon : public NetCmd
{
	BYTE		LogonConfigID;
};
struct GC_Cmd_RsgGame : public NetCmd
{
	BYTE		GameConfigID;
};
struct CG_Cmd_RsgGame : public NetCmd
{
	BYTE		States;
	WORD        Sum;//Logon������������
	BYTE		Array[1];
};
struct CL_Cmd_UnRsgGame : public NetCmd
{
	BYTE		GameConfigID;
};
struct CL_Cmd_RsgUser : public NetCmd
{
	tagPlayerInfo	PlayerInfo;
};
struct CL_Cmd_UnRsgUser : public NetCmd
{
	DWORD			dwUserID;
};
struct CL_Cmd_GameInfo : public NetCmd
{
	BYTE		States;
	WORD        Sum;//Game������������
	BYTE		Array[1];
};
//struct CC_Cmd_ClientToServer : public NetCmd
//{
//	DWORD			Length;
//	BYTE			Data[1];//������ 
//};
struct SS_Cmd_LogonToGame : public NetCmd
{
	BYTE		GameID;
	WORD		Length;
	BYTE		Array[1];
};
struct SS_Cmd_GameToLogon : public NetCmd
{
	BYTE		LogonID;
	WORD		Length;
	BYTE		Array[1];
};
struct SS_Cmd_GameToGame : public NetCmd
{
	BYTE		GameID;
	WORD		Length;
	BYTE		Array[1];
};
struct SS_Cmd_LogonToLogon : public NetCmd
{
	BYTE		LogonID;
	WORD		Length;
	BYTE		Array[1];
};

//Launcher Main_Launcher
enum LauncherSub
{
	LC_LauncherData = 1,
};
struct LC_Cmd_LauncherData : public NetCmd
{
	DWORD	LauncherData;//��̨����
};

//Message Main_Message
enum MessageSub
{
	GC_SendMessage = 1,//GameServer->Center
	CG_SendMessage = 2,//Center->GameServer
	LC_SendMessage = 3,//GameServer->Client
	CL_SendMessage = 4,//Client->GameServer
};
enum MessageType
{
	MT_Table  =1,//��ǰ����
	MT_TableMessage = 2,//��ǰ���� ����

	MT_Game = 3,//��ǰGameServer
	MT_GameMessage = 4,

	MT_Center = 5,//��ǰ������
	MT_CenterMessage=6,

	MT_User = 7,//�������
	MT_UserMessage = 8,
};
struct GC_Cmd_SendMessage : public NetCmd
{
	DWORD		Param;
	DWORD		MessageColor;
	BYTE		StepNum;
	BYTE		StepSec;
	BYTE		MessageType;//��Ϣ����
	WORD		MessageSize;
	TCHAR		Message[1];//�䳤������
	//TCHAR		Message[MAX_MESSAGE_LENGTH+1];//��Ϣ
};
struct CG_Cmd_SendMessage : public NetCmd
{
	DWORD		Param;
	DWORD		MessageColor;
	BYTE		StepNum;
	BYTE		StepSec;
	BYTE		MessageType;//��Ϣ����
	//TCHAR		Message[MAX_MESSAGE_LENGTH + 1];//��Ϣ
	WORD		MessageSize;
	TCHAR		Message[1];//�䳤������
};
struct LC_Cmd_SendMessage : public NetCmd
{
	DWORD		Param;
	DWORD		MessageColor;
	BYTE		StepNum;
	BYTE		StepSec;
	BYTE		MessageType;//��Ϣ����
	//TCHAR		Message[MAX_MESSAGE_LENGTH + 1];//��Ϣ
	WORD		MessageSize;
	TCHAR		Message[1];//�䳤������
};
struct CL_Cmd_SendMessage : public NetCmd
{
	DWORD		Param;
	//�ͻ�����������˷�����Ϣ
	DWORD		MessageColor;
	BYTE		StepNum;
	BYTE		StepSec;
	BYTE		MessageType;//��Ϣ����
	//TCHAR		Message[MAX_MESSAGE_LENGTH + 1];//��Ϣ
	WORD		MessageSize;
	TCHAR		Message[1];//�䳤������
};
//Main_Recharge
enum RechargeSub
{
	CL_Recharge = 1,
	LC_Recharge = 2,

	LC_GetOrderID = 3,

	CL_IOSRecharge = 4,//IOSƽ̨��ֵ
	GO_IOSRecharge = 5,//IOSƽ̨��ֵ
};
struct CL_Cmd_Recharge : public NetCmd
{
	DWORD	ID;
	bool    IsPC;
};
struct LC_Cmd_Recharge : public NetCmd
{
	DWORD	ID;
	bool	Result;
};
struct LC_Cmd_GetOrderID : public NetCmd
{
	DWORD	OrderID;
	DWORD	ShopID;
	bool	Result;
};
struct IOSRechargeInfo
{
	//char			OrderString
	BYTE			HandleSum;
	DWORD			Sum;
	BYTE			Array[1];
};
struct CL_Cmd_IOSRecharge : public NetCmd
{
	IOSRechargeInfo		OrderInfo;
};
struct GO_Cmd_IOSRecharge : public NetCmd
{
	DWORD				dwUserID;
	IOSRechargeInfo		OrderInfo;
};
//Main_Announcement
enum AnnouncementSub
{
	GC_GetAllAnnouncement = 1,
	CG_GetAllAnnouncement = 2,
	//CG_GetAllAnnouncementFinish = 3,
	GC_SendNewAnnouncementOnce = 4,
	CG_SendNewAnnouncementOnce = 5,
	CL_GetAllAnnouncement = 6,
	LC_GetAllAnnouncement = 7,
	//LC_GetAllAnnouncementFinish = 8,
	LC_SendNewAnnouncementOnce =9,
};
struct GC_Cmd_GetAllAnnouncement : public NetCmd
{

};
struct CG_Cmd_GetAllAnnouncement : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	AnnouncementOnce Array[1];
};
//struct CG_Cmd_GetAllAnnouncementFinish : public NetCmd
//{
//
//};
struct GC_Cmd_SendNewAnnouncementOnce : public NetCmd
{
	AnnouncementOnce		pOnce;
};
struct CG_Cmd_SendNewAnnouncementOnce : public NetCmd
{
	AnnouncementOnce		pOnce;
};
struct CL_Cmd_GetAllAnnouncement : public NetCmd
{

};
struct LC_Cmd_GetAllAnnouncement : public NetCmd
{
	BYTE		States;
	WORD		Sum;
	AnnouncementOnce Array[1];
};
//struct LC_Cmd_GetAllAnnouncementFinish : public NetCmd
//{
//
//};
struct LC_Cmd_SendNewAnnouncementOnce : public NetCmd
{
	AnnouncementOnce		pOnce;
};
//��Ӫ Main_Operate
enum OperateSub
{
	//1.ʵ����֤  ���� �� ���֤����
	CL_RealNameVerification		=1,//��  ����  ���֤���� ���͸� GameServer
	GO_RealNameVerification		=2,//�����ID ���� ���֤���� ���͸���Ӫ������
	OG_RealNameVerification		=3,//��Ӫ����������֤�Ľ�����ظ�GameServer GameServer���д��� �洢���ݿ� 
	LC_RealNameVerification		=4,//����֤�Ľ��������ʾ Я��ErrorID 

	//2.���ֻ�
	CL_GetPhoneVerificationNum	=11,//���͸�GameServer Я���ֻ����� 
	GO_GetPhoneVerificationNum	=12,//�����ID �ֻ����� ���͸���Ӫ������
	OG_GetPhoneVerificationNum	=13,//��Ӫ�����������صĽ�� ���ظ�GameServer
	LC_GetPhoneVerificationNum  =14,//֪ͨ�ͻ��� �����ֻ���֤��Ľ�� ErrorID 
	CL_BindPhone				=15,//����֤�� �� �ֻ����� ���͵�GameServer
	GO_BindPhone				=16,//�����ID ��֤�� �� �ֻ����� ���͵���Ӫ������
	OG_BindPhone				=17,//��Ӫ���������󶨲����Ľ�� ����GameServer
	LC_BindPhone				=18,//�����ֻ������Ľ�����ؿͻ���

	//3.BuyEntityItem
	GO_BuyEntityItem			=21,//����Ӫ������ �������� ��ҹ���ָ����ʵ����Ʒ
	OG_BuyEntityItem			=22,//��Ӫ������������������GameServer
	LC_BuyEntityItem			=23,//������ʵ����Ʒ�Ľ�����ظ��ͻ��� ErrorID

	//4.������
	CL_BindEmail				= 31,//���ͷ������� ���ʼ�
	GO_BindEmail				= 32,//���͵���Ӫ������ ������
	OC_BindEmail				= 33,//��Ӫ���������� ������Ľ��
	CG_BindEmail				= 34,//
	LC_BindEmail				= 35,//���߿ͻ��� ������Ľ��

	//5.�۳�rmb
	//GO_UseRMB					= 36,
	OC_UseRMB					= 36,
	CG_UseRMB					= 37,

	//6.���ѳ�ֵ
	/*CL_PrepaidRecharge		    =40,
	GO_PrepaidRecharge			=41,
	OG_PrepaidRecharge			=42,
	LC_PrepaidRecharge			=43,*/
	GO_PhonePay					= 40,
	OC_PhonePay					= 41,
	CG_PhonePay					= 42,
	LC_PhonePay					= 43,

	//7.��ͨ��ֵ
	GO_AddNormalOrderID			= 50,
	OC_AddNormalOrderID			= 51,
	CG_AddNormalOrderID			= 52,
	LC_AddNormalOrderID			= 53,

	//8.�㸶����ֵ
	CL_HYPay					= 60,
	GO_HYPay					= 61,
	OG_HYPay					= 62,
	LC_HYPay					= 63,
};
//ʵ����֤
struct CL_Cmd_RealNameVerification : public NetCmd
{
	//������� �� ���֤
	TCHAR			Name[MAX_NAME_LENTH + 1];
	TCHAR			IDCard[MAX_IDENTITY_LENGTH+1];
};
struct GO_Cmd_RealNameVerification : public NetCmd
{
	DWORD			dwUserID;
	TCHAR			Name[MAX_NAME_LENTH + 1];
	TCHAR			IDCard[MAX_IDENTITY_LENGTH + 1];
};
struct OG_Cmd_RealNameVerification : public NetCmd
{
	bool			Result;
	BYTE			ErrorID;//����ID
	DWORD			dwUserID;
	TCHAR			Name[MAX_NAME_LENTH + 1];
	TCHAR			IDCard[MAX_IDENTITY_LENGTH + 1];
};
struct LC_Cmd_RealNameVerification : public NetCmd
{
	BYTE			ErrorID;//����ID
	TCHAR			Name[MAX_NAME_LENTH + 1];
	TCHAR			IDCard[MAX_IDENTITY_LENGTH + 1];
};
//�ֻ���
struct CL_Cmd_GetPhoneVerificationNum : public NetCmd
{
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
	unsigned __int64  PhoneNumber;
};
struct GO_Cmd_GetPhoneVerificationNum : public NetCmd
{
	DWORD			dwUserID;
	unsigned __int64  PhoneNumber;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
};
struct OG_Cmd_GetPhoneVerificationNum : public NetCmd
{
	BYTE			ErrorID;
	DWORD			dwUserID;
	unsigned __int64  PhoneNumber;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
};
struct LC_Cmd_GetPhoneVerificationNum : public NetCmd
{
	BYTE			ErrorID;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
	unsigned __int64  PhoneNumber;
};
struct CL_Cmd_BindPhone : public NetCmd
{
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
	//unsigned __int64  PhoneNumber;
	DWORD			BindNumber;
	DWORD			SecPasswordCrc1;
	DWORD			SecPasswordCrc2;
	DWORD			SecPasswordCrc3;
};
struct GO_Cmd_BindPhone : public NetCmd
{
	DWORD			dwUserID;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH + 1];
	//unsigned __int64  PhoneNumber;
	DWORD			BindNumber;
	DWORD			SecPasswordCrc1;
	DWORD			SecPasswordCrc2;
	DWORD			SecPasswordCrc3;
};
struct OG_Cmd_BindPhone : public NetCmd
{
	bool			Result;
	BYTE			ErrorID;
	DWORD			dwUserID;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH+1];
	unsigned __int64  PhoneNumber;
	DWORD			SecPasswordCrc1;
	DWORD			SecPasswordCrc2;
	DWORD			SecPasswordCrc3;
};
struct LC_Cmd_BindPhone : public NetCmd
{
	BYTE			ErrorID;
	bool			IsFirstBind;
	//TCHAR			PhoneNumber[MAX_PHONE_LENGTH+1];
	unsigned __int64  PhoneNumber;
};
//3.ʵ����Ʒ���� �г�ֵ���͵� ������ �ʵ����͵� �ⲿ���� 
struct GO_Cmd_BuyEntityItem : public NetCmd
{
	//������Ӫ������ ��ҹ���ʵ����Ʒ
	DWORD			dwUserID;
	//��ַ �ֻ� ���� 
	TCHAR			Name[MAX_NAME_LENTH + 1];//����
	unsigned __int64	Phone;//�ֻ�����
	TCHAR			Addres[MAX_ADDRESS_LENGTH + 1];//��ʵ��ַ
	//��Ʒ����
	BYTE			ShopID;
	BYTE			ShopOnlyID;
	DWORD			ItemID;
	DWORD			ItemSum;
};
struct OG_Cmd_BuyEntityItem : public NetCmd
{
	bool			Result;
	BYTE			ErrorID;
	DWORD			dwUserID;
	BYTE			ShopID;
	BYTE			ShopOnlyID;
	DWORD			ItemSum;
};
struct LC_Cmd_BuyEntityItem : public NetCmd
{
	BYTE			ErrorID;//��ʾ��ҹ���ɹ� ���� ʧ��
};
//������
struct CL_Cmd_BindEmail : public NetCmd
{
	TCHAR			EMail[MAX_EMAIL_LENGTH + 1];
};
struct GO_Cmd_BindEmail : public NetCmd
{
	DWORD			dwUserID;
	TCHAR			EMail[MAX_EMAIL_LENGTH + 1];
};
struct OC_Cmd_BindEmail : public NetCmd
{
	bool			Result;
	BYTE			ErrorID;
	DWORD			dwUserID;
	TCHAR			EMail[MAX_EMAIL_LENGTH + 1];
};
struct CG_Cmd_BindEmail : public NetCmd
{
	bool			Result;
	BYTE			ErrorID;
	DWORD			dwUserID;
	TCHAR			EMail[MAX_EMAIL_LENGTH + 1];
};
struct LC_Cmd_BindEmail : public NetCmd
{
	BYTE			ErrorID;
	TCHAR			EMail[MAX_EMAIL_LENGTH + 1];
};
//
//struct GO_Cmd_UseRMB : public NetCmd
//{
//	DWORD			dwUserID;
//	DWORD			OnlyID;
//	float			LostRMB;
//	TCHAR			OnceOnlyID[RMB_OnceOnlyID_Length];//����Ψһ���
//};

struct OC_Cmd_UseRMB : public NetCmd
{
	RechageInfo		rechargeInfo;
};
struct CG_Cmd_UseRMB : public NetCmd
{
	RechageInfo		rechargeInfo;
};
//struct CL_Cmd_PrepaidRecharge : public NetCmd
//{
//	BYTE		ShopID;
//	BYTE		ShopOnlyID;
//	TCHAR		PhoneNumber[MAX_PHONE_LENGTH + 1];//�ֻ�����
//};
//struct GO_Cmd_PrepaidRecharge : public NetCmd
//{
//	TCHAR		PhoneNumber[MAX_PHONE_LENGTH + 1];//�ֻ�����
//	BYTE		ShopID;
//	BYTE		ShopOnlyID;
//	DWORD		RechargeMoney;
//};
//struct OG_Cmd_PrepaidRecharge : public NetCmd
//{
//	bool		Result;//�ɹ�����ʧ����
//	BYTE		ErrorID;
//	TCHAR		PhoneNumber[MAX_PHONE_LENGTH + 1];//�ֻ�����
//	BYTE		ShopID;
//	BYTE		ShopOnlyID;
//	DWORD		RechargeMoney;
//};
//struct LC_Cmd_PrepaidRecharge : public NetCmd
//{
//	bool		Result;//�ɹ�����ʧ����
//	BYTE		ErrorID;
//};

struct GO_Cmd_PhonePay : public NetCmd
{
	DWORD		dwUserID;
	BYTE		ShopID;
	BYTE		ShopOnlyID;
	DWORD		ShopSum;
	unsigned __int64 Phone;
};
struct OC_Cmd_PhonePay : public NetCmd
{
	unsigned __int64 OrderID;
	DWORD		dwUserID;
	unsigned __int64 Phone;
	bool		Result;
	BYTE		ErrorID;
	BYTE		ShopID;
	BYTE		ShopOnlyID;
	DWORD		ShopSum;
};
struct CG_Cmd_PhonePay : public NetCmd
{
	unsigned __int64 OrderID;
	DWORD		dwUserID;
	unsigned __int64 Phone;
	bool		Result;
	BYTE		ErrorID;
	BYTE		ShopID;
	BYTE		ShopOnlyID;
	DWORD		ShopSum;
};
struct LC_Cmd_PhonePay : public NetCmd
{
	BYTE		ErrorID;
};

struct GO_Cmd_AddNormalOrderID : public NetCmd
{
	DWORD	OrderID;
	DWORD	ShopID;
	DWORD	dwUserID;
};
struct OC_Cmd_AddNormalOrderID : public NetCmd
{
	DWORD	dwUserID;
	bool	Result;
	char	Transid[33];//������ˮ��
	char    Sign[256];
	DWORD	ShopID;
	DWORD	OrderID;
};
struct CG_Cmd_AddNormalOrderID : public NetCmd
{
	DWORD	dwUserID;
	bool	Result;
	char	Transid[33];//������ˮ��
	char    Sign[256];
	DWORD	ShopID;
	DWORD	OrderID;
};
struct LC_Cmd_AddNormalOrderID : public NetCmd
{
	bool	Result;
	char	Transid[33];//������ˮ��
	char    Sign[256];
	DWORD	ShopID;
	DWORD	OrderID;
};
struct CL_Cmd_HYPay : public NetCmd
{
	DWORD	dwShopID;
	DWORD	dwAgentType;
	DWORD	dwShopNum;
};
struct GO_Cmd_HYPay : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwShopID;
	DWORD	dwClientIP;
	DWORD	dwAgentType;
	DWORD	dwShopNum;
};
struct OG_Cmd_HYPay : public NetCmd
{
	DWORD	dwUserID;
	bool	bResult;
	UINT64  OrderID;
	UINT32  AgentID;
	UINT32  AgentType;
	WORD	wTokenLength;
	TCHAR	strToken[1];
};
struct LC_Cmd_HYPay : public NetCmd
{
	bool	bResult;
	UINT64  OrderID;
	UINT32  AgentID;
	UINT32  AgentType;
	WORD	wTokenLength;
	TCHAR	strToken[1];
};

//Main_RoleMessage
enum RoleMessageSub
{
	LC_RoleMessageChange  = 1,
};
struct LC_Cmd_RoleMessageChange : public NetCmd
{
	DWORD			RoleMessageData;
};

//Main_Exchange
enum ExChangeSub
{
	CL_RoleUseExChangeCode = 1,//ʹ�öһ���
	LC_RoleUseExChangeCode = 2,
};
struct CL_Cmd_RoleUseExChangeCode : public NetCmd
{
	TCHAR			ExChangeCode[ExChangeCode_Length+1];
};
struct  LC_Cmd_RoleUseExChangeCode : public NetCmd
{
	bool			Result;//ʹ���Ƿ�ɹ�
	BYTE			ID;
};

//Main_Control
enum ControlSub
{
	CL_CheckClientInfo = 1,//�ⲿ��½��Control
	LC_CheckClientInfo = 2,

	//CL_GetCenterPlayerSum = 3,//��ȡ����������������
	//LC_GetCenterPlayerSum = 4,

	CL_SendMsgToAllGame = 5,//����ȫ������

	//CL_ResetNoticeConfig= 6,

	CL_KickUserByID = 7,//�޳����
	LC_KickUserResult = 8,

	CL_ReloadConfig = 9,//���¼��������ļ�

	CL_HandleEntityItem = 10,//ʵ����Ʒ����
	LC_HandleEntityItem = 11,

	CL_ResetRolePassword = 12,//�����û�������
	CL_FreezeAccount = 13,//������� 

	CL_SendSystemEmail = 14,//����ϵͳ�ʼ�

	CL_ChangeaNickName = 15,//�޸�����

	CL_QueryOnlineUserInfo = 16,
	LC_QueryOnlineUserInfo = 17,

	CL_ChangeParticularStates = 18,
	LC_ChangeParticularStates = 19,

	//���Ʒ���������ͳ��
	LC_CenterInfo = 20,
	LC_GameInfo = 21,
	LC_LogonInfo = 22,
	LC_DBInfo = 23,
	LC_RankInfo = 24,
	LC_FTPInfo = 25,
	LC_OperateInfo = 26,
	LC_MiNiGameInfo = 27,
	CL_QueryFishPool = 28,//��ѯ���
	LC_QueryFishPoolResult=29,//��ѯ�����
	CL_QueryBlackList = 30,//��ѯ������
	LC_QueryBlackListResult = 31,//��ѯ���������
	CL_SetBlackList = 32,//���ú�����
	LC_SetBlackListResult=33,//���ú��������
	CL_OxAdmin=40,//ţţ����
	LC_OxAdmin=41,//ţţ����
	LC_OxAdminQueryName=42,//��ѯ����
	LC_OxAdminQueryAllJetton = 43,//��ѯ����
	LC_OxAdminQueryPlayerJetton = 44,//
	
	CL_AnimalAdmin = 45,////������
	LC_AnimalAdmin = 46,//
	LC_AnimalAdminQueryName = 47,//��ѯ����
	LC_AnimalAdminQueryAllJetton = 48,//��ѯ����
	LC_AnimalAdminQueryPlayerJetton = 49,//

	CL_CarAdmin = 50,////������
	LC_CarAdmin = 51,//
	LC_CarAdminQueryName = 52,//��ѯ����
	LC_CarAdminQueryAllJetton = 53,//��ѯ����
	LC_CarAdminQueryPlayerJetton = 54,//

	//
	CL_HandleCustMail	= 55, //����һ���ͷ����ʼ�����
	CL_ChangeIsShop		= 56,
};
//ţţ,���,������ͨ��
enum  CONTROL_COMMON_INNER//ͨ�ÿ��ƴ���,,����״̬
{
	CS_BANKER_LOSE = 1,
	CS_BANKER_WIN = 2,
	CS_BET_AREA = 3,

	RQ_SET_WIN_AREA = 4,
	RQ_RESET_CONTROL = 5,
	RQ_PRINT_SYN = 6,
	RQ_QUERY_NAMES = 7,  //��id
	RQ_QUERY_PLAYERJETTON = 8,  //��ע
	RQ_QUERY_ALLJETTON = 9, //����ע 

	ACK_SET_WIN_AREA = 10,
	ACK_PRINT_SYN = 11,
	ACK_RESET_CONTROL = 12,

	CR_ACCEPT = 13,			//����
	CR_REFUSAL = 14,			//��
};

struct CL_Cmd_CheckClientInfo : public NetCmd
{
	DWORD			RankValue;
	char			MachineCode[MAC_LENGTH];
};
struct LC_Cmd_CheckClientInfo : public NetCmd
{
	bool			Result;
};

//struct CL_Cmd_GetCenterPlayerSum : public NetCmd
//{
//
//};
//struct LC_Cmd_GetCenterPlayerSum : public NetCmd
//{
//	DWORD		PlayerSum;
//};

struct CL_Cmd_SendMsgToAllGame : public NetCmd
{
	DWORD		Param;
	DWORD		MessageColor;
	BYTE		StepNum;
	BYTE		StepSec;
	WORD		MessageSize;
	TCHAR		CenterMessage[1];
};

//struct CL_Cmd_ResetNoticeConfig : public NetCmd
//{
//};


struct CL_Cmd_KickUserByID : public NetCmd
{
	DWORD			dwUserID;
	DWORD			ClientID;
	DWORD			FreezeMin;
};
struct LC_Cmd_KickUserResult : public NetCmd
{
	DWORD			dwUserID;
	DWORD			ClientID;
	BYTE			Result;
};
struct CL_Cmd_ReloadConfig : public NetCmd
{
	char tableStr[0];
};
struct CL_Cmd_ChangeParticularStates : public NetCmd
{
	DWORD		dwUserID;
	DWORD		ParticularStates;
	DWORD		ClientID;
};
struct LC_Cmd_ChangeParticularStates : public NetCmd
{
	DWORD		dwUserID;
	DWORD		ParticularStates;
	DWORD		ClientID;
	bool		Result;
};


struct CL_Cmd_HandleEntityItem : public NetCmd
{
	DWORD			ID;
	DWORD			ClientID;
	DWORD			ClientIP;
	char			OrderNumber[32];//��ݶ�����
};
struct LC_Cmd_HandleEntityItem : public NetCmd
{
	DWORD			ClientID;
	bool			Result;
};

struct CL_Cmd_ResetRolePassword : public NetCmd
{
	//�����û�������
	DWORD			dwUserID;
	DWORD			Password1;
	DWORD			Password2;
	DWORD			Password3;
};
struct CL_Cmd_FreezeAccount : public NetCmd
{
	DWORD			dwUserID;
	DWORD			FreezeMin;//��������� 
};
struct CL_Cmd_SendSystemEmail : public NetCmd
{
	DWORD		dwUserID;
	DWORD		RewardID;
	DWORD		RewardSum;
	WORD		ContextSize;
	TCHAR		EmailContext[1];
};
struct CL_Cmd_ChangeaNickName : public NetCmd
{
	DWORD		dwUserID;
	TCHAR		NickName[MAX_NICKNAME + 1];
};
struct CL_Cmd_QueryOnlineUserInfo : public NetCmd
{
	DWORD		dwUserID;
	DWORD		ClientID;
};
struct LC_Cmd_QueryOnlineUserInfo : public NetCmd
{
	DWORD		ClientID;
	bool		Result;
	tagRoleInfo RoleInfo;
};

struct LC_Cmd_CenterInfo : public NetCmd
{
	//
	BYTE		CenterID;
	DWORD		AllGamePlayerSum;//����������
	DWORD		GameServerSum;
	DWORD		LogonServerSum;
};
struct LC_Cmd_GameInfo : public NetCmd
{
	BYTE		GameID;
	DWORD		OnlinePlayerSum;
	DWORD		TableSum;
	DWORD		WriteLogonPlayerSum;
};
struct LC_Cmd_LogonInfo : public NetCmd
{
	BYTE		LogonID;
	DWORD		LogonTablePlayerSum;
};
struct LC_Cmd_DBInfo : public NetCmd
{
	BYTE		DBID;
};
struct LC_Cmd_RankInfo : public NetCmd
{
	DWORD		RankID;
};
struct LC_Cmd_FTPInfo : public NetCmd
{
	DWORD		FTPID;
};
struct LC_Cmd_OperateInfo : public NetCmd
{
	DWORD		OperateID;
};
struct LC_Cmd_MiNiGameInfo :public NetCmd
{
	BYTE		MiniGameID;

	//NiuNiu������
	INT64		NowNiuNiuSystemGlobel;//��ǰţţ�Ľ��
};
struct CL_Cmd_HandleCustMail :public NetCmd
{
	DWORD		u32MailID;
	TCHAR		strMailContext[256];
};
struct CL_Cmd_ChangeIsShop :public NetCmd
{
	DWORD		dwUserID;
	bool		bShop;
};
//��ѯ���
struct CL_CMD_QueryFishPool :public NetCmd
{
	DWORD ClientID;
};

struct TableTypePool
{
	bool bopen;
	int64 npool;
};

struct LC_CMD_QueryFishPoolResult :public NetCmd
{
	DWORD ClientID;
	BYTE byServerid;	
	TableTypePool  table[1];
};
//��ѯ������
struct LC_CMD_QueryFishBlackList :public NetCmd
{
	DWORD ClientID;
};

struct LC_CMD_QueryFishBlackListResult :public NetCmd
{
	DWORD ClientID;
	BYTE byServerid;	
	DWORD dwUserID[1];
};
//���ú�����
struct LC_CMD_SetFishBlackList :public NetCmd
{
	DWORD ClientID;
	DWORD dwUserID[1];
};
struct LC_CMD_SetFishBlackListResult :public NetCmd
{
	DWORD ClientID;
	BYTE byServerid;
	BYTE byCount;
};
//ox
struct tagOxAdminReq
{
	BYTE							m_cbExcuteTimes;					//ִ�д���
	BYTE							m_cbControlStyle;					//���Ʒ�ʽ
	bool							m_bWinArea[MAX_NIUNIU_ClientSum];						//Ӯ������
};
struct CL_CMD_OxAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbReqType;
	BYTE cbExtendData[20];			//��������
};

struct LC_CMD_OxAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbAckType;					//�ظ�����
	BYTE cbResult;
	BYTE cbExtendData[20];			//��������
};
struct LC_CMD_OxAdmin_AllJetton :public NetCmd
{
	DWORD ClientID;
	UINT64 nStock;
	UINT64 nJetton[MAX_NIUNIU_ClientSum];
};
struct LC_CMD_OxAdmin_PlayerJetton :public NetCmd
{
	DWORD ClientID;
	TCHAR name[MAX_NICKNAME+1];
	DWORD dwJetton[MAX_NIUNIU_ClientSum];
};

struct LC_CMD_OxAdmin_PlayerNames :public NetCmd
{
	DWORD ClientID;
	TCHAR names[1];
};

//���
struct tagAnimalAdminReq
{
	BYTE							m_cbExcuteTimes;					//ִ�д���
	BYTE							m_cbControlStyle;					//���Ʒ�ʽ
	BYTE							m_cbarea;
};
struct CL_CMD_AnimalAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbReqType;
	BYTE cbExtendData[20];			//��������
};

struct LC_CMD_AnimalAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbAckType;					//�ظ�����
	BYTE cbResult;
	BYTE cbExtendData[20];			//��������
};

struct LC_CMD_AnimalAdmin_AllJetton :public NetCmd
{
	DWORD ClientID;
	UINT64 nStock;
	UINT64 nJetton[MAX_DIAL_ClientSum];
};
struct LC_CMD_AnimalAdmin_PlayerJetton :public NetCmd
{
	DWORD ClientID;
	TCHAR name[MAX_NICKNAME + 1];
	DWORD dwJetton[MAX_DIAL_ClientSum];
};

struct LC_CMD_AnimalAdmin_PlayerNames :public NetCmd
{
	DWORD ClientID;
	TCHAR names[1];
};

//��
struct tagCarAdminReq
{
	BYTE							m_cbExcuteTimes;					//ִ�д���
	BYTE							m_cbControlStyle;					//���Ʒ�ʽ
	BYTE							m_cbarea;
};
struct CL_CMD_CarAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbReqType;
	BYTE cbExtendData[20];			//��������
};

struct LC_CMD_CarAdminReq :public NetCmd
{
	DWORD ClientID;
	BYTE cbAckType;					//�ظ�����
	BYTE cbResult;
	BYTE cbExtendData[20];			//��������
};

struct LC_CMD_CarAdmin_AllJetton :public NetCmd
{
	DWORD ClientID;
	UINT64 nStock;
	UINT64 nJetton[MAX_CAR_ClientSum];
};
struct LC_CMD_CarAdmin_PlayerJetton :public NetCmd
{
	DWORD ClientID;
	TCHAR name[MAX_NICKNAME + 1];
	DWORD dwJetton[MAX_CAR_ClientSum];
};

struct LC_CMD_CarAdmin_PlayerNames :public NetCmd
{
	DWORD ClientID;
	TCHAR names[1];
};


//Main_Lottery
enum LotterySub
{
	CL_GetLotteryReward = 1,
	LC_GetLotteryReward = 2,

	CL_LotteryUIStates   = 3,
	LC_LotteryUIStates   = 4,
};
struct CL_Cmd_GetLotteryReward : public NetCmd
{
	BYTE		LotteryID;
};
struct LC_Cmd_GetLotteryReward : public NetCmd
{
	BYTE		LotteryID;
	WORD		RewardID;
	bool		Result;
};
struct CL_Cmd_LotteryUIStates : public NetCmd
{
	DWORD		dwUserID;
	bool		IsOpen;
};
struct LC_Cmd_LotteryUIStates : public NetCmd
{
	DWORD		dwUserID;
	bool		IsOpen;
};
enum ExtraDownCmd
{
	EXTRA_DOWN_CHECK_VER = 1,
	EXTRA_DOWN_FILE		 = 2,
	EXTRA_DOWN_FILE_EX	 = 3,
};

//Main_MiniGame
//ţţ��һЩ�����ṹ

enum MiniGameSub
{
	////С��Ϸ�Ĵ���
	////ţţ
	//CL_JoinRoom = 1,//����ţţ����
	//GM_JoinRoom = 2,
	//MG_JoinRoom = 3,//�������ϵ����µ����ݷ��͸����
	//LC_JoinRoom = 4,

	//MG_TableUpdate = 5,//����ע�׶� ��ͣ�ķ������µĽ������ݵ��ͻ���ȥ
	//LC_TableUpdate = 6,

	//CL_AddGlobel = 7,//��ע
	//GM_AddGlobel = 8,
	//MG_AddGlobel = 9,
	//LC_AddGlobel = 10,//��ע�Ľ��

	//MG_TableResult = 11,//���
	//LC_TableResult = 12,

	//MG_TableBegin = 13,//�������½�����ע�׶�
	//LC_TableBegin = 14,

	//CL_LeaveNiuNiuTable = 15,
	//GM_LeaveNiuNiuTable = 16,

	//MG_BankerChange = 17,
	//LC_BankerChange = 18,

	///*MG_WirteBankerListChange = 19,
	//LC_WirteBankerListChange = 20,*/

	//CL_AddBankerList = 21, //��ׯ
	//GM_AddBankerList = 22, 
	//MG_AddBankerList = 23,
	//LC_AddBankerList = 24,

	//CL_NextBankerSeat = 25,//��ׯ
	//GM_NextBankerSeat = 26,
	//MG_NextBankerSeat = 27,
	//LC_NextBankerSeat = 28,

	//CL_GetBankerList  = 29,//��ȡ�Ŷ��б�
	//GM_GetBankerList  = 30,
	//MG_GetBankerList  = 31,
	//LC_GetBankerList  = 32,

	//CL_LeaveBankerList = 33,//�뿪�Ŷ��б�
	//GM_LeaveBankerList = 34,
	//MG_LeaveBankerList = 35,
	//LC_LeaveBankerList = 36,

	//CL_CanelBankerSeat = 37,
	//GM_CanelBankerSeat = 38,
	//MG_CanelBankerSeat = 39,
	//LC_CanelBankerSeat = 40,


	//MiniGame��������������
	//CL_RoleJoinMiniGame    = 30,
	GM_RoleJoinMiniGame    = 31,
	//CL_RoleLeaveMiniGame   = 32,
	GM_RoleLeaveMiniGame   = 33,
	GM_RoleChangeNickName  = 34,
	GM_RoleChangeFaceID    = 35,
	GM_RoleChangeGlobel    = 36,
	GM_RoleChangeMadel     = 37,
	GM_RoleChangeCurrcey   = 38,
	GM_RoleChangeVipLevel  = 39,
	GM_ChangeRoleParticularStates = 40,
};
////��ҽ�������
//struct CL_Cmd_JoinRoom : public NetCmd
//{
//	//��ҽ������� ţţ
//};
//struct GM_Cmd_JoinRoom : public NetCmd
//{
//	DWORD				dwUserID;
//};
//struct MG_Cmd_JoinRoom : public NetCmd
//{
//	DWORD				dwUserID;
//	BYTE				TableStates;
//	BYTE				TableUpdateSec;//������ʣ�������
//	//�������ϵ����ݷ��͵��ͻ���ȥ
//	TableUpdateInfo		TableInfo;//�����Ͻ��ص�����
//	TableBrandResult	TableBrand;//�������Ƶ�����
//	BankerInfo			TableBanker;//ׯ������
//	BYTE				BankerGameSum;//ׯ�ҽ�����Ϸ�Ĵ���
//	WriteBankerInfo		ListInfo;//�Ŷӵ���Ϣ
//};
//struct LC_Cmd_JoinRoom : public NetCmd
//{
//	BYTE				TableUpdateSec;//������ʣ�������
//	BYTE				TableStates;//����״̬
//	TableUpdateInfo		TableInfo;//�����Ͻ��ص�����
//	TableBrandResult	TableBrand;//�������Ƶ�����
//	BankerInfo			TableBanker;//ׯ������
//	BYTE				BankerGameSum;//ׯ�ҽ�����Ϸ�Ĵ���
//	WriteBankerInfo		ListInfo;//�Ŷӵ���Ϣ
//};
////����ˢ��
//struct MG_Cmd_TableUpdate : public NetCmd
//{
//	DWORD				dwUserID;
//	TableUpdateInfo		TableInfo;
//};
//struct LC_Cmd_TableUpdate : public NetCmd
//{
//	TableUpdateInfo		TableInfo;
//};
////�����ע
//struct CL_Cmd_AddGlobel : public NetCmd
//{
//	//�����ע
//	BYTE				Index;//��ע��
//	DWORD				GlobelSum;
//};
//struct GM_Cmd_AddGlobel : public NetCmd
//{
//	DWORD				dwUserID;
//	BYTE				Index;//��ע��
//	DWORD				GlobelSum;
//	DWORD				RoleGlobelSum;
//};
//struct MG_Cmd_AddGlobel : public NetCmd
//{
//	DWORD				dwUserID;
//	BYTE				Index;//��ע��
//	DWORD				GlobelSum;
//	bool				Result;
//};
//struct LC_Cmd_AddGlobel : public NetCmd
//{
//	BYTE				Index;//��ע��
//	DWORD				GlobelSum;//�������
//	bool				Result;//���
//};
////���ӿ��ƽ׶�
//struct MG_Cmd_TableResult : public NetCmd
//{
//	DWORD				dwUserID;
//	int					AddGlobel;
//	TableBrandResult	TableBrand;//�������Ƶ�����
//	TableUpdateInfo		TableInfo;
//	DWORD				BankerUserID;
//	DWORD				BankerGlobelSum;
//};
//struct LC_Cmd_TableResult : public NetCmd
//{
//	int					AddGlobel;
//	TableBrandResult	TableBrand;//�������Ƶ�����
//	TableUpdateInfo		TableInfo;
//	DWORD				BankerUserID;
//	DWORD				BankerGlobelSum;
//};
////�������½�����ע�׶�
//struct MG_Cmd_TableBegin : public NetCmd
//{
//	DWORD				dwUserID;				
//};
//struct LC_Cmd_TableBegin : public NetCmd
//{
//
//};
////�뿪
//struct CL_Cmd_LeaveNiuNiuTable : public NetCmd
//{
//
//};
//struct GM_Cmd_LeaveNiuNiuTable : public NetCmd
//{
//	DWORD				dwUserID;
//};
////ׯ�ұ仯��ʱ��
//struct MG_Cmd_BankerChange : public NetCmd
//{
//	//��ׯ�ҷ����仯��ʱ��
//	DWORD				dwUserID;
//	BankerInfo			TableBanker;//ׯ������
//};
//struct LC_Cmd_BankerChange : public NetCmd
//{
//	BankerInfo			TableBanker;//ׯ������
//};
//
//struct MG_Cmd_WirteBankerListChange : public NetCmd
//{
//	DWORD				dwUserID;
//};
//struct LC_Cmd_WirteBankerListChange : public NetCmd
//{
//
//};
//
//struct CL_Cmd_AddBankerList : public NetCmd
//{
//
//};
//struct GM_Cmd_AddBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct MG_Cmd_AddBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//	bool		Result;
//	DWORD		SeatIndex;//λ��
//};
//struct LC_Cmd_AddBankerList : public NetCmd
//{
//	bool		Result;
//	DWORD		SeatIndex;//λ��
//};
//
//struct CL_Cmd_NextBankerSeat : public NetCmd
//{
//
//};
//struct GM_Cmd_NextBankerSeat : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct MG_Cmd_NextBankerSeat : public NetCmd
//{
//	DWORD		dwUserID;
//	bool		Result;
//};
//struct LC_Cmd_NextBankerSeat : public NetCmd
//{
//	bool		Result;
//};
//
//struct CL_Cmd_GetBankerList : public NetCmd
//{
//
//};
//struct GM_Cmd_GetBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct MG_Cmd_GetBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//	DWORD		SeatIndex;
//	WriteBankerInfo		ListInfo;//�Ŷӵ���Ϣ
//};
//struct LC_Cmd_GetBankerList : public NetCmd
//{
//	DWORD		SeatIndex;
//	WriteBankerInfo		ListInfo;//�Ŷӵ���Ϣ
//};
//struct CL_Cmd_LeaveBankerList : public NetCmd
//{
//
//};
//struct GM_Cmd_LeaveBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct MG_Cmd_LeaveBankerList : public NetCmd
//{
//	DWORD		dwUserID;
//	bool		Result;
//};
//struct LC_Cmd_LeaveBankerList : public NetCmd
//{
//	bool		Result;
//};
//struct CL_Cmd_CanelBankerSeat : public NetCmd
//{
//
//};
//struct GM_Cmd_CanelBankerSeat : public NetCmd
//{
//	DWORD		dwUserID;
//};
//struct MG_Cmd_CanelBankerSeat : public NetCmd
//{
//	DWORD		dwUserID;
//	bool		Result;
//};
//struct LC_Cmd_CanelBankerSeat : public NetCmd
//{
//	bool		Result;
//};
//������������
//struct CL_Cmd_RoleJoinMiniGame : public NetCmd
//{
//	
//};

struct GM_Cmd_RoleJoinMiniGame : public NetCmd
{
	tagMiniGameRoleInfo   RoleInfo;
};
//struct CL_Cmd_RoleLeaveMiniGame : public NetCmd
//{
//
//};
struct GM_Cmd_RoleLeaveMiniGame : public NetCmd
{
	DWORD					dwUserID;
};
struct GM_Cmd_RoleChangeNickName : public NetCmd
{
	DWORD					dwUserID;
	TCHAR					NickName[MAX_NICKNAME + 1];
};
struct GM_Cmd_RoleChangeFaceID : public NetCmd
{
	DWORD					dwUserID;
	DWORD					FaceID;
};
struct GM_Cmd_RoleChangeGlobel : public NetCmd
{
	DWORD					dwUserID;
	DWORD					dwGlobel;
};
struct GM_Cmd_RoleChangeMadel : public NetCmd
{
	DWORD					dwUserID;
	DWORD					dwMadel;
};
struct GM_Cmd_RoleChangeCurrcey : public NetCmd
{
	DWORD					dwUserID;
	DWORD					dwCurrcey;
};
struct GM_Cmd_RoleChangeVipLevel : public NetCmd
{
	DWORD					dwUserID;
	BYTE					VipLevel;
};
struct GM_Cmd_ChangeRoleParticularStates : public NetCmd
{
	DWORD					dwUserID;
	DWORD					ParticularStates;
};
//Main_Dial
enum DialSub
{
	//��ҽ���ţţ������
	CL_DialRoleJoinTable = 1,
	GM_DialRoleJoinTable = 2,
	MG_DialRoleJoinTable = 3,
	LC_DialRoleJoinTable = 4,

	//����뿪ţţ����
	CL_DialRoleLeaveTable = 5,
	GM_DialRoleLeaveTable = 6,

	//�����ע
	CL_DialRoleBetGlobel = 7,
	GM_DialRoleBetGlobel = 8,
	MG_DialRoleBetGlobel = 9,
	LC_DialRoleBetGlobel = 10,

	//��һ�ȡׯ���Ŷ��б�
	CL_DialGetBankerList = 11,
	GM_DialGetBankerList = 12,
	MG_DialGetBankerList = 13,
	LC_DialGetBankerList = 14,

	//ׯ�Ҷ��б仯��ʱ��
	MG_DialBankerListChange = 15,
	LC_DialBankerListChange = 16,

	//ׯ�ҷ����仯��ʱ��
	MG_DialBankerUserChange = 17,
	LC_DialBankerUserChange = 18,

	//���ӽ�����ע�׶�
	MG_DialTableJoinBegin = 19,
	LC_DialTableJoinBegin = 20,

	//��������ע�׶εĸ�������
	MG_DialTableUpdate = 21,
	LC_DialTableUpdate = 22,

	//���ӽ��뿪�ƽ׶�
	MG_DialTableJoinEnd = 23,
	LC_DialTableJoinEnd = 24,

	//�����ׯ
	CL_DialRoleJoinBankerList = 25,
	GM_DialRoleJoinBankerList = 26,
	MG_DialRoleJoinBankerList = 27,
	LC_DialRoleJoinBankerList = 28,

	//�����ׯ
	CL_DialRoleGetBankerFirstSeat = 29,
	GM_DialRoleGetBankerFirstSeat = 30,
	MG_DialRoleGetBankerFirstSeat = 31,
	LC_DialRoleGetBankerFirstSeat = 32,

	//����뿪����
	CL_DialRoleLeaveBankerList = 33,
	GM_DialRoleLeaveBankerList = 34,
	MG_DialRoleLeaveBankerList = 35,
	LC_DialRoleLeaveBankerList = 36,

	//���ȡ����ׯ
	CL_DialRoleCanelBankerSeat = 37,
	GM_DialRoleCanelBankerSeat = 38,
	MG_DialRoleCanelBankerSeat = 39,
	LC_DialRoleCanelBankerSeat = 40,

	//ׯ�ҽ�ұ仯��ʱ��
	MG_DialBankerUserGlobelChange = 41,
	LC_DialBankerUserGlobelChange = 42,

	//
	MG_DialWriteBankerUserGlobelMsg = 43,
	LC_DialWriteBankerUserGlobelMsg = 44,

	//��ҽ�����ϯ
	CL_DialRoleJoinVipSeat = 45,
	GM_DialRoleJoinVipSeat = 46,
	MG_DialRoleJoinVipSeat = 47,
	LC_DialRoleJoinVipSeat = 48,

	CL_DialRoleLeaveVipSeat = 49,
	GM_DialRoleLeaveVipSeat = 50,
	MG_DialRoleLeaveVipSeat = 51,
	LC_DialRoleLeaveVipSeat = 52,

	MG_DialRoleBeLeaveVipSeat = 53,
	LC_DialRoleBeLeaveVipSeat = 54,

	//����ϯ����
	CL_DialGetNormalSeatInfo = 55,
	GM_DialGetNormalSeatInfo = 56,
	MG_DialGetNormalSeatInfo = 57,
	LC_DialGetNormalSeatInfo = 58,

	//
	MG_DialVipSeatGlobelChange = 59,
	LC_DialVipSeatGlobelChange = 60,

	MG_DialTableStopUpdate = 61,
	LC_DialTableStopUpdate = 62,

	MG_DialVipGlobelChange = 63,
	LC_DialVipGlobelChange = 64,

	//������ע
	CL_DialRoleBetGlobelByLog = 65,
	GM_DialRoleBetGlobelByLog = 66,
	MG_DialRoleBetGlobelByLog = 67,
	LC_DialRoleBetGlobelByLog = 68,

};
struct DialRoleInfo
{
	DWORD	dwUserID;
	DWORD	GlobelSum;
	DWORD	dwFaceID;
	TCHAR   NickName[MAX_NICKNAME + 1];
};
struct CL_Cmd_DialRoleJoinTable : public NetCmd
{

};
struct GM_Cmd_DialRoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_DialRoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_DIAL_ClientSum];//��ע���������
	BYTE			TableAreaData[MAX_DIAL_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_DIAL_ClientSum];//���̱���
	DialRoleInfo	TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	DialRoleInfo	TableWriteBankerList[MAX_DIALSHOWBRAND_Sum];
	DialRoleInfo	VipSeatList[MAX_DIALVIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	WORD			TableResultIndex;
};
struct LC_Cmd_DialRoleJoinTable : public NetCmd
{
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_DIAL_ClientSum];//��ע���������
	BYTE			TableAreaData[MAX_DIAL_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_DIAL_ClientSum];//���̱���
	DialRoleInfo	TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	DialRoleInfo	TableWriteBankerList[MAX_DIALSHOWBRAND_Sum];
	DialRoleInfo	VipSeatList[MAX_DIALVIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	WORD			TableResultIndex;
};

struct CL_Cmd_DialRoleBetGlobel : public NetCmd
{
	BYTE			Index;
	DWORD			AddGlobel;
};
struct GM_Cmd_DialRoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	BYTE			Index;
	DWORD			AddGlobel;
};
struct MG_Cmd_DialRoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};
struct LC_Cmd_DialRoleBetGlobel : public NetCmd
{
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};

struct CL_Cmd_DialGetBankerList : public NetCmd
{

};
struct GM_Cmd_DialGetBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_DialGetBankerList : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwMySeatIndex;
	DialRoleInfo	TableWriteBankerList[MAX_DIALSHOWBRAND_Sum];
};
struct LC_Cmd_DialGetBankerList : public NetCmd
{
	DWORD			dwMySeatIndex;
	DialRoleInfo	TableWriteBankerList[MAX_DIALSHOWBRAND_Sum];
};

struct CL_Cmd_DialRoleJoinBankerList : public NetCmd
{

};
struct GM_Cmd_DialRoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_DialRoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
	DWORD		SeatIndex;
};
struct LC_Cmd_DialRoleJoinBankerList : public NetCmd
{
	bool		Result;
	DWORD		SeatIndex;
};

struct CL_Cmd_DialRoleGetBankerFirstSeat : public NetCmd
{

};
struct GM_Cmd_DialRoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_DialRoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_DialRoleGetBankerFirstSeat : public NetCmd
{
	bool		Result;
};


struct CL_Cmd_DialRoleLeaveBankerList : public NetCmd
{

};
struct GM_Cmd_DialRoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_DialRoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
};
struct LC_Cmd_DialRoleLeaveBankerList : public NetCmd
{
	bool			Result;
};

struct CL_Cmd_DialRoleCanelBankerSeat : public NetCmd
{

};
struct GM_Cmd_DialRoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_DialRoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_DialRoleCanelBankerSeat : public NetCmd
{
	bool		Result;
};

struct MG_Cmd_DialBankerListChange : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_DialBankerListChange : public NetCmd
{

};

struct MG_Cmd_DialBankerUserChange : public NetCmd  //GameServer�����ʱ�� ��Ҫ�黹��ҵĽ������
{
	DWORD			dwUserID;
	DialRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};
struct LC_Cmd_DialBankerUserChange : public NetCmd
{
	DialRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};

struct MG_Cmd_DialTableJoinBegin : public NetCmd
{
	DWORD			dwUserID;
	BYTE			TableAreaData[MAX_DIAL_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_DIAL_ClientSum];//���̱���
};
struct LC_Cmd_DialTableJoinBegin : public NetCmd
{
	BYTE			TableAreaData[MAX_DIAL_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_DIAL_ClientSum];//���̱���
};

struct MG_Cmd_DialTableUpdate : public NetCmd
{
	DWORD		dwUserID;
	UINT64		TableBetAreaGlobel[MAX_DIAL_ClientSum];
};
struct LC_Cmd_DialTableUpdate : public NetCmd
{
	UINT64		TableBetAreaGlobel[MAX_DIAL_ClientSum];
};
struct DialUserGlobelInfo
{
	DWORD	dwUserID;
	DWORD	dwGlobelSum;
};
struct MG_Cmd_DialTableJoinEnd : public NetCmd
{
	DWORD		dwUserID;
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_DIAL_ClientSum];
	WORD		TableResultIndex;
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	DialUserGlobelInfo BankerUserGlobelInfo;
	DialUserGlobelInfo BankerListGlobelInfo[MAX_DIALSHOWBRAND_Sum];
	DialUserGlobelInfo VipGlobelInfo[MAX_DIALVIPSEAT_Sum];
};
struct LC_Cmd_DialTableJoinEnd : public NetCmd
{
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_DIAL_ClientSum];
	WORD		TableResultIndex;
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	DialUserGlobelInfo BankerUserGlobelInfo;
	DialUserGlobelInfo BankerListGlobelInfo[MAX_DIALSHOWBRAND_Sum];
	DialUserGlobelInfo VipGlobelInfo[MAX_DIALVIPSEAT_Sum];
};

struct CL_Cmd_DialRoleLeaveTable : public NetCmd
{

};
struct GM_Cmd_DialRoleLeaveTable : public NetCmd
{
	DWORD		dwUserID;
};

struct MG_Cmd_DialBankerUserGlobelChange : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};
struct LC_Cmd_DialBankerUserGlobelChange : public NetCmd
{
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};

struct MG_Cmd_DialWriteBankerUserGlobelMsg : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_DialWriteBankerUserGlobelMsg : public NetCmd
{

};

struct CL_Cmd_DialRoleJoinVipSeat : public NetCmd
{
	BYTE	VipSeatIndex;
};
struct GM_Cmd_DialRoleJoinVipSeat : public NetCmd
{
	DWORD	dwUserID;
	BYTE	VipSeatIndex;
};
struct MG_Cmd_DialRoleJoinVipSeat : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	BYTE			VipSeatIndex;
	DialRoleInfo	DestUserInfo;

};
struct LC_Cmd_DialRoleJoinVipSeat : public NetCmd
{
	bool			Result;
	BYTE			VipSeatIndex;
	DialRoleInfo	DestUserInfo;
};

struct CL_Cmd_DialRoleLeaveVipSeat : public NetCmd
{

};
struct GM_Cmd_DialRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
};
struct MG_Cmd_DialRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};
struct LC_Cmd_DialRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};

struct MG_Cmd_DialRoleBeLeaveVipSeat : public NetCmd
{
	DWORD		   dwUserID;
	DialRoleInfo DestRoleInfo;
};
struct LC_Cmd_DialRoleBeLeaveVipSeat : public NetCmd
{
	DialRoleInfo DestRoleInfo;
};
struct CL_Cmd_DialGetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
};
struct GM_Cmd_DialGetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
};
struct MG_Cmd_DialGetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
	DWORD			TotalRoleSum;
	DialRoleInfo	Array[MAX_DIALNORMAL_PAGESUM];
};
struct LC_Cmd_DialGetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
	DWORD			TotalRoleSum;
	DialRoleInfo	Array[MAX_DIALNORMAL_PAGESUM];
};
struct MG_Cmd_DialVipSeatGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct LC_Cmd_DialVipSeatGlobelChange : public NetCmd
{
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct MG_Cmd_DialTableStopUpdate : public NetCmd
{
	DWORD			dwUserID;
};
struct LC_Cmd_DialTableStopUpdate : public NetCmd
{

};
struct MG_Cmd_DialVipGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct LC_Cmd_DialVipGlobelChange : public NetCmd
{
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct CL_Cmd_DialRoleBetGlobelByLog : public NetCmd
{
	DWORD			betGlobel[MAX_DIAL_ClientSum];
};
struct GM_Cmd_DialRoleBetGlobelByLog : public NetCmd
{
	DWORD			dwUserID;
	DWORD			betGlobel[MAX_DIAL_ClientSum];
};
struct MG_Cmd_DialRoleBetGlobelByLog : public NetCmd
{
	DWORD			dwUserID;
	DWORD			AllGlobel;
	bool			Result;
	DWORD			betGlobel[MAX_DIAL_ClientSum];
};
struct LC_Cmd_DialRoleBetGlobelByLog : public NetCmd
{
	bool			Result;
	DWORD			betGlobel[MAX_DIAL_ClientSum];
};
//Main_NiuNiu
enum NiuNiuSub
{
	//��ҽ���ţţ������
	CL_RoleJoinTable   = 1,
	GM_RoleJoinTable   = 2,
	MG_RoleJoinTable   = 3,
	LC_RoleJoinTable   = 4,

	//����뿪ţţ����
	CL_RoleLeaveTable  = 5,
	GM_RoleLeaveTable  = 6,

	//�����ע
	CL_RoleBetGlobel   = 7,
	GM_RoleBetGlobel   = 8,
	MG_RoleBetGlobel   = 9,
	LC_RoleBetGlobel   = 10,

	//��һ�ȡׯ���Ŷ��б�
	CL_GetBankerList   = 11,
	GM_GetBankerList   = 12,
	MG_GetBankerList   = 13,
	LC_GetBankerList   = 14,

	//ׯ�Ҷ��б仯��ʱ��
	MG_BankerListChange = 15,
	LC_BankerListChange = 16,

	//ׯ�ҷ����仯��ʱ��
	MG_BankerUserChange = 17,
	LC_BankerUserChange = 18,

	//���ӽ�����ע�׶�
	MG_TableJoinBegin   = 19,
	LC_TableJoinBegin   = 20,

	//��������ע�׶εĸ�������
	MG_TableUpdate      = 21,
	LC_TableUpdate		= 22,

	//���ӽ��뿪�ƽ׶�
	MG_TableJoinEnd		= 23,
	LC_TableJoinEnd		= 24,

	//�����ׯ
	CL_RoleJoinBankerList = 25,
	GM_RoleJoinBankerList = 26,
	MG_RoleJoinBankerList = 27,
	LC_RoleJoinBankerList = 28,

	//�����ׯ
	CL_RoleGetBankerFirstSeat = 29,
	GM_RoleGetBankerFirstSeat = 30,
	MG_RoleGetBankerFirstSeat = 31,
	LC_RoleGetBankerFirstSeat = 32,

	//����뿪����
	CL_RoleLeaveBankerList    = 33,
	GM_RoleLeaveBankerList    = 34,
	MG_RoleLeaveBankerList	  = 35,
	LC_RoleLeaveBankerList    = 36,

	//���ȡ����ׯ
	CL_RoleCanelBankerSeat    = 37,
	GM_RoleCanelBankerSeat	  = 38,
	MG_RoleCanelBankerSeat    = 39,
	LC_RoleCanelBankerSeat    = 40,

	//ׯ�ҽ�ұ仯��ʱ��
	MG_BankerUserGlobelChange = 41,
	LC_BankerUserGlobelChange = 42,

	//
	MG_WriteBankerUserGlobelMsg = 43,
	LC_WriteBankerUserGlobelMsg = 44,

	//��ҽ�����ϯ
	CL_RoleJoinVipSeat = 45,
	GM_RoleJoinVipSeat = 46,
	MG_RoleJoinVipSeat = 47,
	LC_RoleJoinVipSeat = 48,

	CL_RoleLeaveVipSeat = 49,
	GM_RoleLeaveVipSeat = 50,
	MG_RoleLeaveVipSeat = 51,
	LC_RoleLeaveVipSeat = 52,

	MG_RoleBeLeaveVipSeat = 53,
	LC_RoleBeLeaveVipSeat = 54,

	//����ϯ����
	CL_GetNormalSeatInfo = 55,
	GM_GetNormalSeatInfo = 56,
	MG_GetNormalSeatInfo = 57,
	LC_GetNormalSeatInfo = 58,

	//
	MG_VipSeatGlobelChange = 59,
	LC_VipSeatGlobelChange = 60,

	MG_TableStopUpdate = 61,
	LC_TableStopUpdate = 62,

	MG_VipGlobelChange = 63,
	LC_VipGlobelChange = 64,

	//����ӵ�

	CL_CreateNiuNiuRoom = 100,
	GM_CreateNiuNiuRoom = 101,

	//������ѽ���һ������
	CL_RequestJoinNiuNiuRoom = 105,
	GC_RequestJoinNiuNiuRoom = 106,
	CG_RequestJoinNiuNiuRoom = 107,
	LC_RequestJoinNiuNiuRoom = 108,

	CL_RequestJoinNiuNiuRoomResult = 109,
	GC_RequestJoinNiuNiuRoomResult = 110,
	CG_RequestJoinNiuNiuRoomResult = 111,
	LC_RequestJoinNiuNiuRoomResult = 112,

	GM_RequestJoinNiuNiuRoom  = 113,
};
struct NiuNiuRoleInfo
{
	DWORD	dwUserID;
	DWORD	GlobelSum;
	DWORD	dwFaceID;
	TCHAR   NickName[MAX_NICKNAME +1];
};
struct CL_Cmd_RoleJoinTable : public NetCmd
{

};
struct GM_Cmd_RoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_RoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_NIUNIU_ClientSum];//��ע���������
	BYTE			TableBrandArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//������ 5x5 ��
	WORD			TableBrandResult[MAX_NIUNIU_ClientSum + 1];//�ƵĽ��
	NiuNiuRoleInfo  TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	NiuNiuRoleInfo	TableWriteBankerList[MAX_SHOWBRAND_Sum];
	NiuNiuRoleInfo	VipSeatList[MAX_VIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	bool			IsNormalRoom;
};
struct LC_Cmd_RoleJoinTable : public NetCmd
{
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_NIUNIU_ClientSum];//��ע���������
	BYTE			TableBrandArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//������ 5x5 ��
	WORD			TableBrandResult[MAX_NIUNIU_ClientSum + 1];//�ƵĽ��
	NiuNiuRoleInfo  TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	NiuNiuRoleInfo	TableWriteBankerList[MAX_SHOWBRAND_Sum];
	NiuNiuRoleInfo	VipSeatList[MAX_VIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	bool			IsNormalRoom;
};

struct CL_Cmd_RoleBetGlobel : public NetCmd
{
	BYTE			Index;
	DWORD			AddGlobel;
};
struct GM_Cmd_RoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	BYTE			Index;
	DWORD			AddGlobel;
};
struct MG_Cmd_RoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};

struct LG_StopFishServer : public NetCmd
{
	INT rcpid;
	INT64 closeTime;
};

struct LG_ReloadFishConfig :public NetCmd
{
	INT rpcid;
};

struct tagAchDataMap //�ɾ�
{
	INT	achtype;      //�ɾ�����
	UINT64 finishtime; //���ʱ��
	INT gameid;       //��ϷID  60
	INT	liansheng;    //��ʤ����
	INT	lianshu;      //����
	INT	qId;          //�ɾ�ID
	INT	qachNum;      //�ɾ��������
	INT	qachfinishnum;//�ɾ��Ѿ���ɴ���
	UINT64 receivetime;//
	float rewardMoney;//����Ǯ
	INT	titleID;      //�����ƺ�ID
	float titlemoney; //�ƺŽ���Ǯ
};

struct tagClientUserData
{
	double money1;
	double money2;
	char name[MAX_NICKNAME + 1];
	char icon[ICON_LENGTH];
	INT sex;
	INT loginid; //��½������id
	INT deleteAt;   //�˺�״̬ ���׵�״̬Ҫ�������
	INT gmLevel; //gm�ȼ�
	INT robot; //
	INT titleIndex; //�ɾͱ�ʶ
	double gameMaxWinMoney; //���ӯ��
	INT charmValue; //����ֵ
	INT vip; //vip
	INT barrageNum; //��Ļʣ�����
	double taxRatio; //˰��
	INT  cardBg; //�Ʊ�
	INT  headIcon; //ͷ���
	char signature[MAX_SIGNATURE + 1]; //����ǩ��
	int64 taxFreeCardEndTime; //��˰������ʱ��
	double taxFreeCardRate; //��˰��˰��
	INT ratioValue;//����ֵ
	DWORD enterTime;
	INT32 achSize; //�ɾ������С
	tagAchDataMap achDataMap[0]; //�ɾ�����
};

struct LG_UDPClientConnect : public NetCmd
{
	INT			rpcid;
	USHORT		Port;
	ULONG		Addr;
	UINT64		uid;
	bool		isAccess;
	tagClientUserData udata;
};

struct GL_LoginSubGame :public NetCmd
{
	INT32 rpcId;
	UINT64 uid;
	INT32 isReconnect;
	bool loginSubGame;
	INT32 mapModelId;
	ULONG fishIp;
	INT32 fishPort;
};

struct LG_UseGoods :public NetCmd
{
	int64 uid;
	int64 itemId;		//��ƷID
	INT32 itemModelId;  //��Ʒģ��ID
	INT32 itemNum;      //��Ʒ����
	int64 keep_time;    //Ч������ʱ�� ����
};

struct LG_UseGoodsKB :public NetCmd
{
	INT32 seatID;
	UINT32 keepTime;
};

struct LG_UseGoodsBD :public NetCmd
{
	INT32 seatID;
	UINT32 keepTime;
};

struct LG_UpdateMoney :public NetCmd
{
	int64 uid;
	double money1;
	double money2;
	INT reason;
};
struct LG_SyncAddMoney : public NetCmd
{
	int64 add_goldNum;
	INT reason;
};

struct LG_ClientLeave :public NetCmd
{
	INT msgID;
	INT rpcid;
	bool offLine;
	UINT64 uid;
};
struct GL_TickOut :public NetCmd
{
};
struct GL_QuitSubGame : public NetCmd
{
	INT64 uid;
	INT rpcId ; // �������Ϣ���漰���ص��Ļ������봫��rpcId
	bool quitSubGame ;  // �˳�����Ϸ���
	double money1 ;  // ͬ�����
	double money2 ;  // ͬ�����
	INT winNum;		//ͳ��ÿ���ӵ���Ӯ���˳�ʱ������Ӯ����
	INT loseNum;	//ͳ��ÿ���ӵ���Ӯ���˳�ʱ������Ӯ����	
	double winMoney;  //һ����Ϸ��õĽ��
	double loseMoney; //һ����Ϸ���ĵĽ��
	INT32 catchBossFishCount; //��ɱBOSS��������������Ҫ
	INT32 charmValue; //����ֵ
	INT32 achSize;    //�ɾ����ݴ�С
	tagAchDataMap achData[0]; //�ɾ�����
};

struct GL_SyncDataToHall :public NetCmd
{
	INT64	uid;
	double money1; // ���ջ���ֵ
	double money2; // ���ջ���ֵ
//	INT  playerNum; //��Ϸ����
//	string gameRecord; // ��Ϸ���ݼ�¼
	INT  subGameId; //��ϷID
};

struct LC_Cmd_RoleBetGlobel : public NetCmd
{
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};

struct CL_Cmd_GetBankerList : public NetCmd
{

};
struct GM_Cmd_GetBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_GetBankerList : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwMySeatIndex;
	NiuNiuRoleInfo	TableWriteBankerList[MAX_SHOWBRAND_Sum];
};
struct LC_Cmd_GetBankerList : public NetCmd
{
	DWORD			dwMySeatIndex;
	NiuNiuRoleInfo	TableWriteBankerList[MAX_SHOWBRAND_Sum];
};

struct CL_Cmd_RoleJoinBankerList : public NetCmd
{

};
struct GM_Cmd_RoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_RoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
	DWORD		SeatIndex;
};
struct LC_Cmd_RoleJoinBankerList : public NetCmd
{
	bool		Result;
	DWORD		SeatIndex;
};

struct CL_Cmd_RoleGetBankerFirstSeat : public NetCmd
{

};
struct GM_Cmd_RoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_RoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_RoleGetBankerFirstSeat : public NetCmd
{
	bool		Result;
};


struct CL_Cmd_RoleLeaveBankerList : public NetCmd
{

};
struct GM_Cmd_RoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_RoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
};
struct LC_Cmd_RoleLeaveBankerList : public NetCmd
{
	bool			Result;
};

struct CL_Cmd_RoleCanelBankerSeat : public NetCmd
{

};
struct GM_Cmd_RoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_RoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_RoleCanelBankerSeat : public NetCmd
{
	bool		Result;
};

struct MG_Cmd_BankerListChange : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_BankerListChange : public NetCmd
{

};

struct MG_Cmd_BankerUserChange : public NetCmd  //GameServer�����ʱ�� ��Ҫ�黹��ҵĽ������
{
	DWORD			dwUserID;
	NiuNiuRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};
struct LC_Cmd_BankerUserChange : public NetCmd
{
	NiuNiuRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};

struct MG_Cmd_TableJoinBegin : public NetCmd
{
	DWORD			dwUserID;
};
struct LC_Cmd_TableJoinBegin : public NetCmd
{

};

struct MG_Cmd_TableUpdate : public NetCmd
{
	DWORD		dwUserID;
	UINT64		TableBetAreaGlobel[MAX_NIUNIU_ClientSum];
};
struct LC_Cmd_TableUpdate : public NetCmd
{
	UINT64		TableBetAreaGlobel[MAX_NIUNIU_ClientSum];
};
struct NiuNiuUserGlobelInfo
{
	DWORD	dwUserID;
	DWORD	dwGlobelSum;
};
struct MG_Cmd_TableJoinEnd : public NetCmd
{
	DWORD		dwUserID;
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_NIUNIU_ClientSum];
	BYTE		TableBrandArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//������ 5x5 ��
	WORD		TableBrandResult[MAX_NIUNIU_ClientSum + 1];//�ƵĽ��
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	NiuNiuUserGlobelInfo BankerUserGlobelInfo;
	NiuNiuUserGlobelInfo BankerListGlobelInfo[MAX_SHOWBRAND_Sum];
	NiuNiuUserGlobelInfo VipGlobelInfo[MAX_VIPSEAT_Sum];
};
struct LC_Cmd_TableJoinEnd : public NetCmd
{
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_NIUNIU_ClientSum];
	BYTE		TableBrandArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum];//������ 5x5 ��
	WORD		TableBrandResult[MAX_NIUNIU_ClientSum + 1];//�ƵĽ��
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	NiuNiuUserGlobelInfo BankerUserGlobelInfo;
	NiuNiuUserGlobelInfo BankerListGlobelInfo[MAX_SHOWBRAND_Sum];
	NiuNiuUserGlobelInfo VipGlobelInfo[MAX_VIPSEAT_Sum];
};

struct CL_Cmd_RoleLeaveTable : public NetCmd
{

};
struct GM_Cmd_RoleLeaveTable : public NetCmd
{
	DWORD		dwUserID;
};

struct MG_Cmd_BankerUserGlobelChange : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};
struct LC_Cmd_BankerUserGlobelChange : public NetCmd
{
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};

struct MG_Cmd_WriteBankerUserGlobelMsg : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_WriteBankerUserGlobelMsg : public NetCmd
{

};

struct CL_Cmd_RoleJoinVipSeat : public NetCmd
{
	BYTE	VipSeatIndex;
};
struct GM_Cmd_RoleJoinVipSeat : public NetCmd
{
	DWORD	dwUserID;
	BYTE	VipSeatIndex;
};
struct MG_Cmd_RoleJoinVipSeat : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	BYTE			VipSeatIndex;
	NiuNiuRoleInfo	DestUserInfo;
	
};
struct LC_Cmd_RoleJoinVipSeat : public NetCmd
{
	bool			Result;
	BYTE			VipSeatIndex;
	NiuNiuRoleInfo	DestUserInfo;
};

struct CL_Cmd_RoleLeaveVipSeat : public NetCmd
{

};
struct GM_Cmd_RoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
};
struct MG_Cmd_RoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};
struct LC_Cmd_RoleLeaveVipSeat : public NetCmd
{
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};

struct MG_Cmd_RoleBeLeaveVipSeat : public NetCmd
{
	DWORD		   dwUserID;
	NiuNiuRoleInfo DestRoleInfo;
};
struct LC_Cmd_RoleBeLeaveVipSeat : public NetCmd
{
	NiuNiuRoleInfo DestRoleInfo;
};
struct CL_Cmd_GetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
};
struct GM_Cmd_GetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
};
struct MG_Cmd_GetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
	DWORD			TotalRoleSum;
	NiuNiuRoleInfo  Array[MAX_NORMAL_PAGESUM];
};
struct LC_Cmd_GetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
	DWORD			TotalRoleSum;
	NiuNiuRoleInfo  Array[MAX_NORMAL_PAGESUM];
};
struct MG_Cmd_VipSeatGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct LC_Cmd_VipSeatGlobelChange : public NetCmd
{
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct MG_Cmd_TableStopUpdate : public NetCmd
{
	DWORD			dwUserID;
};
struct LC_Cmd_TableStopUpdate : public NetCmd
{

};
struct MG_Cmd_VipGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct LC_Cmd_VipGlobelChange : public NetCmd
{
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct CL_Cmd_CreateNiuNiuRoom : public NetCmd
{

};
struct GM_Cmd_CreateNiuNiuRoom : public NetCmd
{
	DWORD			dwUserID;
};
struct CL_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	//DWORD			dwDestUserID; //Ŀ�����
	TCHAR			strDestNickName[MAX_NICKNAME + 1];
};
struct GC_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD			dwSrcUserID;
	DWORD			dwDestUserID;
};
struct CG_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD			dwSrcUserID;
	DWORD			dwDestUserID;
	TCHAR			strSrcNickName[MAX_NICKNAME + 1];
};
struct LC_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD			dwSrcUserID;
	TCHAR			strSrcNickName[MAX_NICKNAME + 1];
};
struct CL_Cmd_RequestJoinNiuNiuRoomResult : public NetCmd
{
	DWORD			dwRequestUserID;
	bool			Result;
};
struct GC_Cmd_RequestJoinNiuNiuRoomResult : public NetCmd
{
	DWORD			dwRequestUserID;
	DWORD			dwDestUserID;
	bool			Result;
};
struct CG_Cmd_RequestJoinNiuNiuRoomResult : public NetCmd
{
	DWORD			dwRequestUserID;
	DWORD			dwDestUserID;
	bool			Result;
};
struct LC_Cmd_RequestJoinNiuNiuRoomResult : public NetCmd
{
	DWORD			dwDestUserID;
	bool			Result;
};
struct GM_Cmd_RequestJoinNiuNiuRoom : public NetCmd
{
	DWORD			dwSrcUserID;
	DWORD			dwDestUserID;
};
//Main_Char

enum CharSub
{
	CL_LoadAllCharInfo = 1,
	LC_LoadAllCharInfo = 2,

	CL_LoadCharListByUserID = 3,
	LC_LoadCharListByUserID = 4,

	CL_SendCharInfo = 5,
	GC_SendCharInfo = 6,
	CG_SendCharInfo = 7,
	LC_SendCharInfo = 8,
};
struct CL_Cmd_LoadAllCharInfo : public NetCmd
{
	
};
struct LC_Cmd_LoadAllCharInfo : public NetCmd //�ְ��䳤����
{
	BYTE				States;
	WORD				Sum;
	tagRoleCharInfo		Array[1];
};
struct CL_Cmd_LoadCharListByUserID : public NetCmd
{
	DWORD				SrcUserID;
};
struct LC_Cmd_LoadCharListByUserID : public NetCmd
{
	DWORD				SrcUserID;
	BYTE				States;
	WORD				Sum;
	tagRoleCharInfo		Array[1];
};
struct CL_Cmd_SendCharInfo : public NetCmd
{
	tagRoleCharInfo		MessageInfo;
};
struct GC_Cmd_SendCharInfo : public NetCmd
{
	tagRoleCharInfo		MessageInfo;
};
struct CG_Cmd_SendCharInfo : public NetCmd
{
	tagRoleCharInfo		MessageInfo;
};
struct LC_Cmd_SendCharInfo : public NetCmd
{
	tagRoleCharInfo		MessageInfo;
};

//Main_RelationRequest
enum RelationRequestSub
{
	//��������
	CL_LoadRelationRequest = 1,
	LC_LoadRelationRequest = 2,

	//
	CL_SendRelationRequest = 3,
	GC_SendRelationRequest = 4,
	CG_SendRelationRequest = 5,
	LC_SendRelationRequest = 6,
	LC_BeAddRelationRequest = 7,

	//
	CL_HandleRelationRequest = 8,
	GC_HandleRelationRequest = 9,
	CG_HandleRelationRequest = 10,
	LC_HandleRelationRequest = 11,
	GC_DestDelRelation = 12,
	CG_DestDelRelation = 13,

	//
	LC_DelRelationRequest = 14,
};
struct CL_Cmd_LoadRelationRequest : public NetCmd
{

};
struct LC_Cmd_LoadRelationRequest : public NetCmd
{
	BYTE				States;
	WORD				Sum;
	tagRelationRequest	Array[1];
};

struct CL_Cmd_SendRelationRequest : public NetCmd
{
	tagRelationRequest		Info;
};
struct GC_Cmd_SendRelationRequest : public NetCmd
{
	tagRelationRequest		Info;
};
struct CG_Cmd_SendRelationRequest : public NetCmd
{
	tagRelationRequest		Info;
};
struct LC_Cmd_SendRelationRequest : public NetCmd
{
	bool					Result;
	BYTE					ErrorID;
	tagRelationRequest		Info;
};
struct LC_Cmd_BeAddRelationRequest : public NetCmd
{
	tagRelationRequest		Info;
};

struct CL_Cmd_HandleRelationRequest : public NetCmd
{
	DWORD					ID;
	bool					Result;
};
struct GC_Cmd_HandleRelationRequest : public NetCmd
{
	tagRelationRequest      Info;
	bool					Result;
};
struct CG_Cmd_HandleRelationRequest : public NetCmd
{
	tagRelationRequest      Info;
	bool					Result;
};
struct LC_Cmd_HandleRelationRequest : public NetCmd
{
	tagRelationRequest      Info;
	bool					Result;
};
struct GC_Cmd_DestDelRelation : public NetCmd
{
	tagRelationRequest      Info;
};
struct CG_Cmd_DestDelRelation : public NetCmd
{
	tagRelationRequest      Info;
};
struct LC_Cmd_DelRelationRequest : public NetCmd
{
	DWORD					ID;
};



//Main_Car
enum CarSub
{
	//��ҽ���ţţ������
	CL_CarRoleJoinTable = 1,
	GM_CarRoleJoinTable = 2,
	MG_CarRoleJoinTable = 3,
	LC_CarRoleJoinTable = 4,

	//����뿪ţţ����
	CL_CarRoleLeaveTable = 5,
	GM_CarRoleLeaveTable = 6,

	//�����ע
	CL_CarRoleBetGlobel = 7,
	GM_CarRoleBetGlobel = 8,
	MG_CarRoleBetGlobel = 9,
	LC_CarRoleBetGlobel = 10,

	//��һ�ȡׯ���Ŷ��б�
	CL_CarGetBankerList = 11,
	GM_CarGetBankerList = 12,
	MG_CarGetBankerList = 13,
	LC_CarGetBankerList = 14,

	//ׯ�Ҷ��б仯��ʱ��
	MG_CarBankerListChange = 15,
	LC_CarBankerListChange = 16,

	//ׯ�ҷ����仯��ʱ��
	MG_CarBankerUserChange = 17,
	LC_CarBankerUserChange = 18,

	//���ӽ�����ע�׶�
	MG_CarTableJoinBegin = 19,
	LC_CarTableJoinBegin = 20,

	//��������ע�׶εĸ�������
	MG_CarTableUpdate = 21,
	LC_CarTableUpdate = 22,

	//���ӽ��뿪�ƽ׶�
	MG_CarTableJoinEnd = 23,
	LC_CarTableJoinEnd = 24,

	//�����ׯ
	CL_CarRoleJoinBankerList = 25,
	GM_CarRoleJoinBankerList = 26,
	MG_CarRoleJoinBankerList = 27,
	LC_CarRoleJoinBankerList = 28,

	//�����ׯ
	CL_CarRoleGetBankerFirstSeat = 29,
	GM_CarRoleGetBankerFirstSeat = 30,
	MG_CarRoleGetBankerFirstSeat = 31,
	LC_CarRoleGetBankerFirstSeat = 32,

	//����뿪����
	CL_CarRoleLeaveBankerList = 33,
	GM_CarRoleLeaveBankerList = 34,
	MG_CarRoleLeaveBankerList = 35,
	LC_CarRoleLeaveBankerList = 36,

	//���ȡ����ׯ
	CL_CarRoleCanelBankerSeat = 37,
	GM_CarRoleCanelBankerSeat = 38,
	MG_CarRoleCanelBankerSeat = 39,
	LC_CarRoleCanelBankerSeat = 40,

	//ׯ�ҽ�ұ仯��ʱ��
	MG_CarBankerUserGlobelChange = 41,
	LC_CarBankerUserGlobelChange = 42,

	//
	MG_CarWriteBankerUserGlobelMsg = 43,
	LC_CarWriteBankerUserGlobelMsg = 44,

	//��ҽ�����ϯ
	CL_CarRoleJoinVipSeat = 45,
	GM_CarRoleJoinVipSeat = 46,
	MG_CarRoleJoinVipSeat = 47,
	LC_CarRoleJoinVipSeat = 48,

	CL_CarRoleLeaveVipSeat = 49,
	GM_CarRoleLeaveVipSeat = 50,
	MG_CarRoleLeaveVipSeat = 51,
	LC_CarRoleLeaveVipSeat = 52,

	MG_CarRoleBeLeaveVipSeat = 53,
	LC_CarRoleBeLeaveVipSeat = 54,

	//����ϯ����
	CL_CarGetNormalSeatInfo = 55,
	GM_CarGetNormalSeatInfo = 56,
	MG_CarGetNormalSeatInfo = 57,
	LC_CarGetNormalSeatInfo = 58,

	//
	MG_CarVipSeatGlobelChange = 59,
	LC_CarVipSeatGlobelChange = 60,

	MG_CarTableStopUpdate = 61,
	LC_CarTableStopUpdate = 62,

	MG_CarVipGlobelChange = 63,
	LC_CarVipGlobelChange = 64,

	//������ע
	CL_CarRoleBetGlobelByLog = 65,
	GM_CarRoleBetGlobelByLog = 66,
	MG_CarRoleBetGlobelByLog = 67,
	LC_CarRoleBetGlobelByLog = 68,
};
struct CarRoleInfo
{
	DWORD	dwUserID;
	DWORD	GlobelSum;
	DWORD	dwFaceID;
	TCHAR   NickName[MAX_NICKNAME + 1];
};
struct CL_Cmd_CarRoleJoinTable : public NetCmd
{

};
struct GM_Cmd_CarRoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_CarRoleJoinTable : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_CAR_ClientSum];//��ע���������
	CarRoleInfo	TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	CarRoleInfo	TableWriteBankerList[MAX_CARSHOWBRAND_Sum];
	CarRoleInfo	VipSeatList[MAX_CARVIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	BYTE			TableResultIndex;
};
struct LC_Cmd_CarRoleJoinTable : public NetCmd
{
	bool			Result;
	//�������� �����ϵ�����
	BYTE			TableStates;//����״̬
	BYTE			TableStatesUpdateSec;//���ӵ�ǰ״̬������
	UINT64			TableBetAreaGlobel[MAX_CAR_ClientSum];//��ע���������
	CarRoleInfo	TableBankerInfo;
	BYTE			TableBankerUseGameSum;//��ǰׯ���Ѿ�������Ϸ�ľ���
	CarRoleInfo	TableWriteBankerList[MAX_CARSHOWBRAND_Sum];
	CarRoleInfo	VipSeatList[MAX_CARVIPSEAT_Sum];//���ϯ�����
	BYTE			TableGameSum;//��¼��Ϸ����
	DWORD			TableResultLog;//��Ϸ�����¼
	BYTE			TableResultIndex;
};

struct CL_Cmd_CarRoleBetGlobel : public NetCmd
{
	BYTE			Index;
	DWORD			AddGlobel;
};
struct GM_Cmd_CarRoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	BYTE			Index;
	DWORD			AddGlobel;
};
struct MG_Cmd_CarRoleBetGlobel : public NetCmd
{
	DWORD			dwUserID;
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};
struct LC_Cmd_CarRoleBetGlobel : public NetCmd
{
	DWORD			AddGlobel;
	BYTE            Index;
	bool			Result;
};

struct CL_Cmd_CarGetBankerList : public NetCmd
{

};
struct GM_Cmd_CarGetBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_CarGetBankerList : public NetCmd
{
	DWORD			dwUserID;
	DWORD			dwMySeatIndex;
	CarRoleInfo	TableWriteBankerList[MAX_CARSHOWBRAND_Sum];
};
struct LC_Cmd_CarGetBankerList : public NetCmd
{
	DWORD			dwMySeatIndex;
	CarRoleInfo	TableWriteBankerList[MAX_CARSHOWBRAND_Sum];
};

struct CL_Cmd_CarRoleJoinBankerList : public NetCmd
{

};
struct GM_Cmd_CarRoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_CarRoleJoinBankerList : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
	DWORD		SeatIndex;
};
struct LC_Cmd_CarRoleJoinBankerList : public NetCmd
{
	bool		Result;
	DWORD		SeatIndex;
};

struct CL_Cmd_CarRoleGetBankerFirstSeat : public NetCmd
{

};
struct GM_Cmd_CarRoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_CarRoleGetBankerFirstSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_CarRoleGetBankerFirstSeat : public NetCmd
{
	bool		Result;
};


struct CL_Cmd_CarRoleLeaveBankerList : public NetCmd
{

};
struct GM_Cmd_CarRoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_CarRoleLeaveBankerList : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
};
struct LC_Cmd_CarRoleLeaveBankerList : public NetCmd
{
	bool			Result;
};

struct CL_Cmd_CarRoleCanelBankerSeat : public NetCmd
{

};
struct GM_Cmd_CarRoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
};
struct MG_Cmd_CarRoleCanelBankerSeat : public NetCmd
{
	DWORD		dwUserID;
	bool		Result;
};
struct LC_Cmd_CarRoleCanelBankerSeat : public NetCmd
{
	bool		Result;
};

struct MG_Cmd_CarBankerListChange : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_CarBankerListChange : public NetCmd
{

};

struct MG_Cmd_CarBankerUserChange : public NetCmd  //GameServer�����ʱ�� ��Ҫ�黹��ҵĽ������
{
	DWORD			dwUserID;
	CarRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};
struct LC_Cmd_CarBankerUserChange : public NetCmd
{
	CarRoleInfo	BankerUserInfo;
	BYTE			GameSum;
};

struct MG_Cmd_CarTableJoinBegin : public NetCmd
{
	DWORD			dwUserID;
	//BYTE			TableAreaData[MAX_CAR_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_CAR_ClientSum];//���̱���
};
struct LC_Cmd_CarTableJoinBegin : public NetCmd
{
	//BYTE			TableAreaData[MAX_CAR_GameSum];//��������
	BYTE			TableAreaDataRate[MAX_CAR_ClientSum];//���̱���
};

struct MG_Cmd_CarTableUpdate : public NetCmd
{
	DWORD		dwUserID;
	UINT64		TableBetAreaGlobel[MAX_CAR_ClientSum];
};
struct LC_Cmd_CarTableUpdate : public NetCmd
{
	UINT64		TableBetAreaGlobel[MAX_CAR_ClientSum];
};
struct CarUserGlobelInfo
{
	DWORD	dwUserID;
	DWORD	dwGlobelSum;
};
struct MG_Cmd_CarTableJoinEnd : public NetCmd
{
	DWORD		dwUserID;
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_CAR_ClientSum];
	BYTE		TableResultIndex;
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	CarUserGlobelInfo BankerUserGlobelInfo;
	CarUserGlobelInfo BankerListGlobelInfo[MAX_CARSHOWBRAND_Sum];
	CarUserGlobelInfo VipGlobelInfo[MAX_CARVIPSEAT_Sum];
};
struct LC_Cmd_CarTableJoinEnd : public NetCmd
{
	INT64		AddGlobelSum;
	UINT64		TableBetAreaGlobel[MAX_CAR_ClientSum];
	BYTE		TableResultIndex;
	BYTE		TableGameSum;//��¼��Ϸ����
	DWORD		TableResultLog;//��Ϸ�����¼
	//��ұ仯
	CarUserGlobelInfo BankerUserGlobelInfo;
	CarUserGlobelInfo BankerListGlobelInfo[MAX_CARSHOWBRAND_Sum];
	CarUserGlobelInfo VipGlobelInfo[MAX_CARVIPSEAT_Sum];
};

struct CL_Cmd_CarRoleLeaveTable : public NetCmd
{

};
struct GM_Cmd_CarRoleLeaveTable : public NetCmd
{
	DWORD		dwUserID;
};

struct MG_Cmd_CarBankerUserGlobelChange : public NetCmd
{
	DWORD		dwUserID;
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};
struct LC_Cmd_CarBankerUserGlobelChange : public NetCmd
{
	DWORD		dwBankerUserID;
	DWORD		dwBankerGlobelSum;
};

struct MG_Cmd_CarWriteBankerUserGlobelMsg : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_CarWriteBankerUserGlobelMsg : public NetCmd
{

};

struct CL_Cmd_CarRoleJoinVipSeat : public NetCmd
{
	BYTE	VipSeatIndex;
};
struct GM_Cmd_CarRoleJoinVipSeat : public NetCmd
{
	DWORD	dwUserID;
	BYTE	VipSeatIndex;
};
struct MG_Cmd_CarRoleJoinVipSeat : public NetCmd
{
	DWORD			dwUserID;
	bool			Result;
	BYTE			VipSeatIndex;
	CarRoleInfo	DestUserInfo;

};
struct LC_Cmd_CarRoleJoinVipSeat : public NetCmd
{
	bool			Result;
	BYTE			VipSeatIndex;
	CarRoleInfo	DestUserInfo;
};

struct CL_Cmd_CarRoleLeaveVipSeat : public NetCmd
{

};
struct GM_Cmd_CarRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
};
struct MG_Cmd_CarRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};
struct LC_Cmd_CarRoleLeaveVipSeat : public NetCmd
{
	DWORD	dwDestUserID;
	BYTE	VipSeatIndex;
};

struct MG_Cmd_CarRoleBeLeaveVipSeat : public NetCmd
{
	DWORD		   dwUserID;
	CarRoleInfo DestRoleInfo;
};
struct LC_Cmd_CarRoleBeLeaveVipSeat : public NetCmd
{
	CarRoleInfo DestRoleInfo;
};
struct CL_Cmd_CarGetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
};
struct GM_Cmd_CarGetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
};
struct MG_Cmd_CarGetNormalSeatInfo : public NetCmd
{
	DWORD			dwUserID;
	DWORD			Page;
	DWORD			TotalRoleSum;
	CarRoleInfo	Array[MAX_CARNORMAL_PAGESUM];
};
struct LC_Cmd_CarGetNormalSeatInfo : public NetCmd
{
	DWORD			Page;
	DWORD			TotalRoleSum;
	CarRoleInfo	Array[MAX_CARNORMAL_PAGESUM];
};
struct MG_Cmd_CarVipSeatGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct LC_Cmd_CarVipSeatGlobelChange : public NetCmd
{
	BYTE			VipSeat;
	BYTE			Index;
	DWORD				GlobelSum;
};
struct MG_Cmd_CarTableStopUpdate : public NetCmd
{
	DWORD			dwUserID;
};
struct LC_Cmd_CarTableStopUpdate : public NetCmd
{

};
struct MG_Cmd_CarVipGlobelChange : public NetCmd
{
	DWORD			dwUserID;
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct LC_Cmd_CarVipGlobelChange : public NetCmd
{
	DWORD			VipUserID;
	DWORD			VipGlobelSum;
};
struct CL_Cmd_CarRoleBetGlobelByLog : public NetCmd
{
	DWORD			betGlobel[MAX_CAR_ClientSum];
};
struct GM_Cmd_CarRoleBetGlobelByLog : public NetCmd
{
	DWORD			dwUserID;
	DWORD			betGlobel[MAX_CAR_ClientSum];
};
struct MG_Cmd_CarRoleBetGlobelByLog : public NetCmd
{
	DWORD			dwUserID;
	DWORD			AllGlobel;
	bool			Result;
	DWORD			betGlobel[MAX_CAR_ClientSum];
};
struct LC_Cmd_CarRoleBetGlobelByLog : public NetCmd
{
	bool			Result;
	DWORD			betGlobel[MAX_CAR_ClientSum];
};

//����ţţ
//Main_FiveNiuNiu
enum FiveNiuNiuSub
{
	//����ţţ����
	//1.��ʾָ�������ڵ����� 
	CL_FShowTableInfo = 1,
	GM_FShowTableInfo = 2,
	MG_FShowTableInfo = 3,
	LC_FShowTableInfo = 4,
	//2.����ָ������
	CL_FJoinRoomTable = 5,
	GM_FJoinRoomTable = 6,
	MG_FJoinRoomTable = 7,
	LC_FJoinRoomTable = 8,
	//3.���µ���ҽ���
	MG_FNewRoleJoinTable = 9,
	LC_FNewRoleJoinTable = 10,
	//4.�뿪����
	CL_FRoleLeaveTable = 11,
	GM_FRoleLeaveTable = 12,
	MG_FRoleLeaveTable = 13,
	LC_FRoleLeaveTable = 14,
	//5.����뿪����
	GM_FRoleLeaveRoom = 15,
	//5.������������ݱ仯
	GM_FRoleInfoChange = 16,
	MG_FRoleInfoChange = 17,
	LC_FRoleInfoChange = 18,
	//6.���ӽ�����ע�ȴ��׶�
	MG_FTableJoinBegin = 19,
	LC_FTableJoinBegin = 20,
	//8.���ӽ���ȴ��׶�
	MG_FTableJoinWrite = 21,
	LC_FTableJoinWrite = 22,
	//9.���������ӱ仯
	MG_FOnceTableChange = 23,
	LC_FOnceTableChange = 24,
	//10.
	CL_FLeaveRoom		= 25,
	//11.
	CL_ShowBrandInfo    = 26,
	GM_ShowBrandInfo	= 27,
	MG_ShowBrandInfo	= 28,
	LC_ShowBrandInfo	= 29,
	MG_ShowBrandInfoFailed = 30,
	LC_ShowBrandInfoFailed = 31,
	//12.
	MG_FTableJoinEnd	= 32,
	LC_FTableJoinEnd	= 33,
};
struct tagFiveNiuNiuTableInfo
{
	WORD		TableID;
	BYTE		TableStates;//���ӵ�״̬
};
struct CL_Cmd_FShowTableInfo : public NetCmd
{
	BYTE		bRoomID;
};
struct GM_Cmd_FShowTableInfo : public NetCmd
{
	DWORD		dwUserID;
	BYTE		bRoomID;
};
struct MG_Cmd_FShowTableInfo : public NetCmd
{
	bool						bResult;
	DWORD						dwUserID;
	BYTE						States;
	WORD						Sum;
	tagFiveNiuNiuTableInfo		Array[1];
};
struct LC_Cmd_FShowTableInfo : public NetCmd
{
	bool						bResult;
	BYTE						States;
	WORD						Sum;
	tagFiveNiuNiuTableInfo		Array[1];
};

struct CL_Cmd_FJoinRoomTable : public NetCmd
{
	WORD	wTableID;
};
struct GM_Cmd_FJoinRoomTable : public NetCmd
{
	DWORD	dwUserID;
	WORD	wTableID;
};
struct tagFiveNiuNiuTableRoleInfo
{
	DWORD		dwUserID;
	BYTE		SeatID;//0-(Sum-1)
	TCHAR		chArrNickName[MAX_NICKNAME + 1];
	DWORD		dwFaceID;
	DWORD		dwGlobelSum;
	BYTE		bArrTableBrandArray[MAX_NIUNIU_BrandSum];//�Ƶ�����
	WORD		wBrandValue;
};
struct MG_Cmd_FJoinRoomTable : public NetCmd
{
	bool	bResult;
	DWORD	dwUserID;
	//1.����״̬
	WORD	wTableID;
	BYTE	bTableStates;
	DWORD	dwTableStatesLogTime;
	BYTE	bSeatID;
	//3.�����ϵĻ�������
	BYTE						States;
	WORD						Sum;
	tagFiveNiuNiuTableRoleInfo	Array[1];
};
struct LC_Cmd_FJoinRoomTable : public NetCmd
{
	bool	bResult;
	//1.����״̬
	WORD	wTableID;
	BYTE	bTableStates;
	DWORD	dwTableStatesLogTime;
	BYTE	bSeatID;
	//2.�����ϵĻ�������
	BYTE						States;
	WORD						Sum;
	tagFiveNiuNiuTableRoleInfo	Array[1];
};
struct MG_Cmd_FNewRoleJoinTable : public NetCmd
{
	DWORD						dwUserID;
	tagFiveNiuNiuTableRoleInfo NewTableRoleInfo;
};
struct LC_Cmd_FNewRoleJoinTable : public NetCmd
{
	tagFiveNiuNiuTableRoleInfo NewTableRoleInfo;
};

struct CL_Cmd_FRoleLeaveTable : public NetCmd
{

};
struct GM_Cmd_FRoleLeaveTable : public NetCmd
{
	DWORD			dwUserID;
};
struct MG_Cmd_FRoleLeaveTable : public NetCmd
{
	DWORD			dwUserID;
	//1.�뿪��ҵ�����
	DWORD			dwLeaveUserID;
	BYTE			bSeatID;
};
struct LC_Cmd_FRoleLeaveTable : public NetCmd
{
	DWORD			dwLeaveUserID;
	BYTE			bSeatID;
};

struct GM_Cmd_FRoleLeaveRoom : public NetCmd
{
	DWORD			dwUserID;
};

struct GM_Cmd_FRoleInfoChange : public NetCmd
{
	DWORD						dwUserID;
};
struct MG_Cmd_FRoleInfoChange : public NetCmd
{
	DWORD						dwUserID;
	tagFiveNiuNiuTableRoleInfo RoleInfo;
};
struct LC_Cmd_FRoleInfoChange : public NetCmd
{
	tagFiveNiuNiuTableRoleInfo RoleInfo;
};
struct tagFiveNiuNiuRoleBrandInfo
{
	DWORD   dwUserID;
	BYTE	bArrTableBrandArray[MAX_NIUNIU_BrandSum];
	WORD	wTableBrandValue;
};
struct MG_Cmd_FTableJoinBegin : public NetCmd
{
	DWORD						dwUserID;
	DWORD						AddGlobelSum;
	BYTE						bArrTableBrandArray[MAX_NIUNIU_BrandSum];
};
struct LC_Cmd_FTableJoinBegin : public NetCmd
{
	DWORD						AddGlobelSum;
	BYTE						bArrTableBrandArray[MAX_NIUNIU_BrandSum];
};
struct MG_Cmd_FTableJoinWrite : public NetCmd
{
	DWORD		dwUserID;
};
struct LC_Cmd_FTableJoinWrite : public NetCmd
{

};

struct MG_Cmd_FOnceTableChange : public NetCmd
{
	DWORD					dwUserID;
	tagFiveNiuNiuTableInfo oTableInfo;
};
struct LC_Cmd_FOnceTableChange : public NetCmd
{
	tagFiveNiuNiuTableInfo oTableInfo;
};
struct CL_Cmd_FLeaveRoom : public NetCmd
{

};

struct CL_Cmd_ShowBrandInfo	: public NetCmd
{
	WORD	wBrandValue;
	BYTE	bChangeIndex;
};
struct GM_Cmd_ShowBrandInfo	: public NetCmd
{
	DWORD	dwUserID;
	WORD	wBrandValue;
	BYTE	bChangeIndex;
};
struct MG_Cmd_ShowBrandInfo	: public NetCmd
{
	DWORD	dwUserID;
	DWORD	dwDestUserID;
	BYTE	bArrTableBrandArray[MAX_NIUNIU_BrandSum];
	WORD	wBrandValue;
};
struct LC_Cmd_ShowBrandInfo	: public NetCmd
{
	DWORD	dwDestUserID;
	BYTE	bArrTableBrandArray[MAX_NIUNIU_BrandSum];
	WORD	wBrandValue;
};
struct MG_Cmd_ShowBrandInfoFailed : public NetCmd
{
	DWORD  dwUserID;
};
struct LC_Cmd_ShowBrandInfoFailed : public NetCmd
{

};

struct MG_Cmd_FTableJoinEnd : public NetCmd
{
	DWORD		dwUserID;
	__int64		i64GlobelChange;
};
struct LC_Cmd_FTableJoinEnd : public NetCmd
{
	__int64		i64GlobelChange;
};

//Main_Cash
enum CashSub
{
	CL_AddCashOrder = 1,
	LC_AddCashOrder = 2,
};
struct CL_Cmd_AddCashOrder : public NetCmd
{
	DWORD		dwGoldNum;
};
struct LC_Cmd_AddCashOrder : public NetCmd
{
	bool		Result;
};

struct LOG_Sys_Minute_Record :public NetCmd
{
	BYTE game_id;		  //��Ϸ������ID
	BYTE table_id;        //����ID
	int64 min_produce;	  //��һ���Ӳ���
	int64 min_earn;		  //��һ��������
	int64 total_produce;  //�ܲ���
	int64 total_earn;	  //������
	float rate;			  //������
};
#pragma pack(pop)