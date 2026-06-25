#include "72-interface.h"

#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "15-displays.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "24-loadsave.h"
#include "35-intrface.h"
#include "40-main.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "72-interface-main.h"
#include "72-interface-network.h"
#include "72-interface-quit.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"
#include <ctype.h>

// GLOBAL: CARMA2_HW 0x00764eec
tFrontendMenuType gFrontend_next_menu;

// GLOBAL: CARMA2_HW 0x00688b20
int gFrontend_suppress_mouse;

// GLOBAL: CARMA2_HW 0x00688ab0
tConnected_items* gConnected_items;

// GLOBAL: CARMA2_HW 0x00686508
tStruct_00686508* gPTR_00686508;

// GLOBAL: CARMA2_HW 0x00686820
tFrontend_slider* gCurrent_frontend_scrollbars;

// GLOBAL: CARMA2_HW 0x00604730
tFrontend_item_spec gDefault_last_interface_item = {
    0x401, temp, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

// GLOBAL: CARMA2_HW 0x006864f4
int gFrontend_scrollbars_updated;

// GLOBAL: CARMA2_HW 0x00686850
br_pixelmap* gFrontend_images[100];

// GLOBAL: CARMA2_HW 0x00687248
tFrontend_brender_item gFrontend_brender_items[100]; /* FIXME: parametrize size + index of last item */

// GLOBAL: CARMA2_HW 0x00688770
int gFrontend_selected_item_index;

// GLOBAL: CARMA2_HW 0x0068b8a0
tRace_group_spec* gRace_groups;

// GLOBAL: CARMA2_HW 0x00688af4
char* gFrontend_current_input;

// GLOBAL: CARMA2_HW 0x0068723c
int gFrontend_text_input_item_index;

// GLOBAL: CARMA2_HW 0x00763900
char gFrontend_original_player_name[32];

// GLOBAL: CARMA2_HW 0x0059b0d8
int gINT_0059b0d8 = -1;

// GLOBAL: CARMA2_HW 0x0059b0d4
int gFrontend_maximum_input_length = 9;

// GLOBAL: CARMA2_HW 0x00763924
int gHierarchy_has_actor;

// GLOBAL: CARMA2_HW 0x00686834
tU32 gCredits_scroll_start;

// GLOBAL: CARMA2_HW 0x006883c0
int gCredits_line_count;

// GLOBAL: CARMA2_HW 0x00686f04
int* gCredits_heights;

// GLOBAL: CARMA2_HW 0x00687238
int* gCredits_throbs;

// GLOBAL: CARMA2_HW 0x00688448
char** gCredits_texts;

// GLOBAL: CARMA2_HW 0x0068682c
int* gCredits_fonts;

// GLOBAL: CARMA2_HW 0x00686f90
int gCredits_total_height;

// GLOBAL: CARMA2_HW 0x006886d0
double gFrontend_throb_factor;

// GLOBAL: CARMA2_HW 0x00763940
br_actor* gFrontend_billboard_actors[8];

// GLOBAL: CARMA2_HW 0x00688b10
br_actor* gFrontend_menu_camera;

// GLOBAL: CARMA2_HW 0x007638ac
int gFrontend_load_game_index_top;

// GLOBAL: CARMA2_HW 0x00763898
int gFrontend_scroll_time_left;

// GLOBAL: CARMA2_HW 0x0076389c
int gFrontend_scroll_time_increment;

// GLOBAL: CARMA2_HW 0x00764e9c
int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x00763894
tU32 gFrontend_scroll_last_update;

// GLOBAL: CARMA2_HW 0x00688b04
int gFrontend_mouse_down;

// GLOBAL: CARMA2_HW 0x0068875c
tU32 gFrontend_time_last_input;

// GLOBAL: CARMA2_HW 0x00764ee4
int gNet_join_host_result;  // FIXME: use union instead?

// GLOBAL: CARMA2_HW 0x00688b08
int gPrev_frontend_mouse_down;

// GLOBAL: CARMA2_HW 0x006883a8
int gFrontend_leave_current_menu;

// GLOBAL: CARMA2_HW 0x00688abc
tFrontend_spec* gCurrent_frontend_spec;

// GLOBAL: CARMA2_HW 0x007635f0
br_uint_32 gFrontend_APO_Colour_1;

// GLOBAL: CARMA2_HW 0x007635e0
br_uint_32 gFrontend_APO_Colour_2;

// GLOBAL: CARMA2_HW 0x00763700
br_uint_32 gFrontend_APO_Colour_3;

// GLOBAL: CARMA2_HW 0x0068650c
br_actor* gFrontend_actor;

// GLOBAL: CARMA2_HW 0x0068683c
int gFrontend_count_brender_items;

// GLOBAL: CARMA2_HW 0x00764ee0
int gFrontend_remove_current_backdrop;

// GLOBAL: CARMA2_HW 0x00686f8c
br_pixelmap* gFrontend_backdrop;

// GLOBAL: CARMA2_HW 0x00688ae8
br_colour gFrontend_some_color;

// GLOBAL: CARMA2_HW 0x006870b8
tFrontend_model gFrontend_A_models[13];

// GLOBAL: CARMA2_HW 0x00687188
tFrontend_model gFrontend_B_models[11];

// GLOBAL: CARMA2_HW 0x00687058
tFrontend_model gFrontend_C_models[6];

// GLOBAL: CARMA2_HW 0x00688378
br_model* gFrontend_A_model_from;

// GLOBAL: CARMA2_HW 0x0068844c
br_model* gFrontend_A_model_to;

// GLOBAL: CARMA2_HW 0x00688768
br_model* gFrontend_B_model_from;

// GLOBAL: CARMA2_HW 0x006886e0
br_model* gFrontend_B_model_to;

// GLOBAL: CARMA2_HW 0x00686504
br_model* gFrontend_C_model_from;

// GLOBAL: CARMA2_HW 0x006886bc
br_model* gFrontend_C_model_to;

// GLOBAL: CARMA2_HW 0x006864e0
int gFrontend_backdrop0_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864dc
int gFrontend_backdrop1_opacity_mode;

// GLOBAL: CARMA2_HW 0x006864d4
int gFrontend_backdrop2_opacity_mode;

// GLOBAL: CARMA2_HW 0x00686ef8
int gFrontend_interpolate_steps_left;

// FUNCTION: CARMA2_HW 0x00466450
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {

    return 0;
}

// int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ce0
void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pButton_index) {
    br_model* model;
    int item_index;
    int i;

    for (i = 0; i < pFrontend->scrollers[pButton_index].nbDisplayedAtOnce; i++) {
        item_index = pFrontend->scrollers[pButton_index].indexFirstScrollableItem + i;
        model = gFrontend_brender_items[item_index].model;

        gFrontend_brender_items[item_index].model->vertices[2].p.v[0] = gFrontend_brender_items[item_index].model->vertices[3].p.v[0] = gFrontend_brender_items[item_index].model->vertices[1].p.v[0] + pFrontend->items[item_index].width;
        gFrontend_brender_items[item_index].model->vertices[1].p.v[1] = gFrontend_brender_items[item_index].model->vertices[2].p.v[1] = gFrontend_brender_items[item_index].model->vertices[3].p.v[1] - pFrontend->items[item_index].height;
        BrModelUpdate(model, BR_MODU_VERTEX_POSITIONS);
    }
}

