#ifndef C2_WIN32_SSDX_H
#define C2_WIN32_SSDX_H

#include "win32_types.h"

#include <dsound.h>

#include "c2_hooks.h"
#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(LPDIRECTSOUND, gDirectSound);
C2_HOOK_VARIABLE_DECLARE(LPDIRECTSOUND, gPD_S3_direct_sound);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gPDS3_volume_factors, 256);
C2_HOOK_VARIABLE_DECLARE(tPD_S3_config, gPD_S3_config);
C2_HOOK_VARIABLE_DECLARE(MCI_OPEN_PARMS, gPDS3_mci_open_parms);
C2_HOOK_VARIABLE_DECLARE(MCI_SET_PARMS, gPDS3_mci_set_parms);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_initialized);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_midi_playing);

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};


void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

int C2_HOOK_FASTCALL PDS3Init(void);

int C2_HOOK_FASTCALL PDS3DDXInit(void);

int C2_HOOK_FASTCALL PDS3InitCDA(void);

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer);

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel);

char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);

int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel);

void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel);

#endif // C2_WIN32_SSDX_H
