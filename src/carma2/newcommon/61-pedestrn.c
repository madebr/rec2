#include "61-pedestrn.h"

#include <stdint.h>

#include "10-loading2.h"
#include "27-powerup.h"
#include "63-loading3.h"
#include "69-sound.h"

// GLOBAL: CARMA2_HW 0x006a0414
const char* gPedsFolder;

// GLOBAL: CARMA2_HW 0x00744808
tPedestrian* gPedestrian_array;

// GLOBAL: CARMA2_HW 0x007447d4
int gPed_count;

// GLOBAL: CARMA2_HW 0x0065d7d4
int gGoreLevel = 1;

// GLOBAL: CARMA2_HW 0x0065d7c8
int gAnimalsOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7cc
int gFlameThrowerOn = 1;

// GLOBAL: CARMA2_HW 0x0065d7d0
int gExplosives_on = 1;

// OrientationChanged

// AssertRootObjectsMatrix

// InitFaceCaches

// ResetPedFaceCache

// SetPedCurrentFace

// RecachePedestrian

// SetCharacterDirectionAR

// PedApplyPedDir

// SetCharacterMoveAR

// SetPedStartRun

// PedApplyPedMove

// PedUndoPedMove

// PedApplyPedFPChange

// PedApplyStatus

// PedUndoStatus

// SetCharacterPhysicsLevelAR

// PedApplyPedPhysics

// PedApplyPedPos

// PedApplyVanishDismembered

// SetCharacterPositionAR

// SetCharacterBoneModelAR

// PedApplyPedModelChangeP

// PedApplyPedModelChange

// DismemberCharacterAR

// PedApplyDismember

// PedUndoDismember

// ReadPedSpecs

// GetPedPos

// SetUpGibSlicks

// ReadInGiblets

// ReadSettingsFile

// InitOtherPedStuff

// InitNapalmNolts

// STUB: CARMA2_HW 0x004cadc0
void C2_HOOK_FASTCALL InitPolyPedSystem(void) {
    NOT_IMPLEMENTED();
}

// DisposePedSpawnSpec

// DisposeCharacterInstances

// STUB: CARMA2_HW 0x004cb8d0
void C2_HOOK_FASTCALL DisposePedStuff(void) {

    NOT_IMPLEMENTED();
}

// BonerOpenCharacterForm

// BonerOpenDefaultMoves

// BonerOpenPersonality

// BonerOpenMoves

// BonerOpenRemaps

// BonerReadPersonalityModels

// MakeActorRenderable

// StopActorBeingRenderable

// IsActorRenderworthy

// SetPedXZDirection

// StopPedNoise

// MakePedNoise

// SetPedMove

// StartPedRunning

// AcceleratePed

// MungePedHeadAnim

// SetPedHeadAnim

// ClearPedHeadAnim

// ScarePedestrian

// PedAnimCausesMovement

// StopSmoothTurning

// SetKnockedOverMove

// DoPostElectricution

// CBMoveCompleted

// CBLoadForm

// CBDisposeForm

// CBLoadPersonality

// CBDisposePersonality

// CBFillInObject

// DoGibShower

// PedApplyGibShower

// PedUndoGibShower

// DoGiblets

// DoBloodSpurt

// DoSpurt

// PedApplyBloodSpurt

// PedUndoBloodSpurt

// OneLessPed

// ScoreForKilledPedestrian

// KillPedestrian

// DismemberPed

// DamagePedestrian

// SetLastHitter

// MakePedVanish

// CBPassiveCollision

// BastardIt

// MungeProxRay

// DoToPeds

// DoPedAnnihilator

// RepulseIt

// DoPedestrianRepulsifictor

// CompileBurnableList

// SortPedListFunction

// ChoosePedToNapalm

// DoPedNapalm

// KillNapalmBolt

// MungeNapalm

// PedHeadingTowardsCollisionObject

// OutsideFaceXZ

// RescanPedProximity

// RecacheAndSetFace

// PedScanForObjects

// CatAndMouseBonus

// StillifyCorpse

