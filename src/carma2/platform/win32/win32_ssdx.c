#include "win32_ssdx.h"

#include "win32.h"
#include "win32_sound.h"

#include "52-errors.h"
#include "globvars.h"
#include "16-graphics1.h"

#include "platform.h"

#include <windows.h>
#include <ddraw.h>
#include <dsound.h>

#include "c2_stdlib.h"
#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006aa9e0
int gUse_DirectDraw;

// GLOBAL: CARMA2_HW 0x006aaa08
HWND gHWnd_SSDX;

// GLOBAL: CARMA2_HW 0x006aa9d8
int gEnumerate_DirectX_surfaces;


// GLOBAL: CARMA2_HW 0x006aa5c8
RECT gSSDXRect;

// GLOBAL: CARMA2_HW 0x006aa9f8
LPDIRECTDRAWSURFACE gPrimary_surface;

// GLOBAL: CARMA2_HW 0x006aa9fc
LPDIRECTDRAWSURFACE gAttached_surface;

// GLOBAL: CARMA2_HW 0x006aaa00
LPDIRECTDRAWCLIPPER gDirectDrawClipper;

// GLOBAL: CARMA2_HW 0x006aaa14
LPDIRECTDRAW gDirectDraw;

// GLOBAL: CARMA2_HW 0x006aaa18
LPDIRECTDRAW2 gDirectDraw2;

// GLOBAL: CARMA2_HW 0x006aaa04
LPDIRECTDRAWPALETTE gDirectDrawPalette;

// GLOBAL: CARMA2_HW 0x006aa5d8
PALETTEENTRY gSSDX_system_palette[256];

// GLOBAL: CARMA2_HW 0x006aa9dc
LPRECT gSSDXLockedRect;

// GLOBAL: CARMA2_HW 0x006aaa0c
int gAttached_surface_locked;

// GLOBAL: CARMA2_HW 0x006aa9e8
RECT gSSDXWindowPos;

// GLOBAL: CARMA2_HW 0x007a0540
MCI_OPEN_PARMS mci_open_params;

// GLOBAL: CARMA2_HW 0x007a0530
MCI_PLAY_PARMS mci_play_parms;

// GLOBAL: CARMA2_HW 0x007a04e0
MCI_PLAY_PARMS gPDS3_cda_play_parms;

// GLOBAL: CARMA2_HW 0x006aaa10
int gSSDXPitch;


// FUNCTION: CARMA2_HW 0x00500070
HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext) {
    int* counter = lpContext;
    dr_dprintf("LocalEnumAttachedSurfacesCallback(): Enum-ing attached surface #%d, address 0x%p", *counter, lpSurface);
    gAttached_surface = lpSurface;
    return DDENUMRET_OK;
}

