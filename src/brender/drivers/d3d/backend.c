#define INITGUID

#include "backend.h"

#include <brender/brender.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <d3d.h>
#include <ddraw.h>

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
} tStruct_100385e0;

typedef HRESULT WINAPI tDirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
typedef HRESULT WINAPI tDirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext);

C2_HOOK_VARIABLE_IMPLEMENT(void*, gDevice_pixels, 0x1001bbac);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_uint_32, gPixel_buffer, 640 * 480, 0x10038a80);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tStruct_100385e0, gDAT_100385e0, 100, 0x100385e0);
C2_HOOK_VARIABLE_IMPLEMENT(HWND, gActive_window, 0x1001bbdc);
C2_HOOK_VARIABLE_IMPLEMENT(WNDPROC, gOriginal_wndproc, 0x10038a70);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWSURFACE, gPrimary_surface, 0x1001bc34);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWSURFACE, gAttached_surface, 0x1001bc3c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAWSURFACE, gDepth_surface, 0x1001bc44);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAW, gDirectDraw, 0x1001bc0c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTDRAW2, gDirectDraw2, 0x1001bc10);
C2_HOOK_VARIABLE_IMPLEMENT(HMODULE, gModule_ddraw, 0x100381a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_10038554, 0x10038554);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tStruct_1001dcc0, gBuffered_textures, 128, 0x1001dcc0); /* FIXME: unknown size */
C2_HOOK_VARIABLE_IMPLEMENT(tDirectDrawCreate*, gDirectDrawCreate, 0x10038900);
C2_HOOK_VARIABLE_IMPLEMENT(tDirectDrawEnumerateA*, gDirectDrawEnumerateA, 0x10038904);
C2_HOOK_VARIABLE_IMPLEMENT(int, gError_code, 0x1001bbec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRegister_driver_guid_binary, 0x10038000);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(DWORD, gRegister_flags, 0x10015890, 0xffffffff);
C2_HOOK_VARIABLE_IMPLEMENT(GUID, gRegister_device_guid, 0x10038540);
C2_HOOK_VARIABLE_IMPLEMENT(GUID, gRegister_driver_guid, 0x10038190);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRegister_guid_valid, 0x1001bc58);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_ddraw_drivers, 0x1001dc48);
C2_HOOK_VARIABLE_IMPLEMENT(int, gIndex_ddraw_driver, 0x1001dc3c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDDraw_driver_found, 0x1001bc5c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNo_2d_during_3d_scene, 0x1001bb9c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECT3D, gDirect3D, 0x1001bc14);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, g640x480x16_display_mode_flags, 0x1001dc64);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_display_modes, 0x10038a6c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSelected_display_mode, 0x1003890c);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECT3D2, gDirect3D2, 0x1001bc18);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_d3d_devices, 0x10038908);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSelected_d3d_device, 0x10038550);
C2_HOOK_VARIABLE_IMPLEMENT(DDPIXELFORMAT, gAttached_surface_pixel_format, 0x10038520);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPixel_format_is_xrgb1555, 0x1001bbb8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_width_10015888, 0x10015888);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_height_1001588c, 0x1001588c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_width_10037cc0, 0x10037cc0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_height_10037cc4, 0x10037cc4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_width_10038004, 0x10038004);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisplay_height_10038008, 0x10038008);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDevice_has_color_model, 0x10037cc8);
C2_HOOK_VARIABLE_IMPLEMENT(D3DDEVICEDESC, gDevice_description, 0x10038920);
C2_HOOK_VARIABLE_IMPLEMENT(GUID, gDevice_guid, 0x1001dca0);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECT3DDEVICE2, gD3DDevice2, 0x1001bc24);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECT3DVIEWPORT2, gD3D_Viewport, 0x1001bc48);
C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECT3DMATERIAL2, gViewport_background_material, 0x1001bc2c);
C2_HOOK_VARIABLE_IMPLEMENT(DWORD, gViewport_background_material_handle, 0x1003810c);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gAvailable_pixel_formats, 0x1001bc54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPreferred_texture_format, 0x1001dc60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gTexture_format_is_NOT_ARGB4444, 0x1001bbcc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPreferred_opaque_texture_format, 0x1003818c);
C2_HOOK_VARIABLE_IMPLEMENT(DDPIXELFORMAT, g640x480x16_pixelFormat, 0x1001dc80);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAttached_surface_pixelformat, 0x100156fc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFound_d3d_device, 0x1001bc60);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDisplay_center_x, 0x10038500);
C2_HOOK_VARIABLE_IMPLEMENT(float, gDisplay_center_y, 0x10038504);
C2_HOOK_VARIABLE_IMPLEMENT(DDSURFACEDESC, gPreferred_texture_surface_description, 0x10038120);
C2_HOOK_VARIABLE_IMPLEMENT(DDSURFACEDESC, gPreferred_opaque_texture_surface_description, 0x10038a00);
C2_HOOK_VARIABLE_IMPLEMENT(DDSURFACEDESC, gAvailable_RGB555_texture_format_description, 0x10038020);
C2_HOOK_VARIABLE_IMPLEMENT(DDSURFACEDESC, gAvailable_RGB565_texture_format_description, 0x100380a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gIn_d3d_scene, 0x1001bc74);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_queued_render_states, 0x1001bb90);
C2_HOOK_VARIABLE_IMPLEMENT(br_uint_32, gUINT_1001bc7c, 0x1001bc7c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bb94, 0x1001bb94);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bb98, 0x1001bb98);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFlip_count, 0x1001bbd4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bc70, 0x1001bc70);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbe0, 0x1001bbe0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDirty_min_x, 0x100156ec, 638);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDirty_min_y, 0x100156f4, 478);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDirty_max_x, 0x100156f0, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDirty_max_y, 0x100156f8, 1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDraw_d3d_10037ce0, gQueued_render_states, 100, 0x10037ce0);
C2_HOOK_VARIABLE_IMPLEMENT(DWORD, gDWORD_1001bbb0, 0x1001bbb0);
C2_HOOK_VARIABLE_IMPLEMENT(DWORD, gDWORD_1001bbb4, 0x1001bbb4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbe4, 0x1001bbe4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbf0, 0x1001bbf0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(WORD, gQueued_vertices_indices, 100, 0x1001d258);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_queued_vertices, 0x1001d77c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_queued_vertices_indices, 0x1001d780);
C2_HOOK_VARIABLE_IMPLEMENT(D3DPRIMITIVETYPE, gPrimitive_type, 0x1001dc28);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbc4, 0x1001bbc4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bba4, 0x1001bba4);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(D3DTLVERTEX, gQueued_vertices, 100, 0x1001bcd8);
C2_HOOK_VARIABLE_IMPLEMENT(void*, gDAT_1001dc50, 0x1001dc50);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc6c, 0x1001dc6c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc5c, 0x1001dc5c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc68, 0x1001dc68);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc58, 0x1001dc58);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc4c, 0x1001dc4c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc44, 0x1001dc44);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbbc, 0x1001bbbc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbc0, 0x1001bbc0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbc4, 0x1001bbc4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bba4, 0x1001bba4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc44, 0x1001dc44);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc4c, 0x1001dc4c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc54, 0x1001dc54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001dc30, 0x1001dc30);
C2_HOOK_VARIABLE_IMPLEMENT(br_colour, gCOLOUR_100156e8, 0x100156e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_100389ec, 0x100389ec);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_100381a4, 0x100381a4);
C2_HOOK_VARIABLE_IMPLEMENT(DWORD, gDWORD_1001bc30, 0x1001bc30);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDdraw_d3d_100385e4, gCurrent_render_states, 100, 0x100385e0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_render_state_changes, 0x1001dc38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_skipped_render_state_changes, 0x1001dc40);
C2_HOOK_VARIABLE_IMPLEMENT(int, gLock_count, 0x1001bbd8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbc8, 0x1001bbc8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbe8, 0x1001bbe8);
C2_HOOK_VARIABLE_IMPLEMENT(undefined4, gDAT_1001bc6c, 0x1001bc6c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_uint_16, gD3D_16bit_palette_buffer, 256, 0x1001b070);


BOOL gD3D_windows = TRUE;

void C2_HOOK_CDECL RestoreSurface(LPDIRECTDRAWSURFACE lpddSurface) {
    if (lpddSurface != NULL) {
        IDirectDrawSurface_Restore(lpddSurface);
    }
}
C2_HOOK_FUNCTION(0x10004f20, RestoreSurface)

LRESULT CALLBACK D3DWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ACTIVATEAPP: /* wParam: window is being activated or deactivated, lParam: thread identifier */
        if (wParam) {
            int i;

            OutputDebugStringA("D3D.BDD: WM_ACTIVATEAPP=TRUE");
            RestoreSurface(C2V(gPrimary_surface));
            RestoreSurface(C2V(gAttached_surface));
            RestoreSurface(C2V(gDepth_surface));
            for (i = 0; i < C2V(gINT_10038554); i++) {
                RestoreSurface(C2V(gBuffered_textures)[i].temp_surface);
                RestoreSurface(C2V(gBuffered_textures)[i].surface);
                C2V(gBuffered_textures)[i].field_0x20 = -1;
                C2V(gBuffered_textures)[i].field_0x24 = -1;
            }
        } else {
            if (C2V(gDirectDraw) != NULL) {
                IDirectDraw2_FlipToGDISurface(C2V(gDirectDraw2));
            }
            OutputDebugStringA("D3D.BDD: WM_ACTIVATEAPP=FALSE");
        }
    }
    return C2V(gOriginal_wndproc)(hWnd, uMsg, wParam, lParam);
}
C2_HOOK_FUNCTION(0x10004f30, D3DWndProc)

void C2_HOOK_CDECL d3d_debug_printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
}
C2_HOOK_FUNCTION(0x10005e20, d3d_debug_printf)