// StraightenOutPed

// ActiveHalted

// CBActiveHalted

// RecordPedSlide

// PedProcessForces

// CBMovedByPhysics

// GetPedCount

// GetPedPosition

// TurnLimbsOnAndOff

// SetModelCallBacks

// BuildPedestrian

// AreaOfTriangle

// MakeRandomPointInTriangle

// SpawnPedsOnFace

// FinishUpLoadingPeds

// MakeFlagWavingBastardWaveHisFlagWhichIsTheProbablyTheLastThingHeWillEverDo

// SetNextRandomTurn

// RandomWander

// ResetScanDirection

// CheckForAvoidingAction

// SmoothTurnPedestrian

// SetThisPedPhysicing

// InitProcessData

// PedFallingForever

// MakeEmBleed

// CheckPowerupMoveSubstitution

// SetRandomOmega

// RenderLimbs

// MungePedestrians

// FUN_1011cb0c

// PossiblePedSmear

// LastChanceForPedEffects

// ResetPedSystem

// ResetNapalmBolts

// ARResetNapalmBolts

// AdjustNapalmBolt

// InitPedsForRace

// RenderElectroBastardRays

// AdjustProxRay

// ResetProxRay

// GetTestPed

// NextPedCam

// PrevPedCam

// FindNearestPed

// ResetPedNearness

// PedPreCollisionStuff

// UpdateIfBackwardsAR

// ChangePedScaling

// ChangeHeadScaling

// MakeMaterialBlendy

// UnmakeMaterialBlendy

// TurnOffCollisions

// TurnOnCollisions

// UpdateModels

// TurnOnGhostPeds

// TurnOffGhostPeds

// MutatePed

#if defined(__GNUC__)
typedef tU32 tPedCacheWord __attribute__((__may_alias__));
#else
typedef tU32 tPedCacheWord;
#endif

static tU32 C2_HOOK_CDECL PedCacheRead(const void* base, tU32 offset) {
    return *(const volatile tPedCacheWord*)(uintptr_t)((tU32)(uintptr_t)base + offset);
}

static void C2_HOOK_CDECL PedCacheWrite(void* base, tU32 offset, tU32 value) {
    *(volatile tPedCacheWord*)(uintptr_t)((tU32)(uintptr_t)base + offset) = value;
}

// FUNCTION: CARMA2_HW 0x004d6c70
void C2_HOOK_FASTCALL FlushAllPedCaches(void) {
    tS32 count = *(volatile int*)&gPed_count;
    tPedestrian* base = *(tPedestrian* volatile*)&gPedestrian_array;
    tU32 row = (tU32)(uintptr_t)base + 0x0cu;
    tU32 index = 0;
    tU32 object;
    tU32 cache;

    if (count <= 0) {
        return;
    }
    do {
        object = PedCacheRead((void*)(uintptr_t)row, 0);
        if (object != 0) {
            cache = PedCacheRead((void*)(uintptr_t)object, 0xb0);
            if (cache != 0) {
                PedCacheWrite((void*)(uintptr_t)cache, 0x14, 0);
                object = PedCacheRead((void*)(uintptr_t)row, 0);
                PedCacheWrite((void*)(uintptr_t)object, 0xb0, 0);
            }
        }
        count = *(volatile int*)&gPed_count;
        index += 1u;
        row += 0x54u;
    } while ((tS32)index < count);
}

// SavePedStuff

// ComparePedStuff

// AdjustPedDiagnostics

// CalmDownAllPeds

// FUNCTION: CARMA2_HW 0x004d6f10
void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void) {
    gPedsFolder = "PEDS";
    SetDefaultSoundFolderName();
    SetDefaultPowerupFilename();
    SetDefaultTextFileName();
    SetDefaultPixelmapFolderName();
}

