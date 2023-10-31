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

C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_hither, 0x0074d3e8);
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gFine_jump_start, 3, 0x0074d3d0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_per_second_time_bonus, 3, 0x00761ef0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCunning_stunt_bonus, 3, 0x00763500);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gDefaultCar, 32, 0x00764ea0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gBasic_car_name, 32, 0x00764ec0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gGoreLevel, 0x0065d7d4, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAnimalsOn, 0x0065d7c8, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFlameThrowerOn, 0x0065d7cc, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMinesOn, 0x0065d7d0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDronesOff, 0x00684518);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKnobbledFramePeriod, 0x007634f0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gUnknownOpponentFactor, 0x0065a3cc);
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
    0x9B, 0x52, 0x93, 0x9f, 0x52, 0x98, 0x9b,                                    \
    0x96, 0x96, 0x9e, 0x9B, 0xa0, 0x99, 0x0 });
C2_HOOK_VARIABLE_IMPLEMENT(int, gDecode_thing, 0x00655e30);
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume, gDefault_water_spec_vol, 0x00761b80); /* FIXME: rename to gDefault_default_water_spec_vol */
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume*, gDefault_water_spec_vol_real, 0x00761f44);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gMisc_strings, 300, 0x006b5f40);

C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedsFolder, 0x006a0414);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedSoundPath, 0x00684550);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPowerup_txt_path, 0x006a0ad4);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTextTxtPath, 0x0068c718);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTexturePath, 0x0065852c);

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
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gMax_armour, 0x0074cfa0);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gMax_power, 0x0074cfcc);
C2_HOOK_VARIABLE_IMPLEMENT(tSlot_info, gMax_offensive, 0x0074cff8);
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gRepair_cost, 3, 0x00761d00);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gNet_repair_cost, 8, 0x00761d0c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gRecovery_cost, 3, 0x007634c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gNet_recovery_cost, 8, 0x007634cc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCar_softness, 3, 0x0075ba20);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gNet_car_softness, 8, 0x0075ba2c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCar_car_damage_multiplier, 3, 0x0074d600);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gNet_car_car_damage_multiplier, 8, 0x0074d60c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gNet_score_targets, 8, 0x007638c0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPickup_respawn_min_time_ms, 0x007447d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPickup_respawn_max_extra_time_ms, 0x007447e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDemo_race_rank_equivalent, 0x0074b58c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_demo_opponents, 0x0074b4fc);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDemo_opponents, 15, 0x0074b4c0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDefault_gravity, 0x0074d3bc);
/* FIXME: this might be a struct */
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_pre_smack, 0x0068b8b4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_post_smack, 0x0068b8b8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_not_in_demo, 0x0068b8bc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFlic_sound_delay_post_demo, 0x0068b8c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gAuto_increase_credits_dt, 3, 0x0074b740);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gNet_auto_increase_credits_dt, 8, 0x0074b720);
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

