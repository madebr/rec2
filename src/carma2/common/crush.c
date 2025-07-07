#include "crush.h"

#include "animation.h"
#include "car.h"
#include "compress.h"
#include "controls.h"
#include "depth.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"
#include "physics.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "spark.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_ctype.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <math.h>

C2_HOOK_VARIABLE_IMPLEMENT(float, gDistortion_factor,0x00679698 );
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_crush_force, 0x006796b8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_crush_force, 0x0067bdec);
C2_HOOK_VARIABLE_IMPLEMENT(float, gForce_to_movement_factor, 0x006796b0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_crush_dist_sq, 0x0067bacc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_force_to_split_XZ_per_tonne, 0x0067bac8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_force_to_split_Y_per_tonne, 0x0067bd5c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMin_split_damage, 0x0067bd64);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMax_split_damage, 0x0067b7d0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gChance_of_inverse_buckle, 0x0067be04);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlap_inertia_fudge_biscuit, 0x0067b7b4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gBatty_gravity, 0x0067be7c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gTorque_to_snap_per_tonne, 0x0067bdf8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMax_detach_time_ms, 0x00679550);
C2_HOOK_VARIABLE_IMPLEMENT(float, gNormal_force_to_detach, 0x0067944c);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_bend_force, 0x0067bde8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gChance_of_bending, 0x0067be00);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMin_bend_angle, 0x0067a18c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMin_bend_damage, 0x0067b7b0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMax_bend_damage, 0x0067bdf4);
C2_HOOK_VARIABLE_IMPLEMENT(tU8, gCrush_data_entry_counter, 0x0067be08);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoftness_names, 6, 0x0058f808, {
    "very soft",
    "soft",
    "normal",
    "hard",
    "very hard",
    "uncrushable",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gCrush_type_names, 3, 0x0058f848, {
    "boring",
    "flap",
    "detach",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gEase_of_detachment_names, 5, 0x0058f820, {
    "very easy",
    "easy",
    "normal",
    "difficult",
    "very difficult",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gCar_crush_shape_names, 2, 0x0058f858, {
    "box",
    "poly",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gDetach_type_names, 4, 0x0058f838, {
    "normal",
    "stubborn",
    "fully_detach",
    "joint_index",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPosition_type_names, 2, 0x0065ff00, {
    "relative",
    "absolute",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSmashable_collision_type_names, 3, 0x0065fea0, {
    "solid",
    "passthrough",
    "edges",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(undefined4, gCrush_array_0067a190, 10, 0x0067a190);
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gDAT_00679440, 0x00679440);
C2_HOOK_VARIABLE_IMPLEMENT(tCrush_info_buffer, gDetached_bit_crush_info_buffer, 0x006796a0);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gDetached_bit_collision_infos, 0x00679448);
C2_HOOK_VARIABLE_IMPLEMENT(tDriver, gDetached_bit_driver, 0x006796ac);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPhysics_joint*, gTrack_crush_joints, 32, 0x0067bd68);
C2_HOOK_VARIABLE_IMPLEMENT(tCrush_info_buffer, gSplit_car_crush_info_buffer, 0x0067b7b8);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gSplit_car_collision_infos, 0x0067a188);
C2_HOOK_VARIABLE_IMPLEMENT(tDriver, gSplit_car_driver, 0x0067b7c4);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_shape_sphere*, gGonad_sphere_collision_shape, 0x006796b4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tCrush_list_item, gCrush_lists, 8, 0x006796c8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_queued_drone_crushes, 0x006796bc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_crush_detach_list, 0x0067bad0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0067bdfc, 0x0067bdfc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_net_crush_detach_list, 0x0067bdf0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_net_crush_semi_detach_bit_list, 0x0067b838);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_net_crush_full_detach_bit_list, 0x0067bd60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_net_crush_reattach_bit_list, 0x0067b7c8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_car_damage_crush_list, 0x0067b7cc);

void (C2_HOOK_FASTCALL * InitCrushSystems_original)(void);
void C2_HOOK_FASTCALL InitCrushSystems(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitCrushSystems_original();
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(tCrush_info) != 0x104);
    C2_HOOK_BUG_ON(sizeof(tCollision_info) != 0x4d8);

    for (i = 0; i < REC2_ASIZE(C2V(gCrush_array_0067a190)); i++) {
        C2V(gCrush_array_0067a190)[i] = 0;
    }
    C2V(gDAT_00679440) = 0;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_info_buffer, crush_infos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_info_buffer, capacity, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_info, collision_object, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_info, field_0xdc, 0xdc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCrush_info, field_0xe0, 0xe0);

    C2V(gDetached_bit_crush_info_buffer).capacity = 16;
    C2V(gDetached_bit_crush_info_buffer).crush_infos = BrMemAllocate(C2V(gDetached_bit_crush_info_buffer).capacity * sizeof(tCrush_info), kMem_crush_data);
    C2V(gDetached_bit_collision_infos) = BrMemAllocate(C2V(gDetached_bit_crush_info_buffer).capacity * sizeof(tCollision_info),kMem_crush_data);
    for (i = 0; i < C2V(gDetached_bit_crush_info_buffer).capacity; i++) {
        C2V(gDetached_bit_crush_info_buffer).crush_infos[i].field_0xdc = 1.f;
        C2V(gDetached_bit_crush_info_buffer).crush_infos[i].field_0xe0 = 0.28985506f;
        C2V(gDetached_bit_crush_info_buffer).crush_infos[i].collision_object = &C2V(gDetached_bit_collision_infos)[i];
        // FIXME: AllocateShapePolyhedron should return tCollision_shape pointer
        C2V(gDetached_bit_collision_infos)[i].shape = (tCollision_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
    }
    C2V(gDetached_bit_driver) = eDriver_detached_bit;

    for (i = 0; i < REC2_ASIZE(C2V(gTrack_crush_joints)); i++) {
        C2V(gTrack_crush_joints)[i] = AllocatePhysicsJoint(3, kMem_crush_data);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_shape, common.next, 0x34);

    C2V(gSplit_car_crush_info_buffer).capacity = 3;
    C2V(gSplit_car_crush_info_buffer).crush_infos = BrMemAllocate(C2V(gSplit_car_crush_info_buffer).capacity * sizeof(tCrush_info), kMem_crush_data);
    C2V(gSplit_car_collision_infos) = BrMemAllocate(C2V(gSplit_car_crush_info_buffer).capacity * sizeof(tCollision_info),kMem_crush_data);
    for (i = 0; i < C2V(gSplit_car_crush_info_buffer).capacity; i++) {
        int j;
        tCollision_shape* shape = NULL;
        for (j = 0; j < 4; j++) {
            tCollision_shape* polyhedron = (tCollision_shape*)AllocateShapePolyhedron(16, kMem_crush_data);
            if (shape != NULL) {
                shape->common.next = polyhedron;
            } else {
                C2V(gSplit_car_collision_infos)[i].shape = polyhedron;
            }
            shape = polyhedron;
        }
        if (shape != NULL) {
            shape->common.next = NULL;
        }
    }
    C2V(gSplit_car_driver) = eDriver_split_car;
    C2V(gGonad_sphere_collision_shape) = AllocateShapeSphere(kMem_crush_data);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00429fa0, InitCrushSystems, InitCrushSystems_original)

void C2_HOOK_FASTCALL ClearCrushLists(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gCrush_lists)) != 8);
    C2_HOOK_BUG_ON(sizeof(C2V(gCrush_lists)[0]) != 0xc);

    for (i = 0; C2V(gCrush_lists)[i].car_spec != NULL; i++) {
        C2V(gCrush_lists)[i].car_spec = NULL;
    }
}

void (C2_HOOK_FASTCALL * ResetCrushSystems_original)(void);
void C2_HOOK_FASTCALL ResetCrushSystems(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetCrushSystems_original();
#else
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gTrack_crush_joints)) != 32);

    for (i = 0; i < REC2_ASIZE(C2V(gTrack_crush_joints)); i++) {
        C2V(gTrack_crush_joints)[i]->type = eJoint_none;
    }
    C2V(gDetached_bit_crush_info_buffer).field_0x8 = 0;
    C2V(gSplit_car_crush_info_buffer).field_0x8 = 0;
    ClearCrushLists();
    C2V(gCount_queued_drone_crushes) = 0;
    C2V(gCount_crush_detach_list) = 0;
    C2V(gINT_0067bdfc) = 0;
    C2V(gCount_toggled_doors) = 0;
    C2V(gCrush_data_entry_counter) = 0;
    C2V(gCount_net_crush_detach_list) = 0;
    C2V(gCount_net_crush_semi_detach_bit_list) = 0;
    C2V(gCount_net_crush_full_detach_bit_list) = 0;
    C2V(gCount_net_crush_reattach_bit_list) = 0;
    C2V(gCount_car_damage_crush_list) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042a190, ResetCrushSystems, ResetCrushSystems_original)

