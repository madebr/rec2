#include "convert.h"

void (C2_HOOK_STDCALL * ConvertVertex_original)(brp_vertex* dest, brp_vertex* src);
void C2_HOOK_STDCALL ConvertVertex(brp_vertex* dest, brp_vertex* src) {

#if defined(C2_HOOKS_ENABLED)
    ConvertVertex_original(dest, src);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005460e0, ConvertVertex, ConvertVertex_original)
