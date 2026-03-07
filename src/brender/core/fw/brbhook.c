#include "brbhook.h"

#include "devlist.h"

br_device* C2_HOOK_CDECL BrDrv1SoftRendBegin(const char* arguments);

void (C2_HOOK_CDECL * _BrBeginHook_original)(void);
// FUNCTION: CARMA2_HW 0x0051d610
void C2_HOOK_CDECL _BrBeginHook(void) {

    BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
}

// FUNCTION: CARMA2_HW 0x0051d630
void C2_HOOK_CDECL _BrEndHook(void) {

}
