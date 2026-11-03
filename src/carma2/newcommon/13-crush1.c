/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "13-crush1.h"

#include "00-car.h"
#include "01-network.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "52-errors.h"
#include "60-phil.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

/* crush lists are defined in carma2/common/crush.c, which upstream does not compile */
extern int gCount_crush_detach_list;
extern tCrush_net_detach_list_item gNet_crush_detach_list[8];
extern tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];
extern tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

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

// GLOBAL: CARMA2_HW 0x00589a40
br_scalar gCrushZero = 0.0f;


// GLOBAL: CARMA2_HW 0x00679558
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067be84
tU32 gINT_0067be84;

// GLOBAL: CARMA2_HW 0x00679450
tToggled_door gToggled_doors[16];




// GLOBAL: CARMA2_HW 0x0067bd18
tCar_damge_crush_list_item gCar_damage_crush_list[8];

typedef struct tCrush_net_reattach_list_item_local {
    tCar_spec* actor;
    tCar_spec* car;
    undefined4 field_0x8;
} tCrush_net_reattach_list_item_local;

extern tCrush_net_reattach_list_item_local gNet_crush_reattach_bit_list[8];


extern int C2_HOOK_FASTCALL ProcessForcesCallback(void* pContacts, float* pImpulses, int pCount);
extern int C2_HOOK_FASTCALL ProcessJointForcesCallback(undefined4 param_1, undefined4 param_2, undefined4 param_3);
extern void C2_HOOK_FASTCALL NewFacesListCallback(tPhysics_object* pCollision, undefined4* arg2);

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

// GLOBAL: CARMA2_HW 0x00679430
int gCollision_face_mask__raycast;

// GLOBAL: CARMA2_HW 0x00589c40
float gFloat_00589c40 = -0.5f;

// GLOBAL: CARMA2_HW 0x00589a54
const br_scalar gFLOAT_00589a54 = 1.0f;

extern int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks);
extern tPhysics_object* gPHIL_list_collision_infos;

// FUNCTION: CARMA2_HW 0x0041fbe0
float C2_HOOK_FASTCALL GetCarOverallBoundsMinY(tCar_spec* pCar_spec) {
    float rear;
    float rear_susp;
    float rear_min;
    float front_min;
    float min_value;

    rear = pCar_spec->wpos[0].v[1];
    rear_susp = pCar_spec->susp_height[0];
    rear_min = rear - rear_susp;
    front_min = pCar_spec->wpos[2].v[1] - pCar_spec->susp_height[1];
    rear_min = rear_min * (1.0f / WORLD_SCALE);
    front_min = front_min * (1.0f / WORLD_SCALE);
    min_value = pCar_spec->collision_info->bb1.min.v[1];

    if (rear_min < front_min) {
        if (rear_min < min_value) {
            return rear_min;
        }
    }
    if (front_min < min_value) {
        return front_min;
    }
    return min_value;
}

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

extern int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data);

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

// SoftnessOfNearestPointCB

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

// SetUpShapeLimitingStuff

// CopyShapePolyhedron

// InitShapeStuff

// InitPhysMasterCrushData

// CheckHingePointOrder

// FindNearestVertex

// DistanceToBounds

// FindNearestParentVertex

// SetUpSemiDetachJointStuff

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

// InitPhysModCrushData

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

// PointTriangleDistSq

// PointRectDistSq

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

extern void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds);
extern void C2_HOOK_FASTCALL FUN_00435960(br_model* pModel, const br_vector3* pOffset, tCar_spec** ppCar, br_actor* pActor);

