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


extern int gKey_mapping[77];
extern int gUNK_006793d0;
static br_scalar stop_timer;
static br_scalar slide_dist;

extern float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pMagnitude, br_vector3* pPosition, br_vector3* pNormal, br_vector3* pForce, int pSnap, int pNetwork);

typedef struct {
    br_actor* actor;
    tCar_spec* car;
    tU32 time;
} tUpdate_crush_reattach_item;

// GLOBAL: CARMA2_HW 0x0067be10
tUpdate_crush_reattach_item gNet_crush_reattach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b7c8
int gCount_net_crush_reattach_bit_list;

// GLOBAL: CARMA2_HW 0x00589a40
br_scalar gCrushZero = 0.0f;

// GLOBAL: CARMA2_HW 0x0058f680
int gMetal_crunch_sound_id[5] = { 5000, 5001, 5002, 5003, 5004 };

// GLOBAL: CARMA2_HW 0x0074cf98
int gUNK_0074cf98;

#define gFriction_materials gCurrent_race.material_modifiers

// GLOBAL: CARMA2_HW 0x00661678
br_vector3 gVector3_00661678 = { { 0.0f, 1.0f, 0.0f } };

// GLOBAL: CARMA2_HW 0x0058f6e0
tPhysics_callbacks gCar_physics_callbacks = {
    &gWorld_callbacks,
    APTCPreCollision,
    APTCPostCollision,
    APTCChangedObjects,
    APTCActiveHalted,
    (int (C2_HOOK_FASTCALL *)(tPhysics_object*))APTCPassiveActivated,
    NULL,
};

// GLOBAL: CARMA2_HW 0x006793c0
int gSkid_tag[2];

// GLOBAL: CARMA2_HW 0x006793c8
tCar_spec* gLast_car_to_skid[2];

// GLOBAL: CARMA2_HW 0x00676854
int gStop_opponents_moving;

// GLOBAL: CARMA2_HW 0x0074a5f4
int gOver_shoot;

// GLOBAL: CARMA2_HW 0x006793f4
tU32 last_frame_start;

// GLOBAL: CARMA2_HW 0x0079ef2c
int gNum_cars_and_non_cars;

// GLOBAL: CARMA2_HW 0x005896a8
float gF1000 = 1000.0f;

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

// FUNCTION: CARMA2_HW 0x00416070
void C2_HOOK_FASTCALL APTCPostCollision(void) {
    void** wp;
    tCar_spec* car;
    int i;

    if (gCar_flying) {
        tCar_spec* c = gCar_to_view;

        c->collision_info->transform_matrix.m[3][1] -= gF1000;
        BrMatrix34Copy(&c->car_master_actor->t.t.mat, &c->collision_info->transform_matrix);
        PositionChildren(c->collision_info);
        SetCollisionInfoChildsDoNothing(c->collision_info, 0);
    }

    i = 0;
    if (i < gNum_active_cars) {
        wp = (void**)gActive_car_list;
        do {
            float x;
            float fac;
            float f;

            car = (tCar_spec*)*wp;
            if (car->field_0x4c8 != gZero) {
                x = car->field_0x4c8;
                fac = (float)(gZero / x);
                f = car->collision_info->M; f *= fac; car->collision_info->M = f;
                f = car->collision_info->I.v[0]; f *= fac; car->collision_info->I.v[0] = f;
                f = car->collision_info->I.v[1]; f *= fac; car->collision_info->I.v[1] = f;
                f = car->collision_info->I.v[2]; f *= fac; car->collision_info->I.v[2] = f;
            }
            if (((tCar_spec*)*wp)->number_of_wheels_on_ground != 0) {
                SetCollisionInfoChildsDoNothing(((tCar_spec*)*wp)->collision_info, 0);
            }
            i++;
            wp++;
        } while (i < gNum_active_cars);
    }

    i = 0;
    if (i < gNum_cars_and_non_cars) {
        wp = (void**)gActive_car_list;
        do {
            car = (tCar_spec*)*wp;
            car->frame_collision_flag |= (int)(signed char)car->collision_info->collision_flag;
            i++;
            wp++;
        } while (i < gNum_cars_and_non_cars);
    }

    UpdateCrushTimers();
    if (gNet_mode) {
        UpdateCrushPainList();
    }
    APTCPostCollisionTree(gPHIL_list_collision_infos);
}

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

// FUNCTION: CARMA2_HW 0x00416340
void C2_HOOK_FASTCALL ApplyPhysicsToCars(tU32 pLast_tick_time, tU32 pFrame_period) {
    int i;

    if (gFreeze_mechanics) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        ForceRebuildActiveCarList();
    }
    gNum_cars_and_non_cars = gNum_active_non_cars + gNum_active_cars;
    if (gNum_active_cars < gNum_cars_and_non_cars) {
        memcpy(&gActive_car_list[gNum_active_cars], gActive_non_car_list, (gNum_cars_and_non_cars - gNum_active_cars) * sizeof(void*));
    }
    last_frame_start = pLast_tick_time;
    for (i = 0; i < gNum_cars_and_non_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        car->frame_collision_flag = gOver_shoot && car->collision_info->collision_flag;
        if (car != NULL && car->driver > 5) {
            CalcModifiedLocation(car);
            if (car->driver == eDriver_oppo && gStop_opponents_moving) {
                car->acc_force = 0.f;
                car->brake_force = 0.f;
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
            }
            if (!car->wheel_slip) {
                if (gLast_car_to_skid[0] == car) {
                    DRS3StopSound(gSkid_tag[0]);
                }
                if (gLast_car_to_skid[1] == car) {
                    DRS3StopSound(gSkid_tag[1]);
                }
            }
            if (car->driver == eDriver_net_human && car->collision_info->message_time < pLast_tick_time - 1000) {
                car->keys.acc = 0;
                car->keys.dec = 0;
                car->joystick.acc = -1;
                car->joystick.dec = -1;
                car->keys.horn = 0;
            }
            SetSmokeLastDamageLevel(car);
        }
    }
    PHILDoPhysics(&gCar_physics_callbacks, pLast_tick_time, pFrame_period);
    if (TimeToSendData()) {
        SendCarData(gPHIL_last_physics_tick);
        SendMines(gPHIL_last_physics_tick);
    }
    FinishCars(pLast_tick_time + pFrame_period, pFrame_period);
    CheckForDeAttachmentOfNonCars(pFrame_period);
}

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

