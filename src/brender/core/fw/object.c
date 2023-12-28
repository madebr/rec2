#include "object.h"

#include "tokenval.h"

br_error (C2_HOOK_CDECL * _M_br_object_query_original)(br_object* self, br_uint_32* pvalue, br_token t);
br_error C2_HOOK_CDECL _M_br_object_query(br_object* self, br_uint_32* pvalue, br_token t) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_query_original(self, pvalue, t);
#else
    return BrTokenValueQuery(pvalue, NULL, 0, t, self, self->dispatch->_templateQuery(self));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d060, _M_br_object_query, _M_br_object_query_original)

br_error (C2_HOOK_CDECL * _M_br_object_queryBuffer_original)(br_object* self, br_uint_32* pvalue, br_uint_32* buffer, br_size_t buffer_size, br_token t);
br_error C2_HOOK_CDECL _M_br_object_queryBuffer(br_object* self, br_uint_32* pvalue, br_uint_32* buffer, br_size_t buffer_size, br_token t) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_queryBuffer_original(self, pvalue, buffer, buffer_size, t);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d090, _M_br_object_queryBuffer, _M_br_object_queryBuffer_original)

br_error (C2_HOOK_CDECL * _M_br_object_queryMany_original)(br_object* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount);
br_error C2_HOOK_CDECL _M_br_object_queryMany(br_object* self, br_token_value* tv, void* extra, br_size_t extra_size, br_int_32* pcount) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_queryMany_original(self, tv, extra, extra_size, pcount);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d0c0, _M_br_object_queryMany, _M_br_object_queryMany_original)

br_error (C2_HOOK_CDECL * _M_br_object_queryManySize_original)(br_object* self, br_size_t* pextra_size, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_object_queryManySize(br_object* self, br_size_t* pextra_size, br_token_value* tv) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_queryManySize_original(self, pextra_size, tv);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d0f0, _M_br_object_queryManySize, _M_br_object_queryManySize_original)

br_error (C2_HOOK_CDECL * _M_br_object_queryAll_original)(br_object* self, br_token_value* buffer, br_size_t buffer_size);
br_error C2_HOOK_CDECL _M_br_object_queryAll(br_object* self, br_token_value* buffer, br_size_t buffer_size) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_queryAll_original(self, buffer, buffer_size);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d120, _M_br_object_queryAll, _M_br_object_queryAll_original)

br_error (C2_HOOK_CDECL * _M_br_object_queryAllSize_original)(br_object* self, br_size_t* psize);
br_error C2_HOOK_CDECL _M_br_object_queryAllSize(br_object* self, br_size_t* psize) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_queryAllSize_original(self, psize);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d150, _M_br_object_queryAllSize, _M_br_object_queryAllSize_original)

void (C2_HOOK_CDECL * _BrObjectFree_original)(void* res, br_uint_8 res_class, br_size_t size);
void C2_HOOK_CDECL _BrObjectFree(void* res, br_uint_8 res_class, br_size_t size) {

#if defined(C2_HOOKS_ENABLED)
    _BrObjectFree_original(res, res_class, size);
#else
    br_object* o;

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d170, _BrObjectFree, _BrObjectFree_original)
