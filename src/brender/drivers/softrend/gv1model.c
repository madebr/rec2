#include "gv1model.h"

br_geometry_v1_model* (C2_HOOK_STDCALL * GeometryV1ModelAllocate_original)(br_soft_renderer_facility *type, const char *id);
br_geometry_v1_model* C2_HOOK_STDCALL GeometryV1ModelAllocate(br_soft_renderer_facility *type, const char *id) {

#if defined(C2_HOOKS_ENABLED)
    return GeometryV1ModelAllocate_original(type, id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540d00, GeometryV1ModelAllocate, GeometryV1ModelAllocate_original)
