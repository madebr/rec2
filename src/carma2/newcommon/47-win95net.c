#include "47-win95net.h"

#include "52-errors.h"
#include "win32/win32net.h"

// PDNetShutdown

// StartLookup

// PDNetStartProducingJoinList

// PDNetEndJoinList

// PDNetGetNextJoinGame

// CreateNBPAddress

// FUNCTION: CARMA2_HW 0x0051a080
int C2_HOOK_FASTCALL PDNetHostGame(tNet_game_details* pDetails, char* pHost_name, void** pHost_address) {
    dr_dprintf("PDNetHostGame()");
    *pHost_address = &gLocal_address;
    return 1;
}

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

