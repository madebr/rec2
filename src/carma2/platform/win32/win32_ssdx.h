#ifndef C2_WIN32_SSDX_H
#define C2_WIN32_SSDX_H

#include "win32_types.h"

#include <ddraw.h>
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
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_media_present);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_is_playing);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_paused);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_track);
C2_HOOK_VARIABLE_DECLARE(MCI_STATUS_PARMS, gPDS3_cda_status_parms);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_Number_of_tracks);
C2_HOOK_VARIABLE_DECLARE(MCI_STATUS_PARMS, gPDS3_mci_midi_status_parms);

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};

HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext);

void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch);

void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes);

void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

void C2_HOOK_FASTCALL SSDXStop(void);

void C2_HOOK_FASTCALL SSDXRelease(void);

void C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch);

void C2_HOOK_FASTCALL SSDXLockAttachedSurface(void);

void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void);

void C2_HOOK_FASTCALL SSDXBlitOrFlip(void);

void C2_HOOK_FASTCALL SSDXBlit(void);

void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount);

void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void);

int C2_HOOK_FASTCALL PDS3Init(void);

int C2_HOOK_FASTCALL PDS3DDXInit(void);

int C2_HOOK_FASTCALL PDS3InitCDA(void);

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer);

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor);

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel);

char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void);

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

#endif // C2_WIN32_SSDX_H
