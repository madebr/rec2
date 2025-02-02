#include "win32_ssdx.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"

#include <ddraw.h>

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTSOUND, gDirectSound, 0x006aaa1c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTSOUND, gPD_S3_direct_sound, 0x006b2d9c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUse_DirectDraw, 0x006aa9e0);
C2_HOOK_VARIABLE_IMPLEMENT(HWND, gHWnd_SSDX, 0x006aaa08);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnumerate_DirectX_surfaces, 0x006aa9d8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSSDXPitch, 0x006aaa10);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPDS3_volume_factors, 256, 0x007a00e0);
C2_HOOK_VARIABLE_IMPLEMENT(tPD_S3_config, gPD_S3_config, 0x007a0080);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_OPEN_PARMS, gPDS3_mci_open_parms, 0x007a0500);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_SET_PARMS, gPDS3_mci_set_parms, 0x0079fec0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_initialized, 0x006b2d98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gS3_working_directory_initialized, 0x006b2da0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_working_directory, 260, 0x006b2c90);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_midi_playing, 0x006b2da4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_media_present, 0x006b2dac);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_is_playing, 0x006b2db0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_paused, 0x006b2db4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_cda_track, 0x006b2db8);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_STATUS_PARMS, gPDS3_cda_status_parms, 0x007a0528);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPDS3_Number_of_tracks, 0x007a00ac);
C2_HOOK_VARIABLE_IMPLEMENT(MCI_STATUS_PARMS, gPDS3_mci_midi_status_parms, 0x007a0070);
C2_HOOK_VARIABLE_IMPLEMENT(RECT, gSSDXRect, 0x006aa5c8);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWSURFACE, gPrimary_surface, 0x006aa9f8);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWSURFACE, gAttached_surface, 0x006aa9fc);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWCLIPPER, gDirectDrawClipper, 0x006aaa00);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAW, gDirectDraw, 0x006aaa14);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAW2, gDirectDraw2, 0x006aaa18);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWPALETTE, gDirectDrawPalette, 0x006aaa04);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(PALETTEENTRY, gSSDX_system_palette, 256, 0x006aa5d8);
C2_HOOK_VARIABLE_IMPLEMENT(LPRECT, gSSDXLockedRect, 0x006aa9dc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAttached_surface_locked, 0x006aaa0c);
C2_HOOK_VARIABLE_IMPLEMENT(RECT, gSSDXWindowPos, 0x006aa9e8);


HRESULT (CALLBACK * LocalEnumAttachedSurfacesCallback_original)(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext);
HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LocalEnumAttachedSurfacesCallback_original(lpSurface, lpSurfaceDesc, lpContext);
#else
    int* counter = lpContext;
    dr_dprintf("LocalEnumAttachedSurfacesCallback(): Enum-ing attached surface #%d, address 0x%p", *counter, lpSurface);
    C2V(gAttached_surface) = lpSurface;
    return DDENUMRET_OK;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00500070, LocalEnumAttachedSurfacesCallback, LocalEnumAttachedSurfacesCallback_original)

