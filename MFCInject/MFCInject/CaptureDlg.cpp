#include "pch.h"
#include "CaptureDlg.h"

struct CaptureDlgPrivate
{
	HWND m_hWnd = NULL;
	int m_highLightCount = 0;
	static const int highLightTimeId = 0;

	void registerCaptureDlgClass();
	bool createWindow();

	static void showWindow(HWND hwnd);
	static void hideWindow(HWND hwnd);
	static LRESULT wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

void CaptureDlgPrivate::showWindow(HWND hwnd)
{
	if (::IsWindow(hwnd) && !::IsWindowVisible(hwnd))
		::ShowWindow(hwnd, SW_SHOWNOACTIVATE);
}

void CaptureDlgPrivate::hideWindow(HWND hwnd)
{
	if (::IsWindow(hwnd))
		::ShowWindow(hwnd, SW_HIDE);
}

LRESULT CaptureDlgPrivate::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		RECT rc = {};
		::GetClientRect(hwnd, &rc);
		int nWidth = rc.right;
		int nHeight = rc.bottom;;
		HDC hDC;
		PAINTSTRUCT ps;
		hDC = ::BeginPaint(hwnd, &ps);
		HDC hMemDC = ::CreateCompatibleDC(NULL);//建立内存兼容DC
		HBITMAP hBmpMem = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
		::SelectObject(hMemDC, hBmpMem);
		::SelectObject(hMemDC, ::GetStockObject(WHITE_BRUSH));
		HPEN hPen = ::CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		::SelectObject(hMemDC, hPen);

		::Rectangle(hMemDC, 0, 0, rc.right, rc.bottom);
		::BitBlt(hDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);//将内存DC中的内容拷贝到当前窗口DC中,并显示


		::DeleteObject(hBmpMem);//删除对象
		::DeleteObject(hPen);//删除对象
		::DeleteDC(hMemDC);//删除DC；

		::EndPaint(hwnd, &ps);
	}
	break;
	case WM_ERASEBKGND:
	{
		return true;
	}
	break;
	default:
		break;
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

void CaptureDlgPrivate::registerCaptureDlgClass()
{
	static bool init = false;
	if (init)
		return;
	init = true;

	WNDCLASSEX wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW |
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = CaptureDlgPrivate::wndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = ::GetModuleHandle(NULL);         // handle to instance 
	wcx.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL,
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = nullptr;                  // white background brush 
	wcx.lpszMenuName = L"MainMenu";    // name of menu resource 
	wcx.lpszClassName = L"MainWClass";  // name of window class 
	wcx.hIconSm = NULL;

	// Register the window class. 
	RegisterClassEx(&wcx);
}

bool CaptureDlgPrivate::createWindow()
{
	registerCaptureDlgClass();

	m_hWnd = ::CreateWindowW(
		L"MainWClass",        // name of window class 
		L"Sample",            // title-bar string 
		WS_POPUP, // top-level window 
		0,       // default horizontal position 
		0,       // default vertical position 
		100,       // default width 
		100,       // default height 
		(HWND)::GetDesktopWindow(),         // no owner window 
		(HMENU)NULL,        // use class menu 
		::GetModuleHandle(NULL),           // handle to application instance 
		(LPVOID)NULL);      // no window-creation data 

	LONG_PTR lStyleEx = ::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
	lStyleEx |= WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, lStyleEx);

	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) ^ 0x80000);
	::SetLayeredWindowAttributes(m_hWnd, RGB(255, 255, 255), 255 / 2, LWA_COLORKEY);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::ShowWindow(m_hWnd, SW_HIDE);
	::UpdateWindow(m_hWnd);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CaptureDlg
CaptureDlg::CaptureDlg()
	: d(new CaptureDlgPrivate())
{
	d->createWindow();
}

CaptureDlg::~CaptureDlg()
{

}

void CaptureDlg::updateCaptureDlgPos(HWND hwnd, int bShow/* = true*/)
{
	if (!hwnd)
		return;

	RECT rect = {0};
	::GetWindowRect(hwnd, &rect);
	updateCaptureDlgPos(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1, bShow);
}

void CaptureDlg::updateCaptureDlgPos(int x, int y, int width, int height, int bShow /*= true*/)
{
	::MoveWindow(d->m_hWnd, x, y, width, height, true);
	if (bShow)
		showCaptureDlg();
}

void CaptureDlg::showCaptureDlg()
{
	CaptureDlgPrivate::showWindow(d->m_hWnd);
}

void CaptureDlg::hideCaptureDlg()
{
	CaptureDlgPrivate::hideWindow(d->m_hWnd);
}