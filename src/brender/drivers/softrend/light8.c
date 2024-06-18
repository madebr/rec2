#include "light8.h"

#include "lightmac.h"
#include "setup.h"

void C2_HOOK_CDECL SurfaceIndexZero(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_I] = self->state.cache.comp_offsets[C_I];
}
C2_HOOK_FUNCTION(0x0054a230, SurfaceIndexZero)

void C2_HOOK_CDECL SurfaceIndexUnlit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {

    comp[C_I] = self->state.cache.comp_scales[C_I] * (((float)BR_ALPHA(colour)) / 256.f) + self->state.cache.comp_offsets[C_I];
}
C2_HOOK_FUNCTION(0x0054a250, SurfaceIndexUnlit)

void C2_HOOK_CDECL SurfaceIndexLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
    int i;
    active_light* alp = C2V(scache).lights;
    br_vector3 vp;
    br_vector3 vn;
    br_vector3 fvn;

    if (C2V(scache).light_1md) {
        br_scalar l, dot;

        dot = BrVector3Dot(n, &alp->direction);

        if (dot <= 0.0f) {
            comp[C_I] = self->state.surface.ka;
            CLAMP_SCALE(C_I);
            return;
        }

        l = dot * self->state.surface.kd;

        if (self->state.surface.ks != 0.0f) {
            dot = BrVector3Dot(n, &alp->half);

            if (dot > SPECULARPOW_CUTOFF) {
                l += SPECULAR_POWER(self->state.surface.ks * alp->intensity);
            }
        }

        comp[C_I] = l + self->state.surface.ka;

        CLAMP_SCALE(C_I);
        return;
    }

    comp[C_I] = self->state.surface.ka;

    C2V(rend).eye_l = C2V(scache).eye_m_normalised;

    for (i = 0; i < C2V(scache).nlights_model; i++, alp++) {
        alp->accumulate_index(self, p, n, alp, comp);
    }

    if (C2V(scache).nlights_view != 0) {
        BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
        BrMatrix34TApplyV(&vn, n, &C2V(scache).view_to_model);
        BrVector3Normalise(&fvn, &vn);

        BrVector3Set(&C2V(rend).eye_l, 0.f, 0.f, 1.f);

        for (i = 0; i < C2V(scache).nlights_view; i++, alp++) {
            alp->accumulate_index(self, &vp, &fvn, alp, comp);
        }
    }

    CLAMP_SCALE(C_I);
}
C2_HOOK_FUNCTION(0x0054a290, SurfaceIndexLit)

void C2_HOOK_STDCALL lightingIndexNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x0054a7b0, lightingIndexNull)

void (C2_HOOK_STDCALL * lightingIndexDirect_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingIndexDirect(br_soft_renderer* self, br_vector3* p, br_vector3* n, active_light* alp, br_scalar* comp) {

#if 0//defined(C2_HOOKS_ENABLED)
    lightingIndexDirect_original(self, p, n, alp, comp);
#else
    br_scalar dot;
    br_scalar l;

    dot = BrVector3Dot(n, &alp->direction);
    if (dot <= 0.f) {
        return;
    }
    l = dot * self->state.surface.kd;
    if (self->state.surface.ks != 0.f) {
        dot = BrVector3Dot(&alp->half, n);
        if (dot > SPECULARPOW_CUTOFF) {
            l += alp->intensity * SPECULAR_POWER(self->state.surface.ks);
        }
    }

    comp[C_I] += l;
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

#if 0//defined(C2_HOOKS_ENABLED)
    ActiveLightAccumulateIndexSet_original(alp);
#else
    switch (alp->type) {
    case BRT_DIRECT:
        alp->accumulate_index = lightingIndexDirect;
        break;
    case BRT_POINT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_index = lightingIndexPoint;
        } else {
            alp->accumulate_index = lightingIndexPointAttn;
        }
        break;
    case BRT_SPOT:
        if (alp->s->attenuation_l == 0.f && alp->s->attenuation_q == 0.f) {
            alp->accumulate_index = lightingIndexSpot;
        } else {
            alp->accumulate_index = lightingIndexSpotAttn;
        }
        break;
    case BRT_POINT_LOCAL_1:
        alp->accumulate_index = lightingIndexLocal1;
        break;
    case BRT_POINT_LOCAL_2:
        alp->accumulate_index = lightingIndexLocal2;
        break;
    default:
        alp->accumulate_index = lightingIndexNull;
        break;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0054a5f0, ActiveLightAccumulateIndexSet, ActiveLightAccumulateIndexSet_original)
