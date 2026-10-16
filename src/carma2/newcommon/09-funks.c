#include "09-funks.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0068b840
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b84c
tFunkotronic_spec* gFunkotronics_array;

// CalculateTextureBitsValue

// MapSawToTriangle

// FUNCTION: CARMA2_HW 0x00474880
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)REC2_SQR(pYon * 1.02);
}

// AddFunkGrooveBinding

// ControlBoundFunkGroove

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

// STUB: CARMA2_HW 0x00474950
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    NOT_IMPLEMENTED();
}

// CalcProximities

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

// STUB: CARMA2_HW 0x00476430
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    NOT_IMPLEMENTED();
}

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

// DisableFunkotronic

// EnableFunkotronic

// FUNCTION: CARMA2_HW 0x0047b2b0
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x2;
}

// AdjustFunkEnable

// ActorsGroove

