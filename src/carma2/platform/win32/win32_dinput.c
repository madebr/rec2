#include "win32_dinput.h"

#include "win32.h"

#include "52-errors.h"
#include "globvars.h"
#include "02-init.h"
#include "42-input.h"
#include "joystick.h"
#include "41-utility.h"

#include "rec2_macros.h"

#include "c2_stdlib.h"
#include "c2_string.h"

#include <windows.h>
#include <dinput.h>
#include <iforce2.h>

#define MAX_COUNT_JOYSTICKS 16
#define MAX_COUNT_EFFECTS 30


// GLOBAL: CARMA2_HW 0x006ad1f8
int gASCII_table[151];

// GLOBAL: CARMA2_HW 0x006ac588
int gASCII_shift_table[151];

// GLOBAL: CARMA2_HW 0x006b3470
int gScan_code[256];


// GLOBAL: CARMA2_HW 0x006acc78
LPDIRECTINPUTA gDirectInput;


// GLOBAL: CARMA2_HW 0x00762280
int gJoystick_deadzone;


// GLOBAL: CARMA2_HW 0x00686158
IDirectInputA* gDirectInputJoystickHandle;

// GLOBAL: CARMA2_HW 0x0079e060
IDirectInputDevice2A* gDirectInputJoystickDevices[MAX_COUNT_JOYSTICKS];

// GLOBAL: CARMA2_HW 0x0068615c
int gJoystickFFB;

// GLOBAL: CARMA2_HW 0x00686160
int gCountEnumeratedJoystickDinputDevices;

// GLOBAL: CARMA2_HW 0x0079e0a0
IDirectInputEffect* gDirectInputEffects[MAX_COUNT_EFFECTS];

// GLOBAL: CARMA2_HW 0x00686170
int gDirectInputJoystickEnumerated;

// GLOBAL: CARMA2_HW 0x0079d9c0
tDirectInputJoystickInfo gDirectInputJoystickInfos[MAX_COUNT_JOYSTICKS];

// GLOBAL: CARMA2_HW 0x00684628
tJoystick_force_effect gJoystick_effects[30];

// GLOBAL: CARMA2_HW 0x00686168
int gForceFeedbackAvailable;

// GLOBAL: CARMA2_HW 0x00595f74
int gBasic_friction_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x00595f78
int gBasic_force_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x00595f80
int gCollision_force_joystick_effect_index = -1;

// GLOBAL: CARMA2_HW 0x0079e040
IFR_Project * gIfr_project;

// GLOBAL: CARMA2_HW 0x00686164
int gCount_joystick_effects;

// GLOBAL: CARMA2_HW 0x00589f38
const GUID* gJoystick_effect_guids[13] = {
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
};

// GLOBAL: CARMA2_HW 0x006ad4b8
int gJoy1_valid;

// GLOBAL: CARMA2_HW 0x006ac7e8
JOYINFOEX gJoy1_info;

// GLOBAL: CARMA2_HW 0x006ad4bc
int gJoy2_valid;

// GLOBAL: CARMA2_HW 0x006ac820
JOYINFOEX gJoy2_info;

// GLOBAL: CARMA2_HW 0x00595f98
int gINT_00595f98 = 1;

// GLOBAL: CARMA2_HW 0x00595f9c
int gINT_00595f9c = 1;

// GLOBAL: CARMA2_HW 0x006ad094
tU32 gUINT_006ad094;

// GLOBAL: CARMA2_HW 0x006ad0b8
int gINT_006ad0b8;

static int InitDirectInput(void) {
    int i;

    if (gDirectInputJoystickHandle == NULL) {
        memset(gDirectInputJoystickDevices, 0, sizeof(IDirectInputDeviceA *));
        for (i = 0; i < REC2_ASIZE(gDirectInputJoystickInfos); i++) {
            gDirectInputJoystickInfos[i].buttonMask = 0;
            gDirectInputJoystickInfos[i].axisMask = 0;
            gDirectInputJoystickInfos[i].devSubType = 0;
            gDirectInputJoystickInfos[i].productName[0] = '\0';
            gDirectInputJoystickInfos[i].sizeData = 0;
            gDirectInputJoystickInfos[i].data = NULL;
        }
        memset(gDirectInputEffects, 0, sizeof(gDirectInputEffects));
        gJoystickFFB = 0;
        gCountEnumeratedJoystickDinputDevices = 0;
        DirectInputCreateA(GetModuleHandleA(NULL), 0x0500, &gDirectInputJoystickHandle, NULL);
        if (gDirectInputJoystickHandle == NULL) {
            return 1;
        }
        gDirectInputJoystickEnumerated = 0;
    }
    return 0;
}

void C2_HOOK_FASTCALL CloseDirectInput(void) {

    if (gDirectInputDevice != NULL) {
        IDirectInputDevice_Unacquire(gDirectInputDevice);
        IDirectInputDevice_Release(gDirectInputDevice);
        gDirectInputDevice = NULL;
    }
    if (gDirectInput != NULL) {
        IDirectInput_Release(gDirectInput);
        gDirectInput = NULL;
    }
}

