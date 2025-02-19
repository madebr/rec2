#ifndef D2D_DEVCLUT_H
#define D2D_DEVCLUT_H

#include "types.h"

#include "c2_hooks.h"

br_device_clut_d3d* C2_HOOK_CDECL DeviceClutD3DAllocate(br_device_d3d* dev, const char *identifier);

void C2_HOOK_CDECL _M_br_device_clut_d3d_free(br_device_clut* self);

br_token C2_HOOK_CDECL _M_br_device_clut_d3d_type(br_device_clut *self);

br_boolean C2_HOOK_CDECL _M_br_device_clut_d3d_isType(br_device_clut *self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_device_clut_d3d_space(br_device_clut *self);

struct br_tv_template * C2_HOOK_CDECL _M_br_device_clut_d3d_queryTemplate(br_device_clut *self);

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entrySet(br_device_clut *self, br_int_32 index, br_colour entry);

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entryQuery(br_device_clut *self, br_colour *entry, br_int_32 index);

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entrySetMany(br_device_clut *self, br_int_32 index, br_int_32 count, br_colour *entries);

br_error C2_HOOK_CDECL _M_br_device_clut_d3d_entryQueryMany(br_device_clut *self, br_colour *entries, br_int_32 index, br_int_32 count);

#endif /* D2D_DEVCLUT_H */
