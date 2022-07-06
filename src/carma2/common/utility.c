#include "utility.h"

#include "platform.h"

static br_error (C2_HOOK_FASTCALL * RemoveAllBrenderDevices_original)(void);
br_error C2_HOOK_FASTCALL RemoveAllBrenderDevices(void) {
    return RemoveAllBrenderDevices_original();
}
C2_HOOK_FUNCTION_ORIGINAL(0x00513400, RemoveAllBrenderDevices, RemoveAllBrenderDevices_original);

int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {
    return PDCheckDriveExists2(pThe_path, NULL, 0);
}
C2_HOOK_FUNCTION(0x00515950, PDCheckDriveExists)
