#include "lighting.h"

br_geometry_lighting* (C2_HOOK_STDCALL * GeometryLightingAllocate_original)(br_soft_renderer_facility* type, const char* id);
br_geometry_lighting* C2_HOOK_STDCALL GeometryLightingAllocate(br_soft_renderer_facility* type, const char* id) {

#if defined(C2_HOOKS_ENABLED)
    return GeometryLightingAllocate_original(type, id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540e30, GeometryLightingAllocate, GeometryLightingAllocate_original)