// STUB: CARMA2_HW 0x004d6f50
void C2_HOOK_FASTCALL SetZombiePedFolderNames(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d6f30
void C2_HOOK_FASTCALL SetBloodPedFolderNames(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004d6f70
void C2_HOOK_FASTCALL SetAlienPedFolderNames(void) {
    NOT_IMPLEMENTED();
}

// AddChangedPoint

// GetHowMuchBloodAndSnotToSmearAbout

// AreWeGoingToOutrageAnimalLoversAroundTheGlobe

// IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense

// IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells

// FUNCTION: CARMA2_HW 0x004d7000
void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    gAnimalsOn = pNewAnimalsOn;
}

// FUNCTION: CARMA2_HW 0x004d7010
void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    gFlameThrowerOn = pNewFlameThrowerOn;
}

// FUNCTION: CARMA2_HW 0x004d7020
void C2_HOOK_FASTCALL SetExplosivesOn(int pExplosives_on) {

    gExplosives_on = pExplosives_on;
}

// FUNCTION: CARMA2_HW 0x004d7030
void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    gGoreLevel = 2 - pNewLevel;
}

// ClearOutMorphs

// InitBoner

// DRVector3SafeCross

// DRVector3SafeCrossXZ

// ScaleModelXYZ

// ScaleModel

// DisposeMove

// DisposeAllLoadedMoves

// ReadRemap

// DisposeRemap

// DisposeAllLoadedRemaps

// FindOrOpenRemap

// Flip3DStoBRaxes

// ReadMove

// FindOrOpenMove

// RemapVector

// RemapPowerupVector

// RemapModelAxis

// SetUpCharacterForm

// DisposeCharacterForm

// DisposeAllLoadedForms

// DisposeCharacterInstance

// FindOrOpenForm

// MakeRotationMatrix

// SetBoner

// CalcBoundsRadius

// BonerCloneModel

// ReadPersonality

// DisposePersonality

// DisposeAllLoadedPersonalities

// FindOrOpenPersonality

// BuildCharacterInstance

// GetCharacterMatrixPtr

// GetBoneMatrixPtr

// FUNCTION: CARMA2_HW 0x00407b10
br_actor* C2_HOOK_FASTCALL GetCharacterActorPtr(const tPed_character_instance* pCharacter) {
    int index;

    index = pCharacter->field_0x4;
    if (index < 0) {
        return NULL;
    }
    return pCharacter->personality->form->actor_sets[index].actors[0];
}

// SetCharacterBonePositions

// MakeCharacterRenderable2

// MakeCharacterRenderable

// CharacterNoLongerRenderable

// CalcCollisionObjectThings

// SetCollisionObject

// FindAndSetSeveredPhysicsSet

// SetSeveredLimbObject

// MakeCharacterCollideworthy2

// MakeCharacterCollideworthy

// GetRootObject

// CharacterNoLongerCollideworthy

// SetObjectV

// SetObjectOmega

// MakePartPhysicallyActive

// TurnPhysicsOn

// MakeCharacterPhysicworthy

// RecalculateOrientationOfRoot

// BonerActiveHalted2

// CharacterNoLongerPhysicworthy

// SetCharacterPhysicsLevel

// StopCharacterMorphing

// MorphCharacterBonePositions

// StopCharacterMove

// MungeCharacterAnimation

// SetOnGroundVector

// ResetAnimation

// SetCharacterMove

// DRMatrix34RotateCos

// DropPointOntoPlane

// SetCharacterDirection

// SetCharacterPosition

// DisposeClonedMaterials

// DisposeAllBonerData

// SetCharacterBoneModel

// SetCharacterBoneModelP

// SetCharacterAllBonesModel

// GetCharacterBoneModel

// GetCharacterModelSet

// BonerPassiveCollision

// BonerActiveHalted

// BonerPedMovedByPhysics

// DismemberCharacter

// GetClearanceFromCharacterInstance

// VectorScaleVector

// ScaleCharacterBone

// ScaleCollisionObject

// ScaleFormBone

// ScaleBone

// MungeAllCharacterMaterials

// MungeAllCharacterObjects

// MungeAllCharacterModels

// DisableOverallMovement

// EnableOverallMovement

// EnableRetainRootMode

// DisableRetainRootMode

