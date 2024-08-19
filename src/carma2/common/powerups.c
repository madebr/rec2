#include "powerups.h"

#include "animation.h"
#include "crush.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "loading.h"
#include "network.h"
#include "physics.h"
#include "platform.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_string.h"
#include "rec2_types.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tHeadup_icon, gPickedup_powerups, 5, 0x007059e0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gRepeatability_names, 4, 0x0065feb0, {
    "none",
    "singleshot",
    "repeated",
    "doitregardless",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gFizzle_names, 3, 0x0065ebb0, {
    "CIRCLES",
    "SQUARES",
    "DIAMONDS",
});
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_item_spec, gPowerup_pickup_smashable, 0x006a52d0);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_item_spec, gPowerup_respawn_smashable, 0x006a7ce0);
C2_HOOK_VARIABLE_IMPLEMENT(tSmashable_item_spec, gUnknown_smashable_006a3660, 0x006a3660);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gFizzle_in, 3, 0x006a0ac0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFizzle_height, 0x006a0950);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_powerups, 0x006a0ad0);
C2_HOOK_VARIABLE_IMPLEMENT(tPowerup*, gPowerup_array, 0x006a0a54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNumber_of_powerup_respawn_specs, 0x006a0948);
C2_HOOK_VARIABLE_IMPLEMENT(tPowerup_respawn_spec*, gPowerup_respawn_specs, 0x006a0a64);
C2_HOOK_VARIABLE_IMPLEMENT(tU8*, gRace_powerup_respawn_bools, 0x006a0a50);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_powerup_time_between_goodies_min, 0x006a0a60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_powerup_time_between_goodies_max, 0x006a090c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_count_powerup_goodies, 0x006a0450);
C2_HOOK_VARIABLE_IMPLEMENT(int*, gNet_powerup_goodies, 0x006a094c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tGot_proc*, gGot_procs, 56, 0x0065e910, {
    GotCredits,
    SetPedSpeed,
    SetPedSize,
    SetPedExplode,
    PickAtRandom,
    SetInvulnerability,
    SetFreeRepairs,
    DoInstantRepair,
    FreezeTimer,
    SetEngineFactor,
    SetUnderwater,
    GotTimeOrPower,
    TrashBodywork,
    TakeDrugs,
    SetOpponentsSpeed,
    SetCopsSpeed,
    SetGravity,
    SetPinball,
    SetWallclimb,
    SetBouncey,
    SetSuspension,
    SetTyreGrip,
    SetDamageMultiplier,
    SetHades,
    SetBlindPedestrians,
    SuicidalPedestrians,
    GotVouchers,
    SetMassMultiplier,
    SetInstantHandbrake,
    ShowPedestrians,
    HitMine,
    SetProximity,
    SetPedestrianStupidHead,
    SetPedestrianMutantCorpses,
    GetMineShittingAbility,
    GetOilSlickShittingAbility,
    GetKangoorooOnCommand,
    GetPedestrianAnnihilator,
    GetOpponentRespulsificator,
    SetPedestrianDismemberfest,
    SetEtherealPedestrians,
    SetGroovinPedestrians,
    SetPedestrianPanic,
    SetHeliumFilledPedestrians,
    SetDrunkPedestrians,
    GetPedestrianRepulsificator,
    GetExtraAPO,
    GetExtraAPOSlot,
    SetDrunkDriving,
    GetPedestrianFlameThrower,
    SetPedestrianValium,
    CancelPowerups,
    SetMutantTail,
    SeverTail,
    GotCloak,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tLose_proc*, gLose_procs, 56, 0x0065e9f0, {
    NULL,
    ResetPedSpeed,
    ResetPedSize,
    ResetPedExplode,
    NULL,
    ResetInvulnerability,
    ResetFreeRepairs,
    NULL,
    UnfreezeTimer,
    ResetEngineFactor,
    ResetUnderwater,
    NULL,
    NULL,
    PukeDrugsBackUp,
    ResetOpponentsSpeed,
    ResetCopsSpeed,
    ResetGravity,
    ResetPinball,
    ResetWallclimb,
    ResetBouncey,
    ResetSuspension,
    ResetTyreGrip,
    ResetDamageMultiplier,
    ResetHades,
    ResetBlindPedestrians,
    ResetSuicidalPedestrians,
    NULL,
    ResetMassMultiplier,
    ResetInstantHandbrake,
    HidePedestrians,
    NULL,
    ResetProximity,
    ResetPedestrianStupidHead,
    ResetPedestrianMutantCorpses,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ResetDismemberfest,
    ResetEtherealPedestrians,
    ResetGroovinPedestrians,
    ResetPedestrianPanic,
    ResetHeliumFilledPedestrians,
    ResetDrunkPedestrians,
    NULL,
    NULL,
    NULL,
    ResetDrunkDriving,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ResetCloak,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tPeriodic_proc*, gPeriodic_procs, 56, 0x0065ead0, {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    TheEffectsOfDrugs,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DoBouncey,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MungeVouchers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    DriveAroundDrunk,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tShit_mine, gShit_mines, 20, 0x00705560);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tRepulse_link, gRepulse_links, 6, 0x006a0960);
C2_HOOK_VARIABLE_IMPLEMENT(tPhysics_joint*, gMutant_tail_first_joint, 0x006a0acc);
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gMutant_tail_first_collision_info, 0x006a0908);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMutant_tail_state, 0x00705540);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0074a5ec, 0x0074a5ec);