void C2_HOOK_FASTCALL ReadCrushSettings(FILE* file) {
    char s[256];

    while (1) {
        if (!GetALineAndDontArgue(file, s)) {
            PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
        }
        if (c2_strcmp(s, "START OF CRUSH SETTINGS") == 0) {
            break;
        }
    }
    if (GetAnInt(file) != 4) {
        PDFatalError("Wrong version of CRUSH SETTINGS");
    }
    /* CRUSHING */
    C2V(gDistortion_factor) = GetAScalar(file);
    C2V(gMin_crush_force) = GetAScalar(file);
    C2V(gMax_crush_force) = GetAScalar(file);
    C2V(gForce_to_movement_factor) = GetAScalar(file);
    C2V(gMax_crush_dist_sq) = GetAScalar(file);

    /* SPLITTING */
    C2V(gMin_force_to_split_XZ_per_tonne) = GetAScalar(file);
    C2V(gMin_force_to_split_Y_per_tonne) = GetAScalar(file);
    C2V(gMin_split_damage) = GetAnInt(file);
    C2V(gMax_split_damage) = GetAnInt(file);

    /* BUCKLING */
    C2V(gChance_of_inverse_buckle) = GetAScalar(file);

    /* FLAPPING AND JOINT SNAPPING */
    C2V(gFlap_inertia_fudge_biscuit) = GetAScalar(file);
    C2V(gBatty_gravity) = GetAScalar(file);
    C2V(gTorque_to_snap_per_tonne) = GetAScalar(file);

    /* DETACHING */
    C2V(gMax_detach_time_ms) = GetAScalar(file);
    C2V(gNormal_force_to_detach) = GetAScalar(file);

    /* BENDING */
    C2V(gMin_bend_force) = GetAScalar(file);
    C2V(gChance_of_bending) = GetAScalar(file);
    C2V(gMin_bend_angle) = GetAScalar(file);
    C2V(gMin_bend_damage) = GetAnInt(file);
    C2V(gMax_bend_damage) = GetAnInt(file);
    while (1) {
        if (!GetALineAndDontArgue(file, s)) {
            PDFatalError("Can't find end of CRUSH SETTINGS in .TXT file");
        }
        if (c2_strcmp(s, "END OF CRUSH SETTINGS") == 0) {
            break;
        }
    }
}
C2_HOOK_FUNCTION(0x00429bb0, ReadCrushSettings)

br_scalar C2_HOOK_FASTCALL SquaredDistanceFromLineSegment(br_vector3* pP, br_vector3* pA, br_vector3* pB) {
    br_vector3 v1;
    br_vector3 v2;
    br_scalar f;

    BrVector3Sub(&v1, pB, pA);
    BrVector3Sub(&v2, pP, pA);
    /* FIXME: numerator and denominator are mixed up?! */
    f = BrVector3Dot(&v1, &v1) / BrVector3Dot(&v2, &v1);
    if (f < 0.f) {
        f = 0.f;
    } else if (f > 1.f) {
        f = 1.f;
    }
    BrVector3Scale(&v1, &v1, f);
    BrVector3Sub(&v2, &v2, &v1);
    return BrVector3Dot(&v2, &v2);
}
C2_HOOK_FUNCTION(0x0042c300, SquaredDistanceFromLineSegment)

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds) {
    float x1, x2;
    float y1, y2;
    float z1, z2;

    GetThreeFloats(pF, &x1, &y1, &z1);
    GetThreeFloats(pF, &x2, &y2, &z2);
    pBounds->min.v[0] = MIN(x1, x2);
    pBounds->min.v[1] = MIN(y1, y2);
    pBounds->min.v[2] = MIN(z1, z2);
    pBounds->max.v[0] = MAX(x1, x2);
    pBounds->max.v[1] = MAX(y1, y2);
    pBounds->max.v[2] = MAX(z1, z2);
}
C2_HOOK_FUNCTION(0x004ef460, LoadMinMax)

void C2_HOOK_FASTCALL LoadNoncarActivation(FILE* pF, tNoncar_activation** pNoncar_activations, int* pCount_noncar_activations) {
    int i;
    int v1, v2;
    tNoncar_activation* activation;

    /* Number of non-cars activated */
    *pCount_noncar_activations = GetAnInt(pF);
    C2_HOOK_BUG_ON(sizeof(tNoncar_activation) != 64);
    (*pNoncar_activations) = BrMemAllocate(*pCount_noncar_activations * sizeof(tNoncar_activation), kMem_smash_side_effects);
    for (i = 0; i < *pCount_noncar_activations; i++) {
        activation = &(*pNoncar_activations)[i];
        GetPairOfInts(pF, &v1, &v2);
        activation->time_0x0 = 1000 * v1;
        activation->time_0x2 = 1000 * v2;
        activation->location_type = GetALineAndInterpretCommand(pF, C2V(gPosition_type_names), REC2_ASIZE(C2V(gPosition_type_names)));
        activation->field_0x8 = GetAnInt(pF);
        LoadMinMax(pF, &activation->bounds);
        ReadSmashableInitialSpeed(pF, &activation->speed_info);
    }
}

void C2_HOOK_FASTCALL ReadAward(FILE* pF, tAward_info* pAward_info) {

    pAward_info->frequency = GetALineAndInterpretCommand(pF, C2V(gRepeatability_names), REC2_ASIZE(C2V(gRepeatability_names)));
    if (pAward_info->frequency != kRepeatability_None) {
        pAward_info->field_0x4 = GetAScalar(pF);
        pAward_info->field_0x8 = GetAScalar(pF);
        pAward_info->field_0xc = GetAnInt(pF);
        pAward_info->field_0x10 = GetAnInt(pF);
    }
}

