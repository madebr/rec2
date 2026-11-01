/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_16_GRAPHICS1_H
#define GUARD_16_GRAPHICS1_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gRender_indent;
extern int gWidth;
extern int gHeight;
extern tU8 gTemporary_physics_render_buffer[300000];

extern tClip_details gShadow_clip_planes[8];
extern int gFancy_shadow;
extern br_material* gShadow_material;
extern br_vector3 gShadow_light_ray;
extern br_vector3 gShadow_light_z;
extern br_vector3 gShadow_light_x;
extern br_model* gShadow_model;
extern br_actor* gShadow_actor;
extern int gScreen_wobble_x;
extern int gScreen_wobble_y;
extern int gFrontend_wrecks_car_count;
extern br_material* gMaterial_below;
extern int gHud_actor_storage_size;
extern br_actor* gHud_actor_storage[128];
extern int gSaved_table_count;
extern tSaved_table gSaved_shade_tables[100];
extern int gNumber_of_lollipops;
extern int gHud_tinted4;

// MungeClipPlane

// TryThisEdge

extern void C2_HOOK_FASTCALL ProcessShadow(tCar_spec* pCar, br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform, br_scalar pDistance_factor);

extern void C2_HOOK_FASTCALL RenderShadows(br_actor* pWorld, tTrack_spec* pTrack_spec, br_actor* pCamera, br_matrix34* pCamera_to_world_transform);

extern void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel);

extern tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);

extern void C2_HOOK_FASTCALL ToggleShadow(void);

extern void C2_HOOK_FASTCALL InitShadow(void);

extern br_uint_32 C2_HOOK_CDECL SaveShadeTable(br_pixelmap* pTable, void* pArg);

extern void C2_HOOK_FASTCALL SaveShadeTables(void);

extern void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

// ShadowMode

// LollipopizeActor

extern void C2_HOOK_FASTCALL ResetLollipopQueue(void);

// RenderLollipops

extern void C2_HOOK_FASTCALL SetupDepthBuffer(void);

extern void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

extern void C2_HOOK_FASTCALL AdjustRenderScreenSize(void);

extern void C2_HOOK_FASTCALL ScreenSmaller(void);

extern void C2_HOOK_FASTCALL ScreenLarger(void);

extern void C2_HOOK_FASTCALL ClearWobbles(void);

extern void C2_HOOK_FASTCALL InitWobbleStuff(void);

// NewScreenWobble

extern void C2_HOOK_FASTCALL SetScreenWobble(int pX, int pY);

extern void C2_HOOK_FASTCALL ResetScreenWobble(void);

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

extern void C2_HOOK_FASTCALL RenderAFrame(int pDepth_mask_on);

extern void C2_HOOK_FASTCALL StartRenderingHeadups(void);

extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);

extern void C2_HOOK_FASTCALL StopRenderingHeadups(void);

extern void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

#endif // GUARD_16_GRAPHICS1_H
