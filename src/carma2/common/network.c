#include "network.h"

#include "controls.h"
#include "globvars.h"
#include "loading.h"
#include "newgame.h"
#include "platform.h"
#include "world.h"

#include "brender/brender.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_initialised, 0x0068d96c);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDigits_pix, 0x0075b8f8);
C2_HOOK_VARIABLE_IMPLEMENT(tMin_message*, gMin_messages, 0x0068d97c);
C2_HOOK_VARIABLE_IMPLEMENT(tMid_message*, gMid_messages, 0x0068d970);
C2_HOOK_VARIABLE_IMPLEMENT(tMax_message*, gMax_messages, 0x0068d980);
C2_HOOK_VARIABLE_IMPLEMENT(tDynamic_message*, gDynamic_messages, 0x00690c48);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_service_disable, 0x00690c38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gNet_storage_space_initialized, 0x00688b1c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gScore_winner, 0x00659c2c, 20);

void (C2_HOOK_FASTCALL * NetPlayerStatusChanged_original)(tPlayer_status pNew_status);
void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status) {
#if defined(C2_HOOKS_ENABLED)
NetPlayerStatusChanged_original(pNew_status);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a57e0, NetPlayerStatusChanged, NetPlayerStatusChanged_original)

void C2_HOOK_FASTCALL InitNetHeadups(void) {

    C2V(gDigits_pix) = LoadPixelmap("HDIGITS.PIX");
    if (C2V(gDigits_pix) != NULL) {
        BrMapAdd(C2V(gDigits_pix));
    }
}
C2_HOOK_FUNCTION(0x0049a710, InitNetHeadups)

void C2_HOOK_FASTCALL DisposeNetHeadups(void) {

    if (C2V(gDigits_pix) != NULL) {
        BrMapRemove(C2V(gDigits_pix));
        BrPixelmapFree(C2V(gDigits_pix));
    }
}
C2_HOOK_FUNCTION(0x0049a730, DisposeNetHeadups)

void (C2_HOOK_FASTCALL * NetLeaveGame_original)(tNet_game_details* pNet_game);
void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game) {
#if defined(C2_HOOKS_ENABLED)
    NetLeaveGame_original(pNet_game);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049dc90, NetLeaveGame, NetLeaveGame_original)

int C2_HOOK_FASTCALL NetShutdown(void) {
    int err;

    err = PDNetShutdown();
    DisposeAbuseomatic();
    BrMemFree(C2V(gMin_messages));
    BrMemFree(C2V(gMid_messages));
    BrMemFree(C2V(gMax_messages));
    if (C2V(gDynamic_messages) != NULL) {
        while (C2V(gDynamic_messages) != NULL) {
            tDynamic_message* next = C2V(gDynamic_messages)->next;
            BrMemFree(C2V(gDynamic_messages));
            C2V(gDynamic_messages) = next;
        }
        C2V(gDynamic_messages) = NULL;
    }
    DisposeNetHeadups();
    return err;
}

void C2_HOOK_FASTCALL ShutdownNetIfRequired(void) {

    if (C2V(gNet_initialised)) {
        NetShutdown();
        C2V(gNet_initialised) = 0;
    }
}
C2_HOOK_FUNCTION(0x0049d370, ShutdownNetIfRequired)

void (C2_HOOK_FASTCALL * NetSendHeadupToEverybody_original)(const char* pMessage);
void C2_HOOK_FASTCALL NetSendHeadupToEverybody(const char* pMessage) {

#if defined(C2_HOOKS_ENABLED)
    NetSendHeadupToEverybody_original(pMessage);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049d780, NetSendHeadupToEverybody, NetSendHeadupToEverybody_original)

void (C2_HOOK_FASTCALL * NetSendHeadupToAllPlayers_original)(const char* pMessage);
void C2_HOOK_FASTCALL NetSendHeadupToAllPlayers(char* pMessage) {

#if defined(C2_HOOKS_ENABLED)
    NetSendHeadupToAllPlayers_original(pMessage);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049d430, NetSendHeadupToAllPlayers, NetSendHeadupToAllPlayers_original)

void (C2_HOOK_FASTCALL * NetService_original)(int pIn_race);
void C2_HOOK_FASTCALL NetService(int pIn_race) {
#if defined(C2_HOOKS_ENABLED)
    NetService_original(pIn_race);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a5280, NetService, NetService_original)

void C2_HOOK_FASTCALL DisableNetService(void) {

    C2V(gNet_service_disable) = 1;
}
C2_HOOK_FUNCTION(0x0049d3e0, DisableNetService)

void C2_HOOK_FASTCALL ReenableNetService(void) {

    C2V(gNet_service_disable) = 0;
}
C2_HOOK_FUNCTION(0x0049d3f0, ReenableNetService)

tNet_message_chunk* (C2_HOOK_FASTCALL * NetAllocateMessageChunk_original)(int pType, int pOption);
tNet_message_chunk* C2_HOOK_FASTCALL NetAllocateMessageChunk(int pType, int pOption) {

#if defined(C2_HOOKS_ENABLED)
    return NetAllocateMessageChunk_original(pType, pOption);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049f860, NetAllocateMessageChunk, NetAllocateMessageChunk_original)

