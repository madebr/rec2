#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "brender/br_types.h"
#include "rec2_types.h"

#define TWT_MOUNT_SUCCEEDED(TWTVFS) ((TWTVFS) >= 0)

C2_HOOK_VARIABLE_DECLARE_ARRAY(char*, gMisc_strings, 300);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5);
C2_HOOK_VARIABLE_DECLARE(int, gKey_map_index);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gKey_mapping, 77);

C2_HOOK_VARIABLE_DECLARE(int, gDisableTiffConversion);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gCurrent_load_directory, 256);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gCurrent_load_name, 256);

C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gDefaultCar, 32);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gBasic_car_name, 32);
C2_HOOK_VARIABLE_DECLARE(int, gGoreLevel);
C2_HOOK_VARIABLE_DECLARE(int, gAnimalsOn);
C2_HOOK_VARIABLE_DECLARE(int, gFlameThrowerOn);
C2_HOOK_VARIABLE_DECLARE(int, gMinesOn);
C2_HOOK_VARIABLE_DECLARE(int, gDronesOff);

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

void C2_HOOK_FASTCALL SetDefaultSoundFolderName(void);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gInitial_APO_potential, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSlot_info, gMax_APO, 3);

C2_HOOK_VARIABLE_DECLARE(int, gCount_mutant_tail_parts);
C2_HOOK_VARIABLE_DECLARE(float, gMass_mutant_tail_link);
C2_HOOK_VARIABLE_DECLARE(float, gMass_mutant_tail_ball);
C2_HOOK_VARIABLE_DECLARE(float, gMass_mine);
C2_HOOK_VARIABLE_DECLARE_ARRAY(char, gUnderwater_screen_name, 32);
C2_HOOK_VARIABLE_DECLARE(int, gWasted_explosion_chance);
C2_HOOK_VARIABLE_DECLARE(int, gExplosion_sound_id);
C2_HOOK_VARIABLE_DECLARE(tExplosion_animation, gExplosion_pix_animation_groups);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gCredits_checkpoint, 3);
C2_HOOK_VARIABLE_DECLARE(tSlot_info, gTrade_in_value_APO);
C2_HOOK_VARIABLE_DECLARE(tSlot_info, gSubstitution_value_APO);
C2_HOOK_VARIABLE_DECLARE(tSlot_info, gPotential_substitution_value_APO);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gGroove_funk_type_names, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gAxis_names, 3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gReverseness_type_names, 2);
C2_HOOK_VARIABLE_DECLARE_ARRAY(const char*, gFunk_speed_control_names, 2);

void C2_HOOK_FASTCALL SetDefaultPowerupFilename(void);

void C2_HOOK_FASTCALL SetDefaultTextFileName(void);

void C2_HOOK_FASTCALL SetDefaultPixelmapFolderName(void);

void C2_HOOK_FASTCALL SetDefaultPedFolderNames(void);

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

int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);

void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel);

int C2_HOOK_FASTCALL GetAnimalsOn(void);

void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn);

int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);

void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn);

int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

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

void C2_HOOK_FASTCALL GetFiveScalars(FILE* pF, br_scalar* pS1, br_scalar* pS2, br_scalar* pS3, br_scalar* pS4, br_scalar* pS5);

void C2_HOOK_FASTCALL GetNScalars(FILE* pF, int n, br_scalar* pS);

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);

void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);

void C2_HOOK_FASTCALL GetFourFloats(FILE * pF, float* pF1, float* pF2, float* pF3, float* pF4);

void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);

void C2_HOOK_FASTCALL AllowOpenToFail(void);

void C2_HOOK_FASTCALL DisallowOpenToFail(void);

FILE* OldDRfopen(const char* pFilename, const char* pMode);

FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

void C2_HOOK_FASTCALL PFfclose(FILE* pFile);

br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f);

int C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, c2_fpos_t* pos);

int C2_HOOK_FASTCALL PFfeof(FILE* pFile);

int C2_HOOK_FASTCALL DRferror(FILE* pFile);

void C2_HOOK_FASTCALL DRclearerr(FILE* pFile);

int C2_HOOK_FASTCALL DRfgetc(FILE* pFile);

int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile);

int C2_HOOK_FASTCALL DRungetc(int ch, FILE* file);

char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile);

