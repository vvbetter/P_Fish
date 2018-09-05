#pragma once
struct SqlUpdateStr;
class SqlTable
{
public:
	enum
	{
		TABLE_CREATING		= 0x1,	//������
		TABLE_CREATED		= 0x2,	//���������
		TABLE_DATABASE		= 0x4,	//���ݿ��ȡ�ı�
		TABLE_NEEDCOMMIT	= 0x8,	//��Ҫ�ύ����

		//----------------------------------------------
		//�������ύ״̬���ı������
		TABLE_DBINCREMENT	= 0x10,	//ʹ�����ݿ�ĵ���ID
		TABLE_ATTACHCOLUMN	= 0x20, //���ӵ�����Ϣ
		TABLE_CHUNK			= 0x40,
		TABLE_FIXED_ATTRIBUTES = TABLE_DBINCREMENT | TABLE_ATTACHCOLUMN,
	};
	struct TableData
	{
		BYTE	TableName;		//����
		BYTE	Columns;		//����
		UINT	Flags;			//��ϱ�־��Ϣ
		UINT	RowIncID;		//����ID
		UINT	OrgRowCount;	//ԭʼ���е�������
		UINT	RowSize;		//�е��ֽڴ�С
	};

	SqlTable()
	{
		memset(&m_TableData, 0, sizeof(m_TableData));
		m_TableData.RowIncID = 1;	//mysql ��1��ʼ
		m_pColumns = NULL;
	}
	~SqlTable()
	{
		Clear();
	}
	//�����±�
	bool BeginCreate(BYTE tableName, BYTE columns);
	bool SetColumn(BYTE column, const char *pcColumns, SqlDataType ft, USHORT length);
	bool EndCreate();
	//��������������
	void Clear();
	bool CreateSqlStr(vector<SqlUpdateStr*> &strList, bool bReset);

	//��Sql����Դ��ʼ��
	bool InitFromSqlData(BYTE tableName, void *sql, void *res);

	//����
	bool Insert(UINT num){
		return InnerInsert(num);
	}
	bool Delete(UINT row){
		return InnerDelete(row);
	}
	template<typename T>
	bool Update(T t, UINT row, BYTE col){
		return InnerUpdate(t, row, col);
	}
	bool Update(const WCHAR *pcStr, UINT size, UINT row, BYTE col){
		return InnerUpdate(pcStr, size, row, col);
	}
	bool Update(const WCHAR *pcStr, UINT row, BYTE col){
		return InnerUpdate(pcStr, wcslen(pcStr), row, col);
	}
	UINT Rows()const{
		return m_RowList.size();
	}
	BYTE Columns()const{
		return m_TableData.Columns;
	}
	UINT GetRowID(UINT row)const{
		return *((UINT*)GetField(row, 0));
	}
	UINT GetRowID(SqlRow *pRow)const{
		return *((UINT*)(((BYTE*)pRow) + GetColumn(0).Offset));
	}
	const SqlColumn& GetColumn(BYTE colIndex)const{
		return m_pColumns[colIndex];
	}
	const SqlRow* GetRow(UINT row)const{
		return m_RowList[row];
	}
	UINT GetOrgRowCount()const{
		return m_TableData.OrgRowCount;
	}
	UINT GetInsertRowCount()const{
		return Rows() - m_TableData.OrgRowCount;
	}
	UINT GetDeleteRowCount()const{
		return m_DeleteList.size();
	}
	UINT GetUpdateRowCount()const{
		return m_UpdateList.size();
	}
	UINT GetRowSize()const{
		return m_TableData.RowSize;
	}
	const char *GetTableName()const{
		return ::GetTableName(m_TableData.TableName);
	}
	bool IsOrgRow(UINT row)const{
		return row < GetOrgRowCount();
	}
	bool IsNewRow(UINT row)const{
		return row >= GetOrgRowCount();
	}
	bool IsCreated()const{
		return (m_TableData.Flags & TABLE_CREATED) != 0;
	}
	bool IsNeedCommit()const{
		return (m_TableData.Flags & TABLE_NEEDCOMMIT) != 0;
	}
	
