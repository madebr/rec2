#include "rndstate.h"

#include "onscreen.h"
#include "setup.h"
#include "sstate.h"
#include "state.h"

#include "core/fw/tokenval.h"

#include "c2_string.h"

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

br_error C2_HOOK_CDECL _M_br_soft_renderer_modelInvert(br_soft_renderer* self) {
    br_matrix34 old;

    BrMatrix34Copy(&old, &self->state.matrix.model_to_view);
    if (self->state.matrix.model_to_view_hint == BRT_LENGTH_PRESERVING) {
        BrMatrix34LPInverse(&self->state.matrix.model_to_view, &old);
    } else {
        BrMatrix34Inverse(&self->state.matrix.model_to_view, &old);
    }
    TouchModelToView(self);
    return 0;
}
C2_HOOK_FUNCTION(0x00542210, _M_br_soft_renderer_modelInvert)

br_error C2_HOOK_STDCALL StateCopy(soft_state_all* dest, soft_state_all* src, br_uint_32 copy_mask, void* res) {
    int i;

    if (copy_mask & 0x41) {
        copy_mask |= 0x100;
    }
    copy_mask &= src->valid;
    dest->valid |= copy_mask;

    if (copy_mask & ~0x7f) {
        if (dest->pstate != NULL) {
            if (src->pstate != NULL) {
                dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
            } else {
                dest->pstate->dispatch->_stateDefault(dest->pstate, copy_mask);
            }
        } else {
            if (src->pstate != NULL) {
                if (src->renderer->plib->dispatch->_stateNew(src->renderer->plib, &dest->pstate) == 0) {
                    if (res != NULL) {
                        BrResAdd(res, dest->pstate);
                    }
                    dest->pstate->dispatch->_stateCopy(dest->pstate, src->pstate, copy_mask);
                }
            }
        }
    }

    if (copy_mask & 0x40) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cull) != 0xc);
        c2_memcpy(&dest->cull, &src->cull, sizeof(soft_state_cull));
    }

    if (copy_mask & 0x1) {
        C2_HOOK_BUG_ON(sizeof(soft_state_surface) != 0x48);
        c2_memcpy(&dest->surface, &src->surface, sizeof(soft_state_surface));
    }

    if (copy_mask & 0x2) {
        C2_HOOK_BUG_ON(sizeof(soft_state_matrix) != 0xbc);
        c2_memcpy(&dest->matrix, &src->matrix, sizeof(soft_state_matrix));
    }

    if (copy_mask & 0x4) {
        C2_HOOK_BUG_ON(sizeof(soft_state_enable) != 0x8);
        c2_memcpy(&dest->enable, &src->enable, sizeof(soft_state_enable));
    }

    if (copy_mask & 0x20) {
        C2_HOOK_BUG_ON(sizeof(soft_state_bounds) != 0x10);
        c2_memcpy(&dest->bounds, &src->bounds, sizeof(soft_state_bounds));
    }

    if (copy_mask & 0x8) {
        C2_HOOK_BUG_ON(sizeof(soft_state_light) != 0x3c);
        for (i = 0; i < BR_ASIZE(src->light); i++) {
            c2_memcpy(&dest->light[i], &src->light[i], sizeof(soft_state_light));
        }
        dest->timestamp_lights = src->timestamp_lights;
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, timestamp_lights, 0x41c);
    }

    if (copy_mask & 0x10) {
        C2_HOOK_BUG_ON(sizeof(soft_state_clip) != 0x14);
        for (i = 0; i < BR_ASIZE(src->clip); i++) {
            c2_memcpy(&dest->clip[i], &src->clip[i], sizeof(soft_state_clip));
        }
        dest->timestamp_clips = src->timestamp_clips;
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(soft_state_all, timestamp_clips, 0x498);
    }

    if (copy_mask & 0x100) {
        C2_HOOK_BUG_ON(sizeof(soft_state_cache) != 0x1ec);
        c2_memcpy(&dest->cache, &src->cache, sizeof(soft_state_cache));
    }

    if ((copy_mask & 0x41) != 0x41) {
        dest->cache.valid = 0;
    }

    return 0;
}
C2_HOOK_FUNCTION(0x00541e70, StateCopy)

br_error C2_HOOK_CDECL _M_br_soft_renderer_statePush(br_soft_renderer* self, br_uint_32 mask) {
    soft_state_all* sp;

    if (self->stack_top >= MAX_STATE_STACK) {
        return 0x1004;
    }
    sp = &self->state_stack[self->stack_top];
    self->stack_top += 1;
    return StateCopy(sp, &self->state, mask, self);
}
C2_HOOK_FUNCTION(0x00542280, _M_br_soft_renderer_statePush)