// FUNCTION: CARMA2_HW 0x00418850
void C2_HOOK_FASTCALL CalcForce(tCar_spec* pCar, br_scalar pDt) {
    int i;
    int j;
    int normnum;
    int vol_mod;
    int count;
    int best_index;
    int ffb_force;
    br_scalar force[4];
    br_scalar d[4];
    tFace_ref* collected[4];
    br_scalar dd[4];
    br_scalar rt[4];
    br_scalar val[4];
    br_scalar best;
    br_scalar wheelratio;
    br_scalar k;
    br_scalar ts;
    br_scalar ts2;
    br_scalar ts3;
    br_scalar ts4;
    br_scalar maxfl;
    br_scalar maxfr;
    br_scalar friction_number;
    br_scalar deltaomega;
    br_scalar v98;
    br_scalar v99;
    br_scalar v87;
    br_scalar v106;
    br_scalar v108;
    br_scalar v109;
    br_scalar v116;
    br_scalar v125;
    br_scalar v128;
    br_scalar v129;
    br_scalar v134;
    br_scalar v135;
    br_scalar pV;
    br_scalar fl_oil_factor;
    br_scalar fr_oil_factor;
    br_scalar rl_oil_factor;
    br_scalar rr_oil_factor;
    int delta;
    br_vector3 b;
    br_vector3 f;
    br_vector3 B;
    br_vector3 road_normal;
    br_vector3 v;
    br_vector3 tv;
    br_vector3 tmp;
    br_vector3 ray_dir;
    br_vector3 normal;
    br_vector3 nor2;
    br_vector3 vplane;
    br_vector3 rightplane;
    br_vector3 a;
    br_vector3 v103;
    br_vector3 v136;
    br_vector3 v123;
    br_vector3 norm[4];
    br_vector3 ray_pos[4];
    br_vector3 contact[4];
    br_vector3 wheel_pos[4];
    br_bounds bounds;
    tFace_ref* faces[4];
    tPhysics_object* objs[4];
    tPhysics_object* pObj;
    tPhysics_object** pList;
    tFace_ref* pFace;
    br_matrix34* mat;
    tSpecial_volume* vol;
    tMaterial_modifiers* mat_list;
    br_scalar* pd;

    B.v[0] = 0.f;
    B.v[1] = 0.f;
    B.v[2] = 0.f;
    pCar->curvature = pCar->curvature + pCar->field_0x1260;
    pCar->field_0x195c = 0;
    vol = pCar->collision_info->last_special_volume;
    wheelratio = (pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
        / (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]);
    faces[0] = NULL;
    faces[1] = NULL;
    faces[2] = NULL;
    faces[3] = NULL;
    normnum = 0;
    objs[0] = NULL;
    objs[1] = NULL;
    objs[2] = NULL;
    objs[3] = NULL;
    vol_mod = 0;
    f.v[0] = 0.f;
    f.v[1] = 0.f;
    f.v[2] = 0.f;
    mat = &pCar->car_master_actor->t.t.mat;
    b.v[0] = -mat->m[1][0];
    b.v[1] = -mat->m[1][1];
    b.v[2] = -mat->m[1][2];
    v.v[0] = pCar->collision_info->v.v[0] * 6.9;
    v.v[1] = pCar->collision_info->v.v[1] * 6.9;
    v.v[2] = pCar->collision_info->v.v[2] * 6.9;
    road_normal.v[0] = 0.f;
    road_normal.v[1] = 0.f;
    road_normal.v[2] = 0.f;
    for (i = 0; i < 4; i++) {
        BrMatrix34ApplyP(&wheel_pos[i], &pCar->wpos[i], mat);
    }
    k = (pCar->susp_height[0] < pCar->susp_height[1] ? pCar->susp_height[0] : pCar->susp_height[1]) - (-0.5);
    d[0] = 2.f;
    d[1] = 2.f;
    BrVector3Scale(&tmp, &b, k);
    d[2] = 2.f;
    d[3] = 2.f;
    for (i = 0; i < 4; i++) {
        BrVector3InvScale(&ray_pos[i], &wheel_pos[i], WORLD_SCALE);
    }
    BrVector3InvScale(&ray_dir, &tmp, WORLD_SCALE);
    for (i = pCar->collision_info->box_face_start, pFace = &gFace_list__car[i];
            i < pCar->collision_info->box_face_end;
            i++, pFace++) {
        if (gUNK_006793d0 != 0 && (pFace->flags & 0x80) != 0) {
            continue;
        }
        MultiRayCheckSingleFace(4, pFace, ray_pos, &ray_dir, &normal, rt);
        for (j = 0; j < 4; j++) {
            if (rt[j] < d[j]) {
                faces[j] = pFace;
                d[j] = rt[j];
                BrVector3Copy(&pCar->nor[j], &normal);
            }
        }
    }
    bounds.max = ray_pos[0];
    bounds.min = ray_pos[0];
    for (i = 1; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            if (ray_pos[i].v[j] < bounds.min.v[j]) {
                bounds.min.v[j] = ray_pos[i].v[j];
            }
            if (bounds.max.v[j] < ray_pos[i].v[j]) {
                bounds.max.v[j] = ray_pos[i].v[j];
            }
        }
    }
    for (j = 0; j < 3; j++) {
        if (ray_dir.v[j] < 0.f) {
            bounds.min.v[j] = bounds.min.v[j] + ray_dir.v[j];
        } else {
            bounds.max.v[j] = bounds.max.v[j] + ray_dir.v[j];
        }
    }
    for (i = 0, pList = gDrivable_on_list; i < (int)gDrivable_on_count; i++, pList++) {
        pObj = *pList;
        if (!BoundsOverlapTest__finteray(&bounds, &pObj->field_0xf4)) {
            continue;
        }
        BrMatrix34TApplyV(&ray_dir, &ray_dir, &pObj->transform_matrix);
        if (pObj->shape == NULL) {
            continue;
        }
        for (;;) {
            BrMatrix34ApplyP(&pObj->cmpos, &pObj->cmpos, &pObj->transform_matrix);
            for (j = 0; j < 4; j++) {
                DrMatrix34ApplyLPInverse(&tmp, &ray_pos[j], &pObj->transform_matrix);
                ShapeRayCast(&tmp, &ray_dir, (const tPhysics_shape*)pObj->shape, &pObj->cmpos, &ts, &normal);
                if (ts < d[j]) {
                    normal.v[0] = -normal.v[0];
                    normal.v[1] = -normal.v[1];
                    normal.v[2] = -normal.v[2];
                    BrMatrix34ApplyV(&normal, &normal, &pObj->transform_matrix);
                    d[j] = ts;
                    faces[j] = NULL;
                    objs[j] = pObj;
                }
            }
            if (pObj->next == NULL) {
                break;
            }
            pObj = (tPhysics_object*)pObj->next;
        }
    }
    if (pCar->collision_info->last_special_volume != NULL
        && pCar->collision_info->last_special_volume->material_modifier_index != 0) {
        vol_mod = pCar->collision_info->last_special_volume->material_modifier_index;
    }
    mat_list = gFriction_materials;
    for (i = 0, j = 0; j < 0x30; i++, j += 0xc) {
        pd = &d[i];
        *pd = *pd * k;
        ts = *pd;
        if (faces[i] != NULL
                && faces[i]->material != NULL
                && faces[i]->material->identifier != NULL) {
            pCar->material_index[i] = vol_mod != 0
                ? vol_mod
                : (signed char)faces[i]->material->identifier[0] - '/';
            if (pCar->material_index[i] < 0 || pCar->material_index[i] > 0xb) {
                pCar->material_index[i] = 0;
            }
            if (strlen(faces[i]->material->identifier) == 0xb
                    && faces[i]->material->identifier[5] == '|') {
                pCar->oil_remaining[i] = 0.f;
            }
        } else {
            pCar->material_index[i] = 0;
        }
        BrMatrix34TApplyV(&norm[i], &pCar->nor[i], mat);
        if (mat_list[pCar->material_index[i]].bumpiness != 0.f) {
            br_scalar tv0 = pCar->nor[i].v[0] * *pd;
            br_scalar tv2 = pCar->nor[i].v[2] * *pd;
            {
                int x = abs((int)(512.f * (tv0 + wheel_pos[i].v[0])));
                int y = abs((int)(512.f * (wheel_pos[i].v[2] + tv2)));
                x = x % 2048;
                y = y % 2048;
                if (x > 1024) {
                    x = 2048 - x;
                }
                if (y > 1024) {
                    y = 2048 - y;
                }
                if (x + y > 1024) {
                    delta = 2048 - x - y;
                } else {
                    delta = x + y;
                }
                delta = delta - 400;
                if (delta < 0) {
                    delta = 0;
                }
            }
            d[i] = delta * mat_list[pCar->material_index[i]].bumpiness / 42400.f
                * norm[i].v[1] + *pd;
        }
        if (*pd < -0.5f
                || pCar->wheel_dam_offset[i ^ 2] * 6.9 + pCar->susp_height[i / 2] < *pd) {
            force[i] = 0.f;
            d[i] = pCar->susp_height[i / 2];
        } else {
            BrVector3Accumulate(&road_normal, &norm[i]);
            normnum++;
            d[i] = d[i] - pCar->wheel_dam_offset[i ^ 2] * 6.9;
            force[i] = (pCar->susp_height[i / 2] - d[i]) * pCar->sk[i / 2];
            force[i] = force[i] - (d[i] - pCar->oldd[i]) / pDt * pCar->sb[i / 2];
            if (pCar->susp_height[i / 2] == pCar->oldd[i]
                && pCar->nor[i].v[0] * v.v[0] + pCar->nor[i].v[1] * v.v[1] + pCar->nor[i].v[2] * v.v[2] > -0.01f
                && pCar->collision_info->M * 5.f < force[i]) {
                d[i] = pCar->susp_height[i / 2];
                force[i] = pCar->collision_info->M * 5.f;
            }
            if (force[i] < 0.f) {
                force[i] = 0.f;
            }
            B.v[1] = force[i] + B.v[1];
            f.v[0] = f.v[0] - (pCar->wpos[i].v[2] - pCar->centre_of_mass_world_scale.v[2]) * force[i];
            f.v[2] = (pCar->wpos[i].v[0] - pCar->centre_of_mass_world_scale.v[0]) * force[i] + f.v[2];
        }
        dd[i] = pCar->oldd[i] - d[i];
        pCar->oldd[i] = d[i];
        if (objs[i] == NULL || force[i] == 0.f) {
            continue;
        }
        pCar->field_0x195c = (unsigned int)objs[i];
        if (objs[i]->disable_move_rotate != 0
                && objs[i]->owner != NULL
                && objs[i]->flags_0x238 == 1
                && ((tNon_car_spec*)objs[i]->owner)->driver <= 5
                && (((tNon_car_spec*)objs[i]->owner)->flags & 0x10000) != 0) {
            continue;
        }
        objs[i]->disable_move_rotate = 0;
        ts = pCar->collision_info->M * pDt / (force[i] / 6.9);
        tmp.v[0] = ts * b.v[0];
        tmp.v[1] = ts * b.v[1];
        tmp.v[2] = ts * b.v[2];
        objs[i]->v.v[0] = tmp.v[0] + objs[i]->v.v[0];
        objs[i]->v.v[1] = tmp.v[1] + objs[i]->v.v[1];
        objs[i]->v.v[2] = tmp.v[2] + objs[i]->v.v[2];
        if (gUNK_006793d0 == 2) {
            pCar->collision_info->field_0x49c = (int)&gZero_v__car;
        }
    }
    if (objs[0] != NULL
            && objs[0] == objs[1]
            && objs[0] == objs[2]
            && objs[0] == objs[3]
            && objs[0]->owner != NULL
            && objs[0]->flags_0x238 == 1
            && ((tNon_car_spec*)objs[0]->owner)->driver <= 5
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x10000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0x40000) != 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff) == 0
            && (((tNon_car_spec*)objs[0]->owner)->flags & 0xff00) == 0
            && ((tNon_car_spec*)objs[0]->owner)->field_0xf8 == 0
            && gUNK_006793d0 != 3) {
        ((tNon_car_spec*)objs[0]->owner)->flags = (((tNon_car_spec*)objs[0]->owner)->flags & 1) | 1;
    }
    if (pCar->collision_info->disable_move_rotate != 0 && normnum == 0) {
        goto steer_centre;
    }
    if (pCar->collision_info->disable_move_rotate != 0) {
        SetCollisionInfoDoNothingChain(pCar->collision_info, 0);
    }
    if (pCar == NULL || pCar->driver <= 5 || pCar->wall_climber_mode == 0
            || (road_normal.v[0] == 0.f && road_normal.v[1] == 0.f && road_normal.v[2] == 0.f)) {
        friction_number = pCar->collision_info->M * 10.f;
        if (vol != NULL) {
            friction_number = (1.f - vol->gravity_multiplier) * pCar->collision_info->water_depth_factor;
            if (pCar->underwater_ability != 0) {
                friction_number = friction_number * 0.6f;
            }
            friction_number = (1.f - friction_number) * pCar->collision_info->M;
        } else {
            friction_number = pCar->collision_info->M;
        }
        friction_number = friction_number * gGravity_multiplier * 10.f;
        B.v[0] = B.v[0] - mat->m[0][1] * friction_number;
        B.v[1] = B.v[1] - mat->m[1][1] * friction_number;
        B.v[2] = B.v[2] - mat->m[2][1] * friction_number;
    } else {
        BrVector3Normalise(&tmp, &road_normal);
        BrVector3Scale(&tmp, &tmp, -(pCar->collision_info->M * 10.0f));
        BrVector3Accumulate(&B, &tmp);
    }
