#include "crush.h"

#include "animation.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "platform.h"
#include "powerups.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "smashing.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_ctype.h"
#include "c2_string.h"

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

void (C2_HOOK_FASTCALL * InitCrush_original)(void);
void C2_HOOK_FASTCALL InitCrushSystems(void) {
#if defined(C2_HOOKS_ENABLED)
    InitCrush_original();
#else
#error "Not implemented"
#endif

}
C2_HOOK_FUNCTION_ORIGINAL(0x00429fa0, InitCrushSystems, InitCrush_original)

void (C2_HOOK_FASTCALL * InitTrackCrushables_original)(void);
void C2_HOOK_FASTCALL InitTrackCrushables(void) {

#if defined(C2_HOOKS_ENABLED)
    InitTrackCrushables_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042a190, InitTrackCrushables, InitTrackCrushables_original)

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

void C2_HOOK_FASTCALL LoadAward(FILE* pF, tAward_info* pAward_info) {

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
            level->sounds[j] = LoadSoundInStorage(pBrender_storage, sound);
        }
        ReadShrapnelSpec(pF, level->shrapnels, &level->count_shrapnels);
        ReadExplosion(pF, &level->animation);
        ReadSlick(pF, &level->slick);
        LoadNoncarActivation(pF, &level->noncar_activations, &level->count_noncar_activations);
        ReadShrapnelSideEffects(pF, &level->side_effects);
        /*Extensions flags */
        level->extension_flags = GetAnInt(pF);
        if (level->extension_flags & 0x1) {
            level->field_0x280 = GetAnInt(pF);
        }
        /* Room turn on code */
        level->room_turn_on_code = GetAnInt(pF);
        LoadAward(pF, &level->award);
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
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 2; j++) {
            /* [0] Number of 'X/Y/Z mins' entries.
             * [1] Number of 'X/Y/Z maxs' entries. */
            count = GetAnInt(f);
            car_crush->count_limits[i][j] = count;
            if (count == 0) {
                continue;
            }
            if (count > REC2_ASIZE(car_crush->limits[i].min_max[j])) {
                FatalError(kFatalError_TooManyCrushLimits);
            }
            for (k = 0; k < count; k++) {
                car_crush->limits[i].min_max[j][k] = GetAScalar(f);
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

void (C2_HOOK_FASTCALL * PrepareCarForCrushing_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL PrepareCarForCrushing(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    PrepareCarForCrushing_original(pCar_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042aa20, PrepareCarForCrushing, PrepareCarForCrushing_original)

void (C2_HOOK_FASTCALL * TotallyRepairACar_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    TotallyRepairACar_original(pCar_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00439510, TotallyRepairACar, TotallyRepairACar_original)

void C2_HOOK_FASTCALL TotallyRepairCar(void) {

    if (!C2V(gArrow_mode)) {
        TotallyRepairACar(&C2V(gProgram_state).current_car);
    }
}
C2_HOOK_FUNCTION(0x00439bf0, TotallyRepairCar)

void (C2_HOOK_FASTCALL * FUN_0043b8a0_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL FUN_0043b8a0(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    FUN_0043b8a0_original(pCar_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043b8a0, FUN_0043b8a0, FUN_0043b8a0_original)

void (C2_HOOK_FAKE_THISCALL * TotallySpamTheModel_original)(tCar_spec* pCar_spec, undefined4 pArg2, float pDamage);
void C2_HOOK_FAKE_THISCALL TotallySpamTheModel(tCar_spec* pCar_spec, undefined4 pArg2, float pDamage) {

#if defined(C2_HOOKS_ENABLED)
    TotallySpamTheModel_original(pCar_spec REC2_THISCALL_EDX, pDamage);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00431610, TotallySpamTheModel, TotallySpamTheModel_original)

void (C2_HOOK_FASTCALL * DoDamage_original)(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude);
void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude) {

#if defined(C2_HOOKS_ENABLED)
    DoDamage_original(pCar, pDamage_type, pMagnitude);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0043f930, DoDamage, DoDamage_original)

int (C2_HOOK_FASTCALL * DoCrashEarnings_original)(tCar_spec* pCar1, tCar_spec* pCar2);
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

#if defined(C2_HOOKS_ENABLED)
    return DoCrashEarnings_original(pCar1, pCar2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00440660, DoCrashEarnings, DoCrashEarnings_original)

void (C2_HOOK_FASTCALL * SortOutSmoke_original)(tCar_spec* pCar);
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {

#if defined(C2_HOOKS_ENABLED)
    SortOutSmoke_original(pCar);
#else
#error "Not implemented"
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
