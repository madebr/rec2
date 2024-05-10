#ifndef SOFTREND_RENDERER_H
#define SOFTREND_RENDERER_H

#include "types.h"

#include "c2_hooks.h"

br_renderer* C2_HOOK_STDCALL RendererSoftAllocate(br_device *dev, br_soft_renderer_facility *type, struct br_primitive_library *prims);

void C2_HOOK_CDECL _M_br_soft_renderer_free(br_soft_renderer* self);

br_token C2_HOOK_CDECL _M_br_soft_renderer_type(br_soft_renderer* self);

int C2_HOOK_CDECL _M_br_soft_renderer_isType(br_soft_renderer* self, br_token t);

size_t C2_HOOK_CDECL _M_br_soft_renderer_space(br_soft_renderer* self);

br_tv_template* C2_HOOK_CDECL _M_br_soft_renderer_templateQuery(br_soft_renderer* self);

br_error C2_HOOK_CDECL softrend_renderer_customPartsQuery(br_uint_32* pvalue, void** pextra, br_size_t* pextra_size, void*, br_tv_template_entry* tep);

br_size_t C2_HOOK_CDECL softrend_renderer_customPartsExtra(void* block, br_tv_template_entry* tep);

void* C2_HOOK_CDECL _M_br_soft_renderer_listQuery(br_soft_renderer* self);

br_error C2_HOOK_CDECL _M_br_soft_renderer_validDestination(br_soft_renderer* self, br_boolean* bp, br_object* h);

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateStoredNew(br_soft_renderer* self, br_soft_renderer_state_stored** pss, br_uint_32 mask, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateStoredAvail(br_soft_renderer* self, br_int_32* psize, br_uint_32 mask, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_soft_renderer_bufferStoredNew(br_soft_renderer* self, br_buffer_stored** psm, br_token use, br_device_pixelmap* pm, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_soft_renderer_bufferStoredAvail(br_soft_renderer* self, br_int_32* space, br_token use, br_token_value* tv);

#endif /* SOFTREND_RENDERER_H */
