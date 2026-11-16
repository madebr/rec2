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
#include "18-graphics2.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "41-utility.h"
#include "49-physics.h"
#include "51-smash.h"
#include "53-controls.h"
#include "60-phil.h"
#include "globvars.h"
#include "globvrpb.h"

#include "platform.h"
#include "rec2_macros.h"

#include "c2_math.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0058f808
// GLOBAL: CARMA2_SW 0x0058a808
const char* gSoftness_names[6] = {
    "very_soft",
    "soft",
    "normal",
    "hard",
    "very_hard",
    "uncrushable",
};

// GLOBAL: CARMA2_HW 0x0058f848
// GLOBAL: CARMA2_SW 0x0058a848
const char* gCrush_type_names[3] = {
    "boring",
    "flap",
    "detach",
};

// GLOBAL: CARMA2_HW 0x0058f820
// GLOBAL: CARMA2_SW 0x0058a820
const char* gEase_of_detachment_names[5] = {
    "very_easy",
    "easy",
    "normal",
    "difficult",
    "very_difficult",
};

// GLOBAL: CARMA2_HW 0x0058f858
// GLOBAL: CARMA2_SW 0x0058a858
const char* gCar_crush_shape_names[2] = {
    "box",
    "poly",
};

// GLOBAL: CARMA2_HW 0x0058f838
// GLOBAL: CARMA2_SW 0x0058a838
const char* gDetach_type_names[] = {
    "normal",
    "stubborn",
    "fully_detach",
    "joint_index",
};
// GLOBAL: CARMA2_HW 0x0067a290
// GLOBAL: CARMA2_SW 0x00674a00
tCrush_net_detach_list_item gNet_crush_detach_list[8];

// GLOBAL: CARMA2_HW 0x0067b840
// GLOBAL: CARMA2_SW 0x00675fb0
tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x0067b7d8
// GLOBAL: CARMA2_SW 0x00675f48
tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];

// GLOBAL: CARMA2_HW 0x00679558
// GLOBAL: CARMA2_SW 0x00673cc8
tCrush_detach_list_item gCrush_detach_list[16];

// GLOBAL: CARMA2_HW 0x0067bd18
// GLOBAL: CARMA2_SW 0x00676488
tCar_damge_crush_list_item gCar_damage_crush_list[8];

// GLOBAL: CARMA2_HW 0x00679698
// GLOBAL: CARMA2_SW 0x00673e08
br_scalar gDistortion_factor;

// GLOBAL: CARMA2_HW 0x006796b0
// GLOBAL: CARMA2_SW 0x00673e20
br_scalar gForce_to_movement_factor;

// GLOBAL: CARMA2_HW 0x00679550
// GLOBAL: CARMA2_SW 0x00673cc0
br_scalar gMax_detach_time_ms;

// GLOBAL: CARMA2_HW 0x006796c8
// GLOBAL: CARMA2_SW 0x00673e38
tCrush_list_item gCrush_lists[8];

// GLOBAL: CARMA2_HW 0x006796bc
// GLOBAL: CARMA2_SW 0x00673e2c
int gCount_queued_drone_crushes;

// GLOBAL: CARMA2_HW 0x0067bad0
// GLOBAL: CARMA2_SW 0x00676240
int gCount_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067bdfc
// GLOBAL: CARMA2_SW 0x0067656c
int gINT_0067bdfc;

// GLOBAL: CARMA2_HW 0x0067be08
// GLOBAL: CARMA2_SW 0x00676578
tU8 gCrush_data_entry_counter;

// GLOBAL: CARMA2_HW 0x0067bdf0
// GLOBAL: CARMA2_SW 0x00676560
int gCount_net_crush_detach_list;

// GLOBAL: CARMA2_HW 0x0067b838
// GLOBAL: CARMA2_SW 0x00675fa8
int gCount_net_crush_semi_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067bd60
// GLOBAL: CARMA2_SW 0x006764d0
int gCount_net_crush_full_detach_bit_list;

// GLOBAL: CARMA2_HW 0x0067b7cc
// GLOBAL: CARMA2_SW 0x00675f3c
int gCount_car_damage_crush_list;

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

