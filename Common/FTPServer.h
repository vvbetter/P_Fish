#pragma once
#define OPEN_FILE_STATE			0xEFFFFFFF
#define NEW_CLIENT_COUNT		1024
#define MAX_FILE_COUNT			10	//����ܳ���127��
#include "Cmd_Fish.h"
#pragma pack(push)
#pragma pack(1)
struct NetCmdFileRequest :public NetCmd
{
	USHORT	Length;
	USHORT	FileCount;
	char	FileName[1];
};
struct NetCmdCheckVersion :public NetCmd
{
	UINT	Version;
	BYTE	Plateform;
	USHORT	ScreenWidth;
	USHORT	ScreenHeight;
	char	Mac[48];
	char	Package[48];
	USHORT	Length;
	char	FileName[1];
};
#pragma pack(pop)
enum FTPCmdType
{
	CMD_CHECK_VERSION = (Main_Extra_Down << 8) | (EXTRA_DOWN_CHECK_VER),
	CMD_FILE_REQUEST = (Main_Extra_Down << 8) | (EXTRA_DOWN_FILE),
	CMD_FILE_REQUEST_EX = (Main_Extra_Down << 8) | (EXTRA_DOWN_FILE_EX),
};
enum FTPOpenFileState
{
	FTP_OPEN_WAIT,
	FTP_OPEN_FAILED,
	FTP_OPEN_OK,
};
enum FTPRecvState
{
	FTP_RECV_OK,
	FTP_RECV_ERROR,
	FTP_RECV_FILE_ERROR,
	FTP_RECV_CONTINUE,
};
enum FTPRemoveType
{
	FTP_REMOVE_QUEUE_TIMEOUT,
	FTP_REMOVE_THREAD_NOTSPACE,
	FTP_REMOVE_RECVCMD_TIMEOUT,
	FTP_REMOVE_RECVCMD_ERROR,
	FTP_REMOVE_OPEN_FAILED,
	FTP_REMOVE_OPEN_TIMEOUT,
	FTP_REMOVE_TRANS_TIMEOUT,
	FTP_REMOVE_TRANS_COMPLETION,
	FTP_REMOVE_CREATE_UDP_FAILED,
	FTP_REMOVE_RECV_LOST_ERROR,
	FTP_REMOVE_UDP_CONNECT_TIMEOUT,
};
enum FTPFileChangeType
{
	FTP_FILE_NONE,
	FTP_FILE_DEL,
	FTP_FILE_RENAME,
	FTP_FILE_MODIFY
};
struct FTPFileData
{
	volatile UINT	UseCount;
	UINT			LastTick;
	UINT			FileNameCrc;
	UINT			FileLength;
	char			FileData[1];
};
struct FTPFileModify
{
	FTPFileModify()
	{
	}
	FTPFileModify(int n)
	{
	}
	BYTE			Opt;
	UINT			FileNameCrc;
	UINT			NewFileNameCrc;
};
struct FTPServerInitData
{
	FTPServerInitData()
	{
		BandwidthUsePercent = 0.7f;//
		SocketRecvBuffSize = 2048;
		SocketSendBuffSize = 8192;
		Timeout = 3000;
		SleepTime = 10;
		AcceptPerFrame = 64;
		FileCacheTime = 1000 * 60 * 10;//10����
		RecvThreadNum = 4;
		bFindISP = true;
		PerMaxSendSize = 65536 * 8;
	}
	UINT	SendSafeCount;			//��ȫ���ط�����
	UINT	SendQueueInterval;		//ms, �����Ŷӵļ��
	UINT	SendErrorInterval;		//����ʧ�ܵļ��ʱ��
	UINT	SendChunkInterval;		//ms ���͵�������С���
	UINT	PerMaxSendSize;			//ÿ�η��͵�����ֽ�
	UINT	RecvThreadNum;
	UINT	FileCacheTime;
	UINT	AcceptPerFrame;
	UINT	Timeout;
	UINT	SleepTime;
	UINT	SocketRecvBuffSize;
	UINT	SocketSendBuffSize;
	UINT	ClinetMinSpeed;			//Byte��ÿ���ͻ���ÿ����ʹ����ٶ�
	UINT	Bandwidth;				//Byte, ���������ܴ������������BITҪ����8�������������ֽڴ���
	float	BandwidthUsePercent;	//�����ʹ�ðٷֱȣ�����0.7f���ڴ��䣬0.3�������Ӻ��Ŷӵ�����.
	UINT	BuffSize;
	USHORT	Port;
	UINT	PathLength;
	bool	bFindISP;				//�Ƿ��ѯ�����Ӫ�̡�
	char	FTPPath[256];
};
struct FileNameData
{
	char	*pcName;
	USHORT	NameLength;
	UINT	Offset;
	UINT	Length;
};
enum ISPType
{
	ISP_DX = 0,		//����
	ISP_LT,			//��ͨ
	ISP_YD,			//�ƶ�
};
struct FTPClientData
{
	FTPClientData()
	{
	}
	UINT			MacCrc;
	bool			XorData;
	bool			CheckVer;
	BYTE			ISPType;
	SOCKET			Socket;
	UINT			IP;
	UINT			LocalIP;
	USHORT			Port;
	UINT			Tick;
	UINT			SendTick;
	UINT			OnlineTick;
	UINT			SendSize;
	UINT			TotalSendSize;
	UINT			RecvSize;
	UINT			SendErrorCount;
	BYTE			RemoveType;
	FTPFileData*	pFileHandler[MAX_FILE_COUNT];
	FileNameData	RequestFileList[MAX_FILE_COUNT];
	BYTE			RequestFileCount;
	BYTE			SendFileIndex;
	char			IPStr[30];
	UINT			IPRequestTick;
	volatile BYTE	OpenFileState;
	HashMap<uint, uint> *pResendList;
	char			Buff[1];
};

