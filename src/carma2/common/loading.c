#include "loading.h"

#include "crush.h"
#include "errors.h"
#include "globvars.h"
#include "globvrpb.h"
#include "graphics.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5, 0x00691b40);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsFile, gTwatVfsFiles, 50, 0x00692080);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversionStackPos, 0x006923a0);

C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversion, 0x0068c724);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDisableTiffConversionStack, 2, 0x00692068);

C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_hither, 0x0074d3e8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_cockpit_hither, 0x007634ac);
C2_HOOK_VARIABLE_IMPLEMENT(FILE*, gTempFile, 0x0068c6ec);
C2_HOOK_VARIABLE_IMPLEMENT(float, gCamera_angle, 0x0074d354);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHeadupBackgroundBrightness, 0x0079ec14);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInitial_rank, 0x0074b76c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_per_rank, 3, 0x0074b760);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_per_ped_kill, 3, 0x00763530);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gSeconds_per_unit_car_damage, 3, 0x00761ed0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(float, gCredits_per_unit_car_damage, 3, 0x0075b930);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_wasting_car, 3, 0x00763520);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_wasting_car, 3, 0x0074d3b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gTime_rolling_car, 3, 0x0075b9b0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_rolling_car, 3, 0x00761a50);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_checkpoint, 3, 0x00762140);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gFine_jump_start, 3, 0x0074d3d0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCredits_per_second_time_bonus, 3, 0x00761ef0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gCunning_stunt_bonus, 3, 0x00763500);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gDefaultCar, 32, 0x00764ea0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gDefaultCockpit, 32, 0x00764ec0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gGoreLevel, 0x0065d7d4, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gAnimalsOn, 0x0065d7c8, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gFlameThrowerOn, 0x0065d7cc, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gMinesOn, 0x0065d7d0, 1);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDronesOff, 0x00684518);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKnobbledFramePeriod, 0x007634f0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gUnknownOpponentFactor, 0x0065a3cc);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMinTimeOpponentRepair, 0x0074a684);
C2_HOOK_VARIABLE_IMPLEMENT(float, gMaxTimeOpponentRepair, 0x0074a688);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gUnderwaterScreenName, 32, 0x0068c6f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRusselsFannies, 0x006aa5c4);

C2_HOOK_VARIABLE_IMPLEMENT(int, gKey_map_index, 0x0068b88c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gKey_mapping, 77, 0x0074b5e0);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tHeadup_info, gHeadup_image_info, 45, 0x00657320, TODO);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_pixelmap*, gHeadup_images, 45, 0x0079eb60);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char, gDecode_string, 14, 0x00655e38, {    \
    0x9B, 0x52, 0x93, 0x9f, 0x52, 0x98, 0x9b,                                    \
    0x96, 0x96, 0x9e, 0x9B, 0xa0, 0x99, 0x0 });
C2_HOOK_VARIABLE_IMPLEMENT(int, gDecode_thing, 0x00655e30);
C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume, gUnderwaterSpecialVolumeSettings, 0x00761b80);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gMisc_strings, 300, 0x006b5f40);

C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedsFolder, 0x006a0414);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedSoundPath, 0x00684550);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedPowerupTxtPath, 0x006a0ad4);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTextTxtPath, 0x0068c718);
C2_HOOK_VARIABLE_IMPLEMENT(char*, gPedTexturePath, 0x0065852c);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(char*, gRaces_file_names, 9, 0x00657530, {
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "PEDRACES.TXT",
    "RACES.TXT",
    "RACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT",
    "NETRACES.TXT"
});

C2_HOOK_VARIABLE_IMPLEMENT(int, gApplicationDataTwtMounted, 0x0068b868);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_race_file_index, 0x0068c6f4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDevInitialRace, 0x0076211c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCountRaceGroups, 0x007634ec);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_group_spec*, gRaceGroups, 0x0068b8a0);
C2_HOOK_VARIABLE_IMPLEMENT(tRace_group_spec*, gRaceGroups2, 0x0074d5e4);

