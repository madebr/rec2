#ifndef GUARD_30_OPPONENT_H
#define GUARD_30_OPPONENT_H

#include "rec2_types.h"

extern float gOpponent_nastyness_frigger;
extern int gMinTimeOpponentRepair;
extern int gMaxTimeOpponentRepair;
extern int gNumber_of_cops_before_faffage;

extern void C2_HOOK_CDECL DoNotDprintf(const char* pMessage, ...);

// ProcessCurrentObjective

// ReallocExtraPathNodes

// ReallocExtraPathSections

// PointVisibleFromHere

// WeightedFindNearestNodeAndSection

// FindNearestGeneralSection

// FindNearestPathSection

// DeadStopCar

// TurnOpponentPhysicsOn

// TurnOpponentPhysicsOff

// ApplyOppoRenderage2

// NewObjective

// CalcRaceRoute

// TopUpRandomRoute

// SearchForSection

// UpdatePlayersSection

// AllowForDecel

// ShiftOpponentsProjectedRoute

// NewCalcGetNearPlayerRoute

// CalcReturnToStartPointRoute

// ClearOpponentsProjectedRoute

// AddToOpponentsProjectedRoute

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

// GetOpponentSpecFromCarSpec

extern tCar_spec* C2_HOOK_FASTCALL GetCarSpecFromGlobalOppoIndex(int pIndex);

extern int C2_HOOK_FASTCALL GetOpponentsRealSection(tOpponent_spec* pOpponent_spec, int pSection_no);

extern int C2_HOOK_FASTCALL GetOpponentsFirstSection(const tOpponent_spec* pOpponent_spec);

// GetOpponentsNextSection

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// GetOpponentsSectionWidth

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

extern void C2_HOOK_FASTCALL InitOpponentPsyche(int pOpponent_index);

// ClearTwattageOccurrenceVariables

// TwoCarsHitEachOther

// RecordOpponentTwattageOccurrence

// GetOpponentMood

#endif // GUARD_30_OPPONENT_H
