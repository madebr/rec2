/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "13-crush1.h"

#include "01-network.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "60-phil.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

extern tPolyhedron_edge_indexes gBox_edge_indexes[12];

typedef struct {
    br_model* model;
    float crushability;
    br_bounds3 bounds;
} tDrone_crush_data;
extern tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject);

// GLOBAL: CARMA2_HW 0x006796bc
int gCount_queued_drone_crushes;

// GLOBAL: CARMA2_HW 0x0067bad8
tQueued_drone_crush gQueued_drone_crushes[4];

// GLOBAL: CARMA2_HW 0x0079ecbc
tPhysics_object* gUNK_0079ecbc;

int gNumber_of_net_players;

tNet_game_player_info gNet_players[12];

tNet_mode gNet_mode;

// GLOBAL: CARMA2_HW 0x0067bd58
int gCount_toggled_doors;

// GLOBAL: CARMA2_HW 0x0067b838
int gCount_net_crush_semi_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067bd60
int gCount_net_crush_full_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067bdf0
int gCount_net_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067b7cc
int gCount_car_damage_crush_list;

// GLOBAL: CARMA2_HW 0x00679450
tToggled_door gToggled_doors[16];

// GLOBAL: CARMA2_HW 0x0067bd18
tCar_damge_crush_list_item gCar_damage_crush_list[8];

// GLOBAL: CARMA2_HW 0x00679558
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067bad0
int gCount_crush_detach_list;

typedef struct tCrush_net_reattach_list_item_local {
    tCar_spec* actor;
    tCar_spec* car;
    undefined4 field_0x8;
} tCrush_net_reattach_list_item_local;

tCrush_net_reattach_list_item_local gNet_crush_reattach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b7d8
tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b840
tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067a290
tCrush_net_detach_list_item gNet_crush_detach_list[8];

// GLOBAL: CARMA2_HW 0x00679430
int gCollision_face_mask__raycast;

// GLOBAL: CARMA2_HW 0x00679438
int gSensible_place_collect_active;

tFace_ref gFace_list__car[300];


// GLOBAL: CARMA2_HW 0x00679698
// GLOBAL: CARMA2_SW 0x00673e08
br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b0
// GLOBAL: CARMA2_SW 0x00673e20
br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x00679550
// GLOBAL: CARMA2_SW 0x00673cc0
br_scalar gMax_detach_time_ms;

// GLOBAL: CARMA2_HW 0x006796b8
// GLOBAL: CARMA2_SW 0x00673e28
br_scalar gMin_crush_force;

// GLOBAL: CARMA2_HW 0x0067944c
// GLOBAL: CARMA2_SW 0x00673bbc
br_scalar gNormal_force_to_detach;

// GLOBAL: CARMA2_HW 0x0067a18c
// GLOBAL: CARMA2_SW 0x006748fc
br_scalar gMin_bend_angle;

// GLOBAL: CARMA2_HW 0x0067b7b0
// GLOBAL: CARMA2_SW 0x00675f20
int gMin_bend_damage;

// GLOBAL: CARMA2_HW 0x0067b7b4
// GLOBAL: CARMA2_SW 0x00675f24
br_scalar gFlap_inertia_fudge_biscuit;

// GLOBAL: CARMA2_HW 0x0067b7d0
// GLOBAL: CARMA2_SW 0x00675f40
int gMax_split_damage;

// GLOBAL: CARMA2_HW 0x0067bac8
// GLOBAL: CARMA2_SW 0x00676238
br_scalar gMin_force_to_split_XZ_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bacc
// GLOBAL: CARMA2_SW 0x0067623c
br_scalar gMax_crush_dist_sq;

// GLOBAL: CARMA2_HW 0x0067bd5c
// GLOBAL: CARMA2_SW 0x006764cc
br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
// GLOBAL: CARMA2_SW 0x006764d4
int gMin_split_damage;

// GLOBAL: CARMA2_HW 0x0067bde8
// GLOBAL: CARMA2_SW 0x00676558
br_scalar gMin_bend_force;

// GLOBAL: CARMA2_HW 0x0067bdec
// GLOBAL: CARMA2_SW 0x0067655c
br_scalar gMax_crush_force;

// GLOBAL: CARMA2_HW 0x0067bdf4
// GLOBAL: CARMA2_SW 0x00676564
int gMax_bend_damage;

// GLOBAL: CARMA2_HW 0x0067bdf8
// GLOBAL: CARMA2_SW 0x00676568
br_scalar gTorque_to_snap_per_tonne;

// GLOBAL: CARMA2_HW 0x0067be00
// GLOBAL: CARMA2_SW 0x00676570
br_scalar gChance_of_bending;

// GLOBAL: CARMA2_HW 0x0067be04
// GLOBAL: CARMA2_SW 0x00676574
br_scalar gChance_of_inverse_buckle;

// GLOBAL: CARMA2_HW 0x0067be78
// GLOBAL: CARMA2_SW 0x006765e8
br_vector3 gBatty_gravity;

// GLOBAL: CARMA2_HW 0x0067a190
// GLOBAL: CARMA2_SW 0x00674900
undefined4 gCrush_array_0067a190[10];

// GLOBAL: CARMA2_HW 0x00679440
// GLOBAL: CARMA2_SW 0x00673bb0
undefined4 gDAT_00679440;

// GLOBAL: CARMA2_HW 0x006796a0
// GLOBAL: CARMA2_SW 0x00673e10
tCrush_info_buffer gDetached_bit_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x00679448
// GLOBAL: CARMA2_SW 0x00673bb8
tPhysics_object* gDetached_bit_collision_infos;

// GLOBAL: CARMA2_HW 0x006796ac
// GLOBAL: CARMA2_SW 0x00673e1c
tDriver gDetached_bit_driver;

// GLOBAL: CARMA2_HW 0x0067bd68
// GLOBAL: CARMA2_SW 0x006764d8
tPhysics_joint* gTrack_crush_joints[32];

// GLOBAL: CARMA2_HW 0x0067b7b8
// GLOBAL: CARMA2_SW 0x00675f28
tCrush_info_buffer gSplit_car_crush_info_buffer;

// GLOBAL: CARMA2_HW 0x0067a188
// GLOBAL: CARMA2_SW 0x006748f8
tPhysics_object* gSplit_car_collision_infos;

// GLOBAL: CARMA2_HW 0x0067b7c4
// GLOBAL: CARMA2_SW 0x00675f34
tDriver gSplit_car_driver;

// GLOBAL: CARMA2_HW 0x006796b4
// GLOBAL: CARMA2_SW 0x00673e24
tPhysics_shape* gGonad_sphere_collision_shape;

// FUNCTION: CARMA2_HW 0x00429bb0
// FUNCTION: CARMA2_SW 0x00429c40
void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF) {
    char s[256];
    char *result;

    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("START OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
    }
    if (GetAnInt(pF) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    gDistortion_factor = GetAScalar(pF);
    gMin_crush_force = GetAScalar(pF);
    gMax_crush_force = GetAScalar(pF);
    gForce_to_movement_factor = GetAScalar(pF);
    gMax_crush_dist_sq = GetAScalar(pF);

    /* SPLITTING */
    gMin_force_to_split_XZ_per_tonne = GetAScalar(pF);
    gMin_force_to_split_Y_per_tonne = GetAScalar(pF);
    gMin_split_damage = GetAnInt(pF);
    gMax_split_damage = GetAnInt(pF);

    /* BUCKLING */
    gChance_of_inverse_buckle = GetAScalar(pF);

    /* FLAPPING AND JOINT SNAPPING */
    gFlap_inertia_fudge_biscuit = GetAScalar(pF);
    gBatty_gravity.v[1] = GetAScalar(pF);
    gTorque_to_snap_per_tonne = GetAScalar(pF);

    /* DETACHING */
    gMax_detach_time_ms = GetAScalar(pF);
    gNormal_force_to_detach = GetAScalar(pF);

    /* BENDING */
    gMin_bend_force = GetAScalar(pF);
    gChance_of_bending = GetAScalar(pF);
    gMin_bend_angle = GetAScalar(pF);
    gMin_bend_damage = GetAnInt(pF);
    gMax_bend_damage = GetAnInt(pF);
    while (1) {
        result = GetALineAndDontArgue(pF, s);
        if (result == NULL) {
            break;
        }
        if (strcmp("END OF CRUSH SETTINGS", s) == 0) {
            break;
        }
    }
    if (result == NULL) {
        PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
    }
}

// ClearCrushLists

// FUNCTION: CARMA2_HW 0x00429d90
void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec) {
    extern int gCount_net_crush_detach_list;
    extern int gCount_net_crush_semi_detach_bit_list;
    extern int gCount_net_crush_full_detach_bit_list;
    extern int gCount_net_crush_reattach_bit_list;
    extern int gCount_toggled_doors;
    extern int gCount_car_damage_crush_list;
    int i;
    int new_count;

    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_detach_list[new_count] = gNet_crush_detach_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_detach_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_semi_detach_bit_list; i++) {
        if (gNet_crush_semi_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_semi_detach_bit_list[new_count] = gNet_crush_semi_detach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_semi_detach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_full_detach_bit_list[new_count] = gNet_crush_full_detach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_full_detach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_reattach_bit_list[new_count] = gNet_crush_reattach_bit_list[i];
            }
            new_count += 1;
        }
    }
    gCount_net_crush_reattach_bit_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_crush_detach_list; i++) {
        if (gCrush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCrush_detach_list[new_count] = gCrush_detach_list[i];
            }
            new_count += 1;
        }
    }
    gCount_crush_detach_list = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_toggled_doors; i++) {
        if (gToggled_doors[i].car != pCar_spec) {
            if (new_count != i) {
                gToggled_doors[new_count] = gToggled_doors[i];
            }
            new_count += 1;
        }
    }
    gCount_toggled_doors = new_count;
    i = 0;
    new_count = 0;
    for (; i < gCount_car_damage_crush_list; i++) {
        if (gCar_damage_crush_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCar_damage_crush_list[new_count] = gCar_damage_crush_list[i];
            }
            new_count += 1;
        }
    }
    gCount_car_damage_crush_list = new_count;
}

// FUNCTION: CARMA2_HW 0x00429fa0
// FUNCTION: CARMA2_SW 0x0042a030
void C2_HOOK_FASTCALL InitCrushSystems(void) {
    int i;

    memset(gCrush_array_0067a190, 0, sizeof(gCrush_array_0067a190));
    gDAT_00679440 = 0;
    gDetached_bit_crush_info_buffer.capacity = 16;
    gDetached_bit_crush_info_buffer.crush_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gDetached_bit_collision_infos = BrMemAllocate(gDetached_bit_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gDetached_bit_crush_info_buffer.capacity; i++) {
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xdc = 1.f;
        gDetached_bit_crush_info_buffer.crush_infos[i].field_0xe0 = 0.28985506f;
        gDetached_bit_crush_info_buffer.crush_infos[i].collision_object = &gDetached_bit_collision_infos[i];
        gDetached_bit_collision_infos[i].shape = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
    }
    gDetached_bit_driver = eDriver_detached_bit;
    for (i = 0; i < (int)REC2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i] = AllocatePhysicsJoint(3, kMem_crush_data);
    }
    gSplit_car_crush_info_buffer.capacity = 3;
    gSplit_car_crush_info_buffer.crush_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tCrush_info), kMem_crush_data);
    gSplit_car_collision_infos = BrMemAllocate(gSplit_car_crush_info_buffer.capacity * sizeof(tPhysics_object), kMem_crush_data);
    for (i = 0; i < gSplit_car_crush_info_buffer.capacity; i++) {
        int j;
        tPhysics_shape* shape = NULL;

        gSplit_car_crush_info_buffer.crush_infos[i].field_0xdc = 0.f;
        gSplit_car_crush_info_buffer.crush_infos[i].collision_object = &gSplit_car_collision_infos[i];
        for (j = 0; j < 4; j++) {
            tPhysics_shape* polyhedron = (tPhysics_shape*)AllocateShapePolyhedron(16, kMem_crush_data);

            if (shape != NULL) {
                shape->common.next = polyhedron;
            } else {
                gSplit_car_collision_infos[i].shape = polyhedron;
            }
            shape = polyhedron;
        }
        if (shape != NULL) {
            shape->common.next = NULL;
        }
    }
    gSplit_car_driver = eDriver_split_car;
    gGonad_sphere_collision_shape = (tPhysics_shape*)AllocateShapeSphere(kMem_crush_data);
}

