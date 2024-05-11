#include "v1model.h"


br_error (C2_HOOK_STDCALL * V1Model_Render_original)(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen);
br_error C2_HOOK_STDCALL V1Model_Render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type, br_boolean on_screen) {

#if defined(C2_HOOKS_ENABLED)
    return V1Model_Render_original(self, renderer, model, default_state, type, on_screen);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542960, V1Model_Render, V1Model_Render_original)

br_error C2_HOOK_CDECL _M_br_geometry_v1_model_soft_render(br_geometry_v1_model_soft* self, br_renderer* renderer, v11model* model, br_renderer_state_stored* default_state, br_token type) {
    br_error r;

    r = V1Model_Render(self, renderer, model, default_state, type, 0);

    return r;
}
C2_HOOK_FUNCTION(0x00542930, _M_br_geometry_v1_model_soft_render)
