/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "72-interface.h"

#include "01-network.h"
#include "02-init.h"
#include "04-netgame.h"
#include "05-drmem.h"
#include "08-loading1.h"
#include "10-loading2.h"
#include "12-menuoptions.h"
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
#include "globvrpb.h"
#include "joystick.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"
#include <ctype.h>

// GLOBAL: CARMA2_HW 0x00764eec
// GLOBAL: CARMA2_SW 0x0075b44c
tFrontendMenuType gFrontend_next_menu;

// GLOBAL: CARMA2_HW 0x00764ee8
// GLOBAL: CARMA2_SW 0x0075b448
int gFrontend_change_car_selected_car;

// GLOBAL: CARMA2_HW 0x00688b20
// GLOBAL: CARMA2_SW 0x00683080
int gFrontend_suppress_mouse;

// GLOBAL: CARMA2_HW 0x00688ab0
// GLOBAL: CARMA2_SW 0x00683010
tConnected_items* gConnected_items;

// GLOBAL: CARMA2_HW 0x00686508
// GLOBAL: CARMA2_SW 0x00680c48
tStruct_00686508* gPTR_00686508;

// GLOBAL: CARMA2_HW 0x00686820
// GLOBAL: CARMA2_SW 0x00680f60
tFrontend_slider* gCurrent_frontend_scrollbars;

// GLOBAL: CARMA2_HW 0x00604730
// GLOBAL: CARMA2_SW 0x005ff660
tFrontend_item_spec gDefault_last_interface_item = {
    0x401, temp, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1,
};

// GLOBAL: CARMA2_HW 0x006864f4
// GLOBAL: CARMA2_SW 0x00680c34
int gFrontend_scrollbars_updated;

// GLOBAL: CARMA2_HW 0x00686850
// GLOBAL: CARMA2_SW 0x00680f90
br_pixelmap* gFrontend_images[100];

// GLOBAL: CARMA2_HW 0x00687248
// GLOBAL: CARMA2_SW 0x006817a8
tFrontend_brender_item gFrontend_brender_items[100]; /* FIXME: parametrize size + index of last item */

// GLOBAL: CARMA2_HW 0x00688770
// GLOBAL: CARMA2_SW 0x00682cd0
int gFrontend_selected_item_index;

// GLOBAL: CARMA2_HW 0x0068b8a0
// GLOBAL: CARMA2_SW 0x00685e00
tRace_group_spec* gRace_groups;

// GLOBAL: CARMA2_HW 0x00688af4
// GLOBAL: CARMA2_SW 0x00683054
char* gFrontend_current_input;

// GLOBAL: CARMA2_HW 0x0068723c
// GLOBAL: CARMA2_SW 0x0068179c
int gFrontend_text_input_item_index;

// GLOBAL: CARMA2_HW 0x00763900
// GLOBAL: CARMA2_SW 0x00759e60
char gFrontend_original_player_name[32];

// GLOBAL: CARMA2_HW 0x0059b0d8
// GLOBAL: CARMA2_SW 0x00596008
int gINT_0059b0d8 = -1;

// GLOBAL: CARMA2_HW 0x0059b0d4
// GLOBAL: CARMA2_SW 0x00596004
int gFrontend_maximum_input_length = 9;

// GLOBAL: CARMA2_HW 0x00763924
int gHierarchy_has_actor;

// GLOBAL: CARMA2_HW 0x00686834
// GLOBAL: CARMA2_SW 0x00680f74
tU32 gCredits_scroll_start;

// GLOBAL: CARMA2_HW 0x006883c0
// GLOBAL: CARMA2_SW 0x00682920
int gCredits_line_count;

// GLOBAL: CARMA2_HW 0x00686f04
// GLOBAL: CARMA2_SW 0x00681644
int* gCredits_heights;

// GLOBAL: CARMA2_HW 0x00687238
// GLOBAL: CARMA2_SW 0x00681798
int* gCredits_throbs;

// GLOBAL: CARMA2_HW 0x00688448
// GLOBAL: CARMA2_SW 0x006829a8
char** gCredits_texts;

