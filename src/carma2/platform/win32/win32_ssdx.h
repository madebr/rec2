#ifndef C2_WIN32_SSDX_H
#define C2_WIN32_SSDX_H

#include "win32_types.h"

#include <ddraw.h>

#include "c2_hooks.h"

HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext);

void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch);

void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes);

int C2_HOOK_FASTCALL SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

void C2_HOOK_FASTCALL SSDXStop(void);

void C2_HOOK_FASTCALL SSDXRelease(void);

void C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch);

void C2_HOOK_FASTCALL SSDXLockAttachedSurface(void);

void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void);

void C2_HOOK_FASTCALL SSDXBlitOrFlip(void);

void C2_HOOK_FASTCALL SSDXBlit(void);

void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount);

void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void);

void C2_HOOK_FASTCALL SSDXWindowPosOrSizeChanged(HWND pHwnd);
#endif // C2_WIN32_SSDX_H
