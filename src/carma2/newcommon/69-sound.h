#ifndef GUARD_69_SOUND_H
#define GUARD_69_SOUND_H

#include "brender/brender.h"

#include "c2_hooks.h"

extern int gCD_fully_installed;

extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);

extern void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

extern void C2_HOOK_FASTCALL InitSound(void);

// DRS3StartSound

// DRS3StartSoundNoPiping

// DRS3StartSound2

// DRS3ChangeVolume

// DRS3StopSound

// DRS3Service

// DRS3SoundStillPlaying

extern int C2_HOOK_FASTCALL DRS3ShutDown(void);

// DRS3SetOutletVolume

// DRS3StopOutletSound

// DRS3StopAllOutletSoundsExceptCDA

// ToggleSoundEnable

extern void C2_HOOK_FASTCALL SoundService(void);

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

extern void C2_HOOK_FASTCALL StopMusic(void);

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

extern void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void);

#endif // GUARD_69_SOUND_H
