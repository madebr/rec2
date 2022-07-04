#include "utility.h"

static void(*C2_HOOK_FASTCALL RemoveAllBrenderDevices_original)(void);
void C2_HOOK_FASTCALL RemoveAllBrenderDevices(void) {
    RemoveAllBrenderDevices_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, RemoveAllBrenderDevices, RemoveAllBrenderDevices_original);