BOOL CALLBACK DDEnumCallback(GUID* lpDriverGuid, LPSTR lpDriverName, LPSTR lpDriverDescription, LPVOID lpContext) {
    /* Return 1 to continue execution */
    int enum_phase = *(int*)lpContext;

    d3d_debug_printf("Callback Generic");
    if (enum_phase == 0) {
        LPDIRECTDRAW ddraw;
        d3d_debug_printf("Callback Generic form 0/0, [%s/%s]", lpDriverName, lpDriverDescription);
        if (SUCCEEDED(C2V(gDirectDrawCreate)(lpDriverGuid, &ddraw, NULL))) {
            DDCAPS_DX5 driverCaps, helCaps;
            memset(&driverCaps, 0, sizeof(driverCaps));
            driverCaps.dwSize = sizeof(driverCaps);
            memset(&helCaps, 0, sizeof(helCaps));
            helCaps.dwSize = sizeof(helCaps);
            if (FAILED(IDirectDraw_GetCaps(ddraw, &driverCaps, &helCaps))) {
                IDirectDraw_Release(ddraw);
                return 1;
            }
            IDirectDraw_Release(ddraw);
            if (C2V(gDDraw_driver_found)) {
                return 1;
            }
            C2V(gIndex_ddraw_driver) = C2V(gCount_ddraw_drivers);
            C2V(gCount_ddraw_drivers) += 1;
            if (C2V(gRegister_guid_valid)) {
                if (C2V(gRegister_driver_guid_binary)) {
                    if (memcmp(&C2V(gRegister_driver_guid), lpDriverGuid, sizeof(GUID)) == 0) {
                        C2V(gDDraw_driver_found) = 1;
                    }
                } else {
                    if (lpDriverGuid == NULL) {
                        C2V(gDDraw_driver_found) = 1;
                    }
                }
            }
        }
        return 1;
    } else if (enum_phase == 1) {
        LPDIRECTDRAW ddraw;
        d3d_debug_printf("Callback Generic form 1, [%d/%d]", C2V(gCount_ddraw_drivers), C2V(gIndex_ddraw_driver));
        if (SUCCEEDED(C2V(gDirectDrawCreate)(lpDriverGuid, &ddraw, NULL))) {
            DDCAPS_DX5 driverCaps, helCaps;
            memset(&driverCaps, 0, sizeof(driverCaps));
            driverCaps.dwSize = sizeof(driverCaps);
            memset(&helCaps, 0, sizeof(helCaps));
            helCaps.dwSize = sizeof(helCaps);
            if (FAILED(IDirectDraw_GetCaps(ddraw, &driverCaps, &helCaps))) {
                IDirectDraw_Release(ddraw);
                return 1;
            }
            if (helCaps.dwCaps2 & DDCAPS2_NO2DDURING3DSCENE) {
                    C2V(gNo_2d_during_3d_scene) = 1;
            }
            if (C2V(gIndex_ddraw_driver) == C2V(gCount_ddraw_drivers)) {

                C2V(gDirectDraw) = ddraw;
                d3d_debug_printf("%s %s", lpDriverName, lpDriverDescription);
                if (IDirectDraw_QueryInterface(C2V(gDirectDraw), &IID_IDirect3D, (LPVOID)&C2V(gDirect3D)) != DD_OK) {
                    C2V(gError_code) = 1;
                }
                d3d_debug_printf("QI d3d 1[%d]", C2V(gError_code));
                if (IDirectDraw_QueryInterface(C2V(gDirectDraw), &IID_IDirectDraw2, (LPVOID)&C2V(gDirectDraw2)) != DD_OK) {
                    C2V(gError_code) = 1;
                }
                d3d_debug_printf("QI dd2 1[%d]", C2V(gError_code));
                if (gD3D_windows) {
                    if (IDirectDraw2_SetCooperativeLevel(C2V(gDirectDraw2), C2V(gActive_window), DDSCL_NORMAL) != DD_OK) {
                        C2V(gError_code) = 1;
                    }
                } else {
                    if (IDirectDraw2_SetCooperativeLevel(C2V(gDirectDraw2), C2V(gActive_window), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK) {
                        C2V(gError_code) = 1;
                    }
                }
                d3d_debug_printf("SetCo-op level 1[%d]", C2V(gError_code));
                C2V(gCount_ddraw_drivers) += 1;
                return 1;
            }
            IDirectDraw_Release(ddraw);
            C2V(gCount_ddraw_drivers) += 1;
        }
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x10002130, DDEnumCallback)

void C2_HOOK_CDECL d3d_read_registry(void) {
    HKEY key;
    DWORD type;
    DWORD process_id;
    DWORD size;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\SCI\\Carmageddon2", 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
        return;
    }
    size = 4;
    if (RegQueryValueExA(key,"ProcessId", NULL, &type, (LPBYTE)&process_id, &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    GetCurrentProcessId();
    size = sizeof(C2V(gRegister_driver_guid));
    if (RegQueryValueExA(key,"DrvGuid", NULL, &type, (LPBYTE)&C2V(gRegister_driver_guid), &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    if (type == REG_BINARY) {
        C2V(gRegister_driver_guid_binary) = 1;
    } else {
        if (type != REG_DWORD) {
            RegCloseKey(key);
            return;
        }
        C2V(gRegister_driver_guid_binary) = 0;
    }
    size = sizeof(C2V(gRegister_device_guid));
    if (RegQueryValueExA(key, "DevGuid", NULL, &type, (LPBYTE)&C2V(gRegister_device_guid), &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    if (type != REG_BINARY) {
        RegCloseKey(key);
        return;
    }
    size = sizeof(C2V(gRegister_flags));
    if (RegQueryValueExA(key, "Flags", NULL, &type, (LPBYTE)&C2V(gRegister_flags), &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    if (type != REG_DWORD) {
        C2V(gRegister_flags) = 0xffffffff;
        RegCloseKey(key);
        return;
    }
    RegCloseKey(key);
    C2V(gRegister_guid_valid) = 1;
}
C2_HOOK_FUNCTION(0x10001f90, d3d_read_registry)

int C2_HOOK_CDECL DetectD3DDevices(void) {
    int i;
    int enum_phase;
    C2V(gDevice_pixels) = C2V(gPixel_buffer);

    for (i = 0; i < BR_ASIZE(C2V(gDAT_100385e0)); i++) {
        C2V(gDAT_100385e0)[i].field_0x4 = 0;
    }
    C2V(gActive_window) = GetActiveWindow();
    if (C2V(gOriginal_wndproc) == NULL) {
        C2V(gOriginal_wndproc) = (WNDPROC)SetWindowLongPtrA(C2V(gActive_window), GWLP_WNDPROC, (LONG_PTR)D3DWndProc);
    }
    d3d_debug_printf("Jims Brender driver for D3D  version 0.0.1");
    d3d_debug_printf("------------------------------------------");
    d3d_debug_printf("Compile with #define NOCONSOLE to remove these messages");
    C2V(gModule_ddraw) = LoadLibraryA("DDRAW");
    if (C2V(gModule_ddraw) == NULL) {
        return 0;
    }
    C2V(gDirectDrawCreate) = (tDirectDrawCreate*)GetProcAddress(C2V(gModule_ddraw), "DirectDrawCreate");
    if (C2V(gDirectDrawCreate) == NULL) {
        return 0;
    }
    C2V(gDirectDrawEnumerateA) = (tDirectDrawEnumerateA*)GetProcAddress(C2V(gModule_ddraw), "DirectDrawEnumerateA");
    if (C2V(gDirectDrawEnumerateA) == NULL) {
        return 0;
    }
    d3d_debug_printf("Got proc details %d", C2V(gError_code));
    d3d_read_registry();
    C2V(gCount_ddraw_drivers) = 0;
    C2V(gIndex_ddraw_driver) = 0;
    enum_phase = 0;
    if (C2V(gDirectDrawEnumerateA)(DDEnumCallback, &enum_phase) != DD_OK) {
        return 4;
    }
    if (C2V(gCount_ddraw_drivers) == 0) {
        return 69;
    }
    C2V(gCount_ddraw_drivers) = 0;
    enum_phase = 1;
    if (C2V(gDirectDrawEnumerateA)(DDEnumCallback, &enum_phase) != DD_OK) {
        return 4;
    }
    d3d_debug_printf("finished callback generic %d", C2V(gError_code));
    return 0;
}
C2_HOOK_FUNCTION(0x10005080, DetectD3DDevices)

HRESULT PASCAL DDEnumDisplayModes(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext) {
    int phase = *(int*)lpContext;

    if (lpDDSurfaceDesc->dwWidth < 320) {
        return 1;
    }
    if (lpDDSurfaceDesc->dwHeight < 240) {
        return 1;
    }
    d3d_debug_printf("Enum-Modes: %dx%d  bits=%d  rgb=%d,%d,%d",
        lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight,
        lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
        lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask,
        lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask,
        lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask);
    if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount != 16) {
        return 1;
    }
    switch (phase) {
    case 0:
        if (lpDDSurfaceDesc->dwWidth == 640 && lpDDSurfaceDesc->dwHeight == 480) {
            if (!gD3D_windows &&lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount != 16) {
                break;
            }
            if (!(lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask == 0xf800
                    && lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask == 0x07e0
                    && lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask == 0x001f
                    && C2V(gSelected_display_mode) != -1)) {
                C2V(gSelected_display_mode) = C2V(gCount_display_modes);
            }
        }
        if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 16
                && lpDDSurfaceDesc->dwWidth == 640
                && lpDDSurfaceDesc->dwHeight == 480) {
            C2V(g640x480x16_display_mode_flags) |= 0x1;
            memcpy(&C2V(g640x480x16_pixelFormat), &lpDDSurfaceDesc->ddpfPixelFormat, sizeof(DDPIXELFORMAT));
        }
        break;
    case 1:
        if (C2V(gCount_display_modes) != C2V(gSelected_display_mode)) {
            break;
        }
        C2V(gDisplay_width_10015888) = lpDDSurfaceDesc->dwWidth;
        C2V(gDisplay_height_1001588c) = lpDDSurfaceDesc->dwHeight;
        C2V(gDisplay_width_10037cc0) = C2V(gDisplay_width_10015888);
        C2V(gDisplay_height_10037cc4) = C2V(gDisplay_height_1001588c);
        C2V(gDisplay_width_10038004) = C2V(gDisplay_width_10015888);
        C2V(gDisplay_height_10038008) = C2V(gDisplay_height_1001588c);
        if (!gD3D_windows) {
            if (IDirectDraw2_SetDisplayMode(C2V(gDirectDraw2), C2V(gDisplay_width_10015888),
                    C2V(gDisplay_height_1001588c),
                    lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, 0, 0) != DD_OK) {
                C2V(gSelected_display_mode) += 1;
                if (C2V(gCount_display_modes) == C2V(gSelected_display_mode)) {
                    C2V(gError_code) = 1;
                }
            }
        }

        if (C2V(gAttached_surface_pixel_format.dwRGBAlphaBitMask)
              || (C2V(gAttached_surface_pixel_format).dwRBitMask == 0x7c00
                  && C2V(gAttached_surface_pixel_format).dwGBitMask == 0x03e0
                  && C2V(gAttached_surface_pixel_format).dwBBitMask == 0x001f)) {
            C2V(gAttached_surface_pixelformat) = 2;
        } else if (C2V(gAttached_surface_pixel_format).dwRBitMask == 0xf800
                && C2V(gAttached_surface_pixel_format).dwGBitMask == 0x07e0
                && C2V(gAttached_surface_pixel_format).dwBBitMask == 0x001f) {
            C2V(gAttached_surface_pixelformat) = 4;
        } else {
            C2V(gAttached_surface_pixelformat) = 2;
        }
        memcpy(&C2V(gAttached_surface_pixel_format), &lpDDSurfaceDesc->ddpfPixelFormat, sizeof(DDPIXELFORMAT));
        break;
    default:
        return 1;
    }
    C2V(gCount_display_modes) += 1;
    return 1;
}
C2_HOOK_FUNCTION(0x10002460, DDEnumDisplayModes)

HRESULT CALLBACK D3DEnumDevices(GUID FAR *lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpD3DHWDeviceDesc, LPD3DDEVICEDESC lpD3DHELDeviceDesc, LPVOID lpUserArgs) {
    int phase = *(int*)lpUserArgs;
    if (memcmp(lpGuid, &IID_IDirect3DRampDevice, sizeof(GUID)) == 0) {
        return 1;
    }
    if (memcmp(lpGuid, &IID_IDirect3DMMXDevice, sizeof(GUID)) == 0) {
        return 1;
    }
    if (!((lpD3DHELDeviceDesc->dwFlags | lpD3DHWDeviceDesc->dwFlags) & D3DDD_TRICAPS)) {
        return 1;
    }
    if (lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth == 0 || lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth == 0) {
        return 1;
    }
    switch (phase) {
    case 0:
        if (C2V(gRegister_guid_valid)) {
            if (memcmp(&C2V(gRegister_device_guid), lpGuid, sizeof(GUID)) == 0) {
                C2V(gSelected_d3d_device) = gCount_d3d_devices;
                C2V(gFound_d3d_device) = 1;
            }
        }
        if (!C2V(gFound_d3d_device) && lpD3DHWDeviceDesc->dcmColorModel != 0) {
            gSelected_d3d_device = gCount_d3d_devices;
        }
        C2V(gCount_d3d_devices) += 1;
        d3d_debug_printf("Enum Devices form 0, [%s] %d %d  [%d/%d]",
            lpDeviceName, C2V(gSelected_d3d_device), C2V(gFound_d3d_device),
            lpD3DHWDeviceDesc->dwDeviceZBufferBitDepth,
            lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth);
        break;
    case 1:
        d3d_debug_printf("Enum Devices form 1, [%s] %d %d  [%d/%d]",
            lpDeviceName, C2V(gSelected_d3d_device), C2V(gFound_d3d_device),
                lpD3DHWDeviceDesc->dwDeviceZBufferBitDepth,
                lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth);
        if (C2V(gSelected_d3d_device) == C2V(gCount_d3d_devices)) {
            C2V(gDevice_has_color_model) = lpD3DHWDeviceDesc->dcmColorModel != 0;
            memcpy(&C2V(gDevice_description), C2V(gDevice_has_color_model) ? lpD3DHWDeviceDesc : lpD3DHELDeviceDesc, sizeof(D3DDEVICEDESC));
            d3d_debug_printf("Selected device zbuffer caps %d",
                C2V(gDevice_description).dwDeviceZBufferBitDepth);
            memcpy(&C2V(gDevice_guid), lpGuid, sizeof(GUID));
        }
        C2V(gCount_d3d_devices) += 1;
        break;
    default:
        break;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x10002630, D3DEnumDevices)

HRESULT CALLBACK D3DEnumTextureFormats(LPDDSURFACEDESC lpDdsd, LPVOID lpContext) {
    char format_str[33];
    DWORD red_bit_mask, grn_bit_mask, blu_bit_mask, alp_bit_mask;
    int i;
    int bit_count;

    int phase = *(int*)lpContext;
    if (phase != 0) {
        return 1;
    }
    d3d_debug_printf("Enum Textures bpp=%d", lpDdsd->ddpfPixelFormat.dwRGBBitCount);
    bit_count = lpDdsd->ddpfPixelFormat.dwRGBBitCount;
    if (lpDdsd->ddpfPixelFormat.dwRGBBitCount != 16) {
        if (!(bit_count == 8 && (lpDdsd->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8))) {
            return 1;
        }
    }
    red_bit_mask = lpDdsd->ddpfPixelFormat.dwRBitMask;
    grn_bit_mask = lpDdsd->ddpfPixelFormat.dwGBitMask;
    blu_bit_mask = lpDdsd->ddpfPixelFormat.dwBBitMask;
    alp_bit_mask = lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask;

    memset(format_str, 0, sizeof(format_str));
    for (i = 31; i >= 0; i--) {
        char c = '.';
        if (red_bit_mask & 0x1) {
            c = 'R';
        }
        red_bit_mask >>= 1;
        if (grn_bit_mask & 0x1) {
            c = 'G';
        }
        grn_bit_mask >>= 1;
        if (blu_bit_mask & 0x1) {
            c = 'B';
        }
        blu_bit_mask >>= 1;
        if (alp_bit_mask & 0x1) {
            c = 'A';
        }
        alp_bit_mask >>= 1;
        format_str[i] = c;
    }
    d3d_debug_printf("format=%s r=0x%x g=0x%x b=0x%x a=0x%x",
        format_str,
        lpDdsd->ddpfPixelFormat.dwRBitMask,
        lpDdsd->ddpfPixelFormat.dwGBitMask,
        lpDdsd->ddpfPixelFormat.dwBBitMask,
        lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask);
    if (bit_count == 16
            && lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask == 0
            && lpDdsd->ddpfPixelFormat.dwRBitMask == 0x7c00
            && lpDdsd->ddpfPixelFormat.dwGBitMask == 0x03e0
            && lpDdsd->ddpfPixelFormat.dwBBitMask == 0x001f) {
        C2V(gAvailable_pixel_formats) |= 0x2;
        memcpy(&C2V(gAvailable_RGB555_texture_format_description), lpDdsd, sizeof(*lpDdsd));
        d3d_debug_printf("TEX.ID: RGB 555");
        if (C2V(gPreferred_texture_format) < 0x2) {
            C2V(gPreferred_texture_format) = 0x2;
            memcpy(&C2V(gPreferred_texture_surface_description), lpDdsd, sizeof(*lpDdsd));
        }
        if (C2V(gPreferred_opaque_texture_format) < 0x2) {
            C2V(gPreferred_opaque_texture_format) = 0x2;
            memcpy(&C2V(gPreferred_opaque_texture_surface_description), lpDdsd, sizeof(*lpDdsd));
        }
    }
    if (C2V(gPixel_format_is_xrgb1555) != 1 || C2V(gDevice_has_color_model)) {
        if (bit_count == 16
                && lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask == 0
                && lpDdsd->ddpfPixelFormat.dwRBitMask == 0xf800
                && lpDdsd->ddpfPixelFormat.dwGBitMask == 0x07e0
                && lpDdsd->ddpfPixelFormat.dwBBitMask == 0x001f) {
            C2V(gAvailable_pixel_formats) |= 0x4;
            memcpy(&C2V(gAvailable_RGB565_texture_format_description), lpDdsd, sizeof(*lpDdsd));
            d3d_debug_printf("TEX.ID: RGB 565");
            if (C2V(gPreferred_texture_format) < 0x4) {
                C2V(gPreferred_texture_format) = 0x4;
                memcpy(&C2V(gPreferred_texture_surface_description), lpDdsd, sizeof(*lpDdsd));
            }
            if (C2V(gPreferred_opaque_texture_format) < 0x4) {
                C2V(gPreferred_opaque_texture_format) = 0x4;
                memcpy(&C2V(gPreferred_opaque_texture_surface_description), lpDdsd, sizeof(*lpDdsd));
            }
        }
        if ((C2V(gRegister_flags) & 0x4)
                && bit_count == 16
                && lpDdsd->ddpfPixelFormat.dwAlphaBitDepth == 0xf000
                && lpDdsd->ddpfPixelFormat.dwRBitMask == 0x0f00
                && lpDdsd->ddpfPixelFormat.dwGBitMask == 0x00f0
                && lpDdsd->ddpfPixelFormat.dwBBitMask == 0xf) {
            C2V(gAvailable_pixel_formats) |= 0x10;
            memcpy(&C2V(gAvailable_RGB565_texture_format_description), lpDdsd, sizeof(*lpDdsd));
            d3d_debug_printf("TEX.ID: ARGB 4444");
            if (C2V(gPreferred_texture_format) < 0x10) {
                C2V(gPreferred_texture_format) = 0x10;
                memcpy(&C2V(gPreferred_texture_surface_description), lpDdsd, sizeof(*lpDdsd));
            }
        }
    }
    return 1;
}
C2_HOOK_FUNCTION(0x100027f0, D3DEnumTextureFormats)

int C2_HOOK_CDECL SetD3DViewport(void) {
    D3DVIEWPORT2 viewport2;

    if (C2V(gD3D_Viewport) == NULL) {
        return 0;
    }
    C2V(gDisplay_center_x) = (float)(C2V(gDisplay_width_10015888) / 2);
    C2V(gDisplay_center_y) = (float)(C2V(gDisplay_height_1001588c) / 2);

    memset(&viewport2, 0, sizeof(viewport2));
    viewport2.dwSize = sizeof(viewport2);
    viewport2.dwX = 0;
    viewport2.dwY = 0;
    viewport2.dwWidth = C2V(gDisplay_width_10015888);
    viewport2.dwHeight = C2V(gDisplay_height_1001588c);
    viewport2.dvClipX = 0.f;
    viewport2.dvClipY = 0.f;
    viewport2.dvClipWidth = (float)C2V(gDisplay_width_10015888);
    viewport2.dvClipHeight = (float)C2V(gDisplay_height_1001588c);
    viewport2.dvMinZ = 0.f;
    viewport2.dvMaxZ = 1.f;
    if (IDirect3DViewport2_SetViewport2(C2V(gD3D_Viewport), &viewport2) != D3D_OK) {
        d3d_debug_printf("Set viewport2");
        C2V(gError_code) = 1;
        return 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x10002ac0, SetD3DViewport)

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
    int width;
    int height;
    int count;
} tStruct_10015898;

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tStruct_10015898, gStruct_10015898_ARRAY, 7, 0x10015898, {
    {   4,   0,  16,  16,  16,   0, },
    {  20,   0,   8,  32,  32,   0, },
    {  14,   0,   9,  64,  64,   0, },
    {   2,   0,   6, 128, 128,   0, },
    {   1,   0,   2,  64, 128,   0, },
    {   1,   0,   2, 128,  64,   0, },
    {   0,   0,   0,   0,   0,   0, },
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gSize_texture_cache, 0x10038554);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_1001bbd0, 0x1001bbd0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tStruct_100381c0, gStruct_100381c0, 16, 0x100381c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV_INIT(br_uint_16, gTextures_alpha_init, [16][16][16], 0x10011030, {
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0x0000 },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
    },
    {
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
    },
    {
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000 },
    },
    {
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
        { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff },
    },
});

void C2_HOOK_CDECL D3DSetupTextureCacheColorKeyAlpha(void) {
    int i;
    LPDIRECTDRAWSURFACE surface;
    LPDIRECT3DTEXTURE2 texture;
    DDSURFACEDESC surface_desc;

    d3d_debug_printf("TC: setup color key alpha");
    for (i = 0; i < BR_ASIZE(C2V(gStruct_100381c0)); i++) {
        C2V(gStruct_100381c0)[i].surface = NULL;
        C2V(gStruct_100381c0)[i].texture = NULL;
        C2V(gStruct_100381c0)[i].texture_handle = 0;
    }
    if (!C2V(gTexture_format_is_NOT_ARGB4444)) {
        d3d_debug_printf("TC: setup color key alpha (not needed)");
        return;
    }
    memcpy(&surface_desc, &C2V(gPreferred_opaque_texture_surface_description), sizeof(surface_desc));
    surface_desc.dwWidth = 16;
    surface_desc.dwHeight = 16;
    surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
    surface_desc.dwFlags =  DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &surface, NULL) != DD_OK) {
        return;
    }
    if (IDirectDrawSurface_QueryInterface(surface, &IID_IDirect3DTexture2, (LPVOID)&texture) != DD_OK) {
        IDirectDrawSurface_Release(surface);
        return;
    }
    for (i = 0; i <  BR_ASIZE(C2V(gStruct_100381c0)); i++) {
        DDCOLORKEY color_key;
        int y;

        memset(&surface_desc, 0, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        if (IDirectDrawSurface_Lock(surface, NULL, &surface_desc, 0, NULL) != DD_OK) {
            IDirect3DTexture2_Release(texture);
            IDirectDraw_Release(surface);
            return;
        }
        for (y = 0; y < 16; y++) {
            memcpy((br_uint_8*)surface_desc.lpSurface + y * surface_desc.lPitch, C2V(gTextures_alpha_init)[i][y], 2 * 16);
        }
        if (IDirectDrawSurface_Unlock(surface, NULL) != DD_OK) {
            IDirect3DTexture2_Release(texture);
            IDirectDraw_Release(surface);
            return;
        }
        memcpy(&surface_desc, &C2V(gPreferred_opaque_texture_surface_description), sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        surface_desc.dwWidth = 16;
        surface_desc.dwHeight = 16;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (C2V(gDevice_has_color_model) ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gStruct_100381c0)[i].surface, NULL) != DD_OK) {
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        if (IDirectDraw_QueryInterface(C2V(gStruct_100381c0)[i].surface, &IID_IDirect3DTexture2, (LPVOID)&C2V(gStruct_100381c0)[i].texture) != DD_OK) {
            IDirectDrawSurface_Release(C2V(gStruct_100381c0)[i].surface);
            C2V(gStruct_100381c0)[i].surface = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        if (IDirect3DTexture2_GetHandle(C2V(gStruct_100381c0)[i].texture, C2V(gD3DDevice2), &C2V(gStruct_100381c0)[i].texture_handle) != D3D_OK) {
            IDirectDrawSurface_Release(C2V(gStruct_100381c0)[i].surface);
            C2V(gStruct_100381c0)[i].surface = NULL;
            IDirect3DTexture2_Release(C2V(gStruct_100381c0)[i].texture);
            C2V(gStruct_100381c0)[i].texture = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        color_key.dwColorSpaceLowValue = 0;
        color_key.dwColorSpaceHighValue = 0;
        if (IDirectDrawSurface_SetColorKey(C2V(gStruct_100381c0)[i].surface, DDCKEY_SRCBLT, &color_key) != DD_OK) {
            IDirectDrawSurface_Release(C2V(gStruct_100381c0)[i].surface);
            C2V(gStruct_100381c0)[i].surface = NULL;
            IDirect3DTexture2_Release(C2V(gStruct_100381c0)[i].texture);
            C2V(gStruct_100381c0)[i].texture = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
    }
    IDirect3DTexture2_Release(texture);
    IDirectDrawSurface_Release(surface);
}
C2_HOOK_FUNCTION(0x, D3DSetupTextureCacheColorKeyAlpha)

int C2_HOOK_CDECL FUN_10004280(int count, int width, int height) {
    int i;
    LPDIRECTDRAWSURFACE temp_surface;
    LPDIRECT3DTEXTURE2 temp_texture;
    LPDIRECTDRAWSURFACE temp_opaque_surface;
    LPDIRECT3DTEXTURE2 temp_opaque_texture;

    C2V(gBuffered_textures)[C2V(gSize_texture_cache)].width = width;
    C2V(gBuffered_textures)[C2V(gSize_texture_cache)].height = height;
    for (i = 0; C2V(gBuffered_textures)[i].width != width || C2V(gBuffered_textures)[i].height != height; i++) {
    }
    if (i == C2V(gSize_texture_cache)) {
        DDSURFACEDESC surface_desc;

        memcpy(&surface_desc, &C2V(gPreferred_texture_surface_description), sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &temp_surface, NULL) != DD_OK) {
            abort(); /* This should succeed */
            return -1;
        }
        if (IDirectDraw_QueryInterface(temp_surface, &IID_IDirect3DTexture2, (LPVOID)&temp_texture) != DD_OK) {
            IDirectDrawSurface_Release(temp_surface);
            return -1;
        }

        memcpy(&surface_desc, &C2V(gPreferred_opaque_texture_surface_description), sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &temp_opaque_surface, NULL) != DD_OK) {
            IDirect3DTexture2_Release(temp_texture);
            IDirectDrawSurface_Release(temp_surface);
            return -1;
        }
        if (IDirectDraw_QueryInterface(temp_opaque_surface, &IID_IDirect3DTexture2, (LPVOID)&temp_opaque_texture) != DD_OK) {
            IDirect3DTexture2_Release(temp_texture);
            IDirectDrawSurface_Release(temp_surface);
            IDirectDrawSurface_Release(temp_opaque_surface);
            return -1;
        }
        d3d_debug_printf("TC: made temp textures");
    } else {
        temp_surface = C2V(gBuffered_textures)[i].temp_surface;
        temp_texture = C2V(gBuffered_textures)[i].temp_texture;
        temp_opaque_surface = C2V(gBuffered_textures)[i].temp_opaque_surface;
        temp_opaque_texture = C2V(gBuffered_textures)[i].temp_opaque_texture;
    }
    for (i = 0; i < count; i++) {
        DDSURFACEDESC surface_desc;

        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].width = width;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].height = height;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].temp_texture = temp_texture;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].temp_surface = temp_surface;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].temp_opaque_texture = temp_opaque_texture;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].temp_opaque_surface = temp_opaque_surface;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].field_0x1c = 0;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].field_0x20 = -1;
        C2V(gBuffered_textures)[C2V(gSize_texture_cache)].field_0x24 = -1;
        memcpy(&surface_desc, &C2V(gPreferred_opaque_texture_surface_description), sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (C2V(gDevice_has_color_model) ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
        surface_desc.dwFlags =  DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

        if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gBuffered_textures)[C2V(gSize_texture_cache)].surface, NULL) != DD_OK) {
            return -1;
        }
        if (IDirectDrawSurface_QueryInterface(C2V(gBuffered_textures)[C2V(gSize_texture_cache)].surface, &IID_IDirect3DTexture2, (LPVOID)&C2V(gBuffered_textures)[C2V(gSize_texture_cache)].texture) != DD_OK) {
            return -1;
        }
        d3d_debug_printf("TC: qi");
        if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[C2V(gSize_texture_cache)].texture, temp_opaque_texture) != D3D_OK) {
            return -1;
        }
        d3d_debug_printf("TC: load");
        if (IDirect3DTexture2_GetHandle(C2V(gBuffered_textures)[C2V(gSize_texture_cache)].texture, C2V(gD3DDevice2), &C2V(gBuffered_textures)[C2V(gSize_texture_cache)].texture_handle) != D3D_OK) {
            return -1;
        }
        d3d_debug_printf("TC: allocated texture");
        C2V(gSize_texture_cache) += 1;
    }
    return count;
}
C2_HOOK_FUNCTION(0x10004280, FUN_10004280)

