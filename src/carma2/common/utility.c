#include "utility.h"

static br_error (C2_HOOK_FASTCALL * RemoveAllBrenderDevices_original)(void);
br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void) {
    return RemoveAllBrenderDevices_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, RemoveAllBrenderDevices, RemoveAllBrenderDevices_original);
