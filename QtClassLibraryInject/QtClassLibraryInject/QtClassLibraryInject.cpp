#include "QtClassLibraryInject.h"
#include "WidgetHelper/widgethelper.h"

QtClassLibraryInject::QtClassLibraryInject()
{

}

void QtClassLibraryInject::CtreateWidgetHelper()
{
	static WidgetHelper* widget = new WidgetHelper();
	widget->show();
}