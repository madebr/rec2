#include "72-interface.h"

#include "08-loading1.h"
#include "19-font.h"
#include "41-utility.h"
#include "42-input.h"
#include "52-errors.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

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

// FUNCTION: CARMA2_HW 0x00466450
int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x004677d0
int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY) {
    int i;

    for (i = 0; i < pFrontend->count_items; i++) {

        if (pX >= gFrontend_brender_items[i].model->vertices[1].p.v[0]
                && pX <= gFrontend_brender_items[i].model->vertices[3].p.v[0]
                && pY >= -gFrontend_brender_items[i].model->vertices[0].p.v[1]
                && pY <= -gFrontend_brender_items[i].model->vertices[1].p.v[1]
                && pFrontend->items[i].enabled > 0 /* default or disabled */
                && pFrontend->items[i].visible) {
            return i;
        }
    }
    return -1;
}

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

// GetThisFuckingPixelmapPleaseMrTwatter

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

// FindPrevActiveItem

// RaceIndex

// FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt

// RefreshRacesScroller

// NetworkUpdateSelectedGameInfo

// UpdateNetTrackScroller

// RefreshNetRacesScroller

// NetRaceUp

// NetRaceDn

// SaveSinglePlayerState

// RestoreSinglePlayerState

// MaybeRestoreSavedGame

// NetworkJoinUpdateScroller

// UpdateScrollPositions

// ToggleTyping

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

// FRONTEND_CreateMenuButton

// FRONTEND_CreateMenu

// FRONTEND_DestroyMenu

// FRONTEND_PingPongFlash

// FRONTEND_DrawMenu

// FRONTEND_Setup2D

// FRONTEND_Setup

// FRONTEND_Redraw

// MaybeDoMouseCursor

// ResetInterfaceTimeout

// FRONTEND_Main

// ProcessInputString

// StopGettingInputString

// StartGettingInputString

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

// HeirarchyPick

// DodgyPause

// MorphBlob

// ScrollCredits

// LoadGameInFunc

// CreateAPOactor

// KillAPOactor

// LoadGameOutFunc

// LoadGameScrollUp

// LoadGameScrollDown

// LoadSlot1

// LoadSlot2

// LoadSlot3

// LoadSlot4

// LoadSlot5

// LoadSlot6

// LoadSlot7

// LoadSlot8

// TryToLoadGame

// LoadGameUpdateFunc

// PrepareAPO

// PrintAPO

// BuildAPO

// PolyClipName

// Ians_GetItemAtMousePos

// TranslateSliderItem

// GetActiveSlider

// GetAnyActiveSlider

// PrepareSliders

// Morph_Initialise

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

// Generic_LinkInEffect

// Generic_LinkOutEffect

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
