#ifndef SOFTREND_CONVERT_H
#define SOFTREND_CONVERT_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src);

void C2_HOOK_CDECL RenderConvert1(brp_block* block, brp_vertex* v0);

#endif /* SOFTREND_CONVERT_H */
