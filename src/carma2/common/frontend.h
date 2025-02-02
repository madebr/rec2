#ifndef REC2_FRONTEND_H
#define REC2_FRONTEND_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE_ARRAY(br_actor*, gFrontend_billboard_actors, 8);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gFrontend_menu_camera);
C2_HOOK_VARIABLE_DECLARE(double, gFrontend_throb_factor);
C2_HOOK_VARIABLE_DECLARE(int, gTyping_slot);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_leave_current_menu);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_suppress_mouse);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_actor*, gFrontend_backdrop_actors, 3);
C2_HOOK_VARIABLE_DECLARE(br_uint_32, gFrontend_APO_Colour_1);
C2_HOOK_VARIABLE_DECLARE(br_uint_32, gFrontend_APO_Colour_2);
C2_HOOK_VARIABLE_DECLARE(br_uint_32, gFrontend_APO_Colour_3);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gFrontend_wrecks_actor);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gFrontend_wrecks_camera);
C2_HOOK_VARIABLE_DECLARE(br_pixelmap*, gFrontend_wrecks_pixelmap);
C2_HOOK_VARIABLE_DECLARE(tFrontendMenuType, gFrontend_next_menu);
C2_HOOK_VARIABLE_DECLARE(tConnected_items*, gConnected_items);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_scrollbars_updated);
C2_HOOK_VARIABLE_DECLARE(tFrontend_spec*, gCurrent_frontend_spec);
C2_HOOK_VARIABLE_DECLARE(char*, gFrontend_current_input);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_maximum_input_length);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gFrontend_original_player_name, 32);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_text_input_item_index);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0059b0d8);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_selected_item_index);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tFrontend_brender_item, gFrontend_brender_items, 100);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_last_scroll);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_time_last_input);
C2_HOOK_VARIABLE_DECLARE(tStruct_00686508*, gPTR_00686508);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider*, gCurrent_frontend_scrollbars);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00688444);
C2_HOOK_VARIABLE_DECLARE(tConnected_items, gControls_scroller);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gControls_frontend_to_key_mapping_lut, 29);
C2_HOOK_VARIABLE_DECLARE(tU32, gAuthor_credits_scroll_start_time);
C2_HOOK_VARIABLE_DECLARE(int, gAuthor_credits_line_count);
C2_HOOK_VARIABLE_DECLARE(int*, gAuthor_credits_heights);
C2_HOOK_VARIABLE_DECLARE(int*, gAuthor_credits_throbs);
C2_HOOK_VARIABLE_DECLARE(char**, gAuthor_credits_texts);
C2_HOOK_VARIABLE_DECLARE(int*, gAuthor_credits_fonts);
C2_HOOK_VARIABLE_DECLARE(int, gAuthor_credits_total_height);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_scroll_time_left);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_scroll_time_increment);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrontend_scroll_last_update);


void C2_HOOK_FASTCALL IString_Load(void);

void C2_HOOK_FASTCALL FreeInterfaceStrings(void);

const char* C2_HOOK_FASTCALL IString_Get(int pIndex);

void C2_HOOK_FASTCALL LoadMenuImages(void);

void C2_HOOK_FASTCALL FRONTEND_Setup2D(void);

br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmapPleaseMrTwatter(const char* pFolder, const char* pName);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_CreateMenuButton(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontendType);

int C2_HOOK_FASTCALL FRONTEND_Redraw(void);

void C2_HOOK_FASTCALL ScrollCredits(void);

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void);

void C2_HOOK_FASTCALL Generic_LinkInEffect(void);

void C2_HOOK_FASTCALL Generic_LinkOutEffect(void);

void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActorIdx, int pAPO);

void C2_HOOK_FASTCALL PrepareAPO(int pActorIdx);

int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend);

void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index);

void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void);

void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps);

void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL MungeButtonModels(tFrontend_spec* pFrontend, int pIndex);

void C2_HOOK_FASTCALL UpdateScrollPositions(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL DetermineKeyArrayIndex(void);

int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend,int mouse_x,int mouse_y);

int C2_HOOK_FASTCALL FindNextActiveItem(tFrontend_spec* pFrontend, int pStart_index);

int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);

void C2_HOOK_FASTCALL ToggleSelection(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);

int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL temp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Generic_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Ians_GetItemAtMousePos(tFrontend_spec* pFrontend, int pX, int pY);

tFrontend_slider* C2_HOOK_FASTCALL GetAnyActiveSlider(void);

int C2_HOOK_FASTCALL ScrollSet_TranslateItemToIndex(tConnected_items* pConnected, int pItem);

tConnected_items* C2_HOOK_FASTCALL GetScrollSet(int pItem);

void C2_HOOK_FASTCALL ScrollSet_DisplayEntry(tConnected_items* pScroll_set, int pItem);

void C2_HOOK_FASTCALL RefreshScrollSet(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Generic_FindNextActiveItem(tFrontend_spec* pFrontend, int pItem);

int C2_HOOK_FASTCALL Generic_FindPrevActiveItem(tFrontend_spec* pFrontend, int pItem);

int C2_HOOK_FASTCALL TranslateSliderItem(tFrontend_slider* pScroller, int pIndex);

void C2_HOOK_FASTCALL PrepareSliders(tFrontend_spec* pFrontend);

tFrontend_slider* C2_HOOK_FASTCALL GetActiveSlider(void);

void C2_HOOK_FASTCALL Generic_EventEffect(void);

tStruct_00686508* C2_HOOK_FASTCALL GetUpDown(int pItem);

int C2_HOOK_FASTCALL Generic_MenuHandler(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL Generic_MungeActiveItems(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL Generic_UnMungeActiveItems(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_DrawMenu(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL MaybeDoMouseCursor(void);

void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

char* C2_HOOK_FASTCALL MungeCommas(int pValue);

br_actor* C2_HOOK_FASTCALL CreateAPOactor(void);

void C2_HOOK_FASTCALL KillAPOactor(br_actor* pActor);

void C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pKey, char* pRepl);

void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pKey, char pChar);

void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pKey, int pNum);

void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h);

void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FillInRaceDescription(char *pDest, int pRace_index);

void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);

void C2_HOOK_FASTCALL MenuSetDriverImage(int pOpponent_index, int pFrontend_index);

int C2_HOOK_FASTCALL ProcessInputString(void);

void C2_HOOK_FASTCALL FrontEndShowMouse(void);

void C2_HOOK_FASTCALL FrontEndHideMouse(void);

void C2_HOOK_FASTCALL DodgyPause(tU32 pTime);

void C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size);

void C2_HOOK_FASTCALL StopGettingInputString(void);

int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL RaceIndex(const char* pName);

void C2_HOOK_FASTCALL ScrollUp(tFrontend_spec* pFrontend, int pScroller);

void C2_HOOK_FASTCALL ScrollDn(tFrontend_spec* pFrontend, int pScroller);

void C2_HOOK_FASTCALL SelectThisItemIn(tFrontend_spec* pFrontend, int pGroup, int pValue);

int C2_HOOK_FASTCALL WhichItemIsSelectedIn(tFrontend_spec* pFrontend, int pGroup);

#endif //REC2_FRONTEND_H
