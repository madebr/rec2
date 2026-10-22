#ifndef GUARD_11_WORLD1_H
#define GUARD_11_WORLD1_H

#include "c2_hooks.h"
#include "rec2_types.h"

// DeleteBastards

// DeleteAnyZeroBastards

extern br_uint_32 C2_HOOK_FASTCALL ApplyTransToModels(br_actor* pActor, br_matrix34* pMatrix, void* pArg);

// FindSpecVolIndex

extern void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* pMat, br_material* pMat_1, br_material* pMat_2, int pAxis_0, int pAxis_1);

// SetSpecVolMatSize

// UpdateSpecVol

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

// SaveAdditionalActors

// DoSaveAdditionalStuff

extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);

#endif // GUARD_11_WORLD1_H