void C2_HOOK_FASTCALL InitRepulseEffects(void) {
    br_pixelmap* repulse;
    int i;
    int j;

    repulse = LoadPixelmap("REPULSE.PIX");
    if (repulse == NULL) {
        FatalError(kFatalError_CantFindFile_S, "REPULSE.PIX");
    }
    BrMapAdd(repulse);

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gRepulse_links)) != 6);
    C2_HOOK_BUG_ON(sizeof(tRepulse_link) != 0x28);

    for (i = 0; i < REC2_ASIZE(C2V(gRepulse_links)); i++) {
        tRepulse_link *link = &C2V(gRepulse_links)[i];
        br_model *model;
        br_material *material;

        link->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        model = BrModelAllocate(NULL, 102, 100);
        model->flags |= BR_MODF_UPDATEABLE;
        material = BrMaterialAllocate("");
        material->colour_map = repulse;
        material->flags &= ~BR_MATF_LIGHT;
        material->flags |= BR_MATF_TWO_SIDED;
        BrMaterialAdd(material);
        BlendifyMaterial(material, 100);
        BrMaterialUpdate(material, BR_MATU_ALL);
        for (j = 0; j < model->nfaces / 2; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 0;
            model->faces[j].vertices[1] = j + 1;
            model->faces[j].vertices[2] = j + 1 + model->nvertices / 2;
        }
        for (j = model->nfaces / 2; j < model->nfaces; j++) {
            model->faces[j].material = material;
            model->faces[j].vertices[0] = j + 1;
            model->faces[j].vertices[1] = j + 2;
            model->faces[j].vertices[2] = j + 1 - model->nvertices / 2;
        }
        for (j = 0; j < model->nvertices / 2; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 0.f;
        }
        for (j = model->nvertices / 2; j < model->nvertices; j++) {
            model->vertices[j].p.v[0] = (float) (j % 2);
            model->vertices[j].p.v[1] = 1.f;
        }
        link->actor->model = model;
        link->actor->render_style = BR_RSTYLE_FACES;
        link->material = material;
        BrModelAdd(model);
    }
}

