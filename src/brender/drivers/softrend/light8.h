#ifndef SOFTREND_LIGHT8_H
#define SOFTREND_LIGHT8_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexDirect(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexPoint(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexPointAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL lightingIndexSpot(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);

void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp);

#endif /* SOFTREND_LIGHT8_H */