// GLOBAL: CARMA2_HW 0x0067be84
// GLOBAL: CARMA2_SW 0x006765f4
tU32 gUINT_0067be84;

// GLOBAL: CARMA2_HW 0x0067bd5c
// GLOBAL: CARMA2_SW 0x006764cc
br_scalar gMin_force_to_split_Y_per_tonne;

// GLOBAL: CARMA2_HW 0x0067bd64
// GLOBAL: CARMA2_SW 0x006764d4
int gMin_split_damage;

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

// GLOBAL: CARMA2_HW 0x0067b7c8
// GLOBAL: CARMA2_SW 0x00675f38
int gCount_net_crush_reattach_bit_list;

// GLOBAL: CARMA2_HW 0x0067be10
// GLOBAL: CARMA2_SW 0x00676580
tCrush_net_reattach_bit_list_item gNet_crush_reattach_bit_list[8];

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
tPhysics_joint* gTrack_crush_joints[32] = { 0 };

// GLOBAL: CARMA2_HW 0x0067bde8
// GLOBAL: CARMA2_SW 0x00676558
br_scalar gMin_bend_force = 0.f;

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

// GLOBAL: CARMA2_HW 0x0067bd58
// GLOBAL: CARMA2_SW 0x006764c8
int gCount_toggled_doors;

// GLOBAL: CARMA2_HW 0x00679450
// GLOBAL: CARMA2_SW 0x00673bc0
tToggled_door gToggled_doors[16];

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

void C2_HOOK_FASTCALL ClearCrushLists(void) {
    tCrush_list_item* list;

    for (list = gCrush_lists; list->car_spec != NULL; list++) {
        list->car_spec = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00429d90
// FUNCTION: CARMA2_SW 0x00429e20
void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec) {
    int i;
    int new_count;

    C2_HOOK_BUG_ON(sizeof(tCrush_net_detach_list_item) != 0x24);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_semi_detach_bit_list_item) != 0x4c);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_full_detach_bit_list_item) != 0xc);
    C2_HOOK_BUG_ON(sizeof(tCrush_net_reattach_bit_list_item) != 0xc);
    C2_HOOK_BUG_ON(sizeof(tCrush_detach_list_item) != 0x14);
    C2_HOOK_BUG_ON(sizeof(tToggled_door) != 0x10);
    C2_HOOK_BUG_ON(sizeof(tCar_damge_crush_list_item) != 0x8);

    for (i = 0, new_count = 0; i < gCount_net_crush_detach_list; i++) {
        if (gNet_crush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_detach_list[new_count] = gNet_crush_detach_list[i];
            }
            new_count++;
        }
    }
    gCount_net_crush_detach_list = new_count;

    for (i = 0, new_count = 0; i < gCount_net_crush_semi_detach_bit_list; i++) {
        if (gNet_crush_semi_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_semi_detach_bit_list[new_count] = gNet_crush_semi_detach_bit_list[i];
            }
            new_count++;
        }
    }
    gCount_net_crush_semi_detach_bit_list = new_count;

    for (i = 0, new_count = 0; i < gCount_net_crush_full_detach_bit_list; i++) {
        if (gNet_crush_full_detach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_full_detach_bit_list[new_count] = gNet_crush_full_detach_bit_list[i];
            }
            new_count++;
        }
    }
    gCount_net_crush_full_detach_bit_list = new_count;

    for (i = 0, new_count = 0; i < gCount_net_crush_reattach_bit_list; i++) {
        if (gNet_crush_reattach_bit_list[i].car != pCar_spec) {
            if (new_count != i) {
                gNet_crush_reattach_bit_list[new_count] = gNet_crush_reattach_bit_list[i];
            }
            new_count++;
        }
    }
    gCount_net_crush_reattach_bit_list = new_count;

    for (i = 0, new_count = 0; i < gCount_crush_detach_list; i++) {
        if (gCrush_detach_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCrush_detach_list[new_count] = gCrush_detach_list[i];
            }
            new_count++;
        }
    }
    gCount_crush_detach_list = new_count;

    for (i = 0, new_count = 0; i < gCount_toggled_doors; i++) {
        if (gToggled_doors[i].car != pCar_spec) {
            if (new_count != i) {
                gToggled_doors[new_count] = gToggled_doors[i];
            }
            new_count++;
        }
    }
    gCount_toggled_doors = new_count;

    for (i = 0, new_count = 0; i < gCount_car_damage_crush_list; i++) {
        if (gCar_damage_crush_list[i].car != pCar_spec) {
            if (new_count != i) {
                gCar_damage_crush_list[new_count] = gCar_damage_crush_list[i];
            }
            new_count++;
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

// FUNCTION: CARMA2_HW 0x0042a190
// FUNCTION: CARMA2_SW 0x0042a220
void C2_HOOK_FASTCALL ResetCrushSystems(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gTrack_crush_joints); i++) {
        gTrack_crush_joints[i]->type = eJoint_none;
    }
    gDetached_bit_crush_info_buffer.field_0x8 = 0;
    gSplit_car_crush_info_buffer.field_0x8 = 0;
    ClearCrushLists();
    gCount_queued_drone_crushes = 0;
    gCount_crush_detach_list = 0;
    gINT_0067bdfc = 0;
    gCount_toggled_doors = 0;
    gCrush_data_entry_counter = 0;
    gCount_net_crush_detach_list = 0;
    gCount_net_crush_semi_detach_bit_list = 0;
    gCount_net_crush_full_detach_bit_list = 0;
    gCount_net_crush_reattach_bit_list = 0;
    gCount_car_damage_crush_list = 0;
}

