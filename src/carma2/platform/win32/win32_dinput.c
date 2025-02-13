#include "win32_dinput.h"

#include "win32.h"

#include "errors.h"
#include "globvars.h"
#include "init.h"
#include "input.h"
#include "joystick.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <windows.h>
#include <dinput.h>
#include <iforce2.h>

#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_table, 151, 0x006ad1f8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gASCII_shift_table, 151, 0x006ac588);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gScan_code, 256, 0x006b3470);

C2_HOOK_VARIABLE_IMPLEMENT(LPDIRECTINPUTA, gDirectInput, 0x006acc78);

C2_HOOK_VARIABLE_IMPLEMENT(int, gJoystick_deadzone, 0x00762280);

C2_HOOK_VARIABLE_IMPLEMENT(IDirectInputA*, gDirectInputJoystickHandle, 0x00686158);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(IDirectInputDevice2A*, gDirectInputJoystickDevices, MAX_COUNT_JOYSTICKS, 0x0079e060);
C2_HOOK_VARIABLE_IMPLEMENT(int, gJoystickFFB, 0x0068615c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCountEnumeratedJoystickDinputDevices, 0x00686160);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(IDirectInputEffect*, gDirectInputEffects, MAX_COUNT_EFFECTS, 0x0079e0a0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDirectInputJoystickEnumerated, 0x00686170);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDirectInputJoystickInfo, gDirectInputJoystickInfos, MAX_COUNT_JOYSTICKS, 0x0079d9c0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tJoystick_force_effect, gJoystick_effects, 30, 0x00684628);
C2_HOOK_VARIABLE_IMPLEMENT(int, gForceFeedbackAvailable, 0x00686168);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBasic_friction_joystick_effect_index, 0x00595f74, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gBasic_force_joystick_effect_index, 0x00595f78, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCollision_force_joystick_effect_index, 0x00595f80, -1);
C2_HOOK_VARIABLE_IMPLEMENT(IFR_Project *, gIfr_project, 0x0079e040);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_joystick_effects, 0x00686164);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const GUID*, gJoystick_effect_guids, 13, 0x00589f38, {
    &GUID_ConstantForce,
    &GUID_RampForce,
    &GUID_CustomForce,
    &GUID_Square,
    &GUID_Sine,
    &GUID_Triangle,
    &GUID_SawtoothUp,
    &GUID_SawtoothDown,
    &GUID_Spring,
    &GUID_Spring,
    &GUID_Damper,
    &GUID_Inertia,
    &GUID_Friction,
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gJoy1_valid, 0x006ad4b8);
C2_HOOK_VARIABLE_IMPLEMENT(JOYINFOEX, gJoy1_info, 0x006ac7e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gJoy2_valid, 0x006ad4bc);
C2_HOOK_VARIABLE_IMPLEMENT(JOYINFOEX, gJoy2_info, 0x006ac820);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gINT_00595f98, 0x00595f98, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gINT_00595f9c, 0x00595f9c, 1);

static int InitDirectInput(void) {
    int i;

    if (C2V(gDirectInputJoystickHandle) == NULL) {
        memset(C2V(gDirectInputJoystickDevices), 0, sizeof(IDirectInputDeviceA *));
        for (i = 0; i < REC2_ASIZE(C2V(gDirectInputJoystickInfos)); i++) {
            C2V(gDirectInputJoystickInfos)[i].buttonMask = 0;
            C2V(gDirectInputJoystickInfos)[i].axisMask = 0;
            C2V(gDirectInputJoystickInfos)[i].devSubType = 0;
            C2V(gDirectInputJoystickInfos)[i].productName[0] = '\0';
            C2V(gDirectInputJoystickInfos)[i].sizeData = 0;
            C2V(gDirectInputJoystickInfos)[i].data = NULL;
        }
        memset(C2V(gDirectInputEffects), 0, sizeof(C2V(gDirectInputEffects)));
        C2V(gJoystickFFB) = 0;
        C2V(gCountEnumeratedJoystickDinputDevices) = 0;
        DirectInputCreateA(GetModuleHandleA(NULL), 0x0500, &C2V(gDirectInputJoystickHandle), NULL);
        if (C2V(gDirectInputJoystickHandle) == NULL) {
            return 1;
        }
        C2V(gDirectInputJoystickEnumerated) = 0;
    }
    return 0;
}

