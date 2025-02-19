#ifndef D3D_OUTFCTY_H
#define D3D_OUTFCTY_H

#include "types.h"

#include "c2_hooks.h"

br_uint_32 C2_HOOK_CDECL OutputFacilityD3DInitialise(br_device_d3d* dev, br_primitive_library_d3d* prim_lib);

void C2_HOOK_CDECL _M_br_output_facility_d3d_free(br_output_facility_d3d* self);

br_token C2_HOOK_CDECL _M_br_output_facility_d3d_type(br_output_facility_d3d* self);

br_boolean C2_HOOK_CDECL _M_br_output_facility_d3d_isType(br_output_facility_d3d* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_output_facility_d3d_space(br_output_facility_d3d* self);

struct br_tv_template * C2_HOOK_CDECL _M_br_output_facility_d3d_queryTemplate(br_output_facility_d3d* self);

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_validSource(br_output_facility_d3d* self, br_boolean *bp, br_object *h);

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_pixelmapNew(br_output_facility_d3d* self, br_device_pixelmap_d3d **ppmap, br_token_value *tv);

br_error C2_HOOK_CDECL _M_br_output_facility_d3d_clutNew(br_output_facility_d3d* self, br_device_clut **pclut, br_token_value *tv);

void * C2_HOOK_CDECL _M_br_output_facility_d3d_listQuery(br_output_facility_d3d* self);


#endif /* D3D_OUTFCTY_H */