// FUNCTION: CARMA2_HW 0x00433260
intptr_t C2_HOOK_FASTCALL CrushCarModelCbfn(br_actor* pActor, br_matrix34* pMat, void* pContext) {
    struct {
        tCar_spec* car;
        tCar_crush* crush;
        float force;
    }* context;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    br_vector3 points[4];
    br_vector3 direction;
    float scale;
    int i;

    context = pContext;
    if (pActor->type == 1 && pActor->user != NULL) {
        user = pActor->user;
        crush_data = user->crush_data;
        if (crush_data != NULL) {
            for (i = 0; i < context->crush->count; i++) {
                DrMatrix34ApplyLPInverse(&points[i], &context->crush->vectors[i], pMat);
            }
            BrMatrix34TApplyV(&direction, &context->crush->field_0x44, pMat);
            scale = context->force * crush_data->softness_factor;
            direction.v[0] = direction.v[0] * scale;
            direction.v[1] = direction.v[1] * scale;
            direction.v[2] = direction.v[2] * scale;
            for (i = 0; i < context->car->count_detail_levels; i++) {
                SimpleCrushModel(user->models[i], context->crush->count, points[0].v, direction.v,
                    (float*)&context->car->car_crush_spec->field_0x11c);
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004358a0
intptr_t C2_HOOK_FASTCALL FUN_004358a0(br_actor* pActor, br_vector3* pDelta, void* pContext) {
    tUser_crush_data* user_crush_data;
    tCar_spec** ppCar;
    br_actor** piVar2;
    int i;

    user_crush_data = pActor->user;
    if (user_crush_data != NULL) {
        ppCar = pContext;
        for (i = 0, piVar2 = (*ppCar)->wheel_actors; i < 6; i++, piVar2++) {
            if (pActor == piVar2[6]) {
                return 0;
            }
            if (pActor == piVar2[0]) {
                return 0;
            }
        }
        if (user_crush_data->groove != NULL) {
            PipeSingleGrooveOnOff(0, pActor, 3);
            DisableGroovidelic(pActor, 3);
            PipeSingleTransformType(pActor, 5);
            pActor->t.type = 5;
        }
        if (user_crush_data->crush_data != NULL) {
            i = 0;
            if (0 < (*ppCar)->count_detail_levels) {
                do {
                    FUN_00435960(((void**)user_crush_data)[i], pDelta, ppCar, pActor);
                    i++;
                } while (i < (*ppCar)->count_detail_levels);
            }
            user_crush_data->crush_data->field_0x2c = 1;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004367a0
void C2_HOOK_FASTCALL CrushShapeMorphVertex(float* pOut, int pAxis, float pVertexComponent, float pDirectionComponent, tU32 pFlags, tCar_crush_limits* pLimits, int pFound) {
    int index;
    float t;

    index = ((pFlags & 0xffff) >> ((pAxis * 2 + (pDirectionComponent > gCrushZero)) * 2)) & 3;
    if (index > pFound) {
        *pOut = 0.0f;
        return;
    }
    t = (pLimits->limits[pAxis][pDirectionComponent > gCrushZero].values[index] - pVertexComponent) / pDirectionComponent;
    if (t < gCrushZero) {
        *pOut = 0.0f;
    } else if (t < *pOut) {
        *pOut = t;
    }
}

// FUNCTION: CARMA2_HW 0x004369c0
void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc) {
    tU8 i;

    pDest->count = pSrc->count;
    ExpandVector3(&pDest->vectors[0], &pSrc->vectors[0], -2.5f, 2.5f);
    BrVector3Copy(&pDest->field_0x34, &pDest->vectors[0]);
    for (i = 1; i < pSrc->count; i++) {
        ExpandVector3(&pDest->vectors[i], &pSrc->vectors[i], -2.5f, 2.5f);
        BrVector3Accumulate(&pDest->field_0x34, &pDest->vectors[i]);
    }
    BrVector3Set(&pDest->field_0x34, pDest->field_0x34.v[0] / pDest->count, pDest->field_0x34.v[1] / pDest->count, pDest->field_0x34.v[2] / pDest->count);
    pDest->field_0x40 = DRU16ToScalar(pSrc->field_0x1a, 0.f, 10.f);
    ExpandVector3(&pDest->field_0x44, &pSrc->field_0x1c, -1.f, 1.f);
    pDest->field_0x50 = pSrc->field_0x24;
}

// RemoveDuplicatePoints

// CrushDrone

// CrushDroneObject

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

// QueueDroneCrush

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

// FUNCTION: CARMA2_HW 0x0042d7e0
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {
    int i;
    tUser_crush_data* user;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_detach_data* detach_data;
    tCar_crush_spec* car_crush_spec;

    if (gCount_crush_detach_list == REC2_ASIZE(gCrush_detach_list)) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        if (pActor == NULL) {
            return;
        }
        user = pActor->user;
        if (user == NULL) {
            return;
        }
        crush_data = user->crush_data;
        if (crush_data == NULL) {
            return;
        }
        detach_data = crush_data->detach_data;
        if (detach_data == NULL) {
            return;
        }
        for (i = 0; i < gCount_crush_detach_list; i++) {
            if (gCrush_detach_list[i].actor == pActor) {
                return;
            }
        }
        car_crush_spec = pCar->car_crush_spec;
        for (i = 0; i < car_crush_spec->field_0x270; i++) {
            if (car_crush_spec->field_0x274[i].field_0x0 == pActor) {
                return;
            }
        }
        gCrush_detach_list[gCount_crush_detach_list].actor = pActor;
        gCrush_detach_list[gCount_crush_detach_list].car = pCar;
        gCrush_detach_list[gCount_crush_detach_list].field_0x8 = pArg3;
        if (pArg4) {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick;
        } else {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick > gINT_0067be84 + 0x3e8 ? gPHIL_last_physics_tick : gINT_0067be84 + 0x3e8;
        }
        if (gCrush_detach_list[gCount_crush_detach_list].time > gINT_0067be84) {
            gINT_0067be84 = gCrush_detach_list[gCount_crush_detach_list].time;
        }
        gCrush_detach_list[gCount_crush_detach_list].field_0x10 = (detach_data->type == eDetachType_fully_detachable);
        gCount_crush_detach_list = gCount_crush_detach_list + 1;
    }
}

// FUNCTION: CARMA2_HW 0x0042d8f0
void C2_HOOK_FASTCALL AddToggledDoor(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay) {
    extern tToggled_door gToggled_doors[16];
    extern int gCount_toggled_doors;
    int i;

    if (gCount_toggled_doors != 16) {
        for (i = 0; i < gCount_toggled_doors; i++) {
            if (gToggled_doors[i].actor == pActor) {
                return;
            }
        }
        gToggled_doors[gCount_toggled_doors].actor = pActor;
        gToggled_doors[gCount_toggled_doors].car = pCar;
        gToggled_doors[gCount_toggled_doors].field_0x8 = pFlag;
        gToggled_doors[gCount_toggled_doors].field_0xc = pDelay;
        gCount_toggled_doors = gCount_toggled_doors + 1;
    }
}

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

// CentreModelOnPoint

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

// DoDetaching

// FlapBit

// FlapBits

// StartFlapping

// FUNCTION: CARMA2_HW 0x0042f3d0
void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush) {
    int i;
    int new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x270; i++) {
        tCar_bit_spec* car_bit = &pCar_crush->field_0x274[i];

        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        switch (car_bit->field_0xc[10]) {
        case 1:
            if (new_count != i) {
                pCar_crush->field_0x274[new_count] = *car_bit;
            }
            new_count += 1;
            break;
        case 2:
            pCar_crush->field_0x2b4[pCar_crush->field_0x2b0] = *car_bit;
            pCar_crush->field_0x2b0 += 1;
            break;
        }
    }
    pCar_crush->field_0x270 = new_count;

    new_count = 0;
    for (i = 0; i < pCar_crush->field_0x2b0; i++) {
        /* FIXME: what is type ot tCar_bit_spec::field_0xc? */
        switch (pCar_crush->field_0x2b4[i].field_0xc[10]) {
        case 1:
            pCar_crush->field_0x274[pCar_crush->field_0x270] = pCar_crush->field_0x2b4[i];
            pCar_crush->field_0x270 += 1;
            break;
        case 2:
            if (new_count != i) {
                pCar_crush->field_0x2b4[new_count] = pCar_crush->field_0x2b4[i];
            }
            new_count += 1;
            break;
        }
    }
    pCar_crush->field_0x2b0 = new_count;
}

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

// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* data) {
    tCar_spec* pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}

extern int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data);
extern int gCount_toggled_doors;
extern tU32 gToo_poor_for_recovery_timeout;
extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);
extern tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(tU8 pType, int pOption);

