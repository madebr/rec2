/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_INTERFACE_H
#define GUARD_72_INTERFACE_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tFrontend_brender_item gFrontend_brender_items[100];
extern tFrontend_spec* gCurrent_frontend_spec;

extern int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend, int pX, int pY);

extern void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pIndex);

// ScrollUp

// ScrollDn

// static void C2_HOOK_FASTCALL SetLevelBar(tFrontend_spec* pFrontend, int pLevel_index, int pSelected_item);

extern void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);

extern br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName);

extern void C2_HOOK_FASTCALL LoadMenuSettings(tFrontend_spec* pFrontend);

// LoadMenuImages

// LoadMenuModels

extern void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend);

// FindNextActiveItem

extern int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);

// RaceIndex

extern int C2_HOOK_FASTCALL FuckingMakeTheFuckingRaceAndGroupsDisplayHaveTheRightCuntingStuffInIt(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend);

// NetworkUpdateSelectedGameInfo

// UpdateNetTrackScroller

extern void C2_HOOK_FASTCALL RefreshNetRacesScroller(tFrontend_spec* pFrontend);

// NetRaceUp

// NetRaceDn

// SaveSinglePlayerState

// RestoreSinglePlayerState

// MaybeRestoreSavedGame

// NetworkJoinUpdateScroller

extern void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);

// NetGameToggleTyping

extern int C2_HOOK_FASTCALL ChooseSkill(tFrontend_spec* pFrontend);

// NewGameOutfunc

extern int C2_HOOK_FASTCALL NewGameInfunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend);

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

extern int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend);

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

extern int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend);

// ChangeCarOK

// CarClickPrev

// CarClickNext

extern int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend);

// FRONTEND_ChangeCarMenuHandler

// StartGameInfunc

extern int C2_HOOK_FASTCALL FRONTEND_StartGameMenuHandler(tFrontend_spec* pFrontend);

// CreditsScreenInfunc

// CreditsScreenOutfunc

// FRONTEND_GenericMenuHandler

extern void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* pFrontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);

extern int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

extern int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void);

// FRONTEND_DrawMenu

// FRONTEND_Setup2D

// FRONTEND_Setup

// FRONTEND_Redraw

extern void C2_HOOK_FASTCALL MaybeDoMouseCursor(void);

extern void C2_HOOK_FASTCALL ResetInterfaceTimeout(void);

extern int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend_type);

extern int C2_HOOK_FASTCALL ProcessInputString(void);

extern int C2_HOOK_FASTCALL StopGettingInputString(void);

extern int C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size);

// DisposeWrecksGallery

// ScrollToNextCar

// ScrollToPrevCar

// BuyCurrentCar

// WrecksInFunc

extern int C2_HOOK_FASTCALL KeepInRange(int pIndex, int pRange);

// WrecksOutFunc

// WrecksUpdateFunc

// GotItAlready

// GetCarSelectedByMouse

// WreckPick

extern intptr_t C2_HOOK_CDECL HeirarchyPick(br_actor* a, void* ref);

extern void C2_HOOK_FASTCALL DodgyPause(tU32 pTime);

extern void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps);

extern void C2_HOOK_FASTCALL ScrollCredits(void);

// LoadGameInFunc

extern br_actor* C2_HOOK_FASTCALL CreateAPOactor(void);

extern void C2_HOOK_FASTCALL KillAPOactor(br_actor* pActor);

extern int C2_HOOK_FASTCALL LoadGameOutFunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadGameScrollUp(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadGameScrollDown(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot1(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot2(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot3(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot4(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot5(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot6(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot7(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL LoadSlot8(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL TryToLoadGame(int pN);

extern int C2_HOOK_FASTCALL LoadGameUpdateFunc(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL PrepareAPO(int pActor_index);

extern void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index);

extern void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActor_index, int pAPO);

extern void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth);

extern int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY);

// TranslateSliderItem

// GetActiveSlider

extern tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void);

// PrepareSliders

extern void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);

extern int C2_HOOK_FASTCALL Generic_Infunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend);

// Generic_EventEffect

extern void C2_HOOK_FASTCALL Generic_LinkInEffect(void);

extern void C2_HOOK_FASTCALL Generic_LinkOutEffect(void);

extern tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem);

// ScrollSet_DisplayEntry

// ScrollSet_GetItem

// GetUpdown

extern int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem);

// Generic_FindNextActiveItem

// Generic_FindPrevActiveItem

// Generic_MungeActiveItems

// Generic_UnMungeActiveItems

// Generic_MenuHandler

extern void C2_HOOK_FASTCALL StripControls(char* pStr);

// BackupKeyMappings

// LoadKeyNames

// DisposeKeyNames

extern void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend);

// DetermineKeyArrayIndex

// Joystick_BackupSettings

// Controls_JoystickToggle

// Controls_JoystickDpadToggle

extern void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider);

extern void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider);

extern void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider);

// DisplayJoystickSettings

// SaveAllJoystickData

// Controls_SwitchKeymapSet

extern int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend);

// Controls_Infunc

// Controls_Outfunc

// Controls_SlotActivated

// Controls_Ok

// DisplayNetworkOptions

// BackupNetworkOptions

// RestoreNetworkOptions

// NetOptions_Infunc

extern int C2_HOOK_FASTCALL NetOptions_Outfunc(tFrontend_spec* pFrontend);

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

extern void C2_HOOK_FASTCALL DisplayVolumeSettings(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider);

extern int C2_HOOK_FASTCALL Options_Infunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_Outfunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_MusicOn(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_MusicOff(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_Ok(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_AbortRace(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_AbortGame(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL Options_Quit(tFrontend_spec* pFrontend);

// Graphics_Infunc

extern int C2_HOOK_FASTCALL Graphics_Outfunc(tFrontend_spec* pFrontend);

extern void C2_HOOK_FASTCALL FrontEndShowMouse(void);

extern void C2_HOOK_FASTCALL FrontEndHideMouse(void);

extern void C2_HOOK_FASTCALL IString_Load(void);

extern void C2_HOOK_FASTCALL IString_Free(void);

extern const char* C2_HOOK_FASTCALL IString_Get(int pIndex);

#endif // GUARD_72_INTERFACE_H
