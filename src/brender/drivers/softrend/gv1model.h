#ifndef SOFTREND_GV1MODEL_H
#define SOFTREND_GV1MODEL_H

#include "types.h"

#include "c2_hooks.h"

br_geometry_v1_model* C2_HOOK_STDCALL GeometryV1ModelAllocate(br_soft_renderer_facility *type, const char *id);

void C2_HOOK_CDECL _M_br_geometry_v1_model_soft_free(br_geometry_v1_model_soft* self);

br_token C2_HOOK_CDECL _M_br_geometry_v1_model_soft_type(br_geometry_v1_model_soft* self);

br_boolean C2_HOOK_CDECL _M_br_geometry_v1_model_soft_isType(br_geometry_v1_model_soft* self, br_token t);

br_int_32 C2_HOOK_CDECL _M_br_geometry_v1_model_soft_space(br_geometry_v1_model_soft* self);

br_tv_template* C2_HOOK_CDECL _M_br_geometry_v1_model_soft_templateQuery(br_geometry_v1_model_soft* self);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderFixedToFloat(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_token type);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreenFixedToFloat(br_geometry* self, br_renderer* renderer, v11model* model, br_token type);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedNew(br_geometry* self, br_renderer* renderer, br_geometry_stored** psg, v11model* model, br_token type, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedNewFixedToFloat(br_geometry* self, br_renderer* renderer, br_geometry_stored** psg, v11model* model, br_token type, br_token_value* tv);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_storedAvail(br_geometry* self, br_int_32* psize, br_token_value* tv);

#endif /* SOFTREND_GV1MODEL_H */
