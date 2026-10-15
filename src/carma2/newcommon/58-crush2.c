#include "58-crush2.h"

#include "rec2_macros.h"

#include "brender/brender.h"

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

// DoDamage

// FUNCTION: CARMA2_HW 0x0043f9f0
void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].last_level = pCar->damage_units[i].damage_level;
    }
    pCar->damage_magnitude_accumulator = 0.f;
    pCar->last_impact_location = eImpact_unknown;
    pCar->pre_car_col_mat = pCar->car_master_actor->t.t.mat;
    pCar->pre_car_col_knackered = pCar->knackered;
    pCar->pre_car_col_speed = pCar->speed;
    BrVector3Copy(&pCar->pre_car_col_direction, &pCar->direction);
    BrVector3Copy(&pCar->pre_car_col_velocity, &pCar->collision_info->v);
    BrVector3Copy(&pCar->pre_car_col_velocity_car_space, &pCar->collision_info->velocity_car_space);
}

// CalcModifiedLocation
tImpact_location C2_HOOK_FASTCALL CalcModifiedLocation(tCar_spec* pCar) {
    tImpact_location location;

    location = pCar->last_impact_location;
    if (location == eImpact_left || location == eImpact_right
            || location == eImpact_top || location == eImpact_bottom) {
        if (!(pCar->field_0x13e0 >= 0.25)) {
            return eImpact_front;
        }
        if (pCar->field_0x13e0 > 0.75) {
            return eImpact_back;
        }
    }
    return location;
}

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

// FUNCTION: CARMA2_HW 0x00441350
int C2_HOOK_FASTCALL PercentageCarHealth(tCar_spec* pCar) {
    int engine;
    int transmission;
    int driver;
    int wheels;
    int health;

    if (pCar->knackered) {
        return 0;
    }

    wheels = (pCar->damage_units[eDamage_lr_wheel].damage_level
        + pCar->damage_units[eDamage_rr_wheel].damage_level
        + pCar->damage_units[eDamage_lf_wheel].damage_level
        + pCar->damage_units[eDamage_rf_wheel].damage_level) / 4;
    engine = pCar->damage_units[eDamage_engine].damage_level;
    transmission = pCar->damage_units[eDamage_transmission].damage_level;
    driver = pCar->damage_units[eDamage_driver].damage_level;

    if (engine <= 75 && transmission <= 75 && driver <= 75 && wheels <= 75) {
        health = 100 - (wheels + driver + transmission + engine) / 4;
        if (health < 25) {
            health = 25;
        }
        return health;
    }
    if (engine > transmission && engine > driver && engine > wheels) {
        return 100 - engine;
    }
    if (transmission > driver && transmission > wheels) {
        return 100 - transmission;
    }
    if (driver > wheels) {
        return 100 - driver;
    }
    return 100 - wheels;
}

// ChristmasLights

