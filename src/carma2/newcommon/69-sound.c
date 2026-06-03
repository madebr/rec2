#include "69-sound.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x00684550
const char* gPedSoundPath;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

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

// DRS3StartSoundNoPiping

// DRS3StartSound2

// DRS3ChangeVolume

// DRS3StopSound

// DRS3Service

// DRS3SoundStillPlaying

// STUB: CARMA2_HW 0x00455910
int C2_HOOK_FASTCALL DRS3ShutDown(void) {
    NOT_IMPLEMENTED();
}

// DRS3SetOutletVolume

// DRS3StopOutletSound

// DRS3StopAllOutletSoundsExceptCDA

// ToggleSoundEnable

// SoundService

// InitSoundSources

// DisposeSoundSources

// DRS3StartSound3D

// MungeEngineNoise

// SetSoundVolumes

// GetOutletFromIndex

// GetIndexFromOutlet

// DRS3StartCDA

// DRS3StopCDA

// StartMusicTrack

// StartMusic

// STUB: CARMA2_HW 0x00456910
void C2_HOOK_FASTCALL StopMusic(void) {
    NOT_IMPLEMENTED();
}

// SetSoundDetailLevel

// ReallySetSoundDetailLevel

// GetSoundDetailLevel

// ReadSoundSpec

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

