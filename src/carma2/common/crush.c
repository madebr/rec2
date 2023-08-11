#include "crush.h"

#include "animation.h"
#include "errors.h"
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

#include <string.h>

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
void C2_HOOK_FASTCALL InitCrush(void) {
#if defined(C2_HOOKS_ENABLED)
    InitCrush_original();
#else
#error "Not implemented"
#endif

}
C2_HOOK_FUNCTION_ORIGINAL(0x00429fa0, InitCrush, InitCrush_original)

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(FILE* file) {
    char s[256];

    while (1) {
        if (!GetALineAndDontArgue(file, s)) {
            PDFatalError("Can't find start of CRUSH SETTINGS in .TXT file");
        }
        if (strcmp(s, "START OF CRUSH SETTINGS") == 0) {
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
        if (strcmp(s, "END OF CRUSH SETTINGS") == 0) {
            break;
        }
    }
}
C2_HOOK_FUNCTION(0x00429bb0, LoadGeneralCrushSettings)

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
        ReadShrapnel(pF, level->shrapnels, &level->count_shrapnels);
        ReadExplosionAnimation(pF, &level->animation);
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

int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData) {

    C2_HOOK_BUG_ON(sizeof(tUser_crush_data) != 16);
    C2_HOOK_BUG_ON(sizeof(tGroovidelic_spec) != 132);
    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->user = BrMemAllocate(sizeof(tUser_crush_data), kMem_crush_data);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0048bf20, LinkCrushData)

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, tCar_crush_buffer* pCar_crush_buffer) {
    int i;
    tUser_crush_data* user_crush;

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

void (C2_HOOK_FASTCALL * PrepareCarForCrushing_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL PrepareCarForCrushing(tCar_spec* pCar_spec) {

#if defined(C2_HOOKS_ENABLED)
    PrepareCarForCrushing_original(pCar_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042aa20, PrepareCarForCrushing, PrepareCarForCrushing_original)
