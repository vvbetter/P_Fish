/*
*@file   RoomManager.h
*@brief  ������->����->����->��� ����Ľӿ�ģ��
*@author ���
*@data   2016-6-14 10:00
*/
#pragma once
#include "RoleManager.h"

class IRoom;
class ITable;
class ITableRole;
class IHandleCommon;

enum RoomType //��������
{ 
	//ö��ѭ���жϵ� ��ΧΪ (RoomType_Init,RoomType_Max) ������[RoomType_Init,RoomType_Max]
	RoomType_Init		= 0, //��ʼ��ID ö��ѭ���ж��õ� ��ʵ��ָ���ķ�������
	RoomType_NiuNiu		= 1, //ͨ��ţţ���������ID
	RoomType_Max,			 //���ֵ ö��ѭ���ж��õ� ��ʵ��ָ���ķ�������
};
//

template<typename T>
/**
*  ��������:		����BYTE ������������ת��Ϊ RoomType ���ʧ�� ת��Ϊ RoomType_Init
*  @param T			��ת��������
*
*  @return RoomType ת�����ö��
*/
inline RoomType ConvertToRoomType(T nT)
{
	T nMin = static_cast<T>(RoomType::RoomType_Init);
	T nMax = static_cast<T>(RoomType::RoomType_Max);
	if (nT >= nMax || nT <= nMin)
		return RoomType::RoomType_Init;
	else
		return static_cast<RoomType>(nT);
};

class ITableRobotManager
{
public:
	explicit ITableRobotManager(ITable* pTable, IHandleCommon* pHandleCommon){}
	virtual ~ITableRobotManager(){}

	//�����˽�������
	virtual void OnCreateTableEvent() = 0;
	virtual void OnRoleJoinTableEvent(DWORD dwUserID) = 0;
	virtual void OnRoleLeaveTableEvent(DWORD dwUserID) = 0;
	virtual void OnTableDestoryEvent() = 0;

	//���������̿���
	virtual void OnBeginTableStateChange(ITable* pTable) = 0;
	virtual void OnEndTableStateChange(ITable* pTable) = 0;

	virtual void Update(DWORD dwTimer) = 0;
private:
	virtual	void __AddNewRobot() = 0;
	virtual void __DelRobot(DWORD dwUserID) = 0;
};

class IRoomManager
{
public:
	explicit IRoomManager(){}
	virtual ~IRoomManager(){}

	/**
	*  ��������:		���ݲ����������󷿼�ӿ�ָ��
	*  @param bRoomID   ����ID
	*  @param eRoomType ���������ö��
	*
	*  @return IRoom*   ����ĳ���ӿ�
	*/
	virtual	 IRoom*		CreateRoom(BYTE bRoomID,RoomType eRoomType) = 0;//����һ������
	/**
	*  ��������: ���ݷ���ID ��ѯ�������� ��������� ����nullptr
	*  @param	bRoomID		����ID 
	*
	*  @return	IRoom*		����ĳ���ӿ�
	*/
	virtual  IRoom*		FindRoom(BYTE bRoomID) const = 0;//��ѯ����
	/**
	*  ��������: ���ݷ���ID ������ɾ����
	*  @param	bRoomID		����ID
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		DestoryRoom(BYTE bRoomID) = 0;//ɾ��һ������
	/**
	*  ��������: ���º��� ���ڸ��¹������ڲ����� 
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//����������
	/**
	*  ��������: �����ⲿ��Ϣ����
	*  @param	pCmd		�ⲿ�����ָ��
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		HandleMsg(NetCmd* pCmd) = 0;//�����������ⲿ����

	IRoomManager(const IRoomManager&) = delete;
	IRoomManager& operator=(const IRoomManager&) = delete;
};

class IRoomRoleManager
{
public:
	explicit IRoomRoleManager(){}
	virtual ~IRoomRoleManager(){}

	/**
	*  ��������:		��ѯ��ҵķ���ID ������ڷ��� ����0
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return BYTE		���ط����RoomID 
	*/
	virtual BYTE QueryRoleRoomID(DWORD dwUserID) const = 0;//��ѯ��ҷ���ID
	/**
	*  ��������:		��ѯ��ҵ�����ID ����������� ����0
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return WORD		�������ӵ�TableID
	*/
	virtual WORD QueryRoleTableID(DWORD dwUserID) const = 0;//��ѯ�������ID
	/**
	*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleRoomID ������ѯ
	*  @param dwUserID  ���ID
	*  @param bRoomID	����ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual void SetRoleJoinRoom(DWORD dwUserID, BYTE bRoomID) = 0;//������ҷ���ID
	/**
	*  ��������:		�������ID �� ����ID�Ļ��� �� QueryRoleTableID ������ѯ
	*  @param dwUserID  ���ID
	*  @param wTableID	����ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual void SetRoleJoinTable(DWORD dwUserID, WORD wTableID) = 0;//�����������ID
	/**
	*  ��������:		ɾ�����ID�ͷ���ID ���Һ� ����ID ��ȫ������ 
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual void SetRoleLeaveRoom(DWORD dwUserID) = 0;//����뿪����
	/**
	*  ��������:		ɾ�����ID������ID �Ļ���
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷���ֵ
	*/
	virtual void SetRoleLeaveTable(DWORD dwUserID) = 0;//����뿪����

