#pragma once
class CashManager
{
public:
	CashManager();
	~CashManager();

	void	Init();

	void	Update();

	void	HandleCashOrderInfo(DBO_Cmd_LoadCash* pMsg);
private:
	void	__SendLoadAllCashOrderInfoToDB();
	void	__HandleCashOrderInfo(tagCashOrderInfo* pInfo);
private:
	DWORD   m_LogTime;//上次查询的时间
};