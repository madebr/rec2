#ifndef SOFTREND_V1MODEL_H
#define SOFTREND_V1MODEL_H

#include "types.h"

#include "c2_hooks.h"

br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen);

#endif /* SOFTREND_V1MODEL_H */