void C2_HOOK_FASTCALL ConfigureDefaultPedSoundPath(void) {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457570, ConfigureDefaultPedSoundPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedPowerupTxtPath(void) {
    C2V(gPedPowerupTxtPath) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c00, ConfigureDefaultPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea00, ConfigureDefaultPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureDefaultPedTexturePath(void) {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f7f0, ConfigureDefaultPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedDefaultPaths(void) {
    C2V(gPedsFolder) = "PEDS";
    ConfigureDefaultPedSoundPath();
    ConfigureDefaultPedPowerupTxtPath();
    ConfigureDefaultPedTextTxtPath();
    ConfigureDefaultPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f10, ConfigurePedDefaultPaths)

void C2_HOOK_FASTCALL ConfigureZombiePedSoundPath(void) {
    C2V(gPedSoundPath) = "ZOMSOUND";
}
C2_HOOK_FUNCTION(0x00457590, ConfigureZombiePedSoundPath)

void C2_HOOK_FASTCALL ConfigureZombiePedPowerupTxtPath(void) {
    C2V(gPedPowerupTxtPath) = "ZOMPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c20, ConfigureZombiePedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureZombiePedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "ZOMTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea20, ConfigureZombiePedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureZombiePedTexturePath(void) {
    C2V(gPedTexturePath) = "ZOMPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f810, ConfigureZombiePedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedZombiePaths(void) {
    C2V(gPedsFolder) = "ZOMBIES";
    ConfigureZombiePedSoundPath();
    ConfigureZombiePedPowerupTxtPath();
    ConfigureZombiePedTextTxtPath();
    ConfigureZombiePedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f50, ConfigurePedZombiePaths)

void C2_HOOK_FASTCALL ConfigureBloodPedSoundPath(void) {
    C2V(gPedSoundPath) = NULL;
}
C2_HOOK_FUNCTION(0x00457580, ConfigureBloodPedSoundPath)

void C2_HOOK_FASTCALL ConfigureBloodPedPowerupTxtPath(void) {
    C2V(gPedPowerupTxtPath) = "POWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c10, ConfigureBloodPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureBloodPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "TEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea10, ConfigureBloodPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureBloodPedTexturePath(void) {
    C2V(gPedTexturePath) = "PIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f800, ConfigureBloodPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedBloodPaths(void) {
    C2V(gPedsFolder) = "PEDS";
    ConfigureBloodPedSoundPath();
    ConfigureBloodPedPowerupTxtPath();
    ConfigureBloodPedTextTxtPath();
    ConfigureBloodPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f30, ConfigurePedBloodPaths)

void C2_HOOK_FASTCALL ConfigureAlienPedSoundPath(void) {
    C2V(gPedSoundPath) = "ALSOUND";
}
C2_HOOK_FUNCTION(0x004575a0, ConfigureAlienPedSoundPath)

void C2_HOOK_FASTCALL ConfigureAlienPedPowerupTxtPath(void) {
    C2V(gPedPowerupTxtPath) = "ALPOWERUP.TXT";
}
C2_HOOK_FUNCTION(0x004e0c30, ConfigureAlienPedPowerupTxtPath)

void C2_HOOK_FASTCALL ConfigureAlienPedTextTxtPath(void) {
    C2V(gPedTextTxtPath) = "ALTEXT.TXT";
}
C2_HOOK_FUNCTION(0x0048ea30, ConfigureAlienPedTextTxtPath)

void C2_HOOK_FASTCALL ConfigureAlienPedTexturePath(void) {
    C2V(gPedTexturePath) = "ALPIXELMAP";
}
C2_HOOK_FUNCTION(0x0048f820, ConfigureAlienPedTexturePath)

void C2_HOOK_FASTCALL ConfigurePedAlienPaths(void) {
    C2V(gPedsFolder) = "ALIENS";
    ConfigureAlienPedSoundPath();
    ConfigureAlienPedPowerupTxtPath();
    ConfigureAlienPedTextTxtPath();
    ConfigureAlienPedTexturePath();
}
C2_HOOK_FUNCTION(0x004d6f70, ConfigurePedAlienPaths)

int C2_HOOK_FASTCALL GetGoreLevel(void) {

    return 2 - C2V(gGoreLevel);
}
C2_HOOK_FUNCTION(0x004d6fc0, GetGoreLevel)

void C2_HOOK_FASTCALL SetGoreLevel(int pNewLevel) {

    C2V(gGoreLevel) = 2 - pNewLevel;
}
C2_HOOK_FUNCTION(0x004d7030, SetGoreLevel)

int C2_HOOK_FASTCALL GetAnimalsOn(void) {

    return C2V(gAnimalsOn);
}
C2_HOOK_FUNCTION(0x004d6fd0, GetAnimalsOn)

void C2_HOOK_FASTCALL SetAnimalsOn(int pNewAnimalsOn) {

    C2V(gAnimalsOn) = pNewAnimalsOn;
}
C2_HOOK_FUNCTION(0x004d7000, SetAnimalsOn)

int C2_HOOK_FASTCALL GetFlameThrowerOn(void) {

    return C2V(gFlameThrowerOn);
}
C2_HOOK_FUNCTION(0x004d6fe0, GetFlameThrowerOn)

void C2_HOOK_FASTCALL SetFlameThrowerOn(int pNewFlameThrowerOn) {

    C2V(gFlameThrowerOn) = pNewFlameThrowerOn;
}
C2_HOOK_FUNCTION(0x004d7010, SetFlameThrowerOn)

int C2_HOOK_FASTCALL GetMinesOn(void) {

    return C2V(gMinesOn);
}
C2_HOOK_FUNCTION(0x004d6ff0, GetMinesOn)

void C2_HOOK_FASTCALL SetMinesOn(int pNewMinesOn) {

    C2V(gMinesOn) = pNewMinesOn;
}
C2_HOOK_FUNCTION(0x004d7020, SetMinesOn)

int C2_HOOK_FASTCALL GetDronesOn(void) {

    return !C2V(gDronesOff);
}
C2_HOOK_FUNCTION(0x0044ed00, GetDronesOn)

void C2_HOOK_FASTCALL SetDronesOn(int pNewDronesOn) {

    C2V(gDronesOff) = !pNewDronesOn;
}
C2_HOOK_FUNCTION(0x0044ecf0, SetDronesOn)

void C2_HOOK_FASTCALL StripCRNL(char* line) {
    char* loc;

    loc = c2_strchr(line, '\n');
    if (loc != NULL) {
        *loc = '\0';
    }

    loc = c2_strchr(line, '\r');
    if (loc != NULL) {
        *loc = '\0';
    }
}
C2_HOOK_FUNCTION(0x00490690, StripCRNL)

tU32 C2_HOOK_FASTCALL MemReadU32(char** pPtr) {
    tU32 raw_long;

    c2_memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048fa00, MemReadU32)

tU16 C2_HOOK_FASTCALL MemReadU16(char** pPtr) {
    tU16 raw_short;

    c2_memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048fa10, MemReadU16)

tU8 C2_HOOK_FASTCALL MemReadU8(char** pPtr) {
    tU8 raw_byte;

    c2_memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048fa20, MemReadU8)

tS32 C2_HOOK_FASTCALL MemReadS32(char** pPtr) {
    tS32 raw_long;

    c2_memcpy(&raw_long, *pPtr, sizeof(raw_long));
    *pPtr += sizeof(raw_long);
    return raw_long;
}
C2_HOOK_FUNCTION(0x0048fa30, MemReadS32)

tS16 C2_HOOK_FASTCALL MemReadS16(char** pPtr) {
    tS16 raw_short;

    c2_memcpy(&raw_short, *pPtr, sizeof(raw_short));
    *pPtr += sizeof(raw_short);
    return raw_short;
}
C2_HOOK_FUNCTION(0x0048fa40, MemReadS16)

tS8 C2_HOOK_FASTCALL MemReadS8(char** pPtr) {
    tS8 raw_byte;

    c2_memcpy(&raw_byte, *pPtr, sizeof(raw_byte));
    *pPtr += sizeof(raw_byte);
    return raw_byte;
}
C2_HOOK_FUNCTION(0x0048fa50, MemReadS8)

void C2_HOOK_FASTCALL MemSkipBytes(char** pPtr, int pBytes_to_skip) {

    *pPtr += pBytes_to_skip;
}
C2_HOOK_FUNCTION(0x0048fa60, MemSkipBytes)

int C2_HOOK_FASTCALL GetALineAndInterpretCommand(FILE* pF, const char** pString_list, int pCount) {
    int i;
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);

    str = c2_strtok(s, "\t ,/");
    if (pCount <= 0) {
        return -1;
    }
    for (i = 0; i < pCount; i++) {
        if (DRStricmp(str, pString_list[i]) == 0) {
            return i;
        }
    }
    return -1;
}
C2_HOOK_FUNCTION(0x0048fa70, GetALineAndInterpretCommand)

int C2_HOOK_FASTCALL GetAnInt(FILE* pF) {
    char s[256];
    int value;
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", &value);
    return value;
}
C2_HOOK_FUNCTION(0x0048fb00, GetAnInt)

void C2_HOOK_FASTCALL GetPairOfInts(FILE* pF, int* pF1, int* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF2);
}
C2_HOOK_FUNCTION(0x0048fdc0, GetPairOfInts)

void C2_HOOK_FASTCALL GetThreeInts(FILE* pF, int* pF1, int* pF2, int* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%d", pF3);
}
C2_HOOK_FUNCTION(0x0048fe30, GetThreeInts)

float (C2_HOOK_FASTCALL * GetAFloat_original)(FILE* pF);
float C2_HOOK_FASTCALL GetAFloat(FILE* pF) {
#if defined(C2_HOOKS_ENABLED)
    return GetAFloat_original(pF);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004900e0, GetAFloat, GetAFloat_original)

float C2_HOOK_FASTCALL GetAScalar(FILE* pF) {
    return GetAFloat(pF);
}
C2_HOOK_FUNCTION(0x0048fb80, GetAScalar)

void C2_HOOK_FASTCALL GetPairOfFloats(FILE* pF, float* pF1, float* pF2) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
}
C2_HOOK_FUNCTION(0x0048fc20, GetPairOfFloats)

void C2_HOOK_FASTCALL GetThreeFloats(FILE * pF, float* pF1, float* pF2, float* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    c2_sscanf(str, "%f", pF1);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF2);
    str = c2_strtok(NULL, "\t ,/");
    c2_sscanf(str, "%f", pF3);
}
C2_HOOK_FUNCTION(0x0048fc90, GetThreeFloats)

void (C2_HOOK_FASTCALL * GetAString_original)(FILE* pF, char* pString);
void C2_HOOK_FASTCALL GetAString(FILE* pF, char* pString) {
#if defined(C2_HOOKS_ENABLED)
    GetAString_original(pF, pString);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490630, GetAString, GetAString_original)

FILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
FILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if defined(C2_HOOKS_ENABLED)
    FILE* res = DRfopen_original(pFilename, pMode);
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00491170, DRfopen, DRfopen_original)

