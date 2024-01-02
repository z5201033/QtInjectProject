#pragma once

#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QSize>

namespace Qth
{
	class Common
	{
	public:
		static QSize getContentWidth(QFont font, QString str);
		static QWidget* convertAddrToWidget(quint64 addr);
		static QObject* convertAddrToObject(quint64 addr);
		static QString getCurrentModulePath();
	};
}