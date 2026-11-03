#include "01-network.h"

#include "rec2_macros.h"

extern tJoinable_game gGames_to_join[6];
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern tU32 C2_HOOK_FASTCALL NetExtractGameID(tNet_game_details* pDetails);

// FUNCTION: CARMA2_HW 0x004681d0
void C2_HOOK_FASTCALL AddToJoinList(tNet_game_details* pGame) {
    int i;
    int slot_to_use;
    tU32 this_game_ID;

    slot_to_use = -1;
    this_game_ID = NetExtractGameID(pGame);
    for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
        if (gGames_to_join[i].game != NULL) {
            if (NetExtractGameID(gGames_to_join[i].game) == this_game_ID) {
                NetDisposeGameDetails(gGames_to_join[i].game);
                gGames_to_join[i].game = NULL;
                slot_to_use = i;
                break;
            }
        }
    }
    if (slot_to_use < 0) {
        for (i = 0; i < (int)REC2_ASIZE(gGames_to_join); i++) {
            if (gGames_to_join[i].game == NULL) {
                slot_to_use = i;
                break;
            }
        }
    }
    if (slot_to_use >= 0) {
        gGames_to_join[slot_to_use].game = pGame;
        gGames_to_join[slot_to_use].time = PDGetTotalTime();
    }
}

// STUB: CARMA2_HW 0x0049ee30
tU32 C2_HOOK_FASTCALL NetExtractGameID(tNet_game_details* pDetails) { NOT_IMPLEMENTED(); return 0; }
