#ifndef GUARD_37_BRUCETRK_H
#define GUARD_37_BRUCETRK_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern br_material* gDefault_track_material;

// AssertNoncars

// AllocateActorMatrix

// DisposeActorMatrix

// DisposeRuntimeBuiltModels

// DisposeColumns

extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// RenderTrack

extern br_scalar C2_HOOK_STDCALL GetYonFactor(void);

extern void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

// FoundAnActor

// ProcessNearbyActors

#endif // GUARD_37_BRUCETRK_H
