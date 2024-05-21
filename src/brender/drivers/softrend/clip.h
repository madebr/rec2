#ifndef SOFTREND_CLIP_H
#define SOFTREND_CLIP_H

#include "types.h"

#include "c2_hooks.h"

int ClipFaceToPlane(brp_vertex* vp, brp_vertex* verts_out, int num_in, br_vector4* plane, int cmask);

int C2_HOOK_STDCALL ClipFaceToPlus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask);

int C2_HOOK_STDCALL ClipFaceToMinus1(brp_vertex* vp, brp_vertex* verts_out, int num_in, int axis, int cmask);

brp_vertex* C2_HOOK_STDCALL FaceClip(br_soft_renderer* self, brp_vertex* clip_in, br_uint_32 mask, br_uint_32 codes, int n, int* n_out);

void C2_HOOK_STDCALL ClippedRenderTriangles(br_soft_renderer* renderer, brp_block* block, brp_vertex* cp_in, int n, br_uint_16* fp_vertices, br_uint_16* fp_edges);

br_boolean C2_HOOK_STDCALL ClipLineToPlane(brp_vertex* in, brp_vertex* out, br_vector4* plane, int cmask);

br_boolean C2_HOOK_STDCALL ClipLine(br_soft_renderer* self, brp_vertex* out, brp_vertex* v0, brp_vertex* v1, br_uint_32 mask, br_uint_32 codes);

void C2_HOOK_STDCALL ClippedRenderLine(br_soft_renderer* renderer, brp_block* block, brp_vertex* cp_in);

#endif /* SOFTREND_CLIP_H */
