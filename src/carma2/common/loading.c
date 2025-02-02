#include "loading.h"

#include "brucetrk.h"
#include "controls.h"
#include "crush.h"
#include "depth.h"
#include "drmem.h"
#include "drone.h"
#include "errors.h"
#include "explosions.h"
#include "globvars.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "loadsave.h"
#include "newgame.h"
#include "opponent.h"
#include "physics.h"
#include "powerups.h"
#include "replay.h"
#include "sound.h"
#include "spark.h"
#include "temp.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_logging.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include "brender/brender.h"
#include "rec2_types.h"
#include "brender/br_types.h"

#define OPPONENT_APC_IDX 98

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5, 0x00691b40);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsFile, gTwatVfsFiles, 50, 0x00692070);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversionStackPos, 0x006923a0);

C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversion, 0x0068c724);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDisableTiffConversionStack, 2, 0x00692068);

C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_cockpit_hither, 0x007634ac);
C2_HOOK_VARIABLE_IMPLEMENT(FILE*, gTempFile, 0x0068c6ec);
C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_angle, 0x0074d354);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadupBackgroundBrightness, 0x0079ec14);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInitial_rank, 0x0074b76c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_per_rank, 3, 0x0074b760);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_per_ped_kill, 3, 0x00763530);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gSeconds_per_unit_car_damage, 3, 0x00761ed0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCredits_per_unit_car_damage, 3, 0x0075b930);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_wasting_car, 3, 0x00763520);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_wasting_car, 3, 0x0074d3b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_rolling_car, 3, 0x0075b9b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_rolling_car, 3, 0x00761a50);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_checkpoint, 3, 0x00762140);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_per_second_time_bonus, 3, 0x00761ef0);  /* FIXME: rename to gPoints_per_second */
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCunning_stunt_bonus, 3, 0x00763500);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gDefaultCar, 32, 0x00764ea0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gBasic_car_name, 32, 0x00764ec0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gGoreLevel, 0x0065d7d4, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAnimalsOn, 0x0065d7c8, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFlameThrowerOn, 0x0065d7cc, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMinesOn, 0x0065d7d0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDronesOff, 0x00684518);

C2_HOOK_VARIABLE_IMPLEMENT(int, gMinTimeOpponentRepair, 0x0074a684);
C2_HOOK_VARIABLE_IMPLEMENT(int, gMaxTimeOpponentRepair, 0x0074a688);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKey_map_index, 0x0068b88c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gKey_mapping, 77, 0x0074b5e0);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAllow_open_to_fail, 0x00658610, 1);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tHeadup_info, gHeadup_image_info, 45, 0x00657320, {
    { "", eNot_ever_ever_ever }
    { "", eNot_ever_ever_ever }
    { "", eNot_ever_ever_ever }
    { "", eNot_ever_ever_ever }
    { "GO.PIX", eNet_or_otherwise }
    { "NUMBER1.PIX", eNet_or_otherwise }
    { "NUMBER2.PIX", eNet_or_otherwise }
    { "NUMBER3.PIX", eNet_or_otherwise }
    { "NUMBER4.PIX", eNet_or_otherwise }
    { "NUMBER5.PIX", eNet_or_otherwise }
    { "SPLATTER.PIX", eNet_or_otherwise }
    { "PILEDRIV.PIX", eNet_or_otherwise }
    { "EXTRASTY.PIX", eNet_or_otherwise }
    { "ARTISTIC.PIX", eNet_or_otherwise }
    { "PLAYWITH.PIX", eNet_or_otherwise }
    { "PUNT.PIX", eNet_or_otherwise }
    { "DROP.PIX", eNet_or_otherwise }
    { "POLITE.PIX", eNet_or_otherwise }
    { "2XCOMBO.PIX", eNet_or_otherwise }
    { "3XCOMBO.PIX", eNet_or_otherwise }
    { "4XCOMBO.PIX", eNet_or_otherwise }
    { "5XCOMBO.PIX", eNet_or_otherwise }
    { "6XCOMBO.PIX", eNet_or_otherwise }
    { "7XCOMBO.PIX", eNet_or_otherwise }
    { "8XCOMBO.PIX", eNet_or_otherwise }
    { "9XCOMBO.PIX", eNet_or_otherwise }
    { "10XCOMBO.PIX", eNet_or_otherwise }
    { "BILLIARD.PIX", eNet_or_otherwise }
    { "POLITE.PIX", eNet_or_otherwise }
    { "HEADON.PIX", eNet_or_otherwise }
    { "DESTROY.PIX", eNet_or_otherwise }
    { "CHECKPNT.PIX", eNet_or_otherwise }
    { "TIMEUP.PIX", eNot_net }
    { "RACEOVER.PIX", eNet_or_otherwise }
    { "UWASTED.PIX", eNet_or_otherwise }
    { "MAD.PIX", eNet_or_otherwise }
    { "GAMEOVER.PIX", eNet_or_otherwise }
    { "UBROKE.PIX", eNet_or_otherwise }
    { "ULOST.PIX", eNet_or_otherwise }
    { "UWON.PIX", eNet_or_otherwise }
    { "BBQBON.PIX", eNet_or_otherwise }
    { "TAILBON.PIX", eNet_or_otherwise }
    { "DETTAILBON.PIX", eNet_or_otherwise }
    { "MISSIONFAIL.PIX", eNet_or_otherwise }
    { "MISSIONDONE.PIX", eNet_or_otherwise }
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gHeadup_images, 45, 0x0079eb60);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char, gDecode_string, 14, 0x00655e38, {    \
    0x9b, 0x52, 0x93, 0x9f, 0x52, 0x98, 0x9b,                                    \
    0x96, 0x96, 0x9e, 0x9b, 0xa0, 0x99, 0x00,
});
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume, gDefault_water_spec_vol, 0x00761b80); /* FIXME: rename to gDefault_default_water_spec_vol */
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume*, gDefault_water_spec_vol_real, 0x00761f44);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gMisc_strings, 300, 0x006b5f40);

C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedsFolder, 0x006a0414);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedSoundPath, 0x00684550);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPowerup_txt_path, 0x006a0ad4);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTextTxtPath, 0x0068c718);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(char*, gPedTexturePath, 0x0065852c, "PIXELMAP");

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, gRaces_file_names, 9, 0x00657530, {
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "PEDRACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT"
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gDrone_type_names, 4, 0x00594770, {
    "CAR",
    "PLANE",
    "TRAIN",
    "CHOPPER",
});

C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_race_file_index, 0x0068c6f4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCountRaceGroups, 0x007634ec);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_group_spec*, gRaceGroups, 0x0068b8a0);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_group_spec*, gRaceGroups2, 0x0074d5e4);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSlot_info, gInitial_APO, 3, 0x0074d4c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSlot_info, gInitial_APO_potential, 3, 0x007622a0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tSlot_info, gMax_APO, 3, 0x0074cfa0);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gCost_APO, 0x00763480);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gTrade_in_value_APO, 0x0075b900);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gSubstitution_value_APO, 0x0074d380);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gPotential_substitution_value_APO, 0x00761c60);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gArmour_starting_value, 100, 0x0074d1c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gPower_starting_value, 100, 0x00761f60);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gOffensive_starting_value, 100, 0x00761d40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_powerup_time_replacement, 0x0074d1a4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gStarting_money, 3, 0x00762110);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gNet_starting_money, 8, 0x00762160);
C2_HOOK_VARIABLE_IMPLEMENT(tFloat_bunch_info, gRepair_cost, 0x00761d00);
C2_HOOK_VARIABLE_IMPLEMENT(tFloat_bunch_info, gRecovery_cost, 0x007634c0);
C2_HOOK_VARIABLE_IMPLEMENT(tFloat_bunch_info, gCar_softness, 0x0075ba20);
C2_HOOK_VARIABLE_IMPLEMENT(tFloat_bunch_info, gCar_car_damage_multiplier, 0x0074d600);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gNet_score_targets, 8, 0x007638c0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPickup_respawn_min_time_ms, 0x007447d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPickup_respawn_max_extra_time_ms, 0x007447e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDemo_race_rank_equivalent, 0x0074b58c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_demo_opponents, 0x0074b4fc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDemo_opponents, 15, 0x0074b4c0);
/* FIXME: this might be a struct */
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_pre_smack, 0x0068b8b4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_post_smack, 0x0068b8b8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_not_in_demo, 0x0068b8bc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_post_demo, 0x0068b8c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gAuto_increase_credits_dt, 3, 0x0074b740);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tU32, gNet_auto_increase_credits_dt, 8, 0x0074b720);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_mutant_tail_parts, 0x007059c0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMass_mutant_tail_link, 0x00705b78);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMass_mutant_tail_ball, 0x00705b74);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gMass_mine, 0x0065ebbc, 1.f);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gUnderwater_screen_name, 32, 0x0068c6f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWasted_explosion_chance, 0x00762120);
C2_HOOK_VARIABLE_IMPLEMENT(int, gExplosion_sound_id, 0x00761f5c);
C2_HOOK_VARIABLE_IMPLEMENT(tExplosion_animation, gExplosion_pix_animation_groups, 0x007620f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gNet_avail_names, 4, 0x00657558, {
    "never",
    "eagle",
    "hawk",
    "all",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gFunk_groove_flags, 30, 0x00763540);
C2_HOOK_VARIABLE_IMPLEMENT(int, gGroove_funk_offset, 0x0074b4a8);

C2_HOOK_VARIABLE_IMPLEMENT(tCar_spec*, gCurrent_car_spec, 0x0074b584);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gCurrent_load_directory, 256, 0x00762180);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gCurrent_load_name, 256, 0x00761a80);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gDamage_names, 12, 0x006574d0, {
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
    "rr_wheel"
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gWheel_actor_names, 6, 0x00657500, {
    "FLWHEEL.ACT",
    "FRWHEEL.ACT",
    "RLWHEEL.ACT",
    "RRWHEEL.ACT",
    "IRLWHEEL.ACT",
    "IRRWHEEL.ACT",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gPivot_actor_names, 6, 0x00657518, {
    "FLPIVOT.ACT",
    "FRPIVOT.ACT",
    "RLPIVOT.ACT",
    "RRPIVOT.ACT",
    "IRLPIVOT.ACT",
    "IRRPIVOT.ACT",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_material*, gSimple_materials, 32, 0x0074b500);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tU32, gSimple_material_colours, 12, 0x006572f0, {
    0x005d0303, 0x02a56e04, 0x03a9a905, 0x08006868,
    0x0d3c3626, 0x0e212121, 0x15284c21, 0x1900326a,
    0x1a93491d, 0x1ec0bfc0, 0x23141414, 0x00000000,
});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, g_source_exists, 0x00658614, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFirst_drone_processing, 0x0058f038, 1);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gTime_stamp_for_this_munging, 0x0079efb8);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrame_period_for_this_munging, 0x0079efc4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDrone_delta_time, 0x0079efc0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gTrack_drone_min_y, 0x0079efbc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gGroove_funk_type_names, 3, 0x00594780, {
    "spinnyGroove",
    "steeringGroove",
    "spinnyFunk",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gAxis_names, 3, 0x00594790, {
    "x",
    "y",
    "z",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gReverseness_type_names, 2, 0x005947a8, {
    "forward",
    "reverse",
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gFunk_speed_control_names, 2, 0x005947a0, {
    "controller",
    "dronespeed",
});

void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void) {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457570, SetDefaultSoundFolderName)

void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void) {
    C2V(gPowerup_txt_path) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c00, SetDefaultPowerupFilename)

void C2_HOOK_FASTCALL SetDefaultTextFileName(void) {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea00, SetDefaultTextFileName)

void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void) {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f7f0, SetDefaultPixelmapFolderName)

void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void) {
    C2V(gPedsFolder) = "PEDS";
    SetDefaultSoundFolderName();
    SetDefaultPowerupFilename();
    SetDefaultTextFileName();
    SetDefaultPixelmapFolderName();
}
C2_HOOK_FUNCTION(0x004d6f10, SetDefaultPedFolderNames)

void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath(void) {
    C2V(gPedSoundPath) = "ZOMSOUND";
}
C2_HOOK_FUNCTION(0x00457590, ConfigureZombiePedSoundPath)

void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {
    C2V(gPowerup_txt_path) = "ZOMPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c20, ConfigureZombiePedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "ZOMTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea20, ConfigureZombiePedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath(void) {
    C2V(gPedTexturePath) = "ZOMPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f810, ConfigureZombiePedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedZombiePaths(void) {
    C2V(gPedsFolder) = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f50, ConfigurePedZombiePaths)

void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath(void) {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457580, ConfigureBloodPedSoundPath)

void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {
    C2V(gPowerup_txt_path) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c10, ConfigureBloodPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea10, ConfigureBloodPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath(void) {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f800, ConfigureBloodPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedBloodPaths(void) {
    C2V(gPedsFolder) = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f30, ConfigurePedBloodPaths)

void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath(void) {
    C2V(gPedSoundPath) = "ALSOUND";
}
C2_HOOK_FUNCTION(0x004575a0, ConfigureAlienPedSoundPath)

void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {
    C2V(gPowerup_txt_path) = "ALPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c30, ConfigureAlienPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "ALTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea30, ConfigureAlienPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath(void) {
    C2V(gPedTexturePath) = "ALPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f820, ConfigureAlienPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedAlienPaths(void) {
    C2V(gPedsFolder) = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f70, ConfigurePedAlienPaths)

int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void) {

    return 2 - C2V(gGoreLevel);
}
C2_HOOK_FUNCTION(0x004d6fc0, GetHowMuchBloodAndSnotToSmearAbout)

void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    C2V(gGoreLevel) = 2 - pNewLevel;
}
C2_HOOK_FUNCTION(0x004d7030, SetGoreLevel)

int C2_HOOK_FASTCALL GetAnimalsOn(void) {

    return C2V(gAnimalsOn);
}
C2_HOOK_FUNCTION(0x004d6fd0, GetAnimalsOn)

void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    C2V(gAnimalsOn) = pNewAnimalsOn;
}
C2_HOOK_FUNCTION(0x004d7000, SetAnimalsOn)

int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void) {

    return C2V(gFlameThrowerOn);
}
C2_HOOK_FUNCTION(0x004d6fe0, IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense)

void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    C2V(gFlameThrowerOn) = pNewFlameThrowerOn;
}
C2_HOOK_FUNCTION(0x004d7010, SetFlameThrowerOn)

int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void) {

    return C2V(gMinesOn);
}
C2_HOOK_FUNCTION(0x004d6ff0, IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells)

void C2_HOOK_FASTCALL SetMinesOn(int pNewMinesOn) {

    C2V(gMinesOn) = pNewMinesOn;
}
C2_HOOK_FUNCTION(0x004d7020, SetMinesOn)

int C2_HOOK_FASTCALL GetDronesOn(void) {

    return !C2V(gDronesOff);
}
C2_HOOK_FUNCTION(0x0044ed00, GetDronesOn)

void C2_HOOK_FASTCALL SetDronesOn(int pNewDronesOn) {

    C2V(gDronesOff) = !pNewDronesOn;
}
C2_HOOK_FUNCTION(0x0044ecf0, SetDronesOn)

void C2_HOOK_FASTCALL StripCRNL(char* line) {
    char* loc;

    loc = c2_strchr(line, '\n');
    if (loc != NULL) {
        *loc = '\0';
    }

    loc = c2_strchr(line, '\r');
    if (loc != NULL) {
        *loc = '\0';
    }
}
C2_HOOK_FUNCTION(0x00490690, StripCRNL)

tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF) {
    tU32 raw_long;

    PFfread(&raw_long, sizeof(raw_long), 1, pF);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048f830, ReadU32)

tU16 C2_HOOK_FASTCALL ReadU16(FILE* pF) {
    tU16 raw_short;

    PFfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048f850, ReadU16)

tU8 C2_HOOK_FASTCALL ReadU8(FILE* pF) {
    tU8 raw_byte;

    PFfread(&raw_byte, sizeof(raw_byte), 1, pF);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048f870, ReadU8)

float C2_HOOK_FASTCALL ReadF32(FILE* pF) {
    float f;

    PFfread(&f, sizeof(f), 1, pF);
    return f;
}
C2_HOOK_FUNCTION(0x0048f890, ReadF32)

tS32 C2_HOOK_FASTCALL ReadS32(FILE* pF) {
    tS32 raw_long;

    PFfread(&raw_long, sizeof(raw_long), 1, pF);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048f8b0, ReadS32)

tS16 C2_HOOK_FASTCALL ReadS16(FILE* pF) {
    tS16 raw_short;

    PFfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048f8d0, ReadS16)

tS8 C2_HOOK_FASTCALL ReadS8(FILE* pF) {
    tS8 raw_byte;

    PFfread(&raw_byte, sizeof(raw_byte), 1, pF);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048f8f0, ReadS8)

void C2_HOOK_FASTCALL WriteU32L(FILE* pF, tU32 pNumber) {
    tU32 raw_long;

    raw_long = pNumber;
    DRfwrite(&raw_long, sizeof(raw_long), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f910, WriteU32L)

void C2_HOOK_FASTCALL WriteU16L(FILE* pF, tU16 pNumber) {
    tU16 raw_short;

    raw_short = pNumber;
    DRfwrite(&raw_short, sizeof(raw_short), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f930, WriteU16L)

void C2_HOOK_FASTCALL WriteU8L(FILE* pF, tU8 pNumber) {
    tU8 raw_byte;

    raw_byte = pNumber;
    DRfwrite(&raw_byte, sizeof(raw_byte), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f960, WriteU8L)

void C2_HOOK_FASTCALL WriteS32L(FILE* pF, tS32 pNumber) {
    tS32 raw_long;

    raw_long = pNumber;
    DRfwrite(&raw_long, sizeof(raw_long), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f980, WriteS32L)

void C2_HOOK_FASTCALL WriteS16L(FILE* pF, tS16 pNumber) {
    tS16 raw_short;

    raw_short = pNumber;
    DRfwrite(&raw_short, sizeof(raw_short), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f9a0, WriteS16L)

void C2_HOOK_FASTCALL WriteS8L(FILE* pF, tS8 pNumber) {
    tS8 raw_byte;

    raw_byte = pNumber;
    DRfwrite(&raw_byte, sizeof(raw_byte), 1, pF);
}
C2_HOOK_FUNCTION(0x0048f9d0, WriteS8L)

void C2_HOOK_FASTCALL SkipBytes(FILE* pF, int pBytes_to_skip) {

    PFfseek(pF, pBytes_to_skip, SEEK_CUR);
}
C2_HOOK_FUNCTION(0x0048f9f0, SkipBytes)

tU32 C2_HOOK_FASTCALL MemReadU32(char** pPtr) {
    tU32 raw_long;

    c2_memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048fa00, MemReadU32)

tU16 C2_HOOK_FASTCALL MemReadU16(char** pPtr) {
    tU16 raw_short;

    c2_memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048fa10, MemReadU16)

tU8 C2_HOOK_FASTCALL MemReadU8(char** pPtr) {
    tU8 raw_byte;

    c2_memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048fa20, MemReadU8)

tS32 C2_HOOK_FASTCALL MemReadS32(char** pPtr) {
    tS32 raw_long;

    c2_memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048fa30, MemReadS32)

tS16 C2_HOOK_FASTCALL MemReadS16(char** pPtr) {
    tS16 raw_short;

    c2_memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048fa40, MemReadS16)

tS8 C2_HOOK_FASTCALL MemReadS8(char** pPtr) {
    tS8 raw_byte;

    c2_memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048fa50, MemReadS8)

void C2_HOOK_FASTCALL MemSkipBytes(char** pPtr, int pBytes_to_skip) {

    *pPtr += pBytes_to_skip;
}
C2_HOOK_FUNCTION(0x0048fa60, MemSkipBytes)

int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount) {
    int i;
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);

    str = c2_strtok(s, "\t ,/");
    if (pCount <= 0) {
        return -1;
    }
    for (i = 0; i < pCount; i++) {
        if (DRStricmp(str, pString_list[i]) == 0) {
            return i;
        }
    }
    return -1;
}
C2_HOOK_FUNCTION(0x0048fa70, GetALineAndInterpretCommand)

int C2_HOOK_FASTCALL GetAnInt(FILE* pF) {
    char s[256];
    int value;
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &value);
    return value;
}
C2_HOOK_FUNCTION(0x0048fb00, GetAnInt)

void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF2);
}
C2_HOOK_FUNCTION(0x0048fdc0, GetPairOfInts)

void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF3);
}
C2_HOOK_FUNCTION(0x0048fe30, GetThreeInts)

void C2_HOOK_FASTCALL GetFourInts(FILE* pF, int* pF1, int* pF2, int* pF3, int* pF4) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF3);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF4);
}
C2_HOOK_FUNCTION(0x00490020, GetFourInts)

float (C2_HOOK_FASTCALL * GetAFloat_original)(FILE* pF);
float C2_HOOK_FASTCALL GetAFloat(FILE* pF) {
#if 0//defined(C2_HOOKS_ENABLED)
    return GetAFloat_original(pF);
#else
    char s[256];
    char *str;
    float f;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", &f);
    return f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004900e0, GetAFloat, GetAFloat_original)

float C2_HOOK_FASTCALL GetAScalar(FILE* pF) {
    return GetAFloat(pF);
}
C2_HOOK_FUNCTION(0x0048fb80, GetAScalar)

void C2_HOOK_FASTCALL GetPairOfScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2) {

    GetPairOfFloats(pF, pS1, pS2);
}
C2_HOOK_FUNCTION(0x00490130, GetPairOfScalars)

void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3) {

    GetThreeFloats(pF, pS1, pS2, pS3);
}
C2_HOOK_FUNCTION(0x004901c0, GetThreeScalars)

void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4) {

    GetFourFloats(pF, pS1, pS2, pS3, pS4);
}
C2_HOOK_FUNCTION(0x00490270, GetFourScalars)

void C2_HOOK_FASTCALL GetFiveScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4, br_scalar* pS5) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pS1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pS2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pS3);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pS4);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pS5);
}
C2_HOOK_FUNCTION(0x00490350, GetFiveScalars)

void C2_HOOK_FASTCALL GetNScalars(FILE* pF, int n, br_scalar* pS) {
    char s[256];
    char* str;
    int i;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < n; i++) {
        float f;
        c2_sscanf(str, "%f", &f);
        pS[i] = f;
        str = c2_strtok(NULL, "\t ,/");
    }
}
C2_HOOK_FUNCTION(0x00490460, GetNScalars)

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
}
C2_HOOK_FUNCTION(0x0048fc20, GetPairOfFloats)

void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF3);
}
C2_HOOK_FUNCTION(0x0048fc90, GetThreeFloats)

void C2_HOOK_FASTCALL GetFourFloats(FILE * pF, float* pF1, float* pF2, float* pF3, float* pF4) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF3);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF4);
}

void (C2_HOOK_FASTCALL * GetAString_original)(FILE* pF, char* pString);
void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString) {
#if 0//defined(C2_HOOKS_ENABLED)
    GetAString_original(pF, pString);
#else
    char s[256];
    char* str;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_strcpy(pString, str);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490630, GetAString, GetAString_original)

void C2_HOOK_FASTCALL AllowOpenToFail(void) {
    C2V(gAllow_open_to_fail) = 1;
}
C2_HOOK_FUNCTION(0x004910b0, AllowOpenToFail)

void C2_HOOK_FASTCALL DisallowOpenToFail(void) {
}
C2_HOOK_FUNCTION(0x004910c0, DisallowOpenToFail)

