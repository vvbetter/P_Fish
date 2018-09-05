#pragma once
/*
*@file   FiveNiuNiu.h
*@brief  ͨ��ţţʵ����
*@author ���
*@data   2016-6-14 18:00
*/
#include "RoomManager.h"
class NiuNiuRoom;
class NiuNiuTable;
class NiuNiuTableRole;

//ͨ��ţţ
enum FiveNiuNiuTableStates	//���ӵ�״̬
{
	FNNTS_Begin = 1,		//��ע�׶�
	FNNTS_End = 2,
	FNNTS_WriteBegin = 3,	//�ȴ���ʼ
};

struct NiuNiuOnceBrandList//һ����
{
private:
	std::vector<BYTE> BrandVec;//�Ƶļ���
public:
	NiuNiuOnceBrandList()
	{
		OnClear();
	}
	/**
	*  ��������:		�ӵ�ǰ�������������ȡһ����
	*
	*  @return BYTE		��ȡ����
	*/
	BYTE GetNewBrand()
	{
		BYTE Index = RandUInt() % BrandVec.size();
		BYTE BrandValue = BrandVec[Index];
		BrandVec[Index] = BrandVec[BrandVec.size() - 1];
		BrandVec.pop_back();
		return BrandValue;
	}
	/**
	*  ��������:			��һ���Ʋ�ᵱǰ����������
	*  @param BrandValue	���������
	*
	*  @return void			�޷���ֵ
	*/
	void AddBrand(BYTE BrandValue)
	{
		BrandVec.push_back(BrandValue);
	}
	/**
	*  ��������:		����һ���µ���
	*
	*  @return void		�޷���ֵ
	*/
	void OnClear()
	{
		BrandVec.clear();
		for (BYTE i = 1; i <= 52; ++i)
			BrandVec.push_back(i);
	}
};

class NiuNiuHandleCommon : public IHandleCommon, public IRoomRoleManager  //���������� ���ڴ�����������ϵ��߼�
{
public:
	/**
	*  ��������:			���캯�� explicit����
	*  @param eRoomType		���������ö��
	*  @param pManager		����������Ľӿ� ����Ϊnullptr
	*  @param pRoleManager	��һ������Ľӿ� ����ΪNullptr
	*
	*/
	explicit NiuNiuHandleCommon(RoomType eRoomType, IRoomManager* pManager, IRoomRoleManager* pRoleManager);
	~NiuNiuHandleCommon();

	NiuNiuHandleCommon(const NiuNiuHandleCommon&) = delete;
	NiuNiuHandleCommon& operator=(const NiuNiuHandleCommon&) = delete;

