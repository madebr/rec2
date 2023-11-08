#ifndef REC2_LOADSAVE_H
#define REC2_LOADSAVE_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gValid_stashed_save_game);
C2_HOOK_VARIABLE_DECLARE(int, gSave_game_out_of_sync);
C2_HOOK_VARIABLE_DECLARE(tSave_game, gStashed_save_game);

void C2_HOOK_FASTCALL RestoreStashedSaveGame(void);

int C2_HOOK_FASTCALL LoadTheGame(tSave_game* pSave_game);

void C2_HOOK_FASTCALL LoadLastSaveGame(void);

void C2_HOOK_FASTCALL WriteSaveGame(void);

#endif // REC2_LOADSAVE_H
