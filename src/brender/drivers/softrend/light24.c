#include "light24.h"

void C2_HOOK_CDECL SurfaceColourZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_R] = self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_offsets[C_B];
}
C2_HOOK_FUNCTION(0x00548c10, SurfaceColourZero)

void C2_HOOK_CDECL SurfaceColourUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_R] = self->state.cache.comp_scales[C_R] * (float)BR_RED(colour) / 256.f + self->state.cache.comp_offsets[C_R];
    comp[C_G] = self->state.cache.comp_scales[C_G] * (float)BR_GRN(colour) / 256.f + self->state.cache.comp_offsets[C_G];
    comp[C_B] = self->state.cache.comp_scales[C_B] * (float)BR_BLU(colour) / 256.f + self->state.cache.comp_offsets[C_B];
}
C2_HOOK_FUNCTION(0x00548c40, SurfaceColourUnlit)

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

void (C2_HOOK_STDCALL * lightingColourPointAttnSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourPointAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourPointAttnSpecular_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005498f0, lightingColourPointAttnSpecular, lightingColourPointAttnSpecular_original)

void (C2_HOOK_STDCALL * lightingColourSpotSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourSpotSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourSpotSpecular_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549be0, lightingColourSpotSpecular, lightingColourSpotSpecular_original)

void (C2_HOOK_STDCALL * lightingColourSpotAttnSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourSpotAttnSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

#if defined(C2_HOOKS_ENABLED)
    lightingColourSpotAttnSpecular_original(self, p, n, param_4, alp, comp);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549ef0, lightingColourSpotAttnSpecular, lightingColourSpotAttnSpecular_original)

void (C2_HOOK_STDCALL * ActiveLightAccumulateColourSet_original)(active_light* alp);
void C2_HOOK_STDCALL ActiveLightAccumulateColourSet(active_light* alp) {

#if 0//defined(C2_HOOKS_ENABLED)
ActiveLightAccumulateColourSet_original(alp);
#else

    switch (alp->type) {
    case BRT_DIRECT:
        alp->accumulate_colour = lightingColourDirectSpecular;
        break;
    case BRT_POINT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourPointSpecular;
        } else {
            alp->accumulate_colour = lightingColourPointAttnSpecular;
        }
        break;
    case BRT_SPOT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_colour = lightingColourSpotSpecular;
        } else {
            alp->accumulate_colour = lightingColourSpotAttnSpecular;
        }
        break;
    case BRT_POINT_LOCAL_1:
        alp->accumulate_colour = lightingColourLocal1;
        break;
    case BRT_POINT_LOCAL_2:
        alp->accumulate_colour = lightingColourLocal2;
        break;
    default:
        alp->accumulate_colour = lightingColourNull;
        break;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00548fe0, ActiveLightAccumulateColourSet, ActiveLightAccumulateColourSet_original)