// GLOBAL: CARMA2_HW 0x0068682c
// GLOBAL: CARMA2_SW 0x00680f6c
int* gCredits_fonts;

// GLOBAL: CARMA2_HW 0x00686f90
// GLOBAL: CARMA2_SW 0x006816d0
int gCredits_total_height;

// GLOBAL: CARMA2_HW 0x006886d0
double gFrontend_throb_factor;

// GLOBAL: CARMA2_HW 0x00763940
// GLOBAL: CARMA2_SW 0x00759ea0
br_actor* gFrontend_billboard_actors[8];

// GLOBAL: CARMA2_HW 0x00688b10
// GLOBAL: CARMA2_SW 0x00683070
br_actor* gFrontend_menu_camera;

// GLOBAL: CARMA2_HW 0x007638ac
// GLOBAL: CARMA2_SW 0x00759e0c
int gFrontend_load_game_index_top;

// GLOBAL: CARMA2_HW 0x00763898
// GLOBAL: CARMA2_SW 0x00759df8
int gFrontend_scroll_time_left;

// GLOBAL: CARMA2_HW 0x0076389c
// GLOBAL: CARMA2_SW 0x00759dfc
int gFrontend_scroll_time_increment;

// GLOBAL: CARMA2_HW 0x00764e9c
// GLOBAL: CARMA2_SW 0x0075b3fc
int gFrontend_count_saved_games;

// GLOBAL: CARMA2_HW 0x00763894
tU32 gFrontend_scroll_last_update;

// GLOBAL: CARMA2_HW 0x00688b04
int gFrontend_mouse_down;

// GLOBAL: CARMA2_HW 0x0068875c
// GLOBAL: CARMA2_SW 0x00682cbc
tU32 gFrontend_time_last_input;

// GLOBAL: CARMA2_HW 0x00764ee4
// GLOBAL: CARMA2_SW 0x0075b444
int gNet_join_host_result;  // FIXME: use union instead?

// GLOBAL: CARMA2_HW 0x00688b08
int gPrev_frontend_mouse_down;

// GLOBAL: CARMA2_HW 0x006883a8
// GLOBAL: CARMA2_SW 0x00682908
int gFrontend_leave_current_menu;

// GLOBAL: CARMA2_HW 0x00688abc
// GLOBAL: CARMA2_SW 0x0068301c
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
// GLOBAL: CARMA2_SW 0x00680f7c
int gFrontend_count_brender_items;

// GLOBAL: CARMA2_HW 0x00764ee0
// GLOBAL: CARMA2_SW 0x0075b440
int gFrontend_remove_current_backdrop;

// GLOBAL: CARMA2_HW 0x00686f8c
// GLOBAL: CARMA2_SW 0x006816cc
br_pixelmap* gFrontend_backdrop;

// GLOBAL: CARMA2_HW 0x00688ae8
// GLOBAL: CARMA2_SW 0x00683048
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
// GLOBAL: CARMA2_SW 0x00681638
int gFrontend_interpolate_steps_left;

// GLOBAL: CARMA2_HW 0x00763820
tFrontend_slider gOptions_sound_slider;

// GLOBAL: CARMA2_HW 0x00688ae4
// GLOBAL: CARMA2_SW 0x00683044
int gValid_stashed_save_game; // FIXME: rename variable?

// GLOBAL: CARMA2_HW 0x00688408
tConnected_items gControls_scroller;

// GLOBAL: CARMA2_HW 0x00688458
char* gKey_names[153];

// GLOBAL: CARMA2_HW 0x00604888
const int gControls_frontend_to_key_mapping_lut[29] = {
    49, 50, 47, 48, 54, 45, 60, 58, 56, 46,
    57, 67, 68, 69, 71, 61, 62, 63, 64, 74,
    59, 70, 72, 73, 75, 76, 35, 65, 66,
};

// FUNCTION: CARMA2_HW 0x00466450
// FUNCTION: CARMA2_SW 0x004661e0
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {

    return 0;
}

// int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY); // Moved for inlining purposes

