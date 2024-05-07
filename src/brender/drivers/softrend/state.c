#include "state.h"

int (C2_HOOK_STDCALL * StateInitialise_original)(soft_state_all* state);
int C2_HOOK_STDCALL StateInitialise(soft_state_all* state) {

#if defined(C2_HOOKS_ENABLED)
    return StateInitialise_original(state);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005414a0, StateInitialise, StateInitialise_original)
