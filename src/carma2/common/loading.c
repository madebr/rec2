#include "loading.h"

#include "utility.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5, 0x00691b40);

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsFile, gTwatVfsFiles, 50, 0x00692070);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversionStackSize, 0x006923a0);

C2_HOOK_VARIABLE_IMPLEMENT(int, gDisableTiffConversion, 0x0068c724);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(int, gDisableTiffConversionStack, 2, 0x00692068);

C2_HOOK_VARIABLE_IMPLEMENT(int, gRusselsFannies, 0x006aa5c4);

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

int C2_HOOK_FASTCALL GetALineAndInterpretCommand(tTWTFILE* pF, const char** pString_list, int pCount) {
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

int (C2_HOOK_FASTCALL * GetAnInt_original)(tTWTFILE* pF);
int C2_HOOK_FASTCALL GetAnInt(tTWTFILE* pF) {
#if defined(C2_HOOKS_ENABLED)
    return GetAnInt_original(pF);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048fb00, GetAnInt, GetAnInt_original)

void C2_HOOK_FASTCALL GetThreeInts(tTWTFILE* pF, int* pF1, int* pF2, int* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = c2_strtok(s, "\t ,/");
    sscanf(str, "%d", pF1);
    str = c2_strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF2);
    str = c2_strtok(NULL, "\t ,/");
    sscanf(str, "%d", pF3);
}
C2_HOOK_FUNCTION(0x0048fe30, GetThreeInts)

float (C2_HOOK_FASTCALL * GetAFloat_original)(tTWTFILE* pF);
float C2_HOOK_FASTCALL GetAFloat(tTWTFILE* pF) {
#if defined(C2_HOOKS_ENABLED)
    return GetAFloat_original(pF);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004900e0, GetAFloat, GetAFloat_original)

float C2_HOOK_FASTCALL GetAScalar(tTWTFILE* pF) {
    return GetAFloat(pF);
}
C2_HOOK_FUNCTION(0x0048fb80, GetAScalar)

void C2_HOOK_FASTCALL GetThreeFloats(tTWTFILE * pF, float* pF1, float* pF2, float* pF3) {
    char s[256];
    char* str;

    GetALineAndDontArgue(pF, s);
    str = strtok(s, "\t ,/");
    sscanf(str, "%f", pF1);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF2);
    str = strtok(NULL, "\t ,/");
    sscanf(str, "%f", pF3);
}
C2_HOOK_FUNCTION(0x0048fc90, GetThreeFloats)

void (C2_HOOK_FASTCALL * GetAString_original)(tTWTFILE* pF, char* pString);
void C2_HOOK_FASTCALL GetAString(tTWTFILE* pF, char* pString) {
#if defined(C2_HOOKS_ENABLED)
    GetAString_original(pF, pString);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490630, GetAString, GetAString_original)

tTWTFILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
tTWTFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_STARTF("(%s %s)", pFilename, pMode);
    tTWTFILE* res = DRfopen_original(pFilename, pMode);
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4780, DRfopen, DRfopen_original)

void (C2_HOOK_FASTCALL * DRfclose_original)(tTWTFILE* pFile);
void C2_HOOK_FASTCALL DRfclose(tTWTFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    DRfclose_original(pFile);
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4760, DRfclose, DRfclose_original)

