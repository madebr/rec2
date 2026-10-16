#ifndef GUARD_33_DEPTH_H
#define GUARD_33_DEPTH_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern int gHas_sky_texture;

// Log2

// CalculateWrappingMultiplier

extern br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance);

extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

extern void C2_HOOK_FASTCALL FogCars(void);

// extern void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);

// Tan

// EdgeU

extern void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel);

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

extern br_scalar C2_HOOK_STDCALL GetYon(void);

extern int C2_HOOK_FASTCALL GetSkyTextureOn(void);

extern void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

extern void C2_HOOK_FASTCALL ToggleSkyQuietly(void);

// ToggleSky

extern int C2_HOOK_FASTCALL GetDepthCueingOn(void);

extern void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

extern void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);

// ToggleDepthCueing

extern void C2_HOOK_FASTCALL ChangeDepthEffect(void);

extern void C2_HOOK_FASTCALL MungeForwardSky(void);

extern void C2_HOOK_FASTCALL MungeRearviewSky(void);

extern void C2_HOOK_FASTCALL SkyTextureChanged(void);

extern void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

#endif // GUARD_33_DEPTH_H