int C2_HOOK_CDECL StartTextureCache(void) {
    int i;
    int cache_full;
    int second_loop;

    d3d_debug_printf("--Starting Texture Cache--");
    C2V(gSize_texture_cache) = 0;
    if (C2V(gDevice_description).dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
        d3d_debug_printf("TC: ditching non square textures");

        for (i = 0; C2V(gStruct_10015898_ARRAY)[i].field_0x8 != 0; i++) {
            if (C2V(gStruct_10015898_ARRAY)[i].width != C2V(gStruct_10015898_ARRAY)[i].height) {
                C2V(gStruct_10015898_ARRAY)[i].field_0x0 = 0;
            }
            if (C2V(gStruct_10015898_ARRAY)[i].width == 16 && C2V(gStruct_10015898_ARRAY)[i].height == 16) {
                C2V(gStruct_10015898_ARRAY)[i].field_0x8 *= 2;
            }
        }
    }
    D3DSetupTextureCacheColorKeyAlpha();
    cache_full = 0;
    second_loop = 0;
    for (;;) {
        int count;

        for (i = 0; C2V(gStruct_10015898_ARRAY)[i].field_0x8 != 0; i++) {

            C2V(gStruct_10015898_ARRAY)[i].field_0x4 = C2V(gStruct_10015898_ARRAY)[i].field_0x0;
        }

        count = 1;
        while (count != 0) {
            if (cache_full) {
                break;
            }
            count = 0;
            for (i = 0; C2V(gStruct_10015898_ARRAY)[i].field_0x8 != 0; i++) {
                if (cache_full) {
                    break;
                }
                if (C2V(gStruct_10015898_ARRAY)[i].field_0x4 > 0) {
                    C2V(gStruct_10015898_ARRAY)[i].field_0x4 -= 1;
                    if (FUN_10004280(C2V(gStruct_10015898_ARRAY)[i].field_0x8,
                            C2V(gStruct_10015898_ARRAY)[i].width >> C2V(gINT_1001bbd0),
                            C2V(gStruct_10015898_ARRAY)[i].height >> C2V(gINT_1001bbd0)) == -1) {
                        cache_full = 1;
                    } else {
                        C2V(gStruct_10015898_ARRAY)[i].count += C2V(gStruct_10015898_ARRAY)[i].field_0x8;
                    }
                }
            }
        }
        if (second_loop || count != 0) {
            break;
        }
        second_loop = 1;
    }
    for (i = 0; C2V(gStruct_10015898_ARRAY)[i].field_0x8 != 0; i++) {
        d3d_debug_printf("Texture Alloc %d of %dx%d=%d at %dx%d",
            C2V(gStruct_10015898_ARRAY)[i].count,
            C2V(gStruct_10015898_ARRAY)[i].field_0x0,
            C2V(gStruct_10015898_ARRAY)[i].field_0x8,
            C2V(gStruct_10015898_ARRAY)[i].field_0x0 * C2V(gStruct_10015898_ARRAY)[i].field_0x8,
            C2V(gStruct_10015898_ARRAY)[i].width,
            C2V(gStruct_10015898_ARRAY)[i].height);
    }
    /* FIXME: magic 2048 */
    d3d_debug_printf("Texture cache has %d off %d Entries", C2V(gSize_texture_cache), 2048);
    return 0;
}
C2_HOOK_FUNCTION(0x10004610, StartTextureCache)

