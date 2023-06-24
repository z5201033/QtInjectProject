#include "QtHelperNetWorkExport.h"
#include "QtHelperNetWork.h"

extern "C" QWidget* generateQtHelperNetWorkWidget(QWidget* parent)
{
	return new QtHelperNetWork(parent);
}