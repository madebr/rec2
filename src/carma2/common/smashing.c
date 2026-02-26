#include "smashing.h"

#include "car.h"
#include "crush.h"
#include "explosions.h"
#include "globvars.h"
#include "loading.h"
#include "physics.h"
#include "piping.h"
#include "platform.h"
#include "powerups.h"
#include "replay.h"
#include "sound.h"
#include "utility.h"
#include "world.h"

#include <brender/brender.h>

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_vertex, gSmash_glass_fragments, 200, 0x006b78e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmash_quad, gSmash_decals, 50, 0x006a80f8);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gZero_vector__smash, 0x006abee8);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gInitial_smashable_position_type_names, 3, 0x0065fed0, {
    "sphereclumped",
    "boxclumped",
    "actorbased",
});

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gInitial_position_sphere_where, 2, 0x0065fee0, {
    "impact",
    "model",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSmashable_race_target, gSmashable_race_targets, 300, 0x0068c898);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_smashable_race_targets, 0x0074abe0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_queued_smashes, 0x006a828c);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_munge_smash_edge_triggers, 0x006a82a4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tPowerup_queue_item, gPowerup_queue, 50, 0x006a4430);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tQueued_smash, gQueued_smashes, 20, 0x006a4698);
C2_HOOK_VARIABLE_IMPLEMENT(br_matrix34, gMatrix34_006b78a0, 0x006b78a0);
C2_HOOK_VARIABLE_IMPLEMENT(br_vector3, gVector3_006a4688, 0x006a4688);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006a3334, 0x006a3334);

void C2_HOOK_FASTCALL InitGlassFragments(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSmash_glass_fragments)) != 200);

    for (i = 0; i < REC2_ASIZE(C2V(gSmash_glass_fragments)); i++) {
        tSmash_vertex *smash_vertex;

        smash_vertex = &C2V(gSmash_glass_fragments)[i];
        smash_vertex->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_vertex->actor->model = BrModelAllocate(NULL, 3, 1);
        smash_vertex->actor->model->faces[0].vertices[0] = 0;
        smash_vertex->actor->model->faces[0].vertices[1] = 1;
        smash_vertex->actor->model->faces[0].vertices[2] = 2;
        smash_vertex->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_vertex->actor->model);
        smash_vertex->model = smash_vertex->actor->model;
        smash_vertex->actor->render_style = BR_RSTYLE_FACES;
        smash_vertex->field_0x8 = 0;
    }
}

void C2_HOOK_FASTCALL InitDecals(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gSmash_decals)) != 50);

    for (i = 0; i < REC2_ASIZE(C2V(gSmash_decals)); i++) {
        tSmash_quad *smash_quad;

        smash_quad = &C2V(gSmash_decals)[i];
        smash_quad->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        smash_quad->actor->model = BrModelAllocate(NULL, 4, 2);
        smash_quad->actor->model->faces[0].vertices[0] = 0;
        smash_quad->actor->model->faces[0].vertices[1] = 1;
        smash_quad->actor->model->faces[0].vertices[2] = 3;
        smash_quad->actor->model->faces[1].vertices[0] = 0;
        smash_quad->actor->model->faces[1].vertices[1] = 3;
        smash_quad->actor->model->faces[1].vertices[2] = 2;
        smash_quad->actor->model->flags |= BR_MODF_UPDATEABLE;
        BrModelAdd(smash_quad->actor->model);
        smash_quad->actor->render_style = BR_RSTYLE_FACES;
        smash_quad->field_0x4 = 0;
    }
}

void (C2_HOOK_FASTCALL * InitSmashing_original)(void);
void C2_HOOK_FASTCALL InitSmashing(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitSmashing_original();
#else

    InitGlassFragments();
    InitDecals();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004efe00, InitSmashing, InitSmashing_original)