	/**
	*  ��������:			��ȡ��������ö��
	*
	*  @return RoomType		��������ö��
	*/
	virtual RoomType	GetRoomType() const { return m_eRoomType; }
	/**
	*  ��������:			�������������������Ϣ
	*  @param pCmd			�����������ָ��  ����Ϊnullptr
	*
	*  @return bool			�Ƿ���Դ��� ���Դ�����true  �����Դ�����false
	*/
	virtual bool		HandleMsg(NetCmd* pCmd); //��������
	/**
	*  ��������:			��һ�����ӽ��뿪ʼ�׶ε�ʱ�򴥷�
	*  @param bRoomID		ָ���ķ����ID
	*  @param wTableID		ָ�������ӵ�ID
	*
	*  @return void			�޷���ֵ
	*/
	void				OnTableJoinBegin(BYTE bRoomID, WORD wTableID);
	/**
	*  ��������:			��һ�����ӽ���ȴ���ʼ�׶ε�ʱ�򴥷�
	*  @param bRoomID		ָ���ķ����ID
	*  @param wTableID		ָ�������ӵ�ID
	*
	*  @return void			�޷���ֵ
	*/
	void				OnTableJoinWriteBegin(BYTE bRoomID, WORD wTableID);
	/**
	*  ��������:			��һ�����ӽ�������׶ε�ʱ�򴥷�
	*  @param bRoomID		ָ���ķ����ID
	*  @param wTableID		ָ�������ӵ�ID
	*
	*  @return void			�޷���ֵ
	*/
	void				OnTableJoinEnd(BYTE bRoomID, WORD wTableID);
	/**
	*  ��������:			�������ļ���ȡָ������ID�ķ�������
	*  @param bRoomID		ָ���ķ����ID
	*
	*  @return tagNiuNiuRoomConfig* �����ļ�����ָ��
	*/
	const tagNiuNiuRoomConfig* GetNiuNiuRoomConfig(BYTE bRoomID) const;
	/**
	*  ��������:			�������ļ���ȡָ������ID�ı�������
	*  @param bRoomID		ָ���ķ����ID
	*
	*  @return tagFiveNiuNiuRate* �����ļ����ʶ���ָ��
	*/
	const tagFiveNiuNiuRate*   GetNiuNiuRateConfig(BYTE bRoomID) const;
	/**
	*  ��������:		��ѯ��ҵķ���ID ������ڷ��� ����0
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return BYTE		���ط����RoomID
	*/
	virtual  BYTE		QueryRoleRoomID(DWORD dwUserID) const;
	/**
	*  ��������:		��ѯ��ҵ�����ID ����������� ����0
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return WORD		�������ӵ�TableID
	*/
	virtual  WORD		QueryRoleTableID(DWORD dwUserID) const;
	/**
	*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleRoomID ������ѯ
	*  @param dwUserID  ���ID
	*  @param bRoomID	����ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual	 void		SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID);
	/**
	*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleTableID ������ѯ
	*  @param dwUserID  ���ID
	*  @param wTableID	����ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual  void		SetRoleJoinTable(DWORD dwUserID, WORD wTableID);
	/**
	*  ��������:		ɾ�����ID�ͷ���ID ���Һ� ����ID ��ȫ������
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual  void		SetRoleLeaveRoom(DWORD dwUserID);
	/**
	*  ��������:		ɾ�����ID������ID �Ļ���
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual  void		SetRoleLeaveTable(DWORD dwUserID);
	/**
	*  ��������:			�����Ƶ����� �����ƵĴ�С �Ƶ�����Ϊ MAX_NIUNIU_BrandSum
	*  @param pNiuNiuTable	���ӵ�ָ��  ����Ϊnullptr
	*  @param dwUserID		��ҵ�ID
	*
	*  @return NiuNiuTableRole*	��ҵ�ָ�� ����Ϊnullptr
	*/
	virtual WORD		HandleBrandValue(BYTE* BrandValue);
private:
	/**
	*  ��������:		��ʼ������ �ڹ��캯���е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__Init();
	/**
	*  ��������:		���ٺ��� �����������е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__Destory();
	/**
	*  ��������:			������Ϣ������ ��ʾ������ȫ��������
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleShowRoomTableInfo(GM_Cmd_FShowTableInfo* pMsg);
	/**
	*  ��������:			������Ϣ������ ��ҽ�ͼ�����ڵ����ӵ�ʱ��
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleJoinRoomTable(GM_Cmd_FJoinRoomTable* pMsg);
	/**
	*  ��������:			������Ϣ������ ����뿪���ӵ�ʱ��
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleLeaveTable(GM_Cmd_FRoleLeaveTable* pMsg);
	/**
	*  ��������:			������Ϣ������ ����뿪�����ʱ��
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleLeaveRoom(GM_Cmd_FRoleLeaveRoom* pMsg);
	/**
	*  ��������:			������Ϣ������ �����Ϣ�仯��ʱ�� һ��Ϊ��ҳ�ֵ��ʱ��
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleRoleInfoChange(GM_Cmd_FRoleInfoChange* pMsg);
	/**
	*  ��������:			������Ϣ������ ���̯��
	*  @param pMsg			������Ϣ ����Ϊnullptr
	*
	*  @return void			�޷���ֵ
	*/
	void				__HandleShowBrandInfo(GM_Cmd_ShowBrandInfo* pMsg);
	/**
	*  ��������:			���ݷ���ID ��ѯ�����ָ�� 
	*  @param bRoomID		�����ID
	*
	*  @return NiuNiuRoom*	�����ָ�� ����Ϊnullptr
	*/
	NiuNiuRoom*			__FindRoom(BYTE bRoomID) const;
	/**
	*  ��������:			���ݷ���ָ�� ������ID ��ѯ���ӵ�ָ��
	*  @param pNiuNiuRoom	�����ָ��  ����Ϊnullptr
	*  @param bRoomID		���ӵ�ID
	*
	*  @return NiuNiuRoom*	���ӵ�ָ�� ����Ϊnullptr
	*/
	NiuNiuTable*		__FindTable(const NiuNiuRoom* pNiuNiuRoom, WORD wTableID) const;
	/**
	*  ��������:			��������ָ�� �����ID ��ѯ��ҵ�ָ��
	*  @param pNiuNiuTable	���ӵ�ָ��  ����Ϊnullptr
	*  @param dwUserID		��ҵ�ID
	*
	*  @return NiuNiuTableRole*	��ҵ�ָ�� ����Ϊnullptr
	*/
	NiuNiuTableRole*	__FindTableRole(const NiuNiuTable* pNiuNiuTable, DWORD dwUserID) const;
	/**
	*  ��������:			�������ָ�����Ƶ����� ��ȡ ����
	*  @param pTableRole	��ҵ�ָ��  ����Ϊnullptr
	*
	*  @return BYTE			����
	*/
	BYTE				__GetBrandRateValue(NiuNiuTableRole* pTableRole);
	/**
	*  ��������:			�����Ƶ���ֵ �����Ƶļ�ֵ
	*  @param Value			��
	*
	*  @return BYTE			��ֵ
	*/
	BYTE				__GetBrandHandleValue(BYTE Value);
	/**
	*  ��������:			����A B 2������
	*  @param A				��������
	*  @param B				��������
	*
	*  @return void			�޷���ֵ
	*/
	void				__ChangeValue(BYTE& A, BYTE& B);
	/**
	*  ��������:			������ʾ������Ϣʧ�ܵ���Ϣ
	*  @param pRole			���ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendShowRoomTableInfoFailed(Role* pRole) const;
	/**
	*  ��������:			������ʾ������Ϣ����Ϣ
	*  @param pRole			���ָ��
	*  @param pNiuNiuRoom	�����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendShowRoomTableInfoSuccess(Role* pRole, NiuNiuRoom* pNiuNiuRoom) const;
	/**
	*  ��������:			������ҽ���ָ������ʧ�ܵ���Ϣ
	*  @param pRole			���ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendJoinTableFailed(Role* pRole) const;
	/**
	*  ��������:			���������������������ͬ������
	*  @param pNiuNiuTableRole	��ҵ�ָ��(��Ҫͬ�������)
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendTableOtherRoleInfoToMe(NiuNiuTableRole* pNiuNiuTableRole,NiuNiuTable* pNiuNiuTable) const;
	/**
	*  ��������:			���ҵ���Ϣͬ���������ϵ��������
	*  @param pNiuNiuTableRole	��ҵ�ָ��(��Ҫ��ͬ�������)
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendMeRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const;
	/**
	*  ��������:			������뿪���ӵ���Ϣ���͸����������������
	*  @param pNiuNiuTable	����ָ��
	*  @param dwUserID		���ID
	*  @param bSeatID		���λ������
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendRoleLeaveInfoToTableOtherRole(NiuNiuTable* pNiuNiuTable ,DWORD dwUserID, BYTE bSeatID) const;
	/**
	*  ��������:			��һ��������µ���������ͬ�����������������
	*  @param pNiuNiuTableRole	��Ҫͬ������ҵ�ָ��
	*  @param pNiuNiuTable		����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendNewRoleInfoToTableOtherRole(NiuNiuTableRole* pNiuNiuTableRole, NiuNiuTable* pNiuNiuTable) const;
	/**
	*  ��������:			�����н������ָ������������޳�
	*  @param pNiuNiuTable	����ָ��
	*  @param dwMinGlobel	���ٽ������
	*
	*  @return void			�޷���ֵ
	*/
	void				__KickTableRoleByGlobel(NiuNiuTable* pNiuNiuTable, DWORD dwMinGlobel);
	/**
	*  ��������:			����������ȫ����ҵ��Ƶ�����
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SetTableRoleBrandInfo(NiuNiuTable* pNiuNiuTable);
	/**
	*  ��������:			��ȡ���������������ID
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return DWORD		���ID
	*/
	DWORD				__GetTableMaxBrandValueUserID(NiuNiuTable* pNiuNiuTable) const;
	/**
	*  ��������:			��������������Ƶ����� ���ṹ������� ���ҷ��͵��ͻ���ȥ
	*  @param pNiuNiuTable	����ָ��
	*  @param dwMaxBrandValueUserID	���������ID
	*  @param bRate	����ָ�� ��ǰ���ӵı���
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendTableRoleGameResult(NiuNiuTable* pNiuNiuTable, DWORD dwMaxBrandValueUserID, BYTE bRate);
	/**
	*  ��������:			�жϵ�ǰ�����Ƿ���Ҫ�������� ��Ҫ�Ļ� ֱ�Ӵ���
	*  @param pNiuNiuRoom	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__CheckIsNeedCreateTable(NiuNiuRoom* pNiuNiuRoom);
	/**
	*  ��������:			�ڵ�ǰ�����ϴ���һ������
	*  @param pNiuNiuRoom	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__CreateTable(NiuNiuRoom* pNiuNiuRoom);
	/**
	*  ��������:			��һ�����ӵ��������ݷ��͸��ڵ�ǰ���䵫��������������
	*  @param pNiuNiuRoom	����ָ��
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendTableInfoToClient(NiuNiuRoom* pNiuNiuRoom, NiuNiuTable* pNiuNiuTable);
	/**
	*  ��������:			��ȡ�ڷ����ﵫ�ǲ������������ҵ��б�
	*  @param pNiuNiuRoom	����ָ��
	*  @param rRoomRoleVec	����б����ü���
	*
	*  @return void			�޷���ֵ
	*/
	void				__GetNotInTableRoleVec(NiuNiuRoom* pNiuNiuRoom, vector<DWORD>& rRoomRoleVec);
	/**
	*  ��������:			�ı�������ȫ�����ָ���Ľ��
	*  @param pNiuNiuTable	����ָ��
	*  @param nLostGlobel	�ı�������
	*
	*  @return void			�޷���ֵ
	*/
	void				__LostAllTableRoleGlobel(NiuNiuTable* pNiuNiuTable, int nLostGlobel);
	/**
	*  ��������:			����������ȫ����ҽ��뿪ʼ״̬��
	*  @param pNiuNiuTable	����ָ��
	*
	*  @return void			�޷���ֵ
	*/
	void				__SendTableJoinBeginInfo(NiuNiuTable* pNiuNiuTable) const;
	/**
	*  ��������:			����������ָ������Ƶ�����
	*  @param pNiuNiuTable	����ָ��
	*  @param pNiuNiuTableRole	��������ҵ�ָ��
	*  @param wBrandValue	����ƵĴ�С
	*
	*  @return void			�޷���ֵ
	*/
	void				__SetRoleBrandValue(NiuNiuTable* pNiuNiuTable, NiuNiuTableRole* pNiuNiuTableRole, WORD wBrandValue,BYTE bChangeIndex);
	void				__SendNonShowBrandRole(NiuNiuTable* pNiuNiuTable);
private:
	RoomType			m_eRoomType;//��Ϣģ��ķ�������ö��
	IRoomManager*		m_pRoomManager;//�������Ľӿ�
	IRoomRoleManager*	m_pRoomRoleManager;//��ҹ������Ľӿ�

	HashMap<DWORD, BYTE>	m_RoleRoomIDHashMap;//��ҷ���ID�Ļ��� ��ǰ���ͷ����
	HashMap<DWORD, WORD>	m_RoleTableIDHashMap;//�������ID�Ļ��� ��ǰ���͵�
};

class NiuNiuRoom :public IRoom //ţţ����
{
public:
	/**
	*  ��������:			���캯�� explicit����
	*  @param bRoomID		����ID
	*  @param eRoomType		���������ö��
	*  @param pHandleCommon	��Ϣ�������ָ�� ����Ϊnullptr
	*
	*/
	explicit NiuNiuRoom(BYTE bRoomID, RoomType eRoomType, IHandleCommon* pHandleCommon);
	~NiuNiuRoom();
	/**
	*  ��������:		��ȡ��ǰ����ķ���ID
	*
	*  @return BYTE		���ط����RoomID
	*/
	virtual BYTE					GetRoomID() const { return m_bRoomID; }
	/**
	*  ��������:		��ȡ��ǰ����ķ������͵�ö��
	*
	*  @return RoomType ���ط���ķ�������ö��
	*/
	virtual RoomType				GetRoomType() const { return m_eRoomType; }
	/**
	*  ��������:		�ڵ�ǰ�����´����������һ���µ�����
	*
	*  @return ITable*  ���ش��������ӵ�ָ��  �п���Ϊ nullptr
	*/
	virtual ITable*					CreateTable();
	/**
	*  ��������:		��������ID ��ѯ���ӵ�ָ��
	*  @param wTableID  ���ӵ�ID
	*
	*  @return ITable*	��ѯ�����ӵ�ָ�� �п���Ϊnullptr
	*/
	virtual ITable*					FindTable(WORD wTableID) const;
	/**
	*  ��������:		��������ID ɾ��ָ������
	*  @param wTableID  ���ӵ�ID
	*
	*  @return void		�޷��ز���
	*/
	virtual void					DestoryTable(WORD wTableID);
	/**
	*  ��������: ���º��� ���ڸ��·����ڲ�����
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual void					Update(DWORD dwTimer);
	/**
	*  ��������:			��ȡ������ȫ�����ӵ�HashMap������
	*
	*  @return  HashMap		ȫ�����ӵ�HashMap������
	*/
	const HashMap<WORD, ITable*>&	GetAllTableHashMap(){ return m_TableHashMap; }

