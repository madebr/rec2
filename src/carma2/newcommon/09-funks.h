/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_09_FUNKS_H
#define GUARD_09_FUNKS_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include "brender/brender.h"

extern tFunkotronic_spec* gFunkotronics_array;
extern tGroovidelic_spec* gGroovidelics_array;
extern int gGroovidelics_array_size;
extern tU32 gPrevious_groove_times[2];
extern tGroove_funk_binding gGroove_funk_bindings[1440];
extern int gFunkotronics_array_size;

// CalculateTextureBitsValue

// MapSawToTriangle

extern void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

// AddFunkGrooveBinding

extern void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue);

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

extern void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner);

extern br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData);

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

extern void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);

// AddNewGroovidelic

// AddGroovidelics

extern void C2_HOOK_FASTCALL KillGroovadelic(int pOwner);

extern void C2_HOOK_FASTCALL KillFunkotronic(int pOwner);

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

extern void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it);

extern void C2_HOOK_FASTCALL GrooveThoseDelics(void);

extern void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor);

extern void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value);

extern void C2_HOOK_FASTCALL ResetGrooveFlags(void);

extern intptr_t C2_HOOK_CDECL DelGrooveRef(br_actor* pActor, void* pArg);

extern void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags);

extern tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor);

#endif // GUARD_09_FUNKS_H
