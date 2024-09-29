#ifndef REC2_INTRFACE_H
#define REC2_INTRFACE_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gAlways_typing);
C2_HOOK_VARIABLE_DECLARE(int, gDisabled_count);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gDisabled_choices, 10);
C2_HOOK_VARIABLE_DECLARE(tU32, gStart_time);
C2_HOOK_VARIABLE_DECLARE(const tInterface_spec*, gSpec);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_mode);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_choice);

void C2_HOOK_FASTCALL OriginalResetInterfaceTimeout(void);

int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice);

void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled);

#endif //REC2_INTRFACE_H