// ScrollUp

// ScrollDn

static void C2_HOOK_FASTCALL SetLevelBar(tFrontend_spec* pFrontend, int pLevel_index, int pSelected_item) {
    int i;

    for (i = pFrontend->levels[pLevel_index - 1].first_item_id; i <= pFrontend->levels[pLevel_index - 1].last_item_id; i++) {

        if (i <= pSelected_item) {
            BrPixelmapCopy(gFrontend_brender_items[i].field_0xc, gFrontend_images[4]);
            BrPixelmapCopy(gFrontend_brender_items[i].field_0x10, gFrontend_images[4]);
        } else {
            BrPixelmapCopy(gFrontend_brender_items[i].field_0xc, gFrontend_images[5]);
            BrPixelmapCopy(gFrontend_brender_items[i].field_0x10, gFrontend_images[5]);
        }
        pFrontend->items[i].radioButton_selected = 0;
    }
    pFrontend->items[pSelected_item].radioButton_selected = 1;
}

// FUNCTION: CARMA2_HW 0x00467890
void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend) {
    int i;

    if (pFrontend->items[gFrontend_selected_item_index].group != 0) {
        for (i = 0; i < pFrontend->count_items; i++) {
            if (pFrontend->items[i].group == pFrontend->items[gFrontend_selected_item_index].group && pFrontend->items[gFrontend_selected_item_index].selectable != kFrontendSelectableButton) {
                pFrontend->items[i].radioButton_selected = 0;
            }
        }
        if (pFrontend->items[gFrontend_selected_item_index].selectable != kFrontendSelectableButton) {
            pFrontend->items[gFrontend_selected_item_index].radioButton_selected = !pFrontend->items[gFrontend_selected_item_index].radioButton_selected;
            DRS3StartSound(gEffects_outlet, eSoundId_Done);
        }
    } else {
        if (pFrontend->items[gFrontend_selected_item_index].idLevelBar != 0) {
            SetLevelBar(pFrontend, pFrontend->items[gFrontend_selected_item_index].idLevelBar, gFrontend_selected_item_index);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004666f0
void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend) {
    int i;
    const char* text;

    // item = pFrontend->items;
    for (i = 0; i < pFrontend->count_items; i++) {
#ifdef REC2_FIX_BUGS
        text = NULL;
#endif

        if (pFrontend->items[i].stringId > 0x400) {
            if (pFrontend->items[i].stringId == 0x404) {
                text = pFrontend->items[i].text;
            } else {
                continue;
            }
        } else {
            text = IString_Get(pFrontend->items[i].stringId);
        }
        if (text != NULL && !pFrontend->items[i].wrapText) {
            pFrontend->items[i].width = PolyFontTextWidth(pFrontend->items[i].unlitFont, text);
            pFrontend->items[i].height = PolyFontHeight(pFrontend->items[i].unlitFont);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0046abf0
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName) {
    br_pixelmap* pixelmaps[1000];
    br_pixelmap* result;
    FILE* f;
    tPath_name the_path;
    char* str;
    size_t count;
    size_t i;

    result = NULL;
    PathCat(the_path, pFolder, "PIXIES.P16");
    f = PFfopen(the_path, "rb");
    if (f != NULL) {
        PFfclose(f);
        count = BrPixelmapLoadMany(the_path, pixelmaps, REC2_ASIZE(pixelmaps));
        strcpy(the_path, pName);
        str = strchr(the_path, '.');
        *str = '\0';
        for (i = 0; i < count; i++) {
            if (pixelmaps[i] != NULL) {
                if (DRStricmp(pixelmaps[i]->identifier, the_path) == 0) {
                    result = pixelmaps[i];
                } else {
                    BrPixelmapFree(pixelmaps[i]);
                    pixelmaps[i] = NULL;
                }
            }
        }
        EnsurePixelmapAllowed(result, 0);
        return result;
    } else {
        PathCat(the_path, pFolder, pName);
        result = DRImageLoad(the_path);
        EnsurePixelmapAllowed(result, 0);
        return result;
    }
}

// FUNCTION: CARMA2_HW 0x00466760
void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend) {
    tPath_name path;
    char buffer[256];
    char* str;
    FILE* f;
    int count;
    int version;
    int item_count;
    int len_text;
    int i;
    int j;

    C2_HOOK_BUG_ON(sizeof(tFrontend_spec) != 0xb8c8);
    C2_HOOK_BUG_ON(sizeof(tFrontend_item_spec) != 0x158);
    C2_HOOK_BUG_ON(sizeof(tFrontend_scroller_spec) != 0x34);
    C2_HOOK_BUG_ON(sizeof(tFrontend_radio_spec) != 0x2c);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, items) != 0x130);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, scrollers) != 0x8794);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, radios) != 0x9be8);
    C2_HOOK_BUG_ON(offsetof(tFrontend_spec, levels) != 0xaf24);

    PathCat(path, gApplication_path, "INTRFACE");
    PathCat(path, path, pFrontend->name);
    strcat(path, ".TXT");

    f = DRfopen(path, "rt");
    if (f == NULL) {
#ifdef REC2_FIX_BUGS
        FatalError(kFatalError_Mysterious_SS, pFrontend->name, path);
#else
        FatalError(kFatalError_Mysterious_SS);
#endif
    }
    pFrontend->loaded = 1;
    GetALineAndDontArgue(f, buffer);
    str = strtok(buffer, "\t ,/");
    if (DRStricmp(str, "VERSION") != 0) {
        FatalError(kFatalError_FileMustStartWith_SS, pFrontend->name, "VERSION");
    }
    str = strtok(NULL, "\t ,/");
    count = sscanf(str, "%d", &version);
    if (count == 0 || version != 3) {
        FatalError(kFatalError_CantCopeWithVersionFor_SS, str, pFrontend->name);
    }
    GetAString(f, pFrontend->name);
    item_count = GetAnInt(f);
    if (item_count != pFrontend->count_items) {
        BrFailure("Error - Menu item number mismatch ", pFrontend->name);
    }
    pFrontend->count_groups = GetAnInt(f);
    GetAString(f, pFrontend->backdrop_name);
    for (i = 0; i < item_count; i++) {
        GetAString(f, pFrontend->items[i].text);
        if (DRStricmp("XXX", pFrontend->items[i].text) == 0) {
            strcpy(pFrontend->items[i].text, "");
        }
        len_text = strlen(pFrontend->items[i].text);
        for (j = 0; j < len_text; j++) {
            if (pFrontend->items[i].text[j] == '_') {
                pFrontend->items[i].text[j] = ' ';
            }
        }
        GetPairOfInts(f, &pFrontend->items[i].x, &pFrontend->items[i].y);
        pFrontend->items[i].x2 = pFrontend->items[i].x;
        pFrontend->items[i].y2 = pFrontend->items[i].y;
        GetPairOfInts(f, &pFrontend->items[i].width, &pFrontend->items[i].height);
        GetPairOfInts(f, &pFrontend->items[i].unlitFont, &pFrontend->items[i].highFont);
        pFrontend->items[i].map_index = GetAnInt(f);
        pFrontend->items[i].greyedBackdrop = GetAnInt(f);
        pFrontend->items[i].selectable = GetAnInt(f);
        pFrontend->items[i].group = GetAnInt(f);
        pFrontend->items[i].idLevelBar = GetAnInt(f);
        pFrontend->items[i].wrapText = GetAnInt(f);
        pFrontend->items[i].glowDisabled = GetAnInt(f);
    }
    // Copy item 99 (=last index)
    // FIXME: stringid = 0x401 (==> find out function of this item)
    memcpy(&pFrontend->items[REC2_ASIZE(pFrontend->items) - 1], &gDefault_last_interface_item, sizeof(tFrontend_item_spec));

    pFrontend->count_scrollers = GetAnInt(f);
    if (pFrontend->count_scrollers != 0) {
        for (i = 0; i < pFrontend->count_scrollers; i++) {
            GetPairOfInts(f, &pFrontend->scrollers[i].id, &pFrontend->scrollers[i].count);
            GetPairOfInts(f, &pFrontend->scrollers[i].nbDisplayedAtOnce, &pFrontend->scrollers[i].indexTopItem);
            GetPairOfInts(f, &pFrontend->scrollers[i].indexFirstScrollableItem, &pFrontend->scrollers[i].indexLastScrollableItem);
            pFrontend->scrollers[i].greyedOutBackdrop = GetAnInt(f);
            GetPairOfInts(f, &pFrontend->scrollers[i].x_greyArea, &pFrontend->scrollers[i].y_greyArea);
            GetPairOfInts(f, &pFrontend->scrollers[i].width_greyArea, &pFrontend->scrollers[i].height_greyArea);
            count = pFrontend->scrollers[i].nbDisplayedAtOnce;
            for (j = pFrontend->scrollers[i].indexTopItem; j <= pFrontend->scrollers[i].indexLastScrollableItem; j++) {
                pFrontend->items[j].visible = count > 0;
                count --;
            }
            pFrontend->scrollers[i].indexOfItemAtTop = pFrontend->scrollers[i].indexTopItem;
        }
    }

    pFrontend->count_radios = GetAnInt(f);
    if (pFrontend->count_radios != 0) {
        for (i = 0; i < pFrontend->count_radios; i++) {
            GetPairOfInts(f, &pFrontend->radios[i].id, &pFrontend->radios[i].count);
            GetPairOfInts(f, &pFrontend->radios[i].indexFirstItem, &pFrontend->radios[i].indexLastItem);
            GetPairOfInts(f, &pFrontend->radios[i].indexSelected, &pFrontend->radios[i].greyboxRequested);
            GetPairOfInts(f, &pFrontend->radios[i].x_greybox, &pFrontend->radios[i].y_greybox);
            GetPairOfInts(f, &pFrontend->radios[i].width_greybox, &pFrontend->radios[i].height_greybox);
        }
    }

    pFrontend->count_levels = GetAnInt(f);
    if (pFrontend->count_levels != 0) {
        for (i = 0; i < pFrontend->count_levels; i++) {
            GetPairOfInts(f, &pFrontend->levels[i].field_0x00, &pFrontend->levels[i].field_0x04);
            GetPairOfInts(f, &pFrontend->levels[i].first_item_id, &pFrontend->levels[i].last_item_id);
        }
    }