int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence);

int C2_HOOK_FASTCALL PFftell(FILE* pF);

int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, c2_fpos_t* pos);

void C2_HOOK_FASTCALL PFrewind(FILE* pF);

void C2_HOOK_FASTCALL InitPackFiles(void);

tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path);

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt);

FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode);

void C2_HOOK_FASTCALL LoadInFiles(const char* pThe_path, const char* pArchive_name, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path);

void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt);

void C2_HOOK_FASTCALL PackFileRevertTiffLoading(void);

void C2_HOOK_FASTCALL PackFileRerevertTiffLoading(void);

void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data);

void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2);

br_pixelmap* C2_HOOK_FASTCALL LoadPixelmap(const char* pPath_name);

br_pixelmap* C2_HOOK_FASTCALL RealLoadPixelmap(const char* pPath_name);

void C2_HOOK_FASTCALL LoadKeyMapping(void);

void C2_HOOK_FASTCALL SaveKeyMapping(void);

void C2_HOOK_FASTCALL ChangeKeyMapIndex(int pKey_map_index);

void C2_HOOK_FASTCALL LoadHeadupImages(void);

void C2_HOOK_FASTCALL LoadMiscStrings(void);

void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions);

int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex);

br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName);

FILE* C2_HOOK_FASTCALL OpenRaceFile(void);

void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);

void C2_HOOK_FASTCALL LoadHeadups(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL GetHithers(void);

void C2_HOOK_FASTCALL LoadBunchOParameters(tSlot_info* pSlot_info);

void C2_HOOK_FASTCALL LoadBunchOFloatParameters(tFloat_bunch_info *pBunch);

void C2_HOOK_FASTCALL LoadGeneralParameters(void);

void C2_HOOK_FASTCALL FinishLoadGeneralParameters(void);

int C2_HOOK_FASTCALL SaveOptions(void);

int C2_HOOK_FASTCALL RestoreOptions(void);

void C2_HOOK_FASTCALL LoadInRegisteeDir(const char *pRoot, const char *pSubDir, int pInitialize_palettes);

void C2_HOOK_FASTCALL LoadInRegistees(void);

void C2_HOOK_FASTCALL InitTreeSurgery(void);

int C2_HOOK_FASTCALL TestForOriginalCarmaCDinDrive(void);

int C2_HOOK_FASTCALL OriginalCarmaCDinDrive(void);

int C2_HOOK_FASTCALL CarmaCDinDriveOrFullGameInstalled(void);

br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name);

void C2_HOOK_FASTCALL DRLoadPalette(const char* p_path);

void C2_HOOK_FASTCALL DRLoadShadeTable(const char* p_path);

void C2_HOOK_FASTCALL DRLoadMaterials(const char* p_path);

void C2_HOOK_FASTCALL DRLoadModels(const char* p_path);

br_model* C2_HOOK_FASTCALL LoadModel(const char* pName);

br_actor* C2_HOOK_FASTCALL LoadActor(const char* pName);

void C2_HOOK_FASTCALL DRLoadActors(const char* p_path);

void C2_HOOK_FASTCALL DRLoadLights(const char* p_path);

void C2_HOOK_FASTCALL InitializePalettes(void);

void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount);

br_material* C2_HOOK_FASTCALL LoadMaterial(const char* pThe_path);

void C2_HOOK_FASTCALL FillInRaceInfo(tRace_info* pThe_race);

void C2_HOOK_FASTCALL DisposePhysicsObject(tCollision_info* pCollision_info);

void C2_HOOK_FASTCALL DisposeCar(tCar_spec* pCar_spec, int pOwner);

void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

void C2_HOOK_FASTCALL LoadOpponents(void);

void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void);

void C2_HOOK_FASTCALL LoadRaceInfo(int pRace_index, tRace_info* pRace_info);

void C2_HOOK_FASTCALL InitFunkGrooveFlags(void);

void C2_HOOK_FASTCALL AboutToLoadFirstCar(void);

void C2_HOOK_FASTCALL LoadCopCars(void);

void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);

