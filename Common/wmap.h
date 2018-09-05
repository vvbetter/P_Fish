//mymap.h
//使用该map的方式：包含该头文件即可使用
#ifndef _WDMAP_MYMAP_H_
#define _WDMAP_MYMAP_H_

#if defined  _WIN32                                                         //Windows
#include <Windows.h>
#define MyMap_CLock_Mutex_t                 HANDLE
#define MyMap_CLock_Mutex_Init(_mutex)      (_mutex = CreateSemaphore(NULL,1,1,NULL))
#define MyMap_CLock_Mutex_Lock(_mutex)      (WaitForSingleObject(_mutex, INFINITE))
#define MyMap_CLock_Mutex_UnLock(_mutex)    (ReleaseSemaphore(_mutex,1,NULL))
#define MyMap_CLock_Mutex_Destroy(_mutex)   (CloseHandle(_mutex))
#define MyMap_Declar_Typename               typename
#define MyMap_Type_Typename

#elif defined __linux                                                       //Linux
#include <pthread.h>
#define MyMap_CLock_Mutex_t                 pthread_mutex_t
#define MyMap_CLock_Mutex_Init(_mutex)      (pthread_mutex_init(&_mutex, NULL))
#define MyMap_CLock_Mutex_Lock(_mutex)      (pthread_mutex_lock(&_mutex))
#define MyMap_CLock_Mutex_UnLock(_mutex)    (pthread_mutex_unlock(&_mutex))
#define MyMap_CLock_Mutex_Destroy(_mutex)   (pthread_mutex_destroy(&_mutex))
#define MyMap_Declar_Typename 
#define MyMap_Type_Typename                 typename
#endif

#include <map>

namespace wdmap
{
	//my iterator                                                           
	template<class K, class V>
	class WDItr
	{
	public:
		WDItr();
		WDItr(const std::map<K, V>& t_map);
		WDItr(const WDItr& val);
		~WDItr();


		//************************************
		// Method:    operator
		// FullName:  operator
		// Access:    public 
		// Returns:   
		// Qualifier: 迭代器没有实现自减运算符重载
		// Parameter: 
		//************************************
		WDItr<K, V>& operator=(const WDItr& val);
		WDItr<K, V>& operator++(int);
		bool operator==(const WDItr& val);
		bool operator!=(const WDItr& val);
		WDItr<K, V>& operator++();

	public:
		bool _isEnd;

		K first;
		V second;

	private:
		std::map<K, V> _tempMap;

		WDItr<K, V>* _oldSelf;
	};

	template<class K, class V>
	bool wdmap::WDItr<K, V>::operator!=(const WDItr& val)
	{
		return !(*this == val);
	}
	template<class K, class V>
	WDItr<K, V>& wdmap::WDItr<K, V>::operator++()
	{
		MyMap_Type_Typename std::map<K, V>::iterator _mitr = _tempMap.find(this->first);

		if (_mitr == _tempMap.end() || ++_mitr == _tempMap.end())
		{
			this->_isEnd = true;

			return *this;
		}

		this->_isEnd = false;
		this->first = _mitr->first;
		this->second = _mitr->second;

		return *this;
	}