void (C2_HOOK_FASTCALL * LocalWindowedDDSetup_original)(int pWidth, int pHeight, int* pPitch);
void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LocalWindowedDDSetup_original(pWidth, pHeight, pArg3);
#else
    DDSURFACEDESC desc;
    HRESULT res;
    HDC hdc;
    int i;

    SetRect(&C2V(gSSDXRect), 0, 0, pWidth, pHeight);

    c2_memset(&desc, 0, sizeof(desc));
    C2_HOOK_BUG_ON(sizeof(desc) != 0x6c);
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for primary surface...");
    res = IDirectDraw2_CreateSurface(C2V(gDirectDraw), &desc, &C2V(gPrimary_surface), NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    c2_memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
    desc.dwHeight = pHeight;
    desc.dwWidth = pWidth;
    desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PALETTE | DDSCAPS_FLIP | DDSCAPS_ALPHA;
    dr_dprintf("LocalWindowedDDSetup(): Calling CreateSurface() for attached surface...");
    res = IDirectDraw_CreateSurface(C2V(gDirectDraw), &desc, &C2V(gAttached_surface), NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDraw_CreateClipper(C2V(gDirectDraw), 0, &C2V(gDirectDrawClipper), NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    res = IDirectDrawClipper_SetHWnd(C2V(gDirectDrawClipper), 0, C2V(gHWnd_SSDX));
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    hdc = GetDC(C2V(gHWnd_SSDX));
    GetSystemPaletteEntries(hdc, 0, REC2_ASIZE(C2V(gSSDX_system_palette)), C2V(gSSDX_system_palette));
    ReleaseDC(C2V(gHWnd_SSDX), hdc);
    for (i = 0; i < REC2_ASIZE(C2V(gSSDX_system_palette)); i++) {
        if (i > 10 && i < 245) {
            C2V(gSSDX_system_palette)[i].peFlags = 0;
        } else {
            C2V(gSSDX_system_palette)[i].peFlags = PC_NOCOLLAPSE;
        }
    }
    dr_dprintf("LocalWindowedDDSetup(): Calling CreatePalette()...");
    res = IDirectDraw_CreatePalette(C2V(gDirectDraw), DDPCAPS_ALLOW256 | DDPCAPS_8BIT, C2V(gSSDX_system_palette), &C2V(gDirectDrawPalette), NULL);
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }

    dr_dprintf("LocalWindowedDDSetup(): Calling SetPalette()...");
    res = IDirectDrawSurface_SetPalette(C2V(gPrimary_surface), C2V(gDirectDrawPalette));
    if (res != DD_OK) {
        SSDXLogError(res);
        goto post_creation;
    }
post_creation:
    SSDXLockAttachedSurface();
    if (C2V(gAttached_surface_locked)) {
        res = IDirectDrawSurface_Unlock(C2V(gAttached_surface), C2V(gSSDXLockedRect));
        if (res == DD_OK) {
            C2V(gAttached_surface_locked) = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(res);
        }
    }
    *pPitch = C2V(gSSDXPitch);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005000a0, LocalWindowedDDSetup, LocalWindowedDDSetup_original)

void C2_HOOK_FASTCALL SSDXLogError(HRESULT hRes) {
#define LOG_CASE_DDERR(V) case V: dr_dprintf("%s (%x)", #V, V); break
    switch (hRes) {
        LOG_CASE_DDERR(DDERR_GENERIC);
        LOG_CASE_DDERR(DDERR_UNSUPPORTED);
        LOG_CASE_DDERR(DDERR_INVALIDPARAMS);
        LOG_CASE_DDERR(DDERR_OUTOFMEMORY);
        LOG_CASE_DDERR(DDERR_INVALIDCAPS);
        LOG_CASE_DDERR(DDERR_INCOMPATIBLEPRIMARY);
        LOG_CASE_DDERR(DDERR_INVALIDOBJECT);
        LOG_CASE_DDERR(DDERR_INVALIDMODE);
        LOG_CASE_DDERR(DDERR_INVALIDRECT);
        LOG_CASE_DDERR(DDERR_INVALIDPIXELFORMAT);
        LOG_CASE_DDERR(DDERR_SURFACEBUSY);
        LOG_CASE_DDERR(DDERR_NOEXCLUSIVEMODE);
        LOG_CASE_DDERR(DDERR_WASSTILLDRAWING);
        LOG_CASE_DDERR(DDERR_SURFACELOST);
        LOG_CASE_DDERR(DDERR_WRONGMODE);
        LOG_CASE_DDERR(DDERR_IMPLICITLYCREATED);
        LOG_CASE_DDERR(DD_OK);
        default: dr_dprintf("UNKNOWN DD ERROR (%x) N.B. Look for this number in 'ddraw.h': %d", hRes, hRes);
    }
#undef LOG_CASE_DDERR
}
C2_HOOK_FUNCTION(0x005002d0, SSDXLogError)

void C2_HOOK_FASTCALL SSDXStart(HWND p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {
    HRESULT hRes;

    hRes = 0;
    dr_dprintf("SSDXStart(): START...");
    if (p_EnumerateDDX) {
        C2V(gEnumerate_DirectX_surfaces) = 1;
        if (p_DirectDraw) {
            C2V(gEnumerate_DirectX_surfaces) = 0;
        }
    } else {
        C2V(gEnumerate_DirectX_surfaces) = 0;
    }
    C2V(gUse_DirectDraw) = p_DirectDraw;
    C2V(gHWnd_SSDX) = p_hWnd;
    if (p_flags & 0x2) {
        dr_dprintf("SSDXStart(): Calling DirectSoundCreate()...");
        hRes = DirectSoundCreate(0, &C2V(gDirectSound), NULL);
    }
    if (hRes != DS_OK) {
        SSDXLogError(hRes);
    }
    dr_dprintf("SSDXStart(): END.");
}
C2_HOOK_FUNCTION(0x00500500, SSDXStart)

void (C2_HOOK_FASTCALL * SSDXStop_original)(void);
void C2_HOOK_FASTCALL SSDXStop(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXStop_original();
#else

    SSDXUnlockAttachedSurface();

    if (C2V(gDirectDrawPalette) != NULL) {

        dr_dprintf("SSDXStop(): Releasing palette...");
        IDirectDrawPalette_Release(C2V(gDirectDrawPalette));
        C2V(gDirectDrawPalette) = NULL;
    }

    if (C2V(gDirectDrawClipper) != NULL) {

        dr_dprintf("SSDXStop(): Releasing clipper...");
        IDirectDrawClipper_Release(C2V(gDirectDrawClipper));
        C2V(gDirectDrawClipper) = NULL;
    }

    if (C2V(gUse_DirectDraw)) {

        if (C2V(gAttached_surface) != NULL) {
            dr_dprintf("SSDXStop(): Releaseing attached surface...");
            IDirectDrawSurface_Release(C2V(gAttached_surface));
            C2V(gAttached_surface) = NULL;
        }
        if (C2V(gPrimary_surface) != NULL) {
            dr_dprintf("SSDXStop(): Releaseing primary surface...");
            IDirectDrawSurface_Release(C2V(gPrimary_surface));
            C2V(gPrimary_surface) = NULL;
        }
    } else {

        if (C2V(gDirectDraw2) != NULL) {
            dr_dprintf("SSDXStop(): Restoring display mode");
            IDirectDraw2_RestoreDisplayMode(C2V(gDirectDraw2));
        }
        if (C2V(gPrimary_surface) != NULL) {
            dr_dprintf("SSDXStop(): Releaseing primary surface...");
            IDirectDrawSurface_Release(C2V(gPrimary_surface));
            C2V(gAttached_surface) = NULL;
            C2V(gPrimary_surface) = NULL;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00500590, SSDXStop, SSDXStop_original)

void (C2_HOOK_FASTCALL * SSDXRelease_original)(void);
void C2_HOOK_FASTCALL SSDXRelease(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXRelease_original();
#else

    if (C2V(gDirectDraw2) != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw2 interface...");
        IDirectDraw2_Release(C2V(gDirectDraw2));
        C2V(gDirectDraw2) = NULL;
    }
    if (C2V(gDirectDraw) != NULL) {
        dr_dprintf("SSDXStop(): Releasing DDraw object...");
        IDirectDraw_Release(C2V(gDirectDraw));
        C2V(gDirectDraw) = NULL;
    }
    if (C2V(gDirectSound) != NULL) {
        dr_dprintf("SSDXStop(): Releasing DSound object...");
        IDirectSound_Release(C2V(gDirectSound));
        C2V(gDirectSound) = NULL;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005006d0, SSDXRelease, SSDXRelease_original)

void (C2_HOOK_FASTCALL * SSDXDirectDrawSetup_original)(int pWidth, int pHeight, int pBits, int* pPitch);
void C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXDirectDrawSetup_original(pWidth, pHeight, pBits, pPitch);
#else
    HRESULT result;

    if (C2V(gUse_DirectDraw)) {
        LocalWindowedDDSetup(pWidth, pHeight, pPitch);
    }
    dr_dprintf("SSDXDirectDrawSetup(): Calling SetDisplayMode( %dx%dx%d )...", pWidth, pHeight, pBits);
    result = IDirectDraw2_SetDisplayMode(C2V(gDirectDraw2), pWidth, pHeight, pBits, 0, !C2V(gEnumerate_DirectX_surfaces) && pWidth == 320 ? DDSDM_STANDARDVGAMODE : 0);
    SetRect(&C2V(gSSDXWindowPos), 0, 0, pWidth, pHeight);
    if (result == DD_OK) {
        DDSURFACEDESC surfaceDesc;

        C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
        c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
        surfaceDesc.dwSize = sizeof(surfaceDesc);
        surfaceDesc.dwFlags = DDSD_CAPS | (C2V(gEnumerate_DirectX_surfaces) ? DDSD_BACKBUFFERCOUNT : 0);
        surfaceDesc.dwBackBufferCount = C2V(gEnumerate_DirectX_surfaces) ? 1 : 0;
        surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_PRIMARYSURFACE | (C2V(gEnumerate_DirectX_surfaces) ? DDSCAPS_FLIP | DDSCAPS_COMPLEX : 0);
        dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface()...");
        result = IDirectDraw_CreateSurface(C2V(gDirectDraw), &surfaceDesc, &C2V(gPrimary_surface), NULL);
        if (result == DD_OK) {

            if (C2V(gEnumerate_DirectX_surfaces)) {
                int enum_counter;

                dr_dprintf("SSDXDirectDrawSetup(): Calling EnumAttachedSurfaces()...");
                result = IDirectDrawSurface_EnumAttachedSurfaces(C2V(gPrimary_surface), &enum_counter, LocalEnumAttachedSurfacesCallback);
                dr_dprintf("SSDXDirectDrawSetup(): enum_counter is %d", enum_counter);
            } else {

                C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
                SetRect(&C2V(gSSDXRect), 0, 0, pWidth, pHeight);
                c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
                surfaceDesc.dwSize = sizeof(surfaceDesc);
                surfaceDesc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
                surfaceDesc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
                surfaceDesc.dwHeight = pHeight;
                surfaceDesc.dwWidth = pWidth;

                dr_dprintf("SSDXDirectDrawSetup(): Calling CreateSurface() for attached surface...");
                result = IDirectDraw_CreateSurface(C2V(gDirectDraw), &surfaceDesc, &C2V(gAttached_surface), NULL);
            }
            if (result == DD_OK) {
                PALETTEENTRY palette[256];

                c2_memset(palette, 0, sizeof(palette));
                result = IDirectDraw_CreatePalette(C2V(gDirectDraw), DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette, &C2V(gDirectDrawPalette), NULL);
                if (result == DD_OK) {
                    dr_dprintf("SSDXDirectDrawSetup(): Calling SetPalette");
                    result = IDirectDrawSurface_SetPalette(C2V(gPrimary_surface), C2V(gDirectDrawPalette));
                }
            }
        }
    }
    SSDXLockAttachedSurface();
    SSDXUnlockAttachedSurface();
    *pPitch = C2V(gSSDXPitch);
    if (result == DD_OK) {
        dr_dprintf("SSDXDirectDrawSetup(): Directdraw initialised: surface pith %d", C2V(gSSDXPitch));
    } else {
        dr_dprintf("WARNING: Problems during SSDXDirectDrawSetup()");
        SSDXLogError(result);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00500760, SSDXDirectDrawSetup, SSDXDirectDrawSetup_original)

void (C2_HOOK_FASTCALL * SSDXLockAttachedSurface_original)(void);
void C2_HOOK_FASTCALL SSDXLockAttachedSurface(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXLockAttachedSurface_original();
#else
    HRESULT result;
    DDSURFACEDESC surfaceDesc;

    if (!C2V(gAttached_surface_locked)) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but surface already locked");
        goto error;
    }
    if (C2V(gAttached_surface) == NULL) {
        goto error;
    }
    result = IDirectDrawSurface_IsLost(C2V(gPrimary_surface));
    if (result == DDERR_SURFACELOST) {
        dr_dprintf("SSDXLockAttachedSurface(): Primary surface is lost, restoring...");
        result = IDirectDrawSurface_Restore(C2V(gPrimary_surface));
    }
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    result = IDirectDrawSurface_IsLost(C2V(gAttached_surface));
    if (result == DDERR_SURFACELOST) {
        dr_dprintf("SSDXLockAttachedSurface(): Attached surface is lost, restoring...");
        result = IDirectDrawSurface_Restore(C2V(gAttached_surface));
    }
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    C2_HOOK_BUG_ON(sizeof(surfaceDesc) != 0x6c);
    c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
    surfaceDesc.dwSize = sizeof(surfaceDesc);
    result = IDirectDrawSurface_Lock(C2V(gAttached_surface), NULL, &surfaceDesc, DDLOCK_WAIT, NULL);
    if (result != DD_OK) {
        SSDXLogError(result);
        goto error;
    }
    C2V(gAttached_surface_locked) = 1;
    C2V(gSSDXLockedRect) = surfaceDesc.lpSurface;
    C2V(gSSDXPitch) = surfaceDesc.lPitch;
    return;
error:
    if (C2V(gAttached_surface) == NULL) {
        dr_dprintf("WARNING: SSDXLockAttachedSurface() called but no attached surface available");
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00500a30, SSDXLockAttachedSurface, SSDXLockAttachedSurface_original)

void C2_HOOK_FASTCALL SSDXUnlockAttachedSurface(void) {

    if (C2V(gAttached_surface_locked)) {
        HRESULT result = IDirectDrawSurface_Unlock(C2V(gAttached_surface), C2V(gSSDXLockedRect));
        if (result == DD_OK) {
            C2V(gAttached_surface_locked) = 0;
        } else {
            dr_dprintf("SSDXUnlockAttachedSurface(): Problem while unlocking attached surface...");
            SSDXLogError(result);
        }
    }
}
C2_HOOK_FUNCTION(0x00500b40, SSDXUnlockAttachedSurface)

void C2_HOOK_FASTCALL SSDXBlitOrFlip(void) {

    if (C2V(gUse_DirectDraw)) {
        SSDXBlit();
    } else {
        IDirectDrawSurface_Flip(C2V(gPrimary_surface), NULL, DDFLIP_WAIT);
    }
}
C2_HOOK_FUNCTION(0x00500b90, SSDXBlitOrFlip)

void C2_HOOK_FASTCALL SSDXBlit(void) {

    IDirectDrawSurface_Blt(C2V(gPrimary_surface), &C2V(gSSDXWindowPos), C2V(gAttached_surface), &C2V(gSSDXRect), DDBLT_WAIT, NULL);
}
C2_HOOK_FUNCTION(0x00500bd0, SSDXBlit)

void C2_HOOK_FASTCALL SSDXSetPaletteEntries(PALETTEENTRY* pPalette, int pStart, int pCount) {

    if (C2V(gUse_DirectDraw) && C2V(gDirectDrawPalette) != NULL) {
        int i;

        for (i = MAX(10, pStart); i < MIN(pStart + pCount - 1, 0xf6); i++) {

            C2V(gSSDX_system_palette)[i].peRed = pPalette[i].peRed;
            C2V(gSSDX_system_palette)[i].peGreen = pPalette[i].peGreen;
            C2V(gSSDX_system_palette)[i].peBlue = pPalette[i].peBlue;
        }
        IDirectDrawPalette_SetEntries(C2V(gDirectDrawPalette), 0, pStart, pCount, C2V(gSSDX_system_palette));
    } else {
        IDirectDrawPalette_SetEntries(C2V(gDirectDrawPalette), 0, pStart, pCount, pPalette);
    }
}
C2_HOOK_FUNCTION(0x00500c00, SSDXSetPaletteEntries)

void C2_HOOK_FASTCALL SSDXSetPrimaryPalette(void) {
    HRESULT result;

    if (!C2V(gUse_DirectDraw)) {
        return;
    }
    if (C2V(gDirectDrawPalette) == NULL) {
        return;
    }
    if (C2V(gPrimary_surface) == NULL) {
        return;
    }
    result = IDirectDrawSurface_SetPalette(C2V(gPrimary_surface), C2V(gDirectDrawPalette));
    if (result == DDERR_SURFACELOST) {
        IDirectDrawSurface_Restore(C2V(gPrimary_surface));
        IDirectDrawSurface_SetPalette(C2V(gPrimary_surface), C2V(gDirectDrawPalette));
    }
}
C2_HOOK_FUNCTION(0x00500ca0, SSDXSetPrimaryPalette)

void C2_HOOK_FASTCALL SSDXWindowPosOrSizeChanged(HWND pHwnd) {

    if (C2V(gUse_DirectDraw)) {
        GetClientRect(pHwnd, &C2V(gSSDXWindowPos));
        ClientToScreen(pHwnd, (POINT*)&C2V(gSSDXWindowPos).left);
        ClientToScreen(pHwnd, (POINT*)&C2V(gSSDXWindowPos).right);
        dr_dprintf("SSDXWindowPosOrSizeChanged(): New window rect: (%d,%d)(%d,%d)",
            C2V(gSSDXWindowPos).left, C2V(gSSDXWindowPos).top, C2V(gSSDXWindowPos).right, C2V(gSSDXWindowPos).bottom);
    }
}
C2_HOOK_FUNCTION(0x00500cf0, SSDXWindowPosOrSizeChanged)

int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(C2V(gPD_S3_config)) != 0x20);

    c2_strcpy(C2V(gS3_path_separator), "\\");
    c2_strcpy(C2V(gS3_sound_folder_name), "SOUND");
    c2_memset(&C2V(gPD_S3_config), 0, sizeof(C2V(gPD_S3_config)));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    // nop_FUN_005699a3();
    C2V(gPD_S3_config).field_0x04 = 0;
    C2V(gPD_S3_config).free_direct_sound = 1;
    C2V(gPD_S3_config).independent_pitch = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0056992f, PDS3Init)

int C2_HOOK_FASTCALL PDS3DDXInit(void) {
    int i;

    if (FAILED(DirectSoundCreate(0, &C2V(gPD_S3_direct_sound), NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(C2V(gPD_S3_direct_sound), C2V(gHWnd), DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < REC2_ASIZE(C2V(gPDS3_volume_factors)); i++) {
        float f;
        f = ((float)i + 1.f) * 999.f / 256.f;
        C2V(gPDS3_volume_factors)[i] = (int)((f * 9999.f / 3.f - 4300.f) - 9999.f);
    }
    S3Enable();
    return 1;
}
C2_HOOK_FUNCTION(0x00569a2c, PDS3DDXInit)

int (C2_HOOK_FASTCALL * PDS3InitCDA_original)(void);
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDS3InitCDA_original();
#else
    MCIERROR err;
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&C2V(gPDS3_mci_open_parms));
    if (err != 0) {
        err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&C2V(gPDS3_mci_open_parms));
        if (err != 0) {
            return 0;
        }
    }
    C2V(gPDS3_mci_set_parms).dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&C2V(gPDS3_mci_set_parms));
    S3EnableCDA();
    C2V(gPDS3_cda_initialized) = 1;
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005699aa, PDS3InitCDA, PDS3InitCDA_original)

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC buffer_desc;
    void* buffer_data;
    DWORD buffer_data_size;

    C2_HOOK_BUG_ON(sizeof(DSBUFFERDESC) != 0x14);
    c2_memset(&buffer_desc, 0, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
    buffer_desc.dwBufferBytes = pWav_info->sample_size;
    buffer_desc.lpwfxFormat = (LPWAVEFORMATEX)pWav_info->wav_info_header;
    if (FAILED(IDirectSound_CreateSoundBuffer(C2V(gPD_S3_direct_sound), &buffer_desc, &buffer, NULL))) {
        return NULL;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, pWav_info->sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        IDirectSoundBuffer_Release(buffer);
        return NULL;
    }
    c2_memmove(buffer_data, pWav_info->samples, buffer_data_size);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return buffer;
}

void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {

    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}
C2_HOOK_FUNCTION(0x005693d9, PDS3ReleaseSound)

int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel) {
    tU8* buffer_data;
    DWORD buffer_data_size;
    DWORD i;
    tU8* tmp_buffer;
    DWORD sample_size;

    LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
    if (buffer == NULL) {
        return 1;
    }
    sample_size = pChannel->descriptor->buffer_description->sample_size;
    if (sample_size > 2000000) {
        return 0;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, sample_size, (void**)&buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        return 0;
    }
    tmp_buffer = BrMemAllocate(sample_size, kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample);
    if (tmp_buffer == NULL) {
        IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
        return 1;
    }
    c2_memmove(tmp_buffer, buffer_data, buffer_data_size);
    for (i = 0; i < buffer_data_size; i++) {
        buffer_data[i] = tmp_buffer[sample_size - 1 - i];
    }
    BrMemFree(tmp_buffer);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return 0;
}
C2_HOOK_FUNCTION(0x00569401, PDReverseAudio)

char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!C2V(gS3_working_directory_initialized)) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(C2V(gS3_working_directory)) != 0x104);

        C2V(gS3_working_directory)[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(C2V(gS3_working_directory)), C2V(gS3_working_directory));
        len = c2_strlen(C2V(gS3_working_directory));
        if (len_res != len) {
            C2V(gS3_working_directory)[0] = '\0';
        }
        C2V(gS3_working_directory_initialized) = 1;
    }
    return C2V(gS3_working_directory);
}
C2_HOOK_FUNCTION(0x00569b30, PDS3GetWorkingDirectory)

