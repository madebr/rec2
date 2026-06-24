#include "72-interface.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x0068c6e0
int gCount_interface_strings;

// GLOBAL: CARMA2_HW 0x0068c230
char* gInterface_strings[300];

// temp

// GetItemAtMousePos

// MungeButtonModels

// ScrollUp

// ScrollDn

// SetLevelBar

// ToggleSelection

// FuckWithWidths

// GetThisFuckingPixelmapPleaseMrTwatter

// LoadMenuSettings

// LoadMenuImages

// LoadMenuModels

// DefaultInfunc

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

// Generic_Infunc

// Generic_Outfunc

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

// FrontEndShowMouse

// FrontEndHideMouse

// FUNCTION: CARMA2_HW 0x00484fd0
void C2_HOOK_FASTCALL IString_Load(void) {
    tPath_name the_path;
    FILE* f;
    char s[256];
    int i;

    if (gCount_interface_strings != 0) {
        return;
    }
    PathCat(the_path, gApplication_path, "INTRFACE");
    PathCat(the_path, the_path, "interface.txt");
    f = PFfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    gCount_interface_strings = 0;
    for (i = 0; i < (int)REC2_ASIZE(gInterface_strings); i++) {
        if (PFfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        gInterface_strings[i] = BrMemAllocate(strlen(s) + 1, kMem_misc_string);
        strcpy(gInterface_strings[i], s);
        gCount_interface_strings += 1;
    }
    PFfclose(f);
}

// IString_Get

