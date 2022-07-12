#include "prepmatl.h"

void (C2_HOOK_CDECL * BrMaterialUpdate_original)(br_material* mat, br_uint_16 flags);
void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags) {
#if defined(C2_HOOKS_ENABLED)
    BrMaterialUpdate_original(mat, flags);
#else
    br_token_value tva[32];
    br_token_value* tvp;
    br_token t;
    br_int_32 c;

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00520e70, BrMaterialUpdate, BrMaterialUpdate_original)

void (C2_HOOK_STDCALL * BrMaterialClear_original)(br_material* mat);
void C2_HOOK_STDCALL BrMaterialClear(br_material* mat) {
#if defined(C2_HOOKS_ENABLED)
    BrMaterialClear_original(mat);
#else
    br_token_value tva[32];
    br_token_value* tvp;
    br_token t;
    br_int_32 c;

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00521460, BrMaterialClear, BrMaterialClear_original)
