#ifndef REC2_NETGAME_H
#define REC2_NETGAME_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gInitialised_grid);
C2_HOOK_VARIABLE_DECLARE(int, gIt_or_fox);
C2_HOOK_VARIABLE_DECLARE(tNet_game_player_info*, gLast_lepper);
C2_HOOK_VARIABLE_DECLARE(tU32, gLast_it_change);
C2_HOOK_VARIABLE_DECLARE(int, gNot_shown_race_type_headup);
C2_HOOK_VARIABLE_DECLARE(int, gWinner_declared);
C2_HOOK_VARIABLE_DECLARE(tU32, gTime_for_punishment);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0068d920);

void C2_HOOK_FASTCALL DefaultNetName(void);

void C2_HOOK_FASTCALL NetObtainSystemUserName(char* pName, int pMax_length);

void C2_HOOK_FASTCALL DisableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL EnableCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL InitialisePlayerScore(tNet_game_player_info* pPlayer);

void C2_HOOK_FASTCALL InitPlayers(void);

void C2_HOOK_FASTCALL InitNetGameplayStuff(void);

void C2_HOOK_FASTCALL DeclareWinner(int pWinner_index);

void C2_HOOK_FASTCALL SendGameplay(tPlayer_ID pPlayer, tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

void C2_HOOK_FASTCALL SendGameplayToAllPlayers(tNet_gameplay_mess pMess, int pParam_1, int pParam_2, int pParam_3, int pParam_4);

void C2_HOOK_FASTCALL CalcPlayerScores(void);

void C2_HOOK_FASTCALL SendPlayerScores(void);

void C2_HOOK_FASTCALL DoNetGameManagement(void);

void C2_HOOK_FASTCALL SetUpNetCarPositions(void);

void C2_HOOK_FASTCALL SignalToStartRace(void);

void C2_HOOK_FASTCALL DoNetworkHeadups(int pCredits);

void C2_HOOK_FASTCALL SendCarData(tU32 pNext_frame_time);

void C2_HOOK_FASTCALL DoNetScores2(int pOnly_sort_scores);

void C2_HOOK_FASTCALL DoNetScores(void);

#endif // REC2_NETGAME_H
