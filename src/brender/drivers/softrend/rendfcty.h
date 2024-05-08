#ifndef SOFTREND_FACILITY_H
#define SOFTREND_FACILITY_H

#include "types.h"

#include <brender/brender.h>

#include "c2_hooks.h"

br_renderer_facility* C2_HOOK_STDCALL RendererFacilitySoftAllocate(br_device* dev, const char* identifier);

void C2_HOOK_CDECL _M_br_renderer_facility_soft_free(br_soft_renderer_facility* self);

br_token C2_HOOK_CDECL _M_br_renderer_facility_soft_type(br_soft_renderer_facility* self);

int C2_HOOK_CDECL _M_br_renderer_facility_soft_isType(br_soft_renderer_facility* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_renderer_facility_soft_space(br_soft_renderer_facility* self);

br_tv_template* C2_HOOK_CDECL _M_br_renderer_facility_soft_templateQuery(br_soft_renderer_facility* self);

void* C2_HOOK_CDECL _M_br_renderer_facility_soft_listQuery(br_soft_renderer_facility* self);

#endif //SOFTREND_FACILITY_H
