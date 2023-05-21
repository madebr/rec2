#ifndef C2_WIN32_INPUT_H
#define C2_WIN32_INPUT_H

#include "c2_hooks.h"

#include "platform.h"
#include "win32_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gASCII_table, 151);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gASCII_shift_table, 151);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gScan_code, 151);

void C2_HOOK_FASTCALL KeyBegin(void);

void C2_HOOK_FASTCALL Win32InitInputDevice(void);

#endif // C2_WIN32_INPUT_H
