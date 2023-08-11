#ifndef REC2_DEPTH_H
#define REC2_DEPTH_H

#include "c2_hooks.h"

#include <brender/brender.h>

int C2_HOOK_FASTCALL GetSkyTextureOn(void);

void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn);

int C2_HOOK_FASTCALL GetDepthCueingOn(void);

void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn);

void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void);

void C2_HOOK_FASTCALL ToggleDepthCueing(void);

void C2_HOOK_STDCALL SetYon(br_scalar pYon);

br_scalar C2_HOOK_STDCALL GetYon(void);

void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower);

void C2_HOOK_FASTCALL InitDepthEffects(void);

void C2_HOOK_FASTCALL MungeForwardSky(void);

void C2_HOOK_FASTCALL AssertYons(void);

void C2_HOOK_FASTCALL MungeRearviewSky(void);

void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial);

#endif //REC2_DEPTH_H