	NiuNiuRoom(const NiuNiuRoom&) = delete;
	NiuNiuRoom& operator=(const NiuNiuRoom&) = delete;
private:
	/**
	*  ��������:		��ʼ������ �ڹ��캯���е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void							__Init();
	/**
	*  ��������:		���ٺ��� �����������е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void							__Destory();
	/**
	*  ��������:		��������ȫ��������
	*
	*  @return void		�޷���ֵ
	*/
	void							__ClearAllTable();
private:
	RoomType				m_eRoomType;//���������ö��
	BYTE					m_bRoomID;//����ID
	WORD					m_wNowUseTableID;//����Ŀ��õ�����ID
	HashMap<WORD, ITable*>	m_TableHashMap;//���ӵ�HashMap
	NiuNiuHandleCommon*		m_pIHandleCommon;//��Ϣ����ӿ�
};

class NiuNiuTable : public ITable, public ITimer<NiuNiuTable> //ţţ���� ���Ĺ�������
{
public:
	/**
	*  ��������:			���캯�� explicit����
	*  @param bRoomID		����ID
	*  @param wTableID		����ID
	*  @param pHandleCommon	��Ϣ�������ָ�� ����Ϊnullptr
	*
	*/
	explicit NiuNiuTable(BYTE bRoomID, WORD wTableID, NiuNiuHandleCommon* pHandleCommon);
	~NiuNiuTable();

