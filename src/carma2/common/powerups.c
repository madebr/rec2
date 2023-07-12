#include "powerups.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tHeadup_icon, gPickedup_powerups, 5, 0x007059e0);

void (C2_HOOK_FASTCALL * InitPowerups_original)(void);
void C2_HOOK_FASTCALL InitPowerups(void) {

    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    InitPowerups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d9ea0, InitPowerups, InitPowerups_original)

void (C2_HOOK_FASTCALL * LoadPowerups_original)(void);
void C2_HOOK_FASTCALL LoadPowerups(void) {

    C2_HOOK_ASSUME_UNUSED();
#if defined(C2_HOOKS_ENABLED)
    LoadPowerups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004d96c0, LoadPowerups, LoadPowerups_original)
