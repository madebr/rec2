#ifndef SOFTREND_V1MODEL_H
#define SOFTREND_V1MODEL_H

#include "types.h"

#include "c2_hooks.h"

void C2_HOOK_CDECL V1Faces_ScratchAllocate(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL V1Face_CullNone(br_geometry* self, br_soft_renderer* renderer);

void C2_HOOK_CDECL V1Face_OS_CullNone(br_geometry* self, br_soft_renderer* renderer);

br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type);

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_renderOnScreen(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type);

#endif /* SOFTREND_V1MODEL_H */
