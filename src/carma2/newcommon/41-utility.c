#include "41-utility.h"

#include "01-network.h"
#include "02-init.h"
#include "08-loading1.h"
#include "28-world3.h"
#include "18-graphics2.h"
#include "40-main.h"
#include "42-input.h"
#include "44-mainmenu.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <ctype.h>
#include <float.h>
#include <math.h>


// GLOBAL: CARMA2_HW 0x006abef8
tU32 last_service;

// GLOBAL: CARMA2_HW 0x006b5f40
char* gMisc_strings[300];

// GLOBAL: CARMA2_HW 0x0074ca04
tMaterial_exception* gMaterial_exceptions;

// GLOBAL: CARMA2_HW 0x006b63f4
br_pixelmap* g16bit_palette;

// GLOBAL: CARMA2_HW 0x006b63f0
br_pixelmap* gPalette_source;

// GLOBAL: CARMA2_HW 0x006abef4
tU32 gLost_time;

// GLOBAL: CARMA2_HW 0x0079efb0
tU32 gLast_tick_count;

// GLOBAL: CARMA2_HW 0x00762180
char gCurrent_load_directory[256];

// GLOBAL: CARMA2_HW 0x00761a80
char gCurrent_load_name[256];

// FUNCTION: CARMA2_HW 0x00513400
br_error C2_HOOK_FASTCALL DRBrEnd(void) {
    br_device *dev;

    if (!gBr_initialized) {
        return 0x1006;
    }

    _BrEndHook();
    gBr_initialized = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00513460
void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src) {
    int len;
    int i;

    len = (int)strlen(src);
    for (i = 0; i < (int)len; i++) {
        dest[i] = toupper(src[i]);
    }
    dest[len] = '\0';
}

// FUNCTION: CARMA2_HW 0x005134b0
int C2_HOOK_FASTCALL CheckQuit(void) {
    int result;

    // GLOBAL: CARMA2_HW 0x006abee0
    static int active;

    result = 0;
    if (!active) {
        if (KeyIsDown(1) && KeyIsDown(7)) {
            active = 1;
            do {
            } while (AnyKeyDown());
            result = 1;
            if (DoVerifyQuit(1)) {
                QuitGame();
            }
            active = 0;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00513510
double C2_HOOK_FASTCALL sqr(double pN) {

    return pN * pN;
}

// FUNCTION: CARMA2_HW 0x00513520
int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB) {
    int num;

#if RAND_MAX == 0x7fff
    num = (pB + 1 - pA) * rand() / (RAND_MAX + 1) + pA;
    return num;
#else
    num = (pB + 1 - pA) * (rand() % 0x8000) / (0x7fff + 1) + pA;
    return num;
#endif
}

// FUNCTION: CARMA2_HW 0x00513550
int C2_HOOK_FASTCALL PercentageChance(int pC) {

    if (pC == 0) {
        return 0;
    }
    return IRandomBetween(0, 99) < pC;
}

// FUNCTION: CARMA2_HW 0x00513580
int C2_HOOK_FASTCALL IRandomPosNeg(int pN) {

    return IRandomBetween(-pN, pN);
}

/* Preserve x87 load order, NaN selection and the PosNeg store before rand. */
static void C2_HOOK_FASTCALL UtilityRandomNegative(float* output, const float* input) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov eax, output
        mov ecx, input
        fld dword ptr [ecx]
        fchs
        fstp dword ptr [eax]
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile("flds %1; fchs; fstps %0" : "=m" (*output) : "m" (*input) : "memory", "st");
#else
    volatile double value = *input;

    *output = (float)-value;
#endif
}

static double C2_HOOK_FASTCALL UtilityRandomScalar(const float* pA, const float* pB, int number) {
#if (defined(_MSC_VER) && defined(_M_IX86)) || (defined(__GNUC__) && defined(__i386__))
    double result;
    const float negative_inverse = -1.f / 32768.f;

#if defined(_MSC_VER)
    __asm {
        mov ecx, pA
        mov edx, pB
        fld dword ptr [edx]
        fsub dword ptr [ecx]
        fild number
        fxch st(1)
        fmulp st(1), st(0)
        fmul negative_inverse
        fsubr dword ptr [ecx]
        fstp result
    }
#else
    __asm__ volatile("flds (%2); fsubs (%1); fildl %3; fxch %%st(1); fmulp %%st, %%st(1); fmuls %4; fsubrs (%1); fstpl %0"
        : "=m" (result) : "r" (pA), "r" (pB), "m" (number), "m" (negative_inverse) : "memory", "st", "st(1)");
#endif
    return result;
#else
    return *pA - (((double)*pB - *pA) * number) * (-1.0 / 32768.0);
#endif
}

// FUNCTION: CARMA2_HW 0x005135b0
double C2_HOOK_STDCALL FRandomBetween(float pA, float pB) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    /* MSVC5 retains this arithmetic on x87 without a scalar return spill. */
    return (float)rand() * (pB - pA) / 32768.f + pA;
#else
    int number = rand();

    return UtilityRandomScalar(&pA, &pB, number);
#endif
}

// FUNCTION: CARMA2_HW 0x005135e0
double C2_HOOK_STDCALL FRandomPosNeg(float pN) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return FRandomBetween(-pN, pN);
#else
    float negative;
    int number;

    UtilityRandomNegative(&negative, &pN);
    number = rand();
    return UtilityRandomScalar(&negative, &pN, number);
#endif
}

// FUNCTION: CARMA2_HW 0x00513620
double C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return FRandomBetween(pA, pB);
#else
    int number = rand();

#if defined(__GNUC__) && defined(__i386__)
    /* Prevent identical-function folding from reloading float arguments before rand. */
    __asm__ volatile("" : : : "memory");
#endif
    return UtilityRandomScalar(&pA, &pB, number);
#endif
}

// FUNCTION: CARMA2_HW 0x00513650
double C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN) {
#if defined(_MSC_VER) && _MSC_VER < 1300
    return SRandomBetween(-pN, pN);
#else
    br_scalar negative;
    int number;

    UtilityRandomNegative(&negative, &pN);
    number = rand();
    return UtilityRandomScalar(&negative, &pN, number);
#endif
}

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

#if defined(REC2_FIX_BUGS) || !defined(REC2_MATCHING)
    if (pDestn_str != pStr_1) {
        strcpy(pDestn_str, pStr_1);
    }
#else
    strcpy(pDestn_str, pStr_1);
#endif

    if (strlen(pStr_2) != 0) {
        strcat(pDestn_str, gDir_separator);
        strcat(pDestn_str, pStr_2);
    }
}

// FUNCTION: CARMA2_HW 0x00513770
float C2_HOOK_STDCALL tandeg(float pAngle) {
    double radians;

    radians = pAngle * (REC2_PI / 180.0);
    return (float)(sin(radians) / cos(radians));
}

// FUNCTION: CARMA2_HW 0x00513790
tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    PFfseek(pF, 0, SEEK_END);
    the_size = PFftell(pF);
    PFrewind(pF);
    return the_size;
}

// FUNCTION: CARMA2_HW 0x005137d0
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags) {
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(pType, pW, pH, pPixels, pFlags);
    if (pm != NULL) {
        pm->origin_x = 0;
        pm->origin_y = 0;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00513820
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH) {
    br_pixelmap* the_map;

    the_map = BrPixelmapAllocateSub(pPm, pX, pY, pW, pH);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
    }
    return the_map;
}

// FUNCTION: CARMA2_HW 0x00513870
br_pixelmap* C2_HOOK_FASTCALL DRImageLoad(const char* path) {
    tPath_name path_buffer;
    tPath_name directory;
    tPath_name name;
    size_t length;
    size_t start;
    size_t i;
    int error;

    strcpy(path_buffer, path);
    length = strlen(path_buffer);
    start = 0;
    for (i = length - 1; i != 0; i--) {
        if (path_buffer[i] == gDir_separator[0]) {
            start = i;
            break;
        }
    }
    memcpy(directory, path_buffer, start);
    directory[start] = '\0';
    if (directory[0] != '\0') {
        start++;
    }
    for (i = 0; path_buffer[start + i] != '.' && start + i != length; i++) {
        name[i] = path_buffer[start + i];
    }
    name[i] = '\0';
    return DRLdImg(directory, name, gRender_palette, gPixelFlags, &error);
}

