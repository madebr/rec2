#ifndef SOFTREND_GV1BUCKT_H
#define SOFTREND_GV1BUCKT_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id);

void C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_free(br_geometry_v1_buckets_soft* self);

br_token C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_type(br_geometry_v1_buckets_soft* self);

br_boolean C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_isType(br_geometry_v1_buckets_soft* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_space(br_geometry_v1_buckets_soft* self);

br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_templateQuery(br_geometry_v1_buckets_soft* self);

br_error C2_HOOK_CDECL _M_br_geometry_v1_buckets_soft_render(br_renderer_state_stored* self, br_soft_renderer* renderer, br_primitive** buckets, br_int_32 nbuckets);

#endif /* SOFTREND_GV1BUCKT_H */