// FUNCTION: CARMA2_HW 0x00466ce0
// FUNCTION: CARMA2_SW 0x00466a70
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
// FUNCTION: CARMA2_SW 0x00467610
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
// FUNCTION: CARMA2_SW 0x00466480
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
// FUNCTION: CARMA2_SW 0x0046a930
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName) {
    br_pixelmap* pixelmaps[1000];
    br_pixelmap* result;
    FILE* f;
    tPath_name the_path;
    char* str;
    size_t count;
    size_t i;

    result = NULL;
    PathCat(the_path, pFolder, "PIXIES.P08");
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
// FUNCTION: CARMA2_SW 0x004664f0
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

// FUNCTION: CARMA2_HW 0x00466b60
// FUNCTION: CARMA2_SW 0x004668f0
int C2_HOOK_FASTCALL GameMenuInfunc(tFrontend_spec* pFrontend) {

    DefaultInfunc(pFrontend);
    return 1;
}

// FindNextActiveItem

// FUNCTION: CARMA2_HW 0x00467a30
// FUNCTION: CARMA2_SW 0x004677b0
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
// FUNCTION: CARMA2_SW 0x00466940
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
// FUNCTION: CARMA2_SW 0x004678b0
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
// FUNCTION: CARMA2_SW 0x00466c50
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

// FUNCTION: CARMA2_HW 0x00467050
// FUNCTION: CARMA2_SW 0x00466de0
int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend) {

    ToggleTyping(pFrontend);
    if (!gTyping && strlen(pFrontend->items[gFrontend_selected_item_index].text) != 0) {

        strcpy(gProgram_state.player_name, pFrontend->items[gFrontend_selected_item_index].text);
        SaveOptions();
    }
    return 0;
}

// NetGameToggleTyping

// FUNCTION: CARMA2_HW 0x00467130
// FUNCTION: CARMA2_SW 0x00466ec0
int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend) {

    (void)pFrontend;
    switch (gFrontend_selected_item_index) {
    case 2:
        gProgram_state.skill_level = 0;
        InitGameAccordingToSkillLevel();
        break;
    case 3:
        gProgram_state.skill_level = 1;
        InitGameAccordingToSkillLevel();
        break;
    case 4:
        gProgram_state.skill_level = 2;
        InitGameAccordingToSkillLevel();
        break;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467180
// FUNCTION: CARMA2_SW 0x00466f10
int C2_HOOK_FASTCALL NewGameOutfunc(tFrontend_spec* pFrontend) {

    SaveOptions();
    InitGameAccordingToSkillLevel();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingout);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004671a0
// FUNCTION: CARMA2_SW 0x00466f30
int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend) {

    EdgeTriggerModeOff();
    WaitForNoKeys();
    EdgeTriggerModeOn();
    DRS3StartSound(gEffects_outlet, eSoundId_Swingin);
    switch (gProgram_state.skill_level) {
    case 0:
        pFrontend->items[2].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[2].radioButton_selected = 1;
        break;
    case 1:
    default:
        pFrontend->items[3].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[3].radioButton_selected = 1;
        gProgram_state.skill_level = 1;
        break;
    case 2:
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[4].radioButton_selected = 1;
        break;
    }
    pFrontend->items[5].enabled = kFrontendItemEnabled_enabled;
    pFrontend->default_item = 5;
    gFrontend_selected_item_index = 5;
    DefaultInfunc(pFrontend);
    gMouse_in_use = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00467270
// FUNCTION: CARMA2_SW 0x00467000
int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend) {
    if (gProgram_state.field_0x2c) {
        return 5;
    } else {
        return 0;
    }
}

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

// FUNCTION: CARMA2_HW 0x00469a30
// FUNCTION: CARMA2_SW 0x00469790
int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend) {

    return 0;
}

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

// FUNCTION: CARMA2_HW 0x0046b610
int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0046b620
// FUNCTION: CARMA2_SW 0x0046b340
int C2_HOOK_FASTCALL ChangeCarOK(tFrontend_spec* pFrontend) {

    gProgram_state.current_car_index = gProgram_state.cars_available[gFrontend_change_car_selected_car];
    return 0;
}

