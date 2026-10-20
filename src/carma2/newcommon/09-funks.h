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

// KillFunkotronic

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

extern void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it);

// GrooveThoseDelics

extern void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor);

extern void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value);

extern void C2_HOOK_FASTCALL ResetGrooveFlags(void);

extern void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags);

extern tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor);

#endif // GUARD_09_FUNKS_H
