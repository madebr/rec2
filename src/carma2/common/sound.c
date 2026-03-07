#include "sound.h"

#include "brucetrk.h"
#include "car.h"
#include "52-errors.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "opponent.h"
#include "piping.h"
#include "replay.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <s3/s3.h>

#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x00595c48
int gSound_detail_level = 1;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

// GLOBAL: CARMA2_HW 0x00684560
int gSound_sources_inited;


// GLOBAL: CARMA2_HW 0x006845fc
tS3_outlet* gEffects_outlet;

// GLOBAL: CARMA2_HW 0x00684604
tS3_outlet* gEngine_outlet;

// GLOBAL: CARMA2_HW 0x00684564
int gMusic_available;

// GLOBAL: CARMA2_HW 0x00684554
int gINT_00684554;

// GLOBAL: CARMA2_HW 0x00684568
int gINT_00684568;

// GLOBAL: CARMA2_HW 0x00655dd8
int gVirgin_pass = 1;

// GLOBAL: CARMA2_HW 0x00595c4c
int gOld_sound_detail_level = -1;

// GLOBAL: CARMA2_HW 0x0079e13c
int gCD_is_disabled;

// GLOBAL: CARMA2_HW 0x00684608
tS3_outlet* gDriver_outlet;

// GLOBAL: CARMA2_HW 0x00684610
tS3_outlet* gMusic_outlet;

// GLOBAL: CARMA2_HW 0x00684600
tS3_outlet* gCar_outlet;

// GLOBAL: CARMA2_HW 0x0068460c
tS3_outlet* gPedestrians_outlet;

// GLOBAL: CARMA2_HW 0x00684614
tS3_outlet* gXXX_outlet;

// GLOBAL: CARMA2_HW 0x0079e160
tS3_outlet* gIndexed_outlets[6];

// GLOBAL: CARMA2_HW 0x00595c28
const char* gSound_periodicity_choices[3] = {
    "RANDOM",
    "PERIODIC",
    "CONTINUOUS",
};

// GLOBAL: CARMA2_HW 0x0079e120
br_vector3 gCamera_left;

// GLOBAL: CARMA2_HW 0x0079e130
br_vector3 gCamera_position;

// GLOBAL: CARMA2_HW 0x0079ea60
br_vector3 gCamera_velocity;

// GLOBAL: CARMA2_HW 0x00684540
tU32 gNext_sound_generator_munging;

// GLOBAL: CARMA2_HW 0x0079e18c
int gCount_environmental_sound_sources;

// GLOBAL: CARMA2_HW 0x0079e17c
int gEnvironmental_sound_sources_buffer_index;

// GLOBAL: CARMA2_HW 0x00684570
tEnvironment_sound_source gEnvironment_sound_sources[5];

// GLOBAL: CARMA2_HW 0x00595c44
int gINT_00595c44 = 1;

// GLOBAL: CARMA2_HW 0x00684548
tU32 gNext_track_finished_check;

// GLOBAL: CARMA2_HW 0x00595c20
int gINT_00595c20 = 9998;

// GLOBAL: CARMA2_HW 0x00684558
int gServicing_sound;

// GLOBAL: CARMA2_HW 0x00684544
tU32 gLast_sound_service;

// GLOBAL: CARMA2_HW 0x0068454c
int gCDA_started_playing;

// GLOBAL: CARMA2_HW 0x0068455c
undefined4 gUNK_0068455c;

// GLOBAL: CARMA2_HW 0x00595c50
int gLast_tune = -1;

