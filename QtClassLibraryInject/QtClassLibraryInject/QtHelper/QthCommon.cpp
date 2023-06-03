﻿#include "QthCommon.h"

#include <QWidget>

#ifdef Q_OS_WIN
	#include <windows.h>
#endif

namespace Qth
{
#ifdef Q_OS_WIN
	static int HelperExptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
	{
		Q_UNUSED(code);
		Q_UNUSED(ep);
		return EXCEPTION_EXECUTE_HANDLER;
	}
#endif

	QSize getContentWidth(QFont font, QString str)
	{
		QFontMetrics fontMetrics(font);
		QRect rect = fontMetrics.boundingRect(str);
		return rect.size();
	}

	QWidget* convertAddrToWidget(quint64 addr)
	{
#ifdef Q_OS_WIN
		__try
		{
			QWidget* curWidget = qobject_cast<QWidget*>((QObject*)addr);
			if (!curWidget)
			{
				return nullptr;
			}

			return curWidget;
		}
		__except (HelperExptionFilter(GetExceptionCode(), GetExceptionInformation()))
		{
			return nullptr;
		}
#else
		return nullptr;
#endif
	}
}