// ResetCrushSystems

// ReadCrushData

// FUNCTION: CARMA2_HW 0x0042a220
// FUNCTION: CARMA2_SW 0x0042a2b0
intptr_t C2_HOOK_CDECL CreateDefaultCrushDataCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_buffer_entry) != 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tCar_crush_buffer_entry, actor_name, 0x10);

    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    user_crush = pActor->user;
    if (user_crush == NULL || user_crush->crush_data != NULL || user_crush->groove != NULL) {
        return 0;
    }
    user_crush->crush_data = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
    strncpy(user_crush->crush_data->actor_name, pActor->identifier, sizeof(user_crush->crush_data->actor_name) - 1);
    user_crush->crush_data->id = 0xe9;
    user_crush->crush_data->softness_factor = 1.f;
    user_crush->crush_data->field_0x2c = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042a210
// FUNCTION: CARMA2_SW 0x0042a2a0
void C2_HOOK_FASTCALL CreateDefaultCrushData(br_actor* pActor, tCar_spec* pCar_spec) {
    DRActorEnumRecurse(pActor, CreateDefaultCrushDataCB, pCar_spec);
}

// ReadMasterCrushData

// LoadCrushDataFile

// LinkCrushDataToActor

// LinkCrushDataToActors

// FUNCTION: CARMA2_HW 0x0042a940
// FUNCTION: CARMA2_SW 0x0042a9d0
void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor* pActor, tCar_crush_buffer* pBuffer) {
    DRActorEnumRecurse(pActor, LinkCarCrushData, pBuffer);
}

