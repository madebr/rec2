/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "01-network.h"

#include "04-netgame.h"
#include "32-spark.h"
#include "53-controls.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"

#include "rec2_macros.h"

#include <stdlib.h>

// GLOBAL: CARMA2_HW 0x0068d95c
// GLOBAL: CARMA2_SW 0x006879b4
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d978
// GLOBAL: CARMA2_SW 0x006879d0
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d98c
// GLOBAL: CARMA2_SW 0x006879e4
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690230
// GLOBAL: CARMA2_SW 0x0068a288
tAddToJoinListProc* gAdd_proc;

// GLOBAL: CARMA2_HW 0x00690238
// GLOBAL: CARMA2_SW 0x0068a290
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x00690c34
// GLOBAL: CARMA2_SW 0x0068ac8c
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690c38
// GLOBAL: CARMA2_SW 0x0068ac90
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00690c3c
// GLOBAL: CARMA2_SW 0x0068ac94
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c48
// GLOBAL: CARMA2_SW 0x0068aca0
tNet_message_memory* gDynamic_messages;

// GLOBAL: CARMA2_HW 0x00690c4c
// GLOBAL: CARMA2_SW 0x0068aca4
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x00690c54
// GLOBAL: CARMA2_SW 0x0068acac
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0074b7dc
// GLOBAL: CARMA2_SW 0x00741e9c
tPlayer_ID gLocal_net_ID;

// GLOBAL: CARMA2_HW 0x00690c44
// GLOBAL: CARMA2_SW 0x0068ac9c
int gOnly_receive_guarantee_replies;

// GLOBAL: CARMA2_HW 0x0068d97c
// GLOBAL: CARMA2_SW 0x006879d4
tMin_message* gMin_messages;

// GLOBAL: CARMA2_HW 0x0068d970
// GLOBAL: CARMA2_SW 0x006879c8
tMid_message* gMid_messages;

// GLOBAL: CARMA2_HW 0x0068d980
// GLOBAL: CARMA2_SW 0x006879d8
tMax_message* gMax_messages;

// GLOBAL: CARMA2_HW 0x0068d96c
// GLOBAL: CARMA2_SW 0x006879c4
int gNet_initialised;

// GLOBAL: CARMA2_HW 0x0068d960
// GLOBAL: CARMA2_SW 0x006879b8
int gDont_allow_joiners;

// GLOBAL: CARMA2_HW 0x0074a690
// GLOBAL: CARMA2_SW 0x00741150
tU32 gUINT_0074a690;

// GLOBAL: CARMA2_HW 0x0074a718
// GLOBAL: CARMA2_SW 0x007411d8
tU32 gUINT_0074a718;

// GLOBAL: CARMA2_HW 0x0074a734
// GLOBAL: CARMA2_SW 0x007411f4
int gIt_or_fox;

// GLOBAL: CARMA2_HW 0x00659d30
// GLOBAL: CARMA2_SW 0x00654648
tU16 gGuarantee_number = 1;

// GLOBAL: CARMA2_HW 0x0068d958
// GLOBAL: CARMA2_SW 0x006879b0
int gNext_guarantee;

// FUNCTION: CARMA2_HW 0x0049d210
// FUNCTION: CARMA2_SW 0x00499390
int C2_HOOK_FASTCALL NetInitialise(void) {
    int i;

    InitAbuseomatic();
    gNet_service_disable = 0;
    gIn_net_service = 0;
    gMessage_header_size = PDNetGetHeaderSize();
    gOnly_receive_guarantee_replies = 0;
    gMin_messages = BrMemAllocate(MIN_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMin_message)), kMem_net_min_messages);
    gMid_messages = BrMemAllocate(MID_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMid_message)), kMem_net_mid_messages);
    gMax_messages = BrMemAllocate(MAX_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMax_message)), kMem_net_max_messages);

    for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMin_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMid_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)((br_uint_8*)&gMax_messages[i] + gMessage_header_size))->contents.raw.header.type = eNetMsg_none;
    }
    gNet_initialised = PDNetInitialise() == 0;
    if (gNet_initialised) {
        InitNetHeadups();
    }
    GenerateItFoxShadeTable();
    gDont_allow_joiners = 0;
    gUINT_0074a690 = PDGetTotalTime();
    gUINT_0074a718 = PDGetTotalTime();
    gNext_guarantee = 0;
    gDynamic_messages = NULL;
    gGuarantee_number = ((tU16)PDGetTotalTime() & 0x3ff) + 1;
    return !gNet_initialised;
}

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(gMin_messages);
    BrMemFree(gMid_messages);
    BrMemFree(gMax_messages);
    DisposeExcessMemory();
    DisposeNetHeadups();
    return err;
}

