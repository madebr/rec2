#include "win32_dinput.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "input.h"
#include "utility.h"

#include <windows.h>
#include <dinput.h>

#include <stdio.h>
#include <string.h>

// FIXME: better name + use directinput macros for initial variables
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_table, 151, 0x006ad1f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_shift_table, 151, 0x006ac588);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gScan_code, 151, 0x006b3470);

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUTA, gDirectInput, 0x006acc78);

C2_HOOK_VARIABLE_IMPLEMENT(int, gJoystick_deadzone, 0x00762280);

int (C2_HOOK_FASTCALL * JoystickDInputBegin_original)(void);
int C2_HOOK_FASTCALL JoystickDInputBegin(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    int res = JoystickDInputBegin_original();
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00458860, JoystickDInputBegin, JoystickDInputBegin_original)

void (C2_HOOK_FASTCALL * JoystickDInputDetect_original)(void);
void C2_HOOK_FASTCALL JoystickDInputDetect(void) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    JoystickDInputDetect_original();
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c410, JoystickDInputDetect, JoystickDInputDetect_original)

void C2_HOOK_FASTCALL KeyBegin(void) {
    memset(C2V(gASCII_table), 0, sizeof(C2V(gASCII_table)));
    memset(C2V(gASCII_shift_table), 0, sizeof(C2V(gASCII_shift_table)));

    C2V(gASCII_table)[11] = 0x30;
    C2V(gASCII_table)[12] = 0x31;
    C2V(gASCII_table)[13] = 0x32;
    C2V(gASCII_table)[14] = 0x33;
    C2V(gASCII_table)[15] = 0x34;
    C2V(gASCII_table)[16] = 0x35;
    C2V(gASCII_table)[17] = 0x36;
    C2V(gASCII_table)[18] = 0x37;
    C2V(gASCII_table)[19] = 0x38;
    C2V(gASCII_table)[20] = 0x39;
    C2V(gASCII_table)[21] = 0x61;
    C2V(gASCII_table)[22] = 0x62;
    C2V(gASCII_table)[23] = 0x63;
    C2V(gASCII_table)[24] = 0x64;
    C2V(gASCII_table)[25] = 0x65;
    C2V(gASCII_table)[26] = 0x66;
    C2V(gASCII_table)[27] = 0x67;
    C2V(gASCII_table)[28] = 0x68;
    C2V(gASCII_table)[29] = 0x69;
    C2V(gASCII_table)[30] = 0x6a;
    C2V(gASCII_table)[31] = 0x6b;
    C2V(gASCII_table)[32] = 0x6c;
    C2V(gASCII_table)[33] = 0x6d;
    C2V(gASCII_table)[34] = 0x6e;
    C2V(gASCII_table)[35] = 0x6f;
    C2V(gASCII_table)[36] = 0x70;
    C2V(gASCII_table)[37] = 0x71;
    C2V(gASCII_table)[38] = 0x72;
    C2V(gASCII_table)[39] = 0x73;
    C2V(gASCII_table)[40] = 0x74;
    C2V(gASCII_table)[41] = 0x75;
    C2V(gASCII_table)[42] = 0x76;
    C2V(gASCII_table)[43] = 0x77;
    C2V(gASCII_table)[44] = 0x78;
    C2V(gASCII_table)[45] = 0x79;
    C2V(gASCII_table)[46] = 0x7a;
    C2V(gASCII_table)[48] = 0x2d;
    C2V(gASCII_table)[49] = 0x3d;
    C2V(gASCII_table)[50] = 0x7f;
    C2V(gASCII_table)[51] = 0xd;
    C2V(gASCII_table)[52] = 0xd;
    C2V(gASCII_table)[53] = 0x9;
    C2V(gASCII_table)[58] = 0x2e;
    C2V(gASCII_table)[59] = 0x2c;
    C2V(gASCII_table)[63] = 0x1b;
    C2V(gASCII_table)[106] = 0x20;

    memcpy(C2V(gASCII_shift_table), C2V(gASCII_table), sizeof(C2V(gASCII_table)));

    C2V(gASCII_shift_table)[21] = 0x41;
    C2V(gASCII_shift_table)[22] = 0x42;
    C2V(gASCII_shift_table)[23] = 0x43;
    C2V(gASCII_shift_table)[24] = 0x44;
    C2V(gASCII_shift_table)[25] = 0x45;
    C2V(gASCII_shift_table)[26] = 0x46;
    C2V(gASCII_shift_table)[27] = 0x47;
    C2V(gASCII_shift_table)[28] = 0x48;
    C2V(gASCII_shift_table)[29] = 0x49;
    C2V(gASCII_shift_table)[31] = 0x4b;
    C2V(gASCII_shift_table)[32] = 0x4c;
    C2V(gASCII_shift_table)[33] = 0x4d;
    C2V(gASCII_shift_table)[34] = 0x4e;
    C2V(gASCII_shift_table)[35] = 0x4f;
    C2V(gASCII_shift_table)[36] = 0x50;
    C2V(gASCII_shift_table)[37] = 0x51;
    C2V(gASCII_shift_table)[38] = 0x52;
    C2V(gASCII_shift_table)[39] = 0x53;
    C2V(gASCII_shift_table)[40] = 0x54;
    C2V(gASCII_shift_table)[41] = 0x55;
    C2V(gASCII_shift_table)[42] = 0x56;
    C2V(gASCII_shift_table)[43] = 0x57;
    C2V(gASCII_shift_table)[44] = 0x58;
    C2V(gASCII_shift_table)[45] = 0x59;
    C2V(gASCII_shift_table)[46] = 0x5a;

    C2V(gScan_code)[12] = 0xff;
    C2V(gScan_code)[13] = 0xff;
    C2V(gScan_code)[14] = 0xff;
    C2V(gScan_code)[15] = 0xff;
    C2V(gScan_code)[16] = 0x3a;
    C2V(gScan_code)[17] = 0x36;
    C2V(gScan_code)[18] = 0xb8;
    C2V(gScan_code)[19] = 0x9d;
    C2V(gScan_code)[20] = 0x2a;
    C2V(gScan_code)[21] = 0x38;
    C2V(gScan_code)[22] = 0x1d;
    C2V(gScan_code)[23] = 0xb;
    C2V(gScan_code)[24] = 2;
    C2V(gScan_code)[25] = 3;
    C2V(gScan_code)[26] = 4;
    C2V(gScan_code)[27] = 5;
    C2V(gScan_code)[28] = 6;
    C2V(gScan_code)[29] = 7;
    C2V(gScan_code)[30] = 8;
    C2V(gScan_code)[31] = 9;
    C2V(gScan_code)[32] = 10;
    C2V(gScan_code)[33] = 0x1e;
    C2V(gScan_code)[34] = 0x30;
    C2V(gScan_code)[35] = 0x2e;
    C2V(gScan_code)[36] = 0x20;
    C2V(gScan_code)[37] = 0x12;
    C2V(gScan_code)[38] = 0x21;
    C2V(gScan_code)[39] = 0x22;
    C2V(gScan_code)[40] = 0x23;
    C2V(gScan_code)[41] = 0x17;
    C2V(gScan_code)[42] = 0x24;
    C2V(gScan_code)[43] = 0x25;
    C2V(gScan_code)[44] = 0x26;
    C2V(gScan_code)[45] = 0x32;
    C2V(gScan_code)[46] = 0x31;
    C2V(gScan_code)[47] = 0x18;
    C2V(gScan_code)[48] = 0x19;
    C2V(gScan_code)[49] = 0x10;
    C2V(gScan_code)[50] = 0x13;
    C2V(gScan_code)[51] = 0x1f;
    C2V(gScan_code)[52] = 0x14;
    C2V(gScan_code)[53] = 0x16;
    C2V(gScan_code)[54] = 0x2f;
    C2V(gScan_code)[55] = 0x11;
    C2V(gScan_code)[56] = 0x2d;
    C2V(gScan_code)[57] = 0x15;
    C2V(gScan_code)[58] = 0x2c;
    C2V(gScan_code)[59] = 0x29;
    C2V(gScan_code)[60] = 0xc;
    C2V(gScan_code)[61] = 0xd;
    C2V(gScan_code)[62] = 0xe;
    C2V(gScan_code)[63] = 0x1c;
    C2V(gScan_code)[64] = 0x9c;
    C2V(gScan_code)[65] = 0xf;
    C2V(gScan_code)[66] = 0x35;
    C2V(gScan_code)[67] = 0x2b;
    C2V(gScan_code)[68] = 0x27;
    C2V(gScan_code)[69] = 0x28;
    C2V(gScan_code)[70] = 0x34;
    C2V(gScan_code)[71] = 0x33;
    C2V(gScan_code)[72] = 0x1a;
    C2V(gScan_code)[73] = 0x1b;
    C2V(gScan_code)[74] = 0;
    C2V(gScan_code)[75] = 1;
    C2V(gScan_code)[76] = 0xd2;
    C2V(gScan_code)[77] = 0xd3;
    C2V(gScan_code)[78] = 199;
    C2V(gScan_code)[79] = 0xcf;
    C2V(gScan_code)[80] = 0xc9;
    C2V(gScan_code)[81] = 0xd1;
    C2V(gScan_code)[82] = 0xcb;
    C2V(gScan_code)[83] = 0xcd;
    C2V(gScan_code)[84] = 200;
    C2V(gScan_code)[85] = 0xd0;
    C2V(gScan_code)[86] = 0x45;
    C2V(gScan_code)[87] = 0xb5;
    C2V(gScan_code)[88] = 0x37;
    C2V(gScan_code)[89] = 0x4a;
    C2V(gScan_code)[90] = 0x4e;
    C2V(gScan_code)[91] = 0x53;
    C2V(gScan_code)[92] = 0;
    C2V(gScan_code)[93] = 0x52;
    C2V(gScan_code)[94] = 0x4f;
    C2V(gScan_code)[95] = 0x50;
    C2V(gScan_code)[96] = 0x51;
    C2V(gScan_code)[97] = 0x4b;
    C2V(gScan_code)[98] = 0x4c;
    C2V(gScan_code)[99] = 0x4d;
    C2V(gScan_code)[100] = 0x47;
    C2V(gScan_code)[101] = 0x48;
    C2V(gScan_code)[102] = 0x49;
    C2V(gScan_code)[103] = 0x3b;
    C2V(gScan_code)[104] = 0x3c;
    C2V(gScan_code)[105] = 0x3d;
    C2V(gScan_code)[106] = 0x3e;
    C2V(gScan_code)[107] = 0x3f;
    C2V(gScan_code)[108] = 0x40;
    C2V(gScan_code)[109] = 0x41;
    C2V(gScan_code)[110] = 0x42;
    C2V(gScan_code)[111] = 0x43;
    C2V(gScan_code)[112] = 0x44;
    C2V(gScan_code)[113] = 0x57;
    C2V(gScan_code)[114] = 0x58;
    C2V(gScan_code)[115] = 0xb7;
    C2V(gScan_code)[116] = 0x46;
    C2V(gScan_code)[117] = 0xff;
    C2V(gScan_code)[118] = 0x39;
}
C2_HOOK_FUNCTION(0x0051ba10, KeyBegin)


void C2_HOOK_FASTCALL KeyDInputBegin(void) {
    HRESULT hRes;

    hRes = DirectInputCreateA(C2V(gHInstance), DIRECTINPUT_VERSION, &C2V(gDirectInput), NULL);
    if (hRes != S_OK) {
        dr_dprintf("FATAL ERROR: %s", "Unable to create DirectInput object - please check that DirectX is installed");
        C2V(gFatalErrorMessageValid) = 1;
        sprintf(C2V(gFatalErrorMessage), "%s\n%s", "Unable to create DirectInput object - please check that DirectX is installed", "");
        goto error;
    }

    GUID guid_sysKeyboard = GUID_SysKeyboard;
    hRes = IDirectInput_CreateDevice(C2V(gDirectInput), &guid_sysKeyboard, &C2V(gDirectInputDevice), NULL);
    if (hRes != S_OK) {
        dr_dprintf("FATAL ERROR: %s", "Direct Input: Can't create device");
        C2V(gFatalErrorMessageValid) = 1;
        sprintf(C2V(gFatalErrorMessage), "%s\n%s", "Direct Input: Can't create device", "");
        goto error;
    }

    hRes = IDirectInputDevice_SetDataFormat(C2V(gDirectInputDevice), &c_dfDIKeyboard);
    if (hRes != S_OK) {
        dr_dprintf("FATAL ERROR: %s", "Direct Input: Can't create device");
        C2V(gFatalErrorMessageValid) = 1;
        sprintf(C2V(gFatalErrorMessage), "%s\n%s", "Direct Input: Can't set data format", "");
        goto error;
    }

    hRes = IDirectInputDevice_SetCooperativeLevel(C2V(gDirectInputDevice), C2V(gHWnd), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (hRes != S_OK) {
        dr_dprintf("FATAL ERROR: %s", "Direct Input: Can't set keyboard cooperative level");
        C2V(gFatalErrorMessageValid) = 1;
        sprintf(C2V(gFatalErrorMessage), "%s\n%s", "Direct Input: Can't set keyboard cooperative level", "");
        goto error;
    }

    hRes = IDirectInputDevice_Acquire(C2V(gDirectInputDevice));
    if (!SUCCEEDED(hRes)) {
        dr_dprintf("ERROR: Can't aquire keyboard; HRESULT %x", hRes);
    }
    if (JoystickDInputBegin()) {
        JoystickDInputDetect();
        LoadJoystickPreferences();
    }
    C2V(gJoystick_deadzone) = 8000;
    return;
error:
    C2V(gExitCode) = 700;
    if (C2V(gBack_screen) != NULL) {
        if (C2V(gBack_screen)->pixels != NULL) {
            C2V(gExitCode) = 700;
            PDUnlockRealBackScreen();
        }
    }
    if (C2V(gBr_initialized)) {
        RemoveAllBrenderDevices();
    }
    PDShutdownSystem();
}
C2_HOOK_FUNCTION(0x0051cbf0, KeyDInputBegin)
