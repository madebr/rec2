#ifndef REC2_SDL3_NET_H
#define REC2_SDL3_NET_H

#include "rec2_types.h"

#include <sys/socket.h>

extern void PDNetObtainSystemUserName(char *pName, int pMax_length);

extern int PDNetInitialise(void);

extern int PDNetShutdown(void);

extern int PDNetJoinGame(tNet_game_details* pGame_details, const char *pName);

extern void PDNetHostFinishGame(void);

extern void PDNetLeaveGame(void);

extern void MakeMessageToSend(int pMessage_type);

extern int ReceiveHostResponses(void);

extern int BroadcastMessage(void);

extern int PDNetGetNextJoinGame(tNet_game_details *pGame, int pIndex);

extern int PDNetGetHeaderSize(void);

tNet_message *PDNetGetNextMessage(tNet_game_details *pDetails, void **pSender_address);

extern int PDNetSendMessageToAddress(tNet_game_details *pDetails, tNet_message *pMessage, void *pAddress);

extern void PDNetStartProducingJoinList(void);

extern void PDNetEndJoinList(void);

#endif /* REC2_SDL3_NET_H */
