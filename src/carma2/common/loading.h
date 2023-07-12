#ifndef REC2_LOADING_H
#define REC2_LOADING_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "brender/br_types.h"
#include "rec2_types.h"

#define TWT_MOUNT_SUCCEEDED(TWTVFS) ((TWTVFS) >= 0)

C2_HOOK_VARIABLE_DECLARE_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5);

C2_HOOK_VARIABLE_DECLARE(int, gRusselsFannies);

C2_HOOK_VARIABLE_DECLARE(int, gDisableTiffConversion);

C2_HOOK_VARIABLE_DECLARE(char*, gPedsFolder);
C2_HOOK_VARIABLE_DECLARE(char*, gPedSoundPath);
C2_HOOK_VARIABLE_DECLARE(char*, gPedPowerupTxtPath);
C2_HOOK_VARIABLE_DECLARE(char*, gPedTextTxtPath);
C2_HOOK_VARIABLE_DECLARE(char*, gPedTexturePath);

C2_HOOK_VARIABLE_DECLARE(int, gApplicationDataTwtMounted);
C2_HOOK_VARIABLE_DECLARE(int, gCurrent_race_file_index);
C2_HOOK_VARIABLE_DECLARE(int, gDevInitialRace);
C2_HOOK_VARIABLE_DECLARE(int, gCountRaceGroups);
C2_HOOK_VARIABLE_DECLARE(tRace_group_spec*, gRaceGroups);
C2_HOOK_VARIABLE_DECLARE(tRace_group_spec*, gRaceGroups2);

void C2_HOOK_FASTCALL ConfigureDefaultPedSoundPath(void);

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

float C2_HOOK_FASTCALL GetAFloat(FILE* pF);

float C2_HOOK_FASTCALL GetAScalar(FILE* pF);

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2);

void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3);

void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString);

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

/// FIXME: rename to DRxxx
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file);

void C2_HOOK_FASTCALL TWT_Init(void);

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path);

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt);

FILE* C2_HOOK_FASTCALL TWT_fopen(const char* pPath, const char* mode);

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path);

void C2_HOOK_FASTCALL TWT_UnmountEx(tTWTVFS twt);

void C2_HOOK_FASTCALL ApplyPreviousTiffConversion(void);

void C2_HOOK_FASTCALL ApplyTopTiffConversion(void);

void C2_HOOK_FASTCALL DRForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine);

void C2_HOOK_FASTCALL DREnumPath(const char* path, tEnumPathCallback pCallback, void* data);

br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name);

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

void C2_HOOK_FASTCALL RestoreOptions(void);

void C2_HOOK_FASTCALL LoadInRegistees(void);

void C2_HOOK_FASTCALL LoadTreeSurgery(void);

#endif //REC2_LOADING_H

