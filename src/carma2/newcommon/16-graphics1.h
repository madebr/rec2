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
extern int gHud_actor_storage_size;
extern br_actor* gHud_actor_storage[128];
extern int gSaved_table_count;
extern tSaved_table gSaved_shade_tables[100];
extern int gNumber_of_lollipops;

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

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

// AdjustRenderScreenSize

// ScreenSmaller

// ScreenLarger

// ClearWobbles

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

// RenderAFrame

extern void C2_HOOK_FASTCALL StartRenderingHeadups(void);

extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);

extern void C2_HOOK_FASTCALL StopRenderingHeadups(void);

extern void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

#endif // GUARD_16_GRAPHICS1_H
