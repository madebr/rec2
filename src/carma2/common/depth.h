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

void C2_HOOK_FASTCALL InitDepthEffects(void);

#endif //REC2_DEPTH_H