void C2_HOOK_FASTCALL ReadSmashableInitialPosition(FILE* pFile, tSmashable_initial_position_spec*  pInitial_pos) {

    /* Initial position type */
    pInitial_pos->type = GetALineAndInterpretCommand(pFile, C2V(gInitial_smashable_position_type_names), REC2_ASIZE(C2V(gInitial_smashable_position_type_names)));

    switch (pInitial_pos->type) {
        case kInitialSmashablePosition_SphereClumped:
            /* Sphere radius */
            pInitial_pos->position.sphere.radius = GetAScalar(pFile);
            /* Sphere centre */
            pInitial_pos->position.sphere.where = GetALineAndInterpretCommand(pFile, C2V(gInitial_position_sphere_where), REC2_ASIZE(C2V(gInitial_position_sphere_where)));
            break;
        case kInitialSmashablePosition_BoxClumped:
            GetThreeFloats(pFile, &pInitial_pos->position.box.v[0], &pInitial_pos->position.box.v[1], &pInitial_pos->position.box.v[2]);
            break;
        default:
            break;
    }
}
C2_HOOK_FUNCTION(0x004ee550, ReadSmashableInitialPosition)

void C2_HOOK_FASTCALL ReadSmashableInitialSpeed(FILE* pFile, tSmashable_initial_speed_spec* pInitial_speed) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_initial_speed_spec) != 28);

    /* Min,max speed towards car */
    GetPairOfFloats(pFile, &pInitial_speed->min_speed, &pInitial_speed->max_speed);
    /* Impactee velocity factor */
    pInitial_speed->impactee_velocity_factor = GetAScalar(pFile);
    /* Random velocity (max) */
    pInitial_speed->random_velocity_max = GetAScalar(pFile);
    /* Random up velocity (max) */
    pInitial_speed->random_up_velocity_max = GetAScalar(pFile);
    /* Random normal velocity (max) */
    pInitial_speed->random_normal_velocity_max = GetAScalar(pFile);
    /* Random spin rate (max) */
    pInitial_speed->random_spin_rate_max = GetAScalar(pFile);
}
C2_HOOK_FUNCTION(0x004ee500, ReadSmashableInitialSpeed)

void C2_HOOK_FASTCALL ReadMinMaxTimeInMilliseconds(FILE* pFile, int* pTimes) {
    float f1;
    float f2;

    /* Min time, Max time */
    GetPairOfFloats(pFile, &f1, &f2);
    pTimes[0] = (int)(1000.f * f1);
    pTimes[1] = (int)(1000.f * f2);
}
C2_HOOK_FUNCTION(0x004ee5a0, ReadMinMaxTimeInMilliseconds)

void C2_HOOK_FASTCALL InitSmashTargets(void) {

    C2V(gCount_smashable_race_targets) = 0;
}
C2_HOOK_FUNCTION(0x004977e0, InitSmashTargets)

void C2_HOOK_FASTCALL AddSmashableRaceTarget(br_model* pModel, br_actor* pActor, int pUnknown) {

    C2_HOOK_BUG_ON(sizeof(tSmashable_race_target) != 12);
    if (C2V(gCount_smashable_race_targets) < REC2_ASIZE(C2V(gSmashable_race_targets))) {
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].model = pModel;
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].actor = pActor;
        C2V(gSmashable_race_targets)[C2V(gCount_smashable_race_targets)].field_0x8 = pUnknown;
        C2V(gCount_smashable_race_targets)++;
    }
}
C2_HOOK_FUNCTION(0x004977f0, AddSmashableRaceTarget)

void C2_HOOK_FASTCALL SplondificatalizeIdentifier(br_material* pMaterial, char** ppIdentifier, int pIndex) {
    int len;
    char original_suffix[64];
    char new_identifier[64];
    char* suffix_ptr;

    c2_strcpy(new_identifier, *ppIdentifier);
    len = c2_strlen(new_identifier);
    if (len < 4) {
        c2_sprintf(new_identifier, "Smash material %s has a name that is less than 4 characters long", *ppIdentifier);
        PDFatalError(new_identifier);
    }
    suffix_ptr = new_identifier + len - 4;
    c2_strcpy(original_suffix, suffix_ptr);
    c2_sprintf(suffix_ptr, "        ");
    c2_sprintf(&new_identifier[5], "%cx", '|');
    new_identifier[6] = (char)(pIndex + 1);
    c2_strcat(new_identifier, original_suffix);
    if (c2_strlen(*ppIdentifier) < 12) {
        BrResFree(*ppIdentifier);
        *ppIdentifier = BrResStrDup(pMaterial, new_identifier);
    } else {
        c2_strcpy(*ppIdentifier, new_identifier);
    }
}

