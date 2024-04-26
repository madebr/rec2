#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gSound_detail_level);
C2_HOOK_VARIABLE_DECLARE(int, gCD_fully_installed);
C2_HOOK_VARIABLE_DECLARE(int, gSound_sources_inited);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gEffects_outlet);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gEngine_outlet);
C2_HOOK_VARIABLE_DECLARE(int, gMusic_available);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00684554);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00684568);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

void C2_HOOK_FASTCALL ParseSoundFxDetails(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL StopMusic(void);

tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);

void C2_HOOK_FASTCALL InitSound(void);

void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

void C2_HOOK_FASTCALL SoundService(void);

int C2_HOOK_FASTCALL DRStopCarSounds(void);

int C2_HOOK_FASTCALL DRS3StopOutletSound(tS3_outlet* pOutlet);

void C2_HOOK_FASTCALL ToggleSoundEnable(void);

void C2_HOOK_FASTCALL DisposeSoundSources(void);

void C2_HOOK_FASTCALL InitSoundSources(void);

void C2_HOOK_FASTCALL StartMusicTrack(int pMusic_track);

int C2_HOOK_FASTCALL DRS3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

#endif // REC2_SOUND_H
