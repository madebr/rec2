#ifndef D3D_BACKEND_H
#define D3D_BACKEND_H

#include "types.h"

#include "c2_hooks.h"

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#define QUEUE_RENDER_STATE(TYPE, VALUE) do {                                            \
        C2V(gQueued_render_states)[C2V(gCount_queued_render_states)].type = (TYPE);     \
        C2V(gQueued_render_states)[C2V(gCount_queued_render_states)].value = (VALUE);   \
        C2V(gCount_queued_render_states) += 1;                                          \
    } while (0)

typedef struct {
    LPDIRECT3DTEXTURE2 texture;
    LPDIRECTDRAWSURFACE surface;
    LPDIRECT3DTEXTURE2 temp_texture;
    LPDIRECTDRAWSURFACE temp_surface;
    LPDIRECT3DTEXTURE2 temp_opaque_texture;
    LPDIRECTDRAWSURFACE temp_opaque_surface;
    DWORD texture_handle;
    undefined4 field_0x1c;
    int field_0x20;
    int field_0x24;
    int width;
    int height;
    undefined4 field_0x30;
} tStruct_1001dcc0;
typedef struct {
    int type;
    DWORD value;
} tDraw_d3d_10037ce0;
typedef struct {
    LPDIRECTDRAWSURFACE surface;
    LPDIRECT3DTEXTURE2 texture;
    undefined4 field_0x8;
    undefined4 field_0xc;
    undefined4 field_0x10;
    undefined4 field_0x14;
    DWORD texture_handle;
    undefined4 field_0x1c;
    undefined4 field_0x20;
    undefined4 field_0x24;
    undefined4 field_0x28;
    undefined4 field_0x2c;
    undefined4 field_0x30;
} tStruct_100381c0;
typedef struct {
    DWORD state;
    br_boolean initialized;
} tDdraw_d3d_100385e4;


C2_HOOK_VARIABLE_DECLARE(LPDIRECTDRAWSURFACE, gAttached_surface);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tStruct_1001dcc0, gBuffered_textures, 128);
C2_HOOK_VARIABLE_DECLARE(DDPIXELFORMAT, gAttached_surface_pixel_format);
C2_HOOK_VARIABLE_DECLARE(int, gCount_queued_render_states);
C2_HOOK_VARIABLE_DECLARE(DWORD, gRegister_flags);
C2_HOOK_VARIABLE_DECLARE(int, gTexture_format_is_NOT_ARGB4444);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDraw_d3d_10037ce0, gQueued_render_states, 100);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tStruct_100381c0, gStruct_100381c0, 16);
C2_HOOK_VARIABLE_DECLARE(DWORD, gDWORD_1001bbb0);
C2_HOOK_VARIABLE_DECLARE(DWORD, gDWORD_1001bbb4);
C2_HOOK_VARIABLE_DECLARE(int, gNo_2d_during_3d_scene);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bbf0);
C2_HOOK_VARIABLE_DECLARE(int, fINT_1001bbc4);
C2_HOOK_VARIABLE_DECLARE_ARRAY(D3DTLVERTEX, gQueued_vertices, 100);
C2_HOOK_VARIABLE_DECLARE_ARRAY(WORD, gQueued_vertices_indices, 100);
C2_HOOK_VARIABLE_DECLARE(int, gCount_queued_vertices);
C2_HOOK_VARIABLE_DECLARE(int, gCount_queued_vertices_indices);
C2_HOOK_VARIABLE_DECLARE(D3DPRIMITIVETYPE, gPrimitive_type);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bbc4);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bba4);
C2_HOOK_VARIABLE_DECLARE(void*, gDAT_1001dc50);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bbbc);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bbc0);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bbc4);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bba4);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001dc44);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001dc4c);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001dc54);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001dc30);
C2_HOOK_VARIABLE_DECLARE(br_colour, gCOLOUR_100156e8);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tDdraw_d3d_100385e4, gCurrent_render_states, 100);
C2_HOOK_VARIABLE_DECLARE(int, gLock_count);
C2_HOOK_VARIABLE_DECLARE(int, gINT_1001bb94);


extern BOOL gD3D_windows;

int C2_HOOK_CDECL DetectD3DDevices(void);

int C2_HOOK_CDECL InitD3D(HWND hWnd);

void C2_HOOK_CDECL CloseD3D(void);

void C2_HOOK_CDECL DoD3DOperation(int operation);

int C2_HOOK_CDECL d3d_load(render_buffer_d3d* buffer, int arg2);

void C2_HOOK_CDECL FUN_10007140(void);

void C2_HOOK_CDECL FUN_10004780(void* arg1, int stride);

#endif /* D3D_BACKEND_H */
