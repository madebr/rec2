#include "powerups.h"

#include "animation.h"
#include "errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "platform.h"
#include "shrapnel.h"
#include "skidmark.h"
#include "utility.h"
#include "world.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#include "c2_string.h"

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

void (C2_HOOK_FASTCALL * InitPowerups_original)(void);
void C2_HOOK_FASTCALL InitPowerups(void) {

    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    InitPowerups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d9ea0, InitPowerups, InitPowerups_original)

br_actor* (C2_HOOK_FASTCALL * CreateActorFromPowerupMap_original)(br_pixelmap* pMap);
br_actor* C2_HOOK_FASTCALL CreateActorFromPowerupMap(br_pixelmap* pMap) {

#if defined(C2_HOOKS_ENABLED)
    return CreateActorFromPowerupMap_original(pMap);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e09d0, CreateActorFromPowerupMap, CreateActorFromPowerupMap_original)

static br_pixelmap* find_pixelmap_name(const char* name) {
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
        C2V(gPowerup_txt_path) = "POWERUP.TXT";
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, "POWERUPS");
    twtvfs = TWT_MountEx(the_path);
    LoadAllTexturesFromTexSubdirectories(&C2V(gMisc_storage_space), the_path);
    TWT_UnmountEx(twtvfs);

    for (i = 0; i < REC2_ASIZE(C2V(gFizzle_in)); i++) {
        C2V(gFizzle_in)[i] = find_pixelmap_name(C2V(gFizzle_names)[i]);
    }
    C2V(gFizzle_height) = C2V(gFizzle_in)[0]->height / 4;

    PathCat(the_path, C2V(gApplication_path), C2V(gPowerup_txt_path));
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CantLoadCarResolutionIndependentFile);
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
            the_powerup->icon = find_pixelmap_name(s);
            if (the_powerup->icon != NULL) {
                BrMapAdd(the_powerup->icon);
                DRPixelmapConvertRGB565ToRGB555IfNeeded(the_powerup->icon, C2V(gBack_screen)->type);
            }
            if (the_powerup->icon != NULL) {
                the_powerup->icon_actor = CreateActorFromPowerupMap(the_powerup->icon);
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
    DRfclose(f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d96c0, LoadPowerups, LoadPowerups_original)
