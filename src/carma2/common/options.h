#ifndef REC2_OPTIONS_H
#define REC2_OPTIONS_H

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gKey_names, 153);
C2_HOOK_VARIABLE_DECLARE(int, gOrig_key_map_index);


void C2_HOOK_FASTCALL DoOptions(void);

void C2_HOOK_FASTCALL StripControls(char* pStr);

void C2_HOOK_FASTCALL LoadKeyNames(void);

void C2_HOOK_FASTCALL DisposeKeyNames(void);

void C2_HOOK_FASTCALL BackupKeyMappings(void);

void C2_HOOK_FASTCALL Joystick_BackupSettings(void);

#endif //REC2_OPTIONS_H
