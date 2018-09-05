#pragma once
#include "ISqlAdapter.h"
#include "MySqlAdapter.h"
//SQL SQLTable 
//用于适配原有的数据

class SqlResultProxy :public ISqlResult
{
public:
	SqlResultProxy(){}
	SqlResultProxy(ISqlResult* pResult){ m_pSqlResult = pResult; }
	virtual ~SqlResultProxy(){}

	void	SetProxy(ISqlResult* pResult){ m_pSqlResult = pResult; }

	DWORD	Rows(){ return GetRowCount(); }

	virtual  DWORD				GetRowCount() const{
		return m_pSqlResult->GetRowCount();
	}
	virtual  DWORD				GetColCount() const {
		return m_pSqlResult->GetRowCount();
	}
	virtual  BYTE				GetByte(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetByte(dwRow, dwCol);
	}
	virtual  bool				GetBit(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetBit(dwRow, dwCol);
	}
	virtual  char				GetChar(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetChar(dwRow, dwCol);
	}
	virtual  short				GetShort(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetShort(dwRow, dwCol);
	}
	virtual  WORD				GetUshort(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetUshort(dwRow, dwCol);
	}
	virtual  int				GetInt(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetInt(dwRow, dwCol);
	}
	virtual  DWORD				GetUint(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetUint(dwRow, dwCol);
	}
	virtual  __int64			GetDateTime(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetDateTime(dwRow, dwCol);
	}
	virtual  __int64			GetInt64(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetInt64(dwRow, dwCol);
	}
	virtual  unsigned __int64	GetUint64(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetUint64(dwRow, dwCol);
	}
	virtual  float				GetFloat(DWORD dwRow, DWORD dwCol) {
		return m_pSqlResult->GetFloat(dwRow, dwCol);
	}
	virtual  TCHAR*				GetStr(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetStr(dwRow, dwCol);
	}
	virtual  BYTE*				GetBlob(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetBlob(dwRow, dwCol);
	}
	virtual BYTE*				GetField(DWORD dwRow, DWORD dwCol)  {
		return m_pSqlResult->GetBlob(dwRow, dwCol);
	}
private:
	ISqlResult*		m_pSqlResult;
};

class SqlManager
{
public:
	SqlManager();
	~SqlManager();

	bool	Init(std::vector<SQLInitData>& data);
	void	Shutdown();
	bool	Select(const char *sql, BYTE table, SqlResultProxy &tableResult, bool singleTable);
	bool	IsConnected()const;
	void	GetMySqlEscapeString(char* pArray, UINT Size, char* OutStr);
private:
	ISqlAdapter*	__GetSqlAdapter();
private:
	std::vector<ISqlAdapter*>		m_SqlAdapterVec;
	//std::vector<ISqlResult*>		m_SqlResultVec;
};