void (C2_HOOK_FASTCALL * DRfclose_original)(FILE* pFile);
void C2_HOOK_FASTCALL DRfclose(FILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    DRfclose_original(pFile);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4760, DRfclose, DRfclose_original)

br_size_t (C2_HOOK_FASTCALL * DRfread_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfread_original(buf, size, n, f);
#else
    tTwatVfsFile* twtFile;
    int totalSize;

    if ((int)f < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)f - 1];
        totalSize = size * n;
        if (twtFile->end - twtFile->pos < totalSize) {
            n = ((twtFile->end - twtFile->pos) / size) * size;
            totalSize = size * n;
        }
        c2_memcpy(buf, twtFile->pos, totalSize);
        twtFile->pos += totalSize;
        twtFile->error = 0;
        return n;
    }
    return c2_fread(buf, size, n, f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b49f0, DRfread, DRfread_original)

br_size_t (C2_HOOK_FASTCALL * DRfwrite_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfwrite(void* buf, br_size_t size, unsigned int n, void* f) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfwrite_original(buf, size, n, f);
#else

    if ((int)f < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        FatalError(kFatalError_WriteAttemptToPackedFile_S, "unknown");
    }
    return c2_fwrite(buf, size, n, f);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4a80, DRfwrite, DRfwrite_original)

int (C2_HOOK_FASTCALL * DRfgetpos_original)(FILE*, c2_fpos_t*);
int C2_HOOK_FASTCALL DRfgetpos(FILE* pFile, c2_fpos_t* pos) {
#if 0 // efined(C2_HOOKS_ENABLED)
    return DRfgetpos_original(pFile, pos);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
        *(tU8 **) pos = twtFile->pos;
        twtFile->error = 0;
        return 0;
    }
    return c2_fgetpos(pFile, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4ac0, DRfgetpos, DRfgetpos_original)

int (C2_HOOK_FASTCALL * DRfeof_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfeof(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfeof_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
        return twtFile->pos >= twtFile->end;
    }
    return c2_feof(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c10, DRfeof, DRfeof_original)

int (C2_HOOK_FASTCALL * DRferror_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRferror(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfeof_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
        return twtFile->error;
    }
    return c2_ferror(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c40, DRferror, DRferror_original)

void (C2_HOOK_FASTCALL * DRclearerr_original)(FILE* pFile);
void C2_HOOK_FASTCALL DRclearerr(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    CR_clearerr_original(pFile);
#else
    tTwatVfsFile* twtFile;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int) pFile - 1];
        twtFile->error = 0;
        return;
    }
    c2_clearerr(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c60, DRclearerr, DRclearerr_original)

int (C2_HOOK_FASTCALL * DRfgetc_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfgetc(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgetc_original(pFile);
#else
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
        if (twtFile->pos >= twtFile->end) {
            twtFile->error = -1;
            return EOF;
        }
        twtFile->error = 0;
        result = *twtFile->pos;
        twtFile->pos++;
        return result;
    }
    return c2_fgetc(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4880, DRfgetc, DRfgetc_original)

int (C2_HOOK_FASTCALL * DRfgetc2_original)(FILE* pFile);
int C2_HOOK_FASTCALL DRfgetc2(FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgetc_original(pFile);
#else
    tTwatVfsFile* twtFile;
    int result;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
        if (twtFile->pos >= twtFile->end) {
            twtFile->error = -1;
            return EOF;
        }
        twtFile->error = 0;
        result = *twtFile->pos;
        twtFile->pos++;
        return result;
    }
    return c2_fgetc(pFile);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b48c0, DRfgetc2, DRfgetc2_original)

int (C2_HOOK_FASTCALL * DRungetc_original)(int ch, FILE* file);
int C2_HOOK_FASTCALL DRungetc(int ch, FILE* file) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRungetc_original(ch, file);
#else
    tTwatVfsFile* twtFile;

    if ((int)file < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)file - 1];
        twtFile->pos--;
        *twtFile->pos = ch;
        twtFile->error = 0;
        return ch;
    }
    return c2_ungetc(ch, file);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b49a0, DRungetc, DRungetc_original)

char* (C2_HOOK_FASTCALL * DRfgets_original)(char* buffer, br_size_t size, FILE* pFile);
char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, FILE* pFile) {
#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfgets_original(buffer, size, pFile);
#else
    tTwatVfsFile* twtFile;
    char c;
    char* b;
    size_t i;

    if ((int)pFile < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pFile - 1];
        b = buffer;
        for (i = 0; i < size; i++) {
            c = (char)*twtFile->pos;
            twtFile->pos++;
            if (c == -1) {
                *b = '\0';
                twtFile->error = -1;
                return NULL;
            }
            *b = c;
            b++;
            // FIXME: move this check above?
            if (c == '\n' || twtFile->pos >= twtFile->end) {
                break;
            }
        }
        *b = '\0';
        twtFile->error = 0;
        return buffer;
    }
    return c2_fgets(buffer, size, pFile);

#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4900, DRfgets, DRfgets_original)

