#include "frontend_wrecks.h"

#include "frontend.h"
#include "globvars.h"
#include "intrface.h"
#include "opponent.h"
#include "platform.h"
#include "sound.h"

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
