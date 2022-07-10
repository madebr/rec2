#include "brbhook.h"

#include "devlist.h"

br_device* C2_HOOK_CDECL BrDrv1SoftRendBegin(const char* arguments);

void (C2_HOOK_CDECL * _BrBeginHook_original)(void);
void C2_HOOK_CDECL _BrBeginHook(void) {
#if defined(C2_HOOKS_ENABLED)
    _BrBeginHook_original();
#else
    BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
#endif
}
C2_HOOK_FUNCTION(0x0051d610, _BrBeginHook)

void C2_HOOK_CDECL _BrEndHook(void) {
}
C2_HOOK_FUNCTION(0x0051d630, _BrEndHook)
