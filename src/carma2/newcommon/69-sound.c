#include "69-sound.h"

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x00684550
const char* gPedSoundPath;

// GLOBAL: CARMA2_HW 0x0068b898
int gCD_fully_installed;

// SplungeSomeData

// STUB: CARMA2_HW 0x00454f40
void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void) {
    NOT_IMPLEMENTED();
}

// InitSound

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

