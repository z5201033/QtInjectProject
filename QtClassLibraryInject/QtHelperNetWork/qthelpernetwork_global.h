#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QTHELPERNETWORK_LIB)
#  define QTHELPERNETWORK_EXPORT Q_DECL_EXPORT
# else
#  define QTHELPERNETWORK_EXPORT Q_DECL_IMPORT
# endif
#else
# define QTHELPERNETWORK_EXPORT
#endif