#ifdef REC2_FIX_BUGS
    PFfclose(f);
#else
    fclose(f);
#endif
}

// LoadMenuImages

// LoadMenuModels

void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend) {

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
        FuckWithWidths(pFrontend);
        if (pFrontend->previous != NULL) {
            pFrontend->previous->isPreviousSomeOtherMenu = 1;
        }
    }
    gFrontend_scrollbars_updated = 0;
    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
}

// FindNextActiveItem

// FUNCTION: CARMA2_HW 0x00467a30
int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index) {
    int i;

    for (i = pStart_index - 1; i >= 0; i--) {
        tFrontend_item_spec *item = &pFrontend->items[i];

        if (item->enabled > 0 && item->visible) {
            return i;
        }
    }
    return FindPrevActiveItem(pFrontend, pFrontend->count_items);
}

// RaceIndex

// FUNCTION: CARMA2_HW 0x00466bb0
int C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend) {
    int race_index;

    race_index = gProgram_state.current_race_index;
    if (!gIs_boundary_race && !gProgram_state.game_completed) {
        pFrontend->items[7].enabled = kFrontendItemEnabled_disabled;
    } else {
        pFrontend->items[7].enabled = kFrontendItemEnabled_enabled;
    }
    div(race_index, 4);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467b30
void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend) {
    char group_text[12];
    int group;
    int i;

    group = (gCurrent_race_group - gRace_groups) % 10;
    sprintf(group_text, "%s %d", IString_Get(78), group + 1);
    strcpy(pFrontend->items[2].text, group_text);

    for (i = pFrontend->scrollers[0].indexFirstScrollableItem; i <= pFrontend->scrollers[0].indexLastScrollableItem; i++) {
        tFrontend_item_spec* item = &pFrontend->items[i];
        int race_i = 4 * group + i;

        strcpy(item->text, gRace_list[race_i].name);
        if (race_i == gProgram_state.current_race_index) {
            item->radioButton_selected = 1;
        } else {
            item->radioButton_selected = 0;

        }
        if (gRace_list[race_i].is_boundary) {
            item->unlitFont = 2;
            item->highFont = 3;
        } else {
            item->unlitFont = 0;
            item->highFont = 1;
        }
    }
    FuckWithWidths(pFrontend);
    FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(pFrontend);
    MungeButtonModels(pFrontend, 0);
}