int (C2_HOOK_FASTCALL * DRfseek_original)(FILE* pF, int offset, int whence);
int C2_HOOK_FASTCALL DRfseek(FILE* pF, int offset, int whence) {

#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfseek_original(pF, offset, whence);
#else
    tTwatVfsFile* twtFile;
    tU8 *newpos;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
        switch (whence) {
        case SEEK_SET:
            newpos = twtFile->start + offset;
            break;
        case SEEK_CUR:
            newpos = twtFile->pos + offset;
            break;
        case SEEK_END:
            newpos = twtFile->end + offset;
            break;
        default:
            abort(); // FIXME: use better failure function
            break;
        }
        if (twtFile->start <= newpos && newpos <= twtFile->end) {
            twtFile->pos = newpos;
            twtFile->error = 0;
            return 0;
        } else {
            twtFile->error = -1;
        }
    }
    return c2_fseek(pF, offset, whence);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b70, DRfseek, DRfseek_original)

int (C2_HOOK_FASTCALL * DRftell_original)(FILE* pF);
int C2_HOOK_FASTCALL DRftell(FILE* pF) {

#if 0 //defined(C2_HOOKS_ENABLED)
    return DRftell_original(pF);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
        return twtFile->pos - twtFile->start;
    }
    return c2_ftell(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b00, DRftell, DRftell_original)

int (C2_HOOK_FASTCALL * DRfsetpos_original)(FILE* pF, c2_fpos_t* pos);
int C2_HOOK_FASTCALL DRfsetpos(FILE* pF, c2_fpos_t* pos) {

#if 0 // defined(C2_HOOKS_ENABLED)
    return DRfsetpos_original(pF, pos);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
        if (twtFile->start <= *(tU8**)pos && *(tU8**)pos <= twtFile->end) {
            twtFile->pos = *(tU8**)pos;
            twtFile->error = 0;
            return 0;
        }
        twtFile->error = -1;
        return -1;
    }
    return c2_fsetpos(pF, pos);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4b30, DRfsetpos, DRfsetpos_original)

void (C2_HOOK_FASTCALL * DRrewind_original)(FILE* pF);
void C2_HOOK_FASTCALL DRrewind(FILE* pF) {

#if 0 //defined(C2_HOOKS_ENABLED)
    return DRrewind_original(pF);
#else
    tTwatVfsFile* twtFile;

    if ((int)pF < REC2_ASIZE(C2V(gTwatVfsFiles))) {
        twtFile = &C2V(gTwatVfsFiles)[(int)pF - 1];
        twtFile->pos = twtFile->start;
        twtFile->error = 0;
        return;
    }
    c2_rewind(pF);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4be0, DRrewind, DRrewind_original)

tU32 (C2_HOOK_FASTCALL * TWT_ReadBinaryU32_original)(FILE* file);
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file) {
#if defined(C2_HOOKS_ENABLED)
    return TWT_ReadBinaryU32_original(file);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f830, TWT_ReadBinaryU32, TWT_ReadBinaryU32_original)

void C2_HOOK_FASTCALL TWT_Init(void) {
    size_t i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 0; i < REC2_ASIZE(C2V(gTwatVfsFiles)); i++) {
        C2V(gTwatVfsFiles)[i].start = NULL;
    }
    for (i = 0; i < REC2_ASIZE(C2V(gTwatVfsMountPoints)); i++) {
        C2V(gTwatVfsMountPoints)[i].header = NULL;
    }

    C2V(gDisableTiffConversionStackPos) = 0;
}
C2_HOOK_FUNCTION(0x004b4570, TWT_Init)

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    tU32 fileSize;
    tU32 i;

    // file header must be 56 bytes for compatibility with .TWT files
    REC2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    c2_strcpy(twatFilePath, path);
    c2_strcat(twatFilePath, ".TWT");

    f = c2_fopen(twatFilePath, "rb");
    if (f == NULL) {
        return -1;
    }
    for (twt = 0; ; twt++) {
        if (twt >= REC2_ASIZE(C2V(gTwatVfsMountPoints))) {
            c2_fclose(f);
            return -1;
        }
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            break;
        }
    }
    c2_strcpy(C2V(gTwatVfsMountPoints)[twt].path, path);
    fileSize = TWT_ReadBinaryU32(f);
    c2_rewind(f);

    C2V(gTwatVfsMountPoints)[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
    C2V(gTwatVfsMountPoints)[twt].data = (tU8*)C2V(gTwatVfsMountPoints)[twt].header;
    c2_fread(C2V(gTwatVfsMountPoints)[twt].header, fileSize, 1, f);
    // TWT_verify_header(C2V(gTwatVfsMountPoints)[i].header, size); // not implemented
    c2_fclose(f);

    tU8* fileData = (tU8*)&C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[C2V(gTwatVfsMountPoints)[twt].header->nbFiles];
    for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
        C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data = fileData;
        fileData += (C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
    }
    return twt;
}
C2_HOOK_FUNCTION(0x004b45b0, TWT_Mount)

void C2_HOOK_FASTCALL TWT_Unmount(tTWTVFS twt) {
    if (twt >= 0) {
        BrMemFree(C2V(gTwatVfsMountPoints)[twt].header);
        C2V(gTwatVfsMountPoints)[twt].header = NULL;
    }
}
C2_HOOK_FUNCTION(0x004b4730, TWT_Unmount)

FILE* C2_HOOK_FASTCALL TWT_fopen(const char* pPath, const char* mode) {
    int twt;
    unsigned int i;
    int file_index;
    size_t twt_path_len;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsFile) != 16);
    C2_HOOK_BUG_ON(sizeof(tTwatFileHeader) != 56);

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            continue;
        }
        if (c2_strstr(pPath, C2V(gTwatVfsMountPoints)[twt].path) != pPath) {
            continue;
        }
        twt_path_len = c2_strlen(C2V(gTwatVfsMountPoints)[twt].path);
        for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
            if (DRStricmp(C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename, &pPath[twt_path_len + 1]) == 0) {
                for (file_index = 0; file_index < REC2_ASIZE(C2V(gTwatVfsFiles)); file_index++) {
                    if (C2V(gTwatVfsFiles)[file_index].start != NULL) {
                        continue;
                    }
                    C2V(gTwatVfsFiles)[file_index].start = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].pos = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data;
                    C2V(gTwatVfsFiles)[file_index].end = C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data + C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize;
                    C2V(gTwatVfsFiles)[file_index].error = 0;
                    return (FILE*)(file_index + 1);
                }
                return NULL;
            }
        }
    }
    return c2_fopen(pPath, mode);
}
C2_HOOK_FUNCTION(0x004b4780, TWT_fopen)

