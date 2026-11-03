#define PO_U8(o, off) (*(tU8*)((char*)(o) + (off)))
#define PO_U16(o, off) (*(tU16*)((char*)(o) + (off)))
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
#include "10-loading2.h"
#include "59-camera.h"

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
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "platform.h"

#include "c2_math.h"
#include "c2_stdio.h"
#include "rec2_macros.h"

#include <math.h>

#define PO_U32(o, off) (*(tU32*)((char*)(o) + (off)))
#define PO_F32(o, off) (*(float*)((char*)(o) + (off)))
#define CAR_GEAR(c) (*(int*)&(c)->gear)


extern tU32 gPalette_fade_time;
extern tWorld_callbacks gWorld_callbacks;

// GLOBAL: CARMA2_HW 0x0079ed88
tU32 gDrivable_on_count;

// GLOBAL: CARMA2_HW 0x0079ecc0
tPhysics_object* gDrivable_on_list[50];

// GLOBAL: CARMA2_HW 0x00589694
float gFneg1000 = -1000.0f;

// GLOBAL: CARMA2_HW 0x00679390
br_vector3 gCar_to_view_original_v;

// GLOBAL: CARMA2_HW 0x005895f0
double gZero = 1.0;

// GLOBAL: CARMA2_HW 0x006793ec
tU32 gSafe_position_time_count = 0;

extern void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt);

static void C2_HOOK_FASTCALL RememberSafePosition(tCar_spec* car, tU32 pTime_difference) {
    int i;
    br_vector3 r;

    gSafe_position_time_count += pTime_difference;
    if (gSafe_position_time_count < 5000) {
        return;
    }
    gSafe_position_time_count = 4000;
    if (car->disabled) {
        return;
    }
    if (car->car_crush_spec != NULL && car->car_crush_spec->field_0x4b8) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (car->oldd[i] >= car->susp_height[i >> 1]) {
            return;
        }
    }
    if ((car->collision_info->last_special_volume == NULL || car->collision_info->last_special_volume->gravity_multiplier == 1.0)
        && gCurrent_race.material_modifiers[car->material_index[0]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[1]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[2]].tyre_road_friction >= 0.1
        && gCurrent_race.material_modifiers[car->material_index[3]].tyre_road_friction >= 0.1
        && !car->field_0x195c
        && car->car_master_actor->t.t.mat.m[1][1] >= 0.8f) {

        for (i = 0; i < 4; i++) {
            BrVector3Sub(&r, &car->car_master_actor->t.t.translate.t, (br_vector3*)car->last_safe_positions[i].m[3]);

            if (BrVector3LengthSquared(&r) < 8.4015961f) {
                return;
            }
        }
        for (i = 18; i > 0; i--) {
            BrMatrix34Copy(&car->last_safe_positions[i], &car->last_safe_positions[i - 1]);
        }
        BrMatrix34Copy(&car->last_safe_positions[0], &car->car_master_actor->t.t.mat);
        gSafe_position_time_count = 0;
    }
}


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

// GLOBAL: CARMA2_HW 0x006793d4
int gDouble_pling_water;

// GLOBAL: CARMA2_HW 0x0079eda0
// GLOBAL: CARMA2_SW 0x00795340
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x00679360
br_scalar gMin_world_y;

// GLOBAL: CARMA2_HW 0x006793e4
tPhysics_object* gUnknown_car_collision_info;

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

extern tSlot_info gInitial_APO[3];
extern tSlot_info gInitial_APO_potential[3];

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

