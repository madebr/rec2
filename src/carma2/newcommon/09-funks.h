#ifndef GUARD_09_FUNKS_H
#define GUARD_09_FUNKS_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include "brender/brender.h"

extern tFunkotronic_spec* gFunkotronics_array;
extern tGroovidelic_spec* gGroovidelics_array;
extern int gGroovidelics_array_size;

// CalculateTextureBitsValue

// MapSawToTriangle

extern void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

// AddFunkGrooveBinding

// ControlBoundFunkGroove

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

// KillGroovadelic

// KillFunkotronic

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

// DisableGroovidelic

// EnableGroovidelic

// SetGrooveInterrupt

// ResetGrooveFlags

extern void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex);

extern void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags);

// ActorsGroove

#endif // GUARD_09_FUNKS_H