// FUNCTION: CARMA2_HW 0x0049d370
// FUNCTION: CARMA2_SW 0x004994f0
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {

    if (gNet_initialised) {
        NetShutdown();
        gNet_initialised = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049d3e0
// FUNCTION: CARMA2_SW 0x00499560
void C2_HOOK_FASTCALL DisableNetService(void) {

    gNet_service_disable = 1;
}

// FUNCTION: CARMA2_HW 0x0049d3f0
// FUNCTION: CARMA2_SW 0x00499570
void C2_HOOK_FASTCALL ReenableNetService(void) {

    gNet_service_disable = 0;
}

// FUNCTION: CARMA2_HW 0x0049d400
// FUNCTION: CARMA2_SW 0x00499580
int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void) {

    if (gIn_net_service) {
        gIn_net_service = 0;
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049d420
// FUNCTION: CARMA2_SW 0x004995a0
void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void) {

    gIn_net_service = 1;
}

// STUB: CARMA2_HW 0x0049d430
// STUB: CARMA2_SW 0x004995b0
// NetSendHeadupToAllPlayers

// STUB: CARMA2_HW 0x0049d780
// STUB: CARMA2_SW 0x00499900
// NetSendHeadupToEverybody

// STUB: CARMA2_HW 0x0049daf0
// STUB: CARMA2_SW 0x00499c70
// NetSendHeadupToPlayer

// InitialisePlayerStati

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel();
    }
    gTime_for_next_one = 1;
    gCurrent_join_poll_game = NULL;
}

void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        BrMemFree(gCurrent_join_poll_game);
        gCurrent_join_poll_game = NULL;
    }
}

// STUB: CARMA2_HW 0x0049ec70
// STUB: CARMA2_SW 0x0049adf0
void C2_HOOK_FASTCALL DoNextJoinPoll(void) {

    if (gTime_for_next_one) {
        gCurrent_join_poll_game = NetAllocatePIDGameDetails();
        if (gCurrent_join_poll_game == NULL) {
            return;
        }
        if (PDNetGetNextJoinGame(gCurrent_join_poll_game, gJoin_poll_index)) {
            if (!NetJoinGameLowLevel(gCurrent_join_poll_game, "!TEMP!")) {
                tNet_message* message;
                gTime_for_next_one = 0;
                message = NetBuildMessage(0, 0);
                NetSendMessageToAddress(gCurrent_join_poll_game, message, &gCurrent_join_poll_game->pd_net_info);
                gBastard_has_answered = 0;
                gAsk_time = PDGetTotalTime();
            } else {
                DisposeCurrentJoinPollGame();
            }
            gJoin_poll_index += 1;
            return;
        } else {
            gJoin_poll_index = 0;
            DisposeCurrentJoinPollGame();
            return;
        }
    } else {
        if (gBastard_has_answered) {
            gAdd_proc(gCurrent_join_poll_game);
            LeaveTempGame();
        } else {
            if (PDGetTotalTime() - gAsk_time > 10000) {
                LeaveTempGame();
                DisposeCurrentJoinPollGame();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049dbd0
// FUNCTION: CARMA2_SW 0x00499d50
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc* pAdd_proc) {

    gAdd_proc = pAdd_proc;
    gJoin_list_mode = 1;
    gBastard_has_answered = 0;
    gTime_for_next_one = 1;
    gJoin_poll_index = 0;
    gCurrent_join_poll_game = NULL;
    PDNetStartProducingJoinList();
}

// NetEndJoinList

// FUNCTION: CARMA2_HW 0x0049dc70
// FUNCTION: CARMA2_SW 0x00499df0
void C2_HOOK_FASTCALL NetDisposePIDGameInfo(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

// FUNCTION: CARMA2_HW 0x0049dc80
// FUNCTION: CARMA2_SW 0x00499e00
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);

    return BrMemAllocate(sizeof(tNet_game_details), kMem_net_pid_details);
}

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame();
    } else {
        PDNetLeaveGame();
    }
}

