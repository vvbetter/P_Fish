#pragma once
#define HTTP_SAFE_ARRAY_COUNT 2048
struct HttpRecvData;
typedef vector<string> StrList;
class HttpCallback
{
public:
	virtual string RequestCallback(HttpRecvData *pRecvData) = 0;
};
struct WebAddrData
{
	UINT	RequestCode;					//��ַ�ı�ʶ��,��ͬ�ĳ�ֵ�����̣���ʹ�õĻص���ַ��һ�����������ֲ�ͬ�ĳ�ֵ�ӿڡ�
	char	WebAddr[256];
	char	Response[128];					//����ɹ������صĽ���ַ�����ʧ��ʼ�շ���"fail"��
	UINT	WebAddrLength;					//����ĵ�ַ����
	bool	bKeyToLower;					//��KEYת����Сд
	HashMap<UINT, BYTE> ValidationFields;	//Ҫ��֤���ֶ��б�
	HttpCallback *pCallback;
};
inline void StringToLower(string &str)
{
	for (UINT j = 0; j < str.size(); ++j)
		str[j] = static_cast<char>(tolower(str[j]));
}
//��ָ�����ַ��ָ��ַ���,bEmptyAdd���ַ���Ҳ��ӡ�
inline UINT SplitStrByChar(const char *pc, char c, StrList &strList, bool bEmptyAdd)
{
	size_t size = strlen(pc);
	const char *ps = pc;
	const char *ps_end = ps + size;
	UINT count = 0;
	while (ps < ps_end)
	{
		const char *pfind = strchr(ps, c);
		if (pfind == NULL)
			pfind = ps_end;

		if (pfind != ps || bEmptyAdd)
		{
			string str;
			str.append(ps, static_cast<unsigned int>(pfind - ps));
			strList.push_back(str);
			++count;
		}
		ps = pfind + 1;
	}
	return count;
}

struct HttpServerInitData
{
	//pcValidationField ��֤�������������ֶΣ��Զ��ŷָ�, bToLowerת��ΪСд
	void AddWebAddr(UINT requestCode, const char *pc, const char *pcResponse, bool bKeyToLower = true, const char *pcValidationField = NULL, HttpCallback *pCallback = NULL)
	{
		WebAddrData addr;
		addr.RequestCode = requestCode;
		addr.WebAddrLength = strlen(pc);
		addr.pCallback = pCallback;
		if (addr.WebAddrLength >= sizeof(addr.WebAddr))
		{
			Log("��ַ����:%s", pc);
			return;
		}
		UINT strCount = strlen(pcResponse);
		if (strCount >= sizeof(addr.Response))
		{
			Log("���ص���Ӧ̫��:%s", pcResponse);
			return;
		}
		sprintf_s(addr.Response, "%d\r\n%s\r\n", strCount, pcResponse);

		for (UINT i = 0; i < addr.WebAddrLength; ++i)
			addr.WebAddr[i] = static_cast<char>(tolower(pc[i]));
		addr.bKeyToLower = bKeyToLower;
		if (pcValidationField)
		{
			StrList strList;
			SplitStrByChar(pcValidationField, ',', strList, false);
			for (UINT i = 0; i < strList.size(); ++i)
			{
				if (bKeyToLower)
					StringToLower(strList[i]);
				UINT crc = AECrc32(strList[i].c_str(), strList[i].size(), 0);
				addr.ValidationFields.insert(make_pair(crc, static_cast<BYTE>(0)));
			}
		}
		AddrList.push_back(addr);
	}
	vector<WebAddrData> AddrList;
	USHORT	Port;
	UINT	ThreadNum;
	UINT	Timeout;
	UINT	SleepTime;
};
typedef map<UINT, string>		HttpValueMap;
typedef SafeArray<SOCKET>		SocketList;
struct HttpClientData
{
	bool	IsPost;
	bool	Removed;
	SOCKET	Socket;
	UINT	Tick;
	char	RequestUrl[256];
	char    Buff[4096];
	int		RecvSize;
	UINT	Content_Start;
	UINT	Content_Length;
};
struct HttpRecvData
{
	HttpRecvData()
	{
		IsCallbackSend = false;
	}
	HttpRecvData(int n)
	{
		UNREFERENCED_PARAMETER(n);
	}
	string		 ResponseStr;
	bool		 IsCallbackSend;
	std::string  PostData;
	USHORT		 RequestIdx;
	UINT		 RequestCode;
	HttpValueMap KeyValues;
	
};

typedef SafeArray<HttpRecvData>  HttpRecvList;
struct ThreadRecvData
{
	ThreadRecvData() :
	ThreadRecvList(HTTP_SAFE_ARRAY_COUNT), 
	NewSockeList(HTTP_SAFE_ARRAY_COUNT)
	{

	}
	SocketList		NewSockeList;
	HttpRecvList	ThreadRecvList;
};
inline void GetGMTTimeStr(char* buff, int size, tm *ptm = NULL);
class HttpServer
{
public:
	HttpServer():m_RecvList(HTTP_SAFE_ARRAY_COUNT)
	{
		m_pThreadRecvList = NULL;
		m_Socket = NULL;
		m_bRun = false;
		m_ThreadNum = 0;
		m_ExitNum = 0;
	}
	bool Init(const HttpServerInitData &hsi);
	void _RecvThread();
	void _AcceptThread();
	void Shutdown();
	long TotalRecvDataCount()
	{
		return m_TotalRecvDataCount;
	}
	bool HasRecvData()
	{
		return m_RecvList.HasItem();
	}
	HttpRecvList& GetRecvList()
	{
		return m_RecvList;
	}
protected:
	bool	SplitStr(HttpClientData *pc, UINT endSize, HttpRecvData &strmap);

	bool	CheckPostData(HttpClientData *pc, HttpRecvData &recvData);
	volatile long	m_bRun;
	volatile long	m_ThreadNum;
	volatile UINT	m_ExitNum;
	volatile long	m_TotalRecvDataCount;
	
	SOCKET				m_Socket;
	HttpServerInitData	m_InitData;
	ThreadRecvData		*m_pThreadRecvList;
	HttpRecvList		m_RecvList;
};