void C2_HOOK_FASTCALL InitTail(void) {
    int i;
    tCollision_info* parent_collision_info;
    tPath_name the_path;
    tTWTVFS twt;

    parent_collision_info = NULL;
    PathCat(the_path, C2V(gApplication_path), "TAIL");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&C2V(gMisc_storage_space), the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);
    for (i = 0; i < C2V(gCount_mutant_tail_parts); i++) {
        int is_link;
        tCollision_info *collision_info;
        br_model *model;

        is_link = i != C2V(gCount_mutant_tail_parts) - 1;
        if (is_link) {
            model = BrModelFind("TAILLINK");
            collision_info = MungeBoxObject(model REC2_THISCALL_EDX, C2V(gMass_mutant_tail_link));
        } else {
            model = BrModelFind("TAILBALL");
            collision_info = MungeSphereObject(model REC2_THISCALL_EDX, C2V(gMass_mutant_tail_ball));
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, physics_joint1, 0x188);

        collision_info->flags_0x238 = 0x20;
        collision_info->physics_joint1 = AllocatePhysicsJoint(1, kMem_physics_joint);
        collision_info->physics_joint1->type = eJoint_ball_n_socket;
        BrVector3SetFloat(&collision_info->physics_joint1->field_0x08,
                          0.f, 0.f, model->bounds.min.v[2] * (is_link ? 0.99f : 0.45f));
        if (i == 0) {
            C2V(gMutant_tail_first_joint) = collision_info->physics_joint1;
            C2V(gMutant_tail_first_collision_info) = collision_info;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, type, 0x00);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x04);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, field_0x08, 0x08);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis2, 0x20);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis3, 0x2c);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, hinge_axis, 0x38);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, count_limits, 0x50);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, limits, 0x54);

            collision_info->physics_joint1->count_limits = 1;
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].child, 0.f, 0.f, 1.f);
            BrVector3SetFloat(&collision_info->physics_joint1->limits[0].parent, 0.f, 0.f, 1.f);
        } else {
            BrVector3SetFloat(&collision_info->physics_joint1->field_0x14,
                              0.f, 0.f, model->bounds.min.v[2] * (is_link ? 0.99f : 0.45f));
            PhysicsAddObject(parent_collision_info, collision_info);
        }
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis2, 0.f, 0.f, 1.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis3, 0.f, 1.f, 0.f);
        BrVector3SetFloat(&collision_info->physics_joint1->hinge_axis, 1.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
        parent_collision_info = collision_info;
    }
    C2V(gMutant_tail_state) = 0;
}

void (C2_HOOK_FASTCALL * InitPowerups_original)(void);
void C2_HOOK_FASTCALL InitPowerups(void) {

#if 0// defined(C2_HOOKS_ENABLED)
    InitPowerups_original();
#else

    InitRepulseEffects();
    InitTail();
    InitMineShit();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d9ea0, InitPowerups, InitPowerups_original)

void (C2_HOOK_FASTCALL * ResetPowerups_original)(void);
void C2_HOOK_FASTCALL ResetPowerups(void) {

#if defined(C2_HOOKS_ENABLED)
    ResetPowerups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da630, ResetPowerups, ResetPowerups_original)

br_actor* (C2_HOOK_FASTCALL * CreateActorFromPowerupMap_original)(br_pixelmap* pMap);
br_actor* C2_HOOK_FASTCALL CreateBillBoard(br_pixelmap* pMap) {

#if 0//defined(C2_HOOKS_ENABLED)
    return CreateActorFromPowerupMap_original(pMap);
#else
    br_actor* actor;
    br_material* material;
    br_model* model;
    br_scalar half_width;
    br_scalar half_height;

    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 2;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 0;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    half_width = (float)(pMap->width / 2);
    half_height = (float)(pMap->height / 2);
    BrVector3Set(&model->vertices[0].p, -half_width,  half_height, -2.f);
    BrVector3Set(&model->vertices[1].p, -half_width, -half_height, -2.f);
    BrVector3Set(&model->vertices[2].p,  half_width, -half_height, -2.f);
    BrVector3Set(&model->vertices[3].p,  half_width,  half_height, -2.f);
    BrVector2Set(&model->vertices[0].map, 0.f, 0.f);
    BrVector2Set(&model->vertices[1].map, 0.f, 1.f);
    BrVector2Set(&model->vertices[2].map, 1.f, 1.f);
    BrVector2Set(&model->vertices[3].map, 1.f, 0.f);
    material->colour = 0;
    material->colour_map = pMap;
    material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(pMap);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e09d0, CreateBillBoard, CreateActorFromPowerupMap_original)

static br_pixelmap* DRMapFindMeAPixie(const char* name) {
    char s[256];
    br_pixelmap* map;

    map = BrMapFind(name);
    if (map != NULL) {
        return map;
    }
    c2_strcpy(s, name);
    c2_strcat(s, ".PIX");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    c2_strcpy(s, name);
    c2_strcat(s, ".pix");
    map = BrMapFind(s);
    if (map != NULL) {
        return map;
    }
    return NULL;
}

