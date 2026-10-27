/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "00-car.h"

#include "15-displays.h"
#include "34-finteray.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "58-crush2.h"
#include "brender/brender.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"

#include "c2_stdio.h"

// GLOBAL: CARMA2_HW 0x00763500
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
float gArmour_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761f5c
// GLOBAL: CARMA2_SW 0x007584bc
int gExplosion_sound_id;

// GLOBAL: CARMA2_HW 0x00761f60
float gPower_starting_value[100];

// GLOBAL: CARMA2_HW 0x00761d40
float gOffensive_starting_value[100];

// GLOBAL: CARMA2_HW 0x0075ba20
tFloat_bunch_info gCar_softness;

// GLOBAL: CARMA2_HW 0x0074d600
tFloat_bunch_info gCar_car_damage_multiplier;

// GLOBAL: CARMA2_HW 0x0068b8d0
br_vector3 gZero_v__car;  // FIXME: make const?

// GLOBAL: CARMA2_HW 0x006793dc
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x00679364
// GLOBAL: CARMA2_SW 0x00673ad4
int gInTheSea;

// GLOBAL: CARMA2_HW 0x0065d010
int gFace_num__car = 1;

// FUNCTION: CARMA2_HW 0x00413cc0
// FUNCTION: CARMA2_SW 0x00413cf0
void C2_HOOK_FASTCALL DamageUnit(tCar_spec* pCar, tDamage_type pUnit_type, int pDamage_amount) {
    tDamage_unit* the_damage;

    if (pDamage_amount <= 0) {
        return;
    }
    the_damage = &pCar->damage_units[pUnit_type];
    the_damage->damage_level += pDamage_amount;
    if (the_damage->damage_level >= 100) {
        the_damage->damage_level = 99;
    }
}

// FUNCTION: CARMA2_HW 0x00413d00
// FUNCTION: CARMA2_SW 0x00413d30
void C2_HOOK_FASTCALL DamageEngine(int pDamage_amount) {
    DamageUnit(&gProgram_state.current_car, eDamage_engine, pDamage_amount);
    SortOutSmoke(&gProgram_state.current_car);
}

// FUNCTION: CARMA2_HW 0x00413f10
intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {

    if (pActor->user != NULL) {
        int* index = pData;
        tUser_crush_data* crush_data = pActor->user;
        br_model* model = crush_data->models[*index];

        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00413f40
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &pIndex);
    pCar->field_0xe18 = pIndex;
}

// InitialiseCar2

// InitialiseCar

// InitialiseCarsEtc

// SetInitialPosition

// SetInitialPositions

// InitialiseNonCar

// NewFaceListCallBack

// FUNCTION: CARMA2_HW 0x00414ca0
// FUNCTION: CARMA2_SW 0x00414cd0
int C2_HOOK_FASTCALL IsCarInTheSea(void) {

    return gInTheSea;
}

// RememberSafePosition

// ControlNetCars

// ControlOurCar

// CalcEngineForce

// PrepareCars

// CalcGraphicalWheelStuff

// FinishCars

// GetNonCars

// GetNetPos

// MungeCarsMass

// AddDrag

// DragChildren

// DoCarStuff

// DoNonCarStuff

// RemoveFlyingCar

// RestoreFlyingCar

// GetDrivableOnList

// APTCPreCollision

// SetCollisionFlagsAndStuff

// APTCPostCollision

// APTCActiveHalted

// APTCPassiveActivated

// APTCChangedObjects

// ApplyPhysicsToCars

// MoveAndCollideCar

// MoveAndCollideNonCar

// FUNCTION: CARMA2_HW 0x00417180
// FUNCTION: CARMA2_SW 0x004171b0
void C2_HOOK_FAKE_THISCALL ControlCar2(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar time_steps;

    (void)pUnused;
    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed += time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed -= time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f;
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// FUNCTION: CARMA2_HW 0x004173b0
// FUNCTION: CARMA2_SW 0x004173e0
void C2_HOOK_FAKE_THISCALL ControlCar3(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar time_steps;

    (void)pUnused;
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f || pCar->collision_info->omega.v[1] < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f || pCar->collision_info->omega.v[1] > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 3.0);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.75);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
}

// ControlCar4

