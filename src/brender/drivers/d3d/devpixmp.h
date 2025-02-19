#ifndef D3D_DEVPIXMP_H
#define D3D_DEVPIXMP_H

#include "types.h"

#include "c2_hooks.h"

br_error C2_HOOK_CDECL DevicePixelmapD3DAllocateMode(br_device_d3d* dev, br_output_facility_d3d* facility, br_device_pixelmap_d3d** newpm, br_token_value* tv);

void C2_HOOK_CDECL _M_br_device_pixelmap_d3d_free(br_device_pixelmap_d3d* self);

br_token C2_HOOK_CDECL _M_br_device_pixelmap_d3d_type(br_device_pixelmap_d3d* self);

br_boolean C2_HOOK_CDECL _M_br_device_pixelmap_d3d_isType(br_device_pixelmap_d3d* self, br_token t) ;

br_int_32 C2_HOOK_CDECL _M_br_device_pixelmap_d3d_space(br_device_pixelmap_d3d* self);

br_tv_template* C2_HOOK_CDECL _M_br_device_pixelmap_d3d_queryTemplate(br_device_pixelmap_d3d* self);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_resize(br_device_pixelmap_d3d* self, br_int_32 width, br_int_32 height);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_match(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_allocateSub(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d** newpm, br_rectangle* rect);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_fill(br_device_pixelmap *self, br_uint_32 colour);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_doubleBuffer(br_device_pixelmap_d3d* self, br_device_pixelmap_d3d* src);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopyTo(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* src, br_rectangle* r);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopy(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* src, br_rectangle* r);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleCopyFrom(br_device_pixelmap_d3d* self, br_point* p, br_device_pixelmap_d3d* mem, br_rectangle* r);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopy(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle* s);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopyTo(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle *s);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleStretchCopyFrom(br_device_pixelmap_d3d* self, br_rectangle* d, br_device_pixelmap_d3d* src, br_rectangle* s);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_rectangleFill(br_device_pixelmap_d3d* self, br_rectangle* rect, br_uint_32 colour);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_pixelSet(br_device_pixelmap_d3d* self, br_point* p, br_uint_32 colour);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_flush(br_device_pixelmap_d3d* self);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_synchronise(br_device_pixelmap_d3d* self, br_token sync_type, br_boolean block);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_directLock(br_device_pixelmap_d3d* self, br_boolean block);

br_error C2_HOOK_CDECL _M_br_device_pixelmap_d3d_directUnlock(br_device_pixelmap_d3d* self);

#endif /* D3D_DEVPIXMP_H */
