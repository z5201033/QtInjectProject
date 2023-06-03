#include "QtWidgetsAppDemo.h"
#include <windows.h>

#include "QtClassLibraryInject.h"

QtWidgetsAppDemo::QtWidgetsAppDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.pushButton->setFixedWidth(80);
    connect(ui.pushButton, &QPushButton::clicked, this, [=]() {
        ::LoadLibraryW(L"QtClassLibraryInject.dll");
     });
}

QtWidgetsAppDemo::~QtWidgetsAppDemo()
{}
