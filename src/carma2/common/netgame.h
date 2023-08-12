#ifndef REC2_NETGAME_H
#define REC2_NETGAME_H

#include "rec2_types.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL DefaultNetName(void);

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

#endif // REC2_NETGAME_H
