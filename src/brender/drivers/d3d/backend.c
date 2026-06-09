#define INITGUID

#include "backend.h"

#include <brender/brender.h>

#include <stdio.h>
#include <stdlib.h>
#include "c2_string.h"

#include <windows.h>
#include <d3d.h>
#include <ddraw.h>

typedef HRESULT WINAPI tDirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
typedef HRESULT WINAPI tDirectDrawEnumerateA(LPDDENUMCALLBACKA lpCallback, LPVOID lpContext);


// GLOBAL: D3D 0x1001bbac
void* gDevice_pixels;

// GLOBAL: D3D 0x10038a80
br_uint_32 gPixel_buffer[640 * 480];

// GLOBAL: D3D 0x1001bbdc
HWND gActive_window;

// GLOBAL: D3D 0x10038a70
WNDPROC gOriginal_wndproc;

// GLOBAL: D3D 0x1001bc34
LPDIRECTDRAWSURFACE gPrimary_surface;

// GLOBAL: D3D 0x1001bc3c
LPDIRECTDRAWSURFACE gAttached_surface;

// GLOBAL: D3D 0x1001bc44
LPDIRECTDRAWSURFACE gDepth_surface;

// GLOBAL: D3D 0x1001bc0c
LPDIRECTDRAW gDirectDraw;

// GLOBAL: D3D 0x1001bc10
LPDIRECTDRAW2 gDirectDraw2;

// GLOBAL: D3D 0x100381a0
HMODULE gModule_ddraw;

// GLOBAL: D3D 0x1001dcc0
tStruct_1001dcc0 gBuffered_textures[128]; /* FIXME: unknown size */

// GLOBAL: D3D 0x10038900
tDirectDrawCreate* gDirectDrawCreate;

// GLOBAL: D3D 0x10038904
tDirectDrawEnumerateA* gDirectDrawEnumerateA;

// GLOBAL: D3D 0x1001bbec
int gError_code;

// GLOBAL: D3D 0x10038000
int gRegister_driver_guid_binary;

// GLOBAL: D3D 0x10015890
DWORD gRegister_flags = 0xffffffff;

// GLOBAL: D3D 0x10038540
GUID gRegister_device_guid;

// GLOBAL: D3D 0x10038190
GUID gRegister_driver_guid;

// GLOBAL: D3D 0x1001bc58
int gRegister_guid_valid;

// GLOBAL: D3D 0x1001dc48
int gCount_ddraw_drivers;

// GLOBAL: D3D 0x1001dc3c
int gIndex_ddraw_driver;

// GLOBAL: D3D 0x1001bc5c
int gDDraw_driver_found;

// GLOBAL: D3D 0x1001bb9c
int gNo_2d_during_3d_scene;

// GLOBAL: D3D 0x1001bc14
LPDIRECT3D gDirect3D;

// GLOBAL: D3D 0x1001dc64
br_uint_32 g640x480x16_display_mode_flags;

// GLOBAL: D3D 0x10038a6c
int gCount_display_modes;

// GLOBAL: D3D 0x1003890c
int gSelected_display_mode;

// GLOBAL: D3D 0x1001bc18
LPDIRECT3D2 gDirect3D2;

// GLOBAL: D3D 0x10038908
int gCount_d3d_devices;

// GLOBAL: D3D 0x10038550
int gSelected_d3d_device;

// GLOBAL: D3D 0x10038520
DDPIXELFORMAT gAttached_surface_pixel_format;

// GLOBAL: D3D 0x1001bbb8
int gPixel_format_is_xrgb1555;

// GLOBAL: D3D 0x10015888
int gDisplay_width_10015888;

// GLOBAL: D3D 0x1001588c
int gDisplay_height_1001588c;

// GLOBAL: D3D 0x10037cc0
int gDisplay_width_10037cc0;

// GLOBAL: D3D 0x10037cc4
int gDisplay_height_10037cc4;

// GLOBAL: D3D 0x10038004
int gDisplay_width_10038004;

// GLOBAL: D3D 0x10038008
int gDisplay_height_10038008;

// GLOBAL: D3D 0x10037cc8
int gDevice_has_color_model;

// GLOBAL: D3D 0x10038920
D3DDEVICEDESC gDevice_description;

// GLOBAL: D3D 0x1001dca0
GUID gDevice_guid;

// GLOBAL: D3D 0x1001bc24
LPDIRECT3DDEVICE2 gD3DDevice2;

// GLOBAL: D3D 0x1001bc48
LPDIRECT3DVIEWPORT2 gD3D_Viewport;

// GLOBAL: D3D 0x1001bc2c
LPDIRECT3DMATERIAL2 gViewport_background_material;

// GLOBAL: D3D 0x1003810c
DWORD gViewport_background_material_handle;

// GLOBAL: D3D 0x1001bc54
br_uint_32 gAvailable_pixel_formats;

// GLOBAL: D3D 0x1001dc60
int gPreferred_texture_format;

// GLOBAL: D3D 0x1001bbcc
int gTexture_format_is_NOT_ARGB4444;

// GLOBAL: D3D 0x1003818c
int gPreferred_opaque_texture_format;

// GLOBAL: D3D 0x1001dc80
DDPIXELFORMAT g640x480x16_pixelFormat;

// GLOBAL: D3D 0x100156fc
int gAttached_surface_pixelformat;

// GLOBAL: D3D 0x1001bc60
int gFound_d3d_device;

// GLOBAL: D3D 0x10038500
float gDisplay_center_x;

// GLOBAL: D3D 0x10038504
float gDisplay_center_y;

// GLOBAL: D3D 0x10038120
DDSURFACEDESC gPreferred_texture_surface_description;

// GLOBAL: D3D 0x10038a00
DDSURFACEDESC gPreferred_opaque_texture_surface_description;

// GLOBAL: D3D 0x10038020
DDSURFACEDESC gAvailable_RGB555_texture_format_description;

// GLOBAL: D3D 0x100380a0
DDSURFACEDESC gAvailable_RGB565_texture_format_description;

// GLOBAL: D3D 0x1001bc74
int gIn_d3d_scene;

// GLOBAL: D3D 0x1001bb90
int gCount_queued_render_states;

// GLOBAL: D3D 0x1001bc7c
br_uint_32 gUINT_1001bc7c;

// GLOBAL: D3D 0x1001bb94
int gINT_1001bb94;

// GLOBAL: D3D 0x1001bb98
int gINT_1001bb98;

// GLOBAL: D3D 0x1001bbd4
int gFlip_count;

// GLOBAL: D3D 0x1001bc70
int gINT_1001bc70;

// GLOBAL: D3D 0x1001bbe0
int gINT_1001bbe0;

// GLOBAL: D3D 0x100156ec
int gDirty_min_x = 638;

// GLOBAL: D3D 0x100156f4
int gDirty_min_y = 478;

// GLOBAL: D3D 0x100156f0
int gDirty_max_x = 1;

// GLOBAL: D3D 0x100156f8
int gDirty_max_y = 1;

// GLOBAL: D3D 0x10037ce0
tDraw_d3d_10037ce0 gQueued_render_states[100];

// GLOBAL: D3D 0x1001bbb0
DWORD gDWORD_1001bbb0;

// GLOBAL: D3D 0x1001bbb4
DWORD gDWORD_1001bbb4;

// GLOBAL: D3D 0x1001bbe4
int gINT_1001bbe4;

// GLOBAL: D3D 0x1001bbf0
int gINT_1001bbf0;

// GLOBAL: D3D 0x1001d258
WORD gQueued_vertices_indices[100];

// GLOBAL: D3D 0x1001d77c
int gCount_queued_vertices;

// GLOBAL: D3D 0x1001d780
int gCount_queued_vertices_indices;

// GLOBAL: D3D 0x1001dc28
D3DPRIMITIVETYPE gPrimitive_type;

// GLOBAL: D3D 0x1001bbc4
int gINT_1001bbc4;

// GLOBAL: D3D 0x1001bba4
int gINT_1001bba4;

// GLOBAL: D3D 0x1001bcd8
D3DTLVERTEX gQueued_vertices[100];

// GLOBAL: D3D 0x1001dc50
void* gDAT_1001dc50;

// GLOBAL: D3D 0x1001dc6c
int gINT_1001dc6c;

// GLOBAL: D3D 0x1001dc5c
int gINT_1001dc5c;

// GLOBAL: D3D 0x1001dc68
int gINT_1001dc68;

// GLOBAL: D3D 0x1001dc58
int gINT_1001dc58;

// GLOBAL: D3D 0x1001dc4c
int gINT_1001dc4c;

// GLOBAL: D3D 0x1001dc44
int gINT_1001dc44;

// GLOBAL: D3D 0x1001bbbc
int gINT_1001bbbc;

// GLOBAL: D3D 0x1001bbc0
int gINT_1001bbc0;

// GLOBAL: D3D 0x1001dc54
int gINT_1001dc54;

// GLOBAL: D3D 0x1001dc30
int gINT_1001dc30;

// GLOBAL: D3D 0x100156e8
br_colour gCOLOUR_100156e8;

// GLOBAL: D3D 0x100389ec
int gINT_100389ec;

// GLOBAL: D3D 0x100381a4
int gINT_100381a4;

// GLOBAL: D3D 0x1001bc30
DWORD gDWORD_1001bc30;

// GLOBAL: D3D 0x100385e0
tStruct_100385e0 gCurrent_render_states[100];