void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage) {
    char s[256];
    int i;
    int j;
    int sound;
    int v1;
    int v2;
    tSmashable_level* level;

    /* pixelmap to use when intact */
    GetAString(pF, s);
    /* Number of levels */
    *pCount_smashable_levels = GetAnInt(pF) + 1;
    C2_HOOK_BUG_ON(sizeof(tSmashable_level) != 708);
    *pSmashable_levels = BrMemAllocate(*pCount_smashable_levels * sizeof(tSmashable_level), kMem_smash_levels);

    (*pSmashable_levels)[0].pixelmaps[0] = BrMapFind(s);
    if ((*pSmashable_levels)[0].pixelmaps[0] == NULL) {
        FatalError(kFatalError_CantLoadSmashPix_S, s);
    }
    (*pSmashable_levels)[0].count_shrapnels = 0;
    (*pSmashable_levels)[0].trigger_threshold = 0;
    for (i = 1; i < *pCount_smashable_levels; i++) {
        level = &(*pSmashable_levels)[i];
        /* trigger threshold (default if zero) */
        level->trigger_threshold = GetAScalar(pF);
        /* flags */
        level->flags = GetAnInt(pF);
        if (pIs_texture_change) {
            level->collision_type = GetALineAndInterpretCommand(pF, C2V(gSmashable_collision_type_names), REC2_ASIZE(C2V(gSmashable_collision_type_names)));
        }
        /* number of possible sounds */
        level->count_sounds = GetAnInt(pF);
        for (j = 0; j < level->count_sounds; j++) {
            /* sound ID */
            sound = GetAnInt(pF);
            level->sounds[j] = LoadSingleSound(pBrender_storage, sound);
        }
        ReadShrapnelSpec(pF, level->shrapnels, &level->count_shrapnels);
        ReadExplosion(pF, &level->animation);
        ReadSpillData(pF, &level->slick);
        LoadNoncarActivation(pF, &level->noncar_activations, &level->count_noncar_activations);
        ReadShrapnelSideEffects(pF, &level->side_effects);
        /*Extensions flags */
        level->extension_flags = GetAnInt(pF);
        if (level->extension_flags & 0x1) {
            level->field_0x280 = GetAnInt(pF);
        }
        /* Room turn on code */
        level->room_turn_on_code = GetAnInt(pF);
        ReadAward(pF, &level->award);
        /* run-time variable changes */
        level->count_runtime_variable_changes = GetAnInt(pF);
        for (j = 0; j < level->count_runtime_variable_changes; j++) {
            GetPairOfInts(pF, &v1, &v2);
            level->runtime_variable_changes[j].field_0x0 = v2;
            level->runtime_variable_changes[j].field_0x2 = v1;
        }
        /* Number of pixelmaps */
        level->count_pixelmaps = GetAnInt(pF);
        for (j = 0; j < level->count_pixelmaps; j++) {
            GetAString(pF, s);
            level->pixelmaps[j] = BrMapFind(s);
            if (level->pixelmaps[j] == NULL) {
                FatalError(kFatalError_CantLoadSmashPix_S, s);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x004eea20, LoadSmashableLevels)

void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage) {
    int i;

    /* Number of 'Smash data' entries */
    pCar_crush_buffer_entry->count_smashables = GetAnInt(pF);

    C2_HOOK_BUG_ON(sizeof(tCar_crush_smashable_part) != 124);
    pCar_crush_buffer_entry->smashables = BrMemAllocate(pCar_crush_buffer_entry->count_smashables * sizeof(tCar_crush_smashable_part), kMem_smashable_info);
    for (i = 0; i < pCar_crush_buffer_entry->count_smashables; i++) {
        /* name of material */
        GetAString(pF, pCar_crush_buffer_entry->smashables[i].material_name);
        pCar_crush_buffer_entry->smashables[i].funk = -1;
        LoadSmashableLevels(pF, &pCar_crush_buffer_entry->smashables[i].levels, &pCar_crush_buffer_entry->smashables[i].count_smashable_levels, 0, pBrender_storage);
    }
}
C2_HOOK_FUNCTION(0x004ee990, LoadCarCrushSmashDataEntries)

void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry,tBrender_storage *pBrender_storage) {
    int i;

    /* Actor */
    GetAString(pF, pCar_crush_buffer_entry->actor_name);
    if (c2_strlen(pCar_crush_buffer_entry->actor_name) < 4) {
        PDFatalError("Less than 4 chars in car actor identifier");
    }
    StringTransformToLower(pCar_crush_buffer_entry->actor_name);
    /* Softness */
    switch (GetALineAndInterpretCommand(pF, C2V(gSoftness_names), REC2_ASIZE(C2V(gSoftness_names)))) {
    case eSoftness_VerySoft:
        pCar_crush_buffer_entry->softness_factor = 4.f;
        break;
    case eSoftness_Soft:
        pCar_crush_buffer_entry->softness_factor = 2.f;
        break;
    default:
        pCar_crush_buffer_entry->softness_factor = 1.f;
        break;
    case eSoftness_Hard:
        pCar_crush_buffer_entry->softness_factor = 0.5f;
        break;
    case eSoftness_VeryHard:
        pCar_crush_buffer_entry->softness_factor = 0.25f;
        break;
    case eSoftness_Uncrushable:
        pCar_crush_buffer_entry->softness_factor = 0.f;
        break;
    }
    pCar_crush_buffer_entry->id = C2V(gCrush_data_entry_counter);
    C2V(gCrush_data_entry_counter)++;

    /* Crush type */
    switch (GetALineAndInterpretCommand(pF, C2V(gCrush_type_names), REC2_ASIZE(C2V(gCrush_type_names)))) {
    case eCrushType_Flap:
        C2_HOOK_BUG_ON(sizeof(tCar_crush_flap_data) != 44);
        pCar_crush_buffer_entry->flap_data = BrMemAllocate(sizeof(tCar_crush_flap_data), kMem_crush_data);
        /* Hinge point 0 */
        pCar_crush_buffer_entry->flap_data->hinge0 = GetAnInt(pF);
        /* Hinge point 1 */
        pCar_crush_buffer_entry->flap_data->hinge1 = GetAnInt(pF);
        /* Hinge point 2 */
        pCar_crush_buffer_entry->flap_data->hinge2 = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x4 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x2 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x6 = 0x3ffc;
        pCar_crush_buffer_entry->flap_data->field_0x8 = 0x3ffc;
        /* Kev-o-flap? */
        pCar_crush_buffer_entry->flap_data->kev_o_flap = GetAnInt(pF);
        pCar_crush_buffer_entry->flap_data->field_0x14 = 1;
        pCar_crush_buffer_entry->flap_data->field_0x15 = 0;
        pCar_crush_buffer_entry->flap_data->field_0x18 = 0;
        pCar_crush_buffer_entry->flap_data->is_door = c2_strstr(pCar_crush_buffer_entry->actor_name, "door") != NULL || c2_strstr(pCar_crush_buffer_entry->actor_name, "dor") != NULL;
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 136);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0;
        /* Ease of flap */
        switch (GetALineAndInterpretCommand(pF, C2V(gEase_of_detachment_names), REC2_ASIZE(C2V(gEase_of_detachment_names)))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 0.25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 0.5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach);
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 2.f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 4.f;
            break;
        }
        if (pCar_crush_buffer_entry->flap_data->kev_o_flap) {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_kev_o_flap;
        } else {
            pCar_crush_buffer_entry->detach_data->type = eDetachType_fully_detachable;
        }
        /* shape */
        switch (GetALineAndInterpretCommand(pF, C2V(gCar_crush_shape_names), REC2_ASIZE(C2V(gCar_crush_shape_names)))) {
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
        C2_HOOK_BUG_ON(sizeof(tCar_crush_detach_data) != 136);
        pCar_crush_buffer_entry->detach_data = BrMemAllocate(sizeof(tCar_crush_detach_data), kMem_crush_data);
        pCar_crush_buffer_entry->detach_data->field_0x0 = 0;
        /* Ease of detachment */
        switch (GetALineAndInterpretCommand(pF, C2V(gEase_of_detachment_names), REC2_ASIZE(C2V(gEase_of_detachment_names)))) {
        case eEaseOfDetachment_VeryEasy:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * .25f;
            break;
        case eEaseOfDetachment_Easy:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * .5f;
            break;
        case eEaseOfDetachment_Normal:
        default:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach);
            break;
        case eEaseOfDetachment_Hard:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 2.f;
            break;
        case eEaseOfDetachment_VeryHard:
            pCar_crush_buffer_entry->detach_data->force = C2V(gNormal_force_to_detach) * 4.f;
            break;
        }
        /* Type */
        pCar_crush_buffer_entry->detach_data->type = GetALineAndInterpretCommand(pF, C2V(gDetach_type_names), REC2_ASIZE(C2V(gDetach_type_names)));
        if (pCar_crush_buffer_entry->detach_data->type == eDetachType_joint_index) {
            /* FIXME: not used in C2 game data */
            pCar_crush_buffer_entry->detach_data->field_0x30 = GetAnInt(pF);
        }
        switch (GetALineAndInterpretCommand(pF, C2V(gCar_crush_shape_names), REC2_ASIZE(C2V(gCar_crush_shape_names)))) {
        case eCarCrushShape_poly:
            pCar_crush_buffer_entry->detach_data->count_shape_points = GetAnInt(pF);
            if (pCar_crush_buffer_entry->detach_data->count_shape_points > 16) {
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
    if (C2V(gNet_mode) != eNet_mode_none && pCar_crush_buffer_entry->detach_data != NULL) {
        pCar_crush_buffer_entry->detach_data->count_shape_points = 0;
    }
    pCar_crush_buffer_entry->field_0x2c = 0;
    LoadCarCrushSmashDataEntries(pF, pCar_crush_buffer_entry, pBrender_storage);
}
C2_HOOK_FUNCTION(0x0042a550, LoadCarCrushDataEntry)

int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec) {
    int i;
    int j;
    int k;
    FILE* f;
    char s[256];
    int count;
    int version;
    tCar_crush_spec* car_crush;
    int version_le_100;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_buffer) != 260);

    *pCar_crush_spec = NULL;
    pCar_crush_buffer->count_entries = 0;
    f = DRfopen(pPath, "rt");
    if (f == NULL) {
        return 1;
    }
    /* VERSION %d */
    GetALineAndDontArgue(f, s);
    count = c2_sscanf(s, "VERSION %d", &version);
    if (count != 1) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, 0, 4);
    }
    version_le_100 = version <= 100;
    if (!version_le_100) {
        version -= 100;
    }
    if (version != 4) {
        FatalError(kFatalError_WrongCrushDataFileVersion_SDD, pPath, version, 4);
    }

    C2_HOOK_BUG_ON(sizeof(tCar_crush_spec) != 1404);
    car_crush = BrMemAllocate(sizeof(tCar_crush_spec), kMem_crush_data);
    *pCar_crush_spec = car_crush;

    /* Sub member: Master-crush data */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            /* [0] Number of 'X/Y/Z mins' entries.
             * [1] Number of 'X/Y/Z maxs' entries. */
            count = GetAnInt(f);
            car_crush->count_limits.counts[i][j] = count;
            if (count == 0) {
                continue;
            }
            if (count > 2) {
                FatalError(kFatalError_TooManyCrushLimits);
            }
            for (k = 0; k < count; k++) {
                car_crush->limits.limits[i][j].values[k] = GetAScalar(f);
            }
        }
    }
    car_crush->field_0x134 = 0;
    car_crush->field_0x144 = 0;
    /* Bendability factor */
    car_crush->bendability_factor = GetAScalar(f);
    /* Bend point Z min */
    car_crush->bend_z_min = GetAScalar(f);
    /* Bend point Z max */
    car_crush->bend_z_max = GetAScalar(f);
    car_crush->field_0x270 = 0;
    car_crush->field_0x2b0 = 0;
    car_crush->field_0x4b8 = 0;
    /* Snappability factor */
    car_crush->snappability_factor = GetAScalar(f);
    /* Y split position */
    car_crush->split_y_pos = GetAScalar(f);
    /* Driver position */
    GetThreeFloats(f, &car_crush->driver_position.v[0], &car_crush->driver_position.v[1], &car_crush->driver_position.v[2]);
    car_crush->model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    car_crush->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(car_crush->actor, car_crush->model_actor);
    car_crush->actor->identifier = BrResStrDup(car_crush->actor, "Sx?!");
    car_crush->version_le_100 = version_le_100;
    /* Number of 'Crush data' entries. */
    pCar_crush_buffer->count_entries = car_crush->nb_entries = GetAnInt(f);
    if (pCar_crush_buffer->count_entries > kMax_crush_datas) {
        FatalError(kFatalError_TooManyCrushDatas);
    }
    C2V(gCrush_data_entry_counter) = 1;
    for (i = 0; i < pCar_crush_buffer->count_entries; i++) {
        pCar_crush_buffer->entries[i] = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
        LoadCarCrushDataEntry(f, pCar_crush_buffer->entries[i], pBrender_storage);
    }
    PFfclose(f);
    return 0;
}
C2_HOOK_FUNCTION(0x0042a280, LoadCarCrush)

int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData) {

    C2_HOOK_BUG_ON(sizeof(tUser_crush_data) != 16);
    C2_HOOK_BUG_ON(sizeof(tGroovidelic_spec) != 132);
    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->user = BrMemAllocate(sizeof(tUser_crush_data), kMem_crush_data);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0048bf20, LinkCrushData)

