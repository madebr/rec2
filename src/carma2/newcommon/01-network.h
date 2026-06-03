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

// LeaveTempGame

// DisposeCurrentJoinPollGame

// DoNextJoinPoll

// NetStartProducingJoinList

// NetEndJoinList

// NetDisposePIDGameInfo

// NetDisposeGameDetails

// NetAllocatePIDGameDetails

// NetLeaveGameLowLevel

extern void C2_HOOK_FASTCALL NetLeaveGame(tNet_game_details* pNet_game);

// NetSetPlayerSystemInfo

// FillInThisPlayer

// LoadCarN

// DisposeCarN

// PlayerHasLeft

// RemoveReferencesToCar

// NetPlayersChanged

// NetHostGame

// NetJoinGameLowLevel

// NetJoinGame

// NetObtainSystemUserName

// NetExtractGameID

// NetExtractPlayerID

// NetSendMessageToAddress

// NetReallySendMessageToPlayer

// NetSendMessageToPlayer

// NetSendMessageToHost

// NetSendMessageToAllPlayers

// NetGetContentsSize

// NetGetMessageSize

// NetBuildMessage

// NetBuildGuaranteedMessage

// NetGetToPlayerContentsSize

// NetGetToPlayerContents

// NetGetToHostContents

// NetStartBroadcastContents

// NetBroadcastContents

// NetSendMessageStacks

extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);

// NetFreeExcessMemory

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

// NetReceiveAndProcessMessages

// BroadcastStatus

// CheckForDisappearees

// CheckForPendingStartRace

// NetService

// NetFinishRace

extern void C2_HOOK_FASTCALL NetPlayerStatusChanged(tPlayer_status pNew_status);

// NetGetPlayerStatus

// NetGuaranteedSendMessageToAllPlayers

// NetGuaranteedSendMessageToEverybody

// NetGuaranteedSendMessageToHost

// AddToGuaranteeList

// NetGuaranteedSendMessageToPlayer

// NetGuaranteedSendMessageToAddress

// ResendGuaranteedMessages

// NetWaitForGuaranteeReplies

// NetPlayerFromID

// NetCarFromPlayerID

// NetPlayerFromCar

// GetCheckSum

// CheckCheckSum

// StatReceivePacket

// StatSendPacket

#endif // GUARD_01_NETWORK_H