void C2_HOOK_FASTCALL CloseDirectInput(void) {

    if (C2V(gDirectInputDevice) != NULL) {
        IDirectInputDevice_Unacquire(C2V(gDirectInputDevice));
        IDirectInputDevice_Release(C2V(gDirectInputDevice));
        C2V(gDirectInputDevice) = NULL;
    }
    if (C2V(gDirectInput) != NULL) {
        IDirectInput_Release(C2V(gDirectInput));
        C2V(gDirectInput) = NULL;
    }
}

static int AcquireDInputJoystickDevice(int pIndex) {
    IDirectInputDevice2A *device;

    device = C2V(gDirectInputJoystickDevices)[pIndex];
    if (C2V(gDirectInputJoystickHandle) == NULL || device == NULL) {
        return -1;
    }
    switch (IDirectInputDevice2_Acquire(device)) {
    case DI_OK:
        dr_dprintf("Joystick #%d acquired\n", pIndex);
        return 1;
    case DI_NOEFFECT:
        dr_dprintf("Joystick #%d already acquired\n", pIndex);
        return 1;
    case DIERR_INVALIDPARAM:
        dr_dprintf("Joystick %d DIERR_INVALIDPARAM\n", pIndex);
        return 0;
    case DIERR_OTHERAPPHASPRIO:
        dr_dprintf("Joystick %d DIERR_OTHERAPPHASPRIO\n", pIndex);
        return 0;
    default:
        dr_dprintf("Unknown Error acquiring joystick %d\n", pIndex);
        return 0;
    }
}

tDirectInputJoystickInfo* C2_HOOK_FASTCALL JoystickDInputGetInfo(int pJoystickIndex, tU32* pButtonMask, tU32* pAxisMask) {
    IDirectInputDevice2A* device;
    tDirectInputJoystickInfo* info;

    if (pButtonMask != NULL) {
        *pButtonMask = 0;
    }
    if (pAxisMask != NULL) {
        *pAxisMask = 0;
    }
    if (InitDirectInput()) {
        return NULL;
    }
    device = C2V(gDirectInputJoystickDevices)[pJoystickIndex];
    if (device == NULL) {
        return NULL;
    }
    AcquireDInputJoystickDevice(pJoystickIndex);
    info = &C2V(gDirectInputJoystickInfos)[pJoystickIndex];
    if (info->buttonMask == 0 && info->axisMask == 0) {
        DIDEVCAPS caps;
        HRESULT hRes;
        DIDEVICEOBJECTINSTANCE device_obj_descr;
        tU32 i;

        caps.dwSize = sizeof(caps);
        hRes = IDirectInputDevice2_GetCapabilities(device, &caps);
        if (hRes != DI_OK) {
            return NULL;
        }
        if (caps.dwFlags & DIDC_FORCEFEEDBACK) {
            C2V(gJoystickFFB) |= 1 << pJoystickIndex;
        }
        info->count_buttons = caps.dwButtons; // unsure
        device_obj_descr.dwSize = sizeof(device_obj_descr);

        for (i = 0; i < caps.dwButtons && i < REC2_ASIZE(((tJoystickInputState*)NULL)->buttons); i++) {
            hRes = IDirectInputDevice2_GetObjectInfo(device, &device_obj_descr,
                                                     offsetof(tJoystickInputState, buttons) + i, DIPH_BYOFFSET);
            if (hRes == DI_OK) {
                info->buttonMask |= 1 << i;
                if (pButtonMask != NULL) {
                    *pButtonMask = info->buttonMask; // unsure
                }
            }
        }

        for (i = 0; i < caps.dwAxes && i < 8; i++) {
            hRes = IDirectInputDevice2_GetObjectInfo(device, &device_obj_descr,
                                                     sizeof(tU32) * i, DIPH_BYOFFSET);
            if (hRes == DI_OK) {
                info->axisMask |= 1 << i;
                if (pAxisMask != NULL) {
                    *pAxisMask = info->axisMask; // unsure
                }
            }
        }
    } else {
        if (pButtonMask != NULL) {
            *pButtonMask = info->buttonMask;
        }
        if (pAxisMask != NULL) {
            *pAxisMask = info->axisMask;
        }
    }
    return info;
}
C2_HOOK_FUNCTION(0x004589d0, JoystickDInputGetInfo)