void (C2_HOOK_FASTCALL * MungeSmashMaterialNames_original)(void);
void C2_HOOK_FASTCALL MungeSmashMaterialNames(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    PrepareSmashableTrackItemIdentifiers_original();
#else
    int i;

    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 0x2e0);

    for (i = 0; i < C2V(gCount_track_smashable_environment_specs); i++) {
        tSmashable_item_spec* item_spec = &C2V(gTrack_smashable_environment_specs)[i];

        if (item_spec->trigger_type == kSmashableTrigger_Material) {
            SplondificatalizeIdentifier(
                item_spec->trigger_object.material,
                &item_spec->trigger_object.material->identifier,
                i);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f0bc0, MungeSmashMaterialNames, MungeSmashMaterialNames_original)

void (C2_HOOK_FASTCALL * CleanUpSmashStuff_original)(void);
void C2_HOOK_FASTCALL CleanUpSmashStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    CleanUpSmashStuff_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f02b0, CleanUpSmashStuff,CleanUpSmashStuff_original)

void (C2_HOOK_FASTCALL * MungeGlassFragments_original)(void);
void C2_HOOK_FASTCALL MungeGlassFragments(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeGlassFragments_original();
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f00f0, MungeGlassFragments,MungeGlassFragments_original)

void C2_HOOK_FASTCALL FlushPowerupQueue(void) {
    int i;
    C2_HOOK_BUG_ON(sizeof(tPowerup_queue_item) != 0xc);

    for (i = 0; i < C2V(gSize_powerup_queue); i++) {
        GotPowerup(C2V(gPowerup_queue)[i].car, C2V(gPowerup_queue)[i].powerup_index);
        MayQueuePowerupRespawn(C2V(gPowerup_queue)[i].powerup_index, C2V(gPowerup_queue)[i].actor);
    }
    C2V(gSize_powerup_queue) = 0;
}

void C2_HOOK_FASTCALL SmashItIntoVerySmallPiecesIndeed(tCar_spec* pCar, undefined4 pArg2, br_actor* pActor, undefined4 pArg4, undefined4 pArg5, void* pArg6, void* pArg7, undefined4 pArg8) {
    NOT_IMPLEMENTED();
}

void (C2_HOOK_FASTCALL * FlushSmashQueue_original)(int pFlush_powerups);
void C2_HOOK_FASTCALL FlushSmashQueue(int pFlush_powerups) {

#if defined(C2_HOOKS_ENABLED)
    FlushSmashQueue_original(pFlush_powerups);
#else
    int i;
    int j;
    tQueued_smash* queued_smash;

    C2_HOOK_BUG_ON(sizeof(tQueued_smash) != 0x88);

    for (i = 0; i < C2V(gCount_queued_smashes); i++) {
        queued_smash = &C2V(gQueued_smashes)[i];
        if (queued_smash->actor->model != NULL) {
            if (queued_smash->field_0x40 != 0) {
                C2V(gMatrix34_006b78a0) = queued_smash->field_0x4c;
                BrVector3Copy(&C2V(gVector3_006a4688), &queued_smash->field_0x7c);
            }
            C2V(gINT_006a3334) = queued_smash->field_0x40 != 0;
            SmashItIntoVerySmallPiecesIndeed(
                    queued_smash->car, queued_smash->field_0x4, queued_smash->actor,
                    queued_smash->field_0x48, queued_smash->field_0x10, &queued_smash->field_0x14,
                    &queued_smash->field_0x2c, queued_smash->field_0x38);
        }
    }
    for (i = 0; i < C2V(gCount_queued_smashes); i++) {
        queued_smash = &C2V(gQueued_smashes)[i];

        if (queued_smash->field_0x44) {
            DRModelUpdateAndKevificateMaterials(queued_smash->actor->model, BR_MODU_ALL);
            for (j = i + 1; j < C2V(gCount_queued_smashes); j++) {
                if (queued_smash->actor->model == C2V(gQueued_smashes)[j].actor->model) {
                    queued_smash->field_0x44 = 0;
                }
            }
        }
    }
    if (!pFlush_powerups) {
        FlushPowerupQueue();
    }
    C2V(gCount_queued_smashes) = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ecfb0, FlushSmashQueue, FlushSmashQueue_original)

