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

/* Not in this tree's rec2_types.h yet; only ReceivedRecover/ReceivedCrushSync use them. */
typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player;
    tU32 time;
} tNet_message_chunk_recover;

typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[2];
    tPlayer_ID player_id;
    tU8 count;
    undefined field_0x9[3];
    struct {
        tPlayer_ID player;
        tU16 crush_state;
        undefined field_0x6[2];
    } players[1];
} tNet_message_chunk_car_crush_sync;

/* Not in this tree's rec2_types.h yet; only ReceivedNonCarPosition and
   NetSendEnvironmentChanges use it. */
typedef struct {
    tNet_message_chunk_header header;
    undefined field_0x2[6];
    tU32 ID;
    br_matrix34 mat;
    tU16 flags;
} tNet_message_chunk_non_car_position;

extern int C2_HOOK_FASTCALL TimeToSendData(void);

// SendCarData

extern void C2_HOOK_FASTCALL ReceivedRecover(tNet_message_chunk_recover* pContents);

extern void C2_HOOK_FASTCALL ReceivedCrushSync(tNet_message_chunk_car_crush_sync* pContents);

// CopyMechanics

// ReceivedShapeNumbers

// FillInSimpleCarPos

// ReceivedSimpleCarPos

// ReceivedMechanics

extern void C2_HOOK_FASTCALL ReceivedCopInfo(void);

// SendAllNonCarPositions

extern void C2_HOOK_FASTCALL ReceivedNonCarPosition(tNet_message_chunk_non_car_position* pContents);

// ReceivedNonCar

extern void C2_HOOK_FASTCALL SignalToStartRace(void);

// SignalToRepositionCar

extern void C2_HOOK_FASTCALL SetUpNetCarPositions(void);

extern void C2_HOOK_FASTCALL ReinitialiseCar(tCar_spec* pCar);

// RepositionPlayer

extern void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

extern void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);

extern int C2_HOOK_CDECL SortNetHeadAscending(const void* pFirst, const void* pSecond);

extern int C2_HOOK_CDECL SortNetHeadDescending(const void* pFirst, const void* pSecond);

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

extern void C2_HOOK_FASTCALL PlayerIsInfected(tNet_game_player_info* pPlayer);

// FarEnoughAway

extern void C2_HOOK_FASTCALL CarInContactWithFox(tNet_game_player_info* pPlayer);

extern void C2_HOOK_FASTCALL PlayerIsIt(tNet_game_player_info* pPlayer);

// SelectRandomFox

// CalcPlayerScores

// SendPlayerScores

extern void C2_HOOK_FASTCALL DoNetGameManagement(void);

extern void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

// InitPlayers

// UseGeneralScore

extern void C2_HOOK_FASTCALL NetSendEnvironmentChanges(tNet_game_player_info* pRecipient);

extern void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

// ReceivedGameplay

extern void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL SendGameplayToHost(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

extern void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

extern void C2_HOOK_FASTCALL DefaultNetName(void);

extern void C2_HOOK_FASTCALL AddPlayerToShapeStatusLists(int pPlayer_id, int pCount, tNet_game_player_info* pPlayers);

extern void C2_HOOK_FASTCALL RemovePlayerFromShapeStatusLists(tPlayer_ID pPlayer_id, int pPlayer_count, tNet_game_player_info* pPlayers);

#endif // GUARD_04_NETGAME_H