// FUNCTION: CARMA2_HW 0x0042a950
// FUNCTION: CARMA2_SW 0x0042a9e0
intptr_t C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* pData) {
    int i;
    tUser_crush_data* user_crush;
    tCar_crush_buffer* car_crush_buffer;

    car_crush_buffer = pData;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    user_crush->crush_data = NULL;
    for (i = 0; i < car_crush_buffer->count_entries; i++) {
        if (car_crush_buffer->entries[i] != NULL && DRStricmp(car_crush_buffer->entries[i]->actor_name, pActor->identifier) == 0) {
            user_crush->crush_data = car_crush_buffer->entries[i];
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ac70
// FUNCTION: CARMA2_SW 0x0042ad00
intptr_t C2_HOOK_CDECL z(br_actor* pActor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = pActor->user;

    if (crush_data != NULL && crush_data->crush_data != NULL) {
        int i;

        for (i = 0; i < REC2_ASIZE(crush_data->models); i++) {
            if (crush_data->models[i] != NULL) {
                context->count_vertices += crush_data->models[i]->nvertices;
            }
        }
    }
    return 0;
}

// InitModelMasterCrushData

// FUNCTION: CARMA2_HW 0x0042a9b0
void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush) {
    int i;

    pCar_crush->field_0x24 = 0;
    pCar_crush->field_0x26 = 0;

    if (gNet_mode == eNet_mode_host) {
        pCar_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
        for (i = 1; i < gNumber_of_net_players; i++) {
            pCar_crush->network_stuff->items[i - 1].network_id = gNet_players[i].ID;
            pCar_crush->network_stuff->items[i - 1].field_0x4 = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042acb0
// FUNCTION: CARMA2_SW 0x0042ad40
int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2) {
    if (*(const float*)pValue1 < *(const float*)pValue2) {
        return -1;
    } else if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042ace0
// FUNCTION: CARMA2_SW 0x0042ad70
int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {
    if (*(const float*)pValue1 > *(const float*)pValue2) {
        return -1;
    } else if (*(const float*)pValue1 == *(const float*)pValue2) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042b4c0
// FUNCTION: CARMA2_SW 0x0042b550
tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid) {
    int number;
    int i;
    int mi;
    int ma;

    *pInvalid = 1;
    number = 0;
    for (i = REC2_ASIZE(pPoint->v) - 1; i >= 0; i--) {
        for (mi = 0; mi < pCount_limits->counts[i][1] - 1; mi++) {
            if (pPoint->v[i] < pLimits->limits[i][1].values[mi]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | mi;
        for (ma = 0; ma < pCount_limits->counts[i][0] - 1; ma++) {
            if (pLimits->limits[i][0].values[ma] < pPoint->v[i]) {
                *pInvalid = 0;
                break;
            }
        }
        number = (number << 2) | ma;
    }
    return (tU16)number;
}

// FUNCTION: CARMA2_HW 0x0042b590
intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tSoftnessOfNearestPointCB_Context* context;
    const br_vector3* point;
    br_vector3 diagonal;
    br_vector3 delta_point;
    float dist_bb_min;
    float dist_bb_max;
    float dist_bb_diagonal;
    float dist_p;
    tU16 i;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    model = user_crush_data->models[0];
    if (model == NULL) {
        return 0;
    }
    context = pData;
    point = context->point;
    BrVector3Sub(&delta_point, point, pPoint);
    BrVector3Sub(&diagonal, &model->bounds.max, &model->bounds.min);
    dist_bb_diagonal = BrVector3LengthSquared(&diagonal);
    dist_bb_min = Vector3DistanceSquared(&model->bounds.min, &delta_point);
    dist_bb_max = Vector3DistanceSquared(&model->bounds.max, &delta_point);
    dist_p = dist_bb_min > dist_bb_max ? dist_bb_min : dist_bb_max;
    if (dist_p - dist_bb_diagonal <= context->distance) {
        for (i = 0; i < model->nvertices; i++) {
            float d;

            d = Vector3DistanceSquared(&model->vertices[i].p, &delta_point);
            if (d < context->distance) {
                context->distance = d;
                context->softness_factor = crush_data->softness_factor;
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042b760
// FUNCTION: CARMA2_SW 0x0042b7f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* pCallback, void* pContext) {
    br_vector3 delta;
    intptr_t result;
    br_actor* a;

    if (pDelta != NULL) {
        BrVector3Add(&delta, &pActor->t.t.translate.t, pDelta);
    } else {
        BrVector3Set(&delta, 0.0f, 0.0f, 0.0f);
    }
    result = pCallback(pActor, &delta, pContext);
    if (result != 0) {
        return result;
    }
    for (a = pActor->children; a != NULL; a = a->next) {
        result = DRActorEnumRecurseWithTranslation(a, &delta, pCallback, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// SoftnessOfNearestPoint
float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    NOT_IMPLEMENTED();
    return 0;
}

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder
static __inline void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos) {
    const br_vertex* vertices;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 normal;
    br_vector3 center_offset;

    vertices = pModel->vertices;
    BrVector3Sub(&hinge01, &vertices[pHinge->hinge1].p, &vertices[pHinge->hinge0].p);
    BrVector3Sub(&hinge02, &vertices[pHinge->hinge2].p, &vertices[pHinge->hinge0].p);
    BrVector3Cross(&normal, &hinge02, &hinge01);
    Vector3Average(&center_offset, &pModel->bounds.min, &pModel->bounds.max);
    BrVector3Sub(&center_offset, &center_offset, pPos);
    if (BrVector3Dot(&center_offset, &normal) > 0.0f) {
        tU16 hinge1;

        hinge1 = pHinge->hinge1;
        pHinge->hinge1 = pHinge->hinge0;
        pHinge->hinge0 = hinge1;
    }
}

// FUNCTION: CARMA2_HW 0x0042bb00
tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel) {
    tU16 closest_vertex;
    br_scalar closest_dist;
    tU16 i;

    closest_vertex = 0;
    closest_dist = BR_SCALAR_MAX;
    for (i = 0; i < pModel->nvertices; i++) {
        br_scalar d;

        d = Vector3DistanceSquared(pPoint, &pModel->vertices[i].p);
        if (d >= closest_dist) {
            goto next_vertex;
        }
        closest_dist = d;
        closest_vertex = i;
next_vertex:;
    }
    return closest_vertex;
}

// DistanceToBounds

// FindNearestParentVertex
static __inline tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos) {
    br_actor* parent;
    tUser_crush_data* parent_user;
    br_model* parent_model;
    br_vector3 point;

    parent_model = NULL;
    parent = pActor->parent;
    if (parent != NULL) {
        parent_user = parent->user;
        if (parent_user != NULL) {
            parent_model = parent_user->models[0];
        }
    }
    if (parent_model == NULL) {
        PDFatalError("I can't cope flapping bit doesn't have a valid parent");
    }
    BrMatrix34ApplyP(&point, pPos, &pActor->t.t.mat);
    return FindNearestVertex(&point, parent_model);
}

// FUNCTION: CARMA2_HW 0x0042bb90
void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds) {
    float closest;
    const br_vector3* model_bounds[2];
    br_vector3 corner;
    br_vector3 closest_corner;
    tU8 closest_corner_index;
    tU8 closest_side;
    int i;

    model_bounds[0] = &pModel->bounds.min;
    model_bounds[1] = &pModel->bounds.max;
    if (pDetach_data->type == eDetachType_joint_index) {
        float dist_bb;

        closest_corner = pModel->vertices[pDetach_data->field_0x30].p;
        closest = BR_SCALAR_MAX;
        for (i = 0; i < 8; i++) {
            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_bb = Vector3DistanceSquared(&corner, &closest_corner);
            if (dist_bb < closest) {
                closest = dist_bb;
                closest_corner_index = i;
            }
        }
    } else {
        closest = BR_SCALAR_MAX;
        for (i = 0; i < 8; i++) {
            float dist_closest_side;
            int j;

            BrVector3Set(&corner,
                model_bounds[i & 1]->v[0],
                model_bounds[(i >> 1) & 1]->v[1],
                model_bounds[(i >> 2) & 1]->v[2]);
            dist_closest_side = BR_SCALAR_MAX;
            for (j = 0; j < 3; j++) {
                float d;

                d = (float)fabs(corner.v[j] - pBounds->min.v[j]);
                if (d < dist_closest_side) {
                    dist_closest_side = d;
                }
                d = (float)fabs(corner.v[j] - pBounds->max.v[j]);
                if (d < dist_closest_side) {
                    dist_closest_side = d;
                }
            }
            if (dist_closest_side < closest) {
                closest = dist_closest_side;
                closest_corner = corner;
                closest_corner_index = i;
            }
        }
        pDetach_data->field_0x30 = FindNearestVertex(&closest_corner, pModel);
    }
    pDetach_data->field_0x32 = closest_corner_index;
    Vector3Average(&corner, &pModel->bounds.min, &pModel->bounds.max);
    closest = BR_SCALAR_MAX;
    closest_side = 0;
    model_bounds[0] = &pBounds->min;
    model_bounds[1] = &pBounds->max;
    for (i = 0; i < 6; i++) {
        if (i != 2) {
            float d;

            d = (float)fabs(corner.v[i >> 1] - model_bounds[i & 1]->v[i >> 1]);
            if (d < closest) {
                closest = d;
                closest_side = i;
            }
        }
    }
    pDetach_data->field_0x33 = closest_side;
}

// InitPhysCrushDataCB

// InitPhysCrushData

// CalculateReferencePoints

// FUNCTION: CARMA2_HW 0x0042be20
// FUNCTION: CARMA2_SW 0x0042beb0
intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush_data;
    tCar_crush_detach_data* detach_data;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL && user_crush_data->models[0] != NULL
            && user_crush_data->crush_data != NULL && (detach_data = user_crush_data->crush_data->detach_data) != NULL) {
        tUser_crush_data* parent_user_crush_data;
        br_model* model;
        br_model* parent_model;
        tU16 i;
        float d_min, d_max;
        br_vector3 min_v, max_v;

        model = user_crush_data->models[0];
        parent_user_crush_data = pActor->parent->user;
        parent_model = parent_user_crush_data->models[0];
        d_max = d_min = BR_SCALAR_MAX;
        BrMatrix34ApplyP(&min_v, &model->bounds.min, &pActor->t.t.mat);
        BrMatrix34ApplyP(&max_v, &model->bounds.max, &pActor->t.t.mat);

        for (i = 0; i < parent_model->nvertices; i++) {
            float d;
            br_vector3 delta;

            BrVector3Sub(&delta, &min_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_min) {
                detach_data->min_vertex = i;
                d_min = d;
            }
            BrVector3Sub(&delta, &max_v, &parent_model->vertices[i].p);
            d = BrVector3LengthSquared(&delta);
            if (d < d_max) {
                detach_data->max_vertex = i;
                d_max = d;
            }
        }
    }
    return 0;
}

// InitModelCrushData

// PointEdgeDistSq

// SetFlapCheckVertices

// InitModelVertexData

// InitVertexDataCB

// InitVertexData

// CheckWheelPositions

// FUNCTION: CARMA2_HW 0x0042aa20
void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec) {
    int wheel_i;
    float* wpos_y;
    tPhysics_shape* shape;
    tCar_crush_spec* car_crush;
    int i;

    wpos_y = &pCar_spec->wpos[0].v[1];
    for (wheel_i = 4; wheel_i != 0; wheel_i--) {
        int wheel_not_found = 1;

        for (shape = pCar_spec->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron) {
                int plane_i;
                int inside = 1;
                float local[3];

                local[0] = wpos_y[-1] * (1.0 / 6.9);
                local[1] = wpos_y[0] * (1.0 / 6.9);
                local[2] = wpos_y[1] * (1.0 / 6.9);
                for (plane_i = 0; plane_i < shape->polyhedron.polyhedron.count_planes; plane_i++) {
                    const br_vector4* plane = &shape->polyhedron.polyhedron.planes[plane_i];

                    if (plane->v[2] * local[2] + plane->v[1] * local[1] + plane->v[0] * local[0] + plane->v[3] < 0.f) {
                        inside = 0;
                        break;
                    }
                }
                if (inside) {
                    wheel_not_found = 0;
                    break;
                }
            }
        }
        if (wheel_not_found) {
            PDFatalError("Wheels outside all shapes");
        }
        wpos_y += 3;
    }

    if (pCar_spec->car_crush_spec != NULL) {
        SwitchCarModels(pCar_spec, 0);

        car_crush = pCar_spec->car_crush_spec;
        if (car_crush != NULL) {
            tAccumulateSquashVertices_UserData data;

            data.count_vertices = 0;
            data.bens_z_min = car_crush->bend_z_min;
            data.bens_z_max = car_crush->bend_z_max;
            DRActorEnumRecurse(pCar_spec->car_model_actor, AccumulateSquashVertices, &data);
            car_crush->field_0x1cc = BrMemAllocate(data.count_vertices * sizeof(tCar_crush_vertex_data), kMem_crush_data);
            BrMatrix34Copy(&car_crush->model_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
        }

        DRActorEnumRecurse(pCar_spec->car_model_actor, InitModelCrushDataCB, pCar_spec);
        InitPhysMasterCrushData(pCar_spec);

        {
            tInitPhysCrushDataCB_Data data;

            data.car = pCar_spec;
            data.value = pCar_spec->collision_info->M /
                ((pCar_spec->collision_info->bb2.max.v[0] - pCar_spec->collision_info->bb2.min.v[0]) *
                 (pCar_spec->collision_info->bb2.max.v[1] - pCar_spec->collision_info->bb2.min.v[1]) *
                 (pCar_spec->collision_info->bb2.max.v[2] - pCar_spec->collision_info->bb2.min.v[2]));
            DRActorEnumRecurse(pCar_spec->car_model_actor, InitPhysCrushDataCB, &data);
        }

        if (pCar_spec->car_crush_spec != NULL) {
            car_crush = pCar_spec->car_crush_spec;
            for (i = 0; i < 3; i++) {
                qsort(car_crush->limits.limits[i][0].values, car_crush->count_limits.counts[i][0], sizeof(float), DecreasingCompare);
                qsort(car_crush->limits.limits[i][1].values, car_crush->count_limits.counts[i][1], sizeof(float), IncreasingCompare);
            }
            DRActorEnumRecurse(pCar_spec->car_model_actor, InitVertexDataCB, pCar_spec);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042c420
// FUNCTION: CARMA2_SW 0x0042c4b0
void C2_HOOK_FASTCALL DisposeMasterCrushData(tCar_crush_spec* pCar_crush, tCar_spec* pCar) {
    int i;

    if (pCar_crush != NULL) {
        BrActorFree(pCar_crush->actor);
        if (pCar_crush->field_0x1cc != NULL) {
            BrMemFree(pCar_crush->field_0x1cc);
        }
        for (i = 0; i < pCar_crush->count_shapes; i++) {
            BrMemFree(pCar_crush->field_0x4[i].field_0x18);
            BrMemFree(pCar_crush->field_0x4[i].field_0x0);
            BrMemFree(pCar_crush->field_0x4[i].field_0x4);
        }
        BrMemFree(pCar_crush->field_0x4);
        pCar->collision_info->shape = pCar_crush->field_0xc;
        if (pCar_crush->network_stuff != NULL) {
            BrMemFree(pCar_crush->network_stuff);
        }
        for (i = 0; i < (int)REC2_ASIZE(pCar_crush->field_0x564); i++) {
            BrMemFree(pCar_crush->field_0x564[i]);
        }
        BrMemFree(pCar_crush);
    }
}

// FUNCTION: CARMA2_HW 0x0042c4f0
// FUNCTION: CARMA2_SW 0x0042c580
void C2_HOOK_FASTCALL DisposeCrushData(tCar_crush_buffer_entry* pCrush_data) {

    if (pCrush_data != NULL) {
        if (pCrush_data->flap_data != NULL) {
            BrMemFree(pCrush_data->flap_data);
        }
        if (pCrush_data->detach_data != NULL) {
            BrMemFree(pCrush_data->detach_data);
        }
        DisposeSmashData(pCrush_data);
        BrMemFree(pCrush_data);
    }
}

// FUNCTION: CARMA2_HW 0x0042c530
// FUNCTION: CARMA2_SW 0x0042c5c0
void C2_HOOK_FASTCALL DisposeVertexData(tModel_detail_vertex_data* pVertex_data) {

    if (pVertex_data != NULL) {
        BrMemFree(pVertex_data);
    }
}

// BuckleTheBugger

// OutsideEdgeLine

// FUNCTION: CARMA2_HW 0x0042ca30
float C2_HOOK_FASTCALL PointTriangleDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    br_vector3 ab;
    br_vector3 bc;
    br_vector3 ca;
    br_vector3 projected;
    float nx;
    float ny;
    float nz;
    float length;
    float distance;

    BrVector3Sub(&ab, pB, pA);
    BrVector3Sub(&bc, pC, pB);
    nx = bc.v[2] * ab.v[1] - bc.v[1] * ab.v[2];
    BrVector3Sub(&ca, pA, pC);
    ny = bc.v[0] * ab.v[2] - bc.v[2] * ab.v[0];
    nz = bc.v[1] * ab.v[0] - bc.v[0] * ab.v[1];
    length = (br_scalar)sqrt(nz * nz + ny * ny + nx * nx);
    if (length > BR_SCALAR_EPSILON * 2) {
        length = (br_scalar)(1.0 / length);
        nx = nx * length;
        ny = ny * length;
        nz = nz * length;
    } else {
        nx = 1.0f;
        ny = 0.0f;
        nz = 0.0f;
    }
    distance = (pP->v[0] - pA->v[0]) * nx + (pP->v[2] - pA->v[2]) * nz + (pP->v[1] - pA->v[1]) * ny;
    projected.v[0] = pP->v[0] - distance * nx;
    projected.v[1] = pP->v[1] - distance * ny;
    projected.v[2] = pP->v[2] - distance * nz;
    if (OutsideEdgeLine(&projected, pA, &ab, pC)) {
        return PointEdgeDistSq(pP, pA, pB);
    }
    if (OutsideEdgeLine(&projected, pB, &bc, pA)) {
        return PointEdgeDistSq(pP, pB, pC);
    }
    if (OutsideEdgeLine(&projected, pA, &ca, pB)) {
        return PointEdgeDistSq(pP, pA, pC);
    }
    return distance * distance;
}

// FUNCTION: CARMA2_HW 0x0042ce00
float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC) {
    // based on ApexWorm/rec2 08141eedd
    br_vector3 u;
    br_vector3 v;
    br_vector3 offset;
    br_vector3 projected;
    br_vector3 residual;
    float u_scale;
    float v_scale;

    BrVector3Sub(&u, pC, pA);
    BrVector3Sub(&v, pB, pA);
    if ((float)fabs(BrVector3Dot(&u, &v)) > 0.001f) {
        BrVector3Sub(&u, pC, pB);
        BrVector3Sub(&v, pA, pB);
        pA = pB;
        if ((float)fabs(BrVector3Dot(&u, &v)) > 0.001f) {
            BrVector3Sub(&u, pB, pC);
            BrVector3Sub(&v, pA, pC);
            pA = pC;
        }
    }
    BrVector3Sub(&offset, pP, pA);
    v_scale = BrVector3Dot(&offset, &v) / BrVector3LengthSquared(&v);
    u_scale = BrVector3Dot(&offset, &u) / BrVector3LengthSquared(&u);
    if (!(v_scale >= 0.0f)) {
        v_scale = 0.0f;
    } else if (v_scale > 1.0f) {
        v_scale = 1.0f;
    }
    if (!(u_scale >= 0.0f)) {
        u_scale = 0.0f;
    } else if (u_scale > 1.0f) {
        u_scale = 1.0f;
    }
    projected.v[0] = u_scale * u.v[0] + v_scale * v.v[0] + pA->v[0];
    projected.v[1] = u_scale * u.v[1] + v_scale * v.v[1] + pA->v[1];
    projected.v[2] = u_scale * u.v[2] + v_scale * v.v[2] + pA->v[2];
    BrVector3Sub(&residual, &projected, pP);
    return BrVector3LengthSquared(&residual);
}

// PointSimplexDistSq

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0042c8b0
float C2_HOOK_FASTCALL DistSq(const br_vector3* pP, int pCount, const br_vector3* pVertices) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// LimitImpactScale

// FUNCTION: CARMA2_HW 0x00436850
// FUNCTION: CARMA2_SW 0x00436880
float C2_HOOK_FASTCALL GetImpactScale(const br_vector3* pP, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    float dist_sq;

    dist_sq = DistSq(pP, pCount, pVertices);
    if (dist_sq > gMax_crush_dist_sq) {
        return 0.0f;
    } else {
        return 1.0f - (float)sqrt(dist_sq / pMax_dist_sq);
    }
}

// FUNCTION: CARMA2_HW 0x0042d0e0
// FUNCTION: CARMA2_SW 0x0042d150
int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq) {
    br_vector3 diagonal;
    float threshold;

    BrVector3Sub(&diagonal, &pModel->bounds.max, &pModel->bounds.min);
    threshold = BrVector3LengthSquared(&diagonal) + pMax_dist_sq;
    if (DistSq(&pModel->bounds.min, pCount, pVertices) > threshold
        || DistSq(&pModel->bounds.max, pCount, pVertices) > threshold) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00436890
// FUNCTION: CARMA2_SW 0x004368c0
int C2_HOOK_FASTCALL NearestAxis(const br_vector3* pVector) {

    if ((float)fabs(pVector->v[0]) > 0.707107f) {
        return 0;
    } else if ((float)fabs(pVector->v[1]) > 0.707107f) {
        return 1;
    } else {
        return 2;
    }
}

// CrushModel

// PossibleShockSmash

// CrushModels

// CrushPoint

// FUNCTION: CARMA2_HW 0x004368d0
// FUNCTION: CARMA2_SW 0x00436900
void C2_HOOK_FASTCALL ClipPointToLimits(br_vector3* pPoint, tU16 pLimit_bits, const tCar_crush_limits* pLimits) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pPoint->v); i++) {
        if (pPoint->v[i] < pLimits->limits[i][0].values[pLimit_bits & 0x3]) {
            pPoint->v[i] = pLimits->limits[i][0].values[pLimit_bits & 0x3];
            pLimit_bits >>= 4;
        } else {
            pLimit_bits >>= 2;
            if (pPoint->v[i] > pLimits->limits[i][1].values[pLimit_bits & 0x3]) {
                pPoint->v[i] = pLimits->limits[i][1].values[pLimit_bits & 0x3];
            }
            pLimit_bits >>= 2;
        }
    }
}

// CrushCollisionShapes

// SimpleCrushCarModelsCB

// SimpleCrushCar

// CrushCar

// SendCrush

// FUNCTION: CARMA2_HW 0x00436940
// FUNCTION: CARMA2_SW 0x00436970
void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc) {
    int i;

    pDest->count = (tU8)pSrc->count;
    for (i = 0; i < pSrc->count; i++) {
        CompressVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
    }
    pDest->field_0x1a = DRScalarToU16(pSrc->field_0x40, 0.0f, 10.0f);
    CompressVector3(&pDest->field_0x1c, &pSrc->field_0x44, -1.0f, 1.0f);
    pDest->field_0x24 = pSrc->field_0x50;
}

// ExpandCrush

// RemoveDuplicatePoints

// FUNCTION: CARMA2_HW 0x0042c540
void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds) {
    float unique[12];
    float weights[1024];
    float* pNext;
    float* pOut;
    float* pVert;
    int remaining;
    int count;
    int axis;
    int found;
    int k;
    float len;
    float scale;
    float norm_x;
    float norm_y;
    float bend;
    float a;
    float b;
    float dist_sq;
    tU16 v;

    if (pModel == NULL) {
        return;
    }
    if (pModel->nvertices > 0x400) {
        PDFatalError("SimpleCrushModel can't cope with that many vertices");
    }
    if (pCount < 1) {
        return;
    }
    if (pCount > 4) {
        pCount = 4;
    }
    count = 1;
    unique[0] = pPoints[0];
    unique[1] = pPoints[1];
    unique[2] = pPoints[2];
    if (pCount > 1) {
        remaining = pCount - 1;
        pNext = pPoints + 3;
        pOut = unique + 4;
        do {
            found = 1;
            for (k = 0; k < count; k++) {
                float* pU = unique + k * 3;

                if ((float)fabs(pU[0] - pNext[0]) < 0.005f && (float)fabs(pU[1] - pNext[1]) < 0.005f
                    && (float)fabs(pU[2] - pNext[2]) < 0.005f) {
                    found = 0;
                    break;
                }
            }
            if (found) {
                pOut[-1] = pNext[0];
                pOut[0] = pNext[1];
                pOut[1] = pNext[2];
                pOut += 3;
                count++;
            }
            remaining--;
            pNext += 3;
        } while (remaining != 0);
    }
    if (TooFarToCrush(pModel, count, (br_vector3*)unique, gMax_crush_dist_sq) != 0) {
        return;
    }
    len = (float)sqrt(pDir[1] * pDir[1] + pDir[2] * pDir[2] + pDir[0] * pDir[0]);
    if (len > 2.3841858e-07f) {
        scale = 1.0 / len;
        norm_x = pDir[0] * scale;
        norm_y = pDir[1] * scale;
    } else {
        norm_x = 1.0f;
        norm_y = 0.0f;
    }
    if ((float)fabs(norm_x) <= 0.707107f) {
        axis = 1;
        if ((float)fabs(norm_y) <= 0.707107f) {
            axis = 2;
        }
    } else {
        axis = 0;
    }
    v = 0;
    if (pModel->nvertices > 0) {
        do {
            pVert = (float*)((tU8*)pModel->vertices + (tU32)v * 0x28);
            dist_sq = DistSq((br_vector3*)pVert, count, (br_vector3*)unique);
            if (dist_sq <= gMax_crush_dist_sq) {
                bend = 1.0f - (float)sqrt(dist_sq * 47.61f);
            } else {
                bend = 0.0f;
            }
            weights[v] = bend;
            if (bend != gCrushZero) {
                a = (pBounds[axis] - pVert[axis]) / pDir[axis];
                b = (pBounds[axis + 3] - pVert[axis]) / pDir[axis];
                if (b <= a) {
                    a = b;
                }
                if (a > gCrushZero) {
                    if (a < bend) {
                        weights[v] = a;
                    }
                } else {
                    weights[v] = 0.0f;
                }
                if (weights[v] != gCrushZero) {
                    pVert[0] = pVert[0] + weights[v] * pDir[0];
                    pVert[1] = pDir[1] * weights[v] + pVert[1];
                    pVert[2] = pDir[2] * weights[v] + pVert[2];
                }
            }
            v++;
        } while (v < pModel->nvertices);
    }
    FUN_004c7f40(pModel, (br_vector3*)pDir, weights);
    if ((pModel->flags & 0x20) != 0 && pModel->user != NULL) {
        *(int*)pModel->user = 1;
        return;
    }
    pModel->custom = JitModelUpdate;
    pModel->flags = pModel->flags | 0x20;
}

