
// TestServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTestServerDlg 对话框
class CTestServerDlg : public CDialogEx
{
// 构造
public:
	CTestServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