void C2_HOOK_FASTCALL DoDelayedNonCar(tU32 pTime, tDelayed_non_car* pDelayed_non_car) {
    int i;
    int number_started;
    tDelayed_non_car_actor_item* read_action;
    tDelayed_non_car_actor_item* write_action;

    read_action = pDelayed_non_car->actor_time_actions;
    write_action = read_action;
    number_started = 0;

    for (i = 0; i < pDelayed_non_car->count_actions; read_action++, i++) {
        if (pTime < read_action->time) {
            *write_action++ = *read_action;
        } else if (read_action->actor->type_data == NULL) {
            tNon_car_spec *non_car;

            non_car = DoPullActorFromWorld(read_action->actor);
            if (non_car != NULL) {
                if (non_car->flags & 0x10000) {
                    MakeLiftGoUp(non_car);
                } else {
                    br_vector3 speed;
                    br_vector3 omega;

                    ApplyInitialMovement(pDelayed_non_car->field_0x18, &pDelayed_non_car->field_0x0, &speed, &omega, 1.f, &C2V(gZero_vector__smash),
                             &pDelayed_non_car->field_0xc, &read_action->actor->t.t.translate.t);
                    BrVector3InvScale(&non_car->collision_info->v, &speed, WORLD_SCALE);
                    BrVector3Scale(&non_car->collision_info->omega, &omega, .0958738f);
                }
                non_car->collision_info->disable_move_rotate = 0;
            }
            number_started += 1;
        }
    }
    pDelayed_non_car->count_actions -= number_started;
}

void C2_HOOK_FASTCALL DoDelayedSmash(tDelayed_smash* pDelayed_smash) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0x1c, 0x1c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0x38, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0x80, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0x8c, 0x8c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDelayes_smash, field_0x90, 0x90);

    if (pDelayed_smash->field_0x88 == 1) {
        if (pDelayed_smash->field_0x80->identifier[0] == '&' && (pDelayed_smash->field_0x80->identifier[1] >= '0' && pDelayed_smash->field_0x80->identifier[1] <= '9')) {
            DoPullActorFromWorld(pDelayed_smash->field_0x80);
            pDelayed_smash->field_0x80->identifier[3] = '!';
            BashObject(
                pDelayed_smash->field_0x90, pDelayed_smash->field_0x80,
                pDelayed_smash->field_0x8c, &pDelayed_smash->field_0xc,
                &pDelayed_smash->field_0x0, &pDelayed_smash->field_0x1c,
                1, 0);
            return;
        }
    }
    SmashEnvironment(
        pDelayed_smash->field_0x90, &pDelayed_smash->field_0x38,
        pDelayed_smash->field_0x8c, &pDelayed_smash->field_0xc,
        &pDelayed_smash->field_0x0, &pDelayed_smash->field_0x1c,
        1, 0);
}

void (C2_HOOK_FASTCALL * MungeDelayedSideEffects_original)(void);
void C2_HOOK_FASTCALL MungeDelayedSideEffects(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeDelayedSideEffects_original();
#else
    int i;
    tU32 the_time;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_explosion, what.non_car, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmash_explosion, what.non_car.count_actions, 0x24);

    the_time = PDGetTotalTime();
    for (i = 0; i < REC2_ASIZE(C2V(gSmash_explosions)); i++) {
        tSmash_explosion* delayed_effect = &C2V(gSmash_explosions)[i];

        if (delayed_effect->active) {
            switch (delayed_effect->type) {
            case 0:
                DoDelayedNonCar(the_time, &delayed_effect->what.non_car);
                if (delayed_effect->what.non_car.count_actions == 0) {
                    delayed_effect->active = 0;
                }
                break;
            case 1:
                if (the_time >= delayed_effect->what.smash.field_0x18) {
                    DoDelayedSmash(&delayed_effect->what.smash);
                    delayed_effect->active = 0;
                }
                break;
            }
        }

    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ecc80, MungeDelayedSideEffects, MungeDelayedSideEffects_original)

