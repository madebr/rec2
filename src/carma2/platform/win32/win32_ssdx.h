#ifndef C2_WIN32_SSDX_H
#define C2_WIN32_SSDX_H

#include <dsound.h>

#include "c2_hooks.h"
#include <s3/s3.h>

C2_HOOK_VARIABLE_DECLARE(LPDIRECTSOUND, gDirectSound);

enum tSSDXFlags {
    SSDX_InitDirectSound = 0x2,
};


void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

void* C2_HOOK_FASTCALL PDS3BufferWav(const char* pPath, tS3_buffer_desc* pBUffer_desc);

#endif // C2_WIN32_SSDX_H
