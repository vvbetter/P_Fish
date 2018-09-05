#pragma once
#include <set>
struct PhonePayOnce
{
	unsigned __int64    OrderOnlyID;
	DWORD				UserID;//���ID
	unsigned __int64	PhoneNumber;//�ֻ�����
	DWORD				PayRMBSum;//��ֵ�Ļ��� ��λԪ
	BYTE				ShopID;//�̵�ID
	BYTE				ShopOnlyID;//�̵���ƷID
	DWORD				ShopSum;//���������
	BYTE				ClientID;//�����ҵĿͻ���ID

	PhonePayOnce()
	{
		OrderOnlyID = 0;
		UserID = 0;
		PhoneNumber = 0;
		PayRMBSum = 0;
		ShopID = 0;
		ShopOnlyID = 0;
		ShopSum = 0;
		ClientID = 0;
	}
};

const static char AccountNo[] = "account_no";
const static char FacePrice[] = "face_price";
const static char OrderNo[] = "order_no";
const static char OrderTime[] = "order_time";
const static char OrderType[] = "order_type";
const static char PartNerID[] = "partner_id";
const static char PayMoney[] = "pay_money";
const static char Profit[] = "profit";
const static char SignType[] = "sign_type";
const static char Status[] = "status";
const static char StreamID[] = "stream_id";
const static char Sign[] = "sign";

struct PhonePayCrcInfo
{
	DWORD account_noCrc;
	DWORD face_priceCrc;
	DWORD order_noCrc;
	DWORD order_timeCrc;
	DWORD order_typeCrc;
	DWORD partner_idCrc;
	DWORD pay_moneyCrc;
	DWORD profitCrc;
	DWORD sign_typeCrc;
	DWORD statusCrc;
	DWORD stream_idCrc;
	DWORD signCrc;
	PhonePayCrcInfo()
	{
		account_noCrc	= AECrc32(AccountNo, strlen(AccountNo), 0);
		face_priceCrc	= AECrc32(FacePrice, strlen(FacePrice), 0);
		order_noCrc		= AECrc32(OrderNo, strlen(OrderNo), 0);
		order_timeCrc	= AECrc32(OrderTime, strlen(OrderTime), 0);
		order_typeCrc	= AECrc32(OrderType, strlen(OrderType), 0);
		partner_idCrc	= AECrc32(PartNerID, strlen(PartNerID), 0);
		pay_moneyCrc	= AECrc32(PayMoney, strlen(PayMoney), 0);
		profitCrc		= AECrc32(Profit, strlen(Profit), 0);
		sign_typeCrc	= AECrc32(SignType, strlen(SignType), 0);
		statusCrc		= AECrc32(Status, strlen(Status), 0);
		stream_idCrc	= AECrc32(StreamID, strlen(StreamID), 0);
		signCrc			= AECrc32(Sign, strlen(Sign), 0);
	}
};

//�ֻ���ֵ����
class PhonePayManager
{
public:
	PhonePayManager();
	virtual ~PhonePayManager();

	void OnInit();
	void OnLoadAllPhonePayResult(DBO_Cmd_LoadPhonePayOrderID* pMsg);

	//1.��ҷ���һ���������� ���г�ֵ
	void OnHandleRolePhonePay(ServerClientData* pClient,GO_Cmd_PhonePay* pMsg);//��ҳ�ֵ����
	void OnQueryPhonePayOrder(UINT64 order_no);
	void OnSendPhonePayResult(unsigned __int64 OrderID,char* pData, DWORD Length);
	//void OnRolePhonePayResult(DWORD dwUserID, bool Result, BYTE ErrorID, DWORD OrderID);//��ֵ���ѵĽӿ�
	void OnHandleHttpResult(string account_no, string face_price, string order_no, string order_time, string order_type, string partner_id, string pay_money, string profit, string sign_type, string status, string stream_id, string sign);
	PhonePayCrcInfo& GetPhonePayCrcInfo(){ return m_PhonePayCrcInfo; }
private:
	//SDK ���ѵ�
	//1. ���ѳ�ֵ
	void OnSendPhoePayInfoToHttp(unsigned __int64 OrderID);
private:
	std::set<unsigned __int64>					m_PhoneOrderIDSet;
	HashMap<unsigned __int64, PhonePayOnce>		m_PhonePayMap;//�ֻ���������
	PhonePayCrcInfo								m_PhonePayCrcInfo;
};