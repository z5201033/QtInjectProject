#pragma once

#include <QPointer>
#include <QDialog>

namespace Qth
{
	class WatchGlobalFocusDlg : public QDialog
	{
		Q_OBJECT
	public:
		WatchGlobalFocusDlg(QWidget* parent);
		~WatchGlobalFocusDlg();

	private slots:
		void onPrint();
		void onClear();
		void onFocusChanged(QWidget* old, QWidget* now);
	private:
		void initData();
		void initUI();
		void initConnect();
		void startWatch();
		void stopWatch();
	private:
		struct WatchGlobalFocusDlgPrivate* d;
	};
}