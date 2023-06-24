#pragma once

#include <QDialog>

class QTabWidget;
namespace Qth
{
	class QthMainWindow : public QDialog
	{
		Q_OBJECT
	public:
		explicit QthMainWindow(QWidget* parent = nullptr);
		~QthMainWindow();

	private:
		void initUI();

	private:
		QTabWidget* m_tabWidget = nullptr;
		QWidget* addWidgetHelper();
		QWidget* addObjectHelper();
		QWidget* addNetworkHelper();
	};
}