// CrushDrone

// FUNCTION: CARMA2_HW 0x0042d180
void C2_HOOK_FASTCALL CrushDroneObject(tPhysics_object* pObject, int pCount, float* pPoints, br_vector3* pScale) {
    tDrone_crush_data* crush_data;
    br_vector3 scaled;

    crush_data = GetDroneCrushData(pObject);
    if (crush_data != NULL && pCount >= 1) {
        scaled.v[0] = crush_data->crushability * pScale->v[0];
        scaled.v[1] = crush_data->crushability * pScale->v[1];
        scaled.v[2] = crush_data->crushability * pScale->v[2];
        SimpleCrushModel(crush_data->model, pCount, pPoints, scaled.v, (float*)&crush_data->bounds);
    }
}

// DoDroneCrushing

// FUNCTION: CARMA2_HW 0x0042d390
// FUNCTION: CARMA2_SW 0x0042d400
void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pCrush, const br_vector3* pVector, float pScale, int pCount, const br_vector3* pPoints) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, points, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tQueued_drone_crush_item, points, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tQueued_drone_crush_item, field_0x38, 0x38);

    if (pCount > (int)REC2_ASIZE(pCrush->points)) {
        pCount = REC2_ASIZE(pCrush->points);
    }
    BrVector3Copy(&pCrush->field_0x38, pVector);
    pCrush->field_0x34 = pScale;
    pCrush->count = pCount;
    for (i = 0; i < pCrush->count; i++) {
        BrVector3Copy(&pCrush->points[i], &pPoints[i]);
    }
}

// FUNCTION: CARMA2_HW 0x0042d1e0
void C2_HOOK_FASTCALL QueueDroneCrush(tPhysics_object* pObject, int pChain_count, br_vector3* pChain, br_vector3* pForce) {
    br_vector3 normal;
    float length;
    float dot;
    int i;
    int j;

    length = (float)sqrt(pForce->v[0] * pForce->v[0] + pForce->v[2] * pForce->v[2] + pForce->v[1] * pForce->v[1]);
    normal.v[0] = pForce->v[0] / length;
    normal.v[1] = pForce->v[1] / length;
    normal.v[2] = pForce->v[2] / length;
    for (i = 0; i < gCount_queued_drone_crushes; i++) {
        if (gQueued_drone_crushes[i].field_0x0 != (undefined4*)pObject) {
            continue;
        }
        for (j = 0; j < gQueued_drone_crushes[i].count; j++) {
            dot = gQueued_drone_crushes[i].items[j].field_0x38.v[0] * normal.v[0]
                + gQueued_drone_crushes[i].items[j].field_0x38.v[1] * normal.v[1]
                + gQueued_drone_crushes[i].items[j].field_0x38.v[2] * normal.v[2];
            if (dot > 0.707107f) {
                gQueued_drone_crushes[i].items[j].field_0x34 += length;
                return;
            }
        }
        if (gQueued_drone_crushes[i].count < 2) {
            AddDroneCrush(&gQueued_drone_crushes[i].items[gQueued_drone_crushes[i].count], &normal, length, pChain_count, pChain);
            gQueued_drone_crushes[i].count++;
        }
    }
    if (gCount_queued_drone_crushes < 4) {
        gQueued_drone_crushes[gCount_queued_drone_crushes].field_0x0 = (undefined4*)pObject;
        gQueued_drone_crushes[gCount_queued_drone_crushes].count = 0;
        AddDroneCrush(&gQueued_drone_crushes[gCount_queued_drone_crushes].items[0], &normal, length, pChain_count, pChain);
        gCount_queued_drone_crushes++;
    }
}

// DoCrushing

// FUNCTION: CARMA2_HW 0x0042d770
// FUNCTION: CARMA2_SW 0x0042d7e0
void C2_HOOK_FASTCALL SetCrush(tCar_crush* pCrush, int pCount_vectors, br_vector3* pVectors, br_vector3* pVector1, br_vector3* pVector2, float pScalar, tU32 pValue) {
    int i;

    pCrush->count = pCount_vectors;
    for (i = 0; i < pCount_vectors; i++) {
        BrVector3Copy(&pCrush->vectors[i], &pVectors[i]);
    }
    BrVector3Copy(&pCrush->field_0x34, pVector1);
    BrVector3Copy(&pCrush->field_0x44, pVector2);
    pCrush->field_0x40 = pScalar;
    pCrush->field_0x50 = pValue;
}

// QueueCarCrush

// SetBitForDetachment

// SetFlappage

// GetUniqueID

// InitializeCollisionInfo

// RemoveBit

// RemoveNonCarChildren

// RemoveNonCar

// GetNonCar

// GetNonCarObject

// SetRelationsDoingNothingFlags

// SetUpDetachedBitObject

// SetUpSemiDetachedBitJoint

// BitIsInBentPartOfCar

// SetUpFlapJoint

// FUNCTION: CARMA2_HW 0x0042d950
// FUNCTION: CARMA2_SW 0x0042d9c0
void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* material = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (material != NULL && !(material->flags & BR_MATF_TWO_SIDED)) {
            material->flags |= BR_MATF_TWO_SIDED;
            BrMaterialUpdate(material, BR_MATU_RENDERING);
        }
    }
}

// CheckFlapAngles

// FUNCTION: CARMA2_HW 0x0042d9b0
// FUNCTION: CARMA2_SW 0x0042da20
void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model* pModel) {
    int g;

    if (pModel == NULL) {
        return;
    }
    for (g = 0; g < pModel->prepared->ngroups; g++) {
        br_material* mat = pModel->faces[*pModel->prepared->groups[g].face_user].material;

        if (mat != NULL && (mat->flags & BR_MATF_TWO_SIDED) && mat->identifier != NULL && mat->identifier[0] == 'S') {
            mat->flags &= ~BR_MATF_TWO_SIDED;
            BrMaterialUpdate(mat, BR_MATU_RENDERING);
        }
    }
}

// ReAttachBit

// CentreModelOnPoint

// SemiDetachBit

// FUNCTION: CARMA2_HW 0x0042dbd0
// FUNCTION: CARMA2_SW 0x0042dc40
int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP3, br_bounds3* pBounds, br_actor* pActor) {
    tUser_crush_data* user_crush;
    br_model* model;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_flap_data* flap_data;

    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 1;
    }
    model = user_crush->models[0];
    if (model == NULL) {
        return 1;
    }
    crush_data = user_crush->crush_data;
    if (crush_data == NULL) {
        return 1;
    }
    detach_data = crush_data->detach_data;
    if (detach_data == NULL) {
        return 1;
    }
    DRBoundsCopy(pBounds, &model->bounds);
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        BrVector3Copy(pP1, &model->vertices[flap_data->hinge0].p);
        BrVector3Copy(pP2, &model->vertices[flap_data->hinge1].p);
        BrVector3Copy(pP3, &model->vertices[flap_data->hinge2].p);
    } else {
        BrVector3Copy(pP1, &model->vertices[detach_data->field_0x30].p);
        BrVector3Set(pP2, 0.f, 0.f, 0.f);
        BrVector3Set(pP3, 0.f, 0.f, 0.f);
    }
    return 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0042dab0
