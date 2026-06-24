#ifndef GUARD_72_INTERFACE_H
#define GUARD_72_INTERFACE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);

extern void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pIndex);

// ScrollUp

// ScrollDn

// static void C2_HOOK_FASTCALL SetLevelBar(tFrontend_spec* pFrontend, int pLevel_index, int pSelected_item);

extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend) ;

// GetThisFuckingPixelmapPleaseMrTwatter

extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

// LoadMenuImages

// LoadMenuModels

extern void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend);

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

extern int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend);

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

extern void C2_HOOK_FASTCALL FrontEndShowMouse(void);

extern void C2_HOOK_FASTCALL FrontEndHideMouse(void);

extern void C2_HOOK_FASTCALL IString_Load(void);

extern void C2_HOOK_FASTCALL IString_Free(void);

extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);

#endif // GUARD_72_INTERFACE_H