FILE* OldDRfopen(const char* pFilename, const char* pMode) {
    FILE* fp;

    fp = PFfopen(pFilename, pMode);
    if (fp == NULL) {
        const char* data_pos;
        char source_check[256];
        int source_len;

        if (C2V(gCD_fully_installed)) {
            return NULL;
        }
        if (C2V(g_source_exists) == 1) {
            char general_file[256];
            char paths_file[256];
            char source_check[256];

            c2_strcpy(general_file, "DATA");
            c2_strcat(general_file, C2V(gDir_separator));
            c2_strcat(general_file, "GENERAL.TXT");
            c2_strcpy(paths_file, C2V(gApplication_path));
            c2_strcat(paths_file, C2V(gDir_separator));
            c2_strcat(paths_file, "PATHS.TXT");
            if (!PDCheckDriveExists(paths_file)) {
                C2V(g_source_exists) = 0;
                return NULL;
            }
            fp = PFfopen(paths_file, "rt");
            if (fp == NULL) {
                C2V(g_source_exists) = 0;
                return NULL;
            }
            source_check[0] = '\0';
            PossibleService();
            GetALineWithNoPossibleService(fp, source_check);
            if (c2_strchr(C2V(gDir_separator), source_check[c2_strlen(source_check)-1]) == NULL) {
                c2_strcat(source_check, C2V(gDir_separator));
            }
            c2_strcat(source_check, general_file);
            PFfclose(fp);
            if (!PDCheckDriveExists(source_check)) {
                PDFatalError("Carmageddon CD not in drive.");
            }
            C2V(g_source_exists)++;
        }
        if (C2V(g_source_exists) == 0) {
            return NULL;
        }
        data_pos = c2_strstr(pFilename, "DATA");
        if (data_pos == NULL) {
            return NULL;
        }

        source_len = sizeof(source_check) - 1;
        if (GetRegisterSourceLocation(source_check, &source_len)) {
            if (c2_strchr(C2V(gDir_separator), source_check[c2_strlen(source_check)-1]) == NULL) {
                c2_strcat(source_check, C2V(gDir_separator));
            }
            c2_strcat(source_check, data_pos);
            if (!PDCheckDriveExists(source_check)) {
                return NULL;
            }
            return PFfopen(source_check, pMode);
        }
        return NULL;
    } else {
        int len;
        char ch;

        if (!C2V(gDecode_thing)) {
            return fp;
        }
        len = c2_strlen(pFilename);
        if (c2_strcmp(&pFilename[len - 4], ".TXT") != 0
                || c2_strcmp(&pFilename[len - 12], "DKEYMAP0.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "DKEYMAP1.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "DKEYMAP2.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "DKEYMAP3.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "KEYMAP_0.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "KEYMAP_1.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "KEYMAP_2.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "KEYMAP_3.TXT") == 0
                || c2_strcmp(&pFilename[len - 11], "OPTIONS.TXT") == 0
                || c2_strcmp(&pFilename[len - 12], "KEYNAMES.TXT") == 0
                || c2_strcmp(&pFilename[len - 10], "KEYMAP.TXT") == 0
                || c2_strcmp(&pFilename[len - 9], "PATHS.TXT") == 0
                || c2_strcmp(&pFilename[len - 11], "PRATCAM.TXT") == 0) {
            return fp;
        }
        ch = DRfgetc(fp);
        if (ch == C2V(gDecode_thing)) {
            DRungetc(ch, fp);
            return fp;
        }
        PFfclose(fp);
        return NULL;
    }
}

FILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if 0//defined(C2_HOOKS_ENABLED)
    FILE* res = DRfopen_original(pFilename, pMode);
    return res;
#else
    FILE *result;

    result = OldDRfopen(pFilename, pMode);

    if (result == NULL && !C2V(gAllow_open_to_fail)) {
        char buffer[256];
        int buffer_size = sizeof(buffer) - 1;

        if (GetRegisterSourceLocation(buffer, &buffer_size)) {
            if (!PDCheckDriveExists(buffer)) {
                if (C2V(gMisc_strings)[0] != NULL) {
                    PDFatalError(GetMiscString(243));
                } else {
                    PDFatalError("Could not find the Carmageddon CD");
                }
            }
        }
        sprintf(buffer, "DRfopen( \"%s\", \"%s\" ) failed", pFilename, pMode);
        PDFatalError(buffer);
    }
    return result;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491170, DRfopen, DRfopen_original)

void (C2_HOOK_FASTCALL * PFfclose_original)(FILE* pFile);
void C2_HOOK_FASTCALL PFfclose(FILE* pFile) {
#if 0//defined(C2_HOOKS_ENABLED)
    DRfclose_original(pFile);
#else
    if ((uintptr_t)pFile > REC2_ASIZE(C2V(gTwatVfsFiles))) {
        c2_fclose(pFile);
    } else {
        C2V(gTwatVfsFiles)[(uintptr_t)pFile].start = NULL;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4760, PFfclose, PFfclose_original)

br_size_t (C2_HOOK_FASTCALL * DRfread_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfread_original(buf, size, n, f);
#else
    tTwatVfsFile* twtFile;
    int totalSize;

    if ((int)f < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)f];
        totalSize = size * n;
        if (twtFile->end - twtFile->pos < totalSize) {
            n = ((twtFile->end - twtFile->pos) / size) * size;
            totalSize = size * n;
        }
        c2_memcpy(buf, twtFile->pos, totalSize);
        twtFile->pos += totalSize;
        twtFile->error = 0;
        return n;
    }
    return c2_fread(buf, size, n, f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b49f0, PFfread, DRfread_original)

br_size_t (C2_HOOK_FASTCALL * DRfwrite_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfwrite_original(buf, size, n, f);
#else

    if ((int)f < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        FatalError(kFatalError_WriteAttemptToPackedFile_S, "unknown");
    }
    return c2_fwrite(buf, size, n, f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4a80, DRfwrite, DRfwrite_original)

int (C2_HOOK_FASTCALL * DRfgetpos_original)(FILE*, c2_fpos_t*);
int C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, c2_fpos_t* pos) {
#if 0 // efined(C2_HOOKS_ENABLED)
    return DRfgetpos_original(pFile, pos);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        *(tU8 **) pos = twtFile->pos;
        twtFile->error = 0;
        return 0;
    }
    return c2_fgetpos(pFile, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4ac0, DRfgetpos, DRfgetpos_original)

int (C2_HOOK_FASTCALL * PFfeof_original)(FILE* pFile);
int C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return PFfeof_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        return twtFile->pos >= twtFile->end;
    }
    return c2_feof(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c10, PFfeof, PFfeof_original)

int (C2_HOOK_FASTCALL * DRferror_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRferror(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfeof_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        return twtFile->error;
    }
    return c2_ferror(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c40, DRferror, DRferror_original)

void (C2_HOOK_FASTCALL * DRclearerr_original)(FILE* pFile);
void C2_HOOK_FASTCALL DRclearerr(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    CR_clearerr_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        twtFile->error = 0;
        return;
    }
    c2_clearerr(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c60, DRclearerr, DRclearerr_original)

int (C2_HOOK_FASTCALL * DRfgetc_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfgetc(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgetc_original(pFile);
#else
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        if (twtFile->pos >= twtFile->end) {
            twtFile->error = -1;
            return EOF;
        }
        twtFile->error = 0;
        result = *twtFile->pos;
        twtFile->pos++;
        return result;
    }
    return c2_fgetc(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4880, DRfgetc, DRfgetc_original)

int (C2_HOOK_FASTCALL * DRfgetc2_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgetc_original(pFile);
#else
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        if (twtFile->pos >= twtFile->end) {
            twtFile->error = -1;
            return EOF;
        }
        twtFile->error = 0;
        result = *twtFile->pos;
        twtFile->pos++;
        return result;
    }
    return c2_fgetc(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b48c0, DRfgetc2, DRfgetc2_original)

int (C2_HOOK_FASTCALL * DRungetc_original)(int ch, FILE* file);
int C2_HOOK_FASTCALL DRungetc(int ch, FILE* file) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRungetc_original(ch, file);
#else
    tTwatVfsFile* twtFile;

    if ((int)file < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)file];
        twtFile->pos--;
        *twtFile->pos = ch;
        twtFile->error = 0;
        return ch;
    }
    return c2_ungetc(ch, file);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b49a0, DRungetc, DRungetc_original)

char* (C2_HOOK_FASTCALL * PFfgets_original)(char* buffer, br_size_t size, FILE* pFile);
char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgets_original(buffer, size, pFile);
#else
    tTwatVfsFile* twtFile;
    char c;
    char* b;
    size_t i;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        b = buffer;
        for (i = 0; i < size; i++) {
            c = (char)*twtFile->pos;
            twtFile->pos++;
            if (c == -1) {
                *b = '\0';
                twtFile->error = -1;
                return NULL;
            }
            *b = c;
            b++;
            // FIXME: move this check above?
            if (c == '\n' || twtFile->pos >= twtFile->end) {
                break;
            }
        }
        *b = '\0';
        twtFile->error = 0;
        return buffer;
    }
    return c2_fgets(buffer, size, pFile);

#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4900, PFfgets, PFfgets_original)

int (C2_HOOK_FASTCALL * DRfseek_original)(FILE* pF, int offset, int whence);
int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {

#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfseek_original(pF, offset, whence);
#else
    tTwatVfsFile* twtFile;
    tU8 *newpos;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pF];
        switch (whence) {
        case SEEK_SET:
            newpos = twtFile->start + offset;
            break;
        case SEEK_CUR:
            newpos = twtFile->pos + offset;
            break;
        case SEEK_END:
            newpos = twtFile->end + offset;
            break;
        default:
            abort(); // FIXME: use better failure function
            break;
        }
        if (twtFile->start <= newpos && newpos <= twtFile->end) {
            twtFile->pos = newpos;
            twtFile->error = 0;
            return 0;
        } else {
            twtFile->error = -1;
        }
    }
    return c2_fseek(pF, offset, whence);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b70, PFfseek, DRfseek_original)

int (C2_HOOK_FASTCALL * DRftell_original)(FILE* pF);
int C2_HOOK_FASTCALL PFftell(FILE* pF) {

#if 0 //defined(C2_HOOKS_ENABLED)
    return DRftell_original(pF);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pF];
        return twtFile->pos - twtFile->start;
    }
    return c2_ftell(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b00, PFftell, DRftell_original)

int (C2_HOOK_FASTCALL * DRfsetpos_original)(FILE* pF, c2_fpos_t* pos);
int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, c2_fpos_t* pos) {

#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfsetpos_original(pF, pos);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pF];
        if (twtFile->start <= *(tU8**)pos && *(tU8**)pos <= twtFile->end) {
            twtFile->pos = *(tU8**)pos;
            twtFile->error = 0;
            return 0;
        }
        twtFile->error = -1;
        return -1;
    }
    return c2_fsetpos(pF, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b30, DRfsetpos, DRfsetpos_original)

void (C2_HOOK_FASTCALL * DRrewind_original)(FILE* pF);
void C2_HOOK_FASTCALL PFrewind(FILE* pF) {

#if 0 //defined(C2_HOOKS_ENABLED)
    return DRrewind_original(pF);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pF];
        twtFile->pos = twtFile->start;
        twtFile->error = 0;
        return;
    }
    c2_rewind(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4be0, PFrewind, DRrewind_original)

void C2_HOOK_FASTCALL InitPackFiles(void) {
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 1; i < REC2_ASIZE(C2V(gTwatVfsFiles)); i++) {
        C2V(gTwatVfsFiles)[i].start = NULL;
    }
    for (i = 0; i < REC2_ASIZE(C2V(gTwatVfsMountPoints)); i++) {
        C2V(gTwatVfsMountPoints)[i].header = NULL;
    }

    C2V(gDisableTiffConversionStackPos) = 0;
}
C2_HOOK_FUNCTION(0x004b4570, InitPackFiles)

tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    tU32 fileSize;
    tU32 i;

    // file header must be 56 bytes for compatibility with .TWT files
    REC2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    c2_strcpy(twatFilePath, path);
    c2_strcat(twatFilePath, ".TWT");

    f = c2_fopen(twatFilePath, "rb");
    if (f == NULL) {
        return -1;
    }
    for (twt = 0; ; twt++) {
        if (twt >= REC2_ASIZE(C2V(gTwatVfsMountPoints))) {
            c2_fclose(f);
            return -1;
        }
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            break;
        }
    }
    c2_strcpy(C2V(gTwatVfsMountPoints)[twt].path, path);
    fileSize = ReadU32(f);
    c2_rewind(f);

    C2V(gTwatVfsMountPoints)[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
    C2V(gTwatVfsMountPoints)[twt].data = (tU8*)C2V(gTwatVfsMountPoints)[twt].header;
    c2_fread(C2V(gTwatVfsMountPoints)[twt].header, fileSize, 1, f);
    SplungeSomeData(C2V(gTwatVfsMountPoints)[twt].header, fileSize);
    c2_fclose(f);

    tU8* fileData = (tU8*)&C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[C2V(gTwatVfsMountPoints)[twt].header->nbFiles];
    for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
        C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data = fileData;
        fileData += (C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
    }
    return twt;
}
C2_HOOK_FUNCTION(0x004b45b0, OpenPackFile)

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt) {
    if (twt >= 0) {
        BrMemFree(C2V(gTwatVfsMountPoints)[twt].header);
        C2V(gTwatVfsMountPoints)[twt].header = NULL;
    }
}
C2_HOOK_FUNCTION(0x004b4730, TWT_Unmount)

FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode) {
    int twt;
    unsigned int i;
    int file_index;
    size_t twt_path_len;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsFile) != 16);
    C2_HOOK_BUG_ON(sizeof(tTwatFileHeader) != 56);

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            continue;
        }
        if (c2_strstr(pPath, C2V(gTwatVfsMountPoints)[twt].path) != pPath) {
            continue;
        }
        twt_path_len = c2_strlen(C2V(gTwatVfsMountPoints)[twt].path);
        for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
            if (DRStricmp(C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename, &pPath[twt_path_len + 1]) == 0) {
                for (file_index = 1; file_index < REC2_ASIZE(C2V(gTwatVfsFiles)); file_index++) {
                    if (C2V(gTwatVfsFiles)[file_index].start != NULL) {
                        continue;
                    }
                    C2V(gTwatVfsFiles)[file_index].start = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].pos = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].end = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data + C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize;
                    C2V(gTwatVfsFiles)[file_index].error = 0;
                    return (FILE*)file_index;
                }
                return NULL;
            }
        }
    }
    return c2_fopen(pPath, mode);
}
C2_HOOK_FUNCTION(0x004b4780, PFfopen)

void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine) {
    tPath_name the_path;
    tTWTVFS twt;

    PathCat(the_path, pThe_path, pArchive_name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    PFForEveryFile(the_path, pAction_routine);
    ClosePackFileAndSetTiffLoading(twt);
}
C2_HOOK_FUNCTION(0x0048f360, LoadInFiles)

void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    int twt;
    unsigned int i;
    char buffer[256];

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            continue;
        }
        if (DRStricmp(pThe_path, C2V(gTwatVfsMountPoints)[twt].path) != 0) {
            continue;
        }
        for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
            PathCat(buffer, pThe_path, C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename);
            pAction_routine(buffer);
        }
        return;
    }
    PDForEveryFile(pThe_path, pAction_routine);
}
C2_HOOK_FUNCTION(0x004b4c80, PFForEveryFile)

void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data) {
    int twt;
    size_t i;
    tPath_name twt_filePath;

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header != NULL && DRStricmp(C2V(gTwatVfsMountPoints)[twt].path, path) == 0) {
            for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
                PathCat(twt_filePath, path, C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename);
                pCallback(twt_filePath, data);
            }
            return;
        }
    }
    PDEnumPath(path, pCallback, data);
}
C2_HOOK_FUNCTION(0x004b4d30, PFForEveryFile2)

tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path) {
    tTWTVFS res;

    res = OpenPackFile(path);
    if (TWT_MOUNT_SUCCEEDED(res)) {
        C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversionStackPos)++;
        C2V(gDisableTiffConversion) = 1;
    }
    return res;
}
C2_HOOK_FUNCTION(0x004b4df0, OpenPackFileAndSetTiffLoading)

void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt) {
    if (twt >= 0) {
        C2V(gDisableTiffConversionStackPos)--;
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
        TWT_Unmount(twt);
    }
}
C2_HOOK_FUNCTION(0x004b4e20, ClosePackFileAndSetTiffLoading)

void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void) {
    int count;

    count = C2V(gDisableTiffConversionStackPos);
    if (count != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversionStack)[count] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[count - 1];
    }
}
C2_HOOK_FUNCTION(0x004b4e60, PackFileRevertTiffLoading)

void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void) {

    if (C2V(gDisableTiffConversionStackPos) != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
    }
}
C2_HOOK_FUNCTION(0x004b4e90, PackFileRerevertTiffLoading)

void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2) {

}
C2_HOOK_FUNCTION(0x005193f0, EnsurePixelmapAllowed)

br_pixelmap* (C2_HOOK_FASTCALL * LoadPixelmap_original)(const char* pPath_name);
br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name) {
#if 0//defined(C2_HOOKS_ENABLED)
    return DRLoadPixelmap_original(pPath_name);
#else
    br_pixelmap *pm;

    pm = RealLoadPixelmap(pPath_name);
    EnsurePixelmapAllowed(pm, 0);
    return pm;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ec00, LoadPixelmap, LoadPixelmap_original)

br_pixelmap* (C2_HOOK_FASTCALL * RealLoadPixelmap_original)(const char* pPath_name);
br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name) {
#if 0//defined(C2_HOOKS_ENABLED)
    return DRLoadPixelmap2_original(pPath_name);
#else
    const char* ext;
    tPath_name path;
    char texture_name[256];
    int error;
    br_pixelmap* pixelmaps[1000];
    br_pixelmap* pm;

    ext = c2_strrchr(pPath_name, '.');
    if (ext == NULL) {
        ext = pPath_name + c2_strlen(pPath_name);
    }
    if (ext - pPath_name == 4) {
        if (c2_strncmp(pPath_name, "none", 4) == 0) {
            return NULL;
        }
    }
    PossibleService();
    PathCat(path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(path, path, "PIXELMAP");
    c2_strcpy(texture_name, pPath_name);
    texture_name[c2_strlen(texture_name) - 4] = '\0';
    AllowOpenToFail();
    if (!C2V(gDisableTiffConversion)) {
        pm = LoadTiffTexture_Ex(path, texture_name, C2V(gRender_palette), C2V(gPixelFlags) | 0x40, &error);
    } else {
        if (LoadBunchOfPixies(path, texture_name, pixelmaps, REC2_ASIZE(pixelmaps)) != 0) {
            error = 0;
            pm = pixelmaps[0];
        } else {
            error = 1;
            pm = NULL;
        }
    }
    DisallowOpenToFail();
    if (pm == NULL) {
        PathCat(path, C2V(gApplication_path), C2V(gPedTexturePath));
        if (!C2V(gDisableTiffConversion)) {
            pm = LoadTiffTexture_Ex(path, texture_name, C2V(gRender_palette), C2V(gPixelFlags) | 0x40, &error);
        } else {
            if (LoadBunchOfPixies(path, texture_name, pixelmaps, REC2_ASIZE(pixelmaps)) != 0) {
                pm = pixelmaps[0];
            } else {
                pm = NULL;
            }
        }
    }
    return pm;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ec20, RealLoadPixelmap, RealLoadPixelmap_original)

void C2_HOOK_FASTCALL GetHithers(void) {
    char s[256];
    int result;
    int position;

    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(C2V(gTempFile), s);
    result = c2_sscanf(&s[c2_strspn(s, "\t ,")], "%f%n", &C2V(gCamera_hither), &position);
    if (result == 0) {
        FatalError(kFatalError_MysteriousX_SS, s, "GENERAL.TXT");
    }
    c2_sscanf(&s[position + c2_strspn(&s[position], "\t ,")], "%f", &C2V(gCamera_cockpit_hither));
    C2V(gCamera_hither) *= 2;
    C2V(gCamera_cockpit_hither) *= 2;
}

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info) {
    char s[256];
    const char *str;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pSlot_info->initial) != 3);
    C2_HOOK_BUG_ON(REC2_ASIZE(pSlot_info->initial_network) != 8);

    /* (armour|power|offensive), single player, each skill level */
    GetThreeInts(C2V(gTempFile), &pSlot_info->initial[0], &pSlot_info->initial[1], &pSlot_info->initial[2]);
    /* (armour|power|offensive), each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pSlot_info->initial_network); i++) {
        c2_sscanf(str, "%d", &pSlot_info->initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
}

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch) {
    char s[256];
    const char *str;
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(pBunch->initial) != 3);
    C2_HOOK_BUG_ON(REC2_ASIZE(pBunch->initial_network) != 8);

    GetThreeFloats(C2V(gTempFile), &pBunch->initial[0], &pBunch->initial[1], &pBunch->initial[2]);
    GetALineAndDontArgue(C2V(gTempFile), s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pBunch->initial_network); i++) {
        c2_sscanf(str, "%f", &pBunch->initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
}

void (C2_HOOK_FASTCALL * LoadGeneralParameters_original)(void);
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
#if 0 //defined(C2_HOOKS_ENABLED)
    LoadGeneralParameters_original();
#else
    tPath_name the_path;
    size_t i;
    char* str;
    char s[256];
    char s2[256];
    br_scalar armour_mult, power_mult, offensive_mult;
    int time;

    PathCat(the_path, C2V(gApplication_path), "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    C2V(gTempFile) = PFfopen(the_path, "rb");
    if (C2V(gTempFile) != NULL) {
        PFfgets(s, REC2_ASIZE(s)-1, C2V(gTempFile));
        PFfclose(C2V(gTempFile));

        for (i = 0; i < c2_strlen(C2V(gDecode_string)); i++) {
            C2V(gDecode_string)[i] -= 50;
        }

        // trim trailing CRLF etc
        while (s[0] != '\0' && s[c2_strlen(s) - 1] < 0x20) {
            s[c2_strlen(s) - 1] = 0;
        }

        if (c2_strcmp(s, C2V(gDecode_string)) == 0) {
            C2V(gDecode_thing) = 0;
        }

        for (i = 0; i < c2_strlen(C2V(gDecode_string)); i++) {
            C2V(gDecode_string)[i] += 50;
        }
    }
    PathCat(the_path, C2V(gApplication_path), "GENERAL.TXT");
    C2V(gTempFile) = DRfopen(the_path, "rt");
    if (C2V(gTempFile) == NULL) {
        FatalError(kFatalError_FailToOpenGeneralSettings);
    }

    /* Disable TIFF conversion */
    C2V(gDisableTiffConversion) = GetAnInt(C2V(gTempFile));

    GetHithers();

    /* Yon */
    C2V(gCamera_yon) = GetAFloat(C2V(gTempFile));
    /* Camera angle */
    C2V(gCamera_angle) = GetAFloat(C2V(gTempFile));
    /* Headup background brightness amount */
    C2V(gHeadupBackgroundBrightness) = GetAnInt(C2V(gTempFile));
    /* Initial rank */
    C2V(gInitial_rank) = GetAnInt(C2V(gTempFile));
    /* Credits per rank for each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_per_rank)[0], &C2V(gCredits_per_rank)[1], &C2V(gCredits_per_rank)[2]);

    ReadCrushSettings(C2V(gTempFile));

    /* Time per ped kill for each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gTime_per_ped_kill)[0], &C2V(gTime_per_ped_kill)[1], &C2V(gTime_per_ped_kill)[2]);
    /* Seconds per unit car damage for each skill level (with peds */
    GetThreeFloats(C2V(gTempFile), &C2V(gSeconds_per_unit_car_damage)[0], &C2V(gSeconds_per_unit_car_damage)[1], &C2V(gSeconds_per_unit_car_damage)[2]);
    /* Credits per unit car damage for each skill level (with peds) */
    GetThreeFloats(C2V(gTempFile), &C2V(gCredits_per_unit_car_damage)[0], &C2V(gCredits_per_unit_car_damage)[1], &C2V(gCredits_per_unit_car_damage)[2]);
    /* Time awarded for wasting car for each skill level (with peds) */
    GetThreeInts(C2V(gTempFile), &C2V(gTime_wasting_car)[0], &C2V(gTime_wasting_car)[1], &C2V(gTime_wasting_car)[2]);
    /* Credits awarded for wasting car for each skill level (with peds) */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_wasting_car)[0], &C2V(gCredits_wasting_car)[1], &C2V(gCredits_wasting_car)[2]);
    /* Time awarded for rolling car for each skill level (with peds) */
    GetThreeInts(C2V(gTempFile), &C2V(gTime_rolling_car)[0], &C2V(gTime_rolling_car)[1], &C2V(gTime_rolling_car)[2]);
    /* Credits awarded for rolling car for each skill level (with peds) */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_rolling_car)[0], &C2V(gCredits_rolling_car)[1], &C2V(gCredits_rolling_car)[2]);
    /* Credits awarded for checkpoints for each skill level (with peds) */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_checkpoint)[0], &C2V(gCredits_checkpoint)[1], &C2V(gCredits_checkpoint)[2]);
    /* Jump start fine for each level */
    GetThreeInts(C2V(gTempFile), &C2V(gJump_start_fine)[0], &C2V(gJump_start_fine)[1], &C2V(gJump_start_fine)[2]);
    /* Credits per second of time bonus */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_per_second_time_bonus)[0], &C2V(gCredits_per_second_time_bonus)[1], &C2V(gCredits_per_second_time_bonus)[2]);
    /* Cunning stunt bonus for each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gCunning_stunt_bonus)[0], &C2V(gCunning_stunt_bonus)[1], &C2V(gCunning_stunt_bonus)[2]);

    /* Cars to use as defaults: */
    GetAString(C2V(gTempFile), C2V(gDefaultCar));
    GetAString(C2V(gTempFile), C2V(gBasic_car_name));

    C2V(gOpponent_nastyness_frigger) = 1.f;
    C2V(gKnobbled_frame_period) = 0;
    /* Min time in secs after last contact with play before opponent considers repairing */
    C2V(gMinTimeOpponentRepair) = (int)GetAScalar(C2V(gTempFile));
    /* Max time in secs after last contact with play before opponent considers repairing */
    C2V(gMaxTimeOpponentRepair) = (int)GetAScalar(C2V(gTempFile));

    /* Default underwater special volume parameters */
    ParseSpecialVolume(C2V(gTempFile), &C2V(gDefault_water_spec_vol), C2V(gUnderwater_screen_name), 0);

    /* Initial armour, single player, each skill level */
    /* Initial armour, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO)[0]);

    /* Initial power, single player, each skill level */
    /* Initial power, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO)[1]);

    /* Initial offensive, single player, each skill level */
    /* Initial offensive, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO)[2]);

    /* Initial potential armour, single player, each skill level */
    /* Initial potential armour, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO_potential)[0]);

    /* Initial potential power, single player, each skill level */
    /* Initial potential power, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO_potential)[1]);

    /* Initial potential offensive, single player, each skill level */
    /* Initial potential offensive, each network game type */
    LoadBunchOParameters(&C2V(gInitial_APO_potential)[2]);

    /* Max armour, single player, each skill level */
    /* Max armour, each network game type */
    LoadBunchOParameters(&C2V(gMax_APO)[0]);

    /* Max power, single player, each skill level */
    /* Max power, each network game type */
    LoadBunchOParameters(&C2V(gMax_APO)[1]);

    /* Max offensive, single player, each skill level */
    /* Max offensive, each network game type */
    LoadBunchOParameters(&C2V(gMax_APO)[2]);

    /* APO cost, single player */
    /* APO cost, each network game type */
    LoadBunchOParameters(&C2V(gCost_APO));

    /* Trade-in APO value, single player */
    /* Trade-in APO value, each network game type */
    LoadBunchOParameters(&C2V(gTrade_in_value_APO));

    /* APO substitution value, single player */
    /* APO substitution, each network game type */
    LoadBunchOParameters(&C2V(gSubstitution_value_APO));

    /* APO potential substitution value, single player */
    /* APO potential substitution, each network game type */
    LoadBunchOParameters(&C2V(gPotential_substitution_value_APO));

    /* Armour starting value */
    C2V(gArmour_starting_value)[0] = GetAScalar(C2V(gTempFile));
    /* Power starting value */
    C2V(gPower_starting_value)[0] = GetAScalar(C2V(gTempFile));
    /* Offensive starting value */
    C2V(gOffensive_starting_value)[0] = GetAScalar(C2V(gTempFile));
    /* Armour per-level multiplier */
    armour_mult = GetAScalar(C2V(gTempFile));
    /* Power per-level multiplier */
    power_mult = GetAScalar(C2V(gTempFile));
    /* Offensive per-level multiplier */
    offensive_mult = GetAScalar(C2V(gTempFile));
    for (i = 1; i < REC2_ASIZE(C2V(gArmour_starting_value)); i++) {
        C2V(gArmour_starting_value)[i] = C2V(gArmour_starting_value)[i - 1] * armour_mult;
        C2V(gPower_starting_value)[i] = C2V(gPower_starting_value)[i - 1] * power_mult;
        C2V(gOffensive_starting_value)[i] = C2V(gOffensive_starting_value)[i - 1] * offensive_mult;
    }

    /* Powerup number to use when time powerup got during network game */
    C2V(gNet_powerup_time_replacement) = GetAnInt(C2V(gTempFile));

    /* Starting money for each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gStarting_money)[0], &C2V(gStarting_money)[1], &C2V(gStarting_money)[2]);
    /* Starting money in network mode */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