// GLOBAL: D3D 0x1001dc38
int gCount_render_state_changes;

// GLOBAL: D3D 0x1001dc40
int gCount_skipped_render_state_changes;

// GLOBAL: D3D 0x1001bbd8
int gLock_count;

// GLOBAL: D3D 0x1001bbc8
int gINT_1001bbc8;

// GLOBAL: D3D 0x1001bbe8
int gINT_1001bbe8;

// GLOBAL: D3D 0x1001bc6c
undefined4 gDAT_1001bc6c;

// GLOBAL: D3D 0x1001b070
br_uint_16 gD3D_16bit_palette_buffer[256];

// GLOBAL: D3D 0x10015898
tStruct_10015898 gStruct_10015898_ARRAY[] = {
    {   4,   0,  16,  16,  16,   0, },
    {  20,   0,   8,  32,  32,   0, },
    {  14,   0,   9,  64,  64,   0, },
    {   2,   0,   6, 128, 128,   0, },
    {   1,   0,   2,  64, 128,   0, },
    {   1,   0,   2, 128,  64,   0, },
    {   0,   0,   0,   0,   0,   0, },
};

// GLOBAL: D3D 0x10038554
int gSize_texture_cache;

// GLOBAL: D3D 0x1001bbd0
int gINT_1001bbd0;

// GLOBAL: D3D 0x100381c0
tStruct_100381c0 gStruct_100381c0[16];

// GLOBAL: D3D 0x10038110
const char *gD3D_error_10038110;

// GLOBAL: D3D 0x10011030
br_uint_16 gTextures_alpha_init[16][16][16] = {
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
};


BOOL gD3D_windows = TRUE;

// FUNCTION: D3D 0x10004f20
void C2_HOOK_CDECL RestoreSurface(LPDIRECTDRAWSURFACE lpddSurface) {
    if (lpddSurface != NULL) {
        IDirectDrawSurface_Restore(lpddSurface);
    }
}

// FUNCTION: D3D 0x10004f30
LRESULT CALLBACK D3DWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ACTIVATEAPP: /* wParam: window is being activated or deactivated, lParam: thread identifier */
        if (wParam) {
            int i;

            OutputDebugStringA("D3D.BDD: WM_ACTIVATEAPP=TRUE");
            RestoreSurface(gPrimary_surface);
            RestoreSurface(gAttached_surface);
            RestoreSurface(gDepth_surface);
            for (i = 0; i < gSize_texture_cache; i++) {
                RestoreSurface(gBuffered_textures[i].temp_surface);
                RestoreSurface(gBuffered_textures[i].surface);
                gBuffered_textures[i].field_0x20 = -1;
                gBuffered_textures[i].field_0x24 = -1;
            }
        } else {
            if (gDirectDraw != NULL) {
                IDirectDraw2_FlipToGDISurface(gDirectDraw2);
            }
            OutputDebugStringA("D3D.BDD: WM_ACTIVATEAPP=FALSE");
        }
    }
    return gOriginal_wndproc(hWnd, uMsg, wParam, lParam);
}

// FUNCTION: D3D 0x10005e20
void C2_HOOK_CDECL d3d_debug_printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
}

// FUNCTION: D3D 0x10002130
BOOL CALLBACK DDEnumCallback(GUID* lpDriverGuid, LPSTR lpDriverName, LPSTR lpDriverDescription, LPVOID lpContext) {
    /* Return 1 to continue execution */
    int enum_phase = *(int*)lpContext;

    d3d_debug_printf("Callback Generic");
    if (enum_phase == 0) {
        LPDIRECTDRAW ddraw;
        d3d_debug_printf("Callback Generic form 0/0, [%s/%s]", lpDriverName, lpDriverDescription);
        if (SUCCEEDED(gDirectDrawCreate(lpDriverGuid, &ddraw, NULL))) {
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
            if (gDDraw_driver_found) {
                return 1;
            }
            gIndex_ddraw_driver = gCount_ddraw_drivers;
            gCount_ddraw_drivers += 1;
            if (gRegister_guid_valid) {
                if (gRegister_driver_guid_binary) {
                    if (memcmp(&gRegister_driver_guid, lpDriverGuid, sizeof(GUID)) == 0) {
                        gDDraw_driver_found = 1;
                    }
                } else {
                    if (lpDriverGuid == NULL) {
                        gDDraw_driver_found = 1;
                    }
                }
            }
        }
        return 1;
    } else if (enum_phase == 1) {
        LPDIRECTDRAW ddraw;
        d3d_debug_printf("Callback Generic form 1, [%d/%d]", gCount_ddraw_drivers, gIndex_ddraw_driver);
        if (SUCCEEDED(gDirectDrawCreate(lpDriverGuid, &ddraw, NULL))) {
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
                    gNo_2d_during_3d_scene = 1;
            }
            if (gIndex_ddraw_driver == gCount_ddraw_drivers) {

                gDirectDraw = ddraw;
                d3d_debug_printf("%s %s", lpDriverName, lpDriverDescription);
                if (IDirectDraw_QueryInterface(gDirectDraw, &IID_IDirect3D, (LPVOID)&gDirect3D) != DD_OK) {
                    gError_code = 1;
                }
                d3d_debug_printf("QI d3d 1[%d]", gError_code);
                if (IDirectDraw_QueryInterface(gDirectDraw, &IID_IDirectDraw2, (LPVOID)&gDirectDraw2) != DD_OK) {
                    gError_code = 1;
                }
                d3d_debug_printf("QI dd2 1[%d]", gError_code);
                if (gD3D_windows) {
                    if (IDirectDraw2_SetCooperativeLevel(gDirectDraw2, gActive_window, DDSCL_NORMAL) != DD_OK) {
                        gError_code = 1;
                    }
                } else {
                    if (IDirectDraw2_SetCooperativeLevel(gDirectDraw2, gActive_window, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK) {
                        gError_code = 1;
                    }
                }
                d3d_debug_printf("SetCo-op level 1[%d]", gError_code);
                gCount_ddraw_drivers += 1;
                return 1;
            }
            IDirectDraw_Release(ddraw);
            gCount_ddraw_drivers += 1;
        }
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: D3D 0x10001f90
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
    size = sizeof(gRegister_driver_guid);
    if (RegQueryValueExA(key,"DrvGuid", NULL, &type, (LPBYTE)&gRegister_driver_guid, &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    switch (type) {
    case REG_BINARY:
        gRegister_driver_guid_binary = 1;
        break;
    case REG_DWORD:
        gRegister_driver_guid_binary = 0;
        break;
    default:
        RegCloseKey(key);
        return;
    }
    size = sizeof(gRegister_device_guid);
    if (RegQueryValueExA(key, "DevGuid", NULL, &type, (LPBYTE)&gRegister_device_guid, &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    if (type != REG_BINARY) {
        RegCloseKey(key);
        return;
    }
    size = sizeof(gRegister_flags);
    if (RegQueryValueExA(key, "Flags", NULL, &type, (LPBYTE)&gRegister_flags, &size) != ERROR_SUCCESS) {
        RegCloseKey(key);
        return;
    }
    if (type != REG_DWORD) {
        gRegister_flags = 0xffffffff;
        RegCloseKey(key);
        return;
    }
    RegCloseKey(key);
    gRegister_guid_valid = 1;
}

// FUNCTION: D3D 0x10005080
int C2_HOOK_CDECL DetectD3DDevices(void) {
    int i;
    int enum_phase;
    gDevice_pixels = gPixel_buffer;

    for (i = 0; i < BR_ASIZE(gCurrent_render_states); i++) {
        gCurrent_render_states[i].initialized = 0;
    }
    gActive_window = GetActiveWindow();
    if (gOriginal_wndproc == NULL) {
#ifdef _M_IX86
        gOriginal_wndproc = (WNDPROC)SetWindowLong(gActive_window, GWL_WNDPROC, (LONG)D3DWndProc);
#else
        gOriginal_wndproc = (WNDPROC)SetWindowLongPtrA(gActive_window, GWLP_WNDPROC, (LONG_PTR)D3DWndProc);
#endif
    }
    d3d_debug_printf("Jims Brender driver for D3D  version 0.0.1");
    d3d_debug_printf("------------------------------------------");
    d3d_debug_printf("Compile with #define NOCONSOLE to remove these messages");
    gModule_ddraw = LoadLibraryA("DDRAW");
    if (gModule_ddraw == NULL) {
        return 0;
    }
    gDirectDrawCreate = (tDirectDrawCreate*)GetProcAddress(gModule_ddraw, "DirectDrawCreate");
    if (gDirectDrawCreate == NULL) {
        return 0;
    }
    gDirectDrawEnumerateA = (tDirectDrawEnumerateA*)GetProcAddress(gModule_ddraw, "DirectDrawEnumerateA");
    if (gDirectDrawEnumerateA == NULL) {
        return 0;
    }
    d3d_debug_printf("Got proc details %d", gError_code);
    d3d_read_registry();
    gCount_ddraw_drivers = 0;
    gIndex_ddraw_driver = 0;
    enum_phase = 0;
    if (gDirectDrawEnumerateA(DDEnumCallback, &enum_phase) != DD_OK) {
        return 4;
    }
    if (gCount_ddraw_drivers == 0) {
        return 69;
    }
    gCount_ddraw_drivers = 0;
    enum_phase = 1;
    if (gDirectDrawEnumerateA(DDEnumCallback, &enum_phase) != DD_OK) {
        return 4;
    }
    d3d_debug_printf("finished callback generic %d", gError_code);
    return 0;
}

// FUNCTION: D3D 0x10002460
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
                    && gSelected_display_mode != -1)) {
                gSelected_display_mode = gCount_display_modes;
            }
        }
        if (lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 16
                && lpDDSurfaceDesc->dwWidth == 640
                && lpDDSurfaceDesc->dwHeight == 480) {
            g640x480x16_display_mode_flags |= 0x1;
            memcpy(&g640x480x16_pixelFormat, &lpDDSurfaceDesc->ddpfPixelFormat, sizeof(DDPIXELFORMAT));
        }
        break;
    case 1:
        if (gCount_display_modes != gSelected_display_mode) {
            break;
        }
        gDisplay_width_10015888 = lpDDSurfaceDesc->dwWidth;
        gDisplay_height_1001588c = lpDDSurfaceDesc->dwHeight;
        gDisplay_width_10037cc0 = gDisplay_width_10015888;
        gDisplay_height_10037cc4 = gDisplay_height_1001588c;
        gDisplay_width_10038004 = gDisplay_width_10015888;
        gDisplay_height_10038008 = gDisplay_height_1001588c;
        if (!gD3D_windows) {
            if (IDirectDraw2_SetDisplayMode(gDirectDraw2, gDisplay_width_10015888,
                    gDisplay_height_1001588c,
                    lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, 0, 0) != DD_OK) {
                gSelected_display_mode += 1;
                if (gCount_display_modes == gSelected_display_mode) {
                    gError_code = 1;
                }
            }
        }

        if (gAttached_surface_pixel_format.dwRGBAlphaBitMask
              || (gAttached_surface_pixel_format.dwRBitMask == 0x7c00
                  && gAttached_surface_pixel_format.dwGBitMask == 0x03e0
                  && gAttached_surface_pixel_format.dwBBitMask == 0x001f)) {
            gAttached_surface_pixelformat = 2;
        } else if (gAttached_surface_pixel_format.dwRBitMask == 0xf800
                && gAttached_surface_pixel_format.dwGBitMask == 0x07e0
                && gAttached_surface_pixel_format.dwBBitMask == 0x001f) {
            gAttached_surface_pixelformat = 4;
        } else {
            gAttached_surface_pixelformat = 2;
        }
        memcpy(&gAttached_surface_pixel_format, &lpDDSurfaceDesc->ddpfPixelFormat, sizeof(DDPIXELFORMAT));
        break;
    default:
        return 1;
    }
    gCount_display_modes += 1;
    return 1;
}

