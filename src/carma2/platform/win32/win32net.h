#ifndef C2_WIN32_NET_H
#define C2_WIN32_NET_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>
#include <winsock.h>

C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gPathNetworkIni, 240);
C2_HOOK_VARIABLE_DECLARE(int, gPathNetworkIniValid);

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr);

int C2_HOOK_FASTCALL BroadcastMessage(void);

int C2_HOOK_FASTCALL PDNetGetHeaderSize(void);

int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName);

#endif // C2_WIN32_NET_H
