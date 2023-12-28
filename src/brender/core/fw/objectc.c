#include "objectc.h"

#include "brender/brender.h"

#include "brlists.h"
#include "pattern.h"

void* (C2_HOOK_CDECL * BrObjectListAllocate_original)(void* res);
void* C2_HOOK_CDECL BrObjectListAllocate(void* res) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrObjectListAllocate_original(res);
#else

    object_list* hl;

    hl = BrResAllocate(res, sizeof(object_list), BR_MEMORY_OBJECT_LIST);
    BrSimpleNewList(&hl->l);
    return hl;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d190, BrObjectListAllocate, BrObjectListAllocate_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_addFront_original)(br_object_container* self, br_object* ph);
br_error C2_HOOK_CDECL _M_br_object_container_addFront(br_object_container* self, br_object* ph) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_addFront_original(self, ph);
#else
    object_list* hl;
    object_list_entry* he;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_object_container_dispatch, _listQuery, 0x44);
    C2_HOOK_BUG_ON(sizeof(object_list_entry) != 0xc);

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    he = BrResAllocate(hl, sizeof(object_list_entry), BR_MEMORY_OBJECT_LIST_ENTRY);
    if (he == NULL) {
        return 0x1003;
    }
    he->h = ph;
    BrSimpleAddHead(&hl->l, &he->n);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d1c0, _M_br_object_container_addFront, _M_br_object_container_addFront_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_remove_original)(br_object_container* self, br_object* h);
br_error C2_HOOK_CDECL _M_br_object_container_remove(br_object_container* self, br_object* h) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_remove_original(self, h);
#else
    object_list* hl;
    object_list_entry* he;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    for (he = (object_list_entry*)hl->l.head; he != NULL && he->h != h; he = (object_list_entry*)he->n.next) {
    }
    if (he != NULL && he->h == h) {
        BrSimpleRemove(&he->n);
        BrResFree(he);
        return 0;
    }
    return 0x1002;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d210, _M_br_object_container_remove, _M_br_object_container_remove_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_removeFront_original)(br_object_container* self, br_object** ph);
br_error C2_HOOK_CDECL _M_br_object_container_removeFront(br_object_container* self, br_object** ph) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_removeFront_original(self, ph);
#else
    object_list* hl;
    object_list_entry* he;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    he = (object_list_entry*)BrSimpleRemHead(&hl->l);
    if (he == NULL) {
        *ph = NULL;
        return 0x1002;
    }
    *ph = he->h;
    BrResFree(he);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d260, _M_br_object_container_removeFront, _M_br_object_container_removeFront_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_find_original)(br_object_container* self, br_object** ph, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_object_container_find(br_object_container* self, br_object** ph, br_token type, char* pattern, br_token_value* tv) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_find_original(self, ph, type, pattern, tv);
#else
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    br_error r;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    tvarg = NULL;
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    r = 0x1002;
    for (he = (object_list_entry*)hl->l.head; he != NULL; he = (object_list_entry*)he->n.next) {
        if (type != BR_NULL_TOKEN && he->h->dispatch->_type(he->h) != type) {
            continue;
        }
        if (!BrNamePatternMatch(pattern, he->h->dispatch->_identifier(he->h))) {
            continue;
        }
        if (tv != NULL && !self->dispatch->_tokensMatch(self, he->h, tvarg)) {
            continue;
        }
        r = 0;
        *ph = he->h;
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    return r;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d2b0, _M_br_object_container_find, _M_br_object_container_find_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_findMany_original)(br_object_container* self, br_object** objects, br_int_32 max_objects, br_int_32* num_objects, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_object_container_findMany(br_object_container* self, br_object** objects, br_int_32 max_objects, br_int_32* num_objects, br_token type, char* pattern, br_token_value* tv) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_findMany_original(self, objects, max_objects, num_objects, type, pattern, tv);
#else
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;
    LOG_TRACE("(%p, %p, %d, %p, %d, \"%s\", %p)", self, objects, max_objects, num_objects, type, pattern, tv);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d390, _M_br_object_container_findMany, _M_br_object_container_findMany_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_count_original)(br_object_container* self, br_uint_32* pcount, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_object_container_count(br_object_container* self, br_uint_32* pcount, br_token type, char* pattern, br_token_value* tv) {


#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_count_original(self, pcount, type, pattern, tv);
#else
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;
    LOG_TRACE("(%p, %p, %d, \"%s\", %p)", self, pcount, type, pattern, tv);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d490, _M_br_object_container_count, _M_br_object_container_count_original)

void* (C2_HOOK_CDECL * _M_br_object_container_tokensMatchBegin_original)(br_object_container* self, br_token t, br_token_value* tv);
void* C2_HOOK_CDECL _M_br_object_container_tokensMatchBegin(br_object_container* self, br_token t, br_token_value* tv) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_tokensMatchBegin_original(self, t, tv);
#else
    token_match* tm;
    br_int_32 i;
    LOG_TRACE("(%p, %d, %p)", self, t, tv);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d560, _M_br_object_container_tokensMatchBegin, _M_br_object_container_tokensMatchBegin_original)

br_boolean (C2_HOOK_CDECL * _M_br_object_container_tokensMatch_original)(br_object_container* self, br_object* h, void* arg);
br_boolean C2_HOOK_CDECL _M_br_object_container_tokensMatch(br_object_container* self, br_object* h, void* arg) {

#if defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_tokensMatch_original(self, h, arg);
#else
    token_match* tm;
    br_size_t s;
    br_int_32 n;
    LOG_TRACE("(%p, %p, %p)", self, h, arg);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d5e0, _M_br_object_container_tokensMatch, _M_br_object_container_tokensMatch_original)

void (C2_HOOK_CDECL * _M_br_object_container_tokensMatchEnd_original)(br_object_container* self, void* arg);
void C2_HOOK_CDECL _M_br_object_container_tokensMatchEnd(br_object_container* self, void* arg) {

#if defined(C2_HOOKS_ENABLED)
    _M_br_object_container_tokensMatchEnd_original(self, arg);
#else
    LOG_TRACE("(%p, %p)", self, arg);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d670, _M_br_object_container_tokensMatchEnd, _M_br_object_container_tokensMatchEnd_original)

br_error (C2_HOOK_CDECL * BrObjectContainerFree_original)(br_object_container* self, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrObjectContainerFree(br_object_container* self, br_token type, char* pattern, br_token_value* tv) {

#if defined(C2_HOOKS_ENABLED)
    return BrObjectContainerFree_original(self, type, pattern, tv);
#else
    br_error r;
    br_object** handles;
    br_int_32 count;
    br_int_32 n;
    br_int_32 i;
    LOG_TRACE("(%p, %d, \"%s\", %p)", self, type, pattern, tv);
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d690, BrObjectContainerFree, BrObjectContainerFree_original)
