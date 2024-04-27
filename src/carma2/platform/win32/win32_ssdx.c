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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gPDS3_volume_factors, 256, 0x007a00e0);
C2_HOOK_VARIABLE_IMPLEMENT(tPD_S3_config, gPD_S3_config, 0x007a0080);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_path_separator, 2, 0x007a0554);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gS3_sound_folder_name, 6, 0x007a0558);

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
    C2V(gPD_S3_config).field_0x1c = 0;
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

void* (C2_HOOK_FASTCALL * PDS3BufferWav_original)(const char* pPath, tS3_buffer_desc* pBuffer_desc);
void* C2_HOOK_FASTCALL PDS3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc) {

#if defined(C2_HOOKS_ENABLED)
    return PDS3BufferWav_original(pPath, pBuffer_desc);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0056907c, PDS3BufferWav, PDS3BufferWav_original)

int (C2_HOOK_FASTCALL * PDS3InitCDA_original)(void);
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

#if defined(C2_HOOKS_ENABLED)
    return PDS3InitCDA_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005699aa, PDS3InitCDA, PDS3InitCDA_original)
