/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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

extern void C2_HOOK_FASTCALL PDNetSetPlayerSystemInfo(tNet_game_player_info* pPlayer, void* pSender_address);

// PDNetGetHeaderSize

#endif // GUARD_47_WIN95NET_H
