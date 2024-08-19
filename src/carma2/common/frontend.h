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


void C2_HOOK_FASTCALL LoadFrontendStrings(void);

void C2_HOOK_FASTCALL FreeInterfaceStrings(void);

const char* C2_HOOK_FASTCALL GetInterfaceString(int pIndex);

void C2_HOOK_FASTCALL LoadMenuImages(void);

void C2_HOOK_FASTCALL CreateMenuActors(void);

br_pixelmap* C2_HOOK_FASTCALL Frontend_LoadFrontendPixelmap(const char* pFolder, const char* pName);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_CreateItemBrenderObjects(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pFrontendType);

void C2_HOOK_FASTCALL FRONTEND_RenderAuthorCredits(void);

void C2_HOOK_FASTCALL ResetInterfaceTimeout(void);

void C2_HOOK_FASTCALL Generic_LinkInEffect(void);

void C2_HOOK_FASTCALL BuildAPO(int pCurrent, int pPotential, int pActorIdx, int pAPO);

int C2_HOOK_FASTCALL FRONTEND_Main(tFrontendMenuType pFrontend);

void C2_HOOK_FASTCALL RenderFrontendBillboard(int pX, int pY, int pIndex, int pTex_index);

void C2_HOOK_FASTCALL UpdateThrobFactor(void);

void C2_HOOK_FASTCALL MorphBlob(br_model* pModel_from, br_model* pModel_to, br_model* pModel, int pStep, int pCount_steps);

void C2_HOOK_FASTCALL FRONTEND_CompleteItemSizes(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_UpdateScrollerModels(tFrontend_spec* pFrontend, int pIndex);

void C2_HOOK_FASTCALL FRONTEND_MainMenu_UpdateRaces(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_FindItemUnderMouse(tFrontend_spec *pFrontend,int mouse_x,int mouse_y);

int C2_HOOK_FASTCALL FRONTEND_FindVisibleItem(tFrontend_spec* pFrontend, int pStart_index);

int C2_HOOK_FASTCALL FRONTEND_DestroyMenu(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_SetTransitionModels(tFrontend_spec* pCurrent, tFrontend_spec* pNext);

int C2_HOOK_FASTCALL FRONTEND_GenericMenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_DefaultItem_Action(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_Default_Destroy(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_Default_Tick(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_RenderItems(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL FRONTEND_DoMouse(void);

void C2_HOOK_FASTCALL FRONTEND_RenderPolyTextLine(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender);

#endif //REC2_FRONTEND_H