steer_centre:
    if (pCar != NULL && pCar->driver >= 7) {
        if (pCar->curvature > pCar->maxcurve) {
            pCar->curvature = pCar->maxcurve;
        }
        if (pCar->curvature < -pCar->maxcurve) {
            pCar->curvature = -pCar->maxcurve;
        }
        if (!(pCar->keys.left || pCar->keys.right || pCar->keys.holdw)
            && (gUNK_0074cf98 != 0
                || (pCar->joystick.left <= 0 && pCar->joystick.right <= 0))
            && (pCar->oldd[2] < pCar->susp_height[1] || pCar->oldd[3] < pCar->susp_height[1])) {
            ts = -((pCar->collision_info->omega.v[2] * road_normal.v[2]
                        + pCar->collision_info->omega.v[1] * road_normal.v[1]
                        + pCar->collision_info->omega.v[0] * road_normal.v[0])
                * (pDt / (pCar->wpos[0].v[2] - pCar->wpos[2].v[2])));
            ts2 = -(pCar->curvature * pDt);
            if (fabs(ts) < fabs(ts2) || ts * ts2 < 0.f) {
                ts = ts2;
            }
            if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
                pCar->curvature += ts;
                if (pCar->curvature * ts > 0.f) {
                    pCar->curvature = 0.f;
                }
            }
            ffb_force = (int)(ts * 240000.f);
        } else {
            ffb_force = 0;
        }
        if (pCar->driver == eDriver_local_human) {
            SetJoystickFFBSteeringForce(ffb_force - (int)((dd[2] - dd[3]) * -30000.f));
        }
    }
    if (normnum != 0) {
        count = 0;
        best_index = -1;
        best = 0.f;
        for (i = 0; i < 4; i++) {
            pFace = faces[i];
            if (pFace == NULL || pFace->material == NULL
                    || pFace->material->identifier == NULL) {
                continue;
            }
            if (strlen(pFace->material->identifier) != 0xb
                    || pFace->material->identifier[5] != '|') {
                continue;
            }
            for (j = 0; j < count; j++) {
                if (collected[j] == pFace) {
                    break;
                }
            }
            if (j != count) {
                continue;
            }
            contact[count].v[0] = -pCar->oldd[i] * mat->m[1][0];
            contact[count].v[1] = -pCar->oldd[i] * mat->m[1][1];
            contact[count].v[2] = -pCar->oldd[i] * mat->m[1][2];
            val[count] = force[i] * 0.04f;
            contact[count].v[0] += wheel_pos[i].v[0];
            contact[count].v[1] += wheel_pos[i].v[1];
            contact[count].v[2] += wheel_pos[i].v[2];
            if (val[count] > best) {
                best = val[count];
                best_index = count;
            }
            collected[count] = pFace;
            count++;
        }
        if (best_index >= 0) {
            BrVector3InvScale(&contact[best_index], &contact[best_index], WORLD_SCALE);
            tv.v[0] = -val[best_index] * mat->m[1][0];
            tv.v[1] = -val[best_index] * mat->m[1][1];
            tv.v[2] = -val[best_index] * mat->m[1][2];
            SmashEnvironment(pCar->collision_info,
                (undefined4*)collected[best_index],
                val[best_index], &contact[best_index],
                &pCar->collision_info->v, &tv, 0, 0);
        }
        FlushSmashQueue(1);
        BrVector3NormaliseQuick(&road_normal, &road_normal);
        friction_number = road_normal.v[1] * mat->m[1][1]
            + road_normal.v[2] * mat->m[2][1]
            + road_normal.v[0] * mat->m[0][1];
        if (pCar->driver > 5 && pCar->wall_climber_mode != 0) {
            friction_number = 1.f;
        }
        friction_number = mat_list[pCar->material_index[0]].down_force * friction_number;
        if (friction_number > 0.f) {
            friction_number = fabs(pCar->collision_info->velocity_car_space.v[2])
                * pCar->collision_info->M * 10.0 * friction_number / pCar->downforce_to_weight;
            if (pCar->collision_info->M * 10.0 < friction_number) {
                friction_number = pCar->collision_info->M * 10.0;
            }
            if (pCar->number_of_wheels_on_ground == 4
                && pCar->oldd[2] == pCar->susp_height[1]
                && pCar->oldd[3] == pCar->susp_height[1]) {
                br_vector3 pos;
                br_vector3 delta;
                br_material* material;

                pos.v[0] = (mat->m[3][0] + mat->m[2][0] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                pos.v[1] = (mat->m[3][1] + mat->m[2][1] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                pos.v[2] = (mat->m[3][2] + mat->m[2][2] * pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[0] = b.v[0] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[1] = b.v[1] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                delta.v[2] = b.v[2] * (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * (1.f / WORLD_SCALE);
                ActorFindFace(&pos, &delta, gTrack_actor, &nor2, &ts, &material, NULL);
                if (ts > 1.f) {
                    pCar->field_0x12a8 = 1;
                }
            }
            if (pCar->field_0x12a8 != 0
                && (pCar->oldd[2] < pCar->susp_height[1] || pCar->oldd[3] < pCar->susp_height[1])) {
                pCar->field_0x12a8 = 0;
            }
            if (pCar->field_0x12a8 != 0) {
                friction_number = friction_number
                    * ((pCar->wpos[2].v[2] - pCar->centre_of_mass_world_scale.v[2])
                        / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]));
                f.v[0] = f.v[0]
                    + (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number;
            }
            B.v[1] = B.v[1] - friction_number;
        }
        ts2 = BrVector3Dot(&pCar->collision_info->velocity_car_space, &road_normal);
        vplane.v[0] = pCar->collision_info->velocity_car_space.v[0] - ts2 * road_normal.v[0];
        vplane.v[1] = pCar->collision_info->velocity_car_space.v[1] - ts2 * road_normal.v[1];
        vplane.v[2] = pCar->collision_info->velocity_car_space.v[2] - ts2 * road_normal.v[2];
        if (vplane.v[2] < 0.f) {
            ts = 1.f;
        } else {
            ts = -1.f;
        }
        ts2 = BrVector3Length(&vplane);
        deltaomega = ts2 * pCar->curvature * ts;
        deltaomega = deltaomega - BrVector3Dot(&pCar->collision_info->omega, &road_normal);
        BrVector3Set(&v103, road_normal.v[1], -road_normal.v[0], 0.f);
        BrVector3Normalise(&v103, &v103);
        friction_number = pCar->collision_info->I.v[1] / pDt * deltaomega;
        ts = friction_number / (pCar->wpos[2].v[2] - pCar->wpos[0].v[2]);
        v108 = ts;
        v109 = -ts;
        BrVector3Set(&rightplane, 0.f, road_normal.v[2], -road_normal.v[1]);
        BrVector3Normalise(&rightplane, &rightplane);
        v99 = pCar->acc_force;
        friction_number = BrVector3Dot(&rightplane, &vplane);
        v108 = BrVector3Dot(&v103, &vplane);
        ts2 = fabs(v108);
        friction_number = (pCar->wpos[0].v[2] - pCar->centre_of_mass_world_scale.v[2]) * friction_number * fabs(pCar->curvature);
        if (pCar->curvature <= 0.f) {
            friction_number = v108 - friction_number;
        } else {
            friction_number = v108 + friction_number;
        }
        friction_number = -(pCar->collision_info->M / pDt * friction_number);
        friction_number = friction_number - BrVector3Dot(&B, &v103);
        friction_number = friction_number / (1.0 - wheelratio);
        v108 = friction_number + v108;
        v109 = -wheelratio * friction_number + v109;
        friction_number = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * v108;
        v98 = friction_number * pCar->curvature;
        friction_number = BrVector3Dot(&pCar->collision_info->velocity_car_space, &rightplane) * pCar->collision_info->M / pDt;
        v129 = BrVector3Dot(&rightplane, &B) + friction_number;
        v128 = pCar->mu.v[0] * pCar->brake_force / (pCar->mu.v[1] / pCar->traction_multiplier + pCar->mu.v[0]);
        v125 = pCar->brake_force - v128;
        ts = (pCar->damage_units[7].damage_level + pCar->damage_units[6].damage_level) / 2;
        if (ts > 20.f) {
            v128 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v128;
        }
        ts = (pCar->damage_units[5].damage_level + pCar->damage_units[4].damage_level) / 2;
        if (ts > 20.f) {
            v125 = (1.f - (ts - 20.f) / 80.f) * (1.f - (ts - 20.f) / 80.f) * v125;
        }
        ts2 = (force[1] + force[0]) * pCar->rolling_resistance + v128;
        v106 = (force[2] + force[3]) * pCar->steerable_rolling_resistance + v125;
        v128 = pCar->wpos[0].v[2] - pCar->wpos[2].v[2];
        v128 = sqrt(v128 * v128 * pCar->curvature * pCar->curvature + 1.0);
        v106 = v106 / v128;
        v134 = v106 + ts2;
        if (fabs(v129) < fabs(v134)) {
            ts2 = v129 / v134 * ts2;
            v106 = v129 / v134 * v106;
        }
        if ((v106 + ts2) * v129 < 0.f) {
            ts2 = -ts2;
            v106 = -v106;
        }
        v129 = v129 - (ts2 + v106);
        v99 = v99 - ts2;
        if (pCar->keys.brake
                && pCar->damage_units[7].damage_level < 60
                && pCar->damage_units[6].damage_level < 60) {
            v99 = v99 - v129;
            pCar->gear = 0;
        }
        v99 = v99 / pCar->traction_multiplier;
        v135 = sqrt(v99 * v99 + v109 * v109) / 2.0;
        GetOilFrictionFactors(pCar, &fl_oil_factor, &fr_oil_factor, &rl_oil_factor, &rr_oil_factor);
        for (i = 0; i < 4; i++) {
            if (pCar->oil_remaining[i] != 0.f) {
                ConditionallyNoteSkid(pCar, faces, i);
            }
        }
        if (pCar->driver <= 5) {
            v116 = 1.f;
        } else {
            v116 = pCar->grip_multiplier;
        }
        BrVector3Sub(&a, &pCar->wpos[0], &pCar->centre_of_mass_world_scale);
        BrVector3Cross(&a, &pCar->collision_info->omega, &a);
        BrVector3Accumulate(&a, &pCar->collision_info->velocity_car_space);
        if (pCar != NULL && pCar->driver >= eDriver_net_human
            && (((pCar->keys.left || pCar->joystick.left > 0x8000) && pCar->curvature > 0.f && deltaomega > 0.1 && a.v[0] > 0.f)
                || ((pCar->keys.right || pCar->joystick.right > 0x8000) && pCar->curvature < 0.f && deltaomega < 0.1 && a.v[0] < 0.f))
            && ts > 0.f) {
            friction_number = pCar->mu.v[0];
        } else {
            friction_number = pCar->mu.v[2];
            ts2 = fabs(a.v[0]) / 10.f;
            if (ts2 > 1.f) {
                ts2 = 1.f;
            }
            friction_number = (pCar->mu.v[2] - pCar->mu.v[0]) * ts2 + friction_number;
        }
        maxfl = sqrt(force[0]) * friction_number * (rl_oil_factor * v116) * mat_list[pCar->material_index[0]].tyre_road_friction;
        maxfr = sqrt(force[1]) * friction_number * (rr_oil_factor * v116) * mat_list[pCar->material_index[1]].tyre_road_friction;
        pCar->max_force_rear = maxfr + maxfl;
        if (rl_oil_factor == 1.f && rr_oil_factor == 1.f && pCar->traction_control
                && v135 * 2.f > pCar->max_force_rear && pCar->acc_force > 0.f
                && (pCar == NULL || pCar->driver < eDriver_net_human
                    || (pCar->target_revs > 1000.f && pCar->gear > 0))) {
            ts2 = v99;
            if (v99 * v99 <= v135 * v135 * 4.f) {
                v87 = sqrt(v135 * v135 * 4.f - v99 * v99);
            } else {
                v87 = 0.f;
            }
            if (pCar->max_force_rear <= v87) {
                pCar->torque = -(pCar->revs * pCar->revs / 100000000.f) - 0.1f;
            } else {
                float v177 = sqrt(pCar->max_force_rear * pCar->max_force_rear - v87 * v87);
                ts3 = ts2 < 0.f ? -1.f : 1.f;
                ts4 = (ts2 - ts3 * v177) * 0.99f;
                if (fabs(ts2) > fabs(ts4)) {
                    ts2 = ts4;
                }
            }
            v99 = v99 - ts2;
            v135 = sqrt(v99 * v99 + v109 * v109) / 2.f;
        } else if (pCar != NULL && pCar->driver >= eDriver_net_human && pCar->gear > 0
                && pCar->revs > pCar->target_revs && !pCar->traction_control) {
            if (!pCar->keys.change_down) {
                pCar->traction_control = 1;
            }
            friction_number = 1.f - (pCar->revs - pCar->target_revs) / (float)(400 * pCar->gear);
            if (friction_number < 0.4f) {
                friction_number = 0.4f;
            }
            maxfl = friction_number * maxfl;
            maxfr = friction_number * maxfr;
        }
        if (fabs(v109) > maxfr + maxfl && maxfr + maxfl > 0.1f) {
            v106 = (maxfr + maxfl) / fabs(v109) * pDt;
            v109 = v106 * v109;
            v99 = pCar->traction_multiplier * v106 * v99;
        }
        v98 = v98 - v106;
        v108 = (pCar->wpos[0].v[2] - pCar->wpos[2].v[2]) * pCar->curvature * v106 + v108;
        if (v135 > 0.0001f) {
            v109 = v109 / (v135 * 2.f);
            v99 = v99 / (v135 * 2.0);
        }
        v99 = pCar->traction_multiplier * v99;
        force[0] = v135;
        force[1] = v135;
        pCar->wheel_slip = 0;
        switch ((force[0] > maxfl) + 2 * (force[1] > maxfr)) {
        case 0:
            slide_dist = 0.f;
            break;
        case 1:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = v135 - force[0] + force[1];
            if (force[1] <= maxfr) {
                slide_dist = 0.f;
            } else {
                if (maxfr > 0.1f) {
                    pV = (force[1] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 1);
                    }
                    SkidNoise(pCar, 1, pV, pCar->material_index[1]);
                }
                force[1] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 2;
            }
            break;
        case 2:
            force[1] = pCar->friction_slipping_reduction * maxfr;
            force[0] = v135 - force[1] + force[0];
            if (force[0] <= maxfl) {
                slide_dist = 0.f;
            } else {
                if (maxfl > 0.1f) {
                    pV = (force[0] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 0);
                    }
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
                force[0] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 2;
            }
            break;
        case 3:
            force[0] = pCar->friction_slipping_reduction * maxfl;
            force[1] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 2;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 0);
                }
                if (maxfr > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 1);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1f) {
                    SkidNoise(pCar, 0, pV, pCar->material_index[0]);
                }
            } else {
                if (maxfr > 0.1f) {
                    SkidNoise(pCar, 1, pV, pCar->material_index[1]);
                }
            }
            break;
        }
        v135 = sqrt(v108 * v108 + v98 * v98) / 2.0;
        if (v135 > 0.0001f) {
            v108 = v108 / (v135 * 2.0);
            v98 = v98 / (v135 * 2.0);
        }
        maxfl = sqrt(force[2]) * pCar->mu.v[1] * (fl_oil_factor * v116) * mat_list[pCar->material_index[2]].tyre_road_friction;
        maxfr = sqrt(force[3]) * pCar->mu.v[1] * (fr_oil_factor * v116) * mat_list[pCar->material_index[3]].tyre_road_friction;
        if (pCar != NULL && pCar->driver == eDriver_local_human) {
            SetJoystickFFBForce((int)(225.0f * (maxfl * force[2] + maxfr * force[3]) / (1.0f + 750.0f * pCar->speed)));
        }
        pCar->max_force_front = maxfr + maxfl;
        force[2] = v135;
        force[3] = v135;
        switch ((v135 > maxfl) + 2 * (v135 > maxfr)) {
        case 1:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = v135 - force[2] + force[3];
            if (force[3] > maxfr) {
                if (maxfr > 0.1f) {
                    pV = (force[3] - maxfr) / maxfr;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 3);
                    }
                    SkidNoise(pCar, 3, pV, pCar->material_index[3]);
                }
                force[3] = pCar->friction_slipping_reduction * maxfr;
                pCar->wheel_slip |= 1;
            }
            break;
        case 2:
            force[3] = pCar->friction_slipping_reduction * maxfr;
            force[2] = v135 - force[3] + force[2];
            if (force[2] > maxfl) {
                if (maxfl > 0.1f) {
                    pV = (force[2] - maxfl) / maxfl;
                    if (&gProgram_state.current_car == pCar) {
                        ts2 = 20.f;
                    } else {
                        ts2 = 60.f;
                    }
                    if (ts2 <= pV) {
                        ConditionallyNoteSkid(pCar, faces, 2);
                    }
                    SkidNoise(pCar, 2, pV, pCar->material_index[2]);
                }
                force[2] = pCar->friction_slipping_reduction * maxfl;
                pCar->wheel_slip |= 1;
            }
            break;
        case 3:
            force[2] = pCar->friction_slipping_reduction * maxfl;
            force[3] = pCar->friction_slipping_reduction * maxfr;
            pCar->wheel_slip |= 1;
            pV = (v135 * 2.0 - maxfl - maxfr) / (maxfr + maxfl);
            if (&gProgram_state.current_car == pCar) {
                ts2 = 20.f;
            } else {
                ts2 = 60.f;
            }
            if (ts2 <= pV) {
                if (maxfl > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 2);
                }
                if (maxfr > 0.1f) {
                    ConditionallyNoteSkid(pCar, faces, 3);
                }
            }
            if (IRandomBetween(0, 1)) {
                if (maxfl > 0.1f) {
                    SkidNoise(pCar, 2, pV, pCar->material_index[2]);
                }
            } else {
                if (maxfr > 0.1f) {
                    SkidNoise(pCar, 3, pV, pCar->material_index[3]);
                }
            }
            break;
        }
        BrVector3Scale(&v136, &rightplane, v99);
        BrVector3Scale(&a, &v103, v109);
        BrVector3Accumulate(&v136, &a);
        BrVector3Scale(&v123, &rightplane, v98);
        BrVector3Scale(&a, &v103, v108);
        BrVector3Accumulate(&v123, &a);
        for (i = 0; i < 4; i++) {
            rightplane = pCar->wpos[i];
            rightplane.v[1] = rightplane.v[1] - pCar->oldd[i];
            BrVector3Sub(&rightplane, &rightplane, &pCar->centre_of_mass_world_scale);
            if (i < 2) {
                BrVector3Scale(&b, &v136, force[i]);
            } else {
                BrVector3Scale(&b, &v123, force[i]);
            }
            BrVector3Accumulate(&B, &b);
            BrVector3Cross(&a, &rightplane, &b);
            BrVector3Accumulate(&f, &a);
        }
    } else {
        pCar->max_force_front = 0.f;
        pCar->max_force_rear = 0.f;
        if (gLast_car_to_skid[0] == pCar) {
            DRS3StopSound(gSkid_tag[0]);
        }
        if (gLast_car_to_skid[1] == pCar) {
            DRS3StopSound(gSkid_tag[1]);
        }
    }
    pCar->number_of_wheels_on_ground = normnum;
    BrMatrix34ApplyV(&b, &B, mat);
    BrVector3Scale(&rightplane, &f, pDt);
    PhysicsObjectApplyTorque(pCar->collision_info, &rightplane);
    BrVector3Scale(&rightplane, &b, pDt / (pCar->collision_info->M * WORLD_SCALE));
    BrVector3Accumulate(&pCar->collision_info->v, &rightplane);
    if (pCar->speed < 0.0001f
        && ((!pCar->keys.acc && pCar->joystick.acc <= 0) || !pCar->gear)
        && !pCar->keys.dec
        && pCar->joystick.dec <= 0
        && pCar->bounce_rate == 0.f
        && BrVector3Length(&pCar->collision_info->omega) < 0.05f) {
        if (vol != NULL) {
            ts2 = pCar->driver > 5 && pCar->underwater_ability != 0
                ? 1.0 - (1.0 - vol->gravity_multiplier) * 0.6
                : vol->gravity_multiplier;
            friction_number = BrVector3Length(&b) / ts2 / gGravity_multiplier;
        } else {
            friction_number = BrVector3Length(&b);
        }
        if (pCar->collision_info->M > friction_number || (pCar->keys.brake && normnum >= 3)) {
            if (stop_timer == 100.f) {
                stop_timer = 0.f;
            }
            if (stop_timer > 0.5f) {
                BrVector3SetFloat(&pCar->collision_info->v, 0.f, 0.f, 0.f);
                BrVector3SetFloat(&pCar->collision_info->omega, 0.f, 0.f, 0.f);
                stop_timer = 0.5f;
            }
        }
    }
    stop_timer = pDt + stop_timer;
    if (stop_timer > 1.f) {
        stop_timer = 100.f;
    }
    AddDrag(pCar, (tPhysics_object*)pCar->collision_info, pDt);
    if (pCar->driver >= eDriver_net_human) {
        pCar->acc_force = -(v136.v[2] * force[0]) - v136.v[2] * force[1];
    }
    pCar->curvature = pCar->curvature - pCar->field_0x1260;
}

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

