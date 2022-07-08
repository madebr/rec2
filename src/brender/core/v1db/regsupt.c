#include "regsupt.h"

br_material* (C2_HOOK_CDECL * BrMaterialFind_original)(char* pattern);
br_material* C2_HOOK_CDECL BrMaterialFind(char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    return BrMaterialFind_original(pattern);
#else
    return BrRegistryFind(&v1db.reg_materials, pattern);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051eea0, BrMaterialFind, BrMaterialFind_original)
