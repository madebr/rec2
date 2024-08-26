#include "frontend_main.h"

#include "frontend.h"
#include "globvars.h"
#include "graphics.h"
#include "loading.h"
#include "utility.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_MAIN, 0x005a80f0, {
    FIXME TODO
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrontend_car_image_outdated, 0x00687040);

void C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend) {

    if (C2V(gIs_boundary_race) || C2V(gProgram_state).game_completed) {
        pFrontend->items[7].enabled = 1;
    } else {
        pFrontend->items[7].enabled = -1;
    }
}

int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend) {
    int group;
    int race_index;
    int i;
    char group_name[12];
    br_camera* camera;

    DefaultInfunc(pFrontend);
    ResetInterfaceTimeout();
    group = (C2V(gCurrent_race_group) - C2V(gRaceGroups)) % 10;
    c2_sprintf(group_name, "%s %d", IString_Get(78), group + 1);
    c2_strcpy(pFrontend->items[2].text, group_name);
    race_index = 4 * group;
    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++, race_index += 1) {
        tFrontend_item_spec* item = &pFrontend->items[i];

        c2_strcpy(item->text, C2V(gRace_list)[race_index].name);
        item->radioButton_selected = race_index == C2V(gProgram_state).current_race_index;
        if (C2V(gRace_list)[race_index].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(pFrontend);
    FillInRaceDescription(pFrontend->items[22].text, C2V(gProgram_state).current_race_index);
    c2_strcpy(pFrontend->items[18].text, C2V(gProgram_state).player_name);
    c2_strcpy(pFrontend->items[20].text, C2V(gOpponents)[C2V(gProgram_state).current_car_index].car_name);
    c2_strcpy(pFrontend->items[23].text, MungeCommas(C2V(gProgram_state).credits));

    C2V(gFrontend_car_image_outdated) = 1;
    pFrontend->isPreviousSomeOtherMenu = 1;
    C2V(gMouse_in_use) = 0;
    C2V(gFrontend_menu_camera) = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
    camera = C2V(gFrontend_menu_camera)->type_data;
    camera->type = BR_CAMERA_PARALLEL;
    camera->field_of_view = BrDegreeToAngle(90);
    camera->hither_z = 1.f;
    camera->yon_z = 3.f;
    camera->width = 640.f;
    camera->height = 480.f;

    C2V(gFrontend_billboard_actors)[0] = CreateAPOactor();
    C2V(gFrontend_APO_Colour_1) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0xff, 0x80, 0x00, 0xff);
    C2V(gFrontend_APO_Colour_2) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0x00, 0x00, 0x80, 0xff);
    C2V(gFrontend_APO_Colour_3) = FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(C2V(gFrontend_billboard_actors)[0]->material->colour_map, 0x00, 0x00, 0xff, 0xff);
    FuckWithWidths(pFrontend);
    return 1;
}
C2_HOOK_FUNCTION(0x00469a40, MainMenuInfunc)
