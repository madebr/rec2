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
C2_HOOK_VARIABLE_DECLARE(float, gSky_width);
C2_HOOK_VARIABLE_DECLARE(float, gSky_height);
C2_HOOK_VARIABLE_DECLARE(float, gSky_x_multiplier);
C2_HOOK_VARIABLE_DECLARE(float, gSky_y_multiplier);
C2_HOOK_VARIABLE_DECLARE(int, gHas_sky_texture);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gDepth_shade_table);


intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex);

void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback);

void C2_HOOK_FASTCALL FogCars(void);

void C2_HOOK_FASTCALL FrobFog(void);

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

br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle);

br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect);

br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon);

void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel);

void C2_HOOK_FASTCALL MungeForwardSky(void);

void C2_HOOK_FASTCALL AssertYons(void);

void C2_HOOK_FASTCALL MungeRearviewSky(void);

void C2_HOOK_FASTCALL SkyTextureChanged(void);

void C2_HOOK_FASTCALL SetSkyColour(br_colour c);

void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

void C2_HOOK_FASTCALL ChangeDepthEffect(void);

#endif //REC2_DEPTH_H