	/**
	*  ��������:		��ȡ��ǰ���ӵķ���ID
	*
	*  @return BYTE		�������ӵķ���ID
	*/
	virtual BYTE						GetRoomID() const { return m_bRoomID; }
	/**
	*  ��������:		��ȡ��ǰ���ӵ�����ID
	*
	*  @return WORD		�������ӵ�����ID
	*/
	virtual WORD						GetTableID() const { return m_wTableID; }
	/**
	*  ��������:		�������ID �����������һ���������
	*  @param dwUserID  ���ID
	*
	*  @return ITableRole*	������������ҵ�ָ�� �п���Ϊ nullptr
	*/
	virtual ITableRole*					CreateTableRole(DWORD dwUserID);
	/**
	*  ��������:		�������ID ��ѯ�����ϵ����ָ��
	*  @param dwUserID  ���ID
	*
	*  @return ITableRole*	��ѯ��������ҵ�ָ�� �п���Ϊ nullptr
	*/
	virtual ITableRole*					FindTableRole(DWORD dwUserID) const;
	/**
	*  ��������:		�������ID ɾ��ָ�������ϵ����
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷��ز���
	*/
	virtual void						DestoryTableRole(DWORD dwUserID);
	/**
	*  ��������: ���º��� ���ڸ��������ڲ�����
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual void						Update(DWORD dwTimer);
	/**
	*  ��������:		��ȡ��������ҵ�����
	*
	*  @return BYTE		�������������
	*/
	virtual BYTE						GetTableRoleSum() const { return m_TableRoleHashMap.size(); }
	/**
	*  ��������: ��ȡ������ҵ�״̬  ÿ��λ���Ƿ����� ��״̬
	*
	*  @return  DWORD		״ֵ̬
	*/
	DWORD								GetTableInfoState() const;
	/**
	*  ��������: ��ȡ������ȫ�������
	*
	*  @return  HashMap		ȫ����ҵ�HashMap
	*/
	const HashMap<DWORD, ITableRole*>&	GetAllRole() const { return m_TableRoleHashMap; }
	/**
	*  ��������: ��ȡ���ӵ�ǰ��״̬
	*
	*  @return  FiveNiuNiuTableStates		״ֵ̬
	*/
	FiveNiuNiuTableStates				GetTableState() const { return m_eTableState; }
	/**
	*  ��������: ��ȡ���ӵ�ǰ��״̬�Ľ���ʱ��
	*
	*  @return  DWORD		����ʱ��
	*/
	DWORD								GetTableStateTimeLog() const { return m_dwTableStateTimeLog; }