	IRoomRoleManager(const IRoomRoleManager&) = delete;
	IRoomRoleManager& operator=(const IRoomRoleManager&) = delete;
};

class IRoom
{
public:
	/**
	*  ��������:		���캯��  explicit ����
	*  @param bRoomID   �����ID
	*  @param eRoomType ���������ö��
	*  @param pHandleCommon ��Ϣ�������ָ��
	*/
	explicit IRoom(BYTE bRoomID, RoomType eRoomType, IHandleCommon* pHandleCommon) {}
	virtual ~IRoom(){}

	/**
	*  ��������:		��ȡ��ǰ����ķ���ID
	*
	*  @return BYTE		���ط����RoomID
	*/
	virtual BYTE		GetRoomID() const = 0; //��ȡ����ID
	/**
	*  ��������:		��ȡ��ǰ����ķ������͵�ö��
	*
	*  @return RoomType ���ط���ķ�������ö��
	*/
	virtual RoomType	GetRoomType() const = 0;//��ȡ��������
	/**
	*  ��������:		�ڵ�ǰ�����´����������һ���µ�����
	*
	*  @return ITable*  ���ش��������ӵ�ָ��  �п���Ϊ nullptr
	*/
	virtual ITable*		CreateTable() = 0;//��������
	/**
	*  ��������:		��������ID ��ѯ���ӵ�ָ��
	*  @param wTableID  ���ӵ�ID
	*
	*  @return ITable*	��ѯ�����ӵ�ָ�� �п���Ϊnullptr
	*/
	virtual ITable*		FindTable(WORD wTableID) const = 0;//��ѯ����
	/**
	*  ��������:		��������ID ɾ��ָ������
	*  @param wTableID  ���ӵ�ID
	*
	*  @return void		�޷��ز���
	*/
	virtual void		DestoryTable(WORD wTableID) = 0;//ɾ������
	/**
	*  ��������: ���º��� ���ڸ��·����ڲ�����
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//�������

	IRoom(const IRoom&) = delete;
	IRoom& operator=(const IRoom&) = delete;
};
class ITable
{
public:
	/**
	*  ��������:		���캯��  explicit ����
	*  @param bRoomID   ���������ID
	*  @param wTableID  ��ǰ����ID
	*/
	explicit ITable(BYTE bRoomID, WORD wTableID, IHandleCommon* pHandleCommon){}
	virtual ~ITable(){}

