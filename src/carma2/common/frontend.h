#ifndef REC2_FRONTEND_H
#define REC2_FRONTEND_H

#include "rec2_types.h"

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL LoadFrontendStrings(void);

void C2_HOOK_FASTCALL CreateMenuActors(void);

br_pixelmap* C2_HOOK_FASTCALL Frontend_LoadFrontendPixelmap(const char* pFolder, const char* pName);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_CreateItemBrenderObjects(tFrontend_brender_item* Frontend_brender_item, tS16 pX, tS16 pY, tS16 pWidth, tS16 pHeight, br_colour pColour, br_pixelmap* pMap, const char* pText);

int C2_HOOK_FASTCALL FRONTEND_CreateMenu(tFrontend_spec* pFrontend_spec);

void C2_HOOK_FASTCALL FRONTEND_Setup(tFrontendMenuType pType) ;

#endif //REC2_FRONTEND_H
