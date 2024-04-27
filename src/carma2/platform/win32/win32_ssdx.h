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
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_path_separator, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gS3_sound_folder_name, 6);
C2_HOOK_VARIABLE_DECLARE(MCI_OPEN_PARMS, gPDS3_mci_open_parms);
C2_HOOK_VARIABLE_DECLARE(MCI_SET_PARMS, gPDS3_mci_set_parms);
C2_HOOK_VARIABLE_DECLARE(int, gPDS3_cda_initialized);

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};


void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

int C2_HOOK_FASTCALL PDS3Init(void);

int C2_HOOK_FASTCALL PDS3DDXInit(void);

void* C2_HOOK_FASTCALL PDS3BufferWav(const char* pPath, tS3_buffer_desc* pBUffer_desc);

int C2_HOOK_FASTCALL PDS3InitCDA(void);

#endif // C2_WIN32_SSDX_H