// FUNCTION: D3D 0x10002630
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
        if (gRegister_guid_valid) {
            if (memcmp(&gRegister_device_guid, lpGuid, sizeof(GUID)) == 0) {
                gSelected_d3d_device = gCount_d3d_devices;
                gFound_d3d_device = 1;
            }
        }
        if (lpD3DHWDeviceDesc->dcmColorModel != 0 && !gFound_d3d_device) {
            gSelected_d3d_device = gCount_d3d_devices;
        }
        gCount_d3d_devices += 1;
        d3d_debug_printf("Enum Devices form 0, [%s] %d %d  [%d/%d]",
            lpDeviceName, gSelected_d3d_device, gFound_d3d_device,
            lpD3DHWDeviceDesc->dwDeviceZBufferBitDepth,
            lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth);
        break;
    case 1:
        d3d_debug_printf("Enum Devices form 1, [%s] %d %d  [%d/%d]",
            lpDeviceName, gSelected_d3d_device, gFound_d3d_device,
                lpD3DHWDeviceDesc->dwDeviceZBufferBitDepth,
                lpD3DHELDeviceDesc->dwDeviceZBufferBitDepth);
        if (gSelected_d3d_device == gCount_d3d_devices) {
            gDevice_has_color_model = lpD3DHWDeviceDesc->dcmColorModel != 0;
            memcpy(&gDevice_description, gDevice_has_color_model ? lpD3DHWDeviceDesc : lpD3DHELDeviceDesc, sizeof(D3DDEVICEDESC));
            d3d_debug_printf("Selected device zbuffer caps %d",
                gDevice_description.dwDeviceZBufferBitDepth);
            memcpy(&gDevice_guid, lpGuid, sizeof(GUID));
        }
        gCount_d3d_devices += 1;
        break;
    default:
        break;
    }
    return 1;
}

// FUNCTION: D3D 0x100027f0
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
        gAvailable_pixel_formats |= 0x2;
        memcpy(&gAvailable_RGB555_texture_format_description, lpDdsd, sizeof(*lpDdsd));
        d3d_debug_printf("TEX.ID: RGB 555");
        if (gPreferred_texture_format < 0x2) {
            gPreferred_texture_format = 0x2;
            memcpy(&gPreferred_texture_surface_description, lpDdsd, sizeof(*lpDdsd));
        }
        if (gPreferred_opaque_texture_format < 0x2) {
            gPreferred_opaque_texture_format = 0x2;
            memcpy(&gPreferred_opaque_texture_surface_description, lpDdsd, sizeof(*lpDdsd));
        }
    }
    if (gPixel_format_is_xrgb1555 != 1 || gDevice_has_color_model) {
        if (bit_count == 16
                && lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask == 0
                && lpDdsd->ddpfPixelFormat.dwRBitMask == 0xf800
                && lpDdsd->ddpfPixelFormat.dwGBitMask == 0x07e0
                && lpDdsd->ddpfPixelFormat.dwBBitMask == 0x001f) {
            gAvailable_pixel_formats |= 0x4;
            memcpy(&gAvailable_RGB565_texture_format_description, lpDdsd, sizeof(*lpDdsd));
            d3d_debug_printf("TEX.ID: RGB 565");
            if (gPreferred_texture_format < 0x4) {
                gPreferred_texture_format = 0x4;
                memcpy(&gPreferred_texture_surface_description, lpDdsd, sizeof(*lpDdsd));
            }
            if (gPreferred_opaque_texture_format < 0x4) {
                gPreferred_opaque_texture_format = 0x4;
                memcpy(&gPreferred_opaque_texture_surface_description, lpDdsd, sizeof(*lpDdsd));
            }
        }
        if ((gRegister_flags & 0x4)
                && bit_count == 16
                && lpDdsd->ddpfPixelFormat.dwAlphaBitDepth == 0xf000
                && lpDdsd->ddpfPixelFormat.dwRBitMask == 0x0f00
                && lpDdsd->ddpfPixelFormat.dwGBitMask == 0x00f0
                && lpDdsd->ddpfPixelFormat.dwBBitMask == 0xf) {
            gAvailable_pixel_formats |= 0x10;
            memcpy(&gAvailable_RGB565_texture_format_description, lpDdsd, sizeof(*lpDdsd));
            d3d_debug_printf("TEX.ID: ARGB 4444");
            if (gPreferred_texture_format < 0x10) {
                gPreferred_texture_format = 0x10;
                memcpy(&gPreferred_texture_surface_description, lpDdsd, sizeof(*lpDdsd));
            }
        }
    }
    return 1;
}

// FUNCTION: D3D 0x10002ac0
int C2_HOOK_CDECL SetD3DViewport(void) {
    D3DVIEWPORT2 viewport2;

    if (gD3D_Viewport == NULL) {
        return 0;
    }
    gDisplay_center_x = (float)(gDisplay_width_10015888 / 2);
    gDisplay_center_y = (float)(gDisplay_height_1001588c / 2);

    memset(&viewport2, 0, sizeof(viewport2));
    viewport2.dwSize = sizeof(viewport2);
    viewport2.dwX = 0;
    viewport2.dwY = 0;
    viewport2.dwWidth = gDisplay_width_10015888;
    viewport2.dwHeight = gDisplay_height_1001588c;
    viewport2.dvClipX = 0.f;
    viewport2.dvClipY = 0.f;
    viewport2.dvClipWidth = (float)gDisplay_width_10015888;
    viewport2.dvClipHeight = (float)gDisplay_height_1001588c;
    viewport2.dvMinZ = 0.f;
    viewport2.dvMaxZ = 1.f;
    if (IDirect3DViewport2_SetViewport2(gD3D_Viewport, &viewport2) != D3D_OK) {
        d3d_debug_printf("Set viewport2");
        gError_code = 1;
        return 1;
    }
    return 0;
}

