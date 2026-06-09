#include "register.h"

#include "brlists.h"
#include "fwsetup.h"
#include "pattern.h"
#include "resource.h"

// FUNCTION: CARMA2_HW 0x00529af0
void* C2_HOOK_STDCALL BrRegistryNew(br_registry* reg) {

    BrNewList(&reg->list);
    reg->count = 0;
    return reg;
}

// FUNCTION: CARMA2_HW 0x00529b10
void* C2_HOOK_STDCALL BrRegistryClear(br_registry* reg) {
    br_registry_entry* e;

    e = (br_registry_entry*)reg->list.head;
    while (e->node.next != NULL) {
        BrRemove(&e->node);
        BrResFree(e);
        e = (br_registry_entry*)reg->list.head;
    }
    reg->count = 0;
    return reg;
}

// FUNCTION: CARMA2_HW 0x00529b50
void* C2_HOOK_STDCALL BrRegistryAdd(br_registry* reg, void* item) {
    br_registry_entry* e;

    e = (br_registry_entry*)BrResAllocate(fw.res, sizeof(br_registry_entry), BR_MEMORY_REGISTRY);
    e->item = (char**)item;
    BrAddHead(&reg->list, (br_node*)e);
    reg->count++;
    return item;
}

// FUNCTION: CARMA2_HW 0x00529b90
int C2_HOOK_STDCALL BrRegistryAddMany(br_registry* reg, void** items, int n) {
    int i;

    for(i = 0; i < n; i++) {
        BrRegistryAdd(reg, items[i]);
    }
    return n;
}

// FUNCTION: CARMA2_HW 0x00529be0
void* C2_HOOK_STDCALL BrRegistryRemove(br_registry* reg, void* item) {
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
}

// FUNCTION: CARMA2_HW 0x00529c40
int C2_HOOK_STDCALL BrRegistryRemoveMany(br_registry* reg, void** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        if (BrRegistryRemove(reg, *items++) != NULL) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x00529cc0
void* C2_HOOK_STDCALL BrRegistryFind(br_registry* reg, const char* pattern) {
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
}

// FUNCTION: CARMA2_HW 0x00529d20
int C2_HOOK_STDCALL BrRegistryFindMany(br_registry* reg, const char* pattern, void** items, int max) {
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
}

// FUNCTION: CARMA2_HW 0x00529d70
int C2_HOOK_STDCALL BrRegistryCount(br_registry* reg, const char* pattern) {
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
}

// FUNCTION: CARMA2_HW 0x00529dc0
int C2_HOOK_STDCALL BrRegistryEnum(br_registry* reg, const char* pattern, br_enum_cbfn* callback, void* arg) {
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
}

// FUNCTION: CARMA2_HW 0x00529e50
void* C2_HOOK_STDCALL BrRegistryNewStatic(br_registry* reg, br_registry_entry* base, int limit) {

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00529e60
void* C2_HOOK_STDCALL BrRegistryAddStatic(br_registry* reg, br_registry_entry* base, void* item) {

    return NULL;
}

// FUNCTION: CARMA2_HW 0x00529e70
void* C2_HOOK_STDCALL BrRegistryRemoveStatic(br_registry* reg, void* item) {

    return NULL;
}