// STUB: CARMA2_HW 0x0049dc90
// STUB: CARMA2_SW 0x00499e10
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
    NOT_IMPLEMENTED();
}

// NetSetPlayerSystemInfo

// FillInThisPlayer

// LoadCarN

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

// NetJoinGame

// FUNCTION: CARMA2_HW 0x0049ee20
// FUNCTION: CARMA2_SW 0x0049afa0
void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length) {

    PDNetObtainSystemUserName(pName, pMax_length);
    pName[9] = '\0';
}

// NetExtractGameID

// NetExtractPlayerID

// FUNCTION: CARMA2_HW 0x0049ee50
int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    if (gNet_mode == eNet_mode_none && !gJoin_list_mode) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    return PDNetSendMessageToAddress(pDetails, pMessage, pAddress);
}

// FUNCTION: CARMA2_HW 0x0049eea0
// FUNCTION: CARMA2_SW 0x0049b020
int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        return -3;
    }
    pMessage->header.field_0x8 = gLocal_net_ID;
    pMessage->header.field_0x10 = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer_id) {
            return PDNetSendMessageToAddress(pNet_game, pMessage, &gNet_players[i].pd_net_info);
        }
    }
    return -3;
}

// NetSendMessageToPlayer

// FUNCTION: CARMA2_HW 0x0049f0d0
int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (gNet_mode != eNet_mode_none) {
        pMessage->header.field_0x8 = gLocal_net_ID;
        pMessage->header.field_0x10 = PDGetTotalTime();
        return PDNetSendMessageToAddress(pDetails, pMessage, &pDetails->pd_net_info);
    } else {
        return -3;
    }
}

// NetSendMessageToAllPlayers

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049f440
// STUB: CARMA2_SW 0x0049b5c0
tU32 C2_HOOK_FASTCALL NetGetContentsSize(tU8 pType, tU32 pSize_decider) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// NetGetMessageSize

// FUNCTION: CARMA2_HW 0x0049f650
// FUNCTION: CARMA2_SW 0x0049b7d0
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {
    tNet_message* the_message;
    tU32 the_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    the_size = NetGetContentsSize(pArg1, pArg2) + offsetof(tNet_message, contents);
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->header.field_0x14 = 1;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pArg1;
    }
    return the_message;
}

// FUNCTION: CARMA2_HW 0x0049f680
// FUNCTION: CARMA2_SW 0x0049b800
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {
    tNet_message* the_message;
    tU32 contents_size;
    tU32 guarantee_size;
    tU32 the_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents, 0x18);

    contents_size = NetGetContentsSize(pNet_message_type, pOption);
    guarantee_size = NetGetContentsSize(eNet_message_chunk_type_5, 0);
    the_size = NetGetContentsSize(pNet_message_type, pOption) + offsetof(tNet_message, contents) + guarantee_size;
    the_message = NetAllocateMessage(the_size);
    if (the_message != NULL) {
        the_message->contents.raw.header.size = (tU8)contents_size;
        the_message->header.field_0x14 = 0xff;
        the_message->header.field_0x16 = (tU16)the_size;
        the_message->contents.raw.header.type = pNet_message_type;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.type = 5;
        ((tNet_message_guaranteed*)((tU8*)&the_message->contents + the_message->contents.raw.header.size))->header.size = (tU8)guarantee_size;
    }
    return the_message;
}

tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pPlayer, int pContents_size) {
    tNet_message* message;
    tNet_message_chunk* contents;

    if (pPlayer == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pPlayer->field_0xcc;
    if (message != NULL && (tS32)(message->header.field_0x16 + pContents_size) > (tS32)(sizeof(tMax_message) - sizeof(void*))) {
        NetReallySendMessageToPlayer(gCurrent_net_game, message, pPlayer->ID);
        pPlayer->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        pPlayer->field_0xcc = message = NetAllocateMessage(sizeof(tMax_message) - sizeof(void*));
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    contents = (tNet_message_chunk*)((tU8*)message + message->header.field_0x16);
    message->header.field_0x16 += (tU16)pContents_size;
    message->header.field_0x14++;
    return contents;
}

// FUNCTION: CARMA2_HW 0x0049f6f0
// FUNCTION: CARMA2_SW 0x0049b870
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToPlayerContents(tNet_message_type pType, tU32 pSize_decider, tPlayer_ID pPlayer) {
    tNet_message_chunk* contents;
    tU32 content_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);

    content_size = NetGetContentsSize(pType, pSize_decider);
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            contents = NetGetToPlayerContentsSize(&gNet_players[i], content_size);
            contents->raw.header.type = pType;
            contents->raw.header.size = (tU8)content_size;
            return contents;
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049f860
// FUNCTION: CARMA2_SW 0x0049b9e0
tNet_message_chunk* C2_HOOK_FASTCALL NetGetToHostContents(tNet_message_type pType, tU32 pSize_decider) {

    return NetGetToPlayerContents(pType, pSize_decider, gNet_players[0].ID);
}

// NetStartBroadcastContents

// NetBroadcastContents

// FUNCTION: CARMA2_HW 0x0049fcf0
// FUNCTION: CARMA2_SW 0x0049be70
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {
    int i;

    gLast_flush_message = PDGetTotalTime();
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].field_0xcc != NULL) {
            NetReallySendMessageToPlayer(gCurrent_net_game, gNet_players[i].field_0xcc, gNet_players[i].ID);
            gNet_players[i].field_0xcc = NULL;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049fdb0
// STUB: CARMA2_SW 0x0049bf30
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    NOT_IMPLEMENTED();
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gDynamic_messages != NULL && *(&gDynamic_messages->message.contents.raw.header.type + gMessage_header_size) == eNetMsg_none) {
        tNet_message_memory* next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

void C2_HOOK_FASTCALL DisposeExcessMemory(void) {
    tNet_message_memory* next;

    while (gDynamic_messages != NULL) {
        next = gDynamic_messages->next;
        BrMemFree(gDynamic_messages);
        gDynamic_messages = next;
    }
}

// FUNCTION: CARMA2_HW 0x0049ff50
// FUNCTION: CARMA2_SW 0x0049c0d0
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (pMessage->header.field_0x14 == 0xff) {
        return -1;
    }
    pMessage->contents.raw.header.type = eNetMsg_none;
    return 0;
}

// NetReallyDisposeMessage

// NetGetNextMessage

// ReceivedSendMeDetails

// ReceivedDetails

// SendOutPlayerList

// ReceivedJoin

// KickPlayerOut

// ReceivedLeave

// NetFullScreenMessage

// HostHasBittenTheDust

// ReceivedHosticide

// ReceivedNewPlayerList

// ReceivedRaceOver

// ReceivedStatusReport

// ReceivedGroupStatus

// ReceivedRepositionCar

// ReceivedStartRace

// ReceivedGuaranteeReply

// ReceivedHeadup

// ReceivedHostQuery

// ReceivedHostReply

// ReceivedGuaranteedMessage

// PlayerIsInList

// ReceivedTimeSync

// ReceivedConfirm

// ReceivedDisableCar

// ReceivedEnableCar

// ReceivedActivateLift

// ReceivedScores

// ReceivedWasted

// ReceivedCarDetailsReq

// ReceivedCarDetails

// ReceivedGameScores

// ProcessGuaranteeStuff

// ReceivedMessage

// STUB: CARMA2_HW 0x004a4a40
void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a4ac0
void C2_HOOK_FASTCALL BroadcastStatus(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004a4da0
void C2_HOOK_FASTCALL CheckForDisappearees(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5240
// FUNCTION: CARMA2_SW 0x004a13c0
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_game_player_info, awaiting_confirmation, 0x60);

    if (gSynch_race_start) {
        for (i = 1; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].awaiting_confirmation) {
                return;
            }
        }
        SignalToStartRace();
    }
}

