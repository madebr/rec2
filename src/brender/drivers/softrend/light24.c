#include "light24.h"

void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x005491a0, lightingColourNull)

void (C2_HOOK_STDCALL * lightingColourDirectSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourDirectSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourDirectSpecular_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005491b0, lightingColourDirectSpecular, lightingColourDirectSpecular_original)

void (C2_HOOK_STDCALL * lightingColourLocal2_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourLocal2_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549380, lightingColourLocal2, lightingColourLocal2_original)

void (C2_HOOK_STDCALL * lightingColourLocal1_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourLocal1_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549510, lightingColourLocal1, lightingColourLocal1_original)

void (C2_HOOK_STDCALL * lightingColourPointSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourPointSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourPointSpecular_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549630, lightingColourPointSpecular, lightingColourPointSpecular_original)

void (C2_HOOK_STDCALL * ActiveLightAccumulateColourSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

#if defined(C2_HOOKS_ENABLED)
ActiveLightAccumulateColourSet_original(alp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00548fe0, ActiveLightAccumulateColourSet, ActiveLightAccumulateColourSet_original)