// GLOBAL: CARMA2_HW 0x00595c78
int gRandom_CDA_tunes[8] = { 9600, 9601, 9602, 9603, 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c98
int gRandom_CDA_tunes_1[4] = { 9600, 9601, 9602, 9603 };

// GLOBAL: CARMA2_HW 0x00595ca8
int gRandom_CDA_tunes_2[4] = { 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x0079e180
br_vector3 gOld_camera_position;

// GLOBAL: CARMA2_HW 0x0079e1a0
tEnvironment_sound_generator_info gEnvironmental_sound_sources_buffer[2][20];

// FUNCTION: CARMA2_HW 0x00454f40
void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {
    char line1[80];
    char line2[80];
    char line3[80];
    char path_file[80];
    FILE* fp;

    strcpy(path_file, gApplication_path);
    strcat(path_file, (char*)gDir_separator);
    strcat(path_file, "PATHS.TXT");

    if (PDCheckDriveExists(path_file)) {
        fp = PFfopen(path_file, "rt");
        if (fp == NULL) {
            return;
        }
        line1[0] = '\0';
        line2[0] = '\0';
        line3[0] = '\0';
        GetALineWithNoPossibleService(fp, line1);
        GetALineWithNoPossibleService(fp, line2);
        GetALineWithNoPossibleService(fp, line3);
        PFfclose(fp);
        if (strcmp(line3, "Full") != 0) {
            return;
        }
    }
    gCD_fully_installed = 1;
}

// FUNCTION: CARMA2_HW 0x004569f0
void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;

    pSpec->periodicity = GetALineAndInterpretCommand(pF, gSound_periodicity_choices, REC2_ASIZE(gSound_periodicity_choices));
    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        return;
    }
    if (pSpec->periodicity == kSoundFxPeriodicity_Random) {
        float f1, f2;

        GetPairOfFloats(pF, &f1, &f2);
        pSpec->periodic1 = (int)(1000.f * f1);
        pSpec->periodic2 = (int)(1000.f * f2);
    } else if (pSpec->periodicity == kSoundFxPeriodicity_Periodic) {
        pSpec->periodic1 = (int)(1000.f * GetAScalar(pF));
    }
    pSpec->field_0x14 = BR_FIXED_INT(GetAnInt(pF)) / 100;
    pSpec->count_sound_alternatives = GetAnInt(pF);

    C2_HOOK_BUG_ON(REC2_ASIZE(pSpec->sound_alternatives) != 5);
    if (pSpec->count_sound_alternatives > REC2_ASIZE(pSpec->sound_alternatives)) {
        FatalError(kFatalError_TooManyEnvironmentalSoundAlternatives);
    }
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        pSpec->sound_alternatives[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
    }
    pSpec->field_0xc = 0;
}

