#ifndef GUARD_16_GRAPHICS1_H
#define GUARD_16_GRAPHICS1_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gRender_indent;
extern int gWidth;
extern int gHeight;
extern tU8 gTemporary_physics_render_buffer[300000];

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

extern void C2_HOOK_FASTCALL SetShadowLevel(tShadow_level pLevel);

extern tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);

// ToggleShadow

extern void C2_HOOK_FASTCALL InitShadow(void);

// SaveShadeTable

// SaveShadeTables

extern void C2_HOOK_FASTCALL DisposeSavedShadeTables(void);

// ShadowMode

// LollipopizeActor

// ResetLollipopQueue

// RenderLollipops

// SetupDepthBuffer

extern void C2_HOOK_FASTCALL SetBRenderScreenAndBuffers(int pX_offset, int pY_offset, int pWidth, int pHeight);

// AdjustRenderScreenSize

// ScreenSmaller

// ScreenLarger

// ClearWobbles

extern void C2_HOOK_FASTCALL InitWobbleStuff(void);

// NewScreenWobble

// SetScreenWobble

// ResetScreenWobble

// CalculateWobblitude

// ProcessNonTrackActors

// ConditionallyFillWithSky

// FoxyStuff

// DoARenderPass

// DoACompleteRenderPass

// RenderAFrame

// StartRenderingHeadups

extern void C2_HOOK_FASTCALL RenderThisHeadup(br_actor* pActor);

// StopRenderingHeadups

extern void C2_HOOK_FASTCALL CleanPolyFontDanglers(void);

#endif // GUARD_16_GRAPHICS1_H
