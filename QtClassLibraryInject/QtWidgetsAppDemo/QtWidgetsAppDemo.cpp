#include "QtWidgetsAppDemo.h"
#include <windows.h>
#include <QWindow>
#include <QDialog>
#include <QHBoxLayout>

#include "QtClassLibraryInject.h"

QtWidgetsAppDemo::QtWidgetsAppDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.pushButton->setFixedWidth(80);
    ui.pushButton->setProperty("test", "testProperty");
    connect(ui.pushButton, &QPushButton::clicked, this, [=](bool checked) {
        ::LoadLibraryW(L"QtClassLibraryInject.dll");
     });

    QDialog* dlg = new QDialog(parent);
    QWindow* w = new QWindow();
    w->setObjectName("testWindow--");
    QWidget* widget = QWidget::createWindowContainer(w);
    QHBoxLayout* layout = new QHBoxLayout(dlg);
    layout->addWidget(widget);

    dlg->resize(800, 600);
    dlg->show();

}

QtWidgetsAppDemo::~QtWidgetsAppDemo()
{}