static int AcquireDInputJoystickDevice(int pIndex) {
    IDirectInputDevice2A *device;

    device = gDirectInputJoystickDevices[pIndex];
    if (gDirectInputJoystickHandle == NULL || device == NULL) {
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

// FUNCTION: CARMA2_HW 0x004589d0
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
    device = gDirectInputJoystickDevices[pJoystickIndex];
    if (device == NULL) {
        return NULL;
    }
    AcquireDInputJoystickDevice(pJoystickIndex);
    info = &gDirectInputJoystickInfos[pJoystickIndex];
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
            gJoystickFFB |= 1 << pJoystickIndex;
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

// FUNCTION: CARMA2_HW 0x00459c70
BOOL CALLBACK Win32DInputJoystickEnum(const DIDEVICEINSTANCEA* pDeviceInstance, void *pContext) {
    HRESULT hResult;
    IDirectInputDeviceA *device;
    IDirectInputDevice2A *device2;
    int nb;
    IDirectInputA* pDirectInput = pContext;
    DIPROPRANGE diphRange;
    DIPROPDWORD zoneProp;

    C2_HOOK_ASSERT(sizeof(tJoystickInputState) == 0x50);

    nb = gCountEnumeratedJoystickDinputDevices;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
    strncpy(gDirectInputJoystickInfos[nb].productName, pDeviceInstance->tszProductName, sizeof(gDirectInputJoystickInfos[nb].productName));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    gDirectInputJoystickInfos[nb].productName[sizeof(gDirectInputJoystickInfos[nb].productName) - 1] = '\0';
    switch (pDeviceInstance->dwDevType) {
    case DIDEVTYPEJOYSTICK_GAMEPAD:
        gDirectInputJoystickInfos[nb].count_buttons = 4;
        break;
    case DIDEVTYPEJOYSTICK_WHEEL:
        gDirectInputJoystickInfos[nb].count_buttons = 2;
        break;
    default:
        gDirectInputJoystickInfos[nb].count_buttons = 1;
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
                                                (void **)&gDirectInputJoystickDevices[nb]);
    IDirectInputDevice2_Release(device);
    if (hResult != DI_OK) {
        dr_dprintf("QueryInterface did not return DI_OK\n");
        return DIENUM_CONTINUE;
    }
    device2 = gDirectInputJoystickDevices[nb];
    if (device2 == NULL) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
    hResult = IDirectInputDevice2_SetCooperativeLevel(device2, gHWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    if (hResult != DI_OK) {
        dr_dprintf("Could not set dinput coop level\n");
        return DIENUM_STOP;
    }
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
    gCountEnumeratedJoystickDinputDevices++;
    return DIENUM_CONTINUE;
}

// FUNCTION: CARMA2_HW 0x00459fe0
void C2_HOOK_FASTCALL AttachJoystickButtonInfos(size_t pSize, tWin32_void_voidptr_cbfn pCallback) {
    int original_index;
    int i;

    original_index = gJoystick_index;
    for (i = 0; i < REC2_ASIZE(gDirectInputJoystickDevices); i++) {
        IDirectInputDevice2A *device;

        device = gDirectInputJoystickDevices[i];
        if (device != NULL && gDirectInputJoystickInfos[i].data == NULL) {
            gJoystick_index = i;
            if (pSize != 0) {
                gDirectInputJoystickInfos[i].data = malloc(pSize);
                if (gDirectInputJoystickInfos[i].data != NULL) {
                    gDirectInputJoystickInfos[i].sizeData = pSize;
                }
            }
            if (gDirectInputJoystickInfos[i].data != NULL) {
                pCallback(gDirectInputJoystickInfos[i].data);
            }
        }
    }
    gJoystick_index = original_index;
}

// FUNCTION: CARMA2_HW 0x004599d0
const char* C2_HOOK_FASTCALL GetCurrentJoystickName(void) {
    return gDirectInputJoystickInfos[gJoystick_index].productName;
}

// FUNCTION: CARMA2_HW 0x00459fb0
size_t C2_HOOK_FASTCALL GetCurrentJoystickCountButtons(void) {
    return gDirectInputJoystickInfos[gJoystick_index].count_buttons;
}

// FUNCTION: CARMA2_HW 0x00459f80
tButtonJoystickInfo* C2_HOOK_FASTCALL PDGetCurrentJoystickData(void) {
    if (gJoystick_index == -1) {
        return NULL;
    }
    return gDirectInputJoystickInfos[gJoystick_index].data;
}

// FUNCTION: CARMA2_HW 0x0045c370
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
        pInfo->field_0xd4 = 1;
        pInfo->field_0xd8 = 1.0f;
        pInfo->field_0xdc = 1.0f;
        pInfo->field_0xe0 = 0; /* or 0.f */
        pInfo->field_0xe4 = 50;
        pInfo->field_0xe8 = 0; /* or 0.f */
    }
}

// FUNCTION: CARMA2_HW 0x0045c410
void C2_HOOK_FASTCALL CollectJoystickButtonInfos(void) {
    C2_HOOK_BUG_ON(sizeof(tButtonJoystickInfo) != 0xec);
    AttachJoystickButtonInfos(sizeof(tButtonJoystickInfo), (void(C2_HOOK_FASTCALL*)(void*))CollectJoystickButtonInfo);
}

// FUNCTION: CARMA2_HW 0x00458040
void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index) {
    IDirectInputEffect* effect = gDirectInputEffects[index];
    if (effect != NULL) {
        IDirectInputEffect_Unload(effect);
    }
}