// FUNCTION: CARMA2_HW 0x0041deb0
void C2_HOOK_FASTCALL CrushAndDamageCar(tCar_spec* pCar1, br_vector3* pPoint, br_vector3* pForce, tCar_spec* pCar2, int pChain_count, br_vector3* pChain, br_actor* pActor) {
    extern int gIt_or_fox;
    extern void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch);
    extern void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick);
    extern void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity);
    extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
    extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);
    extern void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce);
    extern void C2_HOOK_FASTCALL PipeSingleLocalCarIncident(br_vector3* pPosition, float pForce);
    br_vector3 vForce;
    br_vector3 vOther;
    br_vector3 vWorld;
    br_matrix34 mat;
    float power;
    int driver2;

    if (pCar2 != NULL) {
        if (pCar2->field_0xc8 == NULL || pCar2->driver > 5) {
            pCar2->field_0xc8 = (undefined*)pCar1;
        }
        if (pCar1->field_0xc8 != NULL) {
            if (pCar1 == NULL) {
                return;
            }
            if (pCar1->driver > 5) {
                pCar1->field_0xc8 = (undefined*)pCar2;
            }
        } else {
            pCar1->field_0xc8 = (undefined*)pCar2;
        }
    }
    if (pCar1 == NULL) {
        return;
    }
    if (pCar1->driver <= 5) {
        return;
    }
    if (gNet_mode != eNet_mode_none) {
        power = gCar_softness.initial_network[gCurrent_net_game->type];
    } else {
        power = gCar_softness.initial[gProgram_state.skill_level];
    }
    if (pCar1->driver == eDriver_oppo) {
        vOther.v[0] = pCar1->pos.v[0] - gCamera_to_world.m[3][0];
        vOther.v[1] = pCar1->pos.v[1] - gCamera_to_world.m[3][1];
        vOther.v[2] = pCar1->pos.v[2] - gCamera_to_world.m[3][2];
        if (200.f < vOther.v[0] * vOther.v[0] + vOther.v[1] * vOther.v[1] + vOther.v[2] * vOther.v[2]) {
            return;
        }
    }
    if (pCar2 != NULL) {
        if (pCar2->driver > 5) {
            TwoCarsHitEachOther(pCar1, pCar2);
        }
        if (pCar1->driver >= eDriver_net_human) {
            power = gArmour_starting_value[pCar1->power_up_levels[0]] * 1.2 * power;
        }
        driver2 = pCar2->driver;
        if (driver2 >= eDriver_net_human) {
            power = power * (gNet_mode != eNet_mode_none
                    ? gCar_car_damage_multiplier.initial_network[gCurrent_net_game->type]
                    : gCar_car_damage_multiplier.initial[gProgram_state.skill_level])
                * gOffensive_starting_value[pCar2->power_up_levels[2]]
                * pCar2->damage_multiplier * 0.9;
        }
        if (pCar1->driver == eDriver_oppo && driver2 == eDriver_oppo) {
            power = power * 0.2f;
        }
        if (driver2 > 5) {
            power = power / ((pCar2->softness_factor - -0.7) * 1.4285715);
        } else {
            pCar2 = NULL;
        }
    } else {
        if (pCar1->driver == eDriver_oppo) {
            power = pCar1->softness_factor * power;
            if ((int)(PDGetTotalTime() - pCar1->time_last_hit) > 5000) {
                power = power * 0.2f;
            }
        }
    }
    power = power * 0.2f;
    vForce.v[0] = pForce->v[0] * power;
    vForce.v[1] = pForce->v[1] * power;
    vForce.v[2] = pForce->v[2] * power;
    power = vForce.v[0] * vForce.v[0] + vForce.v[1] * vForce.v[1] + vForce.v[2] * vForce.v[2];
    if (pCar1->driver <= 5 || pCar1->invulnerable_no_damage == 0) {
        pCar1->damage_magnitude_accumulator = power + pCar1->damage_magnitude_accumulator;
    }
    if (pCar1->driver >= eDriver_net_human) {
        if (pCar1->field_0x4c8 != 1.0) {
            vForce.v[0] = vForce.v[0] / pCar1->field_0x4c8;
            vForce.v[1] = vForce.v[1] / pCar1->field_0x4c8;
            vForce.v[2] = vForce.v[2] / pCar1->field_0x4c8;
        }
        vOther.v[0] = vForce.v[0] * 0.5f;
        vOther.v[1] = vForce.v[1] * 0.5f;
        vOther.v[2] = vForce.v[2] * 0.5f;
        if (pCar1->driver == eDriver_local_human) {
            DoPratcamHit(&vForce);
        }
    } else {
        vOther.v[0] = vForce.v[0] * 1.5f;
        vOther.v[1] = vForce.v[1] * 1.5f;
        vOther.v[2] = vForce.v[2] * 1.5f;
    }
    if (gNet_mode == eNet_mode_host && gCurrent_net_game->type == eNet_game_type_foxy && pCar2 != NULL
            && pCar1->driver >= eDriver_net_human && pCar2->driver >= eDriver_net_human) {
        if (pCar1 == gNet_players[gIt_or_fox].car && pCar2->knackered == 0) {
            CarInContactWithFox(NetPlayerFromCar(pCar2));
        } else if (pCar2 == gNet_players[gIt_or_fox].car && pCar1->knackered == 0) {
            CarInContactWithFox(NetPlayerFromCar(pCar1));
        }
    }
    if (gNet_mode != eNet_mode_client || pCar2 == NULL) {
        FUN_0043fb70(pCar1, pPoint, &vForce, pCar2 != NULL);
    }
    FUN_0042d400(pCar1, pChain_count, pChain, &vOther, gPHIL_last_physics_tick);
    if (pCar2 != NULL && pCar2->driver == eDriver_local_human && power > 0.003) {
        PipeSingleCarIncident(pCar1, pPoint, power);
    }
    if (pCar2 == NULL && pCar1->driver == eDriver_local_human && power > 0.003) {
        BrMatrix34Copy(&mat, &pCar1->car_master_actor->t.t.mat);
        mat.m[3][0] = mat.m[3][0] * 0.14492753f;
        mat.m[3][1] = mat.m[3][1] * 0.14492753f;
        mat.m[3][2] = mat.m[3][2] * 0.14492753f;
        BrMatrix34ApplyP(&vWorld, pPoint, &mat);
        PipeSingleLocalCarIncident(&vWorld, power);
    }
}

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