// FUNCTION: D3D 0x10004030
void C2_HOOK_CDECL D3DSetupTextureCacheColorKeyAlpha(void) {
    int i;
    LPDIRECTDRAWSURFACE surface;
    LPDIRECT3DTEXTURE2 texture;
    DDSURFACEDESC surface_desc;

    d3d_debug_printf("TC: setup color key alpha");
    for (i = 0; i < BR_ASIZE(gStruct_100381c0); i++) {
        gStruct_100381c0[i].surface = NULL;
        gStruct_100381c0[i].texture = NULL;
        gStruct_100381c0[i].texture_handle = 0;
    }
    if (!gTexture_format_is_NOT_ARGB4444) {
        d3d_debug_printf("TC: setup color key alpha (not needed)");
        return;
    }
    memcpy(&surface_desc, &gPreferred_opaque_texture_surface_description, sizeof(surface_desc));
    surface_desc.dwWidth = 16;
    surface_desc.dwHeight = 16;
    surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
    surface_desc.dwFlags =  DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &surface, NULL) != DD_OK) {
        return;
    }
    if (IDirectDrawSurface_QueryInterface(surface, &IID_IDirect3DTexture2, (LPVOID)&texture) != DD_OK) {
        IDirectDrawSurface_Release(surface);
        return;
    }
    for (i = 0; i <  BR_ASIZE(gStruct_100381c0); i++) {
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
            memcpy((br_uint_8*)surface_desc.lpSurface + y * surface_desc.lPitch, gTextures_alpha_init[i][y], 2 * 16);
        }
        if (IDirectDrawSurface_Unlock(surface, NULL) != DD_OK) {
            IDirect3DTexture2_Release(texture);
            IDirectDraw_Release(surface);
            return;
        }
        memcpy(&surface_desc, &gPreferred_opaque_texture_surface_description, sizeof(surface_desc));
        surface_desc.dwSize = sizeof(surface_desc);
        surface_desc.dwWidth = 16;
        surface_desc.dwHeight = 16;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (gDevice_has_color_model ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gStruct_100381c0[i].surface, NULL) != DD_OK) {
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        if (IDirectDraw_QueryInterface(gStruct_100381c0[i].surface, &IID_IDirect3DTexture2, (LPVOID)&gStruct_100381c0[i].texture) != DD_OK) {
            IDirectDrawSurface_Release(gStruct_100381c0[i].surface);
            gStruct_100381c0[i].surface = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        if (IDirect3DTexture2_GetHandle(gStruct_100381c0[i].texture, gD3DDevice2, &gStruct_100381c0[i].texture_handle) != D3D_OK) {
            IDirectDrawSurface_Release(gStruct_100381c0[i].surface);
            gStruct_100381c0[i].surface = NULL;
            IDirect3DTexture2_Release(gStruct_100381c0[i].texture);
            gStruct_100381c0[i].texture = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
        color_key.dwColorSpaceLowValue = 0;
        color_key.dwColorSpaceHighValue = 0;
        if (IDirectDrawSurface_SetColorKey(gStruct_100381c0[i].surface, DDCKEY_SRCBLT, &color_key) != DD_OK) {
            IDirectDrawSurface_Release(gStruct_100381c0[i].surface);
            gStruct_100381c0[i].surface = NULL;
            IDirect3DTexture2_Release(gStruct_100381c0[i].texture);
            gStruct_100381c0[i].texture = NULL;
            IDirect3DTexture2_Release(texture);
            IDirectDrawSurface_Release(surface);
            return;
        }
    }
    IDirect3DTexture2_Release(texture);
    IDirectDrawSurface_Release(surface);
}

// FUNCTION: D3D 0x10004280
int C2_HOOK_CDECL FUN_10004280(int count, int width, int height) {
    int i;
    LPDIRECTDRAWSURFACE temp_surface;
    LPDIRECT3DTEXTURE2 temp_texture;
    LPDIRECTDRAWSURFACE temp_opaque_surface;
    LPDIRECT3DTEXTURE2 temp_opaque_texture;

    gBuffered_textures[gSize_texture_cache].width = width;
    gBuffered_textures[gSize_texture_cache].height = height;
    for (i = 0; gBuffered_textures[i].width != width || gBuffered_textures[i].height != height; i++) {
    }
    if (i == gSize_texture_cache) {
        DDSURFACEDESC surface_desc;

        memcpy(&surface_desc, &gPreferred_texture_surface_description, sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &temp_surface, NULL) != DD_OK) {
            NOT_IMPLEMENTED(); /* This should succeed */
            return -1;
        }
        if (IDirectDraw_QueryInterface(temp_surface, &IID_IDirect3DTexture2, (LPVOID)&temp_texture) != DD_OK) {
            IDirectDrawSurface_Release(temp_surface);
            return -1;
        }

        memcpy(&surface_desc, &gPreferred_opaque_texture_surface_description, sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
        surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &temp_opaque_surface, NULL) != DD_OK) {
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
        temp_surface = gBuffered_textures[i].temp_surface;
        temp_texture = gBuffered_textures[i].temp_texture;
        temp_opaque_surface = gBuffered_textures[i].temp_opaque_surface;
        temp_opaque_texture = gBuffered_textures[i].temp_opaque_texture;
    }
    for (i = 0; i < count; i++) {
        DDSURFACEDESC surface_desc;

        gBuffered_textures[gSize_texture_cache].width = width;
        gBuffered_textures[gSize_texture_cache].height = height;
        gBuffered_textures[gSize_texture_cache].temp_texture = temp_texture;
        gBuffered_textures[gSize_texture_cache].temp_surface = temp_surface;
        gBuffered_textures[gSize_texture_cache].temp_opaque_texture = temp_opaque_texture;
        gBuffered_textures[gSize_texture_cache].temp_opaque_surface = temp_opaque_surface;
        gBuffered_textures[gSize_texture_cache].field_0x1c = 0;
        gBuffered_textures[gSize_texture_cache].field_0x20 = -1;
        gBuffered_textures[gSize_texture_cache].field_0x24 = -1;
        memcpy(&surface_desc, &gPreferred_opaque_texture_surface_description, sizeof(surface_desc));
        surface_desc.dwWidth = width;
        surface_desc.dwHeight = height;
        surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (gDevice_has_color_model ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
        surface_desc.dwFlags =  DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

        if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gBuffered_textures[gSize_texture_cache].surface, NULL) != DD_OK) {
            return -1;
        }
        if (IDirectDrawSurface_QueryInterface(gBuffered_textures[gSize_texture_cache].surface, &IID_IDirect3DTexture2, (LPVOID)&gBuffered_textures[gSize_texture_cache].texture) != DD_OK) {
            return -1;
        }
        d3d_debug_printf("TC: qi");
        if (IDirect3DTexture2_Load(gBuffered_textures[gSize_texture_cache].texture, temp_opaque_texture) != D3D_OK) {
            return -1;
        }
        d3d_debug_printf("TC: load");
        if (IDirect3DTexture2_GetHandle(gBuffered_textures[gSize_texture_cache].texture, gD3DDevice2, &gBuffered_textures[gSize_texture_cache].texture_handle) != D3D_OK) {
            return -1;
        }
        d3d_debug_printf("TC: allocated texture");
        gSize_texture_cache += 1;
    }
    return count;
}

// FUNCTION: D3D 0x10004610
int C2_HOOK_CDECL StartTextureCache(void) {
    int i;
    int cache_full;
    int second_loop;

    d3d_debug_printf("--Starting Texture Cache--");
    gSize_texture_cache = 0;
    if (gDevice_description.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) {
        d3d_debug_printf("TC: ditching non square textures");

        for (i = 0; gStruct_10015898_ARRAY[i].field_0x8 != 0; i++) {
            if (gStruct_10015898_ARRAY[i].width != gStruct_10015898_ARRAY[i].height) {
                gStruct_10015898_ARRAY[i].field_0x0 = 0;
            }
            if (gStruct_10015898_ARRAY[i].width == 16 && gStruct_10015898_ARRAY[i].height == 16) {
                gStruct_10015898_ARRAY[i].field_0x8 *= 2;
            }
        }
    }
    D3DSetupTextureCacheColorKeyAlpha();
    cache_full = 0;
    second_loop = 0;
    for (;;) {
        int count;

        for (i = 0; gStruct_10015898_ARRAY[i].field_0x8 != 0; i++) {

            gStruct_10015898_ARRAY[i].field_0x4 = gStruct_10015898_ARRAY[i].field_0x0;
        }

        count = 1;
        while (count != 0) {
            if (cache_full) {
                break;
            }
            count = 0;
            for (i = 0; gStruct_10015898_ARRAY[i].field_0x8 != 0; i++) {
                if (cache_full) {
                    break;
                }
                if (gStruct_10015898_ARRAY[i].field_0x4 > 0) {
                    gStruct_10015898_ARRAY[i].field_0x4 -= 1;
                    if (FUN_10004280(gStruct_10015898_ARRAY[i].field_0x8,
                            gStruct_10015898_ARRAY[i].width >> gINT_1001bbd0,
                            gStruct_10015898_ARRAY[i].height >> gINT_1001bbd0) == -1) {
                        cache_full = 1;
                    } else {
                        gStruct_10015898_ARRAY[i].count += gStruct_10015898_ARRAY[i].field_0x8;
                    }
                }
            }
        }
        if (second_loop || count != 0) {
            break;
        }
        second_loop = 1;
    }
    for (i = 0; gStruct_10015898_ARRAY[i].field_0x8 != 0; i++) {
        d3d_debug_printf("Texture Alloc %d of %dx%d=%d at %dx%d",
            gStruct_10015898_ARRAY[i].count,
            gStruct_10015898_ARRAY[i].field_0x0,
            gStruct_10015898_ARRAY[i].field_0x8,
            gStruct_10015898_ARRAY[i].field_0x0 * gStruct_10015898_ARRAY[i].field_0x8,
            gStruct_10015898_ARRAY[i].width,
            gStruct_10015898_ARRAY[i].height);
    }
    /* FIXME: magic 2048 */
    d3d_debug_printf("Texture cache has %d off %d Entries", gSize_texture_cache, 2048);
    return 0;
}

