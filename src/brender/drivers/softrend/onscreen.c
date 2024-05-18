#include "onscreen.h"


br_token (C2_HOOK_STDCALL * OnScreenCheck_original)(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds);
br_token C2_HOOK_STDCALL OnScreenCheck(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds) {

#if defined(C2_HOOKS_ENABLED)
    return OnScreenCheck_original(self, model_to_screen, bounds);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00544830, OnScreenCheck, OnScreenCheck_original)
