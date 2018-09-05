#pragma once

//�ڶ��߳�PUSH�Ͷ��߳�POP��ʱ��ʹ�ã�����ܱ��⣬���ʹ��SafeArray��
//2���߳�PUSH��2���߳�POP�ܷ����������ƣ��̴߳ﵽ8��PUSH��8��POP��ʱ�����ܷ����½���
enum SAI_STATE
{
	SAI_NONE,
	SAI_USED,
};
template<typename T>
struct SafeArrayItem
{
	SafeArrayItem()
	{
		State = SAI_NONE;
	}
	CACHELINE32(0);
	volatile BYTE State;
	CACHELINE32(1);
	T		Item;
};
template<typename T>
class SafeArrayMT
{
public:
	typedef SafeArrayItem<T> Items;
	SafeArrayMT(UINT count)
	{ 
		m_ArraySize		= max(AlignToPow2(count), 8);
		m_ArraySizeInv	= m_ArraySize - 1;
		m_pItems		= new Items[m_ArraySize];
		m_PushIdx		= m_PopIdx = 0;
	}
	~SafeArrayMT()
	{
		delete[](m_pItems);
		m_pItems = NULL;
	}
	bool Push(const T & t)
	{
	PUSH_LOOP:
		UINT idx = m_PushIdx;
		Items &it = m_pItems[idx & m_ArraySizeInv];
		if (it.State == SAI_NONE)
		{
			if (::InterlockedCompareExchange(&m_PushIdx, idx + 1, idx) == idx)
			{
				it.Item = t;
				it.State = SAI_USED;
				return true;
			}
			else
				goto PUSH_LOOP;
		}
		else if (idx != m_PushIdx)
			goto PUSH_LOOP;
		else
			return false;
	}
	//���ڶ��߳�ԭ���пռ����Ԫ�صļ��������壬ʼ�շ���true
	bool HasSpace()const
	{
		return true;	
	}
	bool HasItem()const
	{
		return true;
	}
	//���ڶ��̵߳�ԭ��Count()Ҳ�ǲ�׼ȷ�ģ�ʼ�շ���1
	UINT Count()const
	{
		return 1;
	}
	bool Pop(T & t)
	{
	POP_LOOP:
		UINT idx = m_PopIdx;
		Items &it = m_pItems[idx & m_ArraySizeInv];
		if (it.State == SAI_USED)
		{
			if (::InterlockedCompareExchange(&m_PopIdx, idx + 1, idx) == idx)
			{
				t = it.Item;
				it.State = SAI_NONE;
				return true;
			}
			else
				goto POP_LOOP;
		}
		else if (idx != m_PopIdx)
			goto POP_LOOP;
		else
			return false;
	}
protected:
	CACHELINE(0);
	volatile UINT		m_PushIdx;
	CACHELINE(2);
	volatile UINT		m_PopIdx;
	CACHELINE(4);
	Items* volatile		m_pItems;
	UINT				m_ArraySize;
	UINT				m_ArraySizeInv;
};
