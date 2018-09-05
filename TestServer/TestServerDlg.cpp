
// TestServerDlg.cpp : ʵ���ļ�
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTestServerDlg �Ի���



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


// CTestServerDlg ��Ϣ�������

BOOL CTestServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	g_Server.Init();
	SetTimer(0, 1000, NULL);

	m_JoinList.InsertColumn(0, L"�߳�", 0, 100);
	m_JoinList.InsertColumn(1, L"��������", 0, 100);
	m_JoinList.InsertColumn(2, L"����1(MS)", 0, 100);
	m_JoinList.InsertColumn(3, L"����2(MS)", 0, 100);

	m_JoinList.SetExtendedStyle(m_JoinList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	WCHAR str[20];
	const ServerInitData &sid = g_Server.GetServer().GetInitData();
	for (int i = 0; i < sid.RecvThreadNum; ++i)
	{
		swprintf_s(str, 20, L"�����߳�:%d", i);
		m_JoinList.InsertItem(i, str);
	}
	g_Sql.Init();
	for (int i = 0; i < sid.SendThreadNum; ++i)
	{
		swprintf_s(str, 20, L"�����߳�:%d", i);
		m_JoinList.InsertItem(i + sid.RecvThreadNum, str);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	swprintf_s(str, L"����������:%d", g_Server.GetServer().JoinNum());
	SetWindowTextW(str);
}


//void CTestServerDlg::OnDestroy()
//{
//	CDialogEx::OnDestroy();
//
//	// TODO:  �ڴ˴������Ϣ����������
//}


void CTestServerDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
	g_Server.Close();
	g_Sql.Shutdown();
}