// FUNCTION: CARMA2_HW 0x00414cb0
void C2_HOOK_FASTCALL ControlOurCar(tU32 pTime_difference) {
    // GLOBAL: CARMA2_HW 0x0058f6d8
    static int last_key_down = 1;
    tCar_spec* car;
    tU32 time;
    br_vector3 minus_k;
    br_vector3 tmp;
    br_vector3 delta_omega;

    car = &gProgram_state.current_car;

    if (car->keys.change_down) {
        if (!last_key_down) {
            last_key_down = 1;

            if (car->number_of_wheels_on_ground <= 2
                    && BrVector3LengthSquared(&car->collision_info->v) < .04
                    && car->oldd[0] == car->susp_height[0]
                    && car->oldd[1] == car->susp_height[0]
                    && (car->frame_collision_flag || car->collision_info->disable_move_rotate)) {

                tmp.v[0] = -car->collision_info->transform_matrix.m[1][2];
                *(int*)&tmp.v[1] = 0;
                tmp.v[2] = car->collision_info->transform_matrix.m[1][0];
                BrVector3Normalise(&tmp, &tmp);
                BrMatrix34TApplyV(&delta_omega, &tmp, &car->collision_info->transform_matrix);
                BrVector3Accumulate(&car->collision_info->omega, &delta_omega);
                car->collision_info->disable_move_rotate = 0;
            }
        }
    } else {
        last_key_down = 0;
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {

            if (i != gThis_net_player_index) {
                ControlCar[gControl__car](gNet_players[i].car, 0.001f * pTime_difference);
            }
        }
    }
    if (gCar_flying) {
        FlyCar(gCar_to_view, 0.001f * pTime_difference);
        return;
    }

    time = GetTotalTime();
    if (car->damage_units[eDamage_steering].damage_level > 40) {
        if (car->end_steering_damage_effect != 0) {
            if (car->end_steering_damage_effect > time || car->damage_units[eDamage_steering].damage_level == 99) {
                car->keys.left = car->false_key_left;
                car->keys.right = car->false_key_right;
            } else {
                car->end_steering_damage_effect = 0;
            }
        } else {
            if (PercentageChance((int)(pTime_difference * (car->damage_units[eDamage_steering].damage_level - 40) * 0.0045)) && fabs(car->collision_info->velocity_car_space.v[2]) > 1. / (10000. * 6.9f)) {
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
                car->end_steering_damage_effect = (tU32)(time + FRandomBetween(0.0f, (float)(25 * (car->damage_units[eDamage_steering].damage_level - 40))));
            }
        }
    }
    if (car->damage_units[eDamage_transmission].damage_level > 40) {
        if (car->end_trans_damage_effect != 0) {
            if (car->end_trans_damage_effect > time || car->damage_units[eDamage_transmission].damage_level == 99) {
                car->gear = 0;
                car->just_changed_gear = 1;
            } else {
                car->end_trans_damage_effect = 0;
            }
        } else {
            if (PercentageChance((int)(pTime_difference * (car->damage_units[eDamage_transmission].damage_level - 40) * 0.006))) {
                car->end_trans_damage_effect = (tU32)(time + FRandomBetween(0.f, (float)(50 * (car->damage_units[eDamage_transmission].damage_level - 40))));
            }
        }
    }
    ControlCar[gControl__car](car, 0.001f * pTime_difference);
    RememberSafePosition(car, pTime_difference);
    if (gCamera_reset) {
        BrVector3SetFloat(&minus_k, 0.0f, 0.0f, -1.0f);
        gCamera_sign = 0;
        BrMatrix34ApplyV(&car->direction, &minus_k, &car->car_master_actor->t.t.mat);
    }
}