// STUB: CARMA2_HW 0x004a5280
void C2_HOOK_FASTCALL NetService(int pIn_race) {
    tU32 time;

    if (gIn_net_service || gNet_service_disable) {
        return;
    }
    time = PDGetTotalTime();
    gIn_net_service = 1;
    if (gJoin_list_mode) {
        NetFreeExcessMemory();
        DoNextJoinPoll();
        NetReceiveAndProcessMessages();
    } else {
        if (gNet_mode != eNet_mode_none) {
            NetFreeExcessMemory();
            if (!pIn_race) {
                NetReceiveAndProcessMessages();
            }
            if (time - gLast_status_broadcast > 1000) {
                gLast_status_broadcast = PDGetTotalTime();
                BroadcastStatus();
            }
            CheckForDisappearees();
            if (gNet_mode == eNet_mode_host) {
                CheckForPendingStartRace();
                CheckForNeedyEnvironmentRecipients();
            }
        }
    }
    if (gJoin_list_mode || gNet_mode != eNet_mode_none) {
        ResendGuaranteedMessages();
        if (time > gLast_flush_message + 200u || !gProgram_state.racing) {
            NetSendMessageStacks();
        }
    }
    gIn_net_service = 0;
}

// NetFinishRace

// STUB: CARMA2_HW 0x004a57e0
// STUB: CARMA2_SW 0x004a1960
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5b30
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {

    return gNet_players[gThis_net_player_index].player_status;
}

// FUNCTION: CARMA2_HW 0x004a5b50
// FUNCTION: CARMA2_SW 0x004a1cd0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    int i;
    int err;

    err = 0;
    if (gNumber_of_net_players == 1) {
        pMessage->contents.raw.header.type = eNetMsg_none;
        return err;
    }
    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gThis_net_player_index != i) {
            err |= NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[i].ID, pNotifyFail);
        }
    }
    return err;
}

// STUB: CARMA2_HW 0x004a5bc0
// STUB: CARMA2_SW 0x004a1d40
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5c80
// FUNCTION: CARMA2_SW 0x004a1e00
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {
    NetGuaranteedSendMessageToPlayer(pDetails, pMessage, gNet_players[0].ID, pNotifyFail);
}

// AddToGuaranteeList

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004a5e20
// STUB: CARMA2_SW 0x004a1fa0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer, void* pNotifyFail) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// NetGuaranteedSendMessageToAddress

// STUB: CARMA2_HW 0x004a6080
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    NOT_IMPLEMENTED();
}

// NetWaitForGuaranteeReplies

// FUNCTION: CARMA2_HW 0x004a6920
// FUNCTION: CARMA2_SW 0x004a2aa0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].ID == pPlayer) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a6960
// FUNCTION: CARMA2_SW 0x004a2ae0
tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer) {
    tNet_game_player_info* player;

    player = NetPlayerFromID(pPlayer);
    if (player != NULL) {
        return player->car;
    } else {
        return NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004a69b0
// FUNCTION: CARMA2_SW 0x004a2b30
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (gNet_players[i].car == pCar) {
            return &gNet_players[i];
        }
    }
    return NULL;
}

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

