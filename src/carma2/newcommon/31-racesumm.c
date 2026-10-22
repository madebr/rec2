#include "31-racesumm.h"

#include "30-opponent.h"

// SortScores

// SortGameScores

// DoNetRaceSummary

// DoEndRaceSummary

// FUNCTION: CARMA2_HW 0x004e33d0
int C2_HOOK_FASTCALL AnyBuyableCars(void) {
    int count;
    int i;

    count = GetCarCount(eVehicle_opponent);
    for (i = 0; i < count; i++) {
        if (GetCarSpec(eVehicle_opponent, i) != NULL) {
            return 1;
        }
    }
    return 0;
}

// RecordAPOAndCredits

// PickRaceWhenGameCompleted

// MungeRaceProgression