// FUNCTION: CARMA2_HW 0x00415300
void C2_HOOK_FASTCALL CalcEngineForce(tCar_spec* pCar, br_scalar pDt) {
    br_scalar speed;
    br_scalar factor;
    tS32 joy;

    speed = -pCar->collision_info->velocity_car_space.v[2] * 6.9;
    pCar->acc_force = 0.f;
    if (pCar->revs == 0.f) {
        CAR_GEAR(pCar) = 0;
    }
    if ((!pCar->keys.backwards && CAR_GEAR(pCar) < 0) ||
        (CAR_GEAR(pCar) == 0 && speed < -0.5 && !pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }
    if ((pCar->keys.backwards && CAR_GEAR(pCar) > 0) ||
        (CAR_GEAR(pCar) == 0 && speed > 0.5 && pCar->keys.backwards)) {
        pCar->keys.backwards = !pCar->keys.backwards;
        pCar->keys.acc ^= pCar->keys.dec;
        pCar->keys.dec ^= pCar->keys.acc;
        pCar->keys.acc ^= pCar->keys.dec;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }
    if (CAR_GEAR(pCar) == 0 && !pCar->keys.acc && pCar->joystick.acc <= 0 &&
        (pCar->keys.dec || pCar->joystick.dec > 0) && !pCar->keys.backwards &&
        (float)fabs(speed) < 1.f) {
        pCar->keys.backwards = 1;
        pCar->keys.acc = pCar->keys.dec;
        pCar->keys.dec = 0;
        joy = pCar->joystick.acc;
        pCar->joystick.acc = pCar->joystick.dec;
        pCar->joystick.dec = joy;
    }

    pCar->torque = pCar->revs * pCar->revs * -1e-08f - 0.2;
    if (pCar->keys.acc || pCar->joystick.acc >= 0) {
        if (fabs(pCar->curvature) > pCar->maxcurve * 0.5f && CAR_GEAR(pCar) < 2 &&
            CAR_GEAR(pCar) != 0 && pCar->traction_control && pCar->joystick.acc < 0) {
            factor = 0.7f;
        } else if (pCar->joystick.acc >= 0) {
            factor = pCar->joystick.acc * (1.0 / 54613.0);
        } else {
            factor = 1.2f;
        }
        factor = factor * pCar->field_0x4d4;
        factor = factor * gPower_starting_value[pCar->power_up_levels[1]];
        if (pCar->damage_units[0].damage_level > 10) {
            factor = factor * (1.f - (pCar->damage_units[0].damage_level - 10) * 0.01f);
        }
        pCar->torque = pCar->torque + factor;
    } else {
        pCar->traction_control = 1;
    }

    if (pCar->keys.dec || (pCar->keys.acc && CAR_GEAR(pCar) == 0) ||
        pCar->joystick.dec > 0 || (pCar->joystick.acc > 0 && CAR_GEAR(pCar) == 0)) {
        if (pCar->joystick.dec > 0) {
            br_scalar bi = pCar->brake_increase;
            br_scalar q = (pCar->joystick.dec) / 65536;
            pCar->brake_force = pCar->initial_brake + bi * q;
        }
        if (pCar->brake_force == 0.f) {
            pCar->brake_force = pCar->initial_brake;
        } else {
            br_scalar bi = pCar->brake_increase;
            pCar->brake_force = pCar->brake_force + bi * pDt;
            if (pCar->brake_force > pCar->initial_brake + pCar->brake_increase) {
                pCar->brake_force = pCar->initial_brake + pCar->brake_increase;
            }
        }
    } else {
        pCar->brake_force = 0.f;
    }

    if (CAR_GEAR(pCar) == 0) {
        return;
    }
    pCar->acc_force = pCar->force_torque_ratio * pCar->torque / (float)CAR_GEAR(pCar);
    if (pCar->brake_force != 0.f) {
        pCar->revs = pCar->target_revs;
    } else if (pCar->revs - 1.f > pCar->target_revs || pCar->revs + 1.f < pCar->target_revs) {
        br_scalar lt = gZero / (pCar->speed_revs_ratio * pCar->collision_info->M) / (float)CAR_GEAR(pCar);
        pCar->acc_force = pCar->acc_force +
            (pCar->torque * pDt * 5000.0 + (pCar->revs - pCar->target_revs)) /
            ((lt + (float)CAR_GEAR(pCar) * (gZero / (pCar->force_torque_ratio * 0.0002))) * pDt);
    }
}

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

extern void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV);

