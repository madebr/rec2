#include "enables.h"

#include "actsupt.h"
#include "dbsetup.h"

#include "core/fw/diag.h"
#include "core/fw/resource.h"
#include "core/math/matrix34.h"
#include "core/math/transfrm.h"

#include "c2_stdlib.h"

#include <stddef.h>

void C2_HOOK_STDCALL actorEnable(br_v1db_enable* e, br_actor* a) {
    int i;

    if (!e->enabled) {
        e->enabled = BrResAllocate(C2V(v1db).res, e->max * sizeof(*e->enabled), BR_MEMORY_ENABLED_ACTORS);
    }

    for (i = 0; i < e->max; i++) {
        if (e->enabled[i] == a) {
            return;
        }
    }

    for (i = 0; i < e->max; i++) {
        if (!e->enabled[i]) {
            e->enabled[i] = a;
            e->count++;
            return;
        }
    }

    BrFailure("too many enabled %ss", e->name);
}

void C2_HOOK_STDCALL actorDisable(br_v1db_enable* e, br_actor* a) {
    int i;

    if (!e->enabled) {
        return;
    }

    for (i = 0; i < e->max; i++) {
        if (e->enabled[i] == a) {
            e->enabled[i] = NULL;
            e->count--;
            return;
        }
    }
}

void C2_HOOK_CDECL BrLightEnable(br_actor* l) {

    actorEnable(&C2V(v1db).enabled_lights, l);
}
C2_HOOK_FUNCTION(0x00524e30, BrLightEnable);

void C2_HOOK_CDECL BrLightDisable(br_actor* l) {

    actorDisable(&C2V(v1db).enabled_lights, l);
}
C2_HOOK_FUNCTION(0x00524ec0, BrLightDisable);

void C2_HOOK_CDECL BrClipPlaneEnable(br_actor* c) {

    actorEnable(&C2V(v1db).enabled_clip_planes, c);
}
C2_HOOK_FUNCTION(0x00524f10, BrClipPlaneEnable);

void C2_HOOK_CDECL BrClipPlaneDisable(br_actor* c) {

    actorDisable(&C2V(v1db).enabled_clip_planes, c);
}
C2_HOOK_FUNCTION(0x00524fa0, BrClipPlaneDisable);

void C2_HOOK_CDECL BrHorizonPlaneEnable(br_actor* h) {

    actorEnable(&C2V(v1db).enabled_horizon_planes, h);
}
C2_HOOK_FUNCTION(0x00524ff0, BrHorizonPlaneEnable);

void C2_HOOK_CDECL BrHorizonPlaneDisable(br_actor* h) {

    actorDisable(&C2V(v1db).enabled_horizon_planes, h);
}
C2_HOOK_FUNCTION(0x00525080, BrHorizonPlaneDisable);

br_actor* C2_HOOK_CDECL BrEnvironmentSet(br_actor* a) {
    br_actor* old_a;

    old_a = C2V(v1db).enabled_environment;
    C2V(v1db).enabled_environment = a;
    return old_a;
}
C2_HOOK_FUNCTION(0x005250d0, BrEnvironmentSet)

br_boolean C2_HOOK_STDCALL setupView(br_matrix34* view_to_this, br_matrix34* this_to_view, br_matrix34* world_to_view, br_int_32 w2vt, br_actor* world, br_actor* a) {
    br_matrix34 this_to_world;
    br_int_32 root_t;
    br_int_32 t;
    if (ActorToRootTyped(a, world, &this_to_world, &root_t) == 0) {
        return 0;
    }
    BrMatrix34Mul(this_to_view, &this_to_world, world_to_view);
    t = BrTransformCombineTypes(root_t, w2vt);
    if (BrTransformTypeIsLP(t)) {
        BrMatrix34LPInverse(view_to_this, this_to_view);
    } else {
        BrMatrix34Inverse(view_to_this, this_to_view);
    }
    return 1;
}

