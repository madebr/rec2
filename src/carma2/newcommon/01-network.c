#include "01-network.h"

#include "04-netgame.h"
#include "platform.h"
#include "globvars.h"
#include "globvrpb.h"

#include <stdlib.h>

#include "../common/network.h"

// GLOBAL: CARMA2_HW 0x0068d95c
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x0068d978
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0068d98c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c80
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690230
tAddToJoinListProc* gAdd_proc;

// GLOBAL: CARMA2_HW 0x00690238
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x00690c34
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690c38
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00690c3c
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c48
tNet_message_memory* gMessage_to_free;

// GLOBAL: CARMA2_HW 0x00690c4c
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x00690c54
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0074b7dc
tPlayer_ID gLocal_net_ID;

// NetInitialise

// NetShutdown

// STUB: CARMA2_HW 0x0049d370
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {
    NOT_IMPLEMENTED();
}

// DisableNetService

// ReenableNetService

// PermitNetServiceReentrancy

// HaltNetServiceReentrancy

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

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

// NetStartProducingJoinList

// NetEndJoinList

// NetDisposePIDGameInfo

// NetDisposeGameDetails

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

// NetReallySendMessageToPlayer

// NetSendMessageToPlayer

// NetSendMessageToHost

// NetSendMessageToAllPlayers

// NetGetContentsSize

// NetGetMessageSize

// STUB: CARMA2_HW 0x0049f650
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {
    NOT_IMPLEMENTED();
    return NULL;
}

// NetBuildGuaranteedMessage

// NetGetToPlayerContentsSize

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

// STUB: CARMA2_HW 0x0049fcf0
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x0049fdb0
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    NOT_IMPLEMENTED();
    return NULL;
}

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gMessage_to_free != NULL && *(&gMessage_to_free->message.contents.raw.header.type + gMessage_header_size) == eNetMsg_none) {
        tNet_message_memory* next = gMessage_to_free->next;
        BrMemFree(gMessage_to_free);
        gMessage_to_free = next;
    }
}

// DisposeExcessMemory

// STUB: CARMA2_HW 0x0049ff50
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {
    NOT_IMPLEMENTED();
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

// STUB: CARMA2_HW 0x004a5240
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5280
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
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
    NOT_IMPLEMENTED();
}

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList

// NetGuaranteedSendMessageToPlayer

// NetGuaranteedSendMessageToAddress

// STUB: CARMA2_HW 0x004a6080
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    NOT_IMPLEMENTED();
}

// NetWaitForGuaranteeReplies

// NetPlayerFromID

// NetCarFromPlayerID

// NetPlayerFromCar

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