// STUB: CARMA2_SW 0x0042db20
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, tNet_game_player_info* pPlayer) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// SendDetachBit

// FUNCTION: CARMA2_HW 0x0042da20
// FUNCTION: CARMA2_SW 0x0042da90
void C2_HOOK_FASTCALL SendAllSemiDetachedBits(tNet_game_player_info* pPlayer) {
    int i;
    int j;
    tCar_spec* car;
    tCar_crush_spec* crush_spec;

    for (i = 0; i < gNum_active_cars; i++) {
        car = gActive_car_list[i];
        if (car != NULL && car->driver > eDriver_oppo - 1) {
            crush_spec = car->car_crush_spec;
            if (crush_spec != NULL) {
                for (j = 0; j < crush_spec->field_0x270; j++) {
                    SendSemiDetachBit(gActive_car_list[i], crush_spec->field_0x274[j].field_0x0, 0.f, pPlayer);
                }
            }
        }
    }
}

// AddNonCar

// DetachBit

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

// MungeDetachLists

// FUNCTION: CARMA2_HW 0x0043b250
// FUNCTION: CARMA2_SW 0x0043b2a0
void C2_HOOK_FASTCALL SendReAttachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_reattach_bit, player, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_reattach_bit, uid, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_reattach_bit, time, 0xc);
    C2_HOOK_BUG_ON(sizeof(tNet_message_chunk_reattach_bit) != 0x10);

    message = NetBuildGuaranteedMessage(eNet_message_chunk_type_reattach_bit, 0);
    message->contents.reattach_bit.player = NetPlayerFromCar(pCar)->ID;
    message->contents.reattach_bit.uid = pObject->uid;
    message->contents.reattach_bit.time = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, NULL);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004387d0
// STUB: CARMA2_SW 0x00438800
void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// DoFlapping

// ToggleFlapsCB2

// ToggleFlapsCB

// SendToggleFlaps

// ToggleFlaps

// FUNCTION: CARMA2_HW 0x00434910
// FUNCTION: CARMA2_SW 0x00434940
int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject) {
    br_vector3 world_direction;
    br_vector3 parent_direction;
    const br_vector3* limit_direction;
    br_scalar alignment;
    int i;

    for (i = 0; i < pObject->physics_joint1->count_limits; i++) {
        BrMatrix34ApplyV(&world_direction, &pObject->physics_joint1->limits[i].child, &pObject->actor->t.t.mat);
        BrMatrix34TApplyV(&parent_direction, &world_direction, &pObject->parent->actor->t.t.mat);
        limit_direction = &pObject->physics_joint1->limits[i].parent;
        alignment = BrVector3Dot(limit_direction, &parent_direction);
        if (alignment > 0.707107f) {
            return 1;
        }
    }
    return 0;
}

// FullyDetachBit

// SendFullyDetachBit

// DoFullyDetaching

// CalculateCollisionObjectStuffForSplitHalves

// SetUpCollisionInfoStuffForSplitHalf

// SetFace

// AddSplitVertexIndex

// CreateSplitVertex

// GetSplitVertex

// SplitModel

// CreateSplitPoints

// SplitCollisionShape

// SplitCarCollisionShapes

// QuantizeWithinBounds

// SaveVitalBitsOfObject

// FUNCTION: CARMA2_HW 0x00430730
// FUNCTION: CARMA2_SW 0x00430760
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsDoubleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// SwapCarShapes

// IsAWheelOrPivotActor

// FUNCTION: CARMA2_HW 0x0042deb0
// FUNCTION: CARMA2_SW 0x0042df20
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = pActor->user;
    tCar_spec* car_spec = pArg;
    int i;

    if (user_crush != NULL) {
        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsSingleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}

// IsOnNonCarHalf

// SplitCar

// QuantizePlane

// WithinBounds

// SplitCheck

// FindForceEdge

// DoXZSplitting

// DoYSplitting

// DoSplitting

// DoJointSnapping

// PointIsOutsideShape

// PointIsOutsideLimits

// ClipPointToPolyhedron

// QuantizeVector3

// MoveCarToSensiblePlace

// SwapShapesIfPossible

// DoShapeSwapping

// BendPoint

// EdgeCrossesZPlane

// BendCollisionShape

// BendCollisionShapes

// BendModel

// BendModels

// BrAngleToSignedDegree

// BendWheels

// BendActor

// BendOtherThings

// ReAttachOrFullyDetachDifficultBits

// BendCar

// QuantizeAngle

// BendCarOneForce

// DoBending

// FUNCTION: CARMA2_HW 0x00431590
// FUNCTION: CARMA2_SW 0x004315c0
void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    (void)pOn_screen;
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}

// FUNCTION: CARMA2_HW 0x004315e0
// FUNCTION: CARMA2_SW 0x00431610
void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;

    if ((pModel->flags & BR_MODF_CUSTOM) != 0 && pModel->user != NULL) {
        user_detail_model = pModel->user;
        user_detail_model->field_0x0 = 1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}

// RandomlyXZSplitCar

// RandomlyBendCar

// TotallySpamTheCar

// FUNCTION: CARMA2_HW 0x00435dd0
// FUNCTION: CARMA2_SW 0x00435e00
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseUltraPlus(br_actor* pActor, br_actor_enum_cbfn* pCallback, void* pArg) {
    intptr_t result;
    br_actor* actor;
    br_actor* next;

    actor = pActor;
    for (pActor = pActor->children; pActor != NULL; pActor = next) {
        next = pActor->next;
        result = DRActorEnumRecurseUltraPlus(pActor, pCallback, pArg);
        if (result != 0) {
            return result;
        }
    }
    return pCallback(actor, pArg);
}

// FireOffBitsCB

// FireSomeBitsOffCar

// DoSpams

// CrushBendFlapRend

// SphericizeModel

// SphericizePhysics

// SphericizeCar

// FUNCTION: CARMA2_HW 0x004398d0
// FUNCTION: CARMA2_SW 0x00439910
intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    tCar_spec* car_spec = pUser;

    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    if (crush_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data == NULL) {
        return 0;
    }
    if (crush_data->flap_data->field_0x0 && !crush_data->flap_data->kev_o_flap) {
        StopFlapping(pActor, car_spec);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004398c0
// FUNCTION: CARMA2_SW 0x00439900
int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, BattenDownTheHatches, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00439fd0
// FUNCTION: CARMA2_SW 0x0043a010
void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info* pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(&pShape->field_0x18[i].field_0x18, &pShape->field_0x18[i].field_0x2c);
    }
}

// CompletelyUnBendCollisionShapes

// CompletelyUnBendWheels

// CompletelyUnBendOtherThings

// FUNCTION: CARMA2_HW 0x00439910
// FUNCTION: CARMA2_SW 0x00439950
intptr_t C2_HOOK_CDECL EnableGroovers(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 0x3);
        EnableGroovidelic(pActor, 0x3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}

// DoCompletelyUnBentThings

// FUNCTION: CARMA2_HW 0x00439960
// FUNCTION: CARMA2_SW 0x004399a0
void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        tCar_crush_shape_info* shape_info = &pCar_spec->car_crush_spec->field_0x4[i];
        int j;

        for (j = 0; j < shape_info->count_points; j++) {
            BrVector3Copy(&shape_info->field_0x18[j].field_0x18, &shape_info->field_0x18[j].field_0x0);
            if (DRVector3TestForNan(&shape_info->field_0x18[j].field_0x18)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

// TotallyRepairModel

// STUB: CARMA2_HW 0x00439a20
// STUB: CARMA2_SW 0x00439a60
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00439a10
// FUNCTION: CARMA2_SW 0x00439a50
int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data) {
    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0043b2a0
// FUNCTION: CARMA2_SW 0x0043b2f0
void C2_HOOK_FASTCALL SendReAttachAllBits(tCar_spec* pCar) {
    tNet_message* message;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_reattach_all_bits, player, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_reattach_all_bits, time, 0x8);
    C2_HOOK_BUG_ON(sizeof(tNet_message_chunk_reattach_all_bits) != 0xc);

    message = NetBuildGuaranteedMessage(eNet_message_chunk_type_reattach_all_bits, 0);
    message->contents.reattach_all_bits.player = NetPlayerFromCar(pCar)->ID;
    message->contents.reattach_all_bits.time = gPHIL_last_physics_tick + 120;
    NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, NULL);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00439510
// STUB: CARMA2_SW 0x00439550
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00439bf0
// FUNCTION: CARMA2_SW 0x00439c30
void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!gArrow_mode) {
        TotallyRepairACar(&gProgram_state.current_car);
    }
}

// AddTotalDeflectionOfModel

// AddTotalDeflection

// RepairModel

// RepairModels

// UnBendModel

// UnBendModels

// UnBendActor

// UnBendCollisionShape

// UnBendCollisionShapes

// UnBendWheels

// UnBendOtherThings

// FUNCTION: CARMA2_HW 0x0043ac90
// FUNCTION: CARMA2_SW 0x0043ace0
br_scalar C2_HOOK_FASTCALL NormalizedAngle(br_angle pAngle) {
    if (pAngle < BR_ANGLE_DEG(180)) {
        return (br_scalar)(pAngle / (double)BR_ONE_LS);
    } else {
        return (br_scalar)((br_angle)-pAngle / (double)BR_ONE_LS);
    }
}

// CalculateAmountRepairedFraction

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00439c30
// STUB: CARMA2_SW 0x00439c70
br_scalar C2_HOOK_FASTCALL UnBendCar(tCar_spec* pCar_spec, tU32 pDelta_time, br_scalar* pOriginal_amount) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00439c10
// FUNCTION: CARMA2_SW 0x00439c50
void C2_HOOK_FASTCALL CompletelyUnBendCar(tCar_spec* pCar_spec) {
    br_scalar original_amount;

    UnBendCar(pCar_spec, 2000, &original_amount);
}

// ReAttachBits

// RepairObject

// RepairCarCollisionShapes

// RepairCar2

// RepairCar

// RestoreVitalBitsOfObject

// WeldCar

// ARCompressCrush

// ARApplyCrush

// ARUndoCrush

// FUNCTION: CARMA2_HW 0x0043bf30
// FUNCTION: CARMA2_SW 0x0043bf80
void C2_HOOK_FASTCALL ARCompressModelMash(tCompressed_vector3* pDest, br_model* pModel, br_vector3* pSrc) {
    tU16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pDest[i], &pSrc[i], -10.0f, 10.0f);
    }
}

// FUNCTION: CARMA2_HW 0x0043bf80
// FUNCTION: CARMA2_SW 0x0043bfd0
void C2_HOOK_FASTCALL ARApplyModelMash(br_model* pModel, tCompressed_vector3* pSrc) {
    tU16 i;
    br_vector3 delta;

    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&delta, &pSrc[i], -10.0f, 10.0f);
        BrVector3Accumulate(&pModel->vertices[i].p, &delta);
    }
    SetModelForUpdate(pModel);
}

// FUNCTION: CARMA2_HW 0x0043c030
// FUNCTION: CARMA2_SW 0x0043c080
void C2_HOOK_FASTCALL ARUndoModelMash(br_model* pModel, tCompressed_vector3* pSrc) {
    tU16 i;
    br_vector3 delta;

    for (i = 0; i < pModel->nvertices; i++) {
        ExpandVector3(&delta, &pSrc[i], -10.0f, 10.0f);
        BrVector3Sub(&pModel->vertices[i].p, &pModel->vertices[i].p, &delta);
    }
    SetModelForUpdate(pModel);
}

// ARApplyFlap

// ARApplySplitWeld

// ARUndoSplitWeld

// ReceivedCrush

// FindBitWithID

// AttachedBitFromID

// ReceivedSemiDetachBit

// ReceivedDetachBit

// SemiDetachedBitObjectFromID

// ReceivedFullyDetachBit

// DetachedBitFromID

// SemiDetachedBitFromID

