#ifndef C2_WIN32_INPUT_H
#define C2_WIN32_INPUT_H

#include "c2_hooks.h"

#include "platform.h"
#include "win32_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gASCII_table, 151);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gASCII_shift_table, 151);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gScan_code, 256);

void C2_HOOK_FASTCALL KeyBegin(void);

void C2_HOOK_FASTCALL Win32InitInputDevice(void);

void C2_HOOK_FASTCALL CloseDirectInput(void);

int C2_HOOK_FASTCALL InitForceFeedback(void);

void C2_HOOK_FASTCALL KeyBegin(void);

int C2_HOOK_FASTCALL JoystickDInputBegin(void);

int C2_HOOK_FASTCALL InitForceFeedback(void);

void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex);

void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void);

void C2_HOOK_FASTCALL Win32InitInputDevice(void);

int C2_HOOK_FASTCALL JoystickDInputBegin(void);

tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void);

void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark);

void C2_HOOK_FASTCALL PDReadJoysticks(void);

int C2_HOOK_FASTCALL GetDirectInputJoy1X(void);

int C2_HOOK_FASTCALL GetDirectInputJoy1Y(void);

tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void);

tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void);

tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void);

tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void);

int C2_HOOK_FASTCALL PDGetJoy1X(void);

int C2_HOOK_FASTCALL PDGetJoy1Y(void);

tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void);

tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void);

tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void);

tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void);

int C2_HOOK_FASTCALL PDGetJoy2X(void);

#endif // C2_WIN32_INPUT_H