void (C2_HOOK_FASTCALL * LoadPowerups_original)(void);
void C2_HOOK_FASTCALL LoadPowerups(void) {

    C2_HOOK_BUG_ON(sizeof(tPowerup) != 172);

#if 0 //defined(C2_HOOKS_ENABLED)
    LoadPowerups_original();
#else
    FILE* f;
    tPath_name the_path;
    tTWTVFS twtvfs;
    int action_index;
    int first;
    int i, j, k;
    int time;
    int count_index_groups;
    int index_range;
    char s[256];
    tPowerup* the_powerup;

    if (C2V(gPowerup_txt_path) == NULL) {
        SetDefaultPowerupFilename();
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, "POWERUPS");
    twtvfs = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllImagesInDirectory(&C2V(gMisc_storage_space), the_path);
    ClosePackFileAndSetTiffLoading(twtvfs);

    for (i = 0; i < REC2_ASIZE(C2V(gFizzle_in)); i++) {
        C2V(gFizzle_in)[i] = DRMapFindMeAPixie(C2V(gFizzle_names)[i]);
    }
    C2V(gFizzle_height) = C2V(gFizzle_in)[0]->height / 4;

    PathCat(the_path, C2V(gApplication_path), C2V(gPowerup_txt_path));
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }
    /* Number of powerups */
    C2V(gNumber_of_powerups) = GetAnInt(f);
    C2V(gPowerup_array) = BrMemAllocate(sizeof(tPowerup) * C2V(gNumber_of_powerups), kMem_powerup_array);
    for (i = 0; i < C2V(gNumber_of_powerups); i++) {
        the_powerup = &C2V(gPowerup_array)[i];

        GetALineAndDontArgue(f, the_powerup->message);
        if (c2_strcmp(the_powerup->message, "dummy") == 0) {
            the_powerup->type = ePowerup_dummy;
        } else {
            if (c2_strcmp(the_powerup->message, "n/a") == 0) {
                the_powerup->message[0] = '\0';
            }
            GetAString(f, s);
            the_powerup->icon = DRMapFindMeAPixie(s);
            if (the_powerup->icon != NULL) {
                BrMapAdd(the_powerup->icon);
                BRPM_convert(the_powerup->icon, C2V(gBack_screen)->type);
            }
            if (the_powerup->icon != NULL) {
                the_powerup->icon_actor = CreateBillBoard(the_powerup->icon);
                the_powerup->icon_actor->material->extra_prim = the_powerup->material_prims;
            }

            /* Fizzle type */
            the_powerup->fizzle_type = GetAnInt(f);
            /* Time limit (-1 instantaneous, 0 whole race, x seconds) */
            time = 1000 * GetAnInt(f);
            the_powerup->duration = time;
            if (time < 0) {
                the_powerup->type = ePowerup_instantaneous;
            } else if (time == 0) {
                the_powerup->type = ePowerup_whole_race;
            } else {
                the_powerup->type = ePowerup_timed;
            }
            /* Initial value for keyboard operable powerups
             * (zero if not key operable,
             *  positive if time in seconds,
             *  negative if number of uses)
             */
            the_powerup->initial_value = GetAnInt(f);
            if (the_powerup->initial_value > 0) {
                the_powerup->initial_value *= 1000;
            }
            /* Action index */
            action_index = GetAnInt(f);
            if (action_index >= 0) {
                the_powerup->got_proc = C2V(gGot_procs)[action_index];
                the_powerup->lose_proc = C2V(gLose_procs)[action_index];
                the_powerup->periodic_proc = C2V(gPeriodic_procs)[action_index];
            } else {
                the_powerup->lose_proc = NULL;
                the_powerup->periodic_proc = NULL;
                the_powerup->got_proc = NULL;
            }
            /*  Number of floating point params */
            the_powerup->number_of_float_params = GetAnInt(f);
            the_powerup->float_params = BrMemAllocate(sizeof(float) * the_powerup->number_of_float_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_float_params; j++) {
                the_powerup->float_params[j] = GetAFloat(f);
            }
            /* Number of integer params */
            the_powerup->number_of_integer_params = GetAnInt(f);
            the_powerup->integer_params = BrMemAllocate(sizeof(int) * the_powerup->number_of_integer_params, kMem_powerup_parms);
            for (j = 0; j < the_powerup->number_of_integer_params; j++) {
                the_powerup->integer_params[j] = GetAnInt(f);
            }
            /* Group inclusion */
            the_powerup->group_inclusion = GetAnInt(f);
            /* Pratcam sequence */
            the_powerup->prat_cam_event = GetAnInt(f);
            /* Flags (1 = net_global, 2 = net_inappropriate, 4 = off_before_snapweld) */
            the_powerup->net_type = GetAnInt(f);
        }
    }

    /* RESPAWN SPECS */

    /* Number of respawn specs */
    C2V(gNumber_of_powerup_respawn_specs) = GetAnInt(f);
    C2V(gPowerup_respawn_specs) = BrMemAllocate(C2V(gNumber_of_powerup_respawn_specs) * sizeof(tPowerup_respawn_spec), kMem_powerup_array);
    for (i = 0; i < C2V(gNumber_of_powerup_respawn_specs); i++) {
        C2V(gPowerup_respawn_specs)[i].bools = BrMemAllocate(C2V(gNumber_of_powerups) * sizeof(tU8), kMem_powerup_array);
        /* Number of index groups */
        count_index_groups = GetAnInt(f);
        for (j = 0; j < count_index_groups; j++) {
            index_range = GetAnInt(f);
            if (index_range >= 0) {
                /* index_range is index */
                C2V(gPowerup_respawn_specs)[i].bools[index_range] = 1;
            }  else {
                /* index_range is range */
                first = GetAnInt(f);
                for (k = first; k < first - index_range; k++) {
                    C2V(gPowerup_respawn_specs)[i].bools[k] = 1;
                }
            }
        }
    }
    C2V(gRace_powerup_respawn_bools) = C2V(gPowerup_respawn_specs)[0].bools;

    /* NETWORK AUTO-GOODIES */

    /* Min,max times between Auto-goodies */
    GetPairOfInts(f, &C2V(gNet_powerup_time_between_goodies_min), &C2V(gNet_powerup_time_between_goodies_max));
    C2V(gNet_powerup_time_between_goodies_min) *= 1000;
    C2V(gNet_powerup_time_between_goodies_max) *= 1000;
    /* Number of goodies to choose from */
    C2V(gNet_count_powerup_goodies) = GetAnInt(f);
    C2V(gNet_powerup_goodies) = BrMemAllocate(C2V(gNet_count_powerup_goodies) * sizeof(int), kMem_powerup_array);
    for (i = 0; i < C2V(gNet_count_powerup_goodies); i++) {
        C2V(gNet_powerup_goodies)[i] = GetAnInt(f);
    }
    PFfclose(f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d96c0, LoadPowerups, LoadPowerups_original)

