#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gSound_detail_level);
C2_HOOK_VARIABLE_DECLARE(int, gCD_fully_installed);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet_ptr, gEffects_outlet);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

void C2_HOOK_FASTCALL ParseSoundFxDetails(FILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL StopMusic(void);

void C2_HOOK_FASTCALL DRS3StartCDA(tS3_sound_id pCDA_id);

tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet_ptr pOutlet, tS3_sound_id pSound);

void C2_HOOK_FASTCALL InitSound(void);

void C2_HOOK_FASTCALL SetSoundVolumes(int pCD_audio);

void C2_HOOK_FASTCALL SoundService(void);

int C2_HOOK_FASTCALL DRStopCarSounds(void);

#endif // REC2_SOUND_H