void C2_HOOK_FASTCALL DRForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    int twt;
    unsigned int i;
    char buffer[256];

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            continue;
        }
        if (DRStricmp(pThe_path, C2V(gTwatVfsMountPoints)[twt].path) != 0) {
            continue;
        }
        for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
            PathCat(buffer, pThe_path, C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename);
            pAction_routine(buffer);
        }
        return;
    }
    PDForEveryFile(pThe_path, pAction_routine);
}
C2_HOOK_FUNCTION(0x004b4c80, DRForEveryFile)

void C2_HOOK_FASTCALL DREnumPath(const char* path, tEnumPathCallback pCallback, void* data) {
    int twt;
    size_t i;
    tPath_name twt_filePath;

    for (twt = 0; twt < REC2_ASIZE(C2V(gTwatVfsMountPoints)); twt++) {
        if (C2V(gTwatVfsMountPoints)[twt].header != NULL && DRStricmp(C2V(gTwatVfsMountPoints)[twt].path, path) == 0) {
            for (i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
                PathCat(twt_filePath, path, C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].filename);
                pCallback(twt_filePath, data);
            }
            return;
        }
    }
    PDEnumPath(path, pCallback, data);
}
C2_HOOK_FUNCTION(0x004b4d30, DREnumPath)

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path) {
    tTWTVFS res;

    res = TWT_Mount(path);
    if (TWT_MOUNT_SUCCEEDED(res)) {
        C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversionStackPos)++;
        C2V(gDisableTiffConversion) = 1;
    }
    return res;
}
C2_HOOK_FUNCTION(0x004b4df0, TWT_MountEx)

void C2_HOOK_FASTCALL TWT_UnmountEx(tTWTVFS twt) {
    if (twt >= 0) {
        C2V(gDisableTiffConversionStackPos)--;
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
        TWT_Unmount(twt);
    }
}
C2_HOOK_FUNCTION(0x004b4e20, TWT_UnmountEx)

void C2_HOOK_FASTCALL ApplyPreviousTiffConversion(void) {
    int count;

    count = C2V(gDisableTiffConversionStackPos);
    if (count != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversionStack)[count] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[count - 1];
    }
}
C2_HOOK_FUNCTION(0x004b4e60, ApplyPreviousTiffConversion)

void C2_HOOK_FASTCALL ApplyTopTiffConversion(void) {

    if (C2V(gDisableTiffConversionStackPos) != 0) { // or > 0 if it is unsigned
        C2V(gDisableTiffConversion) = C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackPos)];
    }
}
C2_HOOK_FUNCTION(0x004b4e90, ApplyTopTiffConversion)

br_pixelmap* (C2_HOOK_FASTCALL * DRLoadPixelmap_original)(const char* pPath_name);
br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name) {
#if defined(C2_HOOKS_ENABLED)
    return DRLoadPixelmap_original(pPath_name);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ec00, DRLoadPixelmap, DRLoadPixelmap_original)

void (C2_HOOK_FASTCALL * LoadGeneralParameters_original)(void);
void C2_HOOK_FASTCALL LoadGeneralParameters(void) {
#if defined(C2_HOOKS_ENABLED)
    LoadGeneralParameters_original();
#else
    tPath_name the_path;
    char s[256];
    char s2[256];
    int position;
    int result;

    PathCat(the_path, C2V(gApplication_path), "ACTORS");
    PathCat(the_path, the_path, "PROG.ACT");

    C2V(gTempFile) = TWT_fopen(the_path, "rb");
    if (C2V(gTempFile) != NULL) {
        DRfgets(s, REC2_ASIZE(s)-1, C2V(gTempFile));
        DRfclose(C2V(gTempFile));

        for (size_t i = 0; i < strlen(C2V(gDecode_string)); i++) {
            C2V(gDecode_string)[i] -= 50;
        }

        // trim trailing CRLF etc
        while (s[0] != '\0' && s[strlen(s) - 1] < 0x20) {
            s[strlen(s) - 1] = 0;
        }

        if (strcmp(s, C2V(gDecode_string)) == 0) {
            C2V(gDecode_thing) = 0;
        }

        for (size_t  i = 0; i < strlen(C2V(gDecode_string)); i++) {
            C2V(gDecode_string)[i] += 50;
        }
    }
    PathCat(the_path, C2V(gApplication_path), "GENERAL.TXT");
    C2V(gTempFile) = DRfopen(the_path, "rt");
    if (C2V(gTempFile) == NULL) {
        FatalError(kFatalError_FailToOpenGeneralSettings);
    }

    C2V(gDisableTiffConversion) = GetAnInt(C2V(gTempFile));
    GetALineAndDontArgue(C2V(gTempFile), s2);
    result = c2_sscanf(&s2[strspn(s2, "\t ,")], "%f%n", &C2V(gCamera_hither), &position);
    if (result == 0) {
        FatalError(kFatalError_MysteriousX_SS, s2, "GENERAL.TXT");
    }
    c2_sscanf(&s2[position + strspn(&s2[position], "\t ,")], "%f", &C2V(gCamera_cockpit_hither));
    C2V(gCamera_hither) *= 2;
    C2V(gCamera_cockpit_hither) *= 2;
    C2V(gCamera_Yon) = GetAFloat(C2V(gTempFile));
    C2V(gCamera_angle) = GetAFloat(C2V(gTempFile));
    C2V(gHeadupBackgroundBrightness) = GetAnInt(C2V(gTempFile));
    C2V(gInitial_rank) = GetAnInt(C2V(gTempFile));
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_per_rank)[0], &C2V(gCredits_per_rank)[1], &C2V(gCredits_per_rank)[2]);

    LoadGeneralCrushSettings(C2V(gTempFile));

    GetThreeInts(C2V(gTempFile), &C2V(gTime_per_ped_kill)[0], &C2V(gTime_per_ped_kill)[1], &C2V(gTime_per_ped_kill)[2]);
    GetThreeFloats(C2V(gTempFile), &C2V(gSeconds_per_unit_car_damage)[0], &C2V(gSeconds_per_unit_car_damage)[1], &C2V(gSeconds_per_unit_car_damage)[2]);
    GetThreeFloats(C2V(gTempFile), &C2V(gCredits_per_unit_car_damage)[0], &C2V(gCredits_per_unit_car_damage)[1], &C2V(gCredits_per_unit_car_damage)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gTime_wasting_car)[0], &C2V(gTime_wasting_car)[1], &C2V(gTime_wasting_car)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_wasting_car)[0], &C2V(gCredits_wasting_car)[1], &C2V(gCredits_wasting_car)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gTime_rolling_car)[0], &C2V(gTime_rolling_car)[1], &C2V(gTime_rolling_car)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_rolling_car)[0], &C2V(gCredits_rolling_car)[1], &C2V(gCredits_rolling_car)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_checkpoint)[0], &C2V(gCredits_checkpoint)[1], &C2V(gCredits_checkpoint)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gFine_jump_start)[0], &C2V(gFine_jump_start)[1], &C2V(gFine_jump_start)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gCredits_per_second_time_bonus)[0], &C2V(gCredits_per_second_time_bonus)[1], &C2V(gCredits_per_second_time_bonus)[2]);
    GetThreeInts(C2V(gTempFile), &C2V(gCunning_stunt_bonus)[0], &C2V(gCunning_stunt_bonus)[1], &C2V(gCunning_stunt_bonus)[2]);

    GetAString(C2V(gTempFile), C2V(gDefaultCar));
    GetAString(C2V(gTempFile), C2V(gDefaultCockpit));

    C2V(gKnobbledFramePeriod) = 0;
    C2V(gUnknownOpponentFactor) = 1.f;
    C2V(gMinTimeOpponentRepair) = GetAScalar(C2V(gTempFile));
    C2V(gMaxTimeOpponentRepair) = GetAScalar(C2V(gTempFile));

    ParseSpecialVolume(C2V(gTempFile), &C2V(gUnderwaterSpecialVolumeSettings), C2V(gUnderwaterScreenName), 0);

    //unfinished
    c2_abort();
