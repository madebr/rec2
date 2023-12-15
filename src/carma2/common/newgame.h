#ifndef REC2_NEWGAME_H
#define REC2_NEWGAME_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(tNet_game_options, gNet_settings, 9);

void C2_HOOK_FASTCALL DefaultNetSettings(void);

#endif //REC2_NEWGAME_H
