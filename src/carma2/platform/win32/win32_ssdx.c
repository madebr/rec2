#include "win32_ssdx.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"

#include "platform.h"

#include <ddraw.h>

#include "c2_stdlib.h"
#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x006aaa1c
LPDIRECTSOUND gDirectSound;

// GLOBAL: CARMA2_HW 0x006b2d9c
LPDIRECTSOUND gPD_S3_direct_sound;

// GLOBAL: CARMA2_HW 0x006aa9e0
int gUse_DirectDraw;

// GLOBAL: CARMA2_HW 0x006aaa08
HWND gHWnd_SSDX;

// GLOBAL: CARMA2_HW 0x006aa9d8
int gEnumerate_DirectX_surfaces;

// GLOBAL: CARMA2_HW 0x006aaa10
int gSSDXPitch;

// GLOBAL: CARMA2_HW 0x007a00e0
int gPDS3_volume_factors[256];

// GLOBAL: CARMA2_HW 0x007a0080
tPD_S3_config gPD_S3_config;

// GLOBAL: CARMA2_HW 0x007a0500
MCI_OPEN_PARMS gPDS3_mci_open_parms;

// GLOBAL: CARMA2_HW 0x0079fec0
MCI_SET_PARMS gPDS3_mci_set_parms;

// GLOBAL: CARMA2_HW 0x006b2d98
int gPDS3_cda_initialized;

// GLOBAL: CARMA2_HW 0x006b2da0
int gS3_working_directory_initialized;

// GLOBAL: CARMA2_HW 0x006b2c90
char gS3_working_directory[260];

// GLOBAL: CARMA2_HW 0x006b2da4
int gPDS3_midi_playing;

// GLOBAL: CARMA2_HW 0x006b2dac
int gPDS3_cda_media_present;

// GLOBAL: CARMA2_HW 0x006b2db0
int gPDS3_cda_is_playing;

// GLOBAL: CARMA2_HW 0x006b2db4
int gPDS3_cda_paused;

// GLOBAL: CARMA2_HW 0x006b2db8
int gPDS3_cda_track;

// GLOBAL: CARMA2_HW 0x007a0528
MCI_STATUS_PARMS gPDS3_cda_status_parms;

// GLOBAL: CARMA2_HW 0x007a00ac
int gPDS3_Number_of_tracks;

// GLOBAL: CARMA2_HW 0x007a0070
MCI_STATUS_PARMS gPDS3_mci_midi_status_parms;

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

// GLOBAL: CARMA2_HW 0x006b2dc4
tU32 gPDS3_last_cda_status_update;


// FUNCTION: CARMA2_HW 0x00500070
HRESULT CALLBACK LocalEnumAttachedSurfacesCallback(LPDIRECTDRAWSURFACE lpSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID lpContext) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LocalEnumAttachedSurfacesCallback_original(lpSurface, lpSurfaceDesc, lpContext);
#else
    int* counter = lpContext;
    dr_dprintf("LocalEnumAttachedSurfacesCallback(): Enum-ing attached surface #%d, address 0x%p", *counter, lpSurface);
    gAttached_surface = lpSurface;
    return DDENUMRET_OK;
#endif
}

// FUNCTION: CARMA2_HW 0x005000a0
void C2_HOOK_FASTCALL LocalWindowedDDSetup(int pWidth, int pHeight, int* pPitch) {

#if 0//defined(C2_HOOKS_ENABLED)
    return LocalWindowedDDSetup_original(pWidth, pHeight, pArg3);
#else
    DDSURFACEDESC desc;
    HRESULT res;
    HDC hdc;
    int i;

    SetRect(&gSSDXRect, 0, 0, pWidth, pHeight);

    c2_memset(&desc, 0, sizeof(desc));
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

    c2_memset(&desc, 0, sizeof(desc));
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
#endif
}

// FUNCTION: CARMA2_HW 0x005002d0
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

// FUNCTION: CARMA2_HW 0x00500500
void C2_HOOK_FASTCALL SSDXStart(void *p_hWnd, int p_DirectDraw, int p_EnumerateDDX, int p_flags) {
    HRESULT hRes;

    hRes = 0;
    dr_dprintf("SSDXStart(): START...");
    if (p_EnumerateDDX) {
        gEnumerate_DirectX_surfaces = 1;
        if (p_DirectDraw) {
            gEnumerate_DirectX_surfaces = 0;
        }
    } else {
        gEnumerate_DirectX_surfaces = 0;
    }
    gUse_DirectDraw = p_DirectDraw;
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
}

