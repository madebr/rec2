#ifndef REC2_LOADSAVE_H
#define REC2_LOADSAVE_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(int, gValid_stashed_save_game);
C2_HOOK_VARIABLE_DECLARE(int, gSave_game_out_of_sync);
C2_HOOK_VARIABLE_DECLARE(tSave_game, gStashed_save_game);
C2_HOOK_VARIABLE_DECLARE(int, gCount_saved_games);
C2_HOOK_VARIABLE_DECLARE(tSave_game*, gSaved_games);

void C2_HOOK_FASTCALL RestoreSinglePlayerState(void);

void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void);

int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game);

void C2_HOOK_FASTCALL DoLoadMostRecentGame(void);

void C2_HOOK_FASTCALL DoSaveGame(void);

void C2_HOOK_FASTCALL SplungeSomeData(void* pData, size_t size);

void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount);

int C2_HOOK_FASTCALL StartSavedGamesList(void);

void C2_HOOK_FASTCALL EndSavedGamesList(void);

tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);

int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

void C2_HOOK_FASTCALL MakeSavedGame(tSave_game* pSave_game);

#endif // REC2_LOADSAVE_H