// NetworkUpdateSelectedGameInfo

// UpdateNetTrackScroller

// void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend); // Moved for inlining purposes

// NetRaceUp

// NetRaceDn

// SaveSinglePlayerState

// RestoreSinglePlayerState

// MaybeRestoreSavedGame

// NetworkJoinUpdateScroller

// void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ec0
int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend) {

    if (gTyping) {
        EdgeTriggerModeOff();
        WaitForNoKeys();
        EdgeTriggerModeOn();
        FrontEndShowMouse();
        gTyping = 0;
        StopGettingInputString();
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 1;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 1;
        gFrontend_original_player_name[0] = '\0';
        SaveOptions();
    } else {
        FrontEndHideMouse();
        gTyping = 1;
        gFrontend_text_input_item_index = gFrontend_selected_item_index;
        pFrontend->items[gFrontend_text_input_item_index].unlitFont = 2;
        pFrontend->items[gFrontend_text_input_item_index].highFont = 3;
        strcpy(pFrontend->items[gFrontend_text_input_item_index].text, gProgram_state.player_name);
        strcpy(gFrontend_original_player_name, gProgram_state.player_name);
        StartGettingInputString(pFrontend->items[gFrontend_text_input_item_index].text, 8);
    }
    return 0;
}

// NewGameToggleTyping

// NetGameToggleTyping

// ChooseSkill

// NewGameOutfunc

// NewGameInfunc

// StartFudge

// FRONTEND_NewGameMenuHandler

// UpdateNetGameTypeScroller

// NetGameTypeUp

// NetGameTypeDn

// MultiplayerSetup

// NetworkJoinSetup

// NetworkJoinMenuInfunc

// StopAllThatJoinyStuffThisInstant

// NetworkJoinMenuOutfunc

// NetJoinChooseThisGame

// ChooseNetCar

// InitNetStorageSpace

// DisposeNetStorageSpace

// SetUpOtherNetThings

// DoMultiplayerStartStuff

// NetworkJoinGoAhead

// NetSetRaceType

// NetworkStartJoin

// NetworkStartHost

// NetHostChooseThisRace

// NetCancel

// FRONTEND_NetworkJoinMenuHandler

// OnePlayerSetup

// MenuSetCarImage

// MenuSetDriverImage

// MainMenuInfunc

// MainMenuOutfunc

// MainMenuSelectRace

// testUp

// testDn

// FRONTEND_MainMenuHandler

// UpdateCarInfo

// ChangeCarMenuInfunc

// ChangeCarCancel

// ChangeCarOK

// CarClickPrev

// CarClickNext

// ChangeCarMenuOutfunc

// FRONTEND_ChangeCarMenuHandler

// StartGameInfunc

// FRONTEND_StartGameMenuHandler

// CreditsScreenInfunc

// CreditsScreenOutfunc

// FRONTEND_GenericMenuHandler

// FUNCTION: CARMA2_HW 0x0046c5c0
void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText) {
    float map_x;
    float map_y;

    C2_HOOK_BUG_ON(sizeof(tFrontend_brender_item) != 44);

    pFrontend_brender_item->actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    pFrontend_brender_item->model = BrModelAllocate("ButtonModel", 4, 2);
    pFrontend_brender_item->material = BrMaterialAllocate("ButtonMaterial");
    if (pFrontend_brender_item->model == NULL || pFrontend_brender_item->material == NULL || pFrontend_brender_item->actor == NULL) {
#ifdef REC2_FIX_BUGS
        FatalError(kFatalError_OOM_S, "");
#else
        FatalError(kFatalError_OOM_S);
#endif
    }
    if (pMap == NULL) {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapFill(pFrontend_brender_item->field_0x10, BR_COLOUR_RGBA(0, 0, 0x80, 0));
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, 8, 8, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
        pFrontend_brender_item->field_0xc->identifier = BrResStrDup(pFrontend_brender_item->field_0xc, pText);
    } else {
        pFrontend_brender_item->field_0x10 = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0x10, pMap);
        pFrontend_brender_item->field_0xc = BrPixelmapAllocate(BR_PMT_RGBA_4444, pMap->width, pMap->height, NULL, 0);
        BrPixelmapCopy(pFrontend_brender_item->field_0xc, pFrontend_brender_item->field_0x10);
    }
    BrMapAdd(pFrontend_brender_item->field_0xc);
    pFrontend_brender_item->prims[0].t = BRT_BLEND_B;
    pFrontend_brender_item->prims[0].v.b = 1;
    pFrontend_brender_item->prims[1].t = BRT_OPACITY_X;
    pFrontend_brender_item->prims[1].v.x = BR_FIXED_INT(0x80);
    pFrontend_brender_item->prims[2].t = BR_NULL_TOKEN;
    pFrontend_brender_item->prims[2].v.u32 = 0;
    pFrontend_brender_item->material->extra_prim = pFrontend_brender_item->prims;
    pFrontend_brender_item->actor->model = pFrontend_brender_item->model;
    pFrontend_brender_item->actor->material = pFrontend_brender_item->material;
    pFrontend_brender_item->actor->identifier = BrResStrDup(pFrontend_brender_item->actor, "Button");;
    BrMaterialAdd(pFrontend_brender_item->material);
    pFrontend_brender_item->material->colour = pColour;
    pFrontend_brender_item->material->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
    pFrontend_brender_item->material->colour_map = pFrontend_brender_item->field_0xc;
    BrMaterialUpdate(pFrontend_brender_item->material, BR_MATU_ALL);
    pFrontend_brender_item->model->flags |= BR_MODF_KEEP_ORIGINAL;

    pFrontend_brender_item->model->vertices[0].p.v[0] = pFrontend_brender_item->model->vertices[1].p.v[0] = (float)pX;
    pFrontend_brender_item->model->vertices[0].p.v[1] = pFrontend_brender_item->model->vertices[3].p.v[1] = -(float)pY;
    pFrontend_brender_item->model->vertices[2].p.v[0] = pFrontend_brender_item->model->vertices[3].p.v[0] = pFrontend_brender_item->model->vertices[1].p.v[0] + (float)pWidth;
    pFrontend_brender_item->model->vertices[1].p.v[1] = pFrontend_brender_item->model->vertices[2].p.v[1] = pFrontend_brender_item->model->vertices[3].p.v[1] - (float)pHeight;
    pFrontend_brender_item->model->vertices[0].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[1].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[2].p.v[2] = -1.1f;
    pFrontend_brender_item->model->vertices[3].p.v[2] = -1.1f;
    pFrontend_brender_item->model->faces[0].vertices[0] = 0;
    pFrontend_brender_item->model->faces[0].vertices[1] = 1;
    pFrontend_brender_item->model->faces[0].vertices[2] = 2;
    pFrontend_brender_item->model->faces[1].vertices[0] = 2;
    pFrontend_brender_item->model->faces[1].vertices[1] = 3;
    pFrontend_brender_item->model->faces[1].vertices[2] = 0;
    BrModelAdd(pFrontend_brender_item->model);
    if (pMap != NULL) {
        map_x = (float)pWidth / (float)HighResHeadupWidth(pMap->width);
        map_y = (float)pWidth / (float)HighResHeadupWidth(pMap->height);
    } else {
        map_x = (float)pWidth / (float)HighResHeadupWidth(8);
        map_y = (float)pWidth / (float)HighResHeadupWidth(8);
    }
    BrVector2Set(&pFrontend_brender_item->model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&pFrontend_brender_item->model->vertices[1].map, 0.0f, map_y);
    BrVector2Set(&pFrontend_brender_item->model->vertices[2].map, map_x, map_y);
    BrVector2Set(&pFrontend_brender_item->model->vertices[3].map, map_x, 0.0f);
    BrModelUpdate(pFrontend_brender_item->model, BR_MODU_ALL);
    BrActorAdd(gFrontend_actor, pFrontend_brender_item->actor);
    pFrontend_brender_item->actor->render_style = BR_RSTYLE_NONE;
}