// FUNCTION: D3D 0x10005200
int C2_HOOK_CDECL InitD3D(void) {
    DDSURFACEDESC surface_desc;
    DDSCAPS surface_caps;
    int phase;
    D3DMATERIAL black_material;
    DDBLTFX blt_fx;

    g640x480x16_display_mode_flags = 0;

    gCount_display_modes = 0;
    gSelected_display_mode = -1;
    phase = 0;
    if (IDirectDraw2_EnumDisplayModes(gDirectDraw2, 0, NULL, &phase, DDEnumDisplayModes) != DD_OK) {
        return 5;
    }
    gCount_display_modes = 0;
    phase = 1;
    if (IDirectDraw2_EnumDisplayModes(gDirectDraw2, 0, NULL, &phase, DDEnumDisplayModes) != DD_OK) {
        return 5;
    }
    d3d_debug_printf("finished enum display modes %d", gError_code);
    if (IDirectDraw_QueryInterface(gDirectDraw, &IID_IDirect3D2, (LPVOID)&gDirect3D2) != DD_OK) {
        return 1;
    }

    d3d_debug_printf("Create d3d2 object");
    gCount_d3d_devices = 0;
    gSelected_d3d_device = 0;
    phase = 0;
    if (IDirect3D2_EnumDevices(gDirect3D2, D3DEnumDevices, &phase) != D3D_OK) {
        return 7;
    }
    gCount_d3d_devices = 0;
    phase = 1;
    if (IDirect3D2_EnumDevices(gDirect3D2, D3DEnumDevices, &phase) != D3D_OK) {
        return 7;
    }
    d3d_debug_printf("finished enum devices");

    memset(&surface_desc, 0, sizeof(surface_desc));
    surface_desc.dwSize = sizeof(surface_desc);
    surface_desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    surface_desc.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
    surface_desc.dwBackBufferCount = 1;

    if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gPrimary_surface, NULL) != DD_OK) {
        d3d_debug_printf("Create Direct draw surface");
        gError_code = 1;
        return 9;
    }
    d3d_debug_printf("Create primary surface");

    surface_caps.dwCaps = DDSCAPS_BACKBUFFER;
    if (IDirectDrawSurface_GetAttachedSurface(gPrimary_surface, &surface_caps, &gAttached_surface) != DD_OK) {
        d3d_debug_printf("Get back surface");
        gError_code = 1;
        return 9;
    }

    if (IDirectDrawSurface_GetPixelFormat(gAttached_surface, &gAttached_surface_pixel_format) != DD_OK) {
        d3d_debug_printf("Get back buffer pixel format");
        gError_code = 1;
        return 9;
    }

    if (gAttached_surface_pixel_format.dwRGBAlphaBitMask == 0) {
        if (gAttached_surface_pixel_format.dwRBitMask == 0x7c00
                && gAttached_surface_pixel_format.dwGBitMask == 0x03e0
                && gAttached_surface_pixel_format.dwBBitMask == 0x001f) {
            gPixel_format_is_xrgb1555 = 1;
        } else if (gAttached_surface_pixel_format.dwRBitMask == 0xf800
                && gAttached_surface_pixel_format.dwGBitMask == 0x07e0
                && gAttached_surface_pixel_format.dwBBitMask == 0x001f) {
        } else {
            gPixel_format_is_xrgb1555 = 1;
        }
    } else {
        gPixel_format_is_xrgb1555 = 1;
    }
    d3d_debug_printf("Get back surface");

    memset(&surface_desc, 0, sizeof(surface_desc));
    surface_desc.dwSize = sizeof(surface_desc);
    surface_desc.dwWidth = gDisplay_width_10015888;
    surface_desc.dwHeight = gDisplay_height_1001588c;
    surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_ZBUFFERBITDEPTH;
    surface_desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | (gDevice_has_color_model == 1 ? DDSCAPS_VIDEOMEMORY : DDSCAPS_SYSTEMMEMORY);
    if (gDevice_description.dwDeviceZBufferBitDepth & DDBD_32) {
        surface_desc.dwZBufferBitDepth = 32;
    } else if (gDevice_description.dwDeviceZBufferBitDepth & DDBD_24) {
        surface_desc.dwZBufferBitDepth = 24;
    } else if (gDevice_description.dwDeviceZBufferBitDepth & DDBD_16) {
        surface_desc.dwZBufferBitDepth = 16;
    } else if (gDevice_description.dwDeviceZBufferBitDepth & DDBD_8) {
        surface_desc.dwZBufferBitDepth = 8;
    } else {
        d3d_debug_printf("Unsupported Z-buffer depth %d",
            gDevice_description.dwDeviceZBufferBitDepth);
        return 1;
    }

    d3d_debug_printf("About to create Z buffer");
    if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gDepth_surface, NULL) != DD_OK) {
        d3d_debug_printf("Create Z-Buffer");
        gError_code = 1;
        return 1;
    }
    d3d_debug_printf("About to Attach Z buffer");
    if (IDirectDrawSurface_AddAttachedSurface(gAttached_surface, gDepth_surface) != DD_OK) {
        d3d_debug_printf("Attach Z-Buffer");
        gError_code = 1;
        return 1;
    }
    d3d_debug_printf("Attached Z buffer");
    if (IDirect3D2_CreateDevice(gDirect3D2, &gDevice_guid, gAttached_surface, &gD3DDevice2) != D3D_OK) {
        gD3D_error_10038110 = "Create Device";
        gError_code = 1;
        return 7;
    }
    d3d_debug_printf("Create DEvice");

    if (IDirect3D2_CreateViewport(gDirect3D2, &gD3D_Viewport, NULL) != D3D_OK) {
        d3d_debug_printf("Create d3d viewport");
        gError_code = 1;
        return 1;
    }
    if (IDirect3DDevice2_AddViewport(gD3DDevice2, gD3D_Viewport) != D3D_OK) {
        d3d_debug_printf("Add viewport to device");
        gError_code = 1;
        return 1;
    }
    d3d_debug_printf("Create/add viewport Device");
    SetD3DViewport();
    if (IDirect3DDevice2_SetCurrentViewport(gD3DDevice2, gD3D_Viewport) != D3D_OK) {
        d3d_debug_printf("Set current viewport2");
        gError_code = 1;
        return 1;
    }
    d3d_debug_printf("Set current viewport [%d]", gError_code);

    if (IDirect3D2_CreateMaterial(gDirect3D2, &gViewport_background_material, NULL) != D3D_OK) {
        d3d_debug_printf("Create background material");
        gError_code = 1;
        return 1;
    }
    memset(&black_material, 0, sizeof(black_material));
    black_material.dwSize = sizeof(black_material);
    black_material.diffuse.b = 0.f;
    black_material.diffuse.g = 0.f;
    black_material.diffuse.r = 0.f;
    if (IDirect3DMaterial2_SetMaterial(gViewport_background_material, &black_material) != D3D_OK) {
        d3d_debug_printf("Set background material");
        gError_code = 1;
        return 1;
    }
    if (IDirect3DMaterial2_GetHandle(gViewport_background_material, gD3DDevice2, &gViewport_background_material_handle) != D3D_OK) {
        d3d_debug_printf("Get material handle");
        gError_code = 1;
        return 1;
    }
    if (IDirect3DViewport2_SetBackground(gD3D_Viewport, gViewport_background_material_handle) != D3D_OK) {
        d3d_debug_printf("Set viewport material");
        gError_code = 1;
        return 1;
    }
    d3d_debug_printf("Set viewport material[%d]", gError_code);

    if (IDirect3DViewport2_Clear(gD3D_Viewport, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER) != D3D_OK) {
        d3d_debug_printf("Clear viewport");
        gError_code = 1;
        return 1;
    }

    memset(&blt_fx, 0, sizeof(blt_fx));
    blt_fx.dwSize = sizeof(blt_fx);
    IDirectDrawSurface_Blt(gPrimary_surface, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blt_fx);

    memset(&blt_fx, 0, sizeof(blt_fx));
    blt_fx.dwSize = sizeof(blt_fx);
    IDirectDrawSurface_Blt(gAttached_surface, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blt_fx);
    d3d_debug_printf("Clear viewport [%d]", gError_code);

    d3d_debug_printf("Enum Textures [%d]", gError_code);
    gAvailable_pixel_formats = 0;
    gPreferred_texture_format = 0;
    phase = 0;
    if (IDirect3DDevice2_EnumTextureFormats(gD3DDevice2, D3DEnumTextureFormats, &phase) != D3D_OK) {
        return 8;
    }
    switch (gPreferred_texture_format) {
    case 1:
        d3d_debug_printf("Prefered Texture Format: 8bit pal");
        gTexture_format_is_NOT_ARGB4444 = 1;
        break;
    case 2:
        d3d_debug_printf("Prefered Texture Format: RGB555");
        gTexture_format_is_NOT_ARGB4444 = 1;
        break;
    case 4:
        d3d_debug_printf("Prefered Texture Format: RGB565");
        gTexture_format_is_NOT_ARGB4444 = 1;
        break;
    case 8:
        d3d_debug_printf("Prefered Texture Format: ARGB1555");
        gTexture_format_is_NOT_ARGB4444 = 1;
        break;
    case 16:
        d3d_debug_printf("Prefered Texture Format: ARGB4444");
        gTexture_format_is_NOT_ARGB4444 = 0;
        break;
    default:
        break;
    }
    if (gPreferred_texture_format != gPreferred_opaque_texture_format) {
        d3d_debug_printf("We also have a prefered opaqe format");
    }
    if (!(gRegister_flags & 4)) {
        gTexture_format_is_NOT_ARGB4444 = 1;
    }
    StartTextureCache();
    d3d_debug_printf("Maximum Vert count=%d, max buf=%d, ULimit=%d-%d,VLimit=%d-%d",
        gDevice_description.dwMaxVertexCount,   gDevice_description.dwMaxBufferSize,
        gDevice_description.dwMinTextureWidth,  gDevice_description.dwMaxTextureWidth,
        gDevice_description.dwMaxTextureHeight, gDevice_description.dwMinTextureHeight);
    d3d_debug_printf("are we are now ready to Render? [%d]", gError_code);
    return 0;
}

