#ifndef GUARD_33_DEPTH_H
#define GUARD_33_DEPTH_H

#include "c2_hooks.h"
#include "brender/brender.h"

// Log2

// CalculateWrappingMultiplier

extern br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance);

extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

// FogCars

// FrobFog

extern void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

// InstantDepthChange

// Tan

// EdgeU

// MungeSkyModel

// CreateHorizonModel

// LoadDepthTable

extern void C2_HOOK_FASTCALL InitDepthEffects(void);

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// DepthEffectSky

// DoWobbleCamera

// DoDrugWobbleCamera

// DoSubaquaCam

// DoSpecialCameraEffect

// AssertYons

// IncreaseYon

// DecreaseYon

extern void C2_HOOK_STDCALL SetYon(br_scalar pYon);

// GetYon

// GetSkyTextureOn

extern void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

extern void C2_HOOK_FASTCALL ToggleSkyQuietly(void);

// ToggleSky

// GetDepthCueingOn

extern void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

// ToggleDepthCueingQuietly

// ToggleDepthCueing

// ChangeDepthEffect

// MungeForwardSky

// MungeRearviewSky

// SkyTextureChanged

// SetSkyColour

#endif // GUARD_33_DEPTH_H