// FUNCTION: CARMA2_HW 0x00513970
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapLoad(char* pFile_name) {
    br_pixelmap* the_map;
    br_int_16 row_bytes;

    the_map = BrPixelmapLoad(pFile_name);
    if (the_map != NULL) {
        row_bytes = the_map->row_bytes;
        the_map->origin_x = 0;
        the_map->row_bytes = (br_int_16)((row_bytes + sizeof(int32_t) - 1) & ~(sizeof(int32_t) - 1));
        the_map->origin_y = 0;
    }
    return the_map;
}

// FUNCTION: CARMA2_HW 0x005139a0
void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath) {
    size_t pathLen;
    size_t dirLen;
    size_t i;

    pathLen = strlen(path);
    dirLen = 0;
    for (i = pathLen - 1; ; i--) {
        if (i == 0) {
            break;
        }
        if (path[i] == gDir_separator[0]) {
            dirLen = i;
            break;
        }
    }
    memcpy(dirPath, path, dirLen);
    dirPath[dirLen] = '\0';
    if (*dirPath != '\0') {
        dirLen += 1;
    }
    for (i = 0; path[dirLen + i] != '.' && dirLen + i != pathLen; i++) {
        stemPath[i] = path[dirLen + i];
    }
    stemPath[i] = '\0';
}

// FUNCTION: CARMA2_HW 0x00513a30
int C2_HOOK_FASTCALL LoadTextureTryAllLocations(char* pName, br_pixelmap** pMaps, int pCapacity) {
    tPath_name path1;
    tPath_name path2;
    tPath_name path3;
    tPath_name path4;
    tName_list list;
    const char* load_path;
    int i;
    int error;

    (void)pCapacity;
    list.size = 0;
    pName[strlen(pName) - 4] = '\0';
    PathCat(path1, gApplication_path, gCurrent_load_directory);
    if (strcmp(gCurrent_load_directory, "COMMON") != 0 && strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path1, path1, gCurrent_load_name);
    }
    PathCat(path1, path1, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(path1, path1, pName);
    PathCat(path3, gApplication_path, gCurrent_load_directory);
    if (strcmp(gCurrent_load_directory, "COMMON") != 0 && strcmp(gCurrent_load_directory, "INTRFACE") != 0) {
        PathCat(path3, path3, gCurrent_load_name);
    }
    PathCat(path3, path3, pName);

    PathCat(path2, path1, "TIFFX");
    PFForEveryFile2(path2, (tEnumPathCallback)GetFileName, &list);
    PathCat(path4, path1, "PIX8");
    PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
    PathCat(path2, path1, "TIFFRGB");
    PFForEveryFile2(path2, (tEnumPathCallback)GetAdditionalFileName, &list);
    PathCat(path4, path1, "PIX16");
    PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
    load_path = path1;
    if (list.size == 0) {
        PathCat(path2, path3, "TIFFX");
        PFForEveryFile2(path2, (tEnumPathCallback)GetFileName, &list);
        PathCat(path4, path3, "PIX8");
        PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
        PathCat(path2, path3, "TIFFRGB");
        PFForEveryFile2(path2, (tEnumPathCallback)GetAdditionalFileName, &list);
        PathCat(path4, path3, "PIX16");
        PFForEveryFile2(path4, (tEnumPathCallback)GetAdditionalFileName, &list);
        load_path = path3;
    }
    for (i = 0; i < list.size; i++) {
        pMaps[i] = DRLdImg(load_path, list.items[i], gRender_palette, gPixelFlags, &error);
    }
    return i;
}

