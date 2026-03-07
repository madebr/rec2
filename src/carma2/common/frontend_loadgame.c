#include "frontend_loadgame.h"

#include "frontend.h"
#include "frontend_main.h"
#include "frontend_network.h"
#include "frontend_options.h"
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

// GLOBAL: CARMA2_HW 0x005ed5a0
tFrontend_spec gFrontend_LOAD_GAME = {
    "LoadGame",
    0,
    19,
    LoadGameInFunc,
    LoadGameOutFunc,
    LoadGameUpdateFunc,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    9,
    0,
    {
        { 0x18,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot1,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot2,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot3,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot4,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot5,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot6,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot7,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadSlot8,          (void*)1,                   0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4b,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xff,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4e,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x100,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xfe,     temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    temp,               NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x8,      temp,               &gFrontend_OPTIONS,    0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollUp,   NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x401,    LoadGameScrollDown, NULL,                       0, 0, 0, 0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00764e9c
int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x007638ac
int gFrontend_load_game_index_top;

// GLOBAL: CARMA2_HW 0x00688b04
int gFrontend_mouse_down;

// GLOBAL: CARMA2_HW 0x00688b08
int gPrev_frontend_mouse_down;


// FUNCTION: CARMA2_HW 0x0046f750
int C2_HOOK_FASTCALL LoadGameInFunc(tFrontend_spec* pFrontend) {
    br_camera* camera;
    int i;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();

    gFrontend_count_saved_games = StartSavedGamesList();
    gFrontend_load_game_index_top = 0;

    gFrontend_scroll_time_left = 0;
    gFrontend_scroll_time_increment = 25;
    gFrontend_scroll_last_update = PDGetTotalTime();

    gFrontend_menu_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = gFrontend_menu_camera->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    for (i = 0; i < REC2_ASIZE(gFrontend_billboard_actors); i++) {
        gFrontend_billboard_actors[i] = CreateAPOactor();
    }
    gFrontend_APO_Colour_1 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0xff, 0x80, 0x00, 0xff);
    gFrontend_APO_Colour_2 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0x00, 0x00, 0x80, 0xff);
    gFrontend_APO_Colour_3 = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(
        gFrontend_billboard_actors[0]->material->colour_map,
        0x00, 0x00, 0xff, 0xff);
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046fa60
int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < REC2_ASIZE(gFrontend_billboard_actors); i++) {
        KillAPOactor(gFrontend_billboard_actors[i]);
    }
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    EndSavedGamesList();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00470020
int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend) {
    int selected_item_index;
    tU32 now;
    int i;

    ServiceGame();
    selected_item_index = gFrontend_selected_item_index;
    now = PDGetTotalTime();
    gFrontend_scroll_time_left -= now - gFrontend_scroll_last_update;
    if (gFrontend_scroll_time_left < 0) {
        gFrontend_scroll_time_left = 0;
    }
    gFrontend_mouse_down = 0;
    gFrontend_scroll_time_increment = 50;
    gFrontend_scroll_last_update = now;
    if (gMouse_in_use) {
        int mouse_x;
        int mouse_y;

        gFrontend_selected_item_index = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        /* FIXME: rename gNet_join_host_result */
        gNet_join_host_result = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (gNet_join_host_result != -1) {
            gFrontend_selected_item_index = gNet_join_host_result;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
        if (gFrontend_mouse_down && !gPrev_frontend_mouse_down) {
            gFrontend_scroll_time_increment = 175;
        } else {
            gFrontend_scroll_time_increment = 25;
        }
    }
    if (gFrontend_load_game_index_top <= 0) {
        pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    }
    if (gFrontend_load_game_index_top + 8 >= gFrontend_count_saved_games) {
        pFrontend->items[18].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[18].enabled = kFrontendItemEnabled_enabled;
    }
    for (i = 0; i < 8; i++) {
        size_t j;
        int font = (i == gFrontend_selected_item_index - 1) ? kPolyfont_hand_green_15pt_lit : kPolyfont_hand_green_15pt_unlit;
        tSave_game* save_game = GetNthSavedGame(gFrontend_load_game_index_top + i);
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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

        c2_memset(text, 0, sizeof(text));
        c2_strncpy(text, time_str, 5);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

        SolidPolyFontText(text, 120, y_text, font, eJust_left, 1);

        c2_memset(text, 0, sizeof(text));
        c2_strcpy(text, save_game->player_name);
        PolyClipName(text, font, 127);
        SolidPolyFontText(text, 190, y_text, font, 0, 1);

        if (save_game->game_completed) {
            c2_sprintf(text, "! %i", 1 + (gRace_list[save_game->current_race_index].group - gRaceGroups));
        } else if (save_game->is_boundary_race) {
            c2_sprintf(text, "%c %i", '\x1f', 1 + (gRace_list[save_game->current_race_index].group - gRaceGroups));
        } else {
            c2_sprintf(text, "%i", 1 + (gRace_list[save_game->current_race_index].group - gRaceGroups));
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
            gFrontend_load_game_index_top + 1,
            gFrontend_load_game_index_top + 8,
            gFrontend_count_saved_games);
    }

    if (PDKeyDown(51) || PDKeyDown(52) || (gFrontend_mouse_down && gNet_join_host_result != -1)) {
        int result;
        tFrontend_spec* next;
        int go_back;

        ToggleSelection(pFrontend);
        switch (pFrontend->items[gFrontend_selected_item_index].field_0xc) {
        case 2:
            return 2;
        case 1:
            gFrontend_leave_current_menu = 1;
            break;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        } else {
            result = pFrontend->items[selected_item_index].field_0xc;
        }
        next = pFrontend->items[gFrontend_selected_item_index].menuInfo;
        go_back = next == (tFrontend_spec*)(uintptr_t)0x1;
        if (go_back) {
            next = pFrontend->previous;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            if (next == NULL && pFrontend->items[gFrontend_selected_item_index].action != temp) {
                DRS3StartSound(gEffects_outlet, eSoundId_Done);
            }
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (next == NULL) {
            return go_back ? 1 : result;
        }

        if (go_back) {
            Generic_LinkOutEffect();
        } else {
            Generic_LinkInEffect();
        }
        gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
        for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
            gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
        }
        FRONTEND_DestroyMenu(pFrontend);
        gCurrent_frontend_spec = next;
        FRONTEND_CreateMenu(next);
        if (gCurrent_frontend_spec != pFrontend->previous) {
            gCurrent_frontend_spec->previous = pFrontend;
        }
        Morph_Initialise(pFrontend, gCurrent_frontend_spec);
        if (gCurrent_frontend_spec == &gFrontend_QUIT) {
            gFrontend_selected_item_index = 0;
        } else {
            gFrontend_selected_item_index = gCurrent_frontend_spec->default_item;
        }
        for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
            gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
        }
        UpdateScrollPositions(gCurrent_frontend_spec);
        return result;
    } else {
        return gFrontend_leave_current_menu ? 1 : 0;
    }
}

