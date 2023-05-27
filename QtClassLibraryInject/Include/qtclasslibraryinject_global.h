#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QTCLASSLIBRARYINJECT_LIB)
#  define QTCLASSLIBRARYINJECT_EXPORT Q_DECL_EXPORT
# else
#  define QTCLASSLIBRARYINJECT_EXPORT Q_DECL_IMPORT
# endif
#else
# define QTCLASSLIBRARYINJECT_EXPORT
#endif
