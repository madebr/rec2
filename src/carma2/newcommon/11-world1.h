#ifndef GUARD_11_WORLD1_H
#define GUARD_11_WORLD1_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gDelete_count;
extern br_actor* gDelete_list[500];

extern br_uint_32 C2_HOOK_FASTCALL DeleteBastards(br_actor* actor, br_matrix34* matrix, void* data);

// DeleteAnyZeroBastards

// ApplyTransToModels

// FindSpecVolIndex

extern void C2_HOOK_FASTCALL MungeMaterialSV(br_matrix34* matrix, br_material* material1, br_material* material2, int axis0, int axis1);

// SetSpecVolMatSize

// UpdateSpecVol

// PutAdditionalNonCarsOnToAdditionalActors

// PutAdditionalNonCarsOnToNonTrackActor

// SaveAdditionalActors

// DoSaveAdditionalStuff

extern void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);

#endif // GUARD_11_WORLD1_H
