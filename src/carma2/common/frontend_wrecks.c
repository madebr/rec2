#include "frontend_wrecks.h"

#include "frontend.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "graphics.h"
#include "input.h"
#include "intrface.h"
#include "opponent.h"
#include "platform.h"
#include "raycast.h"
#include "sound.h"
#include "utility.h"

#include "rec2_macros.h"

#include "c2_string.h"



// GLOBAL: CARMA2_HW 0x005f8e68
tFrontend_spec gFrontend_WRECKS = {
    "Wrecks",
    0,
    12,
    WrecksInFunc,
    WrecksOutFunc,
    WrecksUpdateFunc,
    &gFrontend_MAIN,
    0,
    0,
    0,
    0,
    8,
    0,
    {
        { 0xd7,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, },
        { 0x403, ScrollToPrevCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x403, ScrollToNextCar,   NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x4f,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xd9,  temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x404, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0xdd,  BuyCurrentCar,     NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x7,   temp,              NULL, 1, 0, 0, 0, 0, 0, 0, 1, 1, },
        { 0x402, temp,              NULL, 0, 0, 0, 0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00763720
tWreck_gallery_sell_info gWreck_gallery_sell_infos[32];

// GLOBAL: CARMA2_HW 0x00763b5c
tWreck_gallery_car_info gWreck_gallery_car_infos[30];

// GLOBAL: CARMA2_HW 0x00763844
br_actor* gFrontend_wrecks_light;

// GLOBAL: CARMA2_HW 0x00705190
int gFrontend_wrecks_car_count;

// GLOBAL: CARMA2_HW 0x00688af8
float gFrontend_wreck_bought_car_dz;

// GLOBAL: CARMA2_HW 0x00763890
int gFrontend_wrecks_current;

// GLOBAL: CARMA2_HW 0x00688afc
int gFrontend_wrecks_hscroll;

// GLOBAL: CARMA2_HW 0x00688b00
int gFrontend_wrecks_pending_hscroll;

// GLOBAL: CARMA2_HW 0x007635ec
tU32 gFrontend_wrecks_previous_update;

// GLOBAL: CARMA2_HW 0x0059b0dc
int gFrontend_wrecks_rotate_prev_x = -1;

// GLOBAL: CARMA2_HW 0x0059b0e0
int gFrontend_wrecks_rotate_prev_y = -1;

// GLOBAL: CARMA2_HW 0x00686828
int gFrontend_opponent_profile_pic_needs_update;

// GLOBAL: CARMA2_HW 0x00763924
int gHierarchy_has_actor;

// GLOBAL: CARMA2_HW 0x00763928
int gPicked_wreck;


// FUNCTION: CARMA2_HW 0x0046e830
int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend) {
    br_camera* camera;
    int car_count;
    int i;

    C2_HOOK_BUG_ON(sizeof(gWreck_gallery_sell_infos) != 0x100);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_car_info) != 0xa4);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_sell_info) != 0x8);

    gCurrent_frontend_spec = pFrontend;
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    gFrontend_wrecks_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    gFrontend_wrecks_camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    gFrontend_wrecks_light = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    camera = gFrontend_wrecks_camera->type_data;
    camera->aspect = 1.6346154f;
    camera->field_of_view = BrDegreeToAngle(55);
    camera->hither_z = 1.f;
    camera->yon_z = 100.f;
    camera->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera->width = 640.f;
    camera->height = 480.f;
    BrActorAdd(gFrontend_wrecks_actor, gFrontend_wrecks_camera);

    gFrontend_wrecks_pixelmap = BrPixelmapAllocateSub(gBack_screen, 65, 55, 510, 312);
    gFrontend_wrecks_pixelmap->origin_x = 255;
    gFrontend_wrecks_pixelmap->origin_y = 120;

    BrMatrix34RotateY(&gFrontend_wrecks_camera->t.t.mat, BrDegreeToAngle(180));
    BrMatrix34PostTranslate(&gFrontend_wrecks_camera->t.t.mat, 0.f, 0.f, -3.f);

    car_count = GetCarCount(eVehicle_opponent);
    gFrontend_wrecks_car_count = 0;

    for (i = 0; i < car_count; i++) {
        tCar_spec* car;
        br_actor* actor;

        car = GetCarSpec(eVehicle_opponent, i);
        actor = car->car_master_actor;
        actor->render_style = BR_RSTYLE_FACES;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorAdd(gFrontend_wrecks_actor, actor);
        gWreck_gallery_car_infos[i].original_matrix = actor->t.t.mat;
        BrMatrix34Identity(&actor->t.t.mat);
        BrMatrix34Identity(&gWreck_gallery_car_infos[i].field_0x14);
        BrMatrix34Identity(&gWreck_gallery_car_infos[i].field_0x74);
        gWreck_gallery_car_infos[i].actor = actor;
        gFrontend_wrecks_car_count += 1;
    }
    gFrontend_wreck_bought_car_dz = 0.f;
    c2_memset(gWreck_gallery_sell_infos, 0, sizeof(gWreck_gallery_sell_infos));

    gFrontend_selected_item_index = 0;
    gFrontend_wrecks_current = 0;
    gFrontend_wrecks_hscroll = 0;
    gFrontend_wrecks_pending_hscroll = 0;
    gFrontend_wrecks_previous_update = PDGetTotalTime();
    gFrontend_wrecks_rotate_prev_x = -1;
    gFrontend_wrecks_rotate_prev_y = -1;
    FuckWithWidths(pFrontend);
    gFrontend_opponent_profile_pic_needs_update = 1;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046e620
