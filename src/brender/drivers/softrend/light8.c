#include "light8.h"

void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x0054a7b0, lightingIndexNull)

void (C2_HOOK_STDCALL * lightingIndexDirect_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexDirect(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexDirect_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a7c0, lightingIndexDirect, lightingIndexDirect_original)

void (C2_HOOK_STDCALL * lightingIndexPoint_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexPoint(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexPoint_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a870, lightingIndexPoint, lightingIndexPoint_original)

void (C2_HOOK_STDCALL * lightingIndexLocal1_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexLocal1_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a9f0, lightingIndexLocal1, lightingIndexLocal1_original)

void (C2_HOOK_STDCALL * lightingIndexLocal2_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexLocal2_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054aa80, lightingIndexLocal2, lightingIndexLocal2_original)

void (C2_HOOK_STDCALL * lightingIndexPointAttn_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexPointAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexPointAttn_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054ab70, lightingIndexPointAttn, lightingIndexPointAttn_original)

void (C2_HOOK_STDCALL * lightingIndexSpot_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexSpot(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexSpot_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054ad20, lightingIndexSpot, lightingIndexSpot_original)

void (C2_HOOK_STDCALL * lightingIndexSpotAttn_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexSpotAttn(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingIndexSpotAttn_original(self, p, n, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054af00, lightingIndexSpotAttn, lightingIndexSpotAttn_original)

void (C2_HOOK_STDCALL * ActiveLightAccumulateIndexSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateIndexSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
    ActiveLightAccumulateIndexSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a5f0, ActiveLightAccumulateIndexSet, ActiveLightAccumulateIndexSet_original)
