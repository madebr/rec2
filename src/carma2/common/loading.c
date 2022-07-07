#include "loading.h"

#include <brender/brender.h>

#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tTwatVfsMountPoint, gTwatVfsMountPoints, 5, 0x00691b40);

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

DRFILE* (C2_HOOK_FASTCALL * DRfopen_original)(const char* pFilename, const char* pMode);
DRFILE* C2_HOOK_FASTCALL DRfopen(const char* pFilename, const char* pMode) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_STARTF("(%s %s)", pFilename, pMode);
    DRFILE* res = DRfopen_original(pFilename, pMode);
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004b4780, DRfopen, DRfopen_original)

char* (C2_HOOK_FASTCALL * DRreadline_original)(DRFILE* pFile, char* pBuffer);
char* C2_HOOK_FASTCALL DRreadline(DRFILE* pFile, char* pBuffer) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    char* res = DRreadline_original(pFile, pBuffer);
    C2_HOOK_FINISH();
    return res;
#else
#error "not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00490f30, DRreadline, DRreadline_original)

void (C2_HOOK_FASTCALL * DRfclose_original)(DRFILE* pFile);
void C2_HOOK_FASTCALL DRfclose(DRFILE* pFile) {
#if defined(C2_HOOKS_ENABLED)
    C2_HOOK_START();
    DRfclose_original(pFile);
    C2_HOOK_FINISH();
#else
#error "not implemented"
#endif
}

tU32 (C2_HOOK_FASTCALL * TWT_ReadBinaryU32_original)(FILE* file);
tU32 C2_HOOK_FASTCALL TWT_ReadBinaryU32(FILE* file) {
#if defined(C2_HOOKS_ENABLED)
    return TWT_ReadBinaryU32_original(file);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0048f830, TWT_ReadBinaryU32, TWT_ReadBinaryU32_original)

tTWT C2_HOOK_FASTCALL TWT_Mount(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWT twt;
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
