#include "rndstate.h"

#include "state.h"

#include "core/fw/tokenval.h"


br_error C2_HOOK_STDCALL CheckPrimitiveState(br_soft_renderer* self) {
    br_error r;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_soft_renderer, state.pstate, 0x794);

    if (self->state.pstate == NULL) {
        r = self->plib->dispatch->_stateNew(self->plib, &self->state.pstate);
        if (r != 0) {
            return r;
        }
        BrResAdd(self, self->state.pstate);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x005417f0, CheckPrimitiveState)