// ReceivedReAttachBit

// ReceivedReAttachAllBits

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// ReceivedToggleFlaps

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// GetNetShapeDataLengthFromShape

// GetNetShapeDataLengthFromData

// FUNCTION: CARMA2_HW 0x0043d9c0
// FUNCTION: CARMA2_SW 0x0043da10
void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pData, tCar_spec* pCar) {
    int i;
    int j;
    int count;
    tCar_crush_shape_info* shape_info;
    tPhysics_shape* shape;
    br_vector3* point;

    C2_HOOK_BUG_ON(sizeof(tCompressed_vector3) != 6);

    if (pCar->car_crush_spec->field_0x4b8) {
        count = 0;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                count++;
            }
        }
        *pData++ = (tU8)count;
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                *pData = (tU8)shape->polyhedron.polyhedron.count_points;
                pData++;
                for (j = 0; j < shape->polyhedron.polyhedron.count_points; j++) {
                    point = shape->polyhedron.polyhedron.points;
                    point += j;
                    CompressVector3((tCompressed_vector3*)pData, point, -2.5f, 2.5f);
                    pData += sizeof(tCompressed_vector3);
                }
            }
        }
    } else {
        count = 0;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            count += pCar->car_crush_spec->field_0x4[i].count_points;
        }
        *pData++ = (tU8)count;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            shape_info = &pCar->car_crush_spec->field_0x4[i];
            for (j = 0; j < shape_info->count_points; j++) {
                CompressVector3((tCompressed_vector3*)pData, &shape_info->field_0x18[j].field_0xc, -2.5f, 2.5f);
                pData += sizeof(tCompressed_vector3);
            }
        }
    }
}

// DecodeNetShapeData

// CopyBendInMessage

// Vector4Equal

// CompressPlane

// ExpandPlane

// CopySplitInMessage

// FUNCTION: CARMA2_HW 0x0043dfd0
// FUNCTION: CARMA2_SW 0x0043e020
void C2_HOOK_FASTCALL SetAllShapeStatusFlags(tCar_crush_network_shapes* pShapes) {
    int i;

    if (pShapes == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(pShapes->items); i++) {
        if (pShapes->items[i].network_id != 0) {
            pShapes->items[i].field_0x4 = 1;
        }
    }
}


extern int C2_HOOK_FASTCALL ProcessForcesCallback(void* pContacts, float* pImpulses, int pCount);
int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3) {
    NOT_IMPLEMENTED();
    return 0;
}
extern void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2);
extern int C2_HOOK_FASTCALL FindFacesInBox(tBounds* pBounds, tFace_ref* pFace_list, int pMax_face, tWorld_callbacks* pWorld_callbacks);
extern tNon_car_spec* C2_HOOK_FASTCALL PullActorFromWorld(br_actor* actor);
extern void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor);
extern float C2_HOOK_FASTCALL GetFrictionFromFace(const tFace_ref* pFace);

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

// STUB: CARMA2_HW 0x0041c1b0
int C2_HOOK_FASTCALL ProcessForcesCallback(void* pContacts, float* pImpulses, int pCount) {
    NOT_IMPLEMENTED();
    return 0;
}

extern int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3);

// STUB: CARMA2_HW 0x0043b8a0
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0042e2e0
void C2_HOOK_FASTCALL SplitCar(tCar_spec* pCar, br_vector4* pPlane, int pFlag) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004307e0
int C2_HOOK_FASTCALL SplitCheck(tCar_spec* pCar, int pMode, const br_vector3* pLine, br_scalar pHeight, br_vector4* pPlane) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x0042f4e0
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0042f3d0
void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004314b0
void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0043deb0
void C2_HOOK_FASTCALL CompressPlane(tU16* pDest, const br_vector4* pSrc) {
    NOT_IMPLEMENTED();
}

tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(tU8 pType, int pSize_decider) {
    NOT_IMPLEMENTED();
    return NULL;
}


// FUNCTION: CARMA2_HW 0x004228b0
int C2_HOOK_FASTCALL FUN_004228b0(tPhysics_object* pObject, tSensible_place_contact* pContacts, int pArg) {
    struct {
        br_vector3 points[128];
        int point_flags[64];
        tU16 point_count;
        tU16 edge_count;
        tPolyhedron_edge_indexes* edges;
    } data;
    br_matrix34 object_to_actor_matrix;
    br_matrix34 inverse_actor_matrix;
    br_vector3 actor_point;
    br_vector3 world_point;
    int result_index;
    tFace_ref* pClosest_face;
    br_vector3 closest_point;
    br_vector3 local_point;
    br_scalar distance;
    br_vector3 normal;
    int has_collision;
    int has_any_collision;
    br_scalar closest_distance;
    int tmp_count;
    br_matrix34* pTransform;
    int collision_count;
    int* pFlags;
    int i;
    tPhysics_shape* pShape;
    br_matrix34* pShape_transform;
    br_scalar neg_radius;
    tFace_ref* pFace;
    int face_index;
    tU32 three;

    three = 3;
    collision_count = 0;
    has_any_collision = 0;
    pShape = pObject->shape;
    result_index = 1;
    if (pObject->box_face_start == pObject->box_face_end) {
        return 0;
    }
    if (pArg > 0x24) {
        return 0;
    }
    BrMatrix34LPInverse(&inverse_actor_matrix, &pObject->actor->t.t.mat);
    pTransform = &pObject->transform_matrix;
    BrMatrix34Mul(&object_to_actor_matrix, pTransform, &inverse_actor_matrix);
    for (; pShape != NULL; pShape = pShape->common.next) {
        if (!EliminateFaces(pObject, (int*)pShape, &object_to_actor_matrix)) {
            continue;
        }
        if (gSensible_place_collect_active != 0) {
            pShape_transform = pTransform;
        } else {
            pShape_transform = &pObject->actor->t.t.mat;
        }
        switch (pShape->common.type) {
        case kCollisionShapeType_Box:
            data.point_count = 8;
            data.edge_count = 12;
            for (i = 0, pFlags = data.point_flags; i < 8; i++, pFlags++) {
                if ((i & 2) != 0) {
                    local_point.v[0] = pShape->box.common.bb.min.v[0];
                } else {
                    local_point.v[0] = pShape->box.common.bb.max.v[0];
                }
                if ((i & 1) != 0) {
                    local_point.v[1] = pShape->box.common.bb.min.v[1];
                } else {
                    local_point.v[1] = pShape->box.common.bb.max.v[1];
                }
                if ((i & 4) != 0) {
                    local_point.v[2] = pShape->box.common.bb.min.v[2];
                } else {
                    local_point.v[2] = pShape->box.common.bb.max.v[2];
                }
                BrMatrix34ApplyP(&data.points[i], &local_point, pTransform);
                BrMatrix34ApplyP(&data.points[i + 64], &local_point, pShape_transform);
                *pFlags = 0;
            }
            data.edges = gBox_edge_indexes;
            break;
        case kCollisionShapeType_Wireframe:
            data.point_count = (tU16)pShape->wireframe.wireframe.count_points;
            data.edge_count = (tU16)pShape->wireframe.wireframe.count_lines;
            if (data.point_count != 0) {
                for (i = 0, pFlags = data.point_flags; i < (int)data.point_count; i++, pFlags++) {
                    BrMatrix34ApplyP(&data.points[i], &pShape->wireframe.wireframe.points[i], pTransform);
                    BrMatrix34ApplyP(&data.points[i + 64], &pShape->wireframe.wireframe.points[i], pShape_transform);
                    *pFlags = 0;
                }
            }
            data.edges = pShape->wireframe.wireframe.lines;
            break;
        case kCollisionShapeType_Polyhedron:
        case kCollisionShapeType_Wireframe_Polyhedron:
            data.point_count = (tU16)pShape->polyhedron.polyhedron.count_points;
            data.edge_count = (tU16)pShape->polyhedron.polyhedron.count_edges;
            if (data.point_count != 0) {
                for (i = 0, pFlags = data.point_flags; i < (int)data.point_count; i++, pFlags++) {
                    BrMatrix34ApplyP(&data.points[i], &pShape->polyhedron.polyhedron.points[i], pTransform);
                    BrMatrix34ApplyP(&data.points[i + 64], &pShape->polyhedron.polyhedron.points[i], pShape_transform);
                    *pFlags = 0;
                }
            }
            data.edges = pShape->polyhedron.polyhedron.edges;
            break;
        case kCollisionShapeType_Sphere:
            data.point_count = 1;
            data.edge_count = 0;
            BrMatrix34ApplyP(&data.points[0], &pShape->sphere.sphere.center, pTransform);
            BrMatrix34ApplyP(&data.points[64], &pShape->sphere.sphere.center, pShape_transform);
            break;
        default:
            PhysicsError(ePhysicsError_UnknownShapeType, "");
            break;
        }
        if (gSensible_place_collect_active != 0) {
            BrMatrix34ApplyP(&data.points[0], &pObject->cmpos, pTransform);
            for (i = 1; i < (int)data.point_count; i++) {
                data.points[i] = data.points[0];
            }
        }
        if (pShape->common.type == kCollisionShapeType_Sphere) {
            tmp_count = collision_count;
            closest_distance = 2.0f;
            for (face_index = pObject->box_face_start, pFace = &gFace_list__car[face_index]; face_index < (int)pObject->box_face_end; face_index++, pFace++) {
                SphereFaceCollide(pFace, &data.points[0], &data.points[64], pShape->sphere.sphere.radius, &normal, &closest_point, &distance);
                if (distance < closest_distance) {
                    closest_distance = distance;
                    pClosest_face = pFace;
                    world_point = closest_point;
                }
            }
            if (closest_distance <= 1.0f) {
                BrMatrix34TApplyV(&closest_point, &world_point, pTransform);
                BrMatrix34TApplyV(&actor_point, &world_point, &pObject->actor->t.t.mat);
                neg_radius = -pShape->sphere.sphere.radius;
                normal.v[0] = pShape->sphere.sphere.center.v[0] + actor_point.v[0] * neg_radius;
                normal.v[1] = pShape->sphere.sphere.center.v[1] + actor_point.v[1] * neg_radius;
                normal.v[2] = pShape->sphere.sphere.center.v[2] + actor_point.v[2] * neg_radius;
                /* pPoint is the sphere surface point pulled towards the wall (our "normal"),
                 * pNormal is the face normal returned by SphereFaceCollide (our "closest_point"). */
                tmp_count = AddObjectWallForce((float*)&pContacts[pArg], &normal, closest_distance, &closest_point, 0, collision_count, pObject, *(float*)&three, *(float*)&pClosest_face, NULL);
            }
            i = GetSphereToEdgePoints(pObject, (int)&pShape->sphere.sphere.radius, (int)&pContacts[pArg], tmp_count, 0x28 - pArg, (int)&data.points[0], (int)&data.points[64], (int)&object_to_actor_matrix);
            collision_count += (collision_count < i);
        } else {
            has_collision = 0;
            if (pObject->field_0xed != 0) {
                collision_count = FUN_004236a0(pObject, pShape, &pContacts[pArg], collision_count, 0x28 - pArg, &data.points[0], &object_to_actor_matrix, &has_collision);
            }
            has_any_collision |= has_collision;
            collision_count = GetCornerToFacePoints(pObject, (int)pShape, (tU32)(int)&pContacts[pArg], collision_count, 0x28 - pArg, (char*)&data.points[0], (tU32)(int)&object_to_actor_matrix);
            if (has_collision == 0) {
                collision_count = FUN_00424140(pObject, pShape, (tPhysics_shape*)&pContacts[pArg], collision_count, 0x28 - pArg, (tU8*)&data.points[0], (undefined4)&object_to_actor_matrix);
            }
        }
    }
    pObject->field_0xed = (tU8)has_any_collision;
    if (collision_count == 0) {
        return 0;
    }
    for (; result_index < collision_count; result_index++) {
        if (pContacts[pArg].normal.v[0] * pContacts[pArg + result_index].normal.v[0]
                + pContacts[pArg].normal.v[1] * pContacts[pArg + result_index].normal.v[1]
                + pContacts[pArg].normal.v[2] * pContacts[pArg + result_index].normal.v[2] <= 0.95) {
            break;
        }
    }
    pObject->omega.v[0] = pObject->rotate_omega.v[0];
    pObject->omega.v[1] = pObject->rotate_omega.v[1];
    pObject->omega.v[2] = pObject->rotate_omega.v[2];
    return result_index;
}

