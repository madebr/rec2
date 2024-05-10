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

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSet(br_soft_renderer* self, br_token part, br_int_32 index, br_token t, br_uint_32 value) {
    br_error r;
    soft_state_all *sp = &self->state;
    struct br_tv_template *tp;
    br_uint_32 m = 0;

    tp = FindStateTemplate(self, &sp, part, index);

    if (tp != NULL) {
        r = BrTokenValueSet(sp, &m, t, value, tp);
        if (m != 0) {
            TemplateActions(sp, part, index, m);
        }
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partSet, 0x44);
        r = self->state.pstate->dispatch->_partSet(self->state.pstate, part, index, t, value);
    }

    return r;
}
C2_HOOK_FUNCTION(0x00541830, _M_br_soft_renderer_partSet)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partSetMany(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* tv, br_int_32* pcount) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;
    br_uint_32 m = 0;
    br_int_32 c;

    tp = FindStateTemplate(self, &sp, part, index);

    if (tp != NULL) {
        r = BrTokenValueSetMany(sp, pcount, &m, tv, tp);
        if (m != 0) {
            TemplateActions(sp, part, index, m);
        }
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        c = 0;
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partSetMany, 0x48);
        r = self->state.pstate->dispatch->_partSetMany(self->state.pstate, part, index, tv, &c);

        if (pcount != NULL) {
            *pcount = c;
        }
    }

    return r;
}
C2_HOOK_FUNCTION(0x005418f0, _M_br_soft_renderer_partSetMany)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQuery(br_soft_renderer* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_token t) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self, &sp, part, index);

    if (tp != NULL) {
        return BrTokenValueQuery(pvalue, NULL, 0, t, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        return self->state.pstate->dispatch->_partQuery(self->state.pstate, part, index, pvalue, t);
    }
}
C2_HOOK_FUNCTION(0x005419c0, _M_br_soft_renderer_partQuery)