// FUNCTION: CARMA2_HW 0x00415ef0
void C2_HOOK_FASTCALL AddDrag(tCar_spec* pCar, tPhysics_object* pObject, br_scalar pDt) {
    br_scalar drag;
    br_scalar k;
    br_vector3 v;

    drag = pDt * 0.0005f;
    if (pObject->last_special_volume != NULL) {
        k = pObject->last_special_volume->viscosity_multiplier;
        if (pCar->underwater_ability != 0) {
            k *= 0.6;
        }
        drag *= k;
        drag *= pObject->water_depth_factor;
    }

    k = BrVector3Length(&pObject->v) * drag * 6.9 / pObject->M;
    v.v[0] = pObject->v.v[0] * k;
    v.v[1] = pObject->v.v[1] * k;
    v.v[2] = pObject->v.v[2] * k;
    DRVector3Diminish(&pObject->v, &v);

    k = BrVector3Length(&pObject->omega) * drag;
    v.v[0] = pObject->omega.v[0] * k;
    v.v[1] = pObject->omega.v[1] * k;
    v.v[2] = pObject->omega.v[2] * k;
    ApplyWaterOmegaBrake(pObject, &v);
}

// DragChildren

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// FUNCTION: CARMA2_HW 0x00415890
void C2_HOOK_FASTCALL APTCPreCollision(void) {
    tPhysics_object* obj;
    tCar_spec* car;
    tNon_car_spec* non_car;
    tPhysics_object* node;
    void** wp;
    int i;
    float x;
    float f;

    gDrivable_on_count = 0;
    for (obj = gPHIL_list_collision_infos; obj != NULL && gDrivable_on_count < 50; obj = obj->next) {
        if (obj->drivable_on) {
            gDrivable_on_list[gDrivable_on_count] = obj;
            gDrivable_on_count += 1;
        }
    }

    gCar_to_view_original_v.v[0] = gCar_to_view->collision_info->v.v[0];
    gCar_to_view_original_v.v[1] = gCar_to_view->collision_info->v.v[1];
    gCar_to_view_original_v.v[2] = gCar_to_view->collision_info->v.v[2];

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            if (*wp != NULL && ((tCar_spec*)*wp)->driver > 5) {
                car = (tCar_spec*)*wp;
                BrMatrix34Copy(&car->car_master_actor->t.t.mat, &car->collision_info->transform_matrix);
                if (PO_U8(car->collision_info, 0x261) == 0x10
                        && car->collision_info->message_time == gPHIL_last_physics_tick) {
                    if (car == NULL || car->driver != eDriver_local_human) {
                        car->curvature = (float)(tS16)PO_U16(car->collision_info, 0x278) * car->maxcurve * 0.000030518509f;
                    }
                    if (gNet_mode == eNet_mode_host) {
                        car->collision_info->field_0xf0 = 1;
                    }
                    SetCollisionFlagsAndStuff(car);
                }
                if (car->disabled == 0) {
                    if (!(car->collision_info->disable_move_rotate != 0
                            && gPalette_fade_time != 0
                            && car != NULL
                            && car->driver == eDriver_local_human)) {
                        if (car->collision_info->box_face_ref != gFace_num__car
                                && (car->collision_info->box_face_ref != gFace_num__car - 1
                                    || car->collision_info->box_face_start <= gFace_count)) {
                            GetFacesInBox(car->collision_info, &gWorld_callbacks);
                        }
                        if (car->dt != 0.f) {
                            MoveAndCollideCar(car, 0.04f);
                        }
                        if (car->collision_info->child != NULL) {
                            PhysicsObjectMoveVelocityList(car->collision_info->child);
                            for (node = car->collision_info->child; node != NULL; node = node->next) {
                                AddDrag(car, node, 0.04f);
                                if (node->child != NULL) {
                                    DragChildren(car, node->child);
                                }
                            }
                            PositionChildren(car->collision_info);
                        }
                    }
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_active_non_cars) {
        wp = (void**)gActive_non_car_list;
        do {
            non_car = (tNon_car_spec*)*wp;
            if (PO_U8(non_car->collision_info, 0x261) != 0
                    && non_car->collision_info->message_time == gPHIL_last_physics_tick) {
                SetCollisionFlagsAndStuff((tCar_spec*)non_car);
            }
            if (non_car->flags & 0x10000) {
                if (PO_U32(non_car, 0xf8) != 0 && PO_U32(non_car, 0xf8) < GetRaceTime()) {
                    PO_U32(non_car, 0xf8) = 0;
                    non_car->flags = (non_car->flags & 0xffff0100) | 0x100;
                    non_car->collision_info->disable_move_rotate = 0;
                }
            }
            if (non_car->collision_info->disable_move_rotate == 0
                    || (non_car->flags & 0xff) != 0
                    || (non_car->flags & 0xff00) != 0) {
                non_car->collision_info->disable_move_rotate = 0;
                if (PO_F32(non_car, 0xc4) != 0.f) {
                    MoveAndCollideNonCar(non_car, 0.04f);
                }
                if (non_car->collision_info->child != NULL) {
                    PhysicsObjectMoveVelocityList(non_car->collision_info->child);
                    PositionChildren(non_car->collision_info);
                }
            }
            i++;
            wp++;
        } while (i < gNum_active_non_cars);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                f = car->collision_info->M; f *= x; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= x; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= x; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= x; car->collision_info->I.v[2] = f;
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;

        c->collision_info->transform_matrix.m[3][1] -= gFneg1000;
        BrMatrix34Copy(&c->car_master_actor->t.t.mat, &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 1);
    }
    MinePreCollisionStuff();
    PedPreCollisionStuff();
    DronePreCollisionStuff();
}

// FUNCTION: CARMA2_HW 0x004161a0
// FUNCTION: CARMA2_SW 0x004161d0
void C2_HOOK_FASTCALL APTCPostCollisionTree(tPhysics_object* pObject) {
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
        APTCPostCollisionTree(pObject->child);
    }
}

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

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

