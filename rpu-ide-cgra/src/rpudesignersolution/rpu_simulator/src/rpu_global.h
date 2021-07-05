#ifndef RPU_GLOBAL_H
#define RPU_GLOBAL_H

#if defined(RPUSIM_LIBRARY)
#include <QtCore/qglobal.h>
#  define RPUSIMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RPUSIMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // RPUSIM_GLOBAL_H
