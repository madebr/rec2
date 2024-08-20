#ifndef REC2_WORLD_H
#define REC2_WORLD_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(br_scalar, gSight_distance_squared);
C2_HOOK_VARIABLE_DECLARE(tBrender_storage*, gStorageForCallbacks);
C2_HOOK_VARIABLE_DECLARE(int, gGroovidelics_array_size);
C2_HOOK_VARIABLE_DECLARE(tGroovidelic_spec*, gGroovidelics_array);
C2_HOOK_VARIABLE_DECLARE(const char*, gSmashable_track_environment_path);
C2_HOOK_VARIABLE_DECLARE(tSmashable_environment_name*, gSmashable_noncars);
C2_HOOK_VARIABLE_DECLARE(int, gCount_smashable_noncars);
C2_HOOK_VARIABLE_DECLARE(int, gCount_smashable_noncar_shrapnel_actors);
C2_HOOK_VARIABLE_DECLARE(tMaterial_exception*, gMaterial_exceptions);
C2_HOOK_VARIABLE_DECLARE(int, gCount_track_smashable_environment_specs);
C2_HOOK_VARIABLE_DECLARE(tSmashable_item_spec*, gTrack_smashable_environment_specs);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmashable_item_spec*, gSmashable_track_indexable_triggers, 100);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tUnknown_0x006a7fc8, gUnknown_0x006a7fc8, 25);
C2_HOOK_VARIABLE_DECLARE(br_actor*, gAdditional_actors);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gSpecial_effects_boundary_choices, 4);
C2_HOOK_VARIABLE_DECLARE(int, gDefault_engine_noise_index);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gSoundGeneratorTypeNames, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gRace_path, 256);

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

int C2_HOOK_FASTCALL LoadBunchOfPixies(const char* pathRoot, const char* textureName, br_pixelmap** textureBuffer, size_t bufferCapacity);

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

br_pixelmap* C2_HOOK_FASTCALL DRLdImg(const char* texturePathDir, const char* textureName, br_pixelmap* pPalette, int flags, int* errorCode);

int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity);

int C2_HOOK_FASTCALL AddPixelmaps(tBrender_storage* pStorage_space, const char* path);

void C2_HOOK_FASTCALL TwatPIX16(const char* pPath);

void C2_HOOK_FASTCALL LoadAllImagesInDirectory(tBrender_storage* pStorage_space, const char* path);

int C2_HOOK_FASTCALL ResolveTexturePathLink(char* realPath, const char* path);

int C2_HOOK_FASTCALL CreatePathLink(const char* targetPath, const char* linkPath);

int C2_HOOK_FASTCALL DRstrcmpi(const char* str1, const char* str2);

int C2_HOOK_FASTCALL DRmemicmp(const char* str1, const char* str2, size_t count) ;

int C2_HOOK_FASTCALL FindLastOccurrenceOfString_CaseInsensitive(int* offset, const char* haystack, size_t haystackLen, const char* needle);

tAdd_to_storage_result C2_HOOK_FASTCALL AddMaterialToStorage(tBrender_storage* pStorage_space, br_material* pThe_mat);

void C2_HOOK_FASTCALL LoadIfItsAShadeTable(const char* pPath);

void C2_HOOK_FASTCALL LoadAllShadeTablesInDirectory(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadIfItsAPixelmap(const char* pPath);

br_pixelmap* C2_HOOK_FASTCALL LoadSinglePixelmap(tBrender_storage* pStorage, const char* pName);

void C2_HOOK_FASTCALL LoadAllPixelmapsInDirectory(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadIfItsAMaterial(const char* pPath);

void C2_HOOK_FASTCALL LoadAllMaterialsInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

br_material* C2_HOOK_FASTCALL LoadSingleMaterial(tBrender_storage* pStorage_space, const char* pName);

int C2_HOOK_FASTCALL AddMaterials(tBrender_storage* pStorage_space, const char* pPath, tRendererShadingType pShading);

void C2_HOOK_FASTCALL LoadSomeMaterials(tBrender_storage *pStorage, FILE* pFile, tRendererShadingType pShading);

br_pixelmap* C2_HOOK_FASTCALL LoadShadeTable(const char* pName);

br_pixelmap* C2_HOOK_FASTCALL LoadSingleShadeTable(tBrender_storage* pStorage_space, const char* pName);

tAdd_to_storage_result C2_HOOK_FASTCALL AddModelToStorage(tBrender_storage* pStorage_space, br_model* pThe_mod);

int C2_HOOK_FASTCALL AddModels(tBrender_storage* pStorage_space, const char* pPath);

void C2_HOOK_FASTCALL LoadIfItsAMode(const char* pPath);

void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadAllModelsInDirectory(tBrender_storage *pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadAllStuffInDirectory(tBrender_storage* pStorage, const char* pPath, tRendererShadingType pShading);

void C2_HOOK_FASTCALL AllowDuplicates(void);

br_pixelmap* C2_HOOK_FASTCALL SwapPixelmapInStorage(br_pixelmap* pNew, br_pixelmap* pOriginal, tBrender_storage* pStorage);

void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);

void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon);

void C2_HOOK_FASTCALL AddFunkotronics(FILE* pF, int pOwner, int pRef_offset, tCar_crush_buffer* pCar_crush_datas);

void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner);

