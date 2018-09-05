#include "stdafx.h"
#include "SqlManager.h"
SqlManager::SqlManager()
{
	m_SqlAdapterVec.clear();
}
SqlManager::~SqlManager()
{
	Shutdown();
}
bool SqlManager::Init(std::vector<SQLInitData>& data)
{
	m_SqlAdapterVec.clear();
	std::vector<SQLInitData>::iterator Iter = data.begin();
	for (; Iter != data.end(); ++Iter)
	{
		ISqlAdapter* SqlAdapter = new MySqlAdapter();
		if (SqlAdapter == nullptr || !SqlAdapter->Connected(*Iter))
		{
			delete SqlAdapter;
			SqlAdapter = nullptr;
			return false;
		}
		m_SqlAdapterVec.push_back(SqlAdapter);
	}
	return true;
}
void SqlManager::Shutdown()
{
	if (m_SqlAdapterVec.empty())
		return;
	std::vector<ISqlAdapter*>::iterator Iter = m_SqlAdapterVec.begin();
	for (; Iter != m_SqlAdapterVec.end(); ++Iter)
	{
		delete (*Iter);
	}
	m_SqlAdapterVec.clear();
}
bool SqlManager::Select(const char *sql, BYTE table, SqlResultProxy &tableResult, bool singleTable)
{
	ISqlAdapter* SqlAdapter =  __GetSqlAdapter();
	if (SqlAdapter == nullptr || !SqlAdapter->Select(sql))
		return false;
	tableResult.SetProxy(dynamic_cast<ISqlResult*>(SqlAdapter));
	return true;
}
void SqlManager::GetMySqlEscapeString(char* pArray, UINT Size, char* OutStr)
{
	ISqlAdapter* SqlAdapter = __GetSqlAdapter();
	if (nullptr == SqlAdapter)
		return;
	SqlAdapter->GetEscapeString(pArray, Size, OutStr);
}
ISqlAdapter* SqlManager::__GetSqlAdapter()
{
	std::vector<ISqlAdapter*>::iterator Iter = m_SqlAdapterVec.begin();
	for (; Iter != m_SqlAdapterVec.end(); ++Iter)
	{
		if ((*Iter)->IsConnected())
			return *Iter;
	}
	return nullptr;
}
bool SqlManager::IsConnected()const
{
	std::vector<ISqlAdapter*>::const_iterator Iter = m_SqlAdapterVec.begin();
	for (; Iter != m_SqlAdapterVec.end(); ++Iter)
	{
		if ((*Iter)->IsConnected())
			return true;
	}
	return false;
}