#ifndef REC2_DEPTH_H
#define REC2_DEPTH_H

#include "c2_hooks.h"

#include "rec2_types.h"
#include <brender/brender.h>

C2_HOOK_VARIABLE_DECLARE(tSpecial_volume*, gDAT_0079ec44);
C2_HOOK_VARIABLE_DECLARE(tDepth_effect_type, gSwap_depth_effect_type);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gSky_texture_0079ec1c);
C2_HOOK_VARIABLE_DECLARE(int, gSwap_depth_effect_start);
C2_HOOK_VARIABLE_DECLARE(int, gSwap_depth_effect_end);
C2_HOOK_VARIABLE_DECLARE(tDepth_effect_type, gSwap_depth_effect_type);
C2_HOOK_VARIABLE_DECLARE(int, gSwap_depth_effect_colour_blue);
C2_HOOK_VARIABLE_DECLARE(int, gSwap_depth_effect_colour_red);
C2_HOOK_VARIABLE_DECLARE(int, gSwap_depth_effect_colour_green);


void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);

int C2_HOOK_FASTCALL GetSkyTextureOn(void);

void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

int C2_HOOK_FASTCALL GetDepthCueingOn(void);

void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial);

void C2_HOOK_FASTCALL ToggleSkyQuietly(void);

void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);

void C2_HOOK_FASTCALL ToggleDepthCueing(void);

void C2_HOOK_STDCALL SetYon(br_scalar pYon);

br_scalar C2_HOOK_STDCALL GetYon(void);

void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower);

void C2_HOOK_FASTCALL InitDepthEffects(void);

void C2_HOOK_FASTCALL MungeForwardSky(void);

void C2_HOOK_FASTCALL AssertYons(void);

void C2_HOOK_FASTCALL MungeRearviewSky(void);

void C2_HOOK_FASTCALL SkyTextureChanged(void);

void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

void C2_HOOK_FASTCALL ChangeDepthEffect(void);

#endif //REC2_DEPTH_H
