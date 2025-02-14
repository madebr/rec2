#ifndef REC2_NETWORK_H
#define REC2_NETWORK_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gDigits_pix);
C2_HOOK_VARIABLE_DECLARE(tMin_message*, gMin_messages);
C2_HOOK_VARIABLE_DECLARE(tMid_message*, gMid_messages);
C2_HOOK_VARIABLE_DECLARE(tMax_message*, gMax_messages);
C2_HOOK_VARIABLE_DECLARE(int, gNet_service_disable);
C2_HOOK_VARIABLE_DECLARE(int, gNet_storage_space_initialized);
C2_HOOK_VARIABLE_DECLARE(int, gScore_winner);
C2_HOOK_VARIABLE_DECLARE(int, gReceived_game_scores);
C2_HOOK_VARIABLE_DECLARE(int, gMessage_header_size);
C2_HOOK_VARIABLE_DECLARE(int, gIn_net_service);
C2_HOOK_VARIABLE_DECLARE(int, gOnly_receive_guarantee_replies);
C2_HOOK_VARIABLE_DECLARE(tU32, gUINT_0074a690);
C2_HOOK_VARIABLE_DECLARE(tU32, gUINT_0074a718);
C2_HOOK_VARIABLE_DECLARE(tU16, gGuarantee_number);
C2_HOOK_VARIABLE_DECLARE(int, gNext_guarantee);
C2_HOOK_VARIABLE_DECLARE(int, gDont_allow_joiners);
C2_HOOK_VARIABLE_DECLARE(tNet_message_memory*, gMessage_to_free);
C2_HOOK_VARIABLE_DECLARE(int, gJoin_list_mode);
C2_HOOK_VARIABLE_DECLARE(int, gBastard_has_answered);
C2_HOOK_VARIABLE_DECLARE(int, gTime_for_next_one);
C2_HOOK_VARIABLE_DECLARE(int, gJoin_poll_index);
C2_HOOK_VARIABLE_DECLARE(tNet_game_details*, gCurrent_join_poll_game);
C2_HOOK_VARIABLE_DECLARE(tAddToJoinListProc*, gAdd_proc);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_status_broadcast);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_flush_message);

void C2_HOOK_FASTCALL BroadcastStatus(void);

void C2_HOOK_FASTCALL InitNetHeadups(void);

void C2_HOOK_FASTCALL DisposeNetHeadups(void);

void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage);

void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage);

tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

void C2_HOOK_FASTCALL DoNextJoinPoll(void);

void C2_HOOK_FASTCALL CheckForDisappearees(void);

void C2_HOOK_FASTCALL CheckForPendingStartRace(void);

void C2_HOOK_FASTCALL CheckForNeedyEnvironmentRecipients(void);

void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

void C2_HOOK_FASTCALL NetService(int pIn_race);

void C2_HOOK_FASTCALL DisableNetService(void);

void C2_HOOK_FASTCALL ReenableNetService(void);

tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(int pType, int pOption);

tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar);

void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason);

void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there);

void C2_HOOK_FASTCALL NetSendMessageStacks(void);

int C2_HOOK_FASTCALL NetInitialise(void);

void C2_HOOK_FASTCALL NetEndJoinList(void);

void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);

void C2_HOOK_FASTCALL StopAllThatJoinyStuffThisInstant(void);

tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index);

int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index);

void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc);

void C2_HOOK_FASTCALL InitNetStorageSpace(void);

void C2_HOOK_FASTCALL DisposeNetStorageSpace(void);

tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);

void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);

int C2_HOOK_FASTCALL NetGuaranteedSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage, undefined4 *pArg3);

#endif // REC2_NETWORK_H
