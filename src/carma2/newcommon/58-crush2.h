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

// RecordLastDamage

// DoDamage

extern void C2_HOOK_FASTCALL CalcModifiedLocation(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity);

// DamageSystems

// GetDirection

extern void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);

// SortOutSmoke

// DoCrashEarnings

// DoWheelDamage

// CrashEarnings

// PercentageCarHealth

// ChristmasLights

#endif // GUARD_58_CRUSH2_H