void C2_HOOK_FASTCALL ConfigureDefaultPedSoundPath(void) {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457570, ConfigureDefaultPedSoundPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedPowerupTxtPath(void) {
    C2V(gPowerup_txt_path) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c00, ConfigureDefaultPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea00, ConfigureDefaultPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedTexturePath(void) {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f7f0, ConfigureDefaultPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedDefaultPaths(void) {
    C2V(gPedsFolder) = "PEDS";
    ConfigureDefaultPedSoundPath();
    ConfigureDefaultPedPowerupTxtPath();
    ConfigureDefaultPedTextTxtPath();
    ConfigureDefaultPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f10, ConfigurePedDefaultPaths)

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

int C2_HOOK_FASTCALL GetGoreLevel(void) {

    return 2 - C2V(gGoreLevel);
}
C2_HOOK_FUNCTION(0x004d6fc0, GetGoreLevel)

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

int C2_HOOK_FASTCALL GetFlameThrowerOn(void) {

    return C2V(gFlameThrowerOn);
}
C2_HOOK_FUNCTION(0x004d6fe0, GetFlameThrowerOn)

void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    C2V(gFlameThrowerOn) = pNewFlameThrowerOn;
}
C2_HOOK_FUNCTION(0x004d7010, SetFlameThrowerOn)

int C2_HOOK_FASTCALL GetMinesOn(void) {

    return C2V(gMinesOn);
}
C2_HOOK_FUNCTION(0x004d6ff0, GetMinesOn)

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

    DRfread(&raw_long, sizeof(raw_long), 1, pF);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048f830, ReadU32)

tU16 C2_HOOK_FASTCALL ReadU16(FILE* pF) {
    tU16 raw_short;

    DRfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048f850, ReadU16)

tU8 C2_HOOK_FASTCALL ReadU8(FILE* pF) {
    tU8 raw_byte;

    DRfread(&raw_byte, sizeof(raw_byte), 1, pF);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048f870, ReadU8)

float C2_HOOK_FASTCALL ReadF32(FILE* pF) {
    float f;

    DRfread(&f, sizeof(f), 1, pF);
    return f;
}
C2_HOOK_FUNCTION(0x0048f890, ReadF32)

tS32 C2_HOOK_FASTCALL ReadS32(FILE* pF) {
    tS32 raw_long;

    DRfread(&raw_long, sizeof(raw_long), 1, pF);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048f8b0, ReadS32)

tS16 C2_HOOK_FASTCALL ReadS16(FILE* pF) {
    tS16 raw_short;

    DRfread(&raw_short, sizeof(raw_short), 1, pF);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048f8d0, ReadS16)

tS8 C2_HOOK_FASTCALL ReadS8(FILE* pF) {
    tS8 raw_byte;

    DRfread(&raw_byte, sizeof(raw_byte), 1, pF);
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

    DRfseek(pF, pBytes_to_skip, SEEK_CUR);
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
#if defined(C2_HOOKS_ENABLED)
    return GetAFloat_original(pF);
#else
#error "not implemented"
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
#if defined(C2_HOOKS_ENABLED)
    GetAString_original(pF, pString);
#else
#error "not implemented"
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

    fp = TWT_fopen(pFilename, pMode);
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
            fp = TWT_fopen(paths_file, "rt");
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
            DRfclose(fp);
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
            return TWT_fopen(source_check, pMode);
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
        DRfclose(fp);
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

void (C2_HOOK_FASTCALL * DRfclose_original)(FILE* pFile);
void C2_HOOK_FASTCALL DRfclose(FILE* pFile) {
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
C2_HOOK_FUNCTION_ORIGINAL(0x004b4760, DRfclose, DRfclose_original)

br_size_t (C2_HOOK_FASTCALL * DRfread_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f) {
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
C2_HOOK_FUNCTION_ORIGINAL(0x004b49f0, DRfread, DRfread_original)

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

int (C2_HOOK_FASTCALL * DRfeof_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfeof(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfeof_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(uintptr_t)pFile];
        return twtFile->pos >= twtFile->end;
    }
    return c2_feof(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c10, DRfeof, DRfeof_original)

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

char* (C2_HOOK_FASTCALL * DRfgets_original)(char* buffer, br_size_t size, FILE* pFile);
char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, FILE* pFile) {
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
C2_HOOK_FUNCTION_ORIGINAL(0x004b4900, DRfgets, DRfgets_original)

int (C2_HOOK_FASTCALL * DRfseek_original)(FILE* pF, int offset, int whence);
int C2_HOOK_FASTCALL DRfseek(FILE* pF, int offset, int whence) {

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
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b70, DRfseek, DRfseek_original)

int (C2_HOOK_FASTCALL * DRftell_original)(FILE* pF);
int C2_HOOK_FASTCALL DRftell(FILE* pF) {

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
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b00, DRftell, DRftell_original)

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
void C2_HOOK_FASTCALL DRrewind(FILE* pF) {

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
C2_HOOK_FUNCTION_ORIGINAL(0x004b4be0, DRrewind, DRrewind_original)

void C2_HOOK_FASTCALL TWT_Init(void) {
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
C2_HOOK_FUNCTION(0x004b4570, TWT_Init)

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path) {
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
    // TWT_verify_header(C2V(gTwatVfsMountPoints)[i].header, size); // not implemented
    c2_fclose(f);

    tU8* fileData = (tU8*)&C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[C2V(gTwatVfsMountPoints)[twt].header->nbFiles];
    for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
        C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data = fileData;
        fileData += (C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
    }
    return twt;
}
C2_HOOK_FUNCTION(0x004b45b0, TWT_Mount)

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt) {
    if (twt >= 0) {
        BrMemFree(C2V(gTwatVfsMountPoints)[twt].header);
        C2V(gTwatVfsMountPoints)[twt].header = NULL;
    }
}
C2_HOOK_FUNCTION(0x004b4730, TWT_Unmount)

FILE* C2_HOOK_FASTCALL TWT_fopen(const char* pPath, const char* mode) {
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
C2_HOOK_FUNCTION(0x004b4780, TWT_fopen)

void C2_HOOK_FASTCALL DRForEveryArchivedFile(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine) {
    tPath_name the_path;
    tTWTVFS twt;

    PathCat(the_path, pThe_path, pArchive_name);
    twt = TWT_MountEx(the_path);
    DRForEveryFile(the_path, pAction_routine);
    TWT_UnmountEx(twt);
}
C2_HOOK_FUNCTION(0x0048f360, DRForEveryArchivedFile)

void C2_HOOK_FASTCALL DRForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
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
C2_HOOK_FUNCTION(0x004b4c80, DRForEveryFile)

void C2_HOOK_FASTCALL DREnumPath(const char* path, tEnumPathCallback pCallback, void* data) {
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
C2_HOOK_FUNCTION(0x004b4d30, DREnumPath)

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path) {
    tTWTVFS res;

    res = TWT_Mount(path);
    if (TWT_MOUNT_SUCCEEDED(res)) {
        C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversionStackPos)++;
        C2V(gDisableTiffConversion) = 1;
    }
    return res;
}
C2_HOOK_FUNCTION(0x004b4df0, TWT_MountEx)

void C2_HOOK_FASTCALL TWT_UnmountEx(tTWTVFS twt) {
    if (twt >= 0) {
        C2V(gDisableTiffConversionStackPos)--;
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
        TWT_Unmount(twt);
    }
}
C2_HOOK_FUNCTION(0x004b4e20, TWT_UnmountEx)

void C2_HOOK_FASTCALL ApplyPreviousTiffConversion(void) {
    int count;

    count = C2V(gDisableTiffConversionStackPos);
    if (count != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversionStack)[count] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[count - 1];
    }
}
C2_HOOK_FUNCTION(0x004b4e60, ApplyPreviousTiffConversion)

void C2_HOOK_FASTCALL ApplyTopTiffConversion(void) {

    if (C2V(gDisableTiffConversionStackPos) != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
    }
}
C2_HOOK_FUNCTION(0x004b4e90, ApplyTopTiffConversion)

br_pixelmap* (C2_HOOK_FASTCALL * DRLoadPixelmap_original)(const char* pPath_name);
br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name) {
#if defined(C2_HOOKS_ENABLED)
    return DRLoadPixelmap_original(pPath_name);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ec00, DRLoadPixelmap, DRLoadPixelmap_original)

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
    int position;
    int result;
    br_scalar armour_mult, power_mult, offensive_mult;
    int time;

    PathCat(the_path, C2V(gApplication_path), "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    C2V(gTempFile) = TWT_fopen(the_path, "rb");
    if (C2V(gTempFile) != NULL) {
        DRfgets(s, REC2_ASIZE(s)-1, C2V(gTempFile));
        DRfclose(C2V(gTempFile));

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
    /* Hithers, general then cockpit mode */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    result = c2_sscanf(&s2[c2_strspn(s2, "\t ,")], "%f%n", &C2V(gCamera_hither), &position);
    if (result == 0) {
        FatalError(kFatalError_MysteriousX_SS, s2, "GENERAL.TXT");
    }
    c2_sscanf(&s2[position + c2_strspn(&s2[position], "\t ,")], "%f", &C2V(gCamera_cockpit_hither));
    C2V(gCamera_hither) *= 2;
    C2V(gCamera_cockpit_hither) *= 2;
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

    LoadGeneralCrushSettings(C2V(gTempFile));

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
    GetThreeInts(C2V(gTempFile), &C2V(gFine_jump_start)[0], &C2V(gFine_jump_start)[1], &C2V(gFine_jump_start)[2]);
    /* Credits per second of time bonus */
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_per_second_time_bonus)[0], &C2V(gCredits_per_second_time_bonus)[1], &C2V(gCredits_per_second_time_bonus)[2]);
    /* Cunning stunt bonus for each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gCunning_stunt_bonus)[0], &C2V(gCunning_stunt_bonus)[1], &C2V(gCunning_stunt_bonus)[2]);

    /* Cars to use as defaults: */
    GetAString(C2V(gTempFile), C2V(gDefaultCar));
    GetAString(C2V(gTempFile), C2V(gBasic_car_name));

    C2V(gKnobbledFramePeriod) = 0;
    C2V(gUnknownOpponentFactor) = 1.f;
    /* Min time in secs after last contact with play before opponent considers repairing */
    C2V(gMinTimeOpponentRepair) = (int)GetAScalar(C2V(gTempFile));
    /* Max time in secs after last contact with play before opponent considers repairing */
    C2V(gMaxTimeOpponentRepair) = (int)GetAScalar(C2V(gTempFile));

    /* Default underwater special volume parameters */
    ParseSpecialVolume(C2V(gTempFile), &C2V(gDefault_water_spec_vol), C2V(gUnderwater_screen_name), 0);

    /* Initial armour, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO)[0].initial[0], &C2V(gInitial_APO)[0].initial[1], &C2V(gInitial_APO)[0].initial[2]);
    /* Initial armour, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO)[0].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO)[0].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Initial power, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO)[1].initial[0], &C2V(gInitial_APO)[1].initial[1], &C2V(gInitial_APO)[1].initial[2]);
    /* Initial power, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO)[1].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO)[1].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Initial offensive, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO)[2].initial[0], &C2V(gInitial_APO)[2].initial[1], &C2V(gInitial_APO)[2].initial[2]);
    /* Initial offensive, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO)[2].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO)[2].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Initial potential armour, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO_potential)[0].initial[0], &C2V(gInitial_APO_potential)[0].initial[1], &C2V(gInitial_APO_potential)[0].initial[2]);
    /* Initial potential armour, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO_potential)[0].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO_potential)[0].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Initial potential power, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO_potential)[1].initial[0], &C2V(gInitial_APO_potential)[1].initial[1], &C2V(gInitial_APO_potential)[1].initial[2]);
    /* Initial potential power, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO_potential)[1].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO_potential)[1].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Initial potential offensive, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gInitial_APO_potential)[2].initial[0], &C2V(gInitial_APO_potential)[2].initial[1], &C2V(gInitial_APO_potential)[2].initial[2]);
    /* Initial potential offensive, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gInitial_APO_potential)[2].initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gInitial_APO_potential)[2].initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Max armour, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gMax_armour).initial[0], &C2V(gMax_armour).initial[1], &C2V(gMax_armour).initial[2]);
    /* Max armour, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gMax_armour).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gMax_armour).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Max power, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gMax_power).initial[0], &C2V(gMax_power).initial[1], &C2V(gMax_power).initial[2]);
    /* Max power, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gMax_power).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gMax_power).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Max offensive, single player, each skill level */
    GetThreeInts(C2V(gTempFile), &C2V(gMax_offensive).initial[0], &C2V(gMax_offensive).initial[1], &C2V(gMax_offensive).initial[2]);
    /* Max offensive, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gMax_offensive).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gMax_offensive).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* APO cost, single player */
    GetThreeInts(C2V(gTempFile), &C2V(gCost_APO).initial[0], &C2V(gCost_APO).initial[1], &C2V(gCost_APO).initial[2]);
    /* APO cost, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gCost_APO).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gCost_APO).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* Trade-in APO value, single player */
    GetThreeInts(C2V(gTempFile), &C2V(gTrade_in_value_APO).initial[0], &C2V(gTrade_in_value_APO).initial[1], &C2V(gTrade_in_value_APO).initial[2]);
    /* Trade-in APO value, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gTrade_in_value_APO).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gTrade_in_value_APO).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* APO substitution value, single player */
    GetThreeInts(C2V(gTempFile), &C2V(gSubstitution_value_APO).initial[0], &C2V(gSubstitution_value_APO).initial[1], &C2V(gSubstitution_value_APO).initial[2]);
    /* APO substitution, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gSubstitution_value_APO).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gSubstitution_value_APO).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }
    /* APO potential substitution value, single player */
    GetThreeInts(C2V(gTempFile), &C2V(gPotential_substitution_value_APO).initial[0], &C2V(gPotential_substitution_value_APO).initial[1], &C2V(gPotential_substitution_value_APO).initial[2]);
    /* APO potential substitution, each network game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gPotential_substitution_value_APO).initial_network); i++) {
        c2_sscanf(str, "%d", &C2V(gPotential_substitution_value_APO).initial_network[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

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
    GetThreeFloats(C2V(gTempFile), &C2V(gRepair_cost)[0], &C2V(gRepair_cost)[1], &C2V(gRepair_cost)[2]);
    /* Repair cost for each net game (cred per % damage) */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_repair_cost)); i++) {
        c2_sscanf(str, "%f", &C2V(gNet_repair_cost)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Recovery cost for each skill level */
    GetThreeFloats(C2V(gTempFile), &C2V(gRecovery_cost)[0], &C2V(gRecovery_cost)[1], &C2V(gRecovery_cost)[2]);
    /* Recovery cost for each net game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_recovery_cost)); i++) {
        c2_sscanf(str, "%f", &C2V(gNet_recovery_cost)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Car softness factor for each net skill level */
    GetThreeFloats(C2V(gTempFile), &C2V(gCar_softness)[0], &C2V(gCar_softness)[1], &C2V(gCar_softness)[2]);
    /* Car softness factor for each net game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_car_softness)); i++) {
        c2_sscanf(str, "%f", &C2V(gNet_car_softness)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Car-to-car damage multiplier for each net skill level */
    GetThreeFloats(C2V(gTempFile), &C2V(gCar_car_damage_multiplier)[0], &C2V(gCar_car_damage_multiplier)[1], &C2V(gCar_car_damage_multiplier)[2]);
    /* Car-to-car damage multiplier for each net game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_car_car_damage_multiplier)); i++) {
        c2_sscanf(str, "%f", &C2V(gNet_car_car_damage_multiplier)[i]);
        str = c2_strtok(NULL, "\t ,/");
    }

    /* Score targets for each net game type */
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; i < REC2_ASIZE(C2V(gNet_score_targets)); i++) {
        c2_sscanf(str, "%f", &C2V(gNet_score_targets)[i]);
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
        C2V(gAuto_increase_credits_dt)[i] = (int)((float)(1000 * time) / (.02f * (float)C2V(gRecovery_cost[i])));
    }
    GetALineAndDontArgue(C2V(gTempFile), s2);
    str = c2_strtok(s2, "\t ,/");
    for (i = 0; (int)i < REC2_ASIZE(C2V(gNet_score_targets)); i++) {
        float t;
        c2_sscanf(str, "%d", &t);
        C2V(gNet_auto_increase_credits_dt)[i] = (int)((float)(1000 * t) / (.02f * (float)C2V(gRecovery_cost[i])));
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
    DRfclose(C2V(gTempFile));
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
    DRfclose(f);
}
C2_HOOK_FUNCTION(0x00487e10, LoadKeyMapping)

void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;
    br_pixelmap* pixmap;

    for (i = 0; i < REC2_ASIZE(C2V(gHeadup_image_info)); i++) {
        PossibleService();
        if (C2V(gHeadup_image_info)[i].avail == eNet_or_otherwise
                || (C2V(gHeadup_image_info)[i].avail == eNot_net && C2V(gNet_mode) == eNet_mode_none)
                || (C2V(gHeadup_image_info)[i].avail == eNet_only && C2V(gNet_mode) != eNet_mode_none)) {
            pixmap = DRLoadPixelmap(C2V(gHeadup_image_info)[i].name);
            DRPixelmapConvertRGB565ToRGB555IfNeeded(pixmap, C2V(gBack_screen)->type);
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
        C2V(gPedTextTxtPath) = "TEXT.TXT";
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gPedTextTxtPath));
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gMisc_strings)); i++) {
        if (DRfeof(f)) {
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
    DRfclose(f);
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
    DRfread(the_font->width, data_size, 1, f);
    data_size = 256 * sizeof(br_uint_16);
    the_font->encoding = BrMemAllocate(data_size, kMem_br_font_enc);
    DRfread(the_font->encoding, data_size, 1, f);
#if !defined(C2_BIG_ENDIAN)
    for (i = 0; i < 256; i++) {
        the_font->encoding[i] = the_font->encoding[i] >> 8 | the_font->encoding[i] << 8;
    }
#endif
    PossibleService();
    DRfread(&data_size, sizeof(tU32), 1, f);
#if !defined(C2_BIG_ENDIAN)
    data_size = BrSwap32(data_size);
#endif
    PossibleService();
    the_font->glyphs = BrMemAllocate(data_size, kMem_br_font_glyphs);
    DRfread(the_font->glyphs, data_size, 1u, f);
    DRfclose(f);
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
        twt = TWT_MountEx(C2V(gApplication_path));
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
    DRfclose(f);
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
        TWT_UnmountEx(twt);
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
    BAIL_IF_NEGATIVE(c2_fprintf(f, "GoreLevel %d\n", GetGoreLevel()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "AnimalsOn %d\n", GetAnimalsOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "FlameThrowerOn %d\n", GetFlameThrowerOn()));
    BAIL_IF_NEGATIVE(c2_fprintf(f, "MinesOn %d\n", GetMinesOn()));
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

    DRfclose(f);

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
    C2V(gHeadup_detail_level) = 5;
    C2V(gMap_view) = 1;
    C2V(gMini_map_visible) = 1;

    DefaultNetSettings();
    DefaultQuicktimeSettings();

    PathCat(the_path, C2V(gApplication_path), "OPTIONS.TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        return 0;
    }

    while (DRfgets(line, sizeof(line), f)) {
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
                DRfgets(line, sizeof(line), f);
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
                DRfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_compressor), s);
            } else if (c2_strcmp(token, "QuickTimeBanner") == 0) {
                C2V(gQuick_time_banner_number) = (int)arg;
                DRfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_banner_texture_name), s);
            } else if (c2_strcmp(token, "QuickTimeTempPath") == 0) {
                DRfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_temp_path), s);
            } else if (c2_strcmp(token, "QuickTimeMoviePathStub") == 0) {
                DRfgets(line, sizeof(line), f);
                s = c2_strtok(line, "\n\r");
                c2_strcpy(C2V(gQuick_time_movie_path_stub), s);
            } else if (c2_strcmp(token, "NetName") == 0) {
                DRfgets(line, sizeof(line), f);
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
    DRfclose(f);
    C2V(gMap_view) = 1;
    C2V(gMap_render_x) = 80.f;
    C2V(gMap_render_y) = 400.f;
    C2V(gMap_render_width) = 128.f;
    C2V(gMap_render_height) = 80.f;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048d8f0, RestoreOptions, RestoreOptions_original)

