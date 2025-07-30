#include "car.h"

#include "compress.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "drone.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "powerups.h"
#include "racemem.h"
#include "raycast.h"
#include "replay.h"
#include "skidmark.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"
#include "brender/br_inline_funcs.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gOver_shoot, 0x0074a5f4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tCar_spec*, gActive_car_list, 139, 0x0074c7c0); /* FIXME: uncertain about array length */
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_cars_and_non_cars, 0x0079ef2c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_active_cars, 0x0074c9ec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFreeze_mechanics, 0x006793a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_simplification_level, 0x006793d8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNon_car_spec*, gActive_non_car_list, 99, 0x0079eda0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_active_non_cars, 0x006793dc);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMin_world_y, 0x00679360);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gUnknown_car_collision_info, 0x006793e4);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gAverage_grid_position, 0x00679268);

C2_HOOK_VARIABLE_IMPLEMENT(int, gTesting_car_for_sensible_place, 0x0067939c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074a5f8, 0x0074a5f8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tWorld_callbacks, gWorld_callbacks, 0x0065cf78, {
    ProcessForcesCallback,
    ProcessJointForcesCallback,
    NewFacesListCallback,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tPhysics_callbacks, gCar_physics_callbacks, 0x0058f6e0, {
    &C2V(gWorld_callbacks),
    APTCPreCollision,
    APTCPostCollision,
    APTCChangedObjects,
    APTCActiveHalted,
    APTCPassiveActivated,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFace_count, 0x006940b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFace_ref, gFace_list__car, 300, 0x00744820);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_mode, 0x00679294);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_frozen, 0x006792b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOpponent_viewing_mode, 0x006792bc);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gNet_player_to_view_index, 0x0058f5fc, -1);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gPanning_camera_angle, 0x00679304);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gZero_v__car, 0x0068b8d0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSave_camera, gSave_camera, 2, 0x006792c8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_scalar, gCamera_zoom, 0x00655f40, 0.2f);
C2_HOOK_VARIABLE_IMPLEMENT(tCamera_key_flags, gCamera_key_flags, 0x0079efa4);
C2_HOOK_VARIABLE_IMPLEMENT(undefined2, gUNK_006792f4, 0x006792f4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUNK_006792f8, 0x006792f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCamera_pos_before_collide, 0x006792e8);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gOld_yaw__car, 0x006792e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCamera_has_collided, 0x006793e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_angle, gOld_zoom, 0x006792e4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInTheSea, 0x00679364);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gWild_start, 0x006793a8);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gQuite_wild_end, 0x006793ac);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gQuite_wild_start, 0x006793b0);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gOn_me_wheels_start, 0x006793b4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWoz_upside_down_at_all, 0x006793b8);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gStop_opponents_moving, 0x00676854, 0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gSkid_tag, 2, 0x006793c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tCar_spec*, gLast_car_to_skid, 2, 0x006793c8);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gCar_to_view_original_v, 0x00679390);

void (C2_HOOK_FASTCALL * SetUpPanningCamera_original)(tCar_spec* c);
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {

#if defined(C2_HOOKS_ENABLED)
    SetUpPanningCamera_original(c);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004105f0, SetUpPanningCamera, SetUpPanningCamera_original)

int (C2_HOOK_FASTCALL * CollideCamera2_original)(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    CollideCamera2_original(car_pos, cam_pos, old_camera_pos, manual_move, collision_info);
#else
    int i;
    br_vector3 cam_hither_pos;
    br_vector3 car_coll_pos;
    br_vector3 dir;
    br_scalar ts;
    br_scalar dist;
    br_scalar hither;
    tBounds bnds;
    br_actor* actor;
    br_matrix34 mat;
    br_vector3 tv;
    tFace_ref face_list[6];
    br_scalar face_dots[6];
    int count_min_dots;
    int index_min_dots[3];
    int non_car_coll_index;
    int loop_done;
    int count;
    br_vector3 nor;
    br_vector3 delta;

    hither = ((br_camera*)C2V(gCamera)->type_data)->hither_z * 3.0f;
    C2V(gCamera_has_collided) = 0;
    non_car_coll_index = 0;
    loop_done = 0;
    for (;;) {
        br_vector3 new_cam_pos;
        br_vector3 tv2;
        br_vector3 p1;
        br_vector3 p2;
        br_vector3 nor_mod;
        br_material* material;
        br_scalar cos_coll;
        br_scalar delta_hither;
        br_scalar nor_factor;
        int i;

        BrVector3Sub(&delta, cam_pos, car_pos);
        dist = BrVector3Length(&delta);
        BrVector3Scale(&delta, &delta, 1.25f);
        ActorFindFace(car_pos, &delta, C2V(gTrack_actor), &nor, &ts, &material, &actor);
        if (ts >= 1.0) {
            BrVector3Copy(&new_cam_pos, cam_pos);
        } else {
            C2V(gCamera_has_collided) = 1;
            BrVector3Scale(&tv, &delta, ts);
            BrVector3Add(&new_cam_pos, car_pos, &tv);
        }
        BrVector3Normalise(&dir, &delta);

        BrVector3Scale(&tv2, &dir, -hither);
        BrVector3Add(&cam_hither_pos, &new_cam_pos, &tv2);
        BrVector3Add(&car_coll_pos, car_pos, &delta);

        for (i = 0; i < C2V(gNum_active_non_cars); i++) {
            tNon_car_spec* non_car = C2V(gActive_non_car_list)[i];

            if (non_car->flags & 0x80000) {
                br_scalar factor;
                br_vector3 temp_normal;

                DrMatrix34ApplyLPInverse(&p1, &cam_hither_pos, &non_car->collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal)) {
                    C2V(gCamera_has_collided) = 1;
                    DrMatrix34ApplyLPInverse(&p1, car_pos, &non_car->collision_info->actor->t.t.mat);
                    DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                    if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal) && factor < ts) {
                        BrVector3Copy(&nor, &temp_normal);
                        actor = NULL;
                        BrMatrix34ApplyP(&tv, &new_cam_pos, &non_car->collision_info->actor->t.t.mat);
                        BrVector3Copy(&new_cam_pos, &tv);
                    }
                }
            }
        }
        if (ts > 1.f) {
            break;
        }
        cos_coll = BrVector3Dot(&nor, &delta);
        if (cos_coll <= 0.f) {
            cos_coll = -cos_coll;
        } else {
            BrVector3Negate(&nor, &nor);
        }
        if (loop_done) {
            break;
        }
        delta_hither = hither + cos_coll / 1.25f - ts * cos_coll;
        if (delta_hither <= 0.f) {
            break;
        }
        BrVector3Copy(&nor_mod, &nor);
        nor_mod.v[1] += nor.v[1] >= 0.7f ? 3.f : -3.f;
        nor_factor = fabsf(nor.v[1]) * delta_hither / BrVector3Length(&nor_mod);
        BrVector3Scale(&tv, &nor_mod, nor_factor);
        BrVector3Accumulate(cam_pos, &tv);

        BrVector3Sub(&delta, cam_pos, car_pos);
        BrVector3Normalise(&delta, &delta);
        BrVector3Scale(&delta, &delta, dist);
        BrVector3Add(cam_pos, car_pos, &delta);
        loop_done = 1;
    }
    if (ts <= 1.f) {
        br_scalar dt;
        br_scalar l;

        C2V(gCamera_has_collided) = 1;
        if (actor != NULL && actor->identifier != NULL && actor->identifier[0] == '&') {
            br_vector3 tv;

            BrMatrix34ApplyV(&tv, &nor, &actor->t.t.mat);
            BrVector3Copy(&nor, &tv);
        }
        dt = -(hither / BrVector3Dot(&nor, &delta));
        ts -= dt;
        if (ts * dist < 1.f) {
            ts += dt - 0.001f;
            if (ts >= 1.f / dist) {
                ts = 1.f / dist;
            }
        }
        if (ts > .8f) {
            ts = .8f;
        }
        BrVector3Scale(&delta, &delta, ts);
        l = BrVector3LengthSquared(&delta);
        BrVector3Add(cam_pos, car_pos, &delta);
        if (sqrtf(l) < C2V(gMin_camera_car_distance) && !loop_done) {
            br_scalar a;
            br_scalar b;
            br_scalar discr;

            BrVector3Scale(&tv, &nor, -nor.v[1]);
            tv.v[1] += 1.f;
            if (C2V(gProgram_state).current_car.car_master_actor->t.t.mat.m[1][1] < 0.f) {
                BrVector3Negate(&tv, &tv);
            }
            a = BrVector3LengthSquared(&tv);
            b = BrVector3Dot(&tv, &delta);
            discr = REC2_SQR(b) - 4.f * a * (l - REC2_SQR(C2V(gMin_camera_car_distance)));
            if (discr >= 0.f && a != 0.f) {
                br_material* material;
                br_vector3 tv2;
                br_vector3 tv3;
                br_scalar f;
                br_scalar dot;

                f = (sqrtf(discr) - b) / (2 * a);
                BrVector3Scale(&tv, &tv, f);
                FindFace(cam_pos, &tv, &nor, &ts, &material);
                if (ts < 1.f) {
                    BrVector3Scale(&tv, &tv, ts);
                }
                BrVector3Copy(&tv2, &delta);
                tv2.v[1] = 0.f;
                BrVector3Normalise(&tv3, &tv2);
                BrVector3Accumulate(&delta, &tv);
                dot = BrVector3Dot(&tv3, &delta);
                if (dot < .03f && !C2V(gAction_replay_mode)) {
                    br_scalar dot2;

                    BrVector3Normalise(&tv, &tv);
                    dot2 = BrVector3Dot(&tv3, &tv);
                    if (dot2 < -.03f) {
                        a = (.03f - dot) / dot2;
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(&delta, &tv);
                    }
                }
            }
            BrVector3Add(cam_pos, car_pos, &delta);
        }
    }
    bnds.mat = &mat;
    BrMatrix34Identity(&mat);
    BrVector3Set(&tv, hither, hither, hither);
    BrVector3Sub(&bnds.original_bounds.min, cam_pos, &tv);
    BrVector3Add(&bnds.original_bounds.min, cam_pos, &tv);
    count = FindFacesInBox(&bnds, face_list, REC2_ASIZE(face_list), NULL);
    for (i = 0; i < count; i++) {
        tFace_ref* fr = &face_list[i];
        if ((fr->material->flags & BR_MATF_TWO_SIDED) && BrVector3Dot(&fr->normal, &delta) > 0.f) {
            BrVector3Negate(&fr->normal, &fr->normal);
        }
        BrVector3Sub(&tv, cam_pos, &fr->v[0]);
        face_dots[i] = BrVector3Dot(&tv, &fr->normal);
    }

    for (i = 0; i < REC2_ASIZE(index_min_dots); i++) {
        int j;
        float min_dot = 100.f;
        int index_min_dot;

        for (j = 0; j < count; j++) {
            if (face_dots[j] > 0.f && face_dots[j] < min_dot) {
                min_dot = face_dots[j];
                index_min_dot = j;
            }
        }
        if (min_dot == 100.f) {
            break;
        }
        index_min_dots[i] = index_min_dot;
        face_dots[index_min_dot] = -100.f;
    }
    count_min_dots = i;
    if (count_min_dots >= 1) {
        br_scalar d;

        BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[0]].v[0]);
        d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &tv);
        if (d < hither) {
            d = hither - d;
            BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, d);
            BrVector3Accumulate(cam_pos, &tv);
        }
        if (count_min_dots >= 2) {
            int o_i = 1;

            d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[1]].normal);
            if (d > .95f && count_min_dots >= 3) {
                o_i = 2;
                d = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[2]].normal);
            }
            if (d <= .95f) {
                br_vector3 tv;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither) {
                    br_scalar a;

                    a = BrVector3Dot(&face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, a);
                    BrVector3Sub(&face_list[index_min_dots[o_i]].normal, &face_list[index_min_dots[o_i]].normal, &tv);
                    BrVector3Scale(&tv, &face_list[index_min_dots[0]].normal, hither - d);
                    BrVector3Accumulate(cam_pos, &tv);
                }
            }
            if (count_min_dots >= 3) {
                br_vector3 tv;
                br_scalar d;

                BrVector3Sub(&tv, cam_pos, &face_list[index_min_dots[o_i]].v[0]);
                d = BrVector3Dot(&face_list[index_min_dots[o_i]].normal, &tv);
                if (d < hither && d >= 0.f) {
                    BrVector3Cross(&tv, &face_list[index_min_dots[0]].normal, &face_list[index_min_dots[o_i]].normal);
                    if (BrVector3Dot(&tv, &face_list[index_min_dots[2]].normal) < 0.f) {
                        BrVector3Negate(&tv, &tv);
                    }
                    if (d > .5f) {
                        br_scalar a;

                        a = (hither - d) / (BrVector3Dot(&face_list[index_min_dots[2]].normal, &tv));
                        BrVector3Scale(&tv, &tv, a);
                        BrVector3Accumulate(cam_pos, &tv);
                    }
                }
            }
        }
        if (collision_info != NULL && C2V(gAction_replay_camera_mode) != kActionReplayCameraMode_Manual) {
            for (;;) {
                br_vector3 p1;
                br_vector3 p2;
                br_vector3 new_cam_pos;
                br_vector3 temp_normal;
                br_vector3 pos_screen;
                int num_collisions;

                BrVector3Sub(&delta, cam_pos, car_pos);
                BrVector3Normalise(&delta, &delta);
                BrVector3Scale(&delta, &delta, hither);
                BrVector3Sub(&pos_screen, cam_pos, &delta);
                DrMatrix34ApplyLPInverse(&p1, cam_pos, &collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &pos_screen, &collision_info->actor->t.t.mat);
                num_collisions = ShapeRayCast(&p1, &p2, collision_info->shape, &new_cam_pos, &ts, &temp_normal);
                if (num_collisions == 0) {
                    break;
                }
                if (num_collisions > 0) {
                    BrMatrix34ApplyP(cam_pos, &new_cam_pos, &collision_info->actor->t.t.mat);
                }
                BrVector3Accumulate(cam_pos, &delta);
            }
        }
    }
    return loop_done;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004122b0, CollideCamera2, CollideCamera2_original)