// FUNCTION: CARMA2_HW 0x00457c20
int C2_HOOK_FASTCALL CreateDinputEffect(int joystick_index, tJoystick_effect_description* description, const char *name) {
    int effect_id = gCount_joystick_effects;
    IDirectInputDevice2A *device = gDirectInputJoystickDevices[joystick_index];
    // GLOBAL: CARMA2_HW 0x00684620
    static DWORD axis[2];

    C2_HOOK_BUG_ON(sizeof(tJoystick_effect_description) != 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, type_specific_params, 0x4);
    C2_HOOK_BUG_ON(sizeof(description->type_specific_params) != 0x18);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, time_ms, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x3c, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x48, 0x48);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tJoystick_effect_description, field_0x4c, 0x4c);

    if (effect_id >= REC2_ASIZE(gJoystick_effects)) {
        dr_dprintf("Reached hardcoded limit for # of effects.");
        return -1;
    }
    if (device == NULL || !(gJoystickFFB & (1 << joystick_index))) {
        return -1;
    }
    gJoystick_effects[effect_id].di_effect.dwDuration = description->time_ms != -1 ? 1000 * description->time_ms : -1;
    gJoystick_effects[effect_id].di_effect.dwGain = description->field_0x3c;
    // FIXME: dwTriggerButton contains offset
    gJoystick_effects[effect_id].di_effect.dwTriggerButton = description->field_0x44 != -1 ? description->field_0x44 + 0x30 : -1;
    gJoystick_effects[effect_id].di_effect.dwTriggerRepeatInterval = description->field_0x48;
    gJoystick_effects[effect_id].field_0x60 = 100 * description->field_0x4c;
    if (description->field_0x40 == 0 || (gDirectInputJoystickInfos[joystick_index].devSubType & 0x2)) {
        gJoystick_effects[effect_id].di_effect.cAxes = 1;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 0;
    } else if (description->field_0x40 == 1) {
        gJoystick_effects[effect_id].di_effect.cAxes = 1;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 4;
    } else {
        gJoystick_effects[effect_id].di_effect.cAxes = 2;
        gJoystick_effects[effect_id].di_effect.rgdwAxes = axis;
        // FIXME: offset
        axis[0] = 0;
        axis[1] = 4;
    }
    memcpy(&gJoystick_effects[effect_id].type_specific_params,
        &description->type_specific_params,
        sizeof(tJoystick_effect_type_params));
    C2_HOOK_BUG_ON(sizeof(DIEFFECT) != 0x34);
    gJoystick_effects[effect_id].di_effect.dwSize = sizeof(DIEFFECT);
    gJoystick_effects[effect_id].di_effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
    gJoystick_effects[effect_id].di_effect.dwSamplePeriod = 0;
    gJoystick_effects[effect_id].di_effect.rglDirection = &gJoystick_effects[effect_id].field_0x60;
    gJoystick_effects[effect_id].di_effect.lpEnvelope = NULL;
    gJoystick_effects[effect_id].di_effect.lpvTypeSpecificParams = &gJoystick_effects[effect_id].type_specific_params;
    gJoystick_effects[effect_id].field_0x64 = 0;

    C2_HOOK_BUG_ON(sizeof(DICONSTANTFORCE) != 0x4);
    C2_HOOK_BUG_ON(sizeof(DIRAMPFORCE) != 0x8);
    C2_HOOK_BUG_ON(sizeof(DICONDITION) != 0x18);
    switch (description->type) {
    case eJoystick_effect_ConstantForce:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        break;
    case eJoystick_effect_RampForce:
    case eJoystick_effect_CustomForce:
    case eJoystick_effect_Square:
    case eJoystick_effect_Sine:
    case eJoystick_effect_Triangle:
    case eJoystick_effect_SawtoothUp:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
        break;
    case eJoystick_effect_Spring_8:
    case eJoystick_effect_Damper:
    case eJoystick_effect_Inertia:
    case eJoystick_effect_Friction:
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = sizeof(DICONDITION);
        break;
    case eJoystick_effect_Spring_9:
        // FIXME: what sizeof() is this? (tye = Spring)
        gJoystick_effects[effect_id].di_effect.cbTypeSpecificParams = 0x30;
        break;
    default:
        dr_dprintf("Illegal effect Type");
        return -1;
    }
    IDirectInputDevice2_CreateEffect(device,
        gJoystick_effect_guids[description->type],
        &gJoystick_effects[effect_id].di_effect,
        &gDirectInputEffects[gCount_joystick_effects],
        NULL);
    strcpy(gJoystick_effects[gCount_joystick_effects].name, name);
    gCount_joystick_effects += 1;
    return effect_id;
}

