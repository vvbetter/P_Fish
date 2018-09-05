
// TestClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestClient.h"
#include "TestClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ClientManager.h"

ClientManager g_Client;
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


// CTestClientDlg 对话框



CTestClientDlg::CTestClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ClientList);
}

BEGIN_MESSAGE_MAP(CTestClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTestClientDlg 消息处理程序

BOOL CTestClientDlg::OnInitDialog()
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

	g_Client.Init("127.0.0.1", 10);
	SetTimer(0, 1000, 0);
	// TODO:  在此添加额外的初始化代码

	m_ClientList.InsertColumn(0, L"线程", 0, 100);
	m_ClientList.InsertColumn(1, L"在线数量", 0, 100);
	m_ClientList.InsertColumn(2, L"消耗1(MS)", 0, 100);

	m_ClientList.SetExtendedStyle(m_ClientList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	WCHAR str[20];
	for (UINT i = 0; i < g_Client.m_Client.GetThreadNum(); ++i)
	{
		swprintf_s(str, 20, L"接收线程:%d", i);
		m_ClientList.InsertItem(i, str);
	}

	for (UINT i = 0; i < g_Client.m_Client.GetThreadNum(); ++i)
	{
		swprintf_s(str, 20, L"发送线程:%d", i);
		m_ClientList.InsertItem(i + g_Client.m_Client.GetThreadNum(), str);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestClientDlg::OnPaint()
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
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	WCHAR str[20];
	
	UINT num = g_Client.m_Client.GetThreadNum();
	UINT allNum = 0;
	for (UINT i = 0; i < num; ++i)
	{
		allNum += g_Client.m_Client.GetRecvThreadData(i).Num;
		swprintf_s(str, 20, L"%d", g_Client.m_Client.GetRecvThreadData(i).Num);
		m_ClientList.SetItemText(i, 1, str);
		swprintf_s(str, 20, L"%f", g_Client.m_Client.GetRecvThreadData(i).Delta);
		m_ClientList.SetItemText(i, 2, str);
	}

	for (UINT i = 0; i < num; ++i)
	{
		swprintf_s(str, 20, L"%d", g_Client.m_Client.GetSendThreadData(i).Num);
		m_ClientList.SetItemText(i + num, 1, str);
		swprintf_s(str, 20, L"%f", g_Client.m_Client.GetSendThreadData(i).Delta);
		m_ClientList.SetItemText(i + num, 2, str);
	}

	swprintf_s(str, L"客户端在线:%d", allNum);
	SetWindowTextW(str);
}


void CTestClientDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
	g_Client.Close();
}
