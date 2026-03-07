#ifndef REC2_INTRFACE_H
#define REC2_INTRFACE_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gAlways_typing;
extern int gDisabled_count;
extern int gDisabled_choices[10];
extern tU32 gStart_time;
extern const tInterface_spec* gSpec;
extern int gCurrent_mode;
extern int gCurrent_choice;

void C2_HOOK_FASTCALL OriginalResetInterfaceTimeout(void);

int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice);

void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled);

void C2_HOOK_FASTCALL ChangeSelectionTo(int pNew_choice, int pNew_mode);

void C2_HOOK_FASTCALL SetAlwaysTyping(void);

#endif //REC2_INTRFACE_H