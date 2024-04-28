#ifndef S3_AUDIO_H
#define S3_AUDIO_H

#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_path_separator, 2);
C2_HOOK_VARIABLE_DECLARE(int, gS3_enabled);
C2_HOOK_VARIABLE_DECLARE(tS3_sound_source*, gS3_sound_sources);
C2_HOOK_VARIABLE_DECLARE(int, gS3_nsound_sources);
C2_HOOK_VARIABLE_DECLARE(tS3_outlet*, gS3_outlets);
C2_HOOK_VARIABLE_DECLARE(int, gS3_CDA_enabled);
C2_HOOK_VARIABLE_DECLARE(int, gS3_next_outlet_id);
C2_HOOK_VARIABLE_DECLARE(int, gS3_noutlets);
C2_HOOK_VARIABLE_DECLARE(int, gS3_soundbank_buffer_len);
C2_HOOK_VARIABLE_DECLARE(char*, gS3_soundbank_buffer);

int C2_HOOK_FASTCALL S3Init(const char* pPath, int pLow_memory_mode, const char* pSound_path);

void C2_HOOK_FASTCALL S3Enable(void);

void C2_HOOK_FASTCALL S3Disable(void);

int C2_HOOK_FASTCALL S3StopChannel(tS3_channel* chan);

int C2_HOOK_FASTCALL S3GetCountChannels(int pCount_channels_1, int pCount_channels_2);

tS3_outlet* C2_HOOK_FASTCALL S3CreateOutlet(int pCount_channels_1, int pCount_channels_2);

int C2_HOOK_FASTCALL S3ReleaseOutlet(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3SetOutletVolume(tS3_outlet* pOutlet, int pVolume);

void C2_HOOK_FASTCALL S3StopAllOutletSounds(void);

int C2_HOOK_FASTCALL S3StopOutletSound(tS3_outlet* pOutlet);

int C2_HOOK_FASTCALL S3ReleaseSoundSource(tS3_sound_source* src);

int C2_HOOK_FASTCALL S3StopSound(int pTag);

void C2_HOOK_FASTCALL S3Service(int inside_cockpit, int unk1);

void C2_HOOK_FASTCALL S3StopAll(void);

void C2_HOOK_FASTCALL S3EnableCDA(void);

void* C2_HOOK_FASTCALL S3LoadSoundBankFile(const char* pPath);

int C2_HOOK_FASTCALL S3LoadSoundbank(const char* pPath, int pLow_memory_mode);

int C2_HOOK_FASTCALL S3CreateOutletChannels(tS3_outlet* outlet, int pChannel_count);

void C2_HOOK_FASTCALL S3SoundBankReaderSkipWhitespace(tS3_soundbank_read_ctx* pContext);

int C2_HOOK_FASTCALL S3SoundBankReadEntry(tS3_soundbank_read_ctx *pContext, const char* pDir_name, int pLow_memory_mode);

void C2_HOOK_FASTCALL S3SoundBankReaderNextLine(tS3_soundbank_read_ctx* pContext);

void C2_HOOK_FASTCALL S3SoundBankReaderSkipToNewline(tS3_soundbank_read_ctx* pContext);

void C2_HOOK_FASTCALL S3SoundBankReaderAdvance(tS3_soundbank_read_ctx* pContext, int pAmount);

#endif // S3_AUDIO_H
