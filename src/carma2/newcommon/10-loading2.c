#include "10-loading2.h"

#include "00-car.h"
#include "02-init.h"
#include "07-structur.h"
#include "08-loading1.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "27-powerup.h"
#include "28-world3.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "33-depth.h"
#include "37-brucetrk.h"
#include "38-flicplay.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "53-controls.h"
#include "55-volume.h"
#include "57-drone.h"
#include "58-crush2.h"
#include "59-camera.h"
#include "61-pedestrn.h"
#include "62-graphics3.h"
#include "63-loading3.h"
#include "64-movie.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "71-newgame.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0068c6ec
FILE* gTempFile;

// GLOBAL: CARMA2_HW 0x0068c718
const char* gPedTextTxtPath;

// GLOBAL: CARMA2_HW 0x0068b88c
int gKey_map_index;

#define DECODE_OFFSET 50
// GLOBAL: CARMA2_HW 0x00655e38
char gDecode_string[14] = {
    (char)('i' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('a' + DECODE_OFFSET),
    (char)('m' + DECODE_OFFSET),
    (char)(' ' + DECODE_OFFSET),
    (char)('f' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('d' + DECODE_OFFSET),
    (char)('l' + DECODE_OFFSET),
    (char)('i' + DECODE_OFFSET),
    (char)('n' + DECODE_OFFSET),
    (char)('g' + DECODE_OFFSET),
    '\0'
};

// GLOBAL: CARMA2_HW 0x0068c6f8
char gUnderwater_screen_name[32];

// GLOBAL: CARMA2_HW 0x00764ea0
char gDefault_car[32];

// GLOBAL: CARMA2_HW 0x00764ec0
char gBasic_car_name[32];

// GLOBAL: CARMA2_HW 0x007638c0
int gNet_score_targets[8];

// GLOBAL: CARMA2_HW 0x0074b58c
int gDemo_race_rank_equivalent;

// GLOBAL: CARMA2_HW 0x0074b4fc
int gCount_demo_opponents;

// GLOBAL: CARMA2_HW 0x0074b4c0
int gDemo_opponents[15];

// GLOBAL: CARMA2_HW 0x00679308
int gCamera_type;

void C2_HOOK_FASTCALL LoadInRegisteeDir(const char *pRoot, const char *pSubDir, int pInitialize_palettes) {
    tPath_name the_path;
    tPath_name the_path2;
    tTWTVFS twt;

    PathCat(the_path, pRoot, pSubDir);
    LoadInFiles(the_path, "PALETTES", DRLoadPalette);
    LoadInFiles(the_path, "SHADETAB", DRLoadShadeTable);

    if (pInitialize_palettes) {
        InitializePalettes();
    }

    PathCat(the_path2, the_path, "PIXELMAP");
    twt = OpenPackFileAndSetTiffLoading(the_path2);
    LoadAllImagesInDirectory(&gMisc_storage_space, the_path2);
    ClosePackFileAndSetTiffLoading(twt);

    LoadInFiles(the_path, "MATERIAL", DRLoadMaterials);
    LoadInFiles(the_path, "MODELS", DRLoadModels);
    LoadInFiles(the_path, "ACTORS", DRLoadActors);
    LoadInFiles(the_path, "LIGHTS", DRLoadLights);
}

// FUNCTION: CARMA2_HW 0x00486e10
void C2_HOOK_FASTCALL LoadInRegistees(void) {

    LoadInRegisteeDir(gApplication_path, "REG", 1);
}

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info) {
    char s[256];
    const char *str;
    int i;

    /* (armour|power|offensive), single player, each skill level */
    GetThreeInts(gTempFile, &pSlot_info->initial[0], &pSlot_info->initial[1], &pSlot_info->initial[2]);
    /* (armour|power|offensive), each network game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(pSlot_info->initial_network); i++) {
        sscanf(str, "%d", &pSlot_info->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch) {
    char s[256];
    const char *str;
    int i;

    GetThreeFloats(gTempFile, &pBunch->initial[0], &pBunch->initial[1], &pBunch->initial[2]);
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(pBunch->initial_network); i++) {
        sscanf(str, "%f", &pBunch->initial_network[i]);
        str = strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL GetHithers(void) {
    char s[256];
    int result;
    int position;

    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(gTempFile, s);
    result = sscanf(&s[strspn(s, "\t ,")], "%f%n", &gCamera_hither, &position);
    if (result == 0) {
        FatalError(kFatalError_MysteriousX_SS, s, "GENERAL.TXT");
    }
    sscanf(&s[position + strspn(&s[position], "\t ,")], "%f", &gCamera_cockpit_hither);
    gCamera_hither *= 2;
    gCamera_cockpit_hither *= 2;
}

// FUNCTION: CARMA2_HW 0x00486ef0
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
    tPath_name the_path;
    int i;
    char* str;
    char s[256];
    br_scalar armour_mult, power_mult, offensive_mult;
    int time;

    PathCat(the_path, gApplication_path, "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    gTempFile = PFfopen(the_path, "rb");
    if (gTempFile != NULL) {
        PFfgets(s, REC2_ASIZE(s) - 1, gTempFile);
        PFfclose(gTempFile);

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] -= DECODE_OFFSET;
        }

        // trim trailing CRLF etc
        while (s[0] != '\0' && s[strlen(s) - 1] < 0x20) {
            s[strlen(s) - 1] = 0;
        }

        if (strcmp(s, gDecode_string) == 0) {
            gDecode_thing = 0;
        }

        for (i = 0; (unsigned)i < strlen(gDecode_string); i++) {
            gDecode_string[i] += DECODE_OFFSET;
        }
    }
    PathCat(the_path, gApplication_path, "GENERAL.TXT");
    gTempFile = DRfopen(the_path, "rt");
    if (gTempFile == NULL) {
        FatalError(kFatalError_FailToOpenGeneralSettings);
    }

    /* Disable TIFF conversion */
    gDisableTiffConversion = GetAnInt(gTempFile);

    GetHithers();

    /* Yon */
    gCamera_yon = GetAFloat(gTempFile);
    /* Camera angle */
    gCamera_angle = GetAFloat(gTempFile);
    /* Headup background brightness amount */
    gDim_amount = GetAnInt(gTempFile);
    /* Initial rank */
    gInitial_rank = GetAnInt(gTempFile);
    /* Credits per rank for each skill level */
    GetThreeInts(gTempFile, &gCredits_per_rank[0], &gCredits_per_rank[1], &gCredits_per_rank[2]);

    ReadCrushSettings(gTempFile);

    /* Time per ped kill for each skill level */
    GetThreeInts(gTempFile, &gTime_per_ped_kill[0], &gTime_per_ped_kill[1], &gTime_per_ped_kill[2]);
    /* Seconds per unit car damage for each skill level (with peds */
    GetThreeFloats(gTempFile, &gSeconds_per_unit_car_damage[0], &gSeconds_per_unit_car_damage[1], &gSeconds_per_unit_car_damage[2]);
    /* Credits per unit car damage for each skill level (with peds) */
    GetThreeFloats(gTempFile, &gCredits_per_unit_car_damage[0], &gCredits_per_unit_car_damage[1], &gCredits_per_unit_car_damage[2]);
    /* Time awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_wasting_car[0], &gTime_wasting_car[1], &gTime_wasting_car[2]);
    /* Credits awarded for wasting car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_wasting_car[0], &gCredits_wasting_car[1], &gCredits_wasting_car[2]);
    /* Time awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gTime_rolling_car[0], &gTime_rolling_car[1], &gTime_rolling_car[2]);
    /* Credits awarded for rolling car for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_rolling_car[0], &gCredits_rolling_car[1], &gCredits_rolling_car[2]);
    /* Credits awarded for checkpoints for each skill level (with peds) */
    GetThreeInts(gTempFile, &gCredits_checkpoint[0], &gCredits_checkpoint[1], &gCredits_checkpoint[2]);
    /* Jump start fine for each level */
    GetThreeInts(gTempFile, &gJump_start_fine[0], &gJump_start_fine[1], &gJump_start_fine[2]);
    /* Credits per second of time bonus */
    GetThreeInts(gTempFile, &gCredits_per_second_time_bonus[0], &gCredits_per_second_time_bonus[1], &gCredits_per_second_time_bonus[2]);
    /* Cunning stunt bonus for each skill level */
    GetThreeInts(gTempFile, &gCunning_stunt_bonus[0], &gCunning_stunt_bonus[1], &gCunning_stunt_bonus[2]);

    /* Cars to use as defaults: */
    GetAString(gTempFile, gDefault_car);
    GetAString(gTempFile, gBasic_car_name);

    gKnobbled_frame_period = 0;
    gOpponent_nastyness_frigger = 1.0f;
    /* Min time in secs after last contact with play before opponent considers repairing */
    gMinTimeOpponentRepair = (int)GetAScalar(gTempFile);
    /* Max time in secs after last contact with play before opponent considers repairing */
    gMaxTimeOpponentRepair = (int)GetAScalar(gTempFile);

    /* Default underwater special volume parameters */
    ParseSpecialVolume(gTempFile, &gDefault_default_water_spec_vol, gUnderwater_screen_name, 0);

    /* Initial armour, single player, each skill level */
    /* Initial armour, each network game type */
    LoadBunchOParameters(&gInitial_APO[0]);

    /* Initial power, single player, each skill level */
    /* Initial power, each network game type */
    LoadBunchOParameters(&gInitial_APO[1]);

    /* Initial offensive, single player, each skill level */
    /* Initial offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO[2]);

    /* Initial potential armour, single player, each skill level */
    /* Initial potential armour, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[0]);

    /* Initial potential power, single player, each skill level */
    /* Initial potential power, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[1]);

    /* Initial potential offensive, single player, each skill level */
    /* Initial potential offensive, each network game type */
    LoadBunchOParameters(&gInitial_APO_potential[2]);

    /* Max armour, single player, each skill level */
    /* Max armour, each network game type */
    LoadBunchOParameters(&gMax_APO[0]);

    /* Max power, single player, each skill level */
    /* Max power, each network game type */
    LoadBunchOParameters(&gMax_APO[1]);

    /* Max offensive, single player, each skill level */
    /* Max offensive, each network game type */
    LoadBunchOParameters(&gMax_APO[2]);

    /* APO cost, single player */
    /* APO cost, each network game type */
    LoadBunchOParameters(&gCost_APO);

    /* Trade-in APO value, single player */
    /* Trade-in APO value, each network game type */
    LoadBunchOParameters(&gTrade_in_value_APO);

    /* APO substitution value, single player */
    /* APO substitution, each network game type */
    LoadBunchOParameters(&gSubstitution_value_APO);

    /* APO potential substitution value, single player */
    /* APO potential substitution, each network game type */
    LoadBunchOParameters(&gPotential_substitution_value_APO);

    /* Armour starting value */
    gArmour_starting_value[0] = GetAScalar(gTempFile);
    /* Power starting value */
    gPower_starting_value[0] = GetAScalar(gTempFile);
    /* Offensive starting value */
    gOffensive_starting_value[0] = GetAScalar(gTempFile);
    /* Armour per-level multiplier */
    armour_mult = GetAScalar(gTempFile);
    /* Power per-level multiplier */
    power_mult = GetAScalar(gTempFile);
    /* Offensive per-level multiplier */
    offensive_mult = GetAScalar(gTempFile);
    for (i = 1; i < (int)REC2_ASIZE(gArmour_starting_value); i++) {
        gArmour_starting_value[i] = gArmour_starting_value[i - 1] * armour_mult;
        gPower_starting_value[i] = gPower_starting_value[i - 1] * power_mult;
        gOffensive_starting_value[i] = gOffensive_starting_value[i - 1] * offensive_mult;
    }

    /* Powerup number to use when time powerup got during network game */
    gNet_powerup_time_replacement = GetAnInt(gTempFile);

    /* Starting money for each skill level */
    GetThreeInts(gTempFile, &gStarting_money[0], &gStarting_money[1], &gStarting_money[2]);
    /* Starting money in network mode */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
#if defined(REC2_FIX_BUGS)
    for (i = 0; i < (int)REC2_ASIZE(gNet_starting_money); i++) {
#else
    for (i = 0; i < 5; i++) {
#endif
        sscanf(str, "%d", &gNet_starting_money[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Repair cost for each skill level (cred per % damage) */
    /* Repair cost for each net game (cred per % damage) */
    LoadBunchOFloatParameters(&gRepair_cost);

    /* Recovery cost for each skill level */
    /* Recovery cost for each net game type */
    LoadBunchOFloatParameters(&gRecovery_cost);

    /* Car softness factor for each net skill level */
    /* Car softness factor for each net game type */
    LoadBunchOFloatParameters(&gCar_softness);

    /* Car-to-car damage multiplier for each net skill level */
    /* Car-to-car damage multiplier for each net game type */
    LoadBunchOFloatParameters(&gCar_car_damage_multiplier);

    /* Score targets for each net game type */
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(gNet_score_targets); i++) {
        sscanf(str, "%d", &gNet_score_targets[i]);
        str = strtok(NULL, "\t ,/");
    }

    /* Pickup respawn min time (seconds) */
    gPickup_respawn_min_time_ms = 1000 * GetAnInt(gTempFile);
    /* Pickup respawn max extra time (seconds) */
    gPickup_respawn_max_extra_time_ms = 1000 * GetAnInt(gTempFile);

    /* Demo race rank equivalent */
    gDemo_race_rank_equivalent = GetAnInt(gTempFile);

    /* Number of demo opponents */
    gCount_demo_opponents = GetAnInt(gTempFile);
    /* Demo opponents */
    for (i = 0; (int)i < gCount_demo_opponents; i++) {
        gDemo_opponents[i] = GetAnInt(gTempFile);
    }

    /* default Gravity Multiplier */
    gDefault_gravity = GetAScalar(gTempFile);

    /* Flic sound delays */
    /* Delay (in seconds) before sound during pre-smack flic */
    gFlic_sound_delay_pre_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-smack flic */
    gFlic_sound_delay_post_smack = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during 'not in demo' flic */
    gFlic_sound_delay_not_in_demo = GetAFloat(gTempFile);
    /* Delay (in seconds) before sound during post-demo slideshow flic */
    gFlic_sound_delay_post_demo = GetAFloat(gTempFile);

    /* Time (in seconds) that credits take before they reach the recovery
     * amount when self-increasing (if starting at zero)
     *
     * First line is for single-player games, second is for each type of
     * network game. Zero means don't tick up.
     */
    time = 1000 * GetAnInt(gTempFile);
    for (i = 0; i < (int)REC2_ASIZE(gAuto_increase_credits_dt); i++) {
        gAuto_increase_credits_dt[i] = (int)((float)time / (0.02f * (float)gRecovery_cost.initial[i]));
    }
    GetALineAndDontArgue(gTempFile, s);
    str = strtok(s, "\t ,/");
    for (i = 0; i < (int)REC2_ASIZE(gNet_score_targets); i++) {
        int t;
        sscanf(str, "%d", &t);
        gNet_auto_increase_credits_dt[i] = (int)((float)(1000 * t) / (.02f * (float)gRecovery_cost.initial_network[i]));
        str = strtok(NULL, "\t ,/");
    }

    /* Mutant tail thing settings */
    /* Number of links including ball */
    gCount_mutant_tail_parts = GetAnInt(gTempFile);
    /* Mass of each link */
    gMass_mutant_tail_link = GetAFloat(gTempFile);
    /* Mass of ball */
    gMass_mutant_tail_ball = GetAFloat(gTempFile);

    /*  Mine / Mortar weight */
    gMass_mine = GetAFloat(gTempFile);
}

// STUB: CARMA2_HW 0x00487dc0
void C2_HOOK_FASTCALL FinishLoadingGeneral(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00487e10
void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, gApplication_path, "KEYMAP_X.TXT");
    the_path[strlen(the_path) - 5] = '0' + gKey_map_index;
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < (int)REC2_ASIZE(gKey_mapping); i++) {
        fscanf((FILE*)f, "%d", &gKey_mapping[i]);
    }
    PFfclose(f);
}

// LoadInterfaceStuff

// FUNCTION: CARMA2_HW 0x00487eb0
void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

}

// AdjustCarCoordinates

// LoadSpeedo

// LoadTacho

// LoadHeadups

// OldReadNonCarMechanicsData

// GetPerpendicular

// ReadNonCarMechanicsData

// ReadPartHeader

// ReadMechanicsShapes

// NewReadMechanicsData

// LoadGear

// AddRefOffset

// GetDamageProgram

// AllocateCarActorData

// AllocateCarModelData

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

// GetSimpleMaterial

// ReadShrapnelMaterials

// SetModelFlags

// FindCarShellModel

// LinkCarActorsToGroovidelicsCB

// LinkCarActorsToGroovidelics

// ReadCarKeywordData

// TrashModelMaterialIdentifiers

// CentreModel

// PivotizeWheel

// SetUpGraphicalWheelStuff

// LoadCar

// STUB: CARMA2_HW 0x0048c150
void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    NOT_IMPLEMENTED();
}

