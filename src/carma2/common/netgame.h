#ifndef REC2_NETGAME_H
#define REC2_NETGAME_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gInitialised_grid);
C2_HOOK_VARIABLE_DECLARE(int, gIt_or_fox);

void C2_HOOK_FASTCALL DefaultNetName(void);

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL InitPlayers(void);

void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

#endif // REC2_NETGAME_H