int C2_HOOK_CDECL InitD3D(HWND hWnd) {
    DDSURFACEDESC surface_desc;
    DDSCAPS surface_caps;
    int phase;
    D3DMATERIAL black_material;
    DDBLTFX blt_fx;

    C2V(g640x480x16_display_mode_flags) = 0;

    C2V(gCount_display_modes) = 0;
    C2V(gSelected_display_mode) = -1;
    phase = 0;
    if (IDirectDraw2_EnumDisplayModes(C2V(gDirectDraw2), 0, NULL, &phase, DDEnumDisplayModes) != DD_OK) {
        return 5;
    }
    C2V(gCount_display_modes) = 0;
    phase = 1;
    if (IDirectDraw2_EnumDisplayModes(C2V(gDirectDraw2), 0, NULL, &phase, DDEnumDisplayModes) != DD_OK) {
        return 5;
    }
    d3d_debug_printf("finished enum display modes %d", C2V(gError_code));
    if (IDirectDraw_QueryInterface(C2V(gDirectDraw), &IID_IDirect3D2, (LPVOID)&C2V(gDirect3D2)) != DD_OK) {
        return 1;
    }

    d3d_debug_printf("Create d3d2 object");
    C2V(gCount_d3d_devices) = 0;
    C2V(gSelected_d3d_device) = 0;
    phase = 0;
    if (IDirect3D2_EnumDevices(C2V(gDirect3D2), D3DEnumDevices, &phase) != D3D_OK) {
        return 7;
    }
    C2V(gCount_d3d_devices) = 0;
    phase = 1;
    if (IDirect3D2_EnumDevices(C2V(gDirect3D2), D3DEnumDevices, &phase) != D3D_OK) {
        return 7;
    }
    d3d_debug_printf("finished enum devices");

    if (gD3D_windows) {
        memset(&surface_desc, 0, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        surface_desc.dwFlags = DDSD_CAPS;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    } else {
        memset(&surface_desc, 0, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        surface_desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
        surface_desc.dwBackBufferCount = 1;
    }
    if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gPrimary_surface), NULL) != DD_OK) {
        d3d_debug_printf("Create Direct draw surface");
        C2V(gError_code) = 1;
        return 9;
    }
    d3d_debug_printf("Create primary surface");

    if (gD3D_windows) {
        LPDIRECTDRAWCLIPPER clipper;

        d3d_debug_printf("Create d3d clipper");
        if (IDirectDraw2_CreateClipper(C2V(gDirectDraw2), 0, &clipper, NULL) != DD_OK) {
            return 7;
        }
        IDirectDrawClipper_SetHWnd(clipper, 0, C2V(hWnd));
        IDirectDrawSurface_SetClipper(C2V(gPrimary_surface), clipper);
        IDirectDrawSurface_Release(clipper);
    }

    if (gD3D_windows) {
        memset(&surface_desc, 0, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        surface_desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        surface_desc.dwWidth = 640;
        surface_desc.dwHeight = 480;

        if (IDirectDraw2_CreateSurface(C2V(gDirectDraw2), &surface_desc, &C2V(gAttached_surface), NULL ) != DD_OK) {
            d3d_debug_printf("Create back surface");
            C2V(gError_code) = 1;
            return 9;
        }
    } else {
        surface_caps.dwCaps = DDSCAPS_BACKBUFFER;
        if (IDirectDrawSurface_GetAttachedSurface(C2V(gPrimary_surface), &surface_caps, &C2V(gAttached_surface)) != DD_OK) {
            d3d_debug_printf("Get back surface");
            C2V(gError_code) = 1;
            return 9;
        }
    }

    if (IDirectDrawSurface_GetPixelFormat(C2V(gAttached_surface), &C2V(gAttached_surface_pixel_format)) != DD_OK) {
        d3d_debug_printf("Get back buffer pixel format");
        C2V(gError_code) = 1;
        return 9;
    }

    if (C2V(gAttached_surface_pixel_format).dwRGBAlphaBitMask == 0) {
        if (C2V(gAttached_surface_pixel_format).dwRBitMask == 0x7c00
                && C2V(gAttached_surface_pixel_format).dwGBitMask == 0x03e0
                && C2V(gAttached_surface_pixel_format).dwBBitMask == 0x001f) {
            C2V(gPixel_format_is_xrgb1555) = 1;
        } else if (C2V(gAttached_surface_pixel_format).dwRBitMask == 0xf800
                && C2V(gAttached_surface_pixel_format).dwGBitMask == 0x07e0
                && C2V(gAttached_surface_pixel_format).dwBBitMask == 0x001f) {
        } else {
            C2V(gPixel_format_is_xrgb1555) = 1;
        }
    } else {
        C2V(gPixel_format_is_xrgb1555) = 1;
    }
    d3d_debug_printf("Get back surface");

    memset(&surface_desc, 0, sizeof(surface_desc));
    surface_desc.dwSize = sizeof(surface_desc);
    surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
    surface_desc.dwWidth = C2V(gDisplay_width_10015888);
    surface_desc.dwHeight = C2V(gDisplay_height_1001588c);
    surface_desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | (C2V(gDevice_has_color_model) == 1 ? DDSCAPS_VIDEOMEMORY : DDSCAPS_SYSTEMMEMORY);
    if (C2V(gDevice_description).dwDeviceZBufferBitDepth & DDBD_32) {
        surface_desc.dwZBufferBitDepth = 32;
    } else if (C2V(gDevice_description).dwDeviceZBufferBitDepth & DDBD_24) {
        surface_desc.dwZBufferBitDepth = 24;
    } else if (C2V(gDevice_description).dwDeviceZBufferBitDepth & DDBD_16) {
        surface_desc.dwZBufferBitDepth = 16;
    } else if (C2V(gDevice_description).dwDeviceZBufferBitDepth & DDBD_8) {
        surface_desc.dwZBufferBitDepth = 8;
    } else {
        d3d_debug_printf("Unsupported Z-buffer depth %d",
            C2V(gDevice_description).dwDeviceZBufferBitDepth);
        return 1;
    }

    d3d_debug_printf("About to create Z buffer");
    if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gDepth_surface), NULL) != DD_OK) {
        d3d_debug_printf("Create Z-Buffer");
        C2V(gError_code) = 1;
        return 1;
    }
    d3d_debug_printf("About to Attach Z buffer");
    if (IDirectDrawSurface_AddAttachedSurface(C2V(gAttached_surface), C2V(gDepth_surface)) != DD_OK) {
        d3d_debug_printf("Attach Z-Buffer");
        C2V(gError_code) = 1;
        return 1;
    }
    d3d_debug_printf("Attached Z buffer");
    if (IDirect3D2_CreateDevice(C2V(gDirect3D2), &C2V(gDevice_guid), C2V(gAttached_surface), &C2V(gD3DDevice2)) != D3D_OK) {
        d3d_debug_printf("Create Device");
        C2V(gError_code) = 1;
        return 7;
    }
    d3d_debug_printf("Create DEvice");

    if (IDirect3D2_CreateViewport(C2V(gDirect3D2), &C2V(gD3D_Viewport), NULL) != D3D_OK) {
        d3d_debug_printf("Create d3d viewport");
        C2V(gError_code) = 1;
        return 1;
    }
    if (IDirect3DDevice2_AddViewport(C2V(gD3DDevice2), C2V(gD3D_Viewport)) != D3D_OK) {
        d3d_debug_printf("Add viewport to device");
        C2V(gError_code) = 1;
        return 1;
    }
    d3d_debug_printf("Create/add viewport Device");
    SetD3DViewport();
    if (IDirect3DDevice2_SetCurrentViewport(C2V(gD3DDevice2), C2V(gD3D_Viewport)) != D3D_OK) {
        d3d_debug_printf("Set current viewport2");
        C2V(gError_code) = 1;
        return 1;
    }
    d3d_debug_printf("Set current viewport [%d]", C2V(gError_code));

    if (IDirect3D2_CreateMaterial(C2V(gDirect3D2), &C2V(gViewport_background_material), NULL) != D3D_OK) {
        d3d_debug_printf("Create background material");
        C2V(gError_code) = 1;
        return 1;
    }
    memset(&black_material, 0, sizeof(black_material));
    black_material.dwSize = sizeof(black_material);
    black_material.diffuse.b = 0.f;
    black_material.diffuse.g = 0.f;
    black_material.diffuse.r = 0.f;
    if (IDirect3DMaterial2_SetMaterial(C2V(gViewport_background_material), &black_material) != D3D_OK) {
        d3d_debug_printf("Set background material");
        C2V(gError_code) = 1;
        return 1;
    }
    if (IDirect3DMaterial2_GetHandle(C2V(gViewport_background_material), C2V(gD3DDevice2), &C2V(gViewport_background_material_handle)) != D3D_OK) {
        d3d_debug_printf("Get material handle");
        C2V(gError_code) = 1;
        return 1;
    }
    if (IDirect3DViewport2_SetBackground(C2V(gD3D_Viewport), C2V(gViewport_background_material_handle)) != D3D_OK) {
        d3d_debug_printf("Set viewport material");
        C2V(gError_code) = 1;
        return 1;
    }
    d3d_debug_printf("Set viewport material[%d]", C2V(gError_code));

    if (IDirect3DViewport2_Clear(C2V(gD3D_Viewport), 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER) != D3D_OK) {
        d3d_debug_printf("Clear viewport");
        C2V(gError_code) = 1;
        return 1;
    }

    memset(&blt_fx, 0, sizeof(blt_fx));
    blt_fx.dwSize = sizeof(blt_fx);
    IDirectDrawSurface_Blt(C2V(gPrimary_surface), NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blt_fx);

    memset(&blt_fx, 0, sizeof(blt_fx));
    blt_fx.dwSize = sizeof(blt_fx);
    IDirectDrawSurface_Blt(C2V(gAttached_surface), NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blt_fx);
    d3d_debug_printf("Clear viewport [%d]", C2V(gError_code));

    d3d_debug_printf("Enum Textures [%d]", C2V(gError_code));
    C2V(gAvailable_pixel_formats) = 0;
    C2V(gPreferred_texture_format) = 0;
    phase = 0;
    if (IDirect3DDevice2_EnumTextureFormats(C2V(gD3DDevice2), D3DEnumTextureFormats, &phase) != D3D_OK) {
        return 8;
    }
    switch (C2V(gPreferred_texture_format)) {
    case 1:
        d3d_debug_printf("Prefered Texture Format: 8bit pal");
        C2V(gTexture_format_is_NOT_ARGB4444) = 1;
        break;
    case 2:
        d3d_debug_printf("Prefered Texture Format: RGB555");
        C2V(gTexture_format_is_NOT_ARGB4444) = 1;
        break;
    case 4:
        d3d_debug_printf("Prefered Texture Format: RGB565");
        C2V(gTexture_format_is_NOT_ARGB4444) = 1;
        break;
    case 8:
        d3d_debug_printf("Prefered Texture Format: ARGB1555");
        C2V(gTexture_format_is_NOT_ARGB4444) = 1;
        break;
    case 16:
        d3d_debug_printf("Prefered Texture Format: ARGB4444");
        C2V(gTexture_format_is_NOT_ARGB4444) = 0;
        break;
    default:
        break;
    }
    if (C2V(gPreferred_texture_format) != C2V(gPreferred_opaque_texture_format)) {
        d3d_debug_printf("We also have a prefered opaqe format");
    }
    if (!(C2V(gRegister_flags) & 4)) {
        C2V(gTexture_format_is_NOT_ARGB4444) = 1;
    }
    StartTextureCache();
    d3d_debug_printf("Maximum Vert count=%d, max buf=%d, ULimit=%d-%d,VLimit=%d-%d",
        C2V(gDevice_description).dwMaxVertexCount,   C2V(gDevice_description).dwMaxBufferSize,
        C2V(gDevice_description).dwMinTextureWidth,  C2V(gDevice_description).dwMaxTextureWidth,
        C2V(gDevice_description).dwMaxTextureHeight, C2V(gDevice_description).dwMinTextureHeight);
    d3d_debug_printf("are we are now ready to Render? [%d]", C2V(gError_code));
    return 0;
}
C2_HOOK_FUNCTION(0x10005200, InitD3D)

