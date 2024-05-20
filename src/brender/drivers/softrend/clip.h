#ifndef SOFTREND_CLIP_H
#define SOFTREND_CLIP_H

#include "types.h"

#include "c2_hooks.h"

int ClipFaceToPlane(brp_vertex* vp, brp_vertex* verts_out, int num_in, br_vector4* plane, int cmask);

int C2_HOOK_STDCALL ClipFaceToPlus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask);

int C2_HOOK_STDCALL ClipFaceToMinus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask);

#endif /* SOFTREND_CLIP_H */