void (C2_HOOK_FASTCALL * LoadInRegistees_original)(void);
void C2_HOOK_FASTCALL LoadInRegistees(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadInRegistees_original();
#else
    tPath_name the_path;
    tPath_name the_path2;
    tTWTVFS twt;

    PathCat(the_path, C2V(gApplication_path), "REG");
    DRForEveryArchivedFile(the_path, "PALETTES", DRLoadPalette);
    DRForEveryArchivedFile(the_path, "SHADETAB", DRLoadShadeTable);
    InitializePalettes();

    PathCat(the_path2, the_path, "PIXELMAP");
    twt = TWT_MountEx(the_path2);
    LoadAllTexturesFromTexSubdirectories(&C2V(gMisc_storage_space), the_path2);
    TWT_UnmountEx(twt);

    DRForEveryArchivedFile(the_path, "MATERIAL", DRLoadMaterials);
    DRForEveryArchivedFile(the_path, "MODELS", DRLoadModels);
    DRForEveryArchivedFile(the_path, "ACTORS", DRLoadActors);
    DRForEveryArchivedFile(the_path, "LIGHTS", DRLoadLights);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00486e10, LoadInRegistees, LoadInRegistees_original)

void (C2_HOOK_FASTCALL * LoadTreeSurgery_original)(void);
void C2_HOOK_FASTCALL LoadTreeSurgery(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadTreeSurgery_original();
#else
    tPath_name the_path;
    FILE* file;
    int i;

    PathCat(the_path, C2V(gApplication_path), "TreeSurgery.TXT");
    file = TWT_fopen(the_path, "rt");
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
    DRfclose(file);
#endif
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00504b30, LoadTreeSurgery, LoadTreeSurgery_original)

