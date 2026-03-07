#include "network.h"

#include "controls.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "netgame.h"
#include "newgame.h"
#include "platform.h"
#include "spark.h"
#include "timers.h"
#include "utility.h"
#include "world.h"

#include "c2_string.h"

#include "brender/brender.h"


// GLOBAL: CARMA2_HW 0x0068d96c
int gNet_initialised;

// GLOBAL: CARMA2_HW 0x0075b8f8
br_pixelmap* gDigits_pix;

// GLOBAL: CARMA2_HW 0x0068d97c
tMin_message* gMin_messages;

// GLOBAL: CARMA2_HW 0x0068d970
tMid_message* gMid_messages;

// GLOBAL: CARMA2_HW 0x0068d980
tMax_message* gMax_messages;

// GLOBAL: CARMA2_HW 0x00690c48
tDynamic_message* gDynamic_messages;

// GLOBAL: CARMA2_HW 0x00690c38
int gNet_service_disable;

// GLOBAL: CARMA2_HW 0x00688b1c
int gNet_storage_space_initialized;

// GLOBAL: CARMA2_HW 0x00659c2c
int gScore_winner = 20;

// GLOBAL: CARMA2_HW 0x0074a694
int gReceived_game_scores;

// GLOBAL: CARMA2_HW 0x0068d98c
int gMessage_header_size;

// GLOBAL: CARMA2_HW 0x00690c3c
int gIn_net_service;

// GLOBAL: CARMA2_HW 0x00690c44
int gOnly_receive_guarantee_replies;

// GLOBAL: CARMA2_HW 0x0074a690
tU32 gUINT_0074a690;

// GLOBAL: CARMA2_HW 0x0074a718
tU32 gUINT_0074a718;

// GLOBAL: CARMA2_HW 0x00659d30
tU16 gGuarantee_number = 1;

// GLOBAL: CARMA2_HW 0x0068d958
int gNext_guarantee;

// GLOBAL: CARMA2_HW 0x0068d960
int gDont_allow_joiners;

// GLOBAL: CARMA2_HW 0x00690c48
tNet_message_memory* gMessage_to_free;

// GLOBAL: CARMA2_HW 0x00690c54
int gJoin_list_mode;

// GLOBAL: CARMA2_HW 0x0068d978
int gBastard_has_answered;

// GLOBAL: CARMA2_HW 0x00690238
int gTime_for_next_one;

// GLOBAL: CARMA2_HW 0x0068d95c
int gJoin_poll_index;

// GLOBAL: CARMA2_HW 0x00690c34
tNet_game_details* gCurrent_join_poll_game;

// GLOBAL: CARMA2_HW 0x00690230
tAddToJoinListProc* gAdd_proc;

// GLOBAL: CARMA2_HW 0x00690c80
tU32 gLast_status_broadcast;

// GLOBAL: CARMA2_HW 0x00690c4c
tU32 gLast_flush_message;

// GLOBAL: CARMA2_HW 0x0068d984
tU32 gAsk_time;

// GLOBAL: CARMA2_HW 0x0074b7dc
tPlayer_ID gLocal_net_ID;

// GLOBAL: CARMA2_HW 0x0068d990
tGuaranteed_message gGuarantee_list[200];

#define MIN_MESSAGES_CAPACITY 200
#define MID_MESSAGES_CAPACITY 200
#define MAX_MESSAGES_CAPACITY 200

#define MESSAGE_MAGIC_NUMBER 0x763a5059

