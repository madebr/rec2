#include "win32_dinput.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "input.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <windows.h>
#include <dinput.h>

#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_table, 151, 0x006ad1f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_shift_table, 151, 0x006ac588);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gScan_code, 151, 0x006b3470);

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUTA, gDirectInput, 0x006acc78);

C2_HOOK_VARIABLE_IMPLEMENT(int, gJoystick_deadzone, 0x00762280);

C2_HOOK_VARIABLE_IMPLEMENT(IDirectInputA*, gDirectInputJoystickHandle, 0x00686158);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(IDirectInputDevice2A*, gDirectInputJoystickDevices, MAX_COUNT_JOYSTICKS, 0x0079e060);
C2_HOOK_VARIABLE_IMPLEMENT(int, gJoystickFFB, 0x0068615c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCountEnumeratedJoystickDinputDevices, 0x00686160);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(void*, gDirectInputEffects, MAX_COUNT_EFFECTS, 0x0079e0a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDirectInputJoystickEnumerated, 0x00686170);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCurrentDirectInputJoysticksIndex, 0x00595f88, -1);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDirectInputJoystickInfo, gDirectInputJoystickInfos, MAX_COUNT_JOYSTICKS, 0x0079d9c0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(DIOBJECTDATAFORMAT, gJoystickDirectIniputDataFormatObjects, 44, 0x0054d050, {
    { &GUID_XAxis, offsetof(tJoystickInputState, xaxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_YAxis, offsetof(tJoystickInputState, yaxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_ZAxis, offsetof(tJoystickInputState, zaxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RyAxis, offsetof(tJoystickInputState, rxAxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RzAxis, offsetof(tJoystickInputState, ryAxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RzAxis, offsetof(tJoystickInputState, rzAxis), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_Slider, offsetof(tJoystickInputState, sliders) + 0 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_Slider, offsetof(tJoystickInputState, sliders) + 1 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_POV, offsetof(tJoystickInputState, povs) + 0 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, offsetof(tJoystickInputState, povs) + 1 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, offsetof(tJoystickInputState, povs) + 2 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, offsetof(tJoystickInputState, povs) + 3 * sizeof(tU32), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  0 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  1 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  2 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  3 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  4 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  5 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  6 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  7 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  8 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) +  9 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 10 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 11 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 12 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 13 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 14 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 15 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 16 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 17 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 18 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 19 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 20 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 21 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 22 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 23 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 24 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 25 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 26 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 27 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 28 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 29 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 30 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, offsetof(tJoystickInputState, buttons) + 31 * sizeof(tU8), DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
});

C2_HOOK_VARIABLE_IMPLEMENT_INIT(DIDATAFORMAT, gJoystickDirectInputDataFormat, 0x0054d310, {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDOI_FFACTUATOR,
    sizeof(tJoystickInputState),
    REC2_ASIZE(gJoystickDirectIniputDataFormatObjects),
    gJoystickDirectIniputDataFormatObjects,
});

BOOL C2_HOOK_STDCALL Win32DInputJoystickEnum(const DIDEVICEINSTANCEA* pDeviceInstance, IDirectInputA* pDirectInput) {
    HRESULT hResult;
    IDirectInputDeviceA *device;
    IDirectInputDevice2A *device2;
    int nb;

    C2_HOOK_ASSERT(sizeof(tJoystickInputState) == 0x50);

    nb = C2V(gCountEnumeratedJoystickDinputDevices);
    c2_strncpy(C2V(gDirectInputJoystickInfos)[nb].productName, pDeviceInstance->tszProductName, 80);
    C2V(gDirectInputJoystickInfos)[nb].productName[79] = '\0';
    switch (pDeviceInstance->dwDevType) {
        case DIDEVTYPEJOYSTICK_GAMEPAD:
            C2V(gDirectInputJoystickInfos)[nb].count_buttons = 4;
            break;
        case DIDEVTYPEJOYSTICK_WHEEL:
            C2V(gDirectInputJoystickInfos)[nb].count_buttons = 2;
            break;
        default:
            C2V(gDirectInputJoystickInfos)[nb].count_buttons = 1;
            break;
    }
    hResult = IDirectInput_CreateDevice(pDirectInput, &pDeviceInstance->guidInstance, &device, NULL);
    if (hResult != DI_OK) {
        dr_dprintf("Could not create dinput object\n");
        return DIENUM_CONTINUE;
    }
    hResult = IDirectInputDevice_SetDataFormat(device, &C2V(gJoystickDirectInputDataFormat));
    if (hResult != DI_OK) {
        dr_dprintf("Could not set dinput device data format\n");
        IDirectInputDevice2_Release(device);
        return DIENUM_CONTINUE;
    }
    hResult = IDirectInputDevice_QueryInterface(device, &IID_IDirectInputDevice2A,
                                                (void **)&C2V(gDirectInputJoystickDevices)[nb]);
    IDirectInputDevice2_Release(device);
    if (hResult != DI_OK) {
        dr_dprintf("QueryInterface did not return DI_OK\n");
        return DIENUM_CONTINUE;
    }
    device2 = C2V(gDirectInputJoystickDevices)[nb];
    if (device2 == NULL) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
    hResult = IDirectInputDevice2_SetCooperativeLevel(device2, C2V(gHWnd), DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    if (hResult != DI_OK) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
    DIPROPRANGE diphRange;
    diphRange.diph.dwSize = sizeof(diphRange);
    diphRange.diph.dwHeaderSize = sizeof(diphRange.diph);
    diphRange.diph.dwObj = offsetof(tJoystickInputState, xaxis);
    diphRange.diph.dwHow = DIPH_BYOFFSET;
    diphRange.lMin = 0;
    diphRange.lMax = 0xffff;
    hResult = IDirectInputDevice2_SetProperty(device2, DIPROP_RANGE, (DIPROPHEADER*)&diphRange);
    if (hResult != DI_OK) {
        dr_dprintf("IDirectInputDevice::SetProperty(DIPH_RANGE)  FAILED\n");
        IDirectInputDevice2_Release(device2);
        return DIENUM_CONTINUE;
    }
    DIPROPDWORD zoneProp;
    zoneProp.diph.dwSize = sizeof(zoneProp);
    zoneProp.diph.dwHeaderSize = sizeof(zoneProp.diph);
    diphRange.diph.dwObj = offsetof(tJoystickInputState, xaxis);
    zoneProp.diph.dwHow = DIPH_BYOFFSET;
    zoneProp.dwData = 1000;
    hResult = IDirectInputDevice2_SetProperty(device2, DIPROP_DEADZONE, (DIPROPHEADER*)&diphRange);
    if (hResult != DI_OK) {
        dr_dprintf("IDirectInputDevice:: SetProperty(DIPH_DEADZONE) FAILED\n");
        IDirectInputDevice2_Release(device2);
        return DIENUM_CONTINUE;
    }
    C2V(gCountEnumeratedJoystickDinputDevices)++;
    return DIENUM_CONTINUE;
}
C2_HOOK_FUNCTION(0x00459c70, Win32DInputJoystickEnum)

void C2_HOOK_FASTCALL AttachJoystickButtonInfos(size_t pSize, tWin32_void_voidptr_cbfn pCallback) {
    int original_index;
    int i;

    original_index = C2V(gCurrentDirectInputJoysticksIndex);
    for (i = 0; i < REC2_ASIZE(C2V(gDirectInputJoystickDevices)); i++) {
        IDirectInputDevice2A *device;

        device = C2V(gDirectInputJoystickDevices)[i];
        if (device != NULL && C2V(gDirectInputJoystickInfos)[i].data == NULL) {
            C2V(gCurrentDirectInputJoysticksIndex) = i;
            if (pSize != 0) {
                C2V(gDirectInputJoystickInfos)[i].data = c2_malloc(pSize);
                if (C2V(gDirectInputJoystickInfos)[i].data != NULL) {
                    C2V(gDirectInputJoystickInfos)[i].sizeData = pSize;
                }
            }
            if (C2V(gDirectInputJoystickInfos)[i].data != NULL) {
                pCallback(C2V(gDirectInputJoystickInfos)[i].data);
            }
        }
    }
    C2V(gCurrentDirectInputJoysticksIndex) = original_index;
}
C2_HOOK_FUNCTION(0x00459fe0, AttachJoystickButtonInfos)

const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void) {
    return C2V(gDirectInputJoystickInfos)[C2V(gCurrentDirectInputJoysticksIndex)].productName;
}
C2_HOOK_FUNCTION(0x004599d0, GetCurrentJoystickName)

size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void) {
    return C2V(gDirectInputJoystickInfos)[C2V(gCurrentDirectInputJoysticksIndex)].count_buttons;
}
C2_HOOK_FUNCTION(0x00459fb0, GetCurrentJoystickCountButtons)

void C2_HOOK_FASTCALL CollectJoystickButtonInfo(tButtonJoystickInfo* pInfo) {
    unsigned int i;

    if (pInfo == NULL) {
        return;
    }
    strcpy(pInfo->productName, GetCurrentJoystickName());
    pInfo->count_buttons = GetCurrentJoystickCountButtons();
    for (i = 0; i < REC2_ASIZE(((tButtonJoystickInfo*)NULL)->buttons); i++) {
        if (i < pInfo->count_buttons) {
            pInfo->buttons[i] = i;
        } else {
            pInfo->buttons[i] = -1;
        }
        pInfo->field3_0xd4 = 1;
        pInfo->field4_0xd8 = 1.0f;
        pInfo->field5_0xdc = 1.0f;
        pInfo->field6_0xe0 = 0; /* or 0.f */
        pInfo->field7_0xe4 = 50;
        pInfo->field8_0xe8 = 0; /* or 0.f */
    }
}
C2_HOOK_FUNCTION(0x0045c370, CollectJoystickButtonInfo)

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
        PDFatalError("Unable to create DirectInput object - please check that DirectX is installed");
    }

    GUID guid_sysKeyboard = GUID_SysKeyboard;
    hRes = IDirectInput_CreateDevice(C2V(gDirectInput), &guid_sysKeyboard, &C2V(gDirectInputDevice), NULL);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetDataFormat(C2V(gDirectInputDevice), &c_dfDIKeyboard);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetCooperativeLevel(C2V(gDirectInputDevice), C2V(gHWnd), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't set keyboard cooperative level");
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
}
C2_HOOK_FUNCTION(0x0051cbf0, Win32InitInputDevice)
