#pragma once
#include "ISqlAdapter.h"


class MySqlAdapter : public ISqlAdapter, public ISqlResult
{
public:
	MySqlAdapter();
	virtual ~MySqlAdapter();

	virtual	bool				Connected(const SQLInitData& rSqlInitData);
	virtual bool				IsConnected() const;
	virtual bool				Select(const char *sql);
	virtual void				GetEscapeString(char* pArray, UINT Size, char* OutStr);

	virtual DWORD				GetRowCount() const;
	virtual DWORD				GetColCount() const;
	virtual BYTE				GetByte(DWORD dwRow, DWORD dwCol);
	virtual bool				GetBit(DWORD dwRow, DWORD dwCol);
	virtual char				GetChar(DWORD dwRow, DWORD dwCol);
	virtual short				GetShort(DWORD dwRow, DWORD dwCol);
	virtual WORD				GetUshort(DWORD dwRow, DWORD dwCol);
	virtual int					GetInt(DWORD dwRow, DWORD dwCol);
	virtual DWORD				GetUint(DWORD dwRow, DWORD dwCol);
	virtual __int64				GetDateTime(DWORD dwRow, DWORD dwCol);
	virtual __int64				GetInt64(DWORD dwRow, DWORD dwCol);
	virtual unsigned __int64	GetUint64(DWORD dwRow, DWORD dwCol);
	virtual float				GetFloat(DWORD dwRow, DWORD dwCol);
	virtual TCHAR*				GetStr(DWORD dwRow, DWORD dwCol);
	virtual BYTE*				GetBlob(DWORD dwRow, DWORD dwCol);
private:
	void			__Init();
	void			__Destory();
	bool			__TryConnectedMySql();
	void			__ClearResult();
	char*			__GetValue(DWORD dwRow,DWORD dwCol);
	void			__ClearPoint();
private:
	void*			m_pMySql;
	bool			m_bIsConnected;
	SQLInitData		m_SqlInitData;
	//当前查询数据
	void*			m_pResult;
	//应该内部消化的指针
	std::vector<void*> m_BeFreeVec;
};