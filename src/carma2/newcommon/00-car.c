/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "00-car.h"

#include "01-network.h"
#include "04-netgame.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "17-world2.h"
#include "18-graphics2.h"

#include "22-replay.h"
#include "23-piping.h"
#include "29-raycast.h"

#include "30-opponent.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "49-physics.h"
#include "58-crush2.h"
#include "60-phil.h"
#include "brender/brender.h"
#include "s3/s3.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"

#include "c2_math.h"
#include "c2_stdio.h"
#include "rec2_macros.h"

#include <math.h>

extern void C2_HOOK_FASTCALL AddCollisionInfoChild(tPhysics_object* pParent, tPhysics_object* pChild);
extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);
extern void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL ResetRecoveryVouchers(void);
extern void C2_HOOK_FASTCALL RemoveFromCloakingList(tCar_spec* pCar);
extern void C2_HOOK_FASTCALL TurnOffCloaking(tPowerup* pPowerup, tCar_spec* pCar);
extern tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
extern tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);
extern void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);
extern br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount);
extern void C2_HOOK_FASTCALL StartDinputEffect(int pIndex);
extern int C2_HOOK_FASTCALL BonerActiveHalted(tPed_character_instance* pCharacter);
extern int C2_HOOK_FASTCALL BonerPassiveCollision(tPed_character_instance* pCharacter, undefined4 pArg2);
extern void C2_HOOK_FASTCALL BonerPedMovedByPhysics(tPed_character_instance* pCharacter, undefined4 pArg2);
extern int C2_HOOK_FASTCALL MyDroneHathHalteth(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther);
extern void C2_HOOK_FASTCALL MoveMyDroneBaby(tPhysics_object* pObject, undefined4 pArg2);
extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL PipeSingleNonCarCreation2(uintptr_t pId, br_actor* pActor);
/* declared here rather than through 13-crush1.h / 23-piping.h: those headers belong to other
   feature areas, and this file must compile in a branch that does not carry them */
extern void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
extern void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);
extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);
extern tSlot_info gInitial_APO[3];
extern tSlot_info gInitial_APO_potential[3];
extern int gCamera_type;
extern tActionReplayCameraMode gAction_replay_camera_mode;
extern br_scalar gFLOAT_00589a2c;
extern int gStop_opponents_moving;
extern int gReplay_splash_flags;
extern tReplay_splash gReplay_splashes[10];
extern const br_scalar gFLOAT_0058b008;
extern tS3_outlet* gCar_outlet;

// GLOBAL: CARMA2_HW 0x00679360
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
tPhysics_object* gUnknown_car_collision_info;

// GLOBAL: CARMA2_HW 0x0058f680
int gMetal_crunch_sound_id[5] = { 5000, 5001, 5002, 5003, 5004 };

// GLOBAL: CARMA2_HW 0x0079eda0
// GLOBAL: CARMA2_SW 0x00795340
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x0067939c
// GLOBAL: CARMA2_SW 0x00673b0c
int gTesting_car_for_sensible_place;

// GLOBAL: CARMA2_HW 0x00763500
// GLOBAL: CARMA2_SW 0x00759a60
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
// GLOBAL: CARMA2_SW 0x00743720
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f5c
// GLOBAL: CARMA2_SW 0x007584bc
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x00761f60
// GLOBAL: CARMA2_SW 0x007584c0
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
// GLOBAL: CARMA2_SW 0x007582a0
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
// GLOBAL: CARMA2_SW 0x00751f80
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
// GLOBAL: CARMA2_SW 0x00743b60
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
// GLOBAL: CARMA2_SW 0x00685e30
br_vector3 gZero_v__car;  // FIXME: make const?

// GLOBAL: CARMA2_HW 0x006793a0
// GLOBAL: CARMA2_SW 0x00673b10
int gFreeze_mechanics;

// GLOBAL: CARMA2_HW 0x006793dc
// GLOBAL: CARMA2_SW 0x00673b4c
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x006793d0
// GLOBAL: CARMA2_SW 0x00673b40
int gEliminate_faces;

// GLOBAL: CARMA2_HW 0x00679364
// GLOBAL: CARMA2_SW 0x00673ad4
int gInTheSea;

// GLOBAL: CARMA2_HW 0x0065d010
// GLOBAL: CARMA2_SW 0x00657948
int gFace_num__car = 1;

// GLOBAL: CARMA2_HW 0x00744820
// GLOBAL: CARMA2_SW 0x0073b2e0
tFace_ref gFace_list__car[300];

// FUNCTION: CARMA2_HW 0x00413cc0
// FUNCTION: CARMA2_SW 0x00413cf0
void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount) {
    tDamage_unit* the_damage;

    if (pDamage_amount <= 0) {
        return;
    }
    the_damage = &pCar->damage_units[pUnit_type];
    the_damage->damage_level += pDamage_amount;
    if (the_damage->damage_level >= 100) {
        the_damage->damage_level = 99;
    }
}