// FUNCTION: CARMA2_HW 0x00500590
void C2_HOOK_FASTCALL SSDXStop(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXStop_original();
#else

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
#endif
}

// FUNCTION: CARMA2_HW 0x005006d0
void C2_HOOK_FASTCALL SSDXRelease(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXRelease_original();
#else

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
#endif
}

// FUNCTION: CARMA2_HW 0x00500760
void C2_HOOK_FASTCALL SSDXDirectDrawSetup(int pWidth, int pHeight, int pBits, int* pPitch) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXDirectDrawSetup_original(pWidth, pHeight, pBits, pPitch);
#else
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
        c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
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
                c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
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

                c2_memset(palette, 0, sizeof(palette));
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
#endif
}

// FUNCTION: CARMA2_HW 0x00500a30
void C2_HOOK_FASTCALL SSDXLockAttachedSurface(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    SSDXLockAttachedSurface_original();
#else
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
    c2_memset(&surfaceDesc, 0, sizeof(surfaceDesc));
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
#endif
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

        for (i = MAX(10, pStart); i < MIN(pStart + pCount - 1, 0xf6); i++) {

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

// FUNCTION: CARMA2_HW 0x0056992f
int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(gPD_S3_config) != 0x20);

    c2_strcpy(gS3_path_separator, "\\");
    c2_strcpy(gS3_sound_folder_name, "SOUND");
    c2_memset(&gPD_S3_config, 0, sizeof(gPD_S3_config));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    // nop_FUN_005699a3();
    gPD_S3_config.field_0x04 = 0;
    gPD_S3_config.free_direct_sound = 1;
    gPD_S3_config.independent_pitch = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569a2c
int C2_HOOK_FASTCALL PDS3DDXInit(void) {
#ifdef DSOUND_ENABLED
    int i;

    if (FAILED(DirectSoundCreate(0, &gPD_S3_direct_sound, NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(gPD_S3_direct_sound, gHWnd, DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < REC2_ASIZE(gPDS3_volume_factors); i++) {
        float f;
        f = ((float)i + 1.f) * 999.f / 256.f;
        gPDS3_volume_factors[i] = (int)((f * 9999.f / 3.f - 4300.f) - 9999.f);
    }
    S3Enable();
    return 1;
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x005699aa
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return PDS3InitCDA_original();
#else
    MCIERROR err;
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&gPDS3_mci_open_parms);
    if (err != 0) {
        err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&gPDS3_mci_open_parms);
        if (err != 0) {
            return 0;
        }
    }
    gPDS3_mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&gPDS3_mci_set_parms);
    S3EnableCDA();
    gPDS3_cda_initialized = 1;
    return 1;
#endif
}

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
    if (FAILED(IDirectSound_CreateSoundBuffer(gPD_S3_direct_sound, &buffer_desc, &buffer, NULL))) {
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

// FUNCTION: CARMA2_HW 0x005693d9
void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {

    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}

// FUNCTION: CARMA2_HW 0x00569401
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

// FUNCTION: CARMA2_HW 0x00569b30
const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!gS3_working_directory_initialized) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(gS3_working_directory) != 0x104);

        gS3_working_directory[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(gS3_working_directory), gS3_working_directory);
        len = c2_strlen(gS3_working_directory);
        if (len_res != len) {
            gS3_working_directory[0] = '\0';
        }
        gS3_working_directory_initialized = 1;
    }
    return gS3_working_directory;
}