// CarClickPrev

// CarClickNext

// FUNCTION: CARMA2_HW 0x0046ba70
int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend) {

    return 1;
}

// FRONTEND_ChangeCarMenuHandler

// StartGameInfunc

// FUNCTION: CARMA2_HW 0x0046bd50
int C2_HOOK_FASTCALL FRONTEND_StartGameMenuHandler(tFrontend_spec* pFrontend) {

    return 1;
}

// CreditsScreenInfunc

// FUNCTION: CARMA2_HW 0x0046c090
// FUNCTION: CARMA2_SW 0x0046bdc0
int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gCredits_line_count; i++) {
        free(gCredits_texts[i]);
    }
    free(gCredits_texts);
    return 1;
}

// FRONTEND_GenericMenuHandler

// FUNCTION: CARMA2_HW 0x0046c5c0
// FUNCTION: CARMA2_SW 0x0046c2d0
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
// FUNCTION: CARMA2_SW 0x0046c610
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
#ifdef REC2_SW
            strcat(s, "l.tif");
#endif
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
#ifdef REC2_SW
            strcat(s, "l.tif");
#endif
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
// FUNCTION: CARMA2_SW 0x0046c9a0
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

// STUB: CARMA2_HW 0x0046d8e0
// STUB: CARMA2_SW 0x0046cfc0
int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0046e470
// FUNCTION: CARMA2_SW 0x0046db10
int C2_HOOK_FASTCALL ProcessInputString(void) {
    int len;
    int key;
    int ch;

    if (gFrontend_current_input == NULL) {
        return 0;
    }

    len = strlen(gFrontend_current_input);
    if (len > gFrontend_maximum_input_length) {
        gFrontend_current_input[len - 1] = '\0';
        return -1;
    }
    PollKeys();
    key = PDAnyKeyDown();
    if (key == 50) {
        if (len > 0) {
            gFrontend_current_input[len - 1] = '\0';
        }
        return 1;
    }
    if (key == 63) {
        strcpy(gFrontend_current_input, gFrontend_original_player_name);
        return -2;
    }
    if (key == 51) {
        if (len == 0) {
            strcpy(gFrontend_current_input, gFrontend_original_player_name);
        }
        return -1;
    }
    ch = PDGetKeyboardCharacter();
    if (ch != 0) {
        dr_dprintf("FRONTEND: Got char %d", ch);
        ch = (tU8)PDConvertToASCIILessThan128(ch);
        dr_dprintf("FRONTEND: Char converted to %d", ch);
        if (ch > 0x1f && ch != 0x7f) {
            gFrontend_current_input[len + 0] = ch;
            gFrontend_current_input[len + 1] = '\0';
            dr_dprintf("FRONTEND: Adding char %d to string. String now '%s'", ch, gFrontend_current_input);
            return 1;
        }
        dr_dprintf("FRONTEND: Invalid Char", ch);
    }
    return 0;
}

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