static void C2_HOOK_FASTCALL ReadPowerupSmashable(FILE* pF, tSmashable_item_spec* pSmashable_spec) {
    int i;
    int d1, d2;

    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, initial_pos) != 40);
    C2_HOOK_BUG_ON(offsetof(tShrapnel_spec, type_info) != 56);
    C2_HOOK_BUG_ON(sizeof(tShrapnel_spec) != 88);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.shrapnel) != 44);
    C2_HOOK_BUG_ON(offsetof(tSmashable_item_spec, mode_data) + offsetof(tSmashable_item_spec_shrapnel, connotations.special_effects.explosion_animation) != 572);
    C2_HOOK_BUG_ON(sizeof(tSmashable_item_spec) != 736);

    c2_memset(pSmashable_spec, 0, sizeof(tSmashable_item_spec));
    /* Number of sounds */
    pSmashable_spec->mode_data.shrapnel.connotations.count_sounds = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.count_sounds; i++) {
        /* Sound ID */
        pSmashable_spec->mode_data.shrapnel.connotations.sounds[i] = LoadSoundInStorage(&C2V(gTrack_storage_space), GetAnInt(pF));
    }
    ReadShrapnelSpec(pF, pSmashable_spec->mode_data.shrapnel.connotations.shrapnel, &pSmashable_spec->mode_data.shrapnel.connotations.count_shrapnel);
    ReadExplosion(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.explosion_animation);
    ReadSpillData(pF, &pSmashable_spec->mode_data.shrapnel.connotations.special_effects.slick);
    ReadNonCarCuboidActivation(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.activations);
    ReadShrapnelSideEffects(pF, &pSmashable_spec->mode_data.shrapnel.connotations.side_effects.side_effects);

    /* Extension flags */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags = GetAnInt(pF);
    if (pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_flags & 0x1) {
        pSmashable_spec->mode_data.shrapnel.connotations.side_effects.extension_arg = GetAnInt(pF);
    }
    /* Room turn on */
    pSmashable_spec->mode_data.shrapnel.connotations.side_effects.room_turn_on_code = GetAnInt(pF);
    /* Award code */
    ReadAward(pF, &pSmashable_spec->mode_data.shrapnel.connotations.award);
    /* Count variable changes */
    pSmashable_spec->mode_data.shrapnel.connotations.count_runtime_variable_changes = GetAnInt(pF);
    for (i = 0; i < pSmashable_spec->mode_data.shrapnel.connotations.count_runtime_variable_changes; i++) {
        GetPairOfInts(pF, &d1, &d2);
        pSmashable_spec->mode_data.shrapnel.connotations.runtime_variable_changes[i].field_0x0 = d2;
        pSmashable_spec->mode_data.shrapnel.connotations.runtime_variable_changes[i].field_0x2 = d1;
    }
    pSmashable_spec->trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    REC2_BUG_ON((kSmashableTrigger_Model | kSmashableTrigger_Number) != 0x3);
    pSmashable_spec->mode = kSmashableMode_Remove;
    REC2_BUG_ON(kSmashableMode_Remove != 0x3);
}

