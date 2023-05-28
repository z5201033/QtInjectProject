#pragma once

#include <QDialog>
#include <QPointer>
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace Qth
{
	class CaptureDlg;
	class CaptureDlgNative;

	//////////////////////////////////////////////////////////////////////////
	// CaptureDlgMgr
	class CaptureDlgMgr : public QObject 
	{
		Q_OBJECT
	public:
		CaptureDlgMgr(QObject* parent = nullptr);
		~CaptureDlgMgr();

		void startAutoPickerWidget();
		void highLightWidget(QWidget* highLightWidget = nullptr);

	signals:
		void sigCatchWidgetChanged(QWidget* targetWidget);
		void sigCatchWidgetFinish(QWidget* targetWidget);

	private:
		void updateCaptureDlgPos();
		void hideCaptureDlg();
	private:
#ifdef Q_OS_WIN
		CaptureDlgNative*		m_CaptureDlgNative;
		QPointer<QWindow>		m_captureDlg = nullptr;
#else
		QPointer<QWidget>		m_captureDlg = nullptr;
#endif
		QTimer*					m_checkWidgetTimer = nullptr;
		QTimer*					m_highLightTimer = nullptr;
		int						m_highLightCount = 0;
		QPointer<QWidget>		m_targetWidget = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////
	// CaptureDlg
	class CaptureDlg : public QDialog
	{
		Q_OBJECT
	public:
		explicit CaptureDlg(QWidget* parent = nullptr);
		~CaptureDlg();

	protected:
		void paintEvent(QPaintEvent* event) override;
	};

	//////////////////////////////////////////////////////////////////////////
	// CaptureDlgNative
	class CaptureDlgNative
	{
	public:
		CaptureDlgNative();
		~CaptureDlgNative();

		QWindow* getCaptureDlg();

	private:
#ifdef Q_OS_WIN
		bool createCaptureDlg();
		static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		HWND  m_hWnd = nullptr;
#endif
		QPointer<QWindow> m_CaptureWindow;
	};
}