#include "stdafx.h"
#include "MySqlAdapter.h"
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")

MySqlAdapter::MySqlAdapter() :m_pMySql(nullptr), m_bIsConnected(false), m_pResult(nullptr)
{
	__Init();
}
MySqlAdapter::~MySqlAdapter()
{
	__Destory();
}
void MySqlAdapter::__Init()
{
	ZeroMemory(m_SqlInitData.IP, sizeof(m_SqlInitData.IP));
	ZeroMemory(m_SqlInitData.DB, sizeof(m_SqlInitData.DB));
	ZeroMemory(m_SqlInitData.PWD, sizeof(m_SqlInitData.PWD));
	ZeroMemory(m_SqlInitData.UID, sizeof(m_SqlInitData.UID));
}
void MySqlAdapter::__Destory()
{
	__ClearPoint();
	__ClearResult();
	if (m_pMySql)
	{
		mysql_close((MYSQL*)m_pMySql);
		delete(m_pMySql);
		m_pMySql = NULL;
		m_bIsConnected = false;
	}
}
bool MySqlAdapter::Connected(const SQLInitData& rSqlInitData)
{
	m_pMySql = mysql_init(new MYSQL);
	if (mysql_real_connect((MYSQL*)m_pMySql, rSqlInitData.IP, rSqlInitData.UID, rSqlInitData.PWD, rSqlInitData.DB, 3306, NULL, CLIENT_MULTI_STATEMENTS))
	{
		char value = 1;
		mysql_options((MYSQL*)m_pMySql, MYSQL_OPT_RECONNECT, &value);//设置MySQL 自动连接 不会断开
		int ret = mysql_set_character_set((MYSQL*)m_pMySql, "gbk");
		//mysql_set_server_option(m_MYSQL, MYSQL_OPTION_MULTI_STATEMENTS_ON);
		m_bIsConnected = true;
		m_SqlInitData = rSqlInitData;
		return true;
	}
	else
	{
		Log("Connect mysql error:%s", mysql_error((MYSQL*)m_pMySql));
		return false;
	}
}
bool MySqlAdapter::__TryConnectedMySql()
{
	if (mysql_ping((MYSQL*)m_pMySql) != 0)
	{
		__Destory();
		m_pMySql = mysql_init(new MYSQL);
		if (mysql_real_connect((MYSQL*)m_pMySql, m_SqlInitData.IP, m_SqlInitData.UID, m_SqlInitData.PWD, m_SqlInitData.DB, 3306, NULL, CLIENT_MULTI_STATEMENTS))
		{
			char value = 1;
			mysql_options((MYSQL*)m_pMySql, MYSQL_OPT_RECONNECT, &value);//设置MySQL 自动连接 不会断开
			int ret = mysql_set_character_set((MYSQL*)m_pMySql, "gbk");
			m_bIsConnected = true;
			return true;
		}
		else
		{
			m_bIsConnected = false;
			Log("Connect mysql error:%s", mysql_error((MYSQL*)m_pMySql));
			return false;
		}
	}
	return false;
}
bool MySqlAdapter::IsConnected() const
{
	return m_bIsConnected;
}
void MySqlAdapter::__ClearResult()
{
	if (nullptr != m_pResult)
	{
		mysql_free_result((MYSQL_RES*)m_pResult);
		m_pResult = nullptr;
	}
	while (mysql_next_result((MYSQL*)m_pMySql))
	{
		MYSQL_RES *pRes2;
		pRes2 = mysql_store_result((MYSQL*)m_pMySql);
		if (pRes2 == nullptr)
			break;
		mysql_free_result(pRes2);
	}
}
bool MySqlAdapter::Select(const char *sql)
{
	__ClearPoint();
	__ClearResult();
	if (mysql_query((MYSQL*)m_pMySql, sql) == 0)
	{
		m_pResult = mysql_store_result((MYSQL*)m_pMySql);
		if (m_pResult == nullptr)
		{
			if (mysql_error((MYSQL*)m_pMySql) != nullptr)
			{
				Log("Select Error:%s, msg:%s", sql, mysql_error((MYSQL*)m_pMySql));
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (__TryConnectedMySql())
		{
			if (mysql_query((MYSQL*)m_pMySql, sql) == 0)
			{
				m_pResult = mysql_store_result((MYSQL*)m_pMySql);
				if (m_pResult == nullptr)
				{
					if (mysql_error((MYSQL*)m_pMySql) != nullptr)
					{
						Log("Select Error:%s, msg:%s", sql, mysql_error((MYSQL*)m_pMySql));
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
			else
			{
				Log("SelectSql Error:%s, msg:%s", sql, mysql_error((MYSQL*)m_pMySql));
				return false;
			}
		}
		else
		{
			Log("SelectSql Error:%s, msg:%s", sql, mysql_error((MYSQL*)m_pMySql));
			return false;
		}
	}
}
void MySqlAdapter::GetEscapeString(char* pArray, UINT Size, char* OutStr)
{
	mysql_real_escape_string((MYSQL*)m_pMySql, OutStr, pArray, Size);
}
DWORD MySqlAdapter::GetRowCount() const
{
	if (nullptr == m_pResult)
		return 0;
	return static_cast<DWORD>(mysql_num_rows((MYSQL_RES*)m_pResult));
}
DWORD MySqlAdapter::GetColCount() const
{
	if (nullptr == m_pResult)
		return 0;
	return static_cast<DWORD>(mysql_num_fields((MYSQL_RES*)m_pResult));
}
char* MySqlAdapter::__GetValue(DWORD dwRow,DWORD dwCol)
{
	if (nullptr == m_pResult || dwRow >= GetRowCount() || dwCol >= GetColCount())
		return nullptr;
	mysql_data_seek((MYSQL_RES*)m_pResult, dwRow);
	MYSQL_ROW rRow = mysql_fetch_row((MYSQL_RES*)m_pResult);
	return rRow[dwCol];
}
BYTE MySqlAdapter::GetByte(DWORD dwRow, DWORD dwCol)
{
	if (dwRow>=GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	BYTE Result = (BYTE)strtoul(pResult, NULL, 10);
	return Result;
}
bool MySqlAdapter::GetBit(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	bool Result = (pResult[0] == 1 || pResult[0] == '1') ? true : false;
	return Result;
}
char MySqlAdapter::GetChar(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	char Result = (char)strtol(pResult, NULL, 10);
	return Result;
}
short MySqlAdapter::GetShort(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	short Result = (SHORT)strtol(pResult, NULL, 10);
	return Result;
}
WORD MySqlAdapter::GetUshort(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	WORD Result = (WORD)strtoul(pResult, NULL, 10);
	return Result;
}
int MySqlAdapter::GetInt(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	int Result = (int)strtol(pResult, NULL, 10);
	return Result;
}
DWORD MySqlAdapter::GetUint(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	DWORD Result = (DWORD)strtoul(pResult, NULL, 10);
	return Result;
}
__int64 MySqlAdapter::GetDateTime(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	tm pTime;
	sscanf_s(pResult, "%4d-%2d-%2d %2d:%2d:%2d", &pTime.tm_year, &pTime.tm_mon, &pTime.tm_mday, &pTime.tm_hour, &pTime.tm_min, &pTime.tm_sec);
	pTime.tm_year -= 1900;
	pTime.tm_mon -= 1;
	time_t time = mktime(&pTime);
	if (time < 0)
		time = 0;
	__int64 Result = time;
	return Result;
}
__int64 MySqlAdapter::GetInt64(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	__int64 Result = (__int64)strtoll(pResult, NULL, 10);
	return Result;
}
unsigned __int64 MySqlAdapter::GetUint64(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0;
	unsigned __int64 Result = (unsigned __int64)strtoull(pResult, NULL, 10);
	return Result;
}
float MySqlAdapter::GetFloat(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return 0.0f;
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return 0.0f;
	float Result = (float)strtof(pResult, NULL);
	return Result;
}
TCHAR* MySqlAdapter::GetStr(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
		return TEXT("");
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
		return TEXT("");
	UINT count;
	TCHAR *Result = CharToWChar(pResult, count);
	m_BeFreeVec.push_back(Result);
	return Result;
}
BYTE* MySqlAdapter::GetBlob(DWORD dwRow, DWORD dwCol)
{
	if (dwRow >= GetRowCount() || dwCol >= GetColCount())
	{
		BYTE* Result = (BYTE*)malloc(MAXBLOB_LENGTH*sizeof(BYTE));
		ZeroMemory(Result, MAXBLOB_LENGTH*sizeof(BYTE));
		m_BeFreeVec.push_back(Result);
		return Result;
	}
	char* pResult = __GetValue(dwRow, dwCol);
	if (nullptr == pResult)
	{
		BYTE* Result = (BYTE*)malloc(MAXBLOB_LENGTH*sizeof(BYTE));
		ZeroMemory(Result, MAXBLOB_LENGTH*sizeof(BYTE));
		m_BeFreeVec.push_back(Result);
		return Result;
	}
	BYTE* Result = (BYTE*)malloc(MAXBLOB_LENGTH*sizeof(BYTE));
	memcpy_s(Result, MAXBLOB_LENGTH*sizeof(BYTE), reinterpret_cast<BYTE*>(pResult), MAXBLOB_LENGTH*sizeof(BYTE));
	m_BeFreeVec.push_back(Result);
	return Result;
}
void MySqlAdapter::__ClearPoint()
{
	std::vector<void*>::iterator Iter = m_BeFreeVec.begin();
	for (; Iter != m_BeFreeVec.end(); ++Iter)
	{
		free(*Iter);
	}
	m_BeFreeVec.clear();
}