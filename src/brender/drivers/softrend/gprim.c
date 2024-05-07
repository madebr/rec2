#include "gprim.h"

br_geometry_primitives* (C2_HOOK_STDCALL * GeometryPrimitivesAllocate_original)(br_soft_renderer_facility* type, const char* id);
br_geometry_primitives* C2_HOOK_STDCALL GeometryPrimitivesAllocate(br_soft_renderer_facility* type, const char* id) {

#if defined(C2_HOOKS_ENABLED)
    return GeometryPrimitivesAllocate_original(type, id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00541160, GeometryPrimitivesAllocate, GeometryPrimitivesAllocate_original)