// FUNCTION: CARMA2_HW 0x004207a0
int C2_HOOK_FASTCALL PipeNonCarObject(tPhysics_object* pCollision_info, void* pUser_data) {

    if (!pCollision_info->disable_move_rotate && pCollision_info->actor != NULL && pCollision_info->actor->type_data != NULL) {
        AddNonCarToPipingSession((tU16)((tNon_car_spec*)pCollision_info->actor->type_data)->car_ID, pCollision_info->actor);
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

// STUB: CARMA2_HW 0x0043ceb0
void C2_HOOK_FASTCALL UpdateCrushPainList(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004203c0
void C2_HOOK_FASTCALL CheckForDeAttachmentOfNonCars(tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00416500
void C2_HOOK_FASTCALL FinishCars(tU32 pLast_frame_time, tU32 pTime) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00497900
void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004dcfb0
void C2_HOOK_FASTCALL SendMines(tU32 pNext_frame_time) {
    NOT_IMPLEMENTED();
}


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


// FUNCTION: CARMA2_HW 0x00417fb0
void C2_HOOK_FASTCALL ApplyNonCarTumble(tPhysics_object* pObject, br_vector3* pV, br_scalar pTumbleFactor, br_scalar pTumbleThreshold) {
    extern void C2_HOOK_FASTCALL DRVector3SafeCross(br_vector3* pDest, const br_vector3* pA, const br_vector3* pB);
    br_vector3 cross;
    br_vector3 out;
    br_vector3 size;
    br_scalar len2;
    br_scalar speed;
    br_scalar over;
    br_scalar scale;
    br_scalar k;
    br_scalar nx;
    br_scalar ny;
    br_scalar nz;
    br_scalar new_len2;
    int i;

    if (pObject->collision_flag == 0 && pObject->disable_move_rotate == 0) {
        return;
    }
    len2 = pV->v[0] * pV->v[0] + pV->v[1] * pV->v[1] + pV->v[2] * pV->v[2];
    if (len2 < pTumbleThreshold * pTumbleThreshold) {
        return;
    }
    DRVector3SafeCross(&cross, &gFace_list__car[pObject->box_face_start].normal, pV);
    scale = (float)sqrt(cross.v[0] * cross.v[0] + cross.v[1] * cross.v[1] + cross.v[2] * cross.v[2]);
    if (scale > BR_SCALAR_EPSILON * 2) {
        scale = (float)(1.0 / scale);
        cross.v[0] = cross.v[0] * scale;
        cross.v[1] = cross.v[1] * scale;
        cross.v[2] = cross.v[2] * scale;
    } else {
        cross.v[0] = 1.f;
        cross.v[1] = 0.f;
        cross.v[2] = 0.f;
    }
    BrMatrix34TApplyV(&out, &cross, &pObject->actor->t.t.mat);
    if (out.v[0] * pObject->omega.v[0] + out.v[2] * pObject->omega.v[2] + out.v[1] * pObject->omega.v[1] > 1.5f) {
        return;
    }
    speed = (float)sqrt(len2);
    size.v[0] = pObject->bb1.max.v[0] - pObject->bb1.min.v[0];
    size.v[1] = pObject->bb1.max.v[1] - pObject->bb1.min.v[1];
    size.v[2] = pObject->bb1.max.v[2] - pObject->bb1.min.v[2];
    over = speed - pTumbleThreshold;
    for (i = 0; i < 3; i++) {
        out.v[i] = out.v[i] * (0.5f * size.v[i]) / pObject->I.v[i];
    }
    k = 0.1f * pTumbleFactor * over * pObject->M;
    nx = out.v[0] * k + pObject->omega.v[0];
    ny = out.v[1] * k + pObject->omega.v[1];
    nz = out.v[2] * k + pObject->omega.v[2];
    new_len2 = nx * nx + ny * ny + nz * nz;
    pObject->omega.v[0] = nx;
    pObject->omega.v[1] = ny;
    pObject->omega.v[2] = nz;
    if (new_len2 > 400.f) {
        scale = (float)sqrt(400.f / new_len2);
        pObject->omega.v[0] = nx * scale;
        pObject->omega.v[1] = ny * scale;
        pObject->omega.v[2] = nz * scale;
    }
}


// FUNCTION: CARMA2_HW 0x00418250
void C2_HOOK_FASTCALL MoveNonCar(tNon_car_spec* pNon_car, br_scalar pDt) {
    tSpecial_volume* last_special_volume;
    tJoint_translation_params* translation_parameters;
    br_vector3 drag_vector;
    br_vector3 projected_axis;
    br_scalar drag;
    br_scalar k;
    br_scalar speed;
    br_scalar along;
    br_scalar acceleration;
    br_scalar scale;
    br_scalar gravity;

    last_special_volume = pNon_car->collision_info->last_special_volume;
    pNon_car->flags |= 0x100000;
    if (gCar_flying && gCar_to_view == (tCar_spec*)pNon_car) {
        return;
    }
    if (pNon_car->actor->identifier[3] != '!') {
        pNon_car->actor->identifier[3] = '&';
        if (pNon_car->collision_info->physics_joint2 == NULL) {
            pNon_car->collision_info->actor->identifier[3] = '!';
            pNon_car->collision_info->omega.v[0] = 0.f;
            pNon_car->collision_info->omega.v[1] = 0.f;
            pNon_car->collision_info->omega.v[2] = 0.f;
            pNon_car->collision_info->v.v[0] = 0.f;
            pNon_car->collision_info->v.v[1] = 0.f;
            pNon_car->collision_info->v.v[2] = 0.f;
            pNon_car->collision_info->M = pNon_car->free_mass;
            pNon_car->collision_info->field_0xed = 1;
            pNon_car->collision_info->I.v[0] = pNon_car->I_over_M.v[0] * pNon_car->collision_info->M;
            pNon_car->collision_info->I.v[1] = pNon_car->I_over_M.v[1] * pNon_car->collision_info->M;
            pNon_car->collision_info->I.v[2] = pNon_car->I_over_M.v[2] * pNon_car->collision_info->M;
            if (pNon_car->collision_info->field183_0x1d8 != 0
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->owner != NULL
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->flags_0x238 == 1
                    && ((tPhysics_object*)pNon_car->collision_info->field183_0x1d8)->owner != (void*)pNon_car) {
                NonCarSnapSmashCallback(pNon_car->collision_info, &((tCar_spec*)pNon_car)->pos, &pNon_car->collision_info->v, &gVector3_00661678);
            }
        } else {
            BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
            BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
            BrVector3SetFloat(&pNon_car->collision_info->rotate_omega, 0.f, 0.f, 0.f);
            pNon_car->collision_info->disable_move_rotate = 1;
        }
    }
    if (pNon_car->actor->identifier[3] != '!') {
        return;
    }
    if (last_special_volume != NULL) {
        gravity = pDt * 10.f;
        gravity *= last_special_volume->gravity_multiplier;
        gravity *= 0.14492753623188406;
    } else {
        gravity = pDt * 1.4492753623188406;
    }
    pNon_car->collision_info->field_0x54.v[0] = 0.f;
    pNon_car->collision_info->field_0x54.v[1] = -gravity;
    pNon_car->collision_info->field_0x54.v[2] = 0.f;
    pNon_car->collision_info->v.v[1] -= gravity;
    speed = BrVector3Length(&pNon_car->collision_info->v) * 6.9;
    if (last_special_volume != NULL) {
        speed *= last_special_volume->viscosity_multiplier;
    }
    drag = pDt * 0.0005f;
    speed *= drag;
    k = speed / pNon_car->collision_info->M;
    drag_vector.v[0] = pNon_car->collision_info->v.v[0] * k;
    drag_vector.v[1] = pNon_car->collision_info->v.v[1] * k;
    drag_vector.v[2] = pNon_car->collision_info->v.v[2] * k;
    DRVector3Diminish(&pNon_car->collision_info->v, &drag_vector);
    speed = BrVector3Length(&pNon_car->collision_info->omega);
    if (last_special_volume != NULL) {
        speed *= last_special_volume->viscosity_multiplier;
    }
    speed *= drag;
    k = speed;
    drag_vector.v[0] = pNon_car->collision_info->omega.v[0] * k;
    drag_vector.v[1] = pNon_car->collision_info->omega.v[1] * k;
    drag_vector.v[2] = pNon_car->collision_info->omega.v[2] * k;
    ApplyWaterOmegaBrake(pNon_car->collision_info, &drag_vector);
    if (pNon_car->tumble_factor != 0.f) {
        ApplyNonCarTumble(pNon_car->collision_info, &pNon_car->collision_info->v, pNon_car->tumble_factor, pNon_car->tumble_threshold);
    }
    if ((pNon_car->flags & 0x10000) == 0) {
        return;
    }
    BrMatrix34ApplyV(&projected_axis, &pNon_car->field_0xf0->hinge_axis, &pNon_car->collision_info->transform_matrix);
    translation_parameters = pNon_car->translation_parameters;
    along = BrVector3Dot(&pNon_car->collision_info->velocity_car_space, &pNon_car->field_0xf0->hinge_axis);
    scale = translation_parameters->forward_resistance;
    acceleration = 0.f;
    speed = along - (BrVector3Dot((br_vector3*)pNon_car->collision_info->field_0x1b4, &projected_axis) + translation_parameters->forward_resistance);
    if ((pNon_car->flags & 0x20000) != 0
            && (pNon_car->collision_info->collision_flag & 2) != 0
            && (pNon_car->flags & 0xff) == 0
            && (pNon_car->flags & 0xff00) == 0
            && *(int*)pNon_car->field_0xf8 == 0
            && gNet_mode != eNet_mode_client) {
        pNon_car->flags = (pNon_car->flags & 1) | 1;
    }
    if ((pNon_car->flags & 0xff) != 0) {
        goto forward;
    }
    if (pNon_car->collision_info->disable_move_rotate == 0 && *(int*)pNon_car->field_0xf8 != 0) {
        goto forward;
    }
    if ((pNon_car->flags & 0xff00) != 0) {
        goto reverse;
    }
    if (pNon_car->collision_info->disable_move_rotate != 0 || *(int*)pNon_car->field_0xf8 != 0) {
        goto apply;
    }
reverse:
    acceleration = -translation_parameters->reverse_acceleration * 0.057971014492753624;
    scale = translation_parameters->reverse_resistance;
    if (speed <= 0.05) {
        goto apply;
    }
    if (along <= -0.005) {
        goto apply;
    }
    if ((pNon_car->flags >> 8) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 2) != 0) {
        pNon_car->flags = (pNon_car->flags & ~0xff00) | 0x100;
        goto apply;
    }
    pNon_car->flags = pNon_car->flags & ~0xff00;
    pNon_car->collision_info->disable_move_rotate = 1;
    BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
    acceleration = 0.f;
    goto apply;
forward:
    acceleration = translation_parameters->forward_acceleration * 0.057971014492753624;
    if (speed >= -0.05) {
        goto apply;
    }
    if (along >= 0.005) {
        goto apply;
    }
    if (*(int*)pNon_car->field_0xf8 == 0 || (pNon_car->flags & 0xff) == 1) {
        *(int*)pNon_car->field_0xf8 = GetRaceTime() + translation_parameters->pause_at_top;
    }
    if ((pNon_car->flags & 0xff) < pNon_car->number_of_pushes && (pNon_car->collision_info->collision_flag & 2) != 0) {
        if ((pNon_car->flags & 0xff) == 0) {
            goto apply;
        }
        pNon_car->flags = (pNon_car->flags & ~0xff) | ((pNon_car->flags + 1) & 0xff);
        goto apply;
    }
    pNon_car->flags = pNon_car->flags & ~0xff;
    pNon_car->collision_info->disable_move_rotate = 1;
    BrVector3SetFloat(&pNon_car->collision_info->v, 0.f, 0.f, 0.f);
    BrVector3SetFloat(&pNon_car->collision_info->omega, 0.f, 0.f, 0.f);
    acceleration = 0.f;
apply:
    pNon_car->collision_info->v.v[0] = pNon_car->field_0xf0->hinge_axis.v[0] * acceleration + pNon_car->collision_info->v.v[0];
    pNon_car->collision_info->v.v[1] = pNon_car->field_0xf0->hinge_axis.v[1] * acceleration + pNon_car->collision_info->v.v[1];
    pNon_car->collision_info->v.v[2] = pNon_car->field_0xf0->hinge_axis.v[2] * acceleration + pNon_car->collision_info->v.v[2];
    pNon_car->collision_info->v.v[0] = pNon_car->collision_info->v.v[0] * scale;
    pNon_car->collision_info->v.v[1] = pNon_car->collision_info->v.v[1] * scale;
    pNon_car->collision_info->v.v[2] = pNon_car->collision_info->v.v[2] * scale;
}

// STUB: CARMA2_HW 0x00425ba0
int C2_HOOK_FASTCALL BoundsOverlapTest__finteray(br_bounds* b1, br_bounds* b2) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x0041bd40
void C2_HOOK_FASTCALL ConditionallyNoteSkid(tCar_spec* pCar, tFace_ref** faces, int pIndex) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ecfb0
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004c2c30
void C2_HOOK_FASTCALL PhysicsObjectApplyTorque(tPhysics_object* pObject, const br_vector3* pTorque) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0041bed0
void C2_HOOK_FASTCALL SkidNoise(tCar_spec* pCar, int pSide, br_scalar pV, int pMaterial_index) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0042d400
void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pChain_count, br_vector3* pChain, br_vector3* pForce, tU32 pTick) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043fb70
void C2_HOOK_FASTCALL FUN_0043fb70(tCar_spec* pCar, br_vector3* pPoint, br_vector3* pForce, int pCars_touch) {
    NOT_IMPLEMENTED();
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


// FUNCTION: CARMA2_HW 0x0042dfd0
int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    int flags[40];
    int split_indices[40];
    br_vector4 split_plane;
    int count;
    int i;
    int j;
    int split_count;
    int can_split;
    float total_force;
    float split_y;
    float threshold;
    tPhysics_object* object;
    tCar_spec* car;
    extern int C2_HOOK_FASTCALL FUN_00430e30(tCollision_contact* pContacts, float* pImpulses, int pCount);
    extern int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane);
    extern void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag);
    extern float gMin_force_to_split_Y_per_tonne;

    if (FUN_00430e30(pContacts, pImpulses, pCount) != 0) {
        return 1;
    }
    count = pCount;
    if (count > 40) {
        count = 40;
    }
    for (i = 0; i < count; i++) {
        flags[i] = 0;
    }
    for (i = 0; i < count; i++) {
        if (pImpulses[i] >= 0.01f
                && flags[i] == 0
                && (object = pContacts[i].object1) != NULL
                && pContacts[i].object2 == NULL
                && object->owner != NULL
                && object->flags_0x238 == 1
                && (car = (tCar_spec*)object->owner) != NULL
                && car->driver > 5
                && car->car_crush_spec != NULL
                && car->car_crush_spec->field_0x4b8 == 0
                && car->invulnerable_no_crushage == 0) {
            can_split = 1;
            split_y = car->car_crush_spec->split_y_pos - car->collision_info->cmpos.v[1];
            if (pContacts[i].r1.v[1] < split_y) {
                can_split = 0;
            } else {
                split_indices[0] = i;
                split_count = 1;
                total_force = pImpulses[i];
            }
            flags[i] = 1;
            j = i + 1;
            while (j < count) {
                if (pImpulses[j] >= 0.01f
                        && flags[j] == 0
                        && pContacts[j].object2 == NULL
                        && (tCar_spec*)pContacts[j].object1->owner == car) {
                    if (can_split) {
                        if (pContacts[j].r1.v[1] < split_y) {
                            can_split = 0;
                        } else {
                            split_indices[split_count] = j;
                            split_count++;
                            total_force += pImpulses[j];
                        }
                    }
                    flags[j] = 1;
                }
                j++;
            }
            if (can_split) {
                threshold = (car->collision_info->M * gMin_force_to_split_Y_per_tonne)
                    / (car->softness_factor * car->car_crush_spec->snappability_factor);
                if (total_force > threshold
                        && SplitCheck(car, 1, NULL, car->car_crush_spec->split_y_pos, &split_plane) != 0) {
                    ScaleCollisionImpulses(pContacts, pImpulses, count, split_indices, split_count, threshold);
                    SplitCar(car, &split_plane, 1);
                    return 1;
                }
            }
        }
    }
    return 0;
}

