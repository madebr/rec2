#include "70-packfile.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00691b40
tTwatVfsMountPoint gTwatVfsMountPoints[5];

// GLOBAL: CARMA2_HW 0x00692070
tTwatVfsFile gTwatVfsFiles[50];

// GLOBAL: CARMA2_HW 0x006923a0
int gDisableTiffConversionStackPos;

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

// OpenPackFile

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

// PFungetc

// STUB: CARMA2_HW 0x004b49f0
br_size_t C2_HOOK_FASTCALL PFfread(void* buf, br_size_t size, unsigned int n, void* f) {
    NOT_IMPLEMENTED();
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

// STUB: CARMA2_HW 0x004b4df0
tTWTVFS C2_HOOK_FASTCALL OpenPackFileAndSetTiffLoading(const char* path) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004b4e20
void C2_HOOK_FASTCALL ClosePackFileAndSetTiffLoading(tTWTVFS twt) {
    NOT_IMPLEMENTED();
}

// PackFileRevertTiffLoading

// PackFileRerevertTiffLoading

