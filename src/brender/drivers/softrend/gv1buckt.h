#ifndef SOFTREND_GV1BUCKT_H
#define SOFTREND_GV1BUCKT_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_v1_buckets* C2_HOOK_STDCALL GeometryV1BucketsAllocate(br_soft_renderer_facility *type, const char *id);

#endif /* SOFTREND_GV1BUCKT_H */
