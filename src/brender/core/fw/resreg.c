#include "resreg.h"

#include "fwsetup.h"
#include "register.h"

#include <stdio.h>
#include "c2_stdlib.h"

// FUNCTION: CARMA2_HW 0x00527fd0
br_resource_class* C2_HOOK_CDECL BrResClassAdd(br_resource_class* rclass) {
    br_resource_class* r;

    r = BrRegistryAdd(&fw.reg_resource_classes, rclass);
    if (r != NULL) {
        fw.resource_class_index[rclass->res_class] = r;
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x00528000
br_resource_class* C2_HOOK_CDECL BrResClassRemove(br_resource_class* rclass) {
    br_resource_class* r;

    r = BrRegistryRemove(&fw.reg_resource_classes, rclass);
    if (r != NULL) {
        fw.resource_class_index[rclass->res_class] = NULL;
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x00528030
br_resource_class* C2_HOOK_CDECL BrResClassFind(char* pattern) {

    return BrRegistryFind(&fw.reg_resource_classes, pattern);
}

// FUNCTION: CARMA2_HW 0x00528040
br_resclass_find_cbfn* C2_HOOK_CDECL BrResClassFindHook(br_resclass_find_cbfn* hook) {
    br_resclass_find_cbfn* old;

    old = (br_resclass_find_cbfn*)fw.reg_resource_classes.find_failed_hook;
    fw.reg_resource_classes.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}

// FUNCTION: CARMA2_HW 0x00528050
br_uint_32 C2_HOOK_CDECL BrResClassAddMany(br_resource_class** items, int n) {
    int i;

    for (i = 0; i < n; i++) {
        BrResClassAdd(*items++);
    }
    return n;
}

// FUNCTION: CARMA2_HW 0x00528090
br_uint_32 C2_HOOK_CDECL BrResClassRemoveMany(br_resource_class** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        if (BrResClassRemove(*items++) != 0) {
            r++;
        }
    }
    return r;
}

// FUNCTION: CARMA2_HW 0x005280e0
br_uint_32 C2_HOOK_CDECL BrResClassFindMany(char* pattern, br_resource_class** items, int max) {

    return BrRegistryFindMany(&fw.reg_resource_classes, pattern, (void**)items, max);
}

// FUNCTION: CARMA2_HW 0x00528100
br_uint_32 C2_HOOK_CDECL BrResClassCount(char* pattern) {

    return BrRegistryCount(&fw.reg_resource_classes, pattern);
}

// FUNCTION: CARMA2_HW 0x00528110
br_uint_32 C2_HOOK_CDECL BrResClassEnum(char* pattern, br_resclass_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&fw.reg_resource_classes, pattern, (br_enum_cbfn*)callback, arg);
}
