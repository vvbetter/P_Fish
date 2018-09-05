
// TestServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestServer.h"
#include "TestServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ServerManager.h"
HashMap<UINT, int> g_Map;

ServerManager g_Server;
TestSql g_Sql;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestServerDlg 对话框



CTestServerDlg::CTestServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JOIN, m_JoinList);
	DDX_Control(pDX, IDC_STATIC1, m_NumCaption);
}

BEGIN_MESSAGE_MAP(CTestServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
//	ON_WM_DESTROY()
ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTestServerDlg 消息处理程序

BOOL CTestServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	g_Server.Init();
	SetTimer(0, 1000, NULL);

	m_JoinList.InsertColumn(0, L"线程", 0, 100);
	m_JoinList.InsertColumn(1, L"在线数量", 0, 100);
	m_JoinList.InsertColumn(2, L"消耗1(MS)", 0, 100);
	m_JoinList.InsertColumn(3, L"消耗2(MS)", 0, 100);

	m_JoinList.SetExtendedStyle(m_JoinList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	WCHAR str[20];
	const ServerInitData &sid = g_Server.GetServer().GetInitData();
	for (int i = 0; i < sid.RecvThreadNum; ++i)
	{
		swprintf_s(str, 20, L"接收线程:%d", i);
		m_JoinList.InsertItem(i, str);
	}
	g_Sql.Init();
	for (int i = 0; i < sid.SendThreadNum; ++i)
	{
		swprintf_s(str, 20, L"发送线程:%d", i);
		m_JoinList.InsertItem(i + sid.RecvThreadNum, str);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
	g_Server.Update();
	WCHAR str[20];
	const ServerInitData &sid = g_Server.GetServer().GetInitData();
	for (int i = 0; i < sid.RecvThreadNum; ++i)
	{
		swprintf_s(str, 20, L"%d", g_Server.GetServer().GetRecvThreadData(i).Num);
		m_JoinList.SetItemText(i, 1, str);
		swprintf_s(str, 20, L"%f", g_Server.GetServer().GetRecvThreadData(i).Delta1);
		m_JoinList.SetItemText(i, 2, str);
		swprintf_s(str, 20, L"%f", g_Server.GetServer().GetRecvThreadData(i).Delta2);
		m_JoinList.SetItemText(i, 3, str);
	}
	for (int i = 0; i < sid.SendThreadNum; ++i)
	{
		swprintf_s(str, 20, L"%d", g_Server.GetServer().GetSendThreadData(i).Num);
		m_JoinList.SetItemText(i + sid.RecvThreadNum, 1, str);
		swprintf_s(str, 20, L"%f", g_Server.GetServer().GetSendThreadData(i).Delta1);
		m_JoinList.SetItemText(i + sid.RecvThreadNum, 2, str);
	}

	swprintf_s(str, L"服务器在线:%d", g_Server.GetServer().JoinNum());
	SetWindowTextW(str);
}


//void CTestServerDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO:  在此处添加消息处理程序代码
//}


void CTestServerDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
	g_Server.Close();
	g_Sql.Shutdown();
}
