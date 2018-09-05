
// TestClientDlg.cpp : ʵ���ļ�
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


// CTestClientDlg �Ի���



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


// CTestClientDlg ��Ϣ�������

BOOL CTestClientDlg::OnInitDialog()
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

	g_Client.Init("127.0.0.1", 10);
	SetTimer(0, 1000, 0);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	m_ClientList.InsertColumn(0, L"�߳�", 0, 100);
	m_ClientList.InsertColumn(1, L"��������", 0, 100);
	m_ClientList.InsertColumn(2, L"����1(MS)", 0, 100);

	m_ClientList.SetExtendedStyle(m_ClientList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	WCHAR str[20];
	for (UINT i = 0; i < g_Client.m_Client.GetThreadNum(); ++i)
	{
		swprintf_s(str, 20, L"�����߳�:%d", i);
		m_ClientList.InsertItem(i, str);
	}

	for (UINT i = 0; i < g_Client.m_Client.GetThreadNum(); ++i)
	{
		swprintf_s(str, 20, L"�����߳�:%d", i);
		m_ClientList.InsertItem(i + g_Client.m_Client.GetThreadNum(), str);
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestClientDlg::OnPaint()
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
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

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

	swprintf_s(str, L"�ͻ�������:%d", allNum);
	SetWindowTextW(str);
}


void CTestClientDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
	g_Client.Close();
}
