/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_04_NETGAME_H
#define GUARD_04_NETGAME_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tNet_game_player_info* gLast_lepper;
extern int gLast_it_change;

// SendShapeNumbers

extern void C2_HOOK_FASTCALL ClearShapeStatusFlag(tCar_crush_network_shapes* pShapes, int pPlayer_id);

// SendNonCar

// HostFillInCarContents

// ClientSendCarData

// DistanceBetweenCars

extern int C2_HOOK_FASTCALL CarShapeNeedsSendingToPlayer(tCar_crush_network_shapes* pShapes, int pPlayer_id);

// AddShapeStuffToMechanicsMessage

// TimeToSendData

// SendCarData

// ReceivedRecover

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);

// SendAllNonCarPositions

// ReceivedNonCarPosition

// ReceivedNonCar

extern void C2_HOOK_FASTCALL SignalToStartRace(void);

// SignalToRepositionCar

// SetUpNetCarPositions

// ReinitialiseCar

// RepositionPlayer

extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

// DoNetworkHeadups

// SortNetHeadAscending

// SortNetHeadDescending

// ClipName

// CreateBoxes

// DrawScoreBoxes

extern void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

extern void C2_HOOK_FASTCALL DoNetScores(void);

extern void C2_HOOK_FASTCALL InitNetHeadups(void);

extern void C2_HOOK_FASTCALL DisposeNetHeadups(void);

// EverybodysLost

extern void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

extern void C2_HOOK_FASTCALL CheckForVampireWinner(void);

// PlayerIsInfected

// FarEnoughAway

extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

// DoNetGameManagement

extern void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

// InitPlayers

// UseGeneralScore

// NetSendEnvironmentChanges

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

// ReceivedGameplay

// SendGameplay

extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

// SendGameplayToHost

extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

extern void C2_HOOK_FASTCALL DefaultNetName(void);

extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);

// RemovePlayerFromShapeStatusLists

#endif // GUARD_04_NETGAME_H
