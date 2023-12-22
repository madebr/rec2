#ifndef REC2_WORLD_H
#define REC2_WORLD_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(br_scalar, gSight_distance_squared);
C2_HOOK_VARIABLE_DECLARE(tBrender_storage*, gStorageForCallbacks);
C2_HOOK_VARIABLE_DECLARE(int, gGroovidelics_array_size);
C2_HOOK_VARIABLE_DECLARE(tGroovidelic_spec*, gGroovidelics_array);
C2_HOOK_VARIABLE_DECLARE(tSpecial_volume*, gDefault_water_spec_vol_real);
C2_HOOK_VARIABLE_DECLARE(tMaterial_exception*, gMaterial_exceptions);

tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);

void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void);

void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);

void C2_HOOK_FASTCALL ApplyMaterialCallbackOnAllMaterials(br_model* pModel, material_cbfn* pCallback);

int C2_HOOK_CDECL ApplyMaterialCallbackOnAllModelActorsCallback(br_actor* pActor, material_cbfn* pCallback);

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

tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

void C2_HOOK_FASTCALL LoadShadeTableCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomeShadeTables(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadPixelmapCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomePixelmaps(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadMaterialCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomeMaterialsWithShading(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

int C2_HOOK_FASTCALL LoadMaterialsInto(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading);

int C2_HOOK_FASTCALL LoadModelsInto(tBrender_storage* pStorage_space, const char* pPath);

void C2_HOOK_FASTCALL LoadModelCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadSomeModels(tBrender_storage *pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadSomeModels(tBrender_storage *pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadFolderInStorageWithShading(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);

void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);

void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);

tGroovidelic_spec* C2_HOOK_FASTCALL FindGroovidelicForActor(br_actor* pActor);

tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void);

#endif // REC2_WORLD_H
