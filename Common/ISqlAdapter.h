#pragma once
//数据库的适配器 接口
struct SQLInitData;
class ISqlResult
{
public:
	ISqlResult(){}
	virtual ~ISqlResult(){}

	virtual  DWORD				GetRowCount() const = 0;
	virtual  DWORD				GetColCount() const = 0;
	virtual  BYTE				GetByte(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  bool				GetBit(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  char				GetChar(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  short				GetShort(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  WORD				GetUshort(DWORD dwRow, DWORD dwCol) = 0;
	virtual  int				GetInt(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  DWORD				GetUint(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  __int64			GetDateTime(DWORD dwRow, DWORD dwCol) = 0;
	virtual  __int64			GetInt64(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  unsigned __int64	GetUint64(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  float				GetFloat(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  TCHAR*				GetStr(DWORD dwRow, DWORD dwCol)		= 0;
	virtual  BYTE*				GetBlob(DWORD dwRow, DWORD dwCol)		= 0;
};

class ISqlAdapter
{
public:
	ISqlAdapter(){}
	virtual ~ISqlAdapter(){}

	virtual bool	Connected(const SQLInitData& rSqlInitData) = 0;	//根据参数连接到数据库去
	virtual bool	IsConnected() const = 0;//判断数据库是否连接
	virtual bool	Select(const char *sql) = 0;//执行SQL语句 包括执行存储过程
	virtual void	GetEscapeString(char* pArray, UINT Size, char* OutStr) = 0;
};