br_error C2_HOOK_CDECL _M_br_soft_state_statePop(br_soft_renderer* self, br_uint_32 mask) {
    soft_state_all* sp;
    br_error r;

    if (self->stack_top == 0) {
        return 0x1005;
    }
    self->stack_top -= 1;
    sp = &self->state_stack[self->stack_top];
    r = StateCopy(&self->state, sp, mask, self);
    sp->valid = 0;
    return r;
}
C2_HOOK_FUNCTION(0x005422c0, _M_br_soft_state_statePop)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateSave(br_soft_renderer* self, br_renderer_state_stored_soft* save, br_uint_32 mask) {

    return StateCopyToStored(save, &self->state, mask, save);
}
C2_HOOK_FUNCTION(0x00542410, _M_br_soft_renderer_stateSave)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateRestore(br_soft_renderer* self, br_renderer_state_stored_soft* save, br_uint_32 mask) {

    self->last_restored = save;
    return StateCopyFromStored(&self->state, save, mask, self);
}
C2_HOOK_FUNCTION(0x00542430, _M_br_soft_renderer_stateRestore)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateMask(br_soft_renderer* self, br_uint_32* mask, br_token* parts, int n_parts) {
    int i;
    br_uint_32 m;

    m = 0;
    self->plib->dispatch->_mask(self->plib, &m, parts, n_parts);
    for (i = 0; i < n_parts; i++) {
        switch (parts[i]) {
        case BRT_CULL:
            m |= 0x40;
            break;
        case BRT_SURFACE:
            m |= 0x01;
            break;
        case BRT_MATRIX:
            m |= 0x02;
            break;
        case BRT_ENABLE:
            m |= 0x04;
            break;
        case BRT_LIGHT:
            m |= 0x08;
            break;
        case BRT_CLIP:
            m |= 0x10;
            break;
        }
    }
    *mask = m;
    return 0;
}
C2_HOOK_FUNCTION(0x00542490, _M_br_soft_renderer_stateMask)

br_error C2_HOOK_CDECL _M_br_soft_renderer_stateDefault(br_soft_renderer* self, br_uint_32 mask) {

    if(self->state.pstate != NULL) {
        self->state.pstate->dispatch->_stateDefault(self->state.pstate, mask);
    }
    return StateCopy(&self->state, self->default_state, mask & 0x7f, self);
}
C2_HOOK_FUNCTION(0x00542450, _M_br_soft_renderer_stateDefault)

br_error (C2_HOOK_CDECL * _M_br_soft_renderer_boundsTestF_original)(br_soft_renderer* self, br_token* r, br_bounds3_f* bounds);
br_error C2_HOOK_CDECL _M_br_soft_renderer_boundsTestF(br_soft_renderer* self, br_token* r, br_bounds3_f* bounds) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_soft_renderer_boundsTestF_original(self, r, bounds);
#else

    if (!C2V(scache).valid_m2s) {
        ModelToScreenUpdate(self);
        C2V(scache).valid_m2s = 1;
    }

    *r = OnScreenCheck(self, &C2V(scache).model_to_screen, (br_bounds3 *)bounds);

    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542390, _M_br_soft_renderer_boundsTestF, _M_br_soft_renderer_boundsTestF_original)

br_error (C2_HOOK_CDECL * _M_br_soft_renderer_boundsTestX_original)(br_soft_renderer* self, br_token* r, br_bounds3_x* bounds_in);
br_error C2_HOOK_CDECL _M_br_soft_renderer_boundsTestX(br_soft_renderer* self, br_token* r, br_bounds3_x* bounds_in) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_soft_renderer_boundsTestX_original(self, r, bounds_in);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00542320, _M_br_soft_renderer_boundsTestX, _M_br_soft_renderer_boundsTestX_original)

br_error C2_HOOK_CDECL _M_br_soft_renderer_coverageTestF(br_soft_renderer* self, br_float* r, br_bounds3_f* bounds) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x005423d0, _M_br_soft_renderer_coverageTestF)

br_error C2_HOOK_CDECL _M_br_soft_renderer_coverageTestX(br_soft_renderer* self, br_fixed_ls* r, br_bounds3_x* bounds) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x005423e0, _M_br_soft_renderer_coverageTestX)

br_error C2_HOOK_CDECL _M_br_soft_renderer_viewDistanceF(br_soft_renderer* self, br_float* r) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x005423f0, _M_br_soft_renderer_viewDistanceF)

br_error C2_HOOK_CDECL _M_br_soft_renderer_viewDistanceX(br_soft_renderer* self, br_fixed_ls* r) {

    return 0x1002;
}
C2_HOOK_FUNCTION(0x00542400, _M_br_soft_renderer_viewDistanceX)
