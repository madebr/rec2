#include "input.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gEdge_trigger_mode, 0x0068c1c4);

int (C2_HOOK_FASTCALL * LoadJoystickPreferences_original)(void);
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
#if defined(C2_HOOKS_ENABLED)
    int res = LoadJoystickPreferences_original();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c0b0, LoadJoystickPreferences, LoadJoystickPreferences_original)

int (C2_HOOK_FASTCALL * AnyKeyDown_original)(void);
int C2_HOOK_FASTCALL AnyKeyDown(void) {
#if defined(C2_HOOKS_ENABLED)
    int res = AnyKeyDown_original();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482d70, AnyKeyDown, AnyKeyDown_original)

void (C2_HOOK_FASTCALL * WaitForNoKeys_original)(void);
void C2_HOOK_FASTCALL WaitForNoKeys(void) {
#if defined(C2_HOOKS_ENABLED)
    WaitForNoKeys_original();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004833b0, WaitForNoKeys, WaitForNoKeys_original)

void C2_HOOK_FASTCALL EdgeTriggerModeOn(void) {
    C2V(gEdge_trigger_mode) = 1;
}
C2_HOOK_FUNCTION(0x00484600, EdgeTriggerModeOn)