void C2_HOOK_CDECL CloseD3D(void) {

    if (C2V(gD3DDevice2) != NULL) {
        IDirect3DDevice2_Release(C2V(gD3DDevice2));
        C2V(gD3DDevice2) = NULL;
    }
    if (C2V(gPrimary_surface) != NULL) {
        IDirectDrawSurface_Release(C2V(gPrimary_surface));
        C2V(gPrimary_surface) = NULL;
    }
    if (C2V(gDirect3D2) != NULL) {
        IDirect3D2_Release(C2V(gDirect3D2));
        C2V(gDirect3D2) = NULL;
    }
    if (C2V(gDirectDraw2) != NULL) {
        IDirectDraw2_Release(C2V(gDirectDraw2));
        C2V(gDirectDraw2) = NULL;
    }
    if (C2V(gDirect3D) != NULL) {
        IDirect3D_Release(C2V(gDirect3D));
        C2V(gDirect3D) = NULL;
    }
    if (C2V(gDirectDraw) != NULL) {
        IDirectDraw_Release(C2V(gDirectDraw));
        C2V(gDirectDraw) = NULL;
    }
    FreeLibrary(C2V(gModule_ddraw));
    ExitProcess(0);
}
C2_HOOK_FUNCTION(0x10005a50, CloseD3D)

void C2_HOOK_CDECL ApplyD3DRenderStateQueue(void) {
    int i;

    for (i = 0; i < C2V(gCount_queued_render_states); i++) {
        DWORD new_state_type = C2V(gQueued_render_states)[i].type;
        DWORD new_state = gQueued_render_states[i].value;
        if (!C2V(gCurrent_render_states)[new_state_type].initialized || C2V(gCurrent_render_states)[new_state_type].state != new_state) {
            C2V(gCurrent_render_states)[new_state_type].state = new_state;
            C2V(gCurrent_render_states)[new_state_type].initialized = 1;
            IDirect3DDevice2_SetRenderState(C2V(gD3DDevice2), new_state_type, new_state);
            C2V(gCount_render_state_changes) += 1;
        } else {
            C2V(gCount_skipped_render_state_changes) += 1;
        }
    }
    C2V(gCount_queued_render_states) = 0;
}
C2_HOOK_FUNCTION(0x10005000, ApplyD3DRenderStateQueue)

void C2_HOOK_CDECL DoD3DOperation(int operation) {

    switch(operation) {
    case 2:
        if (C2V(gIn_d3d_scene)) {
            return;
        }
        C2V(gIn_d3d_scene) = 1;
        if (IDirect3DDevice2_BeginScene(C2V(gD3DDevice2)) != D3D_OK) {
            d3d_debug_printf("SRS Begin scene");
            C2V(gError_code) = 1;
            return;
        }
        C2V(gCount_queued_render_states) = 0;
        QUEUE_RENDER_STATE(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZENABLE, 1);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, 1);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        QUEUE_RENDER_STATE(D3DCMP_LESSEQUAL, (C2V(gRegister_flags) & 0x8) != 0);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, 0);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_DITHERENABLE, (C2V(gRegister_flags) & 0x10) != 0);
        ApplyD3DRenderStateQueue();
        return;
    case 3:
        if (C2V(gIn_d3d_scene) != 1) {
            return;
        }
        C2V(gIn_d3d_scene) = 0;
        if (IDirect3DDevice2_EndScene(C2V(gD3DDevice2)) != D3D_OK) {
            d3d_debug_printf("SRS End scene");
            C2V(gError_code) = 1;
            return;
        }
        return;
    case 4:
        gUINT_1001bc7c = (gUINT_1001bc7c + 1) % 16;
        DoD3DOperation(3);
        if (C2V(gNo_2d_during_3d_scene) == 1 && C2V(gINT_1001bb98)) {
            DDSURFACEDESC surface_desc;

            memset(&surface_desc, 0, sizeof(surface_desc));
            surface_desc.dwSize = sizeof(surface_desc);
            if (IDirectDrawSurface_Lock(C2V(gAttached_surface), NULL, &surface_desc, DDLOCK_WAIT, NULL) == DD_OK) {
                int y;
                br_uint_16* src = C2V(gDevice_pixels);
                br_uint_16* dst = surface_desc.lpSurface;

                for (y = 0; y < 480; y++) {
                    int x;
                    br_uint_32 sum = 0;
                    br_uint_32 *src4 = (br_uint_32*)src;

                    for (x = 0; x < 640 / (4 * 2); x++) {
                        sum += src4[0] + src4[1] + src4[2] + src4[3];
                    }
                    if (sum != 0) {
                        for (x = 0; x < 640; x++) {
                            if (src[x] != 0) {
                                dst[x] = src[x];
                            }
                        }
                    }
                    memset(src, 0, 2 * 640);

                    src += 640;
                    dst = (br_uint_16*)((br_uint_8*)dst + surface_desc.lPitch);
                }
                IDirectDrawSurface_Unlock(C2V(gAttached_surface), NULL);
            }
        }
        if (IDirectDrawSurface_Flip(C2V(gPrimary_surface), NULL, DDFLIP_WAIT) != DD_OK) {
            d3d_debug_printf("Flip");
            C2V(gError_code) = 1;
            return;
        }
        C2V(gFlip_count) += 1;
        C2V(gINT_1001bc70) = 0;
        DoD3DOperation(5);
        DoD3DOperation(2);
        C2V(gINT_1001bbe0) = 0;
        C2V(gINT_1001bb94) = 0;
        C2V(gINT_1001bb98) = 0;
        C2V(gDirty_min_x) = 639;
        C2V(gDirty_min_y) = 479;
        C2V(gDirty_max_x) = 1;
        C2V(gDirty_max_y) = 1;
        return;
    case 5:
        {
            D3DRECT rects[1];
            rects[0].x1 = 0;
            rects[0].y1 = 0;
            rects[0].x2 = C2V(gDisplay_width_10015888);
            rects[0].y2 = C2V(gDisplay_height_1001588c);
            C2V(gINT_1001bc70) = 1;
            if (IDirect3DViewport2_Clear(C2V(gD3D_Viewport), BR_ASIZE(rects), rects, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER) != D3D_OK) {
                d3d_debug_printf("Clear viewport");
                C2V(gError_code) = 1;
                return;
            }
            return;
        }
    default:
        return;
    }
}
C2_HOOK_FUNCTION(0x10005b10, DoD3DOperation)