void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor *pActor, tCar_crush_buffer *pBuffer) {

    DRActorEnumRecurse(pActor, LinkCarCrushData, pBuffer);
}
C2_HOOK_FUNCTION(0x0042a940, LinkCarCrushDatas)

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* data) {
    int i;
    tUser_crush_data* user_crush;
    tCar_crush_buffer* pCar_crush_buffer = data;

    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    user_crush->crush_data = NULL;
    for (i = 0; i < pCar_crush_buffer->count_entries; i++) {
        if (DRStricmp(pCar_crush_buffer->entries[i]->actor_name, pActor->identifier) == 0) {
            user_crush->crush_data = pCar_crush_buffer->entries[i];
            return 0;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0042a950, LinkCarCrushData)

int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;
    int i;
    tCar_spec* car_spec = pData;

    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        if (user_crush->models[i] != NULL) {
            C2_HOOK_BUG_ON(sizeof(tUser_detail_level_model) != 12);
            user_crush->models[i]->user = BrMemAllocate(sizeof(tUser_detail_level_model), kMem_crush_data);
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0048bf50, AllocateUserDetailLevel)

int C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData) {
    tCrush_model_pool* pool = pData;
    tUser_crush_data* user_crush_data = pData;
    int len_actor_identifier;
    int len_model_identifier;
    int i;
    int crush_idx;
    const char *dot;

    if (pActor == NULL || pActor->identifier == NULL) {
        return 0;
    }
    user_crush_data = pActor->user;
    if (user_crush_data == NULL) {
        return 0;
    }

    dot = c2_strrchr(pActor->identifier, '.');
    if (dot == NULL) {
        len_actor_identifier = c2_strlen(pActor->identifier);
    } else {
        len_actor_identifier = dot - pActor->identifier;
    }
    for (i = 0; i < pool->model_count; i++) {
        br_model* model = pool->models[i];
        if (model == NULL || model->identifier == NULL) {
            continue;
        }
        dot = c2_strrchr(model->identifier, '.');
        if (dot == NULL) {
            len_model_identifier = c2_strlen(model->identifier);
        } else {
            len_model_identifier = dot - model->identifier;
        }
        if (len_actor_identifier != len_model_identifier || c2_strncmp(model->identifier, pActor->identifier, len_actor_identifier) != 0) {
            continue;
        }
        if (dot != NULL && c2_isdigit(dot[1])) {
            crush_idx = dot[1] - '0';
        } else {
            crush_idx  = -1;
        }
        if (crush_idx < 0) {
            for (crush_idx = 0; crush_idx < pool->count_detail_levels; crush_idx++) {
                if (user_crush_data->models[crush_idx] == NULL || user_crush_data->models[crush_idx]->nvertices < model->nvertices) {
                    break;
                }
            }
        }
        if (crush_idx < REC2_ASIZE(user_crush_data->models)) {
            if (user_crush_data->models[crush_idx] != NULL && crush_idx < REC2_ASIZE(user_crush_data->models) - 1) {
                c2_memmove(&user_crush_data->models[crush_idx + 1],
                    &user_crush_data->models[crush_idx],
                    (REC2_ASIZE(user_crush_data->models) - crush_idx - 1) * sizeof(br_model*));
            }
            user_crush_data->models[crush_idx] = model;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0048bfa0, LinkCrushModel)

intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData) {
    tAccumulateSquashVertices_UserData* context = pData;
    tUser_crush_data* crush_data = actor->user;

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
C2_HOOK_FUNCTION(0x0042ac70, AccumulateSquashVertices)

int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2) {
    float v1 = *(const float*)pValue1;
    float v2 = *(const float*)pValue2;

    if (v1 < v2) {
        return -1;
    } else if (v1 == v2) {
        return 0;
    } else {
        return 1;
    }
}
C2_HOOK_FUNCTION(0x0042acb0, IncreasingCompare)

int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2) {
    float v1 = *(const float*)pValue1;
    float v2 = *(const float*)pValue2;

    if (v2 < v1) {
        return -1;
    } else if (v2 == v1) {
        return 0;
    } else {
        return 1;
    }
}
C2_HOOK_FUNCTION(0x0042ace0, DecreasingCompare)

tU16 (C2_HOOK_FASTCALL * CrushLimitNumber_original)(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);
tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid) {

#if defined(C2_HOOKS_ENABLED)
    return CrushLimitNumber_original(pPoint, pLimits, pCount_limits, pInvalid);
#else
    tU16 number;
    int i;

    *pInvalid = 0;
    number = 0;
    for (i = 2; i >= 0; i--) {
        int j;
        tU16 mi = 0;
        tU16 ma = 0;

        for (j = 0; j < pCount_limits->counts[i][0] - 1; j++) {
            if (pPoint->v[i] > pLimits->limits[i][0].values[j]) {
                *pInvalid = 0;
                break;
            }
            mi += 1;
        }

        for (j = 0; j < pCount_limits->counts[i][1] - 1; j++) {
            if (pPoint->v[i] < pLimits->limits[i][1].values[j]) {
                *pInvalid = 0;
                break;
            }
            ma += 1;
        }
        number = (number << 4) | (mi << 2) | (ma << 0);
    }
    return number;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042b4c0, CrushLimitNumber, CrushLimitNumber_original)

void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_vertex_data) != 0x6);

    car_crush = pCar_spec->car_crush_spec;
    if (car_crush != NULL) {
        tAccumulateSquashVertices_UserData data;
        data.count_vertices = 0;
        data.bens_z_min = car_crush->bend_z_min;
        data.bens_z_max = car_crush->bend_z_max;
        DRActorEnumRecurse(pCar_spec->car_model_actor, AccumulateSquashVertices, &data);
        car_crush->field_0x1cc = BrMemAllocate(data.count_vertices * sizeof(tCar_crush_vertex_data), kMem_crush_data);
        BrMatrix34Copy(&car_crush->model_actor->t.t.mat ,&pCar_spec->car_model_actor->t.t.mat);
    }
}

void C2_HOOK_FASTCALL CheckHingePointOrder(tCar_crush_flap_data* pHinge, br_model* pModel, br_vector3* pPos) {
    br_vector3 tv1, tv2, tv3;

    BrVector3Sub(&tv1, &pModel->vertices[pHinge->hinge1].p, &pModel->vertices[pHinge->hinge0].p);
    BrVector3Sub(&tv2, &pModel->vertices[pHinge->hinge2].p, &pModel->vertices[pHinge->hinge0].p);
    BrVector3Cross(&tv3, &tv2, &tv1);
    Vector3Average(&tv1, &pModel->bounds.min, &pModel->bounds.max);
    BrVector3Sub(&tv1, &tv1, pPos);
    if (BrVector3Dot(&tv1, &tv3) > 0.f) {
        tU16 tmp;

        tmp = pHinge->hinge0;
        pHinge->hinge0 = pHinge->hinge1;
        pHinge->hinge1 = tmp;
    }
}

tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel) {
    tU16 closest_vertex = 0;
    br_scalar closest_dist = BR_SCALAR_MAX;
    tU16 i;

    for (i = 0; i < pModel->nvertices; i++) {
        br_scalar d = Vector3DistanceSquared(pPoint, &pModel->vertices[i].p);
        if (d < closest_dist) {
            closest_dist = d;
            closest_vertex = i;
        }
    }
    return closest_vertex;
}
C2_HOOK_FUNCTION(0x0042bb00, FindNearestVertex)

tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos) {
    br_vector3 p;

    if (pActor->parent == NULL || pActor->parent->user == NULL || ((tUser_crush_data*)pActor->parent->user)->models[0] == NULL) {
        PDFatalError("I can't cope flapping bit doesn't have a valid parent");
    }
    BrMatrix34ApplyP(&p, pPos, &pActor->t.t.mat);
    return FindNearestVertex(&p,  ((tUser_crush_data*)pActor->parent->user)->models[0]);
}

void (C2_HOOK_FASTCALL * SetUpSemiDetachJointStuff_original)(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);
void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds) {

#if defined(C2_HOOKS_ENABLED)
    SetUpSemiDetachJointStuff_original(pDetach_data, pModel, pBounds);
#else

    if (pDetach_data->type == eDetachType_joint_index) {
        float closest_corner_distance = BR_SCALAR_MAX;
        br_vector3 vp;
        int i;
        const br_vector3* bounds_points = &pBounds->min;

        BrVector3Copy(&vp, &pModel->vertices[pDetach_data->field_0x30].p);
        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            float dist_bb;

            BrVector3Set(&corner,
                bounds_points[!!(i & 0x1)].v[0],
                bounds_points[!!(i & 0x2)].v[1],
                bounds_points[!!(i & 0x4)].v[2]);
            dist_bb = Vector3DistanceSquared(&corner, &vp);
            if (dist_bb < closest_corner_distance) {
                pDetach_data->field_0x32 = i;
                closest_corner_distance = dist_bb;
            }
        }
    } else {
        float closest_distance = BR_SCALAR_MAX;
        int i;
        const br_vector3* bounds_points = &pBounds->min;
        br_vector3 closest_corner;

        for (i = 0; i < 8; i++) {
            br_vector3 corner;
            int j;
            float dist_closest_side = BR_SCALAR_MAX;

            BrVector3Set(&corner,
                bounds_points[!!(i & 0x1)].v[0],
                bounds_points[!!(i & 0x2)].v[1],
                bounds_points[!!(i & 0x4)].v[2]);
            for (j = 0; j < 3; j++) {
                float ds;

                ds = fabsf(corner.v[j] - pBounds->min.v[j]);
                if (ds < dist_closest_side) {
                    dist_closest_side = ds;
                }
                ds = fabsf(corner.v[j] - pBounds->max.v[j]);
                if (ds < dist_closest_side) {
                    dist_closest_side = ds;
                }
            }
            if (dist_closest_side < closest_distance) {
                pDetach_data->field_0x32 = i;
                closest_distance = dist_closest_side;
                BrVector3Copy(&closest_corner, &corner);
            }
        }
        pDetach_data->field_0x30 = FindNearestVertex(&closest_corner, pModel);
    }
    {
        br_vector3 center;
        int i;
        const br_vector3* bounds_points = &pBounds->min;
        float closest = BR_SCALAR_MAX;

        Vector3Average(&center, &pBounds->min, &pBounds->max);
        for (i = 0; i < 6; i++) {
            if (i != 2) {
                float d = fabsf(center.v[i / 2] - bounds_points[!!(i & 0x1)].v[i / 2]);
                if (d < closest) {
                    pDetach_data->field_0x33 = i;
                    closest = d;
                }
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042bb90, SetUpSemiDetachJointStuff, SetUpSemiDetachJointStuff_original)

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
C2_HOOK_FUNCTION(0x0042b7f0, InitPhysCrushDataCB)

void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec) {
    tCollision_info* collision_info;
    tInitPhysCrushDataCB_Data data;

    collision_info = pCar_spec->collision_info;
    data.car = pCar_spec;
    data.value = collision_info->M /
                 ((collision_info->bb2.max.v[0] - collision_info->bb2.min.v[0]) *
                  (collision_info->bb2.max.v[1] - collision_info->bb2.min.v[1]) *
                  (collision_info->bb2.max.v[2] - collision_info->bb2.min.v[2]));
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitPhysCrushDataCB, &data);
}

