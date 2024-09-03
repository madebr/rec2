#ifndef REC2_NEWGAME_H
#define REC2_NEWGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tNet_game_options, gNet_settings, 9);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gNet_grid_starts, 10);
C2_HOOK_VARIABLE_DECLARE(int, gRace_index);
C2_HOOK_VARIABLE_DECLARE(int, gLast_graph_sel);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tJoinable_game,  gGames_to_join, 6);

void C2_HOOK_FASTCALL DefaultNetSettings(void);

void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options);

void C2_HOOK_FASTCALL SetGameTarget(tNet_game_type* pGame_type, tNet_game_options* pGame_options);

void C2_HOOK_FASTCALL ReadNetGameChoices(tNet_game_type* pGame_type, tNet_game_options* pGame_options, int* pRace_index);

void C2_HOOK_FASTCALL DisposeJoinList(int pExemption);

void C2_HOOK_FASTCALL DisposeJoinableGame(int pIndex);

#endif //REC2_NEWGAME_H