void (C2_HOOK_FAKE_THISCALL * FlyCar_original)(tCar_spec* c, undefined4 pArg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    REC2_THISCALL_UNUSED(pArg2);

#if defined(C2_HOOKS_ENABLED)
    FlyCar_original(c, pArg2, dt);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f4f0, FlyCar, FlyCar_original)

float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar) {
    float rear_min;
    float front_min;
    float min_value;

    rear_min = (pCar->wpos[0].v[1] - pCar->susp_height[0]) / WORLD_SCALE;
    front_min = (pCar->wpos[2].v[1] - pCar->susp_height[1]) / WORLD_SCALE;
    min_value = pCar->collision_info->bb1.min.v[1];

    min_value = MIN(min_value, rear_min);
    min_value = MIN(min_value, front_min);
    return min_value;
}

void (C2_HOOK_FAKE_THISCALL * SetCarSuspGiveAndHeight_original)(tCar_spec* pCar, undefined4 pArg2, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height);
void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetCarSuspGiveAndHeight_original(pCar, pArg2, pFront_give_factor, pRear_give_factor, pDamping_factor, pExtra_front_height, pExtra_rear_height);
#else
    float ratio;
    float front_give;
    float rear_give;
    float damping;

#define UNK_SUSPENION_FACTOR 5.0f

    front_give = pCar->susp_give * pFront_give_factor * WORLD_SCALE;
    rear_give = pCar->steerable_suspension_give * pRear_give_factor * WORLD_SCALE;
    damping = pCar->damping * pDamping_factor;
    ratio = fabsf((pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) / (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2]));
    pCar->sk[0] = (pCar->collision_info->M / (ratio + 1.0f)) * UNK_SUSPENION_FACTOR / rear_give;
    pCar->sb[0] = (pCar->collision_info->M / (ratio + 1.0f)) * sqrtf(UNK_SUSPENION_FACTOR) / sqrtf(rear_give);
    ratio = 1.0f / ratio;
    pCar->sk[1] = (pCar->collision_info->M / (ratio + 1.0f)) * UNK_SUSPENION_FACTOR / front_give;
    pCar->sb[1] = (pCar->collision_info->M / (ratio + 1.0f)) * sqrtf(UNK_SUSPENION_FACTOR) / sqrtf(front_give);

    pCar->sb[0] *= damping;
    pCar->sb[1] *= damping;
    pCar->susp_height[0] = pCar->ride_height + rear_give + pExtra_rear_height;
    pCar->susp_height[1] = pCar->ride_height + front_give + pExtra_front_height;

    pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);

#undef UNK_SUSPENION_FACTOR
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041fc60, SetCarSuspGiveAndHeight, SetCarSuspGiveAndHeight_original)

int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3) {
    int r;

    if (!C2V(gProgram_state).racing) {
        return 1;
    }
    C2V(gTesting_car_for_sensible_place) = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        C2V(gList_collision_infos),
        pVec3,
        &C2V(gWorld_callbacks));
    C2V(gTesting_car_for_sensible_place) = 0;
    return r;
}
C2_HOOK_FUNCTION(0x0041fe50, TestForCarInSensiblePlace)

void (C2_HOOK_FASTCALL * PanningExternalCamera_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PanningExternalCamera_original(c, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004104b0, PanningExternalCamera, PanningExternalCamera_original)

int (C2_HOOK_FASTCALL * IncidentCam_original)(tCar_spec* c, tU32 pTime);
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return IncidentCam_original(c, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f790, IncidentCam, IncidentCam_original)

void (C2_HOOK_FASTCALL * ResetCarSpecialVolume_original)(tCollision_info* pCar);
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    ResetCarSpecialVolume_original(pCollision_info);
#else
    br_vector3 cast_v;
    br_vector3 norm;
    br_scalar t;
    int id_len;
    char* mat_id;
    tSpecial_volume* new_special_volume;
    br_material* material;

    new_special_volume = NULL;
    BrVector3Set(&cast_v, 0.f, 200.f, 0.f);
    DisablePlingMaterials();
    FindFace(&pCollision_info->actor->t.t.translate.t, &cast_v, &norm, &t, &material);
    EnablePlingMaterials();
    if (t < 100.0f && material != NULL) {
        mat_id = material->identifier;
        if (mat_id != NULL) {
            id_len = c2_strlen(mat_id);
            if (id_len != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                new_special_volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pCollision_info->auto_special_volume = new_special_volume;
    pCollision_info->water_depth_factor = 1.0f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ff530, ResetCarSpecialVolume, ResetCarSpecialVolume_original)

void C2_HOOK_FAKE_THISCALL ControlCar4(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    REC2_THISCALL_UNUSED(pArg2);

    if (c->keys.left) {
        if (c->turn_speed < 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed += .25f * dt;
        } else if ((c->curvature >= 0.f && c->collision_info->omega.v[1] >= -.001f) || c->turn_speed != 0.f) {
            c->turn_speed += 25.f * dt * (0.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v) )) * .25f;
        } else {
            c->turn_speed = 25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01f) {
                c->turn_speed -= .25f * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (c->keys.right) {
        if (c->turn_speed > 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed -= .25f * dt;
        } else if ((c->curvature <= 0.f && c->collision_info->omega.v[1] <= .001f) || c->turn_speed != 0.f) {
            c->turn_speed -= 25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v))) * .25f;
        } else {
            c->turn_speed = -25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01f) {
                c->turn_speed -= .25f * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.f;
    } else if (fabsf(c->turn_speed) < fabsf(2.f * dt * c->curvature) && c->curvature * c->turn_speed < 0.f) {
        c->turn_speed = -(2.f * dt * c->curvature);
    }
    c->curvature += c->turn_speed;
    if (c->joystick.left > 0) {
        c->curvature = (float)c->joystick.left * (float)c->joystick.left / 4294967300.f * c->maxcurve;
    } else if (c->joystick.right >= 0) {
        c->curvature = -((float)c->joystick.right * (float)c->joystick.right / 4294967300.f) * c->maxcurve;
    }
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}
C2_HOOK_FUNCTION(0x004175e0, ControlCar4)

void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime_difference) {
    static tU32 time_count;
    int i;
    br_vector3 r;

    time_count += pTime_difference;
    if (time_count < 5000) {
        return;
    }
    time_count = 4000;
    if (car->disabled) {
        return;
    }
    if (car->car_crush_spec != NULL && car->car_crush_spec->field_0x4b8) {
        return;
    }

    REC2_BUG_ON(REC2_ASIZE(car->oldd) != 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_height, 0x1218);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);

    for (i = 0; i < REC2_ASIZE(car->oldd); i++) {
        if (car->susp_height[i / 2] <= car->oldd[i]) {
            return;
        }
    }
    if ((car->collision_info->last_special_volume == NULL || car->collision_info->last_special_volume->gravity_multiplier == 1.f)
        && C2V(gCurrent_race).material_modifiers[car->material_index[0]].tyre_road_friction >= 0.1f
        && C2V(gCurrent_race).material_modifiers[car->material_index[1]].tyre_road_friction >= 0.1f
        && C2V(gCurrent_race).material_modifiers[car->material_index[2]].tyre_road_friction >= 0.1f
        && C2V(gCurrent_race).material_modifiers[car->material_index[3]].tyre_road_friction >= 0.1f
        && !car->field_0x195c
        && car->car_master_actor->t.t.mat.m[1][1] >= 0.8f) {

        REC2_BUG_ON(REC2_ASIZE(car->last_safe_positions) != 20);
        /* Only check last 5 positions */
        for (i = 0; i < 5; i++) {
            BrVector3Sub(&r, &car->car_master_actor->t.t.translate.t, (br_vector3*)car->last_safe_positions[i].m[3]);

            if (BrVector3LengthSquared(&r) < 8.4015961f) {
                return;
            }
        }
        for (i = REC2_ASIZE(car->last_safe_positions) - 1; i > 0; i--) {
            BrMatrix34Copy(&car->last_safe_positions[i], &car->last_safe_positions[i - 1]);
        }
        BrMatrix34Copy(&car->last_safe_positions[0], &car->car_master_actor->t.t.mat);
        time_count = 0;
    }
}

typedef void (C2_HOOK_FAKE_THISCALL * tControl_car_fn)(tCar_spec* pCar_spec, undefined4 pArg2, float pT);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tControl_car_fn, ControlCar, 6, 0x0058f660, {
    ControlCar1,
    ControlCar2,
    ControlCar3,
    ControlCar4,
    ControlCar5,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gControl__car, 0x0058f678, 3);

void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference) {
    static int last_key_down = 1;
    tCar_spec* car;
    tU32 time;
    br_vector3 minus_k;

    car = &C2V(gProgram_state).current_car;

    if (!car->keys.change_down) {
        last_key_down = 0;
    } else if (!last_key_down) {
        last_key_down = 1;

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, number_of_wheels_on_ground, 0x12e8);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, oldd, 0x1264);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_height, 0x1218);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, frame_collision_flag, 0x64);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, disable_move_rotate, 0xec);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, omega, 0x74);

        if (car->number_of_wheels_on_ground < 3
                && BrVector3LengthSquared(&car->collision_info->v) < .04f
                && car->oldd[0] == car->susp_height[0]
                && car->oldd[1] == car->susp_height[1]
                && (car->frame_collision_flag || car->collision_info->disable_move_rotate)) {
            br_vector3 tmp;
            br_vector3 delta_omega;

            BrVector3Copy(&tmp, (br_vector3*)car->collision_info->transform_matrix.m[1]);
            tmp.v[1] = 0.f;
            BrVector3Normalise(&tmp, &tmp);
            BrMatrix34TApplyV(&delta_omega, &tmp, &car->collision_info->transform_matrix);
            BrVector3Accumulate(&car->collision_info->omega, &delta_omega);
            car->collision_info->disable_move_rotate = 0;
        }
    }
    if (C2V(gNet_mode) != eNet_mode_none) {
        int i;

        for (i = 0; i < C2V(gNumber_of_net_players); i++) {

            if (i != C2V(gThis_net_player_index)) {
                C2V(ControlCar)[C2V(gControl__car)](C2V(gNet_players)[i].car REC2_THISCALL_EDX, 0.001f * pTime_difference);
            }
        }
    }
    if (C2V(gCar_flying)) {
        FlyCar(C2V(gCar_to_view) REC2_THISCALL_EDX, pTime_difference / 1000.f);
        return;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, end_steering_damage_effect, 0x154c);

    time = GetTotalTime();
    if (car->damage_units[eDamage_steering].damage_level > 40) {
        if (car->end_steering_damage_effect != 0) {
            if (time < car->end_steering_damage_effect || car->damage_units[eDamage_steering].damage_level == 99) {
                car->keys.left = car->false_key_left;
                car->keys.right = car->false_key_right;
            } else {
                car->end_steering_damage_effect = 0;
            }
        } else {
            float ts;

            ts = pTime_difference * (car->damage_units[eDamage_steering].damage_level - 40) * 0.0045f;
            if (PercentageChance((int)ts) && fabsf(car->collision_info->velocity_car_space.v[2]) > 1.f / (10000.f * WORLD_SCALE)) {
                if (car->keys.left || car->keys.right) {
                    car->false_key_left = !car->keys.left;
                    car->false_key_right = !car->keys.right;
                } else {
                    if (PercentageChance(50)) {
                        car->false_key_left = 1;
                    } else {
                        car->false_key_right = 1;
                    }
                }
                ts = (float)(25 * (car->damage_units[eDamage_steering].damage_level - 40));
                car->end_steering_damage_effect = (tU32)(time + FRandomBetween(0.0f, ts));
            }
        }
    }
    if (car->damage_units[eDamage_transmission].damage_level > 40) {
        if (car->end_trans_damage_effect != 0) {
            if (time < car->end_trans_damage_effect || car->damage_units[eDamage_transmission].damage_level == 99) {
                car->gear = 0.f;
                car->just_changed_gear = 1;
            } else {
                car->end_trans_damage_effect = 0;
            }
        } else {
            float ts;

            ts = pTime_difference * (car->damage_units[eDamage_transmission].damage_level - 40) * 0.006f;
            if (PercentageChance((int)ts)) {
                ts = (float)(50 * (car->damage_units[eDamage_transmission].damage_level - 40));
                car->end_trans_damage_effect = (tU32)(time + FRandomBetween(0.f, ts));
            }
        }
    }
    C2V(ControlCar)[C2V(gControl__car)](car REC2_THISCALL_EDX, pTime_difference / 1000.0f);
    RememberSafePosition(car, pTime_difference);
    if (C2V(gCamera_reset)) {
        BrVector3SetFloat(&minus_k, 0.0f, 0.0f, -1.0f);
        C2V(gCamera_sign) = 0;
        BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
    }
}
C2_HOOK_FUNCTION(0x00414cb0, ControlOurCar)

void (C2_HOOK_FASTCALL * SetInitialPosition_original)(tRace_info* pThe_race, int pCar_index, int pGrid_index);
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetInitialPosition_original(pThe_race, pCar_index, pGrid_index);
#else
    int place_on_grid;
    int i;
    int start_i;
    int j;
    br_actor* car_actor;
    br_angle initial_yaw;
    br_scalar nearest_y_above;
    br_scalar nearest_y_below;
    int below_face_index;
    int above_face_index;
    br_model* below_model;
    br_model* above_model;
    tCar_spec* car;
    br_vector3 grid_offset;
    br_vector3 dist;
    br_vector3 real_pos;
    br_matrix34 initial_yaw_matrix;

    initial_yaw = 0;
    car_actor = pThe_race->opponent_list[pCar_index].car_spec->car_master_actor;
    car = pThe_race->opponent_list[pCar_index].car_spec;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (C2V(gNet_mode) != eNet_mode_none && !C2V(gCurrent_net_game)->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < C2V(gNumber_of_net_players); j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.f) {
                        BrVector3Sub(&real_pos, &real_pos, &C2V(gInitial_position));
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.f) {
                        break;
                    }
                }
            }
            if (j == C2V(gNumber_of_net_players)) {
                BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[i].pos);
                initial_yaw = BrDegreeToAngle(pThe_race->net_starts[i].yaw);
                place_on_grid = 0;
            }
            i++;
            if (i == pThe_race->count_network_start_points) {
                i = 0;
            }
        } while (i != start_i);
    }
    if (C2V(gNet_mode) == eNet_mode_none && pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-pGrid_index].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-pGrid_index].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)(pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = 2.0f * (br_scalar)(pGrid_index / 2) + 0.4f * (br_scalar)(pGrid_index % 2);
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (C2V(gTrack_actor) != NULL) {
        FindBestY(
            &car_actor->t.t.translate.t,
            C2V(gTrack_actor),
            10.0f,
            &nearest_y_above,
            &nearest_y_below,
            &above_model,
            &below_model,
            &above_face_index,
            &below_face_index);
        if (nearest_y_above != 30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_above;
        } else if (nearest_y_below != -30000.0f) {
            car_actor->t.t.translate.t.v[1] = nearest_y_below;
        } else {
            car_actor->t.t.translate.t.v[1] = 0.0f;
        }
    }
    BrMatrix34PreRotateY(&car_actor->t.t.mat, initial_yaw);
    if (C2V(gNet_mode) != eNet_mode_none) {
        tCompressed_matrix3 compressed;
        int inactive;

        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(
            &C2V(gNet_players)[pThe_race->opponent_list[pCar_index].net_player_index].initial_position,
            &car->car_master_actor->t.t.mat);
    }
    if (C2V(gNet_mode) != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00414510, SetInitialPosition, SetInitialPosition_original)

void (C2_HOOK_FASTCALL * InitialiseCar2_original)(tCar_spec* pCar, int pClear_disabled_flag);
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitialiseCar2_original(pCar, pClear_disabled_flag);
#else
    int index;
    int j;
    br_actor* car_actor;
    br_matrix34 safe_position;
    tNet_game_player_info* net_player;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x261, 0x261);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, message_time, 0x268);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x49c, 0x49c);

    PossibleService();
    if (pCar->disabled && pClear_disabled_flag) {
        if (C2V(gNet_mode) == eNet_mode_none) {
            EnableCar(pCar);
        } else {
            net_player = NetPlayerFromCar(pCar);
            if (net_player->player_status == ePlayer_status_ready || net_player->player_status == ePlayer_status_racing) {
                EnableCar(pCar);
            }
        }
    }
    car_actor = pCar->car_master_actor;
    InitCarSkidStuff(pCar);
    pCar->car_model_actor->render_style = BR_RSTYLE_DEFAULT;
    SwitchCarModels(pCar, 0);
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    pCar->field_0xc8 = NULL;
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        ResetRecoveryVouchers();
    }
    BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
    pCar->curvature = 0.f;
    pCar->field_0x1260 = 0.f;
    BrMatrix34Copy(&safe_position, &car_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &C2V(gInitial_position));
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < REC2_ASIZE(pCar->oldd); j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0.f;
    pCar->revs = 0.f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)car_actor->t.t.mat.m[2]);
    for (j = 0; j < REC2_ASIZE(pCar->last_safe_positions); j++) {
        BrMatrix34Copy(&pCar->last_safe_positions[j], &safe_position);
    }
    pCar->collision_info->field_0x261 = 0;
    pCar->collision_info->message_time = 0;
    pCar->dt = -1.f;
    pCar->collision_info->field_0x49c = 1;
    pCar->time_to_recover = 0;
    pCar->repair_time  = 0;
    pCar->collision_info->water_d  = 10000.f;

    switch (pCar->driver) {
    case eDriver_oppo:
        index = 0;
        for (j = 0; j < C2V(gCurrent_race).number_of_racers; j++) {
            if (C2V(gCurrent_race).opponent_list[j].car_spec != NULL
                    && C2V(gCurrent_race).opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (C2V(gCurrent_race).opponent_list[j].car_spec == pCar) {
                    pCar->car_ID = 0x200 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_net_human:
        index = 0;
        for (j = 0; j < C2V(gCurrent_race).number_of_racers; j++) {
            if (C2V(gCurrent_race).opponent_list[j].car_spec != NULL
                    && C2V(gCurrent_race).opponent_list[j].car_spec->driver == eDriver_net_human) {
                if (C2V(gCurrent_race).opponent_list[j].car_spec == pCar) {
                    pCar->car_ID = 0x100 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_local_human:
        pCar->car_ID = 0;
        break;
    default:
        c2_abort();
        break;
    }
    PossibleService();
    pCar->collision_info->box_face_ref = C2V(gFace_num__car) - 2;
    pCar->collision_info->box_face_end = 0;
    pCar->collision_info->box_face_start = 0;
    pCar->collision_info->disable_move_rotate = 0;
    pCar->end_steering_damage_effect = 0;
    pCar->end_trans_damage_effect = 0;
    pCar->wheel_dam_offset[0] = 0.f;
    pCar->wheel_dam_offset[1] = 0.f;
    pCar->wheel_dam_offset[2] = 0.f;
    pCar->wheel_dam_offset[3] = 0.f;
    pCar->shadow_intersection_flags = 0;
    pCar->underwater_ability = 0;

    if (C2V(gNet_mode) == eNet_mode_none) {
        net_player = NULL;
    } else {
        net_player = NetPlayerFromCar(pCar);
    }
    if (net_player != NULL && net_player->field_0x80) {
        pCar->invulnerable_no_crushage = 1;
        pCar->invulnerable_no_damage = 1;
        pCar->invulnerable_no_wastage = 1;
    } else {
        pCar->invulnerable_no_crushage = 0;
        pCar->invulnerable_no_damage = 0;
        pCar->invulnerable_no_wastage = 0;
    }
    pCar->wall_climber_mode = 0;
    pCar->grip_multiplier = 1.f;
    pCar->damage_multiplier = 1.f;
    pCar->field_0x4c8 = 1.f;
    pCar->field_0x4d4 = 1.f;
    pCar->bounce_rate = 0.f;
    pCar->bounce_amount = 0.f;
    pCar->knackered = 0;
    pCar->collision_info->last_special_volume = NULL;
    pCar->collision_info->auto_special_volume = NULL;
    RemoveFromCloakingList(pCar);
    TurnOffCloaking(NULL, pCar);
    if (pCar != NULL && pCar->driver != eDriver_local_human) {
        pCar->joystick.left = -1;
        pCar->joystick.right = -1;
    }
    TotallyRepairACar(pCar);
    SetCarSuspGiveAndHeight(pCar REC2_THISCALL_EDX, 1.f, 1.f, 1.f, 0.f, 0.f);
    for (j = 0; j < REC2_ASIZE(pCar->powerups); j++) {
        pCar->powerups[j] = 0;
    }
    if (C2V(gNet_mode) != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < REC2_ASIZE(pCar->power_up_levels); j++) {
            if (C2V(gNet_mode) == eNet_mode_none) {
                pCar->power_up_levels[j] = C2V(gInitial_APO)[j].initial[C2V(gProgram_state).skill_level];
            } else {
                pCar->power_up_levels[j] = C2V(gInitial_APO)[j].initial_network[C2V(gProgram_state).skill_level];
            }
            if (C2V(gNet_mode) == eNet_mode_none) {
                pCar->power_up_slots[j] = C2V(gInitial_APO_potential)[j].initial[C2V(gProgram_state).skill_level];
            } else {
                pCar->power_up_slots[j] = C2V(gInitial_APO_potential)[j].initial_network[C2V(gProgram_state).skill_level];
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00413f70, InitialiseCar2, InitialiseCar2_original)

void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {

    InitialiseCar2(pCar, 1);
}
C2_HOOK_FUNCTION(0x00414400, InitialiseCar)

void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race) {
    int i;
    int cat;
    int car_count;
    tCar_spec* car;
    br_bounds bnds;

    BrVector3Copy(&C2V(gProgram_state).initial_position, &pThe_race->initial_position);
    C2V(gProgram_state).initial_yaw = pThe_race->initial_yaw;
    BrActorToBounds(&bnds, C2V(gProgram_state).track_spec.the_actor);
    C2V(gMin_world_y) = bnds.min.v[1];
    C2V(gNum_active_non_cars) = 0;
    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            PossibleService();
            if (cat == eVehicle_self) {
                car = &C2V(gProgram_state).current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                InitialiseCar(car);
            }
        }
    }
    C2V(gCamera_yaw) = 0;
    if (C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Manual) {
        C2V(gCamera_type) = 0;
        C2V(gAction_replay_camera_mode) = kActionReplayCameraMode_Standard;
    }
    InitialiseExternalCamera();
    if (C2V(gUnknown_car_collision_info) != NULL && C2V(gProgram_state).current_car.collision_info != C2V(gUnknown_car_collision_info)->parent) {
        AddCollisionInfoChild(C2V(gProgram_state).current_car.collision_info, C2V(gUnknown_car_collision_info));
    }
}
C2_HOOK_FUNCTION(0x00414410, InitialiseCarsEtc)

void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, number_of_racers, 0x90);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, opponent_list, 0xce4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tOpp_spec, ranking, 0x8);
    C2_HOOK_BUG_ON(sizeof(tOpp_spec) != 16);

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}
C2_HOOK_FUNCTION(0x004148d0, SetInitialPositions)


void (C2_HOOK_FASTCALL * InitialiseExternalCamera_original)(void);
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitialiseExternalCamera_original();
#else
    br_scalar ts;
    tCar_spec* c;
    br_angle yaw;

    c = C2V(gCar_to_view);
    if (!C2V(gProgram_state).racing) {
        c = &C2V(gProgram_state).current_car;
    }
    C2V(gCamera_height) = c->pos.v[1];
    BrVector3Set(&C2V(gView_direction), c->direction.v[0], 0.0f, c->direction.v[2]);
    BrVector3Normalise(&C2V(gView_direction), &C2V(gView_direction));
    ts = -BrVector3Dot(&C2V(gView_direction), (br_vector3*)c->car_master_actor->t.t.mat.m[2]);
    C2V(gCamera_sign) = ts < 0;
    C2V(gCamera_mode) = 0;
    if (C2V(gCamera_sign)) {
        yaw = -C2V(gCamera_yaw);
    } else {
        yaw = C2V(gCamera_yaw);
    }
    DrVector3RotateY(&C2V(gView_direction), yaw);
    C2V(gMin_camera_car_distance) = 0.6f;
    C2V(gCamera_frozen) = 0;
    C2V(gCamera_mode) = -2;
    if (C2V(gCountdown) && (C2V(gNet_mode) == eNet_mode_none || C2V(gCurrent_net_game)->options.grid_start) && C2V(gCountdown) > 4) {
        C2V(gCamera_height) += 10.f;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00413580, InitialiseExternalCamera, InitialiseExternalCamera_original)

void (C2_HOOK_FASTCALL * MungeCarGraphics_original)(tU32 pFrame_period);
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    MungeCarGraphics_original(pFrame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041e660, MungeCarGraphics, MungeCarGraphics_original)

void (C2_HOOK_FASTCALL * DoLODCarModels_original)(void);
void C2_HOOK_FASTCALL DoLODCarModels(void) {

#if defined(C2_HOOKS_ENABLED)
    DoLODCarModels_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f110, DoLODCarModels, DoLODCarModels_original)

void (C2_HOOK_FASTCALL * DoComplexCarModels_original)(void);
void C2_HOOK_FASTCALL DoComplexCarModels(void) {

#if defined(C2_HOOKS_ENABLED)
    DoComplexCarModels_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f2a0, DoComplexCarModels, DoComplexCarModels_original)

void (C2_HOOK_FASTCALL * ResetCarScreens_original)(void);
void C2_HOOK_FASTCALL ResetCarScreens(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetCarScreens_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f300, ResetCarScreens, ResetCarScreens_original)

void (C2_HOOK_FASTCALL * CameraBugFix_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    CameraBugFix_original(c, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f760, CameraBugFix, CameraBugFix_original)

void (C2_HOOK_FASTCALL * MungeSomeOtherCarGraphics_original)(void);
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeSomeOtherCarGraphics_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041e5a0, MungeSomeOtherCarGraphics, MungeSomeOtherCarGraphics_original)

void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {

    BrVector3SetFloat(&C2V(gAverage_grid_position), 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&C2V(gAverage_grid_position), &C2V(gProgram_state).current_car.pos);
    } else {
        int i;
        br_scalar total_cars;

        total_cars = 0.0f;
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            tCar_spec* car;

            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&C2V(gAverage_grid_position), &car->pos);
            total_cars += 1.0f;
        }
        BrVector3InvScale(&C2V(gAverage_grid_position), &C2V(gAverage_grid_position), total_cars);
    }
}
C2_HOOK_FUNCTION(0x00413780, GetAverageGridPosition)

int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref == C2V(gFace_num__car)
        || (pCar->collision_info->box_face_ref == C2V(gFace_num__car) - 1 && C2V(gFace_count) < pCar->collision_info->box_face_start)) {
        *pFace_refs = &C2V(gFace_list__car)[pCar->collision_info->box_face_start];
        return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00420880, GetPrecalculatedFacesUnderCar)

int (C2_HOOK_FASTCALL * ProcessForcesCallback_original)(void* arg1, float* arg2, int arg3);
int C2_HOOK_FASTCALL ProcessForcesCallback(void* arg1, float* arg2, int arg3) {

#if defined(C2_HOOKS_ENABLED)
    return ProcessForcesCallback_original(arg1, arg2, arg3);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041c1b0, ProcessForcesCallback, ProcessForcesCallback_original)

int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) {
    return 0;
}
C2_HOOK_FUNCTION(0x0041e310, ProcessJointForcesCallback)

void (C2_HOOK_FASTCALL * NewFacesListCallback_original)(tCollision_info* pCollision, undefined4 *arg2);
void C2_HOOK_FASTCALL NewFacesListCallback(tCollision_info* pCollision, undefined4 *arg2) {

#if defined(C2_HOOKS_ENABLED)
    NewFacesListCallback_original(pCollision, arg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00414910, NewFacesListCallback, NewFacesListCallback_original)

tCar_spec* (C2_HOOK_FASTCALL * DoPullActorFromWorld_original)(br_actor* actor);
tCar_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor) {

#if defined(C2_HOOKS_ENABLED)
    return DoPullActorFromWorld_original(actor);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041ff20, DoPullActorFromWorld, DoPullActorFromWorld_original)

tCar_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor) {

    if (!C2V(gINT_0074a5f8) && !C2V(gTesting_car_for_sensible_place) == 0) {
        return NULL;
    }
    return DoPullActorFromWorld(actor);
}
C2_HOOK_FUNCTION(0x0041ff00, PullActorFromWorld)

float (C2_HOOK_FASTCALL * GetFrictionFromFace_original)(void *arg1);
float C2_HOOK_FASTCALL GetFrictionFromFace(void *arg1) {

#if defined(C2_HOOKS_ENABLED)
    return GetFrictionFromFace_original(arg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5970, GetFrictionFromFace, GetFrictionFromFace_original)

void (C2_HOOK_FAKE_THISCALL * ControlCar1_original)(tCar_spec* c, undefined4 arg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL ControlCar1(tCar_spec* c, undefined4 arg2, br_scalar dt) {

#if defined(C2_HOOKS_ENABLED)
    ControlCar1_original(c, arg2, dt);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00417de0, ControlCar1, ControlCar1_original)

void (C2_HOOK_FAKE_THISCALL * ControlCar2_original)(tCar_spec* c, undefined4 arg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* c, undefined4 arg2, br_scalar dt) {

#if defined(C2_HOOKS_ENABLED)
    ControlCar2_original(c, arg2, dt);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00417180, ControlCar2, ControlCar2_original)

void (C2_HOOK_FAKE_THISCALL * ControlCar3_original)(tCar_spec* c, undefined4 arg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* c, undefined4 arg2, br_scalar dt) {

#if defined(C2_HOOKS_ENABLED)
    ControlCar3_original(c, arg2, dt);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004173b0, ControlCar3, ControlCar3_original)

void (C2_HOOK_FAKE_THISCALL * ControlCar5_original)(tCar_spec* c, undefined4 arg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* c, undefined4 arg2, br_scalar dt) {

#if defined(C2_HOOKS_ENABLED)
    ControlCar5_original(c, arg2, dt);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00417a20, ControlCar5, ControlCar5_original)

void C2_HOOK_FASTCALL DrVector3RotateY(br_vector3* v, br_angle t) {
    br_scalar c;
    br_scalar s;
    br_scalar ts;

    c = cosf(BrAngleToRadian(t));
    s = sinf(BrAngleToRadian(t));
    ts = v->v[0] * c + v->v[2] * s;
    v->v[2] = v->v[2] * c - v->v[0] * s;
    v->v[0] = ts;
}

intptr_t C2_HOOK_CDECL ActorFunks(br_actor* pActor, void* pContext) {
    tFunk_index_cbfn* funk_index_callback = pContext;
    tUser_crush_data *user_crush_data = pActor->user;
    int i;

    if (user_crush_data == NULL || user_crush_data->crush_data == NULL || user_crush_data->crush_data->smashables == NULL) {
        return 0;
    }
    for (i = 0; i < user_crush_data->crush_data->count_smashables; i++) {
        int funk_index = user_crush_data->crush_data->smashables[i].funk;
        if (funk_index >= 0) {
            funk_index_callback(funk_index);
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004f8dc0, ActorFunks)

void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index) {

    C2V(gFunkotronics_array)[pFunk_index].flags &= ~0x2;
}
C2_HOOK_FUNCTION(0x0047b2e0, MasterEnableFunkotronic)

void (C2_HOOK_FASTCALL * MasterEnableCarFunks_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    MasterEnableCarFunks_original(pCar_spec);
#else
#if 0
    int i;
#endif

    DRActorEnumRecurse(pCar_spec->car_model_actor, ActorFunks, MasterEnableFunkotronic);

    NOT_IMPLEMENTED();
#if 0
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x2b0; i++) {
        ActorFunks(pCar_spec->car_crush_spec->field_0x274[i].actor, MasterEnableFunkotronic);
    }

    for (i = 0; i < pCar_spec->car_crush_spec->count_of_field_0x2b0; i++) {
        ActorFunks(pCar_spec->car_crush_spec->field_0x2b4[0].actor, MasterEnableFunkotronic);
    }
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f8e10, MasterEnableCarFunks, MasterEnableCarFunks_original)

int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != (br_pixelmap*)pMaterial->user) {
        pMaterial->colour_map = (br_pixelmap*)pMaterial->user;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004f9760, RestorePixelmap)

void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {

    MasterEnableCarFunks(pCar_spec);
    ForEveryCarMaterial(pCar_spec, RestorePixelmap, 1);
}
C2_HOOK_FUNCTION(0x004f9620, RestoreCarPixelmaps)

int C2_HOOK_FASTCALL TestForNan(float* f) {

    if (f == NULL) {
        return 0;
    }
    return isnan(*f);
}
C2_HOOK_FUNCTION(0x005160f0, TestForNan)

void C2_HOOK_FASTCALL CheckDisablePlingMaterials(tCar_spec* pCar) {
    br_matrix34* mat;
    br_scalar height;
    int i;

    height = 0.f;
    if (pCar->collision_info->water_d == 10000.f) {
        DisablePlingMaterials();
    } else {
        mat = &pCar->car_master_actor->t.t.mat;
        for (i = 0; i < 3; i++) {
            if (mat->m[i][1] > 0.f) {
                height += pCar->collision_info->bb2.max.v[i] * mat->m[i][1];
            } else {
                height += pCar->collision_info->bb2.min.v[i] * mat->m[i][1];
            }
        }
        if (mat->m[3][1] / WORLD_SCALE + height < pCar->collision_info->water_d) {
            DisablePlingMaterials();
        }
    }
}
C2_HOOK_FUNCTION(0x0040f510, CheckDisablePlingMaterials)

void (C2_HOOK_FASTCALL * PositionCarMountedCamera_original)(tCar_spec* pCar, tU32 pTime);
void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PositionCarMountedCamera_original(pCar, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f590, PositionCarMountedCamera, PositionCarMountedCamera_original)

tCar_spec* (C2_HOOK_FASTCALL * GetRaceLeader_original)(void);
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetRaceLeader_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f4a0, GetRaceLeader, GetRaceLeader_original)

void C2_HOOK_FASTCALL CheckCameraHither(void) {
    br_camera *cam;
    static C2_HOOK_VARIABLE_IMPLEMENT(int, old_hither, 0x0067931c);

    cam = C2V(gCamera)->type_data;
    if (TestForNan(&cam->hither_z)) {
        cam->hither_z = (float)C2V(old_hither);
    }
    C2V(old_hither) = (int)cam->hither_z;
}

void C2_HOOK_FASTCALL AmIGettingBoredWatchingCameraSpin(void) {
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, time_of_death, 0x00679298);
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, headup_timer, 0x00679320);
    char s[256];

    if (C2V(gNet_mode) != eNet_mode_none
            && (C2V(gCurrent_net_game)->type == eNet_game_type_4
                    || C2V(gCurrent_net_game)->type == eNet_game_type_fight_to_death)) {
        if (!C2V(gRace_finished)) {
            C2V(time_of_death) = 0;
            C2V(gOpponent_viewing_mode) = 0;
        } else if (C2V(time_of_death) == 0) {
            C2V(time_of_death) = GetRaceTime();
        } else if (GetRaceTime() >= C2V(time_of_death) + 10000) {
            if (C2V(gOpponent_viewing_mode) == 0) {
                C2V(gOpponent_viewing_mode) = 1;
                C2V(gNet_player_to_view_index) = -2;
                ViewNetPlayer();
            }
            if (C2V(gNet_player_to_view_index) >= C2V(gNumber_of_net_players)) {
                C2V(gNet_player_to_view_index) = -2;
                ViewNetPlayer();
            }
            if (C2V(gNet_player_to_view_index) < 0 && C2V(gCar_to_view) != GetRaceLeader()) {
                C2V(gNet_player_to_view_index) = -2;
                ViewNetPlayer();
            }
            if ((GetRaceTime() > C2V(headup_timer) + 1000 || C2V(headup_timer) > GetRaceTime()) && C2V(gRace_over_reason) == eRace_not_over_yet) {
                c2_strcpy(s, GetMiscString(eMiscString_watching));
                c2_strcat(s, " ");
                if (C2V(gNet_player_to_view_index) >= 0) {
                    c2_strcat(s, C2V(gNet_players)[C2V(gNet_player_to_view_index)].player_name);
                } else {
                    c2_strcat(s, GetMiscString(eMiscString_race_leader));
                }
                C2V(headup_timer) = GetRaceTime();
                NewTextHeadupSlot(6, 0, 500, -4, s);
            }
        }
    }
}

void C2_HOOK_FASTCALL SaveCameraPosition(int i) {

    if (C2V(gSave_camera)[i].saved != 1) {
        C2V(gSave_camera)[i].zoom = C2V(gCamera_zoom);
        C2V(gSave_camera)[i].yaw = C2V(gCamera_yaw);
        C2V(gSave_camera)[i].saved = 1;
    }
}
C2_HOOK_FUNCTION(0x00410be0, SaveCameraPosition)

void C2_HOOK_FASTCALL RestoreCameraPosition(int i) {

    if (C2V(gSave_camera)[i].saved != 0) {
        C2V(gCamera_zoom) = C2V(gSave_camera)[i].zoom;
        C2V(gCamera_yaw) = C2V(gSave_camera)[i].yaw;
        C2V(gSave_camera)[i].saved = 0;
    }
}
C2_HOOK_FUNCTION(0x00410c20, RestoreCameraPosition)

void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags *pCamera_controls, tU32 pTime_difference) {
    int flag;
    int swirl_mode;
    int up_and_down_mode;
    int going_down;
    static C2_HOOK_VARIABLE_IMPLEMENT(int, last_swirl_mode, 0x0067930c);

    flag = 0;
    swirl_mode = !GetRuntimeVariable(99) && C2V(gRace_finished) && !C2V(gAction_replay_mode) && (C2V(gCar_to_view) == &C2V(gProgram_state).current_car || C2V(gCar_to_view)->knackered);
    up_and_down_mode = swirl_mode && !C2V(gCamera_has_collided);
    going_down = C2V(gCamera_zoom) > 1.0;
    if (C2V(last_swirl_mode) != swirl_mode) {
        if (swirl_mode) {
            SaveCameraPosition(0);
        } else {
            RestoreCameraPosition(0);
        }
        C2V(last_swirl_mode) = swirl_mode;
    }
    if (C2V(gMap_view) != 2 && !C2V(gProgram_state).cockpit_on && (!C2V(gAction_replay_mode) || C2V(gAction_replay_camera_mode) < kActionReplayCameraMode_Panning)) {
        if (pCamera_controls->field_0x0_bit2 || (up_and_down_mode && going_down)) {
            C2V(gCamera_zoom) += (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (C2V(gCamera_zoom) > 2.f) {
                C2V(gCamera_zoom) = 2.f;
            }
            if (up_and_down_mode && C2V(gCamera_zoom) > 1.f) {
                C2V(gCamera_zoom) = 1.f;
            }
        }
        if (pCamera_controls->field_0x0_bit1 || (up_and_down_mode && !going_down)) {
            float zoom;

            C2V(gCamera_zoom) -= (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Peds) {
                zoom = 0.001f;
            } else {
                zoom = 0.1f;
            }
            if (C2V(gCamera_zoom) < zoom) {
                C2V(gCamera_zoom) = zoom;
                if (up_and_down_mode) {
                    if (C2V(gCamera_zoom) < 1.0f) {
                        C2V(gCamera_zoom) = 1.0f;
                    }
                }
            }
        }
        if (swirl_mode && C2V(gProgram_state).current_car.speedo_speed < 0.001449275362318841) {
            pCamera_controls->field_0x0_bit4 = 0;
            pCamera_controls->field_0x0_bit3 = 1;
        }

        if (C2V(gCamera_sign) ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!C2V(gCamera_reset)) {
                C2V(gCamera_yaw) += BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            flag = 1;
        }
        if (C2V(gCamera_sign) ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!C2V(gCamera_reset)) {
                C2V(gCamera_yaw) -= BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            if (flag) {
                C2V(gCamera_yaw) = 0;
                C2V(gCamera_reset) = 1;
            }
        } else {
            if (!flag) {
                C2V(gCamera_reset) = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) {
    br_angle yaw;
    br_angle theta;
    static C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, move_with_wheels, 0x0058f638, 1);

    if (pCar != NULL && pCar->speed <= 0.0001f && !C2V(gCamera_mode)) {
        if (pManual_swing) {
            if (C2V(gCamera_yaw) > BrDegreeToAngle(180)) {
                yaw = C2V(gCamera_yaw) - BrDegreeToAngle(180);
            } else {
                yaw = C2V(gCamera_yaw);
            }
            if (yaw > BrDegreeToAngle(45) && yaw < BrDegreeToAngle(135)) {
                if (C2V(move_with_wheels)) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    C2V(gCamera_yaw) += (-2 * C2V(gCamera_sign) + 1) * theta;
                    C2V(move_with_wheels) = 0;
                }
            } else {
                if (!C2V(move_with_wheels)) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    C2V(gCamera_yaw) -= (-2 * C2V(gCamera_sign) + 1) * theta;
                    C2V(move_with_wheels) = 1;
                }
            }
        }
        if (C2V(move_with_wheels)) {
            if (!C2V(gCar_flying) && C2V(gAction_replay_camera_mode) != kActionReplayCameraMode_Rigid) {
                theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                DrVector3RotateY(pDir, theta);
            }
        }
    }
}

void (C2_HOOK_FASTCALL * GeneralisedPositionExternalCamera_original)(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference);
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference) {

#if 0//defined(C2_HOOKS_ENABLED)
    GeneralisedPositionExternalCamera_original(pCar, pMat, pPos, pSpeed, pSpeedo_speed, pDirection, pOmega, pTime);
#else
    br_vector3 old_camera_pos;
    br_matrix34* m1;
    int swoop;

    m1 = &C2V(gCamera)->t.t.mat;
    swoop = C2V(gCountdown) && C2V(gCamera_height) > pPos->v[1] + 0.001f;
    BrVector3Copy(&old_camera_pos, &C2V(gCamera)->t.t.translate.t);
    if (!C2V(gProgram_state).cockpit_on) {
        int manual_swing;
        float d;
        float l;
        float height_inc;
        float time;

        DoCameraControls(&C2V(gCamera_key_flags), pTime_difference);
        manual_swing = C2V(gCamera_key_flags).field_0x0_bit4 || C2V(gCamera_key_flags).field_0x0_bit3 || swoop;
        if (swoop) {
            C2V(gCamera_yaw) = 0;
        }
        if (fabsf(pSpeedo_speed) > 0.0006f && C2V(gCamera_mode) > 0) {
            C2V(gCamera_mode) = -1;
            if (C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Standard) {
                C2V(gCamera_sign) = 0;
            } else if (BrVector3Dot((br_vector3*)pMat->m[2], pDirection) <= 0.f) {
                C2V(gCamera_sign) = 0;
            } else {
                C2V(gCamera_sign) = 1;
            }
        }
        if (pCar != NULL && pCar->frame_collision_flag && C2V(gCamera_mode) != -2
                && (!C2V(gAction_replay_mode) || ARReplayForwards())) {
            C2V(gCamera_mode) = 1;
        }
        if (C2V(gCar_flying) || C2V(gCamera_reset)
                || C2V(gCamera_mode) == -2 || C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Rigid) {
            C2V(gCamera_mode) = 0;
        }
        d = sqrtf(C2V(gCamera_zoom)) + 4.f / WORLD_SCALE;
        if (!C2V(gCamera_mode) || C2V(gCamera_mode) == -1) {
            br_vector3 vn;
            br_vector3 a;

            if (C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Rigid) {
                BrVector3Negate(&vn, (br_vector3 *) pMat->m[2]);
            } else {
                BrVector3Copy(&vn, pDirection);
            }
            MoveWithWheels(pCar, &vn, manual_swing);
            vn.v[1] = 0.0f;
            BrVector3Normalise(&vn, &vn);
            vn.v[1] = 0.0f;
            if (C2V(gCar_flying) || C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Rigid) {
                C2V(gCamera_sign) = 0;
            }
            if (!C2V(gAction_replay_mode) || !ARReplayIsReallyPaused()) {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, pTime_difference);
            } else {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, 0);
            }
            BrVector3Scale(&a, &vn, d);
            BrVector3Sub(&C2V(gCamera)->t.t.translate.t, pPos, &a);
            BrVector3Copy(&C2V(gView_direction), &vn);
        } else {
            C2V(gUNK_006792f4) = 0;
            C2V(gUNK_006792f8) = 0;
        }
        if (C2V(gCamera_mode) == 1) {
            br_vector3 vn;
            br_vector3 a;
            br_scalar dist;
            br_scalar l;

            BrVector3Sub(&a, pPos, &old_camera_pos);
            BrVector3Copy(&old_camera_pos, &C2V(gCamera_pos_before_collide));
            a.v[1] = 0.0f;
            if (manual_swing) {
                DrVector3RotateY(&a, (C2V(gCamera_sign) == 0 ? 1 : -1) * (C2V(gCamera_yaw) - C2V(gOld_yaw__car)));
                C2V(gCamera_yaw) = C2V(gOld_yaw__car);
            }
            BrVector3Normalise(&vn, &a);
            if (C2V(gAction_replay_camera_mode) != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &C2V(gView_direction))) {
                BrVector3Copy(&C2V(gView_direction), &vn);
            }
            BrVector3Scale(&vn, &vn, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime_difference / 1000.0f * (dist + 1.0f) / dist;
            if (C2V(gAction_replay_camera_mode) != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add(&C2V(gCamera)->t.t.translate.t, pPos, &vn);
        }

        height_inc = C2V(gCamera_zoom) * C2V(gCamera_zoom) + 0.3f;
        time = pTime_difference / 1000.f;
        if (!C2V(gCamera_frozen) && (!C2V(gAction_replay_mode) || !ARReplayIsReallyPaused())) {
            if (pTime_difference >= 5000) {
                C2V(gCamera_height) = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                C2V(gCamera_height) -= 5.0f * time;
                if (C2V(gCamera_height) < pPos->v[1]) {
                    C2V(gCamera_height) = pPos->v[1];
                }
            } else {
                C2V(gCamera_height) += 5.0f * time * pPos->v[1];
                C2V(gCamera_height) /= 5.0f * time + 1.0f;
            }
        }
        l = pDirection->v[1] * d;
        if (l > 0) {
            br_scalar new_height = pPos->v[1] - l - height_inc / 2.0f;
            if (new_height > C2V(gCamera_height)) {
                C2V(gCamera_height) = new_height;
            }
        }

        C2V(gCamera)->t.t.translate.t.v[1] = height_inc + C2V(gCamera_height);
        BrVector3Copy(&C2V(gCamera_pos_before_collide), &C2V(gCamera)->t.t.translate.t);
        CollideCameraWithOtherCars(pPos, &C2V(gCamera)->t.t.translate.t);
        CollideCamera2(pPos, &C2V(gCamera)->t.t.translate.t, &old_camera_pos,
            manual_swing || C2V(gCamera_key_flags).field_0x0_bit1 || C2V(gCamera_key_flags).field_0x0_bit2, pCar != NULL ? pCar->collision_info : NULL);
        if (C2V(gCamera_has_collided) && swoop) {
            C2V(gCamera_height) = pPos->v[1];
        }
        PointCameraAtCar(pPos, m1, 1.f);
    }
    C2V(gOld_yaw__car) = C2V(gCamera_yaw);
    C2V(gOld_zoom) = (int)C2V(gCamera_zoom);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00410c60, GeneralisedPositionExternalCamera, GeneralisedPositionExternalCamera_original)

void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime_difference) {

    GeneralisedPositionExternalCamera(pCar, &pCar->car_master_actor->t.t.mat,
        &pCar->pos, pCar->speed, pCar->speedo_speed, &pCar->direction, &pCar->collision_info->omega, pTime_difference);
}

void C2_HOOK_FASTCALL SetPanningFieldOfView(void) {
    br_camera* camera_ptr;

    camera_ptr = C2V(gCamera)->type_data;
    if (C2V(gPanning_camera_angle) == 0) {
        C2V(gPanning_camera_angle) = BrDegreeToAngle(C2V(gCamera_angle) * 0.7f);
    }
    camera_ptr->field_of_view = C2V(gPanning_camera_angle);
}

void (C2_HOOK_FASTCALL * FrozenCamera_original)(tCar_spec* pCar, tU32 pTime);
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    FrozenCamera_original(pCar, pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040ef90, FrozenCamera, FrozenCamera_original)

void C2_HOOK_FASTCALL PositionPedCam(tPed_character_instance* pPed_character, tU32 pTime) {

    if (pPed_character == NULL) {
        ChangeCameraType();
    } else {
        br_matrix34* mat;

        mat = GetCharacterMatrixPtr(pPed_character);
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3],
            0.f, 0.f, &pPed_character->field_0xc0, &C2V(gZero_v__car), pTime);
    }
}

void C2_HOOK_FASTCALL PositionDroneCam(tU32 pTime_difference) {

    if (OKToViewDrones()) {
        br_matrix34* mat;
        br_vector3* dir;

        mat = GetCurrentViewDroneMat();
        dir = GetCurrentViewDroneDirection();
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3], 0.f, 0.f, dir, &C2V(gZero_v__car), pTime_difference);
    }
}

void (C2_HOOK_FASTCALL * SwingCamera_original)(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeeo_speed, tU32 pTime_difference);
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference) {

#if 0//
    // defined(C2_HOOKS_ENABLED)
    SwingCamera_original(pM1, pM2, pVn, pOmega, pSpeed, pSpeedo_speed, pTime_difference);
#else
    br_angle yaw;
    br_scalar cos_dtheta;
    br_scalar sign;
    int manual_swing;
    static C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, elapsed_time, 0x0058f63c, -1);
    static C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, old_vn, 0x00679338);

    manual_swing = C2V(gOld_yaw__car) != C2V(gCamera_yaw);
    if (C2V(elapsed_time) > 500) {
        C2V(elapsed_time) = -1;
    }
    if (C2V(elapsed_time) >= 0) {
        C2V(elapsed_time) += pTime_difference;
    }
    sign = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);

    if ((sign < 0.0f) == C2V(gCamera_sign)) {
        C2V(elapsed_time) = -1;
    } else if (BrVector3Dot(pVn, &C2V(old_vn)) <= 0.0 || C2V(elapsed_time) >= 0) {
        if (C2V(gAction_replay_camera_mode) != kActionReplayCameraMode_Standard || C2V(gCamera_sign)) {
            if (C2V(elapsed_time) < 0) {
                C2V(elapsed_time) = 0;
            }
            if (C2V(elapsed_time) < 500 && sign <= 0.0f) {
                BrVector3Negate(pVn, pVn);
            } else {
                C2V(gCamera_sign) = !C2V(gCamera_sign);
                C2V(gUNK_006792f4) = BrDegreeToAngle(200.f * (C2V(gUNK_006792f8) ? .06f : .04f));

                if (C2V(gCamera_yaw) > BR_ANGLE_DEG(180)) {
                    yaw = C2V(gCamera_yaw) - BR_ANGLE_DEG(180);
                } else {
                    yaw = C2V(gCamera_yaw);
                }
                if (C2V(gCamera_yaw) + BR_ANGLE_DEG(90) > BR_ANGLE_DEG(180)) {
                    C2V(gCamera_yaw) = BR_ANGLE_DEG(180) - C2V(gCamera_yaw);
                } else if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                    C2V(gCamera_yaw) = BR_ANGLE_DEG(180) - C2V(gCamera_yaw);
                }
            }
        }
    } else {
        C2V(gCamera_sign) = !C2V(gCamera_sign);
        if (C2V(gCamera_yaw) > BR_ANGLE_DEG(180)) {
            yaw = C2V(gCamera_yaw) - BR_ANGLE_DEG(180);
        } else {
            yaw = C2V(gCamera_yaw);
        }
        if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
            C2V(gCamera_yaw) = -C2V(gCamera_yaw);
        }
    }
    BrVector3Copy(&C2V(old_vn), pVn);
    if (C2V(gCamera_sign)) {
        yaw = -C2V(gCamera_yaw);
    } else {
        yaw = C2V(gCamera_yaw);
    }
    if (!C2V(gCar_flying)) {
        DrVector3RotateY(pVn, yaw);
    }
    br_scalar v16 = pVn->v[0] * C2V(gView_direction).v[0] + pVn->v[2] * C2V(gView_direction).v[2];
    br_scalar v17 = pVn->v[0] * C2V(gView_direction).v[2] - pVn->v[2] * C2V(gView_direction).v[0];

    if (v16 < 0.5f && C2V(gCamera_yaw) == 0) {
        C2V(gUNK_006792f8) = 1;
    }

    br_scalar v18 = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    br_scalar abs_v18 = fabsf(v18);
    br_angle v8 = BrRadianToAngle((float)pTime_difference * (abs_v18 + REC2_PI_F / 36.f) / 1000.f);
    br_angle v9 = BrRadianToAngle(sqrtf(fabsf(v17)));

    if (!(C2V(gUNK_006792f4) == 0 && v16 > 0.f && v9 < v8) && !C2V(gCar_flying) && !manual_swing) {
        br_angle omega;

        if (C2V(gUNK_006792f4) == 0) {
            C2V(gUNK_006792f4) = BrDegreeToAngle((C2V(gUNK_006792f8) ? .06f : .04f) * 50.f);
        }
        omega = pTime_difference * C2V(gUNK_006792f4) / 100;
        if (omega < v8) {
            omega = v8;
        }
        cos_dtheta = BR_COS(omega);
        if (cos_dtheta > v16) {
            br_scalar ts;

            if (v16 < -.7f && abs_v18 > 0.8f && v18 * v17 > 0.f) {
                omega = -omega;
            }
            ts = BrAngleToRadian(omega);
            if (v17 > 0.f) {
                pVn->v[0] = sinf(ts) * C2V(gView_direction).v[2] + cosf(ts) * C2V(gView_direction).v[0];
                pVn->v[2] = cosf(ts) * C2V(gView_direction).v[2] - sinf(ts) * C2V(gView_direction).v[0];
            } else {
                pVn->v[0] = cosf(ts) * C2V(gView_direction).v[0] - sinf(ts) * C2V(gView_direction).v[2];
                pVn->v[2] = sinf(ts) * C2V(gView_direction).v[0] + cosf(ts) * C2V(gView_direction).v[2];
            }
            C2V(gUNK_006792f4) += BrDegreeToAngle(pTime_difference * (C2V(gUNK_006792f8) ? .06f : .04f));

            if (C2V(gUNK_006792f4) > BrDegreeToAngle(C2V(gUNK_006792f8) ? 40.f : 10.f)) {
                C2V(gUNK_006792f4) = BrDegreeToAngle(C2V(gUNK_006792f8) ? 40.f : 10.f);
            }
            return;
        }
    }
    C2V(gUNK_006792f8) = 0;
    C2V(gCamera_mode) = 0;
    C2V(gUNK_006792f4) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00411980, SwingCamera, SwingCamera_original)

void C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {

    // empty
}
C2_HOOK_FUNCTION(0x00413570, CollideCameraWithOtherCars)

void (C2_HOOK_FASTCALL * PointCameraAtCar_original)(br_vector3* pPos, br_matrix34* pMat, float pFov_factor);
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {

#if 0//defined(C2_HOOKS_ENABLED)
    PointCameraAtCar(pPos, pMat, pFov_factor);
#else
    br_vector3 vn;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar dist;
    br_scalar frac;
    br_angle theta;
    br_vector3* pos;
    br_camera* camera_ptr;
    int swoop;

    camera_ptr = C2V(gCamera)->type_data;
    theta = (br_angle)(pFov_factor * camera_ptr->field_of_view / 5.f);
    swoop = C2V(gCountdown) && C2V(gCamera_height) > pPos->v[1] + 0.01f;
    if (swoop) {
        BrVector3Sub(&tv, &C2V(gAverage_grid_position), pPos);
        frac = (C2V(gCamera_height) - pPos->v[1]) / 10.0f;
        BrVector3Scale(&tv, &tv, frac);
        BrVector3Add(&tv, pPos, &tv);
        pos = &tv;
        theta = (br_angle)((1.0f - frac) * (float)theta);
    } else {
        pos = pPos;
    }
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[2]);
    vn.v[1] = 0.f;
    BrVector3Normalise(&vn, &vn);
    pMat->m[0][0] = -vn.v[2];
    pMat->m[0][1] = 0.0f;
    pMat->m[0][2] = vn.v[0];
    pMat->m[1][0] = 0.0f;
    pMat->m[1][1] = 1.0f;
    pMat->m[1][2] = 0.0f;
    pMat->m[2][0] = -vn.v[0];
    pMat->m[2][1] = 0.0f;
    pMat->m[2][2] = -vn.v[2];
    BrVector3Sub(&tv2, pos, (br_vector3*)pMat->m[3]);
    dist = BrVector3Dot(&tv2, &vn);
    BrMatrix34PreRotateX(pMat, theta - BrRadianToAngle(atan2f(pMat->m[3][1] - pos->v[1], dist)));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00411fc0, PointCameraAtCar, PointCameraAtCar_original)

int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return C2V(gInTheSea);
}
C2_HOOK_FUNCTION(0x00414ca0, IsCarInTheSea);

float (C2_HOOK_FASTCALL * RepairCar_original)(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection);
float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection) {

#if defined(C2_HOOKS_ENABLED)
    return RepairCar_original(pCar_ID, pFrame_period, pTotal_deflection);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043b840, RepairCar, RepairCar_original)

void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {

    C2V(gQuite_wild_end) = 0;
    C2V(gQuite_wild_start) = 0;
    C2V(gOn_me_wheels_start) = 0;
    C2V(gWoz_upside_down_at_all) = 0;
    C2V(gWild_start) = 0;
}
C2_HOOK_FUNCTION(0x0041e580, CancelPendingCunningStunt)

void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = BrRadianToDegree(atanf((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE;
    pCar->rr_sus_position = (pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE;
    pCar->lf_sus_position = (pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE;
    pCar->rf_sus_position = (pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE;
    PipeSingleGraphicalWheelStuff(pCar);
}
C2_HOOK_FUNCTION(0x004157e0, CalcGraphicalWheelStuff)

void (C2_HOOK_FASTCALL * FinishCars_original)(tU32 pLast_frame_time, tU32 pTime);
void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime) {

#if 0//defined(C2_HOOKS_ENABLED)
    FinishCars_original(pLast_frame_time, pTime);
#else
    int i;

    if (C2V(gCar_flying)) {
        BrMatrix34Copy(&C2V(gCar_to_view)->collision_info->actor->t.t.mat,
            &C2V(gCar_to_view)->collision_info->transform_matrix);
    }
    for (i = 0; i < C2V(gNum_cars_and_non_cars); i++) {
        tCar_spec* car;
        float original_speed;
        br_vector3 minus_k;

        car = C2V(gActive_car_list)[i];
        BrMatrix34ApplyP(&car->pos, &car->collision_info->cmpos, &car->car_master_actor->t.t.mat);
        original_speed = car->speed;
        car->speed = BR_LENGTH2(car->collision_info->v.v[0], car->collision_info->v.v[2]) / 1000.f;
        BrVector3Negate(&minus_k, (br_vector3*)&car->car_master_actor->t.t.mat.m[2]);
        if (original_speed < 0.0001f && car->speed > 0.0001f) {
            car->speed = .0001f;
        }
        if (car->speed > .0001f) {
            BrVector3Normalise(&car->direction, &car->collision_info->v);
        } else {
            float ts;
            if (BrVector3Dot(&car->direction, &minus_k) < 0.f ) {
                ts = 1.f;
            } else {
                ts = -1.f;
            }
            BrVector3SetFloat(&minus_k, 0.f, 0.f, ts);
            BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
        }
        if (car != NULL && car->driver >= eDriver_oppo) {
            int wheel;

            car->speedo_speed = BrVector3Dot(&minus_k, &car->collision_info->v) / 1000.f;
            CalcGraphicalWheelStuff(car);

            for (wheel = 0; wheel < 4; wheel++) {
                if (car->oldd[wheel] < car->susp_height[wheel / 2] && C2V(gCurrent_race).material_modifiers[car->material_index[wheel]].smoke_type >= 2 && !car->collision_info->disable_move_rotate) {
                    GenerateContinuousSmoke(car, wheel, pTime);
                }
            }
        }
    }
    if (pLast_frame_time < C2V(gPHIL_last_physics_tick) && C2V(gCar_to_view)->speed > .0001f) {
        br_vector3 tv;
        br_scalar dt;

        dt = (C2V(gPHIL_last_physics_tick) - pLast_frame_time) / 40.f;
        BrVector3Sub(&tv, &C2V(gCar_to_view_original_v), &C2V(gCar_to_view)->collision_info->v);
        BrVector3Scale(&tv, &tv, dt);
        BrVector3Accumulate(&tv, &C2V(gCar_to_view)->collision_info->v);
        BrVector3Normalise(&C2V(gCar_to_view)->direction, &tv);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00416500, FinishCars, FinishCars_original)

void (C2_HOOK_FASTCALL * CheckForDeAttachmentOfNonCars_original)(tU32 pTime);
void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    CheckForDeAttachmentOfNonCars_original(pTime);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004203c0, CheckForDeAttachmentOfNonCars, CheckForDeAttachmentOfNonCars_original)

void C2_HOOK_FASTCALL PrepareCars(tU32 pFrame_start_time) {
    int i;
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, last_frame_start, 0x006793f4);

    C2V(last_frame_start) = pFrame_start_time;
    for (i = 0; i < C2V(gNum_cars_and_non_cars); i++) {
        tCar_spec* car;

        car = C2V(gActive_car_list)[i];
        car->frame_collision_flag = C2V(gOver_shoot) && car->collision_info->collision_flag;
        if (car != NULL && car->driver >= eDriver_oppo) {
            RecordLastDamage(car);
            if (car->driver == eDriver_oppo && C2V(gStop_opponents_moving)) {
                car->acc_force = 0.f;
                car->brake_force = 0.f;
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
            }
            if (!car->wheel_slip) {
                StopSkid(car);
            }
            if (car->driver == eDriver_net_human && pFrame_start_time - 1000 > car->collision_info->message_time) {
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
                car->keys.horn = 0;
            }
            SetSmokeLastDamageLevel(car);
        }
    }
}

void C2_HOOK_FASTCALL StopSkid(tCar_spec* pC) {

    if (C2V(gLast_car_to_skid)[0] == pC) {
        DRS3StopSound(C2V(gSkid_tag)[0]);
    }
    if (C2V(gLast_car_to_skid)[1] == pC) {
        DRS3StopSound(C2V(gSkid_tag)[1]);
    }
}

void (C2_HOOK_FASTCALL * APTCPreCollision_original)(void);
void C2_HOOK_FASTCALL APTCPreCollision(void) {
#if defined(C2_HOOKS_ENABLED)
    APTCPreCollision_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00415890, APTCPreCollision, APTCPreCollision_original)

void (C2_HOOK_FASTCALL * APTCPostCollision_original)(void);
void C2_HOOK_FASTCALL APTCPostCollision(void) {
#if defined(C2_HOOKS_ENABLED)
    APTCPostCollision_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00416070, APTCPostCollision, APTCPostCollision_original)

void (C2_HOOK_FASTCALL * APTCChangedObjects_original)(tCollision_info* pArg1, undefined4 pArg2);
void C2_HOOK_FASTCALL APTCChangedObjects(tCollision_info* pArg1, undefined4 pArg2) {
#if defined(C2_HOOKS_ENABLED)
    APTCChangedObjects_original(pArg1, pArg2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00416300, APTCChangedObjects, APTCChangedObjects_original)

int (C2_HOOK_FASTCALL * APTCActiveHalted_original)(tCollision_info* pArg1);
int C2_HOOK_FASTCALL APTCActiveHalted(tCollision_info* pArg1) {
#if defined(C2_HOOKS_ENABLED)
    APTCActiveHalted_original(pArg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00416270, APTCActiveHalted, APTCActiveHalted_original)

int (C2_HOOK_FASTCALL * APTCPassiveActivated_original)(tCollision_info* pArg1);
int C2_HOOK_FASTCALL APTCPassiveActivated(tCollision_info* pArg1) {
#if defined(C2_HOOKS_ENABLED)
    APTCPassiveActivated_original(pArg1);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004162b0, APTCPassiveActivated, APTCPassiveActivated_original)