void C2_HOOK_FASTCALL ReadPowerupSmashables(FILE* pF) {

    ReadPowerupSmashable(pF, &C2V(gPowerup_pickup_smashable));
    ReadPowerupSmashable(pF, &C2V(gPowerup_respawn_smashable));

    c2_memset(&C2V(gUnknown_smashable_006a3660), 0, sizeof(C2V(gUnknown_smashable_006a3660)));
    C2V(gUnknown_smashable_006a3660).trigger_type = kSmashableTrigger_Model | kSmashableTrigger_Number;
    C2V(gUnknown_smashable_006a3660).mode = kSmashableMode_Remove;
}
C2_HOOK_FUNCTION(0x004efa00, ReadPowerupSmashables)

void C2_HOOK_FASTCALL InitMineShit(void) {
    tPath_name the_path;
    tTWTVFS twt;
    int i;

    PathCat(the_path, C2V(gApplication_path), "MINE");
    twt = OpenPackFileAndSetTiffLoading(the_path);
    LoadAllStuffInDirectory(&C2V(gMisc_storage_space), the_path, kRendererShadingType_Specular);
    ClosePackFileAndSetTiffLoading(twt);

    C2_HOOK_BUG_ON(sizeof(tShit_mine) != 56);
    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gShit_mines)) != 20);
    for (i = 0; i < REC2_ASIZE(C2V(gShit_mines)); i++) {
        C2V(gShit_mines)[i].flags = 4;
        C2V(gShit_mines)[i].field_0x35 = 0;
        C2V(gShit_mines)[i].collision_info = MungeSphereObject(BrModelFind("SHITMINE.ACT") REC2_THISCALL_EDX, C2V(gMass_mine));
        C2V(gShit_mines)[i].collision_info->uid = i;
        C2V(gShit_mines)[i].collision_info->flags_0x238 = 0x40;
        C2V(gShit_mines)[i].collision_info->owner = &C2V(gShit_mines)[i];
        BrMatrix34Translate(&C2V(gShit_mines)[i].collision_info->transform_matrix, 0.f, 0.f, 0.f);
        BrMatrix34Translate(&C2V(gShit_mines)[i].collision_info->actor->t.t.mat, 0.f, 0.f, 0.f);
    }
}
C2_HOOK_FUNCTION(0x004da530, InitMineShit)

int (C2_HOOK_FASTCALL * DoExplodingMineEffect_original)(tShit_mine *pMine);
int C2_HOOK_FASTCALL DoExplodingMineEffect(tShit_mine *pMine) {

#if defined(C2_HOOKS_ENABLED)
    return DoExplodingMineEffect_original(pMine);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ddab0, DoExplodingMineEffect, DoExplodingMineEffect_original)