int (C2_HOOK_FASTCALL * TestForOriginalCarmaCDinDrive_original)(void);
int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void) {

#if defined(C2_HOOKS_ENABLED)
    return TestForOriginalCarmaCDinDrive_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f3b0, TestForOriginalCarmaCDinDrive, TestForOriginalCarmaCDinDrive_original)

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

br_uint_32 C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* pFile_name, br_pixelmap** pPixelmaps, br_uint_16 pNum) {
    br_pixelmap* the_map;
    int number_loaded;
    int i;

    number_loaded = BrPixelmapLoadMany(pFile_name, pPixelmaps, pNum);
    for (i = 0; i < number_loaded; i++) {
        the_map = pPixelmaps[i];
        the_map->row_bytes = (the_map->row_bytes + sizeof(int32_t) - 1) & ~(sizeof(int32_t) - 1);
        the_map->base_x = 0;
        the_map->base_y = 0;
    }
    return number_loaded;
}

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
    DisableVertexColours(model_array, number_of_models);
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

#if defined(C2_HOOKS_ENABLED)
    InitializePalettes_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b5090, InitializePalettes, InitializePalettes_original)

void (C2_HOOK_FASTCALL * DisableVertexColours_original)(br_model** pModels, int pCount);
void C2_HOOK_FASTCALL DisableVertexColours(br_model** pModels, int pCount) {

#if defined(C2_HOOKS_ENABLED)
    DisableVertexColours_original(pModels, pCount);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00518690, DisableVertexColours, DisableVertexColours_original)

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
#if defined(C2_HOOKS_ENABLED)
    FillInRaceInfo_original(pThe_race);
