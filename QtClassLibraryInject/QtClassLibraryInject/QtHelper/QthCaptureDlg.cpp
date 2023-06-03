#include "QthCaptureDlg.h"

#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QTimer>
#include <QWindow>

namespace Qth
{
	//////////////////////////////////////////////////////////////////////////
	// CaptureDlgMgr
	CaptureDlgMgr::CaptureDlgMgr(QObject* parent/* = nullptr*/)
		: QObject(parent)
	{
#ifdef Q_OS_WIN
		m_CaptureDlgNative = new CaptureDlgNative();
		m_captureDlg = m_CaptureDlgNative->getCaptureDlg();
#else
		m_captureDlg = new CaptureDlg();
#endif
		m_captureDlg->setVisible(false);
	}

	CaptureDlgMgr::~CaptureDlgMgr()
	{
		if (m_captureDlg)
			delete m_captureDlg;
	}

	void CaptureDlgMgr::startAutoPickerWidget()
	{
		if (!m_checkWidgetTimer)
		{
			m_checkWidgetTimer = new QTimer(this);
			m_checkWidgetTimer->setSingleShot(false);
			m_checkWidgetTimer->setInterval(100);

			connect(m_checkWidgetTimer, &QTimer::timeout, this, [=]() {
				if (!(QApplication::mouseButtons() & Qt::LeftButton))
				{
					m_checkWidgetTimer->stop();
					hideCaptureDlg();
					emit sigCatchWidgetFinish(m_targetWidget);
					return;
				}

				QWidget* curWidget = QApplication::widgetAt(QCursor::pos());
				if (m_targetWidget != curWidget)
				{
					m_targetWidget = curWidget;
					emit sigCatchWidgetChanged(m_targetWidget);
					updateCaptureDlgPos();
					return;
				}

				if (m_targetWidget)
					updateCaptureDlgPos();
				else
					hideCaptureDlg();
			});
		}

		m_targetWidget = nullptr;
		m_checkWidgetTimer->stop();
		m_checkWidgetTimer->start();
	}

	void CaptureDlgMgr::updateCaptureDlgPos()
	{
		if (!m_targetWidget)
			return;

		QRect rect = m_targetWidget->frameGeometry();
		rect.moveTo(m_targetWidget->mapToGlobal(QPoint(m_targetWidget->width() - rect.width(), m_targetWidget->height() - rect.height())));
		if (m_captureDlg)
		{
			m_captureDlg->setGeometry(rect);
			m_captureDlg->showNormal();
		}
	}

	void CaptureDlgMgr::hideCaptureDlg()
	{
		if (m_captureDlg)
			m_captureDlg->hide();
	}

	void CaptureDlgMgr::highLightWidget(QWidget* highLightWidget/* = nullptr*/)
	{
		if (!m_highLightTimer)
		{
			m_highLightTimer = new QTimer(this);
			m_highLightTimer->setInterval(100);
			connect(m_highLightTimer, &QTimer::timeout, this, [=]() {
				if (m_highLightCount % 2 == 0)
					updateCaptureDlgPos();
				else
					hideCaptureDlg();

			if (++m_highLightCount > 10)
			{
				m_highLightCount = 0;
				m_highLightTimer->stop();
				hideCaptureDlg();
			}
			});
		}

		if (highLightWidget)
			m_targetWidget = highLightWidget;
		m_highLightCount = 0;
		m_highLightTimer->start();
	}

	//////////////////////////////////////////////////////////////////////////
	// CaptureDlg
	CaptureDlg::CaptureDlg(QWidget* parent/* = nullptr*/)
		: QDialog(parent)
	{
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setAttribute(Qt::WA_TransparentForMouseEvents);
	}

	CaptureDlg::~CaptureDlg()
	{

	}

	void CaptureDlg::paintEvent(QPaintEvent* event)
	{
		QRect rc = rect();
		QPainter painter(this);
		QPen pen;
		pen.setColor("#FF0000");
		pen.setWidth(2);
		painter.setPen(pen);
		painter.setBrush(Qt::transparent);
		painter.drawRect(rc);
	}


	//////////////////////////////////////////////////////////////////////////
	// CaptureDlgNative
	CaptureDlgNative::CaptureDlgNative()
	{
		createCaptureDlg();
	}

	CaptureDlgNative::~CaptureDlgNative()
	{

	}

	QWindow* CaptureDlgNative::getCaptureDlg()
	{
		return m_CaptureWindow;
	}

#ifdef Q_OS_WIN
	bool CaptureDlgNative::createCaptureDlg()
	{
		WNDCLASSEX wcx;

		// Fill in the window class structure with parameters 
		// that describe the main window. 

		wcx.cbSize = sizeof(wcx);          // size of structure 
		wcx.style = CS_HREDRAW |
			CS_VREDRAW;                    // redraw if size changes 
		wcx.lpfnWndProc = wndProc;     // points to window procedure 
		wcx.cbClsExtra = 0;                // no extra class memory 
		wcx.cbWndExtra = 0;                // no extra window memory 
		wcx.hInstance = ::GetModuleHandle(NULL);         // handle to instance 
		wcx.hIcon = LoadIcon(NULL,
			IDI_APPLICATION);              // predefined app. icon 
		wcx.hCursor = LoadCursor(NULL,
			IDC_ARROW);                    // predefined arrow 
		wcx.hbrBackground = nullptr;                  // white background brush 
		wcx.lpszMenuName = TEXT("MainMenu");    // name of menu resource 
		wcx.lpszClassName = TEXT("MainWClass");  // name of window class 
		wcx.hIconSm = NULL;

		// Register the window class. 
		RegisterClassEx(&wcx);

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

		m_CaptureWindow = QWindow::fromWinId((WId)m_hWnd);

		return true;
	}

	LRESULT CaptureDlgNative::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
#endif
}