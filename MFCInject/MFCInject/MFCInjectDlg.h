
// MFCInjectDlg.h: 头文件
//

#pragma once

#include "CaptureDlg.h"

// CMFCInjectDlg 对话框
class CMFCInjectDlg : public CDialogEx
{
// 构造
public:
	CMFCInjectDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCINJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	static CString generateErrorInfo(const CString& customError = _T(""));
	static bool RemoteThreadInject(DWORD dwProcessID, const CStringW& dllPath, CString* strError = nullptr);

// 实现
protected:
	HICON m_hIcon;
	HWND		m_currentHwnd = NULL;
	DWORD	m_currentProcessID = 0;
	CaptureDlg m_captureDlg;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPickor();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	void updateCurrentPosInfo();
	void updateCurrentProcessInfo(HWND hwnd);
	void updateCurrentProcessInfo(DWORD dwProcessID);
public:
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnExplore();
	afx_msg void OnBnClickedBtnInject();
};
