#ifndef SOFTREND_STATE_H
#define SOFTREND_STATE_H

#include "types.h"

#include "c2_hooks.h"

int C2_HOOK_STDCALL StateInitialise(soft_state_all* state);

br_tv_template* C2_HOOK_STDCALL FindStateTemplate(br_soft_renderer* self, soft_state_all** state, br_token part, br_int_32 index);

void C2_HOOK_STDCALL TemplateActions(soft_state_all *state, br_token part, br_int_32 index, br_uint_32 mask);

#endif /* SOFTREND_STATE_H */