#if defined(REC2_FIX_BUGS)
    for (i = 0; i < REC2_ASIZE(C2V(gNet_starting_money)); i++) {
#else
    for (i = 0; i < 5; i++) {
#endif
        c2_sscanf(str, "%d", &C2V(gNet_starting_money)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Repair cost for each skill level (cred per % damage) */
    /* Repair cost for each net game (cred per % damage) */
    LoadBunchOFloatParameters(&C2V(gRepair_cost));

    /* Recovery cost for each skill level */
    /* Recovery cost for each net game type */
    LoadBunchOFloatParameters(&C2V(gRecovery_cost));

    /* Car softness factor for each net skill level */
    /* Car softness factor for each net game type */
    LoadBunchOFloatParameters(&C2V(gCar_softness));

    /* Car-to-car damage multiplier for each net skill level */
    /* Car-to-car damage multiplier for each net game type */
    LoadBunchOFloatParameters(&C2V(gCar_car_damage_multiplier));

    /* Score targets for each net game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_score_targets)); i++) {
        c2_sscanf(str, "%d", &C2V(gNet_score_targets)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Pickup respawn min time (seconds) */
    C2V(gPickup_respawn_min_time_ms) = 1000 * GetAnInt(C2V(gTempFile));
    /* Pickup respawn max extra time (seconds) */
    C2V(gPickup_respawn_max_extra_time_ms) = 100 * GetAnInt(C2V(gTempFile));

    /* Demo race rank equivalent */
    C2V(gDemo_race_rank_equivalent) = GetAnInt(C2V(gTempFile));

    /* Number of demo opponents */
    C2V(gCount_demo_opponents) = GetAnInt(C2V(gTempFile));
    /* Demo opponents */
    for (i = 0; (int)i < C2V(gCount_demo_opponents); i++) {
        C2V(gDemo_opponents)[i] = GetAnInt(C2V(gTempFile));
    }

    /* default Gravity Multiplier */
    C2V(gDefault_gravity) = GetAScalar(C2V(gTempFile));

    /* Flic sound delays */
    /* Delay (in seconds) before sound during pre-smack flic */
    C2V(gFlic_sound_delay_pre_smack) = GetAFloat(C2V(gTempFile));
    /* Delay (in seconds) before sound during post-smack flic */
    C2V(gFlic_sound_delay_post_smack) = GetAFloat(C2V(gTempFile));
    /* Delay (in seconds) before sound during 'not in demo' flic */
    C2V(gFlic_sound_delay_not_in_demo) = GetAFloat(C2V(gTempFile));
    /* Delay (in seconds) before sound during post-demo slideshow flic */
    C2V(gFlic_sound_delay_post_demo) = GetAFloat(C2V(gTempFile));

    /* Time (in seconds) that credits take before they reach the recovery
     * amount when self-increasing (if starting at zero)
     *
     * First line is for single-player games, second is for each type of
     * network game. Zero means don't tick up.
     */
    time = GetAnInt(C2V(gTempFile));
    for (i = 0; (int)i < REC2_ASIZE(C2V(gAuto_increase_credits_dt)); i++) {
        C2V(gAuto_increase_credits_dt)[i] = (int)((float)(1000 * time) / (.02f * (float)C2V(gRecovery_cost).initial[i]));
    }
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; (int)i < REC2_ASIZE(C2V(gNet_score_targets)); i++) {
        float t;
        c2_sscanf(str, "%d", &t);
        C2V(gNet_auto_increase_credits_dt)[i] = (int)((float)(1000 * t) / (.02f * (float)C2V(gRecovery_cost).initial_network[i]));
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Mutant tail thing settings */
    /* Number of links including ball */
    C2V(gCount_mutant_tail_parts) = GetAnInt(C2V(gTempFile));
    /* Mass of each link */
    C2V(gMass_mutant_tail_link) = GetAFloat(C2V(gTempFile));
    /* Mass of ball */
    C2V(gMass_mutant_tail_ball) = GetAFloat(C2V(gTempFile));

    /*  Mine / Mortar weight */
    C2V(gMass_mine) = GetAFloat(C2V(gTempFile));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00486ef0, LoadGeneralParameters, LoadGeneralParameters_original)

void (C2_HOOK_FASTCALL * FinishLoadGeneralParameters_original)(void);
void C2_HOOK_FASTCALL FinishLoadGeneralParameters(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    FinishLoadGeneralParameters_original();
#else
    C2V(gDefault_water_spec_vol).screen_pixelmap = BrMapFind(C2V(gUnderwater_screen_name));
    ReadExplosionInfo(C2V(gTempFile), &C2V(gWasted_explosion_chance), &C2V(gExplosion_sound_id), &C2V(gExplosion_pix_animation_groups));
    ReadPowerupSmashables(C2V(gTempFile));
    PFfclose(C2V(gTempFile));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00487dc0, FinishLoadGeneralParameters, FinishLoadGeneralParameters_original)


void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, C2V(gApplication_path), "KEYMAP_X.TXT");
    the_path[c2_strlen(the_path) - 5] = '0' + C2V(gKey_map_index);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {
        c2_fscanf((FILE*)f, "%d", &C2V(gKey_mapping)[i]);
    }
    PFfclose(f);
}
C2_HOOK_FUNCTION(0x00487e10, LoadKeyMapping)

void C2_HOOK_FASTCALL SaveKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, C2V(gApplication_path), "KEYMAP_X.TXT");
    the_path[c2_strlen(the_path) - 5] = '0' + C2V(gKey_map_index);
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "wb");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {
        c2_fprintf(f, "%d", C2V(gKey_mapping)[i]);
        c2_fputc('\r', f);
        c2_fputc('\n', f);
    }
    PFfclose(f);
}

void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pKey_map_index) {

    SaveKeyMapping();
    C2V(gKey_map_index) = pKey_map_index;
    LoadKeyMapping();
}

void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;
    br_pixelmap* pixmap;

    for (i = 0; i < REC2_ASIZE(C2V(gHeadup_image_info)); i++) {
        PossibleService();
        if (C2V(gHeadup_image_info)[i].avail == eNet_or_otherwise
                || (C2V(gHeadup_image_info)[i].avail == eNot_net && C2V(gNet_mode) == eNet_mode_none)
                || (C2V(gHeadup_image_info)[i].avail == eNet_only && C2V(gNet_mode) != eNet_mode_none)) {
            pixmap = LoadPixelmap(C2V(gHeadup_image_info)[i].name);
            BRPM_convert(pixmap, C2V(gBack_screen)->type);
        } else {
            pixmap = NULL;
        }
        C2V(gHeadup_images)[i] = pixmap;
    }
}
C2_HOOK_FUNCTION(0x0048c150, LoadHeadupImages)

void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    int i;
    FILE *f;
    char s[256];
    tPath_name the_path;

    if (C2V(gPedTextTxtPath) == NULL) {
        SetDefaultTextFileName();
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gPedTextTxtPath));
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gMisc_strings)); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        C2V(gMisc_strings)[i] = BrMemAllocate(c2_strlen(s) + 1, kMem_misc_string);
        c2_strcpy(C2V(gMisc_strings)[i], s);
    }
    // Thousands delimiter
    C2V(gMisc_strings)[294][1] = '\0';
    switch (C2V(gMisc_strings)[294][0]) {
    case 'C':
        C2V(gMisc_strings)[294][0] = ',';
        break;
    case 'P':
        C2V(gMisc_strings)[294][0] = '.';
        break;
    case 'S':
        C2V(gMisc_strings)[294][0] = ' ';
        break;
    default:
        break;
    }
    PFfclose(f);
}
C2_HOOK_FUNCTION(0x0048cfc0, LoadMiscStrings)

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
C2_HOOK_FUNCTION(0x0048d110, ReadNetworkSettings)

int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex) {

    c2_fprintf(pF, "NETSETTINGS %d\n", pIndex);
    c2_fprintf(pF, "%d // Allow the sending of Abuse-o-Matic(tm) text messages\n", C2V(gNet_settings)[pIndex].enable_text_messages);
    c2_fprintf(pF, "%d // Show cars on map\n", C2V(gNet_settings)[pIndex].show_players_on_map);
    c2_fprintf(pF, "%d // Show pickups on map\n", C2V(gNet_settings)[pIndex].show_powerups_on_map);
    c2_fprintf(pF, "%d // Pickup respawn\n", C2V(gNet_settings)[pIndex].powerup_respawn);
    c2_fprintf(pF, "%d // Waste to transfer\n", C2V(gNet_settings)[pIndex].waste_to_transfer);
    c2_fprintf(pF, "%d // Open game\n", C2V(gNet_settings)[pIndex].open_game);
    c2_fprintf(pF, "%d // Grid start\n", C2V(gNet_settings)[pIndex].grid_start);
    c2_fprintf(pF, "%d // Race order\n", C2V(gNet_settings)[pIndex].race_sequence_type);
    c2_fprintf(pF, "%d // Auto-goody\n", C2V(gNet_settings)[pIndex].random_car_choice);
    c2_fprintf(pF, "%d // Car choice mode\n", C2V(gNet_settings)[pIndex].car_choice);
    c2_fprintf(pF, "%d // Starting credits\n\n", C2V(gNet_settings)[pIndex].starting_credits);
    c2_fprintf(pF, "%d // Target\n\n", C2V(gNet_settings)[pIndex].starting_target);
    return 0;
}
C2_HOOK_FUNCTION(0x0048d7d0, PrintNetOptions)

br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName) {
    FILE* f;
    tPath_name the_path;
    br_font* the_font;
    tU32 data_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(br_font) != 24);

    PathCat(the_path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);
    f = DRfopen(the_path, "rb");
    PossibleService();
    the_font = BrMemAllocate(sizeof(br_font), kMem_br_font);

    // we read 0x18 bytes as that is the size of the struct in 32 bit code.
    c2_fread(the_font, 0x18, 1, f);
#if !defined(C2_BIG_ENDIAN)
    the_font->flags = BrSwap32(the_font->flags);

    // swap endianness
    the_font->glyph_x = the_font->glyph_x >> 8 | the_font->glyph_x << 8;
    the_font->glyph_y = the_font->glyph_y >> 8 | the_font->glyph_y << 8;
    the_font->spacing_x = the_font->spacing_x >> 8 | the_font->spacing_x << 8;
    the_font->spacing_y = the_font->spacing_y >> 8 | the_font->spacing_y << 8;
#endif

    data_size = 256 * sizeof(br_int_8);
    the_font->width = BrMemAllocate(data_size, kMem_br_font_wid);
    PFfread(the_font->width, data_size, 1, f);
    data_size = 256 * sizeof(br_uint_16);
    the_font->encoding = BrMemAllocate(data_size, kMem_br_font_enc);
    PFfread(the_font->encoding, data_size, 1, f);
#if !defined(C2_BIG_ENDIAN)
    for (i = 0; i < 256; i++) {
        the_font->encoding[i] = the_font->encoding[i] >> 8 | the_font->encoding[i] << 8;
    }
#endif
    PossibleService();
    PFfread(&data_size, sizeof(tU32), 1, f);
#if !defined(C2_BIG_ENDIAN)
    data_size = BrSwap32(data_size);
#endif
    PossibleService();
    the_font->glyphs = BrMemAllocate(data_size, kMem_br_font_glyphs);
    PFfread(the_font->glyphs, data_size, 1u, f);
    PFfclose(f);
    return the_font;
}
C2_HOOK_FUNCTION(0x00466050, LoadBRFont)

FILE* C2_HOOK_FASTCALL OpenRaceFile(void) {
    FILE* f;
    tPath_name the_path;

    PathCat(the_path, C2V(gApplication_path), C2V(gRaces_file_names)[C2V(gCurrent_race_file_index)]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    return f;
}

void (C2_HOOK_FASTCALL * LoadRaces_original)(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadRaces_original(pRace_list, pCount, pRace_type_index);
#else
    FILE* f;
    br_scalar opponent_count_first_race;
    br_scalar opponent_count_last_race;
    br_scalar opponent_softness_first_race, opponent_hardness_first_race;
    br_scalar opponent_softness_last_race, opponent_hardness_last_race;
    br_scalar opponent_nastiness_first_race;
    br_scalar opponent_nastiness_last_race;
    tTWTVFS twt;
    int powerup_exclusions[50];
    tRace_group_spec* current_group;
    float factor;
    float softness;
    float hardness;
    int hardness_oppo;
    int i;
    int j;
    int count_races;
    int last_race;
    char s[256];
    char* str;

    C2_HOOK_BUG_ON(sizeof(tRace_list_spec) != 0x244);

    twt = -1;
    if (!C2V(gApplicationDataTwtMounted)) {
        twt = OpenPackFileAndSetTiffLoading(C2V(gApplication_path));
    }
    C2V(gCurrent_race_file_index) = pRace_type_index + 1;

    f = OpenRaceFile();

#if defined(C2_STAINLESS_DEVEL)
    /* Initial race is in DATA.TWT */
    C2V(gDev_initial_race) = GetAnInt(f);
#else
    /* Ignore Initial race */
    GetAnInt(f);
    C2V(gDev_initial_race) = 0;
#endif

    /* Default number of opponents in first race */
    opponent_count_first_race = GetAScalar(f);
    /* Default number of opponents in last race */
    opponent_count_last_race = GetAScalar(f);
    /* Softness,hardest rank of opponents in first race */
    GetPairOfFloats(f, &opponent_softness_first_race, &opponent_hardness_first_race);
    /* Softness,hardest rank of opponents in last race */
    GetPairOfFloats(f, &opponent_softness_last_race, &opponent_hardness_last_race);
    /* Opponent nastiness influencer for first race */
    opponent_nastiness_first_race = GetAScalar(f);
    /* Opponent nastiness influencer for last race */
    opponent_nastiness_last_race = GetAScalar(f);

    C2V(gCountRaceGroups) = 0;

    for (i = 0; 1;) {
        GetALineAndDontArgue(f, pRace_list[i].name);
        if (c2_strcmp(pRace_list[i].name, "END") == 0) {
            last_race = 1;
            count_races = i;
        } else {
            last_race = 0;
        }
        if (last_race) {
            break;
        }
        /* Text file name */
        GetAString(f, pRace_list[i].file_name);
        /* Name of interface element */
        GetAString(f, pRace_list[i].interface_name);
        /* Number of opponents (-1 = use default) */
        pRace_list[i].count_opponents = GetAnInt(f);
        /* Number of explicit opponents */
        pRace_list[i].count_explicit_opponents = GetAnInt(f);
        C2_HOOK_ASSERT(pRace_list[i].count_explicit_opponents <= REC2_ASIZE(pRace_list[i].explicit_opponents));
        for (j = 0; j < pRace_list[i].count_explicit_opponents; j++) {
            pRace_list[i].explicit_opponents[j] = GetAnInt(f);
        }
        /* Opponent nastiness level (-1 = use default) */
        pRace_list[i].opponent_nastiness_level = GetAScalar(f);
        /* Powerup exclusions */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        for (j = 0; str != NULL; j++) {
            c2_sscanf(str, "%d", &powerup_exclusions[j]);
            str = c2_strtok(NULL, "\t ,/");
        }
        pRace_list[i].count_powerup_exclusions = j;
        if (pRace_list[i].count_powerup_exclusions != 0) {
            pRace_list[i].powerup_exclusions = BrMemAllocate(pRace_list[i].count_powerup_exclusions * sizeof(int), kMem_misc);
            c2_memcpy(pRace_list[i].powerup_exclusions, powerup_exclusions, pRace_list[i].count_powerup_exclusions * sizeof(int));
        }
        /* Disable time awards */
        pRace_list[i].no_time_awards = GetAnInt(f);
        /* Boundary race (mission) */
        pRace_list[i].is_boundary = GetAnInt(f);
        if (pRace_list[i].is_boundary) {
            C2V(gCountRaceGroups)++;
        }
        /* Race type (0 = Carma1, 1 = Cars, 2 = Peds, 3 = Checkpoints, 4 = Smash, 5 = smash'n'ped) */
        pRace_list[i].race_type = GetAnInt(f);
        /* Initial timer count for each skill level */
        GetThreeInts(f, &pRace_list[i].initial_timer[0], &pRace_list[i].initial_timer[1], &pRace_list[i].initial_timer[2]);
        switch (pRace_list[i].race_type) {
        case kRaceType_Carma1:
        case kRaceType_Checkpoints:
            /* # laps */
            pRace_list[i].count_laps = GetAnInt(f);
            break;
        case kRaceType_Cars:
            /* Number of opponents that must be killed (-1 means all) */
            pRace_list[i].options.cars.count_opponents = GetAnInt(f);
            C2_HOOK_ASSERT(pRace_list[i].options.cars.count_opponents <= (int)REC2_ASIZE(pRace_list[i].options.cars.opponents));
            for (j = 0; j < pRace_list[i].options.cars.count_opponents; j++) {
                pRace_list[i].options.cars.opponents[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Peds:
            /* Number of ped groups (-1 means all) */
            pRace_list[i].options.peds.count_ped_groups = GetAnInt(f);
            for (j = 0; j < pRace_list[i].options.peds.count_ped_groups; j++) {
                pRace_list[i].options.peds.ped_groups[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Smash:
            /* Smash variable number */
            pRace_list[i].options.smash.var_smash_number = GetAnInt(f);
            /* Smash variable target */
            pRace_list[i].options.smash.var_smash_target = GetAnInt(f);
            break;
        case kRaceType_SmashNPed:
            /* Smash variable number */
            pRace_list[i].options.smash_and_peds.var_smash_number = GetAnInt(f);
            /* Smash variable target */
            pRace_list[i].options.smash_and_peds.var_smash_target = GetAnInt(f);
            /* Ped group index for required extra kills */
            pRace_list[i].options.smash_and_peds.ped_group_index = GetAnInt(f);
            break;
        }
        /* Race completed bonus for each skill level (all laps raced or target met)*/
        GetThreeInts(f, &pRace_list[i].completion_bonus[0], &pRace_list[i].completion_bonus[1], &pRace_list[i].completion_bonus[2]);
        if (pRace_list[i].race_type == kRaceType_Carma1) {
            /* Race completed bonus (all peds killed) for each skill level */
            GetThreeInts(f, &pRace_list[i].completion_bonus_peds[0], &pRace_list[i].completion_bonus_peds[1], &pRace_list[i].completion_bonus_peds[2]);
            /* Race completed bonus (all oppos wasted) for each skill level */
            GetThreeInts(f, &pRace_list[i].completion_bonus_opponents[0], &pRace_list[i].completion_bonus_opponents[1], &pRace_list[i].completion_bonus_opponents[2]);
        }
        /* Race description */
        GetALineAndDontArgue(f, pRace_list[i].description);
        if (DRStricmp(pRace_list[i].description, "none") == 0) {
            pRace_list[i].description[0] = '\0';
        }
        /* Expansion */
        pRace_list[i].expansion = GetAnInt(f) & 0x1;

        if (pRace_type_index < 0 || pRace_list[i].race_type == kRaceType_Carma1) {
            i++;
        }
    }
    *pCount = count_races;
    PFfclose(f);
    if (C2V(gRaceGroups) != NULL) {
        BrMemFree(C2V(gRaceGroups));
    }
    C2_HOOK_BUG_ON(sizeof(tRace_group_spec) != 12);
    C2V(gRaceGroups) = BrMemAllocate((C2V(gCountRaceGroups) == 0 ? 1 : C2V(gCountRaceGroups)) * sizeof(tRace_group_spec), kMem_misc);
    C2V(gRaceGroups)[0].count_races = 0;
    C2V(gRaceGroups)[0].races = pRace_list;

    current_group = &C2V(gRaceGroups)[0];

    for (i = 0; i < count_races; i++) {
        int count_opponents;

        if (pRace_list[i].opponent_nastiness_level <= 0.f) {
            pRace_list[i].opponent_nastiness_level = opponent_nastiness_first_race + (opponent_nastiness_last_race - opponent_nastiness_first_race) * (float)i / (float)(count_races - 1);
        }
        count_opponents = pRace_list[i].count_opponents;
        if (count_opponents < 0) {
            count_opponents = (int)(opponent_count_first_race + (float)i / (float)(count_races - 1) * (opponent_count_last_race - opponent_count_first_race));
        }
        pRace_list[i].count_opponents = 0;
        if (pRace_list[i].count_explicit_opponents < count_opponents) {
            factor =  (float)i / (float)(count_races - 1);
            softness = opponent_softness_first_race + factor * (opponent_softness_last_race - opponent_softness_first_race);
            hardness = opponent_hardness_first_race + factor * (opponent_hardness_last_race - opponent_hardness_first_race) - softness;
            for (j = pRace_list[i].count_explicit_opponents; j < count_opponents; j++) {
                hardness_oppo = (int)(softness + (float)hardness * (float)(j - pRace_list[i].count_explicit_opponents) / (float)(count_opponents - 1 - pRace_list[i].count_explicit_opponents));

                if (hardness_oppo <= 0) {
                    pRace_list[i].explicit_opponents[j] = -1;
                } else {
                    pRace_list[i].explicit_opponents[j] = -MIN(hardness_oppo, 5);
                }
            }
        } else {
            pRace_list[i].count_explicit_opponents = count_opponents;
        }
        pRace_list[i].count_explicit_opponents = count_opponents;
        pRace_list[i].group = current_group;
        if (pRace_list[i].is_boundary) {
            current_group->mission = &pRace_list[i];
            current_group++;
            if (current_group - C2V(gRaceGroups) < C2V(gCountRaceGroups)) {
                current_group->count_races = 0;
                current_group->races = &pRace_list[i+1];
            }
        } else {
            current_group->count_races++;
        }
    }
    C2V(gRaceGroups2) = C2V(gRaceGroups);
    if (!C2V(gApplicationDataTwtMounted)) {
        ClosePackFileAndSetTiffLoading(twt);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048c1c0, LoadRaces, LoadRaces_original)

void (C2_HOOK_FASTCALL * LoadHeadups_original)(FILE* pF, int pIndex, tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
#if 0//defined(C2_HOOKS_ENABLED)
    LoadHeadups_original(pF, pIndex, pCar_spec);
#else
    char s[256];
    char* str;
    int j;
    int number_of_slots;

    C2_HOOK_BUG_ON(offsetof(tCar_spec, headup_slots) != 0x728);
    C2_HOOK_BUG_ON(sizeof(tHeadup_slot) != 44);

    /* pIndex=0 -> Number of headups (external) */
    /* pIndex=1 -> Number of headups (internal) */
    number_of_slots = GetAnInt(pF);
    for (j = 0; j < number_of_slots; j++) {
        /* [ 0]: Development info display
         * [ 1]: Credits earned
         * [ 2]: Pedestrian kill count
         * [ 3]: Pedestrian warning
         * [ 4]: Miscellaneous messages
         * [ 5]: Start of race countdown
         * [ 6]: Fancy bonuses, timeup, race over etc
         * [ 7]: Timer
         * [ 8]: Lap count
         * [ 9]: End of race bonus
         * [10]: Time bonus
         * [11]: Time awarded
         * [12]: Cars out count
         * [13]: Cash in network mode
         * [14]: Cash in network mode
         * */
        GetALineAndDontArgue(pF, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].field_0x28);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].x);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].y);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].colour);
        str = c2_strtok(NULL, "\t ,/");
        c2_strcpy(s, str);
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
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_left);
        if (pCar_spec->headup_slots[pIndex][j].dim_left < 0) {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 0;
        } else {
            pCar_spec->headup_slots[pIndex][j].dimmed_background = 1;
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_top);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_right);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->headup_slots[pIndex][j].dim_bottom);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ba60, LoadHeadups, LoadHeadups_original)

