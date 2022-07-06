#include "win32_dinput.h"

#include "win32.h"

#include "errors.h"
#include "init.h"
#include "input.h"
#include "utility.h"

#include <windows.h>
#include <dinput.h>

#include <stdio.h>
#include <string.h>

// FIXME: better name + use directinput macros for initial variables
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, key_lut_1, 151, 0x006ad1f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, key_lut_2, 151, 0x006ac588);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, key_lut_3, 151, 0x006b3470);

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUT, gDirectInput, 0x006acc78);

int (C2_HOOK_FASTCALL * JoystickDInputBegin_original)(void);
int C2_HOOK_FASTCALL JoystickDInputBegin() {
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
void C2_HOOK_FASTCALL JoystickDInputDetect() {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    JoystickDInputDetect_original();
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c410, JoystickDInputDetect, JoystickDInputDetect_original)

void C2_HOOK_FASTCALL KeyScanCodeBegin(void) {
    memset(C2V(key_lut_1), 0, sizeof(C2V(key_lut_1)));
    memset(C2V(key_lut_2), 0, sizeof(C2V(key_lut_2)));

    C2V(key_lut_1)[11] = 0x30;
    C2V(key_lut_1)[12] = 0x31;
    C2V(key_lut_1)[13] = 0x32;
    C2V(key_lut_1)[14] = 0x33;
    C2V(key_lut_1)[15] = 0x34;
    C2V(key_lut_1)[16] = 0x35;
    C2V(key_lut_1)[17] = 0x36;
    C2V(key_lut_1)[18] = 0x37;
    C2V(key_lut_1)[19] = 0x38;
    C2V(key_lut_1)[20] = 0x39;
    C2V(key_lut_1)[21] = 0x61;
    C2V(key_lut_1)[22] = 0x62;
    C2V(key_lut_1)[23] = 0x63;
    C2V(key_lut_1)[24] = 0x64;
    C2V(key_lut_1)[25] = 0x65;
    C2V(key_lut_1)[26] = 0x66;
    C2V(key_lut_1)[27] = 0x67;
    C2V(key_lut_1)[28] = 0x68;
    C2V(key_lut_1)[29] = 0x69;
    C2V(key_lut_1)[30] = 0x6a;
    C2V(key_lut_1)[31] = 0x6b;
    C2V(key_lut_1)[32] = 0x6c;
    C2V(key_lut_1)[33] = 0x6d;
    C2V(key_lut_1)[34] = 0x6e;
    C2V(key_lut_1)[35] = 0x6f;
    C2V(key_lut_1)[36] = 0x70;
    C2V(key_lut_1)[37] = 0x71;
    C2V(key_lut_1)[38] = 0x72;
    C2V(key_lut_1)[39] = 0x73;
    C2V(key_lut_1)[40] = 0x74;
    C2V(key_lut_1)[41] = 0x75;
    C2V(key_lut_1)[42] = 0x76;
    C2V(key_lut_1)[43] = 0x77;
    C2V(key_lut_1)[44] = 0x78;
    C2V(key_lut_1)[45] = 0x79;
    C2V(key_lut_1)[46] = 0x7a;
    C2V(key_lut_1)[48] = 0x2d;
    C2V(key_lut_1)[49] = 0x3d;
    C2V(key_lut_1)[50] = 0x7f;
    C2V(key_lut_1)[51] = 0xd;
    C2V(key_lut_1)[52] = 0xd;
    C2V(key_lut_1)[53] = 0x9;
    C2V(key_lut_1)[58] = 0x2e;
    C2V(key_lut_1)[59] = 0x2c;
    C2V(key_lut_1)[63] = 0x1b;
    C2V(key_lut_1)[106] = 0x20;

    memcpy(C2V(key_lut_2), C2V(key_lut_1), sizeof(C2V(key_lut_1)));

    C2V(key_lut_2)[21] = 0x41;
    C2V(key_lut_2)[22] = 0x42;
    C2V(key_lut_2)[23] = 0x43;
    C2V(key_lut_2)[24] = 0x44;
    C2V(key_lut_2)[25] = 0x45;
    C2V(key_lut_2)[26] = 0x46;
    C2V(key_lut_2)[27] = 0x47;
    C2V(key_lut_2)[28] = 0x48;
    C2V(key_lut_2)[29] = 0x49;
    C2V(key_lut_2)[31] = 0x4b;
    C2V(key_lut_2)[32] = 0x4c;
    C2V(key_lut_2)[33] = 0x4d;
    C2V(key_lut_2)[34] = 0x4e;
    C2V(key_lut_2)[35] = 0x4f;
    C2V(key_lut_2)[36] = 0x50;
    C2V(key_lut_2)[37] = 0x51;
    C2V(key_lut_2)[38] = 0x52;
    C2V(key_lut_2)[39] = 0x53;
    C2V(key_lut_2)[40] = 0x54;
    C2V(key_lut_2)[41] = 0x55;
    C2V(key_lut_2)[42] = 0x56;
    C2V(key_lut_2)[43] = 0x57;
    C2V(key_lut_2)[44] = 0x58;
    C2V(key_lut_2)[45] = 0x59;
    C2V(key_lut_2)[46] = 0x5a;

    C2V(key_lut_3)[12] = 0xff;
    C2V(key_lut_3)[13] = 0xff;
    C2V(key_lut_3)[14] = 0xff;
    C2V(key_lut_3)[15] = 0xff;
    C2V(key_lut_3)[16] = 0x3a;
    C2V(key_lut_3)[17] = 0x36;
    C2V(key_lut_3)[18] = 0xb8;
    C2V(key_lut_3)[19] = 0x9d;
    C2V(key_lut_3)[20] = 0x2a;
    C2V(key_lut_3)[21] = 0x38;
    C2V(key_lut_3)[22] = 0x1d;
    C2V(key_lut_3)[23] = 0xb;
    C2V(key_lut_3)[24] = 2;
    C2V(key_lut_3)[25] = 3;
    C2V(key_lut_3)[26] = 4;
    C2V(key_lut_3)[27] = 5;
    C2V(key_lut_3)[28] = 6;
    C2V(key_lut_3)[29] = 7;
    C2V(key_lut_3)[30] = 8;
    C2V(key_lut_3)[31] = 9;
    C2V(key_lut_3)[32] = 10;
    C2V(key_lut_3)[33] = 0x1e;
    C2V(key_lut_3)[34] = 0x30;
    C2V(key_lut_3)[35] = 0x2e;
    C2V(key_lut_3)[36] = 0x20;
    C2V(key_lut_3)[37] = 0x12;
    C2V(key_lut_3)[38] = 0x21;
    C2V(key_lut_3)[39] = 0x22;
    C2V(key_lut_3)[40] = 0x23;
    C2V(key_lut_3)[41] = 0x17;
    C2V(key_lut_3)[42] = 0x24;
    C2V(key_lut_3)[43] = 0x25;
    C2V(key_lut_3)[44] = 0x26;
    C2V(key_lut_3)[45] = 0x32;
    C2V(key_lut_3)[46] = 0x31;
    C2V(key_lut_3)[47] = 0x18;
    C2V(key_lut_3)[48] = 0x19;
    C2V(key_lut_3)[49] = 0x10;
    C2V(key_lut_3)[50] = 0x13;
    C2V(key_lut_3)[51] = 0x1f;
    C2V(key_lut_3)[52] = 0x14;
    C2V(key_lut_3)[53] = 0x16;
    C2V(key_lut_3)[54] = 0x2f;
    C2V(key_lut_3)[55] = 0x11;
    C2V(key_lut_3)[56] = 0x2d;
    C2V(key_lut_3)[57] = 0x15;
    C2V(key_lut_3)[58] = 0x2c;
    C2V(key_lut_3)[59] = 0x29;
    C2V(key_lut_3)[60] = 0xc;
    C2V(key_lut_3)[61] = 0xd;
    C2V(key_lut_3)[62] = 0xe;
    C2V(key_lut_3)[63] = 0x1c;
    C2V(key_lut_3)[64] = 0x9c;
    C2V(key_lut_3)[65] = 0xf;
    C2V(key_lut_3)[66] = 0x35;
    C2V(key_lut_3)[67] = 0x2b;
    C2V(key_lut_3)[68] = 0x27;
    C2V(key_lut_3)[69] = 0x28;
    C2V(key_lut_3)[70] = 0x34;
    C2V(key_lut_3)[71] = 0x33;
    C2V(key_lut_3)[72] = 0x1a;
    C2V(key_lut_3)[73] = 0x1b;
    C2V(key_lut_3)[74] = 0;
    C2V(key_lut_3)[75] = 1;
    C2V(key_lut_3)[76] = 0xd2;
    C2V(key_lut_3)[77] = 0xd3;
    C2V(key_lut_3)[78] = 199;
    C2V(key_lut_3)[79] = 0xcf;
    C2V(key_lut_3)[80] = 0xc9;
    C2V(key_lut_3)[81] = 0xd1;
    C2V(key_lut_3)[82] = 0xcb;
    C2V(key_lut_3)[83] = 0xcd;
    C2V(key_lut_3)[84] = 200;
    C2V(key_lut_3)[85] = 0xd0;
    C2V(key_lut_3)[86] = 0x45;
    C2V(key_lut_3)[87] = 0xb5;
    C2V(key_lut_3)[88] = 0x37;
    C2V(key_lut_3)[89] = 0x4a;
    C2V(key_lut_3)[90] = 0x4e;
    C2V(key_lut_3)[91] = 0x53;
    C2V(key_lut_3)[92] = 0;
    C2V(key_lut_3)[93] = 0x52;
    C2V(key_lut_3)[94] = 0x4f;
    C2V(key_lut_3)[95] = 0x50;
    C2V(key_lut_3)[96] = 0x51;
    C2V(key_lut_3)[97] = 0x4b;
    C2V(key_lut_3)[98] = 0x4c;
    C2V(key_lut_3)[99] = 0x4d;
    C2V(key_lut_3)[100] = 0x47;
    C2V(key_lut_3)[101] = 0x48;
    C2V(key_lut_3)[102] = 0x49;
    C2V(key_lut_3)[103] = 0x3b;
    C2V(key_lut_3)[104] = 0x3c;
    C2V(key_lut_3)[105] = 0x3d;
    C2V(key_lut_3)[106] = 0x3e;
    C2V(key_lut_3)[107] = 0x3f;
    C2V(key_lut_3)[108] = 0x40;
    C2V(key_lut_3)[109] = 0x41;
    C2V(key_lut_3)[110] = 0x42;
    C2V(key_lut_3)[111] = 0x43;
    C2V(key_lut_3)[112] = 0x44;
    C2V(key_lut_3)[113] = 0x57;
    C2V(key_lut_3)[114] = 0x58;
    C2V(key_lut_3)[115] = 0xb7;
    C2V(key_lut_3)[116] = 0x46;
    C2V(key_lut_3)[117] = 0xff;
    C2V(key_lut_3)[118] = 0x39;
}
C2_HOOK_FUNCTION(0x0051ba10, KeyScanCodeBegin)


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
    return;
error:
    C2V(gExitCode) = 700;
    if (C2V(gReal_back_screen) != NULL) {
        if (C2V(gReal_back_screen)->pixels != NULL) {
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