	template<class K, class V>
	bool wdmap::WDItr<K, V>::operator==(const WDItr& val)
	{
		if (this->_isEnd == true && val._isEnd == true)
		{
			return true;
		}

		if (this->_isEnd == val._isEnd && this->first == val.first)
		{
			return true;
		}

		return false;
	}
	template<class K, class V>
	WDItr<K, V>& wdmap::WDItr<K, V>::operator++(int)
	{
		if (this->_oldSelf)
		{
			delete this->_oldSelf;
			this->_oldSelf = NULL;
		}

		this->_oldSelf = new WDItr<K, V>(*this);

		++(*this);

		return *(this->_oldSelf);
	}
	template<class K, class V>
	WDItr<K, V>& wdmap::WDItr<K, V>::operator=(const WDItr& val)
	{
		if (this == &val)
		{
			return *this;
		}

		this->first = val.first;
		this->second = val.second;
		this->_isEnd = val._isEnd;
		this->_tempMap = val._tempMap;
		this->_oldSelf = NULL;

		if (val._oldSelf)
		{
			this->_oldSelf = new WDItr<K, V>(*val._oldSelf);
		}

		return *this;
	}
	template<class K, class V>
	wdmap::WDItr<K, V>::WDItr(const WDItr& val)
	{
		this->first = val.first;
		this->second = val.second;
		this->_isEnd = val._isEnd;
		this->_tempMap = val._tempMap;
		this->_oldSelf = NULL;

		if (val._oldSelf)
		{
			this->_oldSelf = new WDItr<K, V>(*val._oldSelf);
		}
	}
	template<class K, class V>
	wdmap::WDItr<K, V>::WDItr(const std::map<K, V>& t_map)
	{
		this->_isEnd = true;
		this->_tempMap = t_map;
		this->_oldSelf = NULL;
	}
	template<class K, class V>
	wdmap::WDItr<K, V>::~WDItr()
	{
		if (this->_oldSelf)
		{
			delete this->_oldSelf;
			this->_oldSelf = NULL;
		}
	}
	template<class K, class V>
	wdmap::WDItr<K, V>::WDItr()
	{
		this->_isEnd = true;
		this->_oldSelf = NULL;
	}

	//lock
	class CLock
	{
	public:
		CLock(){ MyMap_CLock_Mutex_Init(_mutex); }
		~CLock(){ MyMap_CLock_Mutex_Destroy(_mutex); }

		void Lock() { MyMap_CLock_Mutex_Lock(_mutex); }
		void UnLock() { MyMap_CLock_Mutex_UnLock(_mutex); }

	private:
		MyMap_CLock_Mutex_t _mutex;
	};

	//map
	template<class K, class V>
	class map
	{
	public:
		map(void);
		map(const map& val);
		~map(void);

		typedef MyMap_Declar_Typename WDItr<K, V> iterator;
		typedef unsigned int size_type;

		V& operator[](const K& t_key);
		map& operator=(const map& val);

	public:
		iterator begin();
		iterator end();

		void insert(const K t_key, const V t_val);
		iterator find(const K t_key);
		void erase(const K t_key);
		void erase(const iterator _val);
		bool update(const K t_key, const V t_val);
		bool update(const iterator _val);

		size_type size();

	private:
		std::map<K, V> _wdmap;

		CLock _myLock;
	};

	template<class K, class V>
	wdmap::map<K, V>& wdmap::map<K, V>::operator=(const map& val)
	{
		if (this == &val)
		{
			return *this;
		}

		this->_myLock.Lock();
		val._myLock.Lock();

		this->_wdmap = val._wdmap;

		val._myLock.UnLock();
		this->_myLock.UnLock();

		return *this;
	}
	template<class K, class V>
	wdmap::map<K, V>::map(const map& val)
	{
		val._myLock.Lock();

		this->_wdmap = val._wdmap;

		val._myLock.UnLock();
	}
	template<class K, class V>
	V& wdmap::map<K, V>::operator[](const K& t_key)
	{
		this->_myLock.Lock();

		MyMap_Type_Typename std::map<K, V>::iterator _mitr = this->_wdmap.find(t_key);
		if (_mitr == this->_wdmap.end())
		{
			this->_wdmap.insert(MyMap_Type_Typename std::map<K, V>::value_type(t_key, V()));
			_mitr = this->_wdmap.find(t_key);
		}

		this->_myLock.UnLock();

		return _mitr->second;
	}