// FUNCTION: CARMA2_HW 0x004a4ac0
void C2_HOOK_FASTCALL BroadcastStatus(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a57e0
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {

    if (gNet_mode != eNet_mode_none && gNet_players[gThis_net_player_index].player_status != pNew_status) {
        gNet_players[gThis_net_player_index].player_status = pNew_status;
        BroadcastStatus();
        if (gProgram_state.current_car.disabled) {
            if (pNew_status >= 6 && pNew_status != ePlayer_status_recovering) {
                EnableCar(&gProgram_state.current_car);
            }
        } else {
            if (pNew_status < 6) {
                DisableCar(&gProgram_state.current_car);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0049a710
void C2_HOOK_FASTCALL InitNetHeadups(void) {

    gDigits_pix = LoadPixelmap("HDIGITS.PIX");
    if (gDigits_pix != NULL) {
        BrMapAdd(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049a730
void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (gDigits_pix != NULL) {
        BrMapRemove(gDigits_pix);
        BrPixelmapFree(gDigits_pix);
    }
}

// FUNCTION: CARMA2_HW 0x0049dc90
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
    NOT_IMPLEMENTED();
}

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(gMin_messages);
    BrMemFree(gMid_messages);
    BrMemFree(gMax_messages);
    if (gDynamic_messages != NULL) {
        while (gDynamic_messages != NULL) {
            tDynamic_message* next = gDynamic_messages->next;
            BrMemFree(gDynamic_messages);
            gDynamic_messages = next;
        }
        gDynamic_messages = NULL;
    }
    DisposeNetHeadups();
    return err;
}

// FUNCTION: CARMA2_HW 0x0049d370
void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {

    if (gNet_initialised) {
        NetShutdown();
        gNet_initialised = 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049d780
void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049d430
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049f650
tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049ee50
int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    NOT_IMPLEMENTED();
    return 0;
}

tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void) {

    C2_HOOK_BUG_ON(sizeof(tNet_game_details) != 0x78);

    return BrMemAllocate(sizeof(tNet_game_details), kMem_net_pid_details);
}

void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        BrMemFree(gCurrent_join_poll_game);
        gCurrent_join_poll_game = NULL;
    }
}

int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName) {

    return PDNetJoinGame(pGame_details, pName);
}

void C2_HOOK_FASTCALL LeaveTempGame(void) {

    if (gCurrent_join_poll_game != NULL) {
        NetLeaveGameLowLevel();
    }
    gTime_for_next_one = 1;
    gCurrent_join_poll_game = NULL;
}

void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void) {

    if (gNet_mode == eNet_mode_host) {
        PDNetHostFinishGame();
    } else {
        PDNetLeaveGame();
    }
}

// FUNCTION: CARMA2_HW 0x0049ec70
void C2_HOOK_FASTCALL DoNextJoinPoll(void) {

    if (gTime_for_next_one) {
        gCurrent_join_poll_game = NetAllocatePIDGameDetails();
        if (gCurrent_join_poll_game == NULL) {
            return;
        }
        if (!PDNetGetNextJoinGame(gCurrent_join_poll_game, gJoin_poll_index)) {
            gJoin_poll_index = 0;
            DisposeCurrentJoinPollGame();
            return;
        }
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

// FUNCTION: CARMA2_HW 0x004a4da0
void C2_HOOK_FASTCALL CheckForDisappearees(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5240
void C2_HOOK_FASTCALL CheckForPendingStartRace(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049b9a0
void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL GetCheckSum(tNet_message* pMessage) {

}

// FUNCTION: CARMA2_HW 0x004a6080
void C2_HOOK_FASTCALL ResendGuaranteedMessages(void) {
    int i;
    int j;
    tU32 time;

    C2_HOOK_BUG_ON(sizeof(tGuaranteed_message) != 0x34);

    i = 0;
    time = PDGetTotalTime();
    for (j = 0; j < gNext_guarantee; j++) {
        if (i != j) {
            c2_memcpy(&gGuarantee_list[i], &gGuarantee_list[j], sizeof(tGuaranteed_message));
        }
        if (!gGuarantee_list[i].recieved) {
            if (gGuarantee_list[i].message->header.field_0x14 != 0xff) {
                PDFatalError("Dodgy message in guarantee list");
            }
            if (gGuarantee_list[i].NotifyFail != NULL) {
                gGuarantee_list[i].recieved |= gGuarantee_list[i].NotifyFail(time - gGuarantee_list[i].send_time, gGuarantee_list[i].message);
            } else {
                if (time - gGuarantee_list[i].send_time > 10000) {
                    gGuarantee_list[i].recieved = 1;
                }
            }
        }
        if (!gGuarantee_list[j].recieved) {
            if (time > gGuarantee_list[i].next_resend_time) {
                /* guarantee number is appended to message (and not part of the header) */
                *(tU16*)(&gGuarantee_list[j].message->contents.raw.data[gGuarantee_list[j].message->contents.raw.header.size]) = gGuarantee_list[i].field_0x2c;
                if (gGuarantee_list[i].field_0x30 == 1) {
                    GetCheckSum(gGuarantee_list[i].message);
                    PDNetSendMessageToAddress(gCurrent_net_game, gGuarantee_list[i].message, &gGuarantee_list[i].pd_address);
                } else if (!gGuarantee_list[i].field_0x30) {
                    int k;

                    for (k = 0; i < gNumber_of_net_players; k++) {
                        if (gNet_players[k].ID == gGuarantee_list[i].local_id) {
                            break;
                        }
                    }
                    if (k != gNumber_of_net_players) {
                        int guarantee_size = 0;
                        void* message_data;
                        tNet_message_chunk* message_chunk;

                        message_chunk = &gGuarantee_list[i].message->contents;
                        for (k = 0; k < 2; k++) {
                            guarantee_size += message_chunk->raw.header.size;
                            message_chunk = (tNet_message_chunk*)((tU8*)message_chunk + message_chunk->raw.header.size);
                        }
                        message_data = NetGetToPlayerContentsSize(&gNet_players[k], guarantee_size);
                        c2_memcpy(message_data, &gGuarantee_list[i].message->contents, guarantee_size);
                    } else {
                        i++;
                        continue;
                    }
                } else {
                    PDFatalError("oops - guaranteed messages");
                }
                gGuarantee_list[i].resend_period = (tU32)(gGuarantee_list[i].resend_period * 1.2f);
                gGuarantee_list[i].next_resend_time += gGuarantee_list[i].resend_period;
                i++;
            }
        } else if ((i <= 0 || gGuarantee_list[i - 1].message != gGuarantee_list[i].message)
                   && (gNext_guarantee <= i + 1 || gGuarantee_list[j + 1].message != gGuarantee_list[i].message)) {
            gGuarantee_list[i].message->header.field_0x14 = 0;
            NetDisposeMessage(gCurrent_net_game, gGuarantee_list[j].message);
        }
    }
    gNext_guarantee = j;
}

void C2_HOOK_FASTCALL NetFreeExcessMemory(void) {

    while (gMessage_to_free != NULL && gMessage_to_free->message.contents.raw.header.type == eNetMsg_none) {
        tNet_message_memory* next = gMessage_to_free->next;
        BrMemFree(gMessage_to_free);
        gMessage_to_free = next;
    }
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

// FUNCTION: CARMA2_HW 0x0049d3e0
void C2_HOOK_FASTCALL DisableNetService(void) {

    gNet_service_disable = 1;
}

// FUNCTION: CARMA2_HW 0x0049d3f0
void C2_HOOK_FASTCALL ReenableNetService(void) {

    gNet_service_disable = 0;
}

// FUNCTION: CARMA2_HW 0x0049fdb0
tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize) {
    void* pointer;
    tNet_message* message;

    // GLOBAL: CARMA2_HW 0x00690c60
    static int rr_min;
    // GLOBAL: CARMA2_HW 0x00690c64
    static int rr_mid;
    // GLOBAL: CARMA2_HW 0x00690c68
    static int rr_max;

    C2_HOOK_BUG_ON(sizeof(tMin_message) - sizeof(void*) != 0x58);
    C2_HOOK_BUG_ON(sizeof(tMid_message) - sizeof(void*) != 0x80);
    C2_HOOK_BUG_ON(sizeof(tMax_message) - sizeof(void*) != 0x200);

    pointer = NULL;
    if (pSize <= sizeof(tMin_message) - sizeof(void*)) {
        int i;

        for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
            if (((tNet_message*)&gMin_messages[rr_min])->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMin_messages[rr_min];
                break;
            }
            rr_min++;
            if (rr_min >= MIN_MESSAGES_CAPACITY) {
                rr_min = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= sizeof(tMid_message) - sizeof(void*)) {
        int i;

        for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
            if (((tNet_message*)&gMid_messages[rr_mid])->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMid_messages[rr_mid];
                break;
            }
            rr_mid++;
            if (rr_mid >= MID_MESSAGES_CAPACITY) {
                rr_mid = 0;
            }
        }
    }
    if (pointer == NULL && pSize <= sizeof(tMax_message) - sizeof(void*)) {
        int i;

        for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
            if (((tNet_message*)&gMax_messages[rr_max])->contents.raw.header.type == eNetMsg_none) {
                pointer = &gMax_messages[rr_max];
                break;
            }
            rr_max++;
            if (rr_max >= MAX_MESSAGES_CAPACITY) {
                rr_max = 0;
            }
        }
    }
    if (pointer == NULL) {
        tNet_message_memory* new_message = BrMemAllocate(sizeof(tDynamic_message*) + gMessage_header_size + pSize, kMem_dynamic_message);
        if (new_message != NULL) {
            tNet_message_memory* last_message;
            if (gMessage_to_free != NULL) {
                for (last_message = gMessage_to_free; last_message->next != NULL; last_message = last_message->next) {
                }
                last_message->next = new_message;
            } else {
                gMessage_to_free = new_message;
            }
            pointer = &new_message->message;
        }
    }
    if (pointer == NULL) {
        abort();
        message = NULL;
    } else {
        message = (tNet_message*)((tU8*)pointer + gMessage_header_size);
        message->header.field_0xc = 1;
        message->header.field_0x4 = MESSAGE_MAGIC_NUMBER;
    }
    return message;
}

// FUNCTION: CARMA2_HW 0x0049f860
tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(int pType, int pOption) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a69b0
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a5710
void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a0080
void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049fcf0
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

// FUNCTION: CARMA2_HW 0x0049d210
int C2_HOOK_FASTCALL NetInitialise(void) {
    int i;

#ifndef REC2_MATCHING
    C2_HOOK_BUG_ON(sizeof(tMin_message) != 0x5c);
    C2_HOOK_BUG_ON(sizeof(tMid_message) != 0x84);
    C2_HOOK_BUG_ON(sizeof(tMax_message) != 0x204);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNet_message, contents.raw.header.type, 0x19);
#endif

    InitAbuseomatic();
    gNet_service_disable = 0;
    gIn_net_service = 0;
    gMessage_header_size = PDNetGetHeaderSize();
    gOnly_receive_guarantee_replies = 0;
    gMin_messages = BrMemAllocate(MIN_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMin_message)), kMem_net_min_messages);
    gMid_messages = BrMemAllocate(MID_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMid_message)), kMem_net_mid_messages);
    gMax_messages = BrMemAllocate(MAX_MESSAGES_CAPACITY * (gMessage_header_size + sizeof(tMax_message)), kMem_net_max_messages);

    for (i = 0; i < MIN_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)&gMin_messages[i])->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MID_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)&gMid_messages[i])->contents.raw.header.type = eNetMsg_none;
    }
    for (i = 0; i < MAX_MESSAGES_CAPACITY; i++) {
        ((tNet_message*)&gMax_messages[i])->contents.raw.header.type = eNetMsg_none;
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
    gMessage_to_free = NULL;
    gGuarantee_number = ((tU16)PDGetTotalTime() & 0x3ff) + 1;
    return !gNet_initialised;
}

