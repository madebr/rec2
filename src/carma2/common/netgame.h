#ifndef REC2_NETGAME_H
#define REC2_NETGAME_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL DefaultNetName(void);

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

#endif // REC2_NETGAME_H
