#include "brbhook.h"

#include "devlist.h"

br_device* C2_HOOK_CDECL BrDrv1SoftRendBegin(char* arguments);

void C2_HOOK_CDECL _BrBeginHook(void) {
    BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
}
C2_HOOK_FUNCTION(0x0051d610, _BrBeginHook)

void C2_HOOK_CDECL _BrEndHook(void) {
}
C2_HOOK_FUNCTION(0x0051d630, _BrEndHook)
