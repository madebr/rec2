#include "58-crush2.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0074d3b0
int gCredits_wasting_car[3];

// GLOBAL: CARMA2_HW 0x0075b930
float gCredits_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x0075b9b0
int gTime_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761a50
int gCredits_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761ed0
float gSeconds_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x00763520
int gTime_wasting_car[3];

// GLOBAL: CARMA2_HW 0x00763530
int gTime_per_ped_kill[3];

// CheckLastCar

// KnackerThisCar

// ExplodeMeCarUpMeOldMatey

// SetKnackeredFlag

// DamageUnit2

// RecordLastDamage

// DoDamage

// CalcModifiedLocation

// DoPratcamHit

// DamageSystems

// GetDirection

// FUNCTION: CARMA2_HW 0x00440210
void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

// SortOutSmoke

// DoCrashEarnings

// DoWheelDamage

// CrashEarnings

// PercentageCarHealth

// ChristmasLights

