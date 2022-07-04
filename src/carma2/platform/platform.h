#ifndef C2_PLATFORM_H
#define C2_PLATFORM_H

#if defined(_WIN32)
#include "win32/win32.h"
#else
#error "Unknown platform"
#endif

#endif // C2_PLATFORM_H