BOOL CALLBACK Win32DInputJoystickEnum(const DIDEVICEINSTANCEA* pDeviceInstance, void *pContext) {
    HRESULT hResult;
    IDirectInputDeviceA *device;
    IDirectInputDevice2A *device2;
    int nb;
    IDirectInputA* pDirectInput = pContext;

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
    hResult = IDirectInputDevice_SetDataFormat(device, &c_dfDIJoystick);
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

    original_index = C2V(gJoystick_index);
    for (i = 0; i < REC2_ASIZE(C2V(gDirectInputJoystickDevices)); i++) {
        IDirectInputDevice2A *device;

        device = C2V(gDirectInputJoystickDevices)[i];
        if (device != NULL && C2V(gDirectInputJoystickInfos)[i].data == NULL) {
            C2V(gJoystick_index) = i;
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
    C2V(gJoystick_index) = original_index;
}
C2_HOOK_FUNCTION(0x00459fe0, AttachJoystickButtonInfos)

const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void) {
    return C2V(gDirectInputJoystickInfos)[C2V(gJoystick_index)].productName;
}
C2_HOOK_FUNCTION(0x004599d0, GetCurrentJoystickName)

size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void) {
    return C2V(gDirectInputJoystickInfos)[C2V(gJoystick_index)].count_buttons;
}
C2_HOOK_FUNCTION(0x00459fb0, GetCurrentJoystickCountButtons)

tButtonJoystickInfo* C2_HOOK_FASTCALL GetCurrentJoystickData(void) {
    if (C2V(gJoystick_index) == -1) {
        return NULL;
    }
    return C2V(gDirectInputJoystickInfos)[C2V(gJoystick_index)].data;
}
C2_HOOK_FUNCTION(0x00459f80, GetCurrentJoystickData)

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

void (C2_HOOK_FASTCALL * CollectJoystickButtonInfos_original)(void);
void C2_HOOK_FASTCALL CollectJoystickButtonInfos(void) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return JoystickDInputDetect_original();
#else
    C2_HOOK_BUG_ON(sizeof(tButtonJoystickInfo) != 0xec);
    AttachJoystickButtonInfos(sizeof(tButtonJoystickInfo), (void(C2_HOOK_FASTCALL*)(void*))CollectJoystickButtonInfo);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c410, CollectJoystickButtonInfos, CollectJoystickButtonInfos_original)

void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index) {
    IDirectInputEffect* effect = C2V(gDirectInputEffects)[index];
    if (effect != NULL) {
        IDirectInputEffect_Unload(effect);
    }
}
C2_HOOK_FUNCTION(0x00458040, UnloadDinputFFBEffectAtIndex)

