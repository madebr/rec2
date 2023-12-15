#include "win32_ssdx.h"

#include "errors.h"
#include "globvars.h"
#include "graphics.h"

#include <ddraw.h>

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTSOUND, gDirectSound, 0x006aaa1c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gUse_DirectDraw, 0x006aa9e0);
C2_HOOK_VARIABLE_IMPLEMENT(HWND, gHWnd_SSDX, 0x006aaa08);
C2_HOOK_VARIABLE_IMPLEMENT(int, gEnumerate_DirectX_surfaces, 0x006aa9d8);

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
    if (hRes != DD_OK) {
        SSDXLogError(hRes);
    }
    dr_dprintf("SSDXStart(): END.");
}
C2_HOOK_FUNCTION(0x00500500, SSDXStart)
