#ifndef SOFTREND_ONSCREEN_H
#define SOFTREND_ONSCREEN_H

#include "types.h"

#include "c2_hooks.h"

br_token C2_HOOK_STDCALL OnScreenCheck(br_soft_renderer* self, br_matrix4* model_to_screen, br_bounds3* bounds);

#endif /* SOFTREND_ONSCREEN_H */