void FUN_10008570(void* dst, br_uint_32 dst_width, br_uint_32 dst_height, br_uint_32 dst_stride, void* src, br_uint_32 src_width, br_uint_32 src_height, br_uint_32 src_stride) {
    abort();
}
C2_HOOK_FUNCTION(0x10008570, FUN_10008570)

int C2_HOOK_CDECL d3d_update_texture(int index, render_buffer_d3d* buffer, int arg3) {
    DDSURFACEDESC surface_desc;
    int texture_format;

    C2V(gINT_1001bbe0) += 1;
    buffer->field_0x74 = -2;
    if (C2V(gPreferred_texture_format) != C2V(gPreferred_opaque_texture_format)) {
        if (buffer->blended == 1) {
            if (C2V(gBuffered_textures)[index].field_0x1c == 0) {
                C2V(gBuffered_textures)[index].field_0x1c = 1;
                C2V(gINT_1001bbc8) += 1;
                IDirect3DTexture2_Release(C2V(gBuffered_textures)[index].texture);
                IDirectDrawSurface_Release(C2V(gBuffered_textures)[index].surface);
                C2V(gBuffered_textures)[index].surface = NULL;
                C2V(gBuffered_textures)[index].texture = NULL;
                memcpy(&surface_desc, &C2V(gPreferred_texture_surface_description), sizeof(surface_desc));
                surface_desc.dwWidth = C2V(gBuffered_textures)[index].width;
                surface_desc.dwHeight = C2V(gBuffered_textures)[index].height;
                surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (C2V(gDevice_has_color_model) ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
                surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
                if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gBuffered_textures)[index].surface, NULL) != DD_OK) {
                    return -1;
                }
                if (IDirectDrawSurface_QueryInterface(C2V(gBuffered_textures)[index].surface, &IID_IDirect3DTexture2, (LPVOID)&C2V(gBuffered_textures)[index].texture) != DD_OK) {
                    return -1;
                }
                if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_texture) != DD_OK) {
                    return -1;
                }
                if (IDirect3DTexture2_GetHandle(C2V(gBuffered_textures)[index].texture, C2V(gD3DDevice2), &C2V(gBuffered_textures)[index].texture_handle) != DD_OK) {
                    return -1;
                }
            }
        } else if (gBuffered_textures[index].field_0x1c == 1) {
            C2V(gBuffered_textures)[index].field_0x1c = 0;
            C2V(gINT_1001bbc8) += 1;
            IDirect3DTexture2_Release(C2V(gBuffered_textures)[index].texture);
            IDirectDrawSurface_Release(C2V(gBuffered_textures)[index].surface);
            C2V(gBuffered_textures)[index].surface = NULL;
            C2V(gBuffered_textures)[index].texture = NULL;
            memcpy(&surface_desc, &C2V(gPreferred_opaque_texture_surface_description), sizeof(surface_desc));
            surface_desc.dwWidth = C2V(gBuffered_textures)[index].width;
            surface_desc.dwHeight = C2V(gBuffered_textures)[index].height;
            surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (C2V(gDevice_has_color_model) ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
            surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
            if (IDirectDraw_CreateSurface(C2V(gDirectDraw), &surface_desc, &C2V(gBuffered_textures)[index].surface, NULL) != DD_OK) {
                return -1;
            }
            if (IDirectDrawSurface_QueryInterface(C2V(gBuffered_textures)[index].surface, &IID_IDirect3DTexture2, (LPVOID)&C2V(gBuffered_textures)[index].texture) != DD_OK) {
                return -1;
            }
            if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_texture) != DD_OK) {
                return -1;
            }
            if (IDirect3DTexture2_GetHandle(C2V(gBuffered_textures)[index].texture, C2V(gD3DDevice2), &C2V(gBuffered_textures)[index].texture_handle) != DD_OK) {
                return -1;
            }
        }
    }
    C2V(gINT_1001bbe8) += 1;
    buffer->field_0x58 = 0;
    if (C2V(gBuffered_textures)[index].field_0x24 == C2V(gFlip_count)) {
        IDirect3DDevice2_SetRenderState(C2V(gD3DDevice2), D3DRENDERSTATE_FLUSHBATCH, 0);
    }
    memset(&surface_desc, 0, sizeof(surface_desc));
    surface_desc.dwSize = sizeof(surface_desc);
    if (C2V(gBuffered_textures)[index].field_0x1c) {
        if (IDirectDrawSurface_Lock(C2V(gBuffered_textures)[index].temp_surface, NULL, &surface_desc, 0, NULL) != DD_OK) {
            d3d_debug_printf("Lock Temporary Texture Surface");
            return 1;
        }
    } else {
        if (IDirectDrawSurface_Lock(C2V(gBuffered_textures)[index].temp_opaque_surface, NULL, &surface_desc, 0, NULL) != DD_OK) {
            d3d_debug_printf("Lock Temporary Texture Surface");
            return 1;
        }
    }
    texture_format = C2V(gBuffered_textures)[index].field_0x1c ? C2V(gPreferred_texture_format) : C2V(gPreferred_opaque_texture_format);
    if (buffer->pm_type == 3 && buffer->width_p == 64 && buffer->height == 64) {
        br_colour* pal_ptr = buffer->palette_pointer;
        int pal_i;
        br_uint_8* read_row_ptr;
        br_uint_8* write_row_ptr;
        int y;

        switch (texture_format) {
        case 2:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                C2V(gD3D_16bit_palette_buffer)[pal_i] = ((c & 0xf80000) >> 9) | ((c & 0xf800) >> 6) | ((c & 0xff) >> 3);
            }
            break;
        case 4:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                C2V(gD3D_16bit_palette_buffer)[pal_i] = ((c & 0xf80000) >> 8) | ((c & 0xfc00) >> 5) | ((c & 0xff) >> 3);
            }
            break;
        case 8:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                C2V(gD3D_16bit_palette_buffer)[pal_i] = 0x1000 | ((c & 0xf80000) >> 9) | ((c & 0xf800) >> 5) | ((c & 0xff) >> 3);
            }
            break;
        case 16:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                C2V(gD3D_16bit_palette_buffer)[pal_i] = 0xf000 | ((c & 0xf00000) >> 12) | ((c & 0xf000) >> 8) | ((c & 0xf0) >> 4);
            }
            break;
        }

        write_row_ptr = surface_desc.lpSurface;
        read_row_ptr = buffer->field_0x68;
        for (y = 0; y < 64; y++) {
            int x;
            br_uint_16* write_ptr = (br_uint_16*)read_row_ptr;
            br_uint_8* read_ptr = read_row_ptr;
            for (x = 0; x < 64; x++, read_ptr++, write_ptr++) {
                *write_ptr = C2V(gD3D_16bit_palette_buffer)[*read_ptr];
            }
            write_row_ptr += surface_desc.lPitch;
            read_row_ptr += buffer->width_b;
        }
        if (C2V(gBuffered_textures)[index].field_0x1c) {
            /* FIXME: original uses temp_opaque_texture everywhere */
            if (IDirectDrawSurface_Unlock(C2V(gBuffered_textures)[index].temp_surface, NULL) != DD_OK) {
                d3d_debug_printf("UNLock Temporary Texture Surface");
                return 1;
            }
            if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_opaque_texture) != D3D_OK) {
                d3d_debug_printf("Load Temporary Texture Surface");
                return 1;
            }
        } else {
            if (IDirectDrawSurface_Unlock(C2V(gBuffered_textures)[index].temp_opaque_surface, NULL) != DD_OK) {
                d3d_debug_printf("UNLock Temporary Texture Surface");
                return 1;
            }
            if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_opaque_texture) != D3D_OK) {
                d3d_debug_printf("Load Temporary Texture Surface");
                return 1;
            }
        }
        C2V(gBuffered_textures)[index].field_0x20 = buffer->uid;
        buffer->field_0x4c = index;
        C2V(gBuffered_textures)[index].field_0x24 = C2V(gFlip_count);
        return 0;
    }
    if (buffer->width_p == surface_desc.dwWidth && buffer->height == surface_desc.dwHeight) {
        br_uint_32 y;
        br_uint_8* read_ptr = buffer->field_0x68;
        br_uint_8* write_ptr = surface_desc.lpSurface;
        for (y = 0; y < buffer->height; y++, write_ptr += surface_desc.lPitch, read_ptr += buffer->width_b) {
            memcpy(write_ptr, read_ptr, 2 * surface_desc.dwWidth);
        }
    } else {
        FUN_10008570(surface_desc.lpSurface, surface_desc.dwWidth, surface_desc.dwHeight, surface_desc.lPitch,
            buffer->field_0x68, buffer->width_p, buffer->height, buffer->width_b);
    }
    if (C2V(gTexture_format_is_NOT_ARGB4444) && (buffer->pm_type == BR_PMT_RGBA_4444 || buffer->pm_type == BR_PMT_ARGB_1555)) {
        if (arg3 == -2) {
            br_uint_32 y;
            br_uint_8* row_ptr = surface_desc.lpSurface;
            switch (texture_format) {
            case 2:
            case 8:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, ptr++) {
                        if (buffer->pm_type == BR_PMT_RGBA_4444) {
                            br_uint_16 c = *ptr;
                            br_uint_32 a = (c & 0xf000) >> 12;
                            /* RGBA4444 -> RGB555 */
                            *ptr = (((a * (c & 0xf00)) >> 11) << 10) | (((a * (c & 0xf0)) >> 7) << 5) | ((a * (c & 0xf)) >> 3);
                            if (a < 25) {
                                *ptr = 0;
                            }
                        }
                    }
                }
                break;
            case 4:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, ptr++) {
                        if (buffer->pm_type == BR_PMT_RGBA_4444) {
                            br_uint_16 c = *ptr;
                            br_uint_32 a = (c & 0xf000) >> 12;
                            /* RGBA4444 -> RGB565 */
                            *ptr = (((a * (c & 0xf00)) >> 11) << 11) | (((a * (c & 0xf0)) >> 6) << 5) | ((a * (c & 0xf)) >> 3);
                            if (a < 25) {
                                *ptr = 0;
                            }
                        } else if (buffer->pm_type == BR_PMT_ARGB_1555) {
                            /* RGB555 -> RGB565 */
                            /* FIXME: original converts to RBB565 */
                            br_uint_16 c = *ptr;
                            *ptr = ((c & 0x7fe0) << 1) | (c & 0x1f);
                        }
                    }
                }
                break;
            }
        }
        else {
            br_uint_8* row_ptr = surface_desc.lpSurface;
            br_uint_32 y;
            C2V(gDAT_1001bc6c) = 0;
            switch (texture_format) {
            case 2:
            case 8:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, ptr++) {
                        if (buffer->pm_type == BR_PMT_RGBA_4444) {
                            br_uint_16 c = *ptr;
                            br_uint_32 a = ((c & 0xf000) >> 12) * 0x11;
                            if (arg3 >= 0) {
                                a = (a * arg3) >> 8;
                            }
                            /* RGBA4444 -> RGB555 */
                            *ptr = ((c & 0xf00) << 3) | ((c & 0xf0) << 2) | ((c & 0xf) << 1);
                            if (*ptr == 0) {
                                *ptr = 1;
                            }
                            if (C2V(gTextures_alpha_init)[0][16 * ((a ^ y) & 0xf ^ a) + (x & 0xf)] == 0) {
                                *ptr = 0;
                            }
                        }
                    }
                }
                break;
            case 4:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, ptr++) {
                        if (buffer->pm_type == BR_PMT_RGBA_4444) {
                            br_uint_16 c = *ptr;
                            br_uint_32 a = ((c & 0xf000) >> 12) * 0x11;
                            if (arg3 >= 0) {
                                a = (a * arg3) >> 8;
                            }
                            /* RGBA4444 -> RGB565 */
                            *ptr = ((c & 0xf00) << 4) | ((c & 0xf0) << 3) | ((c & 0xf) << 1);
                            if (*ptr == 0) {
                                *ptr = 1;
                            }
                            if (C2V(gTextures_alpha_init)[0][16 * ((a ^ y) & 0xf ^ a) + (x & 0xf)] == 0) {
                                *ptr = 0;
                            }
                        } else if (buffer->pm_type == BR_PMT_ARGB_1555) {
                            /* RGB555 -> RGB565 */
                            /* FIXME: original converts to RBB565 */
                            br_uint_16 c = *ptr;
                            *ptr = ((c & 0x7fe0) << 1) | (c & 0x1f);
                        }
                    }
                }
                break;
            }
        }
    }
    if (C2V(gTexture_format_is_NOT_ARGB4444) && (buffer->pm_type == BR_PMT_RGBA_4444 || buffer->pm_type == BR_PMT_ARGB_1555)) {
        int extra_processing = 0;
        switch (texture_format) {
        case 2:
        case 8:
            switch (buffer->pm_type) {
            case 5:
            case 18:
            case 31:
                extra_processing = 1;
                break;
            }
            break;
        case 4:
            switch (buffer->pm_type) {
            case 4:
            case 18:
            case 31:
                extra_processing = 1;
                break;
            }
            break;
        case 16:
            switch (buffer->pm_type) {
            case 4:
            case 5:
            case 31:
                extra_processing = 1;
                break;
            case 18:
                if (C2V(gNo_2d_during_3d_scene)) {
                    extra_processing = 1;
                }
                break;
            }
            break;
        }
        if (extra_processing) {
            br_uint_32 y;
            br_uint_8* row_ptr = surface_desc.lpSurface;
            switch (texture_format) {
            case 2:
            case 8:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, row_ptr++) {
                        br_uint_16 c;
                        switch (buffer->pm_type) {
                        case BR_PMT_RGB_555:
                            *ptr |= 0x8000;
                            break;
                        case BR_PMT_RGB_565:
                            c = *ptr;
                            *ptr = 0x8000 | ((c & 0xffc0) >> 1) | (c & 0x1f);
                            break;
                        case BR_PMT_RGBA_4444:
                            c = *ptr;
                            *ptr = (c & 0x8000) | ((c & 0xf00) << 3) | ((c & 0xf0) << 2) | ((c & 0xf) << 1);
                            break;
                        }
                    }
                }
                break;
            case 4:
                for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                    br_uint_32 x;
                    br_uint_16* ptr = (br_uint_16*)row_ptr;
                    for (x = 0; x < surface_desc.dwWidth; x++, row_ptr++) {
                        br_uint_16 c;
                        /* FIXME: original does RGB -> RBB */
                        switch (buffer->pm_type) {
                        case BR_PMT_RGB_555:
                            /* 555 -> 565*/
                            c = *ptr;
                            *ptr = (c & 0x1f) * 0x21 | (c & 0x7c00) * 2;
                            break;
                        case BR_PMT_RGBA_4444:
                            c = *ptr;
                            *ptr = ((c & 0xf00) << 3) | ((c & 0xf0) << 2) | ((c & 0xf) << 1);
                            break;
                        case BR_PMT_ARGB_1555:
                            c = *ptr;
                            *ptr = (c & 0x1f) * 0x21 | (c & 0x7c00) * 2;
                            break;
                        }
                    }
                }
                break;
            case 16:
                {
                    int min_x = 6000;
                    int max_x = -1;
                    int min_y = 6000;
                    int max_y = -1;
                    for (y = 0; y < surface_desc.dwHeight; y++, row_ptr += surface_desc.lPitch) {
                        br_uint_32 x;
                        br_uint_16 *ptr = (br_uint_16*)row_ptr;
                        for (x = 0; x < surface_desc.dwWidth; x++, row_ptr++) {
                            br_uint_16 c;
                            switch (buffer->pm_type) {
                            case BR_PMT_RGB_555:
                                /* RGB555 -> ARGB4444 */
                                c = *ptr;
                                *ptr = ((c & 0x7800) >> 3) | ((c >> 2) & 0xf0) | ((c >> 1) & 0xf);
                                break;
                            case BR_PMT_RGB_565:
                                /* RGB565 -> ARGB4444 */
                                c = *ptr;
                                *ptr = ((c & 0xf000) >> 4) | ((c >> 3) & 0xf0) | ((c >> 1) & 0xf);
                                break;
                            case BR_PMT_RGBA_4444:
                                c = *ptr;
                                if ((c & 0xf000) <= 0x3000) {
                                    if ((int)x < min_x) {
                                        min_x = x;
                                    }
                                    if ((int)x > max_x) {
                                        max_x = x;
                                    }
                                    if ((int)y < min_y) {
                                        min_y = y;
                                    }
                                    if ((int)y > max_y) {
                                        max_y = y;
                                    }
                                }
                                break;
                            case BR_PMT_ARGB_1555:
                                c = *ptr;
                                *ptr = ((c & 0x8000) ? 0xf000 : 0) | ((c >> 3) & 0xf00) | ((c >> 2) & 0xf0) | ((c >> 1) & 0xf);
                                break;
                            }
                        }
                    }
                    buffer->field_0x6c = min_x;
                    buffer->field_0x74 = max_x;
                    buffer->field_0x70 = min_y;
                    buffer->field_0x78 = max_y;
                    break;
                }
            }
        }
    }
    if (C2V(gBuffered_textures)[index].field_0x1c) {
        if (IDirectDrawSurface_Unlock(C2V(gBuffered_textures)[index].temp_surface, NULL) != DD_OK) {
            d3d_debug_printf("UNLock Temporary Texture Surface");
            return 1;
        }
        if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_texture) != D3D_OK) {
            d3d_debug_printf("Load Temporary Texture Surface");
            return 1;
        }
    } else {
        if (IDirectDrawSurface_Unlock(C2V(gBuffered_textures)[index].temp_opaque_surface, NULL) != DD_OK) {
            d3d_debug_printf("UNLock Temporary Texture Surface");
            return 1;
        }
        if (IDirect3DTexture2_Load(C2V(gBuffered_textures)[index].texture, C2V(gBuffered_textures)[index].temp_opaque_texture) != D3D_OK) {
            d3d_debug_printf("Load Temporary Texture Surface");
            return 1;
        }
    }
    if (C2V(gTexture_format_is_NOT_ARGB4444)) {
        DDCOLORKEY color_key;

        color_key.dwColorSpaceHighValue = 0;
        color_key.dwColorSpaceLowValue = 0;
        if (IDirectDrawSurface_SetColorKey(C2V(gBuffered_textures)[index].surface, 8, &color_key) != DD_OK) {
            d3d_debug_printf("Set color key");
            return 1;
        }
    }
    C2V(gBuffered_textures)[index].field_0x20 = buffer->uid;
    buffer->field_0x4c = index;
    C2V(gBuffered_textures)[index].field_0x24 = C2V(gFlip_count);
    return 0;
}
C2_HOOK_FUNCTION(0x10002ba0, d3d_update_texture)