//#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00486ef0, LoadGeneralParameters, LoadGeneralParameters_original)

void C2_HOOK_FASTCALL LoadKeyMapping(void) {
    FILE* f;
    tPath_name the_path;
    int i;

    PathCat(the_path, C2V(gApplication_path), "KEYMAP_X.TXT");
    the_path[c2_strlen(the_path) - 5] = '0' + C2V(gKey_map_index);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CouldNotOpenKeyMapFile);
    }

    for (i = 0; i < REC2_ASIZE(C2V(gKey_mapping)); i++) {
        c2_fscanf((FILE*)f, "%d", &C2V(gKey_mapping)[i]);
    }
    DRfclose(f);
}
C2_HOOK_FUNCTION(0x00487e10, LoadKeyMapping)

void C2_HOOK_FASTCALL LoadHeadupImages(void) {
    int i;
    br_pixelmap* pixmap;

    for (i = 0; i < REC2_ASIZE(C2V(gHeadup_image_info)); i++) {
        PossibleService();
        if (C2V(gHeadup_image_info)[i].avail == eNet_or_otherwise
                || (C2V(gHeadup_image_info)[i].avail == eNot_net && C2V(gNet_mode) == eNet_mode_none)
                || (C2V(gHeadup_image_info)[i].avail == eNet_only && C2V(gNet_mode) != eNet_mode_none)) {
            pixmap = DRLoadPixelmap(C2V(gHeadup_image_info)[i].name);
            DRPixelmapConvertRGB565ToRGB555IfNeeded(pixmap, C2V(gBack_screen)->type);
        } else {
            pixmap = NULL;
        }
        C2V(gHeadup_images)[i] = pixmap;
    }
}
C2_HOOK_FUNCTION(0x0048c150, LoadHeadupImages)

void C2_HOOK_FASTCALL LoadMiscStrings(void) {
    int i;
    FILE *f;
    char s[256];
    tPath_name the_path;

    if (C2V(gPedTextTxtPath) == NULL) {
        C2V(gPedTextTxtPath) = "TEXT.TXT";
    }
    PathCat(the_path, C2V(gApplication_path), C2V(gPedTextTxtPath));
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenTEXT_TXT);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gMisc_strings)); i++) {
        if (DRfeof(f)) {
            break;
        }
        GetALineAndDontArgue(f, s);
        C2V(gMisc_strings)[i] = BrMemAllocate(c2_strlen(s) + 1, kMem_misc_string);
        c2_strcpy(C2V(gMisc_strings)[i], s);
    }
    // Thousands delimiter
    C2V(gMisc_strings)[294][1] = '\0';
    switch (C2V(gMisc_strings)[294][0]) {
    case 'C':
        C2V(gMisc_strings)[294][0] = ',';
        break;
    case 'P':
        C2V(gMisc_strings)[294][0] = '.';
        break;
    case 'S':
        C2V(gMisc_strings)[294][0] = ' ';
        break;
    default:
        break;
    }
    DRfclose(f);
}
C2_HOOK_FUNCTION(0x0048cfc0, LoadMiscStrings)

void C2_HOOK_FASTCALL ReadNetworkSettings(FILE* pF, tNet_game_options* pOptions) {

    pOptions->enable_text_messages = GetAnInt(pF);
    pOptions->show_players_on_map = GetAnInt(pF);
    pOptions->show_powerups_on_map = GetAnInt(pF);
    pOptions->powerup_respawn = GetAnInt(pF);
    pOptions->waste_to_transfer = GetAnInt(pF);
    pOptions->open_game = GetAnInt(pF);
    pOptions->grid_start = GetAnInt(pF);
    pOptions->race_sequence_type = GetAnInt(pF);
    pOptions->random_car_choice = GetAnInt(pF);
    pOptions->car_choice = GetAnInt(pF);
    pOptions->starting_credits = GetAnInt(pF);
    pOptions->starting_target = GetAnInt(pF);
}
C2_HOOK_FUNCTION(0x0048d110, ReadNetworkSettings)

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tNet_game_options, gNet_settings, 9, 0x00763960);

int C2_HOOK_FASTCALL PrintNetOptions(FILE* pF, int pIndex) {

    c2_fprintf(pF, "NETSETTINGS %d\n", pIndex);
    c2_fprintf(pF, "%d // Allow the sending of Abuse-o-Matic(tm) text messages\n", C2V(gNet_settings)[pIndex].enable_text_messages);
    c2_fprintf(pF, "%d // Show cars on map\n", C2V(gNet_settings)[pIndex].show_players_on_map);
    c2_fprintf(pF, "%d // Show pickups on map\n", C2V(gNet_settings)[pIndex].show_powerups_on_map);
    c2_fprintf(pF, "%d // Pickup respawn\n", C2V(gNet_settings)[pIndex].powerup_respawn);
    c2_fprintf(pF, "%d // Waste to transfer\n", C2V(gNet_settings)[pIndex].waste_to_transfer);
    c2_fprintf(pF, "%d // Open game\n", C2V(gNet_settings)[pIndex].open_game);
    c2_fprintf(pF, "%d // Grid start\n", C2V(gNet_settings)[pIndex].grid_start);
    c2_fprintf(pF, "%d // Race order\n", C2V(gNet_settings)[pIndex].race_sequence_type);
    c2_fprintf(pF, "%d // Auto-goody\n", C2V(gNet_settings)[pIndex].random_car_choice);
    c2_fprintf(pF, "%d // Car choice mode\n", C2V(gNet_settings)[pIndex].car_choice);
    c2_fprintf(pF, "%d // Starting credits\n\n", C2V(gNet_settings)[pIndex].starting_credits);
    c2_fprintf(pF, "%d // Target\n\n", C2V(gNet_settings)[pIndex].starting_target);
    return 0;
}
C2_HOOK_FUNCTION(0x0048d7d0, PrintNetOptions)