// FUNCTION: CARMA2_HW 0x0046eab0
// FUNCTION: CARMA2_SW 0x0046e170
int C2_HOOK_FASTCALL KeepInRange(int pIndex, int pRange) {

    C2_HOOK_ASSERT(pRange > 0);

    while (pIndex < 0) {
        pIndex += pRange;
    }
    while (pIndex >= pRange) {
        pIndex -= pRange;
    }
    return pIndex;
}

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
// FUNCTION: CARMA2_SW 0x0046ec60
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
// FUNCTION: CARMA2_SW 0x0046ece0
void C2_HOOK_FASTCALL ScrollCredits(void) {
    int y;
    int i;

    y = (int)(430.0 - (float)(PDGetTotalTime() - gCredits_scroll_start) * 0.03);
    for (i = 0; i < gCredits_line_count; i++) {
        y += gCredits_heights[i];
        if (y < 430 && y > 30) {
            if (gCredits_throbs[i]) {

                SolidPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i] - 1, eJust_centre, 1);
#ifdef REC2_HW
                TransparentPolyFontText(gCredits_texts[i], 320, y, gCredits_fonts[i], eJust_centre, 1, gFrontend_throb_factor);
#endif
#ifdef REC2_SW
                PolyFontText(gCredits_texts[i], 320, y, gFrontend_throb_factor <= 0.5 ? gCredits_fonts[i] - 1 : gCredits_fonts[i], eJust_centre, 1);
#endif
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
// FUNCTION: CARMA2_SW 0x0046ef50
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
// FUNCTION: CARMA2_SW 0x0046f090
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
// FUNCTION: CARMA2_SW 0x0046f110
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
// FUNCTION: CARMA2_SW 0x0046f1e0
int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend) {

    if (gFrontend_load_game_index_top > 0 && gFrontend_scroll_time_left == 0) {
        gFrontend_load_game_index_top -= 1;
        DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
        gFrontend_scroll_time_left += gFrontend_scroll_time_increment;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0046fb80
// FUNCTION: CARMA2_SW 0x0046f230
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
// FUNCTION: CARMA2_SW 0x0046f280
int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 0);
}

// FUNCTION: CARMA2_HW 0x0046fc50
// FUNCTION: CARMA2_SW 0x0046f300
int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 1);
}

// FUNCTION: CARMA2_HW 0x0046fcd0
// FUNCTION: CARMA2_SW 0x0046f380
int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 2);
}

// FUNCTION: CARMA2_HW 0x0046fd50
// FUNCTION: CARMA2_SW 0x0046f400
int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 3);
}

// FUNCTION: CARMA2_HW 0x0046fdd0
// FUNCTION: CARMA2_SW 0x0046f480
int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 4);
}

// FUNCTION: CARMA2_HW 0x0046fe50
// FUNCTION: CARMA2_SW 0x0046f500
int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 5);
}

// FUNCTION: CARMA2_HW 0x0046fed0
// FUNCTION: CARMA2_SW 0x0046f580
int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 6);
}

// FUNCTION: CARMA2_HW 0x0046ff50
// FUNCTION: CARMA2_SW 0x0046f600
int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend) {

    LOAD_SLOT_N(pFrontend, 7);
}

#undef LOAD_SLOT_N

// FUNCTION: CARMA2_HW 0x0046ffd0
// FUNCTION: CARMA2_SW 0x0046f680
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
// FUNCTION: CARMA2_SW 0x0046f6d0
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
#ifdef REC2_HW
            Morph_Initialise(pFrontend, gCurrent_frontend_spec);
#endif
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
// FUNCTION: CARMA2_SW 0x0046ff10
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
// FUNCTION: CARMA2_SW 0x0046ff50
void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index) {
    br_model *model;
    float map_left;
    float map_right;

    model = gFrontend_billboard_actors[pIndex]->model;
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable : 4244) // '=' : conversion from 'double ' to 'float ', possible loss of data
#endif
    map_left = (float)(pTex_index + 0) * 3.0 / 16.0;
    map_right = (float)(pTex_index + 1) * 3.0 / 16.0;
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(pop)
#endif
    BrVector2Set(&model->vertices[0].map, 0.0f,   map_left);
    BrVector2Set(&model->vertices[1].map, 10.0f / 16.0f, map_left);
    BrVector2Set(&model->vertices[2].map, 0.0f, map_right);
    BrVector2Set(&model->vertices[3].map, 10.0f / 16.0f, map_right);
    BrModelUpdate(model, BR_MODU_VERTEX_MAPPING);
    BrVector3Set(&gFrontend_billboard_actors[pIndex]->t.t.translate.t, (float)pX, (float)-pY, 0.0f);
    BrActorAdd(gFrontend_menu_camera, gFrontend_billboard_actors[pIndex]);
    BrZbSceneRender(gFrontend_menu_camera, gFrontend_menu_camera, gBack_screen, gDepth_buffer);
    BrActorRemove(gFrontend_billboard_actors[pIndex]);
}