// FUNCTION: CARMA2_HW 0x00413d00
// FUNCTION: CARMA2_SW 0x00413d30
void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_engine, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d30
// FUNCTION: CARMA2_SW 0x00413d60
void C2_HOOK_FASTCALL DamageTrans(int pAmount) {
    DamageUnit(&gProgram_state.current_car, eDamage_transmission, pAmount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413d90
// FUNCTION: CARMA2_SW 0x00413dc0
void C2_HOOK_FASTCALL DamageLFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413dc0
// FUNCTION: CARMA2_SW 0x00413df0
void C2_HOOK_FASTCALL DamageLFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413df0
// FUNCTION: CARMA2_SW 0x00413e20
void C2_HOOK_FASTCALL DamageLRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e20
// FUNCTION: CARMA2_SW 0x00413e50
void C2_HOOK_FASTCALL DamageLRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_lr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e50
// FUNCTION: CARMA2_SW 0x00413e80
void C2_HOOK_FASTCALL DamageRFWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413e80
// FUNCTION: CARMA2_SW 0x00413eb0
void C2_HOOK_FASTCALL DamageRFBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rf_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413eb0
// FUNCTION: CARMA2_SW 0x00413ee0
void C2_HOOK_FASTCALL DamageRRBrake(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_brake, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413ee0
// FUNCTION: CARMA2_SW 0x00413f10
void C2_HOOK_FASTCALL DamageRRWheel(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_rr_wheel, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413f10
// FUNCTION: CARMA2_SW 0x00413f40
intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {

    if (pActor->user != NULL) {
        int* index = pData;
        tUser_crush_data* crush_data = pActor->user;
        br_model* model = crush_data->models[*index];

        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00413f40
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
}

// FUNCTION: CARMA2_HW 0x00413f70
void C2_HOOK_FASTCALL InitialiseCar2(tCar_spec* pCar, int pClear_disabled_flag) {
    int index;
    int j;
    int level;
    br_matrix34 safe_position;
    tNet_game_player_info* net_player;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x261, 0x261);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, message_time, 0x268);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0x49c, 0x49c);

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
    BrMatrix34Copy(&safe_position, &pCar->car_master_actor->t.t.mat);
    if (safe_position.m[3][0] > 500.0f) {
        BrVector3Sub((br_vector3*)safe_position.m[3], (br_vector3*)safe_position.m[3], &gInitial_position);
    }
    BrMatrix34Copy(&pCar->old_frame_mat, &safe_position);
    BrMatrix34Copy(&pCar->collision_info->transform_matrix, &safe_position);
    BrMatrix34ApplyP(&pCar->pos, &pCar->collision_info->cmpos, &pCar->collision_info->transform_matrix);
    for (j = 0; j < 4; j++) {
        pCar->oldd[j] = pCar->ride_height;
    }
    pCar->gear = 0.f;
    pCar->revs = 0.f;
    pCar->traction_control = 1;
    BrVector3Negate(&pCar->direction, (br_vector3*)pCar->car_master_actor->t.t.mat.m[2]);
    for (j = 0; j < 20; j++) {
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
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
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
                if (pCar == gCurrent_race.opponent_list[j].car_spec) {
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
    if (pCar == NULL || pCar->driver != eDriver_local_human) {
        pCar->joystick.left = -1;
        pCar->joystick.right = -1;
    }
    TotallyRepairACar(pCar);
    SetCarSuspGiveAndHeight(pCar, 1.f, 1.f, 1.f, 0.f, 0.f);
    for (j = 0; j < 100; j++) {
        pCar->powerups[j] = 0;
    }
    if (gNet_mode != eNet_mode_none && (net_player == NULL || !net_player->field_0x80)) {
        for (j = 0; j < 3; j++) {
            if (gNet_mode != eNet_mode_none) {
                level = gInitial_APO[j].initial_network[gCurrent_net_game->type];
            } else {
                level = gInitial_APO[j].initial[gProgram_state.skill_level];
            }
            pCar->power_up_levels[j] = level;
            if (gNet_mode != eNet_mode_none) {
                level = gInitial_APO_potential[j].initial_network[gCurrent_net_game->type];
            } else {
                level = gInitial_APO_potential[j].initial[gProgram_state.skill_level];
            }
            pCar->power_up_slots[j] = level;
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

    gProgram_state.initial_position = pThe_race->initial_position;
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
                InitialiseCar2(car, 1);
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

// FUNCTION: CARMA2_HW 0x00414510
// FUNCTION: CARMA2_SW 0x00414540
void C2_HOOK_FASTCALL SetInitialPosition(tRace_info* pThe_race, int pCar_index, tU32 pGrid_index) {
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
    tCompressed_matrix3 compressed;
    int inactive;

    car = pThe_race->opponent_list[pCar_index].car_spec;
    car_actor = car->car_master_actor;
    BrMatrix34Identity(&car_actor->t.t.mat);
    place_on_grid = 1;
    if (gNet_mode != eNet_mode_none && !gCurrent_net_game->options.grid_start && pThe_race->count_network_start_points != 0) {
        start_i = i = IRandomBetween(0, pThe_race->count_network_start_points - 1);
        do {
            PossibleService();
            for (j = 0; j < gNumber_of_net_players; j++) {
                if (j != pCar_index) {
                    BrVector3Copy(&real_pos, &pThe_race->opponent_list[j].car_spec->car_master_actor->t.t.translate.t);
                    if (real_pos.v[0] > 500.0f) {
                        BrVector3Sub(&real_pos, &real_pos, &gInitial_position);
                    }
                    BrVector3Sub(&dist, &real_pos, &pThe_race->net_starts[i].pos);
                    if (BrVector3LengthSquared(&dist) < 16.0f) {
                        break;
                    }
                }
            }
            if (j == gNumber_of_net_players) {
                BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[i].pos);
                initial_yaw = BrDegreeToAngle(pThe_race->net_starts[i].yaw);
                place_on_grid = 0;
                break;
            }
            i++;
            if (i == pThe_race->count_network_start_points) {
                i = 0;
            }
        } while (i != start_i);
    }
    if (gNet_mode == eNet_mode_none && (int)pGrid_index < 0) {
        BrVector3Copy(&car_actor->t.t.translate.t, &pThe_race->net_starts[-((int)pGrid_index + 1)].pos);
        initial_yaw = BrDegreeToAngle(pThe_race->net_starts[-((int)pGrid_index + 1)].yaw);
        place_on_grid = 0;
    }
    if (place_on_grid) {
        initial_yaw = BrDegreeToAngle(pThe_race->initial_yaw);
        BrMatrix34RotateY(&initial_yaw_matrix, initial_yaw);
        grid_offset.v[0] = -(br_scalar)((int)pGrid_index % 2);
        grid_offset.v[1] = 0.0f;
        grid_offset.v[2] = (br_scalar)((int)pGrid_index % 2) * 0.4f - (br_scalar)((int)pGrid_index / 2) * -2.0f;
        BrMatrix34ApplyV(&car_actor->t.t.translate.t, &grid_offset, &initial_yaw_matrix);
        BrVector3Accumulate(&car_actor->t.t.translate.t, &pThe_race->initial_position);
    }
    if (gTrack_actor != NULL) {
        FindBestY(&car_actor->t.t.translate.t, gTrack_actor, 10.0f, &nearest_y_above, &nearest_y_below, &above_model, &below_model, &above_face_index, &below_face_index);
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
        CompressMatrix34(&compressed, &inactive, &car_actor->t.t.mat);
        ExpandMatrix34(&car_actor->t.t.mat, &compressed, inactive);
        BrMatrix34Copy(&gNet_players[pThe_race->opponent_list[pCar_index].net_player_index].initial_position, &car->car_master_actor->t.t.mat);
    }
    if (gNet_mode != eNet_mode_none && car->disabled && car_actor->t.t.translate.t.v[0] < 500.0f) {
        DisableCar(car);
    }
}

// FUNCTION: CARMA2_HW 0x004148d0
// FUNCTION: CARMA2_SW 0x00414900
void C2_HOOK_FASTCALL SetInitialPositions(tRace_info* pThe_race) {
    int i;

    for (i = 0; i < pThe_race->number_of_racers; i++) {
        int grid_index = pThe_race->opponent_list[i].ranking;

        if (grid_index >= 0) {
            grid_index = i;
        }
        SetInitialPosition(pThe_race, i, grid_index);
    }
}

// InitialiseNonCar

// NewFaceListCallBack

// FUNCTION: CARMA2_HW 0x00414ca0
// FUNCTION: CARMA2_SW 0x00414cd0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gInTheSea;
}

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// FUNCTION: CARMA2_HW 0x004157e0
// FUNCTION: CARMA2_SW 0x00415810
void C2_HOOK_FASTCALL CalcGraphicalWheelStuff(tCar_spec* pCar) {

    pCar->steering_angle = (br_scalar)BrRadianToDegree(atan((pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (pCar->field_0x1260 + pCar->curvature)));
    pCar->lr_sus_position = (br_scalar)((pCar->wpos[0].v[1] - pCar->oldd[0]) / WORLD_SCALE_D);
    pCar->rr_sus_position = (br_scalar)((pCar->wpos[1].v[1] - pCar->oldd[1]) / WORLD_SCALE_D);
    pCar->lf_sus_position = (br_scalar)((pCar->wpos[2].v[1] - pCar->oldd[2]) / WORLD_SCALE_D);
    pCar->rf_sus_position = (br_scalar)((pCar->wpos[3].v[1] - pCar->oldd[3]) / WORLD_SCALE_D);
    PipeSingleGraphicalWheelStuff(pCar);
}

// FinishCars

// GetNonCars

// GetNetPos

// MungeCarsMass

// AddDrag

// FUNCTION: CARMA2_HW 0x00416030
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pChild) {

    while (pChild != NULL) {
        AddDrag(pCar, pChild, 0.04f);
        if (pChild->child != NULL) {
            DragChildren(pCar, pChild->child);
        }
        pChild = pChild->next;
    }
}

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

// FUNCTION: CARMA2_HW 0x004161a0
// FUNCTION: CARMA2_SW 0x004161d0
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tPhysics_object* pObject) {
    tPhysics_object* child;
    tCar_spec* car;
    int collided;

    for (; pObject != NULL; pObject = pObject->next) {
        if (gNet_mode == eNet_mode_host) {
            collided = (pObject->collision_flag & 0x2) != 0 || (pObject->flags & 0x1000) != 0;
            for (child = pObject->child; child != NULL; child = child->next) {
                collided |= child->collision_flag & 0x2;
            }
            if (collided) {
                pObject->field_0x49c = gPHIL_last_physics_tick + 40;
                if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
                    car = pObject->owner;
                    if (car->driver > 5 && car->car_crush_spec != NULL) {
                        SetAllShapeStatusFlags(car->car_crush_spec->network_stuff);
                    }
                }
            }
        }
        pObject->disable_move_rotate &= ~0x2;
        SetCollisionFlagsAndStuff(pObject->child);
    }
}

// APTCPostCollision

// FUNCTION: CARMA2_HW 0x00416270
int C2_HOOK_FASTCALL APTCActiveHalted(tPhysics_object* pObject) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerActiveHalted((tPed_character_instance*)pObject->owner);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathHalteth(pObject);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004162b0
int C2_HOOK_FASTCALL APTCPassiveActivated(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        return BonerPassiveCollision((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        return MyDroneHathCollideth(pObject, (tPhysics_object*)pArg2);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00416300
void C2_HOOK_FASTCALL APTCChangedObjects(tPhysics_object* pObject, undefined4 pArg2) {

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 2) {
        BonerPedMovedByPhysics((tPed_character_instance*)pObject->owner, pArg2);
    }
    if (pObject != NULL && pObject->flags_0x238 == 0x10) {
        MoveMyDroneBaby(pObject, pArg2);
    }
}

// ApplyPhysicsToCars

// MoveAndCollideCar

// FUNCTION: CARMA2_HW 0x00417030
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {

    if (pNon_car->collision_info->water_d != 10000.f) {
        TestAutoSpecialVolume(pNon_car->collision_info);
    }
    MungeSpecialVolume(pNon_car->collision_info);
    if (((tCar_spec*)pNon_car)->dt >= 0.f) {
        pDt = ((tCar_spec*)pNon_car)->dt;
    }
    MoveNonCar(pNon_car, pDt);
    BrMatrix34ApplyP(&((tCar_spec*)pNon_car)->pos, &((tCar_spec*)pNon_car)->centre_of_mass_world_scale, &pNon_car->actor->t.t.mat);
    ((tCar_spec*)pNon_car)->pos.v[0] = ((tCar_spec*)pNon_car)->pos.v[0] * 0.14492753f;
    ((tCar_spec*)pNon_car)->pos.v[1] = ((tCar_spec*)pNon_car)->pos.v[1] * 0.14492753f;
    ((tCar_spec*)pNon_car)->pos.v[2] = ((tCar_spec*)pNon_car)->pos.v[2] * 0.14492753f;
}

// FUNCTION: CARMA2_HW 0x00417180
// FUNCTION: CARMA2_SW 0x004171b0
void C2_HOOK_FASTCALL ControlCar2(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed += time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed -= time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x004173b0
// FUNCTION: CARMA2_SW 0x004173e0
void C2_HOOK_FASTCALL ControlCar3(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f || pCar->collision_info->omega.v[1] < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f || pCar->collision_info->omega.v[1] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// ControlCar4

// FUNCTION: CARMA2_HW 0x00417a20
// FUNCTION: CARMA2_SW 0x00417a50
void C2_HOOK_FASTCALL ControlCar5(tCar_spec* pCar, br_scalar pDt) {
    br_scalar time_steps;

    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
        if (pCar->curvature < 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature + (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature > 0.0f) {
                pCar->curvature = 0.0f;
            }
        } else if (pCar->curvature > 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature - (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature < 0.0f) {
                pCar->curvature = 0.0f;
            }
        }
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
    pCar->keys.left = 1;
}

// ControlCar1

typedef void (C2_HOOK_FASTCALL * tControl_car_fn)(tCar_spec* pCar_spec, float pT);

// GLOBAL: CARMA2_HW 0x0058f660
tControl_car_fn ControlCar[6] = {
    NULL,
    NULL,
    NULL,
    (tControl_car_fn)ControlCar3,
    NULL,
    NULL,
};

// GLOBAL: CARMA2_HW 0x0058f678
int gControl__car = 3;

// FUNCTION: CARMA2_HW 0x004170c0
void C2_HOOK_FASTCALL CycleControlCar(void) {
    gControl__car++;
    if (ControlCar[gControl__car] == NULL) {
        gControl__car = 0;
    }
    switch (gControl__car) {
    case 0:
        NewTextHeadupSlot(4, 0, 500, -1, "Original Controls");
        return;
    case 1:
        NewTextHeadupSlot(4, 0, 500, -1, "Accelerated steering");
        return;
    case 2:
        NewTextHeadupSlot(4, 0, 500, -1, "0.75 Accelerated");
        return;
    case 3:
        NewTextHeadupSlot(4, 0, 500, -1, "0.5 Accelerated");
        return;
    default:
        NewTextHeadupSlot(4, 0, 500, -1, "New controls");
        return;
    }
}

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// FUNCTION: CARMA2_HW 0x00418230
// FUNCTION: CARMA2_SW 0x00418260
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// CalcForce

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// FUNCTION: CARMA2_HW 0x0041ddc0
void C2_HOOK_FASTCALL CrashNoise(br_vector3* pForce, br_vector3* pPosition, int pMaterial) {
    // GLOBAL: CARMA2_HW 0x00679410
    static tS3_sound_tag crunch_tag;
    // GLOBAL: CARMA2_HW 0x0067935c
    static tS3_volume last_crunch_vol;
    tS3_volume vol;
    br_vector3 velocity;

    extern tS3_outlet* gCar_outlet;
    extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);
    extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

    vol = 60.0 * BrVector3Length(pForce);
    if (gCurrent_race.material_modifiers[pMaterial].crash_noise_index == -1) {
    } else {
        if (vol > 255) {
            vol = 255;
        }
        if (crunch_tag == 0 || !DRS3SoundStillPlaying(crunch_tag)) {
            if (vol > 30) {
                last_crunch_vol = vol;
                (void)last_crunch_vol;
                velocity.v[0] = 0.f;
                velocity.v[1] = 0.f;
                velocity.v[2] = 0.f;
                crunch_tag = DRS3StartSound3D(gCar_outlet,
                    gMetal_crunch_sound_id[IRandomBetween(0, 4)],
                    pPosition, &velocity, 1, vol, IRandomBetween(49152, 81920), 0x10000);
            }
        }
    }
}

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

// FUNCTION: CARMA2_HW 0x0041e310
// FUNCTION: CARMA2_SW 0x0041e340
int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3) {

    return 0;
}

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e340
// FUNCTION: CARMA2_SW 0x0041e370
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// findfloor

// FUNCTION: CARMA2_HW 0x0041e480
// FUNCTION: CARMA2_SW 0x0041e4b0
int C2_HOOK_FASTCALL FindFloorInBoxBU(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject) {
    br_vector3 nor2;
    br_vector3 collision_position;
    int i;
    int j;
    br_scalar dist;
    tFace_ref* face_ref;

    *pDistance = 2.f;
    i = pObject->box_face_start;
    face_ref = &gFace_list__car[i];
    for (; i < pObject->box_face_end; i++, face_ref++) {
        if (gEliminate_faces && (face_ref->flags & 0x80) != 0) {
            continue;
        }
        CheckSingleFace(face_ref, pPosition, pDirection, &nor2, &dist, &collision_position);
        if (dist < *pDistance) {
            *pDistance = dist;
            j = i;
            BrVector3Copy(pNormal, &nor2);
        }
    }
    if (*pDistance < 2.f) {
        i = gFace_list__car[j].material->identifier[0] - ('0' - 1);
        if (i >= 0 && i < 11) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041e580
// FUNCTION: CARMA2_SW 0x0041e5b0
void C2_HOOK_FASTCALL CancelPendingCunningStunt(void) {
    gQuite_wild_end = 0;
    gQuite_wild_start = 0;
    gOn_me_wheels_start = 0;
    gWoz_upside_down_at_all = 0;
    gWild_start = 0;
}

// SetAmbientPratCam

void C2_HOOK_FASTCALL SetTextureBits(tCar_spec* pCar) {
    pCar->field_0x18cc = 0;
    if (pCar->keys.brake || (pCar->brake_force != 0.f && (float)fabs(pCar->collision_info->velocity_car_space.v[2]) > 1.0 / (1200.0 * WORLD_SCALE_D))) {
        pCar->field_0x18cc |= 0x4;
    }
    if (pCar->gear < 0 || (!(pCar != NULL && pCar->driver == eDriver_local_human) && pCar->collision_info->velocity_car_space.v[2] > 0.f)) {
        pCar->field_0x18cc |= 0x8;
    }
}

// FUNCTION: CARMA2_HW 0x0041e5a0
// FUNCTION: CARMA2_SW 0x0041e5d0
void C2_HOOK_FASTCALL MungeSomeOtherCarGraphics(void) {
    int i;
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear, 0x135c);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car->car_master_actor->render_style != BR_RSTYLE_NONE && !gAction_replay_mode) {
            SetTextureBits(car);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041e660
// STUB: CARMA2_SW 0x0041e690
void C2_HOOK_FASTCALL MungeCarGraphics(tU32 pFrame_period) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0041f110
// FUNCTION: CARMA2_SW 0x0041f140
void C2_HOOK_FASTCALL DoLODCarModels(void) {
    int i;
    int j;
    tCar_spec* car;
    float level;
    float factor;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor) != 2);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_simplification_factor[0]) != 5);

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1 && car->car_master_actor->render_style != BR_RSTYLE_NONE) {
            level = Vector3DistanceSquared(&car->car_master_actor->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]);
            factor = gCar_simplification_factor[gGraf_spec_index][gCar_simplification_level];
            if (!(factor >= 0.001f)) {
                level = BR_SCALAR_MAX;
            } else {
                level /= factor;
            }
#ifdef REC2_SW
            if (gNet_mode != eNet_mode_none && (gNet_players[gIt_or_fox].car == car || NetPlayerFromCar(car)->field_0x80 != 0)) {
                level = 0.0f;
            }
#endif
            for (j = car->count_detail_levels - 1; j > 0; j--) {
                if (level > car->detail_levels[j]) {
                    break;
                }
            }
            if (j > 0) {
                int variant;

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

// FUNCTION: CARMA2_HW 0x0041f280
// FUNCTION: CARMA2_SW 0x0041f300
intptr_t C2_HOOK_CDECL TurnOffNonGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* crush_data = pActor->user;

    (void)pData;
    if (crush_data != NULL && crush_data->groove == NULL) {
        pActor->type = BR_ACTOR_NONE;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0041f2a0
void C2_HOOK_FASTCALL DoComplexCarModels(void) {
    int i;
    tCar_spec* car;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > 5) {
            SwitchCarModels(car, 0);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0041f300
// FUNCTION: CARMA2_SW 0x0041f380
void C2_HOOK_FASTCALL ResetCarScreens(void) {
    int cat;
    int car_count;
    int i;
    tCar_spec* the_car;

    for (cat = eVehicle_self; cat <= eVehicle_rozzer; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            the_car->collision_info->last_special_volume = NULL;
        }
    }
    MungeCarGraphics(gFrame_period);
}

// FUNCTION: CARMA2_HW 0x0041f360
// FUNCTION: CARMA2_SW 0x0041f3e0
void C2_HOOK_FASTCALL ToggleCarToCarCollisions(void) {

    gCar_car_collisions = !gCar_car_collisions;
    if (gCar_car_collisions) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Car Car Collisions");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Ghost Cars");
    }
}

// FUNCTION: CARMA2_HW 0x0041f3b0
// FUNCTION: CARMA2_SW 0x0041f430
void C2_HOOK_FASTCALL SwapCar(void) {
}

// FUNCTION: CARMA2_HW 0x0041f3c0
// FUNCTION: CARMA2_SW 0x0041f440
void C2_HOOK_FASTCALL AdjustDownForce(void) {
    char s[100];
    tCar_spec* c;

    c = gCar_to_view;
    c->downforce_to_weight += 50.f;
    if (c->downforce_to_weight > 2000.f) {
        c->downforce_to_weight = 50.f;
    }
    if (c->downforce_to_weight > 300.f) {
        c->downforce_to_weight = 2000.f;
    }
    sprintf(s, "DownForceSpeed %f", c->downforce_to_weight);
    NewTextHeadupSlot(4, 0, 1500, -4, s);
}

// FUNCTION: CARMA2_HW 0x0041f450
// FUNCTION: CARMA2_SW 0x0041f4d0
void C2_HOOK_FASTCALL FreezeMechanics(void) {

    gFreeze_mechanics = !gFreeze_mechanics;
    if (gFreeze_mechanics) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Mechanics Frozen");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Thawed Mechanics");
    }
}

// FUNCTION: CARMA2_HW 0x0041f4a0
void C2_HOOK_FASTCALL PutOpponentsInNeutral(void) {
    gStop_opponents_moving = !gStop_opponents_moving;
    if (gStop_opponents_moving) {
        NewTextHeadupSlot(4, 0, 3000, -4, "Opponents in neutral");
    } else {
        NewTextHeadupSlot(4, 0, 3000, -4, "Back in gear");
    }
}

// FlyCar

// GetCarOverallBoundsMinY

// STUB: CARMA2_HW 0x0041fc60
// STUB: CARMA2_SW 0x0041fce0
void C2_HOOK_FASTCALL SetCarSuspGiveAndHeight(tCar_spec* pCar, float pFront_give_factor, float pRear_give_factor, float pDamping_factor, float pExtra_front_height, float pExtra_rear_height) {
    NOT_IMPLEMENTED();
}

// TestForCarInSensiblePlace

// FUNCTION: CARMA2_HW 0x0041ff00
// FUNCTION: CARMA2_SW 0x0041ff80
tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* pActor) {
    if (!gPHIL_doing_physics && !gTesting_car_for_sensible_place) {
        return NULL;
    } else {
        return DoPullActorFromWorld(pActor);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0041ff20
// STUB: CARMA2_SW 0x0041ffa0
tNon_car_spec* C2_HOOK_FASTCALL DoPullActorFromWorld(br_actor* pActor) {
    NOT_IMPLEMENTED();
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00420310
int C2_HOOK_FASTCALL FUN_00420310(tPhysics_object* pObject) {
    tNon_car_spec* non_car;

    non_car = (tNon_car_spec*)pObject->owner;
    pObject->field_0xed = 1;
    *(int*)((tU8*)pObject + 0x194) = 0;
    *(int*)((tU8*)pObject + 0x198) = 0;
    if (non_car->snap_off_cosine < 1.0f) {
        pObject->physics_joint2 = NULL;
    }
    pObject->field_0x218 = 0;
    pObject->v.v[0] = 0.0f;
    pObject->v.v[1] = 0.0f;
    pObject->v.v[2] = 0.0f;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00420360
int C2_HOOK_FASTCALL FUN_00420360(tPhysics_object* pObject) {
    tNon_car_spec* non_car;

    non_car = (tNon_car_spec*)pObject->owner;
    if (*(br_scalar*)((tU8*)pObject->actor + 0x3c) < non_car->snap_off_cosine) {
        pObject->field_0xed = 1;
        *(int*)((tU8*)pObject + 0x194) = 0;
        *(int*)((tU8*)pObject + 0x198) = 0;
        if (non_car->snap_off_cosine < 1.0f) {
            pObject->physics_joint2 = NULL;
        }
        pObject->field_0x218 = 0;
        pObject->v.v[0] = 0.0f;
        pObject->v.v[1] = 0.0f;
        pObject->v.v[2] = 0.0f;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data) {

    if (!pCollision_info->disable_move_rotate && pCollision_info->actor != NULL && pCollision_info->actor->type_data != NULL) {
        PipeSingleNonCarCreation2((tU16)((tNon_car_spec*)pCollision_info->actor->type_data)->car_ID, pCollision_info->actor);
    }
    ((tNon_car_spec*)pCollision_info->owner)->field_0xfc = pCollision_info->disable_move_rotate;
    return 0;
}

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// FUNCTION: CARMA2_HW 0x004207e0
// FUNCTION: CARMA2_SW 0x00420860
void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix) {
    tU8 column_x;
    tU8 column_z;
    br_actor* parent;

    BrMatrix34Copy(&pActor->t.t.mat, pMatrix);
    if (pActor->parent == gNon_track_actor) {
        return;
    }
    XZToColumnXZ(&column_x, &column_z, pActor->t.t.mat.m[3][0], pActor->t.t.mat.m[3][2], &gProgram_state.track_spec);
    parent = gProgram_state.track_spec.columns[column_z][column_x].actor_0x0;
    if (parent != pActor->parent && parent != NULL) {
        BrActorRemove(pActor);
        BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, pActor);
    }
}

// FUNCTION: CARMA2_HW 0x00420880
// FUNCTION: CARMA2_SW 0x00420900
int C2_HOOK_FASTCALL GetPrecalculatedFacesUnderCar(tCar_spec* pCar, tFace_ref** pFace_refs) {

    if (pCar->collision_info->box_face_ref != gFace_num__car) {
        if (pCar->collision_info->box_face_ref != gFace_num__car - 1 || pCar->collision_info->box_face_start <= gFace_count) {
            return 0;
        }
    }
    *pFace_refs = &gFace_list__car[pCar->collision_info->box_face_start];
    return pCar->collision_info->box_face_end - pCar->collision_info->box_face_start;
}

// FUNCTION: CARMA2_HW 0x004208f0
// FUNCTION: CARMA2_SW 0x00420970
void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x00420900
// FUNCTION: CARMA2_SW 0x00420980
void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_NONE;
}

/* Crush network lists: the objects live in 13-crush1.c / 07-structur.c. */
#define PO_U8(o, off) (*(tU8*)((char*)(o) + (off)))
#define PO_U16(o, off) (*(tU16*)((char*)(o) + (off)))
#define PO_U32(o, off) (*(tU32*)((char*)(o) + (off)))
#define PO_F32(o, off) (*(float*)((char*)(o) + (off)))

typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
    br_vector3 field_0xc;
    br_vector3 field_0x18;
    br_vector3 field_0x24;
    br_bounds3 bounds;
    float field_0x48;
} tUpdate_crush_semi_detach_item;

typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
    br_bounds3 bounds;
} tUpdate_crush_detach_item;