// FUNCTION: CARMA2_HW 0x0045a390
int C2_HOOK_FASTCALL ReadIforceEffectsNames(const char *path, char **names, size_t capacity) {
    /* Implementation differs */

    FILE *f;
    tU32 count;
    size_t i;
    char buffer[128];

    f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }
    rewind(f);
    fread(buffer, 1, 4, f);
    if (memcmp(buffer, "ifpr", 4) != 0) {
        return 0;
    }
    fread(&count, 1, sizeof(count), f);
    for (i = 0; i < count && i < capacity && !feof(f); i++) {
        tU32 chunk_size;
        size_t len_name = 0;
        char c;
        fread(&chunk_size, 1, sizeof(tU32), f);

        while ((c = fgetc(f)) != '\0' && len_name < REC2_ASIZE(buffer) && !feof(f)) {
            buffer[len_name] = c;
            len_name += 1;
        }
        names[i] = malloc(len_name + 1);
        strncpy(names[i], buffer, len_name);
        names[i][len_name] = '\0';
        fseek(f, chunk_size - len_name - 1 - 4, SEEK_CUR);
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x00458060
int C2_HOOK_FASTCALL UnloadDinputFFBEfectwithName(const char *name) {
    int i;

    for (i = 0; i < gCount_joystick_effects; i++) {
        if (strcmp(gJoystick_effects[i].name, name) == 0) {
            UnloadDinputFFBEffectAtIndex(i);
            return i;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL ResetForceFeedback(void) {
	int result;

    result = 0;
    if (gJoystickFFB != 0) {
        int i;
        for (i = 0; i < REC2_ASIZE(gDirectInputJoystickDevices); i++) {
            IDirectInputDevice2A *device = gDirectInputJoystickDevices[i];
            if (device != NULL && (gJoystickFFB & (1 << i))) {
                IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_RESET);
                result = 1;
            }
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00457760
int C2_HOOK_FASTCALL InitForceFeedback(void) {
    tPath_name path;
    int count_effects;
    char *effect_names[20];
    int i;

    if (gCountEnumeratedJoystickDinputDevices == 0) {
        JoystickDInputBegin();
    }

    if (ResetForceFeedback()) {
        gForceFeedbackAvailable = 1;
    }
    if (!gForceFeedbackAvailable) {
        return 0;
    }
    PathCat(path, gApplication_path, "FFB.IFR");
    gIfr_project = IFLoadProjectFile(path, gDirectInputJoystickDevices[gJoystick_index]);
    if (gIfr_project == NULL) {
        PDFatalError("Cant enable I-Force");
    }
    count_effects = ReadIforceEffectsNames(path, effect_names, REC2_ASIZE(effect_names));
    for (i = 0; i < count_effects; i++) {
        int count;
        DIEFFECT di_effect;
        IFR_Effect **effects;

        effects = IFCreateEffectStructs(gIfr_project, effect_names[i], &count);
        di_effect = *effects[0]->di_effect;
        IDirectInputDevice2_CreateEffect(gDirectInputJoystickDevices[gJoystick_index], &effects[0]->guid, &di_effect, &gDirectInputEffects[gCount_joystick_effects], NULL);
        strcpy(gJoystick_effects[gCount_joystick_effects].name, effect_names[i]);
        gCount_joystick_effects += 1;
        UnloadDinputFFBEfectwithName(effect_names[i]);
    }
    IFReleaseProject(gIfr_project);
    return gForceFeedbackAvailable;
}

// FUNCTION: CARMA2_HW 0x00457ff0
void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex) {
    IDirectInputDevice2A *device;

    device = gDirectInputJoystickDevices[pIndex];
    if (device != NULL && (gJoystickFFB & (1 << pIndex))) {
        IDirectInputDevice2_SendForceFeedbackCommand(device, DISFFC_STOPALL);
    }
}

// FUNCTION: CARMA2_HW 0x00458520
void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void) {
    tJoystick_effect_description description;

#ifdef REC2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_Friction;
    description.type_specific_params.friction.field_0x0 = 0;
    description.type_specific_params.friction.field_0x4 = 10000;
    description.type_specific_params.friction.field_0x8 = 10000;
    description.time_ms = 5000000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gBasic_friction_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Basic Friction");

#ifdef REC2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 5000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gBasic_force_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Basic Force");

#ifdef REC2_FIX_BUGS
    memset(&description, 0, sizeof(description));
#endif
    description.type = eJoystick_effect_ConstantForce;
    description.type_specific_params.constant.field_0x0 = 0;
    description.time_ms = 1000000;
    description.field_0x38 = 1000;
    description.field_0x3c = 5000;
    description.field_0x44 = -1;
    description.field_0x4c = 0x5a;
    gCollision_force_joystick_effect_index = CreateDinputEffect(gJoystick_index, &description, "Collision Force");
}

// FUNCTION: CARMA2_HW 0x00458860
int C2_HOOK_FASTCALL JoystickDInputBegin(void) {
    int i;
    if (InitDirectInput()) {
        return 0;
    }
    if (!gDirectInputJoystickEnumerated) {
        gDirectInputJoystickEnumerated = 1;
        if (gCountEnumeratedJoystickDinputDevices == 0) {
            IDirectInput_EnumDevices(gDirectInputJoystickHandle, DIDEVTYPE_JOYSTICK, Win32DInputJoystickEnum, gDirectInputJoystickHandle, DIEDFL_ATTACHEDONLY);
        }
        if (gCountEnumeratedJoystickDinputDevices != 0) {
            for (i = 0; i < REC2_ASIZE(gDirectInputJoystickDevices); i++) {
                JoystickDInputGetInfo(i, NULL, NULL);
                if (gDirectInputJoystickDevices[i] != NULL) {
                    AcquireDInputJoystickDevice(i);
                }
            }
            gJoystick_index = 0;
        }
        if (gJoystick_index < 0) {
            return 0;
        }
        if (InitForceFeedback()) {
            ResetDInputJoystickFFB(gJoystick_index);
            RegisterJoystickFFBForces();
        }
    }
    return gCountEnumeratedJoystickDinputDevices;
}

// FUNCTION: CARMA2_HW 0x0051ba10
void C2_HOOK_FASTCALL KeyBegin(void) {
    memset(gASCII_table, 0, sizeof(gASCII_table));
    memset(gASCII_shift_table, 0, sizeof(gASCII_shift_table));

    gASCII_table[11] = '0';
    gASCII_table[12] = '1';
    gASCII_table[13] = '2';
    gASCII_table[14] = '3';
    gASCII_table[15] = '4';
    gASCII_table[16] = '5';
    gASCII_table[17] = '6';
    gASCII_table[18] = '7';
    gASCII_table[19] = '8';
    gASCII_table[20] = '9';
    gASCII_table[21] = 'a';
    gASCII_table[22] = 'b';
    gASCII_table[23] = 'c';
    gASCII_table[24] = 'd';
    gASCII_table[25] = 'e';
    gASCII_table[26] = 'f';
    gASCII_table[27] = 'g';
    gASCII_table[28] = 'h';
    gASCII_table[29] = 'i';
    gASCII_table[30] = 'j';
    gASCII_table[31] = 'k';
    gASCII_table[32] = 'l';
    gASCII_table[33] = 'm';
    gASCII_table[34] = 'n';
    gASCII_table[35] = 'o';
    gASCII_table[36] = 'p';
    gASCII_table[37] = 'q';
    gASCII_table[38] = 'r';
    gASCII_table[39] = 's';
    gASCII_table[40] = 't';
    gASCII_table[41] = 'u';
    gASCII_table[42] = 'v';
    gASCII_table[43] = 'w';
    gASCII_table[44] = 'x';
    gASCII_table[45] = 'y';
    gASCII_table[46] = 'z';
    gASCII_table[48] = '-';
    gASCII_table[49] = '=';
    gASCII_table[50] = 0x7f;
    gASCII_table[51] = '\r';
    gASCII_table[52] = '\r';
    gASCII_table[53] = '\t';
    gASCII_table[58] = '.';
    gASCII_table[59] = ',';
    gASCII_table[63] = 0x1b;
    gASCII_table[106] = ' ';

    memcpy(gASCII_shift_table, gASCII_table, sizeof(gASCII_table));

    gASCII_shift_table[21] = 'A';
    gASCII_shift_table[22] = 'B';
    gASCII_shift_table[23] = 'C';
    gASCII_shift_table[24] = 'D';
    gASCII_shift_table[25] = 'E';
    gASCII_shift_table[26] = 'F';
    gASCII_shift_table[27] = 'G';
    gASCII_shift_table[28] = 'H';
    gASCII_shift_table[29] = 'I';
#if defined(REC2_FIX_BUGS)
    gASCII_shift_table[30] = 'J';
#endif
    gASCII_shift_table[31] = 'K';
    gASCII_shift_table[32] = 'L';
    gASCII_shift_table[33] = 'M';
    gASCII_shift_table[34] = 'N';
    gASCII_shift_table[35] = 'O';
    gASCII_shift_table[36] = 'P';
    gASCII_shift_table[37] = 'Q';
    gASCII_shift_table[38] = 'R';
    gASCII_shift_table[39] = 'S';
    gASCII_shift_table[40] = 'T';
    gASCII_shift_table[41] = 'U';
    gASCII_shift_table[42] = 'V';
    gASCII_shift_table[43] = 'W';
    gASCII_shift_table[44] = 'X';
    gASCII_shift_table[45] = 'Y';
    gASCII_shift_table[46] = 'Z';

    gScan_code[12] = 0xff;
    gScan_code[13] = 0xff;
    gScan_code[14] = 0xff;
    gScan_code[15] = 0xff;
    gScan_code[16] = DIK_CAPITAL;
    gScan_code[17] = DIK_RSHIFT;
    gScan_code[18] = DIK_RMENU;
    gScan_code[19] = DIK_RCONTROL;
    gScan_code[20] = DIK_LSHIFT;
    gScan_code[21] = DIK_LMENU;
    gScan_code[22] = DIK_LCONTROL;
    gScan_code[23] = DIK_0;
    gScan_code[24] = DIK_1;
    gScan_code[25] = DIK_2;
    gScan_code[26] = DIK_3;
    gScan_code[27] = DIK_4;
    gScan_code[28] = DIK_5;
    gScan_code[29] = DIK_6;
    gScan_code[30] = DIK_7;
    gScan_code[31] = DIK_8;
    gScan_code[32] = DIK_9;
    gScan_code[33] = DIK_A;
    gScan_code[34] = DIK_B;
    gScan_code[35] = DIK_C;
    gScan_code[36] = DIK_D;
    gScan_code[37] = DIK_E;
    gScan_code[38] = DIK_F;
    gScan_code[39] = DIK_G;
    gScan_code[40] = DIK_H;
    gScan_code[41] = DIK_I;
    gScan_code[42] = DIK_J;
    gScan_code[43] = DIK_K;
    gScan_code[44] = DIK_L;
    gScan_code[45] = DIK_M;
    gScan_code[46] = DIK_N;
    gScan_code[47] = DIK_O;
    gScan_code[48] = DIK_P;
    gScan_code[49] = DIK_Q;
    gScan_code[50] = DIK_R;
    gScan_code[51] = DIK_S;
    gScan_code[52] = DIK_T;
    gScan_code[53] = DIK_U;
    gScan_code[54] = DIK_V;
    gScan_code[55] = DIK_W;
    gScan_code[56] = DIK_X;
    gScan_code[57] = DIK_Y;
    gScan_code[58] = DIK_Z;
    gScan_code[59] = DIK_GRAVE;
    gScan_code[60] = DIK_MINUS;
    gScan_code[61] = DIK_EQUALS;
    gScan_code[62] = DIK_BACK;
    gScan_code[63] = DIK_RETURN;
    gScan_code[64] = DIK_NUMPADENTER;
    gScan_code[65] = DIK_TAB;
    gScan_code[66] = DIK_SLASH;
    gScan_code[67] = DIK_BACKSLASH;
    gScan_code[68] = DIK_SEMICOLON;
    gScan_code[69] = DIK_APOSTROPHE;
    gScan_code[70] = DIK_PERIOD;
    gScan_code[71] = DIK_COMMA;
    gScan_code[72] = DIK_LBRACKET;
    gScan_code[73] = DIK_RBRACKET;
    gScan_code[74] = 0;
    gScan_code[75] = DIK_ESCAPE;
    gScan_code[76] = DIK_INSERT;
    gScan_code[77] = DIK_DELETE;
    gScan_code[78] = DIK_HOME;
    gScan_code[79] = DIK_END;
    gScan_code[80] = DIK_PRIOR;
    gScan_code[81] = DIK_NEXT;
    gScan_code[82] = DIK_LEFT;
    gScan_code[83] = DIK_RIGHT;
    gScan_code[84] = DIK_UP;
    gScan_code[85] = DIK_DOWN;
    gScan_code[86] = DIK_NUMLOCK;
    gScan_code[87] = DIK_DIVIDE;
    gScan_code[88] = DIK_MULTIPLY;
    gScan_code[89] = DIK_SUBTRACT;
    gScan_code[90] = DIK_ADD;
    gScan_code[91] = DIK_DECIMAL;
    gScan_code[92] = 0;
    gScan_code[93] = DIK_NUMPAD0;
    gScan_code[94] = DIK_NUMPAD1;
    gScan_code[95] = DIK_NUMPAD2;
    gScan_code[96] = DIK_NUMPAD3;
    gScan_code[97] = DIK_NUMPAD4;
    gScan_code[98] = DIK_NUMPAD5;
    gScan_code[99] = DIK_NUMPAD6;
    gScan_code[100] = DIK_NUMPAD7;
    gScan_code[101] = DIK_NUMPAD8;
    gScan_code[102] = DIK_NUMPAD9;
    gScan_code[103] = DIK_F1;
    gScan_code[104] = DIK_F2;
    gScan_code[105] = DIK_F3;
    gScan_code[106] = DIK_F4;
    gScan_code[107] = DIK_F5;
    gScan_code[108] = DIK_F6;
    gScan_code[109] = DIK_F7;
    gScan_code[110] = DIK_F8;
    gScan_code[111] = DIK_F9;
    gScan_code[112] = DIK_F10;
    gScan_code[113] = DIK_F11;
    gScan_code[114] = DIK_F12;
    gScan_code[115] = DIK_SYSRQ;
    gScan_code[116] = DIK_SCROLL;
    gScan_code[117] = 0xff;
    gScan_code[118] = DIK_SPACE;
}

// FUNCTION: CARMA2_HW 0x0051cbf0
void C2_HOOK_FASTCALL Win32InitInputDevice(void) {
    HRESULT hRes;
    GUID guid_sysKeyboard;

    hRes = DirectInputCreateA(gHInstance, DIRECTINPUT_VERSION, &gDirectInput, NULL);
    if (hRes != S_OK) {
        PDFatalError("Unable to create DirectInput object - please check that DirectX is installed");
    }

    guid_sysKeyboard = GUID_SysKeyboard;
    hRes = IDirectInput_CreateDevice(gDirectInput, &guid_sysKeyboard, &gDirectInputDevice, NULL);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetDataFormat(gDirectInputDevice, &c_dfDIKeyboard);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't create device");
    }

    hRes = IDirectInputDevice_SetCooperativeLevel(gDirectInputDevice, gHWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (hRes != S_OK) {
        PDFatalError("Direct Input: Can't set keyboard cooperative level");
    }

    hRes = IDirectInputDevice_Acquire(gDirectInputDevice);
    if (!SUCCEEDED(hRes)) {
        dr_dprintf("ERROR: Can't aquire keyboard; HRESULT %x", hRes);
    }
    if (JoystickDInputBegin()) {
        CollectJoystickButtonInfos();
        LoadJoystickPreferences();
    }
    gJoystick_deadzone = 8000;
}

// FUNCTION: CARMA2_HW 0x00459690
tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void) {
    IDirectInputDevice2 *device;
    HRESULT res;

    if (gJoystick_index == -1) {
        return 0;
    }
    device = gDirectInputJoystickDevices[gJoystick_index];
    if (device == NULL) {
        return 0;
    }
    res = IDirectInputDevice2_Poll(device);
    if (FAILED(res)) {
        switch (res) {
        case DIERR_INPUTLOST:
        case DIERR_NOTACQUIRED:
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
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
                AcquireDInputJoystickDevice(gJoystick_index);
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

// FUNCTION: CARMA2_HW 0x0045b360
void C2_HOOK_FASTCALL PDSetKeysFromJoystick(int *keys) {
    if (gJoystick_index != 1) {
        tU32 button_mask;

        button_mask = PDGetJoystickButtonStates();
        if (PDGetCurrentJoystickData() != NULL) {
            int i;

            for (i = 0; i < 32; i++) {
                keys[107 + i] = !!(button_mask & (i << i));
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051cef0
void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark) {
    BYTE diKeys[256];
    HRESULT res;
    int i;

    gKeys_pressed = 0;
    Win32ServiceMessages();
    if (gWindowActiveState != 2) {
        *pKeys = 0;
        return;
    }
    if (gTime_app_activated != 0) {
        if (PDGetTotalTime() < gTime_app_activated + 1000) {
            *pKeys = 0;
            return;
        }
    }
    gTime_app_activated = 0;
    res = IDirectInputDevice_GetDeviceState(gDirectInputDevice, sizeof(diKeys), diKeys);
    if (res != DI_OK) {
        dr_dprintf("ACTIVE but couldn't get keyboard device state on 1st attempt");
        dr_dprintf("Keyboard input lost; reacquiring...");
        if (gDirectInputDevice != NULL) {
            IDirectInputDevice_Acquire(gDirectInputDevice);
        }
        res = IDirectInputDevice_GetDeviceState(gDirectInputDevice, sizeof(diKeys), diKeys);
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

        scan_code = gScan_code[12 + i];
        if (scan_code == 0) {
            continue;
        }
        if (scan_code == 0xff) {
            switch (i) {
            case 0:
                if ((diKeys[gScan_code[20]] & 0x80) || (diKeys[gScan_code[17]] & 0x80)) {
                    pKeys[0] = pMark;
                } else if (pKeys[0] == pMark) {
                    pKeys[0] = 0;
                }
                break;
            case 1:
                if ((diKeys[gScan_code[21]] & 0x80) || (diKeys[gScan_code[18]] & 0x80)) {
                    pKeys[1] = pMark;
                } else if (pKeys[1] == pMark) {
                    pKeys[1] = 0;
                }
                break;
            case 2:
            case 3:
                if ((diKeys[gScan_code[22]] & 0x80) || (diKeys[gScan_code[19]] & 0x80)) {
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
                gKeys_pressed = gKeys_pressed << 8 | (i + 1);
            } else if (pKeys[i] == pMark) {
                pKeys[i] = 0;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051d1d0
void C2_HOOK_FASTCALL PDReadJoysticks(void) {

    if (gJoystick_index != -1) {
        gJoy1_valid = 1;
        return;
    }
    if (gJoy1_valid) {
        if (joyGetPosEx(0, &gJoy1_info) != JOYERR_NOERROR) {
            gJoy1_valid = 0;
        }
    }
    if (gJoy2_valid) {
        if (joyGetPosEx(1, &gJoy2_info) != JOYERR_NOERROR) {
            gJoy2_valid = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00459050
int C2_HOOK_FASTCALL GetDirectInputJoy1X(void) {
    IDirectInputDevice2A* device;
    HRESULT res;
    DIJOYSTATE joy_state;
    int x;

    if (gJoystick_index == -1) {
        return -1;
    }
    device = gDirectInputJoystickDevices[gJoystick_index];
    if (device == NULL) {
        return -1;
    }
    if (!gINT_00595f98) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(device);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(device, sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }
    x = (int)((float)(joy_state.lX - 0x7fff) * gJoystick_x_steering + (float)0x7fff);
    if (x > 0xffff) {
        x = 0xffff;
    }
    if (x < 0) {
        x = 0;
    }
    return x;
}

// FUNCTION: CARMA2_HW 0x00459370
int C2_HOOK_FASTCALL GetDirectInputJoy1Y(void) {
    IDirectInputDevice2A* device;
    HRESULT res;
    DIJOYSTATE joy_state;
    int y;

    if (gJoystick_index == -1) {
        return -1;
    }
    device = gDirectInputJoystickDevices[gJoystick_index];
    if (device == NULL) {
        return -1;
    }
    if (!gINT_00595f9c) {
        return -1;
    }
    res = IDirectInputDevice2_Poll(device);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }

    res = IDirectInputDevice_GetDeviceState(device, sizeof(joy_state), &joy_state);
    if (res != DI_OK) {
        if (res == DIERR_INPUTLOST) {
            if (AcquireDInputJoystickDevice(gJoystick_index) == 0) {
                dr_dprintf("couldn't reacquire joystick %d ", gJoystick_index);
            }
        } else {
            dr_dprintf("Can't poll, unknown error\n");
        }
        return -1;
    }
    y = (int)((float)(joy_state.lY - 0x7fff) * gJoystick_y_throttle + (float)0x7fff);
    if (y > 0xffff) {
        y = 0xffff;
    }
    if (y < 0) {
        y = 0;
    }
    return y;
}

// FUNCTION: CARMA2_HW 0x0051d2c0
tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void) {

    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 0);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d2e0
tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void) {

    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d300
tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void) {

    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 2);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d320
tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void) {

    if (gJoy1_valid) {
        return gJoy1_info.dwButtons & (1 << 3);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d230
int C2_HOOK_FASTCALL PDGetJoy1X(void) {

    if (gJoystick_index != -1) {
        return GetDirectInputJoy1X();
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d250
int C2_HOOK_FASTCALL PDGetJoy1Y(void) {

    if (gJoystick_index != -1) {
        return GetDirectInputJoy1Y();
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d340
tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void) {

    if (gJoy1_valid && gUINT_006ad094 >= 5  ) {
        return gJoy1_info.dwButtons & (1 << 4);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 0);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d370
tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void) {

    if (gJoy1_valid && gUINT_006ad094 >= 6) {
        return gJoy1_info.dwButtons & (1 << 5);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d3a0
tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void) {

    if (gJoy1_valid && gUINT_006ad094 >= 7) {
        return gJoy1_info.dwButtons & (1 << 6);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 2);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d3d0
tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void) {

    if (gJoy1_valid && gUINT_006ad094 >= 8) {
        return gJoy1_info.dwButtons & (1 << 7);
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwButtons & (1 << 3);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051d270
int C2_HOOK_FASTCALL PDGetJoy2X(void) {

    if (gJoy1_valid && (gINT_006ad0b8 & 0x2)) {
        return gJoy1_info.dwRpos;
    }
    if (gJoy2_valid) {
        return gJoy2_info.dwXpos;
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0051d2a0
int C2_HOOK_FASTCALL PDGetJoy2Y(void) {

    if (gJoy2_valid) {
        return gJoy2_info.dwYpos;
    }
    return -1;
}

void C2_HOOK_FASTCALL PDInitJoysticks(void) {

    if (gCountEnumeratedJoystickDinputDevices == 0) {
        JoystickDInputBegin();
    }

    ResetForceFeedback();
}

void C2_HOOK_FASTCALL PDPlayFFBEffectIndex(int index) {
    IDirectInputEffect* effect;

    effect = gDirectInputEffects[index];
    if (effect != NULL) {
        IDirectInputEffect_Start(effect, 1, DIES_NODOWNLOAD);
    }
    return;
}

// FUNCTION: CARMA2_HW 0x004580e0
int C2_HOOK_FASTCALL PDFindJoystickEffect(const char* effectName) {
    int i;

    for (i = 0; i < gCount_joystick_effects; i++) {

        if (strcmp(effectName, gJoystick_effects[i].name) == 0) {
            return i;
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0045c720
void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName) {

    if (gForceFeedbackAvailable) {
        int index = PDFindJoystickEffect(effectName);
        if (index != -1) {
            PDPlayFFBEffectIndex(index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c550
int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void) {

    tButtonJoystickInfo* joystick_info;

    joystick_info = PDGetCurrentJoystickData();
    if (joystick_info == NULL) {
        return 0;
    }
    return joystick_info->field_0xe0;
}
