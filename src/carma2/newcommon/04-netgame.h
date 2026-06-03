#ifndef GUARD_04_NETGAME_H
#define GUARD_04_NETGAME_H

#include "c2_hooks.h"

// SendShapeNumbers

// ClearShapeStatusFlag

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

// CarShapeNeedsSendingToPlayer

// AddShapeStuffToMechanicsMessage

// TimeToSendData

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

// ReceivedCopInfo

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

// SignalToStartRace

// SignalToRepositionCar

// SetUpNetCarPositions

// ReinitialiseCar

// RepositionPlayer

// DisableCar

// EnableCar

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

// DoNetScores2

// DoNetScores

// InitNetHeadups

// DisposeNetHeadups

// EverybodysLost

// DeclareWinner

// PlayerIsIt

// CheckForVampireWinner

// PlayerIsInfected

// FarEnoughAway

// CarInContactWithFox

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

// DoNetGameManagement

// InitialisePlayerScore

// InitPlayers

// UseGeneralScore

// NetSendEnvironmentChanges

// CheckForNeedyEnvironmentRecipients

// ReceivedGameplay

// SendGameplay

// SendGameplayToAllPlayers

// SendGameplayToHost

// InitNetGameplayStuff

extern void C2_HOOK_FASTCALL DefaultNetName(void);

// AddPlayerToShapeStatusLists

// RemovePlayerFromShapeStatusLists

#endif // GUARD_04_NETGAME_H