// FUNCTION: CARMA2_HW 0x004709b0
// FUNCTION: CARMA2_SW 0x00470060
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
// FUNCTION: CARMA2_SW 0x00470100
void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth) {

    while (PolyFontTextWidth(pFont, pText) > pWidth) {
        pText[strlen(pText) + -1] = '\0';
    }
}

// FUNCTION: CARMA2_HW 0x00471ba0
int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY) {
    tFrontend_slider* slider;
    br_vertex* vertices;
    tFrontend_item_spec* item;
    tFrontend_brender_item* brender_item;
    int i;

    item = gCurrent_frontend_spec->items;
    brender_item = gFrontend_brender_items;
    for (i = 0; i < pFrontend->count_items; i++, brender_item++, item++) {
        vertices = brender_item->model->vertices;
        if (pX >= vertices[1].p.v[0] && pX <= vertices[3].p.v[0]
                && pY >= -vertices[0].p.v[1] && pY <= -vertices[1].p.v[1]
                && item->visible
                && item->enabled >= 1) {
            return i;
        }
    }
    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        for (i = 0; i < 3; i++) {
            vertices = gFrontend_brender_items[slider->itemid_start + i].model->vertices;
            if (pX >= vertices[1].p.v[0] && pX <= vertices[3].p.v[0]
                    && pY >= -vertices[0].p.v[1] && pY <= -vertices[1].p.v[1]) {
                return slider->itemid_start + i;
            }
        }
    }
    return -1;
}

// TranslateSliderItem

// GetActiveSlider

// FUNCTION: CARMA2_HW 0x00471cf0
tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void) {
    tFrontend_slider* slider;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_slider, itemid_left_reference, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_slider, itemid_start, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFrontend_slider, next, 0x20);

    for (slider = gCurrent_frontend_scrollbars; slider != NULL; slider = slider->next) {
        if (gFrontend_selected_item_index >= slider->itemid_left_reference
                && gFrontend_selected_item_index < slider->itemid_left_reference + 3) {
            return slider;
        }
        if (gFrontend_selected_item_index >= slider->itemid_start
                && gFrontend_selected_item_index < slider->itemid_start + 3) {
            return slider;
        }
    }
    return NULL;
}

// PrepareSliders

#ifdef REC2_HW
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
#endif

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

// FUNCTION: CARMA2_HW 0x00471d30
tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem) {
    tConnected_items* scrollset;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, range_length, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, count_ranges, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, range_starts, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, next, 0x2c);

    for (scrollset = gConnected_items; scrollset != NULL; scrollset = scrollset->next) {
        for (i = 0; i < scrollset->count_ranges; i++) {
            if (pItem >= scrollset->range_starts[i]
                    && pItem < scrollset->range_starts[i] + scrollset->range_length) {
                return scrollset;
            }
        }
    }
    return NULL;
}

// ScrollSet_DisplayEntry

// ScrollSet_GetItem

// GetUpdown

// FUNCTION: CARMA2_HW 0x00471d70
int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem) {
    int offset;
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, range_length, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, count_ranges, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tConnected_items, range_starts, 0x10);

    offset = -1;
    for (i = 0; i < pConnected->count_ranges; i++) {
        if (pItem >= pConnected->range_starts[i]
                && pItem < pConnected->range_starts[i] + pConnected->range_length) {
            offset = pItem - pConnected->range_starts[i];
            break;
        }
    }
    if (offset == -1) {
        return -1;
    }
    return pConnected->field_0x8 + offset;
}

// Generic_FindNextActiveItem

// Generic_FindPrevActiveItem

// Generic_MungeActiveItems

// Generic_UnMungeActiveItems

// Generic_MenuHandler