void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor) {
    int i;
    float d_min, d_max;
    br_vector3 min_v, max_v;

    if (pModel == NULL || pParent_model == NULL) {
        return;
    }
    d_max = d_min = BR_SCALAR_MAX;
    BrMatrix34ApplyP(&min_v, &pModel->bounds.min, &pActor->t.t.mat);
    BrMatrix34ApplyP(&max_v, &pModel->bounds.max, &pActor->t.t.mat);

    for (i = 0; i < pParent_model->nvertices; i++) {
        float d;

        d = Vector3Distance(&min_v, &pParent_model->vertices[i].p);
        if (d < d_min) {
            pDetach_data->min_vertex = i;
            d_min = d;
        }

        d = Vector3Distance(&max_v, &pParent_model->vertices[i].p);
        if (d < d_max) {
            pDetach_data->max_vertex = i;
            d_max = d;
        }
    }
}

intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData) {
    tUser_crush_data* user_crush_data = pActor->user;
    tCar_crush_buffer_entry* crush_data;
    br_model* model;
    tSoftnessOfNearestPointCB_Context* context = pData;
    br_vector3 delta_point;
    float dist_bb_min;
    float dist_bb_max;
    float dist_p;
    int i;

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
    BrVector3Sub(&delta_point, context->point, pPoint);
    dist_bb_min = Vector3DistanceSquared(&delta_point, &model->bounds.min);
    dist_bb_max = Vector3DistanceSquared(&delta_point, &model->bounds.max);
    dist_p = MAX(dist_bb_min, dist_bb_max);
    if (dist_p - Vector3DistanceSquared(&model->bounds.max, &model->bounds.min) <= context->distance) {
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
C2_HOOK_FUNCTION(0x0042b590, SoftnessOfNearestPointCB)

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext) {
    br_vector3 delta;
    intptr_t result;
    br_actor* a;

    if (pDelta == NULL) {
        BrVector3Set(&delta, 0.f, 0.f, 0.f);
    } else {
        BrVector3Add(&delta, (br_vector3*)pActor->t.t.mat.m[3], pDelta);
    }

    result = cbfn(pActor, &delta, pContext);
    if (result != 0) {
        return result;
    }
    for(a = pActor->children; a != NULL; a = a->next) {
        result = DRActorEnumRecurseWithTranslation(a, &delta, cbfn, pContext);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0042b760, DRActorEnumRecurseWithTranslation)

float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint) {
    tSoftnessOfNearestPointCB_Context softcb_data;

    softcb_data.point = pPoint;
    softcb_data.distance = BR_SCALAR_MAX;
    softcb_data.softness_factor = 1.f;

    DRActorEnumRecurseWithTranslation(pCar_spec->car_model_actor, NULL, SoftnessOfNearestPointCB, &softcb_data);
    return softcb_data.softness_factor;
}

void (C2_HOOK_FASTCALL * SetUpShapeLimitingStuff_original)(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetUpShapeLimitingStuff_original(pCar_crush, pCar_spec);
#else
    br_bounds3 wheel_all_bounds;
    int i;

    BrVector3Copy(&wheel_all_bounds.min, &pCar_spec->wpos[0]);
    BrVector3Copy(&wheel_all_bounds.max, &wheel_all_bounds.min);
    for (i = 1; i < 4; i++) {
        int j;

        for (j = 0; j < 3; j++) {
            if (pCar_spec->wpos[i].v[j] < wheel_all_bounds.min.v[j]) {
                wheel_all_bounds.min.v[j] = pCar_spec->wpos[i].v[j];
            } else if (pCar_spec->wpos[i].v[j] > wheel_all_bounds.max.v[j]) {
                wheel_all_bounds.max.v[j] = pCar_spec->wpos[i].v[j];
            }
        }
    }
    BrVector3InvScale(&wheel_all_bounds.min, &wheel_all_bounds.min, WORLD_SCALE);
    BrVector3InvScale(&wheel_all_bounds.max, &wheel_all_bounds.max, WORLD_SCALE);
    for (i = 0; i < REC2_ASIZE(pCar_spec->wheel_actors); i++) {
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

            if (wheel_bounds.min.v[j] < wheel_all_bounds.min.v[j]) {
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
        pCar_crush->field_0xbc.limits[i][0].values[pCar_crush->count_limits.counts[i][0]] = wheel_all_bounds.max.v[i];
        pCar_crush->field_0xa4.counts[i][0] += 1;

        for (j = 0; j < pCar_crush->count_limits.counts[i][1]; j++) {
            pCar_crush->field_0xbc.limits[i][1].values[j] = pCar_crush->limits.limits[i][1].values[j];
        }
        pCar_crush->field_0xbc.limits[i][1].values[pCar_crush->count_limits.counts[i][1]] = wheel_all_bounds.min.v[i];
        pCar_crush->field_0xa4.counts[i][1] += 1;
    }
    for (i = 0; i < 3; i++) {
        c2_qsort(pCar_crush->field_0xbc.limits[i][0].values, pCar_crush->field_0xa4.counts[i][0], sizeof(float), DecreasingCompare);
        c2_qsort(pCar_crush->field_0xbc.limits[i][1].values, pCar_crush->field_0xa4.counts[i][1], sizeof(float), IncreasingCompare);
    }
    for (i = 0; i < pCar_crush->count_shapes; i++) {
        int j;
        tCar_crush_shape_info* shape_info = &pCar_crush->field_0x4[i];

        for (j = 0; j < shape_info->count_points; j++) {
            int failed;

            shape_info->field_0x18[j].field_0x28 = CrushLimitNumber(
                &shape_info->field_0x18[j].field_0x18,
                &pCar_crush->field_0xbc, &pCar_crush->field_0xa4,
                &failed);
            if (failed == 0) {
                shape_info->field_0x18[j].field_0x24 = SoftnessOfNearestPoint(pCar_spec, &shape_info->field_0x18[j].field_0x18);
            } else {
                shape_info->field_0x18[j].field_0x24 = 0.f;
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042b0e0, SetUpShapeLimitingStuff, SetUpShapeLimitingStuff_original)

intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data) {
    tUser_crush_data* user_crush_data;

    user_crush_data = actor->user;
    if (user_crush_data != NULL && user_crush_data->models[0] != NULL
            && user_crush_data->crush_data != NULL && user_crush_data->crush_data->detach_data != NULL) {
        tUser_crush_data* parent_user_crush_data;

        parent_user_crush_data = actor->parent->user;

        CalculateReferencePoints(user_crush_data->models[0], parent_user_crush_data->models[0],
            user_crush_data->crush_data->detach_data, actor);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0042be20, InitModelCrushDataCB)

void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec) {
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitModelCrushDataCB, pCar_spec);
}

void C2_HOOK_FASTCALL CopyShapePolyhedron(tCollision_shape_polyhedron* pDest, const tCollision_shape_polyhedron* pSrc) {
    int i;

    pDest->common.type = kCollisionShapeType_Polyhedron;
    pDest->polyhedron.count_points = pSrc->polyhedron.count_points;
    for (i = 0; i < pSrc->polyhedron.count_points; i++) {
        BrVector3Copy(&pDest->polyhedron.points[i], &pSrc->polyhedron.points[i]);
    }
    FillInShape((tCollision_shape*)pDest);
}

void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_network_shapes) != 0x60);

    pCar_crush->field_0x24 = 0;
    pCar_crush->field_0x26 = 0;

    if (C2V(gNet_mode) == eNet_mode_host) {
        pCar_crush->network_stuff = BrMemAllocate(sizeof(tCar_crush_network_shapes), kMem_crush_data);
        for (i = 1; i < C2V(gNumber_of_net_players); i++) {
            pCar_crush->network_stuff->items[i - 1].network_id = C2V(gNet_players)[i].ID;
            pCar_crush->network_stuff->items[i - 1].field_0x4 = 0;
        }
    }
}

