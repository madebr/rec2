#ifndef D3D_PLIB_H
#define D3D_PLIB_H

#include "types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(br_primitive_library_d3d, PrimitiveLibraryD3D);

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_token, PrimPartsD3DTokens, 3);

br_error C2_HOOK_CDECL PrimitiveLibraryD3DInitialise(br_primitive_library_d3d* self, br_device_d3d* dev);

void C2_HOOK_CDECL _M_br_primitive_d3d_library_free(br_primitive_library_d3d* self);

br_token C2_HOOK_CDECL _M_br_primitive_d3d_library_type(br_primitive_library_d3d* self);

br_boolean C2_HOOK_CDECL _M_br_primitive_d3d_library_isType(br_primitive_library_d3d* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_primitive_d3d_library_space(br_primitive_library_d3d* self);

struct br_tv_template* C2_HOOK_CDECL _M_br_primitive_d3d_library_templateQuery(br_primitive_library_d3d* self);

void * C2_HOOK_CDECL _M_br_primitive_d3d_library_listQuery(br_primitive_library_d3d* self);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_stateNew(br_primitive_library_d3d* self, br_primitive_state_d3d **rps);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_bufferStoredNew(br_primitive_library_d3d* self, br_buffer_stored_d3d **psm, br_token use, br_device_pixelmap_d3d *pm, br_token_value *tv);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_bufferStoredAvail(br_primitive_library_d3d* self, br_int_32 *space, br_token use, br_token_value *tv);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_flush(br_primitive_library_d3d* self, br_boolean wait);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_synchronise(br_primitive_library_d3d* self, br_token sync_type, br_boolean block);

br_error C2_HOOK_CDECL _M_br_primitive_d3d_library_mask( br_primitive_library_d3d* self, br_uint_32 *mask, br_token *parts, int n_parts);


#endif /* D3D_PLIB_H */
