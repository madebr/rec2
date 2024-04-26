#ifndef S3_AUDIO_H
#define S3_AUDIO_H

#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(int, gS3_enabled);
C2_HOOK_VARIABLE_DECLARE(tS3_sound_source*, gS3_sound_sources);
C2_HOOK_VARIABLE_DECLARE(int, gS3_nsound_sources);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gS3_outlets);

int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);

void C2_HOOK_FASTCALL S3Enable(void);

void C2_HOOK_FASTCALL S3Disable(void);

int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan);

tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void);

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);

int C2_HOOK_FASTCALL S3StopSound(int pTag);

void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);

void C2_HOOK_FASTCALL S3StopAll(void);

#endif // S3_AUDIO_H