void C2_HOOK_FASTCALL InitShapeStuff(tCar_crush_spec* pCar_crush, tCollision_info* pCollision_info, tCar_spec* pCar_spec) {
    tCollision_shape* shape;
    int i;

    C2_HOOK_BUG_ON(sizeof(tCar_crush_shape_info) != 0x1c);
    C2_HOOK_BUG_ON(sizeof(tCar_crush_reordered_shape_info) != 0x38);

    pCar_crush->count_shapes = 0;
    for (shape = pCollision_info->shape; shape != NULL; shape = shape->common.next) {
        pCar_crush->count_shapes += 1;
    }

    pCar_crush->field_0x4 = BrMemAllocate(pCar_crush->count_shapes * sizeof(tCar_crush_shape_info), kMem_crush_data);
    for (i = 0, shape = pCollision_info->shape; shape != NULL; shape = shape->common.next, i++) {
        int j;
        tCar_crush_shape_info* crush_shape_info = &pCar_crush->field_0x4[i];

        crush_shape_info->count_points = shape->polyhedron.polyhedron.count_points;
        crush_shape_info->count_points_add_8 = crush_shape_info->count_points + 8;
        crush_shape_info->field_0x0 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
        CopyShapePolyhedron(crush_shape_info->field_0x0, (tCollision_shape_polyhedron*)shape);
        crush_shape_info->field_0x4 = AllocateShapePolyhedron(crush_shape_info->count_points_add_8, kMem_crush_data);
        crush_shape_info->field_0x18 = BrMemAllocate(crush_shape_info->count_points_add_8 * sizeof(tCar_crush_reordered_shape_info), kMem_crush_data);
        for (j = 0; j < crush_shape_info->count_points; j++) {
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x0, &shape->polyhedron.polyhedron.points[j]);
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0xc, &crush_shape_info->field_0x18[j].field_0x0);
            BrVector3Copy(&crush_shape_info->field_0x18[j].field_0x18, &crush_shape_info->field_0x18[j].field_0x0);
        }
        crush_shape_info->field_0x8 = 0;
    }
    pCar_crush->field_0xc = pCollision_info->shape;
    pCollision_info->shape = (tCollision_shape*)pCar_crush->field_0x4->field_0x0;
    for (i = 0; i < pCar_crush->count_shapes - 1; i++) {
        pCar_crush->field_0x4[i].field_0x0->common.next = (tCollision_shape*)pCar_crush->field_0x4[i + 1].field_0x0;
    }
    pCar_crush->field_0x4[pCar_crush->count_shapes - 1].field_0x0->common.next = NULL;
    pCar_crush->expand_bounding_box = 0;
    SetUpShapeLimitingStuff(pCar_crush, pCar_spec);
    if (C2V(gNet_mode) != eNet_mode_none) {
        InitNetworkShapesStuff(pCar_crush);
    }
}

void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush;
    tCollision_info* collision_info;
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
    InitShapeStuff(car_crush, collision_info, pCar_spec);
    for (i = 0; i < 4; i++) {
        car_crush->field_0x564[i] = AllocateWireFrameCollisionShape(2, 1, kMem_crush_data);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[0], &pCar_spec->wpos[i], WORLD_SCALE);
        BrVector3InvScale(&car_crush->field_0x564[i]->wireframe.points[1], &pCar_spec->wpos[i], WORLD_SCALE);
        car_crush->field_0x564[i]->wireframe.points[1].v[1] = 0.f;
        car_crush->field_0x564[i]->wireframe.lines[0].index1 = 0;
        car_crush->field_0x564[i]->wireframe.lines[0].index2 = 1;

        FillInShape((tCollision_shape*)car_crush->field_0x564[i]);
    }
    car_crush->actor->user = pCar_spec;
    car_crush->field_0x574 = 0;
    for (i = 0; ; i++) {
        int c = c2_tolower(pCar_spec->name[i]);
        if (c == 0) {
            break;
        }
        car_crush->field_0x574 *= 2;
        car_crush->field_0x574 += c;
        car_crush->field_0x574 &= 0xffff;
    }
}
C2_HOOK_FUNCTION(0x0042ad10, InitPhysMasterCrushData)

float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB) {
    br_vector3 pAB;
    br_vector3 pAP;
    br_vector3 pProj;
    float proj;

    BrVector3Sub(&pAB, pB, pA);
    BrVector3Sub(&pAP, pP, pA);
    proj = BrVector3Dot(&pAP, &pAB) / BrVector3Dot(&pAB, &pAB);
    if (proj < 0.f) {
        proj = 0.f;
    } else if (proj > 1.f) {
        proj = 1.f;
    }
    BrVector3Scale(&pProj, &pAB, proj);
    BrVector3Sub(&pProj, &pAP, &pProj);
    return BrVector3LengthSquared(&pProj);
}
C2_HOOK_FUNCTION(0x0042c300, PointEdgeDistSq)

void (C2_HOOK_FASTCALL * SetFlapCheckVertices_original)(tCar_crush_flap_data *pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);
void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data *pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetFlapCheckVertices_original(pFlap_data, pModel, pVertex_data);
#else
    const br_vector3 *p0, *p1, *p2;
    br_vector3 hinge01;
    br_vector3 hinge02;
    br_vector3 hinge_normal;
    int i;

    p0 = &pModel->vertices[pFlap_data->hinge0].p;
    p1 = &pModel->vertices[pFlap_data->hinge1].p;
    p2 = &pModel->vertices[pFlap_data->hinge2].p;

    BrVector3Sub(&hinge02, p2, p0);
    BrVector3Sub(&hinge01, p1, p0);
    BrVector3Cross(&hinge_normal, &hinge01, &hinge02);
    BrVector3Normalise(&hinge_normal, &hinge_normal);

    for (i = 0; i < pModel->nvertices; i++) {

        if (PointEdgeDistSq(&pModel->vertices[i].p, p0, p1) > 0.00021004f && BrVector3Dot(&hinge_normal, &pModel->vertices[i].p) - BrVector3Dot(&hinge_normal, p0) > -0.001449275f) {
            pVertex_data[i].flags |= 0x2;
        } else {
            pVertex_data[i].flags &= ~0x2;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042c0e0, SetFlapCheckVertices, SetFlapCheckVertices_original)

void C2_HOOK_FASTCALL InitModelVertexData(tModel_detail_vertex_data* pVertex_data, br_model* pModel, int pDetail_level, tCar_crush_buffer_entry* pCrush_data, tCar_crush_spec* pCar_crush_spec) {
    int i;

    for (i = 0; i < pModel->nvertices; i++) {
        int invalid;

        CompressVector3(&pVertex_data[i].p, &pModel->vertices[i].p, -10.f, 10.f);
        pVertex_data[i].limit_number = CrushLimitNumber(&pModel->vertices[i].p, &pCar_crush_spec->limits, &pCar_crush_spec->count_limits, &invalid);
    }
    if (pDetail_level == 0 && pCrush_data != NULL && pCrush_data->flap_data != NULL) {

        SetFlapCheckVertices(pCrush_data->flap_data, pModel, pVertex_data);
    }
}

intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data) {
    tUser_crush_data* user_crush_data;
    tCar_spec* car_spec;
    int i;

    C2_HOOK_BUG_ON(sizeof(tModel_detail_vertex_data) != 10);

    user_crush_data = pActor->user;
    car_spec = data;
    if (user_crush_data == NULL || user_crush_data->crush_data == NULL || car_spec->car_crush_spec == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        br_model* model = user_crush_data->models[i];
        if (model != NULL && model->nvertices != 0 && model->user != NULL) {
            tUser_detail_level_model* model_user_data = model->user;
            model_user_data->field_0x4 = BrMemAllocate(model->nvertices * sizeof(tModel_detail_vertex_data), kMem_crush_data);
            InitModelVertexData(model_user_data->field_0x4, model, i, user_crush_data->crush_data, car_spec->car_crush_spec);
            if (i == 0) {
                LinkSmashies(pActor, user_crush_data->crush_data, model_user_data->field_0x4);
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0042bf80, InitVertexDataCB)

void C2_HOOK_FASTCALL InitVertexData(tCar_spec* pCar_spec) {
    tCar_crush_spec* car_crush_spec;
    int i;

    car_crush_spec = pCar_spec->car_crush_spec;
    if (car_crush_spec == NULL) {
        return;
    }
    for (i = 0; i < 3; i++) {
        c2_qsort(car_crush_spec->limits.limits[i][0].values, car_crush_spec->count_limits.counts[i][0], sizeof(float), DecreasingCompare);
        c2_qsort(car_crush_spec->limits.limits[i][1].values, car_crush_spec->count_limits.counts[i][1], sizeof(float), IncreasingCompare);
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, InitVertexDataCB, pCar_spec);
}

void C2_HOOK_FASTCALL CheckWheelPositions(const tCar_spec* pCar_spec) {
    int wheel_i;

    for (wheel_i = 0; wheel_i < 4; wheel_i++) {
        const br_vector3* wpos = &pCar_spec->wpos[wheel_i];
        const tCollision_shape* shape;
        int wheel_outside_all_shapes = 1;

        for (shape = pCar_spec->collision_info->shape; shape != NULL; shape = shape->common.next) {
            int wheel_outside_shape = 1;
            if (shape->common.type == kCollisionShapeType_Polyhedron) {
                int plane_i;

                for (plane_i = 0; plane_i < shape->polyhedron.polyhedron.count_planes; plane_i++) {
                    const br_vector4* plane = &shape->polyhedron.polyhedron.planes[plane_i];
                    if (BrVector3Dot(plane, wpos) / WORLD_SCALE + plane->v[3] < 0.f) {
                        wheel_outside_shape = 0;
                        break;
                    }
                }
            }
            if (wheel_outside_shape) {
                wheel_outside_all_shapes = 0;
                break;
            }
        }
        if (wheel_outside_all_shapes) {
            PDFatalError("Wheels outside all shapes");
        }
    }
}

void (C2_HOOK_FASTCALL * InitPhysModCrushData_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    InitPhysModCrushData_original(pCar_spec);
#else
    CheckWheelPositions(pCar_spec);
    if (pCar_spec->car_crush_spec != NULL) {
        SwitchCarModels(pCar_spec, 0);
        InitModelMasterCrushData(pCar_spec);
        InitModelCrushData(pCar_spec);
        InitPhysMasterCrushData(pCar_spec);
        InitPhysCrushData(pCar_spec);
        InitVertexData(pCar_spec);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042aa20, InitPhysModCrushData, InitPhysModCrushData_original)

void C2_HOOK_CDECL MakeModelMaterialsSingleSided(br_model *pModel) {
    if (pModel != NULL) {
        int g;

        for (g = 0; g < pModel->prepared->ngroups; g++) {
            br_material *mat = pModel->faces[*pModel->prepared->groups[g].face_user].material;
            if (mat != NULL && (mat->flags & BR_MATF_TWO_SIDED) && mat->identifier != NULL && mat->identifier[0] == 'S') {
                mat->flags &= ~BR_MATF_TWO_SIDED;
                BrMaterialUpdate(mat, BR_MATU_RENDERING);
            }
        }
    }
}

intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg) {
    tUser_crush_data* user_crush = (tUser_crush_data*)pActor->user;
    tCar_spec *car_spec = (tCar_spec*)pArg;
    if (user_crush != NULL) {
        int i;

        for (i = 0; i < car_spec->count_detail_levels; i++) {
            if (user_crush->models[i] != NULL) {
                MakeModelMaterialsSingleSided(user_crush->models[i]);
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0042deb0, MakeCarModelsMaterialsSingleSided)

void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen) {

    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
    pModel->flags &= ~BR_MODF_CUSTOM;
    BrZsModelRender(pActor, pModel, pMaterial, pOrder_table, pStyle, BrOnScreenCheck(&pModel->bounds), 0);
}
C2_HOOK_FUNCTION(0x00431590, JitModelUpdate)

void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel) {

    if ((pModel->flags & BR_MODF_CUSTOM) && pModel->user) {
        pModel->user = (void*)(uintptr_t)1;
    } else {
        pModel->custom = JitModelUpdate;
        pModel->flags |= BR_MODF_CUSTOM;
    }
}
C2_HOOK_FUNCTION(0x004315e0, SetModelForUpdate)

void (C2_HOOK_FASTCALL * TotallyRepairModel_original)(br_model* pModel);
void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel) {
    tUser_detail_level_model* user_detail_model;
    tModel_detail_vertex_data* vertex_detail_data;
    int i;
    br_vector3 vector_mash[1024];

    if (pModel == NULL) {
        return;
    }
    user_detail_model = pModel->user;
    if (user_detail_model == NULL) {
        return;
    }
    vertex_detail_data = user_detail_model->field_0x4;
    if (vertex_detail_data == NULL) {
        return;
    }
    if (pModel->nvertices > REC2_ASIZE(vector_mash)) {
        PDFatalError("Holy polygons Batman! There's a lot of vertices on that model.");
    }
    for (i = 0; i < pModel->nvertices; i++) {
        br_vector3 pos;

        ExpandVector3(&pos, &vertex_detail_data[i].p, -10.f, 10.f);
        BrVector3Sub(&vector_mash[i], &pos, &pModel->vertices[i].p);
        BrVector3Copy(&pModel->vertices[i].p, &pos);
        vertex_detail_data[i].flags &= ~0x4;
    }
    AddModelMashToPipingSession(pModel, vector_mash);
    SetModelForUpdate(pModel);
}
C2_HOOK_FUNCTION_ORIGINAL(0x00439a20, TotallyRepairModel, TotallyRepairModel_original)

intptr_t (C2_HOOK_CDECL * TotallyRepairModels_original)(br_actor* pActor, void* pUser);
intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser) {

#if defined(C2_HOOKS_ENABLED)
    return TotallyRepairModels_original(pActor, pUser);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00439a20, TotallyRepairModels, TotallyRepairModels_original)

int C2_HOOK_FASTCALL TotallyRepairObject(tCollision_info* pCollision_info, void* pUser_data) {

    DRActorEnumRecurse(pCollision_info->actor, TotallyRepairModels, pUser_data);
    return 0;
}
C2_HOOK_FUNCTION(0x00439a10, TotallyRepairObject)

intptr_t (C2_HOOK_CDECL * BattenDownTheHatches_original)(br_actor* pActor, void* pUser);
intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser) {

#if defined(C2_HOOKS_ENABLED)
    return BattenDownTheHatches_original(pActor, pUser);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004398d0, BattenDownTheHatches, BattenDownTheHatches_original)

int C2_HOOK_FASTCALL BattenDownTheObjects(tCollision_info* pCollision_info, void* pUser_data) {

    DRActorEnumRecurse(pCollision_info->actor, BattenDownTheHatches, pUser_data);
    return 0;
}
C2_HOOK_FUNCTION(0x004398c0, BattenDownTheObjects)

void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {

        tCar_crush_shape_info *shape_info;
        int j;

        shape_info = &pCar_spec->car_crush_spec->field_0x4[i];
        for (j = 0; j < shape_info->count_points; j++) {

            BrVector3Copy(&shape_info->field_0x18[j].field_0x18,
                          &shape_info->field_0x18[j].field_0x0);
            if (DRVector3TestForNan(&shape_info->field_0x18[j].field_0x18)) {
                PDEnterDebugger("NaN");
            }
        }
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}
C2_HOOK_FUNCTION(0x00439960, TotallyRepairCarCollisionShapes)

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info *pShape) {
    int i;

    pShape->count_points = pShape->original_count_points;
    for (i = 0; i < pShape->count_points; i++) {
        BrVector3Copy(
            &pShape->field_0x18[i].field_0x18,
            &pShape->field_0x18[i].field_0x2c);
    }
}
C2_HOOK_FUNCTION(0x00439fd0, CompletelyUnBendCollisionShape)

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < pCar_spec->car_crush_spec->count_shapes; i++) {
        CompletelyUnBendCollisionShape(&pCar_spec->car_crush_spec->field_0x4[i]);
    }
    pCar_spec->car_crush_spec->expand_bounding_box = 3;
}

