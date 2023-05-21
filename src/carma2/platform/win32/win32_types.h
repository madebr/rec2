#ifndef C2_WIN32_TYPES_H
#define C2_WIN32_TYPES_H

#include "rec2_types.h"

typedef void(C2_HOOK_FASTCALL * tWin32_void_voidptr_cbfn)(void*);

typedef struct {
    tU32 xaxis;
    tU32 yaxis;
    tU32 zaxis;
    tU32 rxAxis;
    tU32 ryAxis;
    tU32 rzAxis;
    tU32 sliders[2];
    tU32 povs[4];
    tU8 buttons[32];
} tJoystickInputState;

typedef struct {
    char productName[80];
    size_t count_buttons;
    int buttons[32]; /* index if available, -1 if not */
    int field3_0xd4;
    float field4_0xd8;
    float field5_0xdc;
    int field6_0xe0; /* float or int. init=0 */
    int field7_0xe4; /* init = 50*/
    int field8_0xe8; /* float or int. init=0 */
} tButtonJoystickInfo;

typedef struct {
    tU32 buttonMask;
    tU32 axisMask;
    int devSubType;
    char productName[80];
    tU32 count_buttons;
    size_t sizeData;
    tButtonJoystickInfo* data;
} tDirectInputJoystickInfo;

#endif // C2_WIN32_TYPES_H