// STUB: CARMA2_HW 0x004ba550
int C2_HOOK_FASTCALL ScaleCollisionImpulses(tCollision_contact* pContacts, float* pImpulses, int pCount, int* pSmash_indices, int pNum_impacts, float pMax_impulse) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x00430e30
int C2_HOOK_FASTCALL FUN_00430e30(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    NOT_IMPLEMENTED();
    return 0;
}


// FUNCTION: CARMA2_HW 0x00431310
int C2_HOOK_FASTCALL FullyDetachSnappedBits(tCollision_contact* pContacts, float* pImpulses, int pCount) {
    extern br_scalar gTorque_to_snap_per_tonne;
    extern void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);
    tPhysics_object* object1;
    tPhysics_object* parent;
    tCar_spec* car;
    tUser_crush_data* user;
    float snap_torque;
    int index;
    int i;

    for (i = 0; i < pCount; i++) {
        if (pContacts[i].type == 0xb
                && (object1 = pContacts[i].object1) != NULL
                && object1->owner != NULL
                && object1->flags_0x238 == 1
                && (parent = object1->parent) != NULL
                && parent->owner != NULL
                && parent->flags_0x238 == 1
                && (car = (tCar_spec*)parent->owner) != NULL
                && car->driver > 5
                && car->invulnerable_no_crushage == 0
                && (user = (tUser_crush_data*)object1->actor->user) != NULL
                && user->crush_data != NULL
                && user->crush_data->flap_data != NULL
                && user->crush_data->flap_data->kev_o_flap != 0) {
            snap_torque = object1->M * gTorque_to_snap_per_tonne;
            if (pImpulses[i] > snap_torque) {
                if (BrVector3Dot(&pContacts[i].normal, &object1->physics_joint1->hinge_axis) < gCrushZero) {
                    if (gNet_mode == eNet_mode_none) {
                        index = i;
                        ScaleCollisionImpulses(pContacts, pImpulses, pCount, &index, 1, snap_torque);
                        FullyDetachBit(car, object1);
                        MungeDetachLists(car->car_crush_spec);
                        return 1;
                    } else if (gNet_mode == eNet_mode_host) {
                        SendFullyDetachBit(car, object1);
                    }
                }
            }
        }
    }
    return 0;
}


