#include "drfile.h"

#include "loading.h"

#include "rec2_types.h"

#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x005933a8
br_filesystem gFile_system = {
    "Carmageddon",
    NULL,
    &DRStdioOpenRead,
    &DRStdioOpenWrite,
    &DRStdioClose,
    &DRStdioEOF,
    &DRStdioGetChr,
    NULL,
    &DRStdioRead,
    &DRStdioWrite,
    &DRStdioGetLine,
    NULL,
    DRStdioAdvance,
};

// GLOBAL: CARMA2_HW 0x006815c4
br_filesystem* gOld_file_system;

// FUNCTION: CARMA2_HW 0x0044c700
void* C2_HOOK_CDECL DRStdioOpenRead(const char* name, br_size_t n_magics, br_mode_test_cbfn* identify, int* mode_result) {
    if (mode_result != NULL) {
        *mode_result = 0;
    }
    return DRfopen(name, "rb");
}

// FUNCTION: CARMA2_HW 0x0044c720
void* C2_HOOK_CDECL DRStdioOpenWrite(const char* name, int mode) {
    return gOld_file_system->open_write(name, mode);
}

// FUNCTION: CARMA2_HW 0x0044c740
void C2_HOOK_CDECL DRStdioClose(void* f) {
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x0044c6a0
int C2_HOOK_CDECL DRStdioEOF(void* f) {
    return PFfeof(f);
}

// FUNCTION: CARMA2_HW 0x0044c6b0
int C2_HOOK_CDECL DRStdioGetChr(void* f) {
    return PFfgetc(f);
}

// FUNCTION: CARMA2_HW 0x0044c750
br_size_t C2_HOOK_CDECL DRStdioRead(void* buf, br_size_t size, unsigned int n, void* f) {
    return PFfread(buf, size, n, f);
}

// FUNCTION: CARMA2_HW 0x0044c770
br_size_t C2_HOOK_CDECL DRStdioWrite(const void* buf, br_size_t size, unsigned int n, void* f) {
    return PFfwrite(buf, size, n, f);
}

// FUNCTION: CARMA2_HW 0x0044c6c0
br_size_t C2_HOOK_CDECL DRStdioGetLine(char* buf, br_size_t buf_len, void* f) {
    PFfgets(buf, buf_len, f);
    return strlen(buf);
}

// FUNCTION: CARMA2_HW 0x0044c6f0
void C2_HOOK_CDECL DRStdioAdvance(br_size_t count, void *f) {
    PFfseek((FILE*)f, count, SEEK_CUR);
}

// FUNCTION: CARMA2_HW 0x0044c790
void C2_HOOK_FASTCALL InstallDRFileCalls(void) {
    br_filesystem* temp_system;

    temp_system = BrMemAllocate(sizeof(br_filesystem), kMem_misc);
    gOld_file_system = BrFilesystemSet(temp_system);
    gFile_system.attributes = gOld_file_system->attributes;
    gFile_system.putchr = gOld_file_system->putchr;
    gFile_system.putline = gOld_file_system->putline;
    BrFilesystemSet(&gFile_system);
    BrMemFree(temp_system);
}
