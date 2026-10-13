#ifndef GUARD_30_OPPONENT_H
#define GUARD_30_OPPONENT_H

#include "rec2_types.h"

extern float gOpponent_nastyness_frigger;
extern int gMinTimeOpponentRepair;
extern int gMaxTimeOpponentRepair;
extern int gNumber_of_cops_before_faffage;
extern float gShortest_route_length;
extern int gRoute_search_target_count;
extern int gRoute_search_last_target_count;
extern tU8* gBit_per_node;
extern int gRoute_search_calls;
extern int gRoute_search_attempts;
extern int gRoute_search_total_calls;
extern int gRoute_search_max_calls;

extern void C2_HOOK_CDECL DoNotDprintf_opponent(const char* pMessage, ...);

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// PointVisibleFromHere

// WeightedFindNearestNodeAndSection

extern tS16 C2_HOOK_FASTCALL FindNearestGeneralSection(tCar_spec* pCar, const br_vector3* pActor_coords, br_vector3* pPath_direction, br_vector3* pIntersect, br_scalar* pDistance);

// FindNearestPathSection

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// NewObjective

extern void C2_HOOK_FASTCALL CalcRaceRoute(tOpponent_spec* pOpponent_spec);

extern void C2_HOOK_FASTCALL TopUpRandomRoute(tOpponent_spec* pOpponent_spec, int pSections_to_add);

extern int C2_HOOK_FASTCALL SearchForSection(tRoute_section* pRoute, tRoute_section* pBest_route, int* pBest_count, tS16 pTarget, int pDepth, br_scalar pLength, tOpponent_spec* pOpponent_spec);

// UpdatePlayersSection

// AllowForDecel

extern int C2_HOOK_FASTCALL ShiftOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, int pPlaces);

extern void C2_HOOK_FASTCALL NewCalcGetNearPlayerRoute(tOpponent_spec* pOpponent_spec, tCar_spec* pPursuee, int pForce);

extern void C2_HOOK_FASTCALL CalcReturnToStartPointRoute(tOpponent_spec* pOpponent_spec);

// ClearOpponentsProjectedRoute

extern int C2_HOOK_FASTCALL AddToOpponentsProjectedRoute(tOpponent_spec* pOpponent_spec, tS16 pSection_no, int pDirection);

// StunTheBugger

// UnStunTheBugger

// ProcessCompleteRace

// StartRecordingTrail

// RecordNextTrailNode

// FindNearestTrailSection

// CalcNextTrailSection

// ProcessPursueAndTwat

// ProcessRunAway

// ProcessWaitForSomeHaplessSod

// ProcessReturnToStart

// ProcessLevitate

// ProcessGetNearPlayer

// ProcessFrozen

// HeadOnWithPlayerPossible

// AlreadyPursuingCar

// LastTwatteeAPlayer

// LastTwatterAPlayer

// ObjectiveComplete

// TeleportOpponentToNearestSafeLocation

// ChooseNewObjective

// ProcessThisOpponent

// IsNetCarActive

// NoteCarsCurrentlyUsed

// AddIfNotInList

// RemoveAnythingStillInList

// RebuildActiveCarList

extern void C2_HOOK_FASTCALL ForceRebuildActiveCarList(void);

// OpponentRepairNecessary

// PossiblyRepairOpponent

// DisplayOpponentRecoveringHeadup

// StartToCheat

// OiStopCheating

// TeleportCopToStart

// CalcDistanceFromHome

// MassageOpponentPosition

// RematerialiseOpponentOnThisSection

// RematerialiseOpponentOnNearestSection

// RematerialiseOpponent

// CalcPlayerConspicuousness

// CalcOpponentConspicuousnessWithAViewToCheatingLikeFuck

// NumberOfOpponentsStillRunning

// ResetPredominantJobbies

extern void C2_HOOK_FASTCALL LoadCopCars(void);

// LoadInOppoPaths

// DisposeOpponentPaths

// MungeOpponents

// InitOpponents

// DisposeOpponents

// WakeUpOpponentsToTheFactThatTheStartHasBeenJumped

extern int C2_HOOK_FASTCALL GetCarCount(tVehicle_type pCategory);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpec(tVehicle_type pCategory, int pIndex);

// GetDriverName

extern tOpponent_spec* C2_HOOK_FASTCALL GetOpponentSpecFromCarSpec(tCar_spec* pCar_spec);

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

extern int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no);

extern int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec);

extern int C2_HOOK_FASTCALL GetOpponentsNextSection(const tOpponent_spec* pOpponent_spec, tS16 pCurrent_section);

extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionStartNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);

extern const br_vector3* C2_HOOK_FASTCALL GetOpponentsSectionFinishNodePoint(const tOpponent_spec* pOpponent_spec, tS16 pSection);

extern double C2_HOOK_FASTCALL GetOpponentsSectionWidth(const tOpponent_spec* pOpponent_spec, tS16 pSection);

extern tS16 C2_HOOK_FASTCALL GetOpponentsSectionMinSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);

extern tS16 C2_HOOK_FASTCALL GetOpponentsSectionMaxSpeed(tOpponent_spec* pOpponent_spec, tS16 pSection, int pTowards_finish);

extern void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

// ClearTwattageOccurrenceVariables

// TwoCarsHitEachOther

// RecordOpponentTwattageOccurrence

// GetOpponentMood

#endif // GUARD_30_OPPONENT_H