void (C2_HOOK_FASTCALL * DoPowerupRespawnSmash_original)(br_actor *pActor);
void C2_HOOK_FASTCALL DoPowerupRespawnSmash(br_actor *pActor) {

#if defined(C2_HOOKS_ENABLED)
    DoPowerupRespawnSmash_original(pActor);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ecea0, DoPowerupRespawnSmash, DoPowerupRespawnSmash_original)

void (C2_HOOK_FASTCALL * MungeSmashEdgeTriggers_original)(tU32 pTime);
void C2_HOOK_FASTCALL MungeSmashEdgeTriggers(tU32 pTime) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeSmashEdgeTriggers_original(pTime);
#else

    if (C2V(gLast_munge_smash_edge_triggers) != C2V(gPHIL_last_physics_tick)) {
        int i;

        for (i = 0; i < C2V(gCount_track_smashable_environment_specs); i++) {
            tSmashable_item_spec* item;

            item = &C2V(gTrack_smashable_environment_specs)[i];
            if (item->field_0x10 != 0 && item->field_0x10 != pTime) {
                C2V(gTrack_smashable_environment_specs)[i].field_0x10 = 0;
            }
        }
    }
    C2V(gLast_munge_smash_edge_triggers) = C2V(gPHIL_last_physics_tick);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004f64d0, MungeSmashEdgeTriggers, MungeSmashEdgeTriggers_original)

void C2_HOOK_FASTCALL InitSmashQueue(void) {

    C2V(gCount_queued_smashes) = 0;
}
C2_HOOK_FUNCTION(0x004ecfa0, InitSmashQueue)

void C2_HOOK_FASTCALL MungeInternalCarGlass(tCar_spec* pCar_spec) {

    if (pCar_spec == &C2V(gProgram_state).current_car && C2V(gAction_replay_camera_mode) == kActionReplayCameraMode_Internal) {
        MungeCarMaterials(pCar_spec, 1);
    }
}

void C2_HOOK_FASTCALL ActuallyRepairSmash(tCar_spec* pCar_spec, tCar_crush_smashable_part* pSmashable, int pLevel) {
    br_pixelmap* texture;

    pSmashable->field_0x4c = pLevel;
    texture = pSmashable->levels[pLevel].pixelmaps[IRandomBetween(0, pSmashable->levels[pLevel].count_pixelmaps - 1)];
    PipeSingleSmashTextureChange(pCar_spec, pSmashable->funk_material, texture);
    pSmashable->funk_material->user = texture;
    if (pCar_spec->field_0x1960 == NULL) {
        pSmashable->funk_material->colour_map = texture;
    }
    BrMaterialUpdate(pSmashable->funk_material, BR_MATU_COLOURMAP);
    MungeInternalCarGlass(pCar_spec);
    if (pLevel == 0 && pSmashable->funk >= 0) {
        EnableFunkotronic(pSmashable->funk);
    }
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && C2V(gProgram_state).racing) {
        DRS3StartSound(C2V(gCar_outlet),  eSoundId_SmashRepair);
    }
}
C2_HOOK_FUNCTION(0x004ef840, ActuallyRepairSmash)

void C2_HOOK_FASTCALL TotallyRepairSmash(tCar_spec *pCar_Spec, tCar_crush_buffer_entry *pSmash_data) {
    int i;

    for (i = 0; i < pSmash_data->count_smashables; i++) {
        tCar_crush_smashable_part* smashable;

        smashable = &pSmash_data->smashables[i];
        if (smashable->field_0x4c) {
            ActuallyRepairSmash(pCar_Spec, smashable, 0);
        }
    }
}
C2_HOOK_FUNCTION(0x004ef9c0, TotallyRepairSmash)

void (C2_HOOK_FASTCALL * ApplyInitialMovement_original)(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8);
void C2_HOOK_FASTCALL ApplyInitialMovement(undefined4* pArg1, br_vector3* pArg2, br_vector3* pArg3, br_vector3* pArg4, float pArg5, br_vector3* pArg6, br_vector3* pArg7, br_vector3* pArg8) {

#if defined(C2_HOOKS_ENABLED)
    ApplyInitialMovement_original(pArg1, pArg2, pArg3, pArg4, pActor, pArg6, pArg7, pArg8, pArg9);
#else
    NOT_IMPLEMENTED();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x, ApplyInitialMovement, ApplyInitialMovement_original)
