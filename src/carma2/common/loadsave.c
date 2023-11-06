#include "loadsave.h"

int (C2_HOOK_FASTCALL * LoadTheGame_original)(tSave_game* pSave_game);
int C2_HOOK_FASTCALL LoadTheGame(tSave_game* pSave_game) {

#if defined(C2_HOOKS_ENABLED)
    return LoadTheGame_original(pSave_game);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491ce0, LoadTheGame, LoadTheGame_original)
