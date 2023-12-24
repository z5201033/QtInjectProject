#include "QthCommon.h"

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

	QSize QthCommon::getContentWidth(QFont font, QString str)
	{
		QFontMetrics fontMetrics(font);
		QRect rect = fontMetrics.boundingRect(str);
		return rect.size();
	}

	QWidget* QthCommon::convertAddrToWidget(quint64 addr)
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

	QString QthCommon::getCurrentModulePath()
	{
		QString currentPath;
#ifdef Q_OS_WIN
		MEMORY_BASIC_INFORMATION mbi;
		HMODULE hModule = ((::VirtualQuery(getCurrentModulePath, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
		WCHAR path[MAX_PATH + 1] = { 0 };

		::GetModuleFileName(hModule, path, MAX_PATH);
		currentPath = QString::fromStdWString(std::wstring(path));
#endif 

		return currentPath;
	}
}