// CrashNoise

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

// DoComplexCarModels

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

// PipeNonCarObject

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


// FUNCTION: CARMA2_HW 0x004c2c90
void C2_HOOK_FASTCALL ApplyWaterOmegaBrake(tPhysics_object* pObject, const br_vector3* pV) {
    br_scalar b0;
    br_scalar b1;
    br_scalar b2;
    br_vector3 om;

    b0 = (pV->v[0] * 0.0210039914f) / pObject->I.v[0];
    om = pObject->omega;
    b1 = (pV->v[1] * 0.0210039914f) / pObject->I.v[1];
    b2 = (pV->v[2] * 0.0210039914f) / pObject->I.v[2];
    pObject->omega.v[0] -= b0;
    pObject->omega.v[1] -= b1;
    pObject->omega.v[2] -= b2;
    if (om.v[0] * pObject->omega.v[0] <= 0.0f) {
        pObject->omega.v[0] = 0.0f;
    }
    if (om.v[1] * pObject->omega.v[1] <= 0.0f) {
        pObject->omega.v[1] = 0.0f;
    }
    if (om.v[2] * pObject->omega.v[2] <= 0.0f) {
        pObject->omega.v[2] = 0.0f;
    }
}


// FUNCTION: CARMA2_HW 0x00414910
void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2) {
    tFace_ref* pFace;
    br_bounds3 current_bounds;
    br_scalar old_d;
    float prop7;

    pFace = (tFace_ref*)arg2;
    old_d = pCollision->water_d;
    if (pCollision->owner == NULL) {
        return;
    }
    prop7 = PHILGetObjectProperty(pCollision, 7);
    if (pCollision->flags_0x238 != 1 && prop7 == 0.f) {
        return;
    }
    if (pCollision != NULL
        && pCollision->owner != NULL
        && pCollision->flags_0x238 == 1
        && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human
        && pCollision->water_d != 10000.f
        && gDouble_pling_water
        && BrVector3Dot(&pCollision->water_normal, &pCollision->field_0xf4.max) - pCollision->water_d <= 0.f) {
        gInTheSea = 1;
        FreezeCamera();
    }
    if (pFace != NULL && fabs(pFace->normal.v[1]) > 0.9f) {
        BrVector3Copy(&pCollision->water_normal, &pFace->normal);
        if (pCollision->water_normal.v[1] < 0.f) {
            BrVector3Negate(&pCollision->water_normal, &pCollision->water_normal);
        }
        pCollision->water_d = (((pFace->v[0].v[2]) * (pCollision->water_normal.v[2])) + ((pFace->v[0].v[1]) * (pCollision->water_normal.v[1]))) + ((pFace->v[0].v[0]) * (pCollision->water_normal.v[0]));
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            if (pFace->material->identifier[1] == '!') {
                if (BrVector3Dot(&pCollision->field_0xf4.min, &pCollision->water_normal) - pCollision->water_d < 0.f) {
                    GetNewBoundingBox(&current_bounds, &pCollision->bb1, &pCollision->actor->t.t.mat);
                    if ((((pCollision->water_normal.v[0]) * (current_bounds.min.v[0])) + ((pCollision->water_normal.v[1]) * (current_bounds.min.v[1]))) + ((pCollision->water_normal.v[2]) * (current_bounds.min.v[2])) - pCollision->water_d < 0.f) {
                        gInTheSea = 1;
                        FreezeCamera();
                    }
                }
                gDouble_pling_water = 1;
            } else {
                gDouble_pling_water = 0;
            }
        }
    } else {
        pCollision->water_d = 10000.f;
        if (pCollision != NULL
            && pCollision->owner != NULL
            && pCollision->flags_0x238 == 1
            && ((tCar_spec*)pCollision->owner)->driver == eDriver_local_human) {
            gInTheSea = gInTheSea == 1 ? 2 : 0;
        }
    }
    if (fabs(old_d - pCollision->water_d) > 1e-05) {
        if (PHILGetObjectProperty(pCollision, 6) != 0.f) {
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision);
            AREndPipingSession();
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 == 1) {
            tCar_spec* owner = (tCar_spec*)pCollision->owner;
            if (owner != NULL && owner->driver > 5) {
                ARStartPipingSession((tPipe_chunk_type)20);
                AddSplashToPipingSession(pCollision);
                AREndPipingSession();
                return;
            }
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision->actor);
            AREndPipingSession();
            return;
        }
        if (pCollision != NULL && pCollision->owner != NULL && pCollision->flags_0x238 >= 2 && pCollision->flags_0x238 <= 4) {
            ARStartPipingSession((tPipe_chunk_type)59);
            AddExtendedSplashToPipingSession(pCollision, pCollision->owner);
            AREndPipingSession();
        }
    }
}