int C2_HOOK_FASTCALL SaveOptions(void) {
    tPath_name the_path;
    FILE* f;

    PathCat(the_path, C2V(gApplication_path), "OPTIONS.TXT");
    PDFileUnlock(the_path);
    f = DRfopen(the_path, "wt");
    if (f == NULL) {
        return 0;
    }

#define BAIL_IF_NEGATIVE(VAL)            \
    if ((VAL) < 0) {                     \
        rec2_log_warn(#VAL " FAILED\n"); \
        return 0;                        \
    }

    BAIL_IF_NEGATIVE(c2_fprintf(f, "YonFactor %f\n", GetYonFactor()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "SkyTextureOn %d\n", GetSkyTextureOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "CarTexturingLevel %d\n", GetCarTexturingLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "RoadTexturingLevel %d\n", GetRoadTexturingLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "WallTexturingLevel %d\n", GetWallTexturingLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "ShadowLevel %d\n", GetShadowLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "DepthCueingOn %d\n", GetDepthCueingOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "Yon %f\n", GetYon()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "CarSimplificationLevel %d\n", GetCarSimplificationLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "AccessoryRendering %d\n", GetAccessoryRendering()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "SmokeOn %d\n", GetSmokeOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "SoundDetailLevel %d\n", GetSoundDetailLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "ScreenSize %d\n", GetScreenSize()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapRenderX %f\n", C2V(gMap_render_x)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapRenderY %f\n", C2V(gMap_render_y)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapRenderWidth %f\n", C2V(gMap_render_width)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapRenderHeight %f\n", C2V(gMap_render_height)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapMode %d\n", 1));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "PlayerName 0\n%s\n", C2V(gProgram_state).player_name[0] == '\0' ? "MAX DAMAGE" : C2V(gProgram_state).player_name));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "EVolume %d\n", C2V(gProgram_state).effects_volume));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MVolume %d\n", C2V(gProgram_state).music_volume));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "KeyMapIndex %d\n", C2V(gKey_map_index)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MapTrans %d\n", C2V(gMap_trans)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "HeadupMapX %d\n", C2V(gHeadup_map_x)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "HeadupMapY %d\n", C2V(gHeadup_map_y)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "HeadupMapW %d\n", C2V(gHeadup_map_w)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "HeadupMapH %d\n", C2V(gHeadup_map_h)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "CameraType %d\n", C2V(gCamera_type)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "ARCameraType %d\n", C2V(gAR_camera_type)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "GoreLevel %d\n", GetHowMuchBloodAndSnotToSmearAbout()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "AnimalsOn %d\n", GetAnimalsOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "FlameThrowerOn %d\n", IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MinesOn %d\n", IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "DronesOn %d\n", GetDronesOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MiniMapVisible %i\n", C2V(gMini_map_visible)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "SkillLevel %d\n", C2V(gProgram_state).skill_level));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "AmbientSound %i\n", C2V(gAmbient_sound)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "AutoLoad %d\n", C2V(gAuto_load)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "RusselsFannies %d\n", C2V(gRussels_fannies)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "QuickTimeQuality 0\n%s\n", C2V(gQuick_time_quality)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "QuickTimeCompressor 0\n%s\n", C2V(gQuick_time_compressor)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "QuickTimeBanner %d\n%s\n", C2V(gQuick_time_banner_number), C2V(gQuick_time_banner_texture_name)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "QuickTimeTempPath 0\n%s\n", C2V(gQuick_time_temp_path)));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "QuickTimeMoviePathStub 0\n%s\n", C2V(gQuick_time_movie_path_stub)));

    BAIL_IF_NEGATIVE(c2_fprintf(f, "NETGAMETYPE %d\n", C2V(gNet_last_game_type)));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 0));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 1));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 2));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 3));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 4));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 5));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 6));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 7));
    BAIL_IF_NEGATIVE(PrintNetOptions(f, 8));

    BAIL_IF_NEGATIVE(c2_fprintf(f, "HeadupDetailLevel %d\n", C2V(gHeadup_detail_level)));

#undef BAIL_IF_NEGATIVE

    PFfclose(f);

    return 1;
}
C2_HOOK_FUNCTION(0x0048d190, SaveOptions)

int (C2_HOOK_FASTCALL * RestoreOptions_original)(void);
int C2_HOOK_FASTCALL RestoreOptions(void) {
    tPath_name the_path;
    FILE* f;
    float arg;
    char line[80];
    char token[80];
    char* s;

#if 0 // defined(C2_HOOKS_ENABLED)
    RestoreOptions_original();
#else
    C2V(gProgram_state).music_volume = 200;
    C2V(gProgram_state).effects_volume = 200;
    C2V(gProgram_state).skill_level = 1;
    C2V(gMap_render_x) = 6.0;
    C2V(gMap_render_y) = 6.0;
    C2V(gMap_render_width) = 64.0;
    C2V(gMap_render_height) = 40.0;
    C2V(gMap_trans) = 0;
    C2V(gHeadup_map_x) = 228;
    C2V(gHeadup_map_y) = 150;
    C2V(gHeadup_map_w) = 48;
    C2V(gHeadup_map_h) = 48;
    C2V(gHeadup_detail_level) = kMax_headup_detail_level;
    C2V(gMap_view) = 1;
    C2V(gMini_map_visible) = 1;

    DefaultNetSettings();
    SetQuickTimeDefaults();

    PathCat(the_path, C2V(gApplication_path), "OPTIONS.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        return 0;
    }

    while (PFfgets(line, sizeof(line), f)) {
        if (c2_sscanf(line, "%79s%f", token, &arg) == 2) {
            if (c2_strcmp(token, "YonFactor") == 0) {
                SetYonFactor(arg);
            } else if (c2_strcmp(token, "SkyTextureOn") == 0) {
                SetSkyTextureOn((int)arg);
            } else if (c2_strcmp(token, "CarTexturingLevel") == 0) {
                SetCarTexturingLevel((tCar_texturing_level)arg);
            } else if (c2_strcmp(token, "RoadTexturingLevel") == 0) {
                SetRoadTexturingLevel((tRoad_texturing_level)arg);
            } else if (c2_strcmp(token, "WallTexturingLevel") == 0) {
                SetWallTexturingLevel((tWall_texturing_level)arg);
            } else if (c2_strcmp(token, "ShadowLevel") == 0) {
                SetShadowLevel((tShadow_level)arg);
            } else if (c2_strcmp(token, "DepthCueingOn") == 0) {
                SetDepthCueingOn((int)arg);
            } else if (c2_strcmp(token, "Yon") == 0) {
                SetYon(arg);
            } else if (c2_strcmp(token, "CarSimplificationLevel") == 0) {
                SetCarSimplificationLevel((int)arg);
            } else if (c2_strcmp(token, "AccessoryRendering") == 0) {
                SetAccessoryRendering((int)arg);
            } else if (c2_strcmp(token, "SmokeOn") == 0) {
                SetSmokeOn((int)arg);
            } else if (c2_strcmp(token, "SoundDetailLevel") == 0) {
                SetSoundDetailLevel((int)arg);
            } else if (c2_strcmp(token, "ScreenSize") == 0) {
                SetScreenSize((int)arg);
            } else if (c2_strcmp(token, "MapRenderX") == 0) {
                C2V(gMap_render_x) = arg;
            } else if (c2_strcmp(token, "MapRenderY") == 0) {
                C2V(gMap_render_y) = arg;
            } else if (c2_strcmp(token, "MapRenderWidth") == 0) {
                C2V(gMap_render_width) = arg;
            } else if (c2_strcmp(token, "MapRenderHeight") == 0) {
                C2V(gMap_render_height) = arg;
            } else if (c2_strcmp(token, "MapMode") == 0) {
            } else if (c2_strcmp(token, "MapTrans") == 0) {
                C2V(gMap_trans) = (int)arg;
            } else if (c2_strcmp(token, "HeadupMapX") == 0) {
                C2V(gHeadup_map_x) = (int)arg;
            } else if (c2_strcmp(token, "HeadupMapY") == 0) {
                C2V(gHeadup_map_y) = (int)arg;
            } else if (c2_strcmp(token, "HeadupMapW") == 0) {
                C2V(gHeadup_map_w) = (int)arg;
            } else if (c2_strcmp(token, "HeadupMapH") == 0) {
                C2V(gHeadup_map_h) = (int)arg;
            } else if (c2_strcmp(token, "PlayerName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gProgram_state).player_name, s);
            } else if (c2_strcmp(token, "EVolume") == 0) {
                C2V(gProgram_state).effects_volume = (int)arg;
            } else if (c2_strcmp(token, "MVolume") == 0) {
                C2V(gProgram_state).music_volume = (int)arg;
            } else if (c2_strcmp(token, "KeyMapIndex") == 0) {
                C2V(gKey_map_index) = (int)arg;
            } else if (c2_strcmp(token, "CameraType") == 0) {
                C2V(gAction_replay_camera_mode) = C2V(gCamera_type) = (int)arg;
            } else if (c2_strcmp(token, "ARCameraType") == 0) {
                C2V(gAR_camera_type) = (int)arg;
            } else if (c2_strcmp(token, "GoreLevel") == 0) {
                SetGoreLevel((int)arg);
            } else if (c2_strcmp(token, "AnimalsOn") == 0) {
                SetAnimalsOn((int)arg);
            } else if (c2_strcmp(token, "FlameThrowerOn") == 0) {
                SetFlameThrowerOn((int)arg);
            } else if (c2_strcmp(token, "MinesOn") == 0) {
                SetMinesOn((int)arg);
            } else if (c2_strcmp(token, "DronesOn") == 0) {
                SetDronesOn((int)arg);
            } else if (c2_strcmp(token, "MiniMapVisible") == 0) {
                C2V(gMini_map_visible) = (int)arg;
            } else if (c2_strcmp(token, "SkillLevel") == 0) {
                C2V(gProgram_state).skill_level = (int)arg;
            } else if (c2_strcmp(token, "AmbientSound") == 0) {
                C2V(gAmbient_sound) = (int)arg;
            } else if (c2_strcmp(token, "AutoLoad") == 0) {
                C2V(gAuto_load) = (int)arg;
            } else if (c2_strcmp(token, "RusselsFannies") == 0) {
                C2V(gRussels_fannies) = (int)arg;
            } else if (c2_strcmp(token, "QuickTimeQuality") == 0) {
                c2_fgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_quality), s);
            } else if (c2_strcmp(token, "QuickTimeCompressor") == 0) {
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_compressor), s);
            } else if (c2_strcmp(token, "QuickTimeBanner") == 0) {
                C2V(gQuick_time_banner_number) = (int)arg;
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_banner_texture_name), s);
            } else if (c2_strcmp(token, "QuickTimeTempPath") == 0) {
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_temp_path), s);
            } else if (c2_strcmp(token, "QuickTimeMoviePathStub") == 0) {
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_movie_path_stub), s);
            } else if (c2_strcmp(token, "NetName") == 0) {
                PFfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
            } else if (c2_strcmp(token, "NETGAMETYPE") == 0) {
                C2V(gNet_last_game_type) = (int)arg;
            } else if (c2_strcmp(token, "NETSETTINGS") == 0) {
                ReadNetworkSettings(f, &C2V(gNet_settings)[(int)arg]);
            } else if (c2_strcmp(token, "HeadupDetailLevel") == 0) {
                C2V(gHeadup_detail_level) = (int)arg;
            }
        }
    }
    PFfclose(f);
    C2V(gMap_view) = 1;
    C2V(gMap_render_x) = 80.f;
    C2V(gMap_render_y) = 400.f;
    C2V(gMap_render_width) = 128.f;
    C2V(gMap_render_height) = 80.f;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048d8f0, RestoreOptions, RestoreOptions_original)

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
    LoadAllImagesInDirectory(&C2V(gMisc_storage_space), the_path2);
    ClosePackFileAndSetTiffLoading(twt);

    LoadInFiles(the_path, "MATERIAL", DRLoadMaterials);
    LoadInFiles(the_path, "MODELS", DRLoadModels);
    LoadInFiles(the_path, "ACTORS", DRLoadActors);
    LoadInFiles(the_path, "LIGHTS", DRLoadLights);

}

void (C2_HOOK_FASTCALL * LoadInRegistees_original)(void);
void C2_HOOK_FASTCALL LoadInRegistees(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInRegistees_original();
#else
    LoadInRegisteeDir(C2V(gApplication_path), "REG", 1);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00486e10, LoadInRegistees, LoadInRegistees_original)

void (C2_HOOK_FASTCALL * LoadTreeSurgery_original)(void);
void C2_HOOK_FASTCALL InitTreeSurgery(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadTreeSurgery_original();
#else
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, C2V(gApplication_path), "TreeSurgery.TXT");
    file = PFfopen(the_path, "rt");
    if (file == NULL) {
        C2V(gTree_surgery_pass1_count) = 0;
        C2V(gTree_surgery_pass2_count) = 0;
        return;
    }
    C2V(gTree_surgery_pass1_count) = GetAnInt(file);
    for (i = 0; i < C2V(gTree_surgery_pass1_count); i++) {
        GetAString(file, C2V(gTree_surgery_pass1)[i].name);
    }
    C2V(gTree_surgery_pass2_count) = GetAnInt(file);
    for (i = 0; i < C2V(gTree_surgery_pass1_count); i++) {
        GetAString(file, C2V(gTree_surgery_pass2)[i].original);
        GetAString(file, C2V(gTree_surgery_pass2)[i].replacement);
    }
#if defined(REC2_FIX_BUGS)
    PFfclose(file);
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00504b30, InitTreeSurgery, LoadTreeSurgery_original)

int (C2_HOOK_FASTCALL * TestForOriginalCarmaCDinDrive_original)(void);
int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return TestForOriginalCarmaCDinDrive_original();
#else
    tPath_name paths_txt;
    tPath_name cd_data_pathname;
    tPath_name cutscene_pathname;
    FILE* paths_txt_fp;
    char paths_txt_first_char;
    char cd_pathname[256];

    PathCat(paths_txt, C2V(gApplication_path), "PATHS.TXT");
    if (!PDCheckDriveExists(paths_txt)) {
        return 0;
    }
    paths_txt_fp = PFfopen(paths_txt, "rt");
    if (paths_txt_fp == NULL) {
        return 0;
    }
    paths_txt_first_char = DRfgetc(paths_txt_fp);
    DRungetc(paths_txt_first_char, paths_txt_fp);
    cd_pathname[0] = '\0';
    GetALineAndDontArgue(paths_txt_fp, cd_pathname);
    PFfclose(paths_txt_fp);
    PathCat(cd_data_pathname, cd_pathname, "DATA");

    if (DRStricmp(cd_pathname, C2V(gApplication_path)) == 0) {
        return 0;
    }

    PathCat(cutscene_pathname, cd_data_pathname, "CUTSCENE");

    if (!PDCheckDriveExists2(cd_data_pathname, "GENERAL.TXT", 100)) {
        return 0;
    }
    if (!PDCheckDriveExists2(cd_pathname, "CARMA.EXE", 1000000)) {
        return 0;
    }
    if (!PDCheckDriveExists2(cutscene_pathname, "MIX_INTR.SMK.EXE", 1000000)) {
        return 0;
    }

    if (paths_txt_first_char != '@') {
        EncodeFile(paths_txt);
    }
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f3b0, TestForOriginalCarmaCDinDrive, TestForOriginalCarmaCDinDrive_original)

int C2_HOOK_FASTCALL OriginalCarmaCDinDrive(void) {

    return C2V(gCD_is_in_drive);
}
C2_HOOK_FUNCTION(0x0048f6a0, OriginalCarmaCDinDrive)

int C2_HOOK_FASTCALL CarmaCDinDriveOrFullGameInstalled(void) {

    if (C2V(gCD_fully_installed)) {
        return 1;
    } else {
        return OriginalCarmaCDinDrive();
    }
}
C2_HOOK_FUNCTION(0x0048f6b0, CarmaCDinDriveOrFullGameInstalled)

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name) {
    br_pixelmap* the_map;

    the_map = BrPixelmapLoad(pFile_name);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
        the_map->row_bytes = (the_map->row_bytes + sizeof(int32_t) - 1) & ~(sizeof(int32_t) - 1);
    }
    return the_map;
}
C2_HOOK_FUNCTION(0x00513970, DRPixelmapLoad)

void C2_HOOK_FASTCALL DRLoadPalette(const char* pPath_name) {
    br_pixelmap* palette_array[100];
    int number_of_palettes;
    int i;

    number_of_palettes = BrPixelmapLoadMany(pPath_name, palette_array, REC2_ASIZE(palette_array));
    for (i = 0; i < number_of_palettes; i++) {
        palette_array[i]->row_bytes = (palette_array[i]->row_bytes + 3) & ~0x3;
        palette_array[i]->base_x = 0;
        palette_array[i]->base_y = 0;
    }
    BrTableAddMany(palette_array, number_of_palettes);
}
C2_HOOK_FUNCTION(0x0048f090, DRLoadPalette)

void C2_HOOK_FASTCALL DRLoadShadeTable(const char* pPath_name) {
    br_pixelmap* table_array[100];
    int number_of_tables;
    int i;

    number_of_tables = BrPixelmapLoadMany(pPath_name, table_array, REC2_ASIZE(table_array));
    for (i = 0; i < number_of_tables; i++) {
        table_array[i]->row_bytes = (table_array[i]->row_bytes + 3) & ~0x3;
        table_array[i]->base_x = 0;
        table_array[i]->base_y = 0;
    }
    BrTableAddMany(table_array, number_of_tables);
}
C2_HOOK_FUNCTION(0x0048f100, DRLoadShadeTable)

void C2_HOOK_FASTCALL DRLoadMaterials(const char* pPath_name) {
    br_material* material_array[100];
    int i;
    int number_of_materials;

    PossibleService();
    number_of_materials = BrMaterialLoadMany(pPath_name, material_array, REC2_ASIZE(material_array));
    for (i = 0; i < number_of_materials; i++) {
        material_array[i]->flags &= ~BR_MATF_LIGHT;
    }
    BrMaterialAddMany(material_array, number_of_materials);
}
C2_HOOK_FUNCTION(0x0048f1d0, DRLoadMaterials)

void C2_HOOK_FASTCALL DRLoadModels(const char* pPath_name) {
    int i;
    br_model* model_array[100];
    int number_of_models;

    PossibleService();
    number_of_models = BrModelLoadMany(pPath_name, model_array, REC2_ASIZE(model_array));
    WhitenVertexRGB(model_array, number_of_models);
    for (i = 0; i < number_of_models; i++) {
        model_array[i]->flags = BR_MODF_UPDATEABLE;
    }
    BrModelAddMany(model_array, number_of_models);
}
C2_HOOK_FUNCTION(0x0048f230, DRLoadModels)

br_model* C2_HOOK_FASTCALL LoadModel(const char* pName) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, C2V(gApplication_path), "MODELS");
    PathCat(the_path, the_path, pName);
    return BrModelLoad(the_path);
}
C2_HOOK_FUNCTION(0x0048efd0, LoadModel)

br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName) {
    tPath_name the_path;
    br_actor* actor;

    PossibleService();
    PathCat(the_path, C2V(gApplication_path), "ACTORS");
    PathCat(the_path, the_path, pName);
    actor = BrActorLoad(the_path);
    if (actor == NULL) {
        PathCat(the_path, C2V(gRace_path), pName);
        actor = BrActorLoad(the_path);
    }
    return actor;
}
C2_HOOK_FUNCTION(0x0048f020, LoadActor)

void C2_HOOK_FASTCALL DRLoadActors(const char* pPath_name) {
    int i;
    br_actor* actor_array[100];
    int number_of_actors;

    PossibleService();
    number_of_actors = BrActorLoadMany(pPath_name, actor_array, REC2_ASIZE(actor_array));
    for (i = 0; i < number_of_actors; i++) {
        C2V(gActor_array)[C2V(gNumber_of_actors)] = actor_array[i];
        C2V(gNumber_of_actors)++;
    }
}
C2_HOOK_FUNCTION(0x0048f290, DRLoadActors)

void C2_HOOK_FASTCALL DRLoadLights(const char* pPath_name) {
    br_actor* actor;
    br_light* light;

    actor = BrActorAllocate(BR_ACTOR_LIGHT, NULL);
    light = actor->type_data;
    light->type = BR_LIGHT_DIRECT;
    light->attenuation_c = 1.f;
    light->colour = BR_COLOUR_RGB(C2V(gLighting_data).directional.red, C2V(gLighting_data).directional.green, C2V(gLighting_data).directional.blue);
    BrMatrix34RotateX(&actor->t.t.mat, BR_ANGLE_DEG(300));
    BrMatrix34PostRotateY(&actor->t.t.mat, BR_ANGLE_DEG(30));
    C2V(gLight_array)[C2V(gNumber_of_lights)] = actor;
    C2V(gNumber_of_lights)++;
    EnableLights();
}
C2_HOOK_FUNCTION(0x0048f2e0, DRLoadLights)

void (C2_HOOK_FASTCALL * InitializePalettes_original)(void);
void C2_HOOK_FASTCALL InitializePalettes(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitializePalettes_original();
#else
    br_pixelmap* render_palette;

    C2V(gCurrent_palette_pixels) = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    C2V(gPalette_changed) = 0;
    C2V(gCurrent_palette) = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, C2V(gCurrent_palette_pixels), 0);
    C2V(gRender_palette) = BrTableFind("DRRENDER.PAL");
    if (C2V(gRender_palette) == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    VerifyPaletteBlackness(C2V(gRender_palette));
    C2V(gOrig_render_palette) = BrPixelmapAllocateSub(C2V(gRender_palette), 0, 0, C2V(gRender_palette)->width, C2V(gRender_palette)->height);
    C2V(gOrig_render_palette)->pixels = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    c2_memcpy(C2V(gOrig_render_palette)->pixels, C2V(gRender_palette)->pixels, 256 * sizeof(br_uint_32));
    C2V(gFlic_palette) = BrTableFind("DRACEFLC.PAL");
    render_palette = C2V(gRender_palette);
    if (C2V(gFlic_palette) == NULL) {
        FatalError(kFatalError_unableToFindRequiredPalette);
    }
    ((br_uint_32*)C2V(gRender_palette)->pixels)[0] = 0;
    c2_memcpy(C2V(gCurrent_palette_pixels), C2V(gRender_palette)->pixels, 256 * sizeof(br_uint_32));
    C2V(gPalette_changed) = 0;
    if (!C2V(gFaded_palette)) {
        PDSetPalette(C2V(gRender_palette));
    }
    C2V(gPalette_munged) |= render_palette != C2V(gRender_palette);
    C2V(gScratch_pixels) = BrMemAllocate(256 * sizeof(br_uint_32), kMem_misc);
    C2V(gScratch_palette) = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, C2V(gScratch_pixels), 0);
    C2V(gMini_map_glowing_line_palettes)[0] = (br_pixelmap*)-1; /* FIXME: invalid pointer! */
    C2V(gMini_map_glowing_line_palettes)[1] = C2V(gPalette_0074a604);
    C2V(gMini_map_glowing_line_palettes)[2] = C2V(gPalette_0074a600);
    C2V(gPalette_0074a66c) = C2V(gPalette_0074a5fc);
    C2V(gPalette_0074a670) = NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5090, InitializePalettes, InitializePalettes_original)

