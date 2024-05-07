#include "gv1buckt.h"

br_geometry_v1_buckets* (C2_HOOK_STDCALL * GeometryV1BucketsAllocate_original)(br_soft_renderer_facility *type, const char *id);
br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id) {

#if defined(C2_HOOKS_ENABLED)
    return GeometryV1BucketsAllocate_original(type, id);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005412b0, GeometryV1BucketsAllocate, GeometryV1BucketsAllocate_original)