// OpenRaceFile

// STUB: CARMA2_HW 0x0048c1c0
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {
    NOT_IMPLEMENTED();
}

// LoadRaceInfo

// LoadOpponents

// AboutToLoadFirstCar

// LoadOpponentsCars

// FUNCTION: CARMA2_HW 0x0048cfc0
void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    int i;
    FILE *f;
    char s[256];
    tPath_name the_path;

    if (gPedTextTxtPath == NULL) {
        SetDefaultTextFileName();
    }
    PathCat(the_path, gApplication_path, gPedTextTxtPath);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < (int)REC2_ASIZE(gMisc_strings); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        gMisc_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gMisc_strings[i], s);
    }
    // Thousands delimiter
    gMisc_strings[294][1] = '\0';
    if (gMisc_strings[294][0] == 'C') {
        gMisc_strings[294][0] = ',';
    } else if (gMisc_strings[294][0] == 'P') {
        gMisc_strings[294][0] = '.';
    } else if (gMisc_strings[294][0] == 'S') {
        gMisc_strings[294][0] = ' ';
    }
    PFfclose(f);
}

// FillInRaceInfo

// FUNCTION: CARMA2_HW 0x0048d110
void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions) {

    pOptions->enable_text_messages = GetAnInt(pF);
    pOptions->show_players_on_map = GetAnInt(pF);
    pOptions->show_powerups_on_map = GetAnInt(pF);
    pOptions->powerup_respawn = GetAnInt(pF);
    pOptions->waste_to_transfer = GetAnInt(pF);
    pOptions->open_game = GetAnInt(pF);
    pOptions->grid_start = GetAnInt(pF);
    pOptions->race_sequence_type = GetAnInt(pF);
    pOptions->random_car_choice = GetAnInt(pF);
    pOptions->car_choice = GetAnInt(pF);
    pOptions->starting_credits = GetAnInt(pF);
    pOptions->starting_target = GetAnInt(pF);
}