br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName) {
    FILE* f;
    tPath_name the_path;
    br_font* the_font;
    tU32 data_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(br_font) != 24);

    PathCat(the_path, C2V(gApplication_path), C2V(gGraf_specs)[C2V(gGraf_spec_index)].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);
    f = DRfopen(the_path, "rb");
    PossibleService();
    the_font = BrMemAllocate(sizeof(br_font), kMem_br_font);

    // we read 0x18 bytes as that is the size of the struct in 32 bit code.
    c2_fread(the_font, 0x18, 1, f);
#if !defined(C2_BIG_ENDIAN)
    the_font->flags = BrSwap32(the_font->flags);

    // swap endianness
    the_font->glyph_x = the_font->glyph_x >> 8 | the_font->glyph_x << 8;
    the_font->glyph_y = the_font->glyph_y >> 8 | the_font->glyph_y << 8;
    the_font->spacing_x = the_font->spacing_x >> 8 | the_font->spacing_x << 8;
    the_font->spacing_y = the_font->spacing_y >> 8 | the_font->spacing_y << 8;
#endif

    data_size = 256 * sizeof(br_int_8);
    the_font->width = BrMemAllocate(data_size, kMem_br_font_wid);
    DRfread(the_font->width, data_size, 1, f);
    data_size = 256 * sizeof(br_uint_16);
    the_font->encoding = BrMemAllocate(data_size, kMem_br_font_enc);
    DRfread(the_font->encoding, data_size, 1, f);
#if !defined(C2_BIG_ENDIAN)
    for (i = 0; i < 256; i++) {
        the_font->encoding[i] = the_font->encoding[i] >> 8 | the_font->encoding[i] << 8;
    }
#endif
    PossibleService();
    DRfread(&data_size, sizeof(tU32), 1, f);
#if !defined(C2_BIG_ENDIAN)
    data_size = BrSwap32(data_size);
#endif
    PossibleService();
    the_font->glyphs = BrMemAllocate(data_size, kMem_br_font_glyphs);
    DRfread(the_font->glyphs, data_size, 1u, f);
    DRfclose(f);
    return the_font;
}
C2_HOOK_FUNCTION(0x00466050, LoadBRFont)

FILE* C2_HOOK_FASTCALL OpenRaceFile(void) {
    FILE* f;
    tPath_name the_path;

    PathCat(the_path, C2V(gApplication_path), C2V(gRaces_file_names)[C2V(gCurrent_race_file_index)]);
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotOpenRacesFile);
    }
    return f;
}