void C2_HOOK_FASTCALL LoadGear(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL AdjustCarCoordinates(tCar_spec* pCar);

void C2_HOOK_FASTCALL LoadTacho(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadSpeedo(FILE* pF, int pIndex, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL GetDamageProgram(FILE* pF, tCar_spec* pCar_spec, tImpact_location pImpact_location);

void C2_HOOK_FASTCALL UpdateMaterialsForCar(tBrender_storage* pStorage, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCarMaterials(tBrender_storage* pStorage, const char* pPath, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCarShrapnelMaterials(FILE* pF, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL LoadCar(const char* pCar_name, tDriver pDriver, tCar_spec* pCar_spec, int pOwner, const char* pDriver_name, tBrender_storage* pStorage_space);

void C2_HOOK_FASTCALL LoadOpponentsCars(tRace_info* pRace_info);

void C2_HOOK_FASTCALL SetModelFlags(br_model* pModel, int pOwner);

void C2_HOOK_FASTCALL AddRefOffset(int* pRef_holder);

void C2_HOOK_FASTCALL ReadMechanics(FILE* pF, tCar_spec* c, int pSpec_version);

br_material* C2_HOOK_FASTCALL GetSimpleMaterial(char* pName, tRendererShadingType pShading_type);

void C2_HOOK_FASTCALL GetAVector(FILE* pF, br_vector3* pV);

int C2_HOOK_CDECL AttachGroovidelic(br_actor* pActor, void* pData);

int C2_HOOK_CDECL ActorModelAttachCrushData(br_actor* pActor, void* pData);

void C2_HOOK_FASTCALL AttachCrushDataToActorModels(br_actor* pActor, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL SmoothificatePowerups(br_actor* pActor);

void C2_HOOK_FASTCALL SmoothificatePowerupMaterial(br_material* pMaterial);

void C2_HOOK_FASTCALL SmoothificateWorldMaterial(br_material* pMaterial);

void C2_HOOK_FASTCALL MungeLightingEffects(void);

void C2_HOOK_FASTCALL SmoothlySetWorldMaterialFlags(tBrender_storage* pStorage);

void C2_HOOK_FASTCALL LoadTrackMaterials(tBrender_storage* pStorage, const char* pPath);

int C2_HOOK_FASTCALL AddSmoothModels(tBrender_storage* pStorage, const char* pPath);

void C2_HOOK_FASTCALL SmoothlyLoadIfItsAModel(const char* pPath);

void C2_HOOK_FASTCALL LoadTrackModels(tBrender_storage *pStorage, const char *pPath);

void C2_HOOK_FASTCALL ReadNonCarMechanicsData(FILE* pF, tNon_car_spec* pNon_car_spec);

int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL LinkDroneActorsToModelsAndSetRenderStyle(tDrone_spec* pDrone, int pModel_start, int pModel_end);

void C2_HOOK_FASTCALL LoadDroneActorsModels(tDrone_spec* pDrone);

intptr_t C2_HOOK_CDECL MrFindy(br_actor* pActor, void* data);

br_actor* C2_HOOK_FASTCALL FindDroneChildActor(tDrone_spec* pDrone, const char* pName);

int C2_HOOK_FASTCALL ReadPastThisLine(FILE* pF, const char* pLine);

int C2_HOOK_FASTCALL MatchFGType(const char* pS);

tFunk_groove_axis GetAxisFromString(const char* pS);

tFunk_groove_reverseness GetReversenessFromString(const char* pS);

tFunk_groove_speed_control GetSpeedControlFromString(const char* pS);

void C2_HOOK_FASTCALL LoadFunksAndGrooves(tDrone_spec* pDrone, FILE* pF);

int C2_HOOK_FASTCALL ReadPastBoundsShapesHeader(FILE* pF);

void C2_HOOK_FASTCALL LoadDrone(int pIndex);

void C2_HOOK_FASTCALL ResetDroneCrushyModel(const br_model* pSrc, br_model* pDest);

void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void);

void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);

void C2_HOOK_FASTCALL LoadInterfaceStuff(int pWithin_race);

void C2_HOOK_FASTCALL InitInterfaceLoadState(void);

void C2_HOOK_FASTCALL InitialiseAIWorldForRace(tRace_info* pRace_info);

void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

void C2_HOOK_FASTCALL DisposeRaceInfo(tRace_info* pRace_info);

void C2_HOOK_FASTCALL DisposeOpponentsCars(tRace_info* pRace_info);

#endif //REC2_LOADING_H
