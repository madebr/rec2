/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_35_INTRFACE_H
#define GUARD_35_INTRFACE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gMouse_in_use;

extern void C2_HOOK_FASTCALL SetAlwaysTyping(void);

extern void C2_HOOK_FASTCALL ClearAlwaysTyping(void);

extern int C2_HOOK_FASTCALL ChoiceDisabled(int pChoice);

extern void C2_HOOK_FASTCALL ResetInterfaceTimeout__intrface(void);

extern void C2_HOOK_FASTCALL DisableChoice(int pChoice);

extern void C2_HOOK_FASTCALL EnableChoice(int pChoice);

extern void C2_HOOK_FASTCALL ChangeSelection(const tInterface_spec* pSpec, int* pOld_selection, int* pNew_selection, int pMode, int pSkip_disabled);

extern void C2_HOOK_FASTCALL RecopyAreas(const tInterface_spec* pSpec);

extern int C2_HOOK_FASTCALL DoInterfaceScreen(const tInterface_spec* pSpec, int pOptions, int pCurrent_choice);

extern void C2_HOOK_FASTCALL ChangeSelectionTo(int pNew_choice, int pNew_mode);

extern int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);

extern void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);

#endif // GUARD_35_INTRFACE_H
