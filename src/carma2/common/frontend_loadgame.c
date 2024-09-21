#include "frontend_loadgame.h"

#include "frontend.h"
#include "frontend_network.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "loading.h"
#include "loadsave.h"
#include "main.h"
#include "platform.h"
#include "polyfont.h"
#include "sound.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_ctype.h"
#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_LOAD_GAME, 0x005ed5a0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_count_saved_games, 0x00764e9c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_load_game_index_top, 0x007638ac);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_mouse_down, 0x00688b04);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPrev_frontend_mouse_down, 0x00688b08);


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

int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gFrontend_billboard_actors)); i++) {
        KillAPOactor(C2V(gFrontend_billboard_actors)[i]);
    }
    BrActorFree(C2V(gFrontend_menu_camera));
    C2V(gFrontend_menu_camera) = NULL;
    EndSavedGamesList();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingout);
    return 0;
}
C2_HOOK_FUNCTION(0x0046fa60, LoadGameOutFunc)

int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend) {
    int selected_item_index;
    tU32 now;
    int i;

    ServiceGame();
    selected_item_index = C2V(gFrontend_selected_item_index);
    now = PDGetTotalTime();
    C2V(gFrontend_scroll_time_left) -= now - C2V(gFrontend_scroll_last_update);
    if (C2V(gFrontend_scroll_time_left) < 0) {
        C2V(gFrontend_scroll_time_left) = 0;
    }
    C2V(gFrontend_mouse_down) = 0;
    C2V(gFrontend_scroll_time_increment) = 50;
    C2V(gFrontend_scroll_last_update) = now;
    if (C2V(gMouse_in_use)) {
        int mouse_x;
        int mouse_y;

        C2V(gFrontend_selected_item_index) = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        /* FIXME: rename gNet_join_host_result */
        C2V(gNet_join_host_result) = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (C2V(gNet_join_host_result) != -1) {
            C2V(gFrontend_selected_item_index) = C2V(gNet_join_host_result);
        }
        C2V(gPrev_frontend_mouse_down) = C2V(gFrontend_mouse_down);
        C2V(gFrontend_mouse_down) = EitherMouseButtonDown();
        if (C2V(gFrontend_mouse_down) && !C2V(gPrev_frontend_mouse_down)) {
            C2V(gFrontend_scroll_time_increment) = 175;
        } else {
            C2V(gFrontend_scroll_time_increment) = 25;
        }
    }
    if (C2V(gFrontend_load_game_index_top) <= 0) {
        pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    }
    if (C2V(gFrontend_load_game_index_top) + 8 >= C2V(gFrontend_count_saved_games)) {
        pFrontend->items[18].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[18].enabled = kFrontendItemEnabled_enabled;
    }
    for (i = 0; i < 8; i++) {
        size_t j;
        int font = (i == C2V(gFrontend_selected_item_index) - 1) ? kPolyfont_hand_green_15pt_lit : kPolyfont_hand_green_15pt_unlit;
        tSave_game* save_game = GetNthSavedGame(C2V(gFrontend_load_game_index_top) + i);
        char* text_ptr;
        char text[128];
        int y_text = 125 + i * 27;
        int y_apo = 128 + i * 27;
        char date_str[12];
        char time_str[16];
        size_t len_date;

        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, player_name, 0x04);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, time, 0x52);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, credits, 0x204);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, game_completed, 0x20c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, is_boundary_race, 0x30c);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, apo_levels, 0x310);
        C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSave_game, apo_potential, 0x31c);

        if (save_game == NULL) {
            continue;
        }
        c2_memset(text, 0, sizeof(text));
        c2_strcpy(date_str, save_game->date);
        c2_strcpy(time_str, save_game->time);

        len_date = c2_strlen(date_str);
        text_ptr = text;
        for (j = 0; j < len_date; j++) {
            char c = date_str[j];

            if (c2_isalnum(c)) {
                *text_ptr++ = c;
            } else if (date_str[j] == '/') {
                *text_ptr++ = '-';
            }
        }
        SolidPolyFontText(text, 42, y_text, font, eJust_left, 1);

        c2_memset(text, 0, sizeof(text));
        c2_strncpy(text, time_str, 5);
        SolidPolyFontText(text, 120, y_text, font, eJust_left, 1);

        c2_memset(text, 0, sizeof(text));
        c2_strcpy(text, save_game->player_name);
        PolyClipName(text, font, 127);
        SolidPolyFontText(text, 190, y_text, font, 0, 1);

        if (save_game->game_completed) {
            c2_sprintf(text, "! %i", 1 + (C2V(gRace_list)[save_game->current_race_index].group - C2V(gRaceGroups)));
        } else if (save_game->is_boundary_race) {
            c2_sprintf(text, "%c %i", '\x1f', 1 + (C2V(gRace_list)[save_game->current_race_index].group - C2V(gRaceGroups)));
        } else {
            c2_sprintf(text, "%i", 1 + (C2V(gRace_list)[save_game->current_race_index].group - C2V(gRaceGroups)));
        }
        SolidPolyFontText(text, 325, y_text, font, eJust_right, 1);

        SolidPolyFontText(MungeCommas(save_game->credits), 366, y_text, font, eJust_left, 1);

        BuildAPO(save_game->apo_levels[0], save_game->apo_potential[0], i, 0);
        BuildAPO(save_game->apo_levels[1], save_game->apo_potential[1], i, 1);
        BuildAPO(save_game->apo_levels[2], save_game->apo_potential[2], i, 2);
        PrepareAPO(i);
        PrintAPO(450, y_apo, i, 0);
        PrintAPO(500, y_apo, i, 1);
        PrintAPO(550, y_apo, i, 2);
        c2_sprintf(pFrontend->items[15].text, "%i-%i (%i)",
            C2V(gFrontend_load_game_index_top) + 1,
            C2V(gFrontend_load_game_index_top) + 8,
            C2V(gFrontend_count_saved_games));
    }

    if (PDKeyDown(51) || PDKeyDown(52) || (C2V(gFrontend_mouse_down) && C2V(gNet_join_host_result) != -1)) {
        int result;
        tFrontend_spec* next;
        int go_back;

        ToggleSelection(pFrontend);
        switch (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc) {
        case 2:
            return 2;
        case 1:
            C2V(gFrontend_leave_current_menu) = 1;
            break;
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
            result = pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
        } else {
            result = pFrontend->items[selected_item_index].field_0xc;
        }
        next = pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo;
        go_back = next == (tFrontend_spec*)(uintptr_t)0x1;
        if (go_back) {
            next = pFrontend->previous;
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
            if (next == NULL && pFrontend->items[C2V(gFrontend_selected_item_index)].action != temp) {
                DRS3StartSound(C2V(gEffects_outlet), eSoundId_Done);
            }
            pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
        }
        if (next == NULL) {
            return go_back ? 1 : result;
        }

        if (go_back) {
            Generic_LinkOutEffect();
        } else {
            Generic_LinkInEffect();
        }
        C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
        for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
            C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
        }
        FRONTEND_DestroyMenu(pFrontend);
        C2V(gCurrent_frontend_spec) = next;
        FRONTEND_CreateMenu(next);
        if (C2V(gCurrent_frontend_spec) != pFrontend->previous) {
            C2V(gCurrent_frontend_spec)->previous = pFrontend;
        }
        Morph_Initialise(pFrontend, C2V(gCurrent_frontend_spec));
        if (C2V(gCurrent_frontend_spec) == &C2V(gFrontend_QUIT)) {
            C2V(gFrontend_selected_item_index) = 0;
        } else {
            C2V(gFrontend_selected_item_index) = C2V(gCurrent_frontend_spec)->default_item;
        }
        for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
            C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
        }
        UpdateScrollPositions(C2V(gCurrent_frontend_spec));
        return result;
    } else {
        return C2V(gFrontend_leave_current_menu) ? 1 : 0;
    }
}
C2_HOOK_FUNCTION(0x00470020, LoadGameUpdateFunc)