typedef struct {
    tCar_spec* car;
    tPhysics_object* object;
    tU32 time;
} tUpdate_crush_full_detach_item;

typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
} tUpdate_crush_reattach_item;

extern tUpdate_crush_semi_detach_item gNet_crush_semi_detach_bit_list[8];
extern int gCount_net_crush_semi_detach_bit_list;
extern tUpdate_crush_detach_item gNet_crush_detach_list[8];
extern int gCount_net_crush_detach_list;
extern tUpdate_crush_full_detach_item gNet_crush_full_detach_bit_list[8];
extern int gCount_net_crush_full_detach_bit_list;
extern tUpdate_crush_reattach_item gNet_crush_reattach_bit_list[8];
extern int gCount_net_crush_reattach_bit_list;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU32 field_0x8;
} tNet_message_chunk_crush_reattach_bit;

/* DirectInput types are declared locally: the DirectX SDK headers are only
   visible to the platform target, not to newcommon. The layouts match the SDK
   ones and tJoystick_force_effect in win32_types.h (size 0xe8, name at 0x68). */
typedef struct tCarDinputEffect tCarDinputEffect;

typedef struct tCarDinputEffectVtbl {
    void* slot_0x00[6];
    int (C2_HOOK_STDCALL* SetParameters)(tCarDinputEffect* pEffect, const void* pEffect_params, tU32 pFlags);
} tCarDinputEffectVtbl;

