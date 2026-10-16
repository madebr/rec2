#include "24-loadsave.h"

#include "rec2_types.h"

// GLOBAL: CARMA2_HW 0x0068b8ec
int gSave_game_out_of_sync;

// GLOBAL: CARMA2_HW 0x0068c72c
tSave_game* gSaved_games;

// GLOBAL: CARMA2_HW 0x0068c728
int gCount_saved_games;

// Encryptificate

// MakeSavedGame

// STUB: CARMA2_HW 0x00491ac0
void C2_HOOK_FASTCALL DoSaveGame(void) {
    NOT_IMPLEMENTED();
}

// StartSavedGamesList

// FUNCTION: CARMA2_HW 0x00491c90
void C2_HOOK_FASTCALL EndSavedGamesList(void) {

    if (gSaved_games != NULL) {
        BrMemFree(gSaved_games);
    }
    gSaved_games = NULL;
}

// FUNCTION: CARMA2_HW 0x00491cb0
tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN) {

    if (gSaved_games != NULL && pN < gCount_saved_games) {
        return &gSaved_games[gCount_saved_games - pN - 1];
    }
    return NULL;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00491ce0
int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00491e20
int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {

    if (gSaved_games != NULL && pIndex >= 0 && pIndex < gCount_saved_games) {
        return DoLoadGame2(GetNthSavedGame(pIndex));
    }
    return 0;
}

// DoLoadMostRecentGame

