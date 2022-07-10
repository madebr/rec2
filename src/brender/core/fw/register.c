#include "register.h"

#define HOOK_REGISTER 0

#include "brlists.h"
#include "fwsetup.h"
#include "pattern.h"
#include "resource.h"

void* (C2_HOOK_STDCALL * BrRegistryNew_original)(br_registry* reg);
void* C2_HOOK_STDCALL BrRegistryNew(br_registry* reg) {
#if HOOK_REGISTER
    return BrRegistryNew_original(reg);
#else
    BrNewList(&reg->list);
    reg->count = 0;
    return reg;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529af0, BrRegistryNew, BrRegistryNew_original)

void* (C2_HOOK_STDCALL * BrRegistryClear_original)(br_registry* reg);
void* C2_HOOK_STDCALL BrRegistryClear(br_registry* reg) {
#if HOOK_REGISTER
    return BrRegistryClear_original(reg);
#else
    br_registry_entry* e;

    e = (br_registry_entry*)reg->list.head;
    while (e->node.next != NULL) {
        BrRemove(&e->node);
        BrResFree(e);
        e = (br_registry_entry*)reg->list.head;
    }
    reg->count = 0;
    return reg;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529b10, BrRegistryClear, BrRegistryClear_original)

void* (C2_HOOK_STDCALL * BrRegistryAdd_original)(br_registry* reg, void* item);
void* C2_HOOK_STDCALL BrRegistryAdd(br_registry* reg, void* item) {
#if HOOK_REGISTER
    return BrRegistryAdd_original(reg, item);
#else
    br_registry_entry* e;

    e = (br_registry_entry*)BrResAllocate(C2V(fw).res, sizeof(br_registry_entry), BR_MEMORY_REGISTRY);
    e->item = (char**)item;
    BrAddHead(&reg->list, (br_node*)e);
    reg->count++;
    return item;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529b50, BrRegistryAdd, BrRegistryAdd_original)

int (C2_HOOK_STDCALL * BrRegistryAddMany_original)(br_registry* reg, void** items, int n);
int C2_HOOK_STDCALL BrRegistryAddMany(br_registry* reg, void** items, int n) {
#if HOOK_REGISTER
    return BrRegistryAddMany_original(reg, items, n);
#else
    int i;

    for(i = 0; i < n; i++) {
        BrRegistryAdd(reg, items[i]);
    }
    return n;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529b90, BrRegistryAddMany, BrRegistryAddMany_original)

void* (C2_HOOK_STDCALL * BrRegistryRemove_original)(br_registry* reg, void* item);
void* C2_HOOK_STDCALL BrRegistryRemove(br_registry* reg, void* item) {
#if HOOK_REGISTER
    return BrRegistryRemove_original(reg, item);
#else
    br_registry_entry* e;
    void* r;

    e = (br_registry_entry*)reg->list.head;
    while ((e->node.next != NULL) && (e->item != item)) {
        e = (br_registry_entry*)e->node.next;
    }
    if (e->node.next == NULL) {
        return NULL;
    }
    BrRemove((br_node*)e);
    r = e->item;
    BrResFree(e);
    reg->count--;
    return r;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529be0, BrRegistryRemove, BrRegistryRemove_original)

int (C2_HOOK_STDCALL * BrRegistryRemoveMany_original)(br_registry* reg, void** items, int n);
int C2_HOOK_STDCALL BrRegistryRemoveMany(br_registry* reg, void** items, int n) {
#if HOOK_REGISTER
    return BrRegistryRemoveMany_original(reg, items, n);
#else
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        if (BrRegistryRemove(reg, *items) != NULL) {
            r++;
        }
        items++;
    }
    return r;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529c40, BrRegistryRemoveMany, BrRegistryRemoveMany_original)

void* (C2_HOOK_STDCALL * BrRegistryFind_original)(br_registry* reg, const char* pattern);
void* C2_HOOK_STDCALL BrRegistryFind(br_registry* reg, const char* pattern) {
#if HOOK_REGISTER
    return BrRegistryFind_original(reg, pattern);
#else
    br_registry_entry* e;

    e = (br_registry_entry*)reg->list.head;
    while (e->node.next != NULL) {
        // 2nd element of item must be char pointer.
        if (BrNamePatternMatch(pattern, e->item[1]) != 0) {
            return e->item;
        }
        e = (br_registry_entry*)e->node.next;
    }
    if (reg->find_failed_hook != NULL) {
        return reg->find_failed_hook(pattern);
    }
    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529cc0, BrRegistryFind, BrRegistryFind_original)

