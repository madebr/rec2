#ifndef D3D_PSTATE_H
#define D3D_PSTATE_H

#include "types.h"

#include "c2_hooks.h"

br_primitive_state_d3d* C2_HOOK_CDECL PrimitiveStateD3DAllocate(br_primitive_library_d3d* plib);

void C2_HOOK_CDECL PrimitiveStateD3DClearTemplates(void);


void C2_HOOK_CDECL _M_br_primitive_state_d3d_free(br_primitive_state_d3d* self);

br_token C2_HOOK_CDECL _M_br_primitive_state_d3d_type(br_primitive_state_d3d* self);

br_boolean C2_HOOK_CDECL _M_br_primitive_state_d3d_isType(br_primitive_state_d3d* self, br_token t);

br_int_32 C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_space(br_primitive_state_d3d *self);

br_tv_template * C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_templateQuery(br_primitive_state_d3d* self);

br_error C2_HOOK_CDECL  C2_HOOK_CDECL _M_br_primitive_state_d3d_partSet(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token t, br_uint_32 value);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partSetMany(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQuery(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryBuffer(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryMany(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryManySize(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_size_t* pextra_size, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryAll(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryAllSize(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_size_t* psize);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partIndexQuery(br_primitive_state_d3d* self, br_token part, br_int_32* pnindex);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateDefault(br_primitive_state_d3d* self, br_uint_32 mask);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateCopy(br_primitive_state_d3d *self, br_primitive_state_d3d *source, br_uint_32 mask);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_partQueryCapability(br_primitive_state_d3d* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size);

br_error C2_HOOK_CDECL _M_br_primitive_state_d3d_stateQueryPerformance(br_primitive_state_d3d* self, br_fixed_lu* speed);

#endif /* D3D_PSTATE_H */
