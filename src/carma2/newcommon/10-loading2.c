#include "10-loading2.h"

#include "00-car.h"
#include "02-init.h"
#include "07-structur.h"
#include "08-loading1.h"
#include "09-funks.h"
#include "13-crush1.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "18-graphics2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "25-grafdata.h"
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
#include "72-interface.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#define OPPONENT_APC_IDX 98

// GLOBAL: CARMA2_HW 0x006572f0
tU32 gSimple_material_colours[12] = {
    0x005d0303, 0x02a56e04, 0x03a9a905, 0x08006868,
    0x0d3c3626, 0x0e212121, 0x15284c21, 0x1900326a,
    0x1a93491d, 0x1ec0bfc0, 0x23141414, 0x00000000,
};

// GLOBAL: CARMA2_HW 0x00657320
tHeadup_info gHeadup_image_info[45] = {
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "", eNot_ever_ever_ever },
    { "GO.PIX", eNet_or_otherwise },
    { "NUMBER1.PIX", eNet_or_otherwise },
    { "NUMBER2.PIX", eNet_or_otherwise },
    { "NUMBER3.PIX", eNet_or_otherwise },
    { "NUMBER4.PIX", eNet_or_otherwise },
    { "NUMBER5.PIX", eNet_or_otherwise },
    { "SPLATTER.PIX", eNet_or_otherwise },
    { "PILEDRIV.PIX", eNet_or_otherwise },
    { "EXTRASTY.PIX", eNet_or_otherwise },
    { "ARTISTIC.PIX", eNet_or_otherwise },
    { "PLAYWITH.PIX", eNet_or_otherwise },
    { "PUNT.PIX", eNet_or_otherwise },
    { "DROP.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "2XCOMBO.PIX", eNet_or_otherwise },
    { "3XCOMBO.PIX", eNet_or_otherwise },
    { "4XCOMBO.PIX", eNet_or_otherwise },
    { "5XCOMBO.PIX", eNet_or_otherwise },
    { "6XCOMBO.PIX", eNet_or_otherwise },
    { "7XCOMBO.PIX", eNet_or_otherwise },
    { "8XCOMBO.PIX", eNet_or_otherwise },
    { "9XCOMBO.PIX", eNet_or_otherwise },
    { "10XCOMBO.PIX", eNet_or_otherwise },
    { "BILLIARD.PIX", eNet_or_otherwise },
    { "POLITE.PIX", eNet_or_otherwise },
    { "HEADON.PIX", eNet_or_otherwise },
    { "DESTROY.PIX", eNet_or_otherwise },
    { "CHECKPNT.PIX", eNet_or_otherwise },
    { "TIMEUP.PIX", eNot_net },
    { "RACEOVER.PIX", eNet_or_otherwise },
    { "UWASTED.PIX", eNet_or_otherwise },
    { "MAD.PIX", eNet_only },
    { "GAMEOVER.PIX", eNet_only },
    { "UBROKE.PIX", eNet_only },
    { "ULOST.PIX", eNet_only },
    { "UWON.PIX", eNet_only },
    { "BBQBON.PIX", eNet_or_otherwise },
    { "TAILBON.PIX", eNet_or_otherwise },
    { "DETTAILBON.PIX", eNet_or_otherwise },
    { "MISSIONFAIL.PIX", eNet_or_otherwise },
    { "MISSIONDONE.PIX", eNet_or_otherwise },
};

// GLOBAL: CARMA2_HW 0x0079eb60
br_pixelmap* gHeadup_images[45];

// GLOBAL: CARMA2_HW 0x006574d0
const char* gDamage_names[12] = {
    "engine",
    "transmission",
    "driver",
    "steering",
    "lf_brake",
    "rf_brake",
    "lr_brake",
    "rr_brake",
    "lf_wheel",
    "rf_wheel",
    "lr_wheel",
    "rr_wheel",
};

// GLOBAL: CARMA2_HW 0x00657530
char* gRaces_file_names[9] = {
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
};

// GLOBAL: CARMA2_HW 0x00657558
const char* gNet_avail_names[4] = {
    "never",
    "eagle",
    "hawk",
    "all",
};

// GLOBAL: CARMA2_HW 0x0068c6f4
int gCurrent_race_file_index;

// GLOBAL: CARMA2_HW 0x007634ec
int gCountRaceGroups;

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

// GLOBAL: CARMA2_HW 0x0074b4a8
int gGroove_funk_offset;

