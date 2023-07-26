#ifndef REC2_MAIN_H
#define REC2_MAIN_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_NORETURN void C2_HOOK_FASTCALL QuitGame(void);
//
//tU32 TrackCount(br_actor* pActor, tU32* pCount);
//
//void CheckNumberOfTracks();
//
//void ServiceTheGame(int pRacing);

void C2_HOOK_FASTCALL ServiceGame(void);

//void ServiceGameInRace();
//
C2_NORETURN void C2_HOOK_FASTCALL GameMain(int pArgc, const char** pArgv);

void C2_HOOK_FASTCALL ServiceTheGame(int pRacing);

void C2_HOOK_FASTCALL ServiceGameInRace(void);

#endif // REC2_MAIN_H