	NiuNiuTable(const NiuNiuTable&) = delete;
	NiuNiuTable& operator=(const NiuNiuTable&) = delete;
private:
	/**
	*  ��������:		��ʼ������ �ڹ��캯���е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void								__Init();
	/**
	*  ��������:		���ٺ��� �����������е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void								__Destory();
	/**
	*  ��������:		����������ȫ�������
	*
	*  @return void		�޷���ֵ
	*/
	void								__ClearAllTableRole();
	/**
	*  ��������:		�������ӵ�״̬
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return void		�޷���ֵ
	*/
	void								__UpdateTableState(DWORD dwTimer);
	/**
	*  ��������:		�����Ƿ���Խ��뿪ʼ״̬
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return bool		���Խ���Ϊtrue  �����Խ���Ϊfalse
	*/
	bool								__IsCanJoinBegin(DWORD dwTimer) const;
	/**
	*  ��������:		�����Ƿ���Խ���ȴ���ʼ״̬
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return bool		���Խ���Ϊtrue  �����Խ���Ϊfalse
	*/
	bool								__IsCanJoinWriteBegin(DWORD dwTimer) const;
	/**
	*  ��������:		�����Ƿ���Խ������״̬
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return bool		���Խ���Ϊtrue  �����Խ���Ϊfalse
	*/
	bool								__IsCanJoinEnd(DWORD dwTimer) const;
	/**
	*  ��������:		�ı����ӵĵ�ǰ״̬ �����������״̬�Ľ���ʱ��(�ȴ���ʼ״̬�޽���ʱ��)
	*  @param	eTableState		��ǰ״̬
	*
	*  @return void		�޷���ֵ
	*/
	void								__SetTableState(FiveNiuNiuTableStates eTableState);
	/**
	*  ��������:		���ӽ��뿪ʼ״̬
	*
	*  @return void		�޷���ֵ
	*/
	void								__OnJoinBegin();
	/**
	*  ��������:		���ӽ���ȴ���ʼ״̬
	*
	*  @return void		�޷���ֵ
	*/
	void								__OnJoinWriteBegin();
	/**
	*  ��������:		���ӽ������״̬
	*
	*  @return void		�޷���ֵ
	*/
	void								__OnJoinEnd();
	/**
	*  ��������:		��ȡ�����ϵ�һ���յ�λ�� ���û�� ����0xff
	*
	*  @return BYTE		���ؿյ�λ�� ���ܷ���0xff ��ʾ��������
	*/
	BYTE								__GetNullSeat() const;