// FUNCTION: CARMA2_HW 0x00417a20
// FUNCTION: CARMA2_SW 0x00417a50
void C2_HOOK_FAKE_THISCALL ControlCar5(tCar_spec* pCar, void* pUnused, br_scalar pDt) {
    br_scalar time_steps;

    (void)pUnused;
    pCar->acc_force = 0.0f;
    if (pCar->keys.acc) {
        pCar->acc_force = 7.0f * pCar->collision_info->M;
    }
    if (pCar->keys.dec) {
        pCar->acc_force = -(7.0f * pCar->collision_info->M);
    }
    if (pCar->keys.left) {
        if (pCar->turn_speed < 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature < 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed + 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed + time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (pCar->keys.right) {
        if (pCar->turn_speed > 0.0f) {
            pCar->turn_speed = 0.0f;
        }
        if (pCar->curvature > 0.0f) {
            pCar->turn_speed = (float)(pCar->turn_speed - 0.01 * pDt / 0.04f / 2.0 * 0.5);
        } else {
            time_steps = pDt / 0.04f;
            pCar->turn_speed = (float)(pCar->turn_speed - time_steps * (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) / 2.0f * 0.5);
        }
    }
    if (!pCar->keys.left && !pCar->keys.right) {
        pCar->turn_speed = 0.0f;
        if (pCar->curvature < 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature + (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature > 0.0f) {
                pCar->curvature = 0.0f;
            }
        } else if (pCar->curvature > 0.0f && !pCar->keys.holdw) {
            time_steps = pDt / 0.04f;
            pCar->curvature = (float)(pCar->curvature - (0.05f / (5.0f + BrVector3Length(&pCar->collision_info->v))) * time_steps / 2.0f * 4.0);
            if (pCar->curvature < 0.0f) {
                pCar->curvature = 0.0f;
            }
        }
    }
    pCar->curvature += pCar->turn_speed;
    if (pCar->curvature > pCar->maxcurve) {
        pCar->curvature = pCar->maxcurve;
    }
    if (pCar->curvature < -pCar->maxcurve) {
        pCar->curvature = -pCar->maxcurve;
    }
    pCar->keys.left = 1;
}

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// FUNCTION: CARMA2_HW 0x00418230
// FUNCTION: CARMA2_SW 0x00418260
void C2_HOOK_FASTCALL MakeLiftGoUp(tNon_car_spec* pNon_car) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, flags, 0x100);

    if (gNet_mode != eNet_mode_client) {
        pNon_car->flags &= ~0xff;
        pNon_car->flags |= 0x1;
    }
}

// NonCarCalcForce

// DoBumpiness

// SmashFacesWithWheels

// ConditionallyNoteSkid

// NudgeObject

// CalcForce

// DoRevs

// ScrapeNoise

// SkidNoise

// StopSkid

// CrashNoise

// CrushAndDamageCar

// PointInFaceByQuiteABitActually

// DoEnvironmentSmashes

// ProcessForcesCallBack

// FUNCTION: CARMA2_HW 0x0041e310
// FUNCTION: CARMA2_SW 0x0041e340
int C2_HOOK_FASTCALL ProcessJointForcesCallBack(void* pArg1, void* pArg2, void* pArg3) {

    return 0;
}

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e340
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

// findfloor

// FindFloorInBoxBU

// CancelPendingCunningStunt

// SetAmbientPratCam

// SetTextureBits

// MungeSomeOtherCarGraphics

// MungeCarGraphics

// TurnOffNonGroovers

// DoLODCarModels

// DoComplexCarModels

// ResetCarScreens

// FUNCTION: CARMA2_HW 0x0041f3b0
// FUNCTION: CARMA2_SW 0x0041f430
void C2_HOOK_FASTCALL SwapCar(void) {
}

// FUNCTION: CARMA2_HW 0x0041f3c0
// FUNCTION: CARMA2_SW 0x0041f440
void C2_HOOK_FASTCALL AdjustDownForce(void) {
    char s[100];
    tCar_spec* c;

    c = gCar_to_view;
    c->downforce_to_weight += 50.f;
    if (c->downforce_to_weight > 2000.f) {
        c->downforce_to_weight = 50.f;
    }
    if (c->downforce_to_weight > 300.f) {
        c->downforce_to_weight = 2000.f;
    }
    sprintf(s, "DownForceSpeed %f", c->downforce_to_weight);
    NewTextHeadupSlot(4, 0, 1500, -4, s);
}

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// FUNCTION: CARMA2_HW 0x004207e0
void C2_HOOK_FASTCALL AdjustNonCar(br_actor* pActor, const br_matrix34* pMatrix) {
    tU8 column_x;
    tU8 column_z;
    br_actor* parent;

    BrMatrix34Copy(&pActor->t.t.mat, pMatrix);
    if (pActor->parent == gNon_track_actor) {
        return;
    }
    XZToColumnXZ(&column_x, &column_z, pActor->t.t.mat.m[3][0], pActor->t.t.mat.m[3][2], &gProgram_state.track_spec);
    parent = gProgram_state.track_spec.columns[column_z][column_x].actor_0x0;
    if (parent != pActor->parent && parent != NULL) {
        BrActorRemove(pActor);
        BrActorAdd(gProgram_state.track_spec.columns[column_z][column_x].actor_0x0, pActor);
    }
}

// GetPrecalculatedFacesUnderCar

// FUNCTION: CARMA2_HW 0x004208f0
void C2_HOOK_FASTCALL TurnOnNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_FACES;
}

// FUNCTION: CARMA2_HW 0x00420900
void C2_HOOK_FASTCALL TurnOffNonCar(int pIndex) {
    gProgram_state.track_spec.non_car_list[pIndex]->render_style = BR_RSTYLE_NONE;
}