struct FTPThreadData
{
	FTPThreadData() :NewClients(NEW_CLIENT_COUNT){
	}
	SafeArray<FTPClientData*>	NewClients;
};

//FTP ʹ��1��Accept�̡߳�N��RecvCMD�̡߳�1��Queue�̡߳�1�������̡߳�
typedef HashMap<UINT, FTPFileData*> FTPFileMap;
typedef vector<FTPFileData*>		FTPFileList;
class FTPServer
{
public:
	FTPServer();
	~FTPServer();
	bool	Init(const FTPServerInitData &data);

	void	_ThreadAccept();
	void	_ThreadTrans();
	void	_ThreadQueue();
	void	_ThreadRecvCmd();
	void	_ThreadOpenFile();
	void	_ThreadFileWatcher();
protected:
	uint				InitFileData(FTPClientData *pc);
	void				CheckFileList(FTPFileMap *pfilemap, FTPFileList*pfileList);
	FTPFileData*		GetFileHandle(FTPFileMap *pfilemap, char *pname, UINT nameLength);
	FTPRecvState		RecvData(FTPClientData *pc);
	void				RemoveClient(FTPClientData *pclient, FTPRemoveType remove);
	UINT				GetSendInterval(UINT sendErrCount, UINT baseInterval);
	CACHELINE(0);
	volatile bool		m_bRun;
	CACHELINE(1);
	volatile UINT		m_TransNum;
	CACHELINE(2);
	volatile UINT		m_RecvCmdNum;
	CACHELINE(3);
	volatile UINT		m_SuccessNum;
	CACHELINE(4);
	volatile UINT		m_RecvThreadIdx;
	CACHELINE(5);
	volatile UINT		m_QueueNum;
	CACHELINE(6);
	FTPThreadData		m_QueueThreadData;
	CACHELINE(7);
	FTPThreadData		m_TransThreadData;
	CACHELINE(8);
	FTPFileMap				m_FileList;
	CACHELINE(9);
	FTPThreadData		   *m_pRecvThreadData;
	CACHELINE(10);
	FTPThreadData		   *m_pQueueThreadData;
	CACHELINE(11);
	FTPThreadData		   *m_pOpenFileThreadData;
	CACHELINE(12);
	volatile UINT		   m_MaxQueueNum;
	CACHELINE(13);
	volatile UINT		   m_MaxRecvCmdNum;
	CACHELINE(14);
	volatile UINT		   m_MaxTransNum;
	CACHELINE(15);
	SafeArray<FTPFileModify>  m_FileModify;
	CACHELINE(16);
	volatile UINT			m_TodayCompletionNum;
	CACHELINE(17);
	HttpClient				m_HTTPClient;
	struct tm				m_Today;
	FTPFileList				m_FreeFileList;
	HANDLE					m_DirHandle;
	UINT					m_TotalTransNum;	//����ͬʱ���������
	UINT					m_TransBandwidth;
	FTPServerInitData		m_InitData;
	SOCKET					m_Socket;

};