#ifndef REC2_WORLD_H
#define REC2_WORLD_H

#include "c2_hooks.h"

#include "rec2_types.h"

tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);

void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void);

void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);

intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag);

void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn);

int C2_HOOK_FASTCALL GetAccessoryRendering(void);

void C2_HOOK_FASTCALL ParseSpecialVolume(tTWTFILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

int C2_HOOK_FASTCALL AddTexturePixTifFileStemToList(const char *path, tName_list *pList);

int C2_HOOK_FASTCALL AddTextureFileStemToList(const char* path, tName_list* pList);

int C2_HOOK_FASTCALL LoadNPixelmapsFromPath(tBrender_storage* pStorage_space, const char* path);

void C2_HOOK_FASTCALL LoadAllTexturesFromTexSubdirectories(tBrender_storage* pStorage_space, const char* path);

#endif // REC2_WORLD_H
