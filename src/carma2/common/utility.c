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

char* (C2_HOOK_FASTCALL * GetALineWithNoPossibleService_original)(tTWTFILE* pF, char* pS);
char* C2_HOOK_FASTCALL GetALineWithNoPossibleService(tTWTFILE* pF, char* pS) {
#if defined(C2_HOOKS_ENABLED)
    return GetALineWithNoPossibleService_original(pF, pS);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490f30, GetALineWithNoPossibleService, GetALineWithNoPossibleService_original)

void (C2_HOOK_FASTCALL * PossibleService_original)(void);
void C2_HOOK_FASTCALL PossibleService(void) {
#if defined(C2_HOOKS_ENABLED)
    PossibleService_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x005155d0, PossibleService, PossibleService_original)