int C2_HOOK_CDECL d3d_load(render_buffer_d3d* buffer, int arg2) {
    br_uint_8 shift1;
    int w_p_shift1;
    int h_p_shift1;
    int size_i;
    int slot_index;
    int tex_w;
    int tex_h;

    C2V(gINT_1001bbe4) += 1;
    C2V(gDWORD_1001bbb0) = 0;
    if (buffer == NULL) {
        d3d_debug_printf("Pixel map pointer was null");
        return 1;
    }
    if (buffer->field_0x68 == NULL) {
        d3d_debug_printf("pixel map data was null");
        return 1;
    }

    shift1 = (br_uint_8)C2V(gINT_1001bbd0);
    w_p_shift1 = buffer->width_p >> shift1;
    w_p_shift1 = D3D_MAX(16 >> shift1, w_p_shift1);

    h_p_shift1 = buffer->height >> shift1;
    h_p_shift1 = D3D_MAX(16 >> shift1, h_p_shift1);

    if (buffer->field_0x5c != C2V(gFlip_count)) {
        buffer->field_0x5c = C2V(gFlip_count);
    }

    if (buffer->field_0x4c != -1 && C2V(gBuffered_textures)[buffer->field_0x4c].field_0x20 == buffer->uid) {
        if (buffer->field_0x58 == 1 && d3d_update_texture(buffer->field_0x4c, buffer, arg2) != 0) {
                d3d_debug_printf("Call to d3d_load to update texture %d failed", buffer->field_0x4c);
                return 1;
        }
        C2V(gDWORD_1001bbb0) = C2V(gBuffered_textures)[buffer->field_0x4c].texture_handle;
        C2V(gBuffered_textures)[buffer->field_0x4c].field_0x24 = C2V(gFlip_count);
        return 0;
    }

    size_i = -1;
    for (;;) {
        int i;
        int new_h_shift = 0;

        for (i = 0; C2V(gStruct_10015898_ARRAY)[i].field_0x8 != 0; i++) {
            if (C2V(gStruct_10015898_ARRAY)[i].count != 0) {
                if (new_h_shift < 10 && w_p_shift1 == C2V(gStruct_10015898_ARRAY)[i].width && h_p_shift1 == C2V(gStruct_10015898_ARRAY)[i].height) {
                    new_h_shift = 10;
                    size_i = i;
                }
                if (new_h_shift < 9 && w_p_shift1 == C2V(gStruct_10015898_ARRAY)[i].width && h_p_shift1 < C2V(gStruct_10015898_ARRAY)[i].height) {
                    new_h_shift = 9;
                    size_i = i;
                }
                if (new_h_shift < 8 && w_p_shift1 < C2V(gStruct_10015898_ARRAY)[i].width && h_p_shift1 == C2V(gStruct_10015898_ARRAY)[i].height) {
                    new_h_shift = 8;
                    size_i = i;
                }
            }
        }
        if (size_i != -1) {
            break;
        }
        w_p_shift1 >>= 1;
        h_p_shift1 >>= 1;
        w_p_shift1 = D3D_MAX(w_p_shift1, 16 >> shift1);
        h_p_shift1 = D3D_MAX(h_p_shift1, 16 >> shift1);
    }
    tex_w = C2V(gStruct_10015898_ARRAY)[size_i].width >> shift1;
    tex_h = C2V(gStruct_10015898_ARRAY)[size_i].height >> shift1;
    slot_index = -1;
    if (C2V(gPreferred_opaque_texture_format) == C2V(gPreferred_texture_format)) {
        int i;
        int oldest_flip_count = C2V(gFlip_count) + 1;

        for (i = 0; i < C2V(gSize_texture_cache); i++) {
            if (C2V(gBuffered_textures)[i].texture != NULL
                    && C2V(gBuffered_textures)[i].width == tex_w
                    && C2V(gBuffered_textures)[i].height == tex_h
                    && C2V(gBuffered_textures)[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = C2V(gBuffered_textures)[i].field_0x24;
            }
        }
    } else if (buffer->blended == 1) {
        int i;
        int oldest_flip_count = C2V(gFlip_count) + 1;

        for (i = 0; i < C2V(gSize_texture_cache); i++) {
            if (C2V(gBuffered_textures)[i].field_0x1c == 0) {
                C2V(gBuffered_textures)[i].field_0x24 -= 2;
            }
            if (C2V(gBuffered_textures)[i].texture != NULL
                    && C2V(gBuffered_textures)[i].width == tex_w
                    && C2V(gBuffered_textures)[i].height == tex_h
                    && C2V(gBuffered_textures)[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = C2V(gBuffered_textures)[i].field_0x24;
            }
            if (C2V(gBuffered_textures)[i].field_0x1c == 0) {
                C2V(gBuffered_textures)[i].field_0x24 += 2;
            }
        }
    } else {
        int i;
        int oldest_flip_count = C2V(gFlip_count) + 1;

        for (i = 0; i < C2V(gSize_texture_cache); i++) {
            if (C2V(gBuffered_textures)[i].field_0x1c == 1) {
                C2V(gBuffered_textures)[i].field_0x24 -= 2;
            }
            if (C2V(gBuffered_textures)[i].texture != NULL
                    && C2V(gBuffered_textures)[i].width == tex_w
                    && C2V(gBuffered_textures)[i].height == tex_h
                    && C2V(gBuffered_textures)[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = C2V(gBuffered_textures)[i].field_0x24;
            }
            if (C2V(gBuffered_textures)[i].field_0x1c == 1) {
                C2V(gBuffered_textures)[i].field_0x24 += 2;
            }
        }
    }

    if (C2V(gNo_2d_during_3d_scene) == 1 && (slot_index == -1 || C2V(gFlip_count) <= C2V(gBuffered_textures)[slot_index].field_0x24)) {
        int i;
        int oldest_flip_count = C2V(gFlip_count) + 1;

        slot_index = -1;
        for (i = 0; i < C2V(gSize_texture_cache); i++) {
            if (C2V(gBuffered_textures)[i].texture != NULL
                    && C2V(gBuffered_textures)[i].width <= tex_w
                    && C2V(gBuffered_textures)[i].height <= tex_h
                    && C2V(gBuffered_textures)[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = C2V(gBuffered_textures)[i].field_0x24;
            }
        }
        if (slot_index == -1 || gBuffered_textures[slot_index].field_0x24 >= C2V(gFlip_count)) {
            slot_index = -1;
            for (i = 0; i < C2V(gSize_texture_cache); i++) {
                if (C2V(gBuffered_textures)[i].texture != NULL
                        && C2V(gBuffered_textures)[i].field_0x24 < oldest_flip_count) {
                    slot_index = i;
                    oldest_flip_count = C2V(gBuffered_textures)[i].field_0x24;
                }
            }
        }
        if (slot_index == -1 || gBuffered_textures[slot_index].field_0x24 >= C2V(gFlip_count)) {
            return -1;
        }
    }
    if (slot_index == -1) {
        d3d_debug_printf("Cant find slot for %dx%d / %dx%d texture",
            buffer->width_p, buffer->height, tex_w, tex_h);
        return 1;
    }
    if (d3d_update_texture(slot_index, buffer, arg2) != 0) {
        d3d_debug_printf("Call to d3d_load to load texture %d failed", slot_index);
        return 1;
    }
    C2V(gDWORD_1001bbb0) = C2V(gBuffered_textures)[slot_index].texture_handle;
    return 0;
}
C2_HOOK_FUNCTION(0x10003bd0, d3d_load)

void C2_HOOK_CDECL FUN_10007140(void) {

    if (C2V(gCount_queued_vertices) == 0) {
        return;
    }
    if (C2V(gIn_d3d_scene) != 1) {
        DoD3DOperation(2);
    }
    if (C2V(gCount_queued_render_states) != 0) {
        ApplyD3DRenderStateQueue();
    }
    if (C2V(gNo_2d_during_3d_scene) == 1) {
        if (C2V(gINT_1001bba4) == 9) {
            C2V(gCount_queued_vertices) = 0;
            C2V(gCount_queued_vertices_indices) = 0;
            return;
        } else if (C2V(gINT_1001bba4) == 2) {
            if (C2V(gCount_queued_vertices) == 6) {
                int i;
                WORD fan[4] = { 0 };

                for (i = 0; i < C2V(gCount_queued_vertices); i++) {
                    if (C2V(gQueued_vertices)[i].tu > 1.5f) {
                        C2V(gCount_queued_vertices) = 0;
                        C2V(gCount_queued_vertices_indices) = 0;
                        return;
                    }
                    if (C2V(gQueued_vertices)[i].sx <= C2V(gQueued_vertices)[fan[0]].sx
                            && C2V(gQueued_vertices)[i].sy <= C2V(gQueued_vertices)[fan[0]].sy) {
                        fan[0] = i;
                    }
                    if (C2V(gQueued_vertices)[i].sx >= C2V(gQueued_vertices)[fan[1]].sx
                            && C2V(gQueued_vertices)[i].sy <= C2V(gQueued_vertices)[fan[1]].sy) {
                        fan[1] = i;
                    }
                    if (C2V(gQueued_vertices)[i].sx >= gQueued_vertices[fan[2]].sx
                            && C2V(gQueued_vertices)[i].sy >= gQueued_vertices[fan[2]].sy) {
                        fan[2] = i;
                    }
                    if (C2V(gQueued_vertices)[i].sx <= gQueued_vertices[fan[3]].sx
                            && C2V(gQueued_vertices)[i].sy >= gQueued_vertices[fan[3]].sy) {
                        fan[3] = i;
                    }
                    if (C2V(gQueued_vertices)[i].sx < 1.f) {
                        C2V(gQueued_vertices)[i].sx = 1.f;
                    }
                    C2V(gQueued_vertices)[i].color = C2V(gQueued_vertices)[0].color;
                    C2V(gQueued_vertices)[i].sz = C2V(gQueued_vertices)[0].sz;
                    C2V(gQueued_vertices)[i].specular = C2V(gQueued_vertices)[0].specular;
                }
                IDirect3DDevice2_DrawIndexedPrimitive(C2V(gD3DDevice2),
                    D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, C2V(gQueued_vertices), 6,
                    fan, BR_ASIZE(fan),
                    D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
                C2V(gCount_queued_vertices_indices) = 0;
                C2V(gCount_queued_vertices) = 0;
                return;
            }
        } else if (C2V(gINT_1001bba4) == 1 && C2V(gCount_queued_vertices) == 6 && C2V(gDAT_1001dc50) != NULL) {
            int u, v, du, dv, u_max, v_max;
            float factor_x, factor_y;
            br_uint_8* map_ptr;

            C2V(gQueued_vertices_indices)[0] = 3;
            C2V(gQueued_vertices_indices)[1] = 0;
            C2V(gQueued_vertices_indices)[2] = 1;
            C2V(gQueued_vertices_indices)[3] = 2;
            C2V(gQueued_vertices_indices)[4] = 3;
            C2V(gINT_1001dc6c) = (int)(C2V(gQueued_vertices)[3].tu * (float)C2V(gINT_1001dc4c));
            C2V(gINT_1001dc5c) = (int)(C2V(gQueued_vertices)[3].tv * (float)C2V(gINT_1001dc44));
            C2V(gINT_1001dc68) = (int)(C2V(gQueued_vertices)[1].tu * (float)C2V(gINT_1001dc4c));
            C2V(gINT_1001dc58) = (int)(C2V(gQueued_vertices)[1].tv * (float)C2V(gINT_1001dc44));
            du = C2V(gINT_1001dc68) - C2V(gINT_1001dc6c);
            dv = C2V(gINT_1001dc58) - C2V(gINT_1001dc5c);
            map_ptr = (br_uint_8*)C2V(gDAT_1001dc50) + C2V(gINT_1001dc5c) * C2V(gINT_1001dc54) + 2 * C2V(gINT_1001dc6c);
            u_max = 0;
            v_max = 0;
            for (v = 0; v < dv; v += 1) {
                br_uint_16* p = (br_uint_16*)map_ptr;
                for (u = 0; u < du; u += 1) {
                    if ((p[u] >> 12) > 3) {
                        if (u >= u_max) {
                            u_max = u;
                        }
                        if (v >= v_max) {
                            v_max = v;
                        }
                    }
                }
                map_ptr += C2V(gINT_1001dc54);
            }
            C2V(gINT_1001dc68) = C2V(gINT_1001dc6c) + u_max + 1;
            C2V(gINT_1001dc58) = C2V(gINT_1001dc5c) + v_max + 1;
            if (du == 0 || dv == 0.0) {
                factor_x = 0.f;
                factor_y = 0.f;
            }
            else {
                factor_x = (float)(u_max + 1) / (float)du;
                factor_y = (float)(v_max + 1) / (float)dv;
            }
            C2V(gQueued_vertices)[0].sx = factor_x * (C2V(gQueued_vertices)[1].sx - C2V(gQueued_vertices)[3].sx) + C2V(gQueued_vertices)[3].sx;
            C2V(gQueued_vertices)[0].tu = (float)C2V(gINT_1001dc68) / (float)C2V(gINT_1001dc4c);
            C2V(gQueued_vertices)[0].tv = (float)C2V(gINT_1001dc5c) / (float)C2V(gINT_1001dc44);
            C2V(gQueued_vertices)[0].color = C2V(gQueued_vertices)[3].color;

            C2V(gQueued_vertices)[1].sx = C2V(gQueued_vertices)[0].sx;
            C2V(gQueued_vertices)[1].sy = factor_y * (C2V(gQueued_vertices)[1].sy - C2V(gQueued_vertices)[3].sy) + C2V(gQueued_vertices)[3].sy;
            C2V(gQueued_vertices)[1].tv = (float)C2V(gINT_1001dc58) / (float)C2V(gINT_1001dc44);
            C2V(gQueued_vertices)[1].tu = C2V(gQueued_vertices)[0].tu;
            C2V(gQueued_vertices)[1].color = C2V(gQueued_vertices)[3].color;

            C2V(gQueued_vertices)[2].sy = C2V(gQueued_vertices)[1].sy;
            C2V(gQueued_vertices)[2].tu = (float)C2V(gINT_1001dc6c) / (float)C2V(gINT_1001dc4c);
            C2V(gQueued_vertices)[2].tv = C2V(gQueued_vertices)[1].tv;
            C2V(gQueued_vertices)[2].color = C2V(gQueued_vertices)[3].color;

            C2V(gQueued_vertices)[3].tu = C2V(gQueued_vertices)[2].tu;
            C2V(gQueued_vertices)[3].tv = C2V(gQueued_vertices)[0].tv;

            C2V(gCount_queued_vertices_indices) = 4;
            IDirect3DDevice2_DrawIndexedPrimitive(C2V(gD3DDevice2), D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                C2V(gQueued_vertices), C2V(gCount_queued_vertices),
                C2V(gQueued_vertices_indices), 4,
                D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
            C2V(gCount_queued_vertices_indices) = 0;
            C2V(gCount_queued_vertices) = 0;
            return;
        } else if (C2V(gINT_1001bba4) == 3) {
            QUEUE_RENDER_STATE(1, 0);
            QUEUE_RENDER_STATE(9, 1);
            QUEUE_RENDER_STATE(0x1b, 0);
            if (C2V(gCount_queued_render_states) != 0) {
                ApplyD3DRenderStateQueue();
            }
        } else {
            C2V(gINT_100389ec) += C2V(gCount_queued_vertices);
            C2V(gINT_100381a4) += C2V(gCount_queued_vertices_indices);
        }
    } else {
        C2V(gINT_100389ec) += C2V(gCount_queued_vertices);
        C2V(gINT_100381a4) += C2V(gCount_queued_vertices_indices);
    }
    IDirect3DDevice2_DrawIndexedPrimitive(C2V(gD3DDevice2), C2V(gPrimitive_type), D3DVT_TLVERTEX,
        C2V(gQueued_vertices), C2V(gCount_queued_vertices),
        C2V(gQueued_vertices_indices), C2V(gCount_queued_vertices_indices),
        C2V(gDWORD_1001bc30) | D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
    C2V(gCount_queued_vertices_indices) = 0;
    C2V(gCount_queued_vertices) = 0;
}
C2_HOOK_FUNCTION(0x10007140, FUN_10007140)

void C2_HOOK_CDECL FUN_10004780(void* arg1, int stride) {
    abort();
}
C2_HOOK_FUNCTION(0x10004780, FUN_10004780)
