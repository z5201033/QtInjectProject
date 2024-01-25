#include "QthMainWindow.h"
#include "ObjectHelper/ObjectHelper.h"
#include "QtHelperNetWorkExport.h"
#include "Common.h"
#include "WidgetHelper/WidgetHelper.h"

#include <QDir>
#include <QLabel>
#include <QLibrary>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTextEdit>

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
		m_tabWidget->addTab(addAboutWidget(), "About");

		mainLayout->addWidget(m_tabWidget);
	}

	QWidget* QthMainWindow::addWidgetHelper()
	{
		return new WidgetHelper();
	}


	QWidget* QthMainWindow::addObjectHelper()
	{
		return new ObjectHelper();
	}

	QWidget* QthMainWindow::addNetworkHelper()
	{
		QWidget* networkWidget = new QWidget(this);
		QVBoxLayout* networklayout = new QVBoxLayout(networkWidget);
		networklayout->setContentsMargins(0, 0, 0, 0);
		networklayout->setSpacing(0);

		QString networkPath = "QtHelperNetWork";
		QString currentPath = Common::getCurrentModulePath();
		if (!currentPath.isEmpty())
		{
			QFileInfo info(currentPath);
			networkPath = QString("%0/%1").arg(info.absoluteDir().absolutePath()).arg("QtHelperNetWork");
		}
		
		bool loadSuccessed = false;
		generateQtHelperNetWorkWidgetFunc generateFunc = (generateQtHelperNetWorkWidgetFunc)QLibrary::resolve(networkPath, "generateQtHelperNetWorkWidget");
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

	QWidget* QthMainWindow::addAboutWidget()
	{
		QWidget* aboutWidget = new QWidget(this);
		QVBoxLayout* aboutLayout = new QVBoxLayout(aboutWidget);
		aboutLayout->setContentsMargins(0, 0, 0, 0);
		aboutLayout->setSpacing(0);

		QTextEdit* textEdit = new QTextEdit(aboutWidget);
		aboutLayout->addWidget(textEdit);
		textEdit->setText("Shortcut Keys:");
		textEdit->append("Ctrl + Shift + T: Display and Top the QtHelper Widget.It can be placed on top of the modal window.");

		return aboutWidget;
	}
}