int C2_HOOK_FASTCALL CreateDinputEffect(int joystick_index, tJoystick_effect_description* description, const char *name) {
    int effect_id = C2V(gCount_joystick_effects);
    IDirectInputDevice2A *device = C2V(gDirectInputJoystickDevices)[joystick_index];
    static C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(DWORD, axis, 2, 0x00684620);

    C2_HOOK_BUG_ON(sizeof(tJoystick_effect_description) != 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type_specific_params, 0x4);
    C2_HOOK_BUG_ON(sizeof(description->type_specific_params) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, time_ms, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x3c, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x4c, 0x4c);

    if (effect_id >= REC2_ASIZE(C2V(gJoystick_effects))) {
        dr_dprintf("Reached hardcoded limit for # of effects.");
        return -1;
    }
    if (device == NULL || !(C2V(gJoystickFFB) & (1 << joystick_index))) {
        return -1;
    }
    C2V(gJoystick_effects)[effect_id].di_effect.dwDuration = description->time_ms != -1 ? 1000 * description->time_ms : -1;
    C2V(gJoystick_effects)[effect_id].di_effect.dwGain = description->field_0x3c;
    // FIXME: dwTriggerButton contains offset
    C2V(gJoystick_effects)[effect_id].di_effect.dwTriggerButton = description->field_0x44 != -1 ? description->field_0x44 + 0x30 : -1;
    C2V(gJoystick_effects)[effect_id].di_effect.dwTriggerRepeatInterval = description->field_0x48;
    C2V(gJoystick_effects)[effect_id].field_0x60 = 100 * description->field_0x4c;
    if (description->field_0x40 == 0 || (C2V(gDirectInputJoystickInfos)[joystick_index].devSubType & 0x2)) {
        C2V(gJoystick_effects)[effect_id].di_effect.cAxes = 1;
        C2V(gJoystick_effects)[effect_id].di_effect.rgdwAxes = C2V(axis);
        // FIXME: offset
        C2V(axis)[0] = 0;
    } else if (description->field_0x40 == 1) {
        C2V(gJoystick_effects)[effect_id].di_effect.cAxes = 1;
        C2V(gJoystick_effects)[effect_id].di_effect.rgdwAxes = C2V(axis);
        // FIXME: offset
        C2V(axis)[0] = 4;
    } else {
        C2V(gJoystick_effects)[effect_id].di_effect.cAxes = 2;
        C2V(gJoystick_effects)[effect_id].di_effect.rgdwAxes = C2V(axis);
        // FIXME: offset
        C2V(axis)[0] = 0;
        C2V(axis)[1] = 4;
    }
    memcpy(&C2V(gJoystick_effects)[effect_id].type_specific_params,
        &description->type_specific_params,
        sizeof(tJoystick_effect_type_params));
    C2_HOOK_BUG_ON(sizeof(DIEFFECT) != 0x34);
    C2V(gJoystick_effects)[effect_id].di_effect.dwSize = sizeof(DIEFFECT);
    C2V(gJoystick_effects)[effect_id].di_effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
    C2V(gJoystick_effects)[effect_id].di_effect.dwSamplePeriod = 0;
    C2V(gJoystick_effects)[effect_id].di_effect.rglDirection = &C2V(gJoystick_effects)[effect_id].field_0x60;
    C2V(gJoystick_effects)[effect_id].di_effect.lpEnvelope = NULL;
    C2V(gJoystick_effects)[effect_id].di_effect.lpvTypeSpecificParams = &C2V(gJoystick_effects)[effect_id].type_specific_params;
    C2V(gJoystick_effects)[effect_id].field_0x64 = 0;

    C2_HOOK_BUG_ON(sizeof(DICONSTANTFORCE) != 0x4);
    C2_HOOK_BUG_ON(sizeof(DIRAMPFORCE) != 0x8);
    C2_HOOK_BUG_ON(sizeof(DICONDITION) != 0x18);
    switch (description->type) {
    case eJoystick_effect_ConstantForce:
        C2V(gJoystick_effects)[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        break;
    case eJoystick_effect_RampForce:
    case eJoystick_effect_CustomForce:
    case eJoystick_effect_Square:
    case eJoystick_effect_Sine:
    case eJoystick_effect_Triangle:
    case eJoystick_effect_SawtoothUp:
        C2V(gJoystick_effects)[effect_id].di_effect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
        break;
    case eJoystick_effect_Spring_8:
    case eJoystick_effect_Damper:
    case eJoystick_effect_Inertia:
    case eJoystick_effect_Friction:
        C2V(gJoystick_effects)[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONDITION);
        break;
    case eJoystick_effect_Spring_9:
        // FIXME: what sizeof() is this? (tye = Spring)
        C2V(gJoystick_effects)[effect_id].di_effect.cbTypeSpecificParams = 0x30;
        break;
    default:
        dr_dprintf("Illegal effect Type");
        return -1;
    }
    IDirectInputDevice2_CreateEffect(device,
        C2V(gJoystick_effect_guids)[description->type],
        &C2V(gJoystick_effects)[effect_id].di_effect,
        &C2V(gDirectInputEffects)[C2V(gCount_joystick_effects)],
        NULL);
    strcpy(C2V(gJoystick_effects)[C2V(gCount_joystick_effects)].name, name);
    C2V(gCount_joystick_effects) += 1;
    return effect_id;
}
C2_HOOK_FUNCTION(0x00457c20, CreateDinputEffect)

int (C2_HOOK_FASTCALL * ReadIforceEffectsNames_original)(const char *path, char **names, size_t capacity);
int C2_HOOK_FASTCALL ReadIforceEffectsNames(const char *path, char **names, size_t capacity) {
#if defined(C2_HOOKS_ENABLED)
    return ReadIforceEffectsNames_original(path, names, capacity);
#else
    /* Implementation differs */

    FILE *f;
    tU32 count;
    size_t i;
    char buffer[128];

    f = c2_fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }
    c2_rewind(f);
    fread(buffer, 1, 4, f);
    if (c2_memcmp(buffer, "ifpr", 4) != 0) {
        return 0;
    }
    c2_fread(&count, 1, sizeof(count), f);
    for (i = 0; i < count && i < capacity && !c2_feof(f); i++) {
        tU32 chunk_size;
        size_t len_name = 0;
        char c;
        fread(&chunk_size, 1, sizeof(tU32), f);

        while ((c = c2_fgetc(f)) != '\0' && len_name < REC2_ASIZE(buffer) && !c2_feof(f)) {
            buffer[len_name] = c;
            len_name += 1;
        }
        names[i] = c2_malloc(len_name + 1);
        c2_strncpy(names[i], buffer, len_name);
        names[i][len_name] = '\0';
        c2_fseek(f, chunk_size - len_name - 1 - 4, SEEK_CUR);
    }
    return i;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045a390, ReadIforceEffectsNames, ReadIforceEffectsNames_original)