struct tCarDinputEffect {
    tCarDinputEffectVtbl* lpVtbl;
};

typedef struct {
    tU8 di_effect[0x34];
    tU8 type_specific_params[24];
    tU8 field_0x4c[0x14];
    tS32 field_0x60;
    tU32 field_0x64;
    char name[128];
} tCarJoystickForceEffect;

extern tCarJoystickForceEffect gJoystick_effects[30];
extern tCarDinputEffect* gDirectInputEffects[30];
extern int gForceFeedbackAvailable;
extern int gCollision_force_joystick_effect_index;

// FUNCTION: CARMA2_HW 0x0043ce60
void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents) {
    extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);
    tCar_spec* car;
    int item_offset;
    int count;
    tU32 bit_index;

    if (gCount_net_crush_reattach_bit_list != 8) {
        car = NetCarFromPlayerID(pContents->player_id);
        if (car != NULL) {
            count = gCount_net_crush_reattach_bit_list;
            item_offset = count * 3;
            item_offset <<= 2;
            count++;
            *(tCar_spec**)((tU8*)&gNet_crush_reattach_bit_list[0].car + item_offset) = car;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].actor + item_offset) = 0;
            bit_index = pContents->field_0x8;
            gCount_net_crush_reattach_bit_list = count;
            *(tU32*)((tU8*)&gNet_crush_reattach_bit_list[0].time + item_offset) = bit_index;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043d960
