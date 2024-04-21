#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "brender/br_types.h"
#include "rec2_types.h"

#define TWT_MOUNT_SUCCEEDED(TWTVFS) ((TWTVFS) >= 0)

C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gMisc_strings, 300);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gKey_mapping, 77);

C2_HOOK_VARIABLE_DECLARE(int, gDisableTiffConversion);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gCurrent_load_directory, 256);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gCurrent_load_name, 256);

C2_HOOK_VARIABLE_DECLARE(tSpecial_volume, gDefault_water_spec_vol);

C2_HOOK_VARIABLE_DECLARE(char*, gPedsFolder);
C2_HOOK_VARIABLE_DECLARE(char*, gPedSoundPath);
C2_HOOK_VARIABLE_DECLARE(char*, gPowerup_txt_path);
C2_HOOK_VARIABLE_DECLARE(char*, gPedTextTxtPath);
C2_HOOK_VARIABLE_DECLARE(char*, gPedTexturePath);

C2_HOOK_VARIABLE_DECLARE(int, gCurrent_race_file_index);
C2_HOOK_VARIABLE_DECLARE(int, gCountRaceGroups);
C2_HOOK_VARIABLE_DECLARE(tRace_group_spec*, gRaceGroups);
C2_HOOK_VARIABLE_DECLARE(tRace_group_spec*, gRaceGroups2);

C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO_potential, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gCredits_per_rank, 3);
C2_HOOK_VARIABLE_DECLARE(int, gInitial_rank);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gStarting_money, 3);
C2_HOOK_VARIABLE_DECLARE(float, gCamera_angle);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gWheel_actor_names, 6);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gPivot_actor_names, 6);
C2_HOOK_VARIABLE_DECLARE(float, gMass_mine);
C2_HOOK_VARIABLE_DECLARE(tSpecial_volume, gDefault_water_spec_vol);/* FIXME: rename to gDefault_default_water_spec_vol*/
C2_HOOK_VARIABLE_DECLARE(tSpecial_volume*, gDefault_water_spec_vol_real);
C2_HOOK_VARIABLE_DECLARE(int, gFirst_drone_processing);
C2_HOOK_VARIABLE_DECLARE(tU32, gTime_stamp_for_this_munging);
C2_HOOK_VARIABLE_DECLARE(tU32, gFrame_period_for_this_munging);
C2_HOOK_VARIABLE_DECLARE(float, gDrone_delta_time);
C2_HOOK_VARIABLE_DECLARE(float, gTrack_drone_min_y);
C2_HOOK_VARIABLE_DECLARE(tFloat_bunch_info, gRecovery_cost);

C2_HOOK_VARIABLE_DECLARE(int, gKnobbledFramePeriod);
C2_HOOK_VARIABLE_DECLARE(int, gMinTimeOpponentRepair);
C2_HOOK_VARIABLE_DECLARE(int, gMaxTimeOpponentRepair);

void C2_HOOK_FASTCALL ConfigureDefaultPedSoundPath(void);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO_potential, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gMax_APO, 3);

void C2_HOOK_FASTCALL ConfigureDefaultPedPowerupTxtPath(void);

void C2_HOOK_FASTCALL ConfigureDefaultPedTextTxtPath(void);

void C2_HOOK_FASTCALL ConfigureDefaultPedTexturePath(void);

void C2_HOOK_FASTCALL ConfigurePedDefaultPaths(void);

void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath(void);

void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void);

void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void);

void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath(void);

void C2_HOOK_FASTCALL ConfigurePedZombiePaths(void);

void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath(void);

void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void);

void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void);

void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath(void);

void C2_HOOK_FASTCALL ConfigurePedBloodPaths(void);

void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath(void);

void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void);

void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void);

void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath(void);

void C2_HOOK_FASTCALL ConfigurePedAlienPaths(void);

int C2_HOOK_FASTCALL GetGoreLevel(void);

void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

int C2_HOOK_FASTCALL GetAnimalsOn(void);

void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

int C2_HOOK_FASTCALL GetFlameThrowerOn(void);

void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

int C2_HOOK_FASTCALL GetMinesOn(void);

void C2_HOOK_FASTCALL SetMinesOn(int pNewMinesOn);

