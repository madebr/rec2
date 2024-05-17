#include "setup.h"

#include "light24.h"
#include "light8.h"

C2_HOOK_VARIABLE_IMPLEMENT(rend_block_soft, rend, 0x0079f980);

C2_HOOK_VARIABLE_IMPLEMENT(static_cache_soft, scache, 0x0079fa00);

static void eyeInModel(br_soft_renderer* self) {
    br_matrix4 s2m;

    if (self->state.matrix.model_to_view_hint == BRT_LENGTH_PRESERVING) {
        if (self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {
            C2V(scache).eye_m.v[0] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[0][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[0][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[0][2]);
            C2V(scache).eye_m.v[1] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[1][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[1][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[1][2]);
            C2V(scache).eye_m.v[2] = -BR_MAC3(
                self->state.matrix.model_to_view.m[3][0], self->state.matrix.model_to_view.m[2][0],
                self->state.matrix.model_to_view.m[3][1], self->state.matrix.model_to_view.m[2][1],
                self->state.matrix.model_to_view.m[3][2], self->state.matrix.model_to_view.m[2][2]);
            C2V(scache).eye_m.v[3] = 1.f;
            return;
        }
        if (self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
            BrVector3Copy(&C2V(scache).eye_m, (br_vector3*)C2V(scache).view_to_model.m[2]);
            C2V(scache).eye_m.v[3] = 0.f;
            return;
        }
    } else {
        if (self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {
            BrVector3Copy(&C2V(scache).eye_m, (br_vector3*)C2V(scache).view_to_model.m[3]);
            C2V(scache).eye_m.v[3] = 1.f;
            return;
        }

        if (self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
            BrVector3Copy(&C2V(scache).eye_m, (br_vector3*)C2V(scache).view_to_model.m[2]);
            C2V(scache).eye_m.v[3] = 0.f;
            return;
        }
    }
    BrMatrix4Mul34(&C2V(scache).model_to_screen, &self->state.matrix.model_to_view, &self->state.matrix.view_to_screen);
    BrMatrix4Inverse(&s2m, &C2V(scache).model_to_screen);

    C2V(scache).eye_m.v[0] = s2m.m[2][0];
    C2V(scache).eye_m.v[1] = s2m.m[2][1];
    C2V(scache).eye_m.v[2] = s2m.m[2][2];
    C2V(scache).eye_m.v[3] = s2m.m[2][3];
}

void C2_HOOK_STDCALL ViewToModelUpdate(br_soft_renderer* self) {

    BrMatrix34Inverse(&C2V(scache).view_to_model, &self->state.matrix.model_to_view);
}
C2_HOOK_FUNCTION(0x00543ec0, ViewToModelUpdate)

void C2_HOOK_STDCALL ModelToScreenUpdate(br_soft_renderer* self) {

    BrMatrix4Mul34(&C2V(scache).model_to_screen,
        &self->state.matrix.model_to_view,
        &self->state.matrix.view_to_screen);
}
C2_HOOK_FUNCTION(0x00543ee0, ModelToScreenUpdate)

void (C2_HOOK_STDCALL * StaticCacheUpdate_PerScene_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL StaticCacheUpdate_PerScene(br_soft_renderer* self) {

#if 0//defined(C2_HOOKS_ENABLED)
    StaticCacheUpdate_PerScene_original(self);
#else
    int i;

    C2V(scache).user_clip_active = 0;

    C2_HOOK_BUG_ON(MAX_STATE_CLIP_PLANES != 6);
    for (i = 0; i < MAX_STATE_CLIP_PLANES; i++) {
        if (self->state.clip[i].type == BRT_PLANE)
            C2V(scache).user_clip_active = 1;
    }
    ActiveLightsFind(self);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543a40, StaticCacheUpdate_PerScene, StaticCacheUpdate_PerScene_original)

void (C2_HOOK_STDCALL * StaticCacheUpdate_PerModel_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL StaticCacheUpdate_PerModel(br_soft_renderer* self) {

#if 0//defined(C2_HOOKS_ENABLED)
    StaticCacheUpdate_PerModel_original(self);
#else
#if COUNTERS
    CounterPerModel++;
#endif

    if (!C2V(scache).valid_v2m) {
        ViewToModelUpdate(self);
        C2V(scache).valid_v2m = 1;
    }

    if (!C2V(scache).valid_m2s) {
        ModelToScreenUpdate(self);
        C2V(scache).valid_m2s = 1;
    }

    eyeInModel(self);
    BrVector3Normalise(&C2V(scache).eye_m_normalised, &C2V(scache).eye_m);

    if (self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
        BrMatrix34Mul(&C2V(scache).model_to_environment,
            &self->state.matrix.model_to_view,
            &self->state.matrix.view_to_environment);
    }

    ActiveLightsUpdate(self);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543a80, StaticCacheUpdate_PerModel, StaticCacheUpdate_PerModel_original)

br_error (C2_HOOK_STDCALL * CacheUpdate_original)(br_soft_renderer* self);
br_error C2_HOOK_STDCALL CacheUpdate(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    return CacheUpdate_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543d80, CacheUpdate, CacheUpdate_original)

void (C2_HOOK_STDCALL * ActiveLightsFind_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL ActiveLightsFind(br_soft_renderer* self) {

#if 0//defined(C2_HOOKS_ENABLED)
    ActiveLightsFind_original(self);
#else
    int l;
    soft_state_light* lp;
    active_light* alp;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(static_cache_soft, nlights_model, 0x498);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(static_cache_soft, nlights_view, 0x49c);
    C2_HOOK_BUG_ON(BR_ASIZE(C2V(scache).lights) != 16);
    C2_HOOK_BUG_ON(BR_ASIZE(self->state.light) != 16);

    C2V(scache).nlights_model = 0;
    C2V(scache).nlights_view = 0;

    alp = &C2V(scache).lights[0];

    for (l = 0; l < BR_ASIZE(C2V(scache).lights); l++) {
        lp = &self->state.light[l];

        if (lp->type == BRT_NONE || lp->lighting_space != BRT_MODEL) {
            continue;
        }

        alp->s = lp;
        alp->intensity = 1.f / lp->attenuation_c;
        alp->type = lp->type;

        ActiveLightAccumulateIndexSet(alp);
        ActiveLightAccumulateColourSet(alp);
        alp++;
        C2V(scache).nlights_model += 1;
    }

    for (l = 0; l < BR_ASIZE(self->state.light); l++) {
        lp = &self->state.light[l];

        if (lp->type == BRT_NONE || lp->lighting_space != BRT_VIEW) {
            continue;
        }

        alp->s = lp;
        alp->intensity = 1.f / lp->attenuation_c;
        alp->type = lp->type;
        BrVector3Copy(&alp->position, &lp->position);

        if (lp->type == BRT_DIRECT) {
            BrVector3Scale(&alp->direction, &lp->direction, alp->intensity);
        } else {
            BrVector3Copy(&alp->direction, &lp->direction);
        }

        ActiveLightAccumulateIndexSet(alp);
        ActiveLightAccumulateColourSet(alp);

        alp++;
        C2V(scache).nlights_view += 1;
    }

    C2V(scache).light_1md = C2V(scache).nlights_view == 0 && C2V(scache).nlights_model == 1 && C2V(scache).lights[0].type == BRT_DIRECT;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00543f10, ActiveLightsFind, ActiveLightsFind_original)

void (C2_HOOK_STDCALL * ActiveLightsUpdate_original)(br_soft_renderer* self);
void C2_HOOK_STDCALL ActiveLightsUpdate(br_soft_renderer* self) {

#if defined(C2_HOOKS_ENABLED)
    ActiveLightsUpdate_original(self);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00544060, ActiveLightsUpdate, ActiveLightsUpdate_original)