// FUNCTION: CARMA2_HW 0x0046c970
int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec) {
    char s[256];
    char s2[256];
    const char* name;
    tTWTVFS twt;
    int count_items;
    int i;

    name = pFrontend_spec->backdrop_name;
    sprintf(s, "START OF FRONTEND_CreateMenu for menu \'%s\'", name);
    PrintMemoryDump(0, s);
    if (pFrontend_spec->create != NULL) {
        pFrontend_spec->create(pFrontend_spec);
    }
    gFrontend_count_brender_items = 0;
    count_items = pFrontend_spec->count_items;
    if (name != NULL && strlen(name) != 0) {
        if (!gFrontend_remove_current_backdrop) {

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(gFrontend_backdrop);
            BrMapAdd(gFrontend_backdrop);
        } else if (gFrontend_backdrop != NULL) {

            BrMapRemove(gFrontend_backdrop);
            BrPixelmapFree(gFrontend_backdrop);

            PathCat(s2, gApplication_path, "INTRFACE");
            PathCat(s2, s2, "BACKDROP");
            strcpy(s, name);
            s[strlen(s) - 4] = '\0';
            PathCat(s2, s2, s);
            twt = OpenPackFileAndSetTiffLoading(s2);
            gFrontend_backdrop = GetThisFuckingPixelmapPleaseMrTwatter(s2, name);
            ClosePackFileAndSetTiffLoading(twt);

            if (gFrontend_backdrop == NULL) {
                BrFailure("FRONTEND: Error loading background graphics.", 0);
            }
            PixelmapSwapByteOrder(gFrontend_backdrop);
            BrMapAdd(gFrontend_backdrop);
        }
    }
    StartMouseCursor();
    for (i = 0; i < count_items; i++) {
        br_pixelmap* map;

        if (pFrontend_spec->items[i].map_index != 0) {
            map = gFrontend_images[pFrontend_spec->items[i].map_index];
        } else {
            map = NULL;
        }
        if (i == gFrontend_selected_item_index) {
            FRONTEND_CreateMenuButton(
                &gFrontend_brender_items[gFrontend_count_brender_items],
                (tS16)pFrontend_spec->items[i].x,
                pFrontend_spec->items[i].y,
                pFrontend_spec->items[i].width,
                pFrontend_spec->items[i].height,
                gFrontend_some_color,
                map,
                pFrontend_spec->items[i].text);
        } else {
            FRONTEND_CreateMenuButton(
                &gFrontend_brender_items[gFrontend_count_brender_items],
                (tS16)pFrontend_spec->items[i].x,
                pFrontend_spec->items[i].y,
                pFrontend_spec->items[i].width,
                pFrontend_spec->items[i].height,
                gFrontend_some_color,
                map,
                pFrontend_spec->items[i].text);
        }
        gFrontend_count_brender_items++;
    }
    ResetInterfaceTimeout();
    FRONTEND_CreateMenuButton(&gFrontend_brender_items[99],
        0,
        0,
        0,
        0,
        gFrontend_some_color,
        NULL,
        "");
    if (gMouse_in_use) {
        gFrontend_selected_item_index = 99;
    } else {
        gFrontend_selected_item_index = 0;
    }
    sprintf(s, "END OF FRONTEND_CreateMenu for menu \'%s\'", pFrontend_spec->name);
    PrintMemoryDump(0, s);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046ccb0
int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend) {
    char s[256];
    tS8 i;

    sprintf(s, "START OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, s);
    if (pFrontend->destroy != NULL) {
        pFrontend->destroy(pFrontend);
    }
    while (gFrontend_actor->children != NULL && strcmp("Backdrop", gFrontend_actor->children->identifier) != 0) {
        BrActorRemove(gFrontend_actor->children);
    }
    for (i = 0; i < gFrontend_count_brender_items; i++) {

        BrMaterialRemove(gFrontend_brender_items[i].material);
        BrModelRemove(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].actor->render_style = BR_RSTYLE_NONE;
        BrMaterialFree(gFrontend_brender_items[i].material);
        gFrontend_brender_items[i].material = NULL;
        BrModelFree(gFrontend_brender_items[i].model);
        gFrontend_brender_items[i].model = NULL;
        BrActorFree(gFrontend_brender_items[i].actor);
        gFrontend_brender_items[i].actor = NULL;
        if (gFrontend_brender_items[i].field_0xc != NULL) {
            BrMapRemove(gFrontend_brender_items[i].field_0xc);
            BrPixelmapFree(gFrontend_brender_items[i].field_0xc);
            gFrontend_brender_items[i].field_0xc = NULL;
        }
        if (gFrontend_brender_items[i].field_0x10 != NULL) {
            BrPixelmapFree(gFrontend_brender_items[i].field_0x10);
            gFrontend_brender_items[i].field_0x10 = NULL;
        }
    }
    /* FIXME: parametrize last item */
    BrMaterialRemove(gFrontend_brender_items[99].material);
    BrModelRemove(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].actor->render_style = BR_RSTYLE_NONE;
    BrMaterialFree(gFrontend_brender_items[99].material);
    gFrontend_brender_items[99].material = NULL;
    BrModelFree(gFrontend_brender_items[99].model);
    gFrontend_brender_items[99].model = NULL;
    BrActorFree(gFrontend_brender_items[99].actor);
    gFrontend_brender_items[99].actor = NULL;
    EndMouseCursor();
    sprintf(s, "END OF FRONTEND_DestroyMenu for menu '%s'", pFrontend->name);
    PrintMemoryDump(0, s);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046cec0
void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void) {
    int time;

    time = (int)PDGetTotalTime() % 750;
    if (time < 375) {
        gFrontend_throb_factor = (double)time / (750.0 / 2.0);
    } else {
        gFrontend_throb_factor = (double)(750 - time) / (750.0 / 2.0);
    }
}

// FRONTEND_DrawMenu

// FRONTEND_Setup2D

// FRONTEND_Setup

// FRONTEND_Redraw

// FUNCTION: CARMA2_HW 0x0046d8b0
void C2_HOOK_FASTCALL MaybeDoMouseCursor(void) {

    if (!gFrontend_suppress_mouse) {
        PossibleLock(1);
        DoMouseCursor();
        PossibleUnlock(1);
    }
}

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void) {

    gFrontend_time_last_input = PDGetTotalTime();
}

