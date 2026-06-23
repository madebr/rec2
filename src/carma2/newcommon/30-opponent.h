#ifndef GUARD_30_OPPONENT_H
#define GUARD_30_OPPONENT_H

#include "rec2_types.h"

extern float gOpponent_nastyness_frigger;
extern int gMinTimeOpponentRepair;
extern int gMaxTimeOpponentRepair;

// DoNotDprintf

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

// LoadCopCars

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

// GetCarSpecFromGlobalOppoIndex

// GetOpponentsRealSection

// GetOpponentsFirstSection

// GetOpponentsNextSection

// GetOpponentsSectionStartNodePoint

// GetOpponentsSectionFinishNodePoint

// GetOpponentsSectionWidth

// GetOpponentsSectionMinSpeed

// GetOpponentsSectionMaxSpeed

// InitOpponentPsyche

// ClearTwattageOccurrenceVariables

// TwoCarsHitEachOther

// RecordOpponentTwattageOccurrence

// GetOpponentMood

#endif // GUARD_30_OPPONENT_H