void (C2_HOOK_FASTCALL * DisableVertexColours_original)(br_model** pModels, int pCount);
void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount) {

#if 0//defined(C2_HOOKS_ENABLED)
    DisableVertexColours_original(pModels, pCount);
#else
    int i;

    if (C2V(gScreen) == NULL) {
        return;
    }
    if (C2V(gScreen)->type == BR_PMT_INDEX_8) {
        return;
    }
    for (i = 0; i < pCount; i++) {
        int j;

        for (j = 0; j < pModels[i]->nvertices; j++) {
            br_vertex* vertex = &pModels[i]->vertices[j];

            vertex->red = 0xff;
            vertex->grn = 0xff;
            vertex->blu = 0xff;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00518690, WhitenVertexRGB, DisableVertexColours_original)

br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path) {
    tPath_name the_path;

    PossibleService();
    PathCat(the_path, C2V(gApplication_path), "MATERIAL");
    PathCat(the_path, the_path, pThe_path);
    return BrMaterialLoad(the_path);
}
C2_HOOK_FUNCTION(0x0048ef80, LoadMaterial)

void (C2_HOOK_FASTCALL * FillInRaceInfo_original)(tRace_info* pThe_race);
void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race) {
#if 0//defined(C2_HOOKS_ENABLED)
    FillInRaceInfo_original(pThe_race);
#else
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tProgram_state, track_file_name, 0x1a1e);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_info, race_spec, 0x98);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tRace_list_spec, file_name, 0x60);

    c2_strcpy(C2V(gProgram_state).track_file_name, pThe_race->race_spec->file_name);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048d0e0, FillInRaceInfo, FillInRaceInfo_original)


void (C2_HOOK_FASTCALL * DisposeNonCarCollisionInfo_original)(tCollision_info* pCollision_info);
void C2_HOOK_FASTCALL DisposePhysicsObject(tCollision_info* pCollision_info) {

#if defined(C2_HOOKS_ENABLED)
    DisposeNonCarCollisionInfo_original(pCollision_info);
#else
#error "Not implemeneted"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044bb90, DisposePhysicsObject, DisposeNonCarCollisionInfo_original)

void (C2_HOOK_FASTCALL * DisposeCar_original)(tCar_spec* pCar_spec, int pOwner);
void C2_HOOK_FASTCALL DisposeCar(tCar_spec* pCar_spec, int pOwner) {
#if defined(C2_HOOKS_ENABLED)
    DisposeCar_original(pCar_spec, pOwner);
#else
#error "Not implemeneted"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044bbe0, DisposeCar, DisposeCar_original)

void (C2_HOOK_FASTCALL * InitAIWorld_original)(void);
void C2_HOOK_FASTCALL InitPanGameAIWorld(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitAIWorld_original();
#else
    PrintMemoryDump(0, "BEFORE LoadOpponents()");
    LoadOpponents();
    PrintMemoryDump(0, "AFTER LoadOpponents(), BEFORE LoadPanGameDroneInfo()");
    LoadPanGameDroneInfo();
    PrintMemoryDump(0, "AFTER LoadPanGameDroneInfo()");
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00401000, InitPanGameAIWorld, InitAIWorld_original)

void (C2_HOOK_FASTCALL * LoadOpponents_original)(void);
void C2_HOOK_FASTCALL LoadOpponents(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadOpponents_original();
#else
    FILE* f;
    tPath_name the_path;
    int i;
    int j;
    int k;
    char s[256];
    char* str;
    tText_chunk* the_chunk;

    PathCat(the_path, C2V(gApplication_path), "OPPONENT.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenOpponentsFile);
    }
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &C2V(gNumber_of_racers));
    C2V(gOpponents) = BrMemAllocate(sizeof(tOpponent) * C2V(gNumber_of_racers), kMem_oppo_array);

    for (i = 0; i < C2V(gNumber_of_racers); i++) {
        PossibleService();
        GetALineAndDontArgue(f, C2V(gOpponents)[i].name);
        if (c2_strcmp(C2V(gOpponents)[i].name, "END") == 0) {
            FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
        }
        GetALineAndDontArgue(f, s);
        c2_strcpy(C2V(gOpponents)[i].abbrev_name, c2_strtok(s, "\t ,/"));
        GetALineAndDontArgue(f, C2V(gOpponents)[i].car_name);
        /* Strength rating (1-5) */
        C2V(gOpponents)[i].strength_rating = GetAnInt(f);
        /* Cost to buy it */
        C2V(gOpponents)[i].price = GetAnInt(f);
        /* Network availability ('eagle', or 'all') */
        C2V(gOpponents)[i].network_availability = GetALineAndInterpretCommand(f, C2V(gNet_avail_names), REC2_ASIZE(C2V(gNet_avail_names)));

        C2V(gOpponents)[i].mug_shot_image_data = NULL;
        C2V(gOpponents)[i].grid_icon_image = NULL;
        C2V(gOpponents)[i].stolen_car_image_data = NULL;

        /* vehicle filename */
        GetALineAndDontArgue(f, s);
        c2_strcpy(C2V(gOpponents)[i].car_file_name, c2_strtok(s, "\t ,/"));

        /* vehicle description */
        GetALineAndDontArgue(f, C2V(gOpponents)[i].line1_topspeed);
        GetALineAndDontArgue(f, C2V(gOpponents)[i].line2_weight);
        GetALineAndDontArgue(f, C2V(gOpponents)[i].line3_acceleration);
        GetALineAndDontArgue(f, C2V(gOpponents)[i].line4_description);

        C2_HOOK_BUG_ON(sizeof(tText_chunk) != 52);
        C2V(gOpponents)[i].text_chunk_count = 0;
        C2V(gOpponents)[i].text_chunks = BrMemAllocate(sizeof(tText_chunk) * C2V(gOpponents)[i].text_chunk_count, kMem_oppo_text_chunk);

        for (j = 0; j < C2V(gOpponents)[i].text_chunk_count; j++) {
            the_chunk = &C2V(gOpponents)[i].text_chunks[j];
            PossibleService();
            GetPairOfInts(f, &the_chunk->x_coord, &the_chunk->y_coord);
            GetPairOfInts(f, &the_chunk->frame_cue, &the_chunk->frame_end);
            the_chunk->line_count = GetAnInt(f);
            while (the_chunk->line_count > REC2_ASIZE(the_chunk->text)) {
                the_chunk->line_count--;
                GetALineAndDontArgue(f, s);
            }

            for (k = 0; k < the_chunk->line_count; k++) {
                GetALineAndDontArgue(f, s);
                the_chunk->text[k] = BrMemAllocate(c2_strlen(s) + 1, kMem_oppo_text_str);
                c2_strcpy(the_chunk->text[k], s);
            }
        }
        C2V(gOpponents)[i].dead = 0;
        InitOpponentPsyche(i);
    }
    GetALineAndDontArgue(f, s);
    if (c2_strcmp(s, "END") != 0) {
        FatalError(kFatalError_OpponentCountMismatchesActualNumberOfOpponents);
    }
    PFfclose(f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048c930, LoadOpponents, LoadOpponents_original)

FILE* C2_HOOK_FASTCALL OpenDroneFile(const char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    c2_strcpy(the_path, C2V(gApplication_path));
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    c2_strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}
C2_HOOK_FUNCTION(0x0044f640, OpenDroneFile)

void C2_HOOK_FASTCALL LoadDroneTypeInfo(const char* pDrone_name) {
    tPath_name the_path;
    tTWTVFS twt;
    int version;
    int count;
    char s[256];
    char* str;
    FILE* f;
    tDrone_form* drone;

    C2_HOOK_BUG_ON(sizeof(tDrone_form) != 136);

    drone = &C2V(gDrone_forms)[C2V(gCount_drone_forms)];
    c2_strcpy(the_path, C2V(gApplication_path));
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    twt = OpenPackFileAndSetTiffLoading(the_path);
    f = OpenDroneFile(pDrone_name);

    /* Version of this text file's format */
    GetALineAndDontArgue(f, s);
    c2_strtok(s, "\t ,/");
    if (c2_strcmp(s, "VERSION") != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }
    str = c2_strtok(NULL, "\t ,/");
    count = c2_sscanf(str, "%d", &version);
    if (count != 1 || version < 2) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Name of this drone, for cross-referencing porpoises */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(pDrone_name, str) != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }
    c2_strncpy(drone->name, pDrone_name, REC2_ASIZE(drone->name) - 1);

    drone->type = GetALineAndInterpretCommand(f, C2V(gDrone_type_names), REC2_ASIZE(C2V(gDrone_type_names)));
    if (drone->type < 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Mass (tonnes) */
    drone->M = GetAScalar(f);

    /* Centre of mass, as %age distance from front, left, bottom (woof) */
    GetThreeFloats(f, &drone->center.v[0], &drone->center.v[1], &drone->center.v[2]);

    /* Cornering (smooth/sharp) */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(str, "smooth") == 0) {
        drone->flags |= 0x1;
    } else if (c2_strcmp(str, "sharp") == 0) {
        drone->flags &= ~0x1;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Speed (constant/variable) */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(str, "constant") == 0) {
        /* if constant, must be followed by ONE number (the speed), */

        /* (BRU/s) */
        drone->speed = GetAScalar(f);
    } else if (c2_strcmp(str, "variable") == 0) {
        /* if variable, must be followed by THREE numbers on separate lines (accel, max speed, min speed) */

        drone->speed = -1.f;

        /* Max accel (BRU/s/s, 1 BRU/s = 15mph approx.) */
        drone->max_acceleration = GetAScalar(f);

        /* Max speed (BRU/s) (that's about 150 mph...we don't really want the F14 to go at 600mph now, do we?) */
        drone->max_speed = GetAScalar(f);

        /* Min speed (BRU/s) (or taxi speed for a plane) */
        drone->min_speed = GetAScalar(f);
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Crushability (0 = no crushing, 0.5 average softness, 1.0 very soft, 2.0 extremely soft) */
    drone->crushability = GetAScalar(f);

    if (drone->crushability != 0.f) {
        /* if not 0 (zero) then must be followed by three lines of 2 values */
        /* Crush limits front, back as %age of total length of vehicle */
        GetPairOfFloats(f, &drone->crush_limits_front, &drone->crush_limits_back);

        /* Crush limits left, right */
        GetPairOfFloats(f, &drone->crush_limits_left, &drone->crush_limits_right);

        /* Crush limits left, right */
        GetPairOfFloats(f, &drone->crush_limits_bottom, &drone->crush_limits_top);
    }

    /* Ability to be resurrected after twattage (respawn / norespawn) */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(str, "respawn") == 0) {
        drone->flags |= 0x2;
    } else if (c2_strcmp(str, "norespawn") == 0) {
        drone->flags &= ~0x2;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* orientation relative to path incline: inline (car, plane), vertical (cable car) */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(str, "vertical") == 0) {
        drone->flags |= 0x10;
    } else if (c2_strcmp(str, "inline") == 0) {
        drone->flags &= ~0x10;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    /* Processing - 'always' or 'distance' */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s," \t ,/");
    if (c2_strcmp(str, "always") == 0) {
        drone->flags |= 0x4;
    } else if (c2_strcmp(str, "distance") == 0) {
        drone->flags &= ~0x4;
    } else {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
    }

    drone->flags &= ~0x8;

    if (version > 2) {
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        if (c2_strcmp(str, "drivable_on") == 0) {
            drone->flags |= 0x8;
        } else if (c2_strcmp(str, "not_drivable_on") == 0) {
            drone->flags &= ~0x8;
        } else {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone_name);
        }
    }
    PFfclose(f);
    ClosePackFileAndSetTiffLoading(twt);
}

void (C2_HOOK_FASTCALL * LoadPanGameDroneInfo_original)(void);
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadPanGameDroneInfo_original();
#else
    int version;
    FILE* f;
    tPath_name the_path;
    char s[256];
    char* str;

    c2_strcpy(the_path, C2V(gApplication_path));
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, "DRONE.TXT");

    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    GetALineAndDontArgue(f, s);
    c2_strtok(s, "\t ,/");
    if (c2_strcmp(s, "VERSION") != 0) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    str = c2_strtok(NULL, "\t ,/");
    sscanf(str, "%d", &version);
    if (version == 1) {
        c2_memset(C2V(gDrone_forms), 0, sizeof(C2V(gDrone_forms)));
        for (C2V(gCount_drone_forms) = 0; C2V(gCount_drone_forms) < REC2_ASIZE(C2V(gDrone_forms)); C2V(gCount_drone_forms)++) {
            GetALineAndDontArgue(f, s);
            if (c2_strcmp(s, "END OF DRONES") == 0 || PFfeof(f)) {
                break;
            }
            LoadDroneTypeInfo(s);
        }
        PFfclose(f);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044ed10, LoadPanGameDroneInfo, LoadPanGameDroneInfo_original)

void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info) {
    int i;

    pRace_info->index = pRace_index;
    pRace_info->race_spec = &C2V(gRace_list)[pRace_index];
    if (C2V(gNet_mode) == eNet_mode_none) {
        pRace_info->number_of_racers = C2V(gRace_list)[pRace_index].count_explicit_opponents;
    }
    pRace_info->total_laps = C2V(gRace_list)[pRace_index].count_laps;
    for (i = 0; i < 3; i++) {
        pRace_info->initial_timer[i] = pRace_info->race_spec->initial_timer[i];
        pRace_info->completion_bonus[i] = pRace_info->race_spec->completion_bonus[i];
        pRace_info->completion_bonus_peds[i] = pRace_info->race_spec->completion_bonus_peds[i];
        pRace_info->completion_bonus_opponents[i] = pRace_info->race_spec->completion_bonus_opponents[i];
    }
}
C2_HOOK_FUNCTION(0x0048c860, LoadRaceInfo)

void C2_HOOK_FASTCALL InitFunkGrooveFlags(void) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gFunk_groove_flags)) != 30);

    // Starting from 1
    for (i = 1; i < REC2_ASIZE(C2V(gFunk_groove_flags)); i++) {
        C2V(gFunk_groove_flags)[i] = 0;
    }
}
C2_HOOK_FUNCTION(0x0048e9e0, InitFunkGrooveFlags)

void C2_HOOK_FASTCALL AboutToLoadFirstCar(void) {

    InitFunkGrooveFlags();
    C2V(gGroove_funk_offset) = 0;
}
C2_HOOK_FUNCTION(0x0048cd80, AboutToLoadFirstCar)

void C2_HOOK_FASTCALL LoadCopCars(void) {

    C2V(gProgram_state).AI_vehicles.number_of_cops = 0;
}
C2_HOOK_FUNCTION(0x004a9600, LoadCopCars)

void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}
C2_HOOK_FUNCTION(0x00401030, LoadAIWorldTrackInfo)

void C2_HOOK_FASTCALL LoadGear(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear_x, 0x2dc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gear_y, 0x2e4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, gears_image, 0x1a8);

    /* Gear display x,y,image */
    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &pCar_spec->gear_x[pIndex]);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", &pCar_spec->gear_y[pIndex]);
    str = c2_strtok(NULL, "\t ,/");
    if (pIndex == 0) {
        pCar_spec->gears_image = LoadPixelmap(str);
    }
}

void C2_HOOK_FASTCALL AdjustCarCoordinates(tCar_spec* pCar) {
    int i;

    for (i = 0; i < REC2_ASIZE(pCar->render_left); i++) {
        pCar->render_left[i] -= C2V(gCurrent_graf_data)->cock_margin_x;
        pCar->render_top[i] -= C2V(gCurrent_graf_data)->cock_margin_y;
        pCar->render_right[i] -= C2V(gCurrent_graf_data)->cock_margin_x;
        pCar->render_bottom[i] -= C2V(gCurrent_graf_data)->cock_margin_y;
    }
    pCar->mirror_left -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->mirror_top -= C2V(gCurrent_graf_data)->cock_margin_y;
    pCar->mirror_right -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->mirror_bottom -= C2V(gCurrent_graf_data)->cock_margin_y;
    pCar->speedo_centre_x[1] -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->speedo_centre_y[1] -= C2V(gCurrent_graf_data)->cock_margin_y;
    pCar->tacho_centre_x[1] -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->tacho_centre_y[1] -= C2V(gCurrent_graf_data)->cock_margin_y;
    pCar->speedo_x[1] -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->speedo_y[1] -= C2V(gCurrent_graf_data)->cock_margin_y;
    pCar->tacho_x[1] -= C2V(gCurrent_graf_data)->cock_margin_x;
    pCar->tacho_y[1] -= C2V(gCurrent_graf_data)->cock_margin_y;
    for (i = 0; i < REC2_ASIZE(pCar->lhands_x); i++) {
        pCar->lhands_x[i] -= C2V(gCurrent_graf_data)->cock_margin_x;
        pCar->lhands_y[i] -= C2V(gCurrent_graf_data)->cock_margin_y;
        pCar->rhands_x[i] -= C2V(gCurrent_graf_data)->cock_margin_x;
        pCar->rhands_y[i] -= C2V(gCurrent_graf_data)->cock_margin_y;
    }
    for (i = 0; i < REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].x_coord -= C2V(gCurrent_graf_data)->cock_margin_x;
        pCar->damage_units[i].y_coord -= C2V(gCurrent_graf_data)->cock_margin_y;
    }
}

void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    /* tacho type, x, y, filename, x-pitch */
    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        pCar_spec->tacho_radius_2[pIndex] = -1;
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
    } else {
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = LoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_centre_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_centre_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_radius_1[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_radius_2[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_start_angle[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_end_angle[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_needle_colour[pIndex]);
    }
}
C2_HOOK_FUNCTION(0x0048b810, LoadTacho)

void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec) {
    char s[256];
    char* str;
    char the_char1;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%c", &the_char1);
    if (the_char1 == 'd') {
        /* Speedo type, x, y, filename, x-pitch */
        pCar_spec->speedo_radius_2[pIndex] = -1;
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        if (pCar_spec->speedo_image[pIndex] == NULL) {
            FatalError(kFatalError_CannotLoadSpeedoImage);
        }
        pCar_spec->speedo_y_pitch[pIndex] = pCar_spec->speedo_image[pIndex]->height / 10;
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_x_pitch[pIndex]);
    } else {
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->speedo_image[pIndex] = LoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_centre_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_centre_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_radius_1[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_radius_2[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_start_angle[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_end_angle[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->speedo_needle_colour[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->max_speed);
    }
}
C2_HOOK_FUNCTION(0x0048b560, LoadSpeedo)

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
    /* Number of clauses */
    count = GetAnInt(pF);
    pCar_spec->damage_programs[pImpact_location].clause_count = count;
    pCar_spec->damage_programs[pImpact_location].clauses = BrMemAllocate(count * sizeof(tDamage_clause), kMem_damage_clauses);
    c2_strcpy(delim, "\t ,/");
    c2_strcat(delim, "&");

    for (i = 0; i < count; i++) {
        the_clause = &pCar_spec->damage_programs[pImpact_location].clauses[i];
        the_clause->condition_count = 0;
        GetALineAndDontArgue(pF, s);
        str = c2_strtok(s, delim);
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
                the_clause->conditions[the_clause->condition_count].condition_operator = 1;
            } else if (str[1] == '<') {
                the_clause->conditions[the_clause->condition_count].condition_operator = 0;
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
            c2_sscanf(&str[2], "%f", &the_clause->conditions[the_clause->condition_count].comparitor);
            the_clause->condition_count++;
            str = c2_strtok(NULL, delim);
        } while (the_clause->condition_count < 2);
        /* Systems count */
        the_clause->effect_count = GetAnInt(pF);
        for (j = 0; j < the_clause->effect_count; j++) {
            the_clause->effects[j].type = -1;
            /* Damage */
            GetALineAndDontArgue(pF, s);
            str = c2_strtok(s, "\t ,/");
            for (k = 0; k < REC2_ASIZE(C2V(gDamage_names)); k++) {
                if (c2_strcmp(str, C2V(gDamage_names)[k]) == 0) {
                    the_clause->effects[j].type = k;
                    break;
                }
            }
            if (the_clause->effects[j].type < 0) {
                FatalError(kFatalError_UnknownDamageType_S, str);
            }
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%f", &the_clause->effects[j].weakness_factor);
        }
    }
}
C2_HOOK_FUNCTION(0x0048bca0, GetDamageProgram)

void C2_HOOK_FASTCALL UpdateMaterialsForCar(tBrender_storage* pStorage, tCar_spec* pCar_spec) {
    int i;
    br_material* material;

    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        material = pStorage->materials[i];
        if (C2V(gNbPixelBits) == 16) {
            material->ka = 0.6f;
            material->kd = 0.2f;
            material->flags &= ~BR_MATF_PRELIT;
            material->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
        }
    }
}
C2_HOOK_FUNCTION(0x004f67a0, UpdateMaterialsForCar)

void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec) {

    pCar_spec->old_material_count = pStorage->materials_count;
    LoadAllMaterialsInDirectory(pStorage, pPath, kRendererShadingType_Specular);
    pCar_spec->new_material_count = pStorage->materials_count;
    UpdateMaterialsForCar(pStorage, pCar_spec);
}
C2_HOOK_FUNCTION(0x004f6740, LoadCarMaterials)

void C2_HOOK_FASTCALL LoadCarShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec) {
    int i;
    char s[256];

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_shrapnel_materials, 0x6c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, shrapnel_materials, 0x84);

    /* number of materials */
    pCar_spec->count_shrapnel_materials = GetAnInt(pF);

    for (i = 0; i < pCar_spec->count_shrapnel_materials; i++) {
        GetALineAndDontArgue(pF, s);
        pCar_spec->shrapnel_materials[i] = GetSimpleMaterial(s, (pCar_spec == NULL || pCar_spec->driver < 6) ? 1 : 2);
    }

}
C2_HOOK_FUNCTION(0x00488f00, LoadCarShrapnelMaterials)