// FUNCTION: CARMA2_HW 0x004b4020
void C2_HOOK_FASTCALL StripControls(char* pStr) {
    int i;
    int len;

    len = strlen(pStr);
    for (i = 0; i < len; i++) {
        if ((tU8)pStr[i] < 0x20 /* ' ' */) {
#ifdef REC2_FIX_BUGS
            memmove(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#else
            memcpy(&pStr[i], &pStr[i + 1], (len - i) * sizeof(char));
#endif
            len--;
#ifdef REC2_FIX_BUGS
            /* correctly handle stripping consecutive control characters */
            i--;
#endif
        }
    }
}

// BackupKeyMappings

// LoadKeyNames

// DisposeKeyNames

// FUNCTION: CARMA2_HW 0x004720e0
void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gControls_scroller.range_length; i++) {

        strcpy(pFrontend->items[43 + i].text, GetMiscString(i + 140 + gControls_scroller.field_0x8));
        strcpy(pFrontend->items[53 + i].text, gKey_names[gKey_mapping[gControls_frontend_to_key_mapping_lut[i + gControls_scroller.field_0x8]] + 2]);
    }
    if (gControls_scroller.field_0x8) {
        pFrontend->items[63].visible = 1;
    } else {
        pFrontend->items[63].visible = 0;
    }
    if (gControls_scroller.field_0x8 == gControls_scroller.field_0x0 - gControls_scroller.range_length) {
        pFrontend->items[64].visible = 0;
    } else {
        pFrontend->items[64].visible = 1;
    }

    FuckWithWidths(pFrontend);
}

// DetermineKeyArrayIndex

// Joystick_BackupSettings

// Controls_JoystickToggle

// Controls_JoystickDpadToggle

// FUNCTION: CARMA2_HW 0x004729d0
// FUNCTION: CARMA2_SW 0x00471f60
void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider) {

    SetJoystickX((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x004729f0
// FUNCTION: CARMA2_SW 0x00471f80
void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider) {

    SetJoystickY((pSlider->field_0x18 - pSlider->field_0x14) * pSlider->value);
}

// FUNCTION: CARMA2_HW 0x00472a10
// FUNCTION: CARMA2_SW 0x00471fa0
void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider) {

    SetJoystickFFBGain((int)(pSlider->value * 100.0));
}

// DisplayJoystickSettings

// SaveAllJoystickData

// Controls_SwitchKeymapSet

// FUNCTION: CARMA2_HW 0x004725a0
// FUNCTION: CARMA2_SW 0x00471b30
int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend) {

    if (gControls_scroller.field_0x8 != 0) {
        gControls_scroller.field_0x8 -= 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004725c0
// FUNCTION: CARMA2_SW 0x00471b50
int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend) {

    if (gControls_scroller.field_0x8 < gControls_scroller.field_0x0 - gControls_scroller.range_length) {
        gControls_scroller.field_0x8 += 1;
        RefreshScrollSet(pFrontend);
    }
    return 0;
}

// Controls_Infunc

// Controls_Outfunc

// Controls_SlotActivated

// Controls_Ok

// DisplayNetworkOptions

// BackupNetworkOptions

// RestoreNetworkOptions

// NetOptions_Infunc

// FUNCTION: CARMA2_HW 0x00473210
// FUNCTION: CARMA2_SW 0x00472780
int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend) {
    return 0;
}

// NetOptions_CreditsRoller

// NetOptions_TargetRoller

// NetOptions_Ok

// NetOptions_Cancel

// CheckPlayersAreResponding

// MungePlayers

// NetSynchRaceStart2

// NetSync_Draw

// FUNCTION: CARMA2_HW 0x004735a0
// FUNCTION: CARMA2_SW 0x00472b10
int C2_HOOK_FASTCALL NetSync_Start(tFrontend_spec* pFrontend) {

    if (gNumber_of_net_players <= 1) {
        return 0;
    }
    SignalToStartRace();
    gStart_race_sent = 1;
    gNo_races_yet = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x004735d0
// FUNCTION: CARMA2_SW 0x00472b40
int C2_HOOK_FASTCALL NetSync_Abort(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing || gNet_mode != eNet_mode_none) {
        if (gProgram_state.racing) {
            gAbandon_game = 1;
        }
        gProgram_state.prog_status = eProg_idling;
        gNo_current_game = 1;
    }
    return 1;
}

// NetSync_Infunc

// NetSync_MenuHandler

// NetSummary_Draw

// SortScores

// SortGameScores

