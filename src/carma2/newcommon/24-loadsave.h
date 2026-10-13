#ifndef GUARD_24_LOADSAVE_H
#define GUARD_24_LOADSAVE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gSave_game_out_of_sync;
extern int gCount_saved_games;
extern tSave_game* gSaved_games;

extern void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount);

// MakeSavedGame

extern void C2_HOOK_FASTCALL DoSaveGame(void);

extern int C2_HOOK_FASTCALL StartSavedGamesList(void);

extern void C2_HOOK_FASTCALL EndSavedGamesList(void);

extern tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);

extern int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game);

extern int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

// DoLoadMostRecentGame

#endif // GUARD_24_LOADSAVE_H
