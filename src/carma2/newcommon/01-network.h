#ifndef GUARD_01_NETWORK_H
#define GUARD_01_NETWORK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int C2_HOOK_FASTCALL NetInitialise(void);

extern int C2_HOOK_FASTCALL NetShutdown(void);

extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

extern void C2_HOOK_FASTCALL DisableNetService(void);

extern void C2_HOOK_FASTCALL ReenableNetService(void);

extern int C2_HOOK_FASTCALL PermitNetServiceReentrancy(void);

extern void C2_HOOK_FASTCALL HaltNetServiceReentrancy(void);

// NetSendHeadupToAllPlayers

// NetSendHeadupToEverybody

// NetSendHeadupToPlayer

// InitialisePlayerStati

extern void C2_HOOK_FASTCALL LeaveTempGame(void);

extern void C2_HOOK_FASTCALL DisposeCurrentJoinPollGame(void);

extern void C2_HOOK_FASTCALL DoNextJoinPoll(void);

// NetStartProducingJoinList

// NetEndJoinList

// NetDisposePIDGameInfo

extern void C2_HOOK_FASTCALL NetDisposeGameDetails(tNet_game_details* pDetails);

extern tNet_game_details* C2_HOOK_FASTCALL NetAllocatePIDGameDetails(void);

extern void C2_HOOK_FASTCALL NetLeaveGameLowLevel(void);

extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

// NetSetPlayerSystemInfo

// FillInThisPlayer

// LoadCarN

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

extern int C2_HOOK_FASTCALL NetJoinGameLowLevel(tNet_game_details* pGame_details, const char* pName);

// NetJoinGame

extern void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

// NetExtractGameID

// NetExtractPlayerID

extern int C2_HOOK_FASTCALL NetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

// NetReallySendMessageToPlayer

// NetSendMessageToPlayer

extern int C2_HOOK_FASTCALL NetSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage);

// NetSendMessageToAllPlayers

// NetGetContentsSize

// NetGetMessageSize

extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

extern tNet_message* C2_HOOK_FASTCALL NetBuildGuaranteedMessage(tU8 pNet_message_type, int pOption);

// NetGetToPlayerContentsSize

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

extern void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

extern void C2_HOOK_FASTCALL DisposeExcessMemory(void);

extern int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);

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

extern void C2_HOOK_FASTCALL NetReceiveAndProcessMessages(void);

extern void C2_HOOK_FASTCALL BroadcastStatus(void);

extern void C2_HOOK_FASTCALL CheckForDisappearees(void);

extern void C2_HOOK_FASTCALL CheckForPendingStartRace(void);

extern void C2_HOOK_FASTCALL NetService(int pIn_race);

// NetFinishRace

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

extern tPlayer_status C2_HOOK_FASTCALL NetGetPlayerStatus(void);

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

extern void C2_HOOK_FASTCALL NetGuaranteedSendMessageToHost(tNet_game_details* pDetails, tNet_message* pMessage, void* pNotifyFail);

// AddToGuaranteeList

// NetGuaranteedSendMessageToPlayer

// NetGuaranteedSendMessageToAddress

extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

// NetWaitForGuaranteeReplies

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromID(tPlayer_ID pPlayer);

extern tCar_spec* C2_HOOK_FASTCALL NetCarFromPlayerID(tPlayer_ID pPlayer);

extern tNet_game_player_info* C2_HOOK_FASTCALL NetPlayerFromCar(tCar_spec* pCar);

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

#endif // GUARD_01_NETWORK_H
