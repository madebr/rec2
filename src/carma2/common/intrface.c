#include "intrface.h"

int (C2_HOOK_FASTCALL * DoInterfaceScreen_original)(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);
int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice) {

    // Relict from Carmageddon 1, unused here
    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    return DoInterfaceScreen_original(pSpec, pOptions, pCurrent_choice);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004846e0, DoInterfaceScreen, DoInterfaceScreen_original)
