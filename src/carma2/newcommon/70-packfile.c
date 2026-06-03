#include "70-packfile.h"

// STUB: CARMA2_HW 0x004b4570
void C2_HOOK_FASTCALL InitPackFiles(void) {
    NOT_IMPLEMENTED();
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