int (C2_HOOK_STDCALL * BrRegistryFindMany_original)(br_registry* reg, const char* pattern, void** items, int max);
int C2_HOOK_STDCALL BrRegistryFindMany(br_registry* reg, const char* pattern, void** items, int max) {
#if HOOK_REGISTER
    return BrRegistryFindMany_original(reg, pattern, items, max);
#else
    br_registry_entry* e;
    int n;

    n = 0;
    e = (br_registry_entry*)reg->list.head;
    while ((e->node.next != NULL) && (n < max)) {
        if (BrNamePatternMatch(pattern, e->item[1]) != 0) {
            items[n] = e->item;
            n++;
        }
        e = (br_registry_entry*)e->node.next;
    }
    return n;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529d20, BrRegistryFindMany, BrRegistryFindMany_original)

int (C2_HOOK_STDCALL * BrRegistryCount_original)(br_registry* reg, const char* pattern);
int C2_HOOK_STDCALL BrRegistryCount(br_registry* reg, const char* pattern) {
#if HOOK_REGISTER
    return BrRegistryCount_original(reg, pattern);
#else
    br_registry_entry* e;
    int n;

    if (pattern == NULL) {
        return reg->count;
    }
    n = 0;
    e = (br_registry_entry*)reg->list.head;
    while (e->node.next != NULL) {
        if (BrNamePatternMatch(pattern, e->item[1]) != 0) {
            n++;
        }
        e = (br_registry_entry*)e->node.next;
    }
    return n;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529d70, BrRegistryCount, BrRegistryCount_original)

int (C2_HOOK_STDCALL * BrRegistryEnum_original)(br_registry* reg, const char* pattern, br_enum_cbfn* callback, void* arg);
int C2_HOOK_STDCALL BrRegistryEnum(br_registry* reg, const char* pattern, br_enum_cbfn* callback, void* arg) {
#if HOOK_REGISTER
    return BrRegistryEnum_original(reg, pattern, callback, arg);
#else
    br_registry_entry* e;
    int r;

    if (pattern == NULL) {
        e = (br_registry_entry*)reg->list.tail;
        while (e->node.prev != NULL) {
            r = callback(e->item, arg);
            if (r != 0) {
                return r;
            }
            e = (br_registry_entry*)e->node.prev;
        }
    } else {
        e = (br_registry_entry*)reg->list.tail;
        while (e->node.prev != NULL) {
            // as a char**, e->item[1] actually points to `identifier` field in a br_* struct etc
            r = BrNamePatternMatch(pattern, e->item[1]);
            if (r != 0) {
                r = callback(e->item, arg);
                if (r != 0) {
                    return r;
                }
            }
            e = (br_registry_entry*)e->node.prev;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529dc0, BrRegistryEnum, BrRegistryEnum_original)

void* (C2_HOOK_STDCALL * BrRegistryNewStatic_original)(br_registry* reg, br_registry_entry* base, int limit);
void* C2_HOOK_STDCALL BrRegistryNewStatic(br_registry* reg, br_registry_entry* base, int limit) {
#if HOOK_REGISTER
    return BrRegistryNewStatic_original(reg, base, limit);
#else
    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529e50, BrRegistryNewStatic, BrRegistryNewStatic_original)

void* (C2_HOOK_STDCALL * BrRegistryAddStatic_original)(br_registry* reg, br_registry_entry* base, void* item);
void* C2_HOOK_STDCALL BrRegistryAddStatic(br_registry* reg, br_registry_entry* base, void* item) {
#if HOOK_REGISTER
    return BrRegistryAddStatic_original(reg, base, item);
#else
    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529e60, BrRegistryAddStatic, BrRegistryAddStatic_original)

void* (C2_HOOK_STDCALL * BrRegistryRemoveStatic_original)(br_registry* reg, void* item);
void* C2_HOOK_STDCALL BrRegistryRemoveStatic(br_registry* reg, void* item) {
#if HOOK_REGISTER
    return BrRegistryRemoveStatic_original(reg, item);
#else
    return NULL;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00529e70, BrRegistryRemoveStatic, BrRegistryRemoveStatic_original)