tNet_game_player_info* (C2_HOOK_FASTCALL * NetPlayerFromCar_original)(tCar_spec *pCar);
tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec *pCar) {

#if defined(C2_HOOKS_ENABLED)
    return NetPlayerFromCar_original(pCar);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a69b0, NetPlayerFromCar, NetPlayerFromCar_original)

void (C2_HOOK_FASTCALL * NetFinishRace_original)(tNet_game_details* pDetails, tRace_over_reason pReason);
void C2_HOOK_FASTCALL NetFinishRace(tNet_game_details* pDetails, tRace_over_reason pReason) {

#if defined(C2_HOOKS_ENABLED)
    NetFinishRace_original(pDetails, pReason);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a5710, NetFinishRace, NetFinishRace_original)

void (C2_HOOK_FASTCALL * NetFullScreenMessage_original)(int pStr_index, int pLeave_it_up_there);
void C2_HOOK_FASTCALL NetFullScreenMessage(int pStr_index, int pLeave_it_up_there) {

#if defined(C2_HOOKS_ENABLED)
    NetFullScreenMessage_original(pStr_index, pLeave_it_up_there);
#else
#error "Not impplemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a0080, NetFullScreenMessage, NetFullScreenMessage_original)

void (C2_HOOK_FASTCALL * NetSendMessageStacks_original)(void);
void C2_HOOK_FASTCALL NetSendMessageStacks(void) {

#if defined(C2_HOOKS_ENABLED)
    NetSendMessageStacks_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049fcf0, NetSendMessageStacks, NetSendMessageStacks_original)

int (C2_HOOK_FASTCALL * NetInitialise_original)(void);
int C2_HOOK_FASTCALL NetInitialise(void) {

#if defined (C2_HOOKS_ENABLED)
    return NetInitialise_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049d210, NetInitialise, NetInitialise_original)

void (C2_HOOK_FASTCALL * NetEndJoinList_original)(void);
void C2_HOOK_FASTCALL NetEndJoinList(void) {

#if defined(C2_HOOKS_ENABLED)
    NetEndJoinList_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049dc00, NetEndJoinList, NetEndJoinList_original)

void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails) {

    if (pDetails != NULL) {
        BrMemFree(pDetails);
    }
}
C2_HOOK_FUNCTION(0x00688718, NetDisposeGameDetails)

void C2_HOOK_FASTCALL StopAllThatJoinyStuffThisInstant(void) {

    NetEndJoinList();
    DisposeJoinList(C2V(gLast_graph_sel));
}

tNet_game_details* (C2_HOOK_FASTCALL * NetHostGame_original)(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index);
tNet_game_details* C2_HOOK_FASTCALL NetHostGame(tNet_game_type pNet_type, tNet_game_options* pOptions, int pRace_index, const char* pHost_name, int pCar_index) {

#if defined(C2_HOOKS_ENABLED)
    return NetHostGame_original(pNet_type, pOptions, pRace_index, pHost_name, pCar_index);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049e210, NetHostGame, NetHostGame_original)

int (C2_HOOK_FASTCALL * NetJoinGame_original)(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index);
int C2_HOOK_FASTCALL NetJoinGame(tNet_game_details *pGame_details,char *pPlayer_name,int pCar_index) {

#if defined(C2_HOOKS_ENABLED)
    return NetJoinGame_original(pGame_details, pPlayer_name, pCar_index);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049e970, NetJoinGame, NetJoinGame_original)

void (C2_HOOK_FASTCALL * NetStartProducingJoinList_original)(tAddToJoinListProc *pAdd_proc);
void C2_HOOK_FASTCALL NetStartProducingJoinList(tAddToJoinListProc *pAdd_proc) {
#if defined(C2_HOOKS_ENABLED)
    NetStartProducingJoinList_original(pAdd_proc);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049dbd0, NetStartProducingJoinList, NetStartProducingJoinList_original)

void C2_HOOK_FASTCALL InitNetStorageSpace(void) {
    int i;

    for (i = 0; i < kMax_netplayers; i++) {
        C2V(gCurrent_race).opponent_list[i].car_spec = NULL;
    }
    C2V(gNet_storage_space_initialized) = 1;
    InitialiseStorageSpace(0, &C2V(gNet_cars_storage_space), 2000, 50, 2000, 2000, 50);
}

void C2_HOOK_FASTCALL DisposeNetStorageSpace(void) {

    if (C2V(gNet_storage_space_initialized)) {
        DisposeStorageSpace(&C2V(gNet_cars_storage_space));
    }
    C2V(gNet_storage_space_initialized) = 0;
}

tNet_message* (C2_HOOK_FASTCALL * NetBuildGuaranteedMessage_original)(tU8 pNet_message_type, int pOption);
tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption) {

#if defined(C2_HOOKS_ENABLED)
    return NetBuildGuaranteedMessage_original(pNet_message_type, pOption);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0049f680, NetBuildGuaranteedMessage, NetBuildGuaranteedMessage_original)

void (C2_HOOK_FASTCALL * NetGuaranteedSendMessageToHost_original)(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);
void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail) {

#if defined(C2_HOOKS_ENABLED)
    NetGuaranteedSendMessageToHost_original(pDetails, pMessage, pNotifyFail);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004a5c80, NetGuaranteedSendMessageToHost, NetGuaranteedSendMessageToHost_original)
