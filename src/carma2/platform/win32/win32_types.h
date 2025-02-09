#ifndef C2_WIN32_TYPES_H
#define C2_WIN32_TYPES_H

#include "rec2_types.h"

#include <dinput.h>

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

typedef struct tHeap_block_information {
    undefined field_0x0[0x48];
    struct tHeap_block_information* next;
    undefined field_0x4c[0x26];
    tU16 field_0x72;
} tHeap_block_information;

typedef union {
    tU32 flags;
    tU8 data[1];
} tHeap_memory;

typedef enum {
    eJoystick_effect_ConstantForce = 0,
    eJoystick_effect_RampForce = 1,
    eJoystick_effect_CustomForce = 2,
    eJoystick_effect_Square = 3,
    eJoystick_effect_Sine = 4,
    eJoystick_effect_Triangle = 5,
    eJoystick_effect_SawtoothUp = 6,
    eJoystick_effect_SawtoothDown = 7,
    eJoystick_effect_Spring_8 = 8,
    eJoystick_effect_Spring_9 = 9,
    eJoystick_effect_Damper = 10,
    eJoystick_effect_Inertia = 11,
    eJoystick_effect_Friction = 12,
} tJoystick_description_type;

typedef union {
    struct {
        undefined4 field_0x0;
    } constant;
    struct {
        undefined4 field_0x0;
        undefined4 field_0x4;
        undefined4 field_0x8;
    } friction;
    undefined field_0x0[24];
} tJoystick_effect_type_params;

typedef struct {
    tJoystick_description_type type;
    tJoystick_effect_type_params type_specific_params;
    undefined field_0x1c[24];
    undefined4 time_ms;
    undefined4 field_0x38;
    undefined4 field_0x3c;
    undefined4 field_0x40;
    int field_0x44;
    undefined4 field_0x48;
    undefined4 field_0x4c;
} tJoystick_effect_description;

typedef struct {
    DIEFFECT di_effect;
    undefined type_specific_params[44];
    undefined field_0x4c[20];
    LONG field_0x60;
    undefined4 field_0x64;
    char name[128];
} tJoystick_force_effect;

#endif // C2_WIN32_TYPES_H
