#include "frontend_newgame.h"

#include "frontend.h"
#include "globvars.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "intrface.h"
#include "loading.h"
#include "sound.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_NEWGAME, 0x005bf280, {
    FIXME TODO
});


int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend) {

    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingin);
    switch (C2V(gProgram_state).skill_level) {
    case 0:
        pFrontend->items[2].enabled = 1;
        pFrontend->items[2].radioButton_selected = 1;
        break;
    case 1:
    default:
        pFrontend->items[3].enabled = 1;
        pFrontend->items[3].radioButton_selected = 1;
        C2V(gProgram_state).skill_level = 1;
        break;
    case 2:
        pFrontend->items[4].enabled = 1;
        pFrontend->items[4].radioButton_selected = 1;
        break;
    }
    pFrontend->items[5].enabled = 1;
    pFrontend->default_item = 5;
    C2V(gFrontend_selected_item_index) = 5;
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous !=NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    C2V(gFrontend_scrollbars_updated) = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    C2V(gMouse_in_use) = 0;
    return 0;
}
C2_HOOK_FUNCTION(0x004671a0, NewGameInfunc)

int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend) {

    SaveOptions();
    InitGameAccordingToSkillLevel();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingout);
    return 0;
}
C2_HOOK_FUNCTION(0x00467180, NewGameOutfunc)
