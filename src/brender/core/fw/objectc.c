#include "objectc.h"

#include "brender/brender.h"

#include "brlists.h"
#include "../std/brstdlib.h"
#include "fwsetup.h"
#include "pattern.h"
#include "tokenval.h"

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

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_findMany_original(self, objects, max_objects, num_objects, type, pattern, tv);
#else
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    tvarg = NULL;
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    n = 0;
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
        if (n < max_objects) {
            objects[n] = he->h;
        }
        n++;
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    *num_objects = n;
    if (n > max_objects) {
        return 0x1004;
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d390, _M_br_object_container_findMany, _M_br_object_container_findMany_original)

br_error (C2_HOOK_CDECL * _M_br_object_container_count_original)(br_object_container* self, br_uint_32* pcount, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL _M_br_object_container_count(br_object_container* self, br_uint_32* pcount, br_token type, char* pattern, br_token_value* tv) {


#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_count_original(self, pcount, type, pattern, tv);
#else
    object_list* hl;
    object_list_entry* he;
    void* tvarg;
    int n;

    hl = self->dispatch->_listQuery(self);
    if (hl == NULL) {
        return 0x1002;
    }
    tvarg = NULL;
    if (tv != NULL) {
        tvarg = self->dispatch->_tokensMatchBegin(self, type, tv);
    }
    n = 0;
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
        n++;
    }
    if (tv != NULL) {
        self->dispatch->_tokensMatchEnd(self, tvarg);
    }
    *pcount = n;
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d490, _M_br_object_container_count, _M_br_object_container_count_original)

void* (C2_HOOK_CDECL * _M_br_object_container_tokensMatchBegin_original)(br_object_container* self, br_token t, br_token_value* tv);
void* C2_HOOK_CDECL _M_br_object_container_tokensMatchBegin(br_object_container* self, br_token t, br_token_value* tv) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_tokensMatchBegin_original(self, t, tv);
#else
    token_match* tm;
    br_int_32 i;

    C2_HOOK_BUG_ON(sizeof(token_match) != 20);

    if (tv == NULL) {
        return 0;
    }
    tm = BrResAllocate(C2V(fw).res, sizeof(token_match), BR_MEMORY_APPLICATION);
    tm->original = tv;
    for (i = 0; tv[i].t != BR_NULL_TOKEN; i++) {
    }
    tm->n = i + 1;
    tm->query = BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(tm->query, tv, i * sizeof(br_token_value));
    return tm;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d560, _M_br_object_container_tokensMatchBegin, _M_br_object_container_tokensMatchBegin_original)

br_boolean (C2_HOOK_CDECL * _M_br_object_container_tokensMatch_original)(br_object_container* self, br_object* h, void* arg);
br_boolean C2_HOOK_CDECL _M_br_object_container_tokensMatch(br_object_container* self, br_object* h, void* arg) {

#if 0//defined(C2_HOOKS_ENABLED)
    return _M_br_object_container_tokensMatch_original(self, h, arg);
#else
    token_match* tm;
    br_size_t s;
    br_int_32 n;

    tm = arg;
    if (arg == NULL) {
        return 1;
    }
    h->dispatch->_queryManySize(h, &s, tm->query);
    if (s > tm->extra_size) {
        if (tm->extra != NULL) {
            BrResFree(tm->extra);
        }
        tm->extra = BrResAllocate(tm, s, BR_MEMORY_APPLICATION);
        tm->extra_size = s;
    }
    h->dispatch->_queryMany(h, tm->query, tm->extra, tm->extra_size, &n);
    return BrTokenValueCompare(tm->original, tm->query);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d5e0, _M_br_object_container_tokensMatch, _M_br_object_container_tokensMatch_original)

void (C2_HOOK_CDECL * _M_br_object_container_tokensMatchEnd_original)(br_object_container* self, void* arg);
void C2_HOOK_CDECL _M_br_object_container_tokensMatchEnd(br_object_container* self, void* arg) {

#if 0//defined(C2_HOOKS_ENABLED)
    _M_br_object_container_tokensMatchEnd_original(self, arg);
#else
    if (arg != NULL) {
        BrResFree(arg);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d670, _M_br_object_container_tokensMatchEnd, _M_br_object_container_tokensMatchEnd_original)

br_error (C2_HOOK_CDECL * BrObjectContainerFree_original)(br_object_container* self, br_token type, char* pattern, br_token_value* tv);
br_error C2_HOOK_CDECL BrObjectContainerFree(br_object_container* self, br_token type, char* pattern, br_token_value* tv) {

#if 0//defined(C2_HOOKS_ENABLED)
    return BrObjectContainerFree_original(self, type, pattern, tv);
#else
    br_error r;
    br_object** handles;
    br_int_32 count;
    br_int_32 n;
    br_int_32 i;

    r = self->dispatch->_count(self, &count, type, pattern, tv);
    if (r != 0) {
        return r;
    }
    handles = BrMemAllocate(count * sizeof(br_object*), BR_MEMORY_DRIVER);
    self->dispatch->_findMany(self, handles, count, &n, type, pattern, tv);
    for (i = 0; i < n; i++) {
        self->dispatch->_remove(self, handles[i]);
        handles[i]->dispatch->_free(handles[i]);
    }
    BrMemFree(handles);
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0052d690, BrObjectContainerFree, BrObjectContainerFree_original)
