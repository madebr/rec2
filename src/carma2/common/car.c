#include "car.h"

#include "brucetrk.h"
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
#include "oil.h"
#include "opponent.h"
#include "pedestrn.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"
#include "pratcam.h"
#include "racemem.h"
#include "raycast.h"
#include "replay.h"
#include "skidmark.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "trig.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"
#include "brender/br_inline_funcs.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include "c2_math.h"


// GLOBAL: CARMA2_HW 0x0074a5f4
int gOver_shoot;

// GLOBAL: CARMA2_HW 0x0074c7c0
tCar_spec* gActive_car_list[139]; /* FIXME: uncertain about array length */

// GLOBAL: CARMA2_HW 0x0079ef2c
int gNum_cars_and_non_cars;

// GLOBAL: CARMA2_HW 0x0074c9ec
int gNum_active_cars;

// GLOBAL: CARMA2_HW 0x006793a0
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x006793d8
int gCar_simplification_level;

// GLOBAL: CARMA2_HW 0x0079eda0
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x006793dc
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x00679360
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
tCollision_info* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x00679268
br_vector3 gAverage_grid_position;


// GLOBAL: CARMA2_HW 0x0067939c
int gTesting_car_for_sensible_place;

// GLOBAL: CARMA2_HW 0x0065cf78
tWorld_callbacks gWorld_callbacks = {
    ProcessForcesCallback,
    ProcessJointForcesCallback,
    NewFacesListCallback,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
};


// GLOBAL: CARMA2_HW 0x0058f6e0
tPhysics_callbacks gCar_physics_callbacks = {
    &gWorld_callbacks,
    APTCPreCollision,
    APTCPostCollision,
    APTCChangedObjects,
    APTCActiveHalted,
    APTCPassiveActivated,
    NULL,
};

// GLOBAL: CARMA2_HW 0x006940b0
int gFace_count;

// GLOBAL: CARMA2_HW 0x00744820
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00679294
int gCamera_mode;

// GLOBAL: CARMA2_HW 0x006792b8
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x006792bc
int gOpponent_viewing_mode;

// GLOBAL: CARMA2_HW 0x0058f5fc
int gNet_player_to_view_index = -1;

// GLOBAL: CARMA2_HW 0x00679304
br_angle gPanning_camera_angle;

// GLOBAL: CARMA2_HW 0x0068b8d0
br_vector3 gZero_v__car;

// GLOBAL: CARMA2_HW 0x006792c8
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x00655f40
br_scalar gCamera_zoom = 0.2f;

// GLOBAL: CARMA2_HW 0x0079efa4
tCamera_key_flags gCamera_key_flags;

// GLOBAL: CARMA2_HW 0x006792f4
undefined2 gUNK_006792f4;

// GLOBAL: CARMA2_HW 0x006792f8
int gUNK_006792f8;

// GLOBAL: CARMA2_HW 0x006792e8
br_vector3 gCamera_pos_before_collide;

// GLOBAL: CARMA2_HW 0x006792e0
br_angle gOld_yaw__car;

// GLOBAL: CARMA2_HW 0x006793e0
int gCamera_has_collided;

// GLOBAL: CARMA2_HW 0x006792e4
br_angle gOld_zoom;

// GLOBAL: CARMA2_HW 0x00679364
int gInTheSea;

// GLOBAL: CARMA2_HW 0x006793a8
tU32 gWild_start;

// GLOBAL: CARMA2_HW 0x006793ac
tU32 gQuite_wild_end;

// GLOBAL: CARMA2_HW 0x006793b0
tU32 gQuite_wild_start;

// GLOBAL: CARMA2_HW 0x006793b4
tU32 gOn_me_wheels_start;

// GLOBAL: CARMA2_HW 0x006793b8
tU32 gWoz_upside_down_at_all;

// GLOBAL: CARMA2_HW 0x00676854
int gStop_opponents_moving = 0;

// GLOBAL: CARMA2_HW 0x006793c0
int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x006793c8
tCar_spec* gLast_car_to_skid[2];

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// GLOBAL: CARMA2_HW 0x006793a4
tU32 gLast_cunning_stunt;

// GLOBAL: CARMA2_HW 0x006792b4
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x0058f6b0
const float gCar_simplification_factor[2][5] = {
    { 20.0f, 3.0f, 1.5f, 0.75f, 0.0f },
    { 50.0f, 5.0f, 2.5f, 1.50f, 0.0f }
};

// FUNCTION: CARMA2_HW 0x004105f0
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004122b0
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info) {
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
    int loop_done;
    int count;
    br_vector3 nor;
    br_vector3 delta;

    hither = ((br_camera*)gCamera->type_data)->hither_z * 3.0f;
    gCamera_has_collided = 0;
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
        ActorFindFace(car_pos, &delta, gTrack_actor, &nor, &ts, &material, &actor);
        if (ts >= 1.0) {
            BrVector3Copy(&new_cam_pos, cam_pos);
        } else {
            gCamera_has_collided = 1;
            BrVector3Scale(&tv, &delta, ts);
            BrVector3Add(&new_cam_pos, car_pos, &tv);
        }
        BrVector3Normalise(&dir, &delta);

        BrVector3Scale(&tv2, &dir, -hither);
        BrVector3Add(&cam_hither_pos, &new_cam_pos, &tv2);
        BrVector3Add(&car_coll_pos, car_pos, &delta);

        for (i = 0; i < gNum_active_non_cars; i++) {
            tNon_car_spec* non_car = gActive_non_car_list[i];

            if (non_car->flags & 0x80000) {
                br_scalar factor;
                br_vector3 temp_normal;

                DrMatrix34ApplyLPInverse(&p1, &cam_hither_pos, &non_car->collision_info->actor->t.t.mat);
                DrMatrix34ApplyLPInverse(&p2, &car_coll_pos, &non_car->collision_info->actor->t.t.mat);
                if (ShapeRayCast(&p1, &p2, non_car->collision_info->shape, &new_cam_pos, &factor, &temp_normal)) {
                    gCamera_has_collided = 1;
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

        gCamera_has_collided = 1;
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
        if ((float)sqrt(l) < gMin_camera_car_distance && !loop_done) {
            br_scalar a;
            br_scalar b;
            br_scalar discr;

            BrVector3Scale(&tv, &nor, -nor.v[1]);
            tv.v[1] += 1.f;
            if (gProgram_state.current_car.car_master_actor->t.t.mat.m[1][1] < 0.f) {
                BrVector3Negate(&tv, &tv);
            }
            a = BrVector3LengthSquared(&tv);
            b = BrVector3Dot(&tv, &delta);
            discr = REC2_SQR(b) - 4.f * a * (l - REC2_SQR(gMin_camera_car_distance));
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
                if (dot < .03f && !gAction_replay_mode) {
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
        if (collision_info != NULL && gAction_replay_camera_mode != kActionReplayCameraMode_Manual) {
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
}

// FUNCTION: CARMA2_HW 0x0041f4f0
void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    REC2_THISCALL_UNUSED(pArg2);

    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x0041fc60
void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
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
}

// FUNCTION: CARMA2_HW 0x0041fe50
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        gList_collision_infos,
        pVec3,
        &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}

// FUNCTION: CARMA2_HW 0x004104b0
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0040f790
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004ff530
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info) {
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
            id_len = strlen(mat_id);
            if (id_len != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                new_special_volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pCollision_info->auto_special_volume = new_special_volume;
    pCollision_info->water_depth_factor = 1.0f;
}

// FUNCTION: CARMA2_HW 0x004175e0
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
        && gCurrent_race.material_modifiers[car->material_index[0]].tyre_road_friction >= 0.1f
        && gCurrent_race.material_modifiers[car->material_index[1]].tyre_road_friction >= 0.1f
        && gCurrent_race.material_modifiers[car->material_index[2]].tyre_road_friction >= 0.1f
        && gCurrent_race.material_modifiers[car->material_index[3]].tyre_road_friction >= 0.1f
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

// GLOBAL: CARMA2_HW 0x0058f660
tControl_car_fn ControlCar[6] = {
    ControlCar1,
    ControlCar2,
    ControlCar3,
    ControlCar4,
    ControlCar5,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0058f678
int gControl__car = 3;

// FUNCTION: CARMA2_HW 0x00414cb0
void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference) {
    static int last_key_down = 1;
    tCar_spec* car;
    tU32 time;
    br_vector3 minus_k;

    car = &gProgram_state.current_car;

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
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {

            if (i != gThis_net_player_index) {
                ControlCar[gControl__car](gNet_players[i].car REC2_THISCALL_EDX, 0.001f * pTime_difference);
            }
        }
    }
    if (gCar_flying) {
        FlyCar(gCar_to_view REC2_THISCALL_EDX, pTime_difference / 1000.f);
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
    ControlCar[gControl__car](car REC2_THISCALL_EDX, pTime_difference / 1000.0f);
    RememberSafePosition(car, pTime_difference);
    if (gCamera_reset) {
        BrVector3SetFloat(&minus_k, 0.0f, 0.0f, -1.0f);
        gCamera_sign = 0;
        BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
    }
}

// FUNCTION: CARMA2_HW 0x00414510
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, int pGrid_index) {
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
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.f) {
                        break;
                    }
                }
            }
            if (j == gNumber_of_net_players) {
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
    if (gNet_mode == eNet_mode_none && pGrid_index < 0) {
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
    if (gTrack_actor != NULL) {
        FindBestY(
            &car_actor->t.t.translate.t,
            gTrack_actor,
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
    if (gNet_mode != eNet_mode_none) {
        tCompressed_matrix3 compressed;
        int inactive;

        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(
            &gNet_players[pThe_race->opponent_list[pCar_index].net_player_index].initial_position,
            &car->car_master_actor->t.t.mat);
    }
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
}

// FUNCTION: CARMA2_HW 0x00413f70
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
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
        if (gNet_mode == eNet_mode_none) {
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
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
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
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_oppo) {
                if (gCurrent_race.opponent_list[j].car_spec == pCar) {
                    pCar->car_ID = 0x200 + index;
                }
                index++;
            }
        }
        break;
    case eDriver_net_human:
        index = 0;
        for (j = 0; j < gCurrent_race.number_of_racers; j++) {
            if (gCurrent_race.opponent_list[j].car_spec != NULL
                    && gCurrent_race.opponent_list[j].car_spec->driver == eDriver_net_human) {
                if (gCurrent_race.opponent_list[j].car_spec == pCar) {
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
        abort();
        break;
    }
    PossibleService();
    pCar->collision_info->box_face_ref = gFace_num__car - 2;
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

    if (gNet_mode == eNet_mode_none) {
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
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < REC2_ASIZE(pCar->power_up_levels); j++) {
            if (gNet_mode == eNet_mode_none) {
                pCar->power_up_levels[j] = gInitial_APO[j].initial[gProgram_state.skill_level];
            } else {
                pCar->power_up_levels[j] = gInitial_APO[j].initial_network[gProgram_state.skill_level];
            }
            if (gNet_mode == eNet_mode_none) {
                pCar->power_up_slots[j] = gInitial_APO_potential[j].initial[gProgram_state.skill_level];
            } else {
                pCar->power_up_slots[j] = gInitial_APO_potential[j].initial_network[gProgram_state.skill_level];
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00414400
void C2_HOOK_FASTCALL InitialiseCar(tCar_spec* pCar) {

    InitialiseCar2(pCar, 1);
}

// FUNCTION: CARMA2_HW 0x00414410
void C2_HOOK_FASTCALL InitialiseCarsEtc(tRace_info* pThe_race) {
    int i;
    int cat;
    int car_count;
    tCar_spec* car;
    br_bounds bnds;

    BrVector3Copy(&gProgram_state.initial_position, &pThe_race->initial_position);
    gProgram_state.initial_yaw = pThe_race->initial_yaw;
    BrActorToBounds(&bnds, gProgram_state.track_spec.the_actor);
    gMin_world_y = bnds.min.v[1];
    gNum_active_non_cars = 0;
    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            PossibleService();
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            if (cat != eVehicle_not_really) {
                InitialiseCar(car);
            }
        }
    }
    gCamera_yaw = 0;
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Manual) {
        gCamera_type = 0;
        gAction_replay_camera_mode = kActionReplayCameraMode_Standard;
    }
    InitialiseExternalCamera();
    if (gUnknown_car_collision_info != NULL && gProgram_state.current_car.collision_info != gUnknown_car_collision_info->parent) {
        AddCollisionInfoChild(gProgram_state.current_car.collision_info, gUnknown_car_collision_info);
    }
}

// FUNCTION: CARMA2_HW 0x004148d0
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


// FUNCTION: CARMA2_HW 0x00413580
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    br_scalar ts;
    tCar_spec* c;
    br_angle yaw;

    c = gCar_to_view;
    if (!gProgram_state.racing) {
        c = &gProgram_state.current_car;
    }
    gCamera_height = c->pos.v[1];
    BrVector3Set(&gView_direction, c->direction.v[0], 0.0f, c->direction.v[2]);
    BrVector3Normalise(&gView_direction, &gView_direction);
    ts = -BrVector3Dot(&gView_direction, (br_vector3*)c->car_master_actor->t.t.mat.m[2]);
    gCamera_sign = ts < 0;
    gCamera_mode = 0;
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    DrVector3RotateY(&gView_direction, yaw);
    gMin_camera_car_distance = 0.6f;
    gCamera_frozen = 0;
    gCamera_mode = -2;
    if (gCountdown && (gNet_mode == eNet_mode_none || gCurrent_net_game->options.grid_start) && gCountdown > 4) {
        gCamera_height += 10.f;
    }
}

void C2_HOOK_FASTCALL SetAmbientPratCam(tCar_spec* pCar) {
    br_scalar vcs_x;
    br_scalar vcs_y;
    br_scalar vcs_z;
    br_scalar abs_vcs_x;
    br_scalar abs_vcs_y;
    br_scalar abs_vcs_z;
    br_scalar abs_omega_x;
    br_scalar abs_omega_y;
    br_scalar abs_omega_z;
    tU32 the_time;
    // GLOBAL: CARMA2_HW 0x00679370
    static tU32 last_time_on_ground;

    if (gRace_finished) {
        return;
    }
    the_time = GetTotalTime();
    if (pCar->number_of_wheels_on_ground != 0) {
        last_time_on_ground = the_time;
    }
    vcs_x = pCar->collision_info->velocity_car_space.v[0] / 1000.f;
    vcs_y = pCar->collision_info->velocity_car_space.v[1] / 1000.f;
    vcs_z = pCar->collision_info->velocity_car_space.v[2] / 1000.f;
    abs_vcs_x = fabsf(vcs_x);
    abs_vcs_y = fabsf(vcs_y);
    abs_vcs_z = fabsf(vcs_z);
    abs_omega_x = fabsf(pCar->collision_info->omega.v[0]);
    abs_omega_y = fabsf(pCar->collision_info->omega.v[1]);
    abs_omega_z = fabsf(pCar->collision_info->omega.v[2]);

    if (abs_omega_x > 4.5f || abs_omega_z > 4.5f) {
        ChangeAmbientPratcam(9);
    } else if (abs_omega_y > 4.5f) {
        ChangeAmbientPratcam(12);
    } else if (abs_omega_x > 3.f || abs_omega_z > 3.f) {
        ChangeAmbientPratcam(8);
    } else if (abs_omega_y > 3.f) {
        ChangeAmbientPratcam(11);
    } else if (pCar->car_master_actor->t.t.mat.m[1][1] < 0.1f) {
        ChangeAmbientPratcam(44);
    } else if (abs_vcs_y > abs_vcs_z && abs_vcs_y > abs_vcs_x && vcs_y < -.004f) {
        ChangeAmbientPratcam(6);
    } else if (the_time - last_time_on_ground > 500) {
        ChangeAmbientPratcam(5);
    } else if (abs_vcs_x > abs_vcs_z && vcs_x > .001f) {
        ChangeAmbientPratcam(26);
    } else if (abs_vcs_x > abs_vcs_z && vcs_x < -.001f) {
        ChangeAmbientPratcam(25);
    } else if (abs_omega_x > 1.5f || abs_omega_z > 1.5f) {
        ChangeAmbientPratcam(7);
    } else if (abs_omega_y > 1.5f) {
        ChangeAmbientPratcam(10);
    } else if (abs_vcs_z > .01f) {
        ChangeAmbientPratcam(3);
    } else if (abs_vcs_z > .004f) {
        ChangeAmbientPratcam(2);
    } else if (abs_vcs_z > .0015f) {
        ChangeAmbientPratcam(1);
    } else {
        ChangeAmbientPratcam(0);
    }
}

// FUNCTION: CARMA2_HW 0x0041e660
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    tU32 the_time;
    int car;
    int cat;

    if (gNet_mode != eNet_mode_none
            && gCurrent_net_game->type == eNet_game_type_foxy
            && gThis_net_player_index == gIt_or_fox) {
        gProgram_state.current_car.power_up_levels[1] = 0;
    }
    SetAmbientPratCam(&gProgram_state.current_car);

    the_time = PDGetTotalTime();
    for (cat = eVehicle_self; cat <= eVehicle_net_player; cat++) {
        int car_count;

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (car = 0; car < car_count; car++) {
            tCar_spec* the_car;

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, car);
            }
            if (!(the_car != NULL && the_car->driver == eDriver_local_human) && PointOutOfSight(&the_car->pos REC2_THISCALL_EDX, gYon_squared)) {
                the_car->car_master_actor->render_style = BR_RSTYLE_NONE;
            } else {
                the_car->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
            }
        }
    }
    if (!(gCar_to_view != NULL && gCar_to_view->driver == eDriver_oppo)) {
        gCar_to_view->car_master_actor->render_style = BR_RSTYLE_DEFAULT;
    }

    for (car = 0; car < gNum_active_cars; car++) {
        tCar_spec* the_car;

        the_car = gActive_car_list[car];
        if (the_car->car_master_actor->render_style != BR_RSTYLE_NONE && the_car != NULL && the_car->driver >= eDriver_oppo) {
            br_scalar car_x;
            br_scalar car_z;
            int oily_count;
            int i;

            the_car->shadow_intersection_flags = 0;
            car_x = the_car->car_master_actor->t.t.translate.t.v[0];
            car_z = the_car->car_master_actor->t.t.translate.t.v[2];
            oily_count = GetOilSpillCount();

            for (i = 0; i < oily_count; i++) {
                br_actor* oily_actor;
                br_scalar oily_size;

                GetOilSpillDetails(i, &oily_actor, &oily_size);
                if (oily_actor != NULL) {
                    br_scalar car_radius;

                    car_radius = the_car->collision_info->shape->common.bb.max.v[2] / WORLD_SCALE * 1.5f;
                    if (oily_actor->t.t.translate.t.v[0] - oily_size < car_x + car_radius
                            && oily_actor->t.t.translate.t.v[0] + oily_size > car_x - car_radius
                            && oily_actor->t.t.translate.t.v[2] - oily_size < car_z + car_radius
                            && oily_actor->t.t.translate.t.v[2] + oily_size > car_z - car_radius) {
                        the_car->shadow_intersection_flags |= 1 << i;
                    }
                }
            }
            if (the_car->driver < eDriver_net_human && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
                if (gCountdown) {
                    float sine_angle;
                    float raw_revs;
                    float rev_reducer;

                    sine_angle = FRandomBetween(.4f, 1.6f) * ((float)GetTotalTime() / ((float)gCountdown * 100.f));
                    sine_angle = frac(sine_angle) * 360.0f;
                    sine_angle = FastScalarSin((int)sine_angle);
                    raw_revs = the_car->red_line * fabsf(sine_angle);
                    rev_reducer = (11.f - (float)(gCountdown)) / 10.f;
                    the_car->revs = rev_reducer * raw_revs;
                } else {
                    the_car->revs = (the_car->speedo_speed / 0.003f - (float)(int)(the_car->speedo_speed / 0.003))
                                    * (float)(the_car->red_line - 800)
                                    + 800.f;
                }
            }
            for (i = 0; i < the_car->number_of_steerable_wheels; i++) {
                ControlBoundFunkGroove(the_car->steering_ref[i] REC2_THISCALL_EDX, the_car->steering_angle);
            }
            for (i = 0; i < REC2_ASIZE(the_car->rf_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rf_sus_ref[i] REC2_THISCALL_EDX, the_car->rf_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i] REC2_THISCALL_EDX, -the_car->lf_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lf_sus_ref[i] REC2_THISCALL_EDX, the_car->lf_sus_position);
                }
            }
            for (i = 0; i < REC2_ASIZE(the_car->rr_sus_ref); i++) {
                ControlBoundFunkGroove(the_car->rr_sus_ref[i] REC2_THISCALL_EDX, the_car->rr_sus_position);
                if ((i & 1) != 0) {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i] REC2_THISCALL_EDX, -the_car->lr_sus_position);
                } else {
                    ControlBoundFunkGroove(the_car->lr_sus_ref[i] REC2_THISCALL_EDX, the_car->lr_sus_position);
                }
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                float wheel_speed;

                wheel_speed = -(the_car->speedo_speed / the_car->non_driven_wheels_circum * (float)gFrame_period);
                if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                    wheel_speed = -wheel_speed;
                }
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_1 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_2 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_3 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->non_driven_wheels_spin_ref_4 REC2_THISCALL_EDX, wheel_speed);
                if (the_car->driver >= eDriver_net_human) {
                    if (the_car->gear != 0.f) {
                        wheel_speed = -(the_car->revs
                                        * the_car->speed_revs_ratio
                                        * (float)the_car->gear
                                        * (float)gFrame_period
                                        / (1000.f * WORLD_SCALE))
                                        / the_car->driven_wheels_circum;
                    } else if (the_car->keys.brake) {
                        wheel_speed = 0.0;
                    } else {
                        wheel_speed = -(the_car->speedo_speed / the_car->driven_wheels_circum * (float)gFrame_period);
                    }
                    if (gAction_replay_mode && ARGetReplayDirection() < 0) {
                        wheel_speed = -wheel_speed;
                    }
                }
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_1 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_2 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_3 REC2_THISCALL_EDX, wheel_speed);
                ControlBoundFunkGroovePlus(the_car->driven_wheels_spin_ref_4 REC2_THISCALL_EDX, wheel_speed);
            }
            if (gAction_replay_mode) {
                MungeSpecialVolume(the_car->collision_info);
            } else if (the_car->driver == eDriver_local_human && the_car->collision_info->M < 5.f) {
                br_scalar abs_omega_x;
                br_scalar abs_omega_y;
                br_scalar abs_omega_z;
                int spinning_wildly;

                abs_omega_x = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[0] + 3.30f) / 2.f * fabsf(the_car->collision_info->omega.v[0]);
                abs_omega_y = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[1] + 3.57f) / 2.f * fabsf(the_car->collision_info->omega.v[1]);
                abs_omega_z = (WORLD_SCALE * WORLD_SCALE * the_car->collision_info->I.v[2] + 0.44f) / 2.f * fabsf(the_car->collision_info->omega.v[2]);
                spinning_wildly = abs_omega_x > 26.4f || abs_omega_y > 49.98f || abs_omega_z > 3.52f;
                if (spinning_wildly && the_time - gLast_cunning_stunt > 10000) {
                    if (gWild_start == 0
                            || (the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                        gWild_start = the_time;
                    } else if (the_time - gWild_start >= 500) {
                        DoFancyHeadup(18);
                        EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                        gLast_cunning_stunt = the_time;
                        gOn_me_wheels_start = 0;
                        gQuite_wild_end = 0;
                        gQuite_wild_start = 0;
                        gWoz_upside_down_at_all = 0;
                    }
                } else {
                    int spinning_mildly;

                    gWild_start = 0;
                    spinning_mildly = abs_omega_x > 1.65f || abs_omega_z > .22f;
                    if (the_car->number_of_wheels_on_ground < 4) {
                        gOn_me_wheels_start = 0;
                        if (the_car->number_of_wheels_on_ground == 0 && spinning_mildly) {
                            if (gQuite_wild_start == 0) {
                                gQuite_wild_start = the_time;
                            }
                            if (the_car->car_master_actor->t.t.mat.m[1][1] < -.8f) {
                                gWoz_upside_down_at_all = the_time;
                            }
                        } else {
                            gQuite_wild_end = the_time;
                        }
                    } else {
                        if (gQuite_wild_end == 0) {
                            gQuite_wild_end = the_time;
                        }
                        if (gQuite_wild_start == 0
                                || the_time - gLast_cunning_stunt <= 10000) {
                            gWild_start = 0;
                        } else if (gQuite_wild_end - gQuite_wild_start >= 2000
                                && gQuite_wild_start <= gWoz_upside_down_at_all
                                && gWoz_upside_down_at_all <= gQuite_wild_end) {

                            if (gOn_me_wheels_start != 0) {
                                if (the_time - gOn_me_wheels_start > 500
                                        && !(the_car->collision_info->last_special_volume != NULL && the_car->collision_info->last_special_volume->gravity_multiplier != 1.f)) {
                                    DoFancyHeadup(18);
                                    EarnCredits(gCunning_stunt_bonus[gProgram_state.skill_level]);
                                    gLast_cunning_stunt = PDGetTotalTime();
                                    gQuite_wild_start = 0;
                                    gQuite_wild_end = 0;
                                    gOn_me_wheels_start = 0;
                                    gWoz_upside_down_at_all = 0;
                                } else {
                                    gWild_start = 0;
                                }
                            } else if (the_time - gQuite_wild_end < 300) {
                                gOn_me_wheels_start = the_time;
                                gWild_start = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f280
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data *crush_data = pActor->user;

    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041f110
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    int variant;
    tCar_spec* car;
    br_vector3 tv;
    float level;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];

        if (car != NULL && car->driver >= eDriver_oppo && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            BrVector3Sub(&tv, &car->car_master_actor->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]);
            level = gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level] >= 0.001f ? BrVector3LengthSquared(&tv) / gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level] : BR_SCALAR_MAX;

            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (car->detail_levels[j] < level) {
                    break;
                }
            }
            if (j > 0) {
                variant = j;
                DRActorEnumRecurse(car->car_model_actor, SwitchCarModel, &variant);
                car->field_0xe18 = variant;
            } else if (!gAction_replay_mode && car->use_shell_model && car->shell_model != NULL) {
                DRActorEnumRecurse(car->car_model_actor, TurnOffNonGroovers, NULL);
                car->car_model_actor->type = BR_ACTOR_MODEL;
                car->car_model_actor->model = car->shell_model;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f2a0
void C2_HOOK_FASTCALL DoComplexCarModels(void) {
    int i;
    tCar_spec* car;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver >= eDriver_oppo) {
            SwitchCarModels(car, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f300
void C2_HOOK_FASTCALL ResetCarScreens(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0040f760
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime) {

    if (gAction_replay_mode
            && (gAction_replay_camera_mode == kActionReplayCameraMode_ActionTracking || gAction_replay_camera_mode == kActionReplayCameraMode_Panning)
            && gPed_actor != NULL
            && !gProgram_state.cockpit_on) {
        IncidentCam(c, pTime);
    }
}

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar) {

    pCar->field_0x18cc = 0;
    if (pCar->keys.brake || (pCar->brake_force != 0.f && fabsf(pCar->collision_info->velocity_car_space.v[2]) > 7.2463765e-05f)) {
        pCar->field_0x18cc |= 0x4;
    }
    if (pCar->gear < 4 || (!(pCar != NULL && pCar->driver == eDriver_local_human) && pCar->collision_info->velocity_car_space.v[2] > 0.f)) {
        pCar->field_0x18cc |= 0x8;
    }
}

// FUNCTION: CARMA2_HW 0x0041e5a0
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            SetTextureBits(car);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00413780
void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {

    BrVector3SetFloat(&gAverage_grid_position, 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&gAverage_grid_position, &gProgram_state.current_car.pos);
    } else {
        int i;
        br_scalar total_cars;

        total_cars = 0.0f;
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            tCar_spec* car;

            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&gAverage_grid_position, &car->pos);
            total_cars += 1.0f;
        }
        BrVector3InvScale(&gAverage_grid_position, &gAverage_grid_position, total_cars);
    }
}

// FUNCTION: CARMA2_HW 0x00420880
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref == gFace_num__car
        || (pCar->collision_info->box_face_ref == gFace_num__car - 1 && gFace_count < pCar->collision_info->box_face_start)) {
        *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
        return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041c1b0
int C2_HOOK_FASTCALL ProcessForcesCallback(void* arg1, float* arg2, int arg3) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e310
int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00414910
void C2_HOOK_FASTCALL NewFacesListCallback(tCollision_info* pCollision, undefined4 *arg2) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0041ff20
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* actor) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0041ff00
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor) {

    if (!gPHIL_doing_physics && !gTesting_car_for_sensible_place) {
        return NULL;
    }
    return DoPullActorFromWorld(actor);
}

// FUNCTION: CARMA2_HW 0x004b5970
float C2_HOOK_FASTCALL GetFrictionFromFace(void *arg1) {

    NOT_IMPLEMENTED();
    return 0.f;
}

// FUNCTION: CARMA2_HW 0x00417de0
void C2_HOOK_FAKE_THISCALL ControlCar1(tCar_spec* c, undefined4 arg2, br_scalar dt) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00417180
void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* c, undefined4 arg2, br_scalar dt) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004173b0
void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* c, undefined4 arg2, br_scalar dt) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00417a20
void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* c, undefined4 arg2, br_scalar dt) {

    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x004f8dc0
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

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pFunk_index) {

    gFunkotronics_array[pFunk_index].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x004f8e10
void C2_HOOK_FASTCALL MasterEnableCarFunks(tCar_spec* pCar_spec) {

    NOT_IMPLEMENTED();
#if 0
    int i;
#endif

    DRActorEnumRecurse(pCar_spec->car_model_actor, ActorFunks, MasterEnableFunkotronic);
#if 0
    for (i = 0; i < pCar_spec->car_crush_spec->field_0x2b0; i++) {
        ActorFunks(pCar_spec->car_crush_spec->field_0x274[i].actor, MasterEnableFunkotronic);
    }

    for (i = 0; i < pCar_spec->car_crush_spec->count_of_field_0x2b0; i++) {
        ActorFunks(pCar_spec->car_crush_spec->field_0x2b4[0].actor, MasterEnableFunkotronic);
    }
#endif
}

// FUNCTION: CARMA2_HW 0x004f9760
int C2_HOOK_FASTCALL RestorePixelmap(br_material* pMaterial) {

    if (pMaterial->colour_map != (br_pixelmap*)pMaterial->user) {
        pMaterial->colour_map = (br_pixelmap*)pMaterial->user;
        BrMaterialUpdate(pMaterial, BR_MATU_ALL);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004f9620
void C2_HOOK_FASTCALL RestoreCarPixelmaps(tCar_spec* pCar_spec) {

    MasterEnableCarFunks(pCar_spec);
    ForEveryCarMaterial(pCar_spec, RestorePixelmap, 1);
}

// FUNCTION: CARMA2_HW 0x005160f0
int C2_HOOK_FASTCALL TestForNan(float* f) {

    if (f == NULL) {
        return 0;
    }
    return isnan(*f);
}

// FUNCTION: CARMA2_HW 0x0040f510
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

// FUNCTION: CARMA2_HW 0x0040f590
void C2_HOOK_FASTCALL PositionCarMountedCamera(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0040f4a0
tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void) {

    NOT_IMPLEMENTED();
    return NULL;
}

void C2_HOOK_FASTCALL CheckCameraHither(void) {
    br_camera *cam;
    // GLOBAL: CARMA2_HW 0x0067931c
    static int old_hither;

    cam = gCamera->type_data;
    if (TestForNan(&cam->hither_z)) {
        cam->hither_z = (float)old_hither;
    }
    old_hither = (int)cam->hither_z;
}

void C2_HOOK_FASTCALL AmIGettingBoredWatchingCameraSpin(void) {
    // GLOBAL: CARMA2_HW 0x00679298
    static tU32 time_of_death;

    // GLOBAL: CARMA2_HW 0x00679320
    static tU32 headup_timer;
    char s[256];

    if (gNet_mode != eNet_mode_none
            && (gCurrent_net_game->type == eNet_game_type_4
                    || gCurrent_net_game->type == eNet_game_type_fight_to_death)) {
        if (!gRace_finished) {
            time_of_death = 0;
            gOpponent_viewing_mode = 0;
        } else if (time_of_death == 0) {
            time_of_death = GetRaceTime();
        } else if (GetRaceTime() >= time_of_death + 10000) {
            if (gOpponent_viewing_mode == 0) {
                gOpponent_viewing_mode = 1;
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNet_player_to_view_index >= gNumber_of_net_players) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if (gNet_player_to_view_index < 0 && gCar_to_view != GetRaceLeader()) {
                gNet_player_to_view_index = -2;
                ViewNetPlayer();
            }
            if ((GetRaceTime() > headup_timer + 1000 || headup_timer > GetRaceTime()) && gRace_over_reason == eRace_not_over_yet) {
                strcpy(s, GetMiscString(eMiscString_watching));
                strcat(s, " ");
                if (gNet_player_to_view_index >= 0) {
                    strcat(s, gNet_players[gNet_player_to_view_index].player_name);
                } else {
                    strcat(s, GetMiscString(eMiscString_race_leader));
                }
                headup_timer = GetRaceTime();
                NewTextHeadupSlot(6, 0, 500, -4, s);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00410be0
void C2_HOOK_FASTCALL SaveCameraPosition(int i) {

    if (gSave_camera[i].saved != 1) {
        gSave_camera[i].zoom = gCamera_zoom;
        gSave_camera[i].yaw = gCamera_yaw;
        gSave_camera[i].saved = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00410c20
void C2_HOOK_FASTCALL RestoreCameraPosition(int i) {

    if (gSave_camera[i].saved != 0) {
        gCamera_zoom = gSave_camera[i].zoom;
        gCamera_yaw = gSave_camera[i].yaw;
        gSave_camera[i].saved = 0;
    }
}

void C2_HOOK_FASTCALL DoCameraControls(tCamera_key_flags *pCamera_controls, tU32 pTime_difference) {
    int flag;
    int swirl_mode;
    int up_and_down_mode;
    int going_down;
    // GLOBAL: CARMA2_HW 0x0067930c
    static int last_swirl_mode;

    flag = 0;
    swirl_mode = !GetRuntimeVariable(99) && gRace_finished && !gAction_replay_mode && (gCar_to_view == &gProgram_state.current_car || gCar_to_view->knackered);
    up_and_down_mode = swirl_mode && !gCamera_has_collided;
    going_down = gCamera_zoom > 1.0;
    if (last_swirl_mode != swirl_mode) {
        if (swirl_mode) {
            SaveCameraPosition(0);
        } else {
            RestoreCameraPosition(0);
        }
        last_swirl_mode = swirl_mode;
    }
    if (gMap_view != 2 && !gProgram_state.cockpit_on && (!gAction_replay_mode || gAction_replay_camera_mode < kActionReplayCameraMode_Panning)) {
        if (pCamera_controls->field_0x0_bit2 || (up_and_down_mode && going_down)) {
            gCamera_zoom += (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (gCamera_zoom > 2.f) {
                gCamera_zoom = 2.f;
            }
            if (up_and_down_mode && gCamera_zoom > 1.f) {
                gCamera_zoom = 1.f;
            }
        }
        if (pCamera_controls->field_0x0_bit1 || (up_and_down_mode && !going_down)) {
            float zoom;

            gCamera_zoom -= (float)pTime_difference * 5.f / 10000.f / (float)(2 * swirl_mode + 1);
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Peds) {
                zoom = 0.001f;
            } else {
                zoom = 0.1f;
            }
            if (gCamera_zoom < zoom) {
                gCamera_zoom = zoom;
                if (up_and_down_mode) {
                    if (gCamera_zoom < 1.0f) {
                        gCamera_zoom = 1.0f;
                    }
                }
            }
        }
        if (swirl_mode && gProgram_state.current_car.speedo_speed < 0.001449275362318841) {
            pCamera_controls->field_0x0_bit4 = 0;
            pCamera_controls->field_0x0_bit3 = 1;
        }

        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw += BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            flag = 1;
        }
        if (gCamera_sign ? pCamera_controls->field_0x0_bit3 : pCamera_controls->field_0x0_bit4) {
            if (!gCamera_reset) {
                gCamera_yaw -= BrDegreeToAngle((float)pTime_difference * 0.05f);
            }
            if (flag) {
                gCamera_yaw = 0;
                gCamera_reset = 1;
            }
        } else {
            if (!flag) {
                gCamera_reset = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL MoveWithWheels(tCar_spec* pCar, br_vector3* pDir, int pManual_swing) {
    br_angle yaw;
    br_angle theta;
    // GLOBAL: CARMA2_HW 0x0058f638
    static int move_with_wheels = 1;


    if (pCar != NULL && pCar->speed <= 0.0001f && !gCamera_mode) {
        if (pManual_swing) {
            if (gCamera_yaw > BrDegreeToAngle(180)) {
                yaw = gCamera_yaw - BrDegreeToAngle(180);
            } else {
                yaw = gCamera_yaw;
            }
            if (yaw > BrDegreeToAngle(45) && yaw < BrDegreeToAngle(135)) {
                if (move_with_wheels) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    gCamera_yaw += (-2 * gCamera_sign + 1) * theta;
                    move_with_wheels = 0;
                }
            } else {
                if (!move_with_wheels) {
                    theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                    gCamera_yaw -= (-2 * gCamera_sign + 1) * theta;
                    move_with_wheels = 1;
                }
            }
        }
        if (move_with_wheels) {
            if (!gCar_flying && gAction_replay_camera_mode != kActionReplayCameraMode_Rigid) {
                theta = BrRadianToAngle(atan2f(pCar->wpos[0].v[2] * pCar->curvature, 1.f));
                DrVector3RotateY(pDir, theta);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00410c60
void C2_HOOK_FASTCALL GeneralisedPositionExternalCamera(tCar_spec* pCar, br_matrix34* pMat, br_vector3* pPos, float pSpeed, float pSpeedo_speed, br_vector3* pDirection, br_vector3* pOmega, tU32 pTime_difference) {
    br_vector3 old_camera_pos;
    br_matrix34* m1;
    int swoop;

    m1 = &gCamera->t.t.mat;
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.001f;
    BrVector3Copy(&old_camera_pos, &gCamera->t.t.translate.t);
    if (!gProgram_state.cockpit_on) {
        int manual_swing;
        float d;
        float l;
        float height_inc;
        float time;

        DoCameraControls(&gCamera_key_flags, pTime_difference);
        manual_swing = gCamera_key_flags.field_0x0_bit4 || gCamera_key_flags.field_0x0_bit3 || swoop;
        if (swoop) {
            gCamera_yaw = 0;
        }
        if (fabsf(pSpeedo_speed) > 0.0006f && gCamera_mode > 0) {
            gCamera_mode = -1;
            if (gAction_replay_camera_mode == kActionReplayCameraMode_Standard) {
                gCamera_sign = 0;
            } else if (BrVector3Dot((br_vector3*)pMat->m[2], pDirection) <= 0.f) {
                gCamera_sign = 0;
            } else {
                gCamera_sign = 1;
            }
        }
        if (pCar != NULL && pCar->frame_collision_flag && gCamera_mode != -2
                && (!gAction_replay_mode || ARReplayForwards())) {
            gCamera_mode = 1;
        }
        if (gCar_flying || gCamera_reset
                || gCamera_mode == -2 || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
            gCamera_mode = 0;
        }
        d = sqrtf(gCamera_zoom) + 4.f / WORLD_SCALE;
        if (!gCamera_mode || gCamera_mode == -1) {
            br_vector3 vn;
            br_vector3 a;

            if (gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                BrVector3Negate(&vn, (br_vector3 *) pMat->m[2]);
            } else {
                BrVector3Copy(&vn, pDirection);
            }
            MoveWithWheels(pCar, &vn, manual_swing);
            vn.v[1] = 0.0f;
            BrVector3Normalise(&vn, &vn);
            vn.v[1] = 0.0f;
            if (gCar_flying || gAction_replay_camera_mode == kActionReplayCameraMode_Rigid) {
                gCamera_sign = 0;
            }
            if (!gAction_replay_mode || !ARReplayIsReallyPaused()) {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, pTime_difference, pCar);
            } else {
                SwingCamera(pMat, m1, &vn, pOmega, pSpeed, pSpeedo_speed, 0, pCar);
            }
            BrVector3Scale(&a, &vn, d);
            BrVector3Sub(&gCamera->t.t.translate.t, pPos, &a);
            BrVector3Copy(&gView_direction, &vn);
        } else {
            gUNK_006792f4 = 0;
            gUNK_006792f8 = 0;
        }
        if (gCamera_mode == 1) {
            br_vector3 vn;
            br_vector3 a;
            br_scalar dist;
            br_scalar l;

            BrVector3Sub(&a, pPos, &old_camera_pos);
            BrVector3Copy(&old_camera_pos, &gCamera_pos_before_collide);
            a.v[1] = 0.0f;
            if (manual_swing) {
                DrVector3RotateY(&a, (gCamera_sign == 0 ? 1 : -1) * (gCamera_yaw - gOld_yaw__car));
                gCamera_yaw = gOld_yaw__car;
            }
            BrVector3Normalise(&vn, &a);
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || manual_swing
                    || BrVector3Dot((br_vector3*)pMat->m[2], &vn) < BrVector3Dot((br_vector3*)pMat->m[2], &gView_direction)) {
                BrVector3Copy(&gView_direction, &vn);
            }
            BrVector3Scale(&vn, &vn, -d);
            BrVector3Accumulate(&a, &vn);
            dist = BrVector3Length(&a);
            l = (float)pTime_difference / 1000.0f * (dist + 1.0f) / dist;
            if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard
                    && l < 1.0f && BrVector3Dot(&a, &vn) > 0.0f) {
                BrVector3Scale(&a, &a, l - 1.f);
                BrVector3Accumulate(&vn, &a);
            }
            BrVector3Add(&gCamera->t.t.translate.t, pPos, &vn);
        }

        height_inc = gCamera_zoom * gCamera_zoom + 0.3f;
        time = pTime_difference / 1000.f;
        if (!gCamera_frozen && (!gAction_replay_mode || !ARReplayIsReallyPaused())) {
            if (pTime_difference >= 5000) {
                gCamera_height = pPos->v[1];
            } else if (swoop) {
                if (time > 0.2f) {
                    time = 0.2f;
                }
                gCamera_height -= 5.0f * time;
                if (gCamera_height < pPos->v[1]) {
                    gCamera_height = pPos->v[1];
                }
            } else {
                gCamera_height += 5.0f * time * pPos->v[1];
                gCamera_height /= 5.0f * time + 1.0f;
            }
        }
        l = pDirection->v[1] * d;
        if (l > 0) {
            br_scalar new_height = pPos->v[1] - l - height_inc / 2.0f;
            if (new_height > gCamera_height) {
                gCamera_height = new_height;
            }
        }

        gCamera->t.t.translate.t.v[1] = height_inc + gCamera_height;
        BrVector3Copy(&gCamera_pos_before_collide, &gCamera->t.t.translate.t);
        CollideCameraWithOtherCars(pPos, &gCamera->t.t.translate.t);
        CollideCamera2(pPos, &gCamera->t.t.translate.t, &old_camera_pos,
            manual_swing || gCamera_key_flags.field_0x0_bit1 || gCamera_key_flags.field_0x0_bit2, pCar != NULL ? pCar->collision_info : NULL);
        if (gCamera_has_collided && swoop) {
            gCamera_height = pPos->v[1];
        }
        PointCameraAtCar(pPos, m1, 1.f);
    }
    gOld_yaw__car = gCamera_yaw;
    gOld_zoom = (int)gCamera_zoom;
}

void C2_HOOK_FASTCALL NormalPositionExternalCamera(tCar_spec* pCar, tU32 pTime_difference) {

    GeneralisedPositionExternalCamera(pCar, &pCar->car_master_actor->t.t.mat,
        &pCar->pos, pCar->speed, pCar->speedo_speed, &pCar->direction, &pCar->collision_info->omega, pTime_difference);
}

void C2_HOOK_FASTCALL SetPanningFieldOfView(void) {
    br_camera* camera_ptr;

    camera_ptr = gCamera->type_data;
    if (gPanning_camera_angle == 0) {
        gPanning_camera_angle = BrDegreeToAngle(gCamera_angle * 0.7f);
    }
    camera_ptr->field_of_view = gPanning_camera_angle;
}

// FUNCTION: CARMA2_HW 0x0040ef90
void C2_HOOK_FASTCALL FrozenCamera(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL PositionPedCam(tPed_character_instance* pPed_character, tU32 pTime) {

    if (pPed_character == NULL) {
        ChangeCameraType();
    } else {
        br_matrix34* mat;

        mat = GetCharacterMatrixPtr(pPed_character);
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3],
            0.f, 0.f, &pPed_character->field_0xc0, &gZero_v__car, pTime);
    }
}

void C2_HOOK_FASTCALL PositionDroneCam(tU32 pTime_difference) {

    if (OKToViewDrones()) {
        br_matrix34* mat;
        br_vector3* dir;

        mat = GetCurrentViewDroneMat();
        dir = GetCurrentViewDroneDirection();
        GeneralisedPositionExternalCamera(NULL, mat, (br_vector3*)mat->m[3], 0.f, 0.f, dir, &gZero_v__car, pTime_difference);
    }
}

// FUNCTION: CARMA2_HW 0x00411980
void C2_HOOK_FASTCALL SwingCamera(br_matrix34* pM1, br_matrix34* pM2, br_vector3* pVn, br_vector3* pOmega, float pSpeed, float pSpeedo_speed, tU32 pTime_difference, tCar_spec* pCar) {
    br_angle yaw;
    br_scalar cos_dtheta;
    br_scalar sign;
    int manual_swing;
    br_scalar v16;
    br_scalar v17;
    br_scalar v18;
    br_scalar abs_v18;
    br_angle v8;
    br_angle v9;
    // GLOBAL: CARMA2_HW 0x0058f63c
    static int elapsed_time = -1;
    // GLOBAL: CARMA2_HW 0x00679338
    static br_vector3 old_vn;

    manual_swing = gOld_yaw__car != gCamera_yaw;
    if (elapsed_time > 500) {
        elapsed_time = -1;
    }
    if (elapsed_time >= 0) {
        elapsed_time += pTime_difference;
    }
    sign = -BrVector3Dot((br_vector3*)pM1->m[2], pVn);

    if ((sign < 0.0f) == gCamera_sign) {
        elapsed_time = -1;
    } else if (BrVector3Dot(pVn, &old_vn) <= 0.0 || elapsed_time >= 0) {
        if (gAction_replay_camera_mode != kActionReplayCameraMode_Standard || gCamera_sign) {
            if (elapsed_time < 0) {
                elapsed_time = 0;
            }
            if (elapsed_time < 500 && sign <= 0.0f) {
                BrVector3Negate(pVn, pVn);
            } else {
                gCamera_sign = !gCamera_sign;
                gUNK_006792f4 = BrDegreeToAngle(200.f * (gUNK_006792f8 ? .06f : .04f));

                if (gCamera_yaw > BR_ANGLE_DEG(180)) {
                    yaw = gCamera_yaw - BR_ANGLE_DEG(180);
                } else {
                    yaw = gCamera_yaw;
                }
                if (gCamera_yaw + BR_ANGLE_DEG(90) > BR_ANGLE_DEG(180)) {
                    gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                } else if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
                    gCamera_yaw = BR_ANGLE_DEG(180) - gCamera_yaw;
                }
            }
        }
    } else {
        gCamera_sign = !gCamera_sign;
        if (gCamera_yaw > BR_ANGLE_DEG(180)) {
            yaw = gCamera_yaw - BR_ANGLE_DEG(180);
        } else {
            yaw = gCamera_yaw;
        }
        if (yaw > BR_ANGLE_DEG(45) && yaw < BR_ANGLE_DEG(135)) {
            gCamera_yaw = -gCamera_yaw;
        }
    }
    BrVector3Copy(&old_vn, pVn);
    if (gCamera_sign) {
        yaw = -gCamera_yaw;
    } else {
        yaw = gCamera_yaw;
    }
    if (!gCar_flying) {
        DrVector3RotateY(pVn, yaw);
    }
    v16 = pVn->v[0] * gView_direction.v[0] + pVn->v[2] * gView_direction.v[2];
    v17 = pVn->v[0] * gView_direction.v[2] - pVn->v[2] * gView_direction.v[0];

    if (v16 < 0.5f && gCamera_yaw == 0) {
        gUNK_006792f8 = 1;
    }

    v18 = pOmega->v[0] * pM1->m[0][1] + pOmega->v[1] * pM1->m[1][1] + pOmega->v[2] * pM1->m[2][1];
    abs_v18 = fabsf(v18);
    v8 = BrRadianToAngle((float)pTime_difference * (abs_v18 + REC2_PI_F / 36.f) / 1000.f);
    v9 = BrRadianToAngle((float)sqrt(fabsf(v17)));

    if (!(gUNK_006792f4 == 0 && v16 > 0.f && v9 < v8) && !gCar_flying && !manual_swing) {
        br_angle omega;

        if (gUNK_006792f4 == 0) {
            gUNK_006792f4 = BrDegreeToAngle((gUNK_006792f8 ? .06f : .04f) * 50.f);
        }
        omega = pTime_difference * gUNK_006792f4 / 100;
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
                pVn->v[0] = sinf(ts) * gView_direction.v[2] + cosf(ts) * gView_direction.v[0];
                pVn->v[2] = cosf(ts) * gView_direction.v[2] - sinf(ts) * gView_direction.v[0];
            } else {
                pVn->v[0] = cosf(ts) * gView_direction.v[0] - sinf(ts) * gView_direction.v[2];
                pVn->v[2] = sinf(ts) * gView_direction.v[0] + cosf(ts) * gView_direction.v[2];
            }
            gUNK_006792f4 += BrDegreeToAngle(pTime_difference * (gUNK_006792f8 ? .06f : .04f));

            if (gUNK_006792f4 > BrDegreeToAngle(gUNK_006792f8 ? 40.f : 10.f)) {
                gUNK_006792f4 = BrDegreeToAngle(gUNK_006792f8 ? 40.f : 10.f);
            }
            return;
        }
    }
    gUNK_006792f8 = 0;
    gCamera_mode = 0;
    gUNK_006792f4 = 0;
}

// FUNCTION: CARMA2_HW 0x00413570
void C2_HOOK_FASTCALL CollideCameraWithOtherCars(br_vector3* pPos, br_vector3* pCamera_pos) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00411fc0
void C2_HOOK_FASTCALL PointCameraAtCar(br_vector3* pPos, br_matrix34* pMat, float pFov_factor) {
    br_vector3 vn;
    br_vector3 tv;
    br_vector3 tv2;
    br_scalar dist;
    br_scalar frac;
    br_angle theta;
    br_vector3* pos;
    br_camera* camera_ptr;
    int swoop;

    camera_ptr = gCamera->type_data;
    theta = (br_angle)(pFov_factor * camera_ptr->field_of_view / 5.f);
    swoop = gCountdown && gCamera_height > pPos->v[1] + 0.01f;
    if (swoop) {
        BrVector3Sub(&tv, &gAverage_grid_position, pPos);
        frac = (gCamera_height - pPos->v[1]) / 10.0f;
        BrVector3Scale(&tv, &tv, frac);
        BrVector3Add(&tv, pPos, &tv);
        pos = &tv;
        theta = (br_angle)((1.0f - frac) * (float)theta);
    } else {
        pos = pPos;
    }
    BrVector3Sub(&vn, pPos, (br_vector3*)pMat->m[3]);
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
}

// FUNCTION: CARMA2_HW 0x00414ca0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gInTheSea;
}

// FUNCTION: CARMA2_HW 0x0043b840
float C2_HOOK_FASTCALL RepairCar(tU16 pCar_ID, tU32 pFrame_period, br_scalar* pTotal_deflection) {

    NOT_IMPLEMENTED();
    return 0.f;
}

// FUNCTION: CARMA2_HW 0x0041e580
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {

    gQuite_wild_end = 0;
    gQuite_wild_start = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}

// FUNCTION: CARMA2_HW 0x004157e0
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = BrRadianToDegree(atanf((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE;
    pCar->rr_sus_position = (pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE;
    pCar->lf_sus_position = (pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE;
    pCar->rf_sus_position = (pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE;
    PipeSingleGraphicalWheelStuff(pCar);
}

// FUNCTION: CARMA2_HW 0x00416500
void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime) {
    int i;

    if (gCar_flying) {
        BrMatrix34Copy(&gCar_to_view->collision_info->actor->t.t.mat,
            &gCar_to_view->collision_info->transform_matrix);
    }
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;
        float original_speed;
        br_vector3 minus_k;

        car = gActive_car_list[i];
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
                if (car->oldd[wheel] < car->susp_height[wheel / 2] && gCurrent_race.material_modifiers[car->material_index[wheel]].smoke_type >= 2 && !car->collision_info->disable_move_rotate) {
                    GenerateContinuousSmoke(car, wheel, pTime);
                }
            }
        }
    }
    if (pLast_frame_time < gPHIL_last_physics_tick && gCar_to_view->speed > .0001f) {
        br_vector3 tv;
        br_scalar dt;

        dt = (gPHIL_last_physics_tick - pLast_frame_time) / 40.f;
        BrVector3Sub(&tv, &gCar_to_view_original_v, &gCar_to_view->collision_info->v);
        BrVector3Scale(&tv, &tv, dt);
        BrVector3Accumulate(&tv, &gCar_to_view->collision_info->v);
        BrVector3Normalise(&gCar_to_view->direction, &tv);
    }
}

// FUNCTION: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tCollision_info* pCollision_info, void* pUser_data) {

    NOT_IMPLEMENTED();
    return 0;
}

void C2_HOOK_FASTCALL PipeNonCars(void) {
    int i;

    ARStartPipingSession(ePipe_chunk_non_car);
    for (i = 0; i < gNum_active_non_cars; i++) {
        tNon_car_spec* non_car;

        non_car = gActive_non_car_list[i];
        if (non_car->car_ID != -1) {
            PhysicsObjectRecurse(non_car->collision_info, PipeNonCarObject, NULL);
        }
    }
    AREndPipingSession();
}

// FUNCTION: CARMA2_HW 0x004203c0
void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime) {
    // GLOBAL: CARMA2_HW 0x00679418
    static tU32 total_time;
    int new_count;

    new_count = 0;
    if (gNum_active_non_cars != 0) {
        PipeNonCars();
        total_time += pTime;
        if (total_time >= 1000) {
            int i;

            total_time = 0;
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* non_car;

                non_car = gActive_non_car_list[i];

                if (non_car->actor->t.t.translate.t.v[1] < gMin_world_y - 10.f) {
                    non_car->collision_info->disable_move_rotate = 1;
                }
                if (TestForNan(&non_car->actor->t.t.translate.t.v[1])) {
                    BrVector3Set(&non_car->collision_info->omega, 0.f, 0.f, 0.f);
                    BrMatrix34Identity(&non_car->actor->t.t.mat);
                    BrVector3Set(&non_car->actor->t.t.translate.t, 2000.f, 0.f, 0.f);
                    non_car->collision_info->disable_move_rotate = 1;
                }
                gActive_non_car_list[new_count] = non_car;
                if (non_car->collision_info->disable_move_rotate && non_car->driver == eDriver_4) {
                    int j;
                    int drop;
                    br_actor *non_car_actor;

                    drop = 1;
                    non_car_actor = non_car->actor;
                    for (j = 0; j < gNum_cars_and_non_cars; j++) {
                        tCar_spec *car;

                        car = gActive_car_list[j];
                        if (car != (tCar_spec*)non_car && !car->collision_info->disable_move_rotate) {
                            br_matrix34 mat;
                            br_bounds3 bb;

                            BrMatrix34Mul(&mat, &non_car_actor->t.t.mat, &car->collision_info->field_0x144);
                            GetNewBoundingBox(&bb, &non_car_actor->model->bounds, &mat);
                            if (!(bb.min.v[0] > car->collision_info->field_0x124.max.v[0]
                                  || bb.min.v[1] > car->collision_info->field_0x124.max.v[1]
                                  || bb.min.v[2] > car->collision_info->field_0x124.max.v[2]
                                  || car->collision_info->field_0x124.min.v[0] > bb.max.v[0]
                                  || car->collision_info->field_0x124.min.v[1] > bb.max.v[1]
                                  || car->collision_info->field_0x124.min.v[2] > bb.max.v[2])) {
                                drop = 0;
                                break;
                            }
                        }
                    }
                    if (drop) {
                        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x17c, 0x17c);

                        if (non_car->flags & 0x10000) {
                            if (Vector3DistanceSquared(&non_car->collision_info->field_0x17c,
                                    (br_vector3*)non_car->collision_info->transform_matrix.m[3]) > REC2_SQR(.005f)) {
                                drop = 0;
                            } else {
                                BrVector3Copy((br_vector3 *) &non_car->collision_info->transform_matrix.m[3],
                                    &non_car->collision_info->field_0x17c);
                            }
                        }
                        if ((tCar_spec*)non_car != gCar_to_view) {
                            BrActorRemove(non_car_actor);
                            ClearSplashes(non_car->collision_info);
                            new_count -= 1;
                            non_car->driver = eDriver_non_car_unused_slot;
                            if (non_car->car_ID != -1) {
                                tU8 col_x;
                                tU8 col_z;
                                br_actor *parent;

                                XZToColumnXZ(&col_x, &col_z, non_car_actor->t.t.mat.m[3][0],
                                    non_car_actor->t.t.mat.m[3][2], &gProgram_state.track_spec);
                                parent = gTrack_actor;
                                if (gProgram_state.track_spec.columns[col_z][col_x].actor_0x0 != NULL) {
                                    parent = gProgram_state.track_spec.columns[col_z][col_x].actor_0x0;
                                }
                                BrActorAdd(parent, non_car_actor);
                            } else if (gAdditional_actors != NULL) {
                                BrActorAdd(gAdditional_actors, non_car_actor);
                            }
                            non_car_actor->type_data = NULL;
                            PHILRemoveObject(non_car->collision_info);
                        }
                    }
                }
                new_count += 1;
            }
            gNum_active_non_cars = new_count;
        }
    }
}

void C2_HOOK_FASTCALL PrepareCars(tU32 pFrame_start_time) {
    int i;
    // GLOBAL: CARMA2_HW 0x006793f4
    static tU32 last_frame_start;

    (void)last_frame_start;

    last_frame_start = pFrame_start_time;
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        car->frame_collision_flag = gOver_shoot && car->collision_info->collision_flag;
        if (car != NULL && car->driver >= eDriver_oppo) {
            RecordLastDamage(car);
            if (car->driver == eDriver_oppo && gStop_opponents_moving) {
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

    if (gLast_car_to_skid[0] == pC) {
        DRS3StopSound(gSkid_tag[0]);
    }
    if (gLast_car_to_skid[1] == pC) {
        DRS3StopSound(gSkid_tag[1]);
    }
}

// FUNCTION: CARMA2_HW 0x00415890
void C2_HOOK_FASTCALL APTCPreCollision(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00416070
void C2_HOOK_FASTCALL APTCPostCollision(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00416300
void C2_HOOK_FASTCALL APTCChangedObjects(tCollision_info* pArg1, undefined4 pArg2) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00416270
int C2_HOOK_FASTCALL APTCActiveHalted(tCollision_info* pArg1) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004162b0
int C2_HOOK_FASTCALL APTCPassiveActivated(tCollision_info* pArg1) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00515c20
float C2_HOOK_STDCALL frac(float pN) {

    return pN - (float)(int)pN;
}

// FUNCTION: CARMA2_HW 0x00418230
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}