void (C2_HOOK_STDCALL * BrSetupLights_original)(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt);
void C2_HOOK_STDCALL BrSetupLights(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrSetupLights_original(world, world_to_view, w2vt);
#else
    br_matrix34 this_to_view;
    br_matrix34 view_to_this;
    int light_part;
    int i;
    br_token_value tv[16];
    br_token_value* tvp;
    br_vector3 view_position;
    br_vector3 view_direction;
    br_light* light;

    if (C2V(v1db).enabled_lights.enabled == NULL) {
        return;
    }
    light_part = 0;
    for (i = 0; i < C2V(v1db).enabled_lights.max; i++) {
        tvp = tv;

        if (C2V(v1db).enabled_lights.enabled[i] == NULL) {
            continue;
        }

        light = C2V(v1db).enabled_lights.enabled[i]->type_data;

        if (!setupView(&view_to_this, &this_to_view, world_to_view, w2vt, world, C2V(v1db).enabled_lights.enabled[i])) {
            continue;
        }

        tvp = tv;

        tvp->t = BRT_COLOUR_RGB;
        tvp->v.rgb = light->colour;
        tvp++;

        tvp->t = BRT_ATTENUATION_C_F;
        tvp->v.f = light->attenuation_c;
        tvp++;

        tvp->t = BRT_ATTENUATION_L_F;
        tvp->v.f = light->attenuation_l;
        tvp++;

        tvp->t = BRT_ATTENUATION_Q_F;
        tvp->v.f = light->attenuation_q;
        tvp++;

        tvp->t = BRT_SPACE_T;
        tvp->v.t = (light->type & BR_LIGHT_VIEW) ? BRT_VIEW : BRT_MODEL;
        tvp++;

        switch (light->type & BR_LIGHT_TYPE) {
            case BR_LIGHT_POINT:
                tvp->t = BRT_TYPE_T;
                tvp->v.t = BRT_POINT;
                tvp++;
                break;
            case BR_LIGHT_POINT_LOCAL_1:
                tvp->t = BRT_TYPE_T;
                tvp->v.t = BRT_POINT_LOCAL_1;
                tvp++;
                break;
            case BR_LIGHT_POINT_LOCAL_2:
                tvp->t = BRT_TYPE_T;
                tvp->v.t = BRT_POINT_LOCAL_2;
                tvp++;
                break;
            case BR_LIGHT_DIRECT:
                tvp->t = BRT_TYPE_T;
                tvp->v.t = BRT_DIRECT;
                tvp++;
                break;
            case BR_LIGHT_SPOT:
                tvp->t = BRT_TYPE_T;
                tvp->v.t = BRT_SPOT;
                tvp++;
                break;
        }
        switch (light->type & BR_LIGHT_TYPE) {
            case BR_LIGHT_POINT:
            case BR_LIGHT_POINT_LOCAL_1:
            case BR_LIGHT_POINT_LOCAL_2:
                view_position.v[0] = this_to_view.m[3][0];
                view_position.v[1] = this_to_view.m[3][1];
                view_position.v[2] = this_to_view.m[3][2];
                tvp->t = BRT_POSITION_V3_F;
                tvp->v.v3_f = (br_vector3_f*)&view_position;
                tvp++;
                break;
            case BR_LIGHT_DIRECT:
                view_direction.v[0] = view_to_this.m[0][2];
                view_direction.v[1] = view_to_this.m[1][2];
                view_direction.v[2] = view_to_this.m[2][2];
                tvp->t = BRT_DIRECTION_V3_F;
                tvp->v.v3_f = (br_vector3_f*)&view_direction;
                tvp++;
                break;
            case BR_LIGHT_SPOT:
                view_position.v[0] = this_to_view.m[3][0];
                view_position.v[1] = this_to_view.m[3][1];
                view_position.v[2] = this_to_view.m[3][2];
                tvp->t = BRT_POSITION_V3_F;
                tvp->v.v3_f = (br_vector3_f*)&view_position;
                tvp++;

                view_direction.v[0] = view_to_this.m[0][2];
                view_direction.v[1] = view_to_this.m[1][2];
                view_direction.v[2] = view_to_this.m[2][2];
                tvp->t = BRT_DIRECTION_V3_F;
                tvp->v.v3_f = (br_vector3_f*)&view_direction;
                tvp++;

                tvp->t = BRT_SPOT_OUTER_F;
                tvp->v.f = BR_COS(light->cone_outer);
                tvp++;

                tvp->t = BRT_SPOT_INNER_F;
                tvp->v.f = BR_COS(light->cone_inner);
                tvp++;
                break;
            default:
                continue;
        }

        tvp->t = BR_NULL_TOKEN;

        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_LIGHT, light_part, tv, NULL);
        light_part++;
    }

    tv[0].t = BRT_TYPE_T;
    tv[0].v.t = BRT_NONE;
    tv[1].t = BR_NULL_TOKEN;

    for( ; light_part < C2V(v1db).max_light; light_part++) {
        C2V(v1db).renderer->dispatch->_partSetMany(C2V(v1db).renderer, BRT_LIGHT, light_part, tv, NULL);
    }

    C2V(v1db).max_light = light_part;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005250e0, BrSetupLights, BrSetupLights_original)

