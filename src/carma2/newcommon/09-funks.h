#ifndef GUARD_09_FUNKS_H
#define GUARD_09_FUNKS_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "brender/brender.h"

extern tGroove_funk_binding gGroove_funk_bindings[1440];
extern tFunk_temp_buffer* gFunk_temp_vertices;

extern int gFunkotronics_array_size;
extern int gGroovidelics_array_size;
extern tFunkotronic_spec* gFunkotronics_array;
extern tGroovidelic_spec* gGroovidelics_array;

// CalculateTextureBitsValue

// MapSawToTriangle

extern void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

extern void C2_HOOK_FASTCALL AddFunkGrooveBinding(int pSlot_number, float* pPeriod_address);

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

extern void C2_HOOK_FASTCALL ShiftBoundGrooveFunks(char* pStart, char* pEnd, ptrdiff_t pDelta);

extern tFunkotronic_spec* C2_HOOK_FASTCALL AddNewFunkotronic(void);

extern void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner);

extern br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData);

extern br_uint_32 C2_HOOK_FASTCALL AddProximities(br_actor* pActor, br_material* pMat, void* pData);

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

extern void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);

extern tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void);

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

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// AdjustFunkEnable

extern tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor);

#endif // GUARD_09_FUNKS_H
