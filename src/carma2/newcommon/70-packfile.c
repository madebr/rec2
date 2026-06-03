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

// PFfgetc

// PFgetc

// PFfgets

// PFungetc

// PFfread

// PFfwrite

// PFftell

// PFfseek

// PFrewind

// PFfeof

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

