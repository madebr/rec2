#include "70-packfile.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "69-sound.h"
#include "platform.h"
#include "rec2_macros.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x00691b40
tTwatVfsMountPoint gTwatVfsMountPoints[5];

// GLOBAL: CARMA2_HW 0x00692070
tTwatVfsFile gTwatVfsFiles[51];

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
    int i;

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

// FUNCTION: CARMA2_HW 0x004b4760
void C2_HOOK_FASTCALL PFfclose(FILE* pFile) {

    if ((uintptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        fclose(pFile);
    } else {
        gTwatVfsFiles[(uintptr_t)pFile].start = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x004b4780
FILE* C2_HOOK_FASTCALL PFfopen(const char* pPath, const char* mode) {
    int twt;
    int i;
    int file_index;
    size_t twt_path_len;

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && strstr(pPath, gTwatVfsMountPoints[twt].path) == pPath) {
            twt_path_len = strlen(gTwatVfsMountPoints[twt].path);
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                if (DRStricmp(gTwatVfsMountPoints[twt].header->fileHeaders[i].filename, &pPath[twt_path_len + 1]) == 0) {
                    for (file_index = 0; file_index < (int)REC2_ASIZE(gTwatVfsFiles) - 1; file_index++) {
                        if (gTwatVfsFiles[file_index].start != NULL) {
                            continue;
                        }
                        gTwatVfsFiles[file_index + 1].start = gTwatVfsMountPoints[twt].header->fileHeaders[i].data;
                        gTwatVfsFiles[file_index + 1].pos = gTwatVfsMountPoints[twt].header->fileHeaders[i].data;
                        gTwatVfsFiles[file_index + 1].end = gTwatVfsMountPoints[twt].header->fileHeaders[i].data + gTwatVfsMountPoints[twt].header->fileHeaders[i].fileSize;
                        gTwatVfsFiles[file_index + 1].error = 0;
                        return (FILE*)(file_index + 1);
                    }
                    return NULL;
                }
            }
        }
    }
    return fopen(pPath, mode);
}

// FUNCTION: CARMA2_HW 0x004b4880
int C2_HOOK_FASTCALL PFfgetc(FILE* pFile) {
    tTwatVfsFile* twtFile;
    int result;

    if ((intptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return fgetc(pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    if (twtFile->pos >= twtFile->end) {
        twtFile->error = -1;
        return EOF;
    }
    twtFile->error = 0;
    result = *twtFile->pos;
    twtFile->pos++;
    return result;
}

// PFgetc

// FUNCTION: CARMA2_HW 0x004b4900
char* C2_HOOK_FASTCALL PFfgets(char* buffer, br_size_t size, FILE* pFile) {
    tTwatVfsFile* twtFile;
    int c;
    char* writePtr;
    int i;

    if ((intptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return fgets(buffer, size, pFile);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pFile];
    writePtr = buffer;
    for (i = 0; i < (int)size - 1; i++) {
        c = (int)*twtFile->pos++;
        if (c == -1) {
            *writePtr = '\0';
            twtFile->error = -1;
            return NULL;
        }
        *writePtr++ = c;
        if (c == '\n') {
            break;
        }
        if (twtFile->pos >= twtFile->end) {
            break;
        }
    }
    *writePtr = '\0';
    twtFile->error = 0;
    return buffer;
}

// FUNCTION: CARMA2_HW 0x004b49a0
int C2_HOOK_FASTCALL PFungetc(int ch, FILE* pFile) {
    tTwatVfsFile* twtFile;

    if ((uintptr_t)pFile >= REC2_ASIZE(gTwatVfsFiles)) {
        return ungetc(ch, pFile);
    }
    twtFile = &gTwatVfsFiles[(intptr_t)pFile];
    if (twtFile->pos > twtFile->start && ch != -1) {
        twtFile->pos--;
        *twtFile->pos = ch;
        twtFile->error = 0;
        return ch;
    } else {
        twtFile->error = -1;
        return -1;
    }
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

// FUNCTION: CARMA2_HW 0x004b4b00
int C2_HOOK_FASTCALL PFftell(FILE* pF) {
    int pos;

    if ((uintptr_t)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        return ftell(pF);
    }
    pos = gTwatVfsFiles[(uintptr_t)pF].pos - gTwatVfsFiles[(uintptr_t)pF].start;
    gTwatVfsFiles[(uintptr_t)pF].error = 0;
    return pos;
}

// FUNCTION: CARMA2_HW 0x004b4b70
int C2_HOOK_FASTCALL PFfseek(FILE* pF, int offset, int whence) {
    tTwatVfsFile* twtFile;
    tU8 *newpos;

    if ((intptr_t)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        return fseek(pF, offset, whence);
    }
    twtFile = &gTwatVfsFiles[(uintptr_t)pF];
    switch (whence) {
    case SEEK_SET:
        newpos = twtFile->start + offset;
        break;
    case SEEK_END:
        newpos = twtFile->end + offset;
        break;
    case SEEK_CUR:
        newpos = twtFile->pos + offset;
        break;
    }
    if (newpos >= twtFile->start && newpos <= twtFile->end) {
        twtFile->pos = newpos;
        twtFile->error = 0;
        return 0;
    } else {
        twtFile->error = -1;
        return -1;
    }
}

// FUNCTION: CARMA2_HW 0x004b4be0
void C2_HOOK_FASTCALL PFrewind(FILE* pF) {
    tTwatVfsFile* twtFile;

    if ((int)pF >= REC2_ASIZE(gTwatVfsFiles)) {
        rewind(pF);
    } else {
        twtFile = &gTwatVfsFiles[(uintptr_t)pF];
        twtFile->pos = twtFile->start;
        twtFile->error = 0;
    }
}
// STUB: CARMA2_HW 0x004b4c10
int C2_HOOK_FASTCALL PFfeof(FILE* pFile) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004b4c80
void C2_HOOK_FASTCALL PFForEveryFile(const char* pThe_path, tPDForEveryFileRecurse_cbfn pAction_routine) {
    int twt;
    int i;
    char buffer[256];

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header == NULL) {
            continue;
        }
        if (DRStricmp(pThe_path, gTwatVfsMountPoints[twt].path) != 0) {
            continue;
        }
        for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
            PathCat(buffer, pThe_path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
            pAction_routine(buffer);
        }
        return;
    }
    PDForEveryFile(pThe_path, pAction_routine);
}

// FUNCTION: CARMA2_HW 0x004b4d30
void C2_HOOK_FASTCALL PFForEveryFile2(const char* path, tEnumPathCallback pCallback, void* data) {
    int twt;
    int i;
    tPath_name twt_filePath;

    for (twt = 0; twt < (int)REC2_ASIZE(gTwatVfsMountPoints); twt++) {
        if (gTwatVfsMountPoints[twt].header != NULL && DRStricmp(gTwatVfsMountPoints[twt].path, path) == 0) {
            for (i = 0; i < gTwatVfsMountPoints[twt].header->nbFiles; i++) {
                PathCat(twt_filePath, path, gTwatVfsMountPoints[twt].header->fileHeaders[i].filename);
                pCallback(twt_filePath, data);
            }
            return;
        }
    }
    PDEnumPath(path, pCallback, data);
}

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

