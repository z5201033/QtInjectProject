#include "QtWidgetsAppDemo.h"
#include <windows.h>

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
}

QtWidgetsAppDemo::~QtWidgetsAppDemo()
{}