int C2_HOOK_FASTCALL GetDronesOn(void);

void C2_HOOK_FASTCALL SetDronesOn(int pNewDronesOn);

void C2_HOOK_FASTCALL StripCRNL(char* line);

tU32 C2_HOOK_FASTCALL ReadU32(FILE* pF);

tU16 C2_HOOK_FASTCALL ReadU16(FILE* pF);

tU8 C2_HOOK_FASTCALL ReadU8(FILE* pF);

float C2_HOOK_FASTCALL ReadF32(FILE* pF);

tS32 C2_HOOK_FASTCALL ReadS32(FILE* pF);

tS16 C2_HOOK_FASTCALL ReadS16(FILE* pF);

tS8 C2_HOOK_FASTCALL ReadS8(FILE* pF);

void C2_HOOK_FASTCALL WriteU32L(FILE* pF, tU32 pNumber);

void C2_HOOK_FASTCALL WriteU16L(FILE* pF, tU16 pNumber);

void C2_HOOK_FASTCALL WriteU8L(FILE* pF, tU8 pNumber);

void C2_HOOK_FASTCALL WriteS32L(FILE* pF, tS32 pNumber);

void C2_HOOK_FASTCALL WriteS16L(FILE* pF, tS16 pNumber);

void C2_HOOK_FASTCALL WriteS8L(FILE* pF, tS8 pNumber);

void C2_HOOK_FASTCALL SkipBytes(FILE* pF, int pBytes_to_skip);

tU32 C2_HOOK_FASTCALL MemReadU32(char** pPtr);

tU16 C2_HOOK_FASTCALL MemReadU16(char** pPtr);

tU8 C2_HOOK_FASTCALL MemReadU8(char** pPtr);

tS32 C2_HOOK_FASTCALL MemReadS32(char** pPtr);

tS16 C2_HOOK_FASTCALL MemReadS16(char** pPtr);

tS8 C2_HOOK_FASTCALL MemReadS8(char** pPtr);

void C2_HOOK_FASTCALL MemSkipBytes(char** pPtr, int pBytes_to_skip);

int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount);

int C2_HOOK_FASTCALL GetAnInt(FILE* pF);

void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2);

void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3);

void C2_HOOK_FASTCALL GetFourInts(FILE* pF, int* pF1, int* pF2, int* pF3, int* pF4);

float C2_HOOK_FASTCALL GetAFloat(FILE* pF);

float C2_HOOK_FASTCALL GetAScalar(FILE* pF);

void C2_HOOK_FASTCALL GetPairOfScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2);

void C2_HOOK_FASTCALL GetThreeScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3);

void C2_HOOK_FASTCALL GetFourScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4);

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);

void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);

void C2_HOOK_FASTCALL GetFourFloats(FILE * pF, float* pF1, float* pF2, float* pF3, float* pF4);

void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);

void C2_HOOK_FASTCALL AllowOpenToFail(void);

void C2_HOOK_FASTCALL DisallowOpenToFail(void);

FILE* OldDRfopen(const char* pFilename, const char* pMode);

FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

void C2_HOOK_FASTCALL DRfclose(FILE* pFile);

br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f);

int C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, c2_fpos_t* pos);

int C2_HOOK_FASTCALL DRfeof(FILE* pFile);

int C2_HOOK_FASTCALL DRferror(FILE* pFile);

void C2_HOOK_FASTCALL DRclearerr(FILE* pFile);

int C2_HOOK_FASTCALL DRfgetc(FILE* pFile);

int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile);

int C2_HOOK_FASTCALL DRungetc(int ch, FILE* file);

char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, FILE* pFile);

int C2_HOOK_FASTCALL DRfseek(FILE* pF, int offset, int whence);

int C2_HOOK_FASTCALL DRftell(FILE* pF);

int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, c2_fpos_t* pos);

void C2_HOOK_FASTCALL DRrewind(FILE* pF);

void C2_HOOK_FASTCALL TWT_Init(void);

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path);

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt);

FILE* C2_HOOK_FASTCALL TWT_fopen(const char* pPath, const char* mode);

void C2_HOOK_FASTCALL DRForEveryArchivedFile(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL DRForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path);