int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData) {
    tU8 count_or_points;
    int count;
    int size;

    count_or_points = *pData++;
    if (count_or_points <= 4) {
        count = count_or_points;
        size = 1;
        if (count > 0) {
            do {
                count_or_points = *pData;
                size = size + 1 + count_or_points * 6;
                pData = pData + count_or_points * 6 + 1;
                count--;
            } while (count != 0);
        }
        return size;
    }
    return count_or_points * 6 + 1;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0043ceb0
void C2_HOOK_FASTCALL UpdateCrushPainList(void) {
    tCar_crush_spec* car_crush;
    tCar_spec* car;
    br_actor* bit;
    int count;
    int keep;
    int found;
    int i;
    int j;

    count = 0;
    for (i = 0; i < gCount_net_crush_semi_detach_bit_list; i++) {
        keep = 1;
        if (gNet_crush_semi_detach_bit_list[i].time <= gPHIL_last_physics_tick) {
            SemiDetachBit(gNet_crush_semi_detach_bit_list[i].car, gNet_crush_semi_detach_bit_list[i].actor,
                *(tU32*)&gNet_crush_semi_detach_bit_list[i].field_0x48, &keep,
                &gNet_crush_semi_detach_bit_list[i].field_0xc,
                &gNet_crush_semi_detach_bit_list[i].bounds,
                &gNet_crush_semi_detach_bit_list[i].field_0x18,
                &gNet_crush_semi_detach_bit_list[i].field_0x24);
        }
        if (keep != 0) {
            if (count != i) {
                gNet_crush_semi_detach_bit_list[count] = gNet_crush_semi_detach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_semi_detach_bit_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].time <= gPHIL_last_physics_tick) {
            DetachBit(gNet_crush_detach_list[i].car, gNet_crush_detach_list[i].actor,
                &gNet_crush_detach_list[i].bounds);
        } else {
            if (count != i) {
                gNet_crush_detach_list[count] = gNet_crush_detach_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_detach_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].time <= gPHIL_last_physics_tick) {
            FullyDetachBit(gNet_crush_full_detach_bit_list[i].car, gNet_crush_full_detach_bit_list[i].object);
            MungeDetachLists(gNet_crush_full_detach_bit_list[i].car->car_crush_spec);
        } else {
            if (count != i) {
                gNet_crush_full_detach_bit_list[count] = gNet_crush_full_detach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_full_detach_bit_list = count;

    count = 0;
    for (i = 0; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].time <= gPHIL_last_physics_tick) {
            car = gNet_crush_reattach_bit_list[i].car;
            bit = gNet_crush_reattach_bit_list[i].actor;
            car_crush = car->car_crush_spec;
            if (bit == NULL) {
                for (j = 0; j < car_crush->field_0x2b0; j++) {
                    ReAttachBit(car, car_crush->field_0x2b4[j].field_0x4, car_crush->field_0x2b4[j].field_0x0, NULL);
                }
                car_crush->field_0x2b0 = 0;
                for (j = 0; j < car_crush->field_0x270; j++) {
                    ReAttachBit(car, car_crush->field_0x274[j].field_0x4, car_crush->field_0x274[j].field_0x0, car_crush->field_0x274[j].field_0x8);
                }
                car_crush->field_0x270 = 0;
            } else {
                found = 0;
                for (j = 0; j < car_crush->field_0x2b0; j++) {
                    if (car_crush->field_0x2b4[j].field_0x0 == bit) {
                        ReAttachBit(car, car_crush->field_0x2b4[j].field_0x4, bit, NULL);
                        MungeDetachLists(car_crush);
                        found = 1;
                    }
                }
                if (!found) {
                    for (j = 0; j < car_crush->field_0x270; j++) {
                        if (car_crush->field_0x274[j].field_0x0 == bit) {
                            ReAttachBit(car, car_crush->field_0x274[j].field_0x4, bit, car_crush->field_0x274[j].field_0x8);
                            MungeDetachLists(car_crush);
                            break;
                        }
                    }
                }
            }
        } else {
            if (count != i) {
                gNet_crush_reattach_bit_list[count] = gNet_crush_reattach_bit_list[i];
            }
            count++;
        }
    }
    gCount_net_crush_reattach_bit_list = count;
}

// FUNCTION: CARMA2_HW 0x0043d900
int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar) {
    tCar_crush_spec* crush;
    int* node;
    int i;
    int total;
    int result;

    crush = pCar->car_crush_spec;
    if (crush->field_0x4b8 != 0) {
        result = 1;
        node = *(int**)(*(int*)((tU8*)pCar + 8) + 0x20);
        if (node != NULL) {
            do {
                if (node[0] == 1 || node[0] == 3) {
                    result = result + 1 + node[0xe] * 6;
                }
                node = (int*)node[0xd];
            } while (node != NULL);
        }
        return result;
    }
    total = 0;
    for (i = 0; i < crush->count_shapes; i++) {
        total += crush->field_0x4[i].count_points;
    }
    return total * 6 + 1;
}

// FUNCTION: CARMA2_HW 0x0043db00
void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pData) {
    extern float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec);
    extern void C2_HOOK_FASTCALL FillInShape(tPhysics_shape* pShape);
    extern void C2_HOOK_FASTCALL UpdateCollisionObject(tPhysics_object* pCollision_info);
    tPhysics_shape* shape;
    tCar_crush_shape_info* shape_info;
    int i;
    int j;
    int count;

    if (pCar->car_crush_spec->field_0x4b8 != 0) {
        count = *pData++;
        shape = pCar->collision_info->shape;
        while (shape != NULL && count > 0) {
            if (shape->common.type == kCollisionShapeType_Polyhedron
                    || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                shape->polyhedron.polyhedron.count_points = *pData++;
                for (i = 0; i < shape->polyhedron.polyhedron.count_points; i++) {
                    ExpandVector3(&shape->polyhedron.polyhedron.points[i], (const tCompressed_vector3*)pData, -2.5f, 2.5f);
                    pData += 6;
                }
                count--;
            }
            shape = shape->common.next;
        }
    } else {
        pData++;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            shape_info = &pCar->car_crush_spec->field_0x4[i];
            for (j = 0; j < shape_info->count_points; j++) {
                ExpandVector3(&shape_info->field_0x18[j].field_0x18, (const tCompressed_vector3*)pData, -2.5f, 2.5f);
                pData += 6;
                shape_info->field_0x18[j].field_0xc.v[0] = shape_info->field_0x18[j].field_0x18.v[0];
                shape_info->field_0x18[j].field_0xc.v[1] = shape_info->field_0x18[j].field_0x18.v[1];
                shape_info->field_0x18[j].field_0xc.v[2] = shape_info->field_0x18[j].field_0x18.v[2];
                shape_info->field_0x0->polyhedron.points[j].v[0] = shape_info->field_0x18[j].field_0xc.v[0];
                shape_info->field_0x0->polyhedron.points[j].v[1] = shape_info->field_0x18[j].field_0xc.v[1];
                shape_info->field_0x0->polyhedron.points[j].v[2] = shape_info->field_0x18[j].field_0xc.v[2];
            }
            FillInShape((tPhysics_shape*)shape_info->field_0x0);
        }
    }
    UpdateCollisionObject(pCar->collision_info);
    pCar->collision_info->bb2.min.v[1] = GetCarOverallBoundsMinY(pCar);
    pCar->car_crush_spec->field_0x26 = PO_U16(pCar->collision_info, 0x290);
    pCar->car_crush_spec->field_0x24 = pCar->car_crush_spec->field_0x26;
    pCar->car_crush_spec->expand_bounding_box = 0;
}

