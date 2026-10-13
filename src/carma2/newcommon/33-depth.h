#ifndef GUARD_33_DEPTH_H
#define GUARD_33_DEPTH_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern br_pixelmap* gDepth_shade_table;
extern int gDepth_shade_table_power;
extern br_pixelmap* gFog_shade_table;
extern int gFog_shade_table_power;
extern br_pixelmap* gAcid_shade_table;
extern int gAcid_shade_table_power;
extern br_pixelmap* gWater_shade_table;
extern int gWater_shade_table_power;
extern br_actor* gForward_sky_actor;
extern br_material* gHorizon_material;
extern br_model* gForward_sky_model;

// Log2

extern br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon);

extern br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance);

extern void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

extern void C2_HOOK_FASTCALL FogCars(void);

// extern void C2_HOOK_FASTCALL FrobFog(void);

extern void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);

extern br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle);

extern br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect);

// MungeSkyModel

// CreateHorizonModel

extern void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower);

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

extern void C2_HOOK_FASTCALL AssertYons(void);

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

// ChangeDepthEffect

// MungeForwardSky

// MungeRearviewSky

extern void C2_HOOK_FASTCALL SkyTextureChanged(void);

extern void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

#endif // GUARD_33_DEPTH_H