	bool								__TableIsAllRoleShowBrand() const;
	void								__UpdateTableRobot(DWORD dwTimer);
private:
	BYTE							m_bRoomID;//���������ķ���ID
	WORD							m_wTableID;//����ID
	HashMap<DWORD, ITableRole*>		m_TableRoleHashMap;//������� �����IDΪKey��HashMap
	vector<ITableRole*>				m_TableRoleVec;//��λ���������
	//�������� ״̬ģʽ
	FiveNiuNiuTableStates			m_eTableState; //���ӵ�ǰ״̬
	DWORD							m_dwTableStateTimeLog;//���ӵ�ǰ״̬����ʱ��
	//����ģ��
	NiuNiuHandleCommon*				m_pIHandleCommon;//��Ϣ�߼�����ģ��
	ITableRobotManager*				m_pTableRobotManager;
};

//���������˹����¼�
struct tagNiuNiuTableRobotEvent
{
	DWORD		dwUserID;
	BYTE		bEventID;// 1 ->��ӽ��  2->���� 
	DWORD		dwTimer;//����ʱ��
};

class NiuNiuTableRobotManager : public ITableRobotManager
{
public:
	explicit NiuNiuTableRobotManager(ITable* pTable, IHandleCommon* pHandleCommon);
	virtual ~NiuNiuTableRobotManager();

	virtual void OnCreateTableEvent();
	virtual void OnRoleJoinTableEvent(DWORD dwUserID);
	virtual void OnRoleLeaveTableEvent(DWORD dwUserID);
	virtual void OnTableDestoryEvent();

	virtual void OnBeginTableStateChange(ITable* pTable);
	virtual void OnEndTableStateChange(ITable* pTable);

	virtual void Update(DWORD dwTimer);
private:
	void __AddNewRobot();
	void __DelRobot(DWORD dwUserID);