// FRONTEND_Main

// ProcessInputString

// FUNCTION: CARMA2_HW 0x0046e5c0
int C2_HOOK_FASTCALL StopGettingInputString(void) {

    gFrontend_current_input = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046e5d0
int C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size) {

    PDClearKeyboardBuffer();
    gINT_0059b0d8 = -1;
    gFrontend_maximum_input_length = pBuffer_size;
    gFrontend_current_input = pBuffer;
    DodgyPause(200);
    return 1;
}

// DisposeWrecksGallery

// ScrollToNextCar

// ScrollToPrevCar

// BuyCurrentCar

// WrecksInFunc

// KeepInRange

// WrecksOutFunc

// WrecksUpdateFunc

// GotItAlready

// GetCarSelectedByMouse

// WreckPick

// FUNCTION: CARMA2_HW 0x0046f560
intptr_t C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref) {

    if (a == (br_actor*)ref) {
        gHierarchy_has_actor = 1;
    }
    BrActorEnum(a, HeirarchyPick, ref);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046f590
void C2_HOOK_FASTCALL DodgyPause(tU32 pTime) {
    tU32 start;

    start = PDGetTotalTime();
    while (PDGetTotalTime() < start + pTime) {
        /* brrrr */
    }
}

// FUNCTION: CARMA2_HW 0x0046f5b0
void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps) {
    float t;
    int i;

    t = (float)pStep / (float)pCount_steps;
    for (i = 0; i < pModel->nvertices; i++) {

        pModel->vertices[i].p.v[0] = pModel_from->vertices[i].p.v[0] + (pModel_to->vertices[i].p.v[0] - pModel_from->vertices[i].p.v[0]) * t;
        pModel->vertices[i].p.v[1] = pModel_from->vertices[i].p.v[1] + (pModel_to->vertices[i].p.v[1] - pModel_from->vertices[i].p.v[1]) * t;
    }
    BrModelUpdate(pModel, BR_MODU_VERTEX_POSITIONS);
}

// FUNCTION: CARMA2_HW 0x0046f630
void C2_HOOK_FASTCALL ScrollCredits(void) {
    int y;
    int i;

    y = (int)(430.0 - (float)(PDGetTotalTime() - gCredits_scroll_start) * 0.03);
    for (i = 0; i < gCredits_line_count; i++) {
        y += gCredits_heights[i];
        if (y < 430 && y > 30) {
            if (gCredits_throbs[i]) {
                SolidPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i] - 1, eJust_centre, 1);
                TransparentPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i], eJust_centre, 1, gFrontend_throb_factor);
            } else {
                SolidPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i], eJust_centre, 1);
            }
        }
    }
    if ((float)(PDGetTotalTime() - gCredits_scroll_start) * 0.03 > (float)gCredits_total_height) {
        gCredits_scroll_start = PDGetTotalTime();
    }
}

// LoadGameInFunc

// FUNCTION: CARMA2_HW 0x0046f8a0
br_actor* C2_HOOK_FASTCALL CreateAPOactor(void) {
    br_pixelmap* map;
    br_actor* actor;
    br_material* material;
    br_model* model;

    map = BrPixelmapAllocate(gBack_screen->type, 64, 64, NULL, 0);
    model = BrModelAllocate("Billboard Model", 4, 2);
    material = BrMaterialAllocate("Billboard Material");
    actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    actor->identifier = "Billboard Actor";
    actor->model = model;
    actor->material = material;
    actor->render_style = BR_RSTYLE_FACES;
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    model->faces[0].material = NULL;
    model->faces[1].material = NULL;
    BrVector3Set(&model->vertices[0].p,  0.0f,   0.0f, -1.05f);
    BrVector3Set(&model->vertices[1].p, 40.0f,   0.0f, -1.05f);
    BrVector3Set(&model->vertices[2].p,  0.0f, -12.0f, -1.05f);
    BrVector3Set(&model->vertices[3].p, 40.0f, -12.0f, -1.05f);
    material->colour = 0;
    material->colour_map = map;
    material->flags = BR_MATF_ALWAYS_VISIBLE;
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    BrMapAdd(map);
    BrMaterialAdd(material);
    BrModelAdd(model);
    return actor;
}

// FUNCTION: CARMA2_HW 0x0046f9e0
void C2_HOOK_FASTCALL KillAPOactor(br_actor* pActor) {

    if (pActor != NULL) {

        if (pActor->material->colour_map != NULL) {
            BrMapRemove(pActor->material->colour_map);
            BrPixelmapFree(pActor->material->colour_map);
        }
        if (pActor->material != NULL) {
            BrMaterialRemove(pActor->material);
            BrMaterialFree(pActor->material);
        }
        if (pActor->model != NULL) {
            BrModelRemove(pActor->model);
            BrModelFree(pActor->model);
        }
        if (pActor->parent != NULL) {
            BrActorRemove(pActor);
        }
        BrActorFree(pActor);
    }
}