// FUNCTION: CARMA2_HW 0x0049dc00
void C2_HOOK_FASTCALL NetEndJoinList(void) {

    if (gJoin_list_mode) {
        gJoin_list_mode = 0;
        DisposeCurrentJoinPollGame();
        LeaveTempGame();
        PDNetEndJoinList();
    }
}

// FUNCTION: CARMA2_HW 0x00688718
void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}

void C2_HOOK_FASTCALL StopAllThatJoinyStuffThisInstant(void) {

    NetEndJoinList();
    DisposeJoinList(gLast_graph_sel);
}

// FUNCTION: CARMA2_HW 0x0049e210
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049e970
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049dbd0
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc) {

    gJoin_list_mode = 1;
    gBastard_has_answered = 0;
    gTime_for_next_one = 1;
    gJoin_poll_index = 0;
    gCurrent_join_poll_game = NULL;
    gAdd_proc = pAdd_proc;
    PDNetStartProducingJoinList();
}

void C2_HOOK_FASTCALL InitNetStorageSpace(void) {
    int i;

    for (i = 0; i < kMax_netplayers; i++) {
        gCurrent_race.opponent_list[i].car_spec = NULL;
    }
    gNet_storage_space_initialized = 1;
    InitialiseStorageSpace(0, &gNet_cars_storage_space, 2000, 50, 2000, 2000, 50);
}

