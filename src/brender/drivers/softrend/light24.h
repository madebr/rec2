#ifndef SOFTREND_LIGHT24_H
#define SOFTREND_LIGHT24_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL SurfaceColourZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourDirectSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourPointSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourPointAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourSpotSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingColourSpotAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp);

#endif /* SOFTREND_LIGHT24_H */