// FUNCTION: CARMA2_HW 0x0042ddc0
int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {
    tCar_spec* pCar = data;
    tUser_crush_data* user_data;
    tCar_crush_buffer_entry* crush_buffer;
    int i;

    user_data = pActor->user;
    if (user_data == NULL) {
        return 0;
    }
    crush_buffer = user_data->crush_data;
    if (crush_buffer == NULL) {
        return 0;
    }
    if (crush_buffer->flap_data == NULL || !crush_buffer->flap_data->kev_o_flap) {
        return 0;
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(gToggled_doors) != 16);

    if (crush_buffer->flap_data->field_0x0) {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 0;
            gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
            gCount_toggled_doors += 1;
        }
    } else {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
            gToggled_doors[gCount_toggled_doors].field_0xc = 20.f;
            gCount_toggled_doors += 1;
        }
    }
    return 0;
}

// SendToggleFlaps

// FUNCTION: CARMA2_HW 0x0042dd50
void C2_HOOK_FASTCALL ToggleFlaps(void) {
    tNet_message_chunk* chunk;
    tNet_game_player_info* player_info;

    if (gCountdown || gToo_poor_for_recovery_timeout != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        PhysicsObjectRecurse(gProgram_state.current_car.collision_info, ToggleFlapsCB, &gProgram_state.current_car);
    } else {
        chunk = NetAllocateMessageChunk(eNet_message_chunk_type_toggle_doors, 0);
        player_info = NetPlayerFromCar(&gProgram_state.current_car);
        if (player_info != NULL) {
            chunk->toggle_doors.id = player_info->ID;
        }
    }
}

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

