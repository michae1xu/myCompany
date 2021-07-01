#include "debug.h"

namespace {
CNoDebug g_noDebug;
}

CNoDebug &noDebug(){
    return g_noDebug;
}
