#include "raycast.h"

#include "brucetrk.h"
#include "globvars.h"
#include "spark.h"

#include <brender/brender.h>

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gY_picking_camera, 0x006a20d0);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gPick_model_to_view__raycast, 0x006a20f0);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gHighest_y_below, 0x006a2120);
C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gMaterial_below, 0x0070518c);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gLowest_y_above, 0x006a20dc);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gCurrent_y, 0x006a20e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gAbove_model, 0x006a20e4);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gBelow_model, 0x006a20d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAbove_face_index, 0x006a20e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gBelow_face_index, 0x006a20d8);


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

int C2_HOOK_CDECL FindYVerticallyBelowPolyCallBack(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pContext) {

    if (pMaterial != NULL && pMaterial->identifier != NULL
            && pMaterial->identifier[0] != '!'
            && pMaterial->identifier[0] != '>') {
        if (C2V(gHighest_y_below) < pPoint->v[1]) {
            C2V(gHighest_y_below) = pPoint->v[1];
            C2V(gMaterial_below) = pMaterial;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004e45b0, FindYVerticallyBelowPolyCallBack)

int C2_HOOK_FASTCALL BadDiv_raycast(br_scalar a, br_scalar b) {

    return fabsf(b) < 1.0f && fabs(a) > fabs(b) * BR_SCALAR_MAX;
}

void C2_HOOK_FASTCALL DRVector2AccumulateScale_raycast(br_vector2* a, br_vector2* b, br_scalar s) {

    a->v[0] = b->v[0] * s + a->v[0];
    a->v[1] = b->v[1] * s + a->v[1];
}

int C2_HOOK_FASTCALL DRModelPick2D_raycast(br_model* model, br_material* material, br_vector3* ray_pos, br_vector3* ray_dir, br_scalar t_near, br_scalar t_far, dr_modelpick2d_raycast_cbfn* callback, void* arg) {
    int f;
    int axis_m;
    int axis_0;
    int axis_1;
    int group;
    br_scalar t;
    br_scalar d;
    br_vector3 p;
    float u0;
    float u1;
    float u2;
    float v0;
    float v1;
    float v2;
    br_scalar v0i1;
    br_scalar v0i2;
    float alpha;
    float beta;
    float f_d;
    float f_n;
    br_scalar s_alpha;
    br_scalar s_beta;
    br_vector2 map;
    int v;
    int e;
    int r;
    br_material* this_material;
    br_scalar numerator;
    v11face* v11f;
    v11group* v11g;
    br_vector4* eqn;

    t_near -= 0.001f;
    t_far += 0.001f;
    for (group = 0; group < V11MODEL(model)->ngroups; group++) {
        v11g = &V11MODEL(model)->groups[group];
        for (f = 0; f < v11g->nfaces; f++) {
            v11f = &v11g->faces[f];
            eqn = &v11f->eqn;
            if (v11g->face_colours.materials[0] != NULL) {
                this_material = v11g->face_colours.materials[0];
            } else {
                this_material = material;
            }
            d = BrVector3Dot(eqn, ray_dir);
            if ((fabsf(d) >= 2.3841858e-7f && this_material != NULL && (this_material->flags & (BR_MATF_TWO_SIDED | BR_MATF_ALWAYS_VISIBLE)) != 0) || d <= 0.0) {
                numerator = BrVector3Dot(eqn, ray_pos) - eqn->v[3];
                if (!BadDiv_raycast(numerator, d)) {
                    t = -(numerator / d);
                    if (t >= t_near && t <= t_far) {
                        BrVector3Scale(&p, ray_dir, t);
                        BrVector3Accumulate(&p, ray_pos);
                        axis_m = (fabsf(eqn->v[1]) > fabsf(eqn->v[0])) ? 1 : 0;
                        if (fabsf(eqn->v[2]) > fabsf(eqn->v[axis_m])) {
                            axis_m = 2;
                        }
                        if (axis_m == 0) {
                            axis_0 = 1;
                            axis_1 = 2;
                        } else if (axis_m == 1) {
                            axis_0 = 0;
                            axis_1 = 2;
                        } else if (axis_m == 2) {
                            axis_0 = 0;
                            axis_1 = 1;
                        }

                        v0 = v11g->vertices[v11f->vertices[0]].p.v[axis_0];
                        u0 = v11g->vertices[v11f->vertices[0]].p.v[axis_1];

                        v1 = v11g->vertices[v11f->vertices[1]].p.v[axis_0] - v0;
                        u1 = v11g->vertices[v11f->vertices[1]].p.v[axis_1] - u0;
                        v2 = v11g->vertices[v11f->vertices[2]].p.v[axis_0] - v0;
                        u2 = v11g->vertices[v11f->vertices[2]].p.v[axis_1] - u0;

                        v0i1 = p.v[axis_0] - v0;
                        v0i2 = p.v[axis_1] - u0;
                        if (fabs(v1) > 2.3841858e-7f) {
                            f_d = v0i2 * v1 - u1 * v0i1;
                            f_n = u2 * v1 - u1 * v2;
                            if (f_n == 0.) {
                                continue;
                            }
                            beta = f_d / f_n;
                            alpha = (v0i1 - beta * v2) / v1;
                        } else {
                            beta = v0i1 / v2;
                            alpha = (v0i2 - beta * u2) / u1;
                        }

                        if (alpha >= 0.0 && beta >= 0.0 && beta + alpha <= 1.0) {
                            s_alpha = alpha;
                            s_beta = beta;
                            BrVector2Scale(&map, &v11g->vertices[v11f->vertices[1]].map, s_alpha);
                            DRVector2AccumulateScale_raycast(
                                &map,
                                &v11g->vertices[v11f->vertices[2]].map,
                                s_beta);
                            DRVector2AccumulateScale_raycast(
                                &map,
                                &v11g->vertices[v11f->vertices[0]].map,
                                1.0f - (s_alpha + s_beta));
                            v = 0;
                            e = 1;
                            if (s_alpha <= s_beta) {
                                if (0.5f - s_beta / 2.0f > s_alpha) {
                                    e = 0;
                                }
                                if (1.0f - s_beta * 2.0f < s_alpha) {
                                    v = 1;
                                }
                            } else {
                                if (1.0f - s_beta * 2.0f > s_alpha) {
                                    e = 2;
                                }
                                if (0.5f - s_beta / 2.0f < s_alpha) {
                                    v = 2;
                                }
                            }
                            r = callback(model, this_material, ray_pos, ray_dir, t, f, e, v, &p, &map, arg);
                            if (r != 0) {
                                return r;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004e3d90, DRModelPick2D_raycast)

int C2_HOOK_CDECL FindYVerticallyBelowCallBack(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, float pT_near, float pT_far, void* pArg) {

    if (C2V(gProgram_state).current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindYVerticallyBelowPolyCallBack, pArg);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004e4570, FindYVerticallyBelowCallBack)

int C2_HOOK_FASTCALL DRScenePick2D(br_actor* world, br_actor* camera, dr_pick2d_cbfn* callback, void* arg) {
    br_matrix34 camera_tfm;
    br_scalar scale;
    br_camera* camera_data;

    camera_data = (br_camera*)camera->type_data;
    DRActorToRoot(camera, world, &camera_tfm);
    BrMatrix34Inverse(&C2V(gPick_model_to_view__raycast), &camera_tfm);
    scale = cosf(BrAngleToRadian(camera_data->field_of_view / 2)) / sinf(BrAngleToRadian(camera_data->field_of_view / 2));

    BrMatrix34PostScale(&C2V(gPick_model_to_view__raycast), scale / camera_data->aspect, scale, 1.0f);
    return ActorPick2D(world, NULL, C2V(gBlack_material), callback, arg);
}

br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow(br_vector3* pPosition) {
    tU8 cx;
    tU8 cz;
    tU8 x;
    tU8 z;
    tTrack_spec* track_spec;

    track_spec = &C2V(gProgram_state).track_spec;
    XZToColumnXZ(&cx, &cz, pPosition->v[0], pPosition->v[2], track_spec);
    C2V(gHighest_y_below) = BR_SCALAR_MIN;
    BrVector3Copy(&C2V(gY_picking_camera)->t.t.translate.t, pPosition);
    for (x = MAX(cx - 1, 0); x < MIN(cx + 2, track_spec->ncolumns_x); x++) {
        for (z = MAX(cz - 1, 0); z < MIN(cz + 2, track_spec->ncolumns_z); z++) {
            if (track_spec->columns[z][x].actor_0x0 != NULL) {
                DRScenePick2D(track_spec->columns[z][x].actor_0x0, C2V(gY_picking_camera), FindYVerticallyBelowCallBack, NULL);
            }
        }
    }
    return C2V(gHighest_y_below);
}
C2_HOOK_FUNCTION(0x004e4370, FindYVerticallyBelow)

br_scalar (C2_HOOK_FASTCALL * FindYVerticallyBelow2_original)(br_vector3* pCast_point);
br_scalar C2_HOOK_FASTCALL FindYVerticallyBelow2(br_vector3* pCast_point) {

#if 0//defined(C2_HOOKS_ENABLED)
    return FindYVerticallyBelow2_original(pCast_point);
#else
    br_scalar result;
    int number_of_attempts;
    br_vector3 cast_point;

    BrVector3Copy(&cast_point, pCast_point);
    for (number_of_attempts = 0; number_of_attempts <= 10; number_of_attempts += 1) {
        result = FindYVerticallyBelow(&cast_point);
        cast_point.v[1] += .2f;
        if (result >= -100.f) {
            return result;
        }
    }
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4600, FindYVerticallyBelow2, FindYVerticallyBelow2_original)

int C2_HOOK_CDECL FindHighestPolyCallBack__raycast(br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT, int pF, int pE, int pV, br_vector3* pPoint, br_vector2* pMap, void* pArg) {
    br_scalar the_y;

    the_y = pPoint->v[1];
    if (the_y > C2V(gCurrent_y)) {
        if (the_y < C2V(gLowest_y_above)) {
            C2V(gLowest_y_above) = the_y;
            C2V(gAbove_face_index) = pF;
            C2V(gAbove_model) = pModel;
        }
    } else {
        if (the_y > C2V(gHighest_y_below)) {
            C2V(gHighest_y_below) = the_y;
            C2V(gBelow_face_index) = pF;
            C2V(gBelow_model) = pModel;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004e4300, FindHighestPolyCallBack__raycast)

int C2_HOOK_CDECL FindHighestCallBack__raycast(br_actor* pActor, br_model* pModel, br_material* pMaterial, br_vector3* pRay_pos, br_vector3* pRay_dir, br_scalar pT_near, br_scalar pT_far, void* pArg) {

    if (C2V(gProgram_state).current_car.car_actor != pActor) {
        DRModelPick2D_raycast(pModel, pMaterial, pRay_pos, pRay_dir, pT_near, pT_far, FindHighestPolyCallBack__raycast, pArg);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004e3d50, FindHighestCallBack__raycast)

void C2_HOOK_FASTCALL FindBestY(br_vector3* pPosition, br_actor* pWorld, br_scalar pStarting_height, br_scalar* pNearest_y_above, br_scalar* pNearest_y_below, br_model** pNearest_above_model, br_model** pNearest_below_model, int* pNearest_above_face_index, int* pNearest_below_face_index) {

    C2V(gLowest_y_above) = 30000.0;
    C2V(gHighest_y_below) = -30000.0;
    C2V(gCurrent_y) = pPosition->v[1] + 1.192093e-5f;
    BrVector3Copy(&C2V(gY_picking_camera)->t.t.translate.t, pPosition);
    C2V(gY_picking_camera)->t.t.mat.m[3][1] += pStarting_height;
    DRScenePick2D(pWorld, C2V(gY_picking_camera), FindHighestCallBack__raycast, 0);
    *pNearest_y_above = C2V(gLowest_y_above);
    *pNearest_y_below = C2V(gHighest_y_below);
    *pNearest_above_model = C2V(gAbove_model);
    *pNearest_below_model = C2V(gBelow_model);
    *pNearest_above_face_index = C2V(gAbove_face_index);
    *pNearest_below_face_index = C2V(gBelow_face_index);
}
C2_HOOK_FUNCTION(0x004e3bc0, FindBestY)
