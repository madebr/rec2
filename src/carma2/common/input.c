#include "input.h"

#include "globvars.h"

#include "platform.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gEdge_trigger_mode, 0x0068c1c4);
C2_HOOK_VARIABLE_IMPLEMENT(tJoy_array, gJoy_array, 0x0074b5c0);
C2_HOOK_VARIABLE_IMPLEMENT(tKey_array, gKey_array, 0x0068bee0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gKey_poll_counter, 0x0068bed4);

int (C2_HOOK_FASTCALL * LoadJoystickPreferences_original)(void);
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
#if defined(C2_HOOKS_ENABLED)
    int res = LoadJoystickPreferences_original();
    return res;
#else
#error "Not implemented"
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

void C2_HOOK_FASTCALL EdgeTriggerModeOff(void) {
    C2V(gEdge_trigger_mode) = 0;
}
C2_HOOK_FUNCTION(0x00484610, EdgeTriggerModeOff)

int (C2_HOOK_FASTCALL * PDKeyDown2_original)(int pKey_index, undefined4 pArg2);
int C2_HOOK_FASTCALL PDKeyDown2(int pKey_index, undefined4 pArg2) {
#if defined(C2_HOOKS_ENABLED)
    return PDKeyDown2_original(pKey_index, pArg2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00482590, PDKeyDown2, PDKeyDown2_original)

int C2_HOOK_FASTCALL PDKeyDown(int pKey_index) {
    tKey_down_result result;

    result = PDKeyDown2(pKey_index, 0);
    if (!C2V(gEdge_trigger_mode) || pKey_index <= 10) {
        return result != tKey_down_no;
    }
    return result == tKey_down_yes || result == tKey_down_repeat;
}
C2_HOOK_FUNCTION(0x00482550, PDKeyDown)

void (C2_HOOK_FASTCALL * PollKeys_original)(void);
void C2_HOOK_FASTCALL PollKeys(void) {

#if defined(C2_HOOKS_ENABLED)
    PollKeys_original();
#else

#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00481eb0, PollKeys, PollKeys_original)

tU32* C2_HOOK_FASTCALL KevKeyService(void) {
    static tU32 sum = 0;
    static tU32 code = 0;
    static tU32 code2 = 0;
    static int last_key = -1;
    static int last_single_key = -1;
    static tU32 last_time = 0;
    static tU32 return_val[2];
    tU32 keys;

    return_val[0] = 0;
    return_val[1] = 0;
    keys = C2V(gKeys_pressed);

    if (keys < 0x6B) {
        last_single_key = C2V(gKeys_pressed);
    } else {
        if (keys > 0x6b00) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) != last_single_key && keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if (keys >> 8 != last_single_key) {
            sum = 0;
            code = 0;
            return return_val;
        }
        if ((keys & 0xff) == last_single_key) {
            keys = keys >> 8;
        }
        keys = keys & 0xff;
    }

    if (keys != 0 && keys != last_key) {
        sum += keys;
        code += keys << 11;
        code = (code >> 17) + (code << 4);
        code2 = (code2 >> 29) + keys * keys + (code2 << 3);
        last_time = PDGetTotalTime();
    } else if ((tU32)PDGetTotalTime() > last_time + 1000) {
        return_val[0] = (code >> 11) + (sum << 21);
        return_val[1] = code2;
        // printf("final value: code=%lx, code2=%lx\n", return_val[0], return_val[1]);
        code = 0;
        code2 = 0;
        sum = 0;
    }
    last_key = keys;
    return return_val;
}
C2_HOOK_FUNCTION(0x00482f10, KevKeyService)

void C2_HOOK_FASTCALL CyclePollKeys(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gKey_array)); i++) {
        if (C2V(gKey_array)[i] > C2V(gKey_poll_counter)) {
            C2V(gKey_array)[i] = 0;
            if (i > 143) {
                C2V(gJoy_array)[i - 143] = -1; // yes this is a little weird I know...
            }
        }
    }
    C2V(gKey_poll_counter) = 0;
}
C2_HOOK_FUNCTION(0x00482160, CyclePollKeys)
