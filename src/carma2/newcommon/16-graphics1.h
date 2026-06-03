#ifndef GUARD_16_GRAPHICS1_H
#define GUARD_16_GRAPHICS1_H

#include "c2_hooks.h"

// MungeClipPlane

// TryThisEdge

// ProcessShadow

// RenderShadows

// SetShadowLevel

// GetShadowLevel

// ToggleShadow

extern void C2_HOOK_FASTCALL InitShadow(void);

// SaveShadeTable

// SaveShadeTables

// DisposeSavedShadeTables

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

// RenderThisHeadup

// StopRenderingHeadups

// CleanPolyFontDanglers

#endif // GUARD_16_GRAPHICS1_H
