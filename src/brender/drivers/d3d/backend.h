#ifndef D3D_BACKEND_H
#define D3D_BACKEND_H

#include "types.h"

#include "c2_hooks.h"

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#define QUEUE_RENDER_STATE(TYPE, VALUE) do {                                            \
        gQueued_render_states[gCount_queued_render_states].type = (TYPE);     \
        gQueued_render_states[gCount_queued_render_states].value = (VALUE);   \
        gCount_queued_render_states += 1;                                          \
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
} tStruct_100385e0;

typedef struct {
    undefined4 field_0x0;
    undefined4 field_0x4;
    undefined4 field_0x8;
    int width;
    int height;
    int count;
} tStruct_10015898;

extern LPDIRECTDRAWSURFACE gAttached_surface;
extern tStruct_1001dcc0 gBuffered_textures[128];
extern DDPIXELFORMAT gAttached_surface_pixel_format;
extern int gCount_queued_render_states;
extern DWORD gRegister_flags;
extern int gTexture_format_is_NOT_ARGB4444;
extern tDraw_d3d_10037ce0 gQueued_render_states[100];
extern tStruct_100381c0 gStruct_100381c0[16];
extern DWORD gDWORD_1001bbb0;
extern DWORD gDWORD_1001bbb4;
extern int gNo_2d_during_3d_scene;
extern int gINT_1001bbf0;
extern int fINT_1001bbc4;
extern D3DTLVERTEX gQueued_vertices[100];
extern WORD gQueued_vertices_indices[100];
extern int gCount_queued_vertices;
extern int gCount_queued_vertices_indices;
extern D3DPRIMITIVETYPE gPrimitive_type;
extern int gINT_1001bbc4;
extern int gINT_1001bba4;
extern void* gDAT_1001dc50;
extern int gINT_1001bbbc;
extern int gINT_1001bbc0;
extern int gINT_1001bbc4;
extern int gINT_1001bba4;
extern int gINT_1001dc44;
extern int gINT_1001dc4c;
extern int gINT_1001dc54;
extern int gINT_1001dc30;
extern br_colour gCOLOUR_100156e8;
extern int gLock_count;
extern int gINT_1001bb94;


extern BOOL gD3D_windows;

int C2_HOOK_CDECL DetectD3DDevices(void);

int C2_HOOK_CDECL InitD3D(HWND hWnd);

void C2_HOOK_CDECL CloseD3D(void);

void C2_HOOK_CDECL DoD3DOperation(int operation);

int C2_HOOK_CDECL d3d_load(render_buffer_d3d* buffer, int arg2);

void C2_HOOK_CDECL FUN_10007140(void);

void C2_HOOK_CDECL FUN_10004780(void* arg1, int stride);

#endif /* D3D_BACKEND_H */