#ifndef GUARD_47_WIN95NET_H
#define GUARD_47_WIN95NET_H

#include "c2_hooks.h"
#include "rec2_types.h"

// PDNetShutdown

// StartLookup

// PDNetStartProducingJoinList

// PDNetEndJoinList

// PDNetGetNextJoinGame

// CreateNBPAddress

extern int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address);

// PDNetJoinGame

// PDNetLeaveGame

// PDNetHostFinishGame

// PDNetExtractGameID

// PDNetExtractPlayerID

// PDNetObtainSystemUserName

// PDNetSendMessageToAddress

// PDNetGetNextMessage

// SetForRecieve

// PDNetSetPlayerSystemInfo

// PDNetGetHeaderSize

#endif // GUARD_47_WIN95NET_H
