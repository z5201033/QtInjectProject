#pragma once

#include <QPointer>
#include <QDialog>

namespace Qth
{
	class WatchGlobalFocusDlg : public QDialog
	{
		Q_OBJECT
	public:
		enum class WatchType {
			QWidgetFocus,
			QObjectFocus,
			QWindowFocus
		};
	public:
		WatchGlobalFocusDlg(QWidget* parent, WatchType watchType = WatchType::QWidgetFocus);
		~WatchGlobalFocusDlg();

	private slots:
		void onPrint();
		void onClear();
		void onFocusChanged(QWidget* old, QWidget* now);
		void onFocusObjectChanged(QObject* object);
		void onFocusWindowChanged(QWindow* window);
	private:
		void initData();
		void initUI();
		void initConnect();
		void startWatch();
		void stopWatch();
		bool outputHead(QObject* focusObject, bool detail = true);
	private:
		struct WatchGlobalFocusDlgPrivate* d;
		WatchType _watchType = WatchType::QWidgetFocus;
	};
}