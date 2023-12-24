#include "QtHelperNetWork.h"

#include <QLabel>
#include <QNetworkProxy>
#include <QVBoxLayout>

QtHelperNetWork::QtHelperNetWork(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(0, 0, 0, 0);
	mainlayout->setSpacing(0);

	QLabel* testLabel = new QLabel(this);
	testLabel->setAlignment(Qt::AlignCenter);
	testLabel->setText("The network module is currently not supported!");

		// 合法格式：127.0.0.1:8888
// 	QNetworkProxy proxy;
// 	proxy.setType(QNetworkProxy::HttpProxy);
// 	proxy.setHostName("127.0.0.1");
// 	proxy.setPort(8888);
// 	QNetworkProxy::setApplicationProxy(proxy);

	mainlayout->addWidget(testLabel);
}