	BYTE GetByte(UINT row, BYTE col)const{
		return *GetField(row, col);
	}
	bool GetBit(UINT row, BYTE col)const{
		return ((*GetField(row, col)) ==1 ?true:false);
	}
	CHAR GetChar(UINT row, BYTE col)const{
		return (CHAR)*GetField(row, col);
	}
	SHORT GetShort(UINT row, BYTE col)const{
		return *(SHORT*)GetField(row, col);
	}
	USHORT GetUshort(UINT row, BYTE col)const{
		return *(USHORT*)GetField(row, col);
	}
	INT GetInt(UINT row, BYTE col)const{
		return *(INT*)GetField(row, col);
	}
	UINT GetUint(UINT row, BYTE col)const{
		return *(UINT*)GetField(row, col);
	}
	INT64 GetDateTime(UINT row, BYTE col)const{
		return *(INT64*)GetField(row, col);
	}
	INT64 GetInt64(UINT row, BYTE col)const{
		return *(INT64*)GetField(row, col);
	}
	UINT64 GetUint64(UINT row, BYTE col)const{
		return *(UINT64*)GetField(row, col);
	}
	float GetFloat(UINT row, BYTE col)const{
		return *(float*)GetField(row, col);
	}
	const WCHAR* GetStr(UINT row, BYTE col)const{
		return (const WCHAR*)GetField(row, col);
	}
	const BYTE* GetField(UINT row, BYTE col)const{
		return (BYTE*)((const BYTE*)GetRow(row)) + GetColumn(col).Offset;
	}
	const BYTE* GetField(const SqlRow* row, BYTE col)const{
		return (BYTE*)((const BYTE*)row) + GetColumn(col).Offset;
	}
	const WCHAR* CopyFieldToStr(UINT row, BYTE col, WCHAR *pcStr, uint size)const{
		return GetSqlFieldValueStr(GetColumn(col), pcStr, size, GetField(row, col));
	}
	const CHAR* CopyFieldToStr(UINT row, BYTE col, CHAR *pcStr, uint size)const{
		return GetSqlFieldValueStr(GetColumn(col), pcStr, size, GetField(row, col));
	}
	const CHAR* CopyFieldToStr(const SqlRow* row, BYTE col, CHAR *pcStr, uint size)const{
		return GetSqlFieldValueStr(GetColumn(col), pcStr, size, GetField(row, col));
	}
	const WCHAR* CopyFieldToStr(const SqlRow* row, BYTE col, WCHAR *pcStr, uint size)const{
		return GetSqlFieldValueStr(GetColumn(col), pcStr, size, GetField(row, col));
	}
	vector<UINT>& InnerGetDeleteList(){
		return m_DeleteList;
	}
	SqlUpdateHashMap& InnerGetUpdateList(){
		return m_UpdateList;
	}
	//ʹ�����ݿ�ĵ���ID�������������һ�����������ʱ��ʹ�����ݿ�ĵ���ID;
	void EnableDatabaseID(){
		SetFlag(TABLE_DBINCREMENT);
	}
	bool IsEnableDatabaseID()const{
		return (m_TableData.Flags & TABLE_DBINCREMENT) != 0;
	}
	bool IsAttachColumn()const{
		return (m_TableData.Flags & TABLE_ATTACHCOLUMN) != 0;
	}
	//�����ֽڣ��ʹ��ֽ����ɣ��������ݴ���
	BYTE*	ToBytes(UINT &size, bool bContainsColumn = true)const;
	bool	FromBytes(const BYTE* pBytes, UINT size, SqlColumn *pAttachColumn = NULL);
	bool	CombineFromBytes(const BYTE* pBytes, UINT size);
	//�������ύ״̬
	void	SetCommited();
	bool	CreateIndex(BYTE column){
		return m_Index.Create(&GetColumn(column));
	}
	bool	Select(const SqlCondition &d, SqlTable &table);
	bool	SelectUnion(const SqlUnion&ud, SqlTable &maintable, SqlTable &retResult);
protected:
	template<typename T>
	bool InnerUpdate(T t, UINT row, BYTE col)
	{
		if (row >= Rows() || col >= Columns())
			return false;
		USHORT length = GetColumn(col).Length;
		if (sizeof(T) != length)
		{
			Log("ֵ���Ȳ���ȷ:%d, %d", sizeof(T), length);
			return false;
		}
		*((T*)InnerGetField(row, col)) = t;
		if (IsOrgRow(row))
			SetFieldUdpate(InnerGetRow(row), col);
		return true;
	}
	void SetNeedCommit()
	{
		SetFlag(TABLE_NEEDCOMMIT);
	}
	void SetFlag(UINT flag)
	{
		m_TableData.Flags |= flag;
	}
	void ClearFlag(UINT flag)
	{
		m_TableData.Flags &= ~flag;
	}
	SqlRow* InnerGetRow(UINT row){
		return m_RowList[row];
	}
	SqlColumn& InnerGetColumn(BYTE colIndex){
		return m_pColumns[colIndex];
	}
	BYTE* InnerGetField(UINT row, BYTE col){
		return (BYTE*)(((BYTE*)InnerGetRow(row)) + GetColumn(col).Offset);
	}
	bool HasFlag()const{
		return m_TableData.Flags != 0;
	}
	void SetRowID(UINT row, UINT id){
		*((UINT*)GetField(row, 0)) = id;
	}
	UINT GetFixedFlags()const{
		return m_TableData.Flags & TABLE_FIXED_ATTRIBUTES;
	}
	bool InitFromResult(SqlIDList &rows, UINT64 columnMask, BYTE tableName, BYTE columnCount, USHORT rowSize, SqlColumn *pColumns);
	bool InnerUpdate(const WCHAR *pcStr, UINT size, UINT row, BYTE col);
	void SetFieldUdpate(SqlRow*pRow, BYTE col);
	bool InnerDelete(UINT row);
	bool InnerInsert(UINT num);
	
protected:
	SqlIndex			m_Index;
	TableData			m_TableData;
	SqlColumn			*m_pColumns;
	vector<SqlRow*>		m_RowList;
	vector<UINT>		m_DeleteList;
	SqlUpdateHashMap	m_UpdateList;
};