// STUB: CARMA2_HW 0x0045e850
void C2_HOOK_FASTCALL SphereFaceCollide(tFace_ref* pFace, br_vector3* pStart, br_vector3* pEnd, br_scalar pRadius, void* pUnused_point, br_vector3* pNormal, br_scalar* pT) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00422fe0
int C2_HOOK_FASTCALL AddObjectWallForce(float* pWalls, const br_vector3* pPoint, float pDistance, const br_vector3* pNormal, tU32 pUnused, int pCount, tPhysics_object* pObject, float pT, float pRadius, const float* pVector) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x00423320
int C2_HOOK_FASTCALL EliminateFaces(tPhysics_object* pObject, int* pCtx, br_matrix34* pMatrix) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x00423ed0
int C2_HOOK_FASTCALL GetCornerToFacePoints(tPhysics_object* pObject, int pEdx, tU32 pParam1, int pParam2, int pParam3, char* pParam4, tU32 pParam5) {
    NOT_IMPLEMENTED();
    return 0;
}

// STUB: CARMA2_HW 0x004236a0
int C2_HOOK_FASTCALL FUN_004236a0(tPhysics_object* pObject, tPhysics_shape* pShape, tSensible_place_contact* pContacts, int pCount, int pMax, br_vector3* pPoints, br_matrix34* pScratch, int* pHas_collision) {
    (void)pObject;
    (void)pShape;
    (void)pContacts;
    (void)pMax;
    (void)pPoints;
    (void)pScratch;
    (void)pHas_collision;
    return pCount;
}

// FUNCTION: CARMA2_HW 0x00424140
int C2_HOOK_FASTCALL FUN_00424140(tPhysics_object* pObject, tPhysics_shape* pBounds_shape, tPhysics_shape* pRay_shape, int pCollision_count, undefined4 pArg5, tU8* pPolyhedron_data, undefined4 pArg7) {
    br_vector3 actor_translation;
    int face_index;
    tFace_ref* face;

    (void)pRay_shape;
    (void)pArg5;
    (void)pArg7;
    gCollision_face_mask__raycast = 0;
    BrMatrix34TApplyV(&actor_translation, (const br_vector3*)&pObject->actor->t.t.mat.m[3], &pObject->actor->t.t.mat);
    pBounds_shape->common.field_0x1c.min.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.min.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.min.v[2] += actor_translation.v[2];
    pBounds_shape->common.field_0x1c.max.v[0] += actor_translation.v[0];
    pBounds_shape->common.field_0x1c.max.v[1] += actor_translation.v[1];
    pBounds_shape->common.field_0x1c.max.v[2] += actor_translation.v[2];

    face_index = (int)pObject->box_face_start;
    face = &gFace_list__car[face_index];
    if (face_index < pObject->box_face_end) {
        do {
            if ((face->flags & 0x80) == 0) {
                if ((face->flags & 1) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[0], (float*)&face->v[1], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 2) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[1], (float*)&face->v[2], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
                if ((face->flags & 4) == 0) {
                    pCollision_count = FUN_00424270((float*)&face->v[2], (float*)&face->v[0], (int*)pObject, (int)pBounds_shape,
                            (undefined4)pRay_shape, pCollision_count, (int)face, (int)pPolyhedron_data);
                }
            }
            face_index++;
            face++;
        } while (face_index < pObject->box_face_end);
    }
    return pCollision_count;
}

// STUB: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data) {
    return pCollision_count;
}


// FUNCTION: CARMA2_HW 0x004293c0
int C2_HOOK_FASTCALL TestPairOverlap(tPhysics_object* pObject, tPhysics_object* pOther) {
    tPhysics_object* child;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    int found;

    found = 0;
    if (pOther != pObject && pObject->parent != pOther && pOther->parent != pObject) {
        shape1 = pObject->shape;
        shape2 = pOther->shape;
        if (FlagsAreGo(pObject, pOther) && BoundsOverlapTest__finteray(&pObject->field_0xf4, &pOther->field_0xf4)) {
            while (shape1 != NULL) {
                while (shape2 != NULL) {
                    if (TestShapeEdgeIntersection(pObject, pOther, shape1, shape2) || TestShapeEdgeIntersection(pOther, pObject, shape2, shape1)) {
                        found = 1;
                        goto done;
                    }
                    shape2 = shape2->common.next;
                }
                shape1 = shape1->common.next;
            }
        }
    }
done:
    if (found != 0) {
        return 1;
    }
    for (child = pOther->child; child != NULL; child = child->next) {
        if (TestPairOverlap(pObject, child)) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042b7f0
intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data) {
    tInitPhysCrushDataCB_Data* context;
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tCar_crush_flap_data* flap_data;

    context = data;
    user_crush_data = actor->user;
    if (user_crush_data == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    model = user_crush_data->models[0];
    if (crush_data == NULL || model == NULL) {
        return 0;
    }
    crush_data->field_0x24 = (
            (model->bounds.max.v[2] - model->bounds.min.v[2])
          * (model->bounds.max.v[1] - model->bounds.min.v[1])
          * (model->bounds.max.v[0] - model->bounds.min.v[0])) * context->value;
    if (crush_data->field_0x24 < 0.01f) {
        crush_data->field_0x24 = 0.01f;
    }
    flap_data = crush_data->flap_data;
    if (flap_data != NULL) {
        br_vector3 tv;
        if ((flap_data->kev_o_flap & 0x2) == 0) {
            CheckHingePointOrder(flap_data, model, &context->car->collision_info->cmpos);
        }
        flap_data->field_0xc = FindNearestParentVertex(actor, &model->vertices[flap_data->hinge0].p);
        flap_data->field_0x12 = FindNearestParentVertex(actor, &model->vertices[flap_data->hinge2].p);
        BrVector3Sub(&tv, &model->vertices[flap_data->hinge2].p, &model->vertices[flap_data->hinge1].p);
        flap_data->field_0x1c = BrVector3Length(&tv) / 2.f;
    }
    if (crush_data->detach_data != NULL) {
        crush_data->detach_data->field_0x2c = context->car;

        SetUpSemiDetachJointStuff(crush_data->detach_data, model, &context->car->collision_info->bb1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042bf80
intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_spec* car_crush_spec;
    tCar_spec* car_spec;
    int i;

    user_crush_data = pActor->user;
    car_spec = data;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL || car_spec->car_crush_spec == NULL) {
        return 0;
    }
    crush_data = user_crush_data->crush_data;
    car_crush_spec = car_spec->car_crush_spec;
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        if (model != NULL && model->nvertices != 0 && model->user != NULL) {
            tModel_detail_vertex_data* vertex_data;
            tUser_detail_level_model* model_user_data;
            int v;

            model_user_data = model->user;

            model_user_data->field_0x4 = BrMemAllocate(model->nvertices * sizeof(tModel_detail_vertex_data), kMem_crush_data);
            vertex_data = model_user_data->field_0x4;
            for (v = 0; v < model->nvertices; v++) {
                int invalid;

                CompressVector3(&vertex_data[v].p, &model->vertices[v].p, -10.0f, 10.0f);
                vertex_data[v].limit_number = CrushLimitNumber(&model->vertices[v].p, &car_crush_spec->limits, &car_crush_spec->count_limits, &invalid);
            }
            if (i == 0 && crush_data->flap_data != NULL) {
                SetFlapCheckVertices(crush_data->flap_data, model, vertex_data);
            }
            if (i == 0) {
                LinkSmashies(pActor, crush_data, model_user_data->field_0x4);
            }
        }
    }
    return 0;
}

intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* pActor, void* pData) {
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ad10
void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;
    tPhysics_object* collision_info;
    int i;

    car_crush = pCar_spec->car_crush_spec;
    collision_info = pCar_spec->collision_info;
    if (car_crush == NULL || collision_info == NULL) {
        return;
    }
    for (i = 0; i < 3; i++) {
        car_crush->limits.limits[i][0].values[car_crush->count_limits.counts[i][0]] = collision_info->bb1.min.v[i] - 0.02898551f;
        car_crush->limits.limits[i][1].values[car_crush->count_limits.counts[i][1]] = collision_info->bb1.max.v[i] + 0.02898551f;
        car_crush->count_limits.counts[i][0] += 1;
        car_crush->count_limits.counts[i][1] += 1;
    }
    {
        tPhysics_shape* shape;
        int j;

        car_crush->count_shapes = 0;
        for (shape = collision_info->shape; shape != NULL; shape = shape->common.next) {
            car_crush->count_shapes += 1;
        }
        car_crush->field_0x4 = BrMemAllocate(car_crush->count_shapes * sizeof(tCar_crush_shape_info), kMem_crush_data);
        for (i = 0, shape = collision_info->shape; shape != NULL; shape = shape->common.next, i++) {
            tCar_crush_shape_info* crush_shape_info = &car_crush->field_0x4[i];
            tCollision_shape_polyhedron* dest;

            crush_shape_info->count_points = shape->polyhedron.polyhedron.count_points;
            crush_shape_info->count_points_add_8 = crush_shape_info->count_points + 8;
            crush_shape_info->field_0x0 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
            dest = crush_shape_info->field_0x0;
            dest->common.type = kCollisionShapeType_Polyhedron;
            dest->polyhedron.count_points = shape->polyhedron.polyhedron.count_points;
            for (j = 0; j < shape->polyhedron.polyhedron.count_points; j++) {
                BrVector3Copy(&dest->polyhedron.points[j], &shape->polyhedron.polyhedron.points[j]);
            }
            FillInShape((tPhysics_shape*)dest);
            crush_shape_info->field_0x4 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
            crush_shape_info->field_0x18 = BrMemAllocate(crush_shape_info->count_points_add_8 * sizeof(tCar_crush_reordered_shape_info), kMem_crush_data);
            for (j = 0; j < crush_shape_info->count_points; j++) {
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x0, &shape->polyhedron.polyhedron.points[j]);
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0xc, &crush_shape_info->field_0x18[j].field_0x0);
                BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x18, &crush_shape_info->field_0x18[j].field_0x0);
            }
            crush_shape_info->field_0x8 = 0;
        }
        car_crush->field_0xc = collision_info->shape;
        collision_info->shape = (tPhysics_shape*)car_crush->field_0x4->field_0x0;
        for (i = 0; i < car_crush->count_shapes - 1; i++) {
            car_crush->field_0x4[i].field_0x0->common.next = (tPhysics_shape*)car_crush->field_0x4[i + 1].field_0x0;
        }
        car_crush->field_0x4[car_crush->count_shapes - 1].field_0x0->common.next = NULL;
        car_crush->expand_bounding_box = 0;
        SetUpShapeLimitingStuff(car_crush, pCar_spec);
        if (gNet_mode != eNet_mode_none) {
            car_crush->field_0x24 = 0;
            car_crush->field_0x26 = 0;
            if (gNet_mode == eNet_mode_host) {
                car_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
                for (i = 1; i < gNumber_of_net_players; i++) {
                    car_crush->network_stuff->items[i - 1].network_id = gNet_players[i].ID;
                    car_crush->network_stuff->items[i - 1].field_0x4 = 0;
                }
            }
        }
    }
    for (i = 0; i < 4; i++) {
        car_crush->field_0x564[i] = AllocateShapeWireFrame(2, 1, kMem_crush_data);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[0], &pCar_spec->wpos[i], WORLD_SCALE);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[1], &pCar_spec->wpos[i], WORLD_SCALE);
        car_crush->field_0x564[i]->wireframe.points[1].v[1] = 0.f;
        car_crush->field_0x564[i]->wireframe.lines[0].index1 = 0;
        car_crush->field_0x564[i]->wireframe.lines[0].index2 = 1;

        FillInShape((tPhysics_shape*)car_crush->field_0x564[i]);
    }
    car_crush->actor->user = pCar_spec;
    car_crush->field_0x574 = 0;
    for (i = 0; ; i++) {
        int c = tolower(pCar_spec->name[i]);
        if (c == 0) {
            break;
        }
        car_crush->field_0x574 *= 2;
        car_crush->field_0x574 += c;
        car_crush->field_0x574 &= 0xffff;
    }
}

