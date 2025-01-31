#ifndef REC2_HOOK_H
#define REC2_HOOK_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_error C2_HOOK_CDECL HostExceptionGet(br_uint_8 exception, br_uint_32 *offp, br_uint_16 *selp);

#endif // REC2_HOOK_H
