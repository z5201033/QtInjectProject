﻿#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <process.h>
#include <QApplication>

#include "QtHelper/QthMainWindow.h"
#include "WidgetHelperSimple/WidgetHelperSimple.h"
#include "Utils/SystemCommonUtils.h"

static DWORD g_dwMainTId = -1;
static HHOOK g_hookMsg = NULL;
static HHOOK g_hookKeyboard = NULL;
static bool g_initWidgetHelper = false;
static QWidget* g_WidgetHelper = nullptr;
static const DWORD g_customMsg = WM_USER + 100;


LRESULT CALLBACK MainThreadMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (::GetCurrentThreadId() != g_dwMainTId)
		return CallNextHookEx(g_hookMsg, nCode, wParam, lParam);

	if (!g_initWidgetHelper)
	{
		g_initWidgetHelper = true;
// 		g_WidgetHelper = new WidgetHelper();
// 		g_WidgetHelper->show();
// 		g_WidgetHelper->raise();
		g_WidgetHelper = new Qth::QthMainWindow();
		g_WidgetHelper->show();
		g_WidgetHelper->raise();

		// 启动了窗口后即可卸载消息钩子
		LRESULT result = CallNextHookEx(g_hookMsg, nCode, wParam, lParam);
		UnhookWindowsHookEx(g_hookMsg);
		g_hookMsg = NULL;

		return result;
	}

	return CallNextHookEx(g_hookMsg, nCode, wParam, lParam);
}

void AutoWidgetHelperSetParent()
{
	if (!g_WidgetHelper)
		return;
	auto pwidget = QApplication::activeModalWidget();
	if (!pwidget)
	{
		for (QWidget* w : QApplication::topLevelWidgets())
		{
			if (w->isModal())
			{
				pwidget = w;
				break;
			}
		}
	}

	do
	{
		if (g_WidgetHelper->parentWidget() == pwidget)
			break;

		Qt::WindowFlags flags = g_WidgetHelper->windowFlags();
		g_WidgetHelper->setParent(pwidget);
		g_WidgetHelper->setWindowFlags(flags);
		g_WidgetHelper->connect(pwidget, &QObject::destroyed, g_WidgetHelper, [&](QObject* parent) {
			if (g_WidgetHelper->parent() == parent)
			{
				bool visible = g_WidgetHelper->isVisible();
				QRect rectOld = g_WidgetHelper->geometry();
				g_WidgetHelper->setParent(nullptr);
				g_WidgetHelper->setGeometry(rectOld);
				g_WidgetHelper->setVisible(visible);

				QTimer::singleShot(100, g_WidgetHelper, [=]() {
					AutoWidgetHelperSetParent();
					g_WidgetHelper->show();
				});
			}
		});
	} while (false);
}

void RaiseWidgetHelper()
{
	if (!g_WidgetHelper)
		return;
	AutoWidgetHelperSetParent();
	g_WidgetHelper->show();
	g_WidgetHelper->raise();
}

LRESULT CALLBACK MainThreadKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (::GetCurrentThreadId() != g_dwMainTId)
		return CallNextHookEx(g_hookKeyboard, nCode, wParam, lParam);

	if (!g_initWidgetHelper)
	{
		g_initWidgetHelper = true;
		g_WidgetHelper = new WidgetHelper();
		g_WidgetHelper->show();
		g_WidgetHelper->raise();
	}

	TCHAR ch[20] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szNewFile[MAX_PATH] = { 0 };
	if (((DWORD)lParam & 0x40000000) && (nCode == HC_ACTION)) {
		if (wParam == 'T' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) // Ctrl + Shift + S
		{
			RaiseWidgetHelper();
		}
	}

	return CallNextHookEx(g_hookKeyboard, nCode, wParam, lParam);
}

unsigned __stdcall InjectThreadFunc(void* pArguments)
{
	if (SystemCommonUtils::getCurrentMainThreadId(g_dwMainTId))
	{
		g_hookMsg = ::SetWindowsHookExW(WH_GETMESSAGE, MainThreadMsgProc, NULL, g_dwMainTId);
		g_hookKeyboard = ::SetWindowsHookExW(WH_KEYBOARD, MainThreadKeyboardProc, NULL, g_dwMainTId);

// 		WCHAR msgText[260] = { 0 };
// 		wsprintfW(msgText, L"inject success，main thread id(%lu) hook(%lu)(%lu) error(%lu)",
// 			g_dwMainTId, g_hookMsg, g_hookKeyboard, ::GetLastError());
	//	::MessageBoxW(NULL, msgText, L"inject box", MB_OK);
	}

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	_endthreadex(0);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		unsigned int threadId = 0;
		HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, InjectThreadFunc, nullptr, 0, &threadId);
		if (handle)
			::CloseHandle(handle);
	}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
	{
		if (g_hookMsg)
			UnhookWindowsHookEx(g_hookMsg);
		if (g_hookKeyboard)
			UnhookWindowsHookEx(g_hookKeyboard);
	}
	break;
	}
	return TRUE;
}