// STUB: CARMA2_HW 0x0041f4f0
void C2_HOOK_FASTCALL FlyCar(tCar_spec* c, br_scalar dt) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00416030
void C2_HOOK_FASTCALL DragChildren(tCar_spec* pCar, tPhysics_object* pChild) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00417030
void C2_HOOK_FASTCALL MoveAndCollideNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004168c0
void C2_HOOK_FASTCALL MoveAndCollideCar(tCar_spec* pCar, br_scalar pDt) {
    NOT_IMPLEMENTED();
}


// FUNCTION: CARMA2_HW 0x00415c40
void C2_HOOK_FASTCALL SetCollisionFlagsAndStuff(tCar_spec* pCar) {
    float temp;
    int i;

    if (gNet_mode == eNet_mode_host
            && PO_U32(pCar->collision_info, 0x49c) > PO_U32(pCar->collision_info, 0x274)) {
        PO_U8(pCar->collision_info, 0x261) = 0;
        pCar->dt = -1.f;
        return;
    }

    switch (pCar->collision_info->field_0xf0) {
    case 0:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 2);
        break;
    case 1:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 1);
        break;
    case 2:
        SelectCrushNetwork((undefined4*)&PO_U8(pCar->collision_info, 0x294), pCar->collision_info, 1);
        break;
    }

    if (pCar != NULL && pCar->driver > 5) {
        for (i = 0; i < 4; i++) {
            pCar->oldd[i] = (float)PO_U8(pCar->collision_info, 0x26c + i)
                    * pCar->susp_height[i >> 1] * (1.f / 255.f);
        }

        if (pCar->driver == eDriver_net_human) {
            pCar->revs = (float)PO_U16(pCar->collision_info, 0x27a);
        }

        if (pCar->driver >= eDriver_net_human
                && PO_U32(pCar->collision_info, 0x27c) > pCar->repair_time) {
            if (PO_U32(pCar->collision_info, 0x27c) - pCar->repair_time >= 100000) {
                TotallyRepairACar(pCar);
                pCar->repair_time = PO_U32(pCar->collision_info, 0x27c);
            } else {
                ComputeCarImpact(pCar, PO_U32(pCar->collision_info, 0x27c) - pCar->repair_time, &temp);
            }
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = PO_U8(pCar->collision_info, 0x280 + i);
            }
            SetSmokeLastDamageLevel(pCar);
            StopCarSmoking(pCar);
        } else {
            for (i = 0; i < 12; i++) {
                pCar->damage_units[i].damage_level = PO_U8(pCar->collision_info, 0x280 + i);
            }
            SortOutSmoke(pCar);
        }

        if (pCar->collision_info->disable_move_rotate != 0) {
            if (BrVector3LengthSquared(&pCar->collision_info->v) != 0.f) {
                pCar->collision_info->disable_move_rotate = 0;
            }
        }

        if (pCar->driver >= eDriver_net_human
                && (PO_U8(pCar->collision_info, 0x292) & 1) != 0
                && (PO_U16(pCar->collision_info, 0x290) != pCar->car_crush_spec->field_0x24
                        || pCar->driver == eDriver_local_human)) {
            if ((PO_U8(pCar->collision_info, 0x292) & 2) != 0) {
                WeldCarByNetwork(pCar);
            } else if (pCar->car_crush_spec->field_0x144 != 0) {
                CompletelyUnBendCar(pCar);
            }

            if ((PO_U8(pCar->collision_info, 0x292) & 4) != 0) {
                WeldCarPartial(pCar);
            } else if (pCar->car_crush_spec->field_0x4b8 != 0) {
                WeldCar(pCar);
            }

            UpdateCrushVertices(pCar, &PO_U8(pCar->collision_info, 0x2fc));
        }

        if (pCar->driver != eDriver_local_human) {
            for (i = 0; i < 4; i++) {
                *(tU32*)&pCar->wheel_dam_offset[i] = PO_U32(pCar->collision_info, 0x48c + i * 4);
            }
        }

        GetFacesInBox(pCar->collision_info, &gWorld_callbacks);
    }

    PO_U8(pCar->collision_info, 0x261) = 0;
    if (gNet_mode == eNet_mode_client) {
        PO_U32(pCar->collision_info, 0x49c) = PO_U32(pCar->collision_info, 0x274);
    }
}

// STUB: CARMA2_HW 0x0043ace0
float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, tU32 pDelta, float* pOut) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x0043db00
void C2_HOOK_FASTCALL UpdateCrushVertices(tCar_spec* pCar, tU8* pData) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043dce0
void C2_HOOK_FASTCALL WeldCarByNetwork(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043df10
void C2_HOOK_FASTCALL WeldCarPartial(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}
