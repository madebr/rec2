#include "loadsave.h"

#include "globvars.h"
#include "loading.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gValid_stashed_save_game, 0x00688ae4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSave_game_out_of_sync, 0x0068b8ec);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game, gStashed_save_game, 0x00688780);

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
