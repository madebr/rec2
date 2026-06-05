#include "37-brucetrk.h"

// GLOBAL: CARMA2_HW 0x00655e60
br_scalar gYon_factor = 0.25f;

// GLOBAL: CARMA2_HW 0x0074b74c
br_material* gDefault_track_material;

// AssertNoncars

// AllocateActorMatrix

// DisposeActorMatrix

// DisposeRuntimeBuiltModels

// DisposeColumns

// XZToColumnXZ

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// RenderTrack

// GetYonFactor

// FUNCTION: CARMA2_HW 0x0040dff0
void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    gYon_factor = pNew;
}

// FoundAnActor

// ProcessNearbyActors

