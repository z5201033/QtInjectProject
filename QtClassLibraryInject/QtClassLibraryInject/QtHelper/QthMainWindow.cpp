#include "QthMainWindow.h"
#include "QthWidgetHelper.h"
#include "QtHelperNetWorkExport.h"

#include <QLabel>
#include <QLibrary>
#include <QTabWidget>
#include <QVBoxLayout>

namespace Qth
{
	QthMainWindow::QthMainWindow(QWidget* parent/* = nullptr*/)
		: QDialog(parent)
	{
		setWindowTitle("QtHelper");
		setWindowFlag(Qt::WindowMinMaxButtonsHint);

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
		m_tabWidget->addTab(addObjectHelper(), "Object");
		m_tabWidget->addTab(addNetworkHelper(), "Network");

		mainLayout->addWidget(m_tabWidget);
	}

	QWidget* QthMainWindow::addWidgetHelper()
	{
		return new WidgetHelper();
	}


	QWidget* QthMainWindow::addObjectHelper()
	{
		QWidget* objectWidget = new QWidget(this);
		QVBoxLayout* objectlayout = new QVBoxLayout(objectWidget);
		objectlayout->setContentsMargins(0, 0, 0, 0);
		objectlayout->setSpacing(0);

		QLabel* labelTip = new QLabel();
		labelTip->setAlignment(Qt::AlignCenter);
		labelTip->setText("The module is currently not supported!");
		objectlayout->addWidget(labelTip);

		return objectWidget;
	}

	QWidget* QthMainWindow::addNetworkHelper()
	{
		QWidget* networkWidget = new QWidget(this);
		QVBoxLayout* networklayout = new QVBoxLayout(networkWidget);
		networklayout->setContentsMargins(0, 0, 0, 0);
		networklayout->setSpacing(0);

		bool loadSuccessed = false;
		generateQtHelperNetWorkWidgetFunc generateFunc = (generateQtHelperNetWorkWidgetFunc)QLibrary::resolve("QtHelperNetWork", "generateQtHelperNetWorkWidget");
		if (generateFunc)
		{
			QWidget* widget = generateFunc(networkWidget);
			if (widget)
			{
				networklayout->addWidget(widget);
				loadSuccessed = true;
			}
		}

		if (!loadSuccessed)
		{
			QLabel* labelTip = new QLabel();
			labelTip->setAlignment(Qt::AlignCenter);
			labelTip->setText("Can not found the NetWork module!");
			networklayout->addWidget(labelTip);
		}

		return networkWidget;
	}
}