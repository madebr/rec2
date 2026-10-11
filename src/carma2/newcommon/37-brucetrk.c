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

// FUNCTION: CARMA2_HW 0x0040cb90
void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec) {
    double column_x;
    double column_z;
    volatile br_scalar rounded_x;

    column_x = ((double)pX - pTrack_spec->origin_x) / pTrack_spec->column_size_x;
    column_z = ((double)pZ - pTrack_spec->origin_z) / pTrack_spec->column_size_z;
    /* Only X is spilled to a scalar before the original upper clamp and conversion. */
    rounded_x = (br_scalar)column_x;
    if (!(column_x >= 0.0)) {
        rounded_x = 0.0f;
    }
    if (rounded_x >= pTrack_spec->ncolumns_x) {
        rounded_x = pTrack_spec->ncolumns_x - 1.0f;
    }
    if (!(column_z >= 0.0)) {
        column_z = 0.0;
    }
    if (column_z >= pTrack_spec->ncolumns_z) {
        column_z = pTrack_spec->ncolumns_z - 1.0;
    }
    *pColumn_x = (tU8)(int)rounded_x;
    *pColumn_z = (tU8)(int)column_z;
}

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// RenderTrack

// FUNCTION: CARMA2_HW 0x0040dfe0
br_scalar C2_HOOK_STDCALL GetYonFactor(void) {
    return gYon_factor;
}

// FUNCTION: CARMA2_HW 0x0040dff0
void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    gYon_factor = pNew;
}

// FoundAnActor

// ProcessNearbyActors

