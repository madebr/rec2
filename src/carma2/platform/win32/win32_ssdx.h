#ifndef C2_WIN32_SSDX_H
#define C2_WIN32_SSDX_H

#include "win32_types.h"

#include <ddraw.h>

#include "c2_hooks.h"

extern PALETTEENTRY gPD_palette_entries[256];

extern HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext);

extern void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch);

extern void C2_HOOK_FASTCALL SSDXLogDirectDrawError(HRESULT hRes);

extern int C2_HOOK_FASTCALL SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags);

extern void C2_HOOK_FASTCALL SSDXStop(void);

extern void C2_HOOK_FASTCALL SSDXRelease(void);

extern int C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch);

extern void* C2_HOOK_FASTCALL SSDXLockAttachedSurface(void);

extern void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void);

extern void C2_HOOK_FASTCALL SSDXBlitOrFlip(void);

extern void C2_HOOK_FASTCALL SSDXBlit(void);

extern void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount);

extern void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void);

extern void C2_HOOK_FASTCALL SSDXWindowPosOrSizeChanged(HWND pHwnd);

#ifdef REC2_SW
extern int C2_HOOK_FASTCALL SSDXFlip(void);
#endif

#endif // C2_WIN32_SSDX_H
