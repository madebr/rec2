#ifndef D3D_SBUFFER_H
#define D3D_SBUFFER_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL BufferStoredD3DClearTemplate(void);

br_buffer_stored_d3d* C2_HOOK_CDECL BufferStoredD3DAllocate(br_primitive_library_d3d* plib, br_token use, br_device_pixelmap_d3d* pm, br_token_value* tv);

void C2_HOOK_CDECL _M_br_buffer_stored_d3d_free(br_buffer_stored_d3d* self);

br_token C2_HOOK_CDECL _M_br_buffer_stored_d3d_type(br_buffer_stored_d3d* self);

br_boolean C2_HOOK_CDECL _M_br_buffer_stored_d3d_isType(br_buffer_stored_d3d* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_buffer_stored_d3d_space(br_buffer_stored_d3d* self);

br_tv_template* C2_HOOK_CDECL _M_br_buffer_stored_d3d_templateQuery(br_buffer_stored_d3d* self);

br_error C2_HOOK_CDECL _M_br_buffer_stored_d3d_update(br_buffer_stored_d3d* self, br_device_pixelmap_d3d* pm, br_token_value* tv);

#endif /* D3D_SBUFFER_H */
