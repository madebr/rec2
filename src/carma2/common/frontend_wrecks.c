#include "frontend_wrecks.h"

#include "frontend.h"
#include "frontend_loadgame.h"
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


C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_WRECKS, 0x005f8e68, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tWreck_gallery_sell_info, gWreck_gallery_sell_infos, 32, 0x00763720);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tWreck_gallery_car_info, gWreck_gallery_car_infos, 30, 0x00763b5c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gFrontend_wrecks_light, 0x00763844);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_wrecks_car_count, 0x00705190);
C2_HOOK_VARIABLE_IMPLEMENT(float, gFrontend_wreck_bought_car_dz, 0x00688af8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_wrecks_current, 0x00763890);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_wrecks_hscroll, 0x00688afc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_wrecks_pending_hscroll, 0x00688b00);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gFrontend_wrecks_previous_update, 0x007635ec);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFrontend_wrecks_rotate_prev_x, 0x0059b0dc, -1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFrontend_wrecks_rotate_prev_y, 0x0059b0e0, -1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_opponent_profile_pic_needs_update, 0x00686828);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHierarchy_has_actor, 0x00763924);
C2_HOOK_VARIABLE_IMPLEMENT(int, gPicked_wreck, 0x00763928);


int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend) {
    br_camera* camera;
    int car_count;
    int i;

    C2_HOOK_BUG_ON(sizeof(C2V(gWreck_gallery_sell_infos)) != 0x100);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_car_info) != 0xa4);
    C2_HOOK_BUG_ON(sizeof(tWreck_gallery_sell_info) != 0x8);

    C2V(gCurrent_frontend_spec) = pFrontend;
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingin);
    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    C2V(gFrontend_wrecks_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    C2V(gFrontend_wrecks_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    C2V(gFrontend_wrecks_light) = BrActorAllocate(BR_ACTOR_LIGHT, NULL);

    camera = C2V(gFrontend_wrecks_camera)->type_data;
    camera->aspect = 1.6346154f;
    camera->field_of_view = BrDegreeToAngle(55);
    camera->hither_z = 1.f;
    camera->yon_z = 100.f;
    camera->type = BR_CAMERA_PERSPECTIVE_FOV;
    camera->width = 640.f;
    camera->height = 480.f;
    BrActorAdd(C2V(gFrontend_wrecks_actor), C2V(gFrontend_wrecks_camera));

    C2V(gFrontend_wrecks_pixelmap) = BrPixelmapAllocateSub(C2V(gBack_screen), 65, 55, 510, 312);
    C2V(gFrontend_wrecks_pixelmap)->origin_x = 255;
    C2V(gFrontend_wrecks_pixelmap)->origin_y = 120;

    BrMatrix34RotateY(&C2V(gFrontend_wrecks_camera)->t.t.mat, BrDegreeToAngle(180));
    BrMatrix34PostTranslate(&C2V(gFrontend_wrecks_camera)->t.t.mat, 0.f, 0.f, -3.f);

    car_count = GetCarCount(eVehicle_opponent);
    C2V(gFrontend_wrecks_car_count) = 0;

    for (i = 0; i < car_count; i++) {
        tCar_spec* car;
        br_actor* actor;

        car = GetCarSpec(eVehicle_opponent, i);
        actor = car->car_master_actor;
        actor->render_style = BR_RSTYLE_FACES;
        if (actor->parent != NULL) {
            BrActorRemove(actor);
        }
        BrActorAdd(C2V(gFrontend_wrecks_actor), actor);
        C2V(gWreck_gallery_car_infos)[i].original_matrix = actor->t.t.mat;
        BrMatrix34Identity(&actor->t.t.mat);
        BrMatrix34Identity(&C2V(gWreck_gallery_car_infos)[i].field_0x14);
        BrMatrix34Identity(&C2V(gWreck_gallery_car_infos)[i].field_0x74);
        C2V(gWreck_gallery_car_infos)[i].actor = actor;
        C2V(gFrontend_wrecks_car_count) += 1;
    }
    C2V(gFrontend_wreck_bought_car_dz) = 0.f;
    c2_memset(C2V(gWreck_gallery_sell_infos), 0, sizeof(C2V(gWreck_gallery_sell_infos)));

    C2V(gFrontend_selected_item_index) = 0;
    C2V(gFrontend_wrecks_current) = 0;
    C2V(gFrontend_wrecks_hscroll) = 0;
    C2V(gFrontend_wrecks_pending_hscroll) = 0;
    C2V(gFrontend_wrecks_previous_update) = PDGetTotalTime();
    C2V(gFrontend_wrecks_rotate_prev_x) = -1;
    C2V(gFrontend_wrecks_rotate_prev_y) = -1;
    FuckWithWidths(pFrontend);
    C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
    return 1;
}
C2_HOOK_FUNCTION(0x0046e830, WrecksInFunc)

void C2_HOOK_FASTCALL DisposeWrecksGallery(void) {
    int i;

    for (i = 0; i < C2V(gFrontend_wrecks_car_count); i++) {
        BrActorRemove(C2V(gWreck_gallery_car_infos)[i].actor);
    }
    if (C2V(gFrontend_wrecks_camera) != NULL) {
        if (C2V(gFrontend_wrecks_camera)->parent != NULL) {
            BrActorRemove(C2V(gFrontend_wrecks_camera));
        }
        BrActorFree(C2V(gFrontend_wrecks_camera));
    }
    if (C2V(gFrontend_wrecks_actor) != NULL) {
        if (C2V(gFrontend_wrecks_actor)->parent != NULL) {
            BrActorRemove(C2V(gFrontend_wrecks_actor));
        }
        BrActorFree(C2V(gFrontend_wrecks_actor));
    }
    C2V(gFrontend_wrecks_actor) = NULL;
    C2V(gFrontend_wrecks_camera) = NULL;
}
C2_HOOK_FUNCTION(0x0046e620, DisposeWrecksGallery)

int C2_HOOK_FASTCALL WrecksOutFunc(tFrontend_spec* pFrontend) {

    DisposeWrecksGallery();
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_Swingout);
    return 1;
}
C2_HOOK_FUNCTION(0x0046ead0, WrecksOutFunc)

