#ifndef GUARD_24_LOADSAVE_H
#define GUARD_24_LOADSAVE_H

#include "c2_hooks.h"

extern int gSave_game_out_of_sync;

// Encryptificate

// MakeSavedGame

extern void C2_HOOK_FASTCALL DoSaveGame(void);

// StartSavedGamesList

extern void C2_HOOK_FASTCALL EndSavedGamesList(void);

// GetNthSavedGame

// DoLoadGame2

extern int C2_HOOK_FASTCALL DoLoadGame(int pIndex);

// DoLoadMostRecentGame

#endif // GUARD_24_LOADSAVE_H