void (C2_HOOK_FASTCALL * LoadCar_original)(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);
void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadCar_original(pCar_name, pDriver, pCar_spec, pOwner, pDriver_name, pStorage_space);
#else
    tBrender_storage* prev_storage;
    tPath_name the_path;
    char load_name[256];
    char load_directory[256];
    char car_path[256];
    char actor_path[256];
    tTWTVFS twt;
    int i;
    int j;
    int k;
    int version;
    float temp_float;
    FILE* f;
    FILE* g;
    FILE* h;
    char s[256];
    char* str;
    int old_model_count;
    tCar_crush_buffer car_crush_buffer;
    br_model* model;
    tUser_crush_data* user_crush;
    int count_vertices;
    br_material* car_material;
    tCrush_model_pool model_pool;

    C2_HOOK_BUG_ON(sizeof(*pCar_spec) != 6500);
    c2_memset(pCar_spec, 0, sizeof(*pCar_spec));

    prev_storage = C2V(gStorageForCallbacks);
    C2V(gCurrent_car_spec) = pCar_spec;

    if (pDriver == eDriver_local_human) {
        c2_strcpy(C2V(gProgram_state).car_name, pCar_name);
    }
    c2_strcpy(C2V(gCurrent_load_directory), "CARS");
    c2_strcpy(C2V(gCurrent_load_name), pCar_name);
    C2V(gCurrent_load_name)[c2_strlen(C2V(gCurrent_load_name)) - 4] = '\0';
    c2_strcpy(load_directory, C2V(gCurrent_load_directory));
    c2_strcpy(load_name, C2V(gCurrent_load_name));
    PathCat(car_path, C2V(gApplication_path), load_directory);
    PathCat(car_path, car_path, load_name);
    twt = OpenPackFileAndSetTiffLoading(car_path);

    pCar_spec->is_girl = 0;
    pCar_spec->driver = pDriver;
    pCar_spec->index = pOwner;

    if (pDriver == eDriver_local_human) {
        C2V(gFunk_groove_flags)[0] = 1;
        C2V(gGroove_funk_offset) = 0;
    } else {
        C2V(gGroove_funk_offset) = -1;
        for (i = 1; i < REC2_ASIZE(C2V(gFunk_groove_flags)); i++) {
            if (!C2V(gFunk_groove_flags)[i]) {
                pCar_spec->fg_index = i;
                C2V(gFunk_groove_flags)[i] = 1;
                C2V(gGroove_funk_offset) = GROOVE_FUNK_MAX_PER_CAR * i;
                break;
            }
        }
    }
    if (C2V(gGroove_funk_offset) < 0) {
        FatalError(kFatalError_RanOutOfFunkGrooveSlotBunches);
    }

    if (c2_strcmp(pCar_name, "STELLA.TXT") == 0) {
        pCar_spec->proxy_ray_distance = 6.0f;
    } else {
        pCar_spec->proxy_ray_distance = 0.0f;
    }

    PathCat(the_path, car_path, pCar_name);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadCarResolutionIndependentFile);
    }

    c2_strcpy(the_path, car_path);
    PathCat(the_path, the_path, C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, pCar_name);
    AllowOpenToFail();
    g = DRfopen(the_path, "rt");
    DisallowOpenToFail();
    if (g == NULL) {
        PathCat(the_path, C2V(gApplication_path), "CARS");
        PathCat(the_path, the_path, C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
        PathCat(the_path, the_path, C2V(gBasic_car_name));
        g = DRfopen(the_path, "rt");
        if (g == NULL) {
            FatalError(kFatalError_CannotOpenCarResolutionDependentFile);
        }
    }

    /* VERSION X
     *      Version 1 :             New crush data
     *              2 :             New windscreen spec
     **/
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    if (c2_strcmp(str, "VERSION") == 0) {
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &version);
        GetAString(f, s);
    } else {
        version = 0;
    }
    if (c2_strcmp(s, "BOY") == 0) {
        pCar_spec->is_girl = 0;
        GetAString(f, s);
    } else if (c2_strcmp(s, "GIRL") == 0) {
        pCar_spec->is_girl = 1;
        GetAString(f, s);
    }
    /* Name of car */
    c2_strcpy(pCar_spec->name, s);
    if (DRStricmp(s, pCar_name) != 0) {
        FatalError(kFatalError_FileIsCorrupted_S, pCar_name);
    }
    if (pDriver_name[0] != '\0') {
        c2_memcpy(pCar_spec->driver_name, pDriver_name, REC2_ASIZE(pCar_spec->driver_name));
        pCar_spec->driver_name[REC2_ASIZE(pCar_spec->driver_name) - 1] = '\0';
    } else {
        c2_strcpy(pCar_spec->driver_name, "X");
    }
    pCar_spec->can_be_stolen = 0;
    pCar_spec->has_been_stolen = 0;
    pCar_spec->knackered = 0;
    pCar_spec->time_last_hit = 0;
    pCar_spec->time_last_victim = 0;
    pCar_spec->disabled = 0;
    pCar_spec->active = 1;
    for (i = 0; i < REC2_ASIZE(pCar_spec->power_up_levels); ++i) {
        pCar_spec->power_up_levels[i] = C2V(gCurrent_APO_levels)[i];
        pCar_spec->power_up_slots[i] = C2V(gCurrent_APO_potential_levels)[i];
    }
    /* softness_factor */
    pCar_spec->softness_factor = GetAFloat(f);
    /* START OF DRIVABLE STUFF */
    GetALineAndDontArgue(f, s);
    C2_HOOK_ASSERT(c2_strcmp(s, "START OF DRIVABLE STUFF") == 0);
    if (pDriver == eDriver_local_human) {
        if (twt >= 0) {
            PackFileRevertTiffLoading();
        }
        for (i = 0; i < REC2_ASIZE(pCar_spec->cockpit_images); i++) {
            /* [0] Cockpit forward image file names
             * [1] Cockpit left image file names
             * [2] Cockpit right image file names */
            GetALineAndDontArgue(g, s);
            c2_strtok(s, "\t ,/");
            pCar_spec->cockpit_images[i] = NULL;

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, cockpit_images, 0x138);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, render_left, 0x1dc);

            /* Left, top, right, bottom rendering coordinates */
            GetALineAndDontArgue(g, s);
            str = c2_strtok(s, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->render_left[i]);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->render_top[i]);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->render_right[i]);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->render_bottom[i]);

            PossibleService();
        }
        /* Speedo type, x, y, filename, x-pitch */
        LoadSpeedo(g, 0, pCar_spec);
        /* Speedo type, x, y, filename, x-pitch */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* tacho type, x, y, filename, x-pitch */
        LoadTacho(g, 0, pCar_spec);
        /* Tacho x, y, filename */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* Gear display x,y,image (external) */
        LoadGear(g, 0, pCar_spec);
        /* Gear display x,y,image (internal) */
        GetALineAndDontArgue(g, s);
        PossibleService();

        /* Number of hands frames */
        GetALineAndDontArgue(g, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->number_of_hands_images);
        for (i = 0; i < pCar_spec->number_of_hands_images; i++) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lhands_y, 0x30c);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lhands_images, 0x16c);

            /* [0] Left-most hands frame
             * [1] Leftish hands frame
             * [2] Leftish hands frame
             * [3] Centre hands frame
             * [4] Rightish hands frame
             * [5] Rightish hands frame
             * [6] Right-most hands frame */
            GetALineAndDontArgue(g, s);
            str = c2_strtok(s, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->lhands_x[i]);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->lhands_y[i]);
            str = c2_strtok(NULL, "\t ,/");
            pCar_spec->lhands_images[i] = LoadPixelmap(str);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->rhands_x[i]);
            str = c2_strtok(NULL, "\t ,/");
            c2_sscanf(str, "%d", &pCar_spec->rhands_y[i]);
            str = c2_strtok(NULL, "\t ,/");
            PossibleService();
        }
        pCar_spec->red_line = 8000;

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driver_x_offset, 0x47c);

        /* Offset of driver's head in 3D space */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->driver_x_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->driver_y_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->driver_z_offset);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, head_left_angle, 0x498);

        /* Angles to turn to make head go left and right */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->head_left_angle);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->head_right_angle);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, mirror_x_offset, 0x488);

        /* Offset of 'mirror camera' in 3D space, viewing angle of mirror */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->mirror_x_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->mirror_y_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->mirror_z_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%f", &pCar_spec->rearview_camera_angle);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, mirror_left, 0x20c);

        /* Left, top, right, bottom of mirror */
        GetALineAndDontArgue(g, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->mirror_left);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->mirror_top);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->mirror_right);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->mirror_bottom);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prat_left, 0x21c);

        /* Left, top, right, bottom of pratcam (*** relative to screen, not cockpit) */
        GetALineAndDontArgue(g, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->prat_left);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->prat_top);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->prat_right);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->prat_bottom);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, prat_cam_left, 0x144);

        /* Pratcam border names (left, top, right, bottom) */
        GetALineAndDontArgue(f, s);
        PossibleService();
        str = c2_strtok(s, "\t ,/");
        pCar_spec->prat_cam_left = LoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_top = LoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_right = LoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_bottom = LoadPixelmap(str);
        PossibleService();

        for (i = 0; i < REC2_ASIZE(pCar_spec->damage_units); i++) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_units, 0x4d8);
            if (i == eDamage_driver) {
                pCar_spec->damage_units[i].images = NULL;
            } else {
                /* [ 0] Engine damage x,y,flash1..5
                 * [ 1] Transmission damage x,y,flash1..5
                 * [ 3] Steering damage x,y,flash1..5
                 * [ 4] lf brake damage x,y,flash1..5
                 * [ 5] rf brake damage x,y,flash1..5
                 * [ 6] lr brake damage x,y,flash1..5
                 * [ 7] rr brake damage x,y,flash1..5
                 * [ 8] lf wheel damage x,y,flash1..5
                 * [ 9] rf wheel damage x,y,flash1..5
                 * [10] lr wheel damage x,y,flash1..5
                 * [11] rr wheel damage x,y,flash1..5 */
                GetALineAndDontArgue(g, s);
                str = c2_strtok(s, "\t ,/");
                c2_sscanf(str, "%d", &pCar_spec->damage_units[i].x_coord);
                str = c2_strtok(NULL, "\t ,/");
                c2_sscanf(str, "%d", &pCar_spec->damage_units[i].y_coord);
                for (k = 0; k < REC2_ASIZE(pCar_spec->damage_units[i].periods); k++) {
                    str = c2_strtok(NULL, "\t ,/");
                    c2_sscanf(str, "%f", &temp_float);
                    pCar_spec->damage_units[i].periods[k] = (int)(1000.0f / temp_float / 2.0f);
                }
                str = c2_strtok(NULL, "\t ,/");
                pCar_spec->damage_units[i].images = LoadPixelmap(str);
                if (pCar_spec->damage_units[i].images == NULL) {
                    FatalError(kFatalError_CannotLoadDamageImage);
                }
            }
            pCar_spec->damage_units[i].damage_level = 0;
            PossibleService();
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damage_x_offset, 0x368);

        GetALineAndDontArgue(g, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->damage_x_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->damage_y_offset);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->damage_background_x);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->damage_background_y);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->damage_background = LoadPixelmap(str);

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dim_count, 0x378);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, dim_right, 0x3c0);

        /* Number of dimmed areas (external) */
        pCar_spec->dim_count[0] = GetAnInt(g);
        for (i = 0; i < REC2_ASIZE(pCar_spec->dim_count); i++) {
            for (j = 0; j < pCar_spec->dim_count[i]; j++) {
                GetFourInts(g,
                    &pCar_spec->dim_left[i][j],
                    &pCar_spec->dim_top[i][j],
                    &pCar_spec->dim_right[i][j],
                    &pCar_spec->dim_bottom[i][j]);
            }
        }

        /* Number of dimmed areas (internal) */
        /* ignored */

        PathCat(the_path,
            C2V(gApplication_path),
            C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
        PathCat(the_path, the_path, "HEADUP.TXT");
        h = DRfopen(the_path, "rt");
        if (h == NULL) {
            FatalError(kFatalError_CannotOpenHeadupsFile);
        }
        PossibleService();
        LoadHeadups(h, 0, pCar_spec);
        LoadHeadups(h, 1, pCar_spec);
        PossibleService();
        PFfclose(h);

        AdjustCarCoordinates(&C2V(gProgram_state).current_car);
        AdjustRenderScreenSize();
        PossibleService();
        ReinitialiseRearviewCamera();
        GetALineAndDontArgue(f, s);
        if (twt >= 0) {
            PackFileRerevertTiffLoading();
        }
        C2_HOOK_ASSERT(c2_strcmp(s, "END OF DRIVABLE STUFF") == 0);
    } else {
        while (!PFfeof(f)) {
            GetALineAndDontArgue(f, s);
            if (c2_strcmp(s, "END OF DRIVABLE STUFF") == 0) {
                break;
            }
        }
        pCar_spec->red_line = 8000;
    }
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, engine_noises, 0x470);

    /* Engine noise (normal, enclosed space, underwater) */
    GetThreeInts(f,
        &pCar_spec->engine_noises[0],
        &pCar_spec->engine_noises[1],
        &pCar_spec->engine_noises[2]);

    /* Can be stolen */
    GetAString(f, s);
    pCar_spec->can_be_stolen = c2_strcmp(s, "stealworthy") == 0;

    /* Damage info for top impacts */
    GetDamageProgram(f, pCar_spec, eImpact_top);
    /* Damage info for bottom impacts */
    GetDamageProgram(f, pCar_spec, eImpact_bottom);
    /* Damage info for left impacts */
    GetDamageProgram(f, pCar_spec, eImpact_left);
    /* Damage info for right impacts */
    GetDamageProgram(f, pCar_spec, eImpact_right);
    /* Damage info for front impacts */
    GetDamageProgram(f, pCar_spec, eImpact_front);
    /* Damage info for rear impacts */
    GetDamageProgram(f, pCar_spec, eImpact_back);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grid_icon_names, 0x10c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, grid_icon_image, 0x1a4);

    /* Grid image (opponent, frank, annie */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_strcpy(pCar_spec->grid_icon_names[0], str);
    str = c2_strtok(NULL, "\t ,/");
    c2_strcpy(pCar_spec->grid_icon_names[1], str);
    str = c2_strtok(NULL, "\t ,/");
    c2_strcpy(pCar_spec->grid_icon_names[2], str);
    pCar_spec->grid_icon_image = NULL;

    LoadAllImagesInDirectory(pStorage_space, car_path);
    LoadAllShadeTablesInDirectory(pStorage_space, car_path);
    LoadCarMaterials(pStorage_space, car_path, pCar_spec);
    old_model_count = pStorage_space->models_count;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_detail_levels, 0xe14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, detail_levels, 0xe1c);

    /* Number of extra levels of detail */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &pCar_spec->count_detail_levels);
    pCar_spec->count_detail_levels++;
    if (pCar_spec->count_detail_levels > REC2_ASIZE(((tUser_crush_data*)0)->models)) {
        PDFatalError("Too many levels of detail");
    }
    pCar_spec->detail_levels[0] = 0.f;
    for (i = 1; i < pCar_spec->count_detail_levels; i++) {
        /* min_dist_squared */
        pCar_spec->detail_levels[i] = GetAScalar(f);
    }
    LoadAllModelsInDirectory(pStorage_space, car_path);
    if (version > 0) {
        PossibleService();

        /* crush data file (will be incorporated into this file) (.WAM filename) */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        PathCat(the_path, car_path, str);
        if (LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec)) {
            PathCat(the_path, car_path, pCar_name);
            c2_strcpy(&the_path[c2_strlen(the_path) - 3], "WAM");
            LoadCarCrush(&car_crush_buffer, the_path, pStorage_space, &pCar_spec->car_crush_spec);
        }
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_master_actor, 0x10);

    pCar_spec->car_master_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrActorAdd(C2V(gNon_track_actor), pCar_spec->car_master_actor);
    if (pDriver == eDriver_local_human) {
        C2V(gPlayer_car_master_actor) = pCar_spec->car_master_actor;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_model_actor, 0xe0c);

    PathCat(actor_path, car_path, load_name);
    c2_strcat(actor_path, ".ACT");
    pCar_spec->car_model_actor = BrActorLoad(actor_path);
    if (pCar_spec->car_model_actor == NULL) {
        FatalError(kFatalError_CannotLoadCarActor);
    }
    BrActorAdd(pCar_spec->car_master_actor, pCar_spec->car_model_actor);

    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCrushData, NULL);

    model_pool.models = &pStorage_space->models[old_model_count];
    model_pool.model_count = pStorage_space->models_count - old_model_count;
    model_pool.count_detail_levels = pCar_spec->count_detail_levels;
    DRActorEnumRecurse(pCar_spec->car_model_actor, LinkCrushModel, &model_pool);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, shell_model, 0xe28);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_actor, 0xe10);

    model = NULL;
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        if (c2_strcmp(pStorage_space->models[i]->identifier, "SHELL") == 0) {
            model = pStorage_space->models[i];
            break;
        }
    }
    pCar_spec->shell_model = model;
    pCar_spec->car_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    user_crush = pCar_spec->car_model_actor->user;
    if (user_crush == NULL || user_crush->models[0] == NULL) {
        PDFatalError("Can't find main car model");
    }
    pCar_spec->car_actor->model = user_crush->models[0];
    BrMatrix34Copy(&pCar_spec->car_actor->t.t.mat, &pCar_spec->car_model_actor->t.t.mat);
    if (version > 0) {
        LinkCarCrushDatas(pCar_spec->car_model_actor, &car_crush_buffer);
    }
    DRActorEnumRecurse(pCar_spec->car_model_actor, AllocateUserDetailLevel, pCar_spec);
    PossibleService();
    for (i = old_model_count; i < pStorage_space->models_count; i++) {
        SetModelFlags(pStorage_space->models[i], pOwner);
    }

    /* Name of reflective screen material (or none if non-reflective) */
    GetAString(f, s);
    if (version > 1) {
        /* Percentage transparency of windscreen */
        GetAnInt(f);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, number_of_steerable_wheels, 0x404);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steering_ref, 0x408);

    /* Number of steerable wheels */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &pCar_spec->number_of_steerable_wheels);
    for (i = 0; i < pCar_spec->number_of_steerable_wheels; i++) {
        /* GroovyFunkRef of x'th steerable wheel */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->steering_ref[i]);
        AddRefOffset(&pCar_spec->steering_ref[i]);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lf_sus_ref, 0x420);

    /* Left-front suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->lf_sus_ref); i++) {
        c2_sscanf(str, "%d", &pCar_spec->lf_sus_ref[i]);
        AddRefOffset(&pCar_spec->lf_sus_ref[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rf_sus_ref, 0x430);

    /* Right-front suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->rf_sus_ref); i++) {
        c2_sscanf(str, "%d", &pCar_spec->rf_sus_ref[i]);
        AddRefOffset(&pCar_spec->rf_sus_ref[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, lr_sus_ref, 0x440);

    /* Left-rear suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->lr_sus_ref); i++) {
        c2_sscanf(str, "%d", &pCar_spec->lr_sus_ref[i]);
        AddRefOffset(&pCar_spec->lr_sus_ref[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rr_sus_ref, 0x448);

    /* Right-rear suspension parts GroovyFunkRef */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    for (i = 0; i < REC2_ASIZE(pCar_spec->rr_sus_ref); i++) {
        c2_sscanf(str, "%d", &pCar_spec->rr_sus_ref[i]);
        AddRefOffset(&pCar_spec->rr_sus_ref[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_1, 0x450);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_2, 0x454);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_3, 0x458);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_spin_ref_4, 0x45c);

    /* Driven wheels GroovyFunkRefs (for spinning) - MUST BE 4 ITEMS */
    GetFourInts(f,
        &pCar_spec->driven_wheels_spin_ref_1,
        &pCar_spec->driven_wheels_spin_ref_2,
        &pCar_spec->driven_wheels_spin_ref_3,
        &pCar_spec->driven_wheels_spin_ref_4);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_1);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_2);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_3);
    AddRefOffset(&pCar_spec->driven_wheels_spin_ref_4);
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_1, 0x460);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_2, 0x464);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_3, 0x468);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_spin_ref_4, 0x46c);

    /* Non-driven wheels GroovyFunkRefs (for spinning) - MUST BE 4 ITEMS */
    GetFourInts(f,
        &pCar_spec->non_driven_wheels_spin_ref_1,
        &pCar_spec->non_driven_wheels_spin_ref_2,
        &pCar_spec->non_driven_wheels_spin_ref_3,
        &pCar_spec->non_driven_wheels_spin_ref_4);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_1);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_2);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_3);
    AddRefOffset(&pCar_spec->non_driven_wheels_spin_ref_4);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, driven_wheels_circum, 0x4b8);

    /* Driven wheels diameter */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", &temp_float);
    pCar_spec->driven_wheels_circum = (float)(2.f * temp_float * REC2_PI);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, non_driven_wheels_circum, 0x4bc);

    /* Non-driven wheels diameter */
    GetALineAndDontArgue(f, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", &temp_float);
    pCar_spec->non_driven_wheels_circum = (float)(temp_float * 2.f * REC2_PI);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_model_variable, 0x400);

    pCar_spec->car_model_variable = pDriver != eDriver_local_human;
    PossibleService();

    /* START OF FUNK */
    GetALineAndDontArgue(f, s);
    AddFunkotronics(f, pOwner, C2V(gGroove_funk_offset), &car_crush_buffer);
    PossibleService();

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wheel_actors, 0x12ec);

    for (i = 0; i < REC2_ASIZE(C2V(gWheel_actor_names)); i++) {
        pCar_spec->wheel_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, C2V(gWheel_actor_names)[i]);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, pivot_actors, 0x1304);

    for (i = 0; i < REC2_ASIZE(C2V(gPivot_actor_names)); i++) {
        pCar_spec->pivot_actors[i] = DRActorFindRecurse(pCar_spec->car_master_actor, C2V(gPivot_actor_names)[i]);
    }

    for (i = 0; i < REC2_ASIZE(pCar_spec->wheel_actors); i++) {
        br_vector3 avg;
        br_actor* actor;
        br_model* model;

        actor = pCar_spec->wheel_actors[i];
        if (actor == NULL) {
            continue;
        }
        model = actor->model;
        if (model == NULL) {
            continue;
        }
        Vector3Average(&avg, &actor->model->bounds.min, &actor->model->bounds.max);
        if (!Vector3IsZero(&avg)) {
            BrVector3Accumulate(&actor->t.t.translate.t, &avg);
            for (j = 0; j < actor->model->nvertices; j++) {
                BrVector3Sub(&actor->model->vertices[j].p, &actor->model->vertices[j].p, &avg);
            }
            BrModelUpdate(actor->model, BR_MODU_VERTEX_POSITIONS);
        }
    }

    for (i = 0; i < REC2_ASIZE(pCar_spec->pivot_actors); i++) {
        br_actor* actor;

        actor = pCar_spec->pivot_actors[i];
        if (actor == NULL || actor->children == NULL) {
            continue;
        }
        BrVector3Accumulate(&actor->t.t.translate.t, &actor->children->t.t.translate.t);
        BrVector3Set(&actor->children->t.t.translate.t, 0.f, 0.f, 0.f);
    }

    /* START OF GROOVE */
    GetALineAndDontArgue(f, s);
    AddGroovidelics(f, pOwner, pCar_spec->car_master_actor, C2V(gGroove_funk_offset), 1);

    ReadMechanics(f, pCar_spec, version);

    PossibleService();

    /* Materials for shrapnel */
    LoadCarShrapnelMaterials(f, pCar_spec);

    count_vertices = 0;
    v11model* v11 = pCar_spec->car_actor->model->prepared;
    for (i = 0; i < v11->ngroups; i++) {
        count_vertices += v11->groups[i].nvertices;
    }

    int all_fire_zero = 1;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, fire_vertex, 0xac);
    C2_HOOK_BUG_ON(REC2_ASIZE(pCar_spec->fire_vertex) != 12);

    /* damage vertices fire points */
    for (i = 0; i < REC2_ASIZE(pCar_spec->fire_vertex); i++) {
        int vertex_i;

        pCar_spec->fire_vertex[i] = GetAnInt(f);
        if (pCar_spec->fire_vertex[i] >= count_vertices) {
            pCar_spec->fire_vertex[i] = 0;
        }
        if (pCar_spec->fire_vertex[i] != 0) {
            all_fire_zero = 0;
        }
        vertex_i = 0;
        for (j = 0; j < v11->ngroups; j++) {
            for (k = 0; k < v11->groups[j].nvertices; k++) {
                if (v11->groups[j].vertex_user[k] == pCar_spec->fire_vertex[i]) {
                    pCar_spec->fire_vertex[i] = vertex_i;
                    j = v11->ngroups;
                    break;
                }
                vertex_i++;
            }
        }
    }

    if (all_fire_zero) {
        for (i = 0; i < REC2_ASIZE(pCar_spec->fire_vertex); i++) {
            pCar_spec->fire_vertex[i] = IRandomBetween(0, count_vertices - 1);
        }
    }

    while (!PFfeof(f)) {
        GetAString(f, s);

        if (DRStricmp(s, "END") == 0) {
            break;
        } else if (DRStricmp(s, "CAMERA_POSITIONS") == 0) {

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, camera_bumper_position, 0x18d8);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, camera_cockpit_position, 0x18e4);

            /* bumper position */
            GetAVector(f, &pCar_spec->camera_bumper_position);

            /* cockpit position */
            GetAVector(f, &pCar_spec->camera_cockpit_position);
        } else if (DRStricmp(s, "CAMERA_TURN_OFF_MATERIALS") == 0) {

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, count_window_materials, 0x18f0);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, window_materials, 0x18f4);

            /* Count */
            pCar_spec->count_window_materials = GetAnInt(f);
            for (i = 0; i < pCar_spec->count_window_materials; i++) {
                GetAString(f, s);
                pCar_spec->window_materials[i].material = BrMaterialFind(s);
                pCar_spec->window_materials[i].count_maps = GetAnInt(f);
                for (j = 0; j < pCar_spec->window_materials[i].count_maps; j++) {
                    GetAString(f, s);
                    pCar_spec->window_materials[i].maps[j] = BrMapFind(s);
                }
            }
        }
    }

    PFfclose(f);
    PFfclose(g);
    C2V(gCurrent_car_spec) = NULL;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x18c8, 0x18c8);

    pCar_spec->field_0x18c8 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, AttachGroovidelic, NULL);
    AttachCrushDataToActorModels(pCar_spec->car_model_actor, pCar_spec);
    PrepareCarForCrushing(pCar_spec);
    ClosePackFileAndSetTiffLoading(twt);
    car_material = BrMaterialAllocate("PorterIsAWxnkxr");
    if (AddMaterialToStorage(pStorage_space, car_material) == eStorage_not_enough_room) {
        FatalError(kFatalError_InsufficientMaterialSlots);
    }
    BrMaterialAdd(car_material);
    pCar_spec->car_master_actor->material = car_material;
    AdaptCachedMaterials(kRendererShadingType_Specular);
    for (i = pCar_spec->old_material_count; i < pCar_spec->new_material_count; i++) {
        br_material* material;

        material = pStorage_space->materials[i];
        if (material->identifier != NULL && material->identifier[0] != '\0') {
            material->identifier[0] = (material->flags & BR_MATF_TWO_SIDED) ? 'D' : 'S';
            BrMaterialUpdate(material, BR_MATU_ALL);
        }
    }
    C2V(gStorageForCallbacks) = prev_storage;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488f70, LoadCar, LoadCar_original)