// FUNCTION: CARMA2_HW 0x0042a550
// FUNCTION: CARMA2_SW 0x0042a5e0
void C2_HOOK_FASTCALL ReadCrushData(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSoftness_names) != 6);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCrush_type_names) != 3);
    C2_HOOK_BUG_ON(REC2_ASIZE(gEase_of_detachment_names) != 5);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_crush_shape_names) != 2);
    C2_HOOK_BUG_ON(REC2_ASIZE(gDetach_type_names) != 4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tCar_crush_detach_data, shape_points, 16 * sizeof(int));

    GetAString(pF, pCar_crush_buffer_entry->actor_name);
    if (strlen(pCar_crush_buffer_entry->actor_name) < 4) {
        PDFatalError("Less than 4 chars in car actor identifier");
    }
    DRstrlwr(pCar_crush_buffer_entry->actor_name);
    switch (GetALineAndInterpretCommand(pF, gSoftness_names, REC2_ASIZE(gSoftness_names))) {
    case eSoftness_VerySoft:
        pCar_crush_buffer_entry->softness_factor = 4.0f;
        break;
    case eSoftness_Soft:
        pCar_crush_buffer_entry->softness_factor = 2.0f;
        break;
    default:
        pCar_crush_buffer_entry->softness_factor = 1.0f;
        break;
    case eSoftness_Hard:
        pCar_crush_buffer_entry->softness_factor = 0.5f;
        break;
    case eSoftness_VeryHard:
        pCar_crush_buffer_entry->softness_factor = 0.25f;
        break;
    case eSoftness_Uncrushable:
        pCar_crush_buffer_entry->softness_factor = 0.0f;
        break;
    }
    pCar_crush_buffer_entry->id = gCrush_data_entry_counter;
    gCrush_data_entry_counter++;

    switch (GetALineAndInterpretCommand(pF, gCrush_type_names, REC2_ASIZE(gCrush_type_names))) {
    case eCrushType_Flap:
        C2_HOOK_BUG_ON(sizeof(tCar_crush_flap_data) != 0x2c);
        pCar_crush_buffer_entry->flap_data = BrMemAllocate(sizeof(tCar_crush_flap_data), kMem_crush_data);
        pCar_crush_buffer_entry->flap_data->hinge0 = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->hinge1 = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->hinge2 = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x4 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x2 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x6 = BR_ANGLE_DEG(90);
        pCar_crush_buffer_entry->flap_data->field_0x8 = BR_ANGLE_DEG(90);
        pCar_crush_buffer_entry->flap_data->kev_o_flap = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x14 = 1;
        pCar_crush_buffer_entry->flap_data->field_0x15 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x18 = 0;
        if (strstr(pCar_crush_buffer_entry->actor_name, "door") != NULL || strstr(pCar_crush_buffer_entry->actor_name, "dor") != NULL) {
            pCar_crush_buffer_entry->flap_data->is_door = 1;
        } else {
            pCar_crush_buffer_entry->flap_data->is_door = 0;
        }
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 0x88);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0.f;
        switch (GetALineAndInterpretCommand(pF, gEase_of_detachment_names, REC2_ASIZE(gEase_of_detachment_names))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach;
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 2.0f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 4.0f;
            break;
        }
        if (pCar_crush_buffer_entry->flap_data->kev_o_flap) {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_kev_o_flap;
        } else {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_fully_detachable;
        }
        switch (GetALineAndInterpretCommand(pF, gCar_crush_shape_names, REC2_ASIZE(gCar_crush_shape_names))) {
        case eCarCrushShape_poly:
            pCar_crush_buffer_entry->detach_data->count_shape_points = GetAnInt(pF);
            for (i = 0; i < pCar_crush_buffer_entry->detach_data->count_shape_points; i++) {
                pCar_crush_buffer_entry->detach_data->shape_points[i] = GetAnInt(pF);
            }
            break;
        case eCarCrushShape_box:
        default:
            pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
            break;
        }
        pCar_crush_buffer_entry->detach_data->field_0x28 = 0;
        break;
    case eCrushType_Detach:
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 0x88);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0.f;
        switch (GetALineAndInterpretCommand(pF, gEase_of_detachment_names, REC2_ASIZE(gEase_of_detachment_names))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 0.5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach;
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 2.0f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = gNormal_force_to_detach * 4.0f;
            break;
        }
        pCar_crush_buffer_entry->detach_data->type = GetALineAndInterpretCommand(pF, gDetach_type_names, REC2_ASIZE(gDetach_type_names));
        if (pCar_crush_buffer_entry->detach_data->type == eDetachType_joint_index) {
            pCar_crush_buffer_entry->detach_data->field_0x30 = GetAnInt(pF);
        }
        switch (GetALineAndInterpretCommand(pF, gCar_crush_shape_names, REC2_ASIZE(gCar_crush_shape_names))) {
        case eCarCrushShape_poly:
            pCar_crush_buffer_entry->detach_data->count_shape_points = GetAnInt(pF);
            if (pCar_crush_buffer_entry->detach_data->count_shape_points > (int)REC2_ASIZE(pCar_crush_buffer_entry->detach_data->shape_points)) {
                PDFatalError("Too many sample points in crush data");
            }
            for (i = 0; i < pCar_crush_buffer_entry->detach_data->count_shape_points; i++) {
                pCar_crush_buffer_entry->detach_data->shape_points[i] = GetAnInt(pF);
            }
            break;
        case eCarCrushShape_box:
        default:
            pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
            break;
        }
        pCar_crush_buffer_entry->detach_data->field_0x28 = 0;
        break;
    }
    if (gNet_mode != eNet_mode_none && pCar_crush_buffer_entry->detach_data != NULL) {
        pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
    }
    pCar_crush_buffer_entry->field_0x2c = 0;
    ReadSmashableData(pF, pCar_crush_buffer_entry, pBrender_storage);
}

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
// FUNCTION: CARMA2_SW 0x0042aa40
void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_network_shapes) != 0x60);

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0042c0e0
// STUB: CARMA2_SW 0x0042c170
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data* pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec) {
    int invalid;
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        CompressVector3(&pVertex_data[i].p, &pModel->vertices[i].p, -10.0f, 10.0f);
        pVertex_data[i].limit_number = CrushLimitNumber(&pModel->vertices[i].p, &pCar_crush_spec->limits, &pCar_crush_spec->count_limits, &invalid);
    }
    if (pDetail_level == 0) {
        if (pCrush_data->flap_data != NULL) {
            SetFlapCheckVertices(pCrush_data->flap_data, pModel, pVertex_data);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042bf80
// FUNCTION: CARMA2_SW 0x0042c010
intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush_data;
    tCar_crush_buffer_entry* crush_data;
    tCar_spec* car_spec;
    tCar_crush_spec* car_crush_spec;
    tUser_detail_level_model* model_user_data;
    br_model* model;
    int i;

    C2_HOOK_BUG_ON(sizeof(tModel_detail_vertex_data) != 10);

    user_crush_data = pActor->user;
    car_spec = pData;
    if (user_crush_data == NULL || (crush_data = user_crush_data->crush_data) == NULL || (car_crush_spec = car_spec->car_crush_spec) == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        model = user_crush_data->models[i];
        if (model != NULL && model->nvertices != 0 && (model_user_data = model->user) != NULL) {
            model_user_data->field_0x4 = BrMemAllocate(model->nvertices * sizeof(tModel_detail_vertex_data), kMem_crush_data);
            InitModelVertexData(model_user_data->field_0x4, model, i, crush_data, car_crush_spec);
            if (i == 0) {
                LinkSmashies(pActor, crush_data, model_user_data->field_0x4);
            }
        }
    }
    return 0;
}

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

// ExpandCrush

// RemoveDuplicatePoints

// SimpleCrushModel

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
// FUNCTION: CARMA2_SW 0x0042d850
void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4) {
    tUser_crush_data* user_crush;
    tCar_crush_detach_data* detach_data;
    int i;
    tU32 time;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCrush_detach_list) != 16);

    if (gCount_crush_detach_list != REC2_ASIZE(gCrush_detach_list)
            && (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host)
            && pActor != NULL
            && (user_crush = pActor->user) != NULL
            && user_crush->crush_data != NULL
            && (detach_data = user_crush->crush_data->detach_data) != NULL) {
        for (i = 0; i < gCount_crush_detach_list; i++) {
            if (gCrush_detach_list[i].actor == pActor) {
                return;
            }
        }
        for (i = 0; i < pCar->car_crush_spec->field_0x270; i++) {
            if (pCar->car_crush_spec->field_0x274[i].field_0x0 == pActor) {
                return;
            }
        }
        gCrush_detach_list[gCount_crush_detach_list].actor = pActor;
        gCrush_detach_list[gCount_crush_detach_list].car = pCar;
        gCrush_detach_list[gCount_crush_detach_list].field_0x8 = pArg3;
        if (pArg4) {
            gCrush_detach_list[gCount_crush_detach_list].time = gPHIL_last_physics_tick;
        } else {
            time = gUINT_0067be84 + 1000;
            if (gPHIL_last_physics_tick > time) {
                time = gPHIL_last_physics_tick;
            }
            gCrush_detach_list[gCount_crush_detach_list].time = time;
        }
        if (gCrush_detach_list[gCount_crush_detach_list].time > gUINT_0067be84) {
            gUINT_0067be84 = gCrush_detach_list[gCount_crush_detach_list].time;
        }
        gCrush_detach_list[gCount_crush_detach_list].field_0x10 = detach_data->type == eDetachType_fully_detachable;
        gCount_crush_detach_list++;
    }
}

