#include "69-sound.h"

#include "08-loading1.h"
#include "28-world3.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00684550
const char* gPedSoundPath;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

// GLOBAL: CARMA2_HW 0x00684558
int gServicing_sound;

// GLOBAL: CARMA2_HW 0x00684544
tU32 gLast_sound_service;

// GLOBAL: CARMA2_HW 0x0068454c
int gCDA_started_playing;

// GLOBAL: CARMA2_HW 0x00684548
tU32 gNext_track_finished_check;

// GLOBAL: CARMA2_HW 0x00684564
int gMusic_available;

// GLOBAL: CARMA2_HW 0x00684568
int gINT_00684568;

// GLOBAL: CARMA2_HW 0x00684554
int gINT_00684554;

// GLOBAL: CARMA2_HW 0x00595c44
int gINT_00595c44 = 1;

// GLOBAL: CARMA2_HW 0x00595c20
int gINT_00595c20 = 9998;

// GLOBAL: CARMA2_HW 0x0068455c
undefined4 gUNK_0068455c;

// GLOBAL: CARMA2_HW 0x0079e13c
int gCD_is_disabled;

// GLOBAL: CARMA2_HW 0x00595c78
int gRandom_CDA_tunes[8] = { 9600, 9601, 9602, 9603, 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c98
int gRandom_CDA_tunes_1[4] = { 9600, 9601, 9602, 9603 };

// GLOBAL: CARMA2_HW 0x00595ca8
int gRandom_CDA_tunes_2[4] = { 9604, 9605, 9606, 9607 };

// GLOBAL: CARMA2_HW 0x00595c50
int gLast_tune = -1;

// GLOBAL: CARMA2_HW 0x00595c28
const char* gSound_periodicity_choices[3] = {
    "RANDOM",
    "PERIODIC",
    "CONTINUOUS",
};

// GLOBAL: CARMA2_HW 0x00595c48
int gSound_detail_level = 1;

// GLOBAL: CARMA2_HW 0x006845fc
tS3_outlet* gEffects_outlet;

// GLOBAL: CARMA2_HW 0x00684600
tS3_outlet* gCar_outlet;

// GLOBAL: CARMA2_HW 0x00684604
tS3_outlet* gEngine_outlet;

// GLOBAL: CARMA2_HW 0x00684608
tS3_outlet* gDriver_outlet;

// GLOBAL: CARMA2_HW 0x0068460c
tS3_outlet* gPedestrians_outlet;

// GLOBAL: CARMA2_HW 0x00684610
tS3_outlet* gMusic_outlet;

// GLOBAL: CARMA2_HW 0x00684614
tS3_outlet* gXXX_outlet;

// GLOBAL: CARMA2_HW 0x0079e160
tS3_outlet* gIndexed_outlets[6];

// GLOBAL: CARMA2_HW 0x00595c38
const char* gSound_generator_type_names[3] = {
    "NONCAR",
    "ACTOR",
    "POINT",
};

// GLOBAL: CARMA2_HW 0x00655dd8
int gVirgin_pass = 1;

// GLOBAL: CARMA2_HW 0x00595c4c
int gOld_sound_detail_level = -1;

// FUNCTION: CARMA2_HW 0x00500060
void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size) {

    // empty
}

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
            gSound_available = gSound_enabled = 0;
        } else {
            gSound_available = gSound_enabled = S3Init(the_path, gAusterity_mode, gPedSoundPath) == 0;
        }
        S3Set3DSoundEnvironment(0.14492753f, -1.0f, -1.0f);
        gVirgin_pass = 0;
        gCD_is_disabled = 0;
        UsePathFileToDetermineIfFullInstallation();
    }
    if (!gSound_available) {
        return;
    }
    if (gSound_detail_level == 0) {
        engine_channel_count = 2;
        ped_channel_count = 3;
        car_channel_count = 2;
        xxx_channel_count = 1;
    } else if (gSound_detail_level == 1) {
        engine_channel_count = 4;
        car_channel_count = 3;
        ped_channel_count = 4;
        xxx_channel_count = 3;
    } else {
        ped_channel_count = 5;
        engine_channel_count = 6;
        car_channel_count = 4;
        xxx_channel_count = 5;
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

// STUB: CARMA2_HW 0x00455690
tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound) {
    if (gSound_enabled) {
        return S3StartSound(pOutlet, pSound);
    } else {
        return 0;
    }
}

// DRS3StartSound2

// DRS3ChangeVolume

// DRS3StopSound

void C2_HOOK_FASTCALL DRS3Service(void) {

    if (gSound_enabled) {
        S3Service(gProgram_state.cockpit_on && gProgram_state.cockpit_image_index >= 0, 1);
    }
}

// DRS3SoundStillPlaying

// STUB: CARMA2_HW 0x00455910
int C2_HOOK_FASTCALL DRS3ShutDown(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00455930
int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume) {

    if (gSound_enabled) {
        return S3SetOutletVolume(pOutlet, pVolume);
    }
    return 0;
}

// DRS3StopOutletSound

// STUB: CARMA2_HW 0x004559e0
int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void) {
    NOT_IMPLEMENTED();
}

// ToggleSoundEnable

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
        if (now > gNext_track_finished_check && !S3IsCDAPlaying()) {
            dr_dprintf("CDINFO: SoundService(): Stopping & starting CD, time %d, gNext_track_finished_check %d",
                now, gNext_track_finished_check);
            StopMusic();
            StartMusic();
        }
    }
    DRS3Service();
    gServicing_sound = 0;
}

// InitSoundSources

// STUB: CARMA2_HW 0x00455de0
void C2_HOOK_FASTCALL DisposeSoundSources(void) {
    NOT_IMPLEMENTED();
}