void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info) {
    int i;
    int j;
    tOpp_spec* opponent;
    int sod_counter;
    char buffer[24];
    const char* driver_name;

    if (C2V(gNet_mode) == eNet_mode_none) {
        AboutToLoadFirstCar();
        PrintMemoryDump(0, "JUST BEFORE LOADING YOUR CAR");
        LoadCar(C2V(gOpponents)[C2V(gProgram_state).current_car_index].car_file_name,
            eDriver_local_human,
            &C2V(gProgram_state).current_car,
            0,
            C2V(gProgram_state).player_name,
            &C2V(gOur_car_storage_space));
        SetCarStorageTexturingLevel(&C2V(gOur_car_storage_space), GetCarTexturingLevel(), eCTL_full);
        PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING YOUR CAR");
    }

    C2V(gGroove_funk_offset) = GROOVE_FUNK_MAX_PER_CAR;
    C2V(gCount_opponents) = 0;
    if (C2V(gNet_mode) == eNet_mode_none) {
        sod_counter = 0;
        for (i = 0; i < pRace_info->number_of_racers; i++) {
            opponent = &pRace_info->opponent_list[i];
            PossibleService();
            if (opponent->index >= 0) {
                C2_HOOK_BUG_ON(sizeof(tCar_spec) != 6500);
                opponent->car_spec = BrMemAllocate(sizeof(tCar_spec), kMem_oppo_car_spec);
                if (DRStricmp("MAX DAMAGE", C2V(gOpponents)[opponent->index].name) == 0) {
                    sod_counter++;
                    c2_sprintf(buffer, "POOR SOD %d", sod_counter);
                    driver_name = buffer;
                } else {
                    driver_name = C2V(gOpponents)[opponent->index].name;
                }
                LoadCar(C2V(gOpponents)[opponent->index].car_file_name,
                    eDriver_oppo,
                    opponent->car_spec,
                    opponent->index,
                    driver_name,
                    &C2V(gTheir_cars_storage_space));
                PrintMemoryDump(0, "IMMEDIATELY AFTER LOADING AN OPPONENT");
            }
            if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Cars) {
                if (C2V(gCurrent_race).race_spec->options.cars.count_opponents < 0) {
                    C2V(gCount_opponents)++;
                    opponent->car_spec->is_race_goal = 1;
                } else {
                    for (j = 0; j < C2V(gCurrent_race).race_spec->options.cars.count_opponents; j++) {
                        if (opponent->index == C2V(gCurrent_race).race_spec->options.cars.opponents[j]) {
                            C2V(gCount_opponents)++;
                            opponent->car_spec->is_race_goal = 1;
                            break;
                        }
                    }
                }
            }
        }
        SetCarStorageTexturingLevel(&C2V(gTheir_cars_storage_space), GetCarTexturingLevel(), eCTL_full);
    }
}
C2_HOOK_FUNCTION(0x0048cda0, LoadOpponentsCars)

void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner) {

    if (pModel != NULL && pModel->nfaces != 0) {
        if (pOwner == OPPONENT_APC_IDX || C2V(gAusterity_mode)) {
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

void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder) {

    if (*pRef_holder >= 0) {
        *pRef_holder += C2V(gGroove_funk_offset);
    }
}

void C2_HOOK_FASTCALL ReadMechanics(FILE* pF, tCar_spec* c, int pSpec_version) {
    int i;
    int j;
    char* str;
    char s[256];
    char* version_str;
    br_scalar red_gear_speed;
    br_scalar acceleration_highest_gear;
    float ixp, iyp, izp;
    int count_wheels;
    br_vector3 wpos;
    int steer_flags;
    br_scalar susp_give;
    br_scalar damping;
    br_scalar friction_slipping_reduction;
    br_scalar friction_angle_1, friction_angle_2;
    br_scalar traction_multiplier;
    br_scalar rolling_resistance;
    br_scalar friction_steer;
    br_scalar friction_non_steer_1, friction_non_steer_2;
    int wpos_i;

#if defined(REC2_FIX_BUGS)
    friction_non_steer_1 = 0.f;
    friction_non_steer_2 = 0.f;
    friction_steer = 0.f;
#endif

    if (pSpec_version == 0) {
        str = GetALineAndDontArgue(pF, s);
        if (str == NULL || c2_strcmp(str, "START OF MECHANICS STUFF") != 0) {
            PDFatalError("Can't find old end of crush data comment");
        }
    } else {
        /* START OF MECHANICS STUFF version 1 */
        GetALineAndDontArgue(pF, s);
    }
    PossibleService();
    for (i = c2_strlen(s) - 1; s[i] == ' '; i--) {
    }
    version_str = &s[i + 1];
    (void)version_str;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, maxcurve, 0x1258);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, initial_brake, 0x12ac);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, brake_increase, 0x12b0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, max_gear, 0x1364);

    /* Minimum turning circle. */
    c->maxcurve = 1.0f / GetAScalar(pF);

    /* Brake multiplier. */
    c->initial_brake = GetAScalar(pF);

    /* Braking strength multiplier. */
    c->brake_increase = GetAScalar(pF);

    /* Number of gears. */
    c->max_gear = GetAnInt(pF);

    /* Speed at red line in highest gear. */
    red_gear_speed = GetAScalar(pF);

    /* Acceleration in highest gear (m/s^2) i.e. engine strength. */
    acceleration_highest_gear = GetAScalar(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, collision_info, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, owner, 0x23c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, flags_0x238, 0x238);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x1a0, 0x1a0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x1a4, 0x1a4);

    C2_HOOK_BUG_ON(sizeof(tCollision_info) != 0x4d8);
    c->collision_info = BrMemAllocate(sizeof(tCollision_info), kMem_collision_object);
    c->collision_info->owner = c;
    c->collision_info->flags_0x238 = 1;
    c->collision_info->field_0x1a0 = 0xffff;
    c->collision_info->field_0x1a4 = 0;

    /* Sub member: Root part */
    /* Type */
    GetAString(pF, s);
    /* Identifier */
    GetAString(pF, s);
    /* Actor */
    GetAString(pF, s);

    c->collision_info->actor = c->car_master_actor;

    /* Sub member: Joint data */
    /* Type */
    GetAString(pF, s);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, cmpos, 0x14);

    /* Centre of mass */
    GetThreeFloats(pF,
        &c->collision_info->cmpos.v[0],
        &c->collision_info->cmpos.v[1],
        &c->collision_info->cmpos.v[2]);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, M, 0x4);

    /* Mass */
    c->collision_info->M = GetAScalar(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, I, 0x8);

    /* Angular momentum proportions */
    GetThreeFloats(pF, &ixp, &iyp, &izp);
    c->collision_info->I.v[0] = (izp * izp + iyp * iyp) * c->collision_info->M / 12.f;
    c->collision_info->I.v[1] = (ixp * ixp + izp * izp) * c->collision_info->M / 12.f;
    c->collision_info->I.v[2] = (iyp * iyp + ixp * ixp) * c->collision_info->M / 12.f;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, downforce_to_weight, 0x12a4);

    /* Downforce-to-weight balance speed */
    c->downforce_to_weight = GetAScalar(pF);

    /* Number of 'Wheels' entries. */
    count_wheels = GetAnInt(pF);

    if (count_wheels != 4) {
        FatalError(kFatalError_InvalidScreenDepthSetting);
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(c->wpos) != 4);

    for (i = 0; i < REC2_ASIZE(c->wpos); i++) {
        /* Wheels entry #1 */
        /* Type */
        GetAnInt(pF);

        /* Identifier */
        GetAString(pF, s);

        /* Actor */
        GetAString(pF, s);

        /* Position */
        GetThreeFloats(pF, &wpos.v[0], &wpos.v[1], &wpos.v[2]);

        if (Vector3IsZero(&wpos)) {
            for (j = 0; j < REC2_ASIZE(c->wheel_actors); j++) {
                br_actor* parent;

                if (c->wheel_actors[j] == NULL) {
                    continue;
                }
                if (DRStricmp(s, c->wheel_actors[j]->identifier) != 0) {
                    continue;
                }

                BrVector3Copy(&wpos, &c->wheel_actors[j]->t.t.translate.t);
                parent = c->wheel_actors[j]->parent;
                while (parent != NULL && parent != c->car_master_actor) {
                    BrVector3Accumulate(&wpos, &parent->t.t.translate.t);
                    parent = parent->parent;
                }
                break;
            }
        }

        /* Steerable flags */
        steer_flags = GetAnInt(pF);

        /* Driven flags */
        GetAnInt(pF);

        /* Suspension give */
        susp_give = GetAScalar(pF);

        /* Damping factor */
        damping = GetAScalar(pF);

        /* Fractional reduction in friction when slipping */
        friction_slipping_reduction = GetAScalar(pF);

        /* Friction angles */
        GetPairOfFloats(pF, &friction_angle_1, &friction_angle_2);

        /* Traction fractional multiplier */
        traction_multiplier = GetAScalar(pF);

        /* Rolling resistance */
        rolling_resistance = GetAScalar(pF);

        if (steer_flags) {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steerable_rolling_resistance, 0x12c8);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, steerable_suspension_give, 0x1210);

            c->steerable_rolling_resistance = rolling_resistance;
            c->steerable_suspension_give = susp_give;
            friction_steer = friction_angle_1;
            wpos_i = 2;
        } else {
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, susp_give, 0x1214);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, rolling_resistance, 0x12cc);
            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, traction_multiplier, 0x12a0);

            c->susp_give = susp_give;
            c->rolling_resistance = rolling_resistance;
            c->traction_multiplier = traction_multiplier;
            friction_non_steer_1 = friction_angle_1;
            friction_non_steer_2 = friction_angle_2;
            wpos_i = 0;
        }
        if (wpos.v[0] > 0.f) {
            wpos_i += 1;
        }

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, wpos, 0x1224);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, friction_slipping_reduction, 0x12b4);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, damping, 0x11fc);

        BrVector3Copy(&c->wpos[wpos_i], &wpos);
        c->friction_slipping_reduction = friction_slipping_reduction;
        c->damping = damping;
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, object_friction, 0x64);

    c->collision_info->object_friction = 0.4f;

    ReadMechanicsShapes(&c->collision_info->shape, pF);

    UpdateCollisionObject(c->collision_info);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, ride_height, 0x1220);

    c->maxcurve /= WORLD_SCALE;
    c->ride_height = (c->collision_info->bb1.min.v[1] + 0.01f) * WORLD_SCALE;

    c->initial_brake *= 12.f * c->collision_info->M;
    c->brake_increase *= 12.f * c->collision_info->M;

    BrVector3Scale(&c->centre_of_mass_world_scale, &c->collision_info->cmpos, WORLD_SCALE);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, speed_revs_ratio, 0x1368);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, force_torque_ratio, 0x136c);

    c->speed_revs_ratio = red_gear_speed * 4.f / 9.f / c->max_gear / 6000.f;
    c->force_torque_ratio = c->collision_info->M * acceleration_highest_gear * c->max_gear;

    c->mu.v[0] = tanf(friction_non_steer_1 * (157.f / 9000.f)) / 4.f;
    c->mu.v[1] = tanf(friction_steer * (157.f / 9000.f)) / 4.f;
    c->mu.v[2] = tanf(friction_non_steer_2 * (157.f / 9000.f)) / 4.f;

    c->mu.v[0] *= sqrtf((c->wpos[2].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[2].v[2] - c->wpos[0].v[2]) * c->collision_info->M * 5.f);
    c->mu.v[1] *= sqrtf((c->wpos[0].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[0].v[2] - c->wpos[2].v[2]) * c->collision_info->M * 5.f);
    c->mu.v[2] *= sqrtf((c->wpos[2].v[2] - c->collision_info->cmpos.v[2]) / (c->wpos[2].v[2] - c->wpos[0].v[2]) * c->collision_info->M * 5.f);

    C2_HOOK_BUG_ON(REC2_ASIZE(c->wpos) != 4);

    for (i = 0; i < REC2_ASIZE(c->wpos); i++) {
        BrVector3Scale(&c->wpos[i], &c->wpos[i], WORLD_SCALE);
        c->wpos[i].v[1] = c->ride_height;
    }

    /* Number of sub-parts. */
    GetAnInt(pF);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, bb1, 0x24);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, bb2, 0x3c);

    c2_memcpy(&c->collision_info->bb2, &c->collision_info->bb1, sizeof(br_bounds3));

    /* END OF MECHANICS STUFF */
    GetAString(pF, s);
    C2_HOOK_ASSERT(c2_strcmp(s, "END OF MECHANICS STUFF") != 0);
}

br_material* (C2_HOOK_FASTCALL * GetSimpleMaterial_original)(char* pName, tRendererShadingType pShading_type);
br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type) {

#if 0//defined(C2_HOOKS_ENABLED)
    return GetSimpleMaterial_original(pName, pShading_type);
#else
    int i;
    br_colour colour;

    if (pName[0] == 'M') {
        c2_strtok(pName, "\t ,/");
        for (i = 0; C2V(gSimple_material_colours)[i] != 0; i++) {
            if (10 * (pName[1] - '0') + pName[2] - '0' == (C2V(gSimple_material_colours)[i] >> 24)) {
                break;
            }
        }
        if (C2V(gSimple_material_colours)[i] == 0) {
            PDFatalError("Obsolete simple material");
        }
        colour = C2V(gSimple_material_colours)[i] & 0xffffff;
    } else {
        int r, g, b;
        c2_sscanf(pName, "%d,%d,%d", &r, &g, &b);
        colour = (r << 16) | (g << 8) | (b << 0);
    }

    for (i = 0; i < REC2_ASIZE(C2V(gSimple_materials)); i++) {
        if (C2V(gSimple_materials)[i] == NULL) {
            break;
        }
        if (C2V(gSimple_materials)[i]->colour == colour) {
            return C2V(gSimple_materials)[i];
        }
    }
    if (i >= REC2_ASIZE(C2V(gSimple_materials))) {
        return NULL;
    }
    C2V(gSimple_materials)[i] = BrMaterialAllocate("Simple");
    C2V(gSimple_materials)[i]->colour = colour;
    C2V(gSimple_materials)[i]->index_base = FindBestMatchingPaletteIndex(colour);
    C2V(gSimple_materials)[i]->index_range = 0;
    GlorifyMaterial(&C2V(gSimple_materials)[i], 1, pShading_type);
    BrMaterialAdd(C2V(gSimple_materials)[i]);
    return C2V(gSimple_materials)[i];
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00488dd0, GetSimpleMaterial, GetSimpleMaterial_original);

void C2_HOOK_FASTCALL GetAVector(FILE* pF, br_vector3* pV) {
    char s[256];
    char* str;

    PossibleService();
    GetALineWithNoPossibleService(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", &pV->v[0]);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", &pV->v[1]);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", &pV->v[2]);
}
C2_HOOK_FUNCTION(0x0048fd30, GetAVector)

int C2_HOOK_CDECL AttachGroovidelic(br_actor* pActor, void* pData) {
    tUser_crush_data* user_crush;

    user_crush = pActor->user;
    if (user_crush != NULL) {
        user_crush->groove = FindGroovidelicForActor(pActor);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0048c130, AttachGroovidelic)

int C2_HOOK_CDECL ActorModelAttachCrushData(br_actor* pActor, void* pData) {
    tCar_spec* c;
    tUser_crush_data* user_crush;

    c = pData;
    (void)c;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    user_crush = pActor->user;
    if (user_crush == NULL || user_crush->crush_data == NULL || user_crush->groove == NULL) {
        return 0;
    }
    user_crush->crush_data = BrMemAllocate(sizeof(tCar_crush_buffer_entry), kMem_crush_data);
    c2_strncpy(user_crush->crush_data->actor_name, pActor->identifier, sizeof(user_crush->crush_data->actor_name) - 1);
    user_crush->crush_data->id = 0xe9;
    user_crush->crush_data->softness_factor = 1.f;
    user_crush->crush_data->field_0x2c = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x0042a220, ActorModelAttachCrushData)

void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec) {

    DRActorEnumRecurse(pActor, ActorModelAttachCrushData, pCar_spec);
}
C2_HOOK_FUNCTION(0x0042a210, AttachCrushDataToActorModels)

void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor) {
    br_model* model;
    int i;

    if (C2V(gNbPixelBits) != 16) {
        return;
    }
    model = pActor->model;
    if (model == NULL) {
        return;
    }
    if (pActor->material != NULL) {
        SmoothificatePowerupMaterial(pActor->material);
    }
    for (i = 0; i < model->nfaces; i++) {

        if (model->faces[i].material != NULL) {
            SmoothificatePowerupMaterial(model->faces[i].material);
        }
    }
}
C2_HOOK_FUNCTION(0x004f6900, SmoothificatePowerups)

void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial) {
    int need_update = 0;
    pMaterial->user = (void*)(uintptr_t)0x5ba0;

    if (pMaterial->ka <= 0.999f) {
        need_update = 1;
    }
    pMaterial->ka = 1.f;

    /* ka is tested twice, but kd is set the 2nd time */
    if (pMaterial->ka < 0.999f) {
        need_update = 1;
    }
    pMaterial->kd = 1.f;

    if (!(pMaterial->flags & BR_MATF_SMOOTH)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_SMOOTH;

    if (!(pMaterial->flags & BR_MATF_LIGHT)) {
        need_update = 1;
    }
    pMaterial->flags |= BR_MATF_LIGHT;

    if (pMaterial->flags & BR_MATF_PRELIT) {
        need_update = 1;
    }
    pMaterial->flags &= ~BR_MATF_PRELIT;

    if (need_update) {
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
}

void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial) {

    if (C2V(gNbPixelBits) != 16) {
        return;
    }
    if ((uintptr_t)pMaterial->user == 0x5ba0) {
        SmoothificatePowerupMaterial(pMaterial);
    } else {
        pMaterial->user = NULL;
        pMaterial->ka = C2V(gLighting_data).ambient_else;
        pMaterial->kd = C2V(gLighting_data).diffuse_else;
        pMaterial->ks = 0.f;
        pMaterial->flags &= ~BR_MATF_PRELIT;
        pMaterial->flags |= BR_MATF_LIGHT | BR_MATF_SMOOTH;
        BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
    }
}
C2_HOOK_FUNCTION(0x004f6a90, SmoothificateWorldMaterial)

void C2_HOOK_FASTCALL MungeLightingEffects(void) {

}
C2_HOOK_FUNCTION(0x00486d60, MungeLightingEffects)

void C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage) {
    int i;
    for (i = 0; i < pStorage->materials_count; i++) {
        br_material *material = pStorage->materials[i];

        SmoothificateWorldMaterial(material);
    }
}

void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath) {

    LoadAllMaterialsInDirectory(pStorage, pPath, kRendererShadingType_Default);
    SmoothlySetWorldMaterialFlags(pStorage);
}
C2_HOOK_FUNCTION(0x004f6640, LoadTrackMaterials)

int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath) {
    br_model* temp_array[2000];
    int count;
    int new_ones;
    int i;

    new_ones = 0;
    count = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    WhitenVertexRGB(temp_array, count);
    if (count == 0) {
        FatalError(kFatalError_CannotLoadModelFileOrItIsEmpty_S, pPath);
    }
    for (i = 0; i < count; i++) {
        if (temp_array[i] == NULL) {
            continue;
        }
        switch (AddModelToStorage(pStorage, temp_array[i])) {
        case eStorage_not_enough_room:
            FatalError(kFatalError_InsufficientMaterialSlots);
            break;
        case eStorage_duplicate:
            BrModelFree(temp_array[i]);
            break;
        case eStorage_allocated:
            temp_array[i]->flags |= BR_MODF_UPDATEABLE;
            temp_array[i]->flags &= ~(BR_MODF_DONT_WELD | BR_MODF_CUSTOM_NORMALS);
            BrModelAdd(temp_array[i]);
            new_ones++;
            break;
        }
    }
    return new_ones;
}
C2_HOOK_FUNCTION(0x004f6580, AddSmoothModels)

