#ifndef SOFTREND_FACEOPS_H
#define SOFTREND_FACEOPS_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL OpTriangleClip(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

#endif