// FUNCTION: CARMA2_HW 0x0046fa60
int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gFrontend_billboard_actors); i++) {
        KillAPOactor(gFrontend_billboard_actors[i]);
    }
    BrActorFree(gFrontend_menu_camera);
    gFrontend_menu_camera = NULL;
    EndSavedGamesList();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
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
int C2_HOOK_FASTCALL LoadGameScrollDownLoadGameScrollDown(tFrontend_spec* pFrontend) {

    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top += 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

#define LOAD_SLOT_N(FRONTEND, N) \
    if (gFrontend_load_game_index_top + (N) + 1 > gFrontend_count_saved_games) { \
        pFrontend->items[N + 1].menuInfo = NULL; \
        return 0; \
    } \
    TryToLoadGame(N); \
    if (gProgram_state.racing) { \
        return 1; \
    } \
    gFrontend_next_menu = kFrontend_menu_main; \
    return 3;

// FUNCTION: CARMA2_HW 0x0046fbd0
int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x0046fc50
int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 1);
}

// FUNCTION: CARMA2_HW 0x0046fcd0
int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 2);
}

// FUNCTION: CARMA2_HW 0x0046fd50
int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 3);
}

// FUNCTION: CARMA2_HW 0x0046fdd0
int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 4);
}

// FUNCTION: CARMA2_HW 0x0046fe50
int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 5);
}

// FUNCTION: CARMA2_HW 0x0046fed0
int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 6);
}

// FUNCTION: CARMA2_HW 0x0046ff50
int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 7);
}

#undef LOAD_SLOT_N

// FUNCTION: CARMA2_HW 0x0046ffd0
int C2_HOOK_FASTCALL TryToLoadGame(int pN) {

    if (!DoLoadGame(gFrontend_load_game_index_top + pN)) {
        DRS3StartSound(gEffects_outlet, eSoundId_CantAffordPart);
        return 0;
    } else {
        gAbandon_game = 1;
        gNo_credits_APO_restore = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_Done);
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00470020
int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend) {
    int selected_item_index;
    tU32 prev;
    int mouse_x;
    int mouse_y;
    int i;

    ServiceGame();
    selected_item_index = gFrontend_selected_item_index;
    prev = gFrontend_scroll_last_update;
    gFrontend_scroll_last_update = PDGetTotalTime();
    gFrontend_scroll_time_left -= gFrontend_scroll_last_update - prev;
    if (gFrontend_scroll_time_left < 0) {
        gFrontend_scroll_time_left = 0;
    }
    gFrontend_mouse_down = 0;
    gFrontend_scroll_time_increment = 50;
    if (gMouse_in_use) {

        gFrontend_selected_item_index = 0;
        ResetInterfaceTimeout();
        GetMousePosition(&mouse_x, &mouse_y);
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
    if (gFrontend_load_game_index_top > 0) {
        pFrontend->items[17].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[17].enabled = kFrontendItemEnabled_disabled;
    }
    if (gFrontend_load_game_index_top + 8 < gFrontend_count_saved_games) {
        pFrontend->items[18].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[18].enabled = kFrontendItemEnabled_disabled;
    }
    for (i = 0; i < 8; i++) {
        int j;
        int font;
        tSave_game* save_game;
        char* text_ptr;
        char text[128];
        int y_text = 125 + i * 27;
        int y_apo = 128 + i * 27;
        char date_str[12];
        char time_str[16];
        int len_date;

        font = (i == gFrontend_selected_item_index - 1) ? kPolyfont_hand_green_15pt_lit : kPolyfont_hand_green_15pt_unlit;
        save_game = GetNthSavedGame(gFrontend_load_game_index_top + i);
        if (save_game == NULL) {
            continue;
        }
        memset(text, 0, sizeof(text));
        strcpy(date_str, save_game->date);
        strcpy(time_str, save_game->time);

        len_date = strlen(date_str);
        text_ptr = text;
        for (j = 0; j < len_date; j++) {

            if (isalnum(date_str[j])) {
                *text_ptr++ = date_str[j];
            } else if (date_str[j] == '/') {
                *text_ptr++ = '-';
            }
        }
        SolidPolyFontText(text, 42, y_text, font, eJust_left, 1);

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

        memset(text, 0, sizeof(text));
        strncpy(text, time_str, 5);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

        SolidPolyFontText(text, 120, y_text, font, eJust_left, 1);

        memset(text, 0, sizeof(text));
        strcpy(text, save_game->player_name);
        PolyClipName(text, font, 127);
        SolidPolyFontText(text, 190, y_text, font, 0, 1);

        if (save_game->game_completed) {
            sprintf(text, "! %i", 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
        } else if (save_game->is_boundary_race) {
            sprintf(text, "%c %i", '\x1f', 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
        } else {
            sprintf(text, "%i", 1 + (gRace_list[save_game->current_race_index].group - gRace_groups));
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
        sprintf(pFrontend->items[15].text, "%i-%i (%i)",
            gFrontend_load_game_index_top + 1,
            gFrontend_load_game_index_top + 8,
            gFrontend_count_saved_games);
    }

    if (PDKeyDown(51) || PDKeyDown(52) || (gFrontend_mouse_down && gNet_join_host_result != -1)) {
        int result;
        tFrontend_spec* next;
        int go_back;

        ToggleSelection(pFrontend);
        if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 2) {
            return 2;
        } else if (pFrontend->items[gFrontend_selected_item_index].field_0xc == 1) {
            gFrontend_leave_current_menu = 1;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            result = pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        } else {
            result = pFrontend->items[selected_item_index].field_0xc;
        }
        next = pFrontend->items[gFrontend_selected_item_index].menuInfo;
        go_back = 0;
        if (next == (tFrontend_spec*)(uintptr_t)0x1) {
            go_back = 1;
            next = pFrontend->previous;
        }
        if (pFrontend->items[gFrontend_selected_item_index].action != NULL) {
            if (next == NULL && pFrontend->items[gFrontend_selected_item_index].action != temp) {
                DRS3StartSound(gEffects_outlet, eSoundId_Done);
            }
            pFrontend->items[gFrontend_selected_item_index].action(pFrontend);
        }
        if (next != NULL) {
            if (!go_back) {
                Generic_LinkInEffect();
            } else {
                Generic_LinkOutEffect();
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
            return go_back ? 1 : result;
        }
    } else {
        return gFrontend_leave_current_menu ? 1 : 0;
    }
}

// FUNCTION: CARMA2_HW 0x00470860
void C2_HOOK_FASTCALL PrepareAPO(int pActor_index) {
    br_material* material;
    br_model* model;

    material = gFrontend_billboard_actors[pActor_index]->material;
    model = gFrontend_billboard_actors[pActor_index]->model;
    BrMapUpdate(material->colour_map, BR_MAPU_ALL);
    BrMaterialUpdate(material, BR_MATU_ALL);
    BrModelUpdate(model, BR_MODU_VERTICES);
}

// FUNCTION: CARMA2_HW 0x004708a0
void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;
    float map_left;
    float map_right;

    model = gFrontend_billboard_actors[pIndex]->model;
    map_left = (double)(pTex_index + 0) * 0.1875;
    map_right = (double)(pTex_index + 1) * 0.1875;
    BrVector2Set(&model->vertices[0].map, 0.0f,   (float)map_left);
    BrVector2Set(&model->vertices[1].map, 0.625f, (float)map_left);
    BrVector2Set(&model->vertices[2].map, 0.0f, (float)map_right);
    BrVector2Set(&model->vertices[3].map, 0.625f, (float)map_right);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&gFrontend_billboard_actors[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.0f);
    BrActorAdd(gFrontend_menu_camera, gFrontend_billboard_actors[pIndex]);
    BrZbSceneRender(gFrontend_menu_camera, gFrontend_menu_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gFrontend_billboard_actors[pIndex]);
}

// FUNCTION: CARMA2_HW 0x004709b0
void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActor_index, int pAPO) {
    br_pixelmap* map;
    int x;
    int y;
    br_colour c;
    int i;

    map = gFrontend_billboard_actors[pActor_index]->material->colour_map;

    if (pAPO == 0) {
        BrPixelmapFill(map, 0);
    }

    for (i = 0; i < 3 * 10; i++) {

        x = 4 * (i % 10);
        y = 0 + 4 * (i / 10);

        if (i >= pPotential) {
            c = gFrontend_APO_Colour_2;
        } else if (i >= pCurrent) {
            c = gFrontend_APO_Colour_3;
        } else {
            c = gFrontend_APO_Colour_1;
        }

        BrPixelmapRectangleFill(map, x, y + 3 * 4 * pAPO, 3, 3, c);
    }
}

// FUNCTION: CARMA2_HW 0x00470a50
void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth) {

    while (PolyFontTextWidth(pFont, pText) > pWidth) {
        pText[strlen(pText) + -1] = '\0';
    }
}

// Ians_GetItemAtMousePos

// TranslateSliderItem

// GetActiveSlider

// GetAnyActiveSlider

// PrepareSliders

// FUNCTION: CARMA2_HW 0x00470a90
void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext) {

    gFrontend_A_model_from = gFrontend_A_models[pCurrent->model_A_index].model;
    gFrontend_A_model_to = gFrontend_A_models[pNext->model_A_index].model;
    if (pCurrent->model_A_index == 0) {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop0_opacity_mode = pNext->model_A_index != 0 ? 0 : -1;
    }

    gFrontend_B_model_from = gFrontend_B_models[pCurrent->model_B_index].model;
    gFrontend_B_model_to = gFrontend_B_models[pNext->model_B_index].model;
    if (pCurrent->model_B_index == 0) {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop1_opacity_mode = pNext->model_B_index != 0 ? 0 : -1;
    }

    gFrontend_C_model_from = gFrontend_C_models[pCurrent->model_C_index].model;
    gFrontend_C_model_to = gFrontend_C_models[pNext->model_C_index].model;
    if (pCurrent->model_C_index == 0) {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 1 : -2;
    } else {
        gFrontend_backdrop2_opacity_mode = pNext->model_C_index != 0 ? 0 : -1;
    }

    gFrontend_interpolate_steps_left = 16;
}

