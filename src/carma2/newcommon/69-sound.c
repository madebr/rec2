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

// GLOBAL: CARMA2_HW 0x00684610
tS3_outlet* gMusic_outlet;

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

// FUNCTION: CARMA2_HW 0x00500060
void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size) {

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

// STUB: CARMA2_HW 0x00455080
void C2_HOOK_FASTCALL InitSound(void) {
    NOT_IMPLEMENTED();
}

// DRS3StartSound

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

// DRS3SetOutletVolume

// DRS3StopOutletSound

// DRS3StopAllOutletSoundsExceptCDA

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

// DisposeSoundSources

// DRS3StartSound3D

// MungeEngineNoise

// SetSoundVolumes

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
            pSpec->periodic1 = (int)(1000.0f * f1);
            break;
        case kSoundFxPeriodicity_Random:
            GetPairOfFloats(pF, &f1, &f2);
            pSpec->periodic1 = (int)(1000.0f * f1);
            pSpec->periodic2 = (int)(1000.0f * f2);
            break;
#ifdef REC2_FIX_BUGS
        default:
#endif
        }
        pSpec->field_0x14 = BR_FIXED_INT(GetAnInt(pF)) / 100;
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

// BuggerModelName

// ReadSoundGenerators

// WriteOutSoundSpec

// FUNCTION: CARMA2_HW 0x00457570
void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void) {
    gPedSoundPath = NULL;
}