	/**
	*  ��������:		��ȡ��ǰ���ӵ�����ID
	*
	*  @return WORD		�������ӵ�����ID
	*/
	virtual WORD		GetTableID() const = 0;//��ȡ����ID
	/**
	*  ��������:		��ȡ��ǰ���ӵķ���ID
	*
	*  @return BYTE		�������ӵķ���ID
	*/
	virtual BYTE		GetRoomID() const = 0;//�������ӵķ���ID
	/**
	*  ��������:		�������ID �����������һ���������
	*  @param dwUserID  ���ID
	*
	*  @return ITableRole*	������������ҵ�ָ�� �п���Ϊ nullptr
	*/
	virtual ITableRole* CreateTableRole(DWORD dwUserID) = 0;//����������һ�����
	/**
	*  ��������:		�������ID ��ѯ�����ϵ����ָ��
	*  @param dwUserID  ���ID
	*
	*  @return ITableRole*	��ѯ��������ҵ�ָ�� �п���Ϊ nullptr
	*/
	virtual ITableRole* FindTableRole(DWORD dwUserID) const = 0;//��ѯ���
	/**
	*  ��������:		�������ID ɾ��ָ�������ϵ����
	*  @param dwUserID  ��ҵ�ID
	*
	*  @return void		�޷��ز���
	*/
	virtual void		DestoryTableRole(DWORD dwUserID) = 0;//ɾ��������һ�����
	/**
	*  ��������:		��ȡ��������ҵ�����
	*
	*  @return BYTE		�������������
	*/
	virtual BYTE		GetTableRoleSum() const = 0;//��ȡ����������
	/**
	*  ��������: ���º��� ���ڸ��������ڲ�����
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		Update(DWORD dwTimer) = 0;//���Ӹ���

	ITable(const ITable&) = delete;
	ITable& operator=(const ITable&) = delete;
};

class ITableRole
{
public:
	/**
	*  ��������:		���캯��  explicit ����
	*  @param bSeatID   ���������ϵ�λ��
	*  @param dwUserID  ��ǰ���ID
	*/
	explicit ITableRole(BYTE bSeatID,DWORD dwUserID){}
	virtual ~ITableRole(){}

	/**
	*  ��������:		��ȡ��ҵ�ID
	*
	*  @return DWORD	���ID
	*/
	virtual DWORD	GetRoleID() const = 0;//��ȡ���ID
	/**
	*  ��������:		��ȡ��ҵ��ⲿ����ָ��
	*
	*  @return Role*	���ָ�� �п���Ϊ nullptr
	*/
	virtual	Role*	GetRole() const = 0;//��ȡ�������ָ��
	/**
	*  ��������:		��ȡ��ҵ�λ������(��0��ʼ)
	*
	*  @return BYTE		��ҵ�λ������(��0��ʼ)
	*/
	virtual BYTE	GetSeatID() const = 0;//��ȡ�����λID

	ITableRole(const ITableRole&) = delete;
	ITableRole& operator=(const ITableRole&) = delete;
};

class IHandleCommon
{
public:
	/**
	*  ��������:		���캯��  explicit ����
	*  @param eRoomType ��������
	*  @param pManager  ������������ ���ڲ�ѯ����
	*  @param pRoleManager  ������ݻ������� �������úͻ�ȡ��ҵ�����ID �ͷ���ID
	*/
	explicit IHandleCommon(RoomType eRoomType, IRoomManager* pManager, IRoomRoleManager* pRoleManager){}
	virtual ~IHandleCommon(){}

	/**
	*  ��������:		��ȡ��ǰ�ӿڵķ�������ö��
	*
	*  @return RoomType	��������ö��
	*/
	virtual RoomType GetRoomType() const = 0;//��ȡ��ǰ������������ķ�������ID
	/**
	*  ��������: ���ڴ������������������Ϣ ������Դ��� ����true  �����Դ�����false
	*  @param	NetCmd*		�����ָ��  �����Ϊnullptr
	*
	*  @return  bool		�Ƿ���Դ���
	*/
	virtual bool HandleMsg(NetCmd* pCmd) = 0; //��������

	IHandleCommon(const IHandleCommon&) = delete;
	IHandleCommon& operator=(const IHandleCommon&) = delete;
};


