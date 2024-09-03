#ifndef REC2_NEWGAME_H
#define REC2_NEWGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tNet_game_options, gNet_settings, 9);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gNet_grid_starts, 10);

void C2_HOOK_FASTCALL DefaultNetSettings(void);

void C2_HOOK_FASTCALL SetOptions(tNet_game_type pGame_type, tNet_game_options* pGame_options);

#endif //REC2_NEWGAME_H
