#include "07-structur.h"

#include "30-opponent.h"
#include "globvars.h"

// GLOBAL: CARMA2_HW 0x00655e48
int gHud_tinted1 = -1;

// GLOBAL: CARMA2_HW 0x00655e4c
int gHud_tinted2 = -1;

// GLOBAL: CARMA2_HW 0x00655e50
int gHud_tinted3 = -1;

// GLOBAL: CARMA2_HW 0x00762140
int gCredits_checkpoint[3];

// FUNCTION: CARMA2_HW 0x00503050
int C2_HOOK_FASTCALL NumberOfOpponentsLeft(void) {
    int result;
    int count;
    int i;
    tCar_spec* car_spec;

    result = 0;
    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        car_spec = GetCarSpec(eVehicle_opponent, i);

        if (!car_spec->knackered && (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || (gCurrent_race.race_spec->race_type == kRaceType_Cars && car_spec->is_race_goal))) {
            result += 1;
        }
    }
    return result;
}

// RaceCompleted

// Checkpoint

// IncrementCheckpoint

// IncrementLap

// RayHitFace

// WrongCheckpoint

// CheckCheckpoints

// TotalRepair

// DoLogos

// DoProgOpeningAnimation

// DoProgramDemo

// ChooseOpponent

// SelectOpponents

// PickNetRace

// SwapNetCarsLoad

// SwapNetCarsDispose

// DoGame

// InitialiseProgramState

// STUB: CARMA2_HW 0x00503c50
void C2_HOOK_FASTCALL DoProgram(void) {
    NOT_IMPLEMENTED();
}

// JumpTheStart

// GoingToInterfaceFromRace

// GoingBackToRaceFromInterface

