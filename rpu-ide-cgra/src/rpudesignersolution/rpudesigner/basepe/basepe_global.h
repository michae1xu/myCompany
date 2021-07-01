#ifndef BASEPE_GLOBAL_H
#define BASEPE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BASEPE_LIBRARY)
#  define BASEPE_EXPORT Q_DECL_EXPORT
#else
#  define BASEPE_EXPORT Q_DECL_IMPORT
#endif

#endif // BASEPE_GLOBAL_H