br_size_t (C2_HOOK_FASTCALL * DRfread_original)(void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfread(void* buf, br_size_t size, unsigned int n, void* f) {
#if defined(C2_HOOKS_ENABLED)
    return DRfread_original(buf, size, n, f);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b49f0, DRfread, DRfread_original)

br_size_t (C2_HOOK_FASTCALL * DRfwrite_original)(const void* buf, br_size_t size, unsigned int n, void* f);
br_size_t C2_HOOK_FASTCALL DRfwrite(const void* buf, br_size_t size, unsigned int n, void* f) {
#if defined(C2_HOOKS_ENABLED)
    return DRfwrite_original(buf, size, n, f);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4a80, DRfwrite, DRfwrite_original)

int (C2_HOOK_FASTCALL * DRfeof_original)(tTWTFILE* pFile);
int C2_HOOK_FASTCALL DRfeof(tTWTFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    return DRfeof_original(pFile);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4c10, DRfeof, DRfeof_original)

int (C2_HOOK_FASTCALL * DRfgetc_original)(tTWTFILE* pFile);
int C2_HOOK_FASTCALL DRfgetc(tTWTFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    return DRfgetc_original(pFile);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4880, DRfgetc, DRfgetc_original)

char* (C2_HOOK_FASTCALL * DRfgets_original)(char* buffer, br_size_t size, tTWTFILE* pFile);
char* C2_HOOK_FASTCALL DRfgets(char* buffer, br_size_t size, tTWTFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    return DRfgets_original(buffer, size, pFile);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4900, DRfgets, DRfgets_original)

tU32 (C2_HOOK_FASTCALL * TWT_ReadBinaryU32_original)(FILE* file);
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file) {
#if defined(C2_HOOKS_ENABLED)
    return TWT_ReadBinaryU32_original(file);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f830, TWT_ReadBinaryU32, TWT_ReadBinaryU32_original)

void C2_HOOK_FASTCALL TWT_Init() {
    for(int i = 1; i < 50; i++) {  // FIXME: use array sizeof
        C2V(gTwatVfsFiles)[i].used = 0;
    }
    for(int i = 0; i < 5; i++) {  // FIXME: use array sizeof
        C2V(gTwatVfsMountPoints)[i].header = NULL;
    }

    C2V(gDisableTiffConversionStackSize) = 0;
}
C2_HOOK_FUNCTION(0x004b4570, TWT_Init)

tTWTVFS C2_HOOK_FASTCALL TWT_Mount(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    int fileSize;

    // file header must be 56 bytes for compatibility with .TWT files
    REC2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    strcpy(twatFilePath, path);
    strcat(twatFilePath, ".TWT");

    f = c2_fopen(twatFilePath, "rb");
    if (f == NULL) {
        return -1;
    }
    for (twt = 0; ; twt++) {
        if (twt >= 5) {  // FIXME: use array_sizeof(gTwatVfsMountPoints) or something similar
            c2_fclose(f);
            return -1;
        }
        if (C2V(gTwatVfsMountPoints)[twt].header == NULL) {
            break;
        }
    }
    strcpy(C2V(gTwatVfsMountPoints)[twt].path, path);
    fileSize = TWT_ReadBinaryU32(f);
    c2_rewind(f);

    C2V(gTwatVfsMountPoints)[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
    C2V(gTwatVfsMountPoints)[twt].data = (tU8*)C2V(gTwatVfsMountPoints)[twt].header;
    c2_fread(C2V(gTwatVfsMountPoints)[twt].header, fileSize, 1, f);
    // TWT_verify_header(C2V(gTwatVfsMountPoints)[i].header, size); // not implemented
    c2_fclose(f);

    tU8* fileData = (tU8*)&C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[C2V(gTwatVfsMountPoints)[twt].header->nbFiles];
    for (tU32 i = 0; i < C2V(gTwatVfsMountPoints)[twt].header->nbFiles; i++) {
        C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].data = fileData;
        fileData += (C2V(gTwatVfsMountPoints)[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
    }
    return twt;
}
C2_HOOK_FUNCTION(0x004b45b0, TWT_Mount)

tTWTVFS C2_HOOK_FASTCALL TWT_MountEx(const char* path) {
    tTWTVFS res;

    res = TWT_Mount(path);
    if (TWT_MOUNT_SUCCEEDED(res)) {
        C2V(gDisableTiffConversionStack)[C2V(gDisableTiffConversionStackSize)] = C2V(gDisableTiffConversion);
        C2V(gDisableTiffConversionStackSize)++;
        C2V(gDisableTiffConversion) = 1;
    }
    return res;
}
C2_HOOK_FUNCTION(0x004b4df0, TWT_MountEx)

br_pixelmap* (C2_HOOK_FASTCALL * DRLoadPixelmap_original)(const char* pPath_name);
br_pixelmap* C2_HOOK_FASTCALL DRLoadPixelmap(const char* pPath_name) {
#if defined(C2_HOOKS_ENABLED)
    return DRLoadPixelmap_original(pPath_name);
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048ec00, DRLoadPixelmap, DRLoadPixelmap_original)
