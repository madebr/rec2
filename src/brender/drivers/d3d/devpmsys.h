#ifndef D3D_DEVPMSYS_H
#define D3D_DEVPMSYS_H

#include "types.h"

#include "c2_hooks.h"


br_error C2_HOOK_CDECL AllocateD3DSysMemPixelmap(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, int w, int h);

#endif /* D3D_DEVPMSYS_H */