// GLOBAL: CARMA2_HW 0x007638c0
int gNet_score_targets[8];

// GLOBAL: CARMA2_HW 0x0074b58c
int gDemo_race_rank_equivalent;

// GLOBAL: CARMA2_HW 0x0074b4fc
int gCount_demo_opponents;

// GLOBAL: CARMA2_HW 0x0074b500
br_material* gSimple_materials[32];

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
    float camera_hither;
    float cockpit_hither;
    char whitespace[] = "\t ,";

    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(gTempFile, s);
    result = sscanf(&s[strspn(s, whitespace)], "%f%n", &camera_hither, &position);
    if (result == 0) {
        FatalError(kFatalError_Mysterious_SS, s, "GENERAL.TXT");
    }
    gCamera_hither = camera_hither;
    if (sscanf(&s[position + strspn(&s[position], "\t ,")], "%f", &cockpit_hither) == 0) {
        cockpit_hither = gCamera_hither;
    }
    gCamera_hither *= 2;
    gCamera_cockpit_hither = 2 * cockpit_hither;
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

// FUNCTION: CARMA2_HW 0x00487ea0
void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race) {

    // empty
}

// FUNCTION: CARMA2_HW 0x00487eb0
void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

    // empty
}

void C2_HOOK_FASTCALL AdjustCarCoordinates(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->render_left); i++) {
        pCar->render_left[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->render_top[i] -= gCurrent_graf_data->cock_margin_y;
        pCar->render_right[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->render_bottom[i] -= gCurrent_graf_data->cock_margin_y;
    }
    pCar->mirror_left -= gCurrent_graf_data->cock_margin_x;
    pCar->mirror_top -= gCurrent_graf_data->cock_margin_y;
    pCar->mirror_right -= gCurrent_graf_data->cock_margin_x;
    pCar->mirror_bottom -= gCurrent_graf_data->cock_margin_y;
    pCar->speedo_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->speedo_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->tacho_centre_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->tacho_centre_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->speedo_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->speedo_y[1] -= gCurrent_graf_data->cock_margin_y;
    pCar->tacho_x[1] -= gCurrent_graf_data->cock_margin_x;
    pCar->tacho_y[1] -= gCurrent_graf_data->cock_margin_y;
    for (i = 0; i < (int)REC2_ASIZE(pCar->lhands_x); i++) {
        pCar->lhands_x[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->lhands_y[i] -= gCurrent_graf_data->cock_margin_y;
        pCar->rhands_x[i] -= gCurrent_graf_data->cock_margin_x;
        pCar->rhands_y[i] -= gCurrent_graf_data->cock_margin_y;
    }
    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].x_coord -= gCurrent_graf_data->cock_margin_x;
        pCar->damage_units[i].y_coord -= gCurrent_graf_data->cock_margin_y;
    }
}

// FUNCTION: CARMA2_HW 0x0048b560
void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->speedo_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        if (pCar_spec->speedo_image[pIndex] == NULL) {
            FatalError(kFatalError_CannotLoadSpeedoImage);
        }
        pCar_spec->speedo_y_pitch[pIndex] = pCar_spec->speedo_image[pIndex]->height / 10;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x_pitch[pIndex]);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->speedo_needle_colour[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->max_speed);
    }
}

// FUNCTION: CARMA2_HW 0x0048b810
void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->tacho_radius_2[pIndex] = -1;
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
    } else {
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_x[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_centre_y[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_1[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_radius_2[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_start_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_end_angle[pIndex]);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->tacho_needle_colour[pIndex]);
    }
}

// FUNCTION: CARMA2_HW 0x0048ba60
void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    int j;
    int number_of_slots;

    number_of_slots = GetAnInt(pF);
    for (j = 0; j < number_of_slots; j++) {
        GetALineAndDontArgue(pF, s);
        str = strtok(s, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].field_0x28);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].y);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].colour);
        str = strtok(NULL, "\t ,/");
        /* The token and destination share the line buffer. */
        memmove(s, str, strlen(str) + 1);
        switch (s[0]) {
        case 'c':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_centre;
            break;
        case 'l':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_left;
            break;
        case 'r':
            pCar_spec->headup_slots[pIndex][j].justification = eJust_right;
            break;
        }
        if (s[1] == 'c') {
            pCar_spec->headup_slots[pIndex][j].cockpit_anchored = 1;
        }
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_left);
        if (pCar_spec->headup_slots[pIndex][j].dim_left < 0) {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 0;
        } else {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 1;
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_top);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_right);
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_bottom);
        }
    }
}

