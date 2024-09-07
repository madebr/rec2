#include "loadsave.h"

#include "globvars.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gValid_stashed_save_game, 0x00688ae4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSave_game_out_of_sync, 0x0068b8ec);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game, gStashed_save_game, 0x00688780);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_saved_games, 0x0068c728);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game*, gSaved_games, 0x0068c72c);

void C2_HOOK_FASTCALL RestoreSinglePlayerState(void) {

    LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
    DoLoadGame2(&C2V(gStashed_save_game));
    C2V(gValid_stashed_save_game) = 0;
}

void C2_HOOK_FASTCALL MaybeRestoreSavedGame(void) {

    if (C2V(gValid_stashed_save_game)) {
        RestoreSinglePlayerState();
    }
}
C2_HOOK_FUNCTION(0x00466e90, MaybeRestoreSavedGame)

int (C2_HOOK_FASTCALL * LoadTheGame_original)(tSave_game* pSave_game);
int C2_HOOK_FASTCALL DoLoadGame2(tSave_game* pSave_game) {

#if defined(C2_HOOKS_ENABLED)
    return LoadTheGame_original(pSave_game);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ce0, DoLoadGame2, LoadTheGame_original)

void (C2_HOOK_FASTCALL * LoadLastSaveGame_original)(void);
void C2_HOOK_FASTCALL DoLoadMostRecentGame(void) {

#if defined(C2_HOOKS_ENABLED)
    LoadLastSaveGame_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491e50, DoLoadMostRecentGame, LoadLastSaveGame_original)

void (C2_HOOK_FASTCALL * WriteSaveGame_original)(void);
void C2_HOOK_FASTCALL DoSaveGame(void) {

#if defined(C2_HOOKS_ENABLED)
    WriteSaveGame_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ac0, DoSaveGame, WriteSaveGame_original)

void C2_HOOK_FASTCALL SplungeSomeData(void* pData, size_t size) {

}
C2_HOOK_FUNCTION(0x00500060, SplungeSomeData)

void C2_HOOK_FASTCALL Encryptificate(tSave_game* pSave_games, int pCount) {

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);
    SplungeSomeData(pSave_games, pCount * sizeof(tSave_game));
}

int C2_HOOK_FASTCALL StartSavedGamesList(void) {
    tPath_name path;
    FILE* f;
    int filesize;

    C2_HOOK_BUG_ON(sizeof(tSave_game) != 0x328);

    if (C2V(gSaved_games) != NULL) {
        PDFatalError("Saved games list already started");
    }
    PathCat(path, C2V(gApplication_path), "SAVEDGAMES.ARS");
    f = DRfopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    PFfseek(f, 0, SEEK_END);
    filesize = PFftell(f);
    PFrewind(f);
    C2V(gCount_saved_games) = filesize / sizeof(tSave_game);
    C2V(gSaved_games) = BrMemAllocate(filesize, kMem_misc);
    PFfread(C2V(gSaved_games), 1, filesize, f);
    Encryptificate(C2V(gSaved_games), C2V(gCount_saved_games));
    PFfclose(f);
    return C2V(gCount_saved_games);
}
C2_HOOK_FUNCTION(0x00491bb0, StartSavedGamesList)

void C2_HOOK_FASTCALL EndSavedGamesList(void) {

    if (C2V(gSaved_games) != NULL) {
        BrMemFree(C2V(gSaved_games));
    }
    C2V(gSaved_games) = NULL;
}
C2_HOOK_FUNCTION(0x00491c90, EndSavedGamesList)

tSave_game* C2_HOOK_FASTCALL GetNthSavedGame(int pN) {

    if (C2V(gSaved_games) != NULL && pN < C2V(gCount_saved_games)) {
        return &C2V(gSaved_games)[C2V(gCount_saved_games) - pN - 1];
    }
    return NULL;
}
C2_HOOK_FUNCTION(0x00491cb0, GetNthSavedGame)

int C2_HOOK_FASTCALL DoLoadGame(int pIndex) {

    if (C2V(gSaved_games) != NULL && pIndex >= 0 && pIndex < C2V(gCount_saved_games)) {
        return DoLoadGame2(&C2V(gSaved_games)[C2V(gCount_saved_games) - pIndex - 1]);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x00491e20, DoLoadGame)