void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_actor *actor;
        tGroovidelic_spec *groove;

        actor = pCar_spec->pivot_actors[i];
        if (actor == NULL) {
            actor = pCar_spec->wheel_actors[i];
        }
        groove = ActorsGroove(actor);
        if (groove != NULL && groove->path_type == eGroove_path_straight) {
            br_vector3 original_centre;
            br_vector3 delta;

            BrVector3Copy(&original_centre, &groove->path_data.straight_info.centre);
            BrVector3Copy(&groove->path_data.straight_info.centre,
                &pCar_spec->car_crush_spec->field_0x1ec[i]);
            BrVector3Sub(&delta,
                &groove->path_data.straight_info.centre,
                &original_centre);
            PipeSingleVector3(&groove->path_data.straight_info.centre, &delta);
        }
    }

    for (i = 0; i < REC2_ASIZE(pCar_spec->wpos); i++) {
        BrVector3Copy(&pCar_spec->wpos[i], &pCar_spec->car_crush_spec->field_0x234[i]);
    }
    SetCarSuspGiveAndHeight(pCar_spec REC2_THISCALL_EDX, 1.f, 1.f, 1.f, 0.f, 0.f);
    pCar_spec->field_0x1260 = 0.f;
}
C2_HOOK_FUNCTION(0x0043a010, CompletelyUnBendWheels)

void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec) {

    CompletelyUnBendCollisionShapes(pCar_spec);
    CompletelyUnBendWheels(pCar_spec);
    BrVector3Copy(
        &pCar_spec->collision_info->cmpos,
        &pCar_spec->car_crush_spec->field_0x264);
}

intptr_t C2_HOOK_CDECL EnableGroovers(br_actor *pActor, void* pData) {
    tUser_crush_data *user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL && user_crush->groove != NULL) {
        PipeSingleGrooveOnOff(1, pActor, 3);
        EnableGroovidelic(pActor, 3);
        PipeSingleTransformType(pActor, BR_TRANSFORM_MATRIX34_LP);
        pActor->t.type = BR_TRANSFORM_MATRIX34_LP;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00439910, EnableGroovers)

int (C2_HOOK_FASTCALL * SwapShapesIfPossible_original)(tCar_spec *pCar_spec);
int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec *pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    return SwapShapesIfPossible_original(pCar_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004349c0, SwapShapesIfPossible, SwapShapesIfPossible_original)

void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec) {

    pCar_spec->car_crush_spec->field_0x144 = 0;
    CompletelyUnBendOtherThings(pCar_spec);
    DRActorEnumRecurse(pCar_spec->car_model_actor, EnableGroovers, NULL);
}

void (C2_HOOK_FASTCALL * TotallyRepairACar_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    TotallyRepairACar_original(pCar_spec);
#else
    int i;
    tCar_crush_spec* car_crush;

    StopCarSmokingInstantly(pCar_spec);
    if (ARIsActionReplayAvailable()) {
        PipeInstantUnSmudge(pCar_spec);
    }
    pCar_spec->repair_time += 100000;
    for (i = 0; i < REC2_ASIZE(pCar_spec->damage_units); i++) {
        pCar_spec->damage_units[i].damage_level = 0;
        pCar_spec->damage_units[i].last_level = 0;
        pCar_spec->damage_units[i].smoke_last_level = 0;
    }
    if (pCar_spec->use_shell_model) {
        return;
    }
    car_crush = pCar_spec->car_crush_spec;
    if (car_crush == NULL) {
        return;
    }
    if (car_crush->field_0x4b8 != 0) {
        WeldCar(pCar_spec);
    }
    for (i = 0; i < car_crush->field_0x2b0; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x2b4[i].field_0x4, car_crush->field_0x2b4[i].field_0x0, NULL);
    }
    for (i = 0; i < car_crush->field_0x270; i++) {
        ReAttachBit(pCar_spec, car_crush->field_0x274[i].field_0x4, car_crush->field_0x274[i].field_0x0, car_crush->field_0x274[i].field_0x8);
    }
    car_crush->field_0x270 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, MakeCarModelsMaterialsSingleSided, pCar_spec);
    PipeSingleDSModel(0, pCar_spec);
    if (car_crush->field_0x144) {
        DoCompletelyUnBentThings(pCar_spec);
    }
    ARStartPipingSession(24);
    PhysicsObjectRecurse(pCar_spec->collision_info, TotallyRepairObject, pCar_spec);
    AREndPipingSession();
    dr_dprintf("TotallyRepairACar() - Point 7");
    TotallyRepairCarCollisionShapes(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 8");
    PhysicsObjectRecurse(pCar_spec->collision_info, BattenDownTheObjects, pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 19");
    if (C2V(gProgram_state).prog_status != eProg_idling) {
        dr_dprintf("TotallyRepairACar() - Point 10");
        SwapShapesIfPossible(pCar_spec);
    }
    dr_dprintf("TotallyRepairACar() - Point 11");
    pCar_spec->use_shell_model = 1;
    RemoveCarFromCrushLists(pCar_spec);
    dr_dprintf("TotallyRepairACar() - Point 12");
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00439510, TotallyRepairACar, TotallyRepairACar_original)

void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!C2V(gArrow_mode)) {
        TotallyRepairACar(&C2V(gProgram_state).current_car);
    }
}
C2_HOOK_FUNCTION(0x00439bf0, TotallyRepairCar)

