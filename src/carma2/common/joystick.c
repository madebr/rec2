#include "joystick.h"

#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, gForce_feedback_upper_limit, 0x00655e58, 2.7f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(float, Force_feedback_lower_limit, 0x00655e5c, 1.5f);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gJoystick_index, 0x00595f88, -1);
C2_HOOK_VARIABLE_IMPLEMENT(float, gOriginal_joystick_x, 0x00688704);
C2_HOOK_VARIABLE_IMPLEMENT(float, gOriginal_joystick_y, 0x00688708);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOriginal_joystick_fbb, 0x0068870c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOriginal_joystick_dpad, 0x00688710);
C2_HOOK_VARIABLE_IMPLEMENT(int, gOrig_joystick_index, 0x00688700);


void C2_HOOK_FASTCALL SetupFFBValues(void) {
    tPath_name path;
    FILE *f;
    char s[256];

    PathCat(path, C2V(gApplication_path), "FFB.TXT");
    f = DRfopen(path, "rt");
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(gForce_feedback_upper_limit));
    GetALineAndDontArgue(f, s);
    c2_sscanf(c2_strtok(s, "\t ,/"), "%f", &C2V(Force_feedback_lower_limit));
    PFfclose(f);
}

int (C2_HOOK_FASTCALL * PlayFFBEffect_original)(const char* pEffect_name, int pArg2);
int C2_HOOK_FASTCALL PlayFFBEffect(const char* pEffect_name, int pArg2) {

#if defined(C2_HOOKS_ENABLED)
    return PlayFFBEffect_original(pEffect_name, pArg2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c6b0, PlayFFBEffect, PlayFFBEffect_original)

void (C2_HOOK_FASTCALL * StopJoysticks_original)(void);
void C2_HOOK_FASTCALL StopJoysticks(void) {

#if defined(C2_HOOKS_ENABLED)
    StopJoysticks_original();
#else
#error "Notimplemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004575b0, StopJoysticks, StopJoysticks_original)

float (C2_HOOK_FASTCALL * GetJoystickX_original)(void);
float C2_HOOK_FASTCALL GetJoystickX(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickX_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c8d0, GetJoystickX, GetJoystickX_original)

float (C2_HOOK_FASTCALL * GetJoystickY_original)(void);
float C2_HOOK_FASTCALL GetJoystickY(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickY_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c8f0, GetJoystickY, GetJoystickY_original)

int (C2_HOOK_FASTCALL * GetJoystickFBBGain_original)(void);
int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {

#if defined(C2_HOOKS_ENABLED)
    return GetJoystickFBBGain_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c7f0, GetJoystickFBBGain, GetJoystickFBBGain_original)

int (C2_HOOK_FASTCALL * IsJoystickDPadEnabled_original)(void);
int C2_HOOK_FASTCALL IsJoystickDPadEnabled(void) {

#if defined(C2_HOOKS_ENABLED)
    return IsJoystickDPadEnabled_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c550, IsJoystickDPadEnabled, IsJoystickDPadEnabled_original)

void (C2_HOOK_FASTCALL * SetJoystickX_original)(float pValue);
void C2_HOOK_FASTCALL SetJoystickX(float pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickX_original(pValue);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c810, SetJoystickX, SetJoystickX_original)

void (C2_HOOK_FASTCALL * SetJoystickY_original)(float pValue);
void C2_HOOK_FASTCALL SetJoystickY(float pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickY_original(pValue);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c870, SetJoystickY, SetJoystickY_original)

void (C2_HOOK_FASTCALL * SetJoystickFFBGain_original)(int pValue);
void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickFFBGain_original(pValue);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c7b0, SetJoystickFFBGain, SetJoystickFFBGain_original)

void (C2_HOOK_FASTCALL * SetJoystickDPadEnabled_original)(int pEnabled);
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {

#if defined(C2_HOOKS_ENABLED)
    SetJoystickDPadEnabled_original(pEnabled);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045c570, SetJoystickDPadEnabled, SetJoystickDPadEnabled_original)

void C2_HOOK_FASTCALL Joystick_BackupSettings(void) {

    C2V(gOrig_joystick_index) = C2V(gJoystick_index);
    C2V(gOriginal_joystick_x) = GetJoystickX();
    C2V(gOriginal_joystick_y) = GetJoystickY();
    C2V(gOriginal_joystick_fbb) = GetJoystickFBBGain();
    C2V(gOriginal_joystick_dpad) = IsJoystickDPadEnabled();
    C2V(gJoystick_index) = C2V(gOrig_joystick_index);
}

void (C2_HOOK_FASTCALL * EnableJoysticks_original)(void);
void C2_HOOK_FASTCALL EnableJoysticks(void) {

#if defined(C2_HOOKS_ENABLED)
    EnableJoysticks_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045bd50, EnableJoysticks, EnableJoysticks_original)

void (C2_HOOK_FASTCALL * DisableJoysticks_original)(void);
void C2_HOOK_FASTCALL DisableJoysticks(void) {

#if defined(C2_HOOKS_ENABLED)
    DisableJoysticks_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045bd70, DisableJoysticks, DisableJoysticks_original)

void (C2_HOOK_FASTCALL * MungeJoystickHeadups_original)(void);
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeJoystickHeadups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045b3b0, MungeJoystickHeadups, MungeJoystickHeadups_original)
