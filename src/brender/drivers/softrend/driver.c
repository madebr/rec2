#include <brender/brender.h>

#include "c2_hooks.h"

br_device* (C2_HOOK_CDECL * BrDrv1Begin_original)(char* arguments);
br_device* C2_HOOK_CDECL BrDrv1Begin(char* arguments) {
#if defined(C2_HOOKS_ENABLED)
    return BrDrv1Begin_original(arguments);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00540590, BrDrv1Begin, BrDrv1Begin_original)
