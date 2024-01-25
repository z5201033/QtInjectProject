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
		enum CaptureMode
		{
			Widget,
			Window,
			All
		};
	public:
		CaptureDlgMgr(QObject* parent = nullptr);
		~CaptureDlgMgr();

		void startAutoPickerWidget();
		void highLightWidget(QObject* highLightWidget = nullptr);
		void seCaptureMode(CaptureMode mode);

	signals:
		void sigCatchWidgetChanged(QObject* target);
		void sigCatchWidgetFinish(QObject* target);

	private:
		void updateCaptureDlgPos();
		void hideCaptureDlg();
		void initWindowCache();
		QWindow* getCaptureWindow();
		QWindow* getDstWindow(QWindow* parent, const QPoint& globalPos);
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
		QPointer<QObject>		m_target = nullptr;
		CaptureMode				m_captureMode = CaptureMode::Widget;
		QHash<WId, QVector<QPointer<QWindow>>> m_windowZOrderCache;
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