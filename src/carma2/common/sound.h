#ifndef REC2_SOUND_H
#define REC2_SOUND_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gSound_detail_level);
C2_HOOK_VARIABLE_DECLARE(int, gCD_fully_installed);

void C2_HOOK_FASTCALL UsePathFileToDetermineIfFullInstallation(void);

void C2_HOOK_FASTCALL ParseSoundFxDetails(tTWTFILE* pF, tSpecial_volume_soundfx_data* pSpec);

void C2_HOOK_FASTCALL StopMusic(void);

void C2_HOOK_FASTCALL DRS3StartCDA(tS3_sound_id pCDA_id);

#endif // REC2_SOUND_H
