#include "fwsetup.h"

C2_HOOK_VARIABLE_IMPLEMENT(br_framework_state, fw, 0x0079f4e0);

br_filesystem* (C2_HOOK_CDECL * BrFilesystemSet_original)(br_filesystem* newfs);
br_filesystem* C2_HOOK_CDECL BrFilesystemSet(br_filesystem* newfs) {
#if defined(C2_HOOKS_ENABLED)
    return BrFilesystemSet_original(newfs);
#else
    br_filesystem* old;

    old = C2V(fw).fsys;
    if (newfs == NULL) {
        newfs = C2V(_BrDefaultFilesystem);
    }
    C2V(fw).fsys = newfs;
    return old;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00527f90, BrFilesystemSet, BrFilesystemSet_original)