void C2_HOOK_FASTCALL DisposeNetStorageSpace(void) {

    if (gNet_storage_space_initialized) {
        DisposeStorageSpace(&gNet_cars_storage_space);
    }
    gNet_storage_space_initialized = 0;
}

// FUNCTION: CARMA2_HW 0x0049f680
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x004a5c80
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0049ff70
tNet_message* C2_HOOK_FASTCALL NetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
    tNet_message* message;

    Timers_Push(TIMER_LLR);
    message = PDNetGetNextMessage(pDetails, pSender_address);
    Timers_Pop(TIMER_LLR);
    return message;
}

// FUNCTION: CARMA2_HW 0x004a0260
void C2_HOOK_FASTCALL ReceivedMessage(tNet_message* pMessage, void* pSender_address, tU32 pReceive_time) {

    NOT_IMPLEMENTED();
}

void C2_HOOK_FASTCALL CheckCheckSum(tNet_message* pMessage) {
}

void C2_HOOK_FASTCALL StatReceivePacket(void) {
}

void C2_HOOK_FASTCALL NetReallyDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {
    pMessage->contents.raw.header.type = eNetMsg_none;
}

// FUNCTION: CARMA2_HW 0x004a4a40
void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void) {
    int old_net_service;

    old_net_service = gIn_net_service;
    if (gNet_mode != eNet_mode_none || gJoin_list_mode) {
        gIn_net_service = 1;
        while (1) {
            tNet_message *message;
            void *sender_address;

            message = NetGetNextMessage(gCurrent_net_game, &sender_address);
            if (message == NULL) {
                break;
            }
            if (message->header.field_0x14 != 0) {
                tU32 receive_time = GetRaceTime();
                if (message->header.field_0x4 == MESSAGE_MAGIC_NUMBER) {
                    CheckCheckSum(message);
                    StatReceivePacket();
                    ReceivedMessage(message, sender_address, receive_time);
                }
                NetReallyDisposeMessage(gCurrent_net_game, message);
            }
        }
    }
    gIn_net_service = old_net_service;
}

