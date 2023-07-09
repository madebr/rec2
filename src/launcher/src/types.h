#ifndef TYPES_H
#define TYPES_H

#include <windows.h>
#include <d3d.h>

enum {
    kClose_Abort = -1,
    kClose_NotClosed = 0,
    kClose_StartGame = 42,
};


enum {
    kBackend_directdraw = 1,
    kBackend_3dfx = 2,
    kBackend_direct3d = 3,
};

enum {
    kBilinearFiltering = 0x1,
    kUnknown_0x2 = 0x2,
    kAlphaBleeding = 0x4,
    kPerspectiveCorrect = 0x8,
    kFrameBufferDithering = 0x10,
    kAlternateFoxEffect = 0x20,
    kSaveOptions = 0x100000,
};

typedef struct {
    int width;
    int height;
    DDPIXELFORMAT pixel_vertex_format;
} Direct3DDisplayModeInfo;

typedef struct {
    GUID guid;
    char name[200];
    D3DCOLORMODEL color_model;
    int zbuffer_not_ok;
    int srcDstBlendsCapsOk;
} Direct3DDeviceInfo;

typedef struct {
    GUID guid;
    int valid;
    char name[200];
    char description[200];
    char* error;
    unsigned int available_memory;
    int device_count;
    int display_mode_count;
    Direct3DDeviceInfo devices[10];
    Direct3DDisplayModeInfo display_modes[20];
} Direct3DDisplayInfo;

#endif /* TYPES_H */
