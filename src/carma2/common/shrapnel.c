#include "shrapnel.h"

#include "crush.h"
#include "errors.h"
#include "loading.h"
#include "smashing.h"
#include "temp.h"
#include "utility.h"
#include "world.h"

#include "platform.h"  /* For PDFatalError */

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gShrapnel_type_names, 4, 0x0065fec0, {
    "abstract",
    "shards",
    "ghostparts",
    "noncars",
});

C2_HOOK_VARIABLE_IMPLEMENT(int, gShrapnel_ghost_actor_count, 0x006a3338);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gSmashable_noncar_shrapnel_actors, 100, 0x006a5140);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSmash_side_effect_direction_names, 3, 0x0065fef0, {
    "parallel",
    "towards",
    "away",
});


intptr_t C2_HOOK_CDECL AddGhostActorToBuffer(br_actor* pActor, void* data) {
    br_actor** pActorBuffer = data;

    pActorBuffer[C2V(gShrapnel_ghost_actor_count)] = pActor;
    C2V(gShrapnel_ghost_actor_count) += 1;
    return 0;
}
C2_HOOK_FUNCTION(0x004ef440, AddGhostActorToBuffer)

void (C2_HOOK_FASTCALL * ReadShrapnel_original)(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);
void C2_HOOK_FASTCALL ReadShrapnelSpec(FILE* pF, tShrapnel_spec* pShrapnel_specs, int* pShrapnel_count) {
#if 0//defined(C2_HOOKS_ENABLED)
    ReadShrapnel_original(pF, pShrapnel_specs, pShrapnel_count);
#else
    char s[256];
    char s2[256];
    int i;
    int j;
    int k;

    /* FIXME: add documentation */

    *pShrapnel_count = GetAnInt(pF);

    for (i = 0; i < *pShrapnel_count; i++) {
        tShrapnel_spec *spec = &pShrapnel_specs[i];

        spec->type = GetALineAndInterpretCommand(pF, C2V(gShrapnel_type_names), REC2_ASIZE(C2V(gShrapnel_type_names)));
        if (spec->type == kShrapnelType_Abstract) {
            spec->type_info.abstract.count_materials = GetAnInt(pF);
            for (j = 0; j < spec->type_info.abstract.count_materials; j++) {

                GetAString(pF, s);
                spec->type_info.abstract.materials[j] = LoadTemporaryMaterial(s);
            }
            continue;
        }

        /* Min, max towards you speed */
        GetPairOfFloats(pF, &spec->min_speed_towards_you, &spec->max_speed_towards_you);
        /* Impactee velocity factor */
        spec->impactee_velocity_factor = GetAScalar(pF);
        /* Random velocity (max) */
        spec->random_velocity_max = GetAScalar(pF);
        /* Random up velocity (max) */
        spec->random_up_velocity_max = GetAScalar(pF);
        /* Random normal velocity (max) */
        spec->random_normal_velocity_max = GetAScalar(pF);
        /* Random spin rate (max) */
        spec->random_spin_rate_max = 0.1f * GetAScalar(pF);

        if (spec->type != kShrapnelType_Shards) {
            ReadSmashableInitialPosition(pF, &spec->initial_pos);
        }
        if (spec->type != kShrapnelType_NonCars) {
            float min_time, max_time;

            /* Min time, Max time */
            GetPairOfFloats(pF, &min_time, &max_time);
            spec->min_time_ms = 1000.f * min_time;
            spec->max_time_ms = 1000.f * max_time;
        }
        switch (spec->type) {
        case kShrapnelType_Shards:
            spec->type_info.shard.field_0x4 = GetAScalar(pF);
            spec->type_info.shard.field_0x8 = GetAnInt(pF);
            GetAString(pF, s);
            spec->type_info.shard.material = LoadTemporaryMaterial(s);
            spec->type_info.shard.material->flags = BR_MATF_TWO_SIDED;
            BrMatrix23Identity(&spec->type_info.shard.material->map_transform);
            BrMaterialUpdate(spec->type_info.shard.material, BR_MATU_ALL);
            break;

        case kShrapnelType_GhostParts:
            GetPairOfInts(pF, &spec->type_info.ghost.field_0x0, &spec->type_info.ghost.field_0x4);
            spec->type_info.ghost.count_actors = GetAnInt(pF);
            if (spec->type_info.ghost.count_actors > 0) {

                spec->type_info.ghost.actors = BrMemAllocate(spec->type_info.ghost.count_actors * sizeof(br_actor *), kMem_smashable_env_info);
                spec->type_info.ghost.bounds_dx = 0.f;
                spec->type_info.ghost.bounds_dy = 0.f;
                for (j = 0; j < spec->type_info.ghost.count_actors; j++) {

                    GetAString(pF, s);
                    PathCat(s2, C2V(gSmashable_track_environment_path), s);
                    spec->type_info.ghost.actors[j] = BrActorLoad(s2);
                    if (spec->type_info.ghost.actors[j] == NULL) {
                        FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                    }
                    ProcessMaterials(spec->type_info.ghost.actors[j], TemporaryMaterialStore);
                }
            } else {
                br_actor* actor;
                br_actor *actor_buffer[50];
                br_bounds bounds;

                GetAString(pF, s);
                PathCat(s2, C2V(gSmashable_track_environment_path), s);
                actor = BrActorLoad(s2);
                if (actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                ProcessMaterials(actor, TemporaryMaterialStore);

                BrActorToBounds(&bounds, actor);
                spec->type_info.ghost.bounds_dx = bounds.max.v[0] - bounds.min.v[0];
                spec->type_info.ghost.bounds_dy = bounds.max.v[1] - bounds.min.v[1];

                C2V(gShrapnel_ghost_actor_count) = 0;
                BrActorEnum(actor, AddGhostActorToBuffer, actor_buffer);
                spec->type_info.ghost.count_actors = C2V(gShrapnel_ghost_actor_count);
                spec->type_info.ghost.actors = BrMemAllocate(C2V(gShrapnel_ghost_actor_count) * sizeof(br_actor*), kMem_smashable_env_info);
                for (k = 0; k < spec->type_info.ghost.count_actors; k++) {
                    BrActorRemove(actor_buffer[k]);
                    spec->type_info.ghost.actors[k] = actor_buffer[k];
                }
                BrActorFree(actor);
            }
            break;

        case kShrapnelType_NonCars: {
            br_actor *actor;

            /* Min number,Max number (-1,-1 means use exactly one of each type of bit) */
            GetPairOfInts(pF, &spec->type_info.noncar.amount_min, &spec->type_info.noncar.amount_max);
            /* % Chance of fire/smoke */
            spec->type_info.noncar.probability_fire = GetAnInt(pF);
            if (spec->type_info.noncar.probability_fire != 0) {
                /* Number of fires/smoke columns */
                spec->type_info.noncar.count_fire_columns = GetAnInt(pF);
                /* Min,Max smokiness (0 = fire, 1 = black smoke, 2 = grey smoke, 3 = white smoke) */
                GetPairOfInts(pF, &spec->type_info.noncar.min_smokiness, &spec->type_info.noncar.max_smokiness);
            }
            /* Name of actor file */
            GetAString(pF, s);
            PathCat(s2, C2V(gSmashable_track_environment_path), s);
            actor = BrActorLoad(s2);
            if (actor == NULL) {
                FatalError(kFatalError_CannotFindSmashActorModel_S, s);
            }
            ProcessMaterials(actor, TemporaryMaterialStore);
            /* Number of separate actors in file */
            spec->type_info.noncar.count_actors = GetAnInt(pF);
            spec->type_info.noncar.actors = BrMemAllocate(spec->type_info.noncar.count_actors * sizeof(br_actor*), kMem_smashable_env_info);

            for (j = 0; j < spec->type_info.noncar.count_actors; j++) {
                br_actor *sep_actor;
                br_actor *match_sep_actor;

                /* Actor name */
                GetAString(pF, s);
                sep_actor = DRActorFindRecurse(actor, s);
                if (sep_actor == NULL) {
                    FatalError(kFatalError_CannotFindSmashActorModel_S, s);
                }
                match_sep_actor = sep_actor;
                for (k = 0; k < C2V(gCount_smashable_noncar_shrapnel_actors); k++) {

#if 1
                    /* FIXME: is this the correct version? */
                    if (c2_strcmp(C2V(gSmashable_noncar_shrapnel_actors)[k]->identifier, sep_actor->identifier) == 0) {
                        match_sep_actor = C2V(gSmashable_noncar_shrapnel_actors)[k];
                        break;
                    }
#else
                    if (c2_strcmp(C2V(gSmashable_noncar_shrapnel_actors)[j]->identifier, sep_actor->identifier) == 0) {
                        match_sep_actor = C2V(gSmashable_noncar_shrapnel_actors)[j];
                        break;
                    }
#endif
                }
                if (k == C2V(gCount_smashable_noncar_shrapnel_actors)) {
                    if (C2V(gCount_smashable_noncar_shrapnel_actors) < REC2_ASIZE(C2V(gSmashable_noncar_shrapnel_actors))) {
                        C2V(gSmashable_noncar_shrapnel_actors)[C2V(gCount_smashable_noncar_shrapnel_actors)] = match_sep_actor;
                        C2V(gCount_smashable_noncar_shrapnel_actors) += 1;
                    }
                    if (match_sep_actor != actor) {
                        BrActorRemove(match_sep_actor);
                    }
                }
                spec->type_info.noncar.actors[j] = match_sep_actor;
                GetAString(pF, s);
                for (k = 0; k < C2V(gCount_smashable_noncars); k++) {
                    if (c2_strcmp(C2V(gSmashable_noncars)[k], s) == 0) {
                        break;
                    }
                }
                if (k == C2V(gCount_smashable_noncars)) {
                    c2_strcpy(C2V(gSmashable_noncars)[C2V(gCount_smashable_noncars)], s);
                    C2V(gCount_smashable_noncars) += 1;
                }
                BrResFree(match_sep_actor->identifier);
                c2_sprintf(s2, "&%c%c!bbbb.ACT", s[0], s[1]);
                match_sep_actor->identifier = BrResStrDup(match_sep_actor, s2);
            }
            if (spec->type_info.noncar.count_actors != 1) {
                BrActorFree(actor);
            }
            if (spec->type_info.noncar.actors[0] != actor) {
                BrActorFree(actor);
            }
            break;
        }
        case kShrapnelType_Abstract:
            break;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004eed70, ReadShrapnelSpec, ReadShrapnel_original)

void (C2_HOOK_FASTCALL * ReadShrapnelSideEffects_original)(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects);
void C2_HOOK_FASTCALL ReadShrapnelSideEffects(FILE* pF, tShrapnel_side_effects* pShrapnel_side_effects) {
#if 0//defined(C2_HOOKS_ENABLED)
    ReadShrapnelSideEffects_original(pF, pShrapnel_side_effects);
#else

    /* Count smash activation cuboids */
    pShrapnel_side_effects->count_side_effects = GetAnInt(pF);
    if (pShrapnel_side_effects->count_side_effects != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tSmash_side_effect) != 0x4c);
#if 0
        /* FIXME: remove once we found something */
        PDFatalError("Non-zero non shash side effect detected! Notify rec2 developers!");
#endif
        pShrapnel_side_effects->side_effects = BrMemAllocate(pShrapnel_side_effects->count_side_effects * sizeof(tSmash_side_effect), kMem_smash_side_effects);
        for (i = 0; i < pShrapnel_side_effects->count_side_effects; i++) {
            int i1, i2;
            char s[256];

            GetPairOfInts(pF, &i1, &i2);
            pShrapnel_side_effects->side_effects[i].field_0x0 = 1000 * i1;
            pShrapnel_side_effects->side_effects[i].field_0x2 = 1000 * i2;

            GetAString(pF, s);
            c2_strcpy(pShrapnel_side_effects->side_effects[i].field_0x8, s);
            if (c2_strcmp(s, "*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            } else if (s[0] == '*' && DRStricmp(s + c2_strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 1;
            } else if (DRStricmp(s + c2_strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + c2_strlen(s) - 4, ".ACT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 2;
            } else if (DRStricmp(s, "&*") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 3;
            } else if (s[0] == '&' && '0' <= s[1] && s[1] <= '9') {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 6;
            } else if (DRStricmp(s + c2_strlen(s) - 4, ".MAT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 4;
            } else if (DRStricmp(s + c2_strlen(s) - 4, ".DAT") == 0 || DRStricmp(s + c2_strlen(s) - 4, ".ACT") == 0) {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 5;
            } else {
                pShrapnel_side_effects->side_effects[i].field_0x28 = 0;
            }
            pShrapnel_side_effects->side_effects[i].field_0x4 = GetALineAndInterpretCommand(pF, C2V(gPosition_type_names), REC2_ASIZE(C2V(gPosition_type_names)));
            LoadMinMax(pF, &pShrapnel_side_effects->side_effects[i].bounds);
            pShrapnel_side_effects->side_effects[i].field_0x44 = GetALineAndInterpretCommand(pF, C2V(gSmash_side_effect_direction_names), REC2_ASIZE(C2V(gSmash_side_effect_direction_names)));
            pShrapnel_side_effects->side_effects[i].field_0x48 = GetAScalar(pF);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ef550, ReadShrapnelSideEffects, ReadShrapnelSideEffects_original)

void (C2_HOOK_FASTCALL * ReadNonCarCuboidActivation_original)(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations);
void C2_HOOK_FASTCALL ReadNonCarCuboidActivation(FILE* pF, tNon_car_cuboid_activations* pNon_car_cuboid_activations) {
#if 0//defined(C2_HOOKS_ENABLED)
    ReadNonCarCuboidActivation_original(pF, pNon_car_cuboid_activations);
#else

    /* no. of non car cuboids activated */
    pNon_car_cuboid_activations->count_activations = GetAnInt(pF);
    if (pNon_car_cuboid_activations->count_activations != 0) {
        int i;

        C2_HOOK_BUG_ON(sizeof(tNon_car_cuboid_activation) != 0x40);

#if 1
        /* FIXME: remove once we found something */
        PDFatalError("Non-zero non car cuboid activation detected! Notify rec2 developers!");
#endif
        pNon_car_cuboid_activations->activations = BrMemAllocate(pNon_car_cuboid_activations->count_activations * sizeof(tNon_car_cuboid_activation), kMem_smash_side_effects);
        for (i = 0; i < pNon_car_cuboid_activations->count_activations; i++) {
            int i1, i2;

            GetPairOfInts(pF, &i1, &i2);
            pNon_car_cuboid_activations->activations[i].field_0x0 = (tS16)(i1 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x2 = (tS16)(i2 * 1000);
            pNon_car_cuboid_activations->activations[i].field_0x4 = GetALineAndInterpretCommand(pF, C2V(gPosition_type_names), REC2_ASIZE(C2V(gPosition_type_names)));
            pNon_car_cuboid_activations->activations[i].field_0x8 = (tS8)GetAnInt(pF);
            LoadMinMax(pF, &pNon_car_cuboid_activations->activations[i].bounds);
            GetPairOfFloats(pF,
                &pNon_car_cuboid_activations->activations[i].field_0x24,
                &pNon_car_cuboid_activations->activations[i].field_0x28);
            pNon_car_cuboid_activations->activations[i].field_0x2c = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x30 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x34 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x38 = GetAScalar(pF);
            pNon_car_cuboid_activations->activations[i].field_0x3c = GetAScalar(pF) / 10.f;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efce0, ReadNonCarCuboidActivation, ReadNonCarCuboidActivation_original)
