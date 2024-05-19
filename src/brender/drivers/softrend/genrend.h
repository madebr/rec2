#ifndef SOFTREND_GENREND_H
#define SOFTREND_GENREND_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL Vertex_TransformProjectOutcodeBounds(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_TransformProjectOutcode(br_geometry *self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProjectBounds(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProject(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsSurf(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProjectSurf(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProjectBoundsGeom(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_OS_TransformProjectGeom(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_SurfaceComponentsSurf(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_SurfaceComponentsTwoSidedSurf(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL ScratchFree(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL Vertex_ClearFlags(br_geometry* self, br_soft_renderer* renderer);

#endif /* SOFTREND_GENREND_H */
