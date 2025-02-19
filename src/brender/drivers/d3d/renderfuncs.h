#ifndef D3D_RENDERFUNCS_H
#define D3D_RENDERFUNCS_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL PointRender_d3d_RGB_555(brp_block* block, brp_vertex* vertex);

void C2_HOOK_CDECL LineRender_d3d(brp_block* block, brp_vertex* vertex);

void C2_HOOK_CDECL FUN_10007030(brp_block_d3d* block);

void C2_HOOK_CDECL FUN_10007080(brp_block_d3d* block);

void C2_HOOK_CDECL FUN_100070e0(brp_block_d3d* block);

void C2_HOOK_CDECL FUN_10007690(brp_block* block, brp_vertex* vertex);

void C2_HOOK_CDECL FUN_10007850(brp_block* block, brp_vertex* vertex);

void C2_HOOK_CDECL FUN_10007a00(brp_block* block, brp_vertex* v0, brp_vertex* v1, brp_vertex* v2);

void C2_HOOK_CDECL FUN_10007ba0(brp_block* block, brp_vertex* vertex);

void C2_HOOK_CDECL FUN_10007da0(brp_block* block, brp_vertex* vertex);

#endif /* D3D_RENDERFUNCS_H */