void C2_HOOK_FASTCALL AddGroovidelics(FILE* pF, int pOwner, br_actor* pParent_actor, int pRef_offset, int pAllowed_to_be_absent);

void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner);

tGroovidelic_spec* C2_HOOK_FASTCALL FindGroovidelicForActor(br_actor* pActor);

tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void);

void C2_HOOK_FASTCALL ReadGlobalLightingInfo(FILE* pF);

void C2_HOOK_FASTCALL FUN_004f0430(void);

void C2_HOOK_FASTCALL ReadSmokeFireChance(FILE* pF, tSmokeFireChance* pSmokeFireChance);

void C2_HOOK_FASTCALL ReadSmashReplace(tSmashReplace* pReplace, FILE* pF);

void C2_HOOK_FASTCALL ReadSmashTexture(tSmashable_item_spec* pSmash_item, tSmashable_item_spec_texture_change* pTexture_change, FILE* pF);

void C2_HOOK_FASTCALL ReadSmashSounds(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage);

void C2_HOOK_FASTCALL ReadSpecialEffectsSpec(FILE* pF, tSpecial_effects_spec* pSpecial_effects);

void C2_HOOK_FASTCALL ReadVariableChanges(FILE* pF, tVariable_changes* pVariable_changes);

void C2_HOOK_FASTCALL ReadConnotations(FILE* pF, tConnotations* pConnotations, tBrender_storage* pStorage);

void C2_HOOK_FASTCALL ReadSideEffects(FILE* pF, tSide_effects* pSide_effects);

void C2_HOOK_FASTCALL ReadSmashableEnvironment(FILE* pF, const char* pPath);

int C2_HOOK_FASTCALL MaterialIsSmashableTrigger(br_material *pMaterial);

void C2_HOOK_FASTCALL DisposeSmashableTrackEnvironment(void);

void C2_HOOK_FASTCALL ResetSmashing(void);

void C2_HOOK_FASTCALL DisposeSmashEnvNonCars(void);

void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF);

void C2_HOOK_FASTCALL ReadSoundGenerators(tTrack_spec* pTrack_spec, FILE* pF);

void C2_HOOK_FASTCALL DodgyModelUpdate(br_model* pM);

void C2_HOOK_FASTCALL AddExceptionToList(tMaterial_exception** pList, tMaterial_exception* pItem);

void C2_HOOK_FASTCALL LoadExceptionsFile(const char* pPath);

void C2_HOOK_FASTCALL LoadExceptionsFileForTrack(const char* pTrack_name);

void C2_HOOK_FASTCALL MungeTrackModel(br_model* pModel);

int C2_HOOK_FASTCALL ModelIsATree(br_model* pModel, char* pName_replacement);

void C2_HOOK_FASTCALL PerformTreeSurgery(tBrender_storage* pStorage, br_model* pModel, const char* pName_replacement);

void C2_HOOK_FASTCALL PossibleTreeSurgery(br_model* pModel);

void C2_HOOK_FASTCALL CheckNonCarModelBounds(void);

void C2_HOOK_FASTCALL FreeExceptions(void);

void C2_HOOK_FASTCALL LoadStaticLightingForRace(const char* race_lighting_path);

void C2_HOOK_FASTCALL LoadTrack(const char* pFile_name, tTrack_spec* pTrack_spec, tRace_info* pRace_info);

void C2_HOOK_FASTCALL ClearOutStorageSpace(tBrender_storage* pStorage_space);

void C2_HOOK_FASTCALL DisposeTexturingMaterials(void);

intptr_t C2_HOOK_CDECL RemoveBounds(br_actor* pActor, void* pArg);

void C2_HOOK_FASTCALL RemoveBoundsStructures(br_actor* pActor);

void C2_HOOK_FASTCALL FreeTrack(tTrack_spec* pTrack_spec);

void C2_HOOK_FASTCALL AutoSaveAdditionalStuff(void);

#endif // REC2_WORLD_H
