#ifndef SOFTREND_GENREND_H
#define SOFTREND_GENREND_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL Vertex_TransformProjectOutcodeBounds(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL ScratchFree(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_ClearFlags(br_geometry* self, br_soft_renderer* renderer);

#endif /* SOFTREND_GENREND_H */