int C2_HOOK_FASTCALL CarVulnerableByMine(tCar_spec* pCar) {

    if (pCar->field_0x4c8 > 1.f) {
        return 0;
    }
    if (C2V(gNet_mode) != eNet_mode_none) {
        tNet_game_player_info* net_player = NetPlayerFromCar(pCar);
        if (net_player != NULL && net_player->field_0x80 != 0) {
            return 0;
        }
    }
    if (pCar->invulnerable1) {
        return 0;
    }
    return 1;
}

void (C2_HOOK_FASTCALL * ProcessShitMines_original)(tU32 pTime);
void C2_HOOK_FASTCALL ProcessShitMines(tU32 pTime) {

#if 0//defined(C2_HOOKS_ENABLED)
    ProcessShitMines_original(pTime);
#else
    int i;
    int play_effect;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gShit_mines)) != 20);

    for (i = 0; i < REC2_ASIZE(C2V(gShit_mines)); i++) {
        tShit_mine* mine = &C2V(gShit_mines)[i];

        if (!(mine->flags & 0x1)) {
            continue;
        }
        if (mine->flags & 0x80) {
            tCar_spec *car = mine->car;

            if (car != NULL) {
                int car_vulnerable;

                mine->flags &= ~0x80;
                car_vulnerable = CarVulnerableByMine(car);
                if (car_vulnerable) {
                    TotallySpamTheModel(car REC2_THISCALL_EDX, 0.1f * mine->max_damage);
                }
                if (car_vulnerable && (C2V(gNet_mode) == eNet_mode_none || C2V(gNet_mode) == eNet_mode_host)) {
                    car->collision_info->v.v[1] += FRandomBetween(mine->field3_0xc, mine->initial_y_speed_factor) / (car->collision_info->M * WORLD_SCALE);
                    car->collision_info->omega.v[2] += FRandomPosNeg(mine->initial_z_omega_factor) * TAU_F / car->collision_info->M;
                    car->collision_info->omega.v[0] += FRandomPosNeg(mine->initial_x_omega_factor) * TAU_F / car->collision_info->M;
                    car->collision_info->disable_move_rotate = 0;

                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x49c, 0x49c);
                    car->collision_info->field_0x49c = 40 + C2V(gINT_0074a5ec);
                    if (!car->invulnerable2) {
                        int j;

                        C2_HOOK_BUG_ON(REC2_ASIZE(car->damage_units) != 12);
                        for (j = 0; j < REC2_ASIZE(car->damage_units); j++) {
                            DoDamage(car, j, IRandomBetween(0, (int)(25.f * mine->max_damage)));
                        }
                        if (car != mine->field_0x30) {
                            car->last_hit_by = mine->field_0x30;
                            car->time_last_hit = PDGetTotalTime();
                            CrashEarnings(car, NULL);
                        }
                    }
                }
            }
        }
        play_effect = 0;
        if (mine->flags & 0x20) {
            mine->flags &= ~0x20;
            play_effect = 1;
        } else {
            if (!(mine->flags & 0x2)) {
                if ((mine->flags & 0x4) && pTime > mine->next_think_time) {
                    play_effect = 1;
                }
            } else if (pTime > mine->next_think_time) {
                mine->flags &= ~0x3;
            }
        }
        if (play_effect) {
            DoExplodingMineEffect(mine);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004dde20, ProcessShitMines, ProcessShitMines_original)

void (C2_HOOK_FASTCALL * DisposeMutantTail_original)(void);
void C2_HOOK_FASTCALL DisposeMutantTail(void) {

#if defined(C2_HOOKS_ENABLED)
    DisposeMutantTail_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e06a0, DisposeMutantTail, DisposeMutantTail_original)

void (C2_HOOK_FASTCALL * CloseDownPowerUps_original)(void);
void C2_HOOK_FASTCALL CloseDownPowerUps(void) {

#if defined(C2_HOOKS_ENABLED)
    CloseDownPowerUps_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da730, CloseDownPowerUps, CloseDownPowerUps_original)

void C2_HOOK_FASTCALL SetAPOToInitialValues(void) {
    int i;

    for (i = 0; i < 3; i++) {
        C2V(gProgram_state).current_car.power_up_levels[i] = C2V(gInitial_powerup_slots)[i];
        C2V(gProgram_state).current_car.power_up_slots[i] = C2V(gInitial_powerup_slots)[i];
    }
}
C2_HOOK_FUNCTION(0x00502e00, SetAPOToInitialValues)