void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath) {
    char s[256];

    Uppercaseificate(s, pPath);
    if (c2_strstr(s, ".DAT") != NULL) {
        AddSmoothModels(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x004f6540, SmoothlyLoadIfItsAModel)

void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage *pStorage, const char *pPath) {
    C2V(gStorageForCallbacks) = pStorage;
    PFForEveryFile(pPath, SmoothlyLoadIfItsAModel);
}
C2_HOOK_FUNCTION(0x004f6520, LoadTrackModels)

void (C2_HOOK_FASTCALL * LoadNonCar_original)(FILE* pF, tNon_car_spec* pNon_car_spec);
void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadNonCar_original(pF, pNon_car_spec);
#else
    int index_version;
    int extra_point_num;
    int i;
    br_scalar len, wid, het;
    br_scalar snap_angle;
    br_scalar ts;
    br_vector3 attached_cmpos;

    C2_HOOK_BUG_ON(sizeof(tNon_car_spec) != 0x104);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, driver, 0xc);
    pNon_car_spec->driver = eDriver_non_car_unused_slot;

    /* Carmageddon 1: number of non car */
    /* Carmageddon 2: version number */
    index_version = GetAnInt(pF);
    if (index_version < 100) {
        br_vector3 attached_cmpos;
        br_bounds3 bounds;

        pNon_car_spec->index = index_version;

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        /* min x, min y, min z */
        GetThreeFloats(pF, &bounds.min.v[0], &bounds.min.v[1], &bounds.min.v[2]);
        /* max x, max y, max z */
        GetThreeFloats(pF, &bounds.max.v[0], &bounds.max.v[1], &bounds.max.v[2]);

        /* number of extra points */
        extra_point_num = GetAnInt(pF);
        if (extra_point_num > 6) {
            FatalError(kFatalError_TooManyExtraPointsForCarIndex_S, index_version);
        }
        for (i = 0; i < extra_point_num; i++) {
            br_vector3 tmp;

            GetThreeFloats(pF, &tmp.v[0], &tmp.v[1], &tmp.v[2]);
        }

        /* mass in tonnes */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am length, width, height */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = REC2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        C2_HOOK_BUG_ON(sizeof(tCollision_shape_polyhedron) != 0x50);
        pNon_car_spec->collision_info->shape = BrMemAllocate(sizeof(tCollision_shape_polyhedron), kMem_collision_shape);
        c2_memcpy(&pNon_car_spec->collision_info->shape->polyhedron.common.bb, &bounds, sizeof(br_bounds3));
        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;
    } else {
        /* non car number */
        pNon_car_spec->index = (int)GetAScalar(pF);

        /* centre of mass position */
        GetThreeFloats(pF,
            &pNon_car_spec->collision_info->cmpos.v[0],
            &pNon_car_spec->collision_info->cmpos.v[1],
            &pNon_car_spec->collision_info->cmpos.v[2]);

        /* centre of mass position when attached */
        GetThreeFloats(pF,
            &attached_cmpos.v[0],
            &attached_cmpos.v[1],
            &attached_cmpos.v[2]);

        ReadMechanicsShapes(&pNon_car_spec->collision_info->shape, pF);

        /* mass unattached, mass attached */
        GetPairOfFloats(pF, &pNon_car_spec->free_mass, &pNon_car_spec->attached_mass);

        /* am width height and length */
        GetThreeFloats(pF, &len, &wid, &het);

        /* bend angle before snapping */
        snap_angle = GetAFloat(pF);
        pNon_car_spec->snap_off_cosine = cosf(BrAngleToRadian(BrDegreeToAngle(snap_angle)));
        pNon_car_spec->break_off_radians_squared = snap_angle * 3.14f / 180.f * (snap_angle * 3.14f / 180.f);

        /* torque (KN m) needed to move object */
        ts = GetAFloat(pF);
        pNon_car_spec->min_torque_squared = REC2_SQR(ts / WORLD_SCALE);

        BrVector3Set(&pNon_car_spec->I_over_M,
            (het * het + wid * wid) / 12.f,
            (het * het + len * len) / 12.f,
            (wid * wid + len * len) / 12.f);

        if (snap_angle != 0.f) {
            pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
            pNon_car_spec->collision_info->physics_joint2->type = eJoint_ball_n_socket;
            BrVector3Copy(&pNon_car_spec->collision_info->physics_joint2->field_0x08, &attached_cmpos);
            pNon_car_spec->collision_info->physics_joint2->count_limits = 1;
            pNon_car_spec->collision_info->physics_joint2->limits[0].type = eJoint_limit_10;
            pNon_car_spec->collision_info->physics_joint2->limits[0].value = pNon_car_spec->min_torque_squared;
        }
        UpdateCollisionObject(pNon_car_spec->collision_info);
        pNon_car_spec->collision_info->actor = pNon_car_spec->actor;
        pNon_car_spec->collision_info->bb2 = pNon_car_spec->collision_info->bb1;

        /* Materials for shrapnel */
        pNon_car_spec->count_shrapnel_materials = GetAnInt(pF);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, count_shrapnel_materials, 0x6c);

        for (i = 0; i < pNon_car_spec->count_shrapnel_materials; i++) {
            char s[256];

            C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, shrapnel_materials, 0x84);

            GetALineAndDontArgue(pF, s);
            pNon_car_spec->shrapnel_materials[i] = GetSimpleMaterial(s, (pNon_car_spec != NULL && pNon_car_spec->driver >= 6) ? kRendererShadingType_Specular : kRendererShadingType_Diffuse1);
        }
        pNon_car_spec->collision_info->object_friction = 0.4f;
        if (index_version > 100) {
            for (;;) {
                char s[256];

                GetAString(pF, s);
                if (DRStricmp(s, "END") == 0) {
                    break;
                } else if (DRStricmp(s, "WORLD_FRICTION") == 0) {
                    pNon_car_spec->collision_info->world_friction = GetAScalar(pF) - 1.f;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, world_friction, 0x60);
                } else if (DRStricmp(s, "OBJECT_FRICTION") == 0) {
                    pNon_car_spec->collision_info->object_friction = GetAScalar(pF) - 1.f;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, object_friction, 0x64);
                } else if (DRStricmp(s, "TUMBLE") == 0) {
                    pNon_car_spec->tumble_factor = GetAScalar(pF);
                    pNon_car_spec->tumble_threshold = GetAScalar(pF);
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, tumble_factor, 0xdc);
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, tumble_threshold, 0xe0);
                } else if (DRStricmp(s, "WORLD_BALL_JOINT") == 0) {
                    br_vector3 tv;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv.v[0], &tv.v[1], &tv.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_joint, friction, 0x4);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_HINGE_JOINT") == 0) {
                    br_vector3 tv1;
                    br_vector3 tv2;
                    br_scalar friction;
                    int count_limits;

                    GetThreeFloats(pF, &tv1.v[0], &tv1.v[1], &tv1.v[2]);
                    GetThreeFloats(pF, &tv2.v[0], &tv2.v[1], &tv2.v[2]);

                    /* friction */
                    friction = GetAScalar(pF);

                    /* num limits */
                    count_limits = GetAnInt(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(count_limits, kMem_physics_joint);
                    pNon_car_spec->field_0xf0->friction = friction;
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &tv1);
                    pNon_car_spec->field_0xf0->type = eJoint_ball_n_socket;
                } else if (DRStricmp(s, "WORLD_TRANSLATION_JOINT") == 0) {
                    br_vector3 pos;
                    br_vector3 dir;
                    br_scalar limit;
                    /* joint position */
                    GetThreeFloats(pF, &pos.v[0], &pos.v[1], &pos.v[2]);

                    /* axis (direction of movement) */
                    GetThreeFloats(pF, &dir.v[0], &dir.v[1], &dir.v[2]);

                    /* lift height */
                    limit = GetAScalar(pF);

                    pNon_car_spec->field_0xf0 = pNon_car_spec->collision_info->physics_joint2 = AllocatePhysicsJoint(1, kMem_physics_joint);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->field_0x08, &pos);
                    BrVector3Copy(&pNon_car_spec->field_0xf0->hinge_axis, &dir);

                    if (fabsf(dir.v[0]) <= fabsf(dir.v[1]) && fabsf(dir.v[0]) <= fabsf(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, 0.f, dir.v[2], -dir.v[1]);
                    } else if (fabsf(dir.v[1]) <= fabsf(dir.v[2])) {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, -dir.v[2], 0.f, -dir.v[0]);
                    } else {
                        BrVector3Set(&pNon_car_spec->field_0xf0->hinge_axis2, dir.v[1], -dir.v[0], 0.f);
                    }
                    BrVector3Normalise(&pNon_car_spec->field_0xf0->hinge_axis2, &pNon_car_spec->field_0xf0->hinge_axis2);
                    BrVector3Cross(&pNon_car_spec->field_0xf0->hinge_axis3, &pNon_car_spec->field_0xf0->hinge_axis, &pNon_car_spec->field_0xf0->hinge_axis2);
                    pNon_car_spec->field_0xf0->type = eJoint_translation;
                    pNon_car_spec->flags |= 0x10000;
                    pNon_car_spec->field_0xf0->count_limits = 1;
                    pNon_car_spec->field_0xf0->limits[0].type = eJoint_limit_11;
                    pNon_car_spec->field_0xf0->limits[0].value = limit;

                    C2_HOOK_BUG_ON(sizeof(tJoint_translation_params) != 0x14);
                    pNon_car_spec->translation_parameters = BrMemAllocate(sizeof(tJoint_translation_params), kMem_non_car_spec);

                    /* forward accel in g */
                    pNon_car_spec->translation_parameters->forward_acceleration = GetAScalar(pF);

                    /* reverse accel in g (nb grvity will pull it down) */
                    pNon_car_spec->translation_parameters->reverse_acceleration = GetAScalar(pF);

                    /* resistance going up */
                    pNon_car_spec->translation_parameters->forward_resistance = GetAScalar(pF);

                    /* resistance going down */
                    pNon_car_spec->translation_parameters->reverse_resistance = GetAScalar(pF);

                    /* pause at the top */
                    pNon_car_spec->translation_parameters->pause_at_top = GetAnInt(pF);

                    pNon_car_spec->translation_parameters->forward_resistance = exp2f(pNon_car_spec->translation_parameters->forward_resistance);
                    pNon_car_spec->translation_parameters->reverse_resistance = exp2f(pNon_car_spec->translation_parameters->reverse_resistance);
                } else if (DRStricmp(s, "RISE_WHEN_HIT") == 0) {
                    pNon_car_spec->flags |= 0x20000;
                } else if (DRStricmp(s, "RISE_WHEN_DRIVEN_ON") == 0) {
                    pNon_car_spec->flags |= 0x40000;
                } else if (DRStricmp(s, "NUMBER_OF_PUSHES") == 0) {
                    pNon_car_spec->number_of_pushes = GetAnInt(pF);
                } else if (DRStricmp(s, "DRIVABLE_ON") == 0) {
                    pNon_car_spec->collision_info->drivable_on = 1;
                } else if (DRStricmp(s, "INFINITE_I") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x800;
                } else if (DRStricmp(s, "IGNORE_WORLD_COLLISIONS") == 0) {
                    pNon_car_spec->collision_info->flags |= 0x80;
                } else if (DRStricmp(s, "BLOCK_CAMERA") == 0) {
                    pNon_car_spec->flags |= 0x80000;
                } else if (DRStricmp(s, "SPARKY") == 0) {
                    pNon_car_spec->flags |= 0x200000;
                } else {
                    FatalError(kFatalError_UnknownNonCarCommand);
                }
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00487ec0, ReadNonCarMechanicsData, LoadNonCar_original)

int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data) {
    tLinkyCallback_context* context = data;
    int i;

    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    pActor->render_style = BR_RSTYLE_DEFAULT;
    for (i = context->model_start; i < context->model_start + context->model_end; i++) {

        if (DRStricmp(C2V(gDroneStorage).models[i]->identifier, pActor->model->identifier) == 0) {
            pActor->model = C2V(gDroneStorage).models[i];
            return 0;
        }
    }
    PDFatalError("No drone model");
}
C2_HOOK_FUNCTION(0x00450600, LinkyCallback)

void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle(tDrone_spec* pDrone, int pModel_start, int pModel_end) {
    tLinkyCallback_context data;

    data.drone = pDrone;
    data.model_start = pModel_start;
    data.model_end = pModel_end;
    DRActorEnumRecurse(pDrone->model_actor, LinkyCallback, &data);
}

void (C2_HOOK_FASTCALL * LoadDroneActorsModels_original)(tDrone_spec* pDrone);
void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadDroneActorsModels_original(pDrone);
#else
    br_matrix34 mat;
    tPath_name dir_path;
    tPath_name actor_path;
    char s[256];
    int i;

    PossibleService();
    BrMatrix34Identity(&mat);
    if (pDrone->model_actor != NULL) {
        if (pDrone->model_actor->parent != NULL) {
            BrActorRemove(pDrone->model_actor);
        }
        BrActorFree(pDrone->model_actor);
        pDrone->model_actor = NULL;
    }
    if (pDrone->actor != NULL) {
        BrMatrix34Copy(&mat, &pDrone->actor->t.t.mat);
        if (pDrone->actor->parent != NULL) {
            BrActorRemove(pDrone->actor);
        }
        BrActorFree(pDrone->actor);
        pDrone->actor = NULL;
    }
    c2_strcpy(C2V(gCurrent_load_directory), "DRONES");
    c2_strcpy(C2V(gCurrent_load_name), pDrone->form->name);
    c2_strcpy(dir_path, C2V(gApplication_path));
    PathCat(dir_path, dir_path, C2V(gCurrent_load_directory));
    PathCat(dir_path, dir_path, C2V(gCurrent_load_name));

    if (pDrone->form->model_start < 0) {
        int material_start;

        PrintMemoryDump(0,"BEFORE LOADING DRONE ACTORS/MODELS");
        LoadAllImagesInDirectory(&C2V(gDroneStorage), dir_path);
        LoadAllShadeTablesInDirectory(&C2V(gDroneStorage), dir_path);
        material_start = C2V(gDroneStorage).materials_count;
        LoadAllMaterialsInDirectory(&C2V(gDroneStorage), dir_path, kRendererShadingType_Specular);
        for (i = material_start; i < C2V(gDroneStorage).materials_count; i++) {
            br_material* material;

            material = C2V(gDroneStorage).materials[i];
            if (material == NULL || material->colour_map == NULL) {
                continue;
            }
            material->flags |= BR_MATF_LIGHT | BR_MATF_PRELIT;
            material->flags |= BR_MATF_SMOOTH;
            BrMaterialUpdate(material, BR_MATU_ALL);
            PossibleService();
        }
        pDrone->form->model_start = C2V(gDroneStorage).models_count;
        LoadAllModelsInDirectory(&C2V(gDroneStorage), dir_path);
        pDrone->form->model_end = C2V(gDroneStorage).models_count - pDrone->form->model_start;
        if (pDrone->form->crushability != 0.f) {
            pDrone->form->model_index = -1;

            for (i = pDrone->form->model_start; i < pDrone->form->model_start + pDrone->form->model_end; i++) {
                if (DRStricmp(C2V(gDroneStorage).models[i]->identifier, pDrone->form->name) == 0) {
                    pDrone->form->model_index = i;
                    break;
                }
            }
            if (pDrone->form->model_index < 0) {
                c2_sprintf(s, "Can't find model called '%s' for drone '%s'",
                    pDrone->form->name, pDrone->form->name);
                PDFatalError(s);
            }
        }
        PrintMemoryDump(0, "AFTER LOADING DRONE ACTORS/MODELS");
    }
    pDrone->actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    BrMatrix34Copy(&pDrone->actor->t.t.mat, &mat);
    c2_sprintf(s, "Drone%d", pDrone->id);
    pDrone->actor->identifier = BrResStrDup(pDrone->actor, s);
    BrActorAdd(C2V(gNon_track_actor), pDrone->actor);
    PathCat(actor_path, dir_path, pDrone->form->name);
    c2_strcat(actor_path,".ACT");
    pDrone->model_actor = BrActorLoad(actor_path);
    if (pDrone->model_actor == NULL) {
        c2_sprintf(s, "Can't load drone car actor file for %s", pDrone->form->name);
        PDFatalError(s);
    }
    LinkDroneActorsToModelsAndSetRenderStyle(pDrone, pDrone->form->model_start, pDrone->form->model_end);
    pDrone->actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(pDrone->actor, pDrone->model_actor);
    if (!(pDrone->form->flags & 0x20)) {
        br_model* model = pDrone->model_actor->model;

        pDrone->form->field_0x54.min.v[0] = model->bounds.min.v[0] + pDrone->form->crush_limits_left    * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.min.v[1] = model->bounds.min.v[1] + pDrone->form->crush_limits_bottom  * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.min.v[2] = model->bounds.min.v[2] + pDrone->form->crush_limits_back    * 0.01f * (model->bounds.max.v[2] - model->bounds.min.v[2]);
        pDrone->form->field_0x54.max.v[0] = model->bounds.max.v[0] - pDrone->form->crush_limits_right   * 0.01f * (model->bounds.max.v[0] - model->bounds.min.v[0]);
        pDrone->form->field_0x54.max.v[1] = model->bounds.max.v[1] - pDrone->form->crush_limits_top     * 0.01f * (model->bounds.max.v[1] - model->bounds.min.v[1]);
        pDrone->form->field_0x54.max.v[2] = model->bounds.max.v[2] - pDrone->form->crush_limits_front   * 0.01f * (model->bounds.max.v[2] - model->bounds.min.v[2]);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00450150, LoadDroneActorsModels, LoadDroneActorsModels_original)

intptr_t C2_HOOK_CDECL MrFindy(br_actor* pActor, void* data) {
    const char* name = data;

    if (DRStricmp(pActor->identifier, name) == 0) {
        return (intptr_t)pActor;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00450b30, MrFindy)

br_actor* C2_HOOK_FASTCALL FindDroneChildActor(tDrone_spec* pDrone, const char* pName) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, model_actor, 0xf0);
    return (br_actor*)DRActorEnumRecurse(pDrone->model_actor, MrFindy, (void*)pName);
}

int C2_HOOK_FASTCALL ReadPastThisLine(FILE* pF, const char* pLine) {
    char s[256];

    for (;;) {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
        if (c2_strcmp(pLine, s) == 0) {
            return 1;
        }
    }
}

int C2_HOOK_FASTCALL MatchFGType(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gGroove_funk_type_names)) != 3);

    for (i = 0; i < REC2_ASIZE(C2V(gGroove_funk_type_names)); i++) {

        if (c2_strcmp(pS, C2V(gGroove_funk_type_names)[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_axis GetAxisFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gAxis_names)) != 3);

    for (i = 0; i < REC2_ASIZE(C2V(gAxis_names)); i++) {

        if (c2_strcmp(pS, C2V(gAxis_names)[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_reverseness GetReversenessFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gReverseness_type_names)) != 2);

    for (i = 0; i < REC2_ASIZE(C2V(gReverseness_type_names)); i++) {

        if (c2_strcmp(pS, C2V(gReverseness_type_names)[i]) == 0) {
            return i;
        }
    }
    return -1;
}

tFunk_groove_speed_control GetSpeedControlFromString(const char* pS) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gFunk_speed_control_names)) != 2);

    for (i = 0; i < REC2_ASIZE(C2V(gFunk_speed_control_names)); i++) {

        if (c2_strcmp(pS, C2V(gFunk_speed_control_names)[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void (C2_HOOK_FASTCALL * LoadFunksAndGrooves_original)(tDrone_spec* pDrone, FILE* pF);
void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadFunksAndGrooves_original(pDrone, pF);
#else

    C2_HOOK_BUG_ON(sizeof(tFunk_grooves) != 0xa4);
    C2_HOOK_BUG_ON(REC2_ASIZE(pDrone->funk_grooves->items) != 10);

    if (!ReadPastThisLine(pF, "START OF FUNKYGROOVY STUFF")) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
    }

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, funk_grooves, 0x5d0);

    if (pDrone->funk_grooves == NULL) {
        pDrone->funk_grooves = BrMemAllocate(sizeof(tFunk_grooves), kMem_drone_funk_groove);
    }
    for (;;) {
        tFunk_groove* funk_groove;
        char s[256];
        int axis;
        int reverseness;
        int speed_control;

        GetALineAndDontArgue(pF, s);
        if (c2_strcmp(s, "END OF FUNKYGROOVY STUFF") == 0 || PFfeof(pF)) {
            break;
        }
        if (pDrone->funk_grooves->count >= REC2_ASIZE(pDrone->funk_grooves->items)) {
            FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
        }
        funk_groove = &pDrone->funk_grooves->items[pDrone->funk_grooves->count];
        funk_groove->type = MatchFGType(s);
        switch (funk_groove->type) {
        case eFunk_groove_type_spinny_groove:
            /* wheel actor */
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = FindDroneChildActor(pDrone, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }

            /* "x", "y" or "z" */
            GetALineAndDontArgue(pF, s);
            axis = GetAxisFromString(s);
            if (axis < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.axis = axis;

            /* "controlled" or "dronespeed" */
            GetALineAndDontArgue(pF, s);
            speed_control = GetSpeedControlFromString(s);
            if (speed_control < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.speed_control = speed_control;

            /* "forward" or "reverse" */
            GetALineAndDontArgue(pF,s);
            reverseness = GetReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->spinny.reverse = reverseness;

            funk_groove->spinny.omega = REC2_PI_F * GetAScalar(pF);
            break;

        case eFunk_groove_type_steering_groove:
            /* pivot actor */
            GetALineAndDontArgue(pF, s);
            funk_groove->actor = FindDroneChildActor(pDrone, s);
            if (funk_groove->actor == NULL) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }

            /* "forward" or "reverse" */
            GetALineAndDontArgue(pF, s);
            reverseness = GetReversenessFromString(s);
            if (reverseness < 0) {
                FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, pDrone->form->name);
            }
            funk_groove->steering.reverse = reverseness;
            break;
        }
        pDrone->funk_grooves->count += 1;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00450680, LoadFunksAndGrooves, LoadFunksAndGrooves_original)

int C2_HOOK_FASTCALL ReadPastBoundsShapesHeader(FILE* pF) {
    char s[256];

    do {
        if (PFfeof(pF)) {
            return 0;
        }
        GetALineAndDontArgue(pF, s);
    } while (c2_strcmp(s, "START OF BOUNDING SHAPES") != 0);
    DoNotDprintf("ReadPastBoundsShapesHeader() returning TRUE");
    return 1;
}

void C2_HOOK_FASTCALL LoadDrone(int pIndex) {
    tDrone_spec* drone;
    tPath_name path;
    tTWTVFS twt;
    FILE* f;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, collision_info.shape, 0x118);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_spec, field_0x46, 0x46);

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 0x5d8);

    drone = &C2V(gDrone_specs)[pIndex];
    c2_strcpy(path, C2V(gApplication_path));
    PathCat(path, path, "DRONES");
    PathCat(path, path, drone->form->name);
    twt = OpenPackFileAndSetTiffLoading(path);
    LoadDroneActorsModels(drone);
    f = OpenDroneFile(drone->form->name);
    if (!ReadPastBoundsShapesHeader(f)) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, drone->form->name);
    } else {
        DoNotDprintf("DRONE.C: Reading mechanics data for drone of type %s", drone->form->name);
        ReadMechanicsShapes(&drone->collision_info.shape, f);
        if (drone->collision_info.shape != NULL) {
            drone->field_0x46 = 1;
        }
    }
    LoadFunksAndGrooves(drone, f);
    PFfclose(f);
    ClosePackFileAndSetTiffLoading(twt);
}

void C2_HOOK_FASTCALL ResetDroneCrushyModel(const br_model* pSrc, br_model* pDest) {

    pDest->faces = pSrc->faces;
    pDest->nfaces = pSrc->nfaces;
    c2_memcpy(pDest->vertices, pSrc->vertices, pDest->nvertices * sizeof(br_vertex));
}

void (C2_HOOK_FASTCALL * LoadPerRaceDroneStuff_original)(void);
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadPerRaceDroneStuff_original();
#else
    int i;

    for (i = 0; i < C2V(gCount_drone_forms); i++) {
        tDrone_form* form = &C2V(gDrone_forms)[i];

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, field_0x6c, 0x6c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_start, 0x70);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_end, 0x74);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, model_index, 0x78);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, count_models, 0x7c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, field_0x80, 0x80);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tDrone_form, models, 0x84);

        form->field_0x6c = 0;
        form->model_start = -1;
        form->model_end = -1;
        form->model_index = -1;
        form->count_models = 0;
        form->field_0x80 = NULL;
        form->models = NULL;
    }

    for (i = 0; i < C2V(gCount_drones); i++) {

        C2V(gDrone_specs)[i].field_0x70 += 1;
        LoadDrone(i);
    }

    for (i = 0; i < C2V(gCount_drone_forms); i++) {
        tDrone_form* form;
        int count_models;

        form = &C2V(gDrone_forms)[i];
        if (form->field_0x6c == 0) {
            continue;
        }
        count_models = form->crushability == 0.f ? 0 : MIN(10, form->field_0x6c);
        form->count_models = count_models;
        if (count_models != 0) {
            int j;

            form->models = BrMemCalloc(sizeof(br_model*), count_models, kMem_drone_model_dup);
            form->field_0x80 = BrMemCalloc(count_models, sizeof(tU8), kMem_drone_model_dup);

            for (j = 0; j < count_models; j++) {
                br_model* orig_model;

                orig_model = C2V(gDroneStorage).models[form->model_index];
                form->models[j] = BrModelAllocate(orig_model->identifier, orig_model->nvertices, 0);
                form->models[j]->flags |= BR_MODF_UPDATEABLE;
                ResetDroneCrushyModel(orig_model, form->models[j]);
                BrModelAdd(form->models[j]);
            }

        }

    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044fda0, LoadPerRaceDroneStuff, LoadPerRaceDroneStuff_original)

void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadOpponentsCars(pRace_info);
    LoadCopCars();
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}
C2_HOOK_FUNCTION(0x00401070, LoadAIWorldForRace)

void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race) {

}
C2_HOOK_FUNCTION(0x00487ea0, LoadInterfaceStuff)

void C2_HOOK_FASTCALL UnlockInterfaceStuff(void) {

}
C2_HOOK_FUNCTION(0x0044bb80, UnlockInterfaceStuff)

void C2_HOOK_FASTCALL InitInterfaceLoadState(void) {

}
C2_HOOK_FUNCTION(0x00487eb0, InitInterfaceLoadState)

void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info) {
    br_bounds3 track_bounds;

    C2V(gFirst_drone_processing) = 1;
    C2V(gTime_stamp_for_this_munging) = GetTotalTime();
    if (C2V(gTime_stamp_for_this_munging) == 0) {
        C2V(gTime_stamp_for_this_munging) = PDGetTotalTime();
    }
    C2V(gFrame_period_for_this_munging) = 100;
    C2V(gDrone_delta_time) = 0.1f;
    BrActorToBounds(&track_bounds, C2V(gTrack_actor));
    if (track_bounds.max.v[1] <= track_bounds.min.v[1]) {
        C2V(gTrack_drone_min_y) = track_bounds.max.v[1];
    } else {
        C2V(gTrack_drone_min_y) = track_bounds.min.v[1];
    }
    PrintMemoryDump(0, "BEFORE InitOpponents()");
    InitOpponents(pRace_info);
    PrintMemoryDump(0, "AFTER InitOpponents(), BEFORE InitDrones()");
    InitDrones();
    PrintMemoryDump(0, "AFTER InitDrones()");
}
C2_HOOK_FUNCTION(0x004010b0, InitialiseAIWorldForRace)

void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void) {
    DisposeOpponents();
    DisposeDronesRaceStuff();
}
C2_HOOK_FUNCTION(0x00401160, DisposeAIWorldRaceStuff)

void C2_HOOK_FASTCALL DisposeRaceInfo(tRace_info* pRace_info) {

}
C2_HOOK_FUNCTION(0x0044bf70, DisposeRaceInfo)

void (C2_HOOK_FASTCALL * DisposeAllCars_original)(tRace_info* pRace_info);
void C2_HOOK_FASTCALL DisposeOpponentsCars(tRace_info* pRace_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    DisposeAllCars_original(pRace_info);
#else

    int i;

    C2V(gCurrent_APO_potential_levels)[0] = C2V(gProgram_state).current_car.power_up_slots[0];
    C2V(gCurrent_APO_potential_levels)[1] = C2V(gProgram_state).current_car.power_up_slots[1];
    C2V(gCurrent_APO_potential_levels)[2] = C2V(gProgram_state).current_car.power_up_slots[2];
    C2V(gCurrent_APO_levels)[0] = C2V(gProgram_state).current_car.power_up_levels[0];
    C2V(gCurrent_APO_levels)[1] = C2V(gProgram_state).current_car.power_up_levels[1];
    C2V(gCurrent_APO_levels)[2] = C2V(gProgram_state).current_car.power_up_levels[2];
    DisposeCar(&C2V(gProgram_state).current_car, C2V(gProgram_state).current_car.index);
    for (i = 0; i < pRace_info->number_of_racers; i++) {
        PossibleService();
        if (pRace_info->opponent_list[i].index >= 0) {
            if (pRace_info->opponent_list[i].car_spec != NULL) {
                DisposeCar(pRace_info->opponent_list[i].car_spec, pRace_info->opponent_list[i].index);
                BrMemFree(pRace_info->opponent_list[i].car_spec);
            }
        }
    }
    ClearOutStorageSpace(&C2V(gOur_car_storage_space));
    ClearOutStorageSpace(&C2V(gTheir_cars_storage_space));
    C2V(gCurrent_race).number_of_racers = 0;
    C2V(gProgram_state).AI_vehicles.number_of_opponents = 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044bfa0, DisposeOpponentsCars, DisposeAllCars_original)

void C2_HOOK_FASTCALL GetPairOfFloatPercents(FILE* pF, float* pF1, float* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
    *pF1 = *pF1 / 100.0f;
    *pF2 = *pF2 / 100.0f;
}
C2_HOOK_FUNCTION(0x004904e0, GetPairOfFloatPercents)

void C2_HOOK_FASTCALL GetThreeFloatPercents(FILE* pF, float* pF1, float* pF2, float* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF3);
    *pF1 = *pF1 / 100.0f;
    *pF2 = *pF2 / 100.0f;
    *pF3 = *pF3 / 100.0f;
}
C2_HOOK_FUNCTION(0x00490570, GetThreeFloatPercents)