	void __TableBeginJoinBegin(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinBegin(NiuNiuTable* pNiuNiuTable);

	void __TableBeginJoinWriteBegin(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinWriteBegin(NiuNiuTable* pNiuNiuTable);

	void __TableBeginJoinEnd(NiuNiuTable* pNiuNiuTable);
	void __TableEndJoinEnd(NiuNiuTable* pNiuNiuTable);

	void __HandleEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent);
	void __HandleJoinGlobelEvent(DWORD dwTimer,const tagNiuNiuTableRobotEvent& rEvent);
	void __HandleShowBrandEvent(DWORD dwTimer, const tagNiuNiuTableRobotEvent& rEvent);
private:
	NiuNiuTable*							m_pNiuNiuTable;
	NiuNiuHandleCommon*						m_NiuNiuHandleCommon;
	std::vector<tagNiuNiuTableRobotEvent>	m_RobotEventVec;
};

class NiuNiuTableRole : public ITableRole
{
public:
	/**
	*  ��������:			���캯�� explicit����
	*  @param bSeatID		��λID ��0��ʼ
	*  @param dwUserID		���ID
	*
	*/
	explicit NiuNiuTableRole(BYTE bSeatID,DWORD dwUserID);
	~NiuNiuTableRole();
	/**
	*  ��������:		��ȡ��ҵ�ID
	*
	*  @return DWORD	���ID
	*/
	virtual DWORD	GetRoleID() const { return m_dwUserID; }
	/**
	*  ��������:		��ȡ��ҵ��ⲿ����ָ��
	*
	*  @return Role*	���ָ�� �п���Ϊ nullptr
	*/
	virtual	Role*	GetRole() const { return m_pRole; }
	/**
	*  ��������:		��ȡ��ҵ�λ������(��0��ʼ)
	*
	*  @return BYTE		��ҵ�λ������(��0��ʼ)
	*/
	virtual BYTE	GetSeatID() const { return m_bSeatID; }
	/**
	*  ��������:		��ȡ��ҵ��Ƶ�����
	*  @param rBrandArray	�Ƶ���������ָ��
	*
	*  @return void		�޷���ֵ
	*/
	const void		GetBrandArray(BYTE* rBrandArray) const{
		for (BYTE i = 0; i < MAX_NIUNIU_BrandSum; ++i)
			rBrandArray[i] = m_bArrTableBrandArray[i];
	}
	/**
	*  ��������:			��������Ƶ�����  ����Ϊ MAX_NIUNIU_BrandSum
	*  @param dwUserID		�Ƶ����� ����Ϊ MAX_NIUNIU_BrandSum
	*
	*  @return void			�޷���ֵ
	*/
	void			SetBrandArray(BYTE* parrBrandArray);
	/**
	*  ��������:			�������Ƶ�����
	*
	*  @return void			�޷���ֵ
	*/
	void			ClearBrandArray(){ ZeroMemory(&m_bArrTableBrandArray, sizeof(BYTE)*MAX_NIUNIU_BrandSum); m_wBrandValue = 0; m_bIsShowBrand = false; }
	/**
	*  ��������:			��ȡ����Ƶ�ͳ��Ȩ��ֵ
	*
	*  @return WORD			�Ƶ�Ȩ��ֵ (���ڱȽ��ƵĴ�С)
	*/
	WORD			GetBrandValue() const { return m_wBrandValue; }
	/**
	*  ��������:			��������Ƶ�Ȩ��ֵ
	*  @param wBrandValue	�Ƶ�Ȩ��ֵ
	*
	*  @return void			�޷���ֵ
	*/
	void			SetBrandValue(WORD wBrandValue){ m_wBrandValue = wBrandValue; }

	void			SetIsShowBrand(){ m_bIsShowBrand = true; }
	bool			GetIsShowBrand(){ return m_bIsShowBrand; }

	NiuNiuTableRole(const NiuNiuTableRole&) = delete;
	NiuNiuTableRole& operator=(const NiuNiuTableRole&) = delete;
private:
	/**
	*  ��������:		��ʼ������ �ڹ��캯���е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void			__Init();
	/**
	*  ��������:		���ٺ��� �����������е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void			__Destory();
private:
	BYTE			m_bSeatID;//��ҵ���λID ��0��ʼ
	DWORD			m_dwUserID;//���ID
	BYTE			m_bArrTableBrandArray[MAX_NIUNIU_BrandSum];//����������Ƶ�����
	WORD			m_wBrandValue;//����Ƶ�Ȩ��ֵ
	Role*			m_pRole;//��ҵ�ָ��
	bool			m_bIsShowBrand;
};