int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {

    if C2V(gPDS3_midi_playing) {
        C2V(gPDS3_midi_playing) = 0;
        mciSendCommandA(pChannel->mciDevice, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00569cb1, PDS3StopMidiChannel)

void (C2_HOOK_FASTCALL * PDS3CheckCDAMedia_original)(tS3_channel* pChannel);
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDS3CheckCDAMedia_original(pChannel);
#else
    C2V(gPDS3_cda_status_parms).dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&C2V(gPDS3_cda_status_parms));
    if (!C2V(gPDS3_cda_media_present) && C2V(gPDS3_cda_status_parms).dwReturn) {
        C2V(gPDS3_cda_media_present) = 1;
        C2V(gPDS3_cda_paused) = 0;
        C2V(gPDS3_cda_track) = 0;
        C2V(gPDS3_cda_status_parms).dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&C2V(gPDS3_cda_status_parms));
        C2V(gPDS3_Number_of_tracks) = MIN(C2V(gPDS3_cda_status_parms).dwReturn, 99);
    } else if (C2V(gPDS3_cda_media_present) && !C2V(gPDS3_cda_status_parms).dwReturn) {
        C2V(gPDS3_cda_media_present) = 0;
        C2V(gPDS3_cda_paused) = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056a0e1, PDS3CheckCDAMedia, PDS3CheckCDAMedia_original)

