#include "resreg.h"

#include "fwsetup.h"
#include "register.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"

br_resource_class* C2_HOOK_CDECL BrResClassAdd(br_resource_class* rclass) {
    br_resource_class* r;

#if defined(C2_HOOKS_ENABLED)
    if (&C2V(fw).reg_resource_classes != (br_registry*)0x0079f4e4) {
        c2_abort();
    }
#endif

    r = BrRegistryAdd(&C2V(fw).reg_resource_classes, rclass);
    if (r != NULL) {
        C2V(fw).resource_class_index[rclass->res_class] = r;
    }
    return r;
}
C2_HOOK_FUNCTION(0x00527fd0, BrResClassAdd)

br_resource_class* C2_HOOK_CDECL BrResClassRemove(br_resource_class* rclass) {
    br_resource_class* r;

    r = BrRegistryRemove(&C2V(fw).reg_resource_classes, rclass);
    if (r != NULL) {
        C2V(fw).resource_class_index[rclass->res_class] = NULL;
    }
    return r;
}
C2_HOOK_FUNCTION(0x00528000, BrResClassRemove)

br_resource_class* C2_HOOK_CDECL BrResClassFind(char* pattern) {

    return BrRegistryFind(&C2V(fw).reg_resource_classes, pattern);
}
C2_HOOK_FUNCTION(0x00528030, BrResClassFind)

br_resclass_find_cbfn* C2_HOOK_CDECL BrResClassFindHook(br_resclass_find_cbfn* hook) {
    br_resclass_find_cbfn* old;

    old = (br_resclass_find_cbfn*)C2V(fw).reg_resource_classes.find_failed_hook;
    C2V(fw).reg_resource_classes.find_failed_hook = (br_find_failed_cbfn*)hook;
    return old;
}
C2_HOOK_FUNCTION(0x00528040, BrResClassFindHook)

br_uint_32 C2_HOOK_CDECL BrResClassAddMany(br_resource_class** items, int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (BrRegistryAdd(&C2V(fw).reg_resource_classes, items[i]) != NULL) {
            C2V(fw).resource_class_index[items[i]->res_class] = items[i];
        }
    }
    return n;
}
C2_HOOK_FUNCTION(0x00528050, BrResClassAddMany)

br_uint_32 C2_HOOK_CDECL BrResClassRemoveMany(br_resource_class** items, int n) {
    int i;
    int r;

    r = 0;
    for (i = 0; i < n; i++) {
        if (BrRegistryRemove(&C2V(fw).reg_resource_classes, items[i]) != NULL) {
            C2V(fw).resource_class_index[items[i]->res_class] = NULL;
            r++;
        }
    }
    return r;
}
C2_HOOK_FUNCTION(0x00528090, BrResClassRemoveMany)

br_uint_32 C2_HOOK_CDECL BrResClassFindMany(char* pattern, br_resource_class** items, int max) {

    return BrRegistryFindMany(&C2V(fw).reg_resource_classes, pattern, (void**)items, max);
}
C2_HOOK_FUNCTION(0x005280e0, BrResClassFindMany)

br_uint_32 C2_HOOK_CDECL BrResClassCount(char* pattern) {

    return BrRegistryCount(&C2V(fw).reg_resource_classes, pattern);
}
C2_HOOK_FUNCTION(0x00528100, BrResClassCount)

br_uint_32 C2_HOOK_CDECL BrResClassEnum(char* pattern, br_resclass_enum_cbfn* callback, void* arg) {

    return BrRegistryEnum(&C2V(fw).reg_resource_classes, pattern, (br_enum_cbfn*)callback, arg);
}
C2_HOOK_FUNCTION(0x00528110, BrResClassEnum)
