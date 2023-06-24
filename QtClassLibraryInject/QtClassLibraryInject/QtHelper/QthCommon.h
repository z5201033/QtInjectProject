#pragma once

#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QSize>

namespace Qth
{
	QSize getContentWidth(QFont font, QString str);
	QWidget* convertAddrToWidget(quint64 addr);
	QString getCurrentModulePath();
}