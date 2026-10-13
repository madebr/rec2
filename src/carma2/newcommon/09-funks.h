#ifndef GUARD_09_FUNKS_H
#define GUARD_09_FUNKS_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "brender/brender.h"

extern const char* gFunk_type_names[5];
extern const char* gFunk_anim_names[4];
extern const char* gTime_mode_names[2];
extern const char* gFunk_move_names[7];
extern const char* gFunk_nature_names[4];
extern const char* gGroove_nature_names[2];
extern const char* gLollipop_names[3];
extern const char* gGroove_path_names[2];
extern const char* gGroove_object_names[4];
extern const char* gCamera_animation_names[2];

extern tGroove_funk_binding gGroove_funk_bindings[1440];
extern tFunk_temp_buffer* gFunk_temp_vertices;

extern int gFunkotronics_array_size;
extern int gGroovidelics_array_size;
extern tFunkotronic_spec* gFunkotronics_array;
extern tGroovidelic_spec* gGroovidelics_array;
extern br_actor* gGroove_by_proxy_actor;

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

extern br_material* C2_HOOK_FASTCALL TryThisFunkLink(tCar_crush_buffer_entry* pFunk_link, const char* pStr, tFunkotronic_spec* pFunk);

extern br_material* C2_HOOK_FASTCALL FindSmashableMaterial(const char* pStr, tFunkotronic_spec* pFunk, tCar_crush_buffer* pCar_crush_datas);

extern void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);

extern void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);

extern tGroovidelic_spec* C2_HOOK_FASTCALL AddNewGroovidelic(void);

extern void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);

// KillGroovadelic

// KillFunkotronic

// NormaliseDegreeAngle

// FunkThoseTronics

// PathGrooveBastard

// ObjectGrooveBastard

// GrooveThisDelic

// GrooveThoseDelics

// StopGroovidelic

extern void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags);

extern void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value);

extern void C2_HOOK_FASTCALL ResetGrooveFlags(void);

// DisableFunkotronic

// EnableFunkotronic

// MasterDisableFunkotronic

// MasterEnableFunkotronic

// AdjustFunkEnable

extern tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor);

#endif // GUARD_09_FUNKS_H