//�����ʵ����
class RoomManager : public IRoomManager, public IRoomRoleManager
{
public:
	explicit RoomManager();
	virtual ~RoomManager();

	/**
	*  ��������:		���ݲ����������󷿼�ӿ�ָ��
	*  @param bRoomID   ����ID
	*  @param eRoomType ���������ö��
	*
	*  @return IRoom*   ����ĳ���ӿ�
	*/
	virtual	 IRoom*		CreateRoom(BYTE bRoomID,RoomType eRoomType);
	/**
	*  ��������: ���ݷ���ID ��ѯ�������� ��������� ����nullptr
	*  @param	bRoomID		����ID
	*
	*  @return	IRoom*		����ĳ���ӿ�
	*/
	virtual  IRoom*		FindRoom(BYTE bRoomID) const;
	/**
	*  ��������: ���ݷ���ID ������ɾ����
	*  @param	bRoomID		����ID
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		DestoryRoom(BYTE bRoomID);
	/**
	*  ��������: ���º��� ���ڸ��¹������ڲ�����
	*  @param	dwTimer		��ǰʱ�� ��timeGetTime()����
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		Update(DWORD dwTimer);
	/**
	*  ��������: �����ⲿ��Ϣ����
	*  @param	pCmd		�ⲿ�����ָ��
	*
	*  @return  void		�޷��ز���
	*/
	virtual  void		HandleMsg(NetCmd* pCmd);
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

	RoomManager(const RoomManager&) = delete;
	RoomManager& operator=(const RoomManager&) = delete;
private:
	/**
	*  ��������:		��ʼ������ �ڹ��캯���е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__Init();//��ʼ��
	/**
	*  ��������:		���ٺ��� �����������е���һ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__Destory();//����
	/**
	*  ��������:		���������������ȫ�������ָ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__ClearRoom();//������
	/**
	*  ��������:		���������������ȫ����Ϣģ���ָ��
	*
	*  @return void		�޷���ֵ
	*/
	void				__ClearHandleCommon();//������ģ��
	/**
	*  ��������:		������ҷ�������Ӽ�¼�Ļ���
	*
	*  @return void		�޷���ֵ
	*/
	void				__ClearRoleCache();//������ҷ�������Ӽ�¼�Ļ���
	/**
	*  ��������:		�ж�ָ�����͵���Ϣģ���Ƿ����
	*  @param eRoomType �������͵�ö��
	*
	*  @return bool		���ڷ���true  �����ڷ���false
	*/
	bool				__IsExistsHandleCommon(RoomType eRoomType) const;//�ж��Ƿ��Ѿ�����ָ���������͵���Ϣ����ģ��
	/**
	*  ��������:		����µ���Ϣ����ģ��
	*  @param pHandleCommon �µ���Ϣ����ģ���ָ��  �п���Ϊnullptr
	*
	*  @return void		�޷���ֵ
	*/
	void				__AddHandleCommon(IHandleCommon* pHandleCommon);//����µ���Ϣ����ģ��
	/**
	*  ��������:		����µķ���
	*  @param pRoom		�µķ����ָ��  �п���Ϊnullptr
	*
	*  @return void		�޷���ֵ
	*/
	void				__AddRoom(IRoom* pRoom);//��ӷ���
	/**
	*  ��������:		��ȡָ�����͵���Ϣ����ģ��
	*  @param eRoomType	��������ö��
	*
	*  @return IHandleCommon* ģ��ָ��  ���ܷ���nullptr
	*/
	IHandleCommon*		__GetHandleCommon(RoomType eRoomType) const;
private:
	HashMap<BYTE, IRoom*>					m_RoomHashMap;//ȫ�������HashMap
	HashMap<RoomType,IHandleCommon*>		m_HandleCommonHashMap;//ȫ����Ϣ����ģ���HashMap
	HashMap<DWORD, BYTE>					m_RoleRoomIDHashMap;//��ҷ���ID�Ļ���
	HashMap<DWORD, WORD>					m_RoleTableIDHashMap;//�������ID�Ļ���
};