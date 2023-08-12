#ifndef C2_WIN32_NET_H
#define C2_WIN32_NET_H

#include "c2_hooks.h"

#include "rec2_types.h"

#include <brender/brender.h>
#include <winsock.h>

void C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr);

#endif // C2_WIN32_NET_H