// FUNCTION: CARMA2_HW 0x0042d8f0
// FUNCTION: CARMA2_SW 0x0042d960
void C2_HOOK_FASTCALL SetFlappage(br_actor* pActor, tCar_spec* pCar, int pFlags, float pScalar) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gToggled_doors) != 16);

    if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
        for (i = 0; i < gCount_toggled_doors; i++) {
            if (gToggled_doors[i].actor == pActor) {
                return;
            }
        }
        gToggled_doors[gCount_toggled_doors].actor = pActor;
        gToggled_doors[gCount_toggled_doors].car = pCar;
        gToggled_doors[gCount_toggled_doors].field_0x8 = pFlags;
        gToggled_doors[gCount_toggled_doors].field_0xc = pScalar;
        gCount_toggled_doors += 1;
    }
}

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

// FUNCTION: CARMA2_HW 0x0042dab0
// FUNCTION: CARMA2_SW 0x0042db20
void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, tNet_game_player_info* pPlayer) {
    tNet_message* message;
    tUser_crush_data* user_crush;
    br_vector3 p1;
    br_vector3 p2;
    br_vector3 p3;
    br_bounds3 bounds;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, player, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, id, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, time, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, point_1, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, point_2, 0x16);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, point_3, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, bounds_min, 0x22);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, bounds_max, 0x28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message_chunk_semi_detach_bit, value, 0x2e);
    C2_HOOK_BUG_ON(sizeof(tNet_message_chunk_semi_detach_bit) != 0x30);

    message = NetBuildGuaranteedMessage(eNet_message_chunk_type_semi_detach_bit, 0);
    message->contents.semi_detach_bit.player = NetPlayerFromCar(pCar)->ID;
    user_crush = pActor->user;
    message->contents.semi_detach_bit.id = user_crush->crush_data->id;
    message->contents.semi_detach_bit.time = gPHIL_last_physics_tick + 120;
    if (GetSDBJointPosAndBounds(&p1, &p2, &p3, &bounds, pActor) == 0) {
        CompressVector3(&message->contents.semi_detach_bit.point_1, &p1, -10.0f, 10.0f);
        CompressVector3(&message->contents.semi_detach_bit.point_2, &p2, -10.0f, 10.0f);
        CompressVector3(&message->contents.semi_detach_bit.point_3, &p3, -10.0f, 10.0f);
        CompressVector3(&message->contents.semi_detach_bit.bounds_min, &bounds.min, -10.0f, 10.0f);
        CompressVector3(&message->contents.semi_detach_bit.bounds_max, &bounds.max, -10.0f, 10.0f);
        message->contents.semi_detach_bit.value = DRScalarToU16(pArg3, 0.0f, 40.0f);
        if (pPlayer != NULL) {
            NetGuaranteedSendMessageToPlayer(gCurrent_net_game, message, pPlayer->ID, NULL);
        } else {
            NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, NULL);
        }
    }
}

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

void C2_HOOK_FASTCALL AddNonCar(tNon_car_spec* pNon_car, const tDriver* pDriver) {

    C2_HOOK_BUG_ON(REC2_ASIZE(gActive_non_car_list) != 99);
    // FIXME: pDriver should be passed by argument, not pointer

    if (gNum_active_non_cars != REC2_ASIZE(gActive_non_car_list)) {
        pNon_car->driver = *pDriver;
        gActive_non_car_list[gNum_active_non_cars++] = pNon_car;
        PHILAddActiveObjectImmediately(pNon_car->collision_info, NULL, NULL, NULL);
    }
}

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

// FUNCTION: CARMA2_HW 0x0042ddc0
// FUNCTION: CARMA2_SW 0x0042de30
intptr_t C2_HOOK_CDECL ToggleFlapsCB2(br_actor* pActor, void* pCar) {
    tUser_crush_data* user_crush;
    tCar_crush_buffer_entry* crush_data;
    tCar_crush_flap_data* flap_data;

    user_crush = pActor->user;
    if (user_crush != NULL) {
        crush_data = user_crush->crush_data;
        if (crush_data != NULL) {
            flap_data = crush_data->flap_data;
            if (flap_data != NULL && flap_data->kev_o_flap != 0) {
                if (flap_data->field_0x0 != 0) {
                    SetFlappage(pActor, pCar, 0, 0.f);
                } else {
                    SetFlappage(pActor, pCar, 0x1, 20.f);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ddb0
// FUNCTION: CARMA2_SW 0x0042de20
int C2_HOOK_FASTCALL ToggleFlapsCB(tPhysics_object* pCollision_info, void* pCar) {
    DRActorEnumRecurse(pCollision_info->actor, ToggleFlapsCB2, pCar);
    return 0;
}

void C2_HOOK_FASTCALL SendToggleFlaps(tCar_spec* pCar) {
    tNet_message_chunk* chunk;
    tNet_game_player_info* player;

    chunk = NetGetToHostContents(eNet_message_chunk_type_toggle_doors, 0);
    player = NetPlayerFromCar(pCar);
    if (player != NULL) {
        chunk->toggle_doors.id = player->ID;
    }
}

// FUNCTION: CARMA2_HW 0x0042dd50
// FUNCTION: CARMA2_SW 0x0042ddc0
void C2_HOOK_FASTCALL ToggleFlaps(void) {

    if (gCountdown || gToo_poor_for_recovery_timeout) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        PhysicsObjectRecurse(gProgram_state.current_car.collision_info, ToggleFlapsCB, &gProgram_state.current_car);
    } else {
        SendToggleFlaps(&gProgram_state.current_car);
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

// FUNCTION: CARMA2_HW 0x0042f4e0
// FUNCTION: CARMA2_SW 0x0042f530
void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject) {
    br_actor* actor;
    tUser_crush_data* user_crush;
    tCar_crush_detach_data* detach_data;
    tNon_car_spec* non_car;

    actor = pObject->actor;
    if (actor != NULL && (user_crush = actor->user) != NULL && user_crush->crush_data != NULL
            && (detach_data = user_crush->crush_data->detach_data) != NULL && detach_data->field_0x28 == 1) {
        PhysicsRemoveObject(pObject);
        if (pObject->physics_joint1 != NULL) {
            pObject->physics_joint1->type = eJoint_none;
            pObject->physics_joint1 = NULL;
        }
        if (gPHIL_doing_physics) {
            TellPhysicsJointsHaveChanged(pCar->collision_info);
        }
        pObject->flags |= 0x100;
        non_car = actor->type_data;
        AddNonCar(non_car, &gDetached_bit_driver);
        PipeSingleNonCar((tU16)non_car->car_ID, actor);
        actor->identifier[3] = '!';
        detach_data->field_0x28 = 2;
        pCar->use_shell_model = 0;
    }
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00434f60
// STUB: CARMA2_SW 0x00434f90
void C2_HOOK_FASTCALL BendPoint(br_vector3* pPoint, const tBend_point_context* pContext) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

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

// STUB: CARMA2_HW 0x00431610
// STUB: CARMA2_SW 0x00431640
void C2_HOOK_FASTCALL TotallySpamTheCar(tCar_spec* pCar, br_scalar pDamage) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x004391d0
// STUB: CARMA2_SW 0x00439210
void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar) {
    NOT_IMPLEMENTED();
}

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

// FUNCTION: CARMA2_HW 0x0043aa20
// FUNCTION: CARMA2_SW 0x0043aa70
void C2_HOOK_FASTCALL UnBendCollisionShape(tCar_crush_shape_info* pShape, const tBend_point_context* pContext) {
    int i;

    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(&pShape->field_0x18[i].field_0x18, &pShape->field_0x18[i].field_0x2c);
        BendPoint(&pShape->field_0x18[i].field_0x18, pContext);
    }
}

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

// FUNCTION: CARMA2_HW 0x0043c0e0
// FUNCTION: CARMA2_SW 0x0043c130
void C2_HOOK_FASTCALL ARApplyFlap(br_actor* pActor, br_angle pAngle) {
    tUser_crush_data* user;
    tUser_crush_data* parent_user;
    br_model* parent_model;
    tCar_crush_flap_data* flap;
    const br_vector3* hinge;
    br_vector3 axis;

    user = pActor->user;
    if (user == NULL || user->crush_data == NULL) {
        return;
    }
    flap = user->crush_data->flap_data;
    if (flap == NULL) {
        return;
    }
    flap->field_0x4 = (tU16)(flap->field_0x4 + pAngle);
    hinge = &user->models[0]->vertices[flap->hinge0].p;
    BrVector3Sub(&axis, &user->models[0]->vertices[flap->hinge1].p, hinge);
    BrVector3Normalise(&axis, &axis);
    BrMatrix34Translate(&pActor->t.t.mat,
        -hinge->v[0],
        -hinge->v[1],
        -hinge->v[2]);
    BrMatrix34PostRotate(&pActor->t.t.mat, flap->field_0x4, &axis);
    parent_user = (tUser_crush_data*)pActor->parent->user;
    parent_model = parent_user->models[0];
    BrMatrix34PostTranslate(&pActor->t.t.mat,
        parent_model->vertices[flap->field_0xc].p.v[0],
        parent_model->vertices[flap->field_0xc].p.v[1],
        parent_model->vertices[flap->field_0xc].p.v[2]);
}

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

// FUNCTION: CARMA2_HW 0x0043ce60
// FUNCTION: CARMA2_SW 0x0043ceb0
void C2_HOOK_FASTCALL ReceivedReAttachAllBits(tNet_message_chunk* pContents) {
    tCar_spec* car;

    if (gCount_net_crush_reattach_bit_list == REC2_ASIZE(gNet_crush_reattach_bit_list)) {
        return;
    }
    car = NetCarFromPlayerID(pContents->reattach_all_bits.player);
    if (car == NULL) {
        return;
    }
    gNet_crush_reattach_bit_list[gCount_net_crush_reattach_bit_list].car = car;
    gNet_crush_reattach_bit_list[gCount_net_crush_reattach_bit_list].actor = NULL;
    gNet_crush_reattach_bit_list[gCount_net_crush_reattach_bit_list].time = pContents->reattach_all_bits.time;
    gCount_net_crush_reattach_bit_list += 1;
}

// DoQueuedNetDetaching

// DBNonCarOwner

// SendDBNonCar

// ReceivedDBNonCar

// SendAllDBNonCars

// ReceivedDBNonCarPosition

// FUNCTION: CARMA2_HW 0x0043d670
// FUNCTION: CARMA2_SW 0x0043d6c0
void C2_HOOK_FASTCALL ReceivedToggleFlaps(tNet_message_chunk* pContents) {
    tCar_spec* car;

    car = NetCarFromPlayerID(pContents->toggle_doors.id);
    if (car != NULL) {
        PhysicsObjectRecurse(car->collision_info, ToggleFlapsCB, car);
    }
}

// SHNonCarOwner

// SendSHNonCar

// SendAllSHNonCars

// ReceivedSHNonCar

// FUNCTION: CARMA2_HW 0x0043d900
// FUNCTION: CARMA2_SW 0x0043d950
int C2_HOOK_FASTCALL GetNetShapeDataLengthFromShape(tCar_spec* pCar) {
    int i;
    int count;
    int length;
    tPhysics_shape* shape;

    C2_HOOK_BUG_ON(sizeof(tCompressed_vector3) != 6);

    if (pCar->car_crush_spec->field_0x4b8) {
        length = sizeof(tU8);
        for (shape = pCar->collision_info->shape; shape != NULL; shape = shape->common.next) {
            if (shape->common.type == kCollisionShapeType_Polyhedron || shape->common.type == kCollisionShapeType_Wireframe_Polyhedron) {
                length += sizeof(tU8) + shape->polyhedron.polyhedron.count_points * sizeof(tCompressed_vector3);
            }
        }
        return length;
    } else {
        count = 0;
        for (i = 0; i < pCar->car_crush_spec->count_shapes; i++) {
            count += pCar->car_crush_spec->field_0x4[i].count_points;
        }
        return sizeof(tU8) + count * sizeof(tCompressed_vector3);
    }
}

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

// FUNCTION: CARMA2_HW 0x0043deb0
// FUNCTION: CARMA2_SW 0x0043df00
void C2_HOOK_FASTCALL CompressPlane(tCompressed_vector4* pDest, const br_vector4* pSrc) {

    CompressVector3((tCompressed_vector3*)pDest, (const br_vector3*)pSrc, -1.0f, 1.0f);
    pDest->v[3] = DRScalarToU16(pSrc->v[3], -2.5f, 2.5f);
}

// FUNCTION: CARMA2_HW 0x0043dee0
// FUNCTION: CARMA2_SW 0x0043df30
void C2_HOOK_FASTCALL ExpandPlane(br_vector4* pDest, const tCompressed_vector4* pSrc) {

    ExpandVector3((br_vector3*)pDest, (const tCompressed_vector3*)pSrc, -1.0f, 1.0f);
    pDest->v[3] = DRU16ToScalar(pSrc->v[3], -2.5f, 2.5f);
}

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