// FUNCTION: CARMA2_HW 0x00569cb1
int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {

    if (gPDS3_midi_playing) {
        gPDS3_midi_playing = 0;
        mciSendCommandA(pChannel->mciDevice, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a0e1
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(tS3_channel* pChannel) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDS3CheckCDAMedia_original(pChannel);
#else
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (!gPDS3_cda_media_present && gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 1;
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        gPDS3_Number_of_tracks = MIN(gPDS3_cda_status_parms.dwReturn, 99);
    } else if (gPDS3_cda_media_present && !gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 0;
        gPDS3_cda_paused = 0;
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0056a093
int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {
    PDS3CheckCDAMedia(pChannel);
    if (gPDS3_cda_media_present) {
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STOP, 0, 0);
    }
    gPDS3_cda_is_playing = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005697ef
int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type && pChannel->descriptor->pd_handle != NULL) {
        LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;
        IDirectSoundBuffer_Stop(buffer);
        if (gS3_callbacks.on_sample_channel_stop != NULL && pChannel->active) {
            gS3_callbacks.on_sample_channel_stop(pChannel);
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569507
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
                    if (gS3_callbacks.on_sample_channel_set_volume != NULL) {
                        gS3_callbacks.on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (gS3_callbacks.on_sample_channel_set_pan != NULL) {
                            gS3_callbacks.on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
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

// FUNCTION: CARMA2_HW 0x00568ed0
void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {

}

// FUNCTION: CARMA2_HW 0x00569ae2
void C2_HOOK_FASTCALL PDS3Stop(void) {

    if (gPD_S3_config.free_direct_sound) {

        IDirectSound_Release(gPD_S3_direct_sound);
        gPD_S3_direct_sound = NULL;
    }
    if (gPDS3_mci_open_parms.wDeviceID != 0) {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_CLOSE, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00569f1b
int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel) {

    gPDS3_mci_midi_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(pChannel->mciDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    return gPDS3_mci_midi_status_parms.dwReturn != MCI_MODE_PLAY;
}

// FUNCTION: CARMA2_HW 0x0056a260
int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void) {

    return gS3_CDA_enabled && gPDS3_cda_is_playing && gPDS3_cda_media_present;
}

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

// FUNCTION: CARMA2_HW 0x0056967e
int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel) {

    if (pChannel->type == 0 && pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer* sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            IDirectSoundBuffer_SetFrequency(sound_buffer, MIN(pChannel->rate, 100000));
        }
    }
    return 1;
}

int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer *sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            int looping = pChannel->repetitions == 0;
            IDirectSoundBuffer_Play(sound_buffer, 0, 0, looping ? DSBPLAY_LOOPING : 0);
            if (!looping) {
                IDirectSoundBuffer_SetCurrentPosition(sound_buffer, 0);
            }
        }
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569b93
tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel) {
    char path[256];
    MCIERROR err;

    path[0] = '\0';
    c2_strcpy(path, pChannel->descriptor->path);
    c2_strcat(path, ".MID");
    mci_open_params.lpstrElementName = path;
    mci_open_params.lpstrDeviceType = "sequencer";
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_open_params);
    MCIDEVICEID device = mci_open_params.wDeviceID;
    if (err != 0) {
        return eS3_error_start_song;
    }
    gPDS3_mci_midi_status_parms.dwItem = MCI_VD_STATUS_FORWARD;
    err = mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    err = mciSendCommandA(device, MCI_PLAY, 0, (DWORD_PTR)&mci_play_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    pChannel->mciDevice = device;
    gPDS3_midi_playing = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00569f92
tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel) {
    char* endptr;

    PDS3CheckCDAMedia(pChannel);
    if (!gPDS3_cda_media_present) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_paused = 0;
    gPDS3_cda_track = c2_strtoul(pChannel->descriptor->path, &endptr, 10);
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (gPDS3_cda_status_parms.dwReturn == MCI_MODE_NOT_READY
            || gPDS3_cda_status_parms.dwReturn == MCI_MODE_OPEN) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_play_parms.dwFrom = gPDS3_cda_track % 256;
    if (gPDS3_cda_track + 1 < gPDS3_Number_of_tracks) {
        gPDS3_cda_play_parms.dwTo = (gPDS3_cda_track + 1) % 256;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD_PTR)&gPDS3_cda_play_parms);
    } else {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM, (DWORD_PTR)&gPDS3_cda_play_parms);
    }
    gPDS3_cda_is_playing = 1;
    return eS3_error_none;
}

void C2_HOOK_FASTCALL PDS3ServiceCDA(void) {
    tU32 now;

    if (!gPDS3_cda_initialized) {
        return;
    }
    now = PDGetTotalTime();
    if (now > gPDS3_last_cda_status_update + 3000) {
        gPDS3_cda_status_parms.dwCallback = (DWORD_PTR)gHWnd;
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
        gPDS3_last_cda_status_update = now;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_NOTIFY, (DWORD_PTR)&gPDS3_cda_status_parms);
        gPDS3_cda_is_playing = gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY;
    }
}