// OldReadNonCarMechanicsData

// GetPerpendicular

// ReadNonCarMechanicsData

// ReadPartHeader

// ReadMechanicsShapes

// NewReadMechanicsData

void C2_HOOK_FASTCALL LoadGear(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_x[pIndex]);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_y[pIndex]);
    str = strtok(NULL, "\t ,/");
    if (pIndex == 0) {
        pCar_spec->gears_image = LoadPixelmap(str);
    }
}

void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder) {
    if (*pRef_holder >= 0) {
        *pRef_holder += gGroove_funk_offset;
    }
}

// FUNCTION: CARMA2_HW 0x0048bca0
void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location) {
    tDamage_clause* the_clause;
    int i;
    int j;
    int k;
    int count;
    char s[256];
    char delim[64];
    char* str;

    PossibleService();
    count = GetAnInt(pF);
    pCar_spec->damage_programs[pImpact_location].clause_count = count;
    the_clause = BrMemAllocate(count * sizeof(tDamage_clause), kMem_damage_clauses);
    pCar_spec->damage_programs[pImpact_location].clauses = the_clause;
    strcpy(delim, "\t ,/");
    strcat(delim, "&");

    for (i = 0; i < count; i++, the_clause++) {
        the_clause->condition_count = 0;
        GetALineAndDontArgue(pF, s);
        str = strtok(s, delim);
        do {
            if (str[0] == 'x') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_x;
            } else if (str[0] == 'y') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_y;
            } else if (str[0] == 'z') {
                the_clause->conditions[the_clause->condition_count].axis_comp = eAxis_z;
            } else {
                break;
            }
            if (str[1] == '>') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_greater_than;
            } else if (str[1] == '<') {
                the_clause->conditions[the_clause->condition_count].condition_operator = eCondition_less_than;
            } else {
                FatalError(kFatalError_ConfusedByFormatOfConditionalDamageInCarFile);
            }
            sscanf(&str[2], "%f", &the_clause->conditions[the_clause->condition_count].comparitor);
            the_clause->condition_count++;
            str = strtok(NULL, delim);
        } while (the_clause->condition_count < 2);

        the_clause->effect_count = GetAnInt(pF);
        for (j = 0; j < the_clause->effect_count; j++) {
            the_clause->effects[j].type = (tDamage_type)-1;
            GetALineAndDontArgue(pF, s);
            str = strtok(s, "\t ,/");
            for (k = 0; k < (int)REC2_ASIZE(gDamage_names); k++) {
                if (strcmp(str, gDamage_names[k]) == 0) {
                    the_clause->effects[j].type = (tDamage_type)k;
                    break;
                }
            }
            if ((int)the_clause->effects[j].type < 0) {
                FatalError(kFatalError_UnknownDamageType_S, str);
            }
            str = strtok(NULL, "\t ,/");
            sscanf(str, "%f", &the_clause->effects[j].weakness_factor);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0048bf20
intptr_t C2_HOOK_CDECL AllocateCarActorData(br_actor* pActor, void* pData) {

    (void)pData;
    if (pActor->type == BR_ACTOR_MODEL) {
        pActor->user = BrMemAllocate(sizeof(tUser_crush_data), kMem_crush_data);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0048bf50
intptr_t C2_HOOK_CDECL AllocateCarModelData(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;
    tCar_spec* car_spec;
    tUser_detail_level_model* model_data;
    int i;

    car_spec = pData;
    user_crush = pActor->user;
    if (user_crush == NULL) {
        return 0;
    }
    for (i = 0; i < car_spec->count_detail_levels; i++) {
        if (user_crush->models[i] != NULL) {
            model_data = BrMemAllocate(sizeof(tUser_detail_level_model), kMem_crush_data);
            user_crush->models[i]->user = model_data;
        }
    }
    return 0;
}

// AddCarModelToList

// LinkCarModel

// LinkCarModelsToActor

// FUNCTION: CARMA2_HW 0x00488dd0
br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type) {
    int i;
    int material_number;
    int red;
    int green;
    int blue;
    int palette_index;
    br_colour colour;

    if (pName[0] == 'M') {
        strtok(pName, "\t ,/");
        material_number = 10 * ((tS8)pName[1] - '0') + (tS8)pName[2] - '0';
        for (i = 0; gSimple_material_colours[i] != 0; i++) {
            if (material_number == (int)(gSimple_material_colours[i] >> 24)) {
                break;
            }
        }
        if (gSimple_material_colours[i] == 0) {
            PDFatalError("Obsolete simple material");
        }
        colour = gSimple_material_colours[i] & 0xffffff;
    } else {
        sscanf(pName, "%d,%d,%d", &red, &green, &blue);
        colour = (((br_colour)red << 8) | (br_colour)green) << 8 | (br_colour)blue;
    }
    /* Materials with the same colour share the first requested shading. */
    for (i = 0; gSimple_materials[i] != NULL; i++) {
        if (gSimple_materials[i]->colour == colour) {
            return gSimple_materials[i];
        }
    }
    if (i < 32) {
        gSimple_materials[i] = BrMaterialAllocate("Simple");
        gSimple_materials[i]->colour = colour;
        palette_index = FindBestColourMatch(gSimple_materials[i]->colour);
        gSimple_materials[i]->index_base = (br_uint_8)palette_index;
        gSimple_materials[i]->index_range = 0;
        GlorifyMaterial(&gSimple_materials[i], 1, pShading_type);
        BrMaterialAdd(gSimple_materials[i]);
    }
    return gSimple_materials[i];
}

// FUNCTION: CARMA2_HW 0x00488f00
void C2_HOOK_FASTCALL ReadShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec) {
    int i;
    char name[256];
    tRendererShadingType shading;

    pCar_spec->count_shrapnel_materials = GetAnInt(pF);
    for (i = 0; i < pCar_spec->count_shrapnel_materials; i++) {
        GetALineAndDontArgue(pF, name);
        if (pCar_spec == NULL || (int)pCar_spec->driver < eDriver_oppo) {
            shading = kRendererShadingType_Default;
        } else {
            shading = kRendererShadingType_Specular;
        }
        pCar_spec->shrapnel_materials[i] = GetSimpleMaterial(name, shading);
    }
}

void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner) {
    if (pModel != NULL && pModel->nfaces != 0) {
        if (pOwner == OPPONENT_APC_IDX || gAusterity_mode) {
            if (!(pModel->flags & BR_MODF_UPDATEABLE)) {
                return;
            }
            pModel->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        } else {
            pModel->flags |= BR_MODF_DONT_WELD | BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE;
        }
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

br_model* C2_HOOK_FASTCALL FindCarShellModel(br_model** pModels, int pCount) {
    int i;

    for (i = 0; i < pCount; i++) {
        if (strcmp(pModels[i]->identifier, "SHELL") == 0) {
            return pModels[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0048c130
intptr_t C2_HOOK_CDECL LinkCarActorsToGroovidelicsCB(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL) {
        user_crush->groove = ActorsGroove(pActor);
    }
    return 0;
}

void C2_HOOK_FASTCALL LinkCarActorsToGroovidelics(br_actor* pActor) {

    DRActorEnumRecurse(pActor, LinkCarActorsToGroovidelicsCB, NULL);
}

void C2_HOOK_FASTCALL ReadCarKeywordData(tCar_spec* pCar_spec, FILE* pF) {
    int i;
    int j;
    char s[256];

    while (!PFfeof(pF)) {
        GetAString(pF, s);
        if (DRStricmp(s, "END") == 0) {
            break;
        } else if (DRStricmp(s, "CAMERA_POSITIONS") == 0) {
            GetAVector3(pF, &pCar_spec->camera_bumper_position);
            GetAVector3(pF, &pCar_spec->camera_cockpit_position);
        } else if (DRStricmp(s, "CAMERA_TURN_OFF_MATERIALS") == 0) {
            pCar_spec->count_window_materials = GetAnInt(pF);
            for (i = 0; i < pCar_spec->count_window_materials; i++) {
                GetAString(pF, s);
                pCar_spec->window_materials[i].material = BrMaterialFind(s);
                pCar_spec->window_materials[i].count_maps = GetAnInt(pF);
                for (j = 0; j < pCar_spec->window_materials[i].count_maps; j++) {
                    GetAString(pF, s);
                    pCar_spec->window_materials[i].maps[j] = BrMapFind(s);
                }
            }
        }
    }
}

void C2_HOOK_FASTCALL TrashModelMaterialIdentifiers(tCar_spec* pCar_spec, tBrender_storage* pStorage_space) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count && i < pStorage_space->materials_count; i++) {
        material = pStorage_space->materials[i];
        if (material->identifier != NULL && material->identifier[0] != '\0') {
            if (material->flags & BR_MATF_TWO_SIDED) {
                material->identifier[0] = 'D';
            } else {
                material->identifier[0] = 'S';
            }
            BrMaterialUpdate(material, BR_MATU_ALL);
        }
    }
}

void C2_HOOK_FASTCALL CentreModel(br_actor* pActor) {
    br_model* model;
    br_vector3 avg;
    int i;

    model = pActor->model;
    if (model != NULL) {
        avg.v[0] = 0.5f * (model->bounds.min.v[0] + model->bounds.max.v[0]);
        avg.v[1] = 0.5f * (model->bounds.min.v[1] + model->bounds.max.v[1]);
        avg.v[2] = 0.5f * (model->bounds.min.v[2] + model->bounds.max.v[2]);
        /* Only an ordered nonzero centre component triggers adjustment. */
        if (avg.v[0] < 0.0f || avg.v[0] > 0.0f
                || avg.v[1] < 0.0f || avg.v[1] > 0.0f
                || avg.v[2] < 0.0f || avg.v[2] > 0.0f) {
            pActor->t.t.translate.t.v[0] += avg.v[0];
            pActor->t.t.translate.t.v[1] += avg.v[1];
            pActor->t.t.translate.t.v[2] += avg.v[2];
            for (i = 0; i < model->nvertices; i++) {
                model->vertices[i].p.v[0] += -avg.v[0];
                model->vertices[i].p.v[1] += -avg.v[1];
                model->vertices[i].p.v[2] += -avg.v[2];
            }
            BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
        }
    }
}

void C2_HOOK_FASTCALL PivotizeWheel(br_actor* pActor) {
    br_actor* child;

    child = pActor->children;
    if (child != NULL) {
        pActor->t.t.translate.t.v[0] += child->t.t.translate.t.v[0];
        pActor->t.t.translate.t.v[1] += child->t.t.translate.t.v[1];
        pActor->t.t.translate.t.v[2] += child->t.t.translate.t.v[2];
        child->t.t.translate.t.v[0] = 0.0f;
        child->t.t.translate.t.v[1] = 0.0f;
        child->t.t.translate.t.v[2] = 0.0f;
    }
}

void C2_HOOK_FASTCALL SetUpGraphicalWheelStuff(tCar_spec* pCar_spec) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        if (pCar_spec->wheel_actors[i] != NULL) {
            CentreModel(pCar_spec->wheel_actors[i]);
        }
    }
    for (i = 0; i < (int)REC2_ASIZE(pCar_spec->pivot_actors); i++) {
        if (pCar_spec->pivot_actors[i] != NULL) {
            PivotizeWheel(pCar_spec->pivot_actors[i]);
        }
    }
}

// LoadCar

// FUNCTION: CARMA2_HW 0x0048c150
void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gHeadup_image_info); i++) {
        PossibleService();
        if (gHeadup_image_info[i].avail == eNet_or_otherwise
                || (gHeadup_image_info[i].avail == eNot_net && gNet_mode == eNet_mode_none)
                || (gHeadup_image_info[i].avail == eNet_only && gNet_mode != eNet_mode_none)) {
            gHeadup_images[i] = LoadPixelmap(gHeadup_image_info[i].name);
            BRPM_convert(gHeadup_images[i], gBack_screen->type);
        } else {
            gHeadup_images[i] = NULL;
        }
    }
}

