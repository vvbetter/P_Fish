
// TestServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTestServerDlg �Ի���
class CTestServerDlg : public CDialogEx
{
// ����
public:
	CTestServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_JoinList;
	CListCtrl m_QueueList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_NumCaption;
//	afx_msg void OnDestroy();
	afx_msg void OnClose();
};