// FUNCTION: D3D 0x10005a50
void C2_HOOK_CDECL CloseD3D(void) {

    if (gD3DDevice2 != NULL) {
        IDirect3DDevice2_Release(gD3DDevice2);
        gD3DDevice2 = NULL;
    }
    if (gPrimary_surface != NULL) {
        IDirectDrawSurface_Release(gPrimary_surface);
        gPrimary_surface = NULL;
    }
    if (gDirect3D2 != NULL) {
        IDirect3D2_Release(gDirect3D2);
        gDirect3D2 = NULL;
    }
    if (gDirectDraw2 != NULL) {
        IDirectDraw2_Release(gDirectDraw2);
        gDirectDraw2 = NULL;
    }
    if (gDirect3D != NULL) {
        IDirect3D_Release(gDirect3D);
        gDirect3D = NULL;
    }
    if (gDirectDraw != NULL) {
        IDirectDraw_Release(gDirectDraw);
        gDirectDraw = NULL;
    }
    FreeLibrary(gModule_ddraw);
    ExitProcess(0);
}

// FUNCTION: D3D 0x10005000
void C2_HOOK_CDECL ApplyD3DRenderStateQueue(void) {
    int i;

    for (i = 0; i < gCount_queued_render_states; i++) {
        DWORD new_state_type = gQueued_render_states[i].type;
        DWORD new_state = gQueued_render_states[i].value;
        if (!gCurrent_render_states[new_state_type].initialized || gCurrent_render_states[new_state_type].state != new_state) {
            gCurrent_render_states[new_state_type].state = new_state;
            gCurrent_render_states[new_state_type].initialized = 1;
            IDirect3DDevice2_SetRenderState(gD3DDevice2, new_state_type, new_state);
            gCount_render_state_changes += 1;
        } else {
            gCount_skipped_render_state_changes += 1;
        }
    }
    gCount_queued_render_states = 0;
}

// FUNCTION: D3D 0x10005b10
void C2_HOOK_CDECL DoD3DOperation(int operation) {

    switch(operation) {
    case 2:
        if (gIn_d3d_scene) {
            return;
        }
        gIn_d3d_scene = 1;
        if (IDirect3DDevice2_BeginScene(gD3DDevice2) != D3D_OK) {
            d3d_debug_printf("SRS Begin scene");
            gError_code = 1;
            return;
        }
        gCount_queued_render_states = 0;
        QUEUE_RENDER_STATE(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZENABLE, 1);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, 1);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        QUEUE_RENDER_STATE(D3DCMP_LESSEQUAL, (gRegister_flags & 0x8) != 0);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, 0);
        QUEUE_RENDER_STATE(D3DRENDERSTATE_DITHERENABLE, (gRegister_flags & 0x10) != 0);
        ApplyD3DRenderStateQueue();
        return;
    case 3:
        if (gIn_d3d_scene != 1) {
            return;
        }
        gIn_d3d_scene = 0;
        if (IDirect3DDevice2_EndScene(gD3DDevice2) != D3D_OK) {
            d3d_debug_printf("SRS End scene");
            gError_code = 1;
            return;
        }
        return;
    case 4:
        gUINT_1001bc7c = (gUINT_1001bc7c + 1) % 16;
        DoD3DOperation(3);
        if (gNo_2d_during_3d_scene == 1 && gINT_1001bb98) {
            DDSURFACEDESC surface_desc;

            memset(&surface_desc, 0, sizeof(surface_desc));
            surface_desc.dwSize = sizeof(surface_desc);
            if (IDirectDrawSurface_Lock(gAttached_surface, NULL, &surface_desc, DDLOCK_WAIT, NULL) == DD_OK) {
                int y;
                br_uint_16* src = gDevice_pixels;
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
                IDirectDrawSurface_Unlock(gAttached_surface, NULL);
            }
        }
        if (IDirectDrawSurface_Flip(gPrimary_surface, NULL, DDFLIP_WAIT) != DD_OK) {
            d3d_debug_printf("Flip");
            gError_code = 1;
            return;
        }
        gFlip_count += 1;
        gINT_1001bc70 = 0;
        DoD3DOperation(5);
        DoD3DOperation(2);
        gINT_1001bbe0 = 0;
        gINT_1001bb94 = 0;
        gINT_1001bb98 = 0;
        gDirty_min_x = 639;
        gDirty_min_y = 479;
        gDirty_max_x = 1;
        gDirty_max_y = 1;
        return;
    case 5:
        {
            D3DRECT rects[1];
            rects[0].x1 = 0;
            rects[0].y1 = 0;
            rects[0].x2 = gDisplay_width_10015888;
            rects[0].y2 = gDisplay_height_1001588c;
            gINT_1001bc70 = 1;
            if (IDirect3DViewport2_Clear(gD3D_Viewport, BR_ASIZE(rects), rects, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER) != D3D_OK) {
                d3d_debug_printf("Clear viewport");
                gError_code = 1;
                return;
            }
            return;
        }
    default:
        return;
    }
}

// STUB: D3D 0x10008570
void C2_HOOK_CDECL FUN_10008570(void* dst, br_uint_32 dst_width, br_uint_32 dst_height, br_uint_32 dst_stride, void* src, br_uint_32 src_width, br_uint_32 src_height, br_uint_32 src_stride) {
    NOT_IMPLEMENTED();
}

