#ifndef GUARD_69_SOUND_H
#define GUARD_69_SOUND_H

#include "rec2_types.h"
#include "brender/brender.h"
#include "s3/s3.h"

#include "c2_hooks.h"

#include <stdio.h>

extern int gCD_fully_installed;

extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);

extern void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

extern void C2_HOOK_FASTCALL InitSound(void);

// DRS3StartSound

extern int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound);

// DRS3StartSound2

// DRS3ChangeVolume

// DRS3StopSound

extern void C2_HOOK_FASTCALL DRS3Service(void);

// DRS3SoundStillPlaying

extern int C2_HOOK_FASTCALL DRS3ShutDown(void);

// DRS3SetOutletVolume

// DRS3StopOutletSound

// DRS3StopAllOutletSoundsExceptCDA

// ToggleSoundEnable

extern void C2_HOOK_FASTCALL SoundService(void);

// InitSoundSources

extern void C2_HOOK_FASTCALL DisposeSoundSources(void);

// DRS3StartSound3D

// MungeEngineNoise

// SetSoundVolumes

// GetOutletFromIndex

// GetIndexFromOutlet

extern int C2_HOOK_FASTCALL DRS3StartCDA(int pSound);

// DRS3StopCDA

// StartMusicTrack

extern void C2_HOOK_FASTCALL StartMusic(void);

extern void C2_HOOK_FASTCALL StopMusic(void);

extern void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel);

// ReallySetSoundDetailLevel

// GetSoundDetailLevel

extern void C2_HOOK_FASTCALL ReadSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

// TryToSetEnvironmentalSound

// DoAnEnvironmentalSound

// DoEnvSound

// FoundSoundSource

// MungeSoundGenerators

// MungeEnvironmentalSound

// BuggerModelName

// ReadSoundGenerators

// WriteOutSoundSpec

extern void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void);

#endif // GUARD_69_SOUND_H
