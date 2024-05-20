#ifndef SOFTREND_CONVERT_H
#define SOFTREND_CONVERT_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src);

#endif /* SOFTREND_CONVERT_H */
