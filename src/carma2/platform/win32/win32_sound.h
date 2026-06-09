#ifndef C2_WIN32_SOUND_H
#define C2_WIN32_SOUND_H

#include "win32_types.h"

#include <s3/s3.h>

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>

extern LPDIRECTSOUND gDirectSound;
extern LPDIRECTSOUND gPD_S3_direct_sound;
extern int gPDS3_volume_factors[256];
extern tPD_S3_config gPD_S3_config;
extern MCI_OPEN_PARMS gPDS3_mci_open_parms;
extern MCI_SET_PARMS gPDS3_mci_set_parms;
extern int gPDS3_cda_initialized;
extern int gPDS3_midi_playing;
extern int gPDS3_cda_media_present;
extern int gPDS3_cda_is_playing;
extern int gPDS3_cda_paused;
extern int gPDS3_cda_track;
extern MCI_STATUS_PARMS gPDS3_cda_status_parms;
extern int gPDS3_Number_of_tracks;
extern MCI_STATUS_PARMS gPDS3_mci_midi_status_parms;
extern MCI_OPEN_PARMS mci_open_params;
extern MCI_PLAY_PARMS mci_play_parms;
extern MCI_PLAY_PARMS gPDS3_cda_play_parms;

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};

int C2_HOOK_FASTCALL PDS3Init(void);

int C2_HOOK_FASTCALL PDS3DDXInit(void);

int C2_HOOK_FASTCALL PDS3InitCDA(void);

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer);

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel);

const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);

int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel);

void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel);

void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume);

void C2_HOOK_FASTCALL PDS3Stop(void);

int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel);

int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void);

int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel);

int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel);

tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel);

tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel);

void C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time);

#endif
