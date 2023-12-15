#ifndef REC2_WORLD_H
#define REC2_WORLD_H

#include "c2_hooks.h"

#include "c2_stdio.h"

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

int C2_HOOK_FASTCALL LoadBrenderTextures(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity);

void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

int C2_HOOK_FASTCALL AddTexturePixTifFileStemToList(const char *path, tName_list *pList);

int C2_HOOK_FASTCALL AddTextureFileStemToList(const char* path, tName_list* pList);

void C2_HOOK_FASTCALL InitialiseStorageSpace(int pUnknown, tBrender_storage* pStorage_space, int pMax_pixelmaps, int pMax_shade_tables, int pMax_materials, int pMax_models, int pMax_sounds);

void C2_HOOK_FASTCALL DisposeStorageSpace(tBrender_storage* pStorage);

tAdd_to_storage_result C2_HOOK_FASTCALL AddPixelmapToStorage(tBrender_storage* pStorage_space, br_pixelmap* pThe_pm);

tAdd_to_storage_result C2_HOOK_FASTCALL LoadSoundInStorage(tBrender_storage* pStorage_space, int pSound_id);

int C2_HOOK_FASTCALL IsValidFile(const char* path);

int C2_HOOK_FASTCALL GetLastModificationTime(const char* path);

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex2(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode, int useTiffx);

br_pixelmap* C2_HOOK_FASTCALL LoadTiffTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

br_pixelmap* C2_HOOK_FASTCALL LoadTiffOrBrenderTexture_Ex(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

int C2_HOOK_FASTCALL LoadTiffOrBrenderTexture(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity);

int C2_HOOK_FASTCALL LoadNPixelmapsFromPath(tBrender_storage* pStorage_space, const char* path);

void C2_HOOK_FASTCALL LoadAllTexturesFromTexSubdirectories(tBrender_storage* pStorage_space, const char* path);

int C2_HOOK_FASTCALL ResolveTexturePathLink(char* realPath, const char* path);

int C2_HOOK_FASTCALL CreatePathLink(const char* targetPath, const char* linkPath);

int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2);

int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) ;

int C2_HOOK_FASTCALL FindLastOccurrenceOfString_CaseInsensitive(int* offset, const char* haystack, size_t haystackLen, const char* needle);

void C2_HOOK_FASTCALL LoadShadeTableCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomeShadeTables(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadPixelmapCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomePixelmaps(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadMaterialCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomeMaterialsWithShading(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

int C2_HOOK_FASTCALL LoadMaterialsInto(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

#endif // REC2_WORLD_H