int C2_HOOK_FASTCALL TryToLoadGame(int pN) {

    if (DoLoadGame(C2V(gFrontend_load_game_index_top) + pN)) {
        C2V(gAbandon_game) = 1;
        C2V(gNo_credits_APO_restore) = 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_Done);
        return 1;
    } else {
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_CantAffordPart);
        return 0;
    }
}
C2_HOOK_FUNCTION(0x0046ffd0, TryToLoadGame)

static int LoadSlotN(tFrontend_spec* pFrontend, int pN) {

    if (C2V(gFrontend_load_game_index_top) + pN + 1 > C2V(gFrontend_count_saved_games)) {
        pFrontend->items[pN + 1].menuInfo = NULL;
        return 0;
    }
    TryToLoadGame(pN);
    if (C2V(gProgram_state).racing) {
        return 1;
    }
    C2V(gFrontend_next_menu) = kFrontend_menu_main;
    return 3;
}

int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 0);
}
C2_HOOK_FUNCTION(0x0046fbd0, LoadSlot1)

int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 1);
}
C2_HOOK_FUNCTION(0x0046fc50, LoadSlot2)

int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 2);
}
C2_HOOK_FUNCTION(0x0046fcd0, LoadSlot3)

int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 3);
}
C2_HOOK_FUNCTION(0x0046fd50, LoadSlot4)

int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 4);
}
C2_HOOK_FUNCTION(0x0046fdd0, LoadSlot5)

int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 5);
}
C2_HOOK_FUNCTION(0x0046fe50, LoadSlot6)

int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 6);
}
C2_HOOK_FUNCTION(0x0046fed0, LoadSlot7)

int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 7);
}
C2_HOOK_FUNCTION(0x0046ff50, LoadSlot8)

int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_load_game_index_top) > 0 && C2V(gFrontend_scroll_time_left) == 0) {
        C2V(gFrontend_load_game_index_top) -= 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        C2V(gFrontend_scroll_time_left) += C2V(gFrontend_scroll_time_increment);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046fb30, LoadGameScrollUp)

int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_load_game_index_top) + 8 < C2V(gFrontend_count_saved_games) && C2V(gFrontend_scroll_time_left) == 0) {
        C2V(gFrontend_load_game_index_top) += 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        C2V(gFrontend_scroll_time_left) += C2V(gFrontend_scroll_time_increment);
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046fb80, LoadGameScrollDown)