// FUNCTION: CARMA2_HW 0x0043dce0
void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar) {
    extern br_scalar gFLOAT_00589a2c;
    extern void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);
    tCar_crush_spec* spec;
    tPhysics_object* info;
    br_scalar temp;
    br_scalar m1;
    tU8 f;
    int flag;
    tU16 id_a;
    tU16 id_b;
    tU16 id_c;
    br_vector3 v;
    int c2;
    int c1;

    spec = pCar->car_crush_spec;
    if (spec->field_0x144 != 0) {
        info = pCar->collision_info;
        f = *(tU8*)((tU8*)info + 0x481);
        c1 = (((f & 1) != 0 && spec->field_0x174 == spec->bend_z_min)
                || ((f & 1) == 0 && spec->field_0x174 == spec->bend_z_max)) & 1;
        c2 = ((f & 2) != 0 && *(tU16*)((tU8*)spec + 0x190) == *(tU16*)((tU8*)info + 0x482))
            || ((f & 4) != 0 && *(tU16*)((tU8*)spec + 0x192) == *(tU16*)((tU8*)info + 0x482))
            || ((f & 8) != 0 && *(tU16*)((tU8*)spec + 0x194) == *(tU16*)((tU8*)info + 0x482));
        if (c1 & c2) {
            return;
        }
        UnBendCar(pCar, 2000, &temp);
    }
    info = pCar->collision_info;
    id_a = 0;
    v.v[0] = 0.f;
    v.v[1] = 0.f;
    v.v[2] = 0.f;
    id_c = 0;
    id_b = 0;
    if ((*(tU8*)((tU8*)info + 0x481) & 1) != 0) {
        v.v[2] = spec->bend_z_min;
        temp = info->bb1.max.v[2];
        flag = 1;
    } else {
        v.v[2] = spec->bend_z_max;
        temp = info->bb1.min.v[2];
        flag = 0;
    }
    switch (*(tU8*)((tU8*)info + 0x481) >> 1) {
    case 1:
        id_a = *(tU16*)((tU8*)info + 0x482);
        if ((id_a < 0x7ff8 && flag) || (id_a > 0x7ff8 && !flag)) {
            v.v[1] = info->bb1.min.v[1];
        } else {
            v.v[1] = info->bb1.max.v[1];
        }
        break;
    case 2:
        id_b = *(tU16*)((tU8*)info + 0x482);
        if ((id_b > 0x7ff8 && flag) || (id_b < 0x7ff8 && !flag)) {
            v.v[0] = info->bb1.min.v[0];
        } else {
            v.v[0] = info->bb1.max.v[0];
        }
        break;
    case 4:
        id_c = *(tU16*)((tU8*)info + 0x482);
        m1 = info->bb1.max.v[0];
        v.v[0] = (m1 + info->bb1.min.v[0]) * gFLOAT_00589a2c;
        m1 = info->bb1.min.v[1];
        v.v[1] = (m1 + info->bb1.max.v[1]) * gFLOAT_00589a2c;
        break;
    }
    BendCar(pCar, id_a, id_b, id_c, &v, temp, 0);
}