int C2_HOOK_FASTCALL UnloadDinputFFBEfectwithName(const char *name) {
    int i;

    for (i = 0; i < C2V(gCount_joystick_effects); i++) {
        if (c2_strcmp(C2V(gJoystick_effects)[i].name, name) == 0) {
            UnloadDinputFFBEffectAtIndex(i);
            return i;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00458060, UnloadDinputFFBEfectwithName)

int (C2_HOOK_FASTCALL * InitForceFeedback_original)(void);
int C2_HOOK_FASTCALL InitForceFeedback(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    return InitForceFeedback_original();
#else
    tPath_name path;
    int count_effects;
    char *effect_names[20];
    int i;

    if (C2V(gCountEnumeratedJoystickDinputDevices) == 0) {
        JoystickDInputBegin();
    }

    if (C2V(gJoystickFFB) != 0) {
        int i;
        for (i = 0; i < REC2_ASIZE(C2V(gDirectInputJoystickDevices)); i++) {
            IDirectInputDevice2A *device = C2V(gDirectInputJoystickDevices)[i];
            if (device != NULL && (C2V(gJoystickFFB) & (1 << i))) {
                IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_RESET);
                C2V(gForceFeedbackAvailable) = 1;
            }
        }
    }
    if (!C2V(gForceFeedbackAvailable)) {
        return 0;
    }
    PathCat(path, C2V(gApplication_path), "FFB.IFR");
    C2V(gIfr_project) = IFLoadProjectFile(path, C2V(gDirectInputJoystickDevices)[C2V(gJoystick_index)]);
    if (C2V(gIfr_project) == NULL) {
        PDFatalError("Cant enable I-Force");
    }
    count_effects = ReadIforceEffectsNames(path, effect_names, REC2_ASIZE(effect_names));
    for (i = 0; i < count_effects; i++) {
        int count;
        DIEFFECT di_effect;
        IFR_Effect **effects;

        effects = IFCreateEffectStructs(C2V(gIfr_project), effect_names[i], &count);
        di_effect = *effects[0]->di_effect;
        IDirectInputDevice2_CreateEffect(C2V(gDirectInputJoystickDevices)[C2V(gJoystick_index)], &effects[0]->guid, &di_effect, &C2V(gDirectInputEffects)[C2V(gCount_joystick_effects)], NULL);
        c2_strcpy(C2V(gJoystick_effects)[C2V(gCount_joystick_effects)].name, effect_names[i]);
        C2V(gCount_joystick_effects) += 1;
        UnloadDinputFFBEfectwithName(effect_names[i]);
    }
    IFReleaseProject(C2V(gIfr_project));
    return C2V(gForceFeedbackAvailable);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00457760, InitForceFeedback, InitForceFeedback_original)

void (C2_HOOK_FASTCALL * ResetDInputJoystickFFB_original)(int pIndex);
void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex) {
#if 0//defined(C2_HOOKS_ENABLED)
    return ResetDInputJoystickFFB_original(pIndex);
#else
    IDirectInputDevice2A *device;

    device = C2V(gDirectInputJoystickDevices)[pIndex];
    if (device != NULL && (C2V(gJoystickFFB) & (1 << pIndex))) {
        IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_STOPALL);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00457ff0, ResetDInputJoystickFFB, ResetDInputJoystickFFB_original)