// DRS3StartSound3D

// MungeEngineNoise

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

// GetOutletFromIndex

// GetIndexFromOutlet

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
            S3Service(gProgram_state.cockpit_on != 0 && gProgram_state.cockpit_image_index >= 0, 0);
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

// DRS3StopCDA

// StartMusicTrack

// FUNCTION: CARMA2_HW 0x004568c0
void C2_HOOK_FASTCALL StartMusic(void) {

    if (!gINT_00595c44 || gProgram_state.music_volume >= 128) {
        if (!S3IsCDAPlaying()) {
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

// FUNCTION: CARMA2_HW 0x00456950
void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    gSound_detail_level = pLevel;
}

// ReallySetSoundDetailLevel

// GetSoundDetailLevel

// FUNCTION: CARMA2_HW 0x004569f0
void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec) {
    int i;
    float f1, f2;

    pSpec->periodicity = GetALineAndInterpretCommand(pF, gSound_periodicity_choices, REC2_ASIZE(gSound_periodicity_choices));
    if (pSpec->periodicity != kSoundFxPeriodicity_None) {
        switch (pSpec->periodicity) {
        case kSoundFxPeriodicity_Periodic:
            f1 = GetAScalar(pF);
            pSpec->period = (int)(1000.0f * f1);
            break;
        case kSoundFxPeriodicity_Random:
            GetPairOfFloats(pF, &f1, &f2);
            pSpec->random.min_gap = (int)(1000.0f * f1);
            pSpec->random.max_gap = (int)(1000.0f * f2);
            break;
#ifdef REC2_FIX_BUGS
        default:
            break;
#endif
        }
        pSpec->max_deviation = BR_FIXED_INT(GetAnInt(pF)) / 100;
        pSpec->count_sound_alternatives = GetAnInt(pF);

        C2_HOOK_BUG_ON(REC2_ASIZE(pSpec->sound_alternatives) != 5);
        if (pSpec->count_sound_alternatives > (int)REC2_ASIZE(pSpec->sound_alternatives)) {
            FatalError(kFatalError_TooManyEnvironmentalSoundAlternatives);
        }
        for (i = 0; i < pSpec->count_sound_alternatives; i++) {

            pSpec->sound_alternatives[i] = LoadSingleSound(&gTrack_storage_space, GetAnInt(pF));
        }
        pSpec->field_0xc = 0;
    }
}

// TryToSetEnvironmentalSound

// DoAnEnvironmentalSound

// DoEnvSound

// FoundSoundSource

// MungeSoundGenerators

// MungeEnvironmentalSound

void C2_HOOK_FASTCALL BuggerModelName(tTrack_spec* pTrack_spec, const char* pName, int pIndex) {
    br_model* model;
    char identifier[4];

    sprintf(identifier, "%c%02d", ')', pIndex);
    model = BrModelFind(identifier);
    if (model == NULL || model->identifier == NULL) {
#ifdef REC2_FIX_BUGS
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S, identifier);
#else
        FatalError(kFatalError_CannotFindModelReferencedInSoundGeneratorList_S);
#endif
    } else {
        memcpy(model->identifier, identifier, sizeof(identifier));
    }
}

// FUNCTION: CARMA2_HW 0x004572f0
void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF) {
    float x;
    float y;
    float z;
    char name[32];
    tTrackSoundGenerator* generator;
    int i;

    gProgram_state.count_track_sound_generators = GetAnInt(pF);
    if (gProgram_state.count_track_sound_generators != 0) {

        gProgram_state.track_sound_generators = BrMemAllocate(gProgram_state.count_track_sound_generators * sizeof(tTrackSoundGenerator), kMem_sound_generator);
        for (i = 0, generator = gProgram_state.track_sound_generators; i < gProgram_state.count_track_sound_generators; i++, generator++) {

            generator->type = GetALineAndInterpretCommand(pF, gSound_generator_type_names, REC2_ASIZE(gSound_generator_type_names));
            switch (generator->type) {
            case kSoundGeneratorType_point:
                GetThreeFloats(pF, &x, &y, &z);
                BrVector3Set(&generator->point, x, y, z);
                break;
            case kSoundGeneratorType_actor:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            case kSoundGeneratorType_noncar:
                GetAString(pF, name);
                BuggerModelName(pTrack_spec, name, i);
                break;
            }
            ReadSoundSpec(pF, &generator->fx);
            if (generator->type == kSoundGeneratorType_noncar) {
                ReadSoundSpec(pF, &generator->fx1_noncar);
                ReadSoundSpec(pF, &generator->fx2_noncar);
            }
        }
    }
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
    case kSoundFxPeriodicity_Periodic:
        fprintf(pF,"%.2f\t\t\t\t\t// period\n",
            (double)pSpec->period / 1000.0);
        break;
    case kSoundFxPeriodicity_Random:
        fprintf(pF, "%.2f,%.2f\t\t\t\t\t// min,max gap\n",
           (double)pSpec->random.min_gap / 1000.0,
           (double)pSpec->random.max_gap / 1000.0);
        break;
#ifdef REC2_FIX_BUGS
    default:
        break;
#endif
    }
    fprintf(pF, "%d\t\t\t\t\t// max deviation\n", (int)((100.0 * (double)pSpec->max_deviation) / 65536.0));
    fprintf(pF, "%d\t\t\t\t\t// num sounds\n", pSpec->count_sound_alternatives);
    for (i = 0; i < pSpec->count_sound_alternatives; i++) {

        fprintf(pF, "%d\t\t\t\t\t// sound ID\n", pSpec->sound_alternatives[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00457570
void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void) {

    gPedSoundPath = NULL;
}