// FUNCTION: CARMA2_HW 0x00458720
int C2_HOOK_FASTCALL SetJoystickFFBCollisionForce(int pMagnitude, int pDirection) {
    /* This SDK spells DIEP_TYPESPECIFICPARAMS and DIEP_DIRECTION 0x100 and 0x40. */
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    if (pMagnitude > 10000) {
        pMagnitude = 10000;
    }
    *(tS32*)&gJoystick_effects[gCollision_force_joystick_effect_index].type_specific_params = pMagnitude;
    gJoystick_effects[gCollision_force_joystick_effect_index].field_0x60 = pDirection;
    if (gDirectInputEffects[gCollision_force_joystick_effect_index] != NULL) {
        gDirectInputEffects[gCollision_force_joystick_effect_index]->lpVtbl->SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, 0x100);
        gDirectInputEffects[gCollision_force_joystick_effect_index]->lpVtbl->SetParameters(gDirectInputEffects[gCollision_force_joystick_effect_index], &gJoystick_effects[gCollision_force_joystick_effect_index].di_effect, 0x40);
    }
    StartDinputEffect(gCollision_force_joystick_effect_index);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00498460
void C2_HOOK_FASTCALL CompressCarDataChunk(tNet_message_chunk* pMessage, int pPlayer_index) {
    tU8* chunk;
    tCar_spec* car;
    br_actor* actor;

    chunk = (tU8*)pMessage;
    car = gNet_players[pPlayer_index].car;
    CompressVector3((tCompressed_vector3*)(chunk + 8), &car->car_master_actor->t.t.translate.t, -500.f, 500.f);
    actor = car->car_master_actor;
    *(tS16*)(chunk + 2) = (tS16)BrRadianToAngle(atan2(actor->t.t.mat.m[2][0], actor->t.t.mat.m[2][2]));
    *(int*)(chunk + 4) = gNet_players[pPlayer_index].ID;
    *(int*)(chunk + 0x10) = *(int*)((tU8*)car + 0x18a4);
}

