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
#include "powerups.h"
#include "replay.h"
#include "spark.h"
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

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5, 0x00691b40);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsFile, gTwatVfsFiles, 50, 0x00692080);
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gDefaultCockpit, 32, 0x00764ec0);
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
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume, gDefault_water_spec_vol, 0x00761b80);

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

FILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if defined(C2_HOOKS_ENABLED)
    FILE* res = DRfopen_original(pFilename, pMode);
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491170, DRfopen, DRfopen_original)

void (C2_HOOK_FASTCALL * DRfclose_original)(FILE* pFile);
void C2_HOOK_FASTCALL DRfclose(FILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    DRfclose_original(pFile);
#else
#error "not implemented"
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
        twtFile = &C2V(gTwatVfsFiles)[(int)f - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)file - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
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
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
        twtFile->pos = twtFile->start;
        twtFile->error = 0;
        return;
    }
    c2_rewind(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4be0, DRrewind, DRrewind_original)

tU32 (C2_HOOK_FASTCALL * TWT_ReadBinaryU32_original)(FILE* file);
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file) {
#if defined(C2_HOOKS_ENABLED)
    return TWT_ReadBinaryU32_original(file);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f830, TWT_ReadBinaryU32, TWT_ReadBinaryU32_original)

void C2_HOOK_FASTCALL TWT_Init(void) {
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 0; i < REC2_ASIZE(C2V(gTwatVfsFiles)); i++) {
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
    fileSize = TWT_ReadBinaryU32(f);
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
                for (file_index = 0; file_index < REC2_ASIZE(C2V(gTwatVfsFiles)); file_index++) {
                    if (C2V(gTwatVfsFiles)[file_index].start != NULL) {
                        continue;
                    }
                    C2V(gTwatVfsFiles)[file_index].start = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].pos = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].end = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data + C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize;
                    C2V(gTwatVfsFiles)[file_index].error = 0;
                    return (FILE*)(file_index + 1);
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
    GetAString(C2V(gTempFile), C2V(gDefaultCockpit));

    C2V(gKnobbledFramePeriod) = 0;
    C2V(gUnknownOpponentFactor) = 1.f;
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
        // FIXME: assert(pRace_list[i].count_explicit_opponents <= REC2_ASIZE(pRace_list[i].explicit_opponents))
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
            // FIXME: assert(pRace_list[i].options.cars.count_opponents <= REC2_ASIZE(pRace_list[i].options.cars.opponents))
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
    light->colour = BR_COLOUR_RGB(C2V(gGlobal_lighting_data).directional.red, C2V(gGlobal_lighting_data).directional.green, C2V(gGlobal_lighting_data).directional.blue);
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
    tDrone* drone;

    C2_HOOK_BUG_ON(sizeof(tDrone) != 136);

    drone = &C2V(gDrones)[C2V(gCount_drones)];
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
        c2_memset(C2V(gDrones), 0, sizeof(C2V(gDrones)));
        for (C2V(gCount_drones) = 0; C2V(gCount_drones) < REC2_ASIZE(C2V(gDrones)); C2V(gCount_drones)++) {
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

    /* Gear display x,y,image */
    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_x[pIndex]);
    str = c2_strtok(NULL, "\t ,/");
    sscanf(str, "%d", &pCar_spec->gear_y[pIndex]);
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

void (C2_HOOK_FASTCALL * LoadCar_original)(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);
void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space) {

#if defined(C2_HOOKS_ENABLED)
    LoadCar_original(pCar_name, pDriver, pCar_spec, pOwner, pDriver_name, pStorage_space);
#else
#error "Not implemented"
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