extern void C2_HOOK_FASTCALL PHILRemoveFromParentChain(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern int gCrush_deferred;
extern int gCount_net_crush_full_detach_bit_list;

// STUB: CARMA2_HW 0x0042f4e0
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) { NOT_IMPLEMENTED(); }

// SendFullyDetachBit

// DoFullyDetaching

// FUN_00439500

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

typedef struct tUnbend_model_context_local {
    br_matrix34 matrix;
    br_vector3 field_0x30;
    br_vector3 field_0x3c;
    br_vector3 field_0x48;
    br_vector3 field_0x54;
    tBend_data_local* bend;
    tCar_spec* car;
} tUnbend_model_context_local;

extern void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context_local* pContext);
extern void C2_HOOK_FASTCALL FUN_0043a5b0(br_model* pModel, br_vector3* pTranslation, void* pContext);

// UnBendActorCB

// UnBendCollisionShape

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

typedef struct {
    tU8 id;
    undefined field_0x1[3];
    br_actor* actor;
} tFind_crush_actor_arg;

// FindActorByCrushEntryID

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

extern void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, int pDamage);

// ApplyRandomDamageToCar

// PHILDetachObjectState

// FUN_004c7f40

// FUN_004c7fa0

// FUN_004c86b0

// FUN_004c86f0

// FUN_004ee250

// FUN_004f5a60

// STUB: CARMA2_HW 0x00434f60
void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context_local* pContext) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00424270
int C2_HOOK_FASTCALL FUN_00424270(float* pStart, float* pEnd, int* pObject, int pBounds_shape, undefined4 pRay_shape, int pCollision_count, int pFace, int pPolyhedron_data) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00434320
intptr_t C2_HOOK_CDECL FUN_00434320(br_actor* pActor, int pValue) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00435960
void C2_HOOK_FASTCALL FUN_00435960(br_model* pModel, const br_vector3* pOffset, tCar_spec** ppCar, br_actor* pActor) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043a5b0
void C2_HOOK_FASTCALL FUN_0043a5b0(br_model* pModel, br_vector3* pTranslation, void* pContext) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043bca0
void C2_HOOK_FASTCALL FUN_0043bca0(undefined4* pArg1, void* pArg2, br_model* pModel, const br_vector3* pSrc, const float* pScalars) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004ee290
tSmashable_level* C2_HOOK_FASTCALL FUN_004ee290(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPos, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pDamage, undefined4 pArg9, undefined4 pArg10) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004f5a70
intptr_t C2_HOOK_CDECL FUN_004f5a70(br_actor* pActor, void* pContext) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0042c540
void C2_HOOK_FASTCALL SimpleCrushModel(br_model* pModel, int pCount, float* pPoints, float* pDir, float* pBounds) { NOT_IMPLEMENTED(); }

// GLOBAL: CARMA2_HW 0x0067bad0
int gCount_crush_detach_list;

// GLOBAL: CARMA2_HW 0x00589a2c
br_scalar gFLOAT_00589a2c;

// GLOBAL: CARMA2_HW 0x0067a290
tCrush_net_detach_list_item gNet_crush_detach_list[8];

// GLOBAL: CARMA2_HW 0x0067b7d8
tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067be10
tCrush_net_reattach_list_item_local gNet_crush_reattach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b840
tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

// STUB: CARMA2_HW 0x00433c70
void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00433350
tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0049f860
tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(tU8 pType, int pSize_decider) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00434010
void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00436d60
tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x004391d0
void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x00429070
int C2_HOOK_FASTCALL TestForObjectInSensiblePlace(tPhysics_object* pCollision_info_1, tPhysics_object* pCollision_info_2, br_vector3* pVec3, tWorld_callbacks* pCar_callbacks) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0043b8a0
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) { NOT_IMPLEMENTED(); }
