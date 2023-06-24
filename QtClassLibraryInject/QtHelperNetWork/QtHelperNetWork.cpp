#include "QtHelperNetWork.h"

#include <QLabel>
#include <QVBoxLayout>

QtHelperNetWork::QtHelperNetWork(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(0, 0, 0, 0);
	mainlayout->setSpacing(0);

	QLabel* testLabel = new QLabel(this);
	testLabel->setText("QtHelperNetWork");

	mainlayout->addWidget(testLabel);
}