void (C2_HOOK_FASTCALL * RegisterJoystickFFBForces_original)(void);
void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    RegisterJoystickFFBForces_original();
#else
    tJoystick_effect_description description;

#ifdef REC2_FIX_BUGS
    c2_memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_Friction;
    description.type_specific_params.friction.field_0x0 = 0;
    description.type_specific_params.friction.field_0x4 = 10000;
    description.type_specific_params.friction.field_0x8 = 10000;
    description.time_ms = 5000000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    C2V(gBasic_friction_joystick_effect_index) = CreateDinputEffect(C2V(gJoystick_index), &description, "Basic Friction");

#ifdef REC2_FIX_BUGS
    c2_memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 5000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    C2V(gBasic_force_joystick_effect_index ) = CreateDinputEffect(C2V(gJoystick_index), &description, "Basic Force");

#ifdef REC2_FIX_BUGS
    c2_memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 1000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    C2V(gCollision_force_joystick_effect_index) = CreateDinputEffect(C2V(gJoystick_index), &description, "Collision Force");
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00458520, RegisterJoystickFFBForces, RegisterJoystickFFBForces_original)

int (C2_HOOK_FASTCALL * JoystickDInputBegin_original)(void);
int C2_HOOK_FASTCALL JoystickDInputBegin(void) {
#if 0 // defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    int res = JoystickDInputBegin_original();
    C2_HOOK_FINISH();
    return res;
#else
    int i;
    if (InitDirectInput()) {
        return 0;
    }
    if (!C2V(gDirectInputJoystickEnumerated)) {
        C2V(gDirectInputJoystickEnumerated) = 1;
        if (C2V(gCountEnumeratedJoystickDinputDevices) == 0) {
            IDirectInput_EnumDevices(C2V(gDirectInputJoystickHandle), DIDEVTYPE_JOYSTICK, Win32DInputJoystickEnum, C2V(gDirectInputJoystickHandle), DIEDFL_ATTACHEDONLY);
        }
        if (C2V(gCountEnumeratedJoystickDinputDevices) != 0) {
            for (i = 0; i < REC2_ASIZE(C2V(gDirectInputJoystickDevices)); i++) {
                JoystickDInputGetInfo(i, NULL, NULL);
                if (C2V(gDirectInputJoystickDevices)[i] != NULL) {
                    AcquireDInputJoystickDevice(i);
                }
            }
            C2V(gJoystick_index) = 0;
        }
        if (C2V(gJoystick_index) < 0) {
            return 0;
        }
        if (InitForceFeedback()) {
            ResetDInputJoystickFFB(C2V(gJoystick_index));
            RegisterJoystickFFBForces();
        }
    }
    return C2V(gCountEnumeratedJoystickDinputDevices);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00458860, JoystickDInputBegin, JoystickDInputBegin_original)

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
    C2V(gScan_code)[16] = DIK_CAPITAL;
    C2V(gScan_code)[17] = DIK_RSHIFT;
    C2V(gScan_code)[18] = DIK_RMENU;
    C2V(gScan_code)[19] = DIK_RCONTROL;
    C2V(gScan_code)[20] = DIK_LSHIFT;
    C2V(gScan_code)[21] = DIK_LMENU;
    C2V(gScan_code)[22] = DIK_LCONTROL;
    C2V(gScan_code)[23] = DIK_0;
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
        CollectJoystickButtonInfos();
        LoadJoystickPreferences();
    }
    C2V(gJoystick_deadzone) = 8000;
}
C2_HOOK_FUNCTION(0x0051cbf0, Win32InitInputDevice)

tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void) {
    IDirectInputDevice2 *device;
    HRESULT res;

    if (C2V(gJoystick_index) == -1) {
        return 0;
    }
    device = C2V(gDirectInputJoystickDevices)[C2V(gJoystick_index)];
    if (device == NULL) {
        return 0;
    }
    res = IDirectInputDevice2_Poll(device);
    if (FAILED(res)) {
        switch (res) {
        case DIERR_INPUTLOST:
        case DIERR_NOTACQUIRED:
            if (AcquireDInputJoystickDevice(C2V(gJoystick_index)) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", C2V(gJoystick_index));
                return 0;
            }
            break;
        default:
            dr_dprintf("Can't poll, unknown error\n");
            return 0;
        }
    } else {
        DIJOYSTATE device_state;
        tU32 button_mask;
        int i;

        C2_HOOK_BUG_ON(sizeof(device_state) != 0x50);
        res = IDirectInputDevice2_GetDeviceState(device, sizeof(device_state), &device_state);
        if (FAILED(res)) {
            if (res == DIERR_INPUTLOST) {
                AcquireDInputJoystickDevice(C2V(gJoystick_index));
            }
            return 0;
        }
        button_mask = 0;
        for (i = 0; i < 32; i++) {
            if (device_state.rgbButtons[i] & 0x80) {
                button_mask |= (1 << i);
            }
        }
        return button_mask;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00459690, PDGetJoystickButtonStates)

void C2_HOOK_FASTCALL PDSetKeysFromJoystick(int *keys) {
    if (C2V(gJoystick_index) != 1) {
        tU32 button_mask;

        button_mask = PDGetJoystickButtonStates();
        if (GetCurrentJoystickData() != NULL) {
            int i;

            for (i = 0; i < 32; i++) {
                keys[107 + i] = !!(button_mask & (i << i));
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0045b360, PDSetKeysFromJoystick)

void (C2_HOOK_FASTCALL * PDSetKeyArray_original)(int* pKeys, int pMark);
void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark) {

#if 0//defined(C2_HOOKS_ENABLED)
    PDSetKeyArray_original(pKeys, pMark);
#else
    BYTE diKeys[256];
    HRESULT res;
    int i;

    C2V(gKeys_pressed) = 0;
    Win32ServiceMessages();
    if (C2V(gWindowActiveState) != 2) {
        *pKeys = 0;
        return;
    }
    if (C2V(gTimeLastKeyboardInput) != 0) {
        if (PDGetTotalTime() < C2V(gTimeLastKeyboardInput) + 1000) {
            *pKeys = 0;
            return;
        }
    }
    C2V(gTimeLastKeyboardInput) = 0;
    res = IDirectInputDevice_GetDeviceState(C2V(gDirectInputDevice), sizeof(diKeys), diKeys);
    if (res != DI_OK) {
        dr_dprintf("ACTIVE but couldn't get keyboard device state on 1st attempt");
        dr_dprintf("Keyboard input lost; reacquiring...");
        if (C2V(gDirectInputDevice) != NULL) {
            IDirectInputDevice_Acquire(C2V(gDirectInputDevice));
        }
        res = IDirectInputDevice_GetDeviceState(C2V(gDirectInputDevice), sizeof(diKeys), diKeys);
        if (res != DI_OK) {
            dr_dprintf("Couldn't get keyboard device state on 2nd attempt");
            dr_dprintf("Zeroing pKeys");
            *pKeys = 0;
            return;
        }
        dr_dprintf("Keyboard reacquired OK.");
    }
    for (i = 0; i < 151; i++) {
        int scan_code;

        scan_code = C2V(gScan_code)[12 + i];
        if (scan_code == 0) {
            continue;
        }
        if (scan_code == 0xff) {
            switch (i) {
            case 0:
                if ((diKeys[C2V(gScan_code)[20]] & 0x80) || (diKeys[C2V(gScan_code)[17]] & 0x80)) {
                    pKeys[0] = pMark;
                } else if (pKeys[0] == pMark) {
                    pKeys[0] = 0;
                }
                break;
            case 1:
                if ((diKeys[C2V(gScan_code)[21]] & 0x80) || (diKeys[C2V(gScan_code)[18]] & 0x80)) {
                    pKeys[1] = pMark;
                } else if (pKeys[1] == pMark) {
                    pKeys[1] = 0;
                }
                break;
            case 2:
            case 3:
                if ((diKeys[C2V(gScan_code)[22]] & 0x80) || (diKeys[C2V(gScan_code)[19]] & 0x80)) {
                    pKeys[2] = pMark;
                    pKeys[3] = pMark;
                } else if (pKeys[2] == pMark || pKeys[3] == pMark) {
                    pKeys[2] = 0;
                    pKeys[3] = 0;
                }
                break;
            }
        } else {
            if (diKeys[scan_code] & 0x80) {
                pKeys[i] = pMark;
                C2V(gKeys_pressed) = C2V(gKeys_pressed) << 8 | (i + 1);
            } else if (pKeys[i] == pMark) {
                pKeys[i] = 0;
            }
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0051cef0, PDSetKeyArray, PDSetKeyArray_original)

void C2_HOOK_FASTCALL PDReadJoysticks(void) {

    if (C2V(gJoystick_index) != -1) {
        C2V(gJoy1_valid) = 1;
        return;
    }
    if (C2V(gJoy1_valid)) {
        if (joyGetPosEx(0, &C2V(gJoy1_info)) != JOYERR_NOERROR) {
            C2V(gJoy1_valid) = 0;
        }
    }
    if (C2V(gJoy2_valid)) {
        if (joyGetPosEx(1, &C2V(gJoy2_info)) != JOYERR_NOERROR) {
            C2V(gJoy2_valid) = 0;
        }
    }
}
C2_HOOK_FUNCTION(0x0051d1d0, PDReadJoysticks)

int C2_HOOK_FASTCALL GetDirectInputJoy1X(void) {
    IDirectInputDevice2A* device;
    HRESULT res;
    DIJOYSTATE joy_state;
    int x;

    if (C2V(gJoystick_index) == -1) {
        return -1;
    }
    device = C2V(gDirectInputJoystickDevices)[C2V(gJoystick_index)];
    if (device == NULL) {
        return -1;
    }
    if (!C2V(gINT_00595f98)) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(device);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(C2V(gJoystick_index)) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", C2V(gJoystick_index));
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(device, sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(C2V(gJoystick_index)) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", C2V(gJoystick_index));
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }
    x = (int)((float)(joy_state.lX - 0x7fff) * C2V(gJoystick_x_steering) + (float)0x7fff);
    if (x > 0xffff) {
        x = 0xffff;
    }
    if (x < 0) {
        x = 0;
    }
    return x;
}
C2_HOOK_FUNCTION(0x00459050, GetDirectInputJoy1X)

int C2_HOOK_FASTCALL GetDirectInputJoy1Y(void) {
    IDirectInputDevice2A* device;
    HRESULT res;
    DIJOYSTATE joy_state;
    int y;

    if (C2V(gJoystick_index) == -1) {
        return -1;
    }
    device = C2V(gDirectInputJoystickDevices)[C2V(gJoystick_index)];
    if (device == NULL) {
        return -1;
    }
    if (!C2V(gINT_00595f9c)) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(device);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(C2V(gJoystick_index)) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", C2V(gJoystick_index));
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(device, sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(C2V(gJoystick_index)) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", C2V(gJoystick_index));
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }
    y = (int)((float)(joy_state.lY - 0x7fff) * C2V(gJoystick_y_throttle) + (float)0x7fff);
    if (y > 0xffff) {
        y = 0xffff;
    }
    if (y < 0) {
        y = 0;
    }
    return y;
}
C2_HOOK_FUNCTION(0x00459370, GetDirectInputJoy1Y)

tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void) {

    if (!C2V(gJoy1_valid)) {
        return 0;
    }
    return C2V(gJoy1_info).dwButtons & (1 << 0);
}
C2_HOOK_FUNCTION(0x0051d2c0, PDGetJoy1Button1)

tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void) {

    if (!C2V(gJoy1_valid)) {
        return 0;
    }
    return C2V(gJoy1_info).dwButtons & (1 << 1);
}
C2_HOOK_FUNCTION(0x0051d2e0, PDGetJoy1Button2)