void C2_HOOK_FASTCALL TWT_UnmountEx(tTWTVFS twt);

void C2_HOOK_FASTCALL ApplyPreviousTiffConversion(void);

void C2_HOOK_FASTCALL ApplyTopTiffConversion(void);

void C2_HOOK_FASTCALL DRForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL DREnumPath(const char* path, tEnumPathCallback pCallback, void* data);

br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name);

br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap2(const char* pPath_name);

void C2_HOOK_FASTCALL LoadKeyMapping(void);

void C2_HOOK_FASTCALL LoadHeadupImages(void);

void C2_HOOK_FASTCALL LoadMiscStrings(void);

void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions);

int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex);

br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName);

FILE* C2_HOOK_FASTCALL OpenRaceFile(void);

void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);

void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info);

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch);

void C2_HOOK_FASTCALL LoadGeneralParameters(void);

void C2_HOOK_FASTCALL FinishLoadGeneralParameters(void);

int C2_HOOK_FASTCALL SaveOptions(void);

int C2_HOOK_FASTCALL RestoreOptions(void);

void C2_HOOK_FASTCALL LoadInRegistees(void);

void C2_HOOK_FASTCALL LoadTreeSurgery(void);

int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name);

br_uint_32 C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* pFile_name, br_pixelmap** pPixelmaps, br_uint_16 pNum);

void C2_HOOK_FASTCALL DRLoadPalette(const char* p_path);

void C2_HOOK_FASTCALL DRLoadShadeTable(const char* p_path);

void C2_HOOK_FASTCALL DRLoadMaterials(const char* p_path);

void C2_HOOK_FASTCALL DRLoadModels(const char* p_path);

br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);

void C2_HOOK_FASTCALL DRLoadActors(const char* p_path);

void C2_HOOK_FASTCALL DRLoadLights(const char* p_path);

void C2_HOOK_FASTCALL InitializePalettes(void);

void C2_HOOK_FASTCALL DisableVertexColours(br_model** pModels, int pCount);

br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path);

void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race);

void C2_HOOK_FASTCALL DisposeNonCarCollisionInfo(tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL DisposeCar(tCar_spec* pCar_spec, int pOwner);

void C2_HOOK_FASTCALL InitAIWorld(void);

void C2_HOOK_FASTCALL LoadOpponents(void);

void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info);

void C2_HOOK_FASTCALL InitFunkGrooveFlags(void);

void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);

void C2_HOOK_FASTCALL LoadCopCars(void);

void C2_HOOK_FASTCALL LoadAIWorld(FILE* pF);

void C2_HOOK_FASTCALL LoadGear(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL AdjustCarCoordinates(tCar_spec* pCar);

void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location);

void C2_HOOK_FASTCALL UpdateMaterialsForCar(tBrender_storage* pStorage, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCarShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);

void C2_HOOK_FASTCALL LoadPlayerCars(tRace_info* pRace_info);

void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner);

void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder);

void C2_HOOK_FASTCALL ReadMechanics(FILE* pF, tCar_spec* c, int pSpec_version);

br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type);

void C2_HOOK_FASTCALL GetAVector(FILE* pF, br_vector3* pV);

int C2_HOOK_CDECL AttachGroovidelic(br_actor* pActor, void* pData);

int C2_HOOK_CDECL ActorModelAttachCrushData(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL SetMaterialTrackLighting(br_material* pMaterial);

void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath);

int C2_HOOK_FASTCALL LoadAllModelsInPath(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL LoadDATModelsCallback(const char* pPath);

void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage *pStorage, const char *pPath);

void C2_HOOK_FASTCALL LoadNonCar(FILE* pF, tNon_car_spec* pNon_car_spec);

void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

void C2_HOOK_FASTCALL LoadCars(tRace_info* pRace_info);

void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race);

void C2_HOOK_FASTCALL InitOpponentsAndDrones(tRace_info* pRace_info);

void C2_HOOK_FASTCALL DisposeOpponentsAndDrones(void);

void C2_HOOK_FASTCALL DisposeRaceInfo(tRace_info* pRace_info);

void C2_HOOK_FASTCALL DisposeAllCars(tRace_info* pRace_info);

#endif //REC2_LOADING_H
