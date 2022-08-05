#ifndef REC2_MALLOC_H
#define REC2_MALLOC_H

#if defined(_WIN32)
#include <malloc.h>

#define c2_alloca _malloca

#else
#include <alloca.h>

#define c2_alloca alloca

#endif

#endif // REC2_MALLOC_H
