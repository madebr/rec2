#ifndef GUARD_69_SOUND_H
#define GUARD_69_SOUND_H

#include "rec2_types.h"
#include "brender/brender.h"
#include "s3/s3.h"

#include "c2_hooks.h"

#include <stdio.h>

extern tS3_outlet* gEffects_outlet;

extern int gCD_fully_installed;

extern void C2_HOOK_FASTCALL SplungeSomeData(void* pData, br_size_t size);

extern void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

extern void C2_HOOK_FASTCALL InitSound(void);

extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

extern int C2_HOOK_FASTCALL DRS3StartSoundNoPiping(tS3_outlet* pOutlet, tS3_sound_id pSound);

extern int C2_HOOK_FASTCALL DRS3StartSound2(tS3_outlet *pOutlet, tS3_sound_id pSound, int pRepeats, int pLeft_volume, int pRight_volume, int pLeft_pitch, int pRight_pitch);

extern int C2_HOOK_FASTCALL DRS3ChangeVolume(int pChannel_id, int pVolume);

extern int C2_HOOK_FASTCALL DRS3StopSound(tS3_sound_tag pSound_tag);

extern void C2_HOOK_FASTCALL DRS3Service(void);

extern int C2_HOOK_FASTCALL DRS3SoundStillPlaying(int pTag);

extern void C2_HOOK_FASTCALL DRS3ShutDown(void);

extern int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

extern int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);

extern int C2_HOOK_FASTCALL DRS3StopAllOutletSoundsExceptCDA(void);

extern void C2_HOOK_FASTCALL ToggleSoundEnable(void);

extern void C2_HOOK_FASTCALL SoundService(void);

extern void C2_HOOK_FASTCALL InitSoundSources(void);

extern void C2_HOOK_FASTCALL DisposeSoundSources(void);

extern int C2_HOOK_FASTCALL DRS3StartSound3D(tS3_outlet* pOutlet, int pSound_id, const br_vector3* pInitial_position, const br_vector3* pInitial_velocity, int pRepeats, int pVolume, int pPitch, int pSpeed);

// MungeEngineNoise

extern void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

// GetOutletFromIndex

// GetIndexFromOutlet

extern int C2_HOOK_FASTCALL DRS3StartCDA(int pSound);

extern void C2_HOOK_FASTCALL DRS3StopCDA(void);

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

extern void C2_HOOK_FASTCALL BuggerModelName(tTrack_spec* pTrack_spec, const char* pName, int pIndex);

extern void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF);

extern void C2_HOOK_FASTCALL WriteOutSoundSpec(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

extern void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void);

#endif // GUARD_69_SOUND_H
