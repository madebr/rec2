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
C2_HOOK_VARIABLE_DECLARE(char*, gFrontend_current_input);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_maximum_input_length);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gFrontend_original_player_name, 32);
C2_HOOK_VARIABLE_DECLARE(int, gFrontend_text_input_item_index);
C2_HOOK_VARIABLE_DECLARE(int, gINT_0059b0d8);


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

void C2_HOOK_FASTCALL ScrollCredits(void);

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void);

void C2_HOOK_FASTCALL Generic_LinkInEffect(void);

void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActorIdx, int pAPO);

void C2_HOOK_FASTCALL PrepareAPO(int pActorIdx);

int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend);

void C2_HOOK_FASTCALL PrintAPO(int pX, int pY, int pIndex, int pTex_index);

void C2_HOOK_FASTCALL FRONTEND_PingPongFlash(void);

void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps);

void C2_HOOK_FASTCALL FuckWithWidths(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_UpdateScrollerModels(tFrontend_spec* pFrontend, int pIndex);

void C2_HOOK_FASTCALL RefreshRacesScroller(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL GetItemAtMousePos(tFrontend_spec *pFrontend,int mouse_x,int mouse_y);

int C2_HOOK_FASTCALL FindPrevActiveItem(tFrontend_spec* pFrontend, int pStart_index);

int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL Morph_Initialise(tFrontend_spec* pCurrent, tFrontend_spec* pNext);

int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_DefaultItem_Action(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_Default_Destroy(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_Default_Tick(tFrontend_spec* pFrontend);

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

void C2_HOOK_FASTCALL DefaultInfunc(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FillInRaceDescription(char *pDest, int pRace_index);

void C2_HOOK_FASTCALL MenuSetCarImage(int pCar_index, int pBrender_index);

int C2_HOOK_FASTCALL ProcessInputString(void);

void C2_HOOK_FASTCALL FrontEndShowMouse(void);

void C2_HOOK_FASTCALL FrontEndHideMouse(void);

void C2_HOOK_FASTCALL DodgyPause(int pTime);

void C2_HOOK_FASTCALL StartGettingInputString(char* pBuffer, int pBuffer_size);

void C2_HOOK_FASTCALL StopGettingInputString(void);

int C2_HOOK_FASTCALL ToggleTyping(tFrontend_spec* pFrontend);

#endif //REC2_FRONTEND_H
