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

    C2V(gASCII_table)[11] = '0';
    C2V(gASCII_table)[12] = '1';
    C2V(gASCII_table)[13] = '2';
    C2V(gASCII_table)[14] = '3';
    C2V(gASCII_table)[15] = '4';
    C2V(gASCII_table)[16] = '5';
    C2V(gASCII_table)[17] = '6';
    C2V(gASCII_table)[18] = '7';
    C2V(gASCII_table)[19] = '8';
    C2V(gASCII_table)[20] = '9';
    C2V(gASCII_table)[21] = 'a';
    C2V(gASCII_table)[22] = 'b';
    C2V(gASCII_table)[23] = 'c';
    C2V(gASCII_table)[24] = 'd';
    C2V(gASCII_table)[25] = 'e';
    C2V(gASCII_table)[26] = 'f';
    C2V(gASCII_table)[27] = 'g';
    C2V(gASCII_table)[28] = 'h';
    C2V(gASCII_table)[29] = 'i';
    C2V(gASCII_table)[30] = 'j';
    C2V(gASCII_table)[31] = 'k';
    C2V(gASCII_table)[32] = 'l';
    C2V(gASCII_table)[33] = 'm';
    C2V(gASCII_table)[34] = 'n';
    C2V(gASCII_table)[35] = 'o';
    C2V(gASCII_table)[36] = 'p';
    C2V(gASCII_table)[37] = 'q';
    C2V(gASCII_table)[38] = 'r';
    C2V(gASCII_table)[39] = 's';
    C2V(gASCII_table)[40] = 't';
    C2V(gASCII_table)[41] = 'u';
    C2V(gASCII_table)[42] = 'v';
    C2V(gASCII_table)[43] = 'w';
    C2V(gASCII_table)[44] = 'x';
    C2V(gASCII_table)[45] = 'y';
    C2V(gASCII_table)[46] = 'z';
    C2V(gASCII_table)[48] = '-';
    C2V(gASCII_table)[49] = '=';
    C2V(gASCII_table)[50] = 0x7f;
    C2V(gASCII_table)[51] = '\r';
    C2V(gASCII_table)[52] = '\r';
    C2V(gASCII_table)[53] = '\t';
    C2V(gASCII_table)[58] = '.';
    C2V(gASCII_table)[59] = ',';
    C2V(gASCII_table)[63] = 0x1b;
    C2V(gASCII_table)[106] = ' ';

    memcpy(C2V(gASCII_shift_table), C2V(gASCII_table), sizeof(C2V(gASCII_table)));

    C2V(gASCII_shift_table)[21] = 'A';
    C2V(gASCII_shift_table)[22] = 'B';
    C2V(gASCII_shift_table)[23] = 'C';
    C2V(gASCII_shift_table)[24] = 'D';
    C2V(gASCII_shift_table)[25] = 'E';
    C2V(gASCII_shift_table)[26] = 'F';
    C2V(gASCII_shift_table)[27] = 'G';
    C2V(gASCII_shift_table)[28] = 'H';
    C2V(gASCII_shift_table)[29] = 'I';
#if defined(REC2_FIX_BUGS)
    C2V(gASCII_shift_table)[30] = 'J';
