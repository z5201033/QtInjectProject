#include "QtWidgetsAppDemo.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtWidgetsAppDemo w;
    w.show();
    return a.exec();
}
