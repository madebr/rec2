#include "light24.h"

#include "lightmac.h"
#include "setup.h"

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

void C2_HOOK_CDECL SurfaceColourLit(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar* comp) {
    int i;
    active_light* alp;
    br_vector3 vp;
    br_vector3 vn;
    br_vector3 fvn;
    br_scalar red, green, blue;

    red = BrFixedToScalar(BR_RED(colour) << 8);
    green = BrFixedToScalar(BR_GRN(colour) << 8);
    blue = BrFixedToScalar(BR_BLU(colour) << 8);

    comp[C_R] = self->state.surface.ka * red;
    comp[C_G] = self->state.surface.ka * green;
    comp[C_B] = self->state.surface.ka * blue;

    BrVector3Copy(&C2V(rend).eye_l, &C2V(scache).eye_m_normalised);

    alp = C2V(scache).lights;

    for (i = 0; i < C2V(scache).nlights_model; i++, alp++) {
        alp->accumulate_colour(self, p, n, colour, alp, comp);
    }

    if (C2V(scache).nlights_view != 0) {
        BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
        BrMatrix34TApplyV(&vn, n, &C2V(scache).view_to_model);
        BrVector3Normalise(&fvn, &vn);

        BrVector3Set(&C2V(rend).eye_l, 0.f, 0.f, 1.f);

        for (i = 0; i < C2V(scache).nlights_view; i++, alp++) {
            alp->accumulate_colour(self, &vp, &fvn, colour, alp, comp);
        }
    }

    CLAMP_SCALE(C_R);
    CLAMP_SCALE(C_G);
    CLAMP_SCALE(C_B);
}
C2_HOOK_FUNCTION(0x00548cf0, SurfaceColourLit)

void C2_HOOK_STDCALL lightingColourNull(br_soft_renderer* self, br_vector3* p, br_vector3* n, undefined4 param_4, active_light* alp, br_scalar* comp) {

}
C2_HOOK_FUNCTION(0x005491a0, lightingColourNull)

void (C2_HOOK_STDCALL * lightingColourDirectSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourDirectSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {

#if 0//defined(C2_HOOKS_ENABLED)
    lightingColourDirectSpecular_original(self, p, n, param_4, alp, comp);
#else
    br_scalar dot;
    br_scalar red, grn, blu;

    dot = BrVector3Dot(&alp->direction, n);
    if (dot <= 0.f) {
        return;
    }

    red = BrFixedToFloat(BR_RED(colour) << 8) * dot;
    grn = BrFixedToFloat(BR_GRN(colour) << 8) * dot;
    blu = BrFixedToFloat(BR_BLU(colour) << 8) * dot;

    if (self->state.surface.ks != 0.f) {
        dot = BrVector3Dot(&alp->half, n);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = alp->intensity * SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005491b0, lightingColourDirectSpecular, lightingColourDirectSpecular_original)

void (C2_HOOK_STDCALL * lightingColourLocal2_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourLocal2(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {

#if 0//defined(C2_HOOKS_ENABLED)
    lightingColourLocal2_original(self, p, n, param_4, alp, comp);
#else
    br_scalar dot;
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        dot = BrVector3Dot(&dirn, n);
        if (dot >= 0.f) {
            br_scalar attn;

            attn = self->state.surface.kd * dot * (alp->s->attenuation_q - dist2) / (alp->s->attenuation_q * sqrtf(dist2));
            comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
            comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
            comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549380, lightingColourLocal2, lightingColourLocal2_original)

void (C2_HOOK_STDCALL * lightingColourLocal1_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourLocal1(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {

#if 0//defined(C2_HOOKS_ENABLED)
    lightingColourLocal1_original(self, p, n, param_4, alp, comp);
#else
    br_scalar dist2;
	br_vector3 dirn;

    BrVector3Sub(&dirn, &alp->position, p);
    dist2 = BrVector3LengthSquared(&dirn);
    if (dist2 <= alp->s->attenuation_q && dist2 >= BR_SCALAR_EPSILON) {
        br_scalar attn;

        attn = self->state.surface.kd * (alp->s->attenuation_q - dist2) / alp->s->attenuation_q ;
        comp[C_R] += attn * BrFixedToFloat(BR_RED(colour)) * BR_RED(alp->s->colour);
        comp[C_G] += attn * BrFixedToFloat(BR_GRN(colour)) * BR_GRN(alp->s->colour);
        comp[C_B] += attn * BrFixedToFloat(BR_BLU(colour)) * BR_BLU(alp->s->colour);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00549510, lightingColourLocal1, lightingColourLocal1_original)

void (C2_HOOK_STDCALL * lightingColourPointSpecular_original)(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp);
void C2_HOOK_STDCALL lightingColourPointSpecular(br_soft_renderer* self, br_vector3* p, br_vector3* n, br_colour colour, active_light* alp, br_scalar* comp) {

#if 0//defined(C2_HOOKS_ENABLED)
    lightingColourPointSpecular_original(self, p, n, param_4, alp, comp);
#else
    br_scalar dot;
    br_vector3 dirn;
    br_scalar red, grn, blu;

    BrVector3Sub(&dirn, &alp->position, p);
    BrVector3Normalise(&dirn, &dirn);

    dot = BrVector3Dot(&dirn, n);
    if (dot <= 0.f) {
        return;
    }
    red = BrFixedToScalar(BR_RED(colour) << 8) * self->state.surface.kd * dot;
    grn = BrFixedToScalar(BR_GRN(colour) << 8) * self->state.surface.kd * dot;
    blu = BrFixedToScalar(BR_BLU(colour) << 8) * self->state.surface.kd * dot;

    if (self->state.surface.ks != 0.f) {
        br_vector3 tmp;

        BrVector3Scale(&tmp, n, 2 * dot);
        BrVector3Sub(&tmp, &tmp, &dirn);
        dot = BrVector3Dot(&tmp , &C2V(rend).eye_l);
        if (dot > SPECULARPOW_CUTOFF) {
            br_scalar specular;

            specular = SPECULAR_POWER(self->state.surface.ks);
            red += specular;
            grn += specular;
            blu += specular;
        }
    }

    comp[C_R] += red * alp->intensity * BrFixedToFloat(BR_RED(alp->s->colour) << 8);
    comp[C_G] += grn * alp->intensity * BrFixedToFloat(BR_GRN(alp->s->colour) << 8);
    comp[C_B] += blu * alp->intensity * BrFixedToFloat(BR_BLU(alp->s->colour) << 8);
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
