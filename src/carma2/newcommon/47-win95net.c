/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "47-win95net.h"

#include "52-errors.h"
#include "win32/win32net.h"

#include "c2_string.h"

#include <wsipx.h>

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

// FUNCTION: CARMA2_HW 0x0051a5f0
void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address) {
    C2_HOOK_BUG_ON(sizeof(SOCKADDR_IPX) != 14);

    dr_dprintf("PDNetSetPlayerSystemInfo()");
    memcpy(pPlayer->pd_net_info.addr, pSender_address, sizeof(SOCKADDR_IPX));
}

// PDNetGetHeaderSize