void (C2_HOOK_STDCALL * BrSetupClipPlanes_original)(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt, br_matrix4* view_to_screen);
void C2_HOOK_STDCALL BrSetupClipPlanes(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt, br_matrix4* view_to_screen) {
#if defined(C2_HOOKS_ENABLED)
    BrSetupClipPlanes_original(world, world_to_view, w2vt, view_to_screen);
#else
    br_matrix34 this_to_view;
    br_matrix34 view_to_this;
    br_matrix4 screen_to_view;
    br_matrix4 tmp4;
    br_matrix4 screen_to_this;
    int light_part;
    int i;
    br_int_32 clip_part;
    br_token_value tv[3];
    br_vector4 sp;

    c2_abort(); // FIXME: not implemented
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525450, BrSetupClipPlanes, BrSetupClipPlanes_original)

void (C2_HOOK_STDCALL * BrSetupEnvironment_original)(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt);
void C2_HOOK_STDCALL BrSetupEnvironment(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt) {
#if 0//defined(C2_HOOKS_ENABLED)
    BrSetupEnvironment_original(world, world_to_view, w2vt);
#else
    br_matrix34 view_to_this;
    br_matrix34 this_to_view;
    br_token h;

    h = BRT_DONT_CARE;
    if (C2V(v1db).enabled_environment != NULL) {
        if (C2V(v1db).enabled_environment == world) {
            if (BrTransformTypeIsLP(w2vt)) {
                BrMatrix34LPInverse(&view_to_this, world_to_view);
            } else {
                BrMatrix34Inverse(&view_to_this, world_to_view);
            }
            h = BRT_NONE;
        } else {
            if (setupView(&view_to_this, &this_to_view, world_to_view, w2vt, world, C2V(v1db).enabled_environment) != 0) {
                h = BRT_NONE;
            }
        }
    }
    if (h != BRT_DONT_CARE) {
        C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_VIEW_TO_ENVIRONMENT_M34_F, (uintptr_t)&view_to_this);
    }
    C2V(v1db).renderer->dispatch->_partSet(C2V(v1db).renderer, BRT_MATRIX, 0, BRT_VIEW_TO_ENVIRONMENT_HINT_T, h);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00525630, BrSetupEnvironment, BrSetupEnvironment_original)

void C2_HOOK_STDCALL BrSetupHorizons(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt) {
}
C2_HOOK_FUNCTION(0x00525750, BrSetupHorizons)

void C2_HOOK_STDCALL BrActorEnableCheck(br_actor* a) {
    if (C2V(v1db).enabled_environment == a) {
        C2V(v1db).enabled_environment = NULL;
    }
    if (a->type == BR_ACTOR_LIGHT) {
        actorDisable(&C2V(v1db).enabled_lights, a);
    } else if (a->type == BR_ACTOR_HORIZONTAL_PLANE) {
        actorDisable(&C2V(v1db).enabled_horizon_planes, a);
    } else if (a->type <= BR_ACTOR_CLIP_PLANE) {
        actorDisable(&C2V(v1db).enabled_clip_planes, a);
    }
}
C2_HOOK_FUNCTION(0x00525760, BrActorEnableCheck)