FILE* C2_HOOK_FASTCALL OpenRaceFile(void) {
    FILE* f;
    tPath_name the_path;

    PathCat(the_path, gApplication_path, gRaces_file_names[gCurrent_race_file_index]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    return f;
}

// FUNCTION: CARMA2_HW 0x0048c1c0
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {
    FILE* f;
    br_scalar opponent_count_first_race;
    br_scalar opponent_count_last_race;
    br_scalar opponent_softness_first_race;
    br_scalar opponent_hardness_first_race;
    br_scalar opponent_softness_last_race;
    br_scalar opponent_hardness_last_race;
    br_scalar opponent_nastiness_first_race;
    br_scalar opponent_nastiness_last_race;
    tTWTVFS twt;
    int powerup_exclusions[50];
    tRace_group_spec* current_group;
    br_scalar factor;
    br_scalar softness;
    br_scalar hardness;
    int hardness_oppo;
    int count_opponents;
    int i;
    int j;
    char s[256];
    char* str;

    C2_HOOK_BUG_ON(sizeof(tRace_list_spec) != 0x244);
    C2_HOOK_BUG_ON(sizeof(tRace_group_spec) != 12);

    twt = -1;
    if (!gApplicationDataTwtMounted) {
        twt = OpenPackFileAndSetTiffLoading(gApplication_path);
    }
    gCurrent_race_file_index = pRace_type_index + 1;
    f = OpenRaceFile();

    GetAnInt(f);
    gDev_initial_race = 0;
    opponent_count_first_race = GetAScalar(f);
    opponent_count_last_race = GetAScalar(f);
    GetPairOfFloats(f, &opponent_softness_first_race, &opponent_hardness_first_race);
    GetPairOfFloats(f, &opponent_softness_last_race, &opponent_hardness_last_race);
    opponent_nastiness_first_race = GetAScalar(f);
    opponent_nastiness_last_race = GetAScalar(f);

    gCountRaceGroups = 0;
    i = 0;
    for (;;) {
        GetALineAndDontArgue(f, pRace_list[i].name);
        if (strcmp(pRace_list[i].name, "END") == 0) {
            break;
        }
        GetAString(f, pRace_list[i].file_name);
        GetAString(f, pRace_list[i].interface_name);
        pRace_list[i].count_opponents = GetAnInt(f);
        pRace_list[i].count_explicit_opponents = GetAnInt(f);
        for (j = 0; j < pRace_list[i].count_explicit_opponents; j++) {
            pRace_list[i].explicit_opponents[j] = GetAnInt(f);
        }
        pRace_list[i].opponent_nastiness_level = GetAScalar(f);

        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        for (j = 0; str != NULL; j++) {
            sscanf(str, "%d", &powerup_exclusions[j]);
            if (powerup_exclusions[j] < 0) {
                break;
            }
            str = strtok(NULL, "\t ,/");
        }
        pRace_list[i].count_powerup_exclusions = j;
        if (pRace_list[i].count_powerup_exclusions != 0) {
            pRace_list[i].powerup_exclusions = BrMemAllocate(pRace_list[i].count_powerup_exclusions * sizeof(int), kMem_misc);
            memcpy(pRace_list[i].powerup_exclusions, powerup_exclusions, pRace_list[i].count_powerup_exclusions * sizeof(int));
        }

        pRace_list[i].no_time_awards = GetAnInt(f);
        pRace_list[i].is_boundary = GetAnInt(f);
        if (pRace_list[i].is_boundary) {
            gCountRaceGroups++;
        }
        pRace_list[i].race_type = GetAnInt(f);
        GetThreeInts(f, &pRace_list[i].initial_timer[0], &pRace_list[i].initial_timer[1], &pRace_list[i].initial_timer[2]);
        switch (pRace_list[i].race_type) {
        case kRaceType_Carma1:
        case kRaceType_Checkpoints:
            pRace_list[i].count_laps = GetAnInt(f);
            break;
        case kRaceType_Cars:
            pRace_list[i].options.cars.count_opponents = GetAnInt(f);
            for (j = 0; j < pRace_list[i].options.cars.count_opponents; j++) {
                pRace_list[i].options.cars.opponents[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Peds:
            pRace_list[i].options.peds.count_ped_groups = GetAnInt(f);
            for (j = 0; j < pRace_list[i].options.peds.count_ped_groups; j++) {
                pRace_list[i].options.peds.ped_groups[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Smash:
            pRace_list[i].options.smash.var_smash_number = GetAnInt(f);
            pRace_list[i].options.smash.var_smash_target = GetAnInt(f);
            break;
        case kRaceType_SmashNPed:
            pRace_list[i].options.smash_and_peds.var_smash_number = GetAnInt(f);
            pRace_list[i].options.smash_and_peds.var_smash_target = GetAnInt(f);
            pRace_list[i].options.smash_and_peds.ped_group_index = GetAnInt(f);
            break;
        }
        GetThreeInts(f, &pRace_list[i].completion_bonus[0], &pRace_list[i].completion_bonus[1], &pRace_list[i].completion_bonus[2]);
        if (pRace_list[i].race_type == kRaceType_Carma1) {
            GetThreeInts(f, &pRace_list[i].completion_bonus_peds[0], &pRace_list[i].completion_bonus_peds[1], &pRace_list[i].completion_bonus_peds[2]);
            GetThreeInts(f, &pRace_list[i].completion_bonus_opponents[0], &pRace_list[i].completion_bonus_opponents[1], &pRace_list[i].completion_bonus_opponents[2]);
        }
        GetALineAndDontArgue(f, pRace_list[i].description);
        if (DRStricmp(pRace_list[i].description, "none") == 0) {
            pRace_list[i].description[0] = '\0';
        }
        pRace_list[i].expansion = GetAnInt(f) & 1;

        if (pRace_type_index < 0 || pRace_list[i].race_type == kRaceType_Carma1) {
            i++;
        }
    }
    *pCount = i;
    PFfclose(f);
    if (gRace_groups != NULL) {
        BrMemFree(gRace_groups);
    }
    gRace_groups = BrMemAllocate((gCountRaceGroups == 0 ? 1 : gCountRaceGroups) * sizeof(tRace_group_spec), kMem_misc);
    gRace_groups[0].count_races = 0;
    gRace_groups[0].races = pRace_list;
    current_group = &gRace_groups[0];

    for (i = 0; i < *pCount; i++) {
        if (!(pRace_list[i].opponent_nastiness_level > 0.f)) {
            pRace_list[i].opponent_nastiness_level = (br_scalar)(opponent_nastiness_first_race
                + (br_scalar)i / ((br_scalar)*pCount - 1.0) * (opponent_nastiness_last_race - opponent_nastiness_first_race));
        }
        count_opponents = pRace_list[i].count_opponents;
        if (count_opponents < 0) {
            count_opponents = (int)(opponent_count_first_race
                + (br_scalar)i / ((br_scalar)*pCount - 1.0) * (opponent_count_last_race - opponent_count_first_race));
        }
        pRace_list[i].count_opponents = 0;
        /* Fill unspecified opponents with negative difficulty codes. */
        if (pRace_list[i].count_explicit_opponents < count_opponents) {
            factor = (br_scalar)((br_scalar)i / ((br_scalar)*pCount - 1.0));
            softness = opponent_softness_first_race + factor * (opponent_softness_last_race - opponent_softness_first_race);
            hardness = opponent_hardness_first_race + factor * (opponent_hardness_last_race - opponent_hardness_first_race) - softness;
            for (j = pRace_list[i].count_explicit_opponents; j < count_opponents; j++) {
                hardness_oppo = (int)(softness
                    + (br_scalar)(j - pRace_list[i].count_explicit_opponents)
                        / ((br_scalar)(count_opponents - pRace_list[i].count_explicit_opponents) - 1.0) * hardness);
                if (hardness_oppo < 1) {
                    pRace_list[i].explicit_opponents[j] = -1;
                } else if (hardness_oppo > 5) {
                    pRace_list[i].explicit_opponents[j] = -5;
                } else {
                    pRace_list[i].explicit_opponents[j] = -hardness_oppo;
                }
            }
        }
        pRace_list[i].count_explicit_opponents = count_opponents;
        pRace_list[i].group = current_group;
        /* A boundary race is the mission that ends the current group. */
        if (pRace_list[i].is_boundary) {
            current_group->mission = &pRace_list[i];
            current_group++;
            if (current_group - gRace_groups < gCountRaceGroups) {
                current_group->count_races = 0;
                current_group->races = &pRace_list[i + 1];
            }
        } else {
            current_group->count_races++;
        }
    }
    gCurrent_race_group = gRace_groups;
    if (!gApplicationDataTwtMounted) {
        ClosePackFileAndSetTiffLoading(twt);
    }
}

// FUNCTION: CARMA2_HW 0x0048c860
void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info) {
    int i;

    pRace_info->index = pRace_index;
    pRace_info->race_spec = &gRace_list[pRace_index];
    if (gNet_mode == eNet_mode_none) {
        pRace_info->number_of_racers = gRace_list[pRace_index].count_explicit_opponents;
    }
    pRace_info->total_laps = gRace_list[pRace_index].count_laps;
    for (i = 0; i < 3; i++) {
        pRace_info->initial_timer[i] = pRace_info->race_spec->initial_timer[i];
        pRace_info->completion_bonus[i] = pRace_info->race_spec->completion_bonus[i];
        pRace_info->completion_bonus_peds[i] = pRace_info->race_spec->completion_bonus_peds[i];
        pRace_info->completion_bonus_opponents[i] = pRace_info->race_spec->completion_bonus_opponents[i];
    }
}

// FUNCTION: CARMA2_HW 0x0048c930
void C2_HOOK_FASTCALL LoadOpponents(void) {
    FILE* f;
    tPath_name the_path;
    int i;
    int j;
    int k;
    int value;
    char s[256];
    char* str;
    tText_chunk* text_chunks;
    tText_chunk* the_chunk;

    C2_HOOK_BUG_ON(sizeof(tOpponent) != 0x1c0);
    C2_HOOK_BUG_ON(sizeof(tText_chunk) != 52);

    PathCat(the_path, gApplication_path, "OPPONENT.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenOpponentsFile);
    }
    GetALineAndDontArgue(f, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%d", &gNumber_of_racers);
    gOpponents = BrMemAllocate(sizeof(tOpponent) * gNumber_of_racers, kMem_oppo_array);

    for (i = 0; i < gNumber_of_racers; i++) {
        PossibleService();
        GetALineAndDontArgue(f, gOpponents[i].name);
        if (strcmp(gOpponents[i].name, "END") == 0) {
            FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
        }
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        strcpy(gOpponents[i].abbrev_name, str);
        GetALineAndDontArgue(f, gOpponents[i].car_name);
        value = GetAnInt(f);
        gOpponents[i].strength_rating = value;
        value = GetAnInt(f);
        gOpponents[i].price = value;
        value = GetALineAndInterpretCommand(f, gNet_avail_names, (int)REC2_ASIZE(gNet_avail_names));
        gOpponents[i].network_availability = (tNet_avail)value;

        gOpponents[i].mug_shot_image_data = NULL;
        gOpponents[i].grid_icon_image = NULL;
        gOpponents[i].stolen_car_image_data = NULL;
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        strcpy(gOpponents[i].car_file_name, str);
        GetALineAndDontArgue(f, gOpponents[i].line1_topspeed);
        GetALineAndDontArgue(f, gOpponents[i].line2_weight);
        GetALineAndDontArgue(f, gOpponents[i].line3_acceleration);
        GetALineAndDontArgue(f, gOpponents[i].line4_description);

        gOpponents[i].text_chunk_count = 0;
        text_chunks = BrMemAllocate(sizeof(tText_chunk) * gOpponents[i].text_chunk_count, kMem_oppo_text_chunk);
        gOpponents[i].text_chunks = text_chunks;
        the_chunk = gOpponents[i].text_chunks;
        for (j = 0; j < gOpponents[i].text_chunk_count; j++, the_chunk++) {
            PossibleService();
            GetPairOfInts(f, &the_chunk->x_coord, &the_chunk->y_coord);
            GetPairOfInts(f, &the_chunk->frame_cue, &the_chunk->frame_end);
            the_chunk->line_count = GetAnInt(f);
            while (the_chunk->line_count > (int)REC2_ASIZE(the_chunk->text)) {
                the_chunk->line_count--;
                GetALineAndDontArgue(f, s);
            }
            for (k = 0; k < the_chunk->line_count; k++) {
                GetALineAndDontArgue(f, s);
                the_chunk->text[k] = BrMemAllocate(strlen(s) + 1, kMem_oppo_text_str);
                strcpy(the_chunk->text[k], s);
            }
        }
        gOpponents[i].dead = 0;
        InitOpponentPsyche(i);
    }
    GetALineAndDontArgue(f, s);
    if (strcmp(s, "END") != 0) {
        FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x0048cd80
void C2_HOOK_FASTCALL AboutToLoadFirstCar(void) {

    InitFunkGrooveFlags();
    gGroove_funk_offset = 0;
}

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

// FUNCTION: CARMA2_HW 0x0048d0e0
void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race) {

    strcpy(gProgram_state.track_file_name, pThe_race->race_spec->file_name);
}

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
    return 0;
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

// FUNCTION: CARMA2_HW 0x0048e9e0
void C2_HOOK_FASTCALL InitFunkGrooveFlags(void) {
    int i;

    /* Leave flag zero unchanged. */
    for (i = 1; i < (int)REC2_ASIZE(gFunk_groove_flags); i++) {
        gFunk_groove_flags[i] = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0048ea00
void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    gPedTextTxtPath = "TEXT.TXT";
}



// FUNCTION: CARMA2_HW 0x0048ea20
void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void) {
    *(const char* volatile*)&gPedTextTxtPath = "ZOMTEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea10
void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void) {
    *(const char* volatile*)&gPedTextTxtPath = "TEXT.TXT";
}

// FUNCTION: CARMA2_HW 0x0048ea30
void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void) {
    *(const char* volatile*)&gPedTextTxtPath = "ALTEXT.TXT";
}