// FUNCTION: CARMA2_HW 0x0046ffd0
int C2_HOOK_FASTCALL TryToLoadGame(int pN) {

    if (DoLoadGame(gFrontend_load_game_index_top + pN)) {
        gAbandon_game = 1;
        gNo_credits_APO_restore = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_Done);
        return 1;
    } else {
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
    }
}

static int LoadSlotN(tFrontend_spec* pFrontend, int pN) {

    if (gFrontend_load_game_index_top + pN + 1 > gFrontend_count_saved_games) {
        pFrontend->items[pN + 1].menuInfo = NULL;
        return 0;
    }
    TryToLoadGame(pN);
    if (gProgram_state.racing) {
        return 1;
    }
    gFrontend_next_menu = kFrontend_menu_main;
    return 3;
}

// FUNCTION: CARMA2_HW 0x0046fbd0
int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x0046fc50
int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 1);
}

// FUNCTION: CARMA2_HW 0x0046fcd0
int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 2);
}

// FUNCTION: CARMA2_HW 0x0046fd50
int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 3);
}

// FUNCTION: CARMA2_HW 0x0046fdd0
int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 4);
}

// FUNCTION: CARMA2_HW 0x0046fe50
int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 5);
}

// FUNCTION: CARMA2_HW 0x0046fed0
int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 6);
}

// FUNCTION: CARMA2_HW 0x0046ff50
int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend) {

    return LoadSlotN(pFrontend, 7);
}

// FUNCTION: CARMA2_HW 0x0046fb30
int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend) {

    if (gFrontend_load_game_index_top > 0 && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top -= 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046fb80
int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend) {

    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top += 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}