// FUNCTION: D3D 0x10002ba0
int C2_HOOK_CDECL d3d_update_texture(int index, render_buffer_d3d* buffer, int arg3) {
    DDSURFACEDESC surface_desc;
    int texture_format;

    gINT_1001bbe0 += 1;
    buffer->field_0x74 = -2;
    if (gPreferred_texture_format != gPreferred_opaque_texture_format) {
        if (buffer->blended == 1) {
            if (gBuffered_textures[index].field_0x1c == 0) {
                gBuffered_textures[index].field_0x1c = 1;
                gINT_1001bbc8 += 1;
                IDirect3DTexture2_Release(gBuffered_textures[index].texture);
                IDirectDrawSurface_Release(gBuffered_textures[index].surface);
                gBuffered_textures[index].surface = NULL;
                gBuffered_textures[index].texture = NULL;
                memcpy(&surface_desc, &gPreferred_texture_surface_description, sizeof(surface_desc));
                surface_desc.dwWidth = gBuffered_textures[index].width;
                surface_desc.dwHeight = gBuffered_textures[index].height;
                surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (gDevice_has_color_model ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
                surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
                if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gBuffered_textures[index].surface, NULL) != DD_OK) {
                    return -1;
                }
                if (IDirectDrawSurface_QueryInterface(gBuffered_textures[index].surface, &IID_IDirect3DTexture2, (LPVOID)&gBuffered_textures[index].texture) != DD_OK) {
                    return -1;
                }
                if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_texture) != DD_OK) {
                    return -1;
                }
                if (IDirect3DTexture2_GetHandle(gBuffered_textures[index].texture, gD3DDevice2, &gBuffered_textures[index].texture_handle) != DD_OK) {
                    return -1;
                }
            }
        } else if (gBuffered_textures[index].field_0x1c == 1) {
            gBuffered_textures[index].field_0x1c = 0;
            gINT_1001bbc8 += 1;
            IDirect3DTexture2_Release(gBuffered_textures[index].texture);
            IDirectDrawSurface_Release(gBuffered_textures[index].surface);
            gBuffered_textures[index].surface = NULL;
            gBuffered_textures[index].texture = NULL;
            memcpy(&surface_desc, &gPreferred_opaque_texture_surface_description, sizeof(surface_desc));
            surface_desc.dwWidth = gBuffered_textures[index].width;
            surface_desc.dwHeight = gBuffered_textures[index].height;
            surface_desc.ddsCaps.dwCaps = DDSCAPS_ALLOCONLOAD | DDSCAPS_TEXTURE | (gDevice_has_color_model ? (DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY) : DDSCAPS_SYSTEMMEMORY);
            surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
            if (IDirectDraw_CreateSurface(gDirectDraw, &surface_desc, &gBuffered_textures[index].surface, NULL) != DD_OK) {
                return -1;
            }
            if (IDirectDrawSurface_QueryInterface(gBuffered_textures[index].surface, &IID_IDirect3DTexture2, (LPVOID)&gBuffered_textures[index].texture) != DD_OK) {
                return -1;
            }
            if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_texture) != DD_OK) {
                return -1;
            }
            if (IDirect3DTexture2_GetHandle(gBuffered_textures[index].texture, gD3DDevice2, &gBuffered_textures[index].texture_handle) != DD_OK) {
                return -1;
            }
        }
    }
    gINT_1001bbe8 += 1;
    buffer->field_0x58 = 0;
    if (gBuffered_textures[index].field_0x24 == gFlip_count) {
        IDirect3DDevice2_SetRenderState(gD3DDevice2, D3DRENDERSTATE_FLUSHBATCH, 0);
    }
    memset(&surface_desc, 0, sizeof(surface_desc));
    surface_desc.dwSize = sizeof(surface_desc);
    if (gBuffered_textures[index].field_0x1c) {
        if (IDirectDrawSurface_Lock(gBuffered_textures[index].temp_surface, NULL, &surface_desc, 0, NULL) != DD_OK) {
            d3d_debug_printf("Lock Temporary Texture Surface");
            return 1;
        }
    } else {
        if (IDirectDrawSurface_Lock(gBuffered_textures[index].temp_opaque_surface, NULL, &surface_desc, 0, NULL) != DD_OK) {
            d3d_debug_printf("Lock Temporary Texture Surface");
            return 1;
        }
    }
    texture_format = gBuffered_textures[index].field_0x1c ? gPreferred_texture_format : gPreferred_opaque_texture_format;
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
                gD3D_16bit_palette_buffer[pal_i] = ((c & 0xf80000) >> 9) | ((c & 0xf800) >> 6) | ((c & 0xff) >> 3);
            }
            break;
        case 4:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                gD3D_16bit_palette_buffer[pal_i] = ((c & 0xf80000) >> 8) | ((c & 0xfc00) >> 5) | ((c & 0xff) >> 3);
            }
            break;
        case 8:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                gD3D_16bit_palette_buffer[pal_i] = 0x1000 | ((c & 0xf80000) >> 9) | ((c & 0xf800) >> 5) | ((c & 0xff) >> 3);
            }
            break;
        case 16:
            for (pal_i = 0; pal_i < 256; pal_i++) {
                br_colour c = pal_ptr[pal_i];
                gD3D_16bit_palette_buffer[pal_i] = 0xf000 | ((c & 0xf00000) >> 12) | ((c & 0xf000) >> 8) | ((c & 0xf0) >> 4);
            }
            break;
        }

        write_row_ptr = surface_desc.lpSurface;
        read_row_ptr = buffer->field_0x68;
        for (y = 0; y < 64; y++) {
            int x;
            br_uint_16* write_ptr = (br_uint_16*)write_row_ptr;
            br_uint_8* read_ptr = read_row_ptr;
            for (x = 0; x < 64; x++, read_ptr++, write_ptr++) {
                *write_ptr = gD3D_16bit_palette_buffer[*read_ptr];
            }
            write_row_ptr += surface_desc.lPitch;
            read_row_ptr += buffer->width_b;
        }
        if (gBuffered_textures[index].field_0x1c) {
            /* FIXME: original uses temp_opaque_texture everywhere */
            if (IDirectDrawSurface_Unlock(gBuffered_textures[index].temp_surface, NULL) != DD_OK) {
                d3d_debug_printf("UNLock Temporary Texture Surface");
                return 1;
            }
            if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_opaque_texture) != D3D_OK) {
                d3d_debug_printf("Load Temporary Texture Surface");
                return 1;
            }
        } else {
            if (IDirectDrawSurface_Unlock(gBuffered_textures[index].temp_opaque_surface, NULL) != DD_OK) {
                d3d_debug_printf("UNLock Temporary Texture Surface");
                return 1;
            }
            if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_opaque_texture) != D3D_OK) {
                d3d_debug_printf("Load Temporary Texture Surface");
                return 1;
            }
        }
        gBuffered_textures[index].field_0x20 = buffer->uid;
        buffer->field_0x4c = index;
        gBuffered_textures[index].field_0x24 = gFlip_count;
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
    if (gTexture_format_is_NOT_ARGB4444 && (buffer->pm_type == BR_PMT_RGBA_4444 || buffer->pm_type == BR_PMT_ARGB_1555)) {
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
        } else {
            br_uint_8* row_ptr = surface_desc.lpSurface;
            br_uint_32 y;
            gDAT_1001bc6c = 0;
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
                            if (gTextures_alpha_init[0][(((a ^ y) & 0xf) ^ a)][x & 0xf] == 0) {
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
                            if (gTextures_alpha_init[0][((a ^ y) & 0xf) ^ a][x & 0xf] == 0) {
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
    if (gTexture_format_is_NOT_ARGB4444 && (buffer->pm_type == BR_PMT_RGBA_4444 || buffer->pm_type == BR_PMT_ARGB_1555)) {
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
                if (gNo_2d_during_3d_scene) {
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
    if (gBuffered_textures[index].field_0x1c) {
        if (IDirectDrawSurface_Unlock(gBuffered_textures[index].temp_surface, NULL) != DD_OK) {
            d3d_debug_printf("UNLock Temporary Texture Surface");
            return 1;
        }
        if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_texture) != D3D_OK) {
            d3d_debug_printf("Load Temporary Texture Surface");
            return 1;
        }
    } else {
        if (IDirectDrawSurface_Unlock(gBuffered_textures[index].temp_opaque_surface, NULL) != DD_OK) {
            d3d_debug_printf("UNLock Temporary Texture Surface");
            return 1;
        }
        if (IDirect3DTexture2_Load(gBuffered_textures[index].texture, gBuffered_textures[index].temp_opaque_texture) != D3D_OK) {
            d3d_debug_printf("Load Temporary Texture Surface");
            return 1;
        }
    }
    if (gTexture_format_is_NOT_ARGB4444) {
        DDCOLORKEY color_key;

        color_key.dwColorSpaceHighValue = 0;
        color_key.dwColorSpaceLowValue = 0;
        if (IDirectDrawSurface_SetColorKey(gBuffered_textures[index].surface, 8, &color_key) != DD_OK) {
            d3d_debug_printf("Set color key");
            return 1;
        }
    }
    gBuffered_textures[index].field_0x20 = buffer->uid;
    buffer->field_0x4c = index;
    gBuffered_textures[index].field_0x24 = gFlip_count;
    return 0;
}

// FUNCTION: D3D 0x10003bd0
int C2_HOOK_CDECL d3d_load(render_buffer_d3d* buffer, int arg2) {
    br_uint_8 shift1;
    int w_p_shift1;
    int h_p_shift1;
    int size_i;
    int slot_index;
    int tex_w;
    int tex_h;

    gINT_1001bbe4 += 1;
    gDWORD_1001bbb0 = 0;
    if (buffer == NULL) {
        d3d_debug_printf("Pixel map pointer was null");
        return 1;
    }
    if (buffer->field_0x68 == NULL) {
        d3d_debug_printf("pixel map data was null");
        return 1;
    }

    shift1 = (br_uint_8)gINT_1001bbd0;
    w_p_shift1 = buffer->width_p >> shift1;
    w_p_shift1 = D3D_MAX(16 >> shift1, w_p_shift1);

    h_p_shift1 = buffer->height >> shift1;
    h_p_shift1 = D3D_MAX(16 >> shift1, h_p_shift1);

    if (buffer->field_0x5c != gFlip_count) {
        buffer->field_0x5c = gFlip_count;
    }

    if (buffer->field_0x4c != -1 && gBuffered_textures[buffer->field_0x4c].field_0x20 == buffer->uid) {
        if (buffer->field_0x58 == 1 && d3d_update_texture(buffer->field_0x4c, buffer, arg2) != 0) {
                d3d_debug_printf("Call to d3d_load to update texture %d failed", buffer->field_0x4c);
                return 1;
        }
        gDWORD_1001bbb0 = gBuffered_textures[buffer->field_0x4c].texture_handle;
        gBuffered_textures[buffer->field_0x4c].field_0x24 = gFlip_count;
        return 0;
    }

    size_i = -1;
    for (;;) {
        int i;
        int new_h_shift = 0;

        for (i = 0; gStruct_10015898_ARRAY[i].field_0x8 != 0; i++) {
            if (gStruct_10015898_ARRAY[i].count != 0) {
                if (new_h_shift < 10 && w_p_shift1 == gStruct_10015898_ARRAY[i].width && h_p_shift1 == gStruct_10015898_ARRAY[i].height) {
                    new_h_shift = 10;
                    size_i = i;
                }
                if (new_h_shift < 9 && w_p_shift1 == gStruct_10015898_ARRAY[i].width && h_p_shift1 < gStruct_10015898_ARRAY[i].height) {
                    new_h_shift = 9;
                    size_i = i;
                }
                if (new_h_shift < 8 && w_p_shift1 < gStruct_10015898_ARRAY[i].width && h_p_shift1 == gStruct_10015898_ARRAY[i].height) {
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
    tex_w = gStruct_10015898_ARRAY[size_i].width >> shift1;
    tex_h = gStruct_10015898_ARRAY[size_i].height >> shift1;
    slot_index = -1;
    if (gPreferred_opaque_texture_format == gPreferred_texture_format) {
        int i;
        int oldest_flip_count = gFlip_count + 1;

        for (i = 0; i < gSize_texture_cache; i++) {
            if (gBuffered_textures[i].texture != NULL
                    && gBuffered_textures[i].width == tex_w
                    && gBuffered_textures[i].height == tex_h
                    && gBuffered_textures[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = gBuffered_textures[i].field_0x24;
            }
        }
    } else if (buffer->blended == 1) {
        int i;
        int oldest_flip_count = gFlip_count + 1;

        for (i = 0; i < gSize_texture_cache; i++) {
            if (gBuffered_textures[i].field_0x1c == 0) {
                gBuffered_textures[i].field_0x24 -= 2;
            }
            if (gBuffered_textures[i].texture != NULL
                    && gBuffered_textures[i].width == tex_w
                    && gBuffered_textures[i].height == tex_h
                    && gBuffered_textures[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = gBuffered_textures[i].field_0x24;
            }
            if (gBuffered_textures[i].field_0x1c == 0) {
                gBuffered_textures[i].field_0x24 += 2;
            }
        }
    } else {
        int i;
        int oldest_flip_count = gFlip_count + 1;

        for (i = 0; i < gSize_texture_cache; i++) {
            if (gBuffered_textures[i].field_0x1c == 1) {
                gBuffered_textures[i].field_0x24 -= 2;
            }
            if (gBuffered_textures[i].texture != NULL
                    && gBuffered_textures[i].width == tex_w
                    && gBuffered_textures[i].height == tex_h
                    && gBuffered_textures[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = gBuffered_textures[i].field_0x24;
            }
            if (gBuffered_textures[i].field_0x1c == 1) {
                gBuffered_textures[i].field_0x24 += 2;
            }
        }
    }

    if (gNo_2d_during_3d_scene == 1 && (slot_index == -1 || gFlip_count <= gBuffered_textures[slot_index].field_0x24)) {
        int i;
        int oldest_flip_count = gFlip_count + 1;

        slot_index = -1;
        for (i = 0; i < gSize_texture_cache; i++) {
            if (gBuffered_textures[i].texture != NULL
                    && gBuffered_textures[i].width <= tex_w
                    && gBuffered_textures[i].height <= tex_h
                    && gBuffered_textures[i].field_0x24 < oldest_flip_count) {
                slot_index = i;
                oldest_flip_count = gBuffered_textures[i].field_0x24;
            }
        }
        if (slot_index == -1 || gBuffered_textures[slot_index].field_0x24 >= gFlip_count) {
            slot_index = -1;
            for (i = 0; i < gSize_texture_cache; i++) {
                if (gBuffered_textures[i].texture != NULL
                        && gBuffered_textures[i].field_0x24 < oldest_flip_count) {
                    slot_index = i;
                    oldest_flip_count = gBuffered_textures[i].field_0x24;
                }
            }
        }
        if (slot_index == -1 || gBuffered_textures[slot_index].field_0x24 >= gFlip_count) {
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
    gDWORD_1001bbb0 = gBuffered_textures[slot_index].texture_handle;
    return 0;
}

// FUNCTION: D3D 0x10007140
void C2_HOOK_CDECL FUN_10007140(void) {

    if (gCount_queued_vertices == 0) {
        return;
    }
    if (gIn_d3d_scene != 1) {
        DoD3DOperation(2);
    }
    if (gCount_queued_render_states != 0) {
        ApplyD3DRenderStateQueue();
    }
    if (gNo_2d_during_3d_scene == 1) {
        if (gINT_1001bba4 == 9) {
            gCount_queued_vertices = 0;
            gCount_queued_vertices_indices = 0;
            return;
        } else if (gINT_1001bba4 == 2) {
            if (gCount_queued_vertices == 6) {
                int i;
                WORD fan[4] = { 0 };

                for (i = 0; i < gCount_queued_vertices; i++) {
                    if (gQueued_vertices[i].tu > 1.5f) {
                        gCount_queued_vertices = 0;
                        gCount_queued_vertices_indices = 0;
                        return;
                    }
                    if (gQueued_vertices[i].sx <= gQueued_vertices[fan[0]].sx
                            && gQueued_vertices[i].sy <= gQueued_vertices[fan[0]].sy) {
                        fan[0] = i;
                    }
                    if (gQueued_vertices[i].sx >= gQueued_vertices[fan[1]].sx
                            && gQueued_vertices[i].sy <= gQueued_vertices[fan[1]].sy) {
                        fan[1] = i;
                    }
                    if (gQueued_vertices[i].sx >= gQueued_vertices[fan[2]].sx
                            && gQueued_vertices[i].sy >= gQueued_vertices[fan[2]].sy) {
                        fan[2] = i;
                    }
                    if (gQueued_vertices[i].sx <= gQueued_vertices[fan[3]].sx
                            && gQueued_vertices[i].sy >= gQueued_vertices[fan[3]].sy) {
                        fan[3] = i;
                    }
                    if (gQueued_vertices[i].sx < 1.0f) {
                        gQueued_vertices[i].sx = 1.0f;
                    }
                    gQueued_vertices[i].color = gQueued_vertices[0].color;
                    gQueued_vertices[i].sz = gQueued_vertices[0].sz;
                    gQueued_vertices[i].specular = gQueued_vertices[0].specular;
                }
                IDirect3DDevice2_DrawIndexedPrimitive(gD3DDevice2,
                    D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, gQueued_vertices, 6,
                    fan, BR_ASIZE(fan),
                    D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
                gCount_queued_vertices_indices = 0;
                gCount_queued_vertices = 0;
                return;
            }
        } else if (gINT_1001bba4 == 1 && gCount_queued_vertices == 6 && gDAT_1001dc50 != NULL) {
            int u, v, du, dv, u_max, v_max;
            float factor_x, factor_y;
            br_uint_8* map_ptr;

            gQueued_vertices_indices[1] = 0;
            gQueued_vertices_indices[0] = 3;
            gQueued_vertices_indices[2] = 1;
            gQueued_vertices_indices[3] = 2;
            gQueued_vertices_indices[4] = 3;
            gINT_1001dc6c = (int)(gQueued_vertices[3].tu * (float)gINT_1001dc4c);
            gINT_1001dc5c = (int)(gQueued_vertices[3].tv * (float)gINT_1001dc44);
            gINT_1001dc68 = (int)(gQueued_vertices[1].tu * (float)gINT_1001dc4c);
            gINT_1001dc58 = (int)(gQueued_vertices[1].tv * (float)gINT_1001dc44);
            du = gINT_1001dc68 - gINT_1001dc6c;
            dv = gINT_1001dc58 - gINT_1001dc5c;
            map_ptr = (br_uint_8*)gDAT_1001dc50 + gINT_1001dc5c * gINT_1001dc54 + 2 * gINT_1001dc6c;
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
                map_ptr += gINT_1001dc54;
            }
            gINT_1001dc68 = gINT_1001dc6c + u_max + 1;
            gINT_1001dc58 = gINT_1001dc5c + v_max + 1;
            if (du == 0 || dv == 0.0) {
                factor_x = 0.f;
                factor_y = 0.f;
            }
            else {
                factor_x = (float)(u_max + 1) / (float)du;
                factor_y = (float)(v_max + 1) / (float)dv;
            }
            gQueued_vertices[0].sx = factor_x * (gQueued_vertices[1].sx - gQueued_vertices[3].sx) + gQueued_vertices[3].sx;
            gQueued_vertices[0].tu = (float)gINT_1001dc68 / (float)gINT_1001dc4c;
            gQueued_vertices[0].tv = (float)gINT_1001dc5c / (float)gINT_1001dc44;
            gQueued_vertices[0].color = gQueued_vertices[3].color;

            gQueued_vertices[1].sx = gQueued_vertices[0].sx;
            gQueued_vertices[1].sy = factor_y * (gQueued_vertices[1].sy - gQueued_vertices[3].sy) + gQueued_vertices[3].sy;
            gQueued_vertices[1].tv = (float)gINT_1001dc58 / (float)gINT_1001dc44;
            gQueued_vertices[1].tu = gQueued_vertices[0].tu;
            gQueued_vertices[1].color = gQueued_vertices[3].color;

            gQueued_vertices[2].sy = gQueued_vertices[1].sy;
            gQueued_vertices[2].tu = (float)gINT_1001dc6c / (float)gINT_1001dc4c;
            gQueued_vertices[2].tv = gQueued_vertices[1].tv;
            gQueued_vertices[2].color = gQueued_vertices[3].color;

            gQueued_vertices[3].tu = gQueued_vertices[2].tu;
            gQueued_vertices[3].tv = gQueued_vertices[0].tv;

            gCount_queued_vertices_indices = 4;
            IDirect3DDevice2_DrawIndexedPrimitive(gD3DDevice2, D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX,
                gQueued_vertices, gCount_queued_vertices,
                gQueued_vertices_indices, 4,
                D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);
            gCount_queued_vertices_indices = 0;
            gCount_queued_vertices = 0;
            return;
        } else if (gINT_1001bba4 == 3) {
            QUEUE_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE, (uintptr_t)NULL);
            QUEUE_RENDER_STATE(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
            QUEUE_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
            if (gCount_queued_render_states != 0) {
                ApplyD3DRenderStateQueue();
            }
            gINT_100389ec += gCount_queued_vertices;
            gINT_100381a4 += gCount_queued_vertices_indices;
        } else {
            gINT_100389ec += gCount_queued_vertices;
            gINT_100381a4 += gCount_queued_vertices_indices;
        }
    } else {
        gINT_100389ec += gCount_queued_vertices;
        gINT_100381a4 += gCount_queued_vertices_indices;
    }
    IDirect3DDevice2_DrawIndexedPrimitive(gD3DDevice2, gPrimitive_type, D3DVT_TLVERTEX,
        gQueued_vertices, gCount_queued_vertices,
        gQueued_vertices_indices, gCount_queued_vertices_indices,
        gDWORD_1001bc30 | D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
    gCount_queued_vertices_indices = 0;
    gCount_queued_vertices = 0;
}

// STUB: D3D 0x10004780
void C2_HOOK_CDECL FUN_10004780(void* arg1, int stride) {
    NOT_IMPLEMENTED();
}
