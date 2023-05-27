
// MFCInjectDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCInject.h"
#include "MFCInjectDlg.h"
#include "afxdialogex.h"
#include <psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int s_TimerId = 1;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCInjectDlg 对话框



CMFCInjectDlg::CMFCInjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCINJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCInjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCInjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PICKOR, &CMFCInjectDlg::OnBnClickedBtnPickor)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CMFCInjectDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CMFCInjectDlg::OnBnClickedBtnExplore)
	ON_BN_CLICKED(IDC_BTN_INJECT, &CMFCInjectDlg::OnBnClickedBtnInject)
END_MESSAGE_MAP()


// CMFCInjectDlg 消息处理程序

BOOL CMFCInjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
	//SetDlgItemTextW(IDC_EDIT_DLLPATH, CStringW(L"E:\\projectcode\\MFCInject\\x64\\Debug\\MFCLibraryInject.dll"));
	SetDlgItemTextW(IDC_EDIT_DLLPATH, CStringW(L"E:\\projectcode\\QtClassLibraryInject\\x64\\Debug\\QtClassLibraryInject.dll"));


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCInjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCInjectDlg::OnPaint()
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
HCURSOR CMFCInjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCInjectDlg::OnBnClickedBtnPickor()
{
	
}


BOOL CMFCInjectDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_BTN_PICKOR)->m_hWnd)
		{
			SetTimer(s_TimerId, 100, NULL);
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMFCInjectDlg::updateCurrentPosInfo()
{
	POINT pos = { 0 };
	::GetCursorPos(&pos);
	HWND hwnd = ::WindowFromPoint(pos);
	updateCurrentProcessInfo(hwnd);
	
	m_currentHwnd = hwnd;
}

void CMFCInjectDlg::updateCurrentProcessInfo(HWND hwnd)
{
	DWORD dwProcessID = 0;
	DWORD dwThreadID = ::GetWindowThreadProcessId(hwnd, &dwProcessID);
	m_currentProcessID = dwProcessID;

	CString strProcess;
	strProcess.Format(_T("%d"), dwProcessID);
	SetDlgItemText(IDC_EDIT_PROCESSID, strProcess);

	updateCurrentProcessInfo(dwProcessID);
}

void CMFCInjectDlg::updateCurrentProcessInfo(DWORD dwProcessID)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessID);
	if (hProcess)
	{
		CString filePath;
		::GetModuleFileNameEx(hProcess, NULL, filePath.GetBuffer(1024), 1024);
		SetDlgItemText(IDC_EDIT_PATH, filePath);

		::CloseHandle(hProcess);
	}
}

void CMFCInjectDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == s_TimerId)
	{
		if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
		{
			KillTimer(s_TimerId);
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			m_captureDlg.hideCaptureDlg();
			return;
		}

		updateCurrentPosInfo();
		m_captureDlg.updateCaptureDlgPos(m_currentHwnd);
		m_captureDlg.showCaptureDlg();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMFCInjectDlg::OnBnClickedBtnRefresh()
{
	updateCurrentProcessInfo(m_currentProcessID);
}

void CMFCInjectDlg::OnBnClickedBtnExplore()
{
	CString filePath;
	GetDlgItemText(IDC_EDIT_PATH, filePath);

	::ShellExecute(NULL, _T("open"), _T("explorer"), _T("/select,") + filePath, NULL, SW_SHOWDEFAULT);
}

void CMFCInjectDlg::OnBnClickedBtnInject()
{
	CStringW dllPath;
	GetDlgItemTextW(IDC_EDIT_DLLPATH, dllPath);
	CString strError;
	if (!RemoteThreadInject(m_currentProcessID, dllPath, &strError))
		MessageBox(strError);
	else
		MessageBox(L"注入成功!");
}

CString CMFCInjectDlg::generateErrorInfo(const CString& customError/* = _T("")*/)
{
	CString strError = customError;
	if (!customError.IsEmpty())
		strError.Append(_T("\r\n"));

	CString strErrorSystem;
	LPVOID lpMsgBuf;
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	strErrorSystem.Format(_T("error: code(%d) str(%s)"), GetLastError(), (LPTSTR)lpMsgBuf);
	::LocalFree(lpMsgBuf);

	strError.Append(strErrorSystem);

	return strError;
}

bool CMFCInjectDlg::RemoteThreadInject(DWORD dwProcessID, const CStringW& dllPath, CString* strError/* = nullptr*/)
{
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
	if (!hProcess)
	{
		if (strError)
			*strError = generateErrorInfo(_T("OpenProcess failed!"));
		return false;
	}

	int nLen = sizeof(WCHAR) * (dllPath.GetLength() + 1);
	LPVOID pBuf = ::VirtualAllocEx(hProcess, NULL, nLen, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!pBuf)
	{
		if (strError)
			*strError = generateErrorInfo(_T("VirtualAllocEx failed!"));

		::CloseHandle(hProcess);
		return false;
	}
	
	SIZE_T dwWrite = 0;
	if (!::WriteProcessMemory(hProcess, pBuf, dllPath.GetString(), nLen, &dwWrite))
	{
		if (strError)
			*strError = generateErrorInfo(_T("WriteProcessMemory failed!"));

		::VirtualFreeEx(hProcess, pBuf, 0, MEM_FREE);
		::CloseHandle(hProcess);
		return false;
	}


	HANDLE hRemoteThread = ::CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryW, pBuf, 0, 0);
	if (!hRemoteThread)
	{
		if (strError)
			*strError = generateErrorInfo(_T("WriteProcessMemory failed!"));

		::VirtualFreeEx(hProcess, pBuf, 0, MEM_FREE);
		CloseHandle(hProcess);
		return false;
	}

	::WaitForSingleObject(hRemoteThread, INFINITE);
	::CloseHandle(hRemoteThread);
	::VirtualFreeEx(hProcess, pBuf, 0, MEM_FREE);
	::CloseHandle(hProcess);
	return true;
}