void (C2_HOOK_FASTCALL * LoadRaces_original)(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index);
void C2_HOOK_FASTCALL LoadRaces(tRace_list_spec* pRace_list, int* pCount, int pRace_type_index) {

#if 0//defined(C2_HOOKS_ENABLED)
    LoadRaces_original(pRace_list, pCount, pRace_type_index);
#else
    FILE* f;
    br_scalar opponent_count_first_race;
    br_scalar opponent_count_last_race;
    br_scalar opponent_softness_first_race, opponent_hardness_first_race;
    br_scalar opponent_softness_last_race, opponent_hardness_last_race;
    br_scalar opponent_nastiness_first_race;
    br_scalar opponent_nastiness_last_race;
    tTWTVFS twt;
    int powerup_exclusions[50];
    tRace_group_spec* current_group;
    float factor;
    float softness;
    float hardness;
    int hardness_oppo;
    int i;
    int j;
    int count_races;
    int last_race;
    char s[256];
    char* str;

    C2_HOOK_BUG_ON(sizeof(tRace_list_spec) != 0x244);

    twt = -1;
    if (!C2V(gApplicationDataTwtMounted)) {
        twt = TWT_MountEx(C2V(gApplication_path));
    }
    C2V(gCurrent_race_file_index) = pRace_type_index + 1;

    f = OpenRaceFile();

#if defined(C2_STAINLESS_DEVEL)
    /* Initial race is in DATA.TWT */
    C2V(gDevInitialRace) = GetAnInt(f);
#else
    /* Ignore Initial race */
    GetAnInt(f);
    C2V(gDevInitialRace) = 0;
#endif

    /* Default number of opponents in first race */
    opponent_count_first_race = GetAScalar(f);
    /* Default number of opponents in last race */
    opponent_count_last_race = GetAScalar(f);
    /* Softness,hardest rank of opponents in first race */
    GetPairOfFloats(f, &opponent_softness_first_race, &opponent_hardness_first_race);
    /* Softness,hardest rank of opponents in last race */
    GetPairOfFloats(f, &opponent_softness_last_race, &opponent_hardness_last_race);
    /* Opponent nastiness influencer for first race */
    opponent_nastiness_first_race = GetAScalar(f);
    /* Opponent nastiness influencer for last race */
    opponent_nastiness_last_race = GetAScalar(f);

    C2V(gCountRaceGroups) = 0;

    for (i = 0; 1;) {
        GetALineAndDontArgue(f, pRace_list[i].name);
        if (c2_strcmp(pRace_list[i].name, "END") == 0) {
            last_race = 1;
            count_races = i;
        } else {
            last_race = 0;
        }
        if (last_race) {
            break;
        }
        /* Text file name */
        GetAString(f, pRace_list[i].file_name);
        /* Name of interface element */
        GetAString(f, pRace_list[i].interface_name);
        /* Number of opponents (-1 = use default) */
        pRace_list[i].count_opponents = GetAnInt(f);
        /* Number of explicit opponents */
        pRace_list[i].count_explicit_opponents = GetAnInt(f);
        // FIXME: assert(pRace_list[i].count_explicit_opponents <= REC2_ASIZE(pRace_list[i].explicit_opponents))
        for (j = 0; j < pRace_list[i].count_explicit_opponents; j++) {
            pRace_list[i].explicit_opponents[j] = GetAnInt(f);
        }
        /* Opponent nastiness level (-1 = use default) */
        pRace_list[i].opponent_nastiness_level = GetAScalar(f);
        /* Powerup exclusions */
        GetALineAndDontArgue(f, s);
        str = c2_strtok(s, "\t ,/");
        for (j = 0; str != NULL; j++) {
            c2_sscanf(str, "%d", &powerup_exclusions[j]);
            str = c2_strtok(NULL, "\t ,/");
        }
        pRace_list[i].count_powerup_exclusions = j;
        if (pRace_list[i].count_powerup_exclusions != 0) {
            pRace_list[i].powerup_exclusions = BrMemAllocate(pRace_list[i].count_powerup_exclusions * sizeof(int), kMem_misc);
            c2_memcpy(pRace_list[i].powerup_exclusions, powerup_exclusions, pRace_list[i].count_powerup_exclusions * sizeof(int));
        }
        /* Disable time awards */
        pRace_list[i].no_time_awards = GetAnInt(f);
        /* Boundary race (mission) */
        pRace_list[i].is_boundary = GetAnInt(f);
        if (pRace_list[i].is_boundary) {
            C2V(gCountRaceGroups)++;
        }
        /* Race type (0 = Carma1, 1 = Cars, 2 = Peds, 3 = Checkpoints, 4 = Smash, 5 = smash'n'ped) */
        pRace_list[i].race_type = GetAnInt(f);
        /* Initial timer count for each skill level */
        GetThreeInts(f, &pRace_list[i].initial_timer[0], &pRace_list[i].initial_timer[1], &pRace_list[i].initial_timer[2]);
        switch (pRace_list[i].race_type) {
        case kRaceType_Carma1:
        case kRaceType_Checkpoints:
            /* # laps */
            pRace_list[i].count_laps = GetAnInt(f);
            break;
        case kRaceType_Cars:
            /* Number of opponents that must be killed (-1 means all) */
            pRace_list[i].options.cars.count_opponents = GetAnInt(f);
            // FIXME: assert(pRace_list[i].options.cars.count_opponents <= REC2_ASIZE(pRace_list[i].options.cars.opponents))
            for (j = 0; j < pRace_list[i].options.cars.count_opponents; j++) {
                pRace_list[i].options.cars.opponents[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Peds:
            /* Number of ped groups (-1 means all) */
            pRace_list[i].options.peds.count_ped_groups = GetAnInt(f);
            for (j = 0; j < pRace_list[i].options.peds.count_ped_groups; j++) {
                pRace_list[i].options.peds.ped_groups[j] = GetAnInt(f);
            }
            break;
        case kRaceType_Smash:
            /* Smash variable number */
            pRace_list[i].options.smash.var_smash_number = GetAnInt(f);
            /* Smash variable target */
            pRace_list[i].options.smash.var_smash_target = GetAnInt(f);
            break;
        case kRaceType_SmashNPed:
            /* Smash variable number */
            pRace_list[i].options.smash_and_peds.var_smash_number = GetAnInt(f);
            /* Smash variable target */
            pRace_list[i].options.smash_and_peds.var_smash_target = GetAnInt(f);
            /* Ped group index for required extra kills */
            pRace_list[i].options.smash_and_peds.ped_group_index = GetAnInt(f);
            break;
        }
        /* Race completed bonus for each skill level (all laps raced or target met)*/
        GetThreeInts(f, &pRace_list[i].completion_bonus[0], &pRace_list[i].completion_bonus[1], &pRace_list[i].completion_bonus[2]);
        if (pRace_list[i].race_type == kRaceType_Carma1) {
            /* Race completed bonus (all peds killed) for each skill level */
            GetThreeInts(f, &pRace_list[i].completion_bonus_peds[0], &pRace_list[i].completion_bonus_peds[1], &pRace_list[i].completion_bonus_peds[2]);
            /* Race completed bonus (all oppos wasted) for each skill level */
            GetThreeInts(f, &pRace_list[i].completion_bonus_opponents[0], &pRace_list[i].completion_bonus_opponents[1], &pRace_list[i].completion_bonus_opponents[2]);
        }
        /* Race description */
        GetALineAndDontArgue(f, pRace_list[i].description);
        if (DRStricmp(pRace_list[i].description, "none") == 0) {
            pRace_list[i].description[0] = '\0';
        }
        /* Expansion */
        pRace_list[i].expansion = GetAnInt(f) & 0x1;

        if (pRace_type_index < 0 || pRace_list[i].race_type == kRaceType_Carma1) {
            i++;
        }
    }
    *pCount = count_races;
    DRfclose(f);
    if (C2V(gRaceGroups) != NULL) {
        BrMemFree(C2V(gRaceGroups));
    }
    C2_HOOK_BUG_ON(sizeof(tRace_group_spec) != 12);
    C2V(gRaceGroups) = BrMemAllocate((C2V(gCountRaceGroups) == 0 ? 1 : C2V(gCountRaceGroups)) * sizeof(tRace_group_spec), kMem_misc);
    C2V(gRaceGroups)[0].count_races = 0;
    C2V(gRaceGroups)[0].races = pRace_list;

    current_group = &C2V(gRaceGroups)[0];

    for (i = 0; i < count_races; i++) {
        int count_opponents;

        if (pRace_list[i].opponent_nastiness_level <= 0.f) {
            pRace_list[i].opponent_nastiness_level = opponent_nastiness_first_race + (opponent_nastiness_last_race - opponent_nastiness_first_race) * (float)i / (float)(count_races - 1);
        }
        count_opponents = pRace_list[i].count_opponents;
        if (count_opponents < 0) {
            count_opponents = (int)(opponent_count_first_race + (float)i / (float)(count_races - 1) * (opponent_count_last_race - opponent_count_first_race));
        }
        pRace_list[i].count_opponents = 0;
        if (pRace_list[i].count_explicit_opponents < count_opponents) {
            factor =  (float)i / (float)(count_races - 1);
            softness = opponent_softness_first_race + factor * (opponent_softness_last_race - opponent_softness_first_race);
            hardness = opponent_hardness_first_race + factor * (opponent_hardness_last_race - opponent_hardness_first_race) - softness;
            for (j = pRace_list[i].count_explicit_opponents; j < count_opponents; j++) {
                hardness_oppo = (int)(softness + (float)hardness * (float)(j - pRace_list[i].count_explicit_opponents) / (float)(count_opponents - 1 - pRace_list[i].count_explicit_opponents));

                if (hardness_oppo <= 0) {
                    pRace_list[i].explicit_opponents[j] = -1;
                } else {
                    pRace_list[i].explicit_opponents[j] = -MIN(hardness_oppo, 5);
                }
            }
        } else {
            pRace_list[i].count_explicit_opponents = count_opponents;
        }
        pRace_list[i].count_explicit_opponents = count_opponents;
        pRace_list[i].group = current_group;
        if (pRace_list[i].is_boundary) {
            current_group->mission = &pRace_list[i];
            current_group++;
            if (current_group - C2V(gRaceGroups) < C2V(gCountRaceGroups)) {
                current_group->count_races = 0;
                current_group->races = &pRace_list[i+1];
            }
        } else {
            current_group->count_races++;
        }
    }
    C2V(gRaceGroups2) = C2V(gRaceGroups);
    if (!C2V(gApplicationDataTwtMounted)) {
        TWT_UnmountEx(twt);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048c1c0, LoadRaces, LoadRaces_original)
