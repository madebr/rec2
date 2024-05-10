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

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryBuffer(br_soft_renderer* self, br_token part, br_int_32 index, br_uint_32* pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self, &sp, part, index);

    if (tp != NULL) {
        return BrTokenValueQuery(pvalue, buffer, buffer_size, t, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partQueryBuffer, 0x50);
        return self->state.pstate->dispatch->_partQueryBuffer(self->state.pstate, part, index, pvalue, buffer, buffer_size, t);
    }
}
C2_HOOK_FUNCTION(0x00541a60, _M_br_soft_renderer_partQueryBuffer)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryMany(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self, &sp, part, index);

    if (tp != NULL) {
        return BrTokenValueQueryMany(tv, extra, extra_size, pcount, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partQueryMany, 0x54);
        return self->state.pstate->dispatch->_partQueryMany(self->state.pstate, part, index, tv, extra, extra_size, pcount);
    }
}
C2_HOOK_FUNCTION(0x00541b10, _M_br_soft_renderer_partQueryMany)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryManySize(br_soft_renderer* self, br_token part, br_int_32 index, br_size_t* pextra_size, br_token_value* tv) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self, &sp, part, index);
    if (tp != NULL) {
        return BrTokenValueQueryManySize(pextra_size, tv, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partQueryManySize, 0x58);
        return self->state.pstate->dispatch->_partQueryManySize(self->state.pstate, part, index, pextra_size, tv);
    }
}
C2_HOOK_FUNCTION(0x00541bc0, _M_br_soft_renderer_partQueryManySize)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryAll(br_soft_renderer* self, br_token part, br_int_32 index, br_token_value* buffer, br_size_t buffer_size) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self,&sp,part,index);

    if (tp != NULL) {
        return BrTokenValueQueryAll(buffer, buffer_size, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partQueryAll, 0x5c);
        return self->state.pstate->dispatch->_partQueryAll(self->state.pstate, part, index, buffer, buffer_size);
    }
}
C2_HOOK_FUNCTION(0x00541c60, _M_br_soft_renderer_partQueryAll)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partQueryAllSize(br_soft_renderer* self, br_token part, br_int_32 index, br_size_t* psize) {
    br_error r;
    soft_state_all* sp = &self->state;
    br_tv_template* tp;

    tp = FindStateTemplate(self, &sp, part, index);
    if (tp != NULL) {
        return BrTokenValueQueryAllSize(psize, sp, tp);
    } else {
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partQueryAllSize, 0x60);
        return self->state.pstate->dispatch->_partQueryAllSize(self->state.pstate, part, index, psize);
    }
}
C2_HOOK_FUNCTION(0x00541d00, _M_br_soft_renderer_partQueryAllSize)

br_error C2_HOOK_CDECL _M_br_soft_renderer_partIndexQuery(br_soft_renderer* self, br_token part, br_int_32* pnindex) {
    br_int_32 n;
    br_error r;

    C2_HOOK_BUG_ON(BR_ASIZE(self->state.light) != 0x10);
    C2_HOOK_BUG_ON(BR_ASIZE(self->state.clip) != 0x6);

    switch(part) {
    case BRT_CULL:
    case BRT_SURFACE:
    case BRT_MATRIX:
    case BRT_ENABLE:
        n = 1;
        break;

    case BRT_LIGHT:
        n = BR_ASIZE(self->state.light);
        break;

    case BRT_CLIP:
        n = BR_ASIZE(self->state.clip);
        break;

    default:
        n = 0;
        r = CheckPrimitiveState(self);
        if (r != 0) {
            return r;
        }
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_primitive_state_dispatch, _partIndexQuery, 0x64);
        r = self->state.pstate->dispatch->_partIndexQuery(self->state.pstate, part, &n);
        if (r != 0) {
            return r;
        }
    }

    if (pnindex == NULL) {
        return 0x1002;
    }
    *pnindex = n;
    return 0;
}
C2_HOOK_FUNCTION(0x00541da0, _M_br_soft_renderer_partIndexQuery)

br_error C2_HOOK_CDECL _M_br_soft_renderer_modelMulF(br_soft_renderer* self, br_matrix34_f* m) {
    br_matrix34 om;

    om = self->state.matrix.model_to_view;
    BrMatrix34Mul(&self->state.matrix.model_to_view, (br_matrix34 *)m, &om);
    self->state.matrix.model_to_view_hint = BRT_NONE;
    TouchModelToView(self);
    return 0;
}
C2_HOOK_FUNCTION(0x00542060, _M_br_soft_renderer_modelMulF)

static void convertM34FixedToFloat(br_matrix34_f* dest, const br_matrix34_x* src)
{
    int i,j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            dest->m[i][j] = BrFixedToFloat(src->m[i][j]);
        }
    }
}

br_error C2_HOOK_CDECL _M_br_soft_renderer_modelMulX(br_soft_renderer* self, br_matrix34_x* m) {
#define CONV(d,s)
    br_matrix34 om;
    br_matrix34 cm;

    om = self->state.matrix.model_to_view;
    convertM34FixedToFloat((br_matrix34_f*)&cm, (const br_matrix34_x*)m);
    BrMatrix34Mul(&self->state.matrix.model_to_view, &cm, &om);
    self->state.matrix.model_to_view_hint = BRT_NONE;
    TouchModelToView(self);
    return 0;
}
C2_HOOK_FUNCTION(0x005420b0, _M_br_soft_renderer_modelMulX)

br_error C2_HOOK_CDECL _M_br_soft_renderer_modelPopPushMulF(br_soft_renderer* self, br_matrix34_f* m) {

    if (self->stack_top == 0) {
        return 0x1005;
    }
    BrMatrix34Mul(&self->state.matrix.model_to_view,
        (br_matrix34*)m,
        &self->state_stack[0].matrix.model_to_view);
    self->state.matrix.model_to_view_hint = BRT_NONE;
    TouchModelToView(self);
    return 0;
}
C2_HOOK_FUNCTION(0x005421c0, _M_br_soft_renderer_modelPopPushMulF)

br_error C2_HOOK_CDECL _M_br_soft_renderer_modelPopPushMulX(br_soft_renderer* self, br_matrix34_x* m) {
    br_matrix34 cm;

    if (self->stack_top == 0) {
        return 0x1005;
    }
    convertM34FixedToFloat((br_matrix34_f*)&cm, (const br_matrix34_x*)m);
    BrMatrix34Mul(
        &self->state.matrix.model_to_view,
        &cm,
        &self->state_stack[0].matrix.model_to_view);
    self->state.matrix.model_to_view_hint = BRT_NONE;
    TouchModelToView(self);
    return 0;
}
C2_HOOK_FUNCTION(0x00542130, _M_br_soft_renderer_modelPopPushMulX)
