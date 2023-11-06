#include "loadsave.h"

#include "globvars.h"
#include "loading.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gValid_stashed_save_game, 0x00688ae4);
C2_HOOK_VARIABLE_IMPLEMENT(tSave_game, gStashed_save_game, 0x00688780);

void C2_HOOK_FASTCALL RestoreStashedSaveGame(void) {

    if (C2V(gValid_stashed_save_game)) {
        LoadRaces(C2V(gRace_list), &C2V(gNumber_of_races), -1);
        LoadTheGame(&C2V(gStashed_save_game));
        C2V(gValid_stashed_save_game) = 0;
    }
}
C2_HOOK_FUNCTION(0x00466e90, RestoreStashedSaveGame)

int (C2_HOOK_FASTCALL * LoadTheGame_original)(tSave_game* pSave_game);
int C2_HOOK_FASTCALL LoadTheGame(tSave_game* pSave_game) {

#if defined(C2_HOOKS_ENABLED)
    return LoadTheGame_original(pSave_game);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ce0, LoadTheGame, LoadTheGame_original)

void (C2_HOOK_FASTCALL * LoadLastSaveGame_original)(void);
void C2_HOOK_FASTCALL LoadLastSaveGame(void) {

#if defined(C2_HOOKS_ENABLED)
    LoadLastSaveGame_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491e50, LoadLastSaveGame, LoadLastSaveGame_original)
