#ifndef GUARD_58_CRUSH2_H
#define GUARD_58_CRUSH2_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCredits_wasting_car[3];
extern float gCredits_per_unit_car_damage[3];
extern int gTime_rolling_car[3];
extern int gCredits_rolling_car[3];
extern float gSeconds_per_unit_car_damage[3];
extern int gTime_wasting_car[3];
extern int gTime_per_ped_kill[3];

// CheckLastCar

// KnackerThisCar

// ExplodeMeCarUpMeOldMatey

// SetKnackeredFlag

// DamageUnit2

// DoDamage

extern void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar);

// CalcModifiedLocation

// DoPratcamHit

// DamageSystems

// GetDirection

extern void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);

// SortOutSmoke

// DoCrashEarnings

// DoWheelDamage

// CrashEarnings

extern int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar);

// ChristmasLights

#endif // GUARD_58_CRUSH2_H
