#ifndef SOFTREND_FACEOPS_H
#define SOFTREND_FACEOPS_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL OpTriangleClip(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

void C2_HOOK_CDECL OpTriangleClipConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp);

void C2_HOOK_CDECL OpTriangleConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, br_uint_16* fp_vertices);

void C2_HOOK_CDECL OpTriangleTwoSidedConstantSurf(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* face, temp_face_soft* tfp);

void C2_HOOK_CDECL OpTriangleMappingWrapFix(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, undefined4* param_5, undefined4* param_6);

void C2_HOOK_CDECL OpTriangleRelightTwoSided(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, temp_face_soft* tfp);

void C2_HOOK_CDECL OpTriangleToLines(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, v11face* fp);

void C2_HOOK_CDECL OpTriangleReplicateConstant(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

void C2_HOOK_CDECL OpTriangleReplicateConstantI(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

void C2_HOOK_CDECL OpTriangleReplicateConstantRGB(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

void C2_HOOK_CDECL OpTriangleToPoints(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices);

void C2_HOOK_CDECL OpTriangleToPoints_OS(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices);

void C2_HOOK_CDECL OpLineClip(brp_block* block, brp_vertex* v0, brp_vertex* v1);

void C2_HOOK_STDCALL SubdivideSetThreshold(br_int_32 subdivide_tolerance);

br_boolean C2_HOOK_CDECL subdivideCheck(brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

void C2_HOOK_STDCALL triangleSubdivideOnScreen(int depth, brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2, br_uint_16* fp_vertices, br_uint_16* fp_edges);

#endif