// FUNCTION: CARMA2_HW 0x005000a0
void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch) {
    DDSURFACEDESC desc;
    HRESULT res;
    HDC hdc;
    int i;

    SetRect(&gSSDXRect, 0, 0, pWidth, pHeight);

    memset(&desc, 0, sizeof(desc));
    C2_HOOK_BUG_ON(sizeof(desc) != 0x6c);
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for primary surface...");
    res = IDirectDraw2_CreateSurface(gDirectDraw, &desc, &gPrimary_surface, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    desc.dwHeight = pHeight;
    desc.dwWidth = pWidth;
    desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PALETTE | DDSCAPS_FLIP | DDSCAPS_ALPHA;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for attached surface...");
    res = IDirectDraw_CreateSurface(gDirectDraw, &desc, &gAttached_surface, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDraw_CreateClipper(gDirectDraw, 0, &gDirectDrawClipper, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDrawClipper_SetHWnd(gDirectDrawClipper, 0, gHWnd_SSDX);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    hdc = GetDC(gHWnd_SSDX);
    GetSystemPaletteEntries(hdc, 0, REC2_ASIZE(gSSDX_system_palette), gSSDX_system_palette);
    ReleaseDC(gHWnd_SSDX, hdc);
    for (i = 0; i < REC2_ASIZE(gSSDX_system_palette); i++) {
        if (i > 10 && i < 245) {
            gSSDX_system_palette[i].peFlags = 0;
        } else {
            gSSDX_system_palette[i].peFlags = PC_NOCOLLAPSE;
        }
    }
    dr_dprintf("LocalWindowedDDSetup(): Calling CreatePalette()...");
    res = IDirectDraw_CreatePalette(gDirectDraw, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, gSSDX_system_palette, &gDirectDrawPalette, NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    dr_dprintf("LocalWindowedDDSetup(): Calling SetPalette()...");
    res = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }
post_creation:
    SSDXLockAttachedSurface();
    if (gAttached_surface_locked) {
        res = IDirectDrawSurface_Unlock(gAttached_surface, gSSDXLockedRect);
        if (res == DD_OK) {
            gAttached_surface_locked = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(res);
        }
    }
    *pPitch = gSSDXPitch;
}

// FUNCTION: CARMA2_HW 0x005002d0
void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes) {

#define LOG_CASE_DDERR(V) case V: dr_dprintf("%s (%x)", #V, hRes); return
    switch (hRes) {
    LOG_CASE_DDERR(DDERR_UNSUPPORTED);
    LOG_CASE_DDERR(DDERR_GENERIC);
    LOG_CASE_DDERR(DDERR_OUTOFMEMORY);
    LOG_CASE_DDERR(DDERR_INVALIDPARAMS);
    LOG_CASE_DDERR(DDERR_INCOMPATIBLEPRIMARY);
    LOG_CASE_DDERR(DDERR_INVALIDCAPS);
    LOG_CASE_DDERR(DDERR_INVALIDMODE);
    LOG_CASE_DDERR(DDERR_INVALIDOBJECT);
    LOG_CASE_DDERR(DDERR_INVALIDPIXELFORMAT);
    LOG_CASE_DDERR(DDERR_INVALIDRECT);
    LOG_CASE_DDERR(DDERR_NOEXCLUSIVEMODE);
    LOG_CASE_DDERR(DDERR_SURFACEBUSY);
    LOG_CASE_DDERR(DDERR_SURFACELOST);
    LOG_CASE_DDERR(DDERR_WASSTILLDRAWING);
    default: dr_dprintf("UNKNOWN DD ERROR (%x) N.B. Look for this number in 'ddraw.h': %d", hRes, HRESULT_CODE(hRes)); return;
    LOG_CASE_DDERR(DD_OK);
    LOG_CASE_DDERR(DDERR_IMPLICITLYCREATED);
    LOG_CASE_DDERR(DDERR_WRONGMODE);
    }
#undef LOG_CASE_DDERR
}

// FUNCTION: CARMA2_HW 0x00500500
int C2_HOOK_FASTCALL SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {
    HRESULT hRes;

    hRes = 0;
    dr_dprintf("SSDXStart(): START...");
    gUse_DirectDraw = p_DirectDraw;
    if (p_EnumerateDDX) {
        gEnumerate_DirectX_surfaces = 1;
        if (p_DirectDraw) {
            gEnumerate_DirectX_surfaces = 0;
        }
    } else {
        gEnumerate_DirectX_surfaces = 0;
    }
    gHWnd_SSDX = p_hWnd;
    if (p_flags & 0x2) {
        dr_dprintf("SSDXStart(): Calling DirectSoundCreate()...");
#ifdef DSOUND_ENABLED
        hRes = DirectSoundCreate(0, &gDirectSound, NULL);
#else
        hRes = DSERR_GENERIC;
#endif
    }
    if (hRes != DS_OK) {
        SSDXLogError(hRes);
    }
    dr_dprintf("SSDXStart(): END.");
    return (int)hRes;
}

// FUNCTION: CARMA2_HW 0x00500590
void C2_HOOK_FASTCALL SSDXStop(void) {

    SSDXUnlockAttachedSurface();

    if (gDirectDrawPalette != NULL) {

        dr_dprintf("SSDXStop(): Releasing palette...");
        IDirectDrawPalette_Release(gDirectDrawPalette);
        gDirectDrawPalette = NULL;
    }

    if (gDirectDrawClipper != NULL) {

        dr_dprintf("SSDXStop(): Releasing clipper...");
        IDirectDrawClipper_Release(gDirectDrawClipper);
        gDirectDrawClipper = NULL;
    }

    if (gUse_DirectDraw) {

        if (gAttached_surface != NULL) {
            dr_dprintf("SSDXStop(): Releaseing attached surface...");
            IDirectDrawSurface_Release(gAttached_surface);
            gAttached_surface = NULL;
        }
        if (gPrimary_surface != NULL) {
            dr_dprintf("SSDXStop(): Releaseing primary surface...");
            IDirectDrawSurface_Release(gPrimary_surface);
            gPrimary_surface = NULL;
        }
    } else {

        if (gDirectDraw2 != NULL) {
            dr_dprintf("SSDXStop(): Restoring display mode");
            IDirectDraw2_RestoreDisplayMode(gDirectDraw2);
        }
        if (gPrimary_surface != NULL) {
            dr_dprintf("SSDXStop(): Releaseing primary surface...");
            IDirectDrawSurface_Release(gPrimary_surface);
            gAttached_surface = NULL;
            gPrimary_surface = NULL;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005006d0
void C2_HOOK_FASTCALL SSDXRelease(void) {

    if (gDirectDraw2 != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw2 interface...");
        IDirectDraw2_Release(gDirectDraw2);
        gDirectDraw2 = NULL;
    }
    if (gDirectDraw != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw object...");
        IDirectDraw_Release(gDirectDraw);
        gDirectDraw = NULL;
    }
    if (gDirectSound != NULL) {
        dr_dprintf("SSDXStop(): Releasing DSound object...");
        IDirectSound_Release(gDirectSound);
        gDirectSound = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00500760
void C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch) {
    HRESULT result;

    if (gUse_DirectDraw) {
        LocalWindowedDDSetup(pWidth, pHeight, pPitch);
    }
    dr_dprintf("SSDXDirectDrawSetup(): Calling SetDisplayMode( %dx%dx%d )...", pWidth, pHeight, pBits);
    result = IDirectDraw2_SetDisplayMode(gDirectDraw2, pWidth, pHeight, pBits, 0, !gEnumerate_DirectX_surfaces && pWidth == 320 ? DDSDM_STANDARDVGAMODE : 0);
    SetRect(&gSSDXWindowPos, 0, 0, pWidth, pHeight);
    if (result == DD_OK) {
        DDSURFACEDESC surfaceDesc;

        C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
        memset(&surfaceDesc, 0, sizeof(surfaceDesc));
        surfaceDesc.dwSize = sizeof(surfaceDesc);
        surfaceDesc.dwFlags = DDSD_CAPS | (gEnumerate_DirectX_surfaces ? DDSD_BACKBUFFERCOUNT : 0);
        surfaceDesc.dwBackBufferCount = gEnumerate_DirectX_surfaces ? 1 : 0;
        surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_PRIMARYSURFACE | (gEnumerate_DirectX_surfaces ? DDSCAPS_FLIP | DDSCAPS_COMPLEX : 0);
        dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface()...");
        result = IDirectDraw_CreateSurface(gDirectDraw, &surfaceDesc, &gPrimary_surface, NULL);
        if (result == DD_OK) {

            if (gEnumerate_DirectX_surfaces) {
                int enum_counter;

                dr_dprintf("SSDXDirectDrawSetup(): Calling EnumAttachedSurfaces()...");
                result = IDirectDrawSurface_EnumAttachedSurfaces(gPrimary_surface, &enum_counter, LocalEnumAttachedSurfacesCallback);
                dr_dprintf("SSDXDirectDrawSetup(): enum_counter is %d", enum_counter);
            } else {

                C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
                SetRect(&gSSDXRect, 0, 0, pWidth, pHeight);
                memset(&surfaceDesc, 0, sizeof(surfaceDesc));
                surfaceDesc.dwSize = sizeof(surfaceDesc);
                surfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
                surfaceDesc.dwHeight = pHeight;
                surfaceDesc.dwWidth = pWidth;

                dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface() for attached surface...");
                result = IDirectDraw_CreateSurface(gDirectDraw, &surfaceDesc, &gAttached_surface, NULL);
            }
            if (result == DD_OK) {
                PALETTEENTRY palette[256];

                memset(palette, 0, sizeof(palette));
                result = IDirectDraw_CreatePalette(gDirectDraw, DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette, &gDirectDrawPalette, NULL);
                if (result == DD_OK) {
                    dr_dprintf("SSDXDirectDrawSetup(): Calling SetPalette");
                    result = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
                }
            }
        }
    }
    SSDXLockAttachedSurface();
    SSDXUnlockAttachedSurface();
    *pPitch = gSSDXPitch;
    if (result == DD_OK) {
        dr_dprintf("SSDXDirectDrawSetup(): Directdraw initialised: surface pith %d", gSSDXPitch);
    } else {
        dr_dprintf("WARNING: Problems during SSDXDirectDrawSetup()");
        SSDXLogError(result);
    }
}

// FUNCTION: CARMA2_HW 0x00500a30
void C2_HOOK_FASTCALL SSDXLockAttachedSurface(void) {
    HRESULT result;
    DDSURFACEDESC surfaceDesc;

    if (!gAttached_surface_locked) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but surface already locked");
        goto error;
    }
    if (gAttached_surface == NULL) {
        goto error;
    }
    result = IDirectDrawSurface_IsLost(gPrimary_surface);
    if (result == DDERR_SURFACELOST) {
        dr_dprintf("SSDXLockAttachedSurface(): Primary surface is lost, restoring...");
        result = IDirectDrawSurface_Restore(gPrimary_surface);
    }
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    result = IDirectDrawSurface_IsLost(gAttached_surface);
    if (result == DDERR_SURFACELOST) {
        dr_dprintf("SSDXLockAttachedSurface(): Attached surface is lost, restoring...");
        result = IDirectDrawSurface_Restore(gAttached_surface);
    }
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
    memset(&surfaceDesc, 0, sizeof(surfaceDesc));
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    result = IDirectDrawSurface_Lock(gAttached_surface, NULL, &surfaceDesc, DDLOCK_WAIT, NULL);
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    gAttached_surface_locked = 1;
    gSSDXLockedRect = surfaceDesc.lpSurface;
    gSSDXPitch = surfaceDesc.lPitch;
    return;
error:
    if (gAttached_surface == NULL) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but no attached surface available");
    }
}

// FUNCTION: CARMA2_HW 0x00500b40
void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void) {

    if (gAttached_surface_locked) {
        HRESULT result = IDirectDrawSurface_Unlock(gAttached_surface, gSSDXLockedRect);
        if (result == DD_OK) {
            gAttached_surface_locked = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(result);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00500b90
void C2_HOOK_FASTCALL SSDXBlitOrFlip(void) {

    if (gUse_DirectDraw) {
        SSDXBlit();
    } else {
        IDirectDrawSurface_Flip(gPrimary_surface, NULL, DDFLIP_WAIT);
    }
}

// FUNCTION: CARMA2_HW 0x00500bd0
void C2_HOOK_FASTCALL SSDXBlit(void) {

    IDirectDrawSurface_Blt(gPrimary_surface, &gSSDXWindowPos, gAttached_surface, &gSSDXRect, DDBLT_WAIT, NULL);
}

// FUNCTION: CARMA2_HW 0x00500c00
void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount) {

    if (gUse_DirectDraw && gDirectDrawPalette != NULL) {
        int i;

        for (i = MAX(10, pStart); i < MIN(0xf6, pStart + pCount - 1); i++) {

            gSSDX_system_palette[i].peRed = pPalette[i].peRed;
            gSSDX_system_palette[i].peGreen = pPalette[i].peGreen;
            gSSDX_system_palette[i].peBlue = pPalette[i].peBlue;
        }
        IDirectDrawPalette_SetEntries(gDirectDrawPalette, 0, pStart, pCount, gSSDX_system_palette);
    } else {
        IDirectDrawPalette_SetEntries(gDirectDrawPalette, 0, pStart, pCount, pPalette);
    }
}

// FUNCTION: CARMA2_HW 0x00500ca0
void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void) {
    HRESULT result;

    if (!gUse_DirectDraw) {
        return;
    }
    if (gDirectDrawPalette == NULL) {
        return;
    }
    if (gPrimary_surface == NULL) {
        return;
    }
    result = IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    if (result == DDERR_SURFACELOST) {
        IDirectDrawSurface_Restore(gPrimary_surface);
        IDirectDrawSurface_SetPalette(gPrimary_surface, gDirectDrawPalette);
    }
}

// FUNCTION: CARMA2_HW 0x00500cf0
void C2_HOOK_FASTCALL SSDXWindowPosOrSizeChanged(HWND pHwnd) {

    if (gUse_DirectDraw) {
        GetClientRect(pHwnd, &gSSDXWindowPos);
        ClientToScreen(pHwnd, (POINT*)&gSSDXWindowPos.left);
        ClientToScreen(pHwnd, (POINT*)&gSSDXWindowPos.right);
        dr_dprintf("SSDXWindowPosOrSizeChanged(): New window rect: (%d,%d)(%d,%d)",
            gSSDXWindowPos.left, gSSDXWindowPos.top, gSSDXWindowPos.right, gSSDXWindowPos.bottom);
    }
}