int C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref) {

    if (a == (br_actor*)ref) {
        C2V(gHierarchy_has_actor) = 1;
    }
    BrActorEnum(a, HeirarchyPick, ref);
    return 0;
}
C2_HOOK_FUNCTION(0x0046f560, HeirarchyPick)

int C2_HOOK_CDECL WreckPick(br_actor* world, br_model* model, br_material* material, br_vector3* pos, br_vector3* dir, br_scalar near, br_scalar far, void* arg) {
    int i;

    for (i = 0; i < C2V(gFrontend_wrecks_car_count); i++) {
        C2V(gHierarchy_has_actor) = 0;
        BrActorEnum(C2V(gWreck_gallery_car_infos)[i].actor, HeirarchyPick, world);
        if (C2V(gHierarchy_has_actor)) {
            C2V(gPicked_wreck) = i;
            return 1;
        }
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046f4f0, WreckPick)

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

    for (i = 0; i < C2V(gProgram_state).number_of_cars; i++) {
        if (C2V(gProgram_state).cars_available[i] == pCar->index) {
            return 1;
        }
    }
    return 0;
}

int C2_HOOK_FASTCALL GetCarSelectedByMouse(int pX, int pY) {

    C2V(gPicked_wreck) = -1;
    DRScenePick2DXY(C2V(gFrontend_wrecks_actor),
        C2V(gFrontend_wrecks_camera),
        C2V(gFrontend_wrecks_pixelmap),
        pX - C2V(gFrontend_wrecks_pixelmap)->origin_x - 65,
        pY - C2V(gFrontend_wrecks_pixelmap)->origin_y - 55,
        WreckPick,
        NULL);
    return C2V(gPicked_wreck);
}

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
    C2V(gFrontend_mouse_down) = 0;
    if (C2V(gMouse_in_use)) {
        C2V(gFrontend_selected_item_index) = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
        item_at_mouse = GetItemAtMousePos(pFrontend, mouse_x, mouse_y);
        if (item_at_mouse != -1) {
            C2V(gFrontend_selected_item_index) = item_at_mouse;
        }
        C2V(gPrev_frontend_mouse_down) = C2V(gFrontend_mouse_down);
        C2V(gFrontend_mouse_down) = EitherMouseButtonDown();
    }
    if (C2V(gFrontend_selected_item_index) == 0) {
        C2V(gFrontend_selected_item_index) = 10;
    }

    now = PDGetTotalTime();
    if (C2V(gFrontend_wreck_bought_car_dz) != 0.f) {
        int dt;

        dt = now - C2V(gFrontend_wrecks_previous_update);
        C2V(gFrontend_wreck_bought_car_dz) += 0.0025f * (float)dt;
        if (C2V(gFrontend_wreck_bought_car_dz) > 2.5f) {
            C2V(gFrontend_wreck_bought_car_dz) = 0.f;
            C2V(gWreck_gallery_sell_infos)[C2V(gFrontend_wrecks_current)].sold = 1;
        }
    }

    if (C2V(gFrontend_wrecks_pending_hscroll) > 0) {
        C2V(gFrontend_wrecks_pending_hscroll) -= 5;
        C2V(gFrontend_wrecks_hscroll) -= 5;
        if (C2V(gFrontend_wrecks_pending_hscroll) < 0) {
            C2V(gFrontend_wrecks_pending_hscroll) = 0;
        }
    }
    if (C2V(gFrontend_wrecks_pending_hscroll) < 0) {
        C2V(gFrontend_wrecks_pending_hscroll) += 5;
        C2V(gFrontend_wrecks_hscroll) += 5;
        if (C2V(gFrontend_wrecks_pending_hscroll) > 0) {
            C2V(gFrontend_wrecks_pending_hscroll) = 0;
        }
    }

    C2V(gFrontend_wrecks_previous_update) = now;

    c2_strcpy(pFrontend->items[6].text, MungeCommas(C2V(gProgram_state).credits));

    if (C2V(gFrontend_wrecks_hscroll) % 30 == 0 && C2V(gFrontend_wreck_bought_car_dz) == 0.f) {
        tCar_spec* car;
        tOpponent_spec* opponent;

        pFrontend->items[2].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;

        C2V(gFrontend_wrecks_current) = KeepInRange((C2V(gFrontend_wrecks_hscroll) / 30) % C2V(gFrontend_wrecks_car_count), C2V(gFrontend_wrecks_car_count));

        car = GetCarSpec(eVehicle_opponent, C2V(gFrontend_wrecks_current));
        opponent = GetOpponentSpecFromCarSpec(car);
        if (!C2V(gWreck_gallery_sell_infos)[C2V(gFrontend_wrecks_current)].sold) {
            int got_it_already;

            c2_sprintf(pFrontend->items[2].text, "%s @R%s",
                C2V(gOpponents)[opponent->index].car_name,
                C2V(gOpponents)[opponent->index].line4_description);
            MungeMetaCharactersChar(pFrontend->items[2].text, 'R', '\r');
            got_it_already = GotItAlready(car);
            pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
            pFrontend->items[2].visible = 1;
            if (!car->knackered) {
                c2_sprintf(pFrontend->items[8].text, "%s", GetMiscString(eMiscString_not_wasted));
            } else {
                int price = C2V(gOpponents)[opponent->index].price;

                if (price > C2V(gProgram_state).credits) {
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
            if (C2V(gFrontend_opponent_profile_pic_needs_update)) {
                MenuSetDriverImage(opponent->index, 11);
                C2V(gFrontend_opponent_profile_pic_needs_update) = 0;
            }
            FuckWithWidths(pFrontend);
        } else {
            pFrontend->items[2].visible = 0;
        }

        if (C2V(gFrontend_mouse_down) && C2V(gFrontend_wrecks_pending_hscroll) == 0) {
            int selected_car;

            if (C2V(gFrontend_wrecks_rotate_prev_x) != -1 && C2V(gFrontend_wrecks_rotate_prev_y) != -1) {
                BrMatrix34RollingBall(&C2V(gWreck_gallery_car_infos)[C2V(gFrontend_wrecks_current)].field_0x14,
                    mouse_x - C2V(gFrontend_wrecks_rotate_prev_x),
                    mouse_y - C2V(gFrontend_wrecks_rotate_prev_y),
                    50);
            }
            selected_car = GetCarSelectedByMouse(mouse_x, mouse_y);
            if (selected_car != -1) {
                if (selected_car == C2V(gFrontend_wrecks_current)) {
                    if (C2V(gFrontend_wrecks_rotate_prev_x) == -1) {
                        C2V(gFrontend_wrecks_rotate_prev_x) = mouse_x;
                        C2V(gFrontend_wrecks_rotate_prev_y) = mouse_y;
                    }
                } else if (!C2V(gPrev_frontend_mouse_down)) {
                    DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);

                    if (C2V(gFrontend_wrecks_current) == KeepInRange(selected_car + 2, C2V(gFrontend_wrecks_car_count)) && C2V(gFrontend_wrecks_car_count) >= 2) {
                        C2V(gFrontend_wrecks_pending_hscroll) += 60;
                    } else if (C2V(gFrontend_wrecks_current) == KeepInRange(selected_car + 1, C2V(gFrontend_wrecks_car_count))) {
                        C2V(gFrontend_wrecks_pending_hscroll) += 30;
                    } else if (C2V(gFrontend_wrecks_current) == KeepInRange(selected_car - 2, C2V(gFrontend_wrecks_car_count)) && C2V(gFrontend_wrecks_car_count) >= 2) {
                        C2V(gFrontend_wrecks_pending_hscroll) -= 60;
                    } else if (C2V(gFrontend_wrecks_current) == KeepInRange(selected_car - 1, C2V(gFrontend_wrecks_car_count))) {
                        C2V(gFrontend_wrecks_pending_hscroll) -= 30;
                    }
                }
            }
        }
        if (!C2V(gFrontend_mouse_down)) {
            C2V(gFrontend_wrecks_rotate_prev_x) = -1;
            C2V(gFrontend_wrecks_rotate_prev_y) = -1;
        }
    } else {
        pFrontend->items[2].visible = 0;
        pFrontend->items[2].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[9].enabled = kFrontendItemEnabled_disabled;
    }

    count_visible_cars = MIN(5, C2V(gFrontend_wrecks_car_count));
    rotate_angle = count_visible_cars / 2 * -30 - C2V(gFrontend_wrecks_hscroll) % 30;

    if (C2V(gFrontend_wrecks_car_count) == 1) {
        pFrontend->items[3].enabled = kFrontendItemEnabled_disabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    }

    index_first_car = KeepInRange((C2V(gFrontend_wrecks_hscroll) / 30) % C2V(gFrontend_wrecks_car_count) - count_visible_cars / 2, C2V(gFrontend_wrecks_car_count));

    for (i = 0; i < C2V(gFrontend_wrecks_car_count); i++) {
        C2V(gWreck_gallery_car_infos)[i].actor->render_style = BR_RSTYLE_NONE;
    }

    for (i = 0; i < count_visible_cars; i++) {
        int car_index;
        br_matrix34 *mat;
        br_vector3 pos;

        car_index = KeepInRange(index_first_car + i, C2V(gFrontend_wrecks_car_count));
        mat = &C2V(gWreck_gallery_car_infos)[car_index].actor->t.t.mat;
        BrMatrix34RotateY(mat, BrDegreeToAngle(rotate_angle + 30 * i));
        BrMatrix34PostRotateX(mat, BrDegreeToAngle(340));
        if (car_index == C2V(gFrontend_wrecks_current)) {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f - C2V(gFrontend_wreck_bought_car_dz));
        } else {
            BrMatrix34PreTranslate(mat, 0.f, 0.f, -1.5f);
        }
        BrMatrix34Post(&C2V(gWreck_gallery_car_infos)[car_index].field_0x74,
            &C2V(gWreck_gallery_car_infos)[car_index].field_0x14);

        BrVector3Copy(&pos, &C2V(gWreck_gallery_car_infos)[car_index].actor->t.t.translate.t);
        BrVector3Set(&C2V(gWreck_gallery_car_infos)[car_index].actor->t.t.translate.t, 0.f, 0.f, 0.f);
        BrMatrix34Pre(mat, &C2V(gWreck_gallery_car_infos[car_index]).field_0x74);
        BrVector3Copy(&C2V(gWreck_gallery_car_infos)[car_index].actor->t.t.translate.t, &pos);
        C2V(gWreck_gallery_car_infos)[car_index].actor-> render_style = BR_RSTYLE_FACES;
        if (C2V(gWreck_gallery_sell_infos)[car_index].sold) {
            C2V(gWreck_gallery_car_infos)[car_index].actor-> render_style = BR_RSTYLE_NONE;
        }
    }

    FuckWithWidths(pFrontend);
    if (PDKeyDown(51) || PDKeyDown(52) || (C2V(gFrontend_mouse_down) && item_at_mouse != -1)) {

        ToggleSelection(pFrontend);
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].action != NULL) {
            pFrontend->items[C2V(gFrontend_selected_item_index)].action(pFrontend);
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].field_0xc == 1) {
            C2V(gFrontend_leave_current_menu) = 1;
        }
        if (pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo != NULL) {

            C2V(gCurrent_frontend_spec)->default_item = C2V(gFrontend_selected_item_index);
            for (i = 0; i < C2V(gCurrent_frontend_spec)->count_scrollers; i++) {
                C2V(gCurrent_frontend_spec)->scrollers[i].indexOfItemAtTop = C2V(gCurrent_frontend_spec)->scrollers[i].indexTopItem;
            }
            FRONTEND_DestroyMenu(pFrontend);
            C2V(gCurrent_frontend_spec) = pFrontend->items[C2V(gFrontend_selected_item_index)].menuInfo;
            FRONTEND_CreateMenu(C2V(gCurrent_frontend_spec));
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
        }
    }
    if (C2V(gFrontend_leave_current_menu) != 0) {
        return 1;
    } else {
        return 0;
    }
}
C2_HOOK_FUNCTION(0x0046eb80, WrecksUpdateFunc)

int C2_HOOK_FASTCALL ScrollToPrevCar(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_wreck_bought_car_dz) == 0.f
            && C2V(gFrontend_wrecks_pending_hscroll) == 0.f
            && (C2V(gFrontend_wrecks_rotate_prev_x) == -1 || C2V(gFrontend_wrecks_rotate_prev_y) == -1)) {

        C2V(gFrontend_wrecks_pending_hscroll) = 30;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046e720, ScrollToPrevCar)

int C2_HOOK_FASTCALL ScrollToNextCar(tFrontend_spec* pFrontend) {

    if (C2V(gFrontend_wreck_bought_car_dz) == 0.f
            && C2V(gFrontend_wrecks_pending_hscroll) == 0.f
            && (C2V(gFrontend_wrecks_rotate_prev_x) == -1 || C2V(gFrontend_wrecks_rotate_prev_y) == -1)) {

        C2V(gFrontend_wrecks_pending_hscroll) = -30;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
        C2V(gFrontend_opponent_profile_pic_needs_update) = 1;
    }
    return 0;
}
C2_HOOK_FUNCTION(0x0046e6c0, ScrollToNextCar)