// FUNCTION: CARMA2_HW 0x0042b0e0
void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec) {
    br_bounds3 wheel_all_bounds;
    const double inverse_world_scale = 1.0 / 6.9;
    int i;

    BrVector3Copy(&wheel_all_bounds.min, &pCar_spec->wpos[0]);
    BrVector3Copy(&wheel_all_bounds.max, &wheel_all_bounds.min);
    for (i = 1; i < 4; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (!(pCar_spec->wpos[i].v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = pCar_spec->wpos[i].v[j];
            } else if (pCar_spec->wpos[i].v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = pCar_spec->wpos[i].v[j];
            }
        }
    }
    wheel_all_bounds.min.v[0] = (float)(wheel_all_bounds.min.v[0] * inverse_world_scale);
    wheel_all_bounds.min.v[1] = (float)(wheel_all_bounds.min.v[1] * inverse_world_scale);
    wheel_all_bounds.min.v[2] = (float)(wheel_all_bounds.min.v[2] * inverse_world_scale);
    wheel_all_bounds.max.v[0] = (float)(wheel_all_bounds.max.v[0] * inverse_world_scale);
    wheel_all_bounds.max.v[1] = (float)(wheel_all_bounds.max.v[1] * inverse_world_scale);
    wheel_all_bounds.max.v[2] = (float)(wheel_all_bounds.max.v[2] * inverse_world_scale);
    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor* wheel_actor = pCar_spec->wheel_actors[i];
        tUser_crush_data* user_crush_data;
        br_model* wheel_model;
        br_matrix34 wheel_to_car;
        br_bounds3 wheel_bounds;
        int j;

        if (wheel_actor == NULL) {
            continue;
        }
        user_crush_data = wheel_actor->user;
        if (user_crush_data == NULL) {
            continue;
        }
        wheel_model = user_crush_data->models[0];
        if (wheel_model == NULL) {
            continue;
        }
        BrActorToActorMatrix34(&wheel_to_car, wheel_actor, pCar_spec->car_model_actor);
        BrVector3Add(&wheel_bounds.min, &wheel_model->bounds.min, (br_vector3*)wheel_to_car.m[3]);
        BrVector3Add(&wheel_bounds.max, &wheel_model->bounds.max, (br_vector3*)wheel_to_car.m[3]);
        for (j = 0; j < 3; j++) {
            if (!(wheel_bounds.min.v[j] >= wheel_all_bounds.min.v[j])) {
                wheel_all_bounds.min.v[j] = wheel_bounds.min.v[j];
            } else if (wheel_bounds.max.v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = wheel_bounds.max.v[j];
            }
        }
    }
    for (i = 0; i < 3; i++) {
        int j;

        for (j = 0; j < pCar_crush->count_limits.counts[i][0]; j++) {
            pCar_crush->field_0xbc.limits[i][0].values[j] = pCar_crush->limits.limits[i][0].values[j];
        }
        pCar_crush->field_0xbc.limits[i][0].values[j] = wheel_all_bounds.max.v[i];
        pCar_crush->field_0xa4.counts[i][0] = j + 1;
        for (j = 0; j < pCar_crush->count_limits.counts[i][1]; j++) {
            pCar_crush->field_0xbc.limits[i][1].values[j] = pCar_crush->limits.limits[i][1].values[j];
        }
        pCar_crush->field_0xbc.limits[i][1].values[j] = wheel_all_bounds.min.v[i];
        pCar_crush->field_0xa4.counts[i][1] = j + 1;
    }
    for (i = 0; i < 3; i++) {
        qsort(pCar_crush->field_0xbc.limits[i][0].values, pCar_crush->field_0xa4.counts[i][0], sizeof(float), DecreasingCompare);
        qsort(pCar_crush->field_0xbc.limits[i][1].values, pCar_crush->field_0xa4.counts[i][1], sizeof(float), IncreasingCompare);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        int j;
        tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

        for (j = 0; j < shape_info->count_points; j++) {
            int failed;

            shape_info->field_0x18[j].field_0x28 = CrushLimitNumber(&shape_info->field_0x18[j].field_0x0, &pCar_crush->field_0xbc, &pCar_crush->field_0xa4, &failed);
            if (failed != 0) {
                shape_info->field_0x18[j].field_0x24 = 0.0f;
            } else {
                shape_info->field_0x18[j].field_0x24 = SoftnessOfNearestPoint(pCar_spec, &shape_info->field_0x18[j].field_0x0);
            }
        }
    }
}

// STUB: CARMA2_HW 0x004f57c0
void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0042c0e0
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {
    const br_vector3 *p0, *p1, *p2;
    tU16 i;
    br_scalar plane;
    br_scalar distance;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 hinge_normal;
    br_scalar hinge_dot;

    p0 = &pModel->vertices[pFlap_data->hinge0].p;
    p1 = &pModel->vertices[pFlap_data->hinge1].p;
    p2 = &pModel->vertices[pFlap_data->hinge2].p;
    BrVector3Sub(&hinge02, p2, p0);
    BrVector3Sub(&hinge01, p1, p0);
    BrVector3Cross(&hinge_normal, &hinge01, &hinge02);
    BrVector3Normalise(&hinge_normal, &hinge_normal);
    hinge_dot = BrVector3Dot(&hinge_normal, p0);

    for (i = 0; i < pModel->nvertices; i++) {
        distance = PointEdgeDistSq(&pModel->vertices[i].p, &pModel->vertices[pFlap_data->hinge0].p, &pModel->vertices[pFlap_data->hinge1].p);
        plane = BrVector3Dot(&hinge_normal, &pModel->vertices[i].p) - hinge_dot;
        if (distance > 0.00021003991f && plane > -0.0014492754f) {
            pVertex_data[i].flags |= 0x2;
        } else {
            pVertex_data[i].flags &= ~0x2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042c300
float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB) {
    br_vector3 pAB;
    br_vector3 pAP;
    br_vector3 pProj;
    float proj;

    BrVector3Sub(&pAP, pP, pA);
    BrVector3Sub(&pAB, pB, pA);
    proj = BrVector3Dot(&pAP, &pAB) / BrVector3Dot(&pAB, &pAB);
    if (!(proj >= gCrushZero)) {
        proj = gCrushZero;
    } else if (proj > 1.0f) {
        proj = 1.0f;
    }
    BrVector3Scale(&pProj, &pAB, proj);
    BrVector3Add(&pProj, &pProj, pA);
    BrVector3Sub(&pProj, pP, &pProj);
    return BrVector3LengthSquared(&pProj);
}

// STUB: CARMA2_HW 0x004c7f40
void C2_HOOK_FASTCALL FUN_004c7f40(br_model* pModel, br_vector3* pSrc, const float* pScalars) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0042cc40
int C2_HOOK_FASTCALL OutsideEdgeLine(const br_vector3* pPoint, const br_vector3* pA, const br_vector3* pDirection, const br_vector3* pInside) {
    NOT_IMPLEMENTED();
    return 0;
}


#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00450b50
tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject) {
    NOT_IMPLEMENTED();
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) {
    tSensible_place_contact contacts[8];
    tPhysics_object* p1;
    tPhysics_object* p2;
    tPhysics_object* child;
    tPhysics_shape* shape1;
    tPhysics_shape* shape2;
    int count;
    int i;
    int found;
    br_vector3 dir;
    float len;

    gUNK_0079ecbc = NULL;
    p1 = pCollision_info_1;
    p2 = pCollision_info_2;
    GetNewBoundingBox(&p1->field_0xf4, &p1->bb1, &p1->transform_matrix);
    GetFacesInBox(p1, pCar_callbacks);
    gSensible_place_collect_active = 1;
    count = FUN_004228b0(p1, contacts, 0);
    gSensible_place_collect_active = 0;
    if (count != 0) {
        dir.v[0] = contacts[0].normal.v[0];
        dir.v[1] = contacts[0].normal.v[1];
        dir.v[2] = contacts[0].normal.v[2];
        if (count > 1) {
            for (i = 1; i < count; i++) {
                dir.v[0] += contacts[i].normal.v[0];
                dir.v[1] += contacts[i].normal.v[1];
                dir.v[2] += contacts[i].normal.v[2];
            }
            len = (float)sqrt(dir.v[0] * dir.v[0] + dir.v[1] * dir.v[1] + dir.v[2] * dir.v[2]);
            if (len > 2.3841858e-07f) {
                len = 1.0 / len;
                dir.v[0] *= len;
                dir.v[1] *= len;
                dir.v[2] *= len;
            } else {
                dir.v[0] = 1.f;
                dir.v[1] = 0.f;
                dir.v[2] = 0.f;
            }
        }
        BrMatrix34ApplyV(pVec3, &dir, &p1->transform_matrix);
        return 0;
    }
    if (p2 == NULL) {
        return -1;
    }
    do {
        found = 0;
        if (p2 != p1 && p1->parent != p2 && p2->parent != p1) {
            shape1 = p1->shape;
            shape2 = p2->shape;
            if (FlagsAreGo(p1, p2) && BoundsOverlapTest__finteray(&p1->field_0xf4, &p2->field_0xf4)) {
                while (shape1 != NULL) {
                    while (shape2 != NULL) {
                        if (TestShapeEdgeIntersection(p1, p2, shape1, shape2) || TestShapeEdgeIntersection(p2, p1, shape2, shape1)) {
                            found = 1;
                            goto found_body;
                        }
                        shape2 = shape2->common.next;
                    }
                    shape1 = shape1->common.next;
                }
            }
        }
        if (found != 0) {
            goto found_body;
        }
        for (child = p2->child; child != NULL; child = child->next) {
            if (TestPairOverlap(p1, child)) {
                goto found_body;
            }
        }
    } while ((p2 = p2->next) != NULL);
    return -1;
found_body:
    {
        pVec3->v[0] = p1->field_0x10c.min.v[0] + p1->field_0x10c.max.v[0];
        pVec3->v[1] = p1->field_0x10c.min.v[1] + p1->field_0x10c.max.v[1];
        pVec3->v[2] = p1->field_0x10c.min.v[2] + p1->field_0x10c.max.v[2];
        pVec3->v[0] -= p2->field_0x10c.min.v[0];
        pVec3->v[1] -= p2->field_0x10c.min.v[1];
        pVec3->v[2] -= p2->field_0x10c.min.v[2];
        pVec3->v[0] -= p2->field_0x10c.max.v[0];
        pVec3->v[1] -= p2->field_0x10c.max.v[1];
        pVec3->v[2] -= p2->field_0x10c.max.v[2];
        len = (float)sqrt(pVec3->v[0] * pVec3->v[0] + pVec3->v[1] * pVec3->v[1] + pVec3->v[2] * pVec3->v[2]);
        if (len > 2.3841858e-07f) {
            len = 1.0 / len;
            pVec3->v[0] *= len;
            pVec3->v[1] *= len;
            pVec3->v[2] *= len;
        } else {
            pVec3->v[0] = 1.f;
            pVec3->v[1] = 0.f;
            pVec3->v[2] = 0.f;
        }
        gUNK_0079ecbc = p2;
        return 0;
    }
}

extern int gTesting_car_for_sensible_place;

// FUNCTION: CARMA2_HW 0x0041fe50
int C2_HOOK_FASTCALL TestForCarInSensiblePlace(tCar_spec* pCar_spec, br_vector3* pVec3) {
    int r;

    if (!gProgram_state.racing) {
        return 1;
    }
    gTesting_car_for_sensible_place = 1;
    r = TestForObjectInSensiblePlace(pCar_spec->collision_info, gPHIL_list_collision_infos, pVec3, &gWorld_callbacks);
    gTesting_car_for_sensible_place = 0;
    return r;
}
