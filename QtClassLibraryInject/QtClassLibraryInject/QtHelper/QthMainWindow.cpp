#include "QthMainWindow.h"

#include <QTabWidget>
#include <QVBoxLayout>

namespace Qth
{
	QthMainWindow::QthMainWindow(QWidget* parent/* = nullptr*/)
		: QDialog(parent)
	{
		setWindowTitle("QtHelper");
		resize(1080, 800);
		initUI();
	}

	QthMainWindow::~QthMainWindow()
	{

	}

	void QthMainWindow::initUI()
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		m_tabWidget = new QTabWidget(this);
		m_tabWidget->setContentsMargins(0, 0, 0, 0);
		// 	m_tabWidget->setStyleSheet("QTabWidget#tabWidget{background:transparent;}\
		// 								QTabWidget::pane{border:0px;}\
		//                                  QTabBar::tab{background-color:#1F1F1F;color:#999999;}\
		//                                  QTabBar::tab::selected{background-color:#0F0F0F;color:#EEEEEE;}");
		m_tabWidget->addTab(addWidgetHelper(), "Widget");

		mainLayout->addWidget(m_tabWidget);
	}

	QWidget* QthMainWindow::addWidgetHelper()
	{
		return new QWidget();
	}
}