#endif
    C2V(gASCII_shift_table)[31] = 'K';
    C2V(gASCII_shift_table)[32] = 'L';
    C2V(gASCII_shift_table)[33] = 'M';
    C2V(gASCII_shift_table)[34] = 'N';
    C2V(gASCII_shift_table)[35] = 'O';
    C2V(gASCII_shift_table)[36] = 'P';
    C2V(gASCII_shift_table)[37] = 'Q';
    C2V(gASCII_shift_table)[38] = 'R';
    C2V(gASCII_shift_table)[39] = 'S';
    C2V(gASCII_shift_table)[40] = 'T';
    C2V(gASCII_shift_table)[41] = 'U';
    C2V(gASCII_shift_table)[42] = 'V';
    C2V(gASCII_shift_table)[43] = 'W';
    C2V(gASCII_shift_table)[44] = 'X';
    C2V(gASCII_shift_table)[45] = 'Y';
    C2V(gASCII_shift_table)[46] = 'Z';

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
    C2V(gScan_code)[24] = DIK_1;
    C2V(gScan_code)[25] = DIK_2;
    C2V(gScan_code)[26] = DIK_3;
    C2V(gScan_code)[27] = DIK_4;
    C2V(gScan_code)[28] = DIK_5;
    C2V(gScan_code)[29] = DIK_6;
    C2V(gScan_code)[30] = DIK_7;
    C2V(gScan_code)[31] = DIK_8;
    C2V(gScan_code)[32] = DIK_9;
    C2V(gScan_code)[33] = DIK_A;
    C2V(gScan_code)[34] = DIK_B;
    C2V(gScan_code)[35] = DIK_C;
    C2V(gScan_code)[36] = DIK_D;
    C2V(gScan_code)[37] = DIK_E;
    C2V(gScan_code)[38] = DIK_F;
    C2V(gScan_code)[39] = DIK_G;
    C2V(gScan_code)[40] = DIK_H;
    C2V(gScan_code)[41] = DIK_I;
    C2V(gScan_code)[42] = DIK_J;
    C2V(gScan_code)[43] = DIK_K;
    C2V(gScan_code)[44] = DIK_L;
    C2V(gScan_code)[45] = DIK_M;
    C2V(gScan_code)[46] = DIK_N;
    C2V(gScan_code)[47] = DIK_O;
    C2V(gScan_code)[48] = DIK_P;
    C2V(gScan_code)[49] = DIK_Q;
    C2V(gScan_code)[50] = DIK_R;
    C2V(gScan_code)[51] = DIK_S;
    C2V(gScan_code)[52] = DIK_T;
    C2V(gScan_code)[53] = DIK_U;
    C2V(gScan_code)[54] = DIK_V;
    C2V(gScan_code)[55] = DIK_W;
    C2V(gScan_code)[56] = DIK_X;
    C2V(gScan_code)[57] = DIK_Y;
    C2V(gScan_code)[58] = DIK_Z;
    C2V(gScan_code)[59] = DIK_GRAVE;
    C2V(gScan_code)[60] = DIK_MINUS;
    C2V(gScan_code)[61] = DIK_EQUALS;
    C2V(gScan_code)[62] = DIK_BACK;
    C2V(gScan_code)[63] = DIK_RETURN;
    C2V(gScan_code)[64] = DIK_NUMPADENTER;
    C2V(gScan_code)[65] = DIK_TAB;
    C2V(gScan_code)[66] = DIK_SLASH;
    C2V(gScan_code)[67] = DIK_BACKSLASH;
    C2V(gScan_code)[68] = DIK_SEMICOLON;
    C2V(gScan_code)[69] = DIK_APOSTROPHE;
    C2V(gScan_code)[70] = DIK_PERIOD;
    C2V(gScan_code)[71] = DIK_COMMA;
    C2V(gScan_code)[72] = DIK_LBRACKET;
    C2V(gScan_code)[73] = DIK_RBRACKET;
    C2V(gScan_code)[74] = 0;
    C2V(gScan_code)[75] = DIK_ESCAPE;
    C2V(gScan_code)[76] = DIK_INSERT;
    C2V(gScan_code)[77] = DIK_DELETE;
    C2V(gScan_code)[78] = DIK_HOME;
    C2V(gScan_code)[79] = DIK_END;
    C2V(gScan_code)[80] = DIK_PRIOR;
    C2V(gScan_code)[81] = DIK_NEXT;
    C2V(gScan_code)[82] = DIK_LEFT;
    C2V(gScan_code)[83] = DIK_RIGHT;
    C2V(gScan_code)[84] = DIK_UP;
    C2V(gScan_code)[85] = DIK_DOWN;
    C2V(gScan_code)[86] = DIK_NUMLOCK;
    C2V(gScan_code)[87] = DIK_DIVIDE;
    C2V(gScan_code)[88] = DIK_MULTIPLY;
    C2V(gScan_code)[89] = DIK_SUBTRACT;
    C2V(gScan_code)[90] = DIK_ADD;
    C2V(gScan_code)[91] = DIK_DECIMAL;
    C2V(gScan_code)[92] = 0;
    C2V(gScan_code)[93] = DIK_NUMPAD0;
    C2V(gScan_code)[94] = DIK_NUMPAD1;
    C2V(gScan_code)[95] = DIK_NUMPAD2;
    C2V(gScan_code)[96] = DIK_NUMPAD3;
    C2V(gScan_code)[97] = DIK_NUMPAD4;
    C2V(gScan_code)[98] = DIK_NUMPAD5;
    C2V(gScan_code)[99] = DIK_NUMPAD6;
    C2V(gScan_code)[100] = DIK_NUMPAD7;
    C2V(gScan_code)[101] = DIK_NUMPAD8;
    C2V(gScan_code)[102] = DIK_NUMPAD9;
    C2V(gScan_code)[103] = DIK_F1;
    C2V(gScan_code)[104] = DIK_F2;
    C2V(gScan_code)[105] = DIK_F3;
    C2V(gScan_code)[106] = DIK_F4;
    C2V(gScan_code)[107] = DIK_F5;
    C2V(gScan_code)[108] = DIK_F6;
    C2V(gScan_code)[109] = DIK_F7;
    C2V(gScan_code)[110] = DIK_F8;
    C2V(gScan_code)[111] = DIK_F9;
    C2V(gScan_code)[112] = DIK_F10;
    C2V(gScan_code)[113] = DIK_F11;
    C2V(gScan_code)[114] = DIK_F12;
    C2V(gScan_code)[115] = DIK_SYSRQ;
    C2V(gScan_code)[116] = DIK_SCROLL;
    C2V(gScan_code)[117] = 0xff;
    C2V(gScan_code)[118] = DIK_SPACE;
}
C2_HOOK_FUNCTION(0x0051ba10, KeyBegin)


void C2_HOOK_FASTCALL Win32InitInputDevice(void) {
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
C2_HOOK_FUNCTION(0x0051cbf0, Win32InitInputDevice)