	template<class K, class V>
	typename wdmap::map<K, V>::size_type wdmap::map<K, V>::size()
	{
		this->_myLock.Lock();

		wdmap::map<K, V>::size_type m_size = this->_wdmap.size();

		this->_myLock.UnLock();

		return m_size;
	}
	//************************************
	// Method:    update
	// FullName:  wdmap::map<K, V>::update
	// Access:    public 
	// Returns:   bool
	// Qualifier: 通过iterator更新map中具有的first key的value值，若无该key，则返回false
	// Parameter: const iterator _val
	//************************************
	template<class K, class V>
	bool wdmap::map<K, V>::update(const iterator _val)
	{
		return update(_val.first, _val.second);
	}
	//************************************
	// Method:    update
	// FullName:  wdmap::map<K, V>::update
	// Access:    public 
	// Returns:   bool
	// Qualifier: 通过key和value更新map中具有的key的value值，若无该key，则返回false
	// Parameter: const K t_key
	// Parameter: const V t_val
	//************************************
	template<class K, class V>
	bool wdmap::map<K, V>::update(const K t_key, const V t_val)
	{
		this->_myLock.Lock();
		bool ret = false;
		MyMap_Type_Typename std::map<K, V>::iterator _mitr = this->_wdmap.find(t_key);
		if (_mitr != this->_wdmap.end())
		{
			_mitr->second = t_val;
			ret = true;
		}

		this->_myLock.UnLock();

		return ret;
	}
	template<class K, class V>
	void wdmap::map<K, V>::erase(const  iterator _val)
	{
		this->_myLock.Lock();

		this->_wdmap.erase(_val.first);

		this->_myLock.UnLock();
	}
	template<class K, class V>
	void wdmap::map<K, V>::erase(const K t_key)
	{
		this->_myLock.Lock();

		this->_wdmap.erase(t_key);

		this->_myLock.UnLock();
	}
	//************************************
	// Method:    find
	// FullName:  wdmap::map<K, V>::find
	// Access:    public 
	// Returns:   ::iterator
	// Qualifier: 通过key进行查找，若没有查找到，则返回end
	// Parameter: const K t_key
	//************************************
	template<class K, class V>
	typename wdmap::map<K, V>::iterator wdmap::map<K, V>::find(const K t_key)
	{
		this->_myLock.Lock();

		wdmap::map<K, V>::iterator _tmpItr(this->_wdmap);

		MyMap_Type_Typename std::map<K, V>::iterator _mitr = this->_wdmap.find(t_key);
		if (_mitr != this->_wdmap.end())
		{
			_tmpItr._isEnd = false;
			_tmpItr.first = _mitr->first;
			_tmpItr.second = _mitr->second;
		}

		this->_myLock.UnLock();

		return _tmpItr;
	}
	//************************************
	// Method:    insert
	// FullName:  wdmap::map<K, V>::insert
	// Access:    public 
	// Returns:   
	// Qualifier: 插入key，value
	// Parameter: const K t_key
	// Parameter: const V t_val
	//************************************
	template<class K, class V>
	void wdmap::map<K, V>::insert(const K t_key, const V t_val)
	{
		this->_myLock.Lock();

		this->_wdmap.insert(MyMap_Type_Typename std::map<K, V>::value_type(t_key, t_val));

		this->_myLock.UnLock();

		return;
	}
	template<class K, class V>
	typename wdmap::map<K, V>::iterator wdmap::map<K, V>::end()
	{
		wdmap::map<K, V>::iterator _tmpItr;

		return _tmpItr;
	}
	template<class K, class V>
	typename wdmap::map<K, V>::iterator wdmap::map<K, V>::begin()
	{
		this->_myLock.Lock();

		wdmap::map<K, V>::iterator _tmpItr(this->_wdmap);

		MyMap_Type_Typename std::map<K, V>::iterator _mitr = this->_wdmap.begin();
		if (_mitr != this->_wdmap.end())
		{
			_tmpItr._isEnd = false;
			_tmpItr.first = _mitr->first;
			_tmpItr.second = _mitr->second;
		}

		this->_myLock.UnLock();

		return _tmpItr;
	}
	template<class K, class V>
	wdmap::map<K, V>::~map(void)
	{
	}
	template<class K, class V>
	wdmap::map<K, V>::map(void)
	{
	}
}

#endif