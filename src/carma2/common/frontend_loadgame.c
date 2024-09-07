#include "frontend_loadgame.h"

#include "frontend.h"
#include "loadsave.h"
#include "platform.h"
#include "sound.h"
#include "utility.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_LOAD_GAME, 0x005ed5a0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_count_saved_games, 0x00764e9c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_load_game_index_top, 0x007638ac);


int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend) {
    br_camera* camera;
    int i;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();

    C2V(gFrontend_count_saved_games) = StartSavedGamesList();
    C2V(gFrontend_load_game_index_top) = 0;

    C2V(gFrontend_scroll_time_left) = 0;
    C2V(gFrontend_scroll_time_increment) = 25;
    C2V(gFrontend_scroll_last_update) = PDGetTotalTime();

    C2V(gFrontend_menu_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = C2V(gFrontend_menu_camera)->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    for (i = 0; i < REC2_ASIZE(C2V(gFrontend_billboard_actors)); i++) {
        C2V(gFrontend_billboard_actors)[i] = CreateAPOactor();
    }
    C2V(gFrontend_APO_Colour_1) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        C2V(gFrontend_billboard_actors)[0]->material->colour_map,
        0xff, 0x80, 0x00, 0xff);
    C2V(gFrontend_APO_Colour_2) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        C2V(gFrontend_billboard_actors)[0]->material->colour_map,
        0x00, 0x00, 0x80, 0xff);
    C2V(gFrontend_APO_Colour_3) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        C2V(gFrontend_billboard_actors)[0]->material->colour_map,
        0x00, 0x00, 0xff, 0xff);
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingin);
    return 0;
}
C2_HOOK_FUNCTION(0x0046f750, LoadGameInFunc)
