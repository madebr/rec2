#ifndef GUARD_35_INTRFACE_H
#define GUARD_35_INTRFACE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern void C2_HOOK_FASTCALL SetAlwaysTyping(void);

extern void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

extern int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice);

extern void C2_HOOK_FASTCALL ResetInterfaceTimeout__intrface(void);

extern void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled);

// RecopyAreas

extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);

extern void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);

#endif // GUARD_35_INTRFACE_H