void (C2_HOOK_FASTCALL * WeldCar_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    WeldCar_original(pCar_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043b8a0, WeldCar, WeldCar_original)

void (C2_HOOK_FASTCALL * ReAttachBit_original)(tCar_spec* pCar_spec, br_actor* pActor_arg2, br_actor* pActor_arg3, tCollision_info *pObject);
void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar_spec, br_actor* pActor_arg2, br_actor* pActor_arg3, tCollision_info *pObject) {

#if defined(C2_HOOKS_ENABLED)
    ReAttachBit_original(pCar_spec, pActor_arg2, pActor_arg3, pObject);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00434010, ReAttachBit, ReAttachBit_original)

void (C2_HOOK_FAKE_THISCALL * TotallySpamTheModel_original)(tCar_spec* pCar_spec, undefined4 pArg2, float pDamage);
void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, undefined4 pArg2, float pDamage) {

#if defined(C2_HOOKS_ENABLED)
    TotallySpamTheModel_original(pCar_spec REC2_THISCALL_EDX, pDamage);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00431610, TotallySpamTheModel, TotallySpamTheModel_original)

void (C2_HOOK_FASTCALL * DoDamage_original)(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude);
void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude) {

#if defined(C2_HOOKS_ENABLED)
    DoDamage_original(pCar, pDamage_type, pMagnitude);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043f930, DoDamage, DoDamage_original)

int (C2_HOOK_FASTCALL * DoCrashEarnings_original)(tCar_spec* pCar1, tCar_spec* pCar2);
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

#if defined(C2_HOOKS_ENABLED)
    return DoCrashEarnings_original(pCar1, pCar2);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00440660, DoCrashEarnings, DoCrashEarnings_original)

void (C2_HOOK_FASTCALL * SortOutSmoke_original)(tCar_spec* pCar);
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {

#if defined(C2_HOOKS_ENABLED)
    SortOutSmoke_original(pCar);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00440230, SortOutSmoke, SortOutSmoke_original)

void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

    if (DoCrashEarnings(pCar1, pCar2)) {
        SortOutSmoke(pCar1);
        SortOutSmoke(pCar2);
    }
}
C2_HOOK_FUNCTION(0x00440640, CrashEarnings)

void (C2_HOOK_FASTCALL * DoWheelDamage_original)(tU32 pFrame_period);
void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period) {

#if defined(C2_HOOKS_ENABLED)
    DoWheelDamage_original(pFrame_period);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00440350, DoWheelDamage, DoWheelDamage_original)

void (C2_HOOK_FASTCALL * CrushBendFlapRend_original)(void);
void C2_HOOK_FASTCALL CrushBendFlapRend(void) {

#if defined(C2_HOOKS_ENABLED)
    CrushBendFlapRend_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00436170, CrushBendFlapRend, CrushBendFlapRend_original)

intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* data) {
    tMungeMaterialCB_Context* context = data;
    br_model* model;
    int face_i;

    model = pActor->model;
    if (model == NULL) {
        return 0;
    }
    for (face_i = 0; face_i < model->nfaces; face_i++) {
        br_face* face = &model->faces[face_i];

        if (face->material == context->material_0x0) {
            face->material = context->material_0x4;
            if (context->indices != NULL && context->count_indices < context->capacity_indices) {
                int i;

                for (i = 0; i < 3; i++) {
                    int j;
                    int found = 0;

                    for (j = 0; j < context->count_indices; j++) {
                        if (face->vertices[i] == context->indices[j]) {
                            if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                                context->vertices[context->count_vertices].field_0x0[i] = (tU16)j;
                            }
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (context->count_indices != 0) {
                            BrVector3Accumulate(&context->field_0x20, &face->n);
                        } else {
                            BrVector3Copy(&context->field_0x20, &face->n);
                        }
                        if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                            context->vertices[context->count_vertices].field_0x0[i] = (tU16)context->count_indices;
                        }
                        context->indices[context->count_indices] = face->vertices[i];
                        context->count_indices += 1;
                    }
                }
            }
            if (context->vertices != NULL && context->count_vertices < context->capacity_vertices) {
                context->count_vertices += 1;
            }
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x004f55d0, MungeMaterialCB)

void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal) {
    tMungeMaterialCB_Context munge_material_cb_ctx;

    munge_material_cb_ctx.material_0x0 = pMaterial_1;
    munge_material_cb_ctx.material_0x4 = pMaterial_2;
    munge_material_cb_ctx.capacity_indices = pCapacity_indices;
    munge_material_cb_ctx.indices = pIndices;
    munge_material_cb_ctx.capacity_vertices = pCapacity_vertices;
    munge_material_cb_ctx.count_indices = 0;
    munge_material_cb_ctx.vertices = pVertices;
    munge_material_cb_ctx.count_vertices = 0;

    if (pRecursive) {
        DRActorEnumRecurse(pActor, MungeMaterialCB, &munge_material_cb_ctx);
    } else {
        MungeMaterialCB(pActor, &munge_material_cb_ctx);
    }
    if (munge_material_cb_ctx.count_indices != 0) {
        BrVector3Normalise(pNormal, &munge_material_cb_ctx.field_0x20);
    }
    *pCount_indices = munge_material_cb_ctx.count_indices;
    *pCount_vertices = munge_material_cb_ctx.count_vertices;
}
C2_HOOK_FUNCTION(0x004f54d0, MungeMaterial)

void (C2_HOOK_FASTCALL * LinkSmashies_original)(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);
void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data) {

#if defined(C2_HOOKS_ENABLED)
    LinkSmashies_original(pActor, pCrush_data, pVertex_data);
#else
    int i;

    for (i = 0; i < pCrush_data->count_smashables; i++) {
        tCar_crush_smashable_part* smashable;
        int j;
        tU16 indices[50];
        int count_indices;
        tCar_crush_vertex_data vertices[50];
        int count_vertices;

        C2_HOOK_BUG_ON(sizeof(vertices[0]) != 6);
        C2_HOOK_BUG_ON(sizeof(vertices[0].field_0x0[0]) != 2);

        smashable = &pCrush_data->smashables[i];
        smashable->funk_material = FindMaterial(smashable->material_name, pActor, 0);
        if (smashable->funk_material == NULL) {
            FatalError(kFatalError_CannotFindSmashMaterial_S, smashable->material_name);
        }
        MungeMaterial(pActor, 0, smashable->funk_material, smashable->funk_material,
            REC2_ASIZE(indices), indices, &count_indices,
            REC2_ASIZE(vertices), vertices, &count_vertices, &smashable->field_0x60);
        for (j = 0; j < count_indices; j++) {
            if (j == 0) {
                BrVector3Copy(&smashable->field_0x54, &pActor->model->vertices[indices[j]].p);
            } else {
                BrVector3Accumulate(&smashable->field_0x54, &pActor->model->vertices[indices[j]].p);
            }
        }
        if (count_indices != 0) {
            BrVector3InvScale(&smashable->field_0x54, &smashable->field_0x54, (float)count_indices);
        }
        smashable->field_0x6c = count_indices;
        if (count_indices != 0) {
            smashable->field_0x70 = BrMemAllocate(count_indices * sizeof(tU16), kMem_smash_poly_array);
            c2_memcpy(smashable->field_0x70, indices, count_indices * sizeof(tU16));
        } else {
            smashable->field_0x70 = NULL;
        }
        smashable->field_0x74 = count_vertices;
        if (count_vertices != 0) {
            smashable->field_0x78 = BrMemAllocate(count_vertices * sizeof(tCar_crush_vertex_data), kMem_smash_vertex_array);
            c2_memcpy(smashable->field_0x78, vertices, count_vertices * sizeof(tCar_crush_vertex_data));
        } else {
            smashable->field_0x78 = NULL;
        }
        BrResFree(smashable->funk_material->identifier);
        smashable->funk_material->identifier = NULL;
        smashable->field_0x4c = 0;
        smashable->funk_material->colour_map = smashable->levels->pixelmaps[0];
        BrMaterialUpdate(smashable->funk_material, BR_MATU_COLOURMAP);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f57c0, LinkSmashies, LinkSmashies_original)

int C2_HOOK_FASTCALL DRVector3TestForNan(const br_vector3* pV) {

    if (pV == NULL) {
        return 0;
    }
    return isnan(pV->v[0]) || isnan(pV->v[1]) || isnan(pV->v[2]);
}
C2_HOOK_FUNCTION(0x00516160, DRVector3TestForNan)

void (C2_HOOK_FASTCALL * RemoveCarFromCrushLists_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    RemoveCarFromCrushLists_original(pCar_spec);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00429d90, RemoveCarFromCrushLists, RemoveCarFromCrushLists_original)