// FUNCTION: CARMA2_HW 0x00457450
void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;

    if (pSpec->periodicity == kSoundFxPeriodicity_None) {
        fprintf(pF, "NONE\t\t\t\t\t// sound time type\n");
        return;
    }

    fprintf(pF, "%s\t\t\t\t\t// sound time type\n", gSound_periodicity_choices[pSpec->periodicity]);
    switch (pSpec->periodicity) {
    case kSoundFxPeriodicity_Random:
        fprintf(pF, "%.2f,%.2f\t\t\t\t\t// min,max gap\n",
           (float)pSpec->periodic1 / 1000.f,
           (float)pSpec->periodic2 / 1000.f);
        break;
    case kSoundFxPeriodicity_Periodic:
        fprintf(pF,"%.2f\t\t\t\t\t// period\n",
            (float)pSpec->periodic1 / 1000.f);
        break;
    default:
        break;
    }
    fprintf(pF, "%d\t\t\t\t\t// max deviation\n", (int)BrFixedToFloat(100.f * pSpec->field_0x14));
    fprintf(pF, "%d\t\t\t\t\t// num sounds\n", pSpec->count_sound_alternatives);
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        fprintf(pF, "%d\t\t\t\t\t// sound ID\n", pSpec->sound_alternatives[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00456af0
void C2_HOOK_FASTCALL DoEnvSound(tSpecial_volume* pVolume, br_vector3* pP, tSpecial_volume_soundfx_type pType, tSpecial_volume_soundfx_data* pSound_data, float pSound_dist, br_vector3 *pSound_dir) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00456c60
void C2_HOOK_FASTCALL DoAnEnvironmentalSound(void* pSrc_object, tEnvironment_sound_generator_info* pEnv_info, int pVolume, br_vector3* pPos) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004568c0
void C2_HOOK_FASTCALL StartMusic(void) {

    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        if (!IsCDAPlaying()) {
            gNext_track_finished_check = PDGetTotalTime() + 10000;
            dr_dprintf("CDINFO: StartMusic(): New gNext_track_finished_check %d", gNext_track_finished_check);
            gINT_00684568 = DRS3StartCDA(gINT_00595c20);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00456910
void C2_HOOK_FASTCALL StopMusic(void) {
    if (gCD_fully_installed && gMusic_available && gINT_00684568 != 0) {
        S3StopSound(gINT_00684568);
        gINT_00684554 = 0;
        gINT_00684568 = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00455690
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (!gSound_enabled) {
        return 0;
    }
    if (pSound != eSoundId_pratcam_whirr_noise && !(pSound >= 3000 && pSound <= 3007) && !(pSound >= 5300 && pSound <= 5320)) {
        PipeSingleSound(pOutlet, pSound, 0, 0, -1, NULL);
    }
    return S3StartSound(pOutlet, pSound);
}

// FUNCTION: CARMA2_HW 0x00455710
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet* pOutlet, tS3_sound_id pSound, tU32 pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch) {
    if (!gSound_enabled) {
        return 0;
    }
    if (pOutlet != gMusic_outlet
            && pSound != 1000
            && !(pSound >= 3000 && pSound <= 3007)
            && !(pSound >= 5300 && pSound <= 5320)
            && (pLeft_volume != 0 || pRight_volume != 0)) {
            PipeSingleSound(pOutlet, pSound, pLeft_volume, pRight_volume, pLeft_pitch, NULL);
        }
    return S3StartSound2(pOutlet, pSound, pRepeats, pLeft_volume, pRight_volume, pLeft_pitch, pRight_pitch);
}

int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (gSound_enabled) {
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004566a0
tS3_outlet* C2_HOOK_FASTCALL GetOutletFromIndex(int pIndex) {

    return gIndexed_outlets[pIndex];
}

// FUNCTION: CARMA2_HW 0x004566b0
int C2_HOOK_FASTCALL GetIndexFromOutlet(tS3_outlet* pOutlet) {
    int i;

    for (i = 0; i < REC2_ASIZE(gIndexed_outlets); i++) {
        if (gIndexed_outlets[i] == pOutlet) {
            return i;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004566d0
int C2_HOOK_FASTCALL DRS3StartCDA(int pSound) {

    if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
        gINT_00595c20 = pSound;
    }
    if (!gCD_is_disabled && gMusic_available && !gINT_00684554 && !gINT_00684568 && S3IsCDAEnabled()) {
        dr_dprintf("CDINFO: DRS3StartCDA(): Requested track id %d", pSound);
        gCDA_started_playing = 1;
        S3StopOutletSound(gMusic_outlet);
        if (gSound_enabled) {
            if (gProgram_state.cockpit_on != 0 && gProgram_state.cockpit_image_index >= 0) {
                S3Service(1, 0);
            } else {
                S3Service(0, 0);
            }
            /* Random CDA track */
            if (pSound == 9999 || pSound == 9998 || pSound == 9997) {
                do {
                    switch (pSound) {
                    case 9997:
                        pSound = gRandom_CDA_tunes_2[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes_2) - 1)];
                        break;
                    case 9998:
                        pSound = gRandom_CDA_tunes_1[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes_1) - 1)];
                        break;
                    default:
                        pSound = gRandom_CDA_tunes[IRandomBetween(0, REC2_ASIZE(gRandom_CDA_tunes) - 1)];
                    }
                } while (pSound == gLast_tune);
            }
            gLast_tune = pSound;
            gINT_00684554 = DRS3StartSoundNoPiping(gMusic_outlet, pSound);
            gINT_00684568 = gINT_00684554;
            if (!gINT_00684554) {
                dr_dprintf("CDINFO: DRS3StartCDA(): Chosen actual CD track %d", pSound);
            }
            gUNK_0068455c = 0;
        }
    }
    return gINT_00684568;
}

// FUNCTION: CARMA2_HW 0x00455080
void C2_HOOK_FASTCALL InitSound(void) {
    tPath_name the_path;
    int engine_channel_count;
    int car_channel_count;
    int ped_channel_count;
    int xxx_channel_count;

    if (gVirgin_pass) {
        PathCat(the_path, gApplication_path, "SOUND");
        PathCat(the_path, the_path, "SOUND.TXT");
        if (gSound_override) {
            gSound_enabled = 0;
        } else {
            gSound_enabled = S3Init(the_path, gAusterity_mode, gPedSoundPath) == 0;
        }
        gSound_available = gSound_enabled;
        S3Set3DSoundEnvironment(0.14492753f, -1.0f, -1.0f);
        gVirgin_pass = 0;
        gCD_is_disabled = 0;
        UsePathFileToDetermineIfFullInstallation();
    }
    if (!gSound_available) {
        return;
    }
    switch (gSound_detail_level) {
    case 0:
        engine_channel_count = 2;
        car_channel_count = 2;
        ped_channel_count = 3;
        xxx_channel_count = 1;
        break;
    case 1:
        engine_channel_count = 4;
        car_channel_count = 3;
        ped_channel_count = 4;
        xxx_channel_count = 3;
        break;
    default:
        engine_channel_count = 6;
        car_channel_count = 4;
        ped_channel_count = 5;
        xxx_channel_count = 5;
        break;
    }
    if (gDriver_outlet == NULL) {
        gIndexed_outlets[0] = gDriver_outlet = S3CreateOutlet(1, 1);
        if (gDriver_outlet == NULL) {
            gSound_available = 0;
            return;
        }
    }
    if (gMusic_outlet == NULL) {
        gIndexed_outlets[2] = gMusic_outlet = S3CreateOutlet(1, 1);
        gMusic_available = gMusic_outlet != NULL;
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
    if (gSound_detail_level != gOld_sound_detail_level) {
        if (gCar_outlet != NULL) {
            S3ReleaseOutlet(gCar_outlet);
            gCar_outlet = NULL;
        }
        if (gPedestrians_outlet != NULL) {
            S3ReleaseOutlet(gPedestrians_outlet);
            gPedestrians_outlet = NULL;
        }
        if (gEngine_outlet != NULL) {
            S3ReleaseOutlet(gEngine_outlet);
            gEngine_outlet = NULL;
        }
        if (gXXX_outlet != NULL) {
            S3ReleaseOutlet(gXXX_outlet);
            gXXX_outlet = NULL;
        }
        if (gEngine_outlet == NULL) {
            gIndexed_outlets[1] = gEngine_outlet = S3CreateOutlet(engine_channel_count, engine_channel_count);
            if (gEngine_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
        }
        if (gCar_outlet == NULL) {
            gIndexed_outlets[3] = gCar_outlet = S3CreateOutlet(car_channel_count, car_channel_count);
            if (gCar_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
        }
        if (gPedestrians_outlet == NULL) {
            gIndexed_outlets[4] = gPedestrians_outlet = S3CreateOutlet(ped_channel_count, ped_channel_count);
            if (gPedestrians_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
        }
        if (gXXX_outlet == NULL) {
            gIndexed_outlets[5] = gXXX_outlet = S3CreateOutlet(xxx_channel_count, xxx_channel_count);
            if (gXXX_outlet == NULL) {
                gSound_available = 0;
                return;
            }
            DRS3SetOutletVolume(gXXX_outlet, gProgram_state.effects_volume);
        }
    }
    if (gEffects_outlet == NULL) {
        gIndexed_outlets[5] = gEffects_outlet = S3CreateOutlet(2, 2);
        if (gEffects_outlet == NULL) {
            gSound_available = 0;
            return;
        }
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    gOld_sound_detail_level = gSound_detail_level;
    SetSoundVolumes(0);
}

// FUNCTION: CARMA2_HW 0x00456530
void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio) {

    if (!gSound_enabled) {
        return;
    }
    if (gEffects_outlet != NULL) {
        DRS3SetOutletVolume(gEffects_outlet, gProgram_state.effects_volume);
    }
    DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gEngine_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gDriver_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gPedestrians_outlet, gProgram_state.effects_volume);
    DRS3SetOutletVolume(gCar_outlet, gProgram_state.effects_volume);
    if (gINT_00595c44) {
        dr_dprintf("CD: Setting volume to %d", gProgram_state.music_volume);
        if (!gCDA_started_playing && !pCD_audio) {
            dr_dprintf("CD: Hasn't played yet, so not stopping/starting it");
        } else {
            if (gProgram_state.music_volume < 128) {
                StopMusic();
            } else {
                StartMusic();
            }
        }
    } else {
        DRS3SetOutletVolume(gMusic_outlet, gProgram_state.music_volume);
    }
}

// FUNCTION: CARMA2_HW 0x00455a80
void C2_HOOK_FASTCALL SoundService(void) {

    if (!gSound_enabled) {
        return;
    }
    if (gServicing_sound) {
        return;
    }
    gServicing_sound = 1;
    gLast_sound_service = PDGetTotalTime();
    if (gCDA_started_playing) {
        tU32 now = PDGetTotalTime();
        if (now > gNext_track_finished_check && !IsCDAPlaying()) {
            dr_dprintf("CDINFO: SoundService(): Stopping & starting CD, time %d, gNext_track_finished_check %d",
                    now, gNext_track_finished_check);
            StopMusic();
            StartMusic();
        }
    }
    DRS3Service();
    gServicing_sound = 0;
}

void C2_HOOK_FASTCALL DRS3Service(void) {

    if (gSound_enabled) {
        if (gProgram_state.cockpit_on  && gProgram_state.cockpit_image_index >= 0) {
            S3Service(1, 1);
        } else {
            S3Service(0, 1);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00565d1a
int C2_HOOK_FASTCALL IsCDAPlaying(void) {
    return PDS3IsCDAPlaying();
}

// FUNCTION: CARMA2_HW 0x004559e0
int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void) {

    if (!gSound_enabled) {
        return 0;
    }
    DRS3StopOutletSound(gCar_outlet);
    DRS3StopOutletSound(gEngine_outlet);
    DRS3StopOutletSound(gXXX_outlet);
    DRS3StopOutletSound(gEffects_outlet);
    DRS3StopOutletSound(gPedestrians_outlet);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00455970
int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet) {

    if (gSound_enabled) {
        return S3StopOutletSound(pOutlet);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00455a50
void C2_HOOK_FASTCALL ToggleSoundEnable(void) {

    if (gSound_enabled) {
        S3StopAllOutletSounds();
        S3Disable();
        gSound_enabled = 0;
    } else {
        S3Enable();
        gSound_enabled = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00455de0
void C2_HOOK_FASTCALL DisposeSoundSources(void) {
    int cat;
    int car_count;
    int i;
    int toggle;
    tCar_spec* the_car;

    if (!gSound_available) {
        return;
    }
    toggle = !gSound_enabled;
    if (toggle) {
        ToggleSoundEnable();
    }
    if (gSound_sources_inited) {
        DRS3StopOutletSound(gEngine_outlet);
        if (gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0) {
            S3Service(1, 0);
        } else {
            S3Service(0, 0);
        }
        for (cat = eVehicle_self; cat <= eVehicle_rozzer; cat++) {
            if (cat == eVehicle_self) {
                car_count = 1;
            } else {
                car_count = GetCarCount(cat);
            }
            for (i = 0; i < car_count; i++) {
                if (cat == eVehicle_self) {
                    the_car = &gProgram_state.current_car;
                } else {
                    the_car = GetCarSpec(cat, i);
                }
                if ((the_car != NULL && the_car->driver == eDriver_local_human) || gSound_detail_level == 3 || cat == eVehicle_rozzer) {
                    if (the_car->sound_source != NULL) {
                        S3UpdateSoundSource(gEngine_outlet, -1, the_car->sound_source, 0.0f, 0, 0, 0, 0x10000, 0x10000);
                        S3ReleaseSoundSource(the_car->sound_source);
                    }
                    the_car->sound_source = NULL;
                }
            }
        }
        gSound_sources_inited = 0;
    }
    if (toggle) {
        ToggleSoundEnable();
    }
}

// FUNCTION: CARMA2_HW 0x00455bb0
void C2_HOOK_FASTCALL InitSoundSources(void) {
    int toggle;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, sound_source, 0x1370);

    if (!gSound_available) {
        return;
    }

    toggle = !gSound_enabled;
    if (toggle) {
        ToggleSoundEnable();
    }

    BrVector3Negate(&gCamera_left, (br_vector3*)gCamera_to_world.m[0]);
    BrVector3Copy(&gCamera_position, (br_vector3*)gCamera_to_world.m[3]);
    S3BindListenerPositionBRender(&gCamera_position);
    S3BindListenerVelocityBRender(&gCamera_velocity);
    S3BindListenerLeftBRender(&gCamera_left);
    if (!gSound_sources_inited) {
        tVehicle_type category;
        int i;

        for (category = eVehicle_rozzer; category >= 0; category -= 1) {
            int car_count;

            car_count = category == eVehicle_self ? 1 : GetCarCount(category);
            for (i = 0; i < car_count; i++) {
                tCar_spec* car;

                PossibleService();
                car = category == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(category, i);

                if ((car != NULL && car->driver == eDriver_local_human) || gSound_detail_level > 2 || category == eVehicle_rozzer) {

                    car->sound_source = S3CreateSoundSourceBR(&car->pos, &car->vel, gEngine_outlet);
                    if (car->sound_source != NULL) {
                        S3BindAmbientSoundToOutlet(gEngine_outlet, category == eVehicle_rozzer ? eSoundId_Cop_Siren : car->engine_noises[0], car->sound_source, 250.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                    }
                }
            }
        }

        C2_HOOK_BUG_ON(REC2_ASIZE(gEnvironment_sound_sources) != 5);
        C2_HOOK_BUG_ON(sizeof(gEnvironment_sound_sources[0]) != 0x1c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, source, 0x0);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, position, 0x4);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tEnvironment_sound_source, field_0x10, 0x10);

        for (i = 0; i < REC2_ASIZE(gEnvironment_sound_sources); i++) {
            tEnvironment_sound_source* env_src;

            env_src = &gEnvironment_sound_sources[i];
            env_src->field_0x10 = NULL;
            if (env_src->source == NULL) {
                env_src->source = S3CreateSoundSourceBR(&env_src->position, &gZero_v__car, gXXX_outlet);
                if (env_src->source != NULL) {
                    S3BindAmbientSoundToOutlet(gXXX_outlet, eSoundId_Cop_Siren, env_src->source, 100.f, 0, 0, 0, BR_FIXED_INT(1), BR_FIXED_INT(1));
                }
            }
        }
        gSound_sources_inited = 1;
    }
    if (toggle) {
        ToggleSoundEnable();
    }

    gCount_environmental_sound_sources = 0;
    gEnvironmental_sound_sources_buffer_index = 0;
    gNext_sound_generator_munging = 0;
}

// FUNCTION: CARMA2_HW 0x00456840
void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00455930
int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

    if (!gSound_enabled) {
        return 0;
    }
    return S3SetOutletVolume(pOutlet, pVolume);
}

// FUNCTION: CARMA2_HW 0x00455910
int C2_HOOK_FASTCALL DRS3ShutDown(void) {

    if (!gSound_enabled) {
        return 0;
    }
    gSound_enabled = 0;
    return S3DisableSound();
}

// FUNCTION: CARMA2_HW 0x004558f0
int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag) {

    if (!gSound_enabled) {
        return 0;
    }

    return S3SoundStillPlaying(pTag);
}

// FUNCTION: CARMA2_HW 0x00455950
int C2_HOOK_FASTCALL DRS3SetVolume(int pVolume) {

    if (!gSound_enabled) {
        return 0;
    }
    return S3SetVolume(pVolume);
}

// FUNCTION: CARMA2_HW 0x00457260
intptr_t C2_HOOK_FASTCALL FoundSoundSource(br_actor* pActor, void* pContext) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00456ea0
void C2_HOOK_FASTCALL MungeSoundGenerators(void) {

    if (gAmbient_sound) {
        int i;

        if (GetTotalTime() - gNext_sound_generator_munging >= 200) {
            tEnvironment_sound_generator_vector found_sources;

            gNext_sound_generator_munging += 200;
            found_sources.sources = gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index ^ 1];
            ProcessNearbyActors(&gProgram_state.track_spec, (br_vector3*)gCamera_to_world.m[3], 10.f, 1, ')', 0, 0x1003, FoundSoundSource, &found_sources);
            for (i = 0; i < gProgram_state.count_track_sound_generators; i++) {
                tTrackSoundGenerator* generator = &gProgram_state.track_sound_generators[i];
                if (generator->type == kSoundGeneratorType_point && Vector3DistanceSquared(&generator->point, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    tEnvironment_sound_generator_info* src = &found_sources.sources[found_sources.count];
                    src->generator = generator;
                    src->soundfx = generator->fx;
                    src->pos = generator->point;
                    src->actor = NULL;
                    found_sources.count += 1;
                }
            }
            for (i = 0; i < gNum_active_non_cars; i++) {
                tNon_car_spec* c = gActive_non_car_list[i];
                br_actor* a = c->collision_info->actor;
                int src_count = found_sources.count;
                if (a != NULL && a->model !=NULL
                        && a->model->identifier != NULL && a->model->identifier[0] == ')'
                        && Vector3DistanceSquared(&a->t.t.translate.t, (br_vector3*)gCamera_to_world.m[3]) < 10.f * 10.f) {
                    FoundSoundSource(a, &found_sources);
                    if (src_count < found_sources.count && !Vector3IsZero(&c->collision_info->v)) {
                        found_sources.sources[src_count].soundfx = found_sources.sources[src_count].generator->fx1_noncar;
                    }
                }
            }
            for (i = 0; i < found_sources.count; i++) {
                int j;
                tEnvironment_sound_generator_info * src_now = &found_sources.sources[i];

                for (j = 0; j < gCount_environmental_sound_sources; j++) {
                    tEnvironment_sound_generator_info* src_prev = &gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][j];

                    if (src_now->generator == src_prev->generator && src_now->actor == src_prev->actor) {
                        src_now->soundfx.field_0xc = src_prev->soundfx.field_0xc;
                        break;
                    }
                }
            }
            gEnvironmental_sound_sources_buffer_index ^= 1;
            gCount_environmental_sound_sources = found_sources.count;
        }

        for (i = 0; i < gCount_environmental_sound_sources; i++) {
            br_vector3 delta;
            tEnvironment_sound_generator_info* src;
            void *src_obj;

            src = &gEnvironmental_sound_sources_buffer[gEnvironmental_sound_sources_buffer_index][i];
            BrVector3Sub(&delta, &src->pos, (br_vector3*)gCamera_to_world.m[3]);
            src_obj = src->actor;
            if (src_obj == NULL) {
                src_obj = src->generator;
            }
            DoAnEnvironmentalSound(src_obj, src, (int)((10.f - BrVector3LengthSquared(&delta)) * 255.f / 10.f), &src->pos);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00456e20
void C2_HOOK_FASTCALL MungeEnvironmentalSound(void) {

    if (gAmbient_sound) {
        int i;

        for (i = 0; i < REC2_ASIZE(gEnvironment_sound_sources); i++) {
            gEnvironment_sound_sources[i].field_0x18 = 0;
        }
        if (!gFaded_palette) {
            FindSpecialVolume((br_vector3*)gCamera_to_world.m[3], 0, 1);
            MungeSoundGenerators();
        }
        for (i = 0; i < REC2_ASIZE(gEnvironment_sound_sources); i++) {

            if (gEnvironment_sound_sources[i].field_0x18 == 0 && gEnvironment_sound_sources[i].field_0x10 != NULL) {
                S3UpdateSoundSource(gXXX_outlet, -1, gEnvironment_sound_sources[i].source, -1.f, -1, -1, 0, -1, -1);
                gEnvironment_sound_sources[i].field_0x10 = NULL;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00456070
void C2_HOOK_FASTCALL MungeEngineNoise(void) {
    int cat;

    if (!gSound_available || !gProgram_state.racing) {
        return;
    }
    BrVector3Copy(&gCamera_position, (br_vector3*)&gCamera_to_world.m[3][0]);
    BrVector3Negate(&gCamera_left, (br_vector3*)&gCamera_to_world.m[0][0]);
    if (gFrame_period != 0) {
        BrVector3Sub(&gCamera_velocity, &gCamera_position, &gOld_camera_position);
        BrVector3InvScale(&gCamera_velocity, &gCamera_velocity, gFrame_period / 1000.0f);
    } else {
        BrVector3Set(&gCamera_velocity, 0.0f, 0.0f, 0.0f);
    }
    BrVector3Copy(&gOld_camera_position, &gCamera_position);
    if (gAction_replay_mode && ARGetReplayRate() <= 1.f) {
        ARGetReplayRate();
    }
    for (cat = eVehicle_rozzer; cat >= 0; cat--) {
        int car_count;
        int i;

        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            tCar_spec* the_car;

            if (cat == eVehicle_self) {
                the_car = &gProgram_state.current_car;
            } else {
                the_car = GetCarSpec(cat, i);
            }
            if ((the_car != NULL && the_car->driver == eDriver_local_human)
                    || gSound_detail_level > 2 || cat == eVehicle_rozzer) {
                float pitch;
                float vol;
                int type_of_engine_noise;

                BrVector3Copy(&the_car->vel, &the_car->collision_info->v);
                if (cat == eVehicle_rozzer) {
                    vol = 255.0f;
                    pitch = (float)BR_FIXED_INT(1);
                } else {
                    if (the_car->collision_info->last_special_volume != NULL) {
                        type_of_engine_noise = the_car->collision_info->last_special_volume->engine_noise_index;
                    } else {
                        type_of_engine_noise = 0;
                    }
                    pitch = 40960.0f + 10.0f * the_car->revs;
                    if (gAction_replay_mode) {
                        pitch *= fabsf(ARGetReplayRate());
                    }
                    if (type_of_engine_noise == 1) {
                        pitch *= 0.75f;
                    } else if (type_of_engine_noise == 2) {
                        pitch *= 0.55f;
                    }

                    pitch = MAX(pitch, 4096.0f);
                    pitch = MIN(pitch, 131072.0f);

                    vol = 96.0f + the_car->revs * 0.0015f;
                    if (type_of_engine_noise == 1) {
                        vol *= 5.0f;
                    } else if (type_of_engine_noise == 2) {
                        vol *= 2.0f;
                    } else {
                        vol *= 2.5f;
                    }
                    vol = MIN(vol, 255);
                }
                if (the_car->field_0x4d4 > 1.0f) {
                    vol *= 1.0f + (the_car->field_0x4d4 - 1.0f) / 12.0f;
                }
                S3UpdateSoundSource(gEngine_outlet, -1, the_car->sound_source, (float)(gAction_replay_mode ? 300 : 250), 0, 0, (int)vol, (int)pitch, BR_FIXED_INT(1));
            }
        }
    }
    SoundService();
}

// FUNCTION: CARMA2_HW 0x00455840
int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag) {

    if (!gSound_enabled) {
        return 0;
    }
    return S3StopSound(pSound_tag);
}

// FUNCTION: CARMA2_HW 0x00455f80
int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed) {

    if (!gSound_enabled) {
        return 0;
    }
    if (pVolume != 0
            && pSound_id != eSoundId_pratcam_whirr_noise
            && !(pSound_id >= 3000 && pSound_id <= 3007)
            && !(pSound_id >= 5300 && pSound_id <= 5320)) {
        PipeSingleSound(pOutlet, pSound_id, pVolume, 0, pPitch, pInitial_position);
    }
    return DRS3StartSound3D(pOutlet, pSound_id, pInitial_position, pInitial_velocity, pRepeats, pVolume, pPitch, pSpeed);
}
