#include "object.h"

#include "tokenval.h"

// FUNCTION: CARMA2_HW 0x0052d060
br_error C2_HOOK_CDECL _M_br_object_query(br_object* self, br_uint_32* pvalue, br_token t) {

    return BrTokenValueQuery(pvalue, NULL, 0, t, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d090
br_error C2_HOOK_CDECL _M_br_object_queryBuffer(br_object* self, br_uint_32* pvalue, br_uint_32* buffer, br_size_t buffer_size, br_token t) {

    return BrTokenValueQuery(pvalue, buffer, buffer_size, t, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d0c0
br_error C2_HOOK_CDECL _M_br_object_queryMany(br_object* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {

    return BrTokenValueQueryMany(tv, extra, extra_size, pcount, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d0f0
br_error C2_HOOK_CDECL _M_br_object_queryManySize(br_object* self, br_size_t* pextra_size, br_token_value* tv) {

    return BrTokenValueQueryManySize(pextra_size, tv, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d120
br_error C2_HOOK_CDECL _M_br_object_queryAll(br_object* self, br_token_value* buffer, br_size_t buffer_size) {

    return BrTokenValueQueryAll(buffer, buffer_size, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d150
br_error C2_HOOK_CDECL _M_br_object_queryAllSize(br_object* self, br_size_t* psize) {

    return BrTokenValueQueryAllSize(psize, self, self->dispatch->_templateQuery(self));
}

// FUNCTION: CARMA2_HW 0x0052d170
void C2_HOOK_CDECL _BrObjectFree(void* res, br_uint_8 res_class, br_size_t size) {

    br_object* o;

    o = res;
    if (o != NULL && o->dispatch != NULL) {
        o->dispatch->_free(o);
    }
}
