#include "input.h"

int (C2_HOOK_FASTCALL * LoadJoystickPreferences_original)(void);
int C2_HOOK_FASTCALL LoadJoystickPreferences() {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    int res = LoadJoystickPreferences_original();
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c0b0, LoadJoystickPreferences, LoadJoystickPreferences_original)

int (C2_HOOK_FASTCALL * AnyKeyDown_original)(void);
int C2_HOOK_FASTCALL AnyKeyDown(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    int res = AnyKeyDown_original();
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482d70, AnyKeyDown, AnyKeyDown_original)