// FUNCTION: CARMA2_HW 0x004fee70
void C2_HOOK_FASTCALL ClearSplashes(tPhysics_object* pObject) {
    extern float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pCollision_info, int pParam);
    extern void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pCollision, void* pArg2);
    extern int gReplay_splash_flags;
    extern tReplay_splash gReplay_splashes[10];
    extern const br_scalar gFLOAT_0058b008;
    void* object;
    int i;

    if (pObject->water_d == 10000.f) {
        return;
    }
    pObject->water_d = 10000.f;
    if (PHILGetObjectProperty(pObject, 6) != gFLOAT_0058b008) {
        object = pObject;
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        object = pObject->actor;
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 >= 2 && pObject->flags_0x238 <= 4) {
        object = pObject->owner;
    } else {
        object = NULL;
    }
    if (object == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gReplay_splashes); i++) {
        if ((gReplay_splash_flags & (1 << i)) && object == gReplay_splashes[i].object.actor) {
            ARStartPipingSession((tPipe_chunk_type)0x3b);
            AddExtendedSplashToPipingSession(pObject, object);
            AREndPipingSession();
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff410
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);
    extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
    extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);
    extern tS3_outlet* gCar_outlet;
    tSpecial_volume* new_vol;
    tCar_spec* sv;

    new_vol = FindSpecialVolume(&pObject->pos, pObject->last_special_volume, 0);

    sv = NULL;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        sv = (tCar_spec*)pObject->owner;
    } else {
        sv = NULL;
    }

    if (pObject->auto_special_volume != NULL) {
        if (new_vol == NULL || new_vol->gravity_multiplier == 1.0f) {
            if (pObject->water_d == 10000.0f && pObject->water_depth_factor != 1.0f) {
                pObject->auto_special_volume = NULL;
            } else {
                new_vol = pObject->auto_special_volume;
            }
        }
    }

    if (new_vol != pObject->last_special_volume && sv != NULL && sv->driver == eDriver_local_human) {
        if (pObject->last_special_volume != NULL && pObject->last_special_volume->exit_noise >= 0
                && (new_vol == NULL || new_vol->exit_noise != pObject->last_special_volume->exit_noise)) {
            DRS3StartSound(gCar_outlet, pObject->last_special_volume->exit_noise);
        }
        if (new_vol != NULL && new_vol->entry_noise >= 0
                && (pObject->last_special_volume == NULL || pObject->last_special_volume->entry_noise != new_vol->entry_noise)) {
            DRS3StartSound(gCar_outlet, new_vol->entry_noise);
        }
    }

    pObject->last_special_volume = new_vol;

    if (sv != NULL && new_vol != NULL) {
        br_scalar gravity_multiplier = new_vol->gravity_multiplier;
        if (gravity_multiplier < 1.0f) {
            if (IsCarSmoking(sv) != 0) {
                StopCarSmoking(sv);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c6ff0
void C2_HOOK_FASTCALL PipeSingleNonCarCreation2(uintptr_t pId, br_actor* pActor) {
    extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

    ARAddVariedDataToSession(ePipe_chunk_non_car, pId, 2, 4, 0, pActor, 0x30, 4, &pActor->t.t.mat);
}

// STUB: CARMA2_HW 0x00415ef0
void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00418250
void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ff5d0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif




/* the stub below needs the type; workv4 defines it in this file (ported with ProcessForcesCallback) */
typedef struct tCollision_contact {
    float impulse;
    tPhysics_object* object1;
    tPhysics_object* object2;
    void* field_0xc;
    float field_0x10;
    br_vector3 r1;
    br_vector3 r2;
    br_vector3 normal;
    br_vector3 field_0x38;
    br_vector3 impulse1;
    br_vector3 impulse2;
    int type;
} tCollision_contact;

// STUB: CARMA2_HW 0x0043ace0
float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, tU32 pDelta, float* pOut) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00414910
void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0041c1b0
int C2_HOOK_FASTCALL ProcessForcesCallback(tCollision_contact* pContacts, float* pImpulses, int pCount) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0041e310
int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) { NOT_IMPLEMENTED(); }