// FUNCTION: CARMA2_HW 0x004a5b50
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, undefined4 *pArg3) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049ff50
int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage) {

    if (pMessage->header.field_0x14 == 0xff) {
        return -1;
    }
    pMessage->contents.raw.header.type = eNetMsg_none;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0049eea0
int C2_HOOK_FASTCALL NetReallySendMessageToPlayer(tNet_game_details* pNet_game, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;

    if (gNet_mode == eNet_mode_none) {
        return -1;
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

// FUNCTION: CARMA2_HW 0x004a5140
void* C2_HOOK_FASTCALL NetGetToPlayerContentsSize(tNet_game_player_info* pNet_player, int pSize) {
    tNet_message* message;
    int offset;

    C2_HOOK_BUG_ON(sizeof(tNet_message_header) != 0x18);

    if (pNet_player == &gNet_players[gThis_net_player_index]) {
        PDFatalError("Trying to send network message to self");
    }
    message = pNet_player->field_0xcc;
    if (pNet_player->field_0xcc != NULL && pNet_player->field_0xcc->header.field_0x16 + pSize > 512) {
        NetReallySendMessageToPlayer(gCurrent_net_game, pNet_player->field_0xcc, pNet_player->ID);
        pNet_player->field_0xcc = message = NULL;
    }
    if (message == NULL) {
        pNet_player->field_0xcc = message = NetAllocateMessage(512);
        message->header.field_0x16 = (tU16)sizeof(tNet_message_header);
        message->header.field_0x14 = 0;
    }
    offset = message->header.field_0x16;
    message->header.field_0x16 += (tU16)pSize;
    message->header.field_0x14 += 1;
    return (tU8*)message + offset;
}

// FUNCTION: CARMA2_HW 0x004a5b30
tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void) {

    return gNet_players[gThis_net_player_index].player_status;
}

// FUNCTION: CARMA2_HW 0x0049f9c0
tNet_message_chunk* C2_HOOK_FASTCALL NetStartBroadcastContents(tNet_message_type pType, tS32 pSize_decider) {

    NOT_IMPLEMENTED();
    return NULL;
}

// FUNCTION: CARMA2_HW 0x0049fb70
void C2_HOOK_FASTCALL NetBroadcastContents(tNet_message_chunk* pMessage) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004a5bc0
int C2_HOOK_FASTCALL NetGuaranteedSendMessageToEverybody(tNet_game_details* pNet_game, tNet_message* pMessage, undefined4 pArg3) {

    NOT_IMPLEMENTED();
    return 0;
}
