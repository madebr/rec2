#include "70-packfile.h"

#include "08-loading1.h"
#include "69-sound.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x00691b40
tTwatVfsMountPoint gTwatVfsMountPoints[5];

// GLOBAL: CARMA2_HW 0x00692070
tTwatVfsFile gTwatVfsFiles[50];

// GLOBAL: CARMA2_HW 0x006923a0
int gDisableTiffConversionStackPos;

// GLOBAL: CARMA2_HW 0x00692068
int gDisableTiffConversionStack[2];

// GLOBAL: CARMA2_HW 0x0068c724
int gDisableTiffConversion;

// FUNCTION: CARMA2_HW 0x004b4570
void C2_HOOK_FASTCALL InitPackFiles(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tTwatVfsMountPoint) != 264);

    for (i = 1; i < (int)REC2_ASIZE(gTwatVfsFiles); i++) {
        gTwatVfsFiles[i].start = NULL;
    }
    for (i = 0; i < (int)REC2_ASIZE(gTwatVfsMountPoints); i++) {
        gTwatVfsMountPoints[i].header = NULL;
    }

    gDisableTiffConversionStackPos = 0;
}

// FUNCTION: CARMA2_HW 0x004b45b0
tTWTVFS C2_HOOK_FASTCALL OpenPackFile(const char* path) {
    tPath_name twatFilePath;
    FILE* f;
    tTWTVFS twt;
    tU32 fileSize;
    tU8* fileData;
    tU32 i;

    // file header must be 56 bytes for compatibility with .TWT files
    REC2_BUG_ON(sizeof(tTwatFileHeader) != 56);

    strcpy(twatFilePath, path);
    strcat(twatFilePath, ".TWT");

    f = fopen(twatFilePath, "rb");
    if (f != NULL) {
        for (twt = 0; ; twt++) {
            if (twt >= (int)REC2_ASIZE(gTwatVfsMountPoints)) {
                fclose(f);
                return -1;
            }
            if (gTwatVfsMountPoints[twt].header == NULL) {
                break;
            }
        }
        strcpy(gTwatVfsMountPoints[twt].path, path);
        fileSize = ReadU32(f);
        rewind(f);

        gTwatVfsMountPoints[twt].header = BrMemAllocate(fileSize, kMem_packed_file);
        fread(gTwatVfsMountPoints[twt].header, fileSize, 1, f);
        SplungeSomeData(gTwatVfsMountPoints[twt].header, fileSize);
        gTwatVfsMountPoints[twt].data = (tU8*)gTwatVfsMountPoints[twt].header;
        fclose(f);

        fileData = (tU8*)&gTwatVfsMountPoints[twt].header->fileHeaders[gTwatVfsMountPoints[twt].header->nbFiles];
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            gTwatVfsMountPoints[twt].header->fileHeaders[i].data = fileData;
            fileData += (gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize + 3u) & ~3u;
        }
        return twt;
    }
    return -1;
}

// ClosePackFile

// STUB: CARMA2_HW 0x004b4760
void C2_HOOK_FASTCALL PFfclose(FILE* pFile) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b4780
FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b4880
int C2_HOOK_FASTCALL PFfgetc(FILE* pFile) {
    NOT_IMPLEMENTED();
}

// PFgetc

// STUB: CARMA2_HW 0x004b4900
char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b49a0
int C2_HOOK_FASTCALL PFungetc(int ch, FILE* file) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004b49f0
br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f) {
    int read_size;
    tU8 *pos;
    tU8 *end;

    if ((uintptr_t)f >= REC2_ASIZE(gTwatVfsFiles) + 1) {
        return fread(buf, size, n, f);
    }
    read_size = size * n;
    end = gTwatVfsFiles[(uintptr_t)f].end;
    pos = gTwatVfsFiles[(uintptr_t)f].pos;
    if (end - pos >= read_size) {
        end = pos + read_size;
    } else {
        n = ((end - pos) / size) * size;
        read_size = size * n;//
    }
    memcpy(buf, gTwatVfsFiles[(uintptr_t)f].pos, read_size);
    gTwatVfsFiles[(uintptr_t)f].pos = end;
    gTwatVfsFiles[(uintptr_t)f].error = 0;
    return n;
}

// STUB: CARMA2_HW 0x004b4a80
br_size_t C2_HOOK_FASTCALL PFfwrite(const void* buf, br_size_t size, unsigned int n, void* f) {
    NOT_IMPLEMENTED();
}

// PFftell

// STUB: CARMA2_HW 0x004b4b70
int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {
    NOT_IMPLEMENTED();
}

// PFrewind

// STUB: CARMA2_HW 0x004b4c10
int C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
    NOT_IMPLEMENTED();
}

// PFForEveryFile

// PFForEveryFile2

// FUNCTION: CARMA2_HW 0x004b4df0
tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path) {
    tTWTVFS res;

    res = OpenPackFile(path);
    if (res >= 0) {
        gDisableTiffConversionStack[gDisableTiffConversionStackPos] = gDisableTiffConversion;
        gDisableTiffConversionStackPos++;
        gDisableTiffConversion = 1;
    }
    return res;
}

// STUB: CARMA2_HW 0x004b4e20
void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt) {
    NOT_IMPLEMENTED();
}

// PackFileRevertTiffLoading

// PackFileRerevertTiffLoading