int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {
    PDS3CheckCDAMedia(pChannel);
    if (C2V(gPDS3_cda_media_present)) {
        C2V(gPDS3_cda_paused) = 0;
        C2V(gPDS3_cda_track) = 0;
        mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_STOP, 0, 0);
    }
    C2V(gPDS3_cda_is_playing) = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x0056a093, PDS3StopCDAChannel)

int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type && pChannel->descriptor->pd_handle != NULL) {
        LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
        IDirectSoundBuffer_Stop(buffer);
        if (C2V(gS3_callbacks).on_sample_channel_stop != NULL && pChannel->active) {
            C2V(gS3_callbacks).on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x005697ef, PDS3StopSampleChannel)

int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel) {

    if (pChannel->type == 0) {
        if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
            LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;

            if (buffer != NULL) {
                int volume = (int)(pChannel->owner_outlet->volume / 255.f * pChannel->volume_multiplier * 11.76470588235294f);
                int ds_volume = volume - 3000;
                ds_volume = MAX(ds_volume, -9999);
                ds_volume = MIN(ds_volume, 0);

                if (SUCCEEDED(IDirectSoundBuffer_SetVolume(buffer, ds_volume))) {
                    if (C2V(gS3_callbacks).on_sample_channel_set_volume != NULL) {
                        C2V(gS3_callbacks).on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (C2V(gS3_callbacks).on_sample_channel_set_pan != NULL) {
                            C2V(gS3_callbacks).on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
                        }
                        IDirectSoundBuffer_SetPan(buffer, pan);
                    }
                }
            }
        }
    } else if (pChannel->type == 1) {

    } else if (pChannel->type == 2) {
        PDS3UpdateCDAVolume(pChannel, pChannel->owner_outlet->volume);
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00569507, PDS3UpdateChannelVolume)