// NetworkSummarySetup

// NetSummary_Infunc

// NetSummary_MenuHandler

void C2_HOOK_FASTCALL DisplayVolumeSettings(tFrontend_spec* pFrontend) {

    gOptions_sound_slider.value = (float)(gProgram_state.effects_volume / 255.0);
    if (gProgram_state.music_volume >= 128) {
        SelectThisItemIn(pFrontend, 1, 14);
    } else {
        SelectThisItemIn(pFrontend, 1, 15);
    }
}

// FUNCTION: CARMA2_HW 0x00474660
void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider) {

    gProgram_state.effects_volume = (int)(255.0 * pSlider->value);
    SetSoundVolumes(0);
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
}

// FUNCTION: CARMA2_HW 0x00474530
int C2_HOOK_FASTCALL Options_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    gOptions_sound_slider.flags = 0;
    gOptions_sound_slider.itemid_left_reference = 10;
    gOptions_sound_slider.itemid_start = 6;
    gOptions_sound_slider.width = 236;
    gOptions_sound_slider.callback = Slider_EffectsProc;
    gOptions_sound_slider.field_0x14 = 0.0f;
    gOptions_sound_slider.field_0x18 = 1.0f;
    gOptions_sound_slider.next = NULL;
    gCurrent_frontend_scrollbars = &gOptions_sound_slider;
    DisplayVolumeSettings(pFrontend);
    if (gProgram_state.racing) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_disabled;
        if (gProgram_state.field_0x2c) {
            pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
        } else {
            pFrontend->items[1].enabled = kFrontendItemEnabled_disabled;
        }
    }
    if (gNet_mode == eNet_mode_none) {
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474690
int C2_HOOK_FASTCALL Options_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    SaveOptions();
    return 1;
}

// FUNCTION: CARMA2_HW 0x004746a0
int C2_HOOK_FASTCALL Options_MusicOn(tFrontend_spec* pFrontend) {

    gProgram_state.music_volume = 255;
    DisplayVolumeSettings(pFrontend);
    SetSoundVolumes(1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004746e0
int C2_HOOK_FASTCALL Options_MusicOff(tFrontend_spec* pFrontend) {

    gProgram_state.music_volume = 0;
    DisplayVolumeSettings(pFrontend);
    SetSoundVolumes(1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00474720
int C2_HOOK_FASTCALL Options_Ok(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00474730
int C2_HOOK_FASTCALL Options_AbortRace(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing) {
        gAbandon_game = 1;
        if (gNet_mode != eNet_mode_host) {
            gProgram_state.prog_status = eProg_idling;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474760
int C2_HOOK_FASTCALL Options_AbortGame(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing || gNet_mode != eNet_mode_none) {
        if (gProgram_state.racing) {
            gAbandon_game = 1;
        }
        gProgram_state.prog_status = eProg_idling;
        gNo_current_game = 1;
        return 1;
    } else {
        gValid_stashed_save_game = gProgram_state.racing;
        ShutdownNetIfRequired();
        LoadRaces(gRace_list, &gNumber_of_races, -1);
        InitGame(gDev_initial_race);
        gFrontend_next_menu = kFrontend_menu_main;
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004747d0
int C2_HOOK_FASTCALL Options_Quit(tFrontend_spec* pFrontend) {

    PDInitJoysticks();
    QuitGame();
    return 2;
}

// FUNCTION: CARMA2_HW 0x004747e0
// FUNCTION: CARMA2_SW 0x00473d40
int C2_HOOK_FASTCALL Graphics_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    GetGraphicsSettingsData(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474850
// FUNCTION: CARMA2_SW 0x00473db0
int C2_HOOK_FASTCALL Graphics_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    SetGraphicsSettingsData(pFrontend);
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474860
void C2_HOOK_FASTCALL FrontEndShowMouse(void) {

    gFrontend_suppress_mouse = 0;
}

// FUNCTION: CARMA2_HW 0x00474870
void C2_HOOK_FASTCALL FrontEndHideMouse(void) {

    gFrontend_suppress_mouse = 1;
}