void C2_HOOK_FASTCALL DisposeWrecksGallery(void) {
    int i;

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        BrActorRemove(gWreck_gallery_car_infos[i].actor);
    }
    if (gFrontend_wrecks_camera != NULL) {
        if (gFrontend_wrecks_camera->parent != NULL) {
            BrActorRemove(gFrontend_wrecks_camera);
        }
        BrActorFree(gFrontend_wrecks_camera);
    }
    if (gFrontend_wrecks_actor != NULL) {
        if (gFrontend_wrecks_actor->parent != NULL) {
            BrActorRemove(gFrontend_wrecks_actor);
        }
        BrActorFree(gFrontend_wrecks_actor);
    }
    gFrontend_wrecks_actor = NULL;
    gFrontend_wrecks_camera = NULL;
}

// FUNCTION: CARMA2_HW 0x0046ead0
int C2_HOOK_FASTCALL WrecksOutFunc(tFrontend_spec* pFrontend) {

    DisposeWrecksGallery();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046f560
int C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref) {

    if (a == (br_actor*)ref) {
        gHierarchy_has_actor = 1;
    }
    BrActorEnum(a, HeirarchyPick, ref);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046f4f0
int C2_HOOK_CDECL WreckPick(br_actor* world, br_model* model, br_material* material, br_vector3* pos, br_vector3* dir, br_scalar pNear, br_scalar pFar, void* arg) {
    int i;

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        gHierarchy_has_actor = 0;
        BrActorEnum(gWreck_gallery_car_infos[i].actor, HeirarchyPick, world);
        if (gHierarchy_has_actor) {
            gPicked_wreck = i;
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL KeepInRange(int pIndex, int pRange) {

    while (pIndex < 0) {
        pIndex += pRange;
    }
    while (pIndex >= pRange) {
        pIndex -= pRange;
    }
    return pIndex;
}

int C2_HOOK_FASTCALL GotItAlready(tCar_spec* pCar) {
    int i;

    for (i = 0; i < gProgram_state.number_of_cars; i++) {
        if (gProgram_state.cars_available[i] == pCar->index) {
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL GetCarSelectedByMouse(int pX, int pY) {

    gPicked_wreck = -1;
    DRScenePick2DXY(gFrontend_wrecks_actor,
        gFrontend_wrecks_camera,
        gFrontend_wrecks_pixelmap,
        pX - gFrontend_wrecks_pixelmap->origin_x - 65,
        pY - gFrontend_wrecks_pixelmap->origin_y - 55,
        WreckPick,
        NULL);
    return gPicked_wreck;
}

// FUNCTION: CARMA2_HW 0x0046eb80
int C2_HOOK_FASTCALL WrecksUpdateFunc(tFrontend_spec* pFrontend) {
    int item_at_mouse;
    int i;
    int mouse_x;
    int mouse_y;
    tU32 now;
    int count_visible_cars;
    int rotate_angle;
    int index_first_car;

    item_at_mouse = -1;
    gFrontend_mouse_down = 0;
    if (gMouse_in_use) {
        gFrontend_selected_item_index = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        item_at_mouse = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (item_at_mouse != -1) {
            gFrontend_selected_item_index = item_at_mouse;
        }
        gPrev_frontend_mouse_down = gFrontend_mouse_down;
        gFrontend_mouse_down = EitherMouseButtonDown();
    }
    if (gFrontend_selected_item_index == 0) {
        gFrontend_selected_item_index = 10;
    }

    now = PDGetTotalTime();
    if (gFrontend_wreck_bought_car_dz != 0.f) {
        int dt;

        dt = now - gFrontend_wrecks_previous_update;
        gFrontend_wreck_bought_car_dz += 0.0025f * (float)dt;
        if (gFrontend_wreck_bought_car_dz > 2.5f) {
            gFrontend_wreck_bought_car_dz = 0.f;
            gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold = 1;
        }
    }

    if (gFrontend_wrecks_pending_hscroll > 0) {
        gFrontend_wrecks_pending_hscroll -= 5;
        gFrontend_wrecks_hscroll -= 5;
        if (gFrontend_wrecks_pending_hscroll < 0) {
            gFrontend_wrecks_pending_hscroll = 0;
        }
    }
    if (gFrontend_wrecks_pending_hscroll < 0) {
        gFrontend_wrecks_pending_hscroll += 5;
        gFrontend_wrecks_hscroll += 5;
        if (gFrontend_wrecks_pending_hscroll > 0) {
            gFrontend_wrecks_pending_hscroll = 0;
        }
    }

    gFrontend_wrecks_previous_update = now;

    c2_strcpy(pFrontend->items[6].text, MungeCommas(gProgram_state.credits));

    if (gFrontend_wrecks_hscroll % 30 == 0 && gFrontend_wreck_bought_car_dz == 0.f) {
        tCar_spec* car;
        tOpponent_spec* opponent;

        pFrontend->items[2].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;

        gFrontend_wrecks_current = KeepInRange((gFrontend_wrecks_hscroll / 30) % gFrontend_wrecks_car_count, gFrontend_wrecks_car_count);

        car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
        opponent = GetOpponentSpecFromCarSpec(car);
        if (!gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
            int got_it_already;

            c2_sprintf(pFrontend->items[2].text, "%s @R%s",
                gOpponents[opponent->index].car_name,
                gOpponents[opponent->index].line4_description);
            MungeMetaCharactersChar(pFrontend->items[2].text, 'R', '\r');
            got_it_already = GotItAlready(car);
            pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[2].visible = 1;
            if (!car->knackered) {
                c2_sprintf(pFrontend->items[8].text, "%s", GetMiscString(eMiscString_not_wasted));
            } else {
                int price = gOpponents[opponent->index].price;

                if (price > gProgram_state.credits) {
                    c2_sprintf(pFrontend->items[8].text, "%s - %s",
                        MungeCommas(price),
                        GetMiscString(eMiscString_dream_on));
                } else if (got_it_already) {
                    c2_sprintf(pFrontend->items[8].text, "%s",
                        GetMiscString(eMiscString_already_have_it));
                } else {
                    c2_strcpy(pFrontend->items[8].text, MungeCommas(price));
                    pFrontend->items[9].enabled = kFrontendItemEnabled_enabled;
                }
            }
            if (gFrontend_opponent_profile_pic_needs_update) {
                MenuSetDriverImage(opponent->index, 11);
                gFrontend_opponent_profile_pic_needs_update = 0;
            }
            FuckWithWidths(pFrontend);
        } else {
            pFrontend->items[2].visible = 0;
        }

        if (gFrontend_mouse_down && gFrontend_wrecks_pending_hscroll == 0) {
            int selected_car;

            if (gFrontend_wrecks_rotate_prev_x != -1 && gFrontend_wrecks_rotate_prev_y != -1) {
                BrMatrix34RollingBall(&gWreck_gallery_car_infos[gFrontend_wrecks_current].field_0x14,
                    mouse_x - gFrontend_wrecks_rotate_prev_x,
                    mouse_y - gFrontend_wrecks_rotate_prev_y,
                    50);
            }
            selected_car = GetCarSelectedByMouse(mouse_x, mouse_y);
            if (selected_car != -1) {
                if (selected_car == gFrontend_wrecks_current) {
                    if (gFrontend_wrecks_rotate_prev_x == -1) {
                        gFrontend_wrecks_rotate_prev_x = mouse_x;
                        gFrontend_wrecks_rotate_prev_y = mouse_y;
                    }
                } else if (!gPrev_frontend_mouse_down) {
                    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);

                    if (gFrontend_wrecks_current == KeepInRange(selected_car + 2, gFrontend_wrecks_car_count) && gFrontend_wrecks_car_count >= 2) {
                        gFrontend_wrecks_pending_hscroll += 60;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car + 1, gFrontend_wrecks_car_count)) {
                        gFrontend_wrecks_pending_hscroll += 30;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car - 2, gFrontend_wrecks_car_count) && gFrontend_wrecks_car_count >= 2) {
                        gFrontend_wrecks_pending_hscroll -= 60;
                    } else if (gFrontend_wrecks_current == KeepInRange(selected_car - 1, gFrontend_wrecks_car_count)) {
                        gFrontend_wrecks_pending_hscroll -= 30;
                    }
                }
            }
        }
        if (!gFrontend_mouse_down) {
            gFrontend_wrecks_rotate_prev_x = -1;
            gFrontend_wrecks_rotate_prev_y = -1;
        }
    } else {
        pFrontend->items[2].visible = 0;
        pFrontend->items[2].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
    }

    count_visible_cars = MIN(5, gFrontend_wrecks_car_count);
    rotate_angle = count_visible_cars / 2 * -30 - gFrontend_wrecks_hscroll % 30;

    if (gFrontend_wrecks_car_count == 1) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    }

    index_first_car = KeepInRange((gFrontend_wrecks_hscroll / 30) % gFrontend_wrecks_car_count - count_visible_cars / 2, gFrontend_wrecks_car_count);

    for (i = 0; i < gFrontend_wrecks_car_count; i++) {
        gWreck_gallery_car_infos[i].actor->render_style = BR_RSTYLE_NONE;
    }

    for (i = 0; i < count_visible_cars; i++) {
        int car_index;
        br_matrix34 *mat;
        br_vector3 pos;

        car_index = KeepInRange(index_first_car + i, gFrontend_wrecks_car_count);
        mat = &gWreck_gallery_car_infos[car_index].actor->t.t.mat;
        BrMatrix34RotateY(mat, BrDegreeToAngle(rotate_angle + 30 * i));
        BrMatrix34PostRotateX(mat, BrDegreeToAngle(340));
        if (car_index == gFrontend_wrecks_current) {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f - gFrontend_wreck_bought_car_dz);
        } else {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f);
        }
        BrMatrix34Post(&gWreck_gallery_car_infos[car_index].field_0x74,
            &gWreck_gallery_car_infos[car_index].field_0x14);

        BrVector3Copy(&pos, &gWreck_gallery_car_infos[car_index].actor->t.t.translate.t);
        BrVector3Set(&gWreck_gallery_car_infos[car_index].actor->t.t.translate.t, 0.f, 0.f, 0.f);
        BrMatrix34Pre(mat, &gWreck_gallery_car_infos[car_index].field_0x74);
        BrVector3Copy(&gWreck_gallery_car_infos[car_index].actor->t.t.translate.t, &pos);
        gWreck_gallery_car_infos[car_index].actor-> render_style = BR_RSTYLE_FACES;
        if (gWreck_gallery_sell_infos[car_index].sold) {
            gWreck_gallery_car_infos[car_index].actor-> render_style = BR_RSTYLE_NONE;
        }
    }

    FuckWithWidths(pFrontend);
    if (PDKeyDown(51) || PDKeyDown(52) || (gFrontend_mouse_down && item_at_mouse != -1)) {

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        if (pFrontend->items[gFrontend_selected_item_index].menuInfo != NULL) {

            gCurrent_frontend_spec->default_item = gFrontend_selected_item_index;
            for (i = 0; i < gCurrent_frontend_spec->count_scrollers; i++) {
                gCurrent_frontend_spec->scrollers[i].indexOfItemAtTop = gCurrent_frontend_spec->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            gCurrent_frontend_spec = pFrontend->items[gFrontend_selected_item_index].menuInfo;
            FRONTEND_CreateMenu(gCurrent_frontend_spec);
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
        }
    }
    if (gFrontend_leave_current_menu != 0) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0046e720
int C2_HOOK_FASTCALL ScrollToPrevCar(tFrontend_spec* pFrontend) {

    if (gFrontend_wreck_bought_car_dz == 0.f
            && gFrontend_wrecks_pending_hscroll == 0.f
            && (gFrontend_wrecks_rotate_prev_x == -1 || gFrontend_wrecks_rotate_prev_y == -1)) {

        gFrontend_wrecks_pending_hscroll = 30;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_opponent_profile_pic_needs_update = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e6c0
int C2_HOOK_FASTCALL ScrollToNextCar(tFrontend_spec* pFrontend) {

    if (gFrontend_wreck_bought_car_dz == 0.f
            && gFrontend_wrecks_pending_hscroll == 0.f
            && (gFrontend_wrecks_rotate_prev_x == -1 || gFrontend_wrecks_rotate_prev_y == -1)) {

        gFrontend_wrecks_pending_hscroll = -30;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_opponent_profile_pic_needs_update = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e780
int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend) {

    if (gFrontend_wreck_bought_car_dz == 0.f
            && !gWreck_gallery_sell_infos[gFrontend_wrecks_current].sold) {
        tCar_spec*car;
        tOpponent_spec* opponent;

        car = GetCarSpec(eVehicle_opponent, gFrontend_wrecks_current);
        opponent = GetOpponentSpecFromCarSpec(car);
        gProgram_state.cars_available[gProgram_state.number_of_cars] = car->index;
        gProgram_state.number_of_cars += 1;
        gProgram_state.current_car_index = car->index;
        gProgram_state.credits -= gOpponents[opponent->index].price;
        gFrontend_wreck_bought_car_dz = 0.001f;
        DRS3StartSound(gEffects_outlet, eSoundId_Done);
    }
    return 0;
}