// PrintNetOptions

// STUB: CARMA2_HW 0x0048d190
int C2_HOOK_FASTCALL SaveOptions(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0048d8f0
int C2_HOOK_FASTCALL RestoreOptions(void) {
    tPath_name the_path;
    FILE* f;
    float arg;
    char line[80];
    char token[80];
    char* s;

    gProgram_state.music_volume = 200;
    gProgram_state.effects_volume = 200;
    gProgram_state.skill_level = 1;
    gMap_render_x = 6.0;
    gMap_render_y = 6.0;
    gMap_render_width = 64.0;
    gMap_render_height = 40.0;
    gMap_trans = 0;
    gHeadup_map_x = 228;
    gHeadup_map_y = 150;
    gHeadup_map_w = 48;
    gHeadup_map_h = 48;
    gHeadup_detail_level = kMax_headup_detail_level;
    gMap_view = 1;
    gMini_map_visible = 1;

    DefaultNetSettings();
    SetQuickTimeDefaults();

    PathCat(the_path, gApplication_path, "OPTIONS.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        return 0;
    }

    while (PFfgets(line, sizeof(line), f)) {
        if (sscanf(line, "%79s%f", token, &arg) == 2) {
            if (strcmp(token, "YonFactor") == 0) {
                SetYonFactor(arg);
            } else if (strcmp(token, "SkyTextureOn") == 0) {
                SetSkyTextureOn((int)arg);
            } else if (strcmp(token, "CarTexturingLevel") == 0) {
                SetCarTexturingLevel((tCar_texturing_level)arg);
            } else if (strcmp(token, "RoadTexturingLevel") == 0) {
                SetRoadTexturingLevel((tRoad_texturing_level)arg);
            } else if (strcmp(token, "WallTexturingLevel") == 0) {
                SetWallTexturingLevel((tWall_texturing_level)arg);
            } else if (strcmp(token, "ShadowLevel") == 0) {
                SetShadowLevel((tShadow_level)arg);
            } else if (strcmp(token, "DepthCueingOn") == 0) {
                SetDepthCueingOn((int)arg);
            } else if (strcmp(token, "Yon") == 0) {
                SetYon(arg);
            } else if (strcmp(token, "CarSimplificationLevel") == 0) {
                SetCarSimplificationLevel((int)arg);
            } else if (strcmp(token, "AccessoryRendering") == 0) {
                SetAccessoryRendering((int)arg);
            } else if (strcmp(token, "SmokeOn") == 0) {
                SetSmokeOn((int)arg);
            } else if (strcmp(token, "SoundDetailLevel") == 0) {
                SetSoundDetailLevel((int)arg);
            } else if (strcmp(token, "ScreenSize") == 0) {
                SetScreenSize((int)arg);
            } else if (strcmp(token, "MapRenderX") == 0) {
                gMap_render_x = arg;
            } else if (strcmp(token, "MapRenderY") == 0) {
                gMap_render_y = arg;
            } else if (strcmp(token, "MapRenderWidth") == 0) {
                gMap_render_width = arg;
            } else if (strcmp(token, "MapRenderHeight") == 0) {
                gMap_render_height = arg;
            } else if (strcmp(token, "MapMode") == 0) {
            } else if (strcmp(token, "MapTrans") == 0) {
                gMap_trans = (int)arg;
            } else if (strcmp(token, "HeadupMapX") == 0) {
                gHeadup_map_x = (int)arg;
            } else if (strcmp(token, "HeadupMapY") == 0) {
                gHeadup_map_y = (int)arg;
            } else if (strcmp(token, "HeadupMapW") == 0) {
                gHeadup_map_w = (int)arg;
            } else if (strcmp(token, "HeadupMapH") == 0) {
                gHeadup_map_h = (int)arg;
            } else if (strcmp(token, "PlayerName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gProgram_state.player_name, s);
            } else if (strcmp(token, "EVolume") == 0) {
                gProgram_state.effects_volume = (int)arg;
            } else if (strcmp(token, "MVolume") == 0) {
                gProgram_state.music_volume = (int)arg;
            } else if (strcmp(token, "KeyMapIndex") == 0) {
                gKey_map_index = (int)arg;
            } else if (strcmp(token, "CameraType") == 0) {
                gAction_replay_camera_mode = gCamera_type = (int)arg;
            } else if (strcmp(token, "ARCameraType") == 0) {
                gAR_camera_type = (int)arg;
            } else if (strcmp(token, "GoreLevel") == 0) {
                SetGoreLevel((int)arg);
            } else if (strcmp(token, "AnimalsOn") == 0) {
                SetAnimalsOn((int)arg);
            } else if (strcmp(token, "FlameThrowerOn") == 0) {
                SetFlameThrowerOn((int)arg);
            } else if (strcmp(token, "MinesOn") == 0) {
                SetExplosivesOn((int)arg);
            } else if (strcmp(token, "DronesOn") == 0) {
                SetTrafficOn((int)arg);
            } else if (strcmp(token, "MiniMapVisible") == 0) {
                gMini_map_visible = (int)arg;
            } else if (strcmp(token, "SkillLevel") == 0) {
                gProgram_state.skill_level = (int)arg;
            } else if (strcmp(token, "AmbientSound") == 0) {
                gAmbient_sound = (int)arg;
            } else if (strcmp(token, "AutoLoad") == 0) {
                gAuto_load = (int)arg;
            } else if (strcmp(token, "RussellsFannies") == 0) {
                gRussels_fannies = (int)arg;
            } else if (strcmp(token, "QuickTimeQuality") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_quality, s);
            } else if (strcmp(token, "QuickTimeCompressor") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_compressor, s);
            } else if (strcmp(token, "QuickTimeBanner") == 0) {
                gQuick_time_banner_number = (int)arg;
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_banner_texture_name, s);
            } else if (strcmp(token, "QuickTimeTempPath") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_temp_path, s);
            } else if (strcmp(token, "QuickTimeMoviePathStub") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
                strcpy(gQuick_time_movie_path_stub, s);
            } else if (strcmp(token, "NetName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = strtok(line, "\n\r");
            } else if (strcmp(token, "NETGAMETYPE") == 0) {
                gNet_last_game_type = (int)arg;
            } else if (strcmp(token, "NETSETTINGS") == 0) {
                ReadNetworkSettings(f, &gNet_settings[(int)arg]);
            } else if (strcmp(token, "HeadupDetailLevel") == 0) {
                gHeadup_detail_level = (int)arg;
            }
        }
    }
    PFfclose(f);
    gMap_view = 1;
    gMap_render_x = 80.f;
    gMap_render_y = 400.f;
    gMap_render_width = 128.f;
    gMap_render_height = 80.f;
    return 1;
}

// InitFunkGrooveFlags

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}

