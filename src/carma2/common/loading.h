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

int C2_HOOK_FASTCALL GetALineAndInterpretCommand(tTWTFILE* pF, const char** pString_list, int pCount);

int C2_HOOK_FASTCALL GetAnInt(tTWTFILE* pF);

void C2_HOOK_FASTCALL GetPairOfInts(tTWTFILE* pF, int* pF1, int* pF2);

void C2_HOOK_FASTCALL GetThreeInts(tTWTFILE* pF, int* pF1, int* pF2, int* pF3);

float C2_HOOK_FASTCALL GetAFloat(tTWTFILE* pF);

float C2_HOOK_FASTCALL GetAScalar(tTWTFILE* pF);

void C2_HOOK_FASTCALL GetThreeFloats(tTWTFILE * pF, float* pF1, float* pF2, float* pF3);

void C2_HOOK_FASTCALL GetAString(tTWTFILE* pF, char* pString);

tTWTFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode);

tTWTFILE* C2_HOOK_FASTCALL TWTfopen(const char* pFilename, const char* pMode);

void C2_HOOK_FASTCALL DRfclose(tTWTFILE* pFile);

br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f);

br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f);

int C2_HOOK_FASTCALL DRfeof(tTWTFILE* pFile);

int C2_HOOK_FASTCALL DRfgetc(tTWTFILE* pFile);

int C2_HOOK_FASTCALL DRungetc(int ch, tTWTFILE* file);

char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, tTWTFILE* pFile);

int C2_HOOK_FASTCALL DRfseek(tTWTFILE* pF, int offset, int whence);

int C2_HOOK_FASTCALL DRftell(tTWTFILE* pF);

int C2_HOOK_FASTCALL DRrewind(tTWTFILE* pF);

/// FIXME: rename to DRxxx
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file);

void C2_HOOK_FASTCALL TWT_Init(void);

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path);

void C2_HOOK_FASTCALL TWT_EnumPath(const char* path, tEnumPathCallback pCallback, void* data);

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path);

br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name);

void C2_HOOK_FASTCALL LoadKeyMapping(void);

void C2_HOOK_FASTCALL LoadHeadupImages(void);

void C2_HOOK_FASTCALL LoadMiscStrings(void);

void C2_HOOK_FASTCALL ReadNetworkSettings(tTWTFILE* pF, tNet_game_options* pOptions);

int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex);

#endif //REC2_LOADING_H