// FUNCTION: CARMA2_HW 0x00514570
int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    strcpy(texturePath, texturePathNoExt);
    strcat(texturePath, ".TIF");
    SepDirAndFilename(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = DRLdImg(texturePathDir, texturePathStem, gRender_palette, gPixelFlags, &errorCode);
    if (pixelmaps[0] == NULL || errorCode != 0) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x005146c0
void C2_HOOK_FASTCALL WaitFor(tU32 pDelay) {
    tU32 end_time;

    end_time = PDGetTotalTime() + pDelay;
    while (PDGetTotalTime() < end_time && !AnyKeyDown()) {
        SoundService();
    }
}

// FUNCTION: CARMA2_HW 0x005146f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg) {
    intptr_t result;

    result = callback(pActor, arg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurse(pActor, callback, arg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005147b0
intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg) {

    if (pActor->identifier != NULL && strcmp(pActor->identifier, (const char*)pArg) == 0) {
        return (intptr_t)pActor;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00514730
br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName) {

    return (br_actor*)DRActorEnumRecurse(pSearch_root, CompareActorID, (void*)pName);
}

// FUNCTION: CARMA2_HW 0x00514800
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithMat(br_actor* pActor, br_material* pMat, recurse_with_mat_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;

    if (pActor->material != NULL) {
        pMat = pActor->material;
    }
    result = pCall_back(pActor, pMat, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithMat(pActor, pMat, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00514850
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;
    br_matrix34 combined_transform;

    if (pMatrix == NULL) {
        BrMatrix34Copy(&combined_transform, &pActor->t.t.mat);
    } else {
        BrMatrix34Mul(&combined_transform, pMatrix, &pActor->t.t.mat);
    }
    result = pCall_back(pActor, &combined_transform, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithTrans(pActor, &combined_transform, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005148c0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithSnart(br_actor* pActor, br_matrix34* pMatrix, tDRActorEnumRecurseWithSnart_cbfn* pCall_back, void* pArg) {
    br_matrix34 combined_transform;
    intptr_t result;

    if (pMatrix != NULL) {
        BrMatrix34Mul(&combined_transform, &pActor->t.t.mat, pMatrix);
    } else {
        BrMatrix34Identity(&combined_transform);
    }
    result = pCall_back(pActor, &combined_transform, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithSnart(pActor, &combined_transform, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00514930
int C2_HOOK_FASTCALL sign(int pNumber) {
    if (pNumber > 0) {
        return 1;
    }
    return pNumber < 0 ? -1 : 0;
}

// FUNCTION: CARMA2_HW 0x005149a0
FILE* C2_HOOK_FASTCALL OpenUniqueFileB(char* pPrefix, char* pExtension) {
    int index;
    FILE* f;
    tPath_name the_path;

    for (index = 0; index < 10000; index++) {
        PathCat(the_path, gApplication_path, pPrefix);
        sprintf(the_path + strlen(the_path), "%04d.%s", index, pExtension);
        f = DRfopen(the_path, "rt");
        if (f == NULL) {
            return DRfopen(the_path, "wb");
        }
        PFfclose(f);
    }
    return NULL;
}

// FUNCTION: CARMA2_HW 0x00514ab0
void C2_HOOK_FASTCALL PrintScreenFile(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    tU8* pixel_ptr;

    bit_map_size = gBack_screen->height * gBack_screen->row_bytes;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, bit_map_size + 0x436);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x436);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, gBack_screen->row_bytes);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (8 -> 8bit palletized ==> #colors = 256)
    WriteU16L(pF, 8);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 3. Color table (=palette)
    for (i = 0; i < 256; i++) {
        // red, green, blue, unused
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 0]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 1]);
        WriteU8L(pF, ((tU8*)gCurrent_palette->pixels)[4 * i + 2]);
        WriteU8L(pF, 0);
    }

    // 4. Pixel Data (=LUT)
    pixel_ptr = (tU8*)gBack_screen->pixels + bit_map_size - gBack_screen->row_bytes;
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->row_bytes; j++) {
            WriteU8L(pF, *pixel_ptr++);
        }
        pixel_ptr -= 2 * gBack_screen->row_bytes;
    }
    WriteU16L(pF, 0);
}

void C2_HOOK_FASTCALL PrintScreenFile16(FILE* pF) {
    int i;
    int j;
    int bit_map_size;
    tU16* pixel_ptr;

    if (gBack_screen->pixels == NULL) {
        BrPixelmapDirectLock(gBack_screen, 1);
    }

    bit_map_size = gBack_screen->height * 3 * gBack_screen->width;

    // 1. BMP Header
    //    1. 'BM' Signature
    WriteU8L(pF, 'B');
    WriteU8L(pF, 'M');
    //    2. File size in bytes (header = 0xe bytes; infoHeader = 0x28 bytes; colorTable = 0x400 bytes; pixelData = xxx)
    WriteU32L(pF, 0x36 + bit_map_size);
    //    3. unused
    WriteU16L(pF, 0);
    //    4. unused
    WriteU16L(pF, 0);
    //    5. pixelData offset (from beginning of file)
    WriteU32L(pF, 0x36);

    // 2. Info Header
    //    1. InfoHeader Size
    WriteU32L(pF, 0x28);
    //    2. Width of bitmap in pixels
    WriteU32L(pF, gBack_screen->width);
    //    3. Height of bitmap in pixels
    WriteU32L(pF, gBack_screen->height);
    //    4. Number of planes
    WriteU16L(pF, 1);
    //    5. Bits per pixels / palletization (0x18 -> 24bit colours ==> #colors = 2^24)
    WriteU16L(pF, 0x18);
    //    6. Compression (0 = BI_RGB -> no compression)
    WriteU32L(pF, 0);
    //    7. Image Size (0 --> no compression)
    WriteU32L(pF, 0);
    //    8. Horizontal Pixels Per Meter
    WriteU32L(pF, 0);
    //    9. Vertical Pixels Per Meter
    WriteU32L(pF, 0);
    //    10. # Actually used colors
    WriteU32L(pF, 0);
    //    11. Number of important colors
    WriteU32L(pF, 256);

    // 4. Pixel Data (=LUT)
    pixel_ptr = (tU16*)((tU8*)gBack_screen->pixels + bit_map_size - gBack_screen->row_bytes);
    for (i = 0; i < gBack_screen->height; i++) {
        for (j = 0; j < gBack_screen->width; j++) {
            tU8 r, g, b;
            tU16 pixel = *pixel_ptr++;
            if (gBack_screen->type == BR_PMT_RGB_565) {
                b = pixel << 3;
                g = (pixel >> 3) & 0xf8;
                r = (pixel >> 8) & 0xf8;
            } else if (gBack_screen->type == BR_PMT_RGB_555) {
                b = pixel << 3;
                g = (pixel >> 2) & 0xf8;
                r = (pixel >> 7) & 0xf8;
            } else {
                b = 0;
                g = 0;
                r = 0;
            }
            WriteU8L(pF, b);
            WriteU8L(pF, g);
            WriteU8L(pF, r);
        }
        pixel_ptr = (tU16*)((tU8*)pixel_ptr + 2 * gBack_screen->width - gBack_screen->row_bytes);
    }
    WriteU16L(pF, 0);

    if (gBack_screen->pixels != NULL) {
        BrPixelmapDirectUnlock(gBack_screen);
    }
}

// FUNCTION: CARMA2_HW 0x00514c30
tU32 C2_HOOK_FASTCALL GetTotalTime(void) {

    if (gAction_replay_mode) {
        return gLast_replay_frame_time;
    } else {
        if (gNet_mode != eNet_mode_none) {
            return gLast_tick_count + gFrame_period;
        } else {
            return gLast_tick_count + gFrame_period - gLost_time;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00514c70
tU32 C2_HOOK_FASTCALL GetRaceTime(void) {

    return PDGetTotalTime() - gRace_start;
}

// FUNCTION: CARMA2_HW 0x00514c80
void C2_HOOK_FASTCALL AddLostTime(tU32 pLost_time) {

    gLost_time += pLost_time;
}

// FUNCTION: CARMA2_HW 0x00514c90
void C2_HOOK_FASTCALL AssertThisTimeAsCurrentTime(void) {
    tU32 now;

    now = PDGetTotalTime();
    gLost_time = now - gLast_tick_count;
    gLast_tick_count += gLost_time;
}

// FUNCTION: CARMA2_HW 0x00514cb0
void C2_HOOK_FASTCALL TimerString(tU32 pTime, char* pStr, undefined4 pArg3, int pFudge_colon, int pFloat) {
    int seconds;

    seconds = (pTime + 500) / 1000;
    if (pFudge_colon || seconds > 59) {
        if (pArg3) {
            sprintf(pStr, "%d:%02d", seconds / 60, seconds % 60);
        } else {
            sprintf(pStr, "%d:%02d", seconds / 60, seconds % 60);
        }
    } else {
        if (pFloat) {
            sprintf(pStr, "%.1f", (double)pTime / 1000.0);
        } else {
            sprintf(pStr, "%d", seconds);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00514d70
const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {

    return gMisc_strings[pIndex];
}

// FUNCTION: CARMA2_HW 0x00514db0
int C2_HOOK_FASTCALL Flash(tU32 pPeriod, tU32* pLast_change, int* pCurrent_state) {
    tU32 the_time;

    the_time = PDGetTotalTime();
    if (the_time - *pLast_change > pPeriod) {
        *pCurrent_state = !*pCurrent_state;
        *pLast_change = the_time;
    }
    return *pCurrent_state;
}

double C2_HOOK_FASTCALL RGBDifferenceSqr(tRGB_colour* pColour_1, tRGB_colour* pColour_2) {

    return REC2_SQR(pColour_1->red - pColour_2->red)
           + REC2_SQR(pColour_1->green - pColour_2->green)
           + REC2_SQR(pColour_1->blue - pColour_2->blue);
}

int C2_HOOK_FASTCALL FindBestMatch(tRGB_colour* pRGB_colour, br_pixelmap* pPalette) {
    int n;
    int near_c;
    double min_d;
    double d;
    tRGB_colour trial_RGB;
    br_colour* dp;

    near_c = 127;
    min_d = DBL_MAX;
    dp = pPalette->pixels;
    for (n = 0; n < 256; n++) {
        trial_RGB.red = (dp[n] >> 16) & 0xff;
        trial_RGB.green = (dp[n] >> 8) & 0xff;
        trial_RGB.blue = (dp[n] >> 0) & 0xff;
        d = RGBDifferenceSqr(pRGB_colour, &trial_RGB);
        if (d < min_d) {
            min_d = d;
            near_c = n;
        }
    }
    return near_c;
}

void C2_HOOK_FASTCALL BuildShadeTablePath(char* pThe_path, int pR, int pG, int pB) {
    char s[32];

    s[0] = 's';
    s[1] = 't';
    s[2] = 'A' + ((pR >> 4) % 0x10);
    s[3] = 'A' + ((pR >> 0) % 0x10);
    s[4] = 'A' + ((pG >> 4) % 0x10);
    s[5] = 'A' + ((pG >> 0) % 0x10);
    s[6] = 'A' + ((pB >> 4) % 0x10);
    s[7] = 'A' + ((pB >> 0) % 0x10);
    s[8] = '\0';
    strcat(s, ".TAB");
    PathCat(pThe_path, gApplication_path, "SHADETAB");
    PathCat(pThe_path, pThe_path, s);
}

br_pixelmap* C2_HOOK_FASTCALL LoadGeneratedShadeTable(int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    return BrPixelmapLoad(the_path);
}

void C2_HOOK_FASTCALL SaveGeneratedShadeTable(br_pixelmap* pThe_table, int pR, int pG, int pB) {
    char the_path[256];

    BuildShadeTablePath(the_path, pR, pG, pB);
    BrPixelmapSave(the_path, pThe_table);
}

// FUNCTION: CARMA2_HW 0x00514e40
br_pixelmap* C2_HOOK_FASTCALL GenerateShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter) {

    PossibleService();
    return GenerateDarkenedShadeTable(
        pHeight,
        pPalette,
        pRed_mix,
        pGreen_mix,
        pBlue_mix,
        pQuarter,
        pHalf,
        pThree_quarter,
        1.0f);
}

// FUNCTION: CARMA2_HW 0x00514eb0
br_pixelmap* C2_HOOK_FASTCALL GenerateDarkenedShadeTable(int pHeight, br_pixelmap* pPalette, int pRed_mix, int pGreen_mix, int pBlue_mix, float pQuarter, float pHalf, float pThree_quarter, br_scalar pDarken) {
    br_pixelmap* the_table;
    tRGB_colour the_RGB;
    tRGB_colour new_RGB;
    tRGB_colour ref_col;
    br_colour* cp;
    char* tab_ptr;
    char* shade_ptr;
    int f_i;
    double f_total_minus_1;
    double ratio1;
    double ratio2;
    int i;
    int c;

    the_table = LoadGeneratedShadeTable(pRed_mix, pGreen_mix, pBlue_mix);
    if (the_table == NULL) {
        the_table = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, pHeight, NULL, 0);
        if (the_table == NULL) {
            FatalError(kFatalError_CannotLoadAGeneratedShadeTable);
        }
        cp = pPalette->pixels;

        ref_col.red = pRed_mix;
        ref_col.green = pGreen_mix;
        ref_col.blue = pBlue_mix;

        for (c = 0, tab_ptr = the_table->pixels; c < 256; c++, tab_ptr++) {
            the_RGB.red = (int)(((cp[c] >> 16) & 0xff) * pDarken);
            the_RGB.green = (int)(((cp[c] >> 8) & 0xff) * pDarken);
            the_RGB.blue = (int)(((cp[c] >> 0) & 0xff) * pDarken);

            if (pHeight == 1) {
                f_total_minus_1 = 1.0;
            } else {
                f_total_minus_1 = (double)(pHeight - 1);
            }
            for (i = 0, shade_ptr = tab_ptr; i < pHeight; i++, shade_ptr += 0x100) {
                if (pHeight == 1) {
                    f_i = 1;
                } else {
                    f_i = i;
                }
                ratio1 = (double)f_i / f_total_minus_1;
                if (ratio1 < 0.5) {
                    if (ratio1 < 0.25) {
                        ratio1 = pQuarter * ratio1 * 4.0;
                    } else {
                        ratio1 = pQuarter + (ratio1 - 0.25f) * (pHalf - pQuarter) * 4.0;
                    }
                } else {
                    if (ratio1 < 0.75) {
                        ratio1 = pHalf + (ratio1 - 0.5) * (pThree_quarter - pHalf) * 4.0;
                    } else {
                        ratio1 = 1.0 - (1.0 - pThree_quarter) * (1.0 - ratio1) * 4.0;
                    }
                }
                ratio2 = 1.0 - ratio1;
                new_RGB.red = (int)(ref_col.red * ratio1 + the_RGB.red * ratio2);
                new_RGB.green = (int)(ref_col.green * ratio1 + the_RGB.green * ratio2);
                new_RGB.blue = (int)(ref_col.blue * ratio1 + the_RGB.blue * ratio2);
                *shade_ptr = FindBestMatch(&new_RGB, pPalette);
            }
        }
        SaveGeneratedShadeTable(the_table, pRed_mix, pGreen_mix, pBlue_mix);
    }
    BrTableAdd(the_table);
    return the_table;
}

// FUNCTION: CARMA2_HW 0x005155d0
void C2_HOOK_FASTCALL PossibleService(void) {
    tU32 time;

    time = PDGetTotalTime();
    if (time - last_service > 200 && !gProgram_state.racing) {
        SoundService();
        NetService(gProgram_state.racing);
        last_service = time;
    }
}

/* Original PC53 grouping, NaN operands and sequential writes require x87 here. */
// FUNCTION: CARMA2_HW 0x00515610
void C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3* pA, const br_vector3* pB, const br_matrix34* pC) {
#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        mov ecx, pA
        mov edx, pB
        mov eax, pC
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 8]
        fld dword ptr [edx]
        fxch st(2)
        fsub dword ptr [eax + 0x28]
        fxch st(1)
        fsub dword ptr [eax + 0x2c]
        fxch st(2)
        fsub dword ptr [eax + 0x24]
        fld dword ptr [eax + 8]
        fld dword ptr [eax + 4]
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(4)
        fld st(2)
        fmul dword ptr [eax]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fstp dword ptr [ecx]
        fld dword ptr [eax + 0xc]
        fld dword ptr [eax + 0x10]
        fld dword ptr [eax + 0x14]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(4)
        fxch st(2)
        fmul st(0), st(5)
        fxch st(2)
        faddp st(1), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fstp dword ptr [ecx + 4]
        fld dword ptr [eax + 0x18]
        fld dword ptr [eax + 0x1c]
        fld dword ptr [eax + 0x20]
        fxch st(2)
        fmul st(0), st(3)
        fxch st(1)
        fmul st(0), st(4)
        fxch st(2)
        fmul st(0), st(5)
        fxch st(2)
        faddp st(1), st(0)
        fxch st(1)
        faddp st(1), st(0)
        fstp dword ptr [ecx + 8]
        fstp st(0)
        fxch st(1)
        fstp st(0)
        fstp st(0)
    }
#elif defined(__GNUC__) && defined(__i386__)
    __asm__ volatile(
        ".intel_syntax noprefix\n\t"
        "fld dword ptr [edx + 4]\n\t"
        "fld dword ptr [edx + 8]\n\t"
        "fld dword ptr [edx]\n\t"
        "fxch st(2)\n\t"
        "fsub dword ptr [eax + 0x28]\n\t"
        "fxch st(1)\n\t"
        "fsub dword ptr [eax + 0x2c]\n\t"
        "fxch st(2)\n\t"
        "fsub dword ptr [eax + 0x24]\n\t"
        "fld dword ptr [eax + 8]\n\t"
        "fld dword ptr [eax + 4]\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fld st(2)\n\t"
        "fmul dword ptr [eax]\n\t"
        "fxch st(1)\n\t"
        "faddp st(2), st(0)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx]\n\t"
        "fld dword ptr [eax + 0xc]\n\t"
        "fld dword ptr [eax + 0x10]\n\t"
        "fld dword ptr [eax + 0x14]\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(5)\n\t"
        "fxch st(2)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx + 4]\n\t"
        "fld dword ptr [eax + 0x18]\n\t"
        "fld dword ptr [eax + 0x1c]\n\t"
        "fld dword ptr [eax + 0x20]\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(3)\n\t"
        "fxch st(1)\n\t"
        "fmul st(0), st(4)\n\t"
        "fxch st(2)\n\t"
        "fmul st(0), st(5)\n\t"
        "fxch st(2)\n\t"
        "faddp st(1), st(0)\n\t"
        "fxch st(1)\n\t"
        "faddp st(1), st(0)\n\t"
        "fstp dword ptr [ecx + 8]\n\t"
        "fstp st(0)\n\t"
        "fxch st(1)\n\t"
        "fstp st(0)\n\t"
        "fstp st(0)\n\t"
        ".att_syntax prefix"
        :
        : "a" (pC), "c" (pA), "d" (pB)
        : "memory", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)");
#else
    double x = (double)pB->v[0] - pC->m[3][0];
    double y = (double)pB->v[1] - pC->m[3][1];
    double z = (double)pB->v[2] - pC->m[3][2];
    volatile double term_x;
    volatile double term_y;
    volatile double term_z;
    volatile double first_sum;
    int i;

    for (i = 0; i < 3; i++) {
        term_x = (double)pC->m[i][0] * x;
        term_y = (double)pC->m[i][1] * y;
        term_z = (double)pC->m[i][2] * z;
        first_sum = i == 0 ? term_y + term_z : term_x + term_y;
        pA->v[i] = (br_scalar)(first_sum + (i == 0 ? term_x : term_z));
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00515690
void C2_HOOK_FASTCALL DRPixelmapRectangleCopy(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_uint_16 w, br_uint_16 h) {

    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}

// FUNCTION: CARMA2_HW 0x00515700
int C2_HOOK_FASTCALL NormalSideOfPlane(br_vector3* pPoint, br_vector3* pNormal, br_scalar pD) {

    return (BrVector3Dot(pNormal, pPoint) - pD) * BrVector3Dot(pNormal, pNormal) >= 0.0f;
}

// FUNCTION: CARMA2_HW 0x00515780
br_material* C2_HOOK_FASTCALL DRMaterialClone(br_material* pMaterial, int pSet_identifier) {
    br_material* the_material;
    char s[256];
    int version;

    // GLOBAL: CARMA2_HW 0x006abefc
    static int gVersion_suffix;

    the_material = BrMaterialAllocate(NULL);
    the_material->flags = pMaterial->flags;
    the_material->ka = pMaterial->ka;
    the_material->kd = pMaterial->kd;
    the_material->ks = pMaterial->ks;
    the_material->power = pMaterial->power;
    the_material->colour = pMaterial->colour;
    the_material->index_base = pMaterial->index_base;
    the_material->index_range = pMaterial->index_range;
    the_material->index_shade = pMaterial->index_shade;
    the_material->index_blend = pMaterial->index_blend;
    the_material->colour_map = pMaterial->colour_map;
    memcpy(&the_material->map_transform, &pMaterial->map_transform, sizeof(the_material->map_transform));
    if (pSet_identifier) {
        version = gVersion_suffix++;
        sprintf(s, "%s(%d)", pMaterial->identifier, version);
        the_material->identifier = BrResAllocate(the_material, strlen(s) + 1, BR_MEMORY_STRING);
        strcpy(the_material->identifier, s);
    }
    BrMaterialAdd(the_material);
    return the_material;
}

// FUNCTION: CARMA2_HW 0x00515870
int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2) {
    int val;

    val = tolower(*p1);
    val -= tolower(*p2);
    while (val == 0) {
        if (*p1++ == '\0') {
            break;
        }
        if (*p2++ == '\0') {
            break;
        }
        val = tolower(*p1);
        val -= tolower(*p2);
    }
    return val;
}

// FUNCTION: CARMA2_HW 0x005158d0
void C2_HOOK_FASTCALL DRstrlwr(char* s) {
    int l = (int)strlen(s);
    int i;

    for (i = 0; i < l; i++) {
        s[i] = toupper(s[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00515950
int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {

    return PDCheckDriveExists2(pThe_path, NULL, 0);
}

br_actor* C2_HOOK_FASTCALL CloneActor(br_actor* pActor) {
    br_actor *clone;
    br_actor *child;

    clone = BrActorAllocate(pActor->type, pActor->type_data);
    clone->model = pActor->model;
    clone->material = pActor->material;
    if (pActor->identifier != NULL) {
        if (clone->identifier != NULL) {
            BrResFree(clone->identifier);
        }
        clone->identifier = BrResStrDup(clone, pActor->identifier);;
    }
    clone->t = pActor->t;
    for (child = pActor->children; child != NULL; child = child->next) {
        BrActorAdd(clone, CloneActor(child));
    }
    return clone;
}

// FUNCTION: CARMA2_HW 0x00515b80
void C2_HOOK_FASTCALL CalcActorGlobalPos(br_vector3* pResult, br_actor* pActor) {
    br_vector3 tv;

    BrVector3Set(pResult, 0.0f, 0.0f, 0.0f);
    for (;pActor != NULL && pActor != gNon_track_actor; pActor = pActor->parent) {
        if (pActor->t.t.mat.m[0][0] == 1.0f) {
            BrVector3Accumulate(pResult, &pActor->t.t.translate.t);
        } else {
            BrMatrix34ApplyP(&tv, pResult, &pActor->t.t.mat);
            BrVector3Accumulate(pResult, &tv);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00515c20
float C2_HOOK_STDCALL frac(float pN) {

    return pN - (float)(int)pN;
}

// FUNCTION: CARMA2_HW 0x00515d90
intptr_t C2_HOOK_CDECL FindMaterialCB(br_actor* pActor, void* data) {
    const char* name = data;
    br_model* model = pActor->model;
    br_face *face;
    int face_i;

    if (model != NULL) {

        face = model->faces;
        for (face_i = 0; face_i < model->nfaces; face_i++, face++) {

            if (face->material != NULL
                    && face->material->identifier != NULL
                    && strcmp(face->material->identifier, name) == 0) {
                return (intptr_t)face->material;
            }
        }
    }
    return (intptr_t)NULL;
}

// FUNCTION: CARMA2_HW 0x00515c40
br_material* C2_HOOK_FASTCALL FindMaterial(const char* pName, br_actor* pActor, int pRecursive) {

    if (pRecursive) {
        return (br_material*)DRActorEnumRecurse(pActor, FindMaterialCB, (void*)pName);
    } else {
        return (br_material*)FindMaterialCB(pActor, (void*)pName);
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialTablishly(br_material* pMaterial, int pPercent) {
    char* s;

    if (pPercent != 0 && pPercent != 100) {
        switch (pPercent) {
        case 25:
            s = "BLEND25.TAB";
            break;
        case 50:
            s = "BLEND50.TAB";
            break;
        case 75:
            s = "BLEND75.TAB";
            break;
        default:
            return;
        }
        pMaterial->index_blend = BrTableFind(s);
        if (pMaterial->index_blend == NULL) {
            pMaterial->index_blend = LoadSingleShadeTable(&gTrack_storage_space, s);
        }
    } else {
        pMaterial->index_blend = NULL;
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent) {

    // GLOBAL: CARMA2_HW 0x00661688
    static br_token_value alpha25[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x40) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616a0
    static br_token_value alpha50[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x80) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616b8
    static br_token_value alpha75[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0xc0) } },
        { 0 },
    };

    switch (pPercent) {
    case 25:
        pMaterial->extra_prim = alpha25;
        break;
    case 50:
        pMaterial->extra_prim = alpha50;
        break;
    case 75:
        pMaterial->extra_prim = alpha75;
        break;
    case 0:
    case 100:
        pMaterial->extra_prim = NULL;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00515e70
void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent) {
    br_uint_8 pixel_format = gScreen->type;

    if (pixel_format == BR_PMT_INDEX_8) {
        BlendifyMaterialTablishly(pMaterial, pPercent);
    } else {
        BlendifyMaterialPrimitively(pMaterial, pPercent);
    }
}

// FUNCTION: CARMA2_HW 0x00515fa0
void C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags) {
    int i;
    v11group* v11g;

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {

            v11g = &V11MODEL(pModel)->groups[i];
            *v11g->face_colours.materials = pModel->faces[*v11g->face_user].material;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00516010
int C2_HOOK_FASTCALL DRModelUpdateDeluxTurbo(br_actor* pActor, br_model* pModel, br_uint_16 pFlags) {

    if (pModel->nfaces == 0 || pModel->nvertices == 0) {

        pActor->type = BR_ACTOR_NONE;
        pActor->model = NULL;
        return 0;
    } else {

        DRModelUpdateAndKevificateMaterials(pModel,pFlags);
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00516090
br_scalar C2_HOOK_FASTCALL DistanceFromFace(const br_vector3* pP, const tFace_ref* pFace) {

    return BrVector3Dot(pP, &pFace->normal) - pFace->d;
}

// FUNCTION: CARMA2_HW 0x005160c0
void C2_HOOK_FASTCALL DRBoundsCopy(br_bounds3* pDest, const br_bounds3* pSrc) {

    pDest->min.v[0] = pSrc->min.v[0];
    pDest->min.v[1] = pSrc->min.v[1];
    pDest->min.v[2] = pSrc->min.v[2];
    pDest->max.v[0] = pSrc->max.v[0];
    pDest->max.v[1] = pSrc->max.v[1];
    pDest->max.v[2] = pSrc->max.v[2];
}

// FUNCTION: CARMA2_HW 0x005160f0
int C2_HOOK_FASTCALL TestForNan(const float* f) {
    tU32 bits;

    if (f == NULL) {
        return 0;
    }
    memcpy(&bits, f, sizeof(bits));
    /* This diagnostic reports infinity as well as NaN. */
    return (bits & 0x7f800000u) == 0x7f800000u;
}

// FUNCTION: CARMA2_HW 0x00516160
int C2_HOOK_FASTCALL DRVector3TestForNan(const br_vector3* pV) {

    if (pV != NULL) {
        return TestForNan(&pV->v[0]) || TestForNan(&pV->v[1]) || TestForNan(&pV->v[2]);
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00516240
void C2_HOOK_FASTCALL DRScaleModel(br_model* pModel, float pScale) {
    int i;
    br_vertex *vertex;

    if (pModel->nvertices != 0) {
        vertex = pModel->vertices;
        for (i = 0; i < pModel->nvertices; i++, vertex++) {

            BrVector3Scale(&vertex->p, &vertex->p, pScale);
        }
        BrModelUpdate(pModel, BR_MODU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x005162a0
double C2_HOOK_FASTCALL DistanceFromFaceND(const br_vector3* pP, const br_vector3* pN, br_scalar pF) {
#if defined(_MSC_VER) && defined(_M_IX86)
    double result;

    __asm {
        mov ecx, pP
        mov edx, pN
        fld dword ptr [edx + 4]
        fld dword ptr [edx + 8]
        fld dword ptr [ecx + 4]
        fld dword ptr [ecx + 8]
        fld dword ptr [edx]
        fxch st(4)
        fmulp st(2), st(0)
        fxch st(2)
        fmulp st(2), st(0)
        fxch st(2)
        fmul dword ptr [ecx]
        fxch st(1)
        faddp st(2), st(0)
        faddp st(1), st(0)
        fsub pF
        fstp result
    }
    return result;
#elif defined(__GNUC__) && defined(__i386__)
    double result;

    __asm__ volatile(
        "flds 4(%%edx); flds 8(%%edx); flds 4(%%ecx); flds 8(%%ecx); flds (%%edx); "
        "fxch %%st(4); fmulp %%st,%%st(2); fxch %%st(2); fmulp %%st,%%st(2); "
        "fxch %%st(2); fmuls (%%ecx); fxch %%st(1); faddp %%st,%%st(2); "
        "faddp %%st,%%st(1); fsubs %3; fstpl %0"
        : "=m" (result) : "c" (pP), "d" (pN), "m" (pF)
        : "st", "st(1)", "st(2)", "st(3)", "st(4)");
    return result;
#else
    volatile double yz = (double)pP->v[1] * pN->v[1] + (double)pP->v[2] * pN->v[2];

    return (yz + (double)pP->v[0] * pN->v[0]) - pF;
#endif
}

// FUNCTION: CARMA2_HW 0x00516350
int C2_HOOK_FASTCALL DRVector3NonZero(const br_vector3* pV) {
    if (pV->v[0] == 0.f && pV->v[1] == 0.f && pV->v[2] == 0.f) {
        return 0;
    } else {
        return 1;
    }
}

// FUNCTION: CARMA2_HW 0x00516390
void C2_HOOK_FASTCALL DRVector3Diminish(br_vector3* pV1, const br_vector3* pV2) {
    br_vector3 old = *pV1;

    pV1->v[0] -= pV2->v[0];
    pV1->v[1] -= pV2->v[1];
    pV1->v[2] -= pV2->v[2];

    if (!((double)old.v[0] * pV1->v[0] > 0.0)) {
        pV1->v[0] = 0.0f;
    }
    if (!((double)old.v[1] * pV1->v[1] > 0.0)) {
        pV1->v[1] = 0.0f;
    }
    if (!((double)old.v[2] * pV1->v[2] > 0.0)) {
        pV1->v[2] = 0.0f;
    }
}

// FUNCTION: CARMA2_HW 0x00516410
tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax) {

    if (!(pValue >= pMin)) {
        pValue = pMin;
    } else if (pValue > pMax) {
        pValue = pMax;
    }
    return (tU16)((pValue - pMin) * 65535.0f / (pMax - pMin) + 0.5f);
}

// FUNCTION: CARMA2_HW 0x00516460
br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax) {

    // FIXME: what call convention does this function use?
    //        __thiscall is possible, but out of place

    return (float)pValue * (pMax - pMin) / 65535.0f + pMin;
}

// FUNCTION: CARMA2_HW 0x00516490
void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax) {

    pDest->v[0] = DRScalarToU16(pSrc->v[0], pMin, pMax);
    pDest->v[1] = DRScalarToU16(pSrc->v[1], pMin, pMax);
    pDest->v[2] = DRScalarToU16(pSrc->v[2], pMin, pMax);
}

// FUNCTION: CARMA2_HW 0x00516570
void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax) {

    pDest->v[0] = DRU16ToScalar(pSrc->v[0], pMin, pMax);
    pDest->v[1] = DRU16ToScalar(pSrc->v[1], pMin, pMax);
    pDest->v[2] = DRU16ToScalar(pSrc->v[2], pMin, pMax);
}

// FUNCTION: CARMA2_HW 0x005165e0
void C2_HOOK_FASTCALL CompressMatrix34(tCompressed_matrix3* pCompressed_matrix3, int* pInactive, const br_matrix34* pMatrix) {
    br_vector3 pos;

    if (pMatrix->m[3][0] >= 500.0f) {
        *pInactive = 1;
        BrVector3Set(&pos,
            pMatrix->m[3][0] - 1000.0f,
            pMatrix->m[3][1] - 1000.0f,
            pMatrix->m[3][2] - 1000.0f);
    } else {
        *pInactive = 0;
        BrVector3Copy(&pos, (const br_vector3*)pMatrix->m[3]);
    }
    CompressVector3(&pCompressed_matrix3->m0, (const br_vector3*)pMatrix->m[0], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->m1, (const br_vector3*)pMatrix->m[1], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->p, &pos, -300.0f, 300.0f);
}

// FUNCTION: CARMA2_HW 0x00516910
void C2_HOOK_FASTCALL ExpandMatrix34(br_matrix34* pMatrix, const tCompressed_matrix3* pCompressed, int pInactive) {

    ExpandVector3((br_vector3*)pMatrix->m[0], &pCompressed->m0, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[0]);
    ExpandVector3((br_vector3*)pMatrix->m[1], &pCompressed->m1, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[1], (br_vector3*)pMatrix->m[1]);
    BrVector3Cross((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[1]);
    BrVector3Normalise((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[2]);
    /* Restore the position range used when compressing the matrix. */
    ExpandVector3((br_vector3*)pMatrix->m[3], &pCompressed->p, -300.0f, 300.0f);
    if (pInactive) {
        BrVector3Set((br_vector3*)pMatrix->m[3],
            pMatrix->m[3][0] + 1000.0f,
            pMatrix->m[3][1] + 1000.0f,
            pMatrix->m[3][2] + 1000.0f);
    }
}

// FUNCTION: CARMA2_HW 0x00516c10
void C2_HOOK_FASTCALL PossibleLock(int pValue) {

    if (gBack_screen->pixels == NULL) {
        BrPixelmapDirectLock(gBack_screen, 1);
    }
}

// FUNCTION: CARMA2_HW 0x00516c30
int C2_HOOK_FASTCALL PossibleUnlock(int pValue) {

    if (gBack_screen->pixels != NULL) {
        BrPixelmapDirectUnlock(gBack_screen);
        return 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00516fd0
tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry) {
    tU32* src_entry;
    int red;
    int green;
    int blue;

    src_entry = pPal->pixels;
    switch (gBack_screen->type) {
    default:
        BrFailure("Unsupported back buffer type.");
        return 0;
    case BR_PMT_RGB_565:
        red = (src_entry[pEntry] >> 8) & 0xf800;
        green = (src_entry[pEntry] >> 5) & 0x07e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    case BR_PMT_RGB_555:
        red = (src_entry[pEntry] >> 9) & 0x7c00;
        green = (src_entry[pEntry] >> 6) & 0x03e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    }
}

// FUNCTION: CARMA2_HW 0x00517050
tU16 C2_HOOK_FASTCALL Colour24BitTo16Bit(br_colour pColour) {
    tU32 colour;

    switch (gBack_screen->type) {
    case BR_PMT_RGB_565:
        colour = ((pColour >> 3) & 0x1f0000) | (pColour & 0xfc00);
        colour = (colour >> 2) | (pColour & 0xf8);
        return (tU16)(colour >> 3);
    case BR_PMT_RGB_555:
        colour = ((pColour >> 3) & 0x1f0000) | (pColour & 0xf800);
        colour = (colour >> 3) | (pColour & 0xf8);
        return (tU16)(colour >> 3);
    default:
        BrFailure("Unsupported back buffer type.");
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x005170c0
br_pixelmap* C2_HOOK_FASTCALL PaletteOf16Bits(br_pixelmap* pSrc) {
    tU16* dst_entry;
    int i;

    if (g16bit_palette == NULL) {
        g16bit_palette = BrPixelmapAllocate(BR_PMT_RGB_565, 1, 256, NULL, 0);
        if (g16bit_palette == NULL) {
            FatalError(kFatalError_OOM_S, "16-bit palette");
        }
    }
    if (!gPalette_changed || gPalette_source != pSrc) {
        dst_entry = g16bit_palette->pixels;
        for (i = 0; i < 256; i++) {
            *dst_entry++ = PaletteEntry16Bit(pSrc, i);
        }
        gPalette_changed = 1;
        gPalette_source = pSrc;
    }
    return g16bit_palette;
}

void C2_HOOK_FASTCALL Copy8BitTo16Bit(br_pixelmap* pDst, br_pixelmap* pSrc, br_pixelmap* pPalette) {
    int x;
    int y;
    tU8* src_start;
    tU16* dst_start;
    tU16* palette_entry;

    palette_entry = PaletteOf16Bits(pPalette)->pixels;
    for (y = 0; y < pDst->height; y++) {
        src_start = (tU8*)pSrc->pixels + pSrc->row_bytes * y;
        dst_start = (tU16*)((tU8*)pDst->pixels + pDst->row_bytes * y);
        for (x = 0; x < pDst->width; x++) {
            *dst_start = palette_entry[*src_start];
            src_start++;
            dst_start++;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00517d90
void C2_HOOK_FASTCALL DRPixelmapCopy(br_pixelmap* dst, br_pixelmap* src) {

    if (dst->type == src->type) {
        BrPixelmapCopy(dst, src);
    } else if (dst->type != BR_PMT_INDEX_8 && src->type == BR_PMT_INDEX_8) {
        Copy8BitTo16Bit(dst, src, gCurrent_palette);
    }
}

static tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList) {

    for (; pList != NULL; pList = pList->next) {
        if (DRStricmp(pIdentifier, pList->texture_name) == 0) {
            break;
        }
    }
    return pList;
}

// FUNCTION: CARMA2_HW 0x00517fa0
void C2_HOOK_FASTCALL NobbleNonzeroBlacks(br_pixelmap* pPalette) {
    int modified;
    int i;
    br_colour *pixels;
    br_colour c;
    int r;
    int g;
    int b;

    pixels= pPalette->pixels;
    modified = 0;
    if (*pixels != BR_COLOUR_RGBA(0, 0, 0, 0)) {
        *pixels = BR_COLOUR_RGBA(0, 0, 0, 0);
        modified = 1;
    }
    pixels++;
    for (i = 0; i < 255; i++ ) {
        c = *pixels;
        r = BR_COLOUR_RED(c);
        g = BR_COLOUR_GRN(c);
        b = BR_COLOUR_BLU(c);
        if (r == 0 && g == 0 && b == 0) {
            *pixels = BR_COLOUR_RGB(1, 1, 1);
            modified = 1;
        }
        pixels++;
    }
    if (modified) {
        BrMapUpdate(pPalette, BR_MAPU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x005182f0
void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading) {
    int i;
    tMaterial_exception *material_exception;

    for (i = 0; i < pCount; i++) {

        if (pMaterials[i]->colour_map != NULL) {
            material_exception = FindExceptionInList(pMaterials[i]->colour_map->identifier, gMaterial_exceptions);
            if (gEnable_texture_interpolation && (material_exception == NULL || !(material_exception->flags & 0x1))) {
                pMaterials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
            }
            if (gEnable_texture_interpolation && material_exception != NULL && material_exception->flags & 0x8) {
                pMaterials[i]->map_transform.m[2][0] = 0.02f;
                pMaterials[i]->map_transform.m[2][1] = 0.02f;
            }
            if (gEnable_texture_antialiasing) {
                pMaterials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
            }
            if (gEnable_perspective_maps) {
                pMaterials[i]->flags |= BR_MATF_PERSPECTIVE;
            }
        }
        switch (pShading) {
        case kRendererShadingType_AmbientOnly:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Diffuse1:
            pMaterials[i]->ka = 0.4f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Specular:
            pMaterials[i]->ka = 0.6f;
            pMaterials[i]->kd = 0.2f;
            pMaterials[i]->ks = 0.8f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Default:
            pMaterials[i]->ka = 0.2f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        default:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            break;
        }
    }
}

// FUNCTION: CARMA2_HW 0x005184c0
int C2_HOOK_FASTCALL FindBestColourMatch(br_colour pColour) {
    tRGB_colour rgb;

    rgb.red = (pColour >> 16) & 0xff;
    rgb.green = (pColour >> 8) & 0xff;
    rgb.blue = pColour & 0xff;
    return FindBestMatch(&rgb, gRender_palette);
}

// FUNCTION: CARMA2_HW 0x00518690
void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount) {
    int i;
    br_vertex* vertex;

    if (gScreen == NULL) {
        return;
    }
    if (gScreen->type == BR_PMT_INDEX_8) {
        return;
    }
    for (i = 0; i < pCount; i++) {
        int j;

        for (j = 0; j < pModels[i]->nvertices; j++) {
            vertex = &pModels[i]->vertices[j];

            vertex->red = 0xff;
            vertex->grn = 0xff;
            vertex->blu = 0xff;
        }
    }
}

#ifdef REC2_MATCHING
// FUNCTION: CARMA2_HW 0x00518700
void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type) {
    void* pixel;
    int row_bytes;
    int width;
    int height;

    if (pMap != NULL && pPixel_type == BR_PMT_RGB_555 && pMap->type == BR_PMT_RGB_565 && pMap->pixels != NULL) {
        pixel = pMap->pixels;
        row_bytes = pMap->row_bytes;
        width = pMap->width;
        height = pMap->height;

        __asm {
            mov        ecx, dword ptr [height]
            mov        esi, dword ptr [pixel]
            mov        edx, dword ptr [row_bytes]
            mov        edi, dword ptr [width]
        next_row:
            push       ecx
            mov        ecx, edi
            push       esi
        next_pixel:
            mov        eax, dword ptr [esi]
            mov        ebx, eax
            and        eax, 0xffc0
            and        ebx, 0x1f
            shr        eax, 0x1
            or         eax, ebx
            mov        word ptr [esi], ax
            add        esi, 0x2
            loop       next_pixel
            pop        esi
            add        esi, edx
            pop        ecx
            loop       next_row
        }
        pMap->type = BR_PMT_RGB_555;
    }
}
#else
void C2_HOOK_FASTCALL BRPM_convert(br_pixelmap* pMap, int pPixel_type) {
    br_uint_8* row_pointer;
    br_uint_16* pixel;
    br_uint_16 row_stride;
    br_uint_32 original_w;
    br_uint_32 w;
    br_uint_32 h;

    if (pMap != NULL && pPixel_type == BR_PMT_RGB_555 && pMap->type == BR_PMT_RGB_565 && pMap->pixels != NULL) {
        row_pointer = pMap->pixels;
        pixel = pMap->pixels;
        row_stride = pMap->row_bytes;
        original_w = pMap->width;
        h = pMap->height;
        w = pMap->width;

        while (h-- != 0) {
            w = original_w;
            pixel = (br_uint_16*)row_pointer;
            while (w-- != 0) {
                // RGB565 -> XRGB1555: remove least significant bit of green (6 bits -> 5 bits)
                *pixel = ((*pixel & 0xffc0)>>1) | (*pixel & 0x001f);
                pixel++;
            }
            row_pointer += row_stride;
        }
        pMap->type = BR_PMT_RGB_555;
    }
}
#endif

// FUNCTION: CARMA2_HW 0x00518780
void C2_HOOK_FASTCALL PrintScreen(void) {
    FILE* f;

    f = OpenUniqueFileB("DUMP", "BMP");
    if (f != NULL) {
        if (gBack_screen->type == BR_PMT_INDEX_8) {
            PrintScreenFile(f);
        } else {
            PrintScreenFile16(f);
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x00518b00
intptr_t C2_HOOK_CDECL DumpVisibleActorsCB(br_actor* pActor, void* pData) {

    if (pActor->render_style != BR_RSTYLE_NONE) {
        const char* str;

        switch (pActor->type) {
        case BR_ACTOR_NONE:
            str = "DUMMY ACTOR\n";
            break;
        case BR_ACTOR_MODEL:
            str = "MODEL ACTOR\n";
            break;
        case BR_ACTOR_LIGHT:
            str = "LIGHT ACTOR\n";
            break;
        case BR_ACTOR_CAMERA:
            str = "CAMERA ACTOR\n";
            break;
        case BR_ACTOR_BOUNDS:
            str = "BOUNDS ACTOR\n";
            break;
        case BR_ACTOR_BOUNDS_CORRECT:
            str = "BOUNDS-CORRECT ACTOR\n";
            break;
        case BR_ACTOR_CLIP_PLANE:
            str = "CLIP-PLANE ACTOR\n";
            break;
        default:
            str = "******** UNKNOWN ********\n";
            break;
        }
        printf(str);
        str = pActor->identifier;
        if (str == NULL) {
            str = "No Identifier";
        }
        printf("\tName:\t\t\"%s\"\n", str);
        printf("\tAddress:\t%08x\n", (unsigned int)(uintptr_t)pActor);
        printf("\tParent:\t\t%08x\n", (unsigned int)(uintptr_t)pActor->parent);
        printf("\tDepth:\t\t%i\n", pActor->depth);
        printf("\tModelPtr:\t0x%08x", (unsigned int)(uintptr_t)pActor->model);
        if (pActor->model == NULL) {
            printf("\n");
        } else {
            str = pActor->model->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            printf(" (\"%s\")\n", str);
        }
        printf("\tMaterialPtr:\t0x%08x", (unsigned int)(uintptr_t)pActor->material);
        if (pActor->material == NULL) {
            printf("\n");
        } else {
            str = pActor->material->identifier;
            if (str == NULL) {
                str = "No Identifier";
            }
            printf(" (\"%s\")\n", str);
        }
        switch (pActor->render_style) {
        case BR_RSTYLE_DEFAULT:
            str = "BR_RSTYLE_DEFAULT";
            break;
        case BR_RSTYLE_NONE:
            str = "BR_RSTYLE_NONE";
            break;
        case BR_RSTYLE_POINTS:
            str = "BR_RSTYLE_POINTS";
            break;
        case BR_RSTYLE_EDGES:
            str = "BR_RSTYLE_EDGES";
            break;
        case BR_RSTYLE_FACES:
            str = "BR_RSTYLE_FACES";
            break;
        case BR_RSTYLE_BOUNDING_POINTS:
            str = "BR_RSTYLE_BOUNDING_POINTS";
            break;
        case BR_RSTYLE_BOUNDING_EDGES:
            str = "BR_RSTYLE_BOUNDING_EDGES";
            break;
        case BR_RSTYLE_BOUNDING_FACES:
            str = "BR_RSTYLE_BOUNDING_FACES";
            break;
        default:
            str = "******** UNKNOWN ********";
            break;
        }
        printf("\tRenderStyle:\t%s\n", str);
        printf("\n");
    }
    return BrActorEnum(pActor, DumpVisibleActorsCB, pData);
}

// FUNCTION: CARMA2_HW 0x00518ac0
void C2_HOOK_FASTCALL DumpVisibleActors(br_actor* pActor, const char* pMsg) {

    printf("MSG:\"%s\"\n", pMsg);
    BrActorEnum(pActor, DumpVisibleActorsCB, NULL);
    printf("----------------------------------\n\n");
    fflush(stdout);
}

// FUNCTION: CARMA2_HW 0x00518d60
tU32 C2_HOOK_FASTCALL FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport(br_pixelmap* pm, tU32 red, tU32 grn, tU32 blu, tU32 alp) {
    tU32 colour;

    switch (pm->type) {
    case BR_PMT_RGB_555:
        colour = red & 0xf8;
        break;
    case BR_PMT_RGB_565:
        return ((((red & 0xf8) << 5) | (grn & 0xfc)) << 3) | ((blu >> 3) & 0x1f);
    case BR_PMT_RGBA_8888:
        return (((((alp << 8) | red) << 8) | grn) << 8) | blu;
    case BR_PMT_RGBA_4444:
        /* The original masks AL only, retaining the upper alpha bits. */
        return ((((alp & 0xfffffff0u) << 4) | (red & 0xfffffff0u)) << 4) | (blu >> 4) | (grn & 0xfffffff0u);
    case BR_PMT_ARGB_1555:
        colour = ((alp & 0xffffff80u) << 1) | (red & 0xf8);
        break;
    default:
        return (((red << 8) | grn) << 8) | blu;
    }
    return (((colour << 5) | (grn & 0xf8)) << 2) | ((blu >> 3) & 0x1f);
}

// GetBlenficatiousnessOfMaterialTablishly
int C2_HOOK_FASTCALL GetBlenficatiousnessOfMaterialTablishly(br_material* pMaterial) {
    br_pixelmap* table;

    table = BrTableFind("BLEND75.TAB");
    if (pMaterial->index_blend == table) {
        return 25;
    }
    table = BrTableFind("BLEND50.TAB");
    if (pMaterial->index_blend == table) {
        return 50;
    }
    table = BrTableFind("BLEND25.TAB");
    if (pMaterial->index_blend == table) {
        return 75;
    }
    return 100;
}

// GetBlenficatiousnessOfMaterialPrimitively
int C2_HOOK_FASTCALL GetBlenficatiousnessOfMaterialPrimitively(br_material* pMaterial) {
    br_token_value* prims;

    prims = pMaterial->extra_prim;
    if (prims != NULL && prims[0].t == BRT_BLEND_B && prims[0].v.b == 1 && prims[1].t == BRT_OPACITY_X) {
        /* Integer division preserves truncation toward zero for negatives. */
        return (100 * (prims[1].v.x >> 16)) / 256;
    }
    return 100;
}

// FUNCTION: CARMA2_HW 0x00518e70
int C2_HOOK_FASTCALL GetBlendificatiousnessOfMaterial(br_material* pMaterial) {
    if (gScreen->type == BR_PMT_INDEX_8) {
        return GetBlenficatiousnessOfMaterialTablishly(pMaterial);
    }
    return GetBlenficatiousnessOfMaterialPrimitively(pMaterial);
}

// FUNCTION: CARMA2_HW 0x00518f20
char* C2_HOOK_FASTCALL MungeCommas(int pValue) {
    // GLOBAL: CARMA2_HW 0x006b5f20
    static char result_buffer[32];
    char buffer[32];
    int len;
    int get_pos;
    int put_pos;
    int remaining;

    sprintf(buffer, "%i", pValue);
    len = (int)strlen(buffer);
    for (get_pos = 0, put_pos = 0, remaining = len; get_pos < len; get_pos++, put_pos++, remaining--) {

        if (remaining % 3 == 0 && get_pos != 0) {
            result_buffer[put_pos] = gMisc_strings[294][0];
            put_pos++;
        }
        result_buffer[put_pos] = buffer[get_pos];
    }
    result_buffer[put_pos] = '\0';
    return result_buffer;
}

// FUNCTION: CARMA2_HW 0x00518fa0
void C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pMeta, const char* pRepl) {
    int len_text;
    int len_repl;
    int i;

    len_text = strlen(pText);
    len_repl = strlen(pRepl);

    for (i = 0; i < len_text; i++) {
        if (pText[i] == '@' && pText[i + 1] == pMeta) {
            memmove(&pText[i + len_repl], &pText[i + 2], len_text - i - 1);
            memcpy(&pText[i], pRepl, len_repl);
            i += len_repl;
            len_text += len_repl - 2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00519040
void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar) {
    char repl[2];

    repl[1] = '\0';
    repl[0] = pChar;
    MungeMetaCharacters(pText, pMeta, repl);
}

// FUNCTION: CARMA2_HW 0x005190f0
void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum) {
    char text[16];

    sprintf(text, "%d", pNum);
    MungeMetaCharacters(pText, pMeta, text);
}

// FUNCTION: CARMA2_HW 0x005191b0
void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_16 dx, br_int_16 dy, br_pixelmap* src, br_int_16 sx, br_int_16 sy, br_int_16 w, br_int_16 h) {

    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
}

// FUNCTION: CARMA2_HW 0x005191f0
void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap) {

#if 0 // FIXME: introduce endian.h-like rec2_endian.h header
    br_uint_16 y;
    br_uint_8* row_ptr;
    br_colour* ptr;
    int w;

    row_ptr = pMap->pixels;
    for (y = 0; y < pMap->height; y++) {
        w = (pMap->width * 2) / sizeof(br_colour);
        ptr = (br_colour*) row_ptr;
        while (w) {
            *ptr = (*ptr >> 0x18) + (*ptr << 0x18) + ((*ptr >> 0x8) & 0xff00) + ((*ptr & 0xff00) <<0x8);
            ptr++;
        }
        row_ptr += pMap->row_bytes;
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005193f0
void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2) {

}

