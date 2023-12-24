#pragma once

#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QSize>

namespace Qth
{
	class QthCommon
	{
	public:
		static QSize getContentWidth(QFont font, QString str);
		static QWidget* convertAddrToWidget(quint64 addr);
		static QString getCurrentModulePath();
	};
}