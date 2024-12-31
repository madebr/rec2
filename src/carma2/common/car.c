#include "car.h"

#include "drone.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "opponent.h"
#include "physics.h"
#include "replay.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"
#include "brender/br_inline_funcs.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_simplification_level, 0x006793d8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNon_car_spec*, gActive_non_car_list, 99, 0x0079eda0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNum_active_non_cars, 0x006793dc);
C2_HOOK_VARIABLE_IMPLEMENT(br_scalar, gMin_world_y, 0x00679360);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gUnknown_car_collision_info, 0x006793e4);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gAverage_grid_position, 0x00679268);

C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0067939c, 0x0067939c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_callbacks, gCar_callbacks, 0x0065cf78, {
    ProcessForcesCallback,
    ProcessJointForcesCallback,
    NewFacesListCallback,
    FindFacesInBox,
    PullActorFromWorld,
    StopGroovidelic,
    GetFrictionFromFace,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFace_count, 0x006940b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tFace_ref, gFace_list__car, 300, 0x00744820);

void (C2_HOOK_FASTCALL * SetUpPanningCamera_original)(tCar_spec* c);
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {

#if defined(C2_HOOKS_ENABLED)
    SetUpPanningCamera_original(c);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004105f0, SetUpPanningCamera, SetUpPanningCamera_original)

int (C2_HOOK_FASTCALL * CollideCamera2_original)(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info) {

#if defined(C2_HOOKS_ENABLED)
    return CollideCamera2_original(car_pos, cam_pos, old_camera_pos, manual_move, collision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004122b0, CollideCamera2, CollideCamera2_original)

void (C2_HOOK_FAKE_THISCALL * FlyCar_original)(tCar_spec* c, undefined4 pArg2, br_scalar dt);
void C2_HOOK_FAKE_THISCALL FlyCar(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    REC2_THISCALL_UNUSED(pArg2);

#if defined(C2_HOOKS_ENABLED)
    FlyCar_original(c, pArg2, dt);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f4f0, FlyCar, FlyCar_original)

void (C2_HOOK_FAKE_THISCALL * SetCarSuspGiveAndHeight_original)(tCar_spec* pCar, undefined4 pArg2, br_scalar pFront_give_factor, br_scalar pRear_give_factor, br_scalar pDamping_factor, br_scalar pExtra_front_height, br_scalar pExtra_rear_height);
void C2_HOOK_FAKE_THISCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, undefined4 pArg2, br_scalar pFront_give_factor, br_scalar pRear_give_factor, br_scalar pDamping_factor, br_scalar pExtra_front_height, br_scalar pExtra_rear_height) {

#if defined(C2_HOOKS_ENABLED)
    SetCarSuspGiveAndHeight_original(pCar, pArg2, pFront_give_factor, pRear_give_factor, pDamping_factor, pExtra_front_height, pExtra_rear_height);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041fc60, SetCarSuspGiveAndHeight, SetCarSuspGiveAndHeight_original)

int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec *pCar_spec, br_vector3 *pVec3) {
    int r;

    if (!C2V(gProgram_state).racing) {
        return 1;
    }
    C2V(gINT_0067939c) = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info,
        C2V(gList_collision_infos),
        pVec3,
        &C2V(gCar_callbacks));
    C2V(gINT_0067939c) = 0;
    return r;
}
C2_HOOK_FUNCTION(0x0041fe50, TestForCarInSensiblePlace)

void (C2_HOOK_FASTCALL * PanningExternalCamera_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PanningExternalCamera_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004104b0, PanningExternalCamera, PanningExternalCamera_original)

int (C2_HOOK_FASTCALL * IncidentCam_original)(tCar_spec* c, tU32 pTime);
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return IncidentCam_original(c, pTime);
#else
#error "Not implemented"
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

#if defined(C2_HOOKS_ENABLED)
    SetInitialPosition_original(pThe_race, pCar_index, pGrid_index);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00414510, SetInitialPosition, SetInitialPosition_original)

void (C2_HOOK_FASTCALL * InitialiseCar2_original)(tCar_spec* pCar, int pClear_disabled_flag);
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {

#if defined(C2_HOOKS_ENABLED)
    InitialiseCar2_original(pCar, pClear_disabled_flag);
#else
#error "Not implemented"
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
    if (C2V(gAction_replay_camera_mode) == tActionReplayCameraMode_Manual) {
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

#if defined(C2_HOOKS_ENABLED)
    InitialiseExternalCamera_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00413580, InitialiseExternalCamera, InitialiseExternalCamera_original)

void (C2_HOOK_FASTCALL * MungeCarGraphics_original)(tU32 pFrame_period);
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    MungeCarGraphics_original(pFrame_period);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041e660, MungeCarGraphics, MungeCarGraphics_original)

void (C2_HOOK_FASTCALL * DoLODCarModels_original)(void);
void C2_HOOK_FASTCALL DoLODCarModels(void) {

#if defined(C2_HOOKS_ENABLED)
    DoLODCarModels_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f110, DoLODCarModels, DoLODCarModels_original)

void (C2_HOOK_FASTCALL * DoComplexCarModels_original)(void);
void C2_HOOK_FASTCALL DoComplexCarModels(void) {

#if defined(C2_HOOKS_ENABLED)
    DoComplexCarModels_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f2a0, DoComplexCarModels, DoComplexCarModels_original)

void (C2_HOOK_FASTCALL * ResetCarScreens_original)(void);
void C2_HOOK_FASTCALL ResetCarScreens(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetCarScreens_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0041f300, ResetCarScreens, ResetCarScreens_original)

void (C2_HOOK_FASTCALL * CameraBugFix_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    CameraBugFix_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f760, CameraBugFix, CameraBugFix_original)

void (C2_HOOK_FASTCALL * MungeSomeOtherCarGraphics_original)(void);
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeSomeOtherCarGraphics_original();
#else
#error "Not implemented"
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
