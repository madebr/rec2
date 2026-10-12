#include "00-car.h"

#include "41-utility.h"
#include "34-finteray.h"
#include "globvars.h"

// GLOBAL: CARMA2_HW 0x0079eda0
tNon_car_spec* gActive_non_car_list[99];

// GLOBAL: CARMA2_HW 0x006793dc
int gNum_active_non_cars;

// GLOBAL: CARMA2_HW 0x006940b0
int gFace_count;

// GLOBAL: CARMA2_HW 0x006793d0
int gCollision_skip_pling_faces;

// GLOBAL: CARMA2_HW 0x00744820
tFace_ref gFace_list__car[300];

// GLOBAL: CARMA2_HW 0x00763500
int gCunning_stunt_bonus[3];

// GLOBAL: CARMA2_HW 0x0074d1c0
float gArmour_starting_value[100];

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

// DamageUnit

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

// IsCarInTheSea

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

// ControlCar2

// ControlCar3

// ControlCar4

// ControlCar5

// ControlCar1

// SteeringSelfCentre

// NonCarSnapOff

// TestNonCarSnapOff

// TumbleObjectWithV

// TumbleObject

// MakeLiftGoUp

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

// ProcessJointForcesCallBack

// MultiFindFloorInBoxM

// MultiRayCastOnObjects

// MultiFindFloorInBoxBU

// FUNCTION: CARMA2_HW 0x0041e340
void C2_HOOK_FASTCALL FindFace(br_vector3* pPosition, br_vector3* pDir, br_vector3* nor, br_scalar* t, br_material** material) {

    ActorFindFace(pPosition, pDir, gTrack_actor, nor, t, material, NULL);
}

#if defined(__GNUC__)
typedef tU32 tVolumeWord __attribute__((__may_alias__));
#else
typedef tU32 tVolumeWord;
#endif

static void C2_HOOK_CDECL AutoVolumeCopy(void* out, const void* in) {
    *(volatile tVolumeWord*)out = *(const volatile tVolumeWord*)in;
}

static int C2_HOOK_CDECL AutoVolumeUnordered(double value) {
    union { double d; tU32 w[2]; } bits;
    bits.d=value;
    return (bits.w[1]&0x7ff00000u)==0x7ff00000u && ((bits.w[1]&0xfffffu)||bits.w[0]);
}

/* The face interval end is reloaded after each face test. */
// FUNCTION: CARMA2_HW 0x0041e480
int C2_HOOK_FASTCALL FUN_0041e480(br_vector3* pPosition, br_vector3* pDirection, br_vector3* pNormal, br_scalar* pDistance, tPhysics_object* pObject) {
    br_vector3 normal,collision;
    br_scalar distance;
    tU32 best=(tU32)(size_t)pNormal;
    tU32 index;
    tFace_ref* face;
    *pDistance=2.f;
    index=((volatile tPhysics_object*)pObject)->box_face_start;
    face=&gFace_list__car[index];
    while ((tS32)index < ((volatile tPhysics_object*)pObject)->box_face_end) {
        if (!*(const volatile int*)&gCollision_skip_pling_faces || !(face->flags&0x80)) {
            CheckSingleFace(face,pPosition,pDirection,&normal,&distance,&collision);
            if (AutoVolumeUnordered(distance) || AutoVolumeUnordered(*pDistance) || distance < *pDistance) {
                AutoVolumeCopy(pDistance,&distance);
                best=index;
                AutoVolumeCopy(&pNormal->v[0],&normal.v[0]);
                AutoVolumeCopy(&pNormal->v[1],&normal.v[1]);
                AutoVolumeCopy(&pNormal->v[2],&normal.v[2]);
            }
        }
        index++;
        face++;
    }
    if (AutoVolumeUnordered(*pDistance) || *pDistance < 2.f) {
        int material_index=(signed char)gFace_list__car[best].material->identifier[0]-'/';
        if (material_index>=0 && material_index<11) return material_index;
    }
    return 0;
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

// FlyCar

// GetCarOverallBoundsMinY

// SetCarSuspGiveAndHeight

// TestForCarInSensiblePlace

// PullActorFromWorld

// DoPullActorFromWorld

// PipeNonCarObject

// PipeNonCars

// CheckForDeAttachmentOfNonCars

// AdjustNonCar

// GetPrecalculatedFacesUnderCar

// TurnOnNonCar

// TurnOffNonCar