#else

    c2_strcpy(C2V(gProgram_state).track_file_name, pThe_race->race_spec->track_file_name);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048d0e0, FillInRaceInfo, FillInRaceInfo_original)

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
void C2_HOOK_FASTCALL InitAIWorld(void) {

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
C2_HOOK_FUNCTION_ORIGINAL(0x00401000, InitAIWorld, InitAIWorld_original)

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
    DRfclose(f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048c930, LoadOpponents, LoadOpponents_original)

FILE* C2_HOOK_FASTCALL OpenDrone(const char* pDrone_name) {
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
C2_HOOK_FUNCTION(0x0044f640, OpenDrone)

void C2_HOOK_FASTCALL LoadDrone(const char* pDrone_name) {
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
    twt = TWT_MountEx(the_path);
    f = OpenDrone(pDrone_name);

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
    DRfclose(f);
    TWT_UnmountEx(twt);
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
            if (c2_strcmp(s, "END OF DRONES") == 0 || DRfeof(f)) {
                break;
            }
            LoadDrone(s);
        }
        DRfclose(f);
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

void C2_HOOK_FASTCALL LoadAIWorld(FILE* pF) {

    PrintMemoryDump(0, "BEFORE LoadInOppoPaths()");
    LoadInOppoPaths(pF);
    PrintMemoryDump(0, "AFTER LoadInOppoPaths(), BEFORE LoadInDronePaths()");
    LoadInDronePaths(pF);
    PrintMemoryDump(0, "AFTER LoadInDronePaths()");
}
C2_HOOK_FUNCTION(0x00401030, LoadAIWorld)

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
        pCar_spec->gears_image = DRLoadPixelmap(str);
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
        pCar_spec->tacho_image[pIndex] = DRLoadPixelmap(str);
    } else {
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_x[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        c2_sscanf(str, "%d", &pCar_spec->tacho_y[pIndex]);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->tacho_image[pIndex] = DRLoadPixelmap(str);
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
        pCar_spec->speedo_image[pIndex] = DRLoadPixelmap(str);
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
        pCar_spec->speedo_image[pIndex] = DRLoadPixelmap(str);
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
    LoadSomeMaterialsWithShading(pStorage, pPath, kRendererShadingType_Specular);
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
    twt = TWT_MountEx(car_path);

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
            ApplyPreviousTiffConversion();
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
            pCar_spec->lhands_images[i] = DRLoadPixelmap(str);
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
        pCar_spec->prat_cam_left = DRLoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_top = DRLoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_right = DRLoadPixelmap(str);
        str = c2_strtok(NULL, "\t ,/");
        pCar_spec->prat_cam_bottom = DRLoadPixelmap(str);
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
                pCar_spec->damage_units[i].images = DRLoadPixelmap(str);
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
        pCar_spec->damage_background = DRLoadPixelmap(str);

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
        DRfclose(h);

        AdjustCarCoordinates(&C2V(gProgram_state).current_car);
        AdjustRenderScreenSize();
        PossibleService();
        ReinitialiseRearviewCamera();
        GetALineAndDontArgue(f, s);
        if (twt >= 0) {
            ApplyTopTiffConversion();
        }
        C2_HOOK_ASSERT(c2_strcmp(s, "END OF DRIVABLE STUFF") == 0);
    } else {
        while (!DRfeof(f)) {
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

    LoadAllTexturesFromTexSubdirectories(pStorage_space, car_path);
    LoadSomeShadeTables(pStorage_space, car_path);
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
    LoadSomeModels(pStorage_space, car_path);
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

    while (!DRfeof(f)) {
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

    DRfclose(f);
    DRfclose(g);
    C2V(gCurrent_car_spec) = NULL;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, field_0x18c8, 0x18c8);

    pCar_spec->field_0x18c8 = 0;
    DRActorEnumRecurse(pCar_spec->car_model_actor, AttachGroovidelic, NULL);
    AttachCrushDataToActorModels(pCar_spec->car_model_actor, pCar_spec);
    PrepareCarForCrushing(pCar_spec);
    TWT_UnmountEx(twt);
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

void C2_HOOK_FASTCALL LoadPlayerCars(tRace_info* pRace_info) {
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
C2_HOOK_FUNCTION(0x0048cda0, LoadPlayerCars)

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

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, field_0x64, 0x64);

    c->collision_info->field_0x64 = 0.4f;

    LoadCollisionShape(&c->collision_info->shape, pF);

    UpdateCollisionBoundingBox(c->collision_info);

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
    AdaptMaterialsForRenderer(&C2V(gSimple_materials)[i], 1, pShading_type);
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

void C2_HOOK_FASTCALL SetMaterialTrackLighting(br_material* pMaterial) {

    if (C2V(gNbPixelBits) != 16) {
        return;
    }
    if ((uintptr_t)pMaterial->user == 0x5ba0) { /* FIXME: what is 0x5ba0/23456 magic? */
        float original_ka = pMaterial->ka;
        int original_flags = pMaterial->flags;
        pMaterial->ka = 1.f;
        pMaterial->kd = 1.f;
        pMaterial->flags |= BR_MATF_SMOOTH | BR_MATF_LIGHT;
        pMaterial->flags &= ~BR_MATF_PRELIT;
        if (original_flags != pMaterial->flags || original_ka <= 0.999f) {
            BrMaterialUpdate(pMaterial, BR_MATU_LIGHTING | BR_MATU_RENDERING);
        }
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
C2_HOOK_FUNCTION(0x004f6a90, SetMaterialTrackLighting)

void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath) {
    int i;

    LoadSomeMaterialsWithShading(pStorage, pPath, kRendererShadingType_Default);
    for (i = 0; i < pStorage->materials_count; i++) {
        br_material* material = pStorage->materials[i];

        SetMaterialTrackLighting(material);
    }
}
C2_HOOK_FUNCTION(0x004f6640, LoadTrackMaterials)

int C2_HOOK_FASTCALL LoadAllModelsInPath(tBrender_storage* pStorage, const char* pPath) {
    br_model* temp_array[2000];
    int count;
    int new_ones;
    int i;

    new_ones = 0;
    count = BrModelLoadMany(pPath, temp_array, REC2_ASIZE(temp_array));
    DisableVertexColours(temp_array, count);
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
C2_HOOK_FUNCTION(0x004f6580, LoadAllModelsInPath)

void C2_HOOK_FASTCALL LoadDATModelsCallback(const char* pPath) {
    char s[256];

    StringToUpper(s, pPath);
    if (c2_strstr(s, ".DAT") != NULL) {
        LoadAllModelsInPath(C2V(gStorageForCallbacks), pPath);
    }
}
C2_HOOK_FUNCTION(0x004f6540, LoadDATModelsCallback)

void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage *pStorage, const char *pPath) {
    C2V(gStorageForCallbacks) = pStorage;
    DRForEveryFile(pPath, LoadDATModelsCallback);
}
C2_HOOK_FUNCTION(0x004f6520, LoadTrackModels)

void (C2_HOOK_FASTCALL * LoadNonCar_original)(FILE* pF, tNon_car_spec* pNon_car_spec);
void C2_HOOK_FASTCALL LoadNonCar(FILE* pF, tNon_car_spec* pNon_car_spec) {

#if defined(C2_HOOKS_ENABLED)
    LoadNonCar_original(pF, pNon_car_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00487ec0, LoadNonCar, LoadNonCar_original)

void (C2_HOOK_FASTCALL * LoadPerRaceDroneStuff_original)(void);
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {

#if defined(C2_HOOKS_ENABLED)
    LoadPerRaceDroneStuff_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044fda0, LoadPerRaceDroneStuff, LoadPerRaceDroneStuff_original)

void C2_HOOK_FASTCALL LoadCars(tRace_info* pRace_info) {

    PrintMemoryDump(0,"BEFORE LoadOpponentsCars()");
    LoadPlayerCars(pRace_info);
    LoadCopCars();
    PrintMemoryDump(0,"AFTER LoadOpponentsCars(), BEFORE LoadPerRaceDroneStuff()");
    LoadPerRaceDroneStuff();
    PrintMemoryDump(0,"AFTER LoadPerRaceDroneStuff()");
}
C2_HOOK_FUNCTION(0x00401070, LoadCars)

void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race) {

}
C2_HOOK_FUNCTION(0x00487ea0, LoadInterfaceStuff)

void C2_HOOK_FASTCALL InitOpponentsAndDrones(tRace_info* pRace_info) {
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
C2_HOOK_FUNCTION(0x004010b0, InitOpponentsAndDrones)
