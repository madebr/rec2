#ifndef GUARD_01_NETWORK_H
#define GUARD_01_NETWORK_H

#include "c2_hooks.h"
#include "rec2_types.h"

// NetInitialise

// NetShutdown

extern void C2_HOOK_FASTCALL ShutdownNetIfRequired(void);

// DisableNetService

// ReenableNetService

// PermitNetServiceReentrancy

// HaltNetServiceReentrancy

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

// NetDisposeGameDetails

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

// NetSendMessageToHost

// NetSendMessageToAllPlayers

// NetGetContentsSize

// NetGetMessageSize

extern tNet_message* C2_HOOK_FASTCALL NetBuildMessage(undefined pArg1, undefined4 pArg2);

// NetBuildGuaranteedMessage

// NetGetToPlayerContentsSize

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

extern void C2_HOOK_FASTCALL NetSendMessageStacks(void);

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

extern void C2_HOOK_FASTCALL NetFreeExcessMemory(void);

// DisposeExcessMemory

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

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList

// NetGuaranteedSendMessageToPlayer

// NetGuaranteedSendMessageToAddress

extern void C2_HOOK_FASTCALL ResendGuaranteedMessages(void);

// NetWaitForGuaranteeReplies

// NetPlayerFromID

// NetCarFromPlayerID

// NetPlayerFromCar

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

#endif // GUARD_01_NETWORK_H
