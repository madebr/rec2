#ifndef GUARD_24_LOADSAVE_H
#define GUARD_24_LOADSAVE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gSave_game_out_of_sync;

// Encryptificate

// MakeSavedGame

extern void C2_HOOK_FASTCALL DoSaveGame(void);

// StartSavedGamesList

extern void C2_HOOK_FASTCALL EndSavedGamesList(void);

extern tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN);

// DoLoadGame2

extern int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

// DoLoadMostRecentGame

#endif // GUARD_24_LOADSAVE_H