// FUNCTION: CARMA2_HW 0x00470bb0
int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend) {

    DefaultInfunc(pFrontend);
    gCurrent_frontend_scrollbars = NULL;
    gConnected_items = NULL;
    gPTR_00686508 = NULL;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00470c10
int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend) {

    return 0;
}

// Generic_EventEffect

void C2_HOOK_FASTCALL Generic_LinkInEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
}

void C2_HOOK_FASTCALL Generic_LinkOutEffect(void) {

    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
}

// GetScrollSet

// ScrollSet_DisplayEntry

// ScrollSet_GetItem

// GetUpdown

// ScrollSet_TranslateItemToIndex

// Generic_FindNextActiveItem

// Generic_FindPrevActiveItem

// Generic_MungeActiveItems

// Generic_UnMungeActiveItems

// Generic_MenuHandler

// StripControls

// BackupKeyMappings

// LoadKeyNames

// DisposeKeyNames

// RefreshScrollSet

// DetermineKeyArrayIndex

// Joystick_BackupSettings

// Controls_JoystickToggle

// Controls_JoystickDpadToggle

// Slider_XProc

// Slider_YProc

// Slider_ForceProc

// DisplayJoystickSettings

// SaveAllJoystickData

// Controls_SwitchKeymapSet

// Controls_KeyUp

// Controls_KeyDown

// Controls_Infunc

// Controls_Outfunc

// Controls_SlotActivated

// Controls_Ok

// DisplayNetworkOptions

// BackupNetworkOptions

// RestoreNetworkOptions

// NetOptions_Infunc

// NetOptions_Outfunc

// NetOptions_CreditsRoller

// NetOptions_TargetRoller

// NetOptions_Ok

// NetOptions_Cancel

// CheckPlayersAreResponding

// MungePlayers

// NetSynchRaceStart2

// NetSync_Draw

// NetSync_Start

// NetSync_Abort

// NetSync_Infunc

// NetSync_MenuHandler

// NetSummary_Draw

// SortScores

// SortGameScores

// NetworkSummarySetup

// NetSummary_Infunc

// NetSummary_MenuHandler

// DisplayVolumeSettings

// Slider_EffectsProc

// Options_Infunc

// Options_Outfunc

// Options_MusicOn

// Options_MusicOff

// Options_Ok

// Options_AbortRace

// Options_AbortGame

// Options_Quit

// Graphics_Infunc

// Graphics_Outfunc

// FUNCTION: CARMA2_HW 0x00474860
void C2_HOOK_FASTCALL FrontEndShowMouse(void) {

    gFrontend_suppress_mouse = 0;
}

// FUNCTION: CARMA2_HW 0x00474870
void C2_HOOK_FASTCALL FrontEndHideMouse(void) {

    gFrontend_suppress_mouse = 1;
}
