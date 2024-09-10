#include "raycast.h"

#include "spark.h"

#include <brender/brender.h>

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gY_picking_camera, 0x006a20d0);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gPick_model_to_view__raycast, 0x006a20f0);


void (C2_HOOK_FASTCALL * InitRayCasting_original)(void);
void C2_HOOK_FASTCALL InitRayCasting(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitRayCasting_original();
#else
    br_camera* camera_ptr;

    C2V(gY_picking_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera_ptr = C2V(gY_picking_camera)->type_data;
    camera_ptr->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera_ptr->field_of_view = BrDegreeToAngle(70.0f);
    camera_ptr->hither_z = 0.001f;
    camera_ptr->yon_z = 1000.0f;
    camera_ptr->aspect = 1.0f;
    C2V(gY_picking_camera)->t.t.mat.m[0][0] =  1.f;
    C2V(gY_picking_camera)->t.t.mat.m[0][1] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[0][2] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][0] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][1] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[1][2] = -1.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][0] =  0.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][1] =  1.f;
    C2V(gY_picking_camera)->t.t.mat.m[2][2] =  0.f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e3660, InitRayCasting, InitRayCasting_original)

int C2_HOOK_FASTCALL PickBoundsTestRay__raycast(br_bounds* b, br_vector3* rp, br_vector3* rd, br_scalar t_near, br_scalar t_far, br_scalar* new_t_near, br_scalar* new_t_far) {
    int i;
    float s;
    float t;

    for (i = 0; i < 3; i++) {
        if (rd->v[i] > 2.384186e-07f) {
            s = (1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (s > t_near) {
                t_near = s;
            }
            t = (1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (t < t_far) {
                t_far = t;
            }
        } else if (rd->v[i] < -2.384186e-07f) {
            s = (1.0f / rd->v[i]) * (rp->v[i] - b->max.v[i]);
            if (s < BR_SCALAR_MIN) {
                t_far = BR_SCALAR_MIN;
            } else if (s < t_far) {
                t_far = s;
            }
            t = (1.0f / rd->v[i]) * (rp->v[i] - b->min.v[i]);
            if (t > BR_SCALAR_MAX) {
                t_near = BR_SCALAR_MAX;
            } else if (t > t_near) {
                t_near = t;
            }
        } else if (rp->v[i] > b->max.v[i] || rp->v[i] < b->min.v[i]) {
            return 0;
        }
    }
    if (t_far < t_near) {
        return 0;
    }
    *new_t_near = t_near;
    *new_t_far = t_far;
    return 1;
}
C2_HOOK_FUNCTION(0x004e3a30, PickBoundsTestRay__raycast)

int C2_HOOK_FASTCALL ActorPick2D(br_actor* ap, br_model* model, br_material* material, dr_pick2d_cbfn* callback, void* arg) {
    br_actor* a;
    br_model* this_model;
    br_material* this_material;
    br_matrix34 m_to_v;
    br_matrix34 v_to_m;
    br_scalar t_near;
    br_scalar t_far;
    int r;
    br_vector3 dir;

    r = 0;
    if (ap->model != NULL) {
        this_model = ap->model;
    } else {
        this_model = model;
    }
    if (ap->material != NULL) {
        this_material = ap->material;
    } else {
        this_material = material;
    }
    if (ap->render_style == BR_RSTYLE_NONE) {
        return 0;
    }
    m_to_v = C2V(gPick_model_to_view__raycast);

    BrMatrix34PreTransform(&C2V(gPick_model_to_view__raycast), &ap->t);
    switch (ap->type) {
    case BR_ACTOR_MODEL:
        if (this_model != NULL) {
            BrMatrix34Inverse(&v_to_m, &C2V(gPick_model_to_view__raycast));
            r = PickBoundsTestRay__raycast(&this_model->bounds,
                    (br_vector3 *) v_to_m.m[3], (br_vector3 *) v_to_m.m[2],
                    0.f, BR_SCALAR_MAX, &t_near, &t_far);
            if (r != 0) {
                BrVector3Negate(&dir, (br_vector3 *) v_to_m.m[2]);
                r = callback(
                        ap, this_model, this_material, (br_vector3 *) v_to_m.m[3],
                        &dir, t_near, t_far, arg);
            }
            if (r != 0) {
                C2V(gPick_model_to_view__raycast) = m_to_v;
                return r;
            }
        }
        break;
    case BR_ACTOR_BOUNDS:
    case BR_ACTOR_BOUNDS_CORRECT:
        BrMatrix34Inverse(&v_to_m, &C2V(gPick_model_to_view__raycast));
        r = PickBoundsTestRay__raycast((br_bounds*)ap->type_data,
                (br_vector3*)v_to_m.m[3], (br_vector3*)v_to_m.m[2],
                0.0f, BR_SCALAR_MAX, &t_near, &t_far);
        if (r != 0) {
            for (a = ap->children; a != NULL; a = a->next) {
                r = ActorPick2D(a, this_model, this_material, callback, arg);
                if (r != 0) {
                    break;
                }
            }
        }
        C2V(gPick_model_to_view__raycast) = m_to_v;
        return r;
    }
    for (a = ap->children; a != NULL; a = a->next) {
        r = ActorPick2D(a, this_model, this_material, callback, arg);
        if (r != 0) {
            break;
        }
    }
    C2V(gPick_model_to_view__raycast) = m_to_v;
    return r;
}
C2_HOOK_FUNCTION(0x004e3840, ActorPick2D)

int C2_HOOK_FASTCALL DRActorToRoot(br_actor* a, br_actor* world, br_matrix34* m) {

    if (world == a) {
        BrMatrix34Identity(m);
        return 1;
    } else {
        BrTransformToMatrix34(m, &a->t);
        for (a = a->parent; a != NULL && world != a; a = a->parent) {
            if (a->t.type != BR_TRANSFORM_IDENTITY) {
                BrMatrix34PostTransform(m, &a->t);
            }
        }
        return world == a;
    }
}

int C2_HOOK_FASTCALL DRScenePick2DXY(br_actor* world, br_actor* camera, br_pixelmap* viewport, int pick_x, int pick_y, dr_pick2d_cbfn* callback, void* arg) {
    br_matrix34 camera_tfm;
    br_scalar scale;
    br_scalar cos_angle;
    br_scalar sin_angle;
    br_camera* camera_data;
    br_angle view_over_2;

    camera_data = camera->type_data;
    DRActorToRoot(camera, world, &camera_tfm);
    BrMatrix34Inverse(&C2V(gPick_model_to_view__raycast), &camera_tfm);
    view_over_2 = camera_data->field_of_view / 2;
    cos_angle = BR_COS(view_over_2);
    sin_angle = BR_SIN(view_over_2);
    scale = cos_angle / sin_angle;
    BrMatrix34PostScale(&C2V(gPick_model_to_view__raycast), scale / camera_data->aspect, scale, 1.f);
    BrMatrix34PostShearZ(&C2V(gPick_model_to_view__raycast),
                         (float)(2 * pick_x) / (float)viewport->width,
                         (float)(-2 * pick_y) / (float)viewport->height);
    return ActorPick2D(world, NULL, C2V(gBlack_material), callback, arg);
}
C2_HOOK_FUNCTION(0x004e36f0, DRScenePick2DXY)