// FUNCTION: CARMA2_HW 0x0043b740
void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep) {
    tCar_crush_spec* crush;
    tCar_crush_shape_info* shape_info;
    tCar_crush_reordered_shape_info* point;
    br_vector3* pCurrent;
    float* pTarget;
    float value;
    int shape_index;
    int point_index;
    int axis;

    crush = pCar->car_crush_spec;
    for (shape_index = 0; shape_index < crush->count_shapes; shape_index++) {
        shape_info = &crush->field_0x4[shape_index];
        for (point_index = 0; point_index < shape_info->count_points; point_index++) {
            point = &shape_info->field_0x18[point_index];
            pCurrent = &point->field_0x18;
            pTarget = point->field_0x0.v;
            for (axis = 0; axis < 3; axis++) {
                if (pCurrent->v[axis] > pTarget[axis]) {
                    value = pCurrent->v[axis] - pStep;
                    pCurrent->v[axis] = value;
                    if (value < pTarget[axis]) {
                        pCurrent->v[axis] = pTarget[axis];
                    }
                } else if (pCurrent->v[axis] < pTarget[axis]) {
                    value = pCurrent->v[axis];
                    pCurrent->v[axis] = value + pStep;
                    if (pTarget[axis] < value + pStep) {
                        pCurrent->v[axis] = pTarget[axis];
                    }
                }
            }
            if (DRVector3TestForNan(pCurrent)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar->car_crush_spec->expand_bounding_box = 3;
}


typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU32 field_0x8;
} tNet_message_chunk_crush_reattach_bit;

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
