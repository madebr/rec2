/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_WIN32_NET_H
#define C2_WIN32_NET_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>
#include <winsock.h>

extern char gPathNetworkIni[240];
extern int gPathNetworkIniValid;
extern struct sockaddr gLocal_address;

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr);

int C2_HOOK_FASTCALL BroadcastMessage(void);

int C2_HOOK_FASTCALL PDNetGetHeaderSize(void);

int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);

void C2_HOOK_FASTCALL PDNetHostFinishGame(void);

void C2_HOOK_FASTCALL PDNetLeaveGame(void);

tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** gRemote_addr);

int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress);

#endif // C2_WIN32_NET_H