void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {

}
C2_HOOK_FUNCTION(0x00568ed0, PDS3UpdateCDAVolume)

void C2_HOOK_FASTCALL PDS3Stop(void) {

    if (C2V(gPD_S3_config).free_direct_sound) {

        IDirectSound_Release(C2V(gPD_S3_direct_sound));
        C2V(gPD_S3_direct_sound) = NULL;
    }
    if (C2V(gPDS3_mci_open_parms).wDeviceID != 0) {
        mciSendCommandA(C2V(gPDS3_mci_open_parms).wDeviceID, MCI_CLOSE, 0, 0);
    }
}
C2_HOOK_FUNCTION(0x00569ae2, PDS3Stop)

int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel) {

    C2V(gPDS3_mci_midi_status_parms).dwItem = MCI_STATUS_MODE;
    mciSendCommandA(pChannel->mciDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&C2V(gPDS3_mci_midi_status_parms));
    return C2V(gPDS3_mci_midi_status_parms).dwReturn != MCI_MODE_PLAY;
}
C2_HOOK_FUNCTION(0x00569f1b, PDS3IsMIDIStopped)

int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void) {

    return C2V(gS3_CDA_enabled) && C2V(gPDS3_cda_is_playing) && C2V(gPDS3_cda_media_present);
}
C2_HOOK_FUNCTION(0x0056a260, PDS3IsCDAPlaying)

int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel) {
    DWORD status;

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        LPDIRECTSOUNDBUFFER buffer;

        buffer = pChannel->descriptor->pd_handle;
        if (buffer != NULL) {
            if (!FAILED(IDirectSoundBuffer_GetStatus(buffer, &status))) {
                if (status & DSBSTATUS_PLAYING) {
                    return 1;
                }
            }
        }
    }
    PDS3StopSampleChannel(pChannel);
    return 0;
}
