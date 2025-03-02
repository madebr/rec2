#include "regsupt.h"

br_material* (C2_HOOK_CDECL * BrMaterialFind_original)(const char* pattern);
br_material* C2_HOOK_CDECL BrMaterialFind(const char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    return BrMaterialFind_original(pattern);
#else
    return BrRegistryFind(&C2V(v1db).reg_materials, pattern);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051eea0, BrMaterialFind, BrMaterialFind_original)

br_pixelmap* (C2_HOOK_CDECL * BrMapFind_original)(char* pattern);
br_pixelmap* C2_HOOK_CDECL BrMapFind(char* pattern) {
#if defined(C2_HOOKS_ENABLED)
    return BrMapFind_original(pattern);
#else
    return BrRegistryFind(&C2V(v1db).reg_textures, pattern);
#endif
}
C2_HOOK_FUNCTION(0x0051eff